#include "AdventApp.h"

Advent::AdventApp* Advent::AdventApp::s_instance = nullptr;

Advent::AdventApp::~AdventApp()
{
    if (m_server)
    {
        delete m_server;
    }
}

void Advent::AdventApp::SetupArguments(argparse::ArgumentParser& args)
{
    // Path to configuration
    args.add_argument("-c", "--conf")
        .help("Path to the configuration file")
        .default_value<std::string>("advent_conf.json")
        .required()
        ;

    // Day override (different day in december)
    args.add_argument("-f", "--force")
        .scan<'i', int>()
        .default_value<int>(-1)
        .help("Lets the server assume that this is the day of December")
        ;

    // Extended logs
    args.add_argument("-v", "--verbose")
        .help("Verbose logging output")
        .default_value(false)
        .implicit_value(true)
        ;
}

int Advent::AdventApp::ApplicationMain(const argparse::ArgumentParser& args)
{
    // Apply logging
    if (args["-v"] == true)
        spdlog::default_logger()->set_level(ADVENT_VERBOSE_LEVEL);

    // Register system signals
    spdlog::info("Registering system signals");
    s_instance = this;
    signal(SIGINT, &SignalHandler);
#ifdef SIGBREAK // Win32
    signal(SIGBREAK, &SignalHandler);
#endif
    signal(SIGABRT, &SignalHandler);
    signal(SIGTERM, &SignalHandler);

    // Load config
    std::filesystem::path configFile = args.get<std::string>("-c");
    std::ifstream configFileStream(configFile);
    ADVENT_ASSERT_FMT(configFileStream.is_open(), "Failed to open file {}", configFile.generic_string());
    nlohmann::json config = nlohmann::json::parse(configFileStream, nullptr, true, true);
    configFileStream.close();
    spdlog::debug("Loaded json config \"{}\"", nlohmann::to_string(config));

    // Application dir
    const std::filesystem::path appDir = config["App"]["Directory"];
    m_serverDir = appDir;
    spdlog::debug("Application dir {}", appDir.generic_string());

    // Override day
    m_overrideDay = args.get<int>("-f");

    // Load plugins
    spdlog::info("Loading plugins");
    m_days.Init(config["App"]["PluginDir"]);

    // Load access tokens
    for (auto& token : config["App"]["AccessTokens"])
    {
        m_accessTokens.push_back(token);
    }
    spdlog::info("Loaded {} access tokens from config.", m_accessTokens.size());

    // Configure web server
    m_server = new httplib::SSLServer(
        config["Server"]["SSLCert"].get<std::string>().c_str(),
        config["Server"]["SSLKey"].get<std::string>().c_str()
    );
    ADVENT_ASSERT(m_server, "Failed to create webserver");

    // Exceptions
    m_server->set_exception_handler(
        [&](const httplib::Request&, httplib::Response&, std::exception_ptr exp)
        {
            try
            {
                std::rethrow_exception(exp);
            }
            catch (const std::exception& ex)
            {
                spdlog::error("Exception occured during web request: {}", ex.what());
            }
            catch (...)
            {
                spdlog::error("Unknown exception occurred during web request!");
            }
        }
    );

    // Mount www-data
    m_server->set_mount_point("/res", (appDir / "www-data").generic_string());

    // Advent routes
    m_server->Get("/",
        std::bind(&AdventApp::RequestIndex, this, std::placeholders::_1, std::placeholders::_2)
    );
    m_server->Get("/auth",
        std::bind(&AdventApp::RequestAuthentication, this, std::placeholders::_1, std::placeholders::_2)
    );
    m_server->Get(R"(/day/(\d+))",
        std::bind(&AdventApp::RequestDay, this, std::placeholders::_1, std::placeholders::_2)
    );
    m_server->Get(R"(/content/(\d+)/(.*))",
        std::bind(&AdventApp::RequestDayContent, this, std::placeholders::_1, std::placeholders::_2)
    );

    // Port config
    std::string serverHost = config["Server"]["Host"];
    int serverPort = config["Server"]["Port"];
    m_server->bind_to_port(serverHost, serverPort);

    // Listen
    spdlog::info("Starting webserver {}:{}", serverHost, serverPort);
    m_server->listen_after_bind();

    spdlog::info("Application terminated successfully");
    return 0;
}

void Advent::AdventApp::SetupSpdlog()
{
    spdlog::default_logger()->set_pattern("[%d.%m.%Y %H:%M:%S.%e] [%^%l%$] [%t] %v");
}

void Advent::AdventApp::RequestAuthentication(const httplib::Request& request, httplib::Response& response)
{
    // https://localhost/auth?token=YwJsUy22TrQfFnPmb4bVS459qcaB48KZn5Wqq77VZRDYZ365Tu7k7G4Ub9JzZ3AT

    if (request.has_param("token"))
    {
        // Get and validate token
        const std::string token = request.get_param_value("token");
        bool tokenValid = false;
        for (const auto& validToken : m_accessTokens)
        {
            if (token == validToken)
            {
                tokenValid = true;
                break;
            }
        }

        // Generate cookie
        if (tokenValid)
        {
            std::string cookieHeader = "ADVENT_TOKEN=" + token + "; Max-Age=3600; Secure; SameSite=Strict";
            response.set_header("Set-Cookie", cookieHeader);
        }
        else
        {
            std::string cookieHeader = "ADVENT_TOKEN=deleted; Max-Age=0; Secure; SameSite=Strict";
            response.set_header("Set-Cookie", cookieHeader);
        }
    }

    // Redirect
    response.set_redirect("/");
}

void Advent::AdventApp::RequestIndex(const httplib::Request& request, httplib::Response& response)
{
    if (IsAuthenticated(request))
    {
        nlohmann::json data = GenerateDefaultJson(request);

        inja::Environment jenv;
        std::string html = 
            jenv.render_file((m_serverDir / "views" / "index.jinja").generic_string(), data);
        response.set_content(html, "text/html;charset=utf-8");
    }
}

void Advent::AdventApp::RequestDay(const httplib::Request& request, httplib::Response& response)
{
    if (IsAuthenticated(request))
    {
        // Get day as match
        std::string daystr = request.matches[1];
        int day = AssertDay(daystr);

        // Call the day
        std::string text;
        ADVENT_ASSERT_FMT(m_days.Invoke(day, m_serverDir / "views", GenerateDefaultJson(request), text), "Failed to render day {}", day);

        // Send output
        response.set_content(text, "text/html;charset=utf-8");
    }   
}

void Advent::AdventApp::RequestDayContent(const httplib::Request& request, httplib::Response& response)
{
    if (IsAuthenticated(request))
    {
        // Get day as match
        std::string daystr = request.matches[1];
        std::string file = request.matches[2];
        int day = AssertDay(daystr);

        // Get days content
        std::string mime;
        char* data = nullptr;
        size_t dataSize = m_days.Content(day, file, &data, mime);

        if (data && dataSize) 
            response.set_content(data, dataSize, mime);
        if (data)
            delete[] data;

        ADVENT_ASSERT_FMT(dataSize, "Failed to get content of {} for day {}", file, day);
    }
}

int Advent::AdventApp::AssertDay(std::string daystr)
{
    int day = -1;
    std::stringstream ss;
    ss << daystr;
    ss >> day;

    // Validate day range
    ADVENT_ASSERT_FMT(day >= 1 && day <= 31, "Invalid day supplied got {} (\"{}\")", day, daystr);

    // Evaluate current day
    int currentDay = GetDecembersDay();
    spdlog::trace("Day {} requested on {} (Force: {})", day, currentDay, m_overrideDay);

    // Evaluate current day
    ADVENT_ASSERT_FMT(currentDay >= 1 && currentDay >= day, "Tried to request {} on {}", day, currentDay);

    return day;
}

int Advent::AdventApp::GetDecembersDay()
{
    // Forced day
    if (m_overrideDay > 0 && m_overrideDay <= 31)
    {
        return m_overrideDay;
    }

    // Get current time
    auto now = std::chrono::system_clock::now();
    time_t timeNow = std::chrono::system_clock::to_time_t(now);
    tm localTime = *localtime(&timeNow);

    // Is December
    if (localTime.tm_mon == 11)
    {
        return localTime.tm_mday;
    }

    return -1;
}

bool Advent::AdventApp::IsAuthenticated(const httplib::Request& request)
{
    // Authenticating everyone if no token is provided
    if (m_accessTokens.size() == 0)
        return true;

    // Authenticate using cookies
    if (request.has_header("Cookie"))
    {
        const std::string cookieHeader = request.get_header_value("Cookie");
        auto eqPos = cookieHeader.find_first_of('=');
        if (eqPos != std::string::npos)
        {
            const std::string cookieName = cookieHeader.substr(0, eqPos);
            const std::string cookieValue = cookieHeader.substr(eqPos + 1);

            if (cookieName == "ADVENT_TOKEN")
            {
                for (const auto& token : m_accessTokens)
                {
                    if (token == cookieValue)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

nlohmann::json Advent::AdventApp::GenerateDefaultJson(const httplib::Request& request)
{
    nlohmann::json data;
    data["HOST"] = "https://" + request.get_header_value("Host");
    data["DAY"] = GetDecembersDay();
    
    return data;
}

void Advent::AdventApp::RequestStop()
{
    if (m_server)
    {
        spdlog::info("Stopping webserver");
        m_server->stop();
    }
}

void Advent::AdventApp::SignalHandler(int signal)
{
    spdlog::info("Interrupt received!");
    if (s_instance)
    {
        s_instance->RequestStop();
    }
}

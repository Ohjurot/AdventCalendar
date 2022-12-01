#include "MimeResolver.h"

std::map<std::string, std::string> AdventLib::MimeResolver::m_mimeTypes{
    {"css", 	"text/css"},
    {"mpga", 	"audio/mpeg"},
    {"csv", 	"text/csv"},
    {"weba", 	"audio/webm"},
    {"txt", 	"text/plain"},
    {"wav", 	"audio/wave"},
    {"vtt", 	"text/vtt"},
    {"otf", 	"font/otf"},
    {"html",    "text/html"},
    {"htm", 	"text/html"},
    {"ttf", 	"font/ttf"},
    {"apng", 	"image/apng"},
    {"woff", 	"font/woff"},
    {"avif", 	"image/avif"},
    {"woff2", 	"font/woff2"},
    {"bmp", 	"image/bmp"},
    {"7z", 	    "application/x-7z-compressed"},
    {"gif", 	"image/gif"},
    {"atom", 	"application/atom+xml"},
    {"png", 	"image/png"},
    {"pdf", 	"application/pdf"},
    {"svg", 	"image/svg+xml"},
    {"mjs",     "application/javascript"},
    {"js", 	    "application/javascript"},
    {"webp", 	"image/webp"},
    {"json", 	"application/json"},
    {"ico", 	"image/x-icon"},
    {"rss", 	"application/rss+xml"},
    {"tif", 	"image/tiff"},
    {"tar", 	"application/x-tar"},
    {"tiff", 	"image/tiff"},
    {"xhtml",   "application/xhtml+xml"},
    {"xht", 	"application/xhtml+xml"},
    {"jpeg",    "image/jpeg"},
    {"jpg", 	"image/jpeg"},
    {"xslt", 	"application/xslt+xml"},
    {"mp4", 	"video/mp4"},
    {"xml", 	"application/xml"},
    {"mpeg", 	"video/mpeg"},
    {"gz", 	    "application/gzip"},
    {"webm", 	"video/webm"},
    {"zip", 	"application/zip"},
    {"mp3", 	"audio/mp3"},
    {"wasm", 	"application/wasm"},
};

bool AdventLib::MimeResolver::GetMimeType(const std::string& extension, std::string& mimeOut)
{
    auto itFind = m_mimeTypes.find(extension);
    if (itFind != m_mimeTypes.end())
    {
        mimeOut = itFind->second;
        return true;
    }

    return false;
}


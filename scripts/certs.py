# This action will generate the project

import subprocess
import colorama 

def osslGenerateNow(openssl, outKey, outCert):
    subprocess.run([openssl, "req", "-config", "./openssl.cnf", "-x509", "-newkey", "rsa:2048", "-keyout", outKey, "-out", outCert, "-sha256", "-days", "365", "-nodes", "-subj", "/CN=localhost"])

def generateCerts(openssl):
    print(colorama.Fore.YELLOW + "Generating self signed ssl certificates debuging" + colorama.Fore.RESET)
    osslGenerateNow(openssl, "./app/sslkey.pem", "./app/sslcert.pem")

def conanGenerateCerts():
    # Process conanbuildinfo
    infoFile = open("conanbuildinfo.txt", "r").readlines()
    infoFile = [info.strip() for info in infoFile]
    if "[bindirs]" in infoFile:
        # Find [bindirs] in data
        startIndex = infoFile.index("[bindirs]") + 1
        endIndex = startIndex
        while infoFile[endIndex] != "":
            endIndex += 1
        exePath = infoFile[startIndex:endIndex]
        
        # Find openssl 
        osslp = [path for path in exePath if path.find("openssl") != -1]
        if len(osslp) == 1:
            osslp = osslp[0] + "/openssl"
            # Run now
            generateCerts(osslp)
        else:
            print("Openssl not found in buildinfo")
    else:
        print("Invalid conanbuildinfo.txt! Run generate-project first.")

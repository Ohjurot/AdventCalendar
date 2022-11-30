import subprocess
import platform
import sys

if __name__ == "__main__":
    # Find out if we role on windows
    pfInfo = platform.uname()
    pfSystem = pfInfo.system.lower() 

    # Path to actual autogen script
    autogenScriptPath = f"./scripts/autogen_{pfSystem}.py"
    pythonCommand = "py" if pfSystem == "windows" else "python3"
    
    # Run python
    args = sys.argv[1::]
    subprocess.run([pythonCommand, autogenScriptPath, *args])

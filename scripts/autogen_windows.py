import subprocess
import sys
import os
import certs

if __name__ == "__main__":
    # Aquire required librarys
    conanBuildType = "Debug" if len(sys.argv) < 2 or sys.argv[1] != "--release" else "Release"
    subprocess.run(["conan", "install", ".", "--build", "missing", "-s", f"build_type={conanBuildType}"])

    # Create output dirs
    os.makedirs("./build/x86_64-debug/bin", exist_ok=True)
    os.makedirs("./build/x86_64-release/bin", exist_ok=True)

    # Create links:
    os.system(f"mklink /D \"{os.path.abspath('./build/x86_64-debug/bin/ExamplePlugin.d')}\" \"{os.path.abspath('./ExamplePlugin.d')}\"")
    os.system(f"mklink /D \"{os.path.abspath('./build/x86_64-release/bin/ExamplePlugin.d')}\" \"{os.path.abspath('./ExamplePlugin.d')}\"")

    # Call preamke to generate visual studio solution
    subprocess.run(["./vendor/premake/premake5.exe", "vs2022"])

    # Generate debug certificate
    certs.conanGenerateCerts()

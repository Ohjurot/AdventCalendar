import subprocess
import sys
import shutil
import certs

if __name__ == "__main__":
    # Aquire required librarys
    conanBuildType = "Debug" if len(sys.argv) < 2 or sys.argv[1] != "--release" else "Release"
    subprocess.run(["conan", "install", ".", "--build", "missing", "-s", f"build_type={conanBuildType}"])

    # Call preamke to generate visual studio solution
    subprocess.run(["./vendor/premake/premake5", "gmake2"])

    # Generate debug certificate
    certs.conanGenerateCerts()

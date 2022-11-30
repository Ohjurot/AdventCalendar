import subprocess
import sys
import shutil
import certs

if __name__ == "__main__":
    # Aquire required librarys
    conanBuildType = "Debug" if len(sys.argv) < 2 or sys.argv[1] != "--release" else "Release"
    subprocess.run(["conan", "install", ".", "--build", "missing", "-s", f"build_type={conanBuildType}"])

    # Call preamke to generate visual studio solution
    subprocess.run(["./vendor/premake/premake5.exe", "vs2022"])

    # Copy conan dependcies
    shutil.copytree("./build/dependencies", f"./build/x86_64-{conanBuildType.lower()}/bin", copy_function = shutil.copy2, dirs_exist_ok=True)

    # Generate debug certificate
    certs.conanGenerateCerts()

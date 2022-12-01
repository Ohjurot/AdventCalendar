import subprocess
import platform
import shutil
import os

def dpack():
    # Make package dir
    if not os.path.exists("./package"):
        os.mkdir("./package")

    # Get version of package
    version = open('VERSION', 'r').read().strip()
    print(f"Using version {version} from VERSION file.")

    # Assert new directory
    packageDir = f"./package/advent-calendar_{version}_amd64"
    if os.path.exists(packageDir):
        shutil.rmtree(packageDir)
    os.mkdir(packageDir) 

    # Create infrastructure
    os.mkdir(f"{packageDir}/DEBIAN")
    controlFile = open(f"{packageDir}/DEBIAN/control","w+")
    controlFile.write(f"Package: advent-calendar\n")
    controlFile.write(f"Version: {version}\n")
    controlFile.write(f"Architecture: amd64\n")
    controlFile.write(f"Essentials: no\n")
    controlFile.write(f"Priority: optional\n")
    controlFile.write(f"Depends: openssl\n")
    controlFile.write(f"Maintainer: Ludwig Fuechsl\n")
    controlFile.write(f"Description: Package for the advencalendar\n")
    controlFile.write(f"Installed-Size: 0\n")
    controlFile.close()

    # Copy scripts
    shutil.copyfile("./deb/postinst", f"{packageDir}/DEBIAN/postinst")
    os.chmod(f"{packageDir}/DEBIAN/postinst", 0o0775)
    shutil.copyfile("./deb/prerm", f"{packageDir}/DEBIAN/prerm")
    os.chmod(f"{packageDir}/DEBIAN/prerm", 0o0775)
    shutil.copyfile("./deb/postrm", f"{packageDir}/DEBIAN/postrm")
    os.chmod(f"{packageDir}/DEBIAN/postrm", 0o0775)

    # Copy all binary files
    os.mkdir(f"{packageDir}/usr")
    os.mkdir(f"{packageDir}/usr/bin")
    shutil.copyfile("./build/x86_64-release/bin/AdventCalendar", f"{packageDir}/usr/bin/advent-calendar")

    # Copy application data
    os.mkdir(f"{packageDir}/usr/bin/advent-calendar.d/")
    os.mkdir(f"{packageDir}/usr/bin/advent-calendar.d/plugins")
    shutil.copytree("./app/views", f"{packageDir}/usr/bin/advent-calendar.d/views")
    shutil.copytree("./app/www-data", f"{packageDir}/usr/bin/advent-calendar.d/www-data") 

    # Create config dirs
    os.mkdir(f"{packageDir}/etc")
    os.mkdir(f"{packageDir}/etc/advent-calendar")
    shutil.copyfile("./app/advent_conf_linux.json", f"{packageDir}/etc/advent-calendar/config.json")

    #  Service 
    os.mkdir(f"{packageDir}/etc/systemd")
    os.mkdir(f"{packageDir}/etc/systemd/system")
    shutil.copyfile("./deb/advent-calendar.service", f"{packageDir}/etc/systemd/system/advent-calendar.service")

    # Build package
    subprocess.run(["dpkg-deb", "--build", "--root-owner-group", packageDir])


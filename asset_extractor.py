import sys
import pathlib
import zipfile

def get_datadir() -> pathlib.Path:
    home = pathlib.Path.home()

    if sys.platform == "win32":
        return home / "AppData/Roaming/.minecraft/"
    elif sys.platform == "linux":
        return home / ".minecraft/"
    elif sys.platform == "darwin":
        return home / "Library/Application Support/minecraft/"
        

jar_path = get_datadir() / "versions/1.8.9/1.8.9.jar"

if jar_path.exists():
    print("Extracting 1.8.9.jar -> Game/assets")
else:
    print("Please run 1.8.9 at least once - from the MC Launcher.")
    sys.exit()

archive = zipfile.ZipFile(jar_path)
for file in archive.namelist():
    if file.startswith('assets/'):
        archive.extract(file, 'Game/')
CMake
=====
> If you are already familiar with cmake skip to your platform's building instructions.

1. Download and install [CMake](https://cmake.org/download/).
2. Add CMake to your path if the installer didn't add it for you.

Windows
-------
1. Open your Environment Variables in the System properties menu.
2. Under System Variables select Path and click 'Edit...'
3. Append to the Path a semicolon and the string: C:\Program Files\CMake\bin
4. Save, start a new cmd prompt and type cmake, you should see output from cmake.


Building
========

Windows
-------

To build on windows generate a Visual Studio 2017 solution file with CMake.

1. Make sure a recursive clone of the project is done to download all submodules.
2. Enter the working directory.
3. Create a build directory for CMake artifacts.
4. Create visual studio project files.
5. Build with visual studio.

```bash
git clone --recursive https://github.com/Blizzard/s2client-api
cd s2client-api
mkdir build
cd build
cmake ../ -G "Visual Studio 15 2017 Win64"
start s2client-api.sln
```

Mac
-------

To build on Mac generate an Xcode project with CMake.

1. Make sure a recursive clone of the project is done to download all submodules.
2. Enter the working directory.
3. Create a build directory for CMake artifacts.
4. Create Xcode project files.
5. Build with Xcode.

```bash
git clone --recursive https://github.com/Blizzard/s2client-api
cd s2client-api
mkdir build
cd build
cmake ../ -G Xcode
open s2client-api.xcodeproj/
```

Linux
-----

To build on Linux generate makefiles with CMake.

1. Make sure a recursive clone of the project is done to download all submodules.
2. Enter the working directory
3. Create a build directory for CMake artifacts.
4. Create makefiles
5. Build with make

```bash
git clone --recursive https://github.com/Blizzard/s2client-api
cd s2client-api
mkdir build
cd build
cmake ../
make
```

Working with submodules
=======================

If you cloned the repo without specifying --recursive you can initialize and checkout
the required submodules with the following git command -

```bash
git submodule update --init --recursive
```

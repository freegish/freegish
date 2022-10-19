# How to compile (Linux)

Make sure the following dependencies are installed:

* cmake
* SDL2
* openal
* libvorbis
* libpng

## Build for development

```sh
mkdir build
(cd build && cmake .. make)
./freegish
```

## Install

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DINSTALL_FHS=ON
make
make install
```

# How to compile (Windows with MSVC/VS)

To develop on Windows the following tools need to be installed:
- CMake
- Visual Studio 2022
- vcpkg


Below is described how to install and set them up.

## CMake

CMake is used for building Freegish and can be downloaded from [here](https://cmake.org/download/). Make sure to use the installer and select "Add CMake to system PATH" during installation.

## Visual Studio 2022

Installing Visual Studio installs the IDE, compiler and some other tools required to build Freegish. Get the community version installer from [here](https://visualstudio.microsoft.com/) and install the workload "Desktop development with C++" with it.

## vcpkg

vcpkg is a package manager for C and C++ which allows the libraries used to be installed. Installation instructions can be found [https://vcpkg.io/en/getting-started.html]. This README assumes that vcpkg is installed in `C:\vcpkg`.

Then install the required package with the following command:

    C:\vcpkg\vcpkg.exe install --triplet x64-windows openal-soft libogg libvorbis libpng sdl2 opengl

## Compiling

To build the game open powershell in the directory in which Freegish is cloned then simply issue:

    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
    cmake --build . --config Release

After that copy the content of the Release directory to the freegish directory. To start the game run freegish.exe


## Using Visual Studio

To get Freegish developing going in Visual Studio, a project file needs to be generated. This can be done with the following commands in a terminal opened in the directory in which Freegish is cloned:

     cmake -DCMAKE_TOOLCHAIN_FILE=E:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -G "Visual Studio 17 2022" .

Then open FreeGish.sln with Visual Studio.

Before being able to run from Visual Studio, the debug configuration has to be set. To do this, press the small arror down next to the run button with "Local Windows Debugger" next to it on the screen, then pick "All_BUILD Debug Properties". Set "Output Directory" to `$(SolutionDir)` and "Target Name" to `Debug/freegish.exe` like in this image:

![](visualstudio.png)

Now you just just start debugging like normal by pressing the run button or F5.

# How to play

Simply run `./gish` or `gish.exe`. There are some assets included (codenamed the *FreeGish* project), making Gish a completely free game!

If you own the original assets, you may also copy those into this directory. You'll need:

- animation
- level
- music
- sound
- texture
- tile01 ... tile07

Afterwards run `rename-levels.sh DIRECTORY_WITH_ASSETS` script to rename files so that they match Freegish naming convention (see https://github.com/freegish/freegish/issues/2#issuecomment-48749365).

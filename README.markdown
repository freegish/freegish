# How to compile (Linux)

Make sure CMake and the development libraries for SDL, OpenAL and Vorbis are installed. In Debian-based systems, you need to install the following packages:

    cmake libsdl1.2-dev libopenal-dev libvorbis-dev

Then simply issue

    mkdir build && cd build
    cmake ..
    make

# How to compile (Windows with MSVC/VS)

Make sure Visual Studio, CMake and vcpkg are installed. Then install the required package with the following command:

    vcpkg install --triplet x64-windows openal-soft libogg libvorbis libpng sdl2 opengl

Then simply issue

    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
    cmake --build . --config Release

After that copy the content of the Release directory to the freegish directory.

# How to play

Simply run `./gish` or `gish.exe`. There are some assets included (codenamed the *FreeGish* project), making Gish a completely free game! There is also one level available, `freegish.lvl`, you'll find it under "Custom Levels". Try it and replace what you don't like.

If you own the original assets, you may also copy those into this directory. You'll need:

- animation
- level
- music
- sound
- texture
- tile01 ... tile07

Afterwards run `rename-levels.sh DIRECTORY_WITH_ASSETS` script to rename files so that they match Freegish naming convention (see https://github.com/freegish/freegish/issues/2#issuecomment-48749365).

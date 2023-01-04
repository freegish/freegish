cmake_minimum_required(VERSION 2.9)

project(FreeGish C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-extras)

option(INSTALL_FHS "Install into Linux FHS" OFF)
set(OpenGL_GL_PREFERENCE LEGACY)

find_package(SDL2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Vorbis REQUIRED)
find_package(PNG REQUIRED)
find_library(LIBM m)

if(NOT INSTALL_FHS)
    set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}")
endif()

install(DIRECTORY animation DESTINATION share/freegish)
install(DIRECTORY level DESTINATION share/freegish)
install(DIRECTORY sound DESTINATION share/freegish)
install(DIRECTORY texture DESTINATION share/freegish)
install(FILES "freegish.bmp" DESTINATION share/freegish)
install(FILES "icons/freegish.svg" DESTINATION share/icons/hicolor/scalable/apps)
install(FILES "icons/freegish.xpm" DESTINATION share/pixmaps)
install(FILES "icons/freegish.desktop" DESTINATION share/applications)
install(FILES "freegish.6" TYPE MAN)

add_subdirectory(src)

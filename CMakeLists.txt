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

install(DIRECTORY animation DESTINATION ${CMAKE_INSTALL_DATADIR}/freegish)
install(DIRECTORY level DESTINATION ${CMAKE_INSTALL_DATADIR}/freegish)
install(DIRECTORY sound DESTINATION ${CMAKE_INSTALL_DATADIR}/freegish)
install(DIRECTORY texture DESTINATION ${CMAKE_INSTALL_DATADIR}/freegish)
install(FILES "freegish.bmp" DESTINATION ${CMAKE_INSTALL_DATADIR}/freegish)
install(FILES "icons/freegish.svg" DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES "icons/freegish.xpm" DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
install(FILES "icons/freegish.desktop" DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES "freegish.6" TYPE MAN)

add_subdirectory(src)

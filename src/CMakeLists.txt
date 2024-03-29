
set(GISH_SRCS
    audio/audio.c
    game/ai.c
    game/animation.c
    game/block.c
    game/boss.c
    game/config.c
    game/credits.c
    game/custom.c
    game/damage.c
    game/debug.c
    game/editor.c
    game/game.c
    game/gameaudio.c
    game/gamemenu.c
    game/gameobject.c
    game/gametexture.c
    game/high.c
    game/level.c
    game/lighting.c
    game/logic.c
    game/mainmenu.c
    game/mappack.c
    game/music.c
    game/objedit.c
    game/objfunc.c
    game/options.c
    game/physics.c
    game/player.c
    game/prerender.c
    game/random.c
    game/record.c
    game/render.c
    game/replay.c
    game/ropeedit.c
    game/setup.c
    game/socket.c
    game/sprite.c
    game/vsmode.c
    input/joystick.c
    input/keyboard.c
    input/mouse.c
    math/intersec.c
    math/vector.c
    menu/menu.c
    parser/parser.c
    physics/bond.c
    physics/object.c
    physics/particle.c
    sdl/endian.c
    sdl/event.c
    sdl/file.c
    sdl/video.c
    video/glfunc.c
    video/opengl.c
    video/text.c
    video/texture.c
    main.c
)

set(GISH_H
    audio/audio.h
    game/ai.h
    game/animation.h
    game/block.h
    game/boss.h
    game/config.h
    game/credits.h
    game/custom.h
    game/damage.h
    game/debug.h
    game/editor.h
    game/english.h
    game/game.h
    game/gameaudio.h
    game/gamemenu.h
    game/gameobject.h
    game/gametexture.h
    game/high.h
    game/level.h
    game/lighting.h
    game/logic.h
    game/mainmenu.h
    game/mappack.h
    game/music.h
    game/objedit.h
    game/objfunc.h
    game/options.h
    game/physics.h
    game/player.h
    game/prerender.h
    game/random.h
    game/record.h
    game/render.h
    game/replay.h
    game/ropeedit.h
    game/setup.h
    game/socket.h
    game/sprite.h
    game/vsmode.h
    input/joystick.h
    input/keyboard.h
    input/mouse.h
    math/intersec.h
    math/math.h
    math/vector.h
    menu/menu.h
    parser/parser.h
    physics/bond.h
    physics/object.h
    physics/particle.h
    sdl/endian.h
    sdl/event.h
    sdl/file.h
    sdl/sdl.h
    sdl/video.h
    video/glfunc.h
    video/opengl.h
    video/text.h
    video/texture.h
    config.h
)

if(WIN32)
    list(APPEND GISH_H video/glext.h)
endif(WIN32)

if(WIN32)
    add_executable(freegish WIN32 ${GISH_SRCS} ${GISH_H})
else()
    add_executable(freegish ${GISH_SRCS} ${GISH_H})
endif()

target_include_directories(freegish PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${Vorbis_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
)

target_link_libraries(freegish
    ${SDL2_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${Vorbis_LIBRARIES}
    ${PNG_LIBRARIES}
)

if(LIBM)
  target_link_libraries(freegish m)
endif(LIBM)

if(INSTALL_FHS)
  target_compile_definitions(freegish PRIVATE DATAPATH="${CMAKE_INSTALL_PREFIX}/share/freegish")
endif()

install(TARGETS freegish)

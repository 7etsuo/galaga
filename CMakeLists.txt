cmake_minimum_required(VERSION 3.10)
project(galaga C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")

set(SOURCES
    main.c
    terminal.c
    input.c
    entities.c
    collision.c
    enemy_ai.c
    game_state.c
    bonus_stage.c
    renderer.c
)

set(HEADERS
    terminal.h
    input.h
    entities.h
    collision.h
    enemy_ai.h
    game_state.h
    bonus_stage.h
    renderer.h
)

add_executable(galaga ${SOURCES} ${HEADERS})

target_link_libraries(galaga m)

install(TARGETS galaga DESTINATION bin)


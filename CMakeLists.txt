cmake_minimum_required(VERSION 3.20)
project(ZooEngine)

include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.14
)

FetchContent_MakeAvailable(SDL3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ZooEngine STATIC
	src/main.cpp
)

target_include_directories(ZooEngine PUBLIC
	include/
)

add_executable(ZooGame
	game/main.cpp
)

target_link_libraries(ZooGame PRIVATE ZooEngine SDL3::SDL3)

add_custom_command(TARGET ZooGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:ZooGame>
)
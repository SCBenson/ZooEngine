cmake_minimum_required(VERSION 3.20)
project(ZooEngine)

include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.14
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.92.9b
)

FetchContent_MakeAvailable(imgui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(imgui STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

add_library(ZooEngine STATIC
	src/main.cpp
)

target_include_directories(ZooEngine PUBLIC
	include/
)

add_executable(ZooGame
	game/main.cpp
)

target_link_libraries(ZooGame PRIVATE ZooEngine SDL3::SDL3 imgui)

add_custom_command(TARGET ZooGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:ZooGame>
)
cmake_minimum_required(VERSION 3.20)
project(chess_clock LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    fltk
    GIT_REPOSITORY https://github.com/fltk/fltk.git
    GIT_TAG release-1.4.5
    GIT_SHALLOW TRUE
)
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fltk)

add_executable(chess_clock WIN32
    src/main.cpp
    src/chess_clock.cpp
    src/prefs.cpp
    src/settings_dialog.cpp
)
target_link_libraries(chess_clock PRIVATE fltk)
target_include_directories(chess_clock PRIVATE src)

if(MSVC)
    target_compile_options(chess_clock PRIVATE /utf-8)
endif()

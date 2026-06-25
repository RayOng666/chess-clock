cmake_minimum_required(VERSION 3.20)
project(chess_clock LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    fltk
    GIT_REPOSITORY https://github.com/fltk/fltk.git
    GIT_TAG release-1.4.2
    GIT_SHALLOW TRUE
)
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBPNG OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fltk)

add_executable(chess_clock WIN32
    src/main.cpp
    src/chess_clock.cpp
    src/prefs.cpp
    src/settings_dialog.cpp
)
target_link_libraries(chess_clock PRIVATE fltk::fltk)
target_include_directories(chess_clock PRIVATE src)

if(MSVC)
    target_compile_options(chess_clock PRIVATE /utf-8)
    set_target_properties(chess_clock PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    if(MSVC)
        target_link_options(chess_clock PRIVATE /OPT:REF /OPT:ICF)
    else()
        target_link_options(chess_clock PRIVATE -s)
    endif()
endif()

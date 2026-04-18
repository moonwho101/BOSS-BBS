cmake_minimum_required(VERSION 3.20)
project(boss VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC-specific flags
if(MSVC)
    add_compile_options(/W3 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

set(SOURCES
    main.cpp
    boss.cpp
    console.cpp
    serial.cpp
    fileio.cpp
    users.cpp
    menus.cpp
    messages.cpp
    editor.cpp
    files.cpp
    timer.cpp
    sysop.cpp
)

add_executable(boss ${SOURCES})
target_include_directories(boss PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link Win32 libraries
target_link_libraries(boss PRIVATE kernel32 user32)

# Install to BOSS directory for easy testing
install(TARGETS boss DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../")

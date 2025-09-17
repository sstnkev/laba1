cmake_minimum_required(VERSION 3.26)
project(calculC)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_executable(calculC
        main.c
        stack.c
        stack.h
        stackChar.c
        stackChar.h
)

target_link_libraries(calculC ${GTK3_LIBRARIES} m)

# Add compiler flags for GTK if necessary
add_compile_options(${GTK3_CFLAGS_OTHER})
cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell)


set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(shell 
    src/main.c
    src/command.c
    src/redirects.c
    src/autocomplete.c
)

target_include_directories(shell PRIVATE src/include)

if (NOT WIN32)
    target_link_libraries(shell PRIVATE readline)
endif()


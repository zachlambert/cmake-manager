cmake_minimum_required(VERSION 3.14)
project(cmake-manager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(argparse REQUIRED)

add_executable(cmm
    src/cli.cpp
    src/config.cpp
    src/exec.cpp
    src/main.cpp
)
target_include_directories(cmm PRIVATE include)
target_link_libraries(cmm argparse)

# Install target

install(TARGETS cmm
    RUNTIME DESTINATION bin COMPONENT Runtime
)

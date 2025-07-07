cmake_minimum_required(VERSION 3.15)
project(HysteriaCLI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(HysteriaCLI main.cpp)

# Add the core AI source files from UE project (adjust relative paths)
target_sources(HysteriaCLI PRIVATE
    ../Source/Hysteria/Private/CoreAI/TestClass.cpp
)

# Add include directories so compiler can find headers
target_include_directories(HysteriaCLI PRIVATE
    ../Source/Hysteria/Public
)

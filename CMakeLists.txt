cmake_minimum_required(VERSION 3.14)
project(tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add raylib subdirectory
add_subdirectory(external/raylib)

# Create executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
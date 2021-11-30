cmake_minimum_required(VERSION 2.8)
project(Perihelion)

set(EXECUTABLE_NAME "Perihelion")

find_package(SFML 2.5 COMPONENTS system graphics window REQUIRED)
find_package(TGUI 0.9 REQUIRED)

# Set the include directory of the project
include_directories("${CMAKE_SOURCE_DIR}/include" ${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR})

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++2a -fconcepts -Wall -Wextra -pedantic -pedantic-errors -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wunreachable-code -Wmissing-declarations")


# List all source files
set(SOURCE_FILES
	src/main.cpp
	src/PhysicalModel.cpp
)

# Create the main executable
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} tgui sfml-graphics sfml-window sfml-system)

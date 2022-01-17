cmake_minimum_required(VERSION 3.0)
project(Perihelion)

find_package(SFML 2.5 COMPONENTS system graphics window audio REQUIRED)
find_package(TGUI 0.10 REQUIRED)

# Set the include directory of the project
include_directories("${CMAKE_SOURCE_DIR}/include")

# Set compiler flags
set (CMAKE_CXX_STANDARD 20)
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pedantic-errors -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wunreachable-code -Wmissing-declarations")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()


# List all source files
set(SOURCE_FILES
	src/systems/CollisionSystem.cpp
	src/systems/GameplaySystem.cpp
	src/systems/LightSystem.cpp
	src/systems/PhysicsSystem.cpp
	src/systems/RenderSystem.cpp
	src/Animation.cpp
	src/Application.cpp
	src/components.cpp
	src/DebugInfo.cpp
	src/DebugOverlay.cpp
	src/format.cpp
	src/InputManager.cpp
	src/main.cpp
	src/MusicManager.cpp
	src/Scene.cpp
	src/SceneLoader.cpp
)

# Create the main executable
set(EXECUTABLE_NAME Perihelion)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} tgui sfml-audio sfml-graphics sfml-window sfml-system)

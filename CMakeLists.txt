cmake_minimum_required(VERSION 3.0)
project(Aphelion)

if (NOT DEFINED COMPILE_STATIC)
    set(COMPILE_STATIC FALSE)
endif()

if (NOT DEFINED COMPILE_TESTS)
    set(COMPILE_TESTS FALSE)
endif()

set(SFML_STATIC_LIBRARIES ${COMPILE_STATIC})
set(TGUI_STATIC_LIBRARIES ${COMPILE_STATIC})

find_package(SFML 2.5 COMPONENTS system graphics window audio REQUIRED)
find_package(TGUI 0.10 REQUIRED)

# Set the include directory of the project
include_directories("${CMAKE_SOURCE_DIR}/include")

# Set compiler flags
set (CMAKE_CXX_STANDARD 20)
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} \
-Wall -Wextra -pedantic -pedantic-errors -Winit-self -Wcast-align -Wundef \
-Wunreachable-code -Wmissing-declarations -Walloc-zero -Walloca \
-Warith-conversion -Wcast-align -Wcast-qual -Wclass-memaccess -Wcomma-subscript \
-Wconditionally-supported -Wconversion -Wctad-maybe-unsupported \
-Wctor-dtor-privacy -Wdate-time -Wdeprecated-copy -Wdeprecated-copy-dtor \
-Wdeprecated-enum-enum-conversion -Wdeprecated-enum-float-conversion \
-Wdisabled-optimization -Wdouble-promotion -Wduplicated-branches \
-Wduplicated-cond -Wextra-semi -Wfloat-equal -Wformat -Wformat-signedness \
-Winvalid-imported-macros -Winvalid-pch -Wlogical-op -Wmismatched-tags \
-Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs \
-Wmissing-noreturn -Wmultichar -Wnon-virtual-dtor \
-Wnull-dereference -Wold-style-cast -Woverloaded-virtual -Wpacked \
-Wpacked-bitfield-compat -Wpessimizing-move -Wplacement-new=2 -Wredundant-decls \
-Wredundant-tags -Wregister -Wrestrict -Wshadow -Wshift-negative-value \
-Wsign-promo -Wstack-protector -Wstrict-aliasing -Wstrict-null-sentinel \
-Wstrict-overflow -Wstring-compare -Wstringop-truncation -Wsuggest-final-methods \
-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsynth \
-Wtrampolines -Wundef -Wunreachable-code -Wunsafe-loop-optimizations \
-Wuseless-cast -Wvector-operation-performance -Wvla -Wvolatile \
-Wwrite-strings -Wzero-as-null-pointer-constant")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
    if (COMPILE_STATIC)
	       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
	       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
endif()

# List all source files
set(SOURCE_FILES
    src/components/Body.cpp
    src/components/Animations.cpp
    src/components/components.cpp
    src/states/AbstractState.cpp
    src/states/GameState.cpp
    src/states/LoadGameState.cpp
    src/states/MainMenuState.cpp
    src/states/MapState.cpp
    src/states/PauseState.cpp
    src/states/SaveGameState.cpp
    src/states/SettingsState.cpp
    src/states/StateStack.cpp
	src/systems/AnimationSystem.cpp
	src/systems/AutoPilotSystem.cpp
	src/systems/CollisionSystem.cpp
	src/systems/GameplaySystem.cpp
	src/systems/LightSystem.cpp
	src/systems/PhysicsSystem.cpp
	src/systems/RenderSystem.cpp
	src/systems/SoundEffectsSystem.cpp
	src/Animation.cpp
	src/Application.cpp
	src/MusicManager.cpp
	src/Paths.cpp
    src/polygon.cpp
	src/Scene.cpp
	src/SceneSerializer.cpp
)

# Create the main executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES} src/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} tgui sfml-audio sfml-graphics sfml-window sfml-system)

# Create the test executable
if (COMPILE_TESTS)
    set(TEST_FILES
        test/polygon.cpp
        src/polygon.cpp
        src/components/Body.cpp
        test/vector.cpp
    )
    include_directories("${CMAKE_SOURCE_DIR}/test")
    add_executable(tests ${TEST_FILES} test/main.cpp)
    target_link_libraries(tests tgui sfml-audio sfml-graphics sfml-window sfml-system)
endif()

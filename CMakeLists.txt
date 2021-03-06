cmake_minimum_required (VERSION 3.1)

project (synthlike LANGUAGES CXX)

set (CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (PROJECT_BASE_SRC "src")
set (PROJECT_SOURCE_DIR "${PROJECT_BASE_SRC}/source")
set (PROJECT_BINARY_DIR "bin")
set (PROJECT_INCLUDE_DIR "${PROJECT_BASE_SRC}/include")
set (PROJECT_LIBRARY_DIR "lib")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_LIBRARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_LIBRARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}")

set (CFG_PROJECT_NAME "${CMAKE_PROJECT_NAME}")
set (CFG_VERSION_MAJOR 0)
set (CFG_VERSION_MINOR 1)
set (CFG_DEBUG true)
set (CFG_IMGUI_ENABLED true)

if (MSVC)
	add_definitions(-DNOMINMAX)
endif (MSVC)


configure_file (
	"${PROJECT_BASE_SRC}/config.hpp.in"
	"${PROJECT_BINARY_DIR}/config.hpp"
)
include_directories ("${PROJECT_BASE_SRC}/")
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_INCLUDE_DIR}")
include_directories ("${PROJECT_INCLUDE_DIR}/*")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/gl3w/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui/examples")

file (GLOB source_files
	"${PROJECT_BASE_SRC}/ecs/**/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*/*.cpp"
	"${PROJECT_LIBRARY_DIR}/*/*.cpp"
	"${PROJECT_LIBRARY_DIR}/gl3w/src/gl3w.cpp" # TODO: bad fix, work this out!
	"${PROJECT_LIBRARY_DIR}/imgui/examples/imgui_impl_glfw.cpp"
	"${PROJECT_LIBRARY_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
)

include(${CMAKE_TOOLCHAIN_FILE})

add_executable (${CMAKE_PROJECT_NAME} "${PROJECT_BASE_SRC}/main.cpp" ${source_files})

# glfw
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)

# cereal
find_package(cereal CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE cereal)

# assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE assimp::assimp)

# entt
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE EnTT::EnTT)

# openal soft
find_package(OpenAL CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenAL::OpenAL)

# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm)

# lua
include(FindLua)
find_package(Lua REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})

# stb
#find_path(STB_INCLUDE_DIRS "stb.h")
#target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})

# sol2
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sol2::sol2)

# freetype2
find_package(freetype CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE freetype)

# OpenGL
set (OpenGL_GL_PREFERENCE GLVND)
find_package (OpenGL REQUIRED)

if (MSVC)
	target_link_libraries (${CMAKE_PROJECT_NAME} PRIVATE opengl32)
else (MSVC)
	target_link_libraries (${CMAKE_PROJECT_NAME} PRIVATE OpenGL)
endif (MSVC)

add_custom_target (
	run ALL
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_ecs_includes.py ${CMAKE_CURRENT_SOURCE_DIR}
	BYPRODUCTS ecs/components.hpp ecs/systems.hpp ecs/events.hpp
	COMMENT "Generating ECS Includes"
)

if (MSVC)
	target_compile_options (${CMAKE_PROJECT_NAME} PRIVATE /W4 /O2)
else (MSVC)
	target_compile_options (${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-implicit-fallthrough -Wno-unused-parameter)
endif (MSVC)

install (TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/config.hpp"
         DESTINATION include)


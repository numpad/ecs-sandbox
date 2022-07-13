cmake_minimum_required (VERSION 3.1)

project (synthlike LANGUAGES CXX)

set (CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (PROJECT_BASE_SRC "src")
set (PROJECT_SOURCE_DIR "${PROJECT_BASE_SRC}/core")
set (PROJECT_BINARY_DIR "bin")
set (PROJECT_LIBRARY_DIR "lib")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_LIBRARY_DIR}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_LIBRARY_DIR}")
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
include_directories ("${PROJECT_SOURCE_DIR}")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/entt/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/dr/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/stb/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/par/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/gl3w/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui/include/imgui")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui/include/imgui/backends")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/yoga")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/glm")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/cereal/include/")

file (GLOB_RECURSE source_files
	"${PROJECT_BASE_SRC}/*.cpp"
)

file (GLOB library_source_files
	"${PROJECT_LIBRARY_DIR}/*/*.cpp"
	"${PROJECT_LIBRARY_DIR}/dr/include/dr/*.cpp"
	"${PROJECT_LIBRARY_DIR}/stb/include/stb/*.cpp"
	"${PROJECT_LIBRARY_DIR}/par/include/par/par_shapes.cpp"
	"${PROJECT_LIBRARY_DIR}/gl3w/src/gl3w.cpp" # TODO: bad fix, work this out!
	"${PROJECT_LIBRARY_DIR}/imgui/include/imgui/*.cpp"
	"${PROJECT_LIBRARY_DIR}/imgui/include/imgui/backends/imgui_impl_glfw.cpp"
	"${PROJECT_LIBRARY_DIR}/imgui/include/imgui/backends/imgui_impl_opengl3.cpp"
	"${PROJECT_LIBRARY_DIR}/yoga/yoga/*.cpp"
	"${PROJECT_LIBRARY_DIR}/yoga/yoga/*/*.cpp"
)

if (CMAKE_TOOLCHAIN_FILE)
	include(${CMAKE_TOOLCHAIN_FILE})
endif()

add_executable (${CMAKE_PROJECT_NAME} "${PROJECT_BASE_SRC}/main.cpp" ${source_files} ${library_source_files})

# glfw
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)

# openal soft
find_package(OpenAL CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenAL\:\:OpenAL)

# luajit
# WIP
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE luajit-5.1)

# freetype2
find_package(freetype CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE freetype)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt-header-only)


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


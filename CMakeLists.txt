cmake_minimum_required (VERSION 3.1)

project (synthlike LANGUAGES CXX)

set (CMAKE_BUILD_TYPE Debug)

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
configure_file (
	"${PROJECT_BASE_SRC}/config.hpp.in"
	"${PROJECT_BINARY_DIR}/config.hpp"
)
include_directories ("${PROJECT_BASE_SRC}/")
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_INCLUDE_DIR}")
include_directories ("${PROJECT_INCLUDE_DIR}/*")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/entt/single_include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/imgui/examples")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/assimp/include")
include_directories (SYSTEM "${PROJECT_LIBRARY_DIR}/assimp/code")

file (GLOB source_files
	"${PROJECT_BASE_SRC}/ecs/**/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*/*.cpp"
	"${PROJECT_LIBRARY_DIR}/*/*.cpp"
	"${PROJECT_LIBRARY_DIR}/imgui/examples/imgui_impl_glfw.cpp"
	"${PROJECT_LIBRARY_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
)

set (OpenGL_GL_PREFERENCE GLVND)
find_package (OpenGL REQUIRED)
find_package (glfw3 REQUIRED)
find_package (glm REQUIRED)

#add_subdirectory (lib/assimp)
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
find_package (assimp REQUIRED)


add_executable (${CMAKE_PROJECT_NAME} "${PROJECT_BASE_SRC}/main.cpp" ${source_files})
target_link_libraries (${CMAKE_PROJECT_NAME}
                       OpenGL
                       glfw
                       dl
                       assimp
)

if (MSVC)
	target_compile_options (${CMAKE_PROJECT_NAME} PRIVATE /W4 /WX)
else (MSVC)
	target_compile_options (${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-implicit-fallthrough)
endif (MSVC)

install (TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/config.hpp"
         DESTINATION include)


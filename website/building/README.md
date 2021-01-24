# Building

If you just want to download a precompiled binary, head over to the [Releases](https://github.com/numpad/ecs-sandbox/releases) instead!

## 1. Requirements


### 1.1 Build tools
Please download and install the following.
 - A C++17 compliant compiler
 - [CMake](https://cmake.org/)
 - [vcpkg](https://github.com/microsoft/vcpkg) (optional)
 - [git](https://git-scm.com/) (optional)

I will use [vcpkg](https://github.com/microsoft/vcpkg) to install the required libraries.
If you prefer to use your package manager or install the libraries manually you might need
to remove the line `include(${CMAKE_TOOLCHAIN_FILE})` from `CMakeLists.txt` possibly among
other things.


### 1.2 Source code
[Download](https://github.com/numpad/ecs-sandbox/archive/develop.zip) and unzip this repo or clone it:
```bash
git clone https://github.com/numpad/ecs-sandbox.git
```


### 1.3 Dependencies
After having set-up [vcpkg](https://github.com/microsoft/vcpkg), switch into its root dir
and install the required libraries with this command:

**Windows**:
```PowerShell
PS> .\vcpkg.exe install glfw3 cereal assimp entt openal-soft glm lua sol2 freetype --triplet=x64-windows-static
```
**Linux**:
```bash
$ ./vcpkg install glfw3 cereal assimp entt openal-soft glm lua sol2 freetype --triplet=x64-linux
```

Both versions expect a 64bit architecture. If you need to install the 32bit versions change
the command to `--triplet=x86-…`, for other architectures see the vcpkg documentation.  
Note: The linux libraries aren't linked statically.

Later on we will need to tell _CMake_ where the vcpkg _toolchain file_ is located so it can find our installed libraries.
Run the following command to make the installed libraries available for use and copy the path it prints to stdout:
```PowerShell
PS> .\vcpkg.exe integrate install
```
**Linux**:
```bash
$ ./vcpkg integrate install
```
The last line should be something in the form of
```
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=D:/Other Software/vcpkg-global/vcpkg/scripts/buildsystems/vcpkg.cmake"
                                                   ^^^^^^^^^^^^^^^^^^^^^^^^   copy this path   ^^^^^^^^^^^^^^^^^^^^^^^^^
```

# 2. Generate native makefiles
See the OS specific instructions for:
- [Windows](build_windows.md)
- [Linux](build_linux.md)

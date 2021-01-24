# Building for Windows
(Complete all steps from the [README](README.md) before)

## 1. Generating the `.sln`

### 1.1
Start `cmake-gui` and for both the source code and build dir select the path to this repository.

### 1.2
Add two cache entries
  - **CMAKE_TOOLCHAIN_FILE** : The path to your `vcpkg.cmake` from the previous step.
  - **VCPKG_TARGET_TRIPLET** : The triplet you used when installing the dependencies, probably `x64-windows-static`

<details>
  <summary>Screenshot</summary>
  
  ![Before Configuring](images/01_before_configure.png)
  
</details>
  
### 1.3
Click `Configure`, select _Visual Studio 16 2019_ as generator, input your platform (in this case _x64_) and press `Finish`

<details>
  <summary>Screenshot</summary>
  
  ![After Configuring](images/02_after_configure.png)
  
</details>

### 1.4
Since use static linking for our libraries, we need to fix the entries with names `CMAKE_CXX_FLAGS_DEBUG`,
`_RELEASE` etc. and replace the `/MD` inside their value with `/MT`. (likewise with `/MDd`).

<details>
  <summary>Screenshot</summary>
  
  ![Fix linker flags](images/03_static_linking.png)
  
</details>

### 1.5
Press `Configure` again, all entries should lose their red background.

<details>
  <summary>Screenshot</summary>
  
  ![Generating done](images/04_generating_done.png)
  
</details>

### 1.6
Now press generate and click on `Open Project`, Visual Studio should open the Solution.  

<details>
  <summary>Screenshot</summary>
  
  ![Visual Studio open](images/05_visual_studio_before_config.png)
  
</details>


## 2. Building the project

### 2.1
Go to `Project > Properties > C/C++ > Code Generation`, select your desired configuration (f.ex. `Release`)
in the top-left dropdown and set the value of `Runtime Library` to ´Multi-threaded (/MT)´ (or the _/MTd Debug_ version)

<details>
  <summary>Screenshot</summary>
  
  ![Select static C runtime library](images/06_runtime_library.png)
  
</details>

## 2.2
Now go to `Build > Build solution`. Once the build completes successfully, you should find a file `synthlike.exe`
in the projects `bin/`-folder. Copy the folder `res/` from the projects root directory next to the executable and
the program should run.

# Building for Windows
(Complete all steps from the [README](README.md) before)

## 1. Generating the `Makefile`

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/vcpkg/scripts/buildsystems/vcpkg.cmake .
```

## 2. Building the project

```bash
make
```

@echo off
setlocal
if not exist build (
    mkdir build
)
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
if errorlevel 1 exit /b %errorlevel%
cmake --build . -- /m
endlocal
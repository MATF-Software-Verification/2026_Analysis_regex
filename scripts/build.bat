@echo off
setlocal
@REM if not exist build (
@REM     mkdir build
@REM )
@REM cd build
cmake --preset=windows-vcpkg
if errorlevel 1 exit /b %errorlevel%
cmake --build --preset=windows-vcpkg
endlocal
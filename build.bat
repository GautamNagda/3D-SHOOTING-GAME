@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building CyberStrike 3D (MinGW + CMake / Direct GCC)
echo ========================================================

REM Create build directory
if not exist "build" mkdir build

echo [1/3] Configuring with CMake...
cmake -B build -G "MinGW Makefiles"
if %errorlevel% neq 0 (
    echo [WARNING] CMake configuration failed. Attempting direct GCC compilation fallback...
    goto direct_build
)

echo [2/3] Compiling with CMake...
cmake --build build
if %errorlevel% neq 0 (
    echo [WARNING] CMake build failed. Attempting direct GCC compilation fallback...
    goto direct_build
)

echo [3/3] Ensuring runtime dependencies and assets are present...
if exist "external\glfw\lib-mingw\glfw3.dll" (
    copy /y "external\glfw\lib-mingw\glfw3.dll" "build\" >nul
)
if not exist "build\shaders" mkdir "build\shaders"
copy /y "shaders\*.glsl" "build\shaders\" >nul

if not exist "build\assets\textures" mkdir "build\assets\textures"
copy /y "assets\textures\*.png" "build\assets\textures\" >nul

echo.
echo ========================================================
echo   Build Successful! Executable: build\CyberStrike3D.exe
echo ========================================================
exit /b 0

:direct_build
echo [FALLBACK] Compiling directly using MinGW g++...
g++ -std=c++14 -O2 ^
    -I include ^
    -I external ^
    -I external\glad\include ^
    -I external\glfw\include ^
    -I external\glm ^
    -I external\stb ^
    src\main.cpp ^
    src\Game.cpp ^
    src\Player.cpp ^
    src\Enemy.cpp ^
    src\Weapon.cpp ^
    src\Collision.cpp ^
    src\Particle.cpp ^
    src\Arena.cpp ^
    src\Camera.cpp ^
    src\UI.cpp ^
    src\Core\Window.cpp ^
    src\Core\Transform.cpp ^
    src\Renderer\Shader.cpp ^
    src\Renderer\CubeMesh.cpp ^
    src\Renderer\Texture.cpp ^
    external\glad\src\glad.c ^
    -L external\glfw\lib-mingw -lglfw3dll ^
    -lopengl32 -lgdi32 -luser32 -lkernel32 ^
    -o build\CyberStrike3D.exe

if %errorlevel% neq 0 (
    echo [ERROR] Direct GCC compilation failed!
    exit /b %errorlevel%
)

if exist "external\glfw\lib-mingw\glfw3.dll" (
    copy /y "external\glfw\lib-mingw\glfw3.dll" "build\" >nul
)
if not exist "build\shaders" mkdir "build\shaders"
copy /y "shaders\*.glsl" "build\shaders\" >nul

if not exist "build\assets\textures" mkdir "build\assets\textures"
copy /y "assets\textures\*.png" "build\assets\textures\" >nul

echo.
echo ========================================================
echo   Direct Build Successful! Executable: build\CyberStrike3D.exe
echo ========================================================
exit /b 0

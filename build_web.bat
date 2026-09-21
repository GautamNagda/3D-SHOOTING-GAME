@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building CyberStrike 3D for Web (WebAssembly/WebGL2)
echo ========================================================

:: 1. Check if emcc is available, if not initialize EMSDK
where emcc >nul 2>nul
if %errorlevel% neq 0 (
    if exist "C:\Users\hp\emsdk\emsdk_env.bat" (
        echo [INFO] Activating EMSDK environment...
        call "C:\Users\hp\emsdk\emsdk_env.bat" >nul 2>nul
    ) else (
        echo [ERROR] EMSDK not found at C:\Users\hp\emsdk!
        echo Please ensure EMSDK is installed.
        exit /b 1
    )
)

where emcc >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] emcc is still not available after calling emsdk_env.bat!
    exit /b 1
)

:: 2. Ensure web directory exists
if not exist "web" mkdir "web"

:: 3. Compile C++ sources to WebAssembly
echo [INFO] Compiling C++ source files with em++...

call em++ -std=c++17 -O2 ^
    -s USE_GLFW=3 ^
    -s USE_WEBGL2=1 ^
    -s FULL_ES3=1 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s INITIAL_MEMORY=67108864 ^
    -s WASM=1 ^
    --preload-file assets@assets ^
    --preload-file shaders@shaders ^
    --shell-file web/shell.html ^
    -Iinclude ^
    -Iexternal ^
    src/main.cpp ^
    src/Game.cpp ^
    src/Player.cpp ^
    src/Enemy.cpp ^
    src/Weapon.cpp ^
    src/Collision.cpp ^
    src/Particle.cpp ^
    src/Arena.cpp ^
    src/Camera.cpp ^
    src/UI.cpp ^
    src/Core/Window.cpp ^
    src/Core/Transform.cpp ^
    src/Renderer/Shader.cpp ^
    src/Renderer/CubeMesh.cpp ^
    src/Renderer/Texture.cpp ^
    -o web/index.html

if %errorlevel% neq 0 (
    echo [ERROR] Web build failed! Check errors above.
    exit /b %errorlevel%
)

echo.
echo ========================================================
echo   Web Build Successful!
echo   Output files located in: web/
echo     - web/index.html
echo     - web/index.js
echo     - web/index.wasm
echo     - web/index.data
echo ========================================================

exit /b 0

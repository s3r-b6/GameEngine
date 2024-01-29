# This script should be run from inside the .\build directory
$include = ( "-I..\deps\",
    "-I..\deps\glad\",
    "-I..\deps\freetype2\",
    "-I..\deps\win32\",
    "-I..\deps\win32\SDL2\",
    "-I..\include" 
)

$libs = (
    "-luser32", "-lgdi32", "-lopengl32", "-L..\lib\", "-lSDL2",
    "-lfreetype"
)

$sources_main = (
    "..\src\main.cpp",
    "..\src\win32_platform.cpp",
    "..\src\initialization.cpp",
    "..\src\assets.cpp",
    "../src/fonts.cpp",
    "..\src\renderer.cpp",
    "..\deps\glad\glad.c"
)

$sources_game = (
    "..\src\assets.cpp",
    "..\src\renderer.cpp",
    "../src/fonts.cpp",
    "..\src\win32_platform.cpp",
    "..\deps\glad\glad.c"
)

$flags = (
    "-Wno-write-strings",
    "-D_REENTRANT",
    "-static-libstdc++", 
    "-pipe"
)

$current_directory = Get-Location

if ($current_directory -notlike "*\build")
{
    Write-Host "Running from wrong directory $current_directory"
    Write-Host "Should be running from .\build..."
    exit 1
}

if (-not (Test-Path ".\SDL2.dll"))
{
    Write-Host "SDL2.dll not found, copying it..."
    Copy-Item ..\lib\SDL2.dll .\SDL2.dll
}


$gamesrc_stamp = (Get-Item "..\src\game.cpp").LastWriteTime.Ticks
$gameobj_stamp = 0

if (Test-Path ".\game.dll")
{
    $gameobj_stamp = (Get-Item ".\game.dll").LastWriteTime.Ticks
}

Write-Host "Trying to recompile game.cpp"

if ($gamesrc_stamp -gt $gameobj_stamp)
{
    $timestamp = Get-Date -Format "yyyyMMddHHmmss"
    Write-Host "Game.cpp file is newer. Recompiling..."
    Remove-Item .\game_* -ErrorAction SilentlyContinue 
    Remove-Item .\game.dll -ErrorAction SilentlyContinue 
    g++ -fPIC $include $libs $flags -shared -o "game_$timestamp.dll" ..\src\game.cpp $sources_game
    Write-Host "Renaming game_$timestamp.dll to game.dll"
    Rename-Item -Path ".\game_$timestamp.dll" -NewName ".\game.dll"
} else
{
    Write-Host "Game.cpp file is not newer. No need to recompile."
}

if (-not (Get-Process -Name "game" -ErrorAction SilentlyContinue))
{
    Write-Host "Game is not running"
    Write-Host "Compiling main.cpp..."
    Remove-Item .\game.exe -ErrorAction SilentlyContinue 
    g++ $include $libs $flags -o game.exe $sources_main
} else
{
    Write-Host "Game is running, skipping main.cpp..."
}

Write-Host "Compilation completed."

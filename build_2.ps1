# This script should be run from inside the .\build directory
$include = ( "-I..\deps\" ) 

$libs = (
    "-luser32", "-lgdi32",
    "-L..\lib\", "-lSDL2", "-lGLEW", "-lopengl32", "-lGLU"
)

$sources_main = (
    "..\src\main.cpp",
    "..\src\initialization.cpp",
    "..\src\assets.cpp",
    "..\src\renderer.cpp"
)

$sources_game = (
    "..\src\assets.cpp",
    "..\src\renderer.cpp"
)

$flags = (
    "-Wno-write-strings"
)


$current_directory = Get-Location

if ($current_directory -notlike "*\build") {
    Write-Host "Running from wrong directory $current_directory"
    Write-Host "Should be running from .\build..."
    exit 1
}

$timestamp = Get-Date -Format "yyyyMMddHHmmss"

$running = Get-Process -Name "game" -ErrorAction SilentlyContinue

if ($running) {
    Write-Output 'Game is running'
    Remove-Item build\game_* -ErrorAction SilentlyContinue
    Write-Output "Start compiling build\game_$timestamp.dll..."
    g++ $libs $include $sources_game -g ..\src\game.cpp -shared -o .\game_$timestamp.dll $flags
    Remove-Item -ErrorAction SilentlyContinue build\game.dll
    Move-Item -ErrorAction SilentlyContinue .\build\game_$timestamp.dll .\game.dll
}
else {
    Write-Output "Could not find game through Get-Process 'game'. Assuming it is not running."
    Remove-Item build\game* -ErrorAction SilentlyContinue
    Write-Output "Start compiling build\game.dll..."
    g++ $include $sources_game -g ..\src\game.cpp -shared -o .\game.dll $flags
    Write-Output "Compiling game.exe"
    g++ $libs $include $sources_main -g ..\src\main.cpp -o .\game.exe $flags
}

Write-Output "Compilation completed."
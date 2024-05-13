$include = ( "-I..\deps\",
    "-I..\deps\glad\",
    "-I..\deps\freetype2\",
    "-I..\deps\win32\",
    "-I..\deps\win32\SDL2\",
    "-I..\src\engine\include",
    "-I..\src\game\include"
)

$libs = (
    "-luser32", "-lgdi32", "-lopengl32", "-L..\lib\", "-lSDL2",
    "-lfreetype"
)

$flags = (
    "-Wno-write-strings",
    "-D_REENTRANT",
    "-static-libstdc++", 
    "-Wno-deprecated-declarations",
    "-pipe"
)

# TODO: check this... I am running linux and don't have pwsh
$current_directory = Get-Location

if ($current_directory -notlike "*\build") {
    Write-Host "Not running from build directory."
    Write-Host "Attempting to change directory to build..."

    $build_directory = Join-Path $current_directory "build"
    if (Test-Path $build_directory) {
        Set-Location $build_directory
        Write-Host "Successfully changed to build directory."
    }
    else {
        Write-Host "Failed to change to build directory. Aborting script."
        exit 1
    }
}

if (-not (Test-Path ".\SDL2.dll")) {
    Write-Host "SDL2.dll not found, copying it..."
    Copy-Item ..\lib\SDL2.dll .\SDL2.dll
}

Write-Host "Trying to recompile game.cpp"

Remove-Item .\game_* -ErrorAction SilentlyContinue 
Remove-Item .\game.dll -ErrorAction SilentlyContinue 
g++ -fno-gnu-unique -fPIC $include $libs $flags -shared -o "game_$timestamp.dll" ..\src\build_game.cpp
Write-Host "Renaming game_$timestamp.dll to game.dll"
Rename-Item -Path ".\game_$timestamp.dll" -NewName ".\game.dll"

if (-not (Get-Process -Name "game" -ErrorAction SilentlyContinue)) {
    Write-Host "Game is not running"
    Write-Host "Compiling main.cpp..."
    Remove-Item .\game.exe -ErrorAction SilentlyContinue 
    g++ -fno-gnu-unique $include $libs $flags -o game.exe ../src/build_engine.cpp
}
else {
    Write-Host "Game is running, skipping main.cpp..."
}

Write-Host "Compilation completed."

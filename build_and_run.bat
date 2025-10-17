@echo off
REM Build and run the game from WSL
REM This works around the UNC path limitation in CMD.EXE

echo ================================
echo   Building Dungeon Quest RPG
echo ================================
echo.

wsl -e bash -c "cd /home/lorinan/projects/Dungeon-Quest-RPG && make clean && make"

if errorlevel 1 (
    echo.
    echo Build FAILED!
    pause
    exit /b 1
)

echo.
echo Build successful! Starting game...
echo.

wsl -e bash -c "cd /home/lorinan/projects/Dungeon-Quest-RPG && ./rpg_game"

pause

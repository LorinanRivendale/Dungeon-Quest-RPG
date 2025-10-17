@echo off
REM Wrapper to run the game from WSL
REM This works around the UNC path limitation in CMD.EXE

echo Starting Dungeon Quest RPG...
echo.

wsl -e bash -c "cd /home/lorinan/projects/Dungeon-Quest-RPG && ./rpg_game"

pause

@echo off
echo ================================
echo   Building Dungeon Quest RPG
echo ================================
echo.

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Compile each source file
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -O2 -c src/main.c -o obj/main.o
gcc -Wall -Wextra -std=c99 -O2 -c src/game_state.c -o obj/game_state.o
gcc -Wall -Wextra -std=c99 -O2 -c src/party.c -o obj/party.o
gcc -Wall -Wextra -std=c99 -O2 -c src/dungeon.c -o obj/dungeon.o
gcc -Wall -Wextra -std=c99 -O2 -c src/battle.c -o obj/battle.o
gcc -Wall -Wextra -std=c99 -O2 -c src/inventory.c -o obj/inventory.o
gcc -Wall -Wextra -std=c99 -O2 -c src/utils.c -o obj/utils.o

if errorlevel 1 (
    echo.
    echo Build FAILED!
    pause
    exit /b 1
)

REM Link all object files
echo Linking...
gcc obj/main.o obj/game_state.o obj/party.o obj/dungeon.o obj/battle.o obj/inventory.o obj/utils.o -o rpg_game.exe

if errorlevel 1 (
    echo.
    echo Linking FAILED!
    pause
    exit /b 1
)

echo.
echo ================================
echo   Build Complete!
echo ================================
echo.
echo Run the game with: rpg_game.exe
echo.
pause
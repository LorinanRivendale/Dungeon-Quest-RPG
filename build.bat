@echo off
echo ================================
echo   Building Dungeon Quest RPG
echo ================================
echo.

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Compile each source file
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -O2 -c SRC/main.c -o obj/main.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/game_state.c -o obj/game_state.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/party.c -o obj/party.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/dungeon.c -o obj/dungeon.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/dungeon_maps.c -o obj/dungeon_maps.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/battle.c -o obj/battle.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/inventory.c -o obj/inventory.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/save_system.c -o obj/save_system.o
gcc -Wall -Wextra -std=c99 -O2 -c SRC/utils.c -o obj/utils.o

if errorlevel 1 (
    echo.
    echo Build FAILED!
    pause
    exit /b 1
)

REM Link all object files
echo Linking...
gcc obj/main.o obj/game_state.o obj/party.o obj/dungeon.o obj/dungeon_maps.o obj/battle.o obj/inventory.o obj/save_system.o obj/utils.o -o rpg_game.exe

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
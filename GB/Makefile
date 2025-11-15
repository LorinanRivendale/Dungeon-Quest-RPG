# Dungeon Quest RPG - GameBoy Build
# Requires GBDK-2020 to be installed

# Project name
PROJECT = dungeon_quest

# GBDK path (modify if installed elsewhere)
GBDK = /opt/gbdk
LCC = $(GBDK)/bin/lcc

# Compiler flags
CFLAGS = -Wa-l -Wl-m -Wl-j

# Source files
SRCS = main.c

# Output ROM
ROM = $(PROJECT).gb

# Build targets
all: $(ROM)

$(ROM): $(SRCS)
	$(LCC) $(CFLAGS) -o $(ROM) $(SRCS)
	@echo "Build complete: $(ROM)"
	@echo "Test with: bgb $(ROM)"
	@echo "Or any GameBoy emulator"

clean:
	rm -f *.o *.lst *.map *.sym *.gb *.gbc
	@echo "Clean complete"

run: $(ROM)
	@echo "Attempting to run with bgb emulator..."
	bgb $(ROM) || echo "BGB not found. Please run manually with your emulator."

.PHONY: all clean run

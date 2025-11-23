# Makefile for Dungeon Quest RPG

CC = gcc
MINGW_CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS =
TARGET = rpg_game
WIN_TARGET = rpg_game.exe

# Directories
SRCDIR = SRC
OBJDIR = obj

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJDIR)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Clean complete"

# Run
run: all
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Windows build (cross-compile with MinGW)
windows:
	@mkdir -p $(OBJDIR)
	$(MINGW_CC) $(CFLAGS) $(SOURCES) -o $(WIN_TARGET) $(LDFLAGS)
	@echo "Windows build complete: $(WIN_TARGET)"

# Help
help:
	@echo "Makefile for Dungeon Quest RPG"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build the game (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run the game"
	@echo "  debug   - Build with debug symbols"
	@echo "  windows - Cross-compile for Windows (requires MinGW)"
	@echo "  help    - Show this help message"

.PHONY: all clean run debug windows help directories
# Dungeon Quest RPG - GameBoy Minimal Demo

This is a minimal GameBoy port demonstrating core dungeon exploration mechanics on actual GB hardware.

## Features

- 16x16 tile dungeon with walls and floors
- Player character with D-pad movement
- Collision detection
- Runs on real GameBoy hardware or emulators

## Requirements

### GBDK-2020 (GameBoy Development Kit)

**Linux/WSL:**
```bash
# Download GBDK-2020
wget https://github.com/gbdk-2020/gbdk-2020/releases/download/4.2.0/gbdk-linux64.tar.gz
tar -xzf gbdk-linux64.tar.gz
sudo mv gbdk /opt/gbdk
```

**macOS:**
```bash
wget https://github.com/gbdk-2020/gbdk-2020/releases/download/4.2.0/gbdk-macos.tar.gz
tar -xzf gbdk-macos.tar.gz
sudo mv gbdk /opt/gbdk
```

**Windows:**
Download from: https://github.com/gbdk-2020/gbdk-2020/releases
Extract to `C:\gbdk` and update Makefile path

## Building

```bash
cd GB
make
```

This will create `dungeon_quest.gb` ROM file.

## Running

### Option 1: BGB Emulator (Recommended for testing)
```bash
make run
```

### Option 2: Other Emulators
- **mGBA**: `mgba dungeon_quest.gb`
- **SameBoy**: `sameboy dungeon_quest.gb`
- **Emulicious**: Open in GUI
- **Real Hardware**: Flash to GB flashcart

### Option 3: Web Emulator
Upload `dungeon_quest.gb` to: https://www.emulatoronline.com/gameboy-games/

## Controls

- **D-Pad**: Move player (Up/Down/Left/Right)
- **Start**: (Not yet implemented)
- **Select**: (Not yet implemented)
- **A/B**: (Not yet implemented)

## Current Limitations

This is a MINIMAL demo to validate the design:
- No combat system
- No menus
- No save/load
- Simple fixed dungeon layout
- No procedural generation (yet)

## Next Steps

Once this demo works on hardware:
1. Add procedural dungeon generation
2. Implement basic combat
3. Add menu system
4. Implement save to SRAM
5. Full game port

## Tile Graphics

Currently using placeholder 8x8 tiles:
- Tile 0: Empty (black)
- Tile 1: Floor (dotted pattern)
- Tile 2: Wall (solid)
- Tile 3: Player (simple character)

**TODO**: Replace with actual designed tiles

## Memory Usage

Current ROM size: ~4-8KB (fits in 32KB ROM, no MBC needed)

## License

Part of Dungeon Quest RPG project

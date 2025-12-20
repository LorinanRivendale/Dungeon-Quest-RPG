#!/usr/bin/env python3
"""
World Map Visualizer with Coordinate Grid
Converts the C array world map data into a visual HTML representation
"""

import re

# Color mapping for different tile types
TILE_COLORS = {
    'grass': '#90EE90',
    'forest': '#228B22',
    'forest_dark': '#006400',
    'water': '#4169E1',
    'water_deep': '#000080',
    'mountain': '#8B7355',
    'mountain_peak': '#FFFFFF',
    'desert': '#F4A460',
    'town': '#FF6347',
    'castle': '#DC143C',
    'dungeon': '#8B008B',
    'sanctum': '#FFD700',
    'default': '#D3D3D3'
}

def get_tile_color(tile_value):
    """Return color based on tile ID"""
    tile = int(tile_value, 16) if isinstance(tile_value, str) else tile_value

    if 0x00 <= tile <= 0x0F:
        return TILE_COLORS['grass']
    elif 0x10 <= tile <= 0x12:
        return TILE_COLORS['forest']
    elif tile == 0x13:
        return TILE_COLORS['forest_dark']
    elif 0x20 <= tile <= 0x2F:
        return TILE_COLORS['water']
    elif 0x30 <= tile <= 0x31:
        return TILE_COLORS['mountain']
    elif tile == 0x32:
        return TILE_COLORS['mountain_peak']
    elif tile == 0x33:
        return TILE_COLORS['dungeon']
    elif 0x40 <= tile <= 0x4F:
        return TILE_COLORS['desert']
    elif 0x50 <= tile <= 0x6F:
        return TILE_COLORS['default']
    elif 0x70 <= tile <= 0x73:
        return TILE_COLORS['town']
    elif 0x74 <= tile <= 0x77:
        return TILE_COLORS['castle']
    elif 0x78 <= tile <= 0x7B:
        return TILE_COLORS['dungeon']
    elif 0x7C <= tile <= 0x7F:
        return TILE_COLORS['sanctum']
    else:
        return TILE_COLORS['default']

def parse_world_map_from_c_file(filename):
    """Extract world map tile data from world_map.c"""
    with open(filename, 'r') as f:
        content = f.read()

    # Find the world_map_tiles array
    pattern = r'static const uint8_t world_map_tiles\[WORLD_MAP_HEIGHT\]\[WORLD_MAP_WIDTH\] = \{(.*?)\};'
    match = re.search(pattern, content, re.DOTALL)

    if not match:
        raise ValueError("Could not find world_map_tiles array in file")

    array_content = match.group(1)

    # Parse rows
    rows = []
    row_pattern = r'\{([^}]+)\}'
    for row_match in re.finditer(row_pattern, array_content):
        row_data = row_match.group(1)
        tiles = [int(x.strip(), 16) for x in row_data.split(',')]
        rows.append(tiles)

    return rows

def generate_html_map(tile_data, output_file):
    """Generate an HTML visualization of the map with coordinate grid"""

    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0
    tile_size = 6  # Smaller tiles for 128x128 map

    html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Dungeon Quest RPG - World Map ({width}×{height})</title>
    <style>
        body {{
            font-family: 'Courier New', monospace;
            background-color: #1a1a1a;
            color: #ffffff;
            padding: 20px;
            margin: 0;
        }}
        h1 {{
            text-align: center;
            color: #FFD700;
            text-shadow: 2px 2px 4px #000000;
        }}
        .center {{
            text-align: center;
        }}
        .map-container {{
            display: inline-block;
            background-color: #000000;
            padding: 10px;
            border: 3px solid #FFD700;
            margin: 20px auto;
            box-shadow: 0 0 20px rgba(255, 215, 0, 0.5);
            position: relative;
        }}
        .map-wrapper {{
            position: relative;
            display: inline-block;
        }}
        .axis-top {{
            height: 20px;
            margin-bottom: 5px;
            position: relative;
        }}
        .axis-left {{
            width: 30px;
            display: inline-block;
            vertical-align: top;
        }}
        .map-with-grid {{
            display: inline-block;
            position: relative;
        }}
        .map-grid {{
            display: grid;
            grid-template-columns: repeat({width}, {tile_size}px);
            gap: 0px;
            background-color: #000000;
        }}
        .tile {{
            width: {tile_size}px;
            height: {tile_size}px;
            cursor: pointer;
        }}
        .tile:hover {{
            outline: 2px solid #FFFF00;
            box-shadow: 0 0 8px #FFFF00;
            z-index: 100;
        }}
        .grid-overlay {{
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            pointer-events: none;
        }}
        .grid-line {{
            stroke: rgba(255, 255, 255, 0.15);
            stroke-width: 1;
        }}
        .grid-line-major {{
            stroke: rgba(255, 215, 0, 0.3);
            stroke-width: 1;
        }}
        .axis-label {{
            fill: #888;
            font-size: 9px;
            font-family: 'Courier New', monospace;
        }}
        .coord-display {{
            position: fixed;
            top: 100px;
            right: 20px;
            background: rgba(0, 0, 0, 0.95);
            color: #FFD700;
            padding: 15px;
            border: 2px solid #FFD700;
            border-radius: 5px;
            font-size: 13px;
            min-width: 200px;
            box-shadow: 0 0 15px rgba(255, 215, 0, 0.5);
            z-index: 1000;
        }}
        .coord-display div {{
            margin: 5px 0;
            font-family: 'Courier New', monospace;
        }}
        .coord-label {{
            color: #888;
            font-size: 11px;
        }}
        .legend {{
            margin-top: 30px;
            padding: 20px;
            background-color: #2a2a2a;
            border: 2px solid #FFD700;
            border-radius: 5px;
            max-width: 900px;
            margin-left: auto;
            margin-right: auto;
        }}
        .legend h2 {{
            color: #FFD700;
            margin-top: 0;
        }}
        .legend-item {{
            display: inline-block;
            margin: 5px 15px;
        }}
        .legend-color {{
            display: inline-block;
            width: 20px;
            height: 20px;
            margin-right: 8px;
            vertical-align: middle;
            border: 1px solid #000;
        }}
        .info {{
            text-align: center;
            margin: 20px;
            color: #888;
        }}
    </style>
</head>
<body>
    <h1>🗺️ DUNGEON QUEST RPG - WORLD MAP 🗺️</h1>
    <div class="info">
        Map Size: {width} × {height} tiles | Total: {width * height} tiles | Hover over tiles to see coordinates
    </div>

    <div id="coordDisplay" class="coord-display">
        <div class="coord-label">Cursor Position:</div>
        <div>X: <span id="coordX">--</span></div>
        <div>Y: <span id="coordY">--</span></div>
        <div>Tile: <span id="coordTile">--</span></div>
    </div>

    <div class="center">
        <div class="map-container">
            <div class="map-wrapper">
                <div class="map-with-grid">
                    <div class="map-grid" id="mapGrid">
"""

    # Generate tiles
    for y, row in enumerate(tile_data):
        for x, tile in enumerate(row):
            color = get_tile_color(tile)
            html += f'                        <div class="tile" style="background-color: {color};" data-x="{x}" data-y="{y}" data-tile="0x{tile:02X}" onmouseover="updateCoords({x}, {y}, \'0x{tile:02X}\')"></div>\n'

    # Calculate SVG dimensions
    svg_width = width * tile_size
    svg_height = height * tile_size

    html += f"""                    </div>
                    <svg class="grid-overlay" width="{svg_width}" height="{svg_height}">
"""

    # Draw grid lines every 10 tiles
    for i in range(0, width + 1, 10):
        x_pos = i * tile_size
        line_class = "grid-line-major" if i % 20 == 0 else "grid-line"
        html += f'                        <line x1="{x_pos}" y1="0" x2="{x_pos}" y2="{svg_height}" class="{line_class}"/>\n'
        # Add X axis labels
        if i < width:
            html += f'                        <text x="{x_pos + 5}" y="12" class="axis-label">{i}</text>\n'

    for i in range(0, height + 1, 10):
        y_pos = i * tile_size
        line_class = "grid-line-major" if i % 20 == 0 else "grid-line"
        html += f'                        <line x1="0" y1="{y_pos}" x2="{svg_width}" y2="{y_pos}" class="{line_class}"/>\n'
        # Add Y axis labels
        if i < height:
            html += f'                        <text x="2" y="{y_pos + 12}" class="axis-label">{i}</text>\n'

    html += """                    </svg>
                </div>
            </div>
        </div>
    </div>

    <div class="legend">
        <h2>🗺️ Legend</h2>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['grass'] + """;"></span>
            <span>Grassland</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['forest'] + """;"></span>
            <span>Forest</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['desert'] + """;"></span>
            <span>Desert</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['water'] + """;"></span>
            <span>Water</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['mountain'] + """;"></span>
            <span>Mountains</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['town'] + """;"></span>
            <span>🏘️ Town</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['dungeon'] + """;"></span>
            <span>⚔️ Dungeon</span>
        </div>
        <div class="legend-item">
            <span class="legend-color" style="background-color: """ + TILE_COLORS['sanctum'] + """;"></span>
            <span>✨ Final Sanctum</span>
        </div>

        <h2>📍 Key Locations</h2>
        <div style="margin-top: 15px; font-size: 12px;">
            <strong>Towns:</strong><br>
            Greenleaf (95,100) • Coral Bay (85,107) • Emberforge (22,85) • Windspire (30,28)<br>
            Crossroads (72,60) • Sandfall (45,102) • Elderwood (85,42) • Scholar Outpost (95,25)
        </div>
        <div style="margin-top: 10px; font-size: 12px;">
            <strong>Dungeons:</strong><br>
            Cave of Earth (105,95) • Water Temple (75,106) • Volcano Keep (18,95) • Sky Tower (25,40)<br>
            Final Sanctum (59,63) • Smuggler's Cave (100,110) • Pyramid (55,108) • Forest Shrine (78,35) • Bonus (105,22)
        </div>
    </div>

    <div class="info" style="margin-top: 40px;">
        Generated from: SRC/world_map.c | Grid: Every 10 tiles (major lines every 20)
    </div>

    <script>
        function updateCoords(x, y, tile) {
            document.getElementById('coordX').textContent = x;
            document.getElementById('coordY').textContent = y;
            document.getElementById('coordTile').textContent = tile;
        }
    </script>
</body>
</html>
"""

    with open(output_file, 'w') as f:
        f.write(html)

    print(f"✅ Map visualization saved to: {output_file}")
    print(f"📊 Map size: {width} × {height} tiles")
    print(f"🎨 Total tiles: {width * height}")

if __name__ == "__main__":
    import sys

    c_file = "SRC/world_map.c"
    html_file = "world_map_preview.html"

    try:
        print("🗺️  Dungeon Quest RPG - World Map Visualizer")
        print("=" * 50)
        print(f"Reading map data from: {c_file}")

        tile_data = parse_world_map_from_c_file(c_file)

        print(f"Generating HTML visualization with coordinate grid...")
        generate_html_map(tile_data, html_file)

        print("\n✨ Done! Open 'world_map_preview.html' in your browser.")
        print("   Features:")
        print("   • Grid lines every 10 tiles (major every 20)")
        print("   • X/Y axis labels")
        print("   • Hover to see exact coordinates")
        print("   • All locations listed in legend")

    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

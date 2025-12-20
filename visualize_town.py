#!/usr/bin/env python3
"""
Town Map Visualizer
Creates an HTML preview of town maps by rendering actual tileset tiles
"""

import re
import sys

def parse_town_map_from_c_file(filename):
    """Extract town map tile data from C file"""
    with open(filename, 'r') as f:
        content = f.read()

    # Find the town tiles array (looking for pattern like greenleaf_tiles)
    pattern = r'static const uint8_t \w+_tiles\[TOWN_MAP_HEIGHT\]\[TOWN_MAP_WIDTH\] = \{(.*?)\};'
    match = re.search(pattern, content, re.DOTALL)

    if not match:
        raise ValueError("Could not find town tiles array in file")

    array_content = match.group(1)

    # Parse rows
    rows = []
    row_pattern = r'\{([^}]+)\}'
    for row_match in re.finditer(row_pattern, array_content):
        row_data = row_match.group(1)
        # Extract numeric values
        tiles = [int(x.strip()) for x in row_data.split(',')]
        rows.append(tiles)

    return rows

def generate_html_town_map(tile_data, tileset_path, output_file, town_name="Town"):
    """Generate HTML visualization using actual tileset image"""

    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0
    tile_size = 16  # Corneria metasprites are 16x16 pixels
    display_scale = 2  # Scale up for better visibility

    html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Dungeon Quest RPG - {town_name} ({width}×{height})</title>
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
            padding: 20px;
            border: 3px solid #8B4513;
            margin: 20px auto;
            box-shadow: 0 0 20px rgba(139, 69, 19, 0.5);
            position: relative;
        }}
        .map-canvas {{
            image-rendering: pixelated;
            image-rendering: crisp-edges;
            border: 2px solid #654321;
        }}
        .coord-display {{
            position: fixed;
            top: 100px;
            right: 20px;
            background: rgba(0, 0, 0, 0.95);
            color: #FFD700;
            padding: 15px;
            border: 2px solid #8B4513;
            border-radius: 5px;
            font-size: 13px;
            min-width: 200px;
            box-shadow: 0 0 15px rgba(139, 69, 19, 0.5);
            z-index: 1000;
        }}
        .coord-display div {{
            margin: 5px 0;
        }}
        .info {{
            text-align: center;
            margin: 20px;
            color: #888;
        }}
        .legend {{
            margin-top: 30px;
            padding: 20px;
            background-color: #2a2a2a;
            border: 2px solid #8B4513;
            border-radius: 5px;
            max-width: 800px;
            margin-left: auto;
            margin-right: auto;
        }}
        .legend h2 {{
            color: #FFD700;
            margin-top: 0;
        }}
    </style>
</head>
<body>
    <h1>🏘️ {town_name.upper()} - TOWN MAP 🏘️</h1>
    <div class="info">
        Map Size: {width} × {height} tiles | Click on map to see tile coordinates
    </div>

    <div id="coordDisplay" class="coord-display" style="display:none;">
        <div>Position:</div>
        <div>X: <span id="coordX">--</span></div>
        <div>Y: <span id="coordY">--</span></div>
        <div>Tile ID: <span id="coordTile">--</span></div>
    </div>

    <div class="center">
        <div class="map-container">
            <canvas id="mapCanvas" width="{width * tile_size}" height="{height * tile_size}"
                    style="width:{width * tile_size * display_scale}px; height:{height * tile_size * display_scale}px;"
                    class="map-canvas"></canvas>
        </div>
    </div>

    <div class="legend">
        <h2>🏘️ {town_name} Layout</h2>
        <div style="margin-top: 15px;">
            <strong>Key Buildings:</strong><br>
            • Inn (NW) - Rest and heal<br>
            • Item Shop (E) - Buy/sell consumables<br>
            • Equipment Shop (SE) - Buy/sell gear<br>
            • Houses - NPC residences<br>
            • Town Well (Center) - Gathering spot<br>
            • South Entrance - Exit to world map
        </div>
    </div>

    <div class="info" style="margin-top: 40px;">
        Generated from: SRC/greenleaf_map.c | Tileset: Corneria (Final Fantasy NES)
    </div>

    <script>
        const canvas = document.getElementById('mapCanvas');
        const ctx = canvas.getContext('2d');
        const coordDisplay = document.getElementById('coordDisplay');

        // Town map data
        const mapData = {json.dumps(tile_data)};
        const tileSize = {tile_size};
        const mapWidth = {width};
        const mapHeight = {height};

        // Load tileset image
        const tileset = new Image();
        tileset.src = '{tileset_path}';

        tileset.onload = function() {{
            console.log('Tileset loaded:', tileset.width, 'x', tileset.height);

            const tilesetWidthTiles = Math.floor(tileset.width / tileSize);
            console.log('Tileset columns:', tilesetWidthTiles);

            // Draw the map
            for (let y = 0; y < mapHeight; y++) {{
                for (let x = 0; x < mapWidth; x++) {{
                    const tileId = mapData[y][x];

                    // Convert tile ID to tileset coordinates
                    const tileX = tileId % tilesetWidthTiles;
                    const tileY = Math.floor(tileId / tilesetWidthTiles);

                    // Draw tile from tileset
                    ctx.drawImage(
                        tileset,
                        tileX * tileSize, tileY * tileSize, tileSize, tileSize,
                        x * tileSize, y * tileSize, tileSize, tileSize
                    );
                }}
            }}

            console.log('Map rendered successfully!');
        }};

        tileset.onerror = function() {{
            console.error('Failed to load tileset');
            ctx.fillStyle = '#ff0000';
            ctx.font = '12px Arial';
            ctx.fillText('Error: Could not load tileset', 10, 20);
        }};

        // Mouse tracking
        canvas.addEventListener('mousemove', function(e) {{
            const rect = canvas.getBoundingClientRect();
            const scaleX = canvas.width / rect.width;
            const scaleY = canvas.height / rect.height;

            const x = Math.floor((e.clientX - rect.left) * scaleX / tileSize);
            const y = Math.floor((e.clientY - rect.top) * scaleY / tileSize);

            if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {{
                document.getElementById('coordX').textContent = x;
                document.getElementById('coordY').textContent = y;
                document.getElementById('coordTile').textContent = mapData[y][x];
                coordDisplay.style.display = 'block';
            }} else {{
                coordDisplay.style.display = 'none';
            }}
        }});

        canvas.addEventListener('mouseleave', function() {{
            coordDisplay.style.display = 'none';
        }});
    </script>
</body>
</html>
"""

    with open(output_file, 'w') as f:
        f.write(html)

    print(f"✅ Town map visualization saved to: {output_file}")
    print(f"📊 Map size: {width} × {height} tiles")

if __name__ == "__main__":
    import json
    import sys

    # Allow command line arguments or use defaults
    if len(sys.argv) >= 4:
        c_file = sys.argv[1]
        html_file = sys.argv[2]
        town_name = sys.argv[3]
    else:
        c_file = "SRC/greenleaf_map.c"
        html_file = "greenleaf_preview.html"
        town_name = "Greenleaf"

    tileset_path = "Tilesets/Corneria Sprites.png"

    try:
        print("🏘️  Town Map Visualizer - Greenleaf")
        print("=" * 50)
        print(f"Reading map data from: {c_file}")

        tile_data = parse_town_map_from_c_file(c_file)

        print(f"Generating HTML visualization...")
        print(f"Using tileset: {tileset_path}")

        generate_html_town_map(tile_data, tileset_path, html_file, town_name)

        print("\n✨ Done! Open 'greenleaf_preview.html' in your browser.")
        print("   The map will render using the actual Minifantasy tileset.")

    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

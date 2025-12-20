#!/usr/bin/env python3
"""
Generate HTML visualizations for all towns
"""
import subprocess

towns = [
    ("SRC/greenleaf_map.c", "greenleaf_preview.html", "Greenleaf"),
    ("SRC/coral_bay_map.c", "coral_bay_preview.html", "Coral Bay"),
    ("SRC/emberforge_map.c", "emberforge_preview.html", "Emberforge"),
    ("SRC/windspire_map.c", "windspire_preview.html", "Windspire"),
]

print("🗺️  Generating town visualizations...\n")

for c_file, html_file, town_name in towns:
    print(f"📍 {town_name}...")

    # Run visualize_town.py with parameters
    result = subprocess.run([
        "python3", "visualize_town.py",
        c_file,
        html_file,
        town_name
    ], capture_output=True, text=True)

    if result.returncode == 0:
        print(f"   ✅ {html_file}")
    else:
        print(f"   ❌ Failed: {result.stderr}")

print("\n✨ All town visualizations complete!")
print("\nView them at:")
print("  http://localhost:8000/greenleaf_preview.html")
print("  http://localhost:8000/coral_bay_preview.html")
print("  http://localhost:8000/emberforge_preview.html")
print("  http://localhost:8000/windspire_preview.html")

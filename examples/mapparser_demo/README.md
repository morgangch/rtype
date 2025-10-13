# MapParser Demo Program

## 📖 Overview

Interactive test program for the MapParser library that allows you to load maps and visualize all their properties, tile definitions, and contents.

## 🚀 Building

The demo is automatically built with the project:

```bash
cd /path/to/rtype
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
```

The executable will be located at: `build/bin/mapparser_demo`

## 💻 Usage

### Interactive Mode

Run without arguments for an interactive menu:

```bash
cd /path/to/rtype
./build/bin/mapparser_demo
```

**Menu Options:**
1. Demo Map (finite, mixed terrain)
2. Welcome Map (finite, with default.def merging)
3. Infinite Demo Map (infinite, with random groups)
4. Custom Path (enter your own map directory)
5. Compare All Maps (load and display all available maps)
0. Exit

### Command-Line Mode

Load a specific map directly:

```bash
./build/bin/mapparser_demo assets/maps/demo
```

Show all tiles (not just first 20):

```bash
./build/bin/mapparser_demo assets/maps/welcome --all
```

## 📊 Output Features

The demo displays comprehensive information about loaded maps:

### 1. Map Metadata
- Description
- Dimensions (width x height)
- Type (Finite or Infinite)
- Raw width value (-1 for infinite)

### 2. Tile Definitions
- Complete list of character-to-asset mappings
- Shows both custom and default definitions
- Character → Asset path mapping

### 3. Loaded Tiles
- Formatted table with:
  - Index
  - X, Y coordinates
  - Character
  - Asset path
- Limits to first 20 tiles by default (use `--all` for complete list)

### 4. Tile Grid Visualization
- ASCII art representation of the map
- Shows actual tile layout
- Only for finite maps ≤ 50 tiles wide

### 5. Statistics
- Total tile count
- Number of tile definitions
- Map type
- Bounding box (min/max coordinates)
- Actual dimensions based on tiles
- Unique characters used

## 📝 Example Output

```
    ╔═══════════════════════════════════════════════════════════════╗
    ║           MapParser Library - Interactive Test Tool          ║
    ║                      R-Type Project                          ║
    ╚═══════════════════════════════════════════════════════════════╝

================================================================================
  Loading Map: assets/maps/demo
================================================================================

Attempting to load map from: assets/maps/demo
✓ Map loaded successfully!

[Map Metadata]
  Description: Demo map showing terrain, walls, and objects.
  Dimensions:  12 x 7
  Type:        Finite (fixed)
  Width Value: 12 (-1 means infinite)
  Height:      7

[Tile Definitions] (15 total)
  Character    Asset Path
  ----------------------------------------------------------------------
  '.'       -> ./assets/default/empty.png
  '#'       -> ./assets/default/block.png
  '*'       -> ./assets/default/star.png
  '~'       -> ./assets/demo/water.png
  '@'       -> ./assets/demo/player.png
  ...

[Loaded Tiles] (84 total)
  Index   X     Y     Char      Asset Path
  ----------------------------------------------------------------------
  0       0     0     '.'       ./assets/default/empty.png
  1       1     0     '.'       ./assets/default/empty.png
  ...

[Tile Grid Visualization]
  +-------------------------+
  | . . . . . . . . . . . . |
  | . . ~ ~ ~ . . . . . . . |
  | . # # # . . * * . . . . |
  | . # @ # . . * * . . . . |
  | . # # # . . . . . . . . |
  | . . . . . . . . . . . . |
  | . . . . . . . . . . . . |
  +-------------------------+

[Statistics]
  Total Tiles:        84
  Tile Definitions:   15
  Map Type:           Finite
  Bounding Box:       (0,0) to (11,6)
  Actual Dimensions:  12 x 7
  Unique Characters:  5
```

## 🎯 Use Cases

### Testing Map Files
Quickly validate that your map files are correctly formatted:
```bash
./build/bin/mapparser_demo assets/maps/my-new-map
```

### Debugging Map Issues
See exactly what tiles are loaded and how they're positioned.

### Verifying Default Merging
Load a map and check which definitions come from `default.def` vs. the map-specific `.def`.

### Exploring Random Groups
Load infinite maps multiple times to see different random tile selections.

### Map Comparison
Use option 5 to compare multiple maps side-by-side.

## 🔧 Customization

The source code is well-commented and easy to modify:

- **Change display limits**: Modify `max_tiles` parameter in `print_tiles()`
- **Add new output formats**: Create new print functions
- **Export data**: Add JSON/XML export functions
- **Custom visualizations**: Modify `print_tile_grid()` for different rendering

## 📁 File Structure

```
examples/mapparser_demo/
├── CMakeLists.txt    # Build configuration
├── main.cpp          # Interactive demo program
└── README.md         # This file
```

## 🎓 Educational Value

This demo serves as:
- **Tutorial**: Shows proper MapParser API usage
- **Reference**: Demonstrates all available features
- **Debugging Tool**: Helps diagnose map loading issues
- **Documentation**: Live examples of the library in action

## 🐛 Troubleshooting

### "Directory does not exist"
- Ensure you're running from the project root
- Use absolute paths or correct relative paths

### "No .def file found"
- Check that your map directory contains a `.def` file
- Verify file extension is exactly `.def`

### Grid not showing
- Grid is only shown for finite maps
- Maps wider than 50 tiles skip grid visualization
- Infinite maps don't show grid

## 🔗 Related Documentation

- [MapParser Library README](../../lib/mapparser/README.md)
- [Map Format Specification](../../docs/MAPS_STANDARD.md)
- [Unit Tests Documentation](../../tests/README_MAPPARSER_TESTS.md)

## 👥 Maintainers

Part of the R-Type project.

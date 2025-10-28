# 🗺️ MapParser Library 2.0 - Complete Rework

## What's New

The MapParser library has been completely reworked to support a more sophisticated map system with:

### ✨ Key Features

1. **Tile Type System**
   - `PlayerSpawn` - Spawn points (1-4 per map, validated)
   - `EnemyClassic` - Basic enemies
   - `EnemyElite` - Advanced enemies with complex behavior
   - `EnemyBoss` - Boss encounters with multi-phase AI
   - `Obstacle` - Hazards and barriers
   - `BackgroundElement` - Non-interactive decorations

2. **`.tile` File Format**
   - Separate definition files for each tile type
   - Key-value metadata (health, damage, speed, etc.)
   - Server-interpreted scripting language for AI behavior
   - Reusable across multiple maps

3. **Parallax Background System**
   - Multi-layer scrolling backgrounds
   - Configurable speed, depth, and tiling
   - Integrated into `.def` files

4. **Enhanced API**
   - Filter tiles by type: `getTilesByType(TileType)`
   - Get player spawns: `getPlayerSpawns()`
   - Access parallax layers: `getParallaxLayers()`

## File Structure Changes

### Old Format (`.def`)
```
width,height
Description
character asset_path
```

### New Format (`.def`)
```
width,height
Description

PARALLAX_BEGIN
texture_path speed depth repeat_x repeat_y
PARALLAX_END

character TileType tile_path
```

### New `.tile` Files
```
sprite: path/to/sprite.png
metadata_key: metadata_value

SCRIPT_BEGIN
MOVE LEFT 100
FIRE LEFT 2.5
ON_DEATH SPAWN_ITEM power_up 0.1
SCRIPT_END
```

## Scripting Language

Simple server-interpreted language for entity behavior:

### Movement
- `MOVE direction speed`
- `MOVE_PATTERN SINE amplitude:50 frequency:2`
- `MOVE_PATTERN ALTERNATE direction:VERTICAL distance:200`
- `ROTATE speed:30`

### Combat
- `FIRE direction interval`
- `FIRE_PATTERN BURST count:3 delay:0.2`
- `FIRE_PATTERN SPREAD angle:45 count:5`

### AI Logic
- `ON_HEALTH_BELOW threshold action`
- `ON_DEATH action`
- `PHASE name { ... }`
- `CHANGE_PATTERN phase_name`

### Collision
- `COLLISION_MASK entity_types`
- `NO_COLLISION`

## Example Usage

```cpp
// Load map
auto& parser = MapParser::getInstance();
parser.loadFromDirectory("assets/maps/my-map");

// Get player spawns (always 1-4, validated)
auto spawns = parser.getPlayerSpawns();

// Get enemies by type
auto basicEnemies = parser.getTilesByType(TileType::EnemyClassic);
auto eliteEnemies = parser.getTilesByType(TileType::EnemyElite);
auto bosses = parser.getTilesByType(TileType::EnemyBoss);

// Access tile data
for (const auto& enemy : basicEnemies) {
    std::string sprite = enemy.definition.sprite_path;
    std::string script = enemy.definition.script;
    
    // Access metadata
    int health = std::stoi(enemy.definition.metadata.at("health"));
    int damage = std::stoi(enemy.definition.metadata.at("damage"));
}

// Get parallax layers
const auto& layers = parser.getParallaxLayers();
for (const auto& layer : layers) {
    // Render background layer
    renderLayer(layer.texture_path, layer.scroll_speed, layer.depth);
}
```

## Directory Structure

```
assets/maps/
├── default.def                    # Default tile definitions
├── tiles/                         # Reusable tile definitions
│   ├── player_spawn.tile
│   ├── enemy_basic.tile
│   ├── enemy_elite.tile
│   ├── enemy_boss.tile
│   ├── obstacle_asteroid.tile
│   └── background_debris.tile
├── space-corridor/                # Example map
│   ├── space-corridor.def
│   └── space-corridor.map
└── my-level/
    ├── my-level.def
    └── my-level.map
```

## Example Maps

### Space Corridor (`assets/maps/space-corridor/`)
Complete example featuring:
- Infinite horizontal scrolling
- 4-layer parallax background
- Mixed enemy types (classic, elite)
- Obstacles and decorations
- Multiple player spawn points

### Files Included
- `space-corridor.def` - Map definition with parallax
- `space-corridor.map` - Layout with random groups
- `tiles/*.tile` - All tile type examples

## Server Integration

The scripting language is designed to be interpreted by the server:

```cpp
// Pseudo-code for server-side script interpretation
void executeScript(const std::string& script, Entity& entity) {
    auto lines = split(script, '\n');
    for (const auto& line : lines) {
        auto tokens = tokenize(line);
        
        if (tokens[0] == "MOVE") {
            entity.setVelocity(parseDirection(tokens[1]), parseFloat(tokens[2]));
        }
        else if (tokens[0] == "FIRE") {
            entity.setFireRate(parseFloat(tokens[2]));
            entity.setFireDirection(parseDirection(tokens[1]));
        }
        // ... handle other commands
    }
}
```

## Validation

The library automatically validates:
- ✅ Player spawn count (1-4 required)
- ✅ Tile type validity
- ✅ `.tile` file existence and format
- ✅ Map dimensions
- ✅ ASCII-only characters

## Migration Path

1. **Keep old maps working**: The library still supports basic formats for backward compatibility
2. **Gradual migration**: Convert maps one at a time
3. **Reuse tiles**: Create a library of `.tile` files to share across maps
4. **Test incrementally**: Start with simple scripts, add complexity gradually

## Documentation

- **Full Format Spec**: [`docs/TILE_FORMAT.md`](../docs/TILE_FORMAT.md)
- **Original Spec**: [`docs/MAPS_STANDARD.md`](../docs/MAPS_STANDARD.md)
- **Example Code**: [`examples/mapparser_v2_example.cpp`](../examples/mapparser_v2_example.cpp)

## API Reference

### Core Functions
- `loadFromDirectory(dirname)` - Load map from directory
- `getTiles()` - Get all loaded tiles
- `getTilesByType(type)` - Filter tiles by type
- `getPlayerSpawns()` - Get player spawn points (validated 1-4)
- `getParallaxLayers()` - Get background layers
- `getMapDefinition()` - Get full map metadata

### Data Structures
- `Tile` - Position, type, character, and full definition
- `TileDefinition` - Sprite path, script, and metadata map
- `TileMapping` - Maps character to type and `.tile` file
- `ParallaxLayer` - Background layer configuration
- `MapDefinition` - Complete map metadata

### Enums
- `TileType` - PlayerSpawn, EnemyClassic, EnemyElite, EnemyBoss, Obstacle, BackgroundElement

## Benefits

1. **Separation of Concerns**: Map layout separate from entity behavior
2. **Reusability**: Share `.tile` definitions across multiple maps
3. **Flexibility**: Easy to add new enemy types and behaviors
4. **Type Safety**: Compile-time type checking for tile categories
5. **Server Authority**: Scripts run server-side for authoritative gameplay
6. **Visual Enhancement**: Built-in parallax support for rich backgrounds
7. **Validation**: Automatic checks prevent common configuration errors

## Future Enhancements

Potential additions:
- Event triggers (e.g., "spawn boss when X enemies defeated")
- Dynamic difficulty scaling
- Map scripting (not just tile scripting)
- Visual script editor/debugger
- Animation definitions in `.tile` files
- Sound effect associations

---

**Questions?** See the full documentation or check the example map!

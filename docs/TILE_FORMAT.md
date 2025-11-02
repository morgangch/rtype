# 🎮 Map System 2.0 - Tile Definition Format

## Overview

The enhanced map system supports:
- **Tile Types**: PlayerSpawn, EnemyClassic, EnemyElite, EnemyBoss, Obstacle, BackgroundElement
- **AI Scripting**: Server-interpreted scripting language for enemy behavior
- **Parallax Backgrounds**: Multi-layer scrolling backgrounds
- **Metadata**: Flexible key-value data for each tile type

## File Structure

### `.def` File Format (Enhanced)

```
width,height           // or "inf,height" for infinite maps
Description text

PARALLAX_BEGIN
texture_path speed depth repeat_x repeat_y
...
PARALLAX_END

character TileType tile_path
...
```

**Example:**
```
inf,7
Space corridor with parallax backgrounds

PARALLAX_BEGIN
./assets/backgrounds/stars_far.png 0.2 0.1 true false
./assets/backgrounds/stars_mid.png 0.5 0.3 true false
./assets/backgrounds/stars_near.png 1.0 0.6 true false
PARALLAX_END

P PlayerSpawn ./assets/maps/tiles/player_spawn.tile
E EnemyClassic ./assets/maps/tiles/enemy_basic.tile
B EnemyBoss ./assets/maps/tiles/enemy_boss.tile
```

### `.tile` File Format (New)

```
/// Comments
key: value              // Metadata
...

SCRIPT_BEGIN
/// Server-interpreted script commands
SCRIPT_END
```

**Key Fields:**
- `sprite:` - Path to sprite/texture (required)
- Other keys become metadata accessible to the game logic

**Example:**
```
sprite: ./assets/sprites/BasicEnemy/basic_enemy.png
health: 1
damage: 1
speed: 100

SCRIPT_BEGIN
MOVE LEFT 100
FIRE LEFT 2.5
ON_DEATH SPAWN_ITEM power_up 0.1
SCRIPT_END
```

## Tile Types

### 1. PlayerSpawn
**Purpose**: Marks player spawn locations  
**Constraints**: 1-4 per map  
**Example**: See `player_spawn.tile`

### 2. EnemyClassic
**Purpose**: Basic enemies  
**Behavior**: Simple movement and firing patterns  
**Example**: See `enemy_basic.tile`

### 3. EnemyElite
**Purpose**: Tougher enemies with complex patterns  
**Behavior**: Advanced movement (sine waves, etc.), burst fire  
**Example**: See `enemy_elite.tile`

### 4. EnemyBoss
**Purpose**: Boss encounters  
**Behavior**: Multi-phase AI, pattern changes based on health  
**Example**: See `enemy_boss.tile`

### 5. Obstacle
**Purpose**: Hazards and barriers  
**Behavior**: Can be static or moving, configurable collision  
**Example**: See `obstacle_asteroid.tile`

### 6. BackgroundElement
**Purpose**: Visual decoration without gameplay impact  
**Behavior**: Parallax movement, no collision  
**Example**: See `background_debris.tile`

## Scripting Language Reference

### Movement Commands

```
MOVE direction speed
  - direction: LEFT, RIGHT, UP, DOWN, DIAGONAL_UP_LEFT, etc.
  - speed: pixels per second

MOVE_PATTERN type parameters
  - SINE amplitude:value frequency:value
  - ALTERNATE direction:VERTICAL|HORIZONTAL distance:value duration:value
  - CIRCLE radius:value speed:value
  
ROTATE speed:degrees_per_second
```

### Combat Commands

```
FIRE direction interval
  - interval: seconds between shots

FIRE_PATTERN type parameters
  - BURST count:value delay:value
  - SPREAD angle:value count:value
  - SPIRAL count:value rotation_speed:value
```

### Conditional Commands

```
ON_HEALTH_BELOW threshold action
ON_DEATH action
ON_PLAYER_SPAWN action
```

### Phase System (Bosses)

```
PHASE name {
    command1
    command2
    ...
}

CHANGE_PATTERN phase_name
```

### Collision

```
COLLISION_MASK entity_types...
NO_COLLISION
```

### Visual Effects

```
FADE_IN duration:seconds
DEBUG_MARKER color:COLOR size:pixels
```

## Parallax System

**Format**: `texture_path speed depth repeat_x repeat_y`

**Parameters:**
- `texture_path`: Path to background texture
- `speed`: Scroll speed multiplier (0.0-2.0, where 1.0 = normal)
- `depth`: Parallax depth (0.0 = far, 1.0 = near)
- `repeat_x`: Horizontal tiling (true/false)
- `repeat_y`: Vertical tiling (true/false)

**Layers are drawn back-to-front** (first layer = furthest back)

**Example:**
```
PARALLAX_BEGIN
./assets/backgrounds/stars_far.png 0.2 0.1 true false
./assets/backgrounds/nebula.png 0.5 0.4 true false
./assets/backgrounds/station.png 1.5 0.9 true true
PARALLAX_END
```

## API Usage

### Loading Maps

```cpp
auto& parser = MapParser::getInstance();
parser.loadFromDirectory("assets/maps/my-map");
```

### Accessing Tiles

```cpp
// Get all tiles
const auto& tiles = parser.getTiles();

// Filter by type
auto enemies = parser.getTilesByType(TileType::EnemyClassic);
auto bosses = parser.getTilesByType(TileType::EnemyBoss);

// Get player spawns (always returns 1-4)
auto spawns = parser.getPlayerSpawns();
```

### Accessing Tile Data

```cpp
for (const auto& tile : tiles) {
    std::cout << "Type: " << tileTypeToString(tile.type) << "\n";
    std::cout << "Sprite: " << tile.definition.sprite_path << "\n";
    std::cout << "Script:\n" << tile.definition.script << "\n";
    
    // Access metadata
    if (tile.definition.metadata.count("health")) {
        int health = std::stoi(tile.definition.metadata.at("health"));
    }
}
```

### Parallax Layers

```cpp
const auto& layers = parser.getParallaxLayers();
for (const auto& layer : layers) {
    // Load and render layer
    loadTexture(layer.texture_path);
    setScrollSpeed(layer.scroll_speed);
    setDepth(layer.depth);
    setRepeat(layer.repeat_x, layer.repeat_y);
}
```

## Migration Guide

### Old Format
```
12,7
Demo map
~ ./assets/demo/water.png
@ ./assets/demo/player.png
```

### New Format
```
12,7
Demo map with tile types

PARALLAX_BEGIN
./assets/backgrounds/sky.png 0.5 0.2 true false
PARALLAX_END

@ PlayerSpawn ./assets/maps/tiles/player_spawn.tile
~ BackgroundElement ./assets/maps/tiles/water.tile
```

## Validation Rules

1. **Player Spawns**: Must have 1-4 `PlayerSpawn` tiles
2. **Tile Definitions**: Each character must map to a valid `.tile` file
3. **Scripts**: Must be enclosed in `SCRIPT_BEGIN`/`SCRIPT_END`
4. **Parallax**: Layers must have all 5 parameters
5. **ASCII Only**: No special characters in any file

## Best Practices

1. **Tile Organization**: Keep `.tile` files in `assets/maps/tiles/`
2. **Naming**: Use descriptive names (`enemy_basic.tile`, not `e1.tile`)
3. **Scripts**: Comment your AI behavior for clarity
4. **Testing**: Start with simple scripts, test incrementally
5. **Reusability**: Share common `.tile` files across maps
6. **Parallax**: Use 3-5 layers for best visual effect
7. **Performance**: Keep scripts simple for many enemies

## Example Complete Map

See `assets/maps/space-corridor/` for a full example with:
- Infinite scrolling
- 4 parallax layers
- Multiple enemy types
- Obstacles and decorations
- Player spawn points

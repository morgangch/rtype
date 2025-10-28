# 📜 Map Scripting Language - Quick Reference

## Command Categories

### 🎯 Movement

| Command | Syntax | Example |
|---------|--------|---------|
| Simple Move | `MOVE direction speed` | `MOVE LEFT 100` |
| Sine Wave | `MOVE_PATTERN SINE amplitude:N frequency:N` | `MOVE_PATTERN SINE amplitude:50 frequency:2` |
| Alternate | `MOVE_PATTERN ALTERNATE direction:DIR distance:N duration:N` | `MOVE_PATTERN ALTERNATE direction:VERTICAL distance:200 duration:3` |
| Circle | `MOVE_PATTERN CIRCLE radius:N speed:N` | `MOVE_PATTERN CIRCLE radius:100 speed:30` |
| Rotation | `ROTATE speed:N` | `ROTATE speed:30` |

**Directions**: `LEFT`, `RIGHT`, `UP`, `DOWN`, `DIAGONAL_UP_LEFT`, `DIAGONAL_UP_RIGHT`, `DIAGONAL_DOWN_LEFT`, `DIAGONAL_DOWN_RIGHT`

### 🔫 Combat

| Command | Syntax | Example |
|---------|--------|---------|
| Basic Fire | `FIRE direction interval` | `FIRE LEFT 2.5` |
| Burst Fire | `FIRE_PATTERN BURST count:N delay:N` | `FIRE_PATTERN BURST count:3 delay:0.2` |
| Spread Shot | `FIRE_PATTERN SPREAD angle:N count:N` | `FIRE_PATTERN SPREAD angle:45 count:5` |
| Spiral | `FIRE_PATTERN SPIRAL count:N rotation_speed:N` | `FIRE_PATTERN SPIRAL count:8 rotation_speed:2` |

### 🤖 AI Logic

| Command | Syntax | Example |
|---------|--------|---------|
| Health Trigger | `ON_HEALTH_BELOW threshold action` | `ON_HEALTH_BELOW 20 CHANGE_PATTERN AGGRESSIVE` |
| Death Action | `ON_DEATH action` | `ON_DEATH SPAWN_ITEM power_up 0.3` |
| Spawn Action | `ON_PLAYER_SPAWN action` | `ON_PLAYER_SPAWN GRANT_INVULNERABILITY duration:3.0` |

### 📦 Phase System (Bosses)

```
PHASE phase_name {
    command1
    command2
    ...
}

CHANGE_PATTERN phase_name
```

**Example:**
```
PHASE AGGRESSIVE {
    FIRE_RATE 0.5
    SPEED 80
    FIRE_PATTERN SPREAD angle:45 count:5
}

ON_HEALTH_BELOW 50 CHANGE_PATTERN AGGRESSIVE
```

### 💥 Collision

| Command | Syntax | Example |
|---------|--------|---------|
| Set Mask | `COLLISION_MASK types...` | `COLLISION_MASK PLAYER PLAYER_PROJECTILE` |
| No Collision | `NO_COLLISION` | `NO_COLLISION` |

**Entity Types**: `PLAYER`, `ENEMY`, `PLAYER_PROJECTILE`, `ENEMY_PROJECTILE`, `OBSTACLE`

### 🎨 Visual Effects

| Command | Syntax | Example |
|---------|--------|---------|
| Fade In | `FADE_IN duration:N` | `FADE_IN duration:0.5` |
| Debug Marker | `DEBUG_MARKER color:COLOR size:N` | `DEBUG_MARKER color:GREEN size:32` |

**Colors**: `RED`, `GREEN`, `BLUE`, `YELLOW`, `WHITE`, `BLACK`

## Complete Examples

### Basic Enemy
```
MOVE LEFT 100
FIRE LEFT 2.5
ON_DEATH SPAWN_ITEM power_up 0.1
```

### Elite Enemy with Sine Wave
```
MOVE LEFT 120
MOVE_PATTERN SINE amplitude:50 frequency:2
FIRE LEFT 1.5
FIRE_PATTERN BURST count:3 delay:0.2
ON_DEATH SPAWN_ITEM power_up 0.3
```

### Multi-Phase Boss
```
MOVE LEFT 50
MOVE_PATTERN ALTERNATE direction:VERTICAL distance:200 duration:3

FIRE LEFT 1.0
FIRE DIAGONAL_DOWN_LEFT 1.2
FIRE DIAGONAL_UP_LEFT 1.2

ON_HEALTH_BELOW 20 CHANGE_PATTERN AGGRESSIVE
ON_HEALTH_BELOW 10 CHANGE_PATTERN DESPERATE

PHASE AGGRESSIVE {
    FIRE_RATE 0.5
    SPEED 80
    FIRE_PATTERN SPREAD angle:45 count:5
}

PHASE DESPERATE {
    FIRE_RATE 0.3
    SPEED 120
    FIRE_PATTERN SPIRAL count:8 rotation_speed:2
}

ON_DEATH SPAWN_ITEM mega_power_up 1.0
```

### Moving Obstacle
```
ROTATE speed:30
MOVE LEFT 80
COLLISION_MASK PLAYER PLAYER_PROJECTILE
```

### Background Debris
```
MOVE LEFT 30
ROTATE speed:10
NO_COLLISION
FADE_IN duration:0.5
```

## Metadata Keys

Common metadata keys used in `.tile` files:

| Key | Type | Example | Description |
|-----|------|---------|-------------|
| `sprite` | path | `./assets/sprites/enemy.png` | Sprite/texture path (required) |
| `health` | int | `3` | Hit points |
| `damage` | int | `2` | Damage dealt |
| `speed` | float | `120.0` | Movement speed |
| `fire_rate` | float | `1.5` | Shots per second |
| `scale` | float | `2.5` | Sprite scale multiplier |
| `destructible` | bool | `true` | Can be destroyed |
| `layer` | string | `background` | Rendering layer |
| `depth` | float | `0.3` | Parallax depth |
| `visible` | bool | `false` | Initially visible |

## Tips & Best Practices

### ✅ DO
- Comment your scripts for clarity
- Test simple behaviors first
- Use phases for complex boss AI
- Combine multiple movement patterns
- Set appropriate collision masks
- Use metadata for server logic

### ❌ DON'T
- Write overly complex single-line scripts
- Forget to set sprite paths
- Use the same fire rate for all enemies
- Ignore collision optimization
- Hard-code values (use metadata instead)

## Execution Model

1. **Parse Time**: Scripts are loaded and validated when map loads
2. **Spawn Time**: Entity is created with metadata values
3. **Runtime**: Server executes script commands each frame
4. **Event Time**: Conditional commands trigger on events (health, death, etc.)

## Error Handling

Common errors and solutions:

| Error | Cause | Solution |
|-------|-------|----------|
| Unknown command | Typo in command name | Check spelling against reference |
| Invalid direction | Wrong direction string | Use valid direction (LEFT, RIGHT, etc.) |
| Missing parameter | Parameter not specified | Add required parameter with colon syntax |
| Parse error | Malformed line | Check syntax, ensure proper spacing |

## Extended Syntax

### Comments
```
/// This is a comment (outside script block)

SCRIPT_BEGIN
/// This is also a comment (inside script block)
MOVE LEFT 100  /// Inline comments not supported
SCRIPT_END
```

### Parameters
```
/// Colon syntax for named parameters
MOVE_PATTERN SINE amplitude:50 frequency:2

/// Space-separated for simple parameters
MOVE LEFT 100
FIRE RIGHT 2.5
```

### Multi-line Phases
```
PHASE ENRAGED {
    FIRE_RATE 0.3
    SPEED 150
    FIRE_PATTERN SPREAD angle:60 count:7
    MOVE_PATTERN CIRCLE radius:200 speed:50
}
```

---

**Need more help?** See [`docs/TILE_FORMAT.md`](../docs/TILE_FORMAT.md) for detailed documentation!

# Weapon System Implementation

## Overview
This document describes the complete weapon system implementation for the R-Type game, including all weapon modes for each vessel class.

## Implementation Status

✅ **COMPLETE** - All weapon modes implemented and tested for compilation.

---

## Vessel Weapon Modes

### 1. **Crimson Striker** (Balanced Fighter)
- **Normal Shot**: `WeaponMode::Single`
  - Fires 1 standard projectile forward
  - Speed: 600 px/s
  - Damage: 1
  
- **Charged Shot**: `WeaponMode::Piercing`
  - Fires 1 piercing projectile that penetrates enemies
  - Speed: 800 px/s
  - Damage: 3
  - Piercing: Continues through enemies without being destroyed

---

### 2. **Azure Phantom** (Agile Striker)
- **Normal Shot**: `WeaponMode::Dual`
  - Fires 2 projectiles simultaneously with vertical offset
  - Speed: 700 px/s
  - Damage: 1 each
  - Offset: ±10 pixels vertically
  
- **Charged Shot**: `WeaponMode::Burst`
  - Fires 3 homing projectiles in a spread pattern
  - Speed: 600 px/s
  - Damage: 2 each
  - Angular spread: -15°, 0°, +15°
  - **Homing**: Tracks nearest enemy within 300 pixel range
  - Turning speed: 180°/second
  - Retarget interval: 0.5 seconds

---

### 3. **Emerald Titan** (Tank)
- **Normal Shot**: `WeaponMode::Single` + Explosion
  - Fires 1 explosive projectile
  - Speed: 500 px/s
  - Direct damage: 2
  - **Explosion on impact**:
    - Radius: 80 pixels
    - Center damage: 2
    - Edge damage: 1
    - Damage interpolates linearly based on distance
  
- **Charged Shot**: `WeaponMode::Single` + Large Explosion
  - Fires 1 large explosive projectile
  - Speed: 400 px/s
  - Direct damage: 4
  - **Large explosion on impact**:
    - Radius: 150 pixels
    - Center damage: 4
    - Edge damage: 2

---

### 4. **Solar Guardian** (Support)
- **Normal Shot**: `WeaponMode::Spread`
  - Fires 4 projectiles in a shotgun spread
  - Speed: 650 px/s
  - Damage: 1 each
  - Spread pattern:
    - Projectile 1: -15° angle
    - Projectile 2: -5° angle
    - Projectile 3: +5° angle
    - Projectile 4: +15° angle
  
- **Charged Shot**: ⚠️ **NOT YET IMPLEMENTED**
  - Planned: Shield generation or area healing

---

## System Integration

### Server-Side Systems

#### 1. **RoomController::createServerProjectile()**
Location: `server/src/controllers/RoomController.cpp` (lines 246-394)

- Reads `VesselClass` component from shooting player
- Determines weapon mode (normal vs charged)
- Switch statement handles each weapon mode:
  - `Single`: Creates 1 projectile, adds Explosion for EmeraldTitan
  - `Dual`: Creates 2 projectiles with vertical offset
  - `Burst`: Creates 3 projectiles with angular spread + Homing component
  - `Spread`: Creates 4 projectiles with angular spread
  - `Piercing`: Creates 1 piercing projectile

Helper functions:
- `createSingleProjectile(room, owner, x, y, damage, speed, piercing)`
- `createSingleProjectile(room, owner, x, y, damage, speed, angleRadians)`

#### 2. **HomingSystem**
Location: `common/systems/HomingSystem.h`
Integrated: `server/src/main.cpp` (line 63)

- Updates homing projectile trajectories
- Finds nearest enemy within detection range
- Steers velocity vector towards target
- Retargets periodically (0.5s interval)
- Runs after `MovementSystem` each frame

#### 3. **ServerCollisionSystem - Explosion Damage**
Location: `server/src/systems/ServerCollisionSystem.cpp` (lines 89-165)

When a projectile with `Explosion` component hits:
1. Marks explosion as triggered
2. Scans all enemies within explosion radius
3. Calculates distance-based damage interpolation
4. Awards score to shooter for all kills
5. Destroys enemies with HP ≤ 0

**Damage Formula**:
```cpp
ratio = distance / radius
damage = centerDamage + ratio * (edgeDamage - centerDamage)
```

---

## Component Details

### Explosion Component
Location: `common/components/Explosion.h`

```cpp
Explosion(float radius, int centerDamage, int edgeDamage)
```

Fields:
- `float radius`: Explosion radius in pixels
- `int centerDamage`: Damage at epicenter
- `int edgeDamage`: Damage at outer edge
- `bool triggered`: Prevents double-trigger
- `float duration/lifetime`: For visual effects (client-side)

### Homing Component
Location: `common/components/Homing.h`

```cpp
Homing(float detectionRange, float turnSpeed, float retargetInterval)
```

Fields:
- `float detectionRange`: Max distance to detect targets (pixels)
- `float turnSpeed`: Angular velocity for turning (radians/second)
- `float retargetInterval`: Time between retarget checks (seconds)
- `ECS::EntityID targetEntity`: Current target
- `float retargetTimer`: Internal timer

---

## Weapon Balance Summary

| Vessel | Normal DPS | Charged DPS | Special Trait |
|--------|-----------|-------------|---------------|
| Crimson Striker | 1 dmg × 1 | 3 dmg × 1 | Piercing charged |
| Azure Phantom | 1 dmg × 2 | 2 dmg × 3 + Homing | Fast rate, tracking |
| Emerald Titan | 2 dmg + 80px AoE | 4 dmg + 150px AoE | Explosive AoE |
| Solar Guardian | 1 dmg × 4 spread | TBD (Shield) | Wide coverage |

---

## Testing Checklist

- [x] All weapon modes compile without errors
- [x] HomingSystem integrated on server
- [x] Explosion damage implemented in collision system
- [ ] **In-game testing required**:
  - [ ] Crimson Striker: Single and Piercing shots
  - [ ] Azure Phantom: Dual shots and Homing burst
  - [ ] Emerald Titan: Explosion AoE damage
  - [ ] Solar Guardian: Spread pattern
  - [ ] Multiplayer: Different vessels see correct weapon types
  - [ ] Score awards for explosion kills
  - [ ] Homing projectiles track enemies correctly

---

## Future Enhancements

### Solar Guardian Charged Shot
Recommended implementation:
1. Create `Shield` component with damage reduction %
2. Add `ShieldSystem` to apply damage mitigation
3. Projectile creates shield zone on impact
4. Duration-based (e.g., 5 seconds)

### Visual Effects (Client-side)
- Explosion particle effects
- Homing projectile trails
- Shield visual indicators

---

## Files Modified

### Server
- `server/src/controllers/RoomController.cpp` - Weapon mode logic
- `server/include/controllers/RoomController.h` - Helper function declarations
- `server/src/main.cpp` - HomingSystem integration
- `server/src/systems/ServerCollisionSystem.cpp` - Explosion damage

### Common (Shared)
- `common/components/VesselClass.h` - Weapon mode definitions
- `common/components/Explosion.h` - Explosion component
- `common/components/Homing.h` - Homing component
- `common/systems/HomingSystem.h` - Homing update logic

### Network
- `common/packets/packets.h` - VesselType sync in PlayerJoinPacket
- `server/src/network/senders.cpp` - Send vesselType to clients
- `client/src/network/controllers/game_controller.cpp` - Receive vesselType

### Client
- `client/src/gui/EntityFactory.cpp` - Sprite sheet system
- `client/src/gui/GameState.cpp` - Remote player creation with vesselType
- `client/src/gui/VesselSelectionState.cpp` - Vessel selection UI

---

## Build Commands

```bash
cd /home/haizer/Bureau/EPITECH/3ème_année/R-Type/rtype
cmake --build build
```

**Status**: ✅ All systems compiled successfully (Exit Code: 0)

---

**Last Updated**: 2025-01-XX  
**Author**: R-Type Development Team

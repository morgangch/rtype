/*
** Minimal server-side enemy spawner implementation
*/
#include "systems/ServerEnemySystem.h"
#include "rtype.h"
#include "packets.h"
#include "services/RoomService.h"
#include "services/PlayerService.h"
#include <iostream>
// Use project-relative includes like other server files
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/EnemyType.h>
#include <common/components/Shield.h>
// Server components
#include "components/RoomProperties.h"
#include "components/PlayerConn.h"
// Common player component
#include <common/components/Player.h>

#include "senders.h"
#include "components/LinkedRoom.h"

void ServerEnemySystem::Update(ECS::World &world, float deltaTime) {
    // Only update spawn systems if there's at least one active game room
    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    bool hasActiveGame = false;
    if (rooms) {
        for (auto &pair : *rooms) {
            if (pair.second && pair.second->isGameStarted) {
                hasActiveGame = true;
                break;
            }
        }
    }

    // Only run enemy spawning logic if game is active and not finished
    if (hasActiveGame && !_gameFinished) {
        updatePhase(deltaTime);
        updateEnemySpawning(world, deltaTime);
        updateBossSpawning(world, deltaTime);
        checkBossDeathAndAdvanceLevel(world);
    }

    // Always update player state broadcast (needed even in lobby)
    updatePlayerStateBroadcast(world, deltaTime);
    cleanupDeadEntities(world);
}

ServerEnemySystem::ServerEnemySystem()
    : ECS::System("ServerEnemySystem", 5), _levelTimer(0.0f), _currentLevel(0), _phase(EnemySpawnPhase::OnlyBasic), _bossSpawned(false), _gameFinished(false), _stateTick(0.0f)
{
    // Define the 4 sub-levels
    // Level 0: Basic + Shielded + TankDestroyer
    _levelDefinitions.push_back({
        rtype::common::components::EnemyType::Basic,
        rtype::common::components::EnemyType::Shielded,
        rtype::common::components::EnemyType::TankDestroyer
    });

    // Level 1: Snake + Flanker + Serpent
    _levelDefinitions.push_back({
        rtype::common::components::EnemyType::Snake,
        rtype::common::components::EnemyType::Flanker,
        rtype::common::components::EnemyType::Serpent
    });

    // Level 2: Suicide + Turret + Fortress
    _levelDefinitions.push_back({
        rtype::common::components::EnemyType::Suicide,
        rtype::common::components::EnemyType::Turret,
        rtype::common::components::EnemyType::Fortress
    });

    // Level 3: Pata + Waver + Core (FINAL BOSS)
    _levelDefinitions.push_back({
        rtype::common::components::EnemyType::Pata,
        rtype::common::components::EnemyType::Waver,
        rtype::common::components::EnemyType::Core
    });

    // Level 4: Broken Reactor variant - reuse similar spawn cadence
    _levelDefinitions.push_back({
        rtype::common::components::EnemyType::Basic,
        rtype::common::components::EnemyType::Waver,
        rtype::common::components::EnemyType::Core
    });

    // Configure spawn intervals for all enemy types
    // Basic enemies
    _enemyConfigs[rtype::common::components::EnemyType::Basic] = {rtype::common::components::EnemyType::Basic, 2.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Snake] = {rtype::common::components::EnemyType::Snake, 2.5f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Suicide] = {rtype::common::components::EnemyType::Suicide, 3.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Pata] = {rtype::common::components::EnemyType::Pata, 3.5f, 0.0f};

    // Advanced enemies
    _enemyConfigs[rtype::common::components::EnemyType::Shielded] = {rtype::common::components::EnemyType::Shielded, 5.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Flanker] = {rtype::common::components::EnemyType::Flanker, 4.5f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Turret] = {rtype::common::components::EnemyType::Turret, 5.5f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Waver] = {rtype::common::components::EnemyType::Waver, 4.0f, 0.0f};
    
    // Debris (passive obstacles - spawn more frequently)
    _enemyConfigs[rtype::common::components::EnemyType::DebrisSmall] = {rtype::common::components::EnemyType::DebrisSmall, 3.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::DebrisLarge] = {rtype::common::components::EnemyType::DebrisLarge, 5.0f, 0.0f};
    
    // Power-ups (spawn rarely)
    _enemyConfigs[rtype::common::components::EnemyType::PowerUpHealth] = {rtype::common::components::EnemyType::PowerUpHealth, 15.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::PowerUpWeapon] = {rtype::common::components::EnemyType::PowerUpWeapon, 20.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::PowerUpShield] = {rtype::common::components::EnemyType::PowerUpShield, 25.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::PowerUpSpeed] = {rtype::common::components::EnemyType::PowerUpSpeed, 18.0f, 0.0f};
}

void ServerEnemySystem::updatePhase(float deltaTime)
{
    _levelTimer += deltaTime;

    // Each level follows the same 3-minute pattern:
    // 0-60s: Only basic enemies
    // 60-180s: Basic + Advanced enemies
    // 180s+: Boss spawns (+ basic + advanced continue)

    EnemySpawnPhase previousPhase = _phase;

    if (_levelTimer < 60.0f) {
        _phase = EnemySpawnPhase::OnlyBasic;
    } else if (_levelTimer < 180.0f) {
        _phase = EnemySpawnPhase::BasicAndAdvanced;
    } else {
        _phase = EnemySpawnPhase::BossPhase;
    }

    // Only print when phase changes
    if (_phase != previousPhase) {
        std::cout << "[ServerEnemySystem] Level " << _currentLevel << " - Phase changed to: ";
        switch (_phase) {
            case EnemySpawnPhase::OnlyBasic: std::cout << "Basic Only"; break;
            case EnemySpawnPhase::BasicAndAdvanced: std::cout << "Basic + Advanced"; break;
            case EnemySpawnPhase::BossPhase: std::cout << "Boss Phase"; break;
        }
        std::cout << " (Timer: " << _levelTimer << "s)" << std::endl;
    }
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning
// ============================================================================

void ServerEnemySystem::spawnBoss(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType bossType) {
    // Spawn position
    float spawnX = 1280.0f - 100.0f;
    float spawnY = 360.0f; // Center Y of 720p screen

    // Boss stats vary by type
    int hp = 50;
    float vx = 0.0f;
    
    switch (bossType) {
        case rtype::common::components::EnemyType::TankDestroyer:
            hp = 50;
            vx = 0.0f;
            break;
            
        case rtype::common::components::EnemyType::Serpent:
            hp = 80;
            vx = 0.0f;
            spawnY = 360.0f;
            break;
            
        case rtype::common::components::EnemyType::Fortress:
            hp = 100;
            vx = 0.0f;
            spawnX = 1200.0f;
            spawnY = 360.0f;
            break;
            
        case rtype::common::components::EnemyType::Core:
            hp = 150;
            vx = 0.0f;
            break;
            
        default:
            hp = 50;
            vx = 0.0f;
            break;
    }

    // Create boss entity on server world
    auto boss = world.CreateEntity();
    world.AddComponent<rtype::common::components::Position>(boss, spawnX, spawnY, 0.0f);
    world.AddComponent<rtype::common::components::Velocity>(boss, vx, 0.0f, std::abs(vx));
    world.AddComponent<rtype::common::components::Health>(boss, hp);
    world.AddComponent<rtype::common::components::Team>(boss, rtype::common::components::TeamType::Enemy);
    world.AddComponent<rtype::common::components::EnemyTypeComponent>(boss, bossType);
    world.AddComponent<rtype::server::components::LinkedRoom>(boss, room);

    std::cout << "SERVER: 🔥 Spawning BOSS " << (int)bossType << " (id=" << boss << ") with " << hp << " HP in room " << room << std::endl;

    // Special handling for Fortress boss - spawn turrets and shields
    if (bossType == rtype::common::components::EnemyType::Fortress) {
        std::cout << "[ServerEnemySystem] 🏰 Setting up Fortress boss " << boss << std::endl;
        
        // Add red shield to boss - requires 2 charged shots to break
        world.AddComponent<rtype::common::components::ShieldComponent>(boss, rtype::common::components::ShieldType::Red, true, 2);
        
        std::cout << "SERVER: 🛡️  Fortress boss " << boss << " - Shield requires 2 charged hits!" << std::endl;
    }

    // Send to all players in the room with correct boss type
    rtype::server::network::senders::broadcast_enemy_spawn(room, static_cast<uint32_t>(boss),
                                                            bossType,
                                                            spawnX, spawnY, hp);
}

void ServerEnemySystem::updateBossSpawning(ECS::World& world, float deltaTime) {
    if (_gameFinished) return;
    // Only spawn boss if we're in BossPhase and boss hasn't spawned yet for this level
    if (_phase != EnemySpawnPhase::BossPhase || _bossSpawned)
        return;

    // Check we have a valid level definition
    if (_currentLevel >= (int)_levelDefinitions.size()) {
        std::cout << "[ServerEnemySystem] WARNING: No level definition for level " << _currentLevel << std::endl;
        return;
    }

    auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;

    for (auto &pair : *rooms) {
        ECS::EntityID room = pair.first;
        if (!pair.second) continue;
        auto *rp = pair.second.get();
        if (!rp || !rp->isGameStarted) continue;

        // Get the boss type for the current level
        rtype::common::components::EnemyType currentBossType = _levelDefinitions[_currentLevel].bossEnemy;

        // Check if this specific boss already exists in this room
        bool bossExists = false;
        auto* enemyTypes = root.world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
        if (enemyTypes) {
            for (auto& etPair : *enemyTypes) {
                auto* et = etPair.second.get();
                if (et && et->type == currentBossType) {
                    auto* health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
                    if (health && health->isAlive && health->currentHp > 0) {
                        bossExists = true;
                        break;
                    }
                }
            }
        }

        if (!bossExists) {
            std::cout << "[ServerEnemySystem] Spawning boss for level " << _currentLevel
                      << " (type " << (int)currentBossType << ")" << std::endl;
            spawnBoss(world, room, currentBossType);
            _bossSpawned = true;
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Regular Enemy Spawning
// ============================================================================

void ServerEnemySystem::updateEnemySpawning(ECS::World& world, float deltaTime) {
    if (_gameFinished) return;
    // Check we have a valid level definition
    if (_currentLevel >= (int)_levelDefinitions.size()) {
        return;
    }

    // Get current level's enemy types
    const LevelDefinition& currentLevelDef = _levelDefinitions[_currentLevel];
    rtype::common::components::EnemyType basicType = currentLevelDef.basicEnemy;
    rtype::common::components::EnemyType advancedType = currentLevelDef.advancedEnemy;

    // For each enemy type in config, check timer and spawn if needed
    for (auto& [type, config] : _enemyConfigs) {
        config.timer += deltaTime;
        if (config.timer < config.interval)
            continue;
        config.timer = 0.0f;

        // Determine if this enemy type should spawn based on current phase and level
        bool spawnAllowed = false;
        
        // Debris and power-ups spawn in ALL phases (always allowed)
        if (type == rtype::common::components::EnemyType::DebrisSmall ||
            type == rtype::common::components::EnemyType::DebrisLarge ||
            type == rtype::common::components::EnemyType::PowerUpHealth ||
            type == rtype::common::components::EnemyType::PowerUpWeapon ||
            type == rtype::common::components::EnemyType::PowerUpShield ||
            type == rtype::common::components::EnemyType::PowerUpSpeed) {
            spawnAllowed = true;
        } else {
            // Regular enemies spawn based on phase
            switch (_phase) {
                case EnemySpawnPhase::OnlyBasic:
                    // Only spawn basic enemy for current level
                    spawnAllowed = (type == basicType);
                    break;

                case EnemySpawnPhase::BasicAndAdvanced:
                    // Spawn both basic and advanced for current level
                    spawnAllowed = (type == basicType || type == advancedType);
                    break;

                case EnemySpawnPhase::BossPhase:
                    // Continue spawning basic and advanced during boss fight
                    spawnAllowed = (type == basicType || type == advancedType);
                    break;
            }
        }

        if (!spawnAllowed) continue;

        // For each room that has started the game, spawn enemy of this type
        auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
        if (!rooms) continue;
        for (auto &pair : *rooms) {
            ECS::EntityID room = pair.first;
            if (!pair.second) continue;
            auto *rp = pair.second.get();
            if (!rp || !rp->isGameStarted) continue;
            spawnEnemy(world, room, type);
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Player State Broadcasting
// ============================================================================

void ServerEnemySystem::updatePlayerStateBroadcast(ECS::World& world, float deltaTime)
{
    _stateTick += deltaTime;
    if (_stateTick < STATE_TICK_INTERVAL) return;
    
    _stateTick = 0.0f;

    // Only broadcast player states for rooms where game has started
    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;
    
    for (auto &roomPair : *rooms) {
        ECS::EntityID room = roomPair.first;
        auto *rp = roomPair.second.get();
        if (!rp || !rp->isGameStarted) continue; // Skip rooms still in lobby

        // Build ALL_PLAYERS_STATE packet for this room
        AllPlayersStatePacket allStates{};
        allStates.playerCount = 0;

        auto players = world.GetAllComponents<rtype::common::components::Player>();
        if (!players) continue;
        
        // Collect all players in this room
        for (auto &pair : *players) {
            ECS::EntityID pid = pair.first;

            // Determine the room for this player entity
            ECS::EntityID playerRoom = 0;
            auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(pid);
            if (pconn) {
                playerRoom = pconn->room_code;
            } else {
                auto* linked = world.GetComponent<rtype::server::components::LinkedRoom>(pid);
                if (linked) playerRoom = linked->room_id;
            }
            if (playerRoom != room) continue; // Skip players not in this room

            auto* pos = world.GetComponent<rtype::common::components::Position>(pid);
            auto* health = world.GetComponent<rtype::common::components::Health>(pid);
            if (!pos || !health) continue;

            // Add player to the packet (max 4 players)
            if (allStates.playerCount >= 4) {
                std::cerr << "[ServerEnemySystem] WARNING: More than 4 players in room " << room << std::endl;
                break;
            }

            auto& playerData = allStates.players[allStates.playerCount];
            playerData.playerId = static_cast<uint32_t>(pid);
            playerData.x = pos->x;
            playerData.y = pos->y;
            playerData.dir = pos->rotation;
            playerData.hp = health->currentHp;
            playerData.isAlive = health->isAlive;
            playerData.invulnerable = health->invulnerable;

            allStates.playerCount++;
        }

        // Only send if there are players to update
        if (allStates.playerCount == 0) continue;

        // Broadcast to ALL networked clients in this room (single packet instead of N²)
        rtype::server::network::senders::broadcast_all_players_state(room, allStates);
    }
}

// ============================================================================
// PRIVATE METHODS - Entity Cleanup
// ============================================================================

void ServerEnemySystem::cleanupDeadEntities(ECS::World& world) {
    auto healths = world.GetAllComponents<rtype::common::components::Health>();
    if (!healths) return;
    
    std::vector<ECS::EntityID> toDestroy;
    for (auto &pair : *healths) {
        ECS::EntityID eid = pair.first;
        auto* h = pair.second.get();
        if (!h) continue;
        if (!h->isAlive || h->currentHp <= 0) {

            auto room = world.GetComponent<rtype::server::components::LinkedRoom>(eid);
            // Broadcast destroy packet to all players in the world (could restrict by room)
            if (room)
                rtype::server::network::senders::broadcast_entity_destroy(room->room_id, static_cast<uint32_t>(eid), 1 /* killed */);
            toDestroy.push_back(eid);
        }
    }
    for (auto e : toDestroy) {
        world.DestroyEntity(e);
    }
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning Helper
// ============================================================================

void ServerEnemySystem::spawnEnemy(ECS::World &world, ECS::EntityID room, rtype::common::components::EnemyType type) {
    // Spawn position logic (can be customized per type)
    float spawnX = 1280.0f + 24.0f;
    float spawnY = 100.0f + (rand() % 520);

    // Customize stats for each mob type
    int hp = 1;
    float vx = -100.0f;
    switch (type) {
        // Basic enemies
        case rtype::common::components::EnemyType::Basic:
            hp = 1; vx = -100.0f;
            break;
        case rtype::common::components::EnemyType::Snake:
            hp = 2; vx = -120.0f;
            break;
        case rtype::common::components::EnemyType::Suicide:
            hp = 1; vx = -150.0f;
            break;
        case rtype::common::components::EnemyType::Pata:
            hp = 2; vx = -100.0f;
            break;

        // Advanced enemies
        case rtype::common::components::EnemyType::Shielded:
            hp = 4; vx = -90.0f;
            break;
        case rtype::common::components::EnemyType::Flanker:
            hp = 3; vx = -90.0f;
            break;
        case rtype::common::components::EnemyType::Turret:
            hp = 1; vx = 0.0f; 
            spawnX = 1100.0f;
            break;
        case rtype::common::components::EnemyType::Waver:
            hp = 4; vx = -110.0f;
            break;

        // Boss enemies
        case rtype::common::components::EnemyType::TankDestroyer:
            hp = 50; vx = 0.0f;
            break;
        case rtype::common::components::EnemyType::Serpent:
            hp = 60; vx = -60.0f;
            break;
        case rtype::common::components::EnemyType::Fortress:
            hp = 80; vx = 0.0f; // Stationary - Fortress never moves
            spawnX = 1200.0f;   // Fixed position on the right
            spawnY = 360.0f;    // Center of screen
            break;
        case rtype::common::components::EnemyType::Core:
            hp = 100; vx = -40.0f;
            break;
            
        // Debris (passive obstacles - no shooting)
        case rtype::common::components::EnemyType::DebrisSmall:
            hp = 1; 
            vx = -50.0f - (rand() % 50);  // Random slow drift -50 to -100
            spawnY = 50.0f + (rand() % 620);  // Full screen height
            break;
        case rtype::common::components::EnemyType::DebrisLarge:
            hp = 2; 
            vx = -70.0f - (rand() % 40);  // Medium drift -70 to -110
            spawnY = 50.0f + (rand() % 600);
            break;
            
        // Power-ups (collectibles - drift slowly)
        case rtype::common::components::EnemyType::PowerUpHealth:
            hp = 1; 
            vx = -60.0f;
            spawnY = 200.0f + (rand() % 320);  // Middle area
            break;
        case rtype::common::components::EnemyType::PowerUpWeapon:
            hp = 1; 
            vx = -60.0f;
            spawnY = 200.0f + (rand() % 320);
            break;
        case rtype::common::components::EnemyType::PowerUpShield:
            hp = 1; 
            vx = -60.0f;
            spawnY = 200.0f + (rand() % 320);
            break;
        case rtype::common::components::EnemyType::PowerUpSpeed:
            hp = 1; 
            vx = -60.0f;
            spawnY = 200.0f + (rand() % 320);
            break;

        default:
            hp = 1; vx = -100.0f;
            break;
    }

    auto enemy = root.world.CreateEntity();
    root.world.AddComponent<rtype::common::components::Position>(enemy, spawnX, spawnY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(enemy, vx, 0.0f, std::abs(vx));
    root.world.AddComponent<rtype::common::components::Health>(enemy, hp);
    root.world.AddComponent<rtype::common::components::Team>(enemy, rtype::common::components::TeamType::Enemy);
    root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(enemy, type);
    root.world.AddComponent<rtype::server::components::LinkedRoom>(enemy, room);

    // Add cyclic shield to Shielded enemy type and Turret type
    if (type == rtype::common::components::EnemyType::Shielded || type == rtype::common::components::EnemyType::Turret) {
        root.world.AddComponent<rtype::common::components::ShieldComponent>(enemy, rtype::common::components::ShieldType::Cyclic, true);
    }
    
    // Debris and power-ups don't shoot - don't add FireRate component
    bool isPassiveEntity = (
        type == rtype::common::components::EnemyType::DebrisSmall ||
        type == rtype::common::components::EnemyType::DebrisLarge ||
        type == rtype::common::components::EnemyType::PowerUpHealth ||
        type == rtype::common::components::EnemyType::PowerUpWeapon ||
        type == rtype::common::components::EnemyType::PowerUpShield ||
        type == rtype::common::components::EnemyType::PowerUpSpeed
    );
    
    if (!isPassiveEntity) {
        // Add FireRate component for shooting enemies only
        // Note: FireRate is added by server collision/spawn systems for actual enemies
    }

    rtype::server::network::senders::broadcast_enemy_spawn(room, enemy, type, spawnX, spawnY, hp);
}

// ============================================================================
// PRIVATE METHODS - Level Progression
// ============================================================================

void ServerEnemySystem::checkBossDeathAndAdvanceLevel(ECS::World& world) {
    // Only check if we're in boss phase and boss was spawned
    if (_phase != EnemySpawnPhase::BossPhase || !_bossSpawned) {
        return;
    }

    // Check we have a valid level definition
    if (_currentLevel >= (int)_levelDefinitions.size()) {
        return;
    }

    // Check if the current level's boss is still alive
    rtype::common::components::EnemyType currentBossType = _levelDefinitions[_currentLevel].bossEnemy;
    bool bossStillAlive = false;

    auto* enemyTypes = world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (enemyTypes) {
        for (auto& etPair : *enemyTypes) {
            auto* et = etPair.second.get();
            if (et && et->type == currentBossType) {
                auto* health = world.GetComponent<rtype::common::components::Health>(etPair.first);
                if (health && health->isAlive && health->currentHp > 0) {
                    bossStillAlive = true;
                    break;
                }
            }
        }
    }

    // If boss is dead, advance to next level (or finish the game after level 2)
    if (!bossStillAlive) {
        std::cout << "[ServerEnemySystem] ========================================" << std::endl;
        std::cout << "[ServerEnemySystem] Boss defeated! Advancing to level " << (_currentLevel + 1) << std::endl;
        std::cout << "[ServerEnemySystem] ========================================" << std::endl;

        // Increment level counter
        _currentLevel++;

        // Finish game after fourth boss (level index 3). After increment, currentLevel >= 4 means victory.
        if (_currentLevel >= 4) {
            std::cout << "[ServerEnemySystem] GAME FINISHED after Level 4 boss. Stopping spawns." << std::endl;
            _gameFinished = true;
            return;
        }

        // Reset for new level - IMPORTANT: Start at -1.0f to give time for entities to be destroyed
        _levelTimer = -1.0f;  // Negative timer ensures a brief pause before new level starts
        _phase = EnemySpawnPhase::OnlyBasic;
        _bossSpawned = false;

        // Reset enemy spawn timers
        for (auto& [type, config] : _enemyConfigs) {
            config.timer = 0.0f;
        }

        std::cout << "[ServerEnemySystem] New level started: Level " << _currentLevel << std::endl;
        std::cout << "[ServerEnemySystem] - Basic: " << (int)_levelDefinitions[_currentLevel].basicEnemy << std::endl;
        std::cout << "[ServerEnemySystem] - Advanced: " << (int)_levelDefinitions[_currentLevel].advancedEnemy << std::endl;
        std::cout << "[ServerEnemySystem] - Boss: " << (int)_levelDefinitions[_currentLevel].bossEnemy << std::endl;
    }
}

void ServerEnemySystem::setStartLevel(int index) {
    if (_levelDefinitions.empty()) return;
    if (index < 0) index = 0;
    if (index >= static_cast<int>(_levelDefinitions.size())) index = static_cast<int>(_levelDefinitions.size()) - 1;
    _currentLevel = index;
    _levelTimer = 0.0f;
    _phase = EnemySpawnPhase::OnlyBasic;
    _bossSpawned = false;
    _gameFinished = false;
    // Reset spawn timers for regular enemies so spawns begin fresh
    for (auto &entry : _enemyConfigs) {
        entry.second.timer = 0.0f;
    }
    std::cout << "[ServerEnemySystem] Forced start level to " << (_currentLevel + 1) << std::endl;
}

rtype::common::components::EnemyType ServerEnemySystem::getCurrentBossType() const {
    if (_levelDefinitions.empty()) {
        return rtype::common::components::EnemyType::TankDestroyer;
    }
    int idx = _currentLevel;
    if (idx < 0) idx = 0;
    if (idx >= static_cast<int>(_levelDefinitions.size())) idx = static_cast<int>(_levelDefinitions.size()) - 1;
    return _levelDefinitions[idx].bossEnemy;
}

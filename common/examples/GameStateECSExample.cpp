/**
 * @file GameStateECSExample.cpp
 * @brief Example of how to use the modular ECS architecture in GameState
 * 
 * This is a reference implementation showing how to integrate the new
 * common ECS components and systems into GameState while maintaining
 * the original game logic.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

/*
==============================================================================
EXAMPLE: Modular ECS Integration in GameState
==============================================================================

This example shows how to update GameState to use the modular ECS architecture
while keeping all your original game logic intact.

------------------------------------------------------------------------------
1. INCLUDE THE MODULAR ECS COMPONENTS
------------------------------------------------------------------------------
*/

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/systems/Systems.h>
#include <common/utils/EntityFactory.h>

/*
------------------------------------------------------------------------------
2. ADD ECS WORLD TO GAMESTATE
------------------------------------------------------------------------------

In GameState.h, add:

class GameState : public State {
private:
    // ECS World for managing entities
    ECS::World m_world;
    
    // Entity IDs
    ECS::EntityID m_playerEntity{0};
    std::vector<ECS::EntityID> m_enemyEntities;
    std::vector<ECS::EntityID> m_projectileEntities;
    
    // Keep existing members for now (can be removed later)
    Player m_player;
    std::vector<Enemy> m_enemies;
    std::vector<Projectile> m_projectiles;
    ...
};

------------------------------------------------------------------------------
3. CREATE ENTITIES USING FACTORY FUNCTIONS
------------------------------------------------------------------------------
*/

void exampleCreatePlayer(ECS::World& world) {
    using namespace rtype::common;
    
    // Create player with common components
    auto playerEntity = factory::createPlayer(world, 100.0f, 360.0f, 3);
    
    // Add client-specific components (if using client components)
    // world.AddComponent<client::components::Sprite>(playerEntity, ...);
    // world.AddComponent<client::components::Controllable>(playerEntity);
}

void exampleCreateEnemy(ECS::World& world, float x, float y) {
    using namespace rtype::common;
    
    auto enemyEntity = factory::createEnemy(world, x, y);
    // Store entity ID for later iteration
    // m_enemyEntities.push_back(enemyEntity);
}

void exampleFireProjectile(ECS::World& world, float x, float y) {
    using namespace rtype::common;
    
    auto projectile = factory::createProjectile(world, x, y, 500.0f);
    // m_projectileEntities.push_back(projectile);
}

/*
------------------------------------------------------------------------------
4. UPDATE GAME LOGIC USING MODULAR SYSTEMS
------------------------------------------------------------------------------
*/

void exampleUpdate(ECS::World& world, float deltaTime) {
    using namespace rtype::common::systems;
    
    // Update movement (shared system)
    MovementSystem::update(world, deltaTime);
    
    // Update health/invulnerability (shared system)
    HealthSystem::update(world, deltaTime);
    
    // Client-specific updates would go here
    // m_renderSystem->update(world, deltaTime);
}

/*
------------------------------------------------------------------------------
5. MANUAL MIGRATION EXAMPLE: updatePlayer()
------------------------------------------------------------------------------

BEFORE (struct-based):
*/

void updatePlayerOLD(Player& player, float deltaTime) {
    // Update invulnerability timer
    if (player.invulnerabilityTimer > 0.0f) {
        player.invulnerabilityTimer -= deltaTime;
    }
    
    // Update position
    sf::Vector2f movement = getMovementInput();
    player.position += movement * player.speed * deltaTime;
}

/*
AFTER (modular ECS):
*/

void updatePlayerNEW(ECS::World& world, ECS::EntityID player, float deltaTime) {
    using namespace rtype::common::components;
    
    // Health system automatically handles invulnerability
    // (no need to manually update timer)
    
    // Get components
    auto* pos = world.GetComponent<Position>(player);
    auto* vel = world.GetComponent<Velocity>(player);
    
    if (!pos || !vel) return;
    
    // Update velocity based on input
    sf::Vector2f movement = getMovementInput();
    vel->vx = movement.x * vel->maxSpeed;
    vel->vy = movement.y * vel->maxSpeed;
    
    // Movement system will automatically update position
    // (no need to manually update position)
}

/*
------------------------------------------------------------------------------
6. COLLISION DETECTION WITH ECS
------------------------------------------------------------------------------
*/

void exampleCheckCollisions(ECS::World& world, 
                            ECS::EntityID player,
                            const std::vector<ECS::EntityID>& enemies) {
    using namespace rtype::common::components;
    
    auto* playerPos = world.GetComponent<Position>(player);
    auto* playerHealth = world.GetComponent<Health>(player);
    
    if (!playerPos || !playerHealth) return;
    if (playerHealth->invulnerable) return; // Skip if invulnerable
    
    // Check collision with each enemy
    for (auto enemyEntity : enemies) {
        auto* enemyPos = world.GetComponent<Position>(enemyEntity);
        if (!enemyPos) continue;
        
        // Simple AABB collision (you can keep your existing logic)
        float dx = playerPos->x - enemyPos->x;
        float dy = playerPos->y - enemyPos->y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance < 32.0f) { // Collision!
            // Damage player
            playerHealth->currentHp -= 1;
            
            // Grant invulnerability
            playerHealth->invulnerable = true;
            playerHealth->invulnerabilityTimer = 2.0f;
            
            std::cout << "Hit! Lives: " << playerHealth->currentHp << std::endl;
        }
    }
}

/*
------------------------------------------------------------------------------
7. RENDERING WITH ECS (keep existing render code!)
------------------------------------------------------------------------------
*/

void exampleRenderPlayer(sf::RenderWindow& window, 
                         ECS::World& world, 
                         ECS::EntityID player) {
    using namespace rtype::common::components;
    
    auto* pos = world.GetComponent<Position>(player);
    auto* health = world.GetComponent<Health>(player);
    
    if (!pos || !health) return;
    
    // Skip rendering if invulnerable and flashing
    if (health->invulnerable) {
        int flashCycle = static_cast<int>(health->invulnerabilityTimer / 0.15f);
        if (flashCycle % 2 == 0) return;
    }
    
    // Your existing rendering code
    sf::RectangleShape playerShip(sf::Vector2f(32.0f, 32.0f));
    playerShip.setPosition(pos->x - 16.0f, pos->y - 16.0f);
    playerShip.setFillColor(sf::Color::Green);
    window.draw(playerShip);
}

/*
------------------------------------------------------------------------------
8. RENDERING HUD WITH HEARTS (3 lives display)
------------------------------------------------------------------------------
*/

void exampleRenderHUD(sf::RenderWindow& window,
                     ECS::World& world,
                     ECS::EntityID player) {
    using namespace rtype::common::components;
    
    auto* health = world.GetComponent<Health>(player);
    if (!health) return;
    
    // Draw lives as hearts
    for (int i = 0; i < health->currentHp; ++i) {
        // Simple heart shape using circles
        sf::CircleShape heart(10.0f);
        heart.setPosition(10.0f + i * 30.0f, 10.0f);
        heart.setFillColor(sf::Color::Red);
        window.draw(heart);
        
        // Or use your own heart texture:
        // sf::Sprite heartSprite;
        // heartSprite.setTexture(heartTexture);
        // heartSprite.setPosition(10.0f + i * 30.0f, 10.0f);
        // window.draw(heartSprite);
    }
}

/*
------------------------------------------------------------------------------
9. CLEANUP: Remove dead entities
------------------------------------------------------------------------------
*/

void exampleCleanup(ECS::World& world, std::vector<ECS::EntityID>& entities) {
    using namespace rtype::common::components;
    
    // Remove dead entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [&world](ECS::EntityID entity) {
                auto* health = world.GetComponent<Health>(entity);
                if (health && !health->isAlive) {
                    world.DestroyEntity(entity);
                    return true;
                }
                
                // Also remove off-screen entities
                auto* pos = world.GetComponent<Position>(entity);
                if (pos && (pos->x < -50.0f || pos->x > 1330.0f)) {
                    world.DestroyEntity(entity);
                    return true;
                }
                
                return false;
            }),
        entities.end()
    );
}

/*
==============================================================================
MIGRATION STRATEGY - Gradual Transition
==============================================================================

You don't need to migrate everything at once! Here's a gradual approach:

PHASE 1: Keep existing code, add ECS in parallel
- Keep your existing Player, Enemy, Projectile structs
- Create ECS entities alongside them
- Sync data between structs and ECS
- Test that both systems work

PHASE 2: Use ECS for new features
- New projectiles use only ECS
- New enemies use only ECS
- Original player still uses struct

PHASE 3: Migrate piece by piece
- Migrate projectiles fully to ECS
- Migrate enemies fully to ECS
- Migrate player last

PHASE 4: Remove old structs
- Delete Player, Enemy, Projectile structs
- Use only ECS

==============================================================================
BENEFITS OF THIS MODULAR APPROACH
==============================================================================

✅ Separation of concerns (client/server logic separated)
✅ Code reuse (common systems used by both client and server)
✅ Easy testing (systems are independent)
✅ Flexibility (add/remove components at runtime)
✅ Performance (cache-friendly data layout)
✅ Scalability (easy to add new entity types)

==============================================================================
*/

// This is just an example file - not meant to be compiled
// Copy the patterns above into your actual GameState.cpp

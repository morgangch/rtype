#include <iostream>
#include <ECS/ECS.hpp>

// Inclure les composants communs

#include <common/components/Position.hpp>
#include <common/components/Velocity.hpp>


// Inclure les composants spécifiques au client
#include "components/client.hpp"
//#include "components/render.hpp"
//#include "components/camera.hpp"
//#include "components/audio.hpp"

// Inclure les systèmes client
#include "client/components/systems/RenderSystem.hpp"
#include "client/components/systems/InputSystem.hpp"

int main() {
    std::cout << "Hello World from Client!" << std::endl;
    std::cout << "Client is connecting..." << std::endl;

    // Démonstration ECS avec composants client/serveur
    std::cout << "\n=== ECS Library Demo (Client avec composants partagés) ===" << std::endl;

    ECS::World world;
    
    // Créer des entités
    auto player = world.CreateEntity();
    auto enemy = world.CreateEntity();
    auto ui_element = world.CreateEntity();

    std::cout << "Created entities: Player(" << player << "), Enemy(" << enemy << "), UI(" << ui_element << ")" << std::endl;

    // Ajouter des composants communs (partagés avec le serveur)

    using rtype::common::components::Position;
    using rtype::common::components::Velocity;
    using rtype::client::components::Renderable;
    using rtype::client::components::RenderLayer;
    using rtype::client::components::Camera;
    using rtype::client::components::AudioSource;
    using rtype::client::components::AudioType;

    world.AddComponent<Position>(player, 100.0f, 200.0f, 0.0f);
    world.AddComponent<Velocity>(player, 0.0f, 0.0f, 250.0f);

    world.AddComponent<Position>(enemy, 300.0f, 150.0f, 3.14f/2);
    world.AddComponent<Velocity>(enemy, -50.0f, 0.0f, 100.0f);

    // Ajouter des composants spécifiques au client
    world.AddComponent<Renderable>(player, "player.png", 64.0f, 64.0f, RenderLayer::Entities);
    world.AddComponent<Renderable>(enemy, "enemy.png", 48.0f, 48.0f, RenderLayer::Entities);
    world.AddComponent<Renderable>(ui_element, "health_bar.png", 200.0f, 20.0f, RenderLayer::UI);

    // Ajouter caméra
    world.AddComponent<Camera>(world.CreateEntity(), 1.0f);

    // Ajouter son
    world.AddComponent<AudioSource>(player, "player_move.wav", AudioType::SFX);

    std::cout << "Added components to entities" << std::endl;

    // Simulation d'une boucle de jeu simplifiée
    std::cout << "\n=== Simulation de boucle de jeu ===" << std::endl;
    
    for (int frame = 0; frame < 5; ++frame) {
        float deltaTime = 0.016f; // ~60 FPS
        
        std::cout << "Frame " << frame << ":" << std::endl;
        
        // Système de mouvement (logique partagée client/serveur)
    auto* positions = world.GetAllComponents<Position>();
        if (positions) {
            for (const auto& pair : *positions) {
                ECS::EntityID entity = pair.first;
                auto* pos = pair.second.get();
                auto* vel = world.GetComponent<Velocity>(entity);
                
                if (vel) {
                    pos->x += vel->vx * deltaTime;
                    pos->y += vel->vy * deltaTime;
                    std::cout << "  Entity " << entity << " moved to (" << pos->x << ", " << pos->y << ")" << std::endl;
                }
            }
        }
        
        // Système de rendu (spécifique client)
    auto* renderables = world.GetAllComponents<rtype::client::components::Renderable>();
        if (renderables) {
            std::cout << "  Rendering:" << std::endl;
            for (const auto& pair : *renderables) {
                ECS::EntityID entity = pair.first;
                auto* renderable = pair.second.get();
                auto* pos = world.GetComponent<rtype::common::components::Position>(entity);
                
                if (pos && renderable->visible) {
                    std::cout << "    " << renderable->texturePath 
                              << " at (" << pos->x << ", " << pos->y << ") "
                              << "layer=" << static_cast<int>(renderable->layer) << std::endl;
                }
            }
        }
    }

    std::cout << "\nTotal alive entities: " << world.GetAliveEntityCount() << std::endl;
    return 0;
}

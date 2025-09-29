#include <iostream>#include <iostream>

#include <graphics/Graphics.hpp>#include <graphics/Graphics.hpp>

#include <chrono>#include <chrono>

#include <thread>#include <thread>

#include <cmath>#include <cmath>



using namespace rtype::client::graphics;using namespace rtype::client::graphics;



int main() {int main() {

    std::cout << "=== R-Type Client - Graphics Test ===" << std::endl;    std::cout << "=== R-Type Client - Graphics Test ===" << std::endl;

        // Initialize graphics system

    // Initialize graphics system    Graphics graphics;

    Graphics graphics;    if (!graphics.Initialize(800, 600, "R-Type - Graphics Test")) {

    if (!graphics.Initialize(800, 600, "R-Type - Graphics Test")) {        std::cout << "Failed to initialize graphics!" << std::endl;

        std::cout << "Failed to initialize graphics!" << std::endl;        return -1;

        return -1;=======

    }    // Créer des entités

        auto player = world.CreateEntity();

    std::cout << "Graphics initialized successfully!" << std::endl;    auto enemy = world.CreateEntity();

    std::cout << "Controls: ESC to close window" << std::endl;    auto ui_element = world.CreateEntity();

    

    // Create some test sprites    std::cout << "Created entities: Player(" << player << "), Enemy(" << enemy << "), UI(" << ui_element << ")" << std::endl;

    Sprite testSprite;

    testSprite.SetPosition(400, 300); // Center of screen    // Ajouter des composants communs (partagés avec le serveur)

    testSprite.SetScale(2.0f);

        using rtype::common::components::Position;

    // Test variables for animation    using rtype::common::components::Velocity;

    float time = 0.0f;    using rtype::client::components::Renderable;

    const float speed = 100.0f;    using rtype::client::components::RenderLayer;

        using rtype::client::components::Camera;

    // Main game loop    using rtype::client::components::AudioSource;

    while (graphics.IsRunning()) {    using rtype::client::components::AudioType;

        // Handle events

        graphics.PollEvents();    world.AddComponent<Position>(player, 100.0f, 200.0f, 0.0f);

            world.AddComponent<Velocity>(player, 0.0f, 0.0f, 250.0f);

        // Update

        time += 0.016f; // ~60 FPS    world.AddComponent<Position>(enemy, 300.0f, 150.0f, 3.14f/2);

            world.AddComponent<Velocity>(enemy, -50.0f, 0.0f, 100.0f);

        // Move test objects

        float circleX = 400 + std::sin(time) * 200;    // Ajouter des composants spécifiques au client

        float circleY = 300 + std::cos(time) * 100;    world.AddComponent<Renderable>(player, "player.png", 64.0f, 64.0f, RenderLayer::Entities);

            world.AddComponent<Renderable>(enemy, "enemy.png", 48.0f, 48.0f, RenderLayer::Entities);

        // Render    world.AddComponent<Renderable>(ui_element, "health_bar.png", 200.0f, 20.0f, RenderLayer::UI);

        graphics.BeginFrame();

            // Ajouter caméra

        // Draw some test primitives    world.AddComponent<Camera>(world.CreateEntity(), 1.0f);

        graphics.DrawRectangle(50, 50, 100, 50, 0xFF0000FF);  // Red rectangle

        graphics.DrawCircle(circleX, circleY, 30, 0x00FF00FF); // Green moving circle    // Ajouter son

        graphics.DrawRectangle(650, 500, 100, 50, 0x0000FFFF); // Blue rectangle    world.AddComponent<AudioSource>(player, "player_move.wav", AudioType::SFX);

        

        // Draw grid    std::cout << "Added components to entities" << std::endl;

        for (int x = 0; x < 800; x += 100) {

            graphics.DrawRectangle(x, 0, 2, 600, 0x444444FF);    // Simulation d'une boucle de jeu simplifiée

        }    std::cout << "\n=== Simulation de boucle de jeu ===" << std::endl;

        for (int y = 0; y < 600; y += 100) {    

            graphics.DrawRectangle(0, y, 800, 2, 0x444444FF);    for (int frame = 0; frame < 5; ++frame) {

        }        float deltaTime = 0.016f; // ~60 FPS

                

        graphics.EndFrame();        std::cout << "Frame " << frame << ":" << std::endl;

                

        // Cap framerate        // Système de mouvement (logique partagée client/serveur)

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS    auto* positions = world.GetAllComponents<Position>();

    }        if (positions) {

                for (const auto& pair : *positions) {

    std::cout << "Graphics test finished!" << std::endl;                ECS::EntityID entity = pair.first;

    return 0;                auto* pos = pair.second.get();

}                auto* vel = world.GetComponent<Velocity>(entity);
                
                if (vel) {
                    pos->x += vel->vx * deltaTime;
                    pos->y += vel->vy * deltaTime;
                    std::cout << "  Entity " << entity << " moved to (" << pos->x << ", " << pos->y << ")" << std::endl;
                }
            }
        }
        
        // Système de rendu (spécifique client)
    auto* renderables = world.GetAllComponents<Renderable>();
        if (renderables) {
            std::cout << "  Rendering:" << std::endl;
            for (const auto& pair : *renderables) {
                ECS::EntityID entity = pair.first;
                auto* renderable = pair.second.get();
                auto* pos = world.GetComponent<Position>(entity);
                
                if (pos && renderable->visible) {
                    std::cout << "    " << renderable->texturePath 
                              << " at (" << pos->x << ", " << pos->y << ") "
                              << "layer=" << static_cast<int>(renderable->layer) << std::endl;
                }
            }
        }
>>>>>>> origin/dev
    }
    
    std::cout << "Graphics initialized successfully!" << std::endl;
    std::cout << "Controls: ESC to close window" << std::endl;
    
    // Create some test sprites
    Sprite testSprite;
    testSprite.SetPosition(400, 300); // Center of screen
    testSprite.SetScale(2.0f);
    
    // Test variables for animation
    float time = 0.0f;
    const float speed = 100.0f;
    
    // Main game loop
    while (graphics.IsRunning()) {
        // Handle events
        graphics.PollEvents();
        
        // Update
        time += 0.016f; // ~60 FPS
        
        // Move test objects
        float circleX = 400 + std::sin(time) * 200;
        float circleY = 300 + std::cos(time) * 100;
        
        // Render
        graphics.BeginFrame();
        
        // Draw some test primitives
        graphics.DrawRectangle(50, 50, 100, 50, 0xFF0000FF);  // Red rectangle
        graphics.DrawCircle(circleX, circleY, 30, 0x00FF00FF); // Green moving circle
        graphics.DrawRectangle(650, 500, 100, 50, 0x0000FFFF); // Blue rectangle
        
        // Draw grid
        for (int x = 0; x < 800; x += 100) {
            graphics.DrawRectangle(x, 0, 2, 600, 0x444444FF);
        }
        for (int y = 0; y < 600; y += 100) {
            graphics.DrawRectangle(0, y, 800, 2, 0x444444FF);
        }
        
        graphics.EndFrame();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "Graphics test finished!" << std::endl;
    return 0;
}

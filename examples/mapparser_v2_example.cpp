/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Example demonstrating the enhanced mapparser with tile types and parallax
*/

#include <iostream>
#include <mapparser.h>

void printTileInfo(const Tile& tile) {
    std::cout << "  Position: (" << tile.x << ", " << tile.y << ")\n";
    std::cout << "  Character: '" << tile.character << "'\n";
    std::cout << "  Type: " << tileTypeToString(tile.type) << "\n";
    std::cout << "  Sprite: " << tile.definition.sprite_path << "\n";
    
    // Print metadata
    if (!tile.definition.metadata.empty()) {
        std::cout << "  Metadata:\n";
        for (const auto& [key, value] : tile.definition.metadata) {
            std::cout << "    " << key << ": " << value << "\n";
        }
    }
    
    // Print script if present
    if (!tile.definition.script.empty()) {
        std::cout << "  Script:\n";
        std::cout << "--- SCRIPT BEGIN ---\n";
        std::cout << tile.definition.script;
        std::cout << "--- SCRIPT END ---\n";
    }
    
    std::cout << "\n";
}

int main() {
    try {
        std::cout << "=== MapParser 2.0 Example ===\n\n";
        
        // Get singleton instance
        auto& parser = MapParser::getInstance();
        
        // Load map from directory
        std::cout << "Loading map from assets/maps/space-corridor...\n";
        parser.loadFromDirectory("assets/maps/space-corridor");
        
        // Get map definition
        const auto& mapDef = parser.getMapDefinition();
        std::cout << "\n=== Map Definition ===\n";
        std::cout << "Description: " << mapDef.description << "\n";
        std::cout << "Dimensions: " << (mapDef.is_infinite ? "infinite" : std::to_string(mapDef.width)) 
                  << " x " << mapDef.height << "\n";
        std::cout << "Infinite: " << (mapDef.is_infinite ? "yes" : "no") << "\n\n";
        
        // Print parallax layers
        std::cout << "=== Parallax Layers (" << mapDef.parallax_layers.size() << ") ===\n";
        for (size_t i = 0; i < mapDef.parallax_layers.size(); ++i) {
            const auto& layer = mapDef.parallax_layers[i];
            std::cout << "Layer " << i << ":\n";
            std::cout << "  Texture: " << layer.texture_path << "\n";
            std::cout << "  Speed: " << layer.scroll_speed << "\n";
            std::cout << "  Depth: " << layer.depth << "\n";
            std::cout << "  Repeat: " << (layer.repeat_x ? "X" : "") 
                      << (layer.repeat_y ? "Y" : "") << "\n";
        }
        std::cout << "\n";
        
        // Print player spawns
        auto spawns = parser.getPlayerSpawns();
        std::cout << "=== Player Spawns (" << spawns.size() << ") ===\n";
        for (const auto& spawn : spawns) {
            printTileInfo(spawn);
        }
        
        // Print enemies by type
        auto basicEnemies = parser.getTilesByType(TileType::EnemyClassic);
        std::cout << "=== Classic Enemies (" << basicEnemies.size() << ") ===\n";
        for (const auto& enemy : basicEnemies) {
            printTileInfo(enemy);
        }
        
        auto eliteEnemies = parser.getTilesByType(TileType::EnemyElite);
        std::cout << "=== Elite Enemies (" << eliteEnemies.size() << ") ===\n";
        for (const auto& enemy : eliteEnemies) {
            printTileInfo(enemy);
        }
        
        auto bosses = parser.getTilesByType(TileType::EnemyBoss);
        std::cout << "=== Bosses (" << bosses.size() << ") ===\n";
        for (const auto& boss : bosses) {
            printTileInfo(boss);
        }
        
        // Print obstacles
        auto obstacles = parser.getTilesByType(TileType::Obstacle);
        std::cout << "=== Obstacles (" << obstacles.size() << ") ===\n";
        for (const auto& obstacle : obstacles) {
            printTileInfo(obstacle);
        }
        
        // Print background elements
        auto bgElements = parser.getTilesByType(TileType::BackgroundElement);
        std::cout << "=== Background Elements (" << bgElements.size() << ") ===\n";
        for (const auto& element : bgElements) {
            printTileInfo(element);
        }
        
        // Summary
        const auto& allTiles = parser.getTiles();
        std::cout << "=== Summary ===\n";
        std::cout << "Total tiles loaded: " << allTiles.size() << "\n";
        std::cout << "  Player Spawns: " << spawns.size() << "\n";
        std::cout << "  Classic Enemies: " << basicEnemies.size() << "\n";
        std::cout << "  Elite Enemies: " << eliteEnemies.size() << "\n";
        std::cout << "  Bosses: " << bosses.size() << "\n";
        std::cout << "  Obstacles: " << obstacles.size() << "\n";
        std::cout << "  Background: " << bgElements.size() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

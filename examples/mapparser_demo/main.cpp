/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MapParser interactive test program
*/

#include "mapparser.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <algorithm>

/**
 * @brief Print a separator line for visual clarity.
 */
void print_separator() {
    std::cout << std::string(80, '=') << std::endl;
}

/**
 * @brief Print a section header.
 * @param title The title of the section.
 */
void print_header(const std::string &title) {
    std::cout << "\n";
    print_separator();
    std::cout << "  " << title << std::endl;
    print_separator();
}

/**
 * @brief Print map definition details.
 * @param map_def The map definition to print.
 */
void print_map_definition(const MapDefinition &map_def) {
    std::cout << "\n[Map Metadata]" << std::endl;
    std::cout << "  Description: " << map_def.description << std::endl;
    std::cout << "  Dimensions:  " << (map_def.is_infinite ? "infinite" : std::to_string(map_def.width)) 
              << " x " << map_def.height << std::endl;
    std::cout << "  Type:        " << (map_def.is_infinite ? "Infinite (scrolling)" : "Finite (fixed)") << std::endl;
    std::cout << "  Width Value: " << map_def.width << " (-1 means infinite)" << std::endl;
    std::cout << "  Height:      " << map_def.height << std::endl;
    
    std::cout << "\n[Tile Definitions] (" << map_def.tile_mapping.size() << " total)" << std::endl;
    std::cout << std::left << std::setw(12) << "  Character" << "Asset Path" << std::endl;
    std::cout << "  " << std::string(70, '-') << std::endl;
    
    for (const auto &pair : map_def.tile_mapping) {
        std::cout << "  '" << pair.first << "'       -> " << pair.second << std::endl;
    }
}

/**
 * @brief Print tile details in a formatted table.
 * @param tiles Vector of tiles to print.
 * @param max_tiles Maximum number of tiles to print (0 = all).
 */
void print_tiles(const std::vector<Tile> &tiles, size_t max_tiles = 0) {
    std::cout << "\n[Loaded Tiles] (" << tiles.size() << " total)" << std::endl;
    
    if (tiles.empty()) {
        std::cout << "  No tiles loaded." << std::endl;
        return;
    }
    
    size_t count = (max_tiles > 0 && max_tiles < tiles.size()) ? max_tiles : tiles.size();
    
    std::cout << std::left 
              << "  " << std::setw(8) << "Index"
              << std::setw(6) << "X"
              << std::setw(6) << "Y"
              << std::setw(10) << "Char"
              << "Asset Path" << std::endl;
    std::cout << "  " << std::string(70, '-') << std::endl;
    
    for (size_t i = 0; i < count; i++) {
        const Tile &tile = tiles[i];
        std::cout << "  " << std::setw(8) << i
                  << std::setw(6) << tile.x
                  << std::setw(6) << tile.y
                  << "'" << tile.character << "'       "
                  << tile.asset_path << std::endl;
    }
    
    if (max_tiles > 0 && tiles.size() > max_tiles) {
        std::cout << "  ... (" << (tiles.size() - max_tiles) << " more tiles)" << std::endl;
    }
}

/**
 * @brief Print tile grid visualization (for small maps).
 * @param tiles Vector of tiles.
 * @param width Map width.
 * @param height Map height.
 */
void print_tile_grid(const std::vector<Tile> &tiles, int width, int height) {
    if (width <= 0 || height <= 0 || width > 50) {
        std::cout << "\n[Tile Grid] Skipped (map too large or infinite)" << std::endl;
        return;
    }
    
    std::cout << "\n[Tile Grid Visualization]" << std::endl;
    
    // Create a 2D grid
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));
    
    // Fill the grid with tile characters
    for (const Tile &tile : tiles) {
        if (tile.x >= 0 && tile.x < width && tile.y >= 0 && tile.y < height) {
            grid[tile.y][tile.x] = tile.character;
        }
    }
    
    // Print the grid with borders
    std::cout << "  +" << std::string(width * 2 + 1, '-') << "+" << std::endl;
    for (int y = 0; y < height; y++) {
        std::cout << "  | ";
        for (int x = 0; x < width; x++) {
            std::cout << grid[y][x] << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "  +" << std::string(width * 2 + 1, '-') << "+" << std::endl;
}

/**
 * @brief Print statistics about the loaded map.
 * @param tiles Vector of tiles.
 * @param map_def Map definition.
 */
void print_statistics(const std::vector<Tile> &tiles, const MapDefinition &map_def) {
    std::cout << "\n[Statistics]" << std::endl;
    std::cout << "  Total Tiles:        " << tiles.size() << std::endl;
    std::cout << "  Tile Definitions:   " << map_def.tile_mapping.size() << std::endl;
    std::cout << "  Map Type:           " << (map_def.is_infinite ? "Infinite" : "Finite") << std::endl;
    
    if (!tiles.empty()) {
        // Calculate bounding box
        int min_x = tiles[0].x, max_x = tiles[0].x;
        int min_y = tiles[0].y, max_y = tiles[0].y;
        
        for (const Tile &tile : tiles) {
            min_x = std::min(min_x, tile.x);
            max_x = std::max(max_x, tile.x);
            min_y = std::min(min_y, tile.y);
            max_y = std::max(max_y, tile.y);
        }
        
        std::cout << "  Bounding Box:       (" << min_x << "," << min_y << ") to (" 
                  << max_x << "," << max_y << ")" << std::endl;
        std::cout << "  Actual Dimensions:  " << (max_x - min_x + 1) << " x " 
                  << (max_y - min_y + 1) << std::endl;
    }
    
    // Count unique characters
    std::set<char> unique_chars;
    for (const Tile &tile : tiles) {
        unique_chars.insert(tile.character);
    }
    std::cout << "  Unique Characters:  " << unique_chars.size() << std::endl;
}

/**
 * @brief Load and display a map with full details.
 * @param map_path Path to the map directory.
 * @param show_all_tiles If true, show all tiles; otherwise limit to first 20.
 * @return 0 on success, 1 on failure.
 */
int load_and_display_map(const std::string &map_path, bool show_all_tiles = false) {
    print_header("Loading Map: " + map_path);
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        std::cout << "\nAttempting to load map from: " << map_path << std::endl;
        
        const std::vector<Tile> &tiles = parser.loadFromDirectory(map_path);
        
        std::cout << "✓ Map loaded successfully!" << std::endl;
        
        // Get map definition
        const MapDefinition &map_def = parser.getMapDefinition();
        
        // Print all details
        print_map_definition(map_def);
        print_tiles(tiles, show_all_tiles ? 0 : 20);
        print_tile_grid(tiles, map_def.width, map_def.height);
        print_statistics(tiles, map_def);
        
        return 0;
        
    } catch (const std::runtime_error &e) {
        std::cerr << "\n✗ Runtime Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::invalid_argument &e) {
        std::cerr << "\n✗ Invalid Argument: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "\n✗ Exception: " << e.what() << std::endl;
        return 1;
    }
}

/**
 * @brief Interactive menu for map selection.
 */
void interactive_mode() {
    print_header("MapParser Interactive Test Program");
    
    std::cout << "\nAvailable Maps:" << std::endl;
    std::cout << "  1. Demo Map (finite, mixed terrain)" << std::endl;
    std::cout << "  2. Welcome Map (finite, with default.def merging)" << std::endl;
    std::cout << "  3. Infinite Demo Map (infinite, with random groups)" << std::endl;
    std::cout << "  4. Custom Path" << std::endl;
    std::cout << "  5. Compare All Maps" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    
    while (true) {
        std::cout << "\nEnter your choice: ";
        int choice;
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        
        std::cin.ignore(10000, '\n');
        
        switch (choice) {
            case 0:
                std::cout << "\nExiting..." << std::endl;
                return;
                
            case 1:
                load_and_display_map("assets/maps/demo", false);
                break;
                
            case 2:
                load_and_display_map("assets/maps/welcome", true);
                break;
                
            case 3:
                load_and_display_map("assets/maps/infinite-demo", false);
                break;
                
            case 4: {
                std::cout << "Enter map directory path: ";
                std::string path;
                std::getline(std::cin, path);
                load_and_display_map(path, false);
                break;
            }
                
            case 5: {
                std::cout << "\n";
                load_and_display_map("assets/maps/demo", false);
                std::cout << "\n";
                load_and_display_map("assets/maps/welcome", false);
                std::cout << "\n";
                load_and_display_map("assets/maps/infinite-demo", false);
                break;
            }
                
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit code.
 */
int main(int argc, char *argv[]) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║           MapParser Library - Interactive Test Tool          ║
    ║                      R-Type Project                          ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Check if a map path was provided as argument
    if (argc >= 2) {
        std::string map_path = argv[1];
        bool show_all = (argc >= 3 && std::string(argv[2]) == "--all");
        
        std::cout << "Command-line mode: loading " << map_path << std::endl;
        int result = load_and_display_map(map_path, show_all);
        
        print_separator();
        return result;
    }
    
    // Otherwise, run interactive mode
    interactive_mode();
    
    print_separator();
    std::cout << "Thank you for using MapParser Test Tool!" << std::endl;
    
    return 0;
}

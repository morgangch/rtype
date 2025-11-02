/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MapParser library implementation for loading and parsing game maps
*/

#include "mapparser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cctype>
#include <string>

namespace fs = std::filesystem;

MapParser& MapParser::getInstance() {
    static MapParser instance;
    return instance;
}

MapParser::MapParser() : _map_loaded(false) {
}

MapParser::~MapParser() {
    clear();
}

void MapParser::clear() {
    _tiles.clear();
    _current_definition.tile_mapping.clear();
    _current_definition.parallax_layers.clear();
    _current_definition.width = 0;
    _current_definition.height = 0;
    _current_definition.description.clear();
    _current_definition.is_infinite = false;
    _map_loaded = false;
}

const std::vector<Tile>& MapParser::getTiles() const {
    return _tiles;
}

std::vector<Tile> MapParser::getTilesByType(TileType type) const {
    std::vector<Tile> filtered;
    for (const auto& tile : _tiles) {
        if (tile.type == type) {
            filtered.push_back(tile);
        }
    }
    return filtered;
}

std::vector<Tile> MapParser::getPlayerSpawns() const {
    return getTilesByType(TileType::PlayerSpawn);
}

const std::vector<ParallaxLayer>& MapParser::getParallaxLayers() const {
    return _current_definition.parallax_layers;
}

const MapDefinition& MapParser::getMapDefinition() const {
    return _current_definition;
}

bool MapParser::isMapLoaded() const {
    return _map_loaded;
}

std::string MapParser::trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void MapParser::validateLine(const std::string &line, int line_number) {
    for (signed char c : line) {
        if (c < 0) {
            throw std::invalid_argument(
                "Invalid character (non-ASCII) found at line " + 
                std::to_string(line_number) + 
                ". Only ASCII characters are allowed."
            );
        }
    }
}

void MapParser::validatePlayerSpawns() {
    auto spawns = getPlayerSpawns();
    if (spawns.size() < 1 || spawns.size() > 4) {
        throw std::invalid_argument(
            "Invalid number of player spawns: " + std::to_string(spawns.size()) + 
            ". Must be between 1 and 4."
        );
    }
}

void MapParser::loadDefaultDefinitions(MapDefinition &map_def) {
    const std::string default_def_path = DEFAULT_DEF_PATH;
    
    if (!fs::exists(default_def_path)) {
        return; // Default definitions are optional
    }
    
    try {
        parseDefFile(default_def_path, map_def, true);
    } catch (const std::exception &e) {
        // If default.def exists but is invalid, we should know about it
        throw std::runtime_error(
            "Failed to load default definitions from " + 
            default_def_path + ": " + e.what()
        );
    }
}

void MapParser::mergeDefinitions(MapDefinition &map_def, const MapDefinition &default_def) {
    // Map-specific definitions override defaults
    for (const auto &pair : default_def.tile_mapping) {
        if (map_def.tile_mapping.find(pair.first) == map_def.tile_mapping.end()) {
            map_def.tile_mapping[pair.first] = pair.second;
        }
    }
    
    // Merge parallax layers (defaults first, then map-specific)
    if (map_def.parallax_layers.empty() && !default_def.parallax_layers.empty()) {
        map_def.parallax_layers = default_def.parallax_layers;
    }
}

TileDefinition MapParser::parseTileFile(const std::string &filepath) {
    TileDefinition tile_def;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open .tile file: " + filepath);
    }
    
    std::string line;
    int line_number = 0;
    bool in_script_block = false;
    std::ostringstream script_content;
    
    while (std::getline(file, line)) {
        line_number++;
        validateLine(line, line_number);
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.rfind("///", 0) == 0) {
            continue;
        }
        
        // Handle script blocks
        if (line == "SCRIPT_BEGIN") {
            in_script_block = true;
            continue;
        }
        
        if (line == "SCRIPT_END") {
            in_script_block = false;
            tile_def.script = script_content.str();
            continue;
        }
        
        if (in_script_block) {
            script_content << line << "\n";
            continue;
        }
        
        // Parse key-value pairs
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            throw std::invalid_argument(
                "Invalid line format at line " + std::to_string(line_number) + 
                " in " + filepath + ". Expected 'key: value'"
            );
        }
        
        std::string key = trim(line.substr(0, colon_pos));
        std::string value = trim(line.substr(colon_pos + 1));
        
        if (key == "sprite") {
            tile_def.sprite_path = value;
        } else if (key == "shape") {
            // Parse shape type
            if (value == "circle") {
                tile_def.shape_type = ShapeType::Circle;
            } else if (value == "rectangle") {
                tile_def.shape_type = ShapeType::Rectangle;
            } else if (value == "particles") {
                tile_def.shape_type = ShapeType::Particles;
            } else {
                tile_def.shape_type = ShapeType::None;
            }
        } else {
            tile_def.metadata[key] = value;
        }
    }
    
    file.close();
    return tile_def;
}

void MapParser::parseDefFile(const std::string &filepath, MapDefinition &map_def, bool is_default) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open .def file: " + filepath);
    }
    
    std::string line;
    int line_number = 0;
    bool in_parallax_section = false;
    
    // Line 1: dimensions (width,height or inf,height)
    if (!std::getline(file, line)) {
        throw std::invalid_argument("Empty .def file: " + filepath);
    }
    line_number++;
    validateLine(line, line_number);
    line = trim(line);
    
    size_t comma_pos = line.find(',');
    if (comma_pos == std::string::npos) {
        throw std::invalid_argument(
            "Invalid dimensions format at line 1 in " + filepath + 
            ". Expected: 'width,height' or 'inf,height'"
        );
    }
    
    std::string width_str = trim(line.substr(0, comma_pos));
    std::string height_str = trim(line.substr(comma_pos + 1));
    
    if (width_str == "inf") {
        map_def.is_infinite = true;
        map_def.width = -1;
    } else {
        map_def.is_infinite = false;
        try {
            map_def.width = std::stoi(width_str);
            if (map_def.width <= 0) {
                throw std::invalid_argument("Width must be positive");
            }
        } catch (const std::exception &e) {
            throw std::invalid_argument(
                "Invalid width value at line 1 in " + filepath + ": " + width_str
            );
        }
    }
    
    try {
        map_def.height = std::stoi(height_str);
        if (map_def.height <= 0) {
            throw std::invalid_argument("Height must be positive");
        }
    } catch (const std::exception &e) {
        throw std::invalid_argument(
            "Invalid height value at line 1 in " + filepath + ": " + height_str
        );
    }
    
    // Line 2: description
    if (!std::getline(file, line)) {
        throw std::invalid_argument("Missing description at line 2 in " + filepath);
    }
    line_number++;
    validateLine(line, line_number);
    map_def.description = trim(line);
    
    // Remaining lines: tile definitions, parallax layers, etc.
    while (std::getline(file, line)) {
        line_number++;
        validateLine(line, line_number);
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.rfind("///", 0) == 0) {
            continue;
        }
        
        // Check for section markers
        if (line == "PARALLAX_BEGIN") {
            in_parallax_section = true;
            continue;
        }
        
        if (line == "PARALLAX_END") {
            in_parallax_section = false;
            continue;
        }
        
        // Parse parallax layers
        if (in_parallax_section) {
            // Format: texture_path speed depth repeat_x repeat_y
            std::istringstream iss(line);
            ParallaxLayer layer;
            std::string repeat_x_str, repeat_y_str;
            
            if (!(iss >> layer.texture_path >> layer.scroll_speed >> layer.depth >> repeat_x_str >> repeat_y_str)) {
                throw std::invalid_argument(
                    "Invalid parallax layer format at line " + std::to_string(line_number) + 
                    " in " + filepath + ". Expected: 'texture_path speed depth repeat_x repeat_y'"
                );
            }
            
            layer.repeat_x = (repeat_x_str == "true" || repeat_x_str == "1");
            layer.repeat_y = (repeat_y_str == "true" || repeat_y_str == "1");
            
            map_def.parallax_layers.push_back(layer);
            continue;
        }
        
        // Parse tile definitions: character TileType tile_path
        std::istringstream iss(line);
        std::string char_str, type_str, tile_path;
        
        if (!(iss >> char_str >> type_str >> tile_path)) {
            throw std::invalid_argument(
                "Invalid tile definition at line " + std::to_string(line_number) + 
                " in " + filepath + ". Expected format: 'character TileType tile_path'"
            );
        }
        
        if (char_str.length() != 1) {
            throw std::invalid_argument(
                "Tile character must be a single character at line " + 
                std::to_string(line_number) + " in " + filepath
            );
        }
        
        char tile_char = char_str[0];
        TileType type = stringToTileType(type_str);
        
        if (type == TileType::Unknown) {
            throw std::invalid_argument(
                "Invalid TileType '" + type_str + "' at line " + 
                std::to_string(line_number) + " in " + filepath
            );
        }
        
        if (tile_path.empty()) {
            throw std::invalid_argument(
                "Empty tile path at line " + std::to_string(line_number) + 
                " in " + filepath
            );
        }
        
        // Check for duplicate character definitions
        if (map_def.tile_mapping.find(tile_char) != map_def.tile_mapping.end()) {
            if (!is_default) {
                throw std::invalid_argument(
                    "Duplicate tile character '" + std::string(1, tile_char) + 
                    "' at line " + std::to_string(line_number) + " in " + filepath
                );
            }
        }
        
        map_def.tile_mapping[tile_char] = TileMapping(type, tile_path);
    }
    
    file.close();
}

char MapParser::selectRandomTile(const std::vector<char> &candidates) {
    if (candidates.empty()) {
        throw std::runtime_error("Cannot select from empty candidate list");
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, candidates.size() - 1);
    
    return candidates[dis(gen)];
}

void MapParser::processTileCharacter(char c, int x, int y, const MapDefinition &map_def,
                                     bool in_random_group, const std::vector<char> &random_candidates) {
    char actual_char = c;
    
    if (in_random_group) {
        actual_char = selectRandomTile(random_candidates);
    }
    
    // Look up the tile mapping for this character
    auto it = map_def.tile_mapping.find(actual_char);
    if (it != map_def.tile_mapping.end()) {
        Tile tile;
        tile.x = x;
        tile.y = y;
        tile.character = actual_char;
        tile.type = it->second.type;
        
        // Load the .tile definition file
        try {
            tile.definition = parseTileFile(it->second.tile_path);
        } catch (const std::exception &e) {
            throw std::runtime_error(
                "Failed to load tile definition from '" + it->second.tile_path + 
                "' for character '" + std::string(1, actual_char) + "': " + e.what()
            );
        }
        
        _tiles.push_back(tile);
    }
    // If character not found in mapping, it's treated as empty (no tile created)
}

void MapParser::parseMapFile(const std::string &filepath, const MapDefinition &map_def) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open .map file: " + filepath);
    }
    
    std::string line;
    int line_number = 0;
    int y = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        validateLine(line, line_number);
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.rfind("///", 0) == 0) {
            continue;
        }
        
        // Parse tiles with spaces as separators
        int x = 0;
        bool in_random_group = false;
        std::vector<char> random_candidates;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            // Handle random groups [...]
            if (c == '[') {
                in_random_group = true;
                random_candidates.clear();
                continue;
            }
            
            if (c == ']') {
                in_random_group = false;
                if (!random_candidates.empty()) {
                    processTileCharacter('\0', x, y, map_def, true, random_candidates);
                    x++;
                }
                continue;
            }
            
            if (in_random_group) {
                if (c != ' ') {
                    random_candidates.push_back(c);
                }
                continue;
            }
            
            // Skip spaces outside of random groups
            if (c == ' ') {
                continue;
            }
            
            // Regular tile character
            processTileCharacter(c, x, y, map_def, false, {});
            x++;
        }
        
        // Validate width for finite maps
        if (!map_def.is_infinite && x != map_def.width) {
            throw std::invalid_argument(
                "Map width mismatch at line " + std::to_string(line_number) + 
                " in " + filepath + ". Expected " + std::to_string(map_def.width) + 
                " tiles, found " + std::to_string(x)
            );
        }
        
        y++;
    }
    
    // Validate height
    if (y != map_def.height) {
        throw std::invalid_argument(
            "Map height mismatch in " + filepath + 
            ". Expected " + std::to_string(map_def.height) + 
            " rows, found " + std::to_string(y)
        );
    }
    
    file.close();
}

const std::vector<Tile>& MapParser::loadFromDirectory(const std::string &dirname) {
    // Clear previous map data
    clear();
    
    // Check if directory exists
    fs::path dir_path(dirname);
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        throw std::runtime_error("Directory not found: " + dirname);
    }
    
    // Find .def and .map files
    std::string def_file;
    std::string map_file;
    
    for (const auto &entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".def") {
                def_file = entry.path().string();
            } else if (ext == ".map") {
                map_file = entry.path().string();
            }
        }
    }
    
    // Validate that both files exist
    if (def_file.empty()) {
        throw std::runtime_error("No .def file found in directory: " + dirname);
    }
    if (map_file.empty()) {
        throw std::runtime_error("No .map file found in directory: " + dirname);
    }
    
    // Load default definitions first
    MapDefinition default_def;
    loadDefaultDefinitions(default_def);
    
    // Parse the map-specific .def file
    parseDefFile(def_file, _current_definition, false);
    
    // Merge with defaults (defaults fill in missing mappings)
    mergeDefinitions(_current_definition, default_def);
    
    // Parse the .map file
    parseMapFile(map_file, _current_definition);
    
    // Validate player spawns
    validatePlayerSpawns();
    
    _map_loaded = true;
    
    return _tiles;
}

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
    _current_definition.width = 0;
    _current_definition.height = 0;
    _current_definition.description.clear();
    _current_definition.is_infinite = false;
    _map_loaded = false;
}

const std::vector<Tile>& MapParser::getTiles() const {
    return _tiles;
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
}

void MapParser::parseDefFile(const std::string &filepath, MapDefinition &map_def, bool is_default) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open .def file: " + filepath);
    }
    
    std::string line;
    int line_number = 0;
    
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
    
    // Remaining lines: tile definitions (character asset_path)
    while (std::getline(file, line)) {
        line_number++;
        validateLine(line, line_number);
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.rfind("///", 0) == 0) {
            continue;
        }
        
        // Find first space to separate character from asset path
        size_t space_pos = line.find(' ');
        if (space_pos == std::string::npos || space_pos == 0) {
            throw std::invalid_argument(
                "Invalid tile definition at line " + std::to_string(line_number) + 
                " in " + filepath + ". Expected format: 'character asset_path'"
            );
        }
        
        std::string char_str = line.substr(0, space_pos);
        std::string asset_path = trim(line.substr(space_pos + 1));
        
        if (char_str.length() != 1) {
            throw std::invalid_argument(
                "Tile character must be a single character at line " + 
                std::to_string(line_number) + " in " + filepath
            );
        }
        
        char tile_char = char_str[0];
        
        if (asset_path.empty()) {
            throw std::invalid_argument(
                "Empty asset path at line " + std::to_string(line_number) + 
                " in " + filepath
            );
        }
        
        map_def.tile_mapping[tile_char] = asset_path;
    }
    
    file.close();
}

char MapParser::selectRandomTile(const std::vector<char> &candidates) {
    if (candidates.empty()) {
        throw std::runtime_error("Cannot select random tile from empty candidates list");
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
    
    // Look up the asset path for this character
    auto it = map_def.tile_mapping.find(actual_char);
    if (it != map_def.tile_mapping.end()) {
        Tile tile;
        tile.x = x;
        tile.y = y;
        tile.asset_path = it->second;
        tile.character = actual_char;
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
        
        // Skip comments
        if (trim(line).rfind("///", 0) == 0) {
            continue;
        }
        
        // Check for tabs (not allowed)
        if (line.find('\t') != std::string::npos) {
            throw std::invalid_argument(
                "Tab characters are not allowed at line " + 
                std::to_string(line_number) + " in " + filepath + 
                ". Use spaces only."
            );
        }
        
        int x = 0;
        size_t i = 0;
        
        while (i < line.length()) {
            char c = line[i];
            
            // Skip spaces
            if (c == ' ') {
                i++;
                continue;
            }
            
            // Handle random groups [...]
            if (c == '[') {
                size_t closing_bracket = line.find(']', i);
                if (closing_bracket == std::string::npos) {
                    throw std::invalid_argument(
                        "Unclosed random group '[' at line " + 
                        std::to_string(line_number) + ", column " + 
                        std::to_string(i + 1) + " in " + filepath
                    );
                }
                
                // Extract characters between brackets
                std::vector<char> candidates;
                for (size_t j = i + 1; j < closing_bracket; j++) {
                    if (line[j] != ' ') {
                        candidates.push_back(line[j]);
                    }
                }
                
                if (candidates.empty()) {
                    throw std::invalid_argument(
                        "Empty random group '[]' at line " + 
                        std::to_string(line_number) + " in " + filepath
                    );
                }
                
                processTileCharacter(candidates[0], x, y, map_def, true, candidates);
                
                i = closing_bracket + 1;
                x++;
            } else {
                // Regular character
                std::vector<char> dummy;
                processTileCharacter(c, x, y, map_def, false, dummy);
                i++;
                x++;
            }
        }
        
        // Validate width for finite maps
        if (!map_def.is_infinite && x > 0 && x != map_def.width) {
            throw std::invalid_argument(
                "Line " + std::to_string(line_number) + " has " + 
                std::to_string(x) + " tiles, but map width is " + 
                std::to_string(map_def.width) + " in " + filepath
            );
        }
        
        if (x > 0) {
            y++;
        }
    }
    
    // Validate height
    if (y != map_def.height) {
        throw std::invalid_argument(
            "Map has " + std::to_string(y) + " rows, but height is " + 
            std::to_string(map_def.height) + " in " + filepath
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
        throw std::runtime_error("Directory does not exist: " + dirname);
    }
    
    // Find .def and .map files
    std::string def_file;
    std::string map_file;
    
    for (const auto &entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string extension = entry.path().extension().string();
            
            if (extension == ".def") {
                def_file = entry.path().string();
            } else if (extension == ".map") {
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
    
    _map_loaded = true;
    
    return _tiles;
}

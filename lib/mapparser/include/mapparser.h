/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MapParser library for loading and parsing game maps
*/

/**
 * @file mapparser.h
 * @brief Defines the map parser for loading .def and .map files.
 */

#ifndef MAPPARSER_H
#define MAPPARSER_H

#include <vector>
#include <map>
#include <string>
#include <stdexcept>

#define DEFAULT_DEF_PATH "assets/maps/default.def"

/**
 * @enum TileType
 * @brief Defines the different types of tiles that can be placed on a map.
 */
enum class TileType {
    PlayerSpawn,        /**< Player spawn point (max 1-4 per map) */
    EnemyClassic,       /**< Classic enemy spawn point */
    EnemyElite,         /**< Elite enemy spawn point */
    EnemyBoss,          /**< Boss enemy spawn point */
    Obstacle,           /**< Static or moving obstacle */
    BackgroundElement,  /**< Decorative background element (debris, etc.) */
    Unknown             /**< Unknown or invalid type */
};

/**
 * @brief Convert string to TileType enum
 * @param str String representation of tile type
 * @return Corresponding TileType enum value
 */
inline TileType stringToTileType(const std::string& str) {
    if (str == "PlayerSpawn") return TileType::PlayerSpawn;
    if (str == "EnemyClassic") return TileType::EnemyClassic;
    if (str == "EnemyElite") return TileType::EnemyElite;
    if (str == "EnemyBoss") return TileType::EnemyBoss;
    if (str == "Obstacle") return TileType::Obstacle;
    if (str == "BackgroundElement") return TileType::BackgroundElement;
    return TileType::Unknown;
}

/**
 * @brief Convert TileType enum to string
 * @param type TileType enum value
 * @return String representation
 */
inline std::string tileTypeToString(TileType type) {
    switch (type) {
        case TileType::PlayerSpawn: return "PlayerSpawn";
        case TileType::EnemyClassic: return "EnemyClassic";
        case TileType::EnemyElite: return "EnemyElite";
        case TileType::EnemyBoss: return "EnemyBoss";
        case TileType::Obstacle: return "Obstacle";
        case TileType::BackgroundElement: return "BackgroundElement";
        default: return "Unknown";
    }
}

/**
 * @enum ShapeType
 * @brief Defines the type of procedural shape for tiles without sprites
 */
enum class ShapeType {
    None,           /**< No shape (sprite-based tile) */
    Circle,         /**< Circular shape */
    Rectangle,      /**< Rectangular shape */
    Particles       /**< Multiple procedurally generated particles */
};

/**
 * @struct TileDefinition
 * @brief Contains the full definition of a tile from a .tile file
 */
struct TileDefinition {
    std::string sprite_path;                    /**< Path to the sprite/texture (empty for shape-based tiles) */
    std::string script;                         /**< AI/movement script (server-interpreted) */
    std::map<std::string, std::string> metadata; /**< Additional key-value metadata */
    ShapeType shape_type;                       /**< Type of procedural shape (for non-sprite tiles) */
    
    TileDefinition() : shape_type(ShapeType::None) {}
};

/**
 * @struct ParallaxLayer
 * @brief Defines a single parallax background layer
 */
struct ParallaxLayer {
    std::string texture_path;   /**< Path to the layer texture */
    float scroll_speed;         /**< Scroll speed multiplier (1.0 = normal speed) */
    float depth;                /**< Depth value for parallax effect (0.0 = far, 1.0 = near) */
    bool repeat_x;              /**< Whether to repeat horizontally */
    bool repeat_y;              /**< Whether to repeat vertically */
    
    ParallaxLayer() : scroll_speed(1.0f), depth(0.5f), repeat_x(true), repeat_y(false) {}
};

/**
 * @struct Tile
 * @brief Represents a single tile instance in the map with its position and properties.
 */
struct Tile {
    int x;                      /**< X coordinate of the tile */
    int y;                      /**< Y coordinate of the tile */
    char character;             /**< Original character from the map file */
    TileType type;              /**< Type of tile (spawn, enemy, obstacle, etc.) */
    TileDefinition definition;  /**< Full tile definition loaded from .tile file */
};

/**
 * @struct TileMapping
 * @brief Maps a character to its tile type and definition file
 */
struct TileMapping {
    TileType type;              /**< Type of the tile */
    std::string tile_path;      /**< Path to the .tile definition file */
    
    TileMapping() : type(TileType::Unknown) {}
    TileMapping(TileType t, const std::string& path) : type(t), tile_path(path) {}
};

/**
 * @struct MapDefinition
 * @brief Holds metadata and tile definitions from a .def file.
 */
struct MapDefinition {
    int width;                                  /**< Map width (-1 for infinite) */
    int height;                                 /**< Map height */
    std::string description;                    /**< Map description */
    std::map<char, TileMapping> tile_mapping;   /**< Character to tile mapping */
    std::vector<ParallaxLayer> parallax_layers; /**< Parallax background layers */
    bool is_infinite;                           /**< True if map width is infinite */
};

/**
 * @class MapParser
 * @brief Singleton class for parsing and loading game maps from .def and .map files.
 * 
 * This class handles loading map definitions and layouts according to the MAPS_STANDARD.md
 * specification. It supports both finite and infinite maps, with random tile selection
 * using the [chars] syntax for infinite maps.
 * 
 * The parser first loads default tile definitions from assets/maps/default.def (if present),
 * then overrides with map-specific definitions.
 * 
 * @note This is a singleton class. Use getInstance() to get the instance.
 */
class MapParser {
public:
    /**
     * @brief Get the singleton instance of MapParser.
     * @return Reference to the MapParser singleton instance.
     */
    static MapParser& getInstance();

    /**
     * @brief Delete copy constructor.
     */
    MapParser(const MapParser&) = delete;
    
    /**
     * @brief Delete move constructor.
     */
    MapParser(MapParser&&) = delete;
    
    /**
     * @brief Delete copy assignment operator.
     */
    MapParser& operator=(const MapParser&) = delete;
    
    /**
     * @brief Delete move assignment operator.
     */
    MapParser& operator=(MapParser&&) = delete;

    /**
     * @brief Load a map from a directory containing .def and .map files.
     * 
     * The directory must contain both a .def and .map file with matching names.
     * The method first loads default.def from assets/maps/, then loads and merges
     * the map-specific .def file, and finally parses the .map file.
     * 
     * @param dirname Path to the directory containing the map files.
     * @return Const reference to the vector of loaded tiles.
     * @throws std::runtime_error If directory doesn't exist, files are missing, or parsing fails.
     * @throws std::invalid_argument If file format is invalid.
     */
    const std::vector<Tile>& loadFromDirectory(const std::string &dirname);

    /**
     * @brief Get the current map definition.
     * @return Const reference to the current MapDefinition.
     */
    const MapDefinition& getMapDefinition() const;

    /**
     * @brief Get the loaded tiles.
     * @return Const reference to the vector of loaded tiles.
     */
    const std::vector<Tile>& getTiles() const;

    /**
     * @brief Get tiles filtered by type.
     * @param type TileType to filter by.
     * @return Vector of tiles matching the specified type.
     */
    std::vector<Tile> getTilesByType(TileType type) const;

    /**
     * @brief Get player spawn tiles (should be 1-4 per map).
     * @return Vector of player spawn tiles.
     */
    std::vector<Tile> getPlayerSpawns() const;

    /**
     * @brief Get the parallax background layers.
     * @return Const reference to the vector of parallax layers.
     */
    const std::vector<ParallaxLayer>& getParallaxLayers() const;

    /**
     * @brief Check if a map is currently loaded.
     * @return true if a map is loaded, false otherwise.
     */
    bool isMapLoaded() const;

    /**
     * @brief Clear the currently loaded map and reset the parser state.
     */
    void clear();

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    MapParser();

    /**
     * @brief Destructor.
     */
    ~MapParser();

    /**
     * @brief Parse a .def file and populate the map definition.
     * @param filepath Path to the .def file.
     * @param map_def Reference to MapDefinition to populate.
     * @param is_default True if parsing default.def, false otherwise.
     * @throws std::runtime_error If file cannot be opened.
     * @throws std::invalid_argument If file format is invalid.
     */
    void parseDefFile(const std::string &filepath, MapDefinition &map_def, bool is_default);

    /**
     * @brief Parse a .tile file and populate a TileDefinition.
     * @param filepath Path to the .tile file.
     * @return Populated TileDefinition structure.
     * @throws std::runtime_error If file cannot be opened.
     * @throws std::invalid_argument If file format is invalid.
     */
    TileDefinition parseTileFile(const std::string &filepath);

    /**
     * @brief Parse a .map file and generate tiles based on the definition.
     * @param filepath Path to the .map file.
     * @param map_def The map definition to use for tile lookup.
     * @throws std::runtime_error If file cannot be opened.
     * @throws std::invalid_argument If map format is invalid or dimensions don't match.
     */
    void parseMapFile(const std::string &filepath, const MapDefinition &map_def);

    /**
     * @brief Validate player spawn count (must be 1-4).
     * @throws std::invalid_argument If player spawn count is invalid.
     */
    void validatePlayerSpawns();

    /**
     * @brief Load default tile definitions from assets/maps/default.def.
     * @param map_def Reference to MapDefinition to populate with defaults.
     */
    void loadDefaultDefinitions(MapDefinition &map_def);

    /**
     * @brief Merge map-specific definitions with default definitions.
     * @param map_def Map definition to merge into (will be modified).
     * @param default_def Default definition to merge from.
     */
    void mergeDefinitions(MapDefinition &map_def, const MapDefinition &default_def);

    /**
     * @brief Process a tile character and add it to the tiles vector.
     * @param c The character to process (may be part of a random group).
     * @param x X coordinate of the tile.
     * @param y Y coordinate of the tile.
     * @param map_def The map definition for tile lookup.
     * @param in_random_group True if character is part of a [...] group.
     * @param random_candidates Vector of candidate characters for random selection.
     */
    void processTileCharacter(char c, int x, int y, const MapDefinition &map_def, 
                              bool in_random_group, const std::vector<char> &random_candidates);

    /**
     * @brief Select a random character from a group.
     * @param candidates Vector of candidate characters.
     * @return Randomly selected character.
     */
    char selectRandomTile(const std::vector<char> &candidates);

    /**
     * @brief Validate that a line contains only ASCII characters and allowed symbols.
     * @param line The line to validate.
     * @param line_number Line number for error reporting.
     * @throws std::invalid_argument If line contains invalid characters.
     */
    void validateLine(const std::string &line, int line_number);

    /**
     * @brief Trim whitespace from both ends of a string.
     * @param str String to trim.
     * @return Trimmed string.
     */
    std::string trim(const std::string &str);

    MapDefinition _current_definition;  /**< Current loaded map definition */
    std::vector<Tile> _tiles;           /**< Current loaded tiles */
    bool _map_loaded;                   /**< Flag indicating if a map is loaded */
};

#endif //MAPPARSER_H

/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef MAPPARSER_H
#define MAPPARSER_H

#include <vector>
#include <memory>
#include <string>

struct Tile {
    int x;
    int y;
    std::string tileType;
};

class MapParser {
public:
    MapParser();
    ~MapParser(); // Add destructor declaration
    
    /**
     * @brief Load a map from a file.
     * @param filename The name of the file to load the map from.
     * @return true if the map was loaded successfully, false otherwise.
     */
    virtual const std::vector<Tile>& loadFromFile(const std::string &filename);

    /** 
     * @brief Load a map from a directory name.
     * @param dirname The name of the directory to load the map from.
     * @return List of "tile" objects representing the map. 
     */
    virtual const std::vector<Tile>& loadFromDirectory(const std::string &dirname);

    /**
     * @brief Clean up the map parser.
     */
    void clean();
    
    /**
     * @brief Register a new tile type.
     * @param tileType The name of the tile type to register.
     */
    void registerTileType(const std::string &tileType);

    /**
     * @brief Unregister a tile type.
     * @param tileType The name of the tile type to unregister.
     */
    void unregisterTileType(const std::string &tileType);

    /**
     * @brief Clear all registered tile types.
     */
    void clearTileTypes();

    /**
     * @brief Check if a tile type is registered.
     * @param tileType The name of the tile type to check.
     * @return true if the tile type is registered, false otherwise.
     */
    bool hasTileType(const std::string &tileType) const;

private:
    std::vector<std::string> _tileTypes;
};

#endif //MAPPARSER_H

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

struct Tile {
    int x;
    int y;
    const std::string tileType;
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
    bool loadFromFile(const std::string &filename);

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
    std::vector<std::string> m_tileTypes;
    std::vector<Tile> m_tiles;
    /**
     * @brief Add a tile to the map.
     * @param tile The tile to add.
     */
    void addTile(const Tile &tile);

};

#endif //MAPPARSER_H

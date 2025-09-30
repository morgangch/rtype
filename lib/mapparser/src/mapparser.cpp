/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "mapparser.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <algorithm>

MapParser::MapParser() {
}

// Add destructor to clean up memory
MapParser::~MapParser() {
    clean();
}

void MapParser::clean() {
    _tileTypes.clear();
}

const std::vector<Tile>& MapParser::loadFromFile(const std::string &filename) {
    return {};
}

const std::vector<Tile>& MapParser::loadFromDirectory(const std::string &dirname) {
    return {};
}

void MapParser::registerTileType(const std::string &tileType) {
    if (!hasTileType(tileType)) {
        _tileTypes.push_back(tileType);
    }
}

void MapParser::unregisterTileType(const std::string &tileType) {
    auto it = std::find(_tileTypes.begin(), _tileTypes.end(), tileType);
    if (it != _tileTypes.end()) {
        _tileTypes.erase(it);
    }
}

void MapParser::clearTileTypes() {
    _tileTypes.clear();
}

bool MapParser::hasTileType(const std::string &tileType) const {
    return std::find(_tileTypes.begin(), _tileTypes.end(), tileType) != _tileTypes.end();
}

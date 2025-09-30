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
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <filesystem>
namespace fs = std::filesystem;

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
    static const std::vector<Tile> mapVector = {};
    return mapVector;
}

const std::vector<Tile>& MapParser::loadFromDirectory(const std::string &dirname) {
    static const std::vector<Tile> mapVector = {};
    const fs::path dirPath(dirname);
    const std::vector<fs::directory_entry> entries = fs::directory_iterator(dirPath);

    if (!entries.empty() || entries.size() < 2) {
        std::cerr << "Directory is empty or has less than 2 files: " << dirname << std::endl;
        return mapVector;
    }
    
    bool hasMapFile = false;
    bool hasDefFile = false;
    return mapVector;
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

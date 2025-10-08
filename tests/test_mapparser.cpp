/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Unit tests for MapParser library
*/

#include "mapparser.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

/**
 * @brief Helper function to create a temporary test directory.
 * @param dir_name Name of the directory to create.
 * @return Path to the created directory.
 */
std::string create_test_dir(const std::string &dir_name) {
    fs::path temp_dir = fs::temp_directory_path() / dir_name;
    fs::create_directories(temp_dir);
    return temp_dir.string();
}

/**
 * @brief Helper function to clean up a temporary test directory.
 * @param dir_path Path to the directory to remove.
 */
void cleanup_test_dir(const std::string &dir_path) {
    if (fs::exists(dir_path)) {
        fs::remove_all(dir_path);
    }
}

/**
 * @brief Helper function to create a test file with content.
 * @param filepath Path to the file to create.
 * @param content Content to write to the file.
 */
void create_test_file(const std::string &filepath, const std::string &content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create test file: " + filepath);
    }
    file << content;
    file.close();
}

// ====================
// Test 1: Singleton Pattern
// ====================
int test_singleton_pattern() {
    std::cout << "[TEST] Singleton pattern..." << std::endl;
    
    // Get two references to the instance
    MapParser &parser1 = MapParser::getInstance();
    MapParser &parser2 = MapParser::getInstance();
    
    // They should be the same instance
    assert(&parser1 == &parser2);
    
    std::cout << "  PASS: getInstance returns same instance" << std::endl;
    return 0;
}

// ====================
// Test 2: Initial State
// ====================
int test_initial_state() {
    std::cout << "[TEST] Initial state..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    assert(!parser.isMapLoaded());
    assert(parser.getTiles().empty());
    
    std::cout << "  PASS: Initial state is correct" << std::endl;
    return 0;
}

// ====================
// Test 3: Valid Finite Map Loading
// ====================
int test_valid_finite_map() {
    std::cout << "[TEST] Valid finite map loading..." << std::endl;
    
    // Use the existing demo map (relative to project root)
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        const std::vector<Tile> &tiles = parser.loadFromDirectory("assets/maps/demo");
        
        assert(parser.isMapLoaded());
        assert(!tiles.empty());
        
        const MapDefinition &map_def = parser.getMapDefinition();
        assert(map_def.width == 12);
        assert(map_def.height == 7);
        assert(!map_def.is_infinite);
        assert(!map_def.description.empty());
        
        // Check that some tiles were loaded
        assert(tiles.size() > 0);
        
        std::cout << "  PASS: Loaded " << tiles.size() << " tiles from demo map" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ====================
// Test 4: Valid Infinite Map Loading
// ====================
int test_valid_infinite_map() {
    std::cout << "[TEST] Valid infinite map loading..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        const std::vector<Tile> &tiles = parser.loadFromDirectory("assets/maps/infinite-demo");
        
        assert(parser.isMapLoaded());
        
        const MapDefinition &map_def = parser.getMapDefinition();
        assert(map_def.width == -1);
        assert(map_def.height == 6);
        assert(map_def.is_infinite);
        
        std::cout << "  PASS: Loaded infinite map with " << tiles.size() << " tiles" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ====================
// Test 5: Welcome Map (with default.def merging)
// ====================
int test_welcome_map() {
    std::cout << "[TEST] Welcome map with default.def merging..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        (void)parser.loadFromDirectory("assets/maps/welcome");
        assert(parser.isMapLoaded());
        const MapDefinition &map_def = parser.getMapDefinition();
        assert(map_def.width == 10);
        assert(map_def.height == 6);
        assert(!map_def.is_infinite);
        
        // Check that tile mappings include both custom and default definitions
        assert(!map_def.tile_mapping.empty());
        
        std::cout << "  PASS: Welcome map loaded with " 
                  << map_def.tile_mapping.size() << " tile definitions" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ====================
// Test 6: Clear Functionality
// ====================
int test_clear_functionality() {
    std::cout << "[TEST] Clear functionality..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        // Load a map
        parser.loadFromDirectory("assets/maps/welcome");
        assert(parser.isMapLoaded());
        assert(!parser.getTiles().empty());
        
        // Clear it
        parser.clear();
        assert(!parser.isMapLoaded());
        assert(parser.getTiles().empty());
        
        std::cout << "  PASS: Clear resets state correctly" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ====================
// Test 7: Tile Structure Validation
// ====================
int test_tile_structure() {
    std::cout << "[TEST] Tile structure validation..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    try {
        const std::vector<Tile> &tiles = parser.loadFromDirectory("assets/maps/demo");
        
        // Check that tiles have valid coordinates and data
        for (const Tile &tile : tiles) {
            assert(tile.x >= 0);
            assert(tile.y >= 0);
            assert(!tile.asset_path.empty());
            assert(tile.character != '\0');
        }
        
        std::cout << "  PASS: All tiles have valid structure" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ====================
// Test 8: Missing Directory Error
// ====================
int test_missing_directory() {
    std::cout << "[TEST] Missing directory error handling..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    parser.clear();
    
    bool caught_exception = false;
    try {
        parser.loadFromDirectory("assets/maps/nonexistent-map-directory-xyz");
    } catch (const std::runtime_error &e) {
        caught_exception = true;
        std::string error_msg = e.what();
        assert(error_msg.find("does not exist") != std::string::npos);
    }
    
    assert(caught_exception);
    assert(!parser.isMapLoaded());
    
    std::cout << "  PASS: Correctly throws exception for missing directory" << std::endl;
    return 0;
}

// ====================
// Test 9: Missing .def File Error
// ====================
int test_missing_def_file() {
    std::cout << "[TEST] Missing .def file error handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_missing_def");
    
    try {
        // Create only a .map file
        create_test_file(test_dir + "/test.map", ". . .\n. . .\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::runtime_error &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("No .def file found") != std::string::npos);
        }
        
        assert(caught_exception);
        assert(!parser.isMapLoaded());
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly throws exception for missing .def file" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 10: Missing .map File Error
// ====================
int test_missing_map_file() {
    std::cout << "[TEST] Missing .map file error handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_missing_map");
    
    try {
        // Create only a .def file
        create_test_file(test_dir + "/test.def", "3,2\nTest map\n. ./test.png\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::runtime_error &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("No .map file found") != std::string::npos);
        }
        
        assert(caught_exception);
        assert(!parser.isMapLoaded());
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly throws exception for missing .map file" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 11: Invalid .def Format (bad dimensions)
// ====================
int test_invalid_def_dimensions() {
    std::cout << "[TEST] Invalid .def dimensions error handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_invalid_def_dims");
    
    try {
        // Create invalid .def with bad dimensions
        create_test_file(test_dir + "/test.def", "invalid_dimensions\nTest map\n. ./test.png\n");
        create_test_file(test_dir + "/test.map", ". . .\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::invalid_argument &e) {
            caught_exception = true;
        }
        
        assert(caught_exception);
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly throws exception for invalid dimensions" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 12: Dimension Mismatch Error
// ====================
int test_dimension_mismatch() {
    std::cout << "[TEST] Dimension mismatch error handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_dim_mismatch");
    
    try {
        // Create .def saying 3x2, but .map has wrong dimensions
        create_test_file(test_dir + "/test.def", "3,2\nTest map\n. ./test.png\n");
        create_test_file(test_dir + "/test.map", ". . .\n. . .\n. . .\n"); // 3 rows instead of 2
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::invalid_argument &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("rows") != std::string::npos || 
                   error_msg.find("height") != std::string::npos);
        }
        
        assert(caught_exception);
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly throws exception for dimension mismatch" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 13: Tab Character Detection
// ====================
int test_tab_character_detection() {
    std::cout << "[TEST] Tab character detection..." << std::endl;
    
    std::string test_dir = create_test_dir("test_tab_chars");
    
    try {
        create_test_file(test_dir + "/test.def", "3,2\nTest map\n. ./test.png\n");
        // Create .map with a tab character
        create_test_file(test_dir + "/test.map", ". .\t.\n. . .\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::invalid_argument &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("Tab") != std::string::npos || 
                   error_msg.find("tab") != std::string::npos);
        }
        
        assert(caught_exception);
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly detects and rejects tab characters" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 14: Comments in .map File
// ====================
int test_map_comments() {
    std::cout << "[TEST] Comments in .map file handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_map_comments");
    
    try {
        create_test_file(test_dir + "/test.def", "3,2\nTest map\n. ./test.png\n# ./wall.png\n");
        create_test_file(test_dir + "/test.map", 
            "/// This is a comment\n"
            ". . #\n"
            "/// Another comment\n"
            "# . .\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        (void)parser.loadFromDirectory(test_dir);
        
        assert(parser.isMapLoaded());
        const MapDefinition &map_def = parser.getMapDefinition();
        assert(map_def.height == 2); // Comments shouldn't count as rows
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Comments are correctly ignored" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 15: Empty Lines Handling
// ====================
int test_empty_lines() {
    std::cout << "[TEST] Empty lines handling..." << std::endl;
    
    std::string test_dir = create_test_dir("test_empty_lines");
    
    try {
        create_test_file(test_dir + "/test.def", 
            "3,2\n"
            "Test map\n"
            "\n"  // Empty line
            ". ./test.png\n"
            "\n"  // Another empty line
            "# ./wall.png\n");
        create_test_file(test_dir + "/test.map", ". . #\n# . .\n");
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();

        (void)parser.loadFromDirectory(test_dir);

        assert(parser.isMapLoaded());
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Empty lines are correctly handled" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 16: Multiple Loads (Singleton Behavior)
// ====================
int test_multiple_loads() {
    std::cout << "[TEST] Multiple map loads (singleton persistence)..." << std::endl;
    
    MapParser &parser = MapParser::getInstance();
    
    try {
        // Load first map
        parser.clear();
        parser.loadFromDirectory("assets/maps/welcome");
        size_t tiles_count_1 = parser.getTiles().size();
        
        // Load second map without clearing
        parser.clear();
        parser.loadFromDirectory("assets/maps/demo");
        size_t tiles_count_2 = parser.getTiles().size();
        
        // Counts should be different (different maps)
        assert(tiles_count_1 != tiles_count_2);
        
        std::cout << "  PASS: Multiple loads work correctly" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 17: Invalid Tile Character (unclosed bracket)
// ====================
int test_unclosed_bracket() {
    std::cout << "[TEST] Unclosed random group bracket detection..." << std::endl;
    
    std::string test_dir = create_test_dir("test_unclosed_bracket");
    
    try {
        create_test_file(test_dir + "/test.def", "5,2\nTest map\n. ./test.png\n* ./star.png\n");
        create_test_file(test_dir + "/test.map", "[.* . . .\n. . . . .\n"); // Missing closing ]
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::invalid_argument &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("Unclosed") != std::string::npos || 
                   error_msg.find("bracket") != std::string::npos);
        }
        
        assert(caught_exception);
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly detects unclosed brackets" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Test 18: Empty Random Group
// ====================
int test_empty_random_group() {
    std::cout << "[TEST] Empty random group detection..." << std::endl;
    
    std::string test_dir = create_test_dir("test_empty_random");
    
    try {
        create_test_file(test_dir + "/test.def", "3,2\nTest map\n. ./test.png\n");
        create_test_file(test_dir + "/test.map", "[] . .\n. . .\n"); // Empty random group
        
        MapParser &parser = MapParser::getInstance();
        parser.clear();
        
        bool caught_exception = false;
        try {
            parser.loadFromDirectory(test_dir);
        } catch (const std::invalid_argument &e) {
            caught_exception = true;
            std::string error_msg = e.what();
            assert(error_msg.find("Empty") != std::string::npos || 
                   error_msg.find("random group") != std::string::npos);
        }
        
        assert(caught_exception);
        
        cleanup_test_dir(test_dir);
        std::cout << "  PASS: Correctly detects empty random groups" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        cleanup_test_dir(test_dir);
        std::cerr << "  FAIL: " << e.what() << std::endl;
        return 1;
    }
}

// ====================
// Main Test Runner
// ====================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running MapParser Library Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Array of test functions
    struct TestCase {
        const char* name;
        int (*func)();
    };
    
    TestCase tests[] = {
        {"Singleton Pattern", test_singleton_pattern},
        {"Initial State", test_initial_state},
        {"Valid Finite Map", test_valid_finite_map},
        {"Valid Infinite Map", test_valid_infinite_map},
        {"Welcome Map with Default Merging", test_welcome_map},
        {"Clear Functionality", test_clear_functionality},
        {"Tile Structure", test_tile_structure},
        {"Missing Directory Error", test_missing_directory},
        {"Missing .def File Error", test_missing_def_file},
        {"Missing .map File Error", test_missing_map_file},
        {"Invalid .def Dimensions", test_invalid_def_dimensions},
        {"Dimension Mismatch", test_dimension_mismatch},
        {"Tab Character Detection", test_tab_character_detection},
        {"Comments in .map", test_map_comments},
        {"Empty Lines Handling", test_empty_lines},
        {"Multiple Loads", test_multiple_loads},
        {"Unclosed Bracket Detection", test_unclosed_bracket},
        {"Empty Random Group Detection", test_empty_random_group},
    };
    
    for (const auto &test : tests) {
        total_tests++;
        try {
            if (test.func() == 0) {
                passed_tests++;
            } else {
                std::cerr << "FAILED: " << test.name << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "EXCEPTION in " << test.name << ": " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "Test Results: " << passed_tests << "/" << total_tests << " passed" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (passed_tests == total_tests) ? 0 : 1;
}

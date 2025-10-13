cmake_minimum_required(VERSION 3.20)

# Try to find criterion library using pkg-config first, then fallback to find_library
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CRITERION criterion)
endif()

# If pkg-config didn't find it, try find_library as fallback
if(NOT CRITERION_FOUND)
    find_library(CRITERION_LIB criterion)
    find_path(CRITERION_INCLUDE_DIR criterion/criterion.h)
    if(CRITERION_LIB AND CRITERION_INCLUDE_DIR)
        set(CRITERION_FOUND TRUE)
        set(CRITERION_LIBRARIES ${CRITERION_LIB})
        set(CRITERION_INCLUDE_DIRS ${CRITERION_INCLUDE_DIR})
    endif()
endif()

if(CRITERION_FOUND)
    message(STATUS "Found Criterion library, building full test suite")

    # Test for ECS library functionality
    add_executable(test_ecs test_ecs.cpp)
    target_link_libraries(test_ecs ecs)
    if(CRITERION_LDFLAGS)
        target_link_libraries(test_ecs ${CRITERION_LDFLAGS})
    else()
        target_link_libraries(test_ecs ${CRITERION_LIBRARIES})
    endif()
    if(CRITERION_INCLUDE_DIRS)
        target_include_directories(test_ecs PRIVATE ${CRITERION_INCLUDE_DIRS})
    endif()

    # Test for server functionality
    add_executable(test_server test_server.cpp)
    target_link_libraries(test_server ecs)
    if(CRITERION_LDFLAGS)
        target_link_libraries(test_server ${CRITERION_LDFLAGS})
    else()
        target_link_libraries(test_server ${CRITERION_LIBRARIES})
    endif()
    if(CRITERION_INCLUDE_DIRS)
        target_include_directories(test_server PRIVATE ${CRITERION_INCLUDE_DIRS})
    endif()

    # Test for client functionality
    add_executable(test_client test_client.cpp)
    target_link_libraries(test_client ecs)
    if(CRITERION_LDFLAGS)
        target_link_libraries(test_client ${CRITERION_LDFLAGS})
    else()
        target_link_libraries(test_client ${CRITERION_LIBRARIES})
    endif()
    if(CRITERION_INCLUDE_DIRS)
        target_include_directories(test_client PRIVATE ${CRITERION_INCLUDE_DIRS})
    endif()

    # Basic fallback test
    add_executable(test_basic test_basic.cpp)
    target_link_libraries(test_basic ecs)

    # PacketManager test - Auto-discover all .cpp files in packetmanager subdirectory
    file(GLOB_RECURSE PACKETMANAGER_TEST_SOURCES "packetmanager/*.cpp")
    add_executable(test_packetmanager ${PACKETMANAGER_TEST_SOURCES})
    target_link_libraries(test_packetmanager packetmanager)

    add_executable(test_packethandler test_packethandler.cpp)
    target_link_libraries(test_packethandler packethandler packetmanager)

    # MapParser test
    add_executable(test_mapparser test_mapparser.cpp)
    target_link_libraries(test_mapparser mapparser)

    # Add tests to CTest
    add_test(NAME ECSTest COMMAND test_ecs)
    add_test(NAME ServerTest COMMAND test_server)
    add_test(NAME ClientTest COMMAND test_client)
    add_test(NAME BasicTest COMMAND test_basic)
    add_test(NAME PacketManagerTest COMMAND test_packetmanager)
    add_test(NAME PacketHandlerTest COMMAND test_packethandler)
    add_test(NAME MapParserTest COMMAND test_mapparser WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    # Set test output directory for all test executables
    set_target_properties(test_ecs test_server test_client test_basic test_packetmanager test_packethandler test_mapparser PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
else()
    message(STATUS "Criterion library not found, using basic tests only")

    # Fallback: simple executable tests without criterion
    add_executable(test_basic test_basic.cpp)
    target_link_libraries(test_basic ecs)

    # PacketManager test - Auto-discover all .cpp files in packetmanager subdirectory
    file(GLOB_RECURSE PACKETMANAGER_TEST_SOURCES "packetmanager/*.cpp")
    add_executable(test_packetmanager ${PACKETMANAGER_TEST_SOURCES})
    target_link_libraries(test_packetmanager packetmanager)

    add_executable(test_packethandler test_packethandler.cpp)
    target_link_libraries(test_packethandler packethandler packetmanager)

    # MapParser test
    add_executable(test_mapparser test_mapparser.cpp)
    target_link_libraries(test_mapparser mapparser)

    add_test(NAME BasicTest COMMAND test_basic)
    add_test(NAME PacketManagerTest COMMAND test_packetmanager)
    add_test(NAME PacketHandlerTest COMMAND test_packethandler)
    add_test(NAME MapParserTest COMMAND test_mapparser WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    set_target_properties(test_basic test_packetmanager test_packethandler test_mapparser PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()

# R-Type Project - Complete Documentation

> Multiplayer shoot'em up game inspired by the classic R-Type arcade game

**Project**: EPITECH R-Type 2025  
**Team**: Morgan Guichard & Contributors  
**Language**: C++17  
**Architecture**: Client-Server with ECS (Entity Component System)

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Architecture](#architecture)
4. [Getting Started](#getting-started)
5. [Installation & Build](#installation--build)
6. [Usage](#usage)
7. [Game Controls](#game-controls)
8. [Network Protocol](#network-protocol)
9. [Project Structure](#project-structure)
10. [Development](#development)
11. [Testing](#testing)
12. [Documentation](#documentation)
13. [Contributing](#contributing)
14. [License](#license)

---

## Project Overview

**R-Type** is a networked multiplayer game project that recreates the classic side-scrolling shoot'em up experience with modern technology. Built entirely in C++17, it features a robust client-server architecture using UDP networking with reliability guarantees, and an Entity Component System (ECS) for flexible game logic.

### Key Highlights

- 🎮 **Classic Gameplay**: Side-scrolling shoot'em up action
- 🌐 **Multiplayer**: Play with friends in public or private rooms
- 🏗️ **Modern Architecture**: ECS pattern for clean, maintainable code
- 🔄 **Reliable UDP**: Custom network layer with packet retransmission
- 🎨 **SFML Graphics**: 2D graphics, animations, and audio
- 🧪 **Tested**: Comprehensive unit tests with Criterion
- 📦 **Cross-Platform**: Windows, macOS, and Linux support

### Technologies

| Component | Technology | Version |
|-----------|-----------|---------|
| Language | C++ | 17 |
| Graphics/Audio | SFML | 2.5.1+ |
| Build System | CMake + Ninja | 3.20+ |
| ECS Library | Custom | In-house |
| Network | UDP (Custom Reliable Layer) | - |
| Testing | Criterion | Latest |
| CI/CD | GitHub Actions | - |

---

## Features

### Core Gameplay

- ✅ **Player Controls**: Smooth 8-directional movement
- ✅ **Shooting Mechanics**: Projectile-based combat with fire rate limiting
- ✅ **Enemy System**: Multiple enemy types with AI behaviors
- ✅ **Health System**: Player and enemy health with visual feedback
- ✅ **Collision Detection**: Accurate hitbox-based collisions
- ✅ **Score System**: Track player performance
- ✅ **Game Over**: End-game state with return to menu

### Multiplayer Features

- ✅ **Public Rooms**: Join public games (Room ID: 0)
- ✅ **Private Rooms**: Create/join private rooms with 4-digit codes (1000-9999)
- ✅ **Admin System**: Room creators have admin privileges
- ✅ **Player Lobby**: Pre-game lobby with ready system
- ✅ **Network Sync**: Real-time entity synchronization
- ✅ **Packet Loss Handling**: Automatic retransmission

### Visual & Audio

- 🎨 **Sprites**: Custom pixel-art graphics for player, enemies, projectiles
- 🎵 **Music**: Background music (menu, gameplay, boss fights, game over)
- 🔊 **Sound Effects**: Shooting, explosions, hits, death sounds
- ✨ **Animations**: Sprite-based animations for entities
- 🌌 **Parallax Background**: Multi-layer scrolling backgrounds

### Game States

1. **Main Menu**: Username entry and server selection
2. **Public Server State**: Direct entry to public game
3. **Private Server State**: Create or join private rooms
4. **Private Lobby**: Wait for players, ready up, start game
5. **Game State**: Active gameplay
6. **Game Over State**: End screen with score

---

## Architecture

### Overview

```
┌─────────────────────────────────────────────────────────────┐
│                         R-Type System                         │
├─────────────────────┬───────────────────────────────────────┤
│   Client            │   Server                              │
│                     │                                       │
│  ┌──────────────┐   │   ┌──────────────┐                  │
│  │ StateManager │   │   │ RoomService  │                  │
│  │   (GUI)      │   │   │ PlayerService│                  │
│  └──────────────┘   │   └──────────────┘                  │
│         │           │          │                           │
│  ┌──────────────┐   │   ┌──────────────┐                  │
│  │  ECS World   │   │   │  ECS World   │                  │
│  │  (Local)     │   │   │  (Authority) │                  │
│  └──────────────┘   │   └──────────────┘                  │
│         │           │          │                           │
│  ┌──────────────────────────────────────┐                 │
│  │     PacketManager (Reliable UDP)     │                 │
│  └──────────────────────────────────────┘                 │
└─────────────────────────────────────────────────────────────┘
```

### ECS (Entity Component System)

The game uses a custom ECS architecture where:

- **Entities**: Unique IDs representing game objects (player, enemies, projectiles)
- **Components**: Data containers (Position, Velocity, Health, Sprite, etc.)
- **Systems**: Logic processors that operate on entities with specific components

**Advantages**:
- Data-oriented design for better performance
- Flexible composition over inheritance
- Easy to add new features
- Shared code between client and server

### Network Architecture

- **Protocol**: UDP with custom reliability layer
- **Packet Types**: Connection, Join Room, Player Input, State Update, etc.
- **Reliability**: Sequence numbers, ACKs, retransmission
- **Client-Server Model**: Server authoritative, clients predict & interpolate

---

## Getting Started

### Prerequisites

Before building R-Type, ensure you have the following installed:

#### Required

- **C++ Compiler**: GCC 13.3.0+ or Clang 15+ or MSVC 2022+
- **CMake**: 3.20 or later
- **Ninja**: Build system (optional but recommended)
- **SFML**: 2.5.1 or later
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libsfml-dev
  
  # macOS
  brew install sfml
  
  # Windows
  # Download from https://www.sfml-dev.org/download.php
  ```

#### Optional (for development)

- **Criterion**: Testing framework
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libcriterion-dev
  
  # macOS
  brew install criterion
  ```
- **PlantUML**: For UML diagram generation
  ```bash
  sudo apt-get install plantuml  # Ubuntu/Debian
  brew install plantuml          # macOS
  ```
- **Doxygen**: For code documentation
  ```bash
  sudo apt-get install doxygen   # Ubuntu/Debian
  brew install doxygen           # macOS
  ```

### Quick Start

```bash
# Clone the repository
git clone https://github.com/morgangch/rtype.git
cd rtype

# Build the project
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)

# Run the server (in one terminal)
./build/bin/server

# Run the client (in another terminal)
./build/bin/client
```

---

## Installation & Build

### Detailed Build Instructions

#### 1. Clone the Repository

```bash
git clone https://github.com/morgangch/rtype.git
cd rtype
```

#### 2. Configure the Project

```bash
# Release build (optimized)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Debug build (with debug symbols)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Without Ninja (using default generator)
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

#### 3. Build

```bash
# Build with all CPU cores
cmake --build build --parallel $(nproc)

# Or with specific number of jobs
cmake --build build --parallel 4

# Build specific target
cmake --build build --target client
cmake --build build --target server
```

#### 4. Run Tests (Optional)

```bash
cd build
ctest --output-on-failure

# Or run tests verbose
ctest --verbose

# Run specific test
./tests/test_ecs
./tests/test_mapparser
```

### Build Outputs

After building, executables are located in `build/bin/`:

```
build/
└── bin/
    ├── client           # Client executable
    ├── server           # Server executable
    └── mapparser_demo   # MapParser demo (optional)
```

### Troubleshooting

#### SFML Not Found

```bash
# Set SFML path manually
cmake -B build -DSFML_DIR=/path/to/SFML/lib/cmake/SFML
```

#### Compiler Issues

```bash
# Use specific compiler
cmake -B build -DCMAKE_CXX_COMPILER=g++-13
cmake -B build -DCMAKE_CXX_COMPILER=clang++-15
```

#### Clean Build

```bash
# Remove build directory and rebuild
rm -rf build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
```

---

## Usage

### Running the Server

The server must be started before clients can connect.

```bash
# Start server on default port (4242)
./build/bin/server

# Server will display:
# - Listening address and port
# - Connected clients
# - Room information
# - Game events
```

**Server Features**:
- Handles multiple rooms simultaneously
- Manages player connections and disconnections
- Authoritative game state
- Broadcast state updates to clients
- Enemy spawning and AI
- Collision detection
- Score tracking

### Running the Client

```bash
# Start client
./build/bin/client

# The client will open a window with the main menu
```

#### Menu Navigation

1. **Main Menu**:
   - Enter your username (3-16 characters)
   - Choose between Public Server or Private Server

2. **Public Server**:
   - Instantly connects to Room ID 0 (public room)
   - Starts game immediately

3. **Private Server**:
   - **Join Server**: Enter 4-digit room code (1000-9999)
   - **Create Server**: Generate new room and become admin
   - Transitions to private lobby

4. **Private Lobby**:
   - Shows room code to share with friends
   - Admin can start game when ready
   - Players toggle ready status
   - Display of ready player count

5. **Game**:
   - Control your ship
   - Shoot enemies
   - Avoid collisions
   - Survive as long as possible

### Command Line Options

Currently, the executables don't support command line arguments. Configuration is done through:
- Server: Port is hardcoded to 4242
- Client: Server IP/Port selected in-game menus

### Multiple Clients

To test multiplayer locally:

```bash
# Terminal 1: Server
./build/bin/server

# Terminal 2: Client 1
./build/bin/client

# Terminal 3: Client 2
./build/bin/client

# Both clients can join the same room
```

---

## Game Controls

### Keyboard Controls

| Key | Action |
|-----|--------|
| **Movement** | |
| `Z` or `↑` | Move Up |
| `S` or `↓` | Move Down |
| `Q` or `←` | Move Left |
| `D` or `→` | Move Right |
| **Combat** | |
| `Space` | Shoot |
| **Menu** | |
| `Mouse` | Click buttons |
| `Text Input` | Enter username/code |
| `Enter` | Confirm |
| `Escape` | Return to Main Menu (in-game) |

### Gameplay Tips

- **Movement**: Player moves smoothly in 8 directions
- **Shooting**: Hold spacebar to fire continuously (fire rate limited)
- **Invulnerability**: Brief invulnerability period after taking damage
- **Enemies**: Different enemy types with unique behaviors
- **Collisions**: Avoid enemy ships and their projectiles
- **Score**: Destroy enemies to increase score

---

## Network Protocol

### Packet Types

The game uses a custom binary protocol over UDP with the following packet types:

| Packet ID | Name | Direction | Description |
|-----------|------|-----------|-------------|
| 0x01 | `JOIN_ROOM` | C→S | Client requests to join room |
| 0x02 | `JOIN_ROOM_ACCEPTED` | S→C | Server accepts join request |
| 0x03 | `PLAYER_READY` | C→S | Player toggles ready status |
| 0x04 | `GAME_START_REQUEST` | C→S | Admin requests game start |
| 0x05 | `GAME_START` | S→C | Server starts game |
| 0x10 | `PLAYER_INPUT` | C→S | Player movement/actions |
| 0x11 | `STATE_UPDATE` | S→C | Game state broadcast |
| 0x20 | `ENTITY_SPAWNED` | S→C | New entity created |
| 0x21 | `ENTITY_DESTROYED` | S→C | Entity removed |
| 0x30 | `DISCONNECT` | C→S | Client disconnects |

### Packet Structure

```cpp
struct packet_header_t {
    uint32_t seqid;          // Sequence number for ordering
    uint32_t ack;            // Acknowledged sequence number
    uint8_t  type;           // Packet type ID
    uint32_t auth;           // Authentication token
    uint32_t data_size;      // Payload size in bytes
    uint8_t  client_addr[4]; // Client IP address
    uint16_t client_port;    // Client port
};

struct packet_t {
    packet_header_t header;
    void* data;              // Payload (depends on packet type)
};
```

### Reliability Mechanism

- **Sequence Numbers**: Each packet has unique `seqid`
- **Acknowledgments**: Receiver sends `ack` back
- **Retransmission**: Sender retries if no ACK received
- **Timeout**: Configurable timeout period
- **Duplicate Detection**: Ignores already-processed packets

### Connection Flow

```
Client                          Server
  │                               │
  ├─── JOIN_ROOM ────────────────>│
  │    (username, room_code)      │
  │                               │
  │<──── JOIN_ROOM_ACCEPTED ──────┤
  │      (player_id, admin_flag)  │
  │                               │
  ├─── PLAYER_READY ─────────────>│ (if not admin)
  │                               │
  ├─── GAME_START_REQUEST ───────>│ (if admin)
  │                               │
  │<──── GAME_START ──────────────┤
  │      (start_timestamp)        │
  │                               │
  ├─── PLAYER_INPUT ─────────────>│ (game loop)
  │                               │
  │<──── STATE_UPDATE ────────────┤ (game loop)
  │      (all entities)           │
  │                               │
```

---

## Project Structure

```
rtype/
├── client/                     # Client Application
│   ├── components/            # Client-specific components
│   ├── include/
│   │   ├── gui/              # GUI system (States, Managers)
│   │   ├── components/       # Animation, Audio, Camera, Input, Render
│   │   └── network/          # Network controllers
│   └── src/
│       ├── gui/              # GUI implementations
│       └── network/          # Network implementations
│
├── server/                     # Server Application
│   ├── include/
│   │   ├── components/       # Server components (EnemyAI, PlayerConn)
│   │   ├── controllers/      # Packet handlers (RoomController)
│   │   ├── services/         # Business logic (RoomService, PlayerService)
│   │   └── systems/          # Game systems (Collision, EnemySpawner)
│   └── src/
│       ├── controllers/      # Controller implementations
│       ├── services/         # Service implementations
│       └── systems/          # System implementations
│
├── common/                     # Shared Code (Client & Server)
│   ├── components/           # Shared components (Position, Velocity, Health, Player, etc.)
│   ├── core/                 # Core types (Entity, EntityType, Event, Hitbox)
│   ├── network/              # Network types (MessageType, Packet, Protocol)
│   ├── systems/              # Shared systems (ChargedShot, HealthSystem, Movement)
│   ├── utils/                # Utilities (Config, EntityFactory, Logger, Random)
│   └── packets/              # Packet definitions
│
├── lib/                        # Libraries
│   ├── ecs/                  # Entity Component System
│   │   ├── include/ECS/      # ECS headers (World, EntityManager, ComponentManager)
│   │   └── src/              # ECS implementations
│   ├── packetmanager/        # Network packet management
│   │   ├── include/          # PacketManager, packet_t
│   │   └── src/              # Network implementations
│   ├── packethandler/        # Packet handler system
│   │   ├── include/          # PacketHandler
│   │   └── src/              # Handler implementations
│   └── mapparser/            # Level parser
│       ├── include/          # MapParser
│       └── src/              # Parser implementations
│
├── assets/                     # Game Resources
│   ├── sprites/              # Images (player, enemies, projectiles, UI)
│   ├── fonts/                # Fonts (r-type.otf)
│   ├── audio/
│   │   ├── music/           # Background music (menu, level, boss, gameover)
│   │   ├── player/          # Player sounds (loselife)
│   │   ├── enemy/           # Enemy sounds (regulardeath, bossdeath, bossfight)
│   │   └── particles/       # Effect sounds (shoot, chargedshoot)
│   └── maps/                 # Level definitions (.map, .def)
│
├── tests/                      # Unit Tests
│   ├── test_ecs.cpp          # ECS tests
│   ├── test_mapparser.cpp    # MapParser tests
│   ├── test_client.cpp       # Client tests
│   ├── test_server.cpp       # Server tests
│   └── packetmanager/        # PacketManager tests
│
├── docs/                       # Documentation
│   ├── README.md             # This file
│   ├── CODE_OF_CONDUCT.md    # Code of conduct
│   ├── CODING_STYLE.md       # Coding standards
│   ├── GIT_STANDARD.md       # Git conventions
│   ├── MAPS_STANDARD.md      # Map file format
│   ├── ORGANISATION.md       # Project organization
│   ├── BENCHMARK.md          # Technology benchmarks
│   └── QUICK_REFERENCE.md    # Quick reference guide
│
├── archi_mindmap/             # UML Architecture Documentation
│   ├── architecture.puml     # Class diagram source
│   ├── sequence.puml         # Sequence diagram source
│   ├── components.puml       # Component diagram source
│   ├── *.png                 # Generated diagrams
│   └── README.md             # Architecture documentation
│
├── scripts/                    # Utility Scripts
│   ├── generate_uml.sh       # Generate UML diagrams
│   └── font_to_sprites.py    # Font to sprite converter
│
├── examples/                   # Example Programs
│   ├── mapparser_demo/       # MapParser demonstration
│   └── packethandler_example.cpp
│
├── .github/                    # GitHub Configuration
│   └── workflows/            # CI/CD workflows
│
├── CMakeLists.txt             # Main CMake configuration
├── CONTRIBUTING.md            # Contribution guidelines
├── Doxyfile                   # Doxygen configuration
└── README.md                  # Project README (root)
```

### Key Directories

- **`client/`**: Client application with GUI, rendering, and input handling
- **`server/`**: Server application with game logic, networking, and services
- **`common/`**: Code shared between client and server (components, utilities)
- **`lib/`**: Reusable libraries (ECS, networking, parsing)
- **`assets/`**: All game resources (sprites, sounds, music, fonts, maps)
- **`tests/`**: Unit tests for all major components
- **`docs/`**: Complete project documentation

---

## Development

### Development Workflow

1. **Check Documentation**: Read `CONTRIBUTING.md` for guidelines
2. **Create Branch**: Follow Git standards (`feat/`, `fix/`, `docs/`)
3. **Write Code**: Follow coding style guidelines
4. **Write Tests**: Add tests for new features
5. **Run Tests**: Ensure all tests pass
6. **Commit**: Use conventional commit messages
7. **Push**: Create pull request for review

### Coding Standards

See [CODING_STYLE.md](CODING_STYLE.md) for detailed guidelines.

**Key Points**:
- **Naming**: `PascalCase` for classes, `camelCase` for functions/variables
- **Files**: `ClassName.h` and `ClassName.cpp`
- **Headers**: Include guards `#ifndef FILENAME_H`
- **Documentation**: Doxygen comments for all public APIs
- **Formatting**: Consistent indentation (4 spaces)
- **Namespaces**: Use `rtype::module::submodule`

### Git Workflow

See [GIT_STANDARD.md](GIT_STANDARD.md) for detailed conventions.

**Branch Naming**:
```
feat/<feature-name>       # New features
fix/<bug-description>     # Bug fixes
docs/<doc-update>         # Documentation
refactor/<change-desc>    # Code refactoring
test/<test-name>          # Test additions
```

**Commit Messages**:
```
<type>(<scope>): <subject>

<body>

<footer>
```

Example:
```
feat(client): add parallax background system

Implement multi-layer parallax scrolling background
with configurable scroll speeds for each layer.

Closes #42
```

### Adding New Features

#### Adding a New Component

```cpp
// 1. Create header in common/components/MyComponent.h
#ifndef COMMON_MYCOMPONENT_H
#define COMMON_MYCOMPONENT_H

#include <ECS/ECS.h>

namespace rtype::common::components {
    class MyComponent : public ECS::Component<MyComponent> {
    public:
        int myData;
        
        explicit MyComponent(int data) : myData(data) {}
    };
}

#endif
```

```cpp
// 2. Use in client or server
auto entity = world.CreateEntity();
world.AddComponent<MyComponent>(entity, 42);

// 3. Create a system to process it
auto* components = world.GetAllComponents<MyComponent>();
for (const auto& pair : *components) {
    auto* comp = pair.second.get();
    // Process component
}
```

#### Adding a New Packet Type

```cpp
// 1. Define packet ID in common/packets/packets.h
enum Packets {
    // ... existing ...
    MY_NEW_PACKET = 0x50,
};

// 2. Define packet structure
struct MyNewPacket {
    uint32_t data;
    char message[64];
};

// 3. Register handler on server
root.packetHandler.registerCallback(
    Packets::MY_NEW_PACKET,
    my_controller::handleMyNewPacket
);

// 4. Send from client
MyNewPacket packet;
packet.data = 123;
strncpy(packet.message, "Hello", sizeof(packet.message));

auto data = network::pm.sendPacketBytesSafe(
    &packet, sizeof(packet), Packets::MY_NEW_PACKET
);
network::pm.send(std::move(data), network::server_addr);
```

### Debugging

#### Enable Debug Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

#### Debug with GDB

```bash
# Server
gdb ./build/bin/server

# Client
gdb ./build/bin/client

# In GDB
(gdb) break main
(gdb) run
(gdb) next
(gdb) print variable
```

#### Debug with Valgrind

```bash
# Check memory leaks
valgrind --leak-check=full ./build/bin/server
valgrind --leak-check=full ./build/bin/client
```

#### Logging

Add logging in your code:

```cpp
#include <iostream>

std::cout << "Debug: " << variable << std::endl;
std::cerr << "Error: " << error_message << std::endl;
```

---

## Testing

### Running Tests

```bash
# Build tests
cmake --build build

# Run all tests
cd build
ctest --output-on-failure

# Run tests verbose
ctest --verbose

# Run specific test
./tests/test_ecs
./tests/test_mapparser
./tests/packetmanager/test_packetmanager
```

### Test Structure

Tests are organized by module:

- **test_ecs.cpp**: ECS library tests
- **test_mapparser.cpp**: MapParser tests
- **test_packetmanager.cpp**: Network tests
- **test_client.cpp**: Client-specific tests
- **test_server.cpp**: Server-specific tests

### Writing Tests

```cpp
// Example test with Criterion
#include <criterion/criterion.h>
#include <ECS/World.h>

Test(ecs, create_entity) {
    ECS::World world;
    auto entity = world.CreateEntity();
    
    cr_assert(entity != 0, "Entity ID should not be 0");
    cr_assert(world.IsEntityAlive(entity), "Entity should be alive");
}

Test(ecs, add_component) {
    ECS::World world;
    auto entity = world.CreateEntity();
    
    world.AddComponent<Position>(entity, 10.0f, 20.0f);
    auto* pos = world.GetComponent<Position>(entity);
    
    cr_assert(pos != nullptr, "Component should exist");
    cr_assert_float_eq(pos->x, 10.0f, 0.01f);
    cr_assert_float_eq(pos->y, 20.0f, 0.01f);
}
```

### Test Coverage

Run tests with coverage:

```bash
# Build with coverage flags
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build

# Run tests
cd build && ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html

# Open in browser
xdg-open coverage_html/index.html
```

---

## Documentation

### Available Documentation

- **[README.md](../README.md)**: Project overview (root)
- **[CONTRIBUTING.md](../CONTRIBUTING.md)**: Contribution guidelines
- **[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)**: Code of conduct
- **[CODING_STYLE.md](CODING_STYLE.md)**: Coding standards (EN)
- **[CODING_STYLE_FR.md](CODING_STYLE_FR.md)**: Coding standards (FR)
- **[GIT_STANDARD.md](GIT_STANDARD.md)**: Git conventions (EN)
- **[GIT_STANDARD_FR.md](GIT_STANDARD_FR.md)**: Git conventions (FR)
- **[MAPS_STANDARD.md](MAPS_STANDARD.md)**: Map file format (EN)
- **[MAPS_STANDARD_FR.md](MAPS_STANDARD_FR.md)**: Map file format (FR)
- **[ORGANISATION.md](ORGANISATION.md)**: Project organization
- **[BENCHMARK.md](BENCHMARK.md)**: Technology benchmarks
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)**: Quick reference
- **[archi_mindmap/README.md](../archi_mindmap/README.md)**: UML architecture

### Generating Documentation

#### Code Documentation (Doxygen)

```bash
# Generate Doxygen documentation
doxygen Doxyfile

# Documentation is generated in doc/html/
xdg-open doc/html/index.html
```

#### UML Diagrams (PlantUML)

```bash
# Generate UML diagrams
./generate_uml.sh

# Diagrams are generated in archi_mindmap/
# - architecture.png
# - sequence.png
# - components.png
```

### API Documentation

All public APIs are documented with Doxygen comments:

```cpp
/**
 * @brief Creates a new entity in the world
 * 
 * Entities are unique identifiers that can have components attached.
 * The entity ID can be used to add, get, or remove components.
 * 
 * @return EntityID The unique identifier for the new entity
 * 
 * @example
 * ECS::World world;
 * auto entity = world.CreateEntity();
 * world.AddComponent<Position>(entity, 0.0f, 0.0f);
 */
EntityID CreateEntity();
```

---

## Contributing

We welcome contributions! Please follow these steps:

1. **Read** [CONTRIBUTING.md](../CONTRIBUTING.md)
2. **Fork** the repository
3. **Create** a feature branch (`feat/my-feature`)
4. **Commit** changes with conventional commits
5. **Push** to your fork
6. **Create** a Pull Request

### Pull Request Checklist

- [ ] Code follows [CODING_STYLE.md](CODING_STYLE.md)
- [ ] Commits follow [GIT_STANDARD.md](GIT_STANDARD.md)
- [ ] All tests pass (`ctest`)
- [ ] New features have tests
- [ ] Documentation is updated
- [ ] No compiler warnings
- [ ] Code is properly formatted

### Communication

- **Issues**: Report bugs or request features
- **Discussions**: Ask questions or propose ideas
- **Pull Requests**: Submit code changes
- **Email**: [morgan.guichard@epitech.eu](mailto:morgan.guichard@epitech.eu)

---

## License

This project is an educational project for EPITECH. All rights reserved.

**Academic Use Only**: This project is created for educational purposes as part of the EPITECH curriculum. Commercial use, distribution, or reproduction without permission is prohibited.

---

## Credits

### Development Team

- **Morgan Guichard** - Project Lead & Core Developer
- **Contributors** - See [CONTRIBUTORS.md](../CONTRIBUTORS.md) (if exists)

### Technologies

- **SFML**: Simple and Fast Multimedia Library
- **CMake**: Cross-platform build system
- **Criterion**: C/C++ unit testing framework
- **PlantUML**: UML diagram generator
- **GitHub Actions**: CI/CD platform

### Assets

- **Sprites**: Custom pixel art
- **Fonts**: Custom R-Type font
- **Music & Sounds**: Free game audio assets

---

## Appendix

### Frequently Asked Questions (FAQ)

**Q: Why UDP instead of TCP?**  
A: UDP provides lower latency, which is crucial for real-time games. We implement reliability on top of UDP for guaranteed delivery when needed.

**Q: Can I play single-player?**  
A: Currently, the game requires both client and server to be running, but you can run both locally for single-player experience.

**Q: How many players can join a room?**  
A: Currently, there's no hard limit, but performance may degrade with many players. Recommended: 2-4 players.

**Q: Can I create custom levels?**  
A: Yes! See [MAPS_STANDARD.md](MAPS_STANDARD.md) for the map file format. Create `.map` and `.def` files in `assets/maps/`.

**Q: How do I report a bug?**  
A: Create an issue on GitHub with a detailed description, steps to reproduce, and relevant logs.

### Glossary

- **ECS**: Entity Component System - architectural pattern
- **UDP**: User Datagram Protocol - connectionless network protocol
- **SFML**: Simple and Fast Multimedia Library
- **ACK**: Acknowledgment packet
- **Seqid**: Sequence ID for packet ordering
- **Hitbox**: Collision detection boundary
- **Sprite**: 2D graphical image or animation

### Performance Tips

**Server**:
- Run on dedicated hardware for best performance
- Monitor CPU usage with `top` or `htop`
- Use Release build for production

**Client**:
- Close unnecessary applications
- Use dedicated GPU if available
- Adjust window resolution if needed

**Network**:
- Use wired connection for lower latency
- Check firewall settings (port 4242)
- Minimize network traffic from other applications

### Useful Commands

```bash
# Quick rebuild
alias rbuild='rm -rf build && cmake -B build -G Ninja && cmake --build build --parallel $(nproc)'

# Run server and client
alias rserver='./build/bin/server'
alias rclient='./build/bin/client'

# Check port usage
netstat -tulpn | grep 4242

# Kill process on port
lsof -ti:4242 | xargs kill -9

# Monitor network traffic
sudo tcpdump -i any port 4242
```

---

**Last Updated**: October 21, 2025  
**Version**: 2.0  
**Documentation Maintainer**: Morgan Guichard

For more information, visit the [project repository](https://github.com/morgangch/rtype).

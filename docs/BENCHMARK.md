# 🧠 Technology Benchmark

## 🧩 Language: **C++17**

### 🔍 Comparison
C++ remains a **reference language for developing high-performance games and applications**.  
- **Compared to Python**: slower and interpreted, Python is great for scripting but not for real-time games.  
- **Compared to C# (Unity)**: C# simplifies development, but C++ offers **full control over memory and performance**.  
- **Compared to Java**: Java automates memory management but sacrifices fine-grained performance.

### 💪 Strengths of C++
- Native compilation for maximum performance  
- Explicit memory management, ideal for games  
- Portability (Windows, Linux, macOS)  
- Large ecosystem of libraries (SFML, SDL, Boost, etc.)  
- Supports multiple paradigms (OOP, generic, functional)

---

## 🎮 Graphics Library: **SFML 2.6**

### 🔍 Comparison
| Engine / Library | Advantages | Drawbacks |
|------------------|-------------|------------|
| **SFML** | Simple, lightweight, clear API, built-in networking | No 3D support |
| **Raylib** | Modern and minimalist | Less complete for audio/network |
| **Unity (C#)** | Powerful visual tools | Heavy framework, dependencies |
| **Unreal Engine 5 (C++)** | Extremely powerful, AAA-ready | Overkill for a 2D project |
| **Ncurses** | Ultra-light, console only | No graphical rendering |

### 💪 Why SFML
- Clear, object-oriented API in C++  
- Integrated management of **graphics, audio, networking, and input**  
- **Lightweight and fast**, perfect for an educational project  
- Helps understand the **foundations of a 2D game engine**

---

## 🧪 Unit Testing: **Criterion 2.4.1**

### 🔍 Comparison
| Tool | Language | Strengths | Limitations |
|------|-----------|------------|--------------|
| **Criterion** | C/C++ | Auto-discovery of tests, parallel execution | Smaller ecosystem than GoogleTest |
| **GoogleTest** | C++ | Comprehensive, well-documented | Heavier syntax |
| **Catch2** | C++ | Header-only, easy integration | Less performant on large projects |
| **Functional / Integration Tests** | N/A | Validate global behavior | Slower, less precise for unit bugs |

### 💪 Why Criterion
- Simple **CMake** integration  
- Clear and readable test results  
- Supports **parallel execution**  
- Complemented by **functional tests (UDP)** for the client/server system

---

## ⚙️ Build System: **CMake + Ninja**

### 🔍 Comparison
| Tool | Description | Advantages | Drawbacks |
|------|--------------|-------------|-------------|
| **CMake + Ninja** | Cross-platform build system | Fast, industry standard, IDE compatible | Syntax can be complex |
| **Make** | Classic tool | Simple | Slow, not portable |
| **Meson + Ninja** | Modern with clear syntax | Fast | Less common |
| **Bazel / Premake** | Advanced automation | Cross-platform | Overkill for small projects |

### 💪 Why CMake + Ninja
- **CMake**: the de facto standard for modern C++  
- **Ninja**: **ultra-fast** parallel builds  
- Compatible with **GitHub Actions**, **VSCode**, **CLion**  
- Ideal for a **modular architecture** (client / server / engine)

---

## 🧱 Architecture: **ECS (Entity Component System)**

### 💪 Advantages
- **Modular and extensible** architecture  
- Clear separation between data and logic  
- Easy to add new behaviors without modifying existing entities  
- Common in modern engines (Unity, Frostbite…)

---

## 🧠 Design Patterns
- **Factory** → dynamic entity instantiation  
- **Singleton** → centralized management (resources, audio, etc.)  
- **Observer** → event-based communication between objects  
- **State Machine** → handles game states (menu, gameplay, pause, etc.)  
- **Manager** → orchestrates systems (rendering, networking, entities…)

---

## 🧰 Documentation & Tools

| Tool | Role | Why |
|------|------|------|
| **Doxygen + doxygen-awesome-css** | Automatic documentation generation | Modern, readable, cross-platform |
| **GitHub Actions** | Continuous Integration / Deployment | Automated multi-OS builds |
| **cppcheck / clang-tidy** | Static code analysis | Error detection and optimization |
| **PlantUML** | UML diagram generation | Quick and easily integrated with docs |

---

## 🌍 Target Platforms
- **Linux, macOS, Windows**  
- Compatibility ensured through **CMake** and **SFML**  
- Automated testing via **GitHub Actions** to guarantee portability

---

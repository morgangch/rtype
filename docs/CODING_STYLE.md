# 🧩 Coding Standard
*French version: [docs/CODING_STYLE_FR.md](./CODING_STYLE_FR.md)*

## 🧠 Documentation Practices

All classes, functions, and methods must be documented using **Doxygen**, following this format:
```cpp
/**
 * @brief Short description of the function or class.
 * @param param_name Parameter description.
 * @return Return value description.
 * @throws Possible exception(s).
 */
```

* Docstrings must be written **in English** to ensure consistency with the code.
* Each `.h` file must start with a `@file` block briefly describing its purpose:
```cpp
/**
 * @file PlayerManager.h
 * @brief Defines the player management logic.
*/
```

---

## ⚙️ General Practices

* Code files have **`.cpp`** suffix, headers have **`.h`**.
* Each `.cpp` source file must have an associated header, except `main` files.
* Use `#ifndef` in all headers. NEVER pragma once.
* No `using namespace std;`.
* Always initialize variables.
* Follow **MISRA C++** and **CERT C++** standards validated via `cppcheck` and `clang-tidy`.

### 🧱 Naming Conventions

| Element    | Convention | Example                     |
| ---------- | ---------- | --------------------------- |
| Classes    | PascalCase | `PlayerManager`             |
| Methods    | camelCase  | `getPlayerName()`           |
| Variables  | snake_case | `player_count`              |
| Constants  | UPPERCASE  | `MAX_PLAYERS`               |
| Namespaces | lowercase  | `namespace network { ... }` |

---

## 🧩 Modularity and Libraries

* Each reusable feature must be isolated in an internal library (`/libs`).
* Libraries must be **autonomous** with a dedicated `CMakeLists.txt`.
* Each library must include:

  * A public header in `/include/libname/`
  * A proper namespace:

    ```cpp
    namespace libname {
        ...
    }
    ```
* Dependencies between modules must be **minimal and explicit** (import only what is necessary).

---

## 🧪 Testing and CI/CD

* Unit tests are placed in `/tests`.
* Each PR triggers a **GitHub Actions pipeline**:
  * Project compilation (CMake)
  * Unit tests execution
  * Style verification with `cppcheck` / `clang-tidy`
* No merge is allowed if:
  * Tests fail,
  * Or if verification tools return errors.
```

## 🌍 Map Standard
*French version: [docs/MAPS_STANDARD_FR.md](./MAPS_STANDARD_FR.md)*

### 🗂️ **Map Organization**

All maps must be placed in the root folder:

```
/assets/maps/
```

Each map must have **its own folder**, named according to the **following naming convention**:

| Element       | Format                                        | Example                                    |
| ------------- | --------------------------------------------- | ------------------------------------------ |
| Map name      | `map-name` *(lowercase, numbers, hyphens)*    | `space-invaders`, `alien-planet`           |
| Map files     | `.map` and `.def`                             | `space-invaders.map`, `space-invaders.def` |

**Expected structure:**

```
assets/
    maps/
    map-name/
        map-name.map
        map-name.def
```

---

### 📄 **`.def` File — Map Definition**

The `.def` file describes the **metadata** and **symbol → assets mapping** of the map.

#### 🧩 General format:

1. **First line**: map dimensions
   * Format: `width,height` (e.g.: `10,5`)
   * Or `inf`,`height` for infinite width map. (e.g.: `inf,5`)

2. **Second line**: textual description of the map.

   * Example: `A space-themed map`

3. **Following lines**: tile definitions

   * Format: `character asset_path`
   * Examples:

     ```
     . ./assets/sprites/space/empty.png
     # ./assets/sprites/space/asteroid.png
     * ./assets/sprites/space/planet.png
     ```

#### ⚙️ Rules and constraints:

* Each definition character must be **unique**.
* Asset paths are **relative to the project root**.
* Characters not defined in the `.def`:
  * Are replaced by **the default asset in `assets/maps/default.def`**, if it exists.
  * Otherwise, are considered as **empty area** (background).

The engine first loads assets/maps/default.def (if present), then overrides characters defined in the local map.

#### 🧠 Example:

```txt
10,5
A space-themed map
. ./assets/sprites/space/empty.png
# ./assets/sprites/space/asteroid.png
* ./assets/sprites/space/planet.png
```

---

### 🗺️ **`.map` File — Map Layout**

The `.map` file describes the **tile layout** based on the `.def` definitions.

#### 🧩 General format:

* Each character corresponds to a **tile**.
* Each **line** corresponds to a **row** of tiles.
* **Spaces** (` `) serve as separators between tiles.
* **Comments** start with `///` and are **ignored**.
* Special characters (accented, emoji, etc.) are **forbidden** in `.def` and `.map` files. Only standard ASCII characters are allowed.

#### 📐 Finite maps:

* The number of columns and rows must match the width and height defined in the `.def`.

* Example:
  ```txt
  . . . . . . . . . .
  . . # # . . * * . .
  . . # # . . * * . .
  . . . . . . . . . .
  . . . . . . . . . .
  ```

#### ♾️ Infinite maps:
* The **width** is infinite, the **height** remains fixed.
* The `.map` content is interpreted as a **repeating horizontal pattern**.
* It is possible to use **random groups**:
  * Syntax: `[chars]`
    → Each tile will be **randomly chosen** from the contained characters.
  * Example:
    ```txt
    [.*] means that each occurrence of this tile can be '.' or '*'.
    ```

* Complete example:
  ```txt
  [.*] . . [.*]
  [.*] * * [.*]
  ```

---

### 🧾 **Additional rules**

* All `.def` and `.map` files must be **UTF-8 without BOM**.
* `.map` files must **never contain tabs** (`\t`) — only **simple spaces**.
* Asset paths must **never contain spaces**.

---

### 📚 **Best practices**

* Always validate the `.def` format before starting the `.map` creation.
* Test maps with a **minimal test map** before pushing them to the repo.
* When modifying a symbol or asset in a `.def`, verify:
  * that the character is consistent throughout the `.map`,
  * and that no other `.def` file defines the same character for another use (inter-map consistency).

---

### 💡 **Complete example**

#### `/assets/maps/space-invaders/space-invaders.def`

```txt
10,5
A space-themed map
. ./assets/sprites/space/empty.png
# ./assets/sprites/space/asteroid.png
* ./assets/sprites/space/planet.png
```

#### `/assets/maps/space-invaders/space-invaders.map`

```txt
. . . . . . . . . .
. . # # . . * * . .
. . # # . . * * . .
. . . . . . . . . .
. . . . . . . . . .
```

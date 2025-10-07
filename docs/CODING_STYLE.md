# 🧩 Norme Code

## 🧠 Pratiques de documentation

Toutes les classes, fonctions et méthodes doivent être documentées en utilisant **Doxygen**, selon le format suivant :
```cpp
/**
 * @brief Courte description de la fonction ou de la classe.
 * @param param_name Description du paramètre.
 * @return Description de la valeur de retour.
 * @throws Exception(s) éventuelle(s).
 */
```

* Les docstrings doivent être écrites **en anglais** pour assurer la cohérence avec le code.
* Chaque fichier `.h` doit commencer par un bloc `@file` décrivant brièvement son rôle :
```cpp
/**
 * @file PlayerManager.h
 * @brief Defines the player management logic.
*/
```

---

## ⚙️ Pratiques générales

* Les fichiers de code sont suffixés en **`.cpp`**, les headers en **`.h`**.
* Chaque fichier source `.cpp` doit avoir un header associé, sauf les fichiers `main`.
* Utiliser `#ifndef` dans tous les headers. JAMAIS pragma once.
* Pas de `using namespace std;`.
* Toujours initialiser les variables.
* Respecter les normes **MISRA C++** et **CERT C++** validées via `cppcheck` et `clang-tidy`.

### 🧱 Conventions de nommage

| Élément        | Convention | Exemple                     |
| -------------- | ---------- | --------------------------- |
| Classes        | PascalCase | `PlayerManager`             |
| Méthodes       | camelCase  | `getPlayerName()`           |
| Variables      | snake_case | `player_count`              |
| Constantes     | MAJUSCULES | `MAX_PLAYERS`               |
| Espaces de nom | lowercase  | `namespace network { ... }` |

---

## 🧩 Modularité et librairies

* Chaque fonctionnalité réutilisable doit être isolée dans une librairie interne (`/libs`).
* Les librairies doivent être **autonomes** avec un `CMakeLists.txt` dédié.
* Chaque librairie doit inclure :

  * Un header public dans `/include/libname/`
  * Un namespace propre :

    ```cpp
    namespace libname {
        ...
    }
    ```
* Les dépendances entre modules doivent être **minimales et explicites** (importer uniquement ce qui est nécessaire).

---

## 🧪 Tests et CI/CD

* Les tests unitaires sont placés dans `/tests`.
* Chaque PR déclenche une **pipeline GitHub Actions** :
  * Compilation du projet (CMake)
  * Exécution des tests unitaires
  * Vérification du style avec `cppcheck` / `clang-tidy`
* Aucun merge n’est autorisé si :
  * Les tests échouent,
  * Ou si les outils de vérification retournent des erreurs.

# 🎮 Guide de Test Offline - R-TYPE

## ✅ Fonctionnalité Ajoutée

Une **touche de raccourci F1** a été ajoutée pour lancer le jeu directement en mode offline (sans serveur) pour faciliter les tests.

## 🚀 Comment Tester le Jeu

### Méthode 1: Touche F1 (Rapide)
1. Lancez le client : `./build/bin/client`
2. Depuis le menu principal, appuyez sur **F1**
3. Le jeu se lance immédiatement en mode local !

### Méthode 2: Via le Menu (Normal)
1. Lancez le client
2. Entrez un nom d'utilisateur (optionnel)
3. Cliquez sur "Public" ou "Private"
4. ⚠️ Nécessite un serveur fonctionnel

## 🎯 Contrôles du Jeu

- **ZQSD** ou **Flèches directionnelles** : Déplacer le vaisseau
- **ESPACE** : Tirer des projectiles
- **ESC** : Pause / Retour au menu
- **F1** (depuis le menu principal) : Mode offline

## 📝 Modifications Apportées

### Fichiers Modifiés

1. **`client/include/gui/MainMenuState.h`**
   - Ajout de `sf::Text debugHintText`
   - Ajout de la méthode `launchOfflineGame()`

2. **`client/src/gui/MainMenuState.cpp`**
   - Ajout de l'include `#include "gui/GameState.h"`
   - Ajout du texte de hint "Press F1 to test game offline (DEBUG)" en vert
   - Ajout du handler pour la touche F1 dans `handleEvent()`
   - Ajout de la méthode `launchOfflineGame()` qui lance directement GameState

### Architecture ECS Modulaire (Common/)

3. **`common/components/Health.h`**
   - Constructeur inline défini (était juste déclaré)

4. **`common/systems/MovementSystem.h`** (NOUVEAU)
   - Système partagé client/serveur pour mise à jour des positions

5. **`common/systems/HealthSystem.h`** (NOUVEAU)
   - Système partagé pour gestion de l'invulnérabilité

6. **`common/utils/EntityFactory.h`** (NOUVEAU)
   - Factory functions pour créer facilement des entités
   - `createPlayer()`, `createEnemy()`, `createProjectile()`

7. **`common/examples/GameStateECSExample.cpp`** (NOUVEAU)
   - Guide complet d'utilisation de l'architecture ECS modulaire
   - Exemples de migration du code existant vers ECS

8. **`common/README_ECS.md`** (NOUVEAU)
   - Documentation complète de l'architecture ECS
   - Principes de design, bonnes pratiques, exemples d'utilisation

## 🏗️ Architecture ECS Modulaire

### Principes de Séparation

```
common/               # Logique partagée client/serveur
├── components/       # Composants ECS (Position, Velocity, Health, etc.)
├── systems/          # Systèmes partagés (Movement, Health)
├── utils/            # Utilitaires (EntityFactory, Config, Random)
└── core/             # Types de base (Entity, Event, etc.)

client/               # Logique client uniquement
├── components/       # Composants client (Sprite, Audio, Input)
└── systems/          # Systèmes client (Render, Input)

server/               # Logique serveur uniquement
├── components/       # Composants serveur (AI, Network)
└── systems/          # Systèmes serveur (Physics, Collision)
```

### Utilisation Simple

```cpp
// Créer un joueur avec l'EntityFactory
using namespace rtype::common;

auto player = factory::createPlayer(world, 100.0f, 360.0f, 3);

// Ajouter des composants client-specific
world.AddComponent<client::components::Sprite>(player, ...);

// Mettre à jour avec les systèmes modulaires
systems::MovementSystem::update(world, deltaTime);
systems::HealthSystem::update(world, deltaTime);
```

## 🔧 État Actuel du Code

- ✅ GameState fonctionne avec le code original (structs Player, Enemy, Projectile)
- ✅ Touche F1 pour test offline ajoutée
- ✅ Architecture ECS modulaire créée dans `common/`
- ⏳ Migration progressive possible (voir `common/examples/GameStateECSExample.cpp`)

## 📊 Prochaines Étapes (Optionnel)

Si vous voulez migrer vers l'ECS modulaire :

1. **Phase 1**: Garder le code actuel, tester l'ECS en parallèle
2. **Phase 2**: Utiliser l'ECS pour les nouvelles features
3. **Phase 3**: Migrer progressivement (projectiles → ennemis → joueur)
4. **Phase 4**: Supprimer les anciennes structs

## 🐛 Debug

Pour débugger le jeu :

```bash
# Compiler en mode debug
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Lancer avec GDB
gdb ./build/bin/client

# Ou simplement voir les logs console
./build/bin/client 2>&1 | tee game.log
```

## 📞 Messages de Debug

Quand vous appuyez sur F1, vous verrez :
```
[DEBUG] F1 pressed - Launching game in offline mode!
=== LAUNCHING OFFLINE GAME (DEBUG MODE) ===
Press F1 anytime from main menu to test the game!
=== Space Invaders Game ===
Controls:
  ZQSD/Arrow keys - Move ship
  SPACE - Fire projectiles
  ESC - Return to menu
Objective: Destroy enemies with your projectiles!
```

## ✨ Astuce

Le texte "Press F1 to test game offline (DEBUG)" apparaît en **vert clair** en haut à gauche du menu principal pour rappeler la fonctionnalité.

---

**Auteur**: R-TYPE Development Team  
**Date**: Octobre 2025  
**Branche**: feat/graphic-game

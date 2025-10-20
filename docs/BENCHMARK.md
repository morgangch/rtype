# 🧠 Benchmark des Technologies Utilisées

## 🧩 Langage : **C++17**

### 🔍 Comparaison
Le C++ reste un **langage de référence pour le développement de jeux et d’applications performantes**.  
- **Face à Python** : plus lent et interprété, Python est idéal pour les scripts mais pas pour les jeux temps réel.  
- **Face à C# (Unity)** : C# simplifie le développement, mais C++ offre un **contrôle total sur la mémoire et la performance**.  
- **Face à Java** : Java automatise la gestion mémoire mais sacrifie la performance fine.

### 💪 Forces du C++
- Compilation native pour des performances maximales  
- Gestion mémoire explicite, adaptée aux jeux  
- Portabilité (Windows, Linux, macOS)  
- Large écosystème de bibliothèques (SFML, SDL, Boost, etc.)  
- Support de multiples paradigmes (OO, générique, fonctionnel)

---

## 🎮 Bibliothèque graphique : **SFML 2.6**

### 🔍 Comparaison
| Moteur / Librairie | Avantages | Inconvénients |
|--------------------|------------|----------------|
| **SFML** | Simple, légère, API claire, réseau intégré | Pas de support 3D |
| **Raylib** | Moderne et minimaliste | Moins complet côté audio/réseau |
| **Unity (C#)** | Outils puissants et visuels | Surcouche lourde, dépendances |
| **Unreal Engine 5 (C++)** | Très puissant, AAA-ready | Trop complexe pour un projet 2D |
| **Ncurses** | Ultra-léger, console only | Pas de rendu graphique |

### 💪 Pourquoi SFML
- API claire et orientée objet en C++  
- Gestion intégrée du **graphisme, son, réseau et input**  
- **Légère et rapide**, idéale pour un projet pédagogique  
- Permet de comprendre les **bases d’un moteur de jeu 2D**

---

## 🧪 Tests Unitaires : **Criterion 2.4.1**

### 🔍 Comparaison
| Outil | Langage | Points forts | Limites |
|--------|----------|--------------|----------|
| **Criterion** | C/C++ | Découverte automatique des tests, exécution parallèle | Moins d’écosystème que GoogleTest |
| **GoogleTest** | C++ | Complet, bien documenté | Syntaxe plus lourde |
| **Catch2** | C++ | Header-only, simple à intégrer | Moins performant sur gros projets |
| **Tests fonctionnels / intégration** | N/A | Valident le comportement global | Plus longs, moins précis pour les bugs unitaires |

### 💪 Pourquoi Criterion
- Intégration simple avec **CMake**  
- Résultats de tests lisibles et clairs  
- Supporte l’exécution **parallèle**  
- Complété par des tests fonctionnels (UDP) pour le client/serveur

---

## ⚙️ Build System : **CMake + Ninja**

### 🔍 Comparaison
| Outil | Description | Avantages | Inconvénients |
|--------|-------------|------------|----------------|
| **CMake + Ninja** | Générateur multiplateforme | Rapide, standard industriel, compatible IDE | Syntaxe parfois complexe |
| **Make** | Historique | Simple | Lent, pas portable |
| **Meson + Ninja** | Moderne, syntaxe claire | Rapide | Moins utilisé |
| **Bazel / Premake** | Automatisation avancée | Multiplateforme | Complexe pour petits projets |

### 💪 Pourquoi CMake + Ninja
- **CMake** : standard de facto du C++ moderne  
- **Ninja** : compilations **ultra-rapides** en parallèle  
- Compatibles avec **GitHub Actions**, **VSCode**, **CLion**  
- Adaptés à une **architecture modulaire** (client / serveur / moteur)

---

## 🧱 Architecture : **ECS (Entity Component System)**

### 💪 Avantages
- Architecture **modulaire et extensible**  
- Séparation claire entre données et logique  
- Ajout facile de nouveaux comportements sans modifier les entités existantes  
- Courant dans les moteurs modernes (Unity, Frostbite…)

---

## 🧠 Patterns de Conception
- **Factory** → instanciation dynamique des entités  
- **Singleton** → gestion centralisée (ressources, audio, etc.)  
- **Observer** → communication événementielle entre objets  
- **State Machine** → gestion des états du jeu (menu, gameplay, pause…)  
- **Manager** → orchestration des systèmes (rendu, réseau, entités…)

---

## 🧰 Documentation & Outils

| Outil | Rôle | Pourquoi |
|--------|------|-----------|
| **Doxygen + doxygen-awesome-css** | Génération automatique de documentation | Moderne, lisible, multiplateforme |
| **GitHub Actions** | Intégration continue / déploiement | Builds multi-OS automatisés |
| **cppcheck / clang-tidy** | Analyse statique du code | Détection d’erreurs et optimisation |
| **PlantUML** | Diagrammes UML | Génération rapide et intégrée à la documentation |

---

## 🌍 Plateformes Cibles
- **Linux, macOS, Windows**  
- Compatibilité assurée par **CMake** et **SFML**  
- Tests automatisés via **GitHub Actions** pour garantir la portabilité

---

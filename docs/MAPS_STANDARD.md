## 🌍 Norme de Mapping

### 🗂️ **Organisation des cartes**

Toutes les cartes doivent être placées dans le dossier racine :

```
/assets/maps/
```

Chaque carte doit avoir **son propre dossier**, nommé selon la **convention de nommage suivante** :

| Élément           | Format                                      | Exemple                                    |
| ----------------- | ------------------------------------------- | ------------------------------------------ |
| Nom de la carte   | `map-name` *(minuscules, chiffres, tirets)* | `space-invaders`, `alien-planet`           |
| Fichiers de carte | `.map` et `.def`                            | `space-invaders.map`, `space-invaders.def` |

**Structure attendue :**

```
assets/
    maps/
    map-name/
        map-name.map
        map-name.def
```

---

### 📄 **Fichier `.def` — Map Definition**

Le fichier `.def` décrit les **métadonnées** et la **correspondance symboles → assets** de la carte.

#### 🧩 Format général :

1. **Première ligne** : dimensions de la carte
   * Format : `width,height` (ex : `10,5`)
   * Ou `inf`,`height` pour carte infinie en largeur. (ex : `inf,5`)

2. **Deuxième ligne** : description textuelle de la carte.

   * Exemple : `A space-themed map`

3. **Lignes suivantes** : définitions de tuiles

   * Format : `character asset_path`
   * Exemples :

     ```
     . ./assets/space/empty.png
     # ./assets/space/asteroid.png
     * ./assets/space/planet.png
     ```

#### ⚙️ Règles et contraintes :

* Chaque caractère de définition doit être **unique**.
* Les chemins d’assets sont **relatifs à la racine du projet**.
* Les caractères non définis dans le `.def` :
  * Sont remplacés par **l’asset par défaut dans `assets/maps/default.def`**, s’il existe.
  * Sinon, sont considérés comme **zone vide** (background).

Le moteur charge d’abord assets/maps/default.def (si présent), puis surcharge les caractères définis dans la map locale.

#### 🧠 Exemple :

```txt
10,5
A space-themed map
. ./assets/space/empty.png
# ./assets/space/asteroid.png
* ./assets/space/planet.png
```

---

### 🗺️ **Fichier `.map` — Map Layout**

Le fichier `.map` décrit la **disposition des tuiles** à partir des définitions du `.def`.

#### 🧩 Format général :

* Chaque caractère correspond à une **tuile**.
* Chaque **ligne** correspond à une **rangée** de tuiles.
* Les **espaces** (` `) servent de séparateurs entre les tuiles.
* Les **commentaires** commencent par `///` et sont **ignorés**.
* Les caractères spéciaux (accentués, emoji, etc.) sont **interdits** dans les fichiers `.def` et `.map`. Seuls les caractères ASCII standards sont autorisés.

#### 📐 Cartes finies :

* Le nombre de colonnes et de lignes doit correspondre à la largeur et hauteur définies dans le `.def`.

* Exemple :
  ```txt
  . . . . . . . . . .
  . . # # . . * * . .
  . . # # . . * * . .
  . . . . . . . . . .
  . . . . . . . . . .
  ```

#### ♾️ Cartes infinies :
* La **largeur** est infinie, la **hauteur** reste fixe.
* Le contenu du `.map` est interprété comme un **motif répétitif horizontalement**.
* Il est possible d’utiliser des **groupes aléatoires** :
  * Syntaxe : `[chars]`
    → Chaque tuile sera choisie **aléatoirement** parmi les caractères contenus.
  * Exemple :
    ```txt
    [.*] signifie que chaque occurence de cette tuile peut être '.' ou '*'.
    ```

* Exemple complet :
  ```txt
  [.*] . . [.*]
  [.*] * * [.*]
  ```

---

### 🧾 **Règles supplémentaires**

* Tous les fichiers `.def` et `.map` doivent être **UTF-8 sans BOM**.
* Les fichiers `.map` ne doivent **jamais contenir de tabulations** (`\t`) — uniquement des **espaces simples**.
* Les chemins d’assets ne doivent **jamais contenir d’espaces**.

---

### 📚 **Bonnes pratiques**

* Toujours valider le format du `.def` avant de commencer la création du `.map`.
* Tester les cartes avec une **map de test minimaliste** avant de les envoyer dans le repo.
* En cas de modification d’un symbole ou d’un asset dans un `.def`, vérifier :
  * que le caractère est cohérent dans tout le `.map`,
  * et qu’aucun autre fichier `.def` ne définit le même caractère pour un autre usage (cohérence inter-maps).

---

### 💡 **Exemple complet**

#### `/assets/maps/space-invaders/space-invaders.def`

```txt
10,5
A space-themed map
. ./assets/space/empty.png
# ./assets/space/asteroid.png
* ./assets/space/planet.png
```

#### `/assets/maps/space-invaders/space-invaders.map`

```txt
. . . . . . . . . .
. . # # . . * * . .
. . # # . . * * . .
. . . . . . . . . .
. . . . . . . . . .
```

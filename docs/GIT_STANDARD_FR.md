# Norme Git
*Version originale en anglais : [docs/GIT_STANDARD.md](./GIT_STANDARD.md)*

## Nomenclatures
Inspirée de la norme Conventional Commit [ici](https://www.conventionalcommits.org/fr/v1.0.0-beta.3/).  
Elle est organisée en deux parties.

### Norme de commits
Format :
`{action}: {brief} {related issue(s)}`

- **{action}** = Type d’action du commit parmi :
  - feat
  - fix
  - refacto
  - docs
- **{brief}** = Résumé clair et concis de l’action
- **{related issue(s)}** = Référence(s) des issues concernées sous forme : `#numéro`

Exemple :  
`feat: ajout du formulaire de connexion #42`

---

### Norme de branches
Format :
`{type}/{scope}/{topic}`

- **{type}** = Type d’action :
  - feat
  - fix
  - refacto
  - docs
- **{scope}** = Domaine ou composant concerné (exemples) :
  - cicd
  - graphic
  - core
  - server
  - (extensible selon les besoins)
- **{topic}** = Sujet de la branche (max 3 mots-clés simples)

Exemple :  
`feat/server/login-api`

---

## Norme de merges
*Note : les Pull Requests sont parfois appelées PR ou MR (Merge Request).*

1. **Tous les merges passent par une Pull Request**, avec review de Copilot + 1 à 2 personnes selon l’importance.
1.2. **Exception : les merges vers `main`** se font en daily et nécessitent validation collective (de `dev` vers `main`).

2. **Les merges se font toujours avec _squash and commit_**, après :
   - validation des reviewers humains,
   - prise en compte des retours IA.

3. **Toujours générer un résumé Copilot** dans la description de la PR.
3.2. En bas de la description de la PR, ajouter :
```md
closes #numéro_issue
```

4. **Ajouter la PR dans le GitHub Project** :
   - Via l’onglet “Development”,
   - Déplacer le ticket créé dans la colonne **“In Review”**.
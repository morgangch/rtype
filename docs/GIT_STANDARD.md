# Git Standard
*French version: [docs/GIT_STANDARD_FR.md](./GIT_STANDARD_FR.md)*

## Nomenclature
Inspired by the Conventional Commit standard [here](https://www.conventionalcommits.org/en/v1.0.0-beta.3/).  
It is organized in two parts.

### Commit Standard
Format:
`{action}: {brief} {related issue(s)}`

- **{action}** = Type of commit action among:
  - feat
  - fix
  - refacto
  - docs
- **{brief}** = Clear and concise summary of the action
- **{related issue(s)}** = Reference(s) to related issues in format: `#number`

Example:  
`feat: add login form #42`

---

### Branch Standard
Format:
`{type}/{scope}/{topic}`

- **{type}** = Type of action:
  - feat
  - fix
  - refacto
  - docs
- **{scope}** = Domain or component concerned (examples):
  - cicd
  - graphic
  - core
  - server
  - (extensible as needed)
- **{topic}** = Branch topic (max 3 simple keywords)

Example:  
`feat/server/login-api`

---

## Merge Standard
*Note: Pull Requests are sometimes called PR or MR (Merge Request).*

1. **All merges go through a Pull Request**, with review by Copilot + 1 to 2 people depending on importance.
1.2. **Exception: merges to `main`** are done daily and require collective validation (from `dev` to `main`).

2. **Merges are always done with _squash and commit_**, after:
   - validation from human reviewers,
   - taking AI feedback into account.

3. **Always generate a Copilot summary** in the PR description.
3.2. At the bottom of the PR description, add:
```md
closes #issue_number
```

4. **Add the PR to the GitHub Project**:
   - Via the "Development" tab,
   - Move the created ticket to the **"In Review"** column.

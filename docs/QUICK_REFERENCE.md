# 🚀 R-Type Contributor Quick Reference

> Quick commands and checklists for daily development

## 📝 Daily Workflow

### Starting Work
```bash
# 1. Pull latest changes
git checkout dev
git pull origin dev

# 2. Create feature branch
git checkout -b feat/scope/my-feature

# 3. Make changes...
```

### Committing Changes
```bash
# Stage changes
git add .

# Commit with proper format
git commit -m "feat: add authentication system #42"
#              ↑     ↑                          ↑
#            action  description            issue #

# Push to remote
git push -u origin feat/scope/my-feature
```

### Creating Pull Request
1. Go to GitHub and create PR
2. Add Copilot summary to description
3. Add `closes #issue_number` at bottom
4. Link to GitHub Project (Development tab)
5. Move ticket to "In Review"

---

## 📋 Commit Actions

| Action | Use Case | Example |
|--------|----------|---------|
| `feat` | New feature | `feat: add user login #42` |
| `fix` | Bug fix | `fix: resolve memory leak #87` |
| `refacto` | Code refactoring | `refacto: optimize parser #65` |
| `docs` | Documentation | `docs: update API docs #91` |

---

## 🌿 Branch Naming

```
{type}/{scope}/{topic}
  ↓       ↓       ↓
feat/server/auth-system
fix/client/crash-on-start
refacto/ecs/components
docs/mapparser/examples
```

**Common Scopes:**
- `server`, `client`, `ecs`, `mapparser`
- `cicd`, `tests`, `network`, `graphics`

---

## ✅ Pre-Commit Checklist

- [ ] Code compiles: `cmake --build build`
- [ ] Tests pass: `cd build && ctest`
- [ ] Style is correct (no warnings)
- [ ] Documentation updated
- [ ] Following naming conventions

---

## 🧪 Testing

```bash
# Build project
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)

# Run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/test_mapparser
```

---

## 📅 Meetings

### Daily (Tue-Wed)
- **10:00 AM** (15 min max)
- 3 questions: Yesterday? Today? Blockers?
- ➡️ Retranscribe on Notion

### Weekly (Monday)
- **2:00 PM** (30-60 min)
- Sprint planning & task assignment
- `dev → main` merge if ready
- ➡️ Retranscribe on Notion

---

## 🎯 Naming Conventions

```cpp
class PlayerManager          // PascalCase
void getPlayerName()         // camelCase
int player_count = 0;        // snake_case
const int MAX_PLAYERS = 100; // UPPERCASE
namespace network { }        // lowercase
```

---

## 📖 Documentation Template

```cpp
/**
 * @brief Brief description.
 * @param param_name Parameter description.
 * @return Return value description.
 * @throws std::exception If error occurs.
 */
void myFunction(int param_name);
```

---

## 🚫 Common Mistakes

❌ `#pragma once` → ✅ Use `#ifndef`
❌ `using namespace std;` → ✅ Use `std::` prefix
❌ Direct merge to main → ✅ PR with reviews

---

## 🆘 Quick Help

| Need | Resource |
|------|----------|
| Full guide | [CONTRIBUTING.md](./CONTRIBUTING.md) |
| Git rules | [docs/GIT_STANDARD.md](./docs/GIT_STANDARD.md) |
| Code style | [docs/CODING_STYLE.md](./docs/CODING_STYLE.md) |
| Team org | [docs/ORGANISATION.md](./docs/ORGANISATION.md) |
| Map format | [docs/MAPS_STANDARD.md](./docs/MAPS_STANDARD.md) |
| Conduct | [docs/CODE_OF_CONDUCT.md](./docs/CODE_OF_CONDUCT.md) |

---

## 💬 Communication

- **Technical**: Discord
- **Absence/Delay**: Discord `#absent-retard`
- **Votes**: Discord `#sondage`
- **Documentation**: Notion
- **Code**: GitHub PRs
- **Tickets**: GitHub Issues/Projects

---

## ⚡ Speed Commands

```bash
# Quick build
alias rbuild='rm -rf build && cmake -B build -G Ninja && cmake --build build --parallel $(nproc)'

# Run tests
alias rtest='cd build && ctest --output-on-failure'

# Format check
alias rcheck='cppcheck client server lib'
```

---

**Save this file and keep it handy! 📌**

For detailed information, always refer to [CONTRIBUTING.md](./CONTRIBUTING.md)

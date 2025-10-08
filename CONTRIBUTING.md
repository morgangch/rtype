# Contributing to R-Type

Thank you for your interest in contributing to the R-Type project! This document provides all the guidelines and standards you need to follow when contributing to this project.

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Git Standards](#git-standards)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Communication](#communication)
- [Review Process](#review-process)

---

## 📜 Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](./docs/CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [morgan.guichard@epitech.eu](mailto:morgan.guichard@epitech.eu).

**Key Principles:**
- Be respectful and inclusive
- Accept constructive feedback gracefully
- Focus on what is best for the community
- Show empathy towards other community members

---

## 🚀 Getting Started

### Prerequisites

- **C++ Compiler**: GCC 13.3.0 or later
- **CMake**: Version 3.20 or later
- **Ninja**: Build system
- **SFML**: Version 2.5.1 or later
- **Criterion**: Testing library (optional, for full test suite)

### Building the Project

```bash
# Clone the repository
git clone https://github.com/morgangch/rtype.git
cd rtype

# Build the project
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)

# Run tests
cd build
ctest --output-on-failure
```

### Project Structure

```
rtype/
├── client/          # Client application
├── server/          # Server application
├── common/          # Shared components and utilities
├── lib/             # Internal libraries (ECS, MapParser, etc.)
├── tests/           # Unit tests
├── examples/        # Example programs and demos
├── assets/          # Game assets (maps, fonts, etc.)
└── docs/            # Documentation
```

---

## 🔄 Development Workflow

### Team Organization

#### Daily Stand-ups (Tuesday & Wednesday)
- **Time**: 10:00 AM (latest)
- **Duration**: 15 minutes max
- **Format**: Answer 3 questions:
  1. What did I do yesterday?
  2. What will I do today?
  3. Any blockers or issues?
- **Mandatory**: Retranscription on Notion after each daily
- **Attendance**: Strongly recommended (not mandatory)
- **If absent**: Must catch up on Notion retranscription

#### Weekly Meetings (Monday)
- **Time**: 2:00 PM (latest)
- **Duration**: 30-60 minutes
- **Objectives**:
  - Define sprint goals and tasks
  - Assign roles and tickets
  - Perform `dev → main` merge if conditions are met
- **Attendance**: Mandatory for all active members
- **Mandatory**: Retranscription on Notion after each weekly

### Task Management

- All tasks are managed via **GitHub Projects**
- Each ticket must include:
  - ✅ Clear description of requirements
  - 👤 One or more assignees
  - 📅 Realistic deadline
  - 🎯 Milestone (sprint) if applicable
- Link tickets to PRs via the "Development" tab
- Move tickets through workflow:
  - **To Do** → **In Progress** → **In Review** → **Done**

---

## 🌿 Git Standards

Our Git workflow follows [Conventional Commits](https://www.conventionalcommits.org/).

### Commit Message Format

```
{action}: {brief description} {related issue(s)}
```

**Actions:**
- `feat`: New feature
- `fix`: Bug fix
- `refacto`: Code refactoring
- `docs`: Documentation changes

**Examples:**
```bash
feat: add user authentication system #42
fix: resolve memory leak in ECS component #87
refacto: optimize map parser performance #65
docs: update API documentation for MapParser #91
```

### Branch Naming Convention

```
{type}/{scope}/{topic}
```

**Structure:**
- **type**: `feat`, `fix`, `refacto`, `docs`
- **scope**: Component/domain (e.g., `server`, `client`, `ecs`, `mapparser`, `cicd`)
- **topic**: Max 3 keywords describing the branch

**Examples:**
```bash
feat/server/authentication-system
fix/client/rendering-crash
refacto/ecs/component-manager
docs/mapparser/api-reference
```

### Pull Request Process

1. **Create Pull Request**
   - Always create a PR for merging (no direct commits to `dev` or `main`)
   - Use descriptive title following commit conventions
   - Add `closes #issue_number` at the bottom of the description

2. **Generate Copilot Summary**
   - Always generate a GitHub Copilot summary in the PR description
   - Include technical details and changes made

3. **Add to GitHub Project**
   - Link PR via "Development" tab
   - Move associated ticket to **"In Review"** column

4. **Reviews Required**
   - Minimum: GitHub Copilot review + 1-2 human reviewers
   - Exception: Merges to `main` require team consensus (daily meeting)

5. **Merge Strategy**
   - Always use **Squash and Merge**
   - Only after all reviews are approved
   - Address all AI and human feedback

6. **Merge to Main**
   - From `dev` to `main` only
   - Performed during weekly meetings
   - Requires collective validation

---

## 💻 Coding Standards

Full details in [CODING_STYLE.md](./docs/CODING_STYLE.md).

### Documentation

All code must be documented using **Doxygen**:

```cpp
/**
 * @brief Brief description of the function or class.
 * @param param_name Description of the parameter.
 * @return Description of return value.
 * @throws Exception(s) if applicable.
 */
```

**Requirements:**
- Documentation in **English** only
- Each `.h` file starts with `@file` block
- Document all public classes, methods, and functions

### Naming Conventions

| Element    | Convention | Example                  |
|------------|------------|--------------------------|
| Classes    | PascalCase | `PlayerManager`          |
| Methods    | camelCase  | `getPlayerName()`        |
| Variables  | snake_case | `player_count`           |
| Constants  | UPPERCASE  | `MAX_PLAYERS`            |
| Namespaces | lowercase  | `namespace network {...}`|

### Code Quality Rules

- ✅ Use `.cpp` for source files, `.h` for headers
- ✅ Use `#ifndef` guards (NEVER `#pragma once`)
- ❌ NO `using namespace std;`
- ✅ Always initialize variables
- ✅ Follow MISRA C++ and CERT C++ guidelines
- ✅ Pass `cppcheck` and `clang-tidy` validation

### File Structure

```cpp
/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Brief description of the file
*/

/**
 * @file filename.h
 * @brief Purpose of this file.
 */

#ifndef FILENAME_H
#define FILENAME_H

// Includes
// Class/function declarations

#endif // FILENAME_H
```

### Libraries and Modularity

- Each reusable feature → separate library in `/lib/`
- Each library must:
  - Have its own `CMakeLists.txt`
  - Include headers in `/include/libname/`
  - Use a dedicated namespace
- Minimize dependencies between modules

---

## 🧪 Testing

### Test Requirements

- All tests in `/tests/` directory
- Each PR must include tests for new features
- All tests must pass before merge
- Test coverage should be maintained or improved

### Running Tests

```bash
# Build and run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/test_mapparser

# Run with verbose output
ctest -R MapParserTest --verbose
```

### Writing Tests

Follow the project's test style (assert-based):

```cpp
int test_feature() {
    std::cout << "[TEST] Feature description..." << std::endl;
    
    // Setup
    YourClass instance;
    
    // Test
    int result = instance.someMethod();
    
    // Assertions
    assert(result == expected_value);
    
    std::cout << "  PASS: Test description" << std::endl;
    return 0;
}
```

### CI/CD Pipeline

Every PR triggers automatic checks:
- ✅ Project compilation (CMake + Ninja)
- ✅ Unit tests execution
- ✅ Code style verification (`cppcheck`, `clang-tidy`)

**Merge is blocked if:**
- ❌ Tests fail
- ❌ Style checks return errors
- ❌ Build fails

---

## 📚 Documentation

### Required Documentation

When adding new features, update:

1. **Code Documentation** (Doxygen comments)
2. **API Documentation** (if library/public API)
3. **README files** (in relevant directories)
4. **Examples** (in `/examples/` if applicable)
5. **This CONTRIBUTING.md** (if workflow changes)

### Map Files Standard

When creating maps, follow [MAPS_STANDARD.md](./docs/MAPS_STANDARD.md):

- Place maps in `/assets/maps/`
- Each map in its own directory
- Include both `.def` and `.map` files
- Use ASCII characters only
- No tabs in `.map` files (spaces only)

Example:
```
assets/maps/
└── my-map/
    ├── my-map.def    # Definitions
    └── my-map.map    # Layout
```

---

## 💬 Communication

### Channels

- **Discord**: All technical communication
  - `#absent-retard`: Report absences/delays
  - `#sondage`: Team votes on technical decisions
- **Notion**: Meeting retranscriptions and documentation
- **GitHub**: Code reviews, issues, PRs

### Best Practices

- 🕐 Respect meeting schedules
- 💬 Avoid digressions during stand-ups
- 🔔 Never block others without communication
- 📊 Update GitHub Projects as tasks progress
- 🗳️ Use team votes for unresolved technical disagreements

### Reporting Issues

When opening an issue:

1. **Check existing issues** first
2. **Use a clear title** describing the problem
3. **Provide details**:
   - Steps to reproduce
   - Expected vs actual behavior
   - Environment (OS, compiler, etc.)
   - Code snippets if applicable
4. **Add labels** (bug, enhancement, documentation, etc.)
5. **Assign to milestone** if applicable

---

## 👀 Review Process

### As a Reviewer

- ✅ Check code quality and style
- ✅ Verify tests are included and passing
- ✅ Ensure documentation is updated
- ✅ Test the changes locally if possible
- ✅ Provide constructive feedback
- ✅ Approve only when fully satisfied

### As a Contributor

- ✅ Respond to all review comments
- ✅ Make requested changes promptly
- ✅ Re-request review after updates
- ✅ Keep PR scope focused (one feature/fix per PR)
- ✅ Keep commits clean and well-organized

### Review Checklist

Before approving a PR, verify:

- [ ] Code follows style guidelines
- [ ] All tests pass
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] No unnecessary files committed
- [ ] Commit messages follow conventions
- [ ] PR description is clear and complete
- [ ] Copilot summary generated
- [ ] Related issues linked

---

## 🎯 Quick Reference

### Starting a New Feature

```bash
# 1. Create branch
git checkout -b feat/scope/feature-name

# 2. Make changes and commit
git add .
git commit -m "feat: add feature description #issue"

# 3. Push and create PR
git push origin feat/scope/feature-name
# Then create PR on GitHub

# 4. Link to GitHub Project and move to "In Review"
```

### Before Submitting PR

- [ ] Code compiles without warnings
- [ ] All tests pass locally
- [ ] Documentation updated
- [ ] Code follows style guide
- [ ] Commit messages follow convention
- [ ] Branch up to date with `dev`

### After PR Approved

```bash
# Squash and merge via GitHub interface
# Delete branch after merge
git branch -d feat/scope/feature-name
git push origin --delete feat/scope/feature-name
```

---

## 📖 Additional Resources

- [Git Standards](./docs/GIT_STANDARD.md) - Detailed Git workflow
- [Coding Style](./docs/CODING_STYLE.md) - Complete coding standards
- [Organization](./docs/ORGANISATION.md) - Team organization and meetings
- [Maps Standard](./docs/MAPS_STANDARD.md) - Map file format specification
- [Code of Conduct](./docs/CODE_OF_CONDUCT.md) - Community guidelines

---

## 🤝 Getting Help

### Technical Issues

1. Check existing documentation
2. Search closed issues on GitHub
3. Ask in Discord technical channels
4. Create a new issue if problem persists

### Process Questions

1. Refer to this CONTRIBUTING.md
2. Check specific docs (GIT_STANDARD, CODING_STYLE, etc.)
3. Ask team lead: @mrGonzalezGomez on Discord
4. Discuss in weekly meeting if needed

---

## 🎉 Recognition

Contributors who follow these guidelines and actively participate in the project will be recognized in:
- Project README
- Release notes
- Team meetings

Thank you for contributing to R-Type! Your efforts help make this project better for everyone. 🚀

---

**Questions or concerns?** Contact the project maintainer: [morgan.guichard@epitech.eu](mailto:morgan.guichard@epitech.eu)

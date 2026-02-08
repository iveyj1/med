# AGENTS.md

Guidance for agentic coding assistants working in this repository.

## Scope
- Applies to `/home/user/dev/iv`.
- Main language is C (ANSI terminal helpers + editor prototype).
- Prefer small, surgical changes over broad refactors unless requested.

## Repository Snapshot (Important)
- Build system: CMake (`CMakeLists.txt`).
- Current CMake source collection is recursive: `file(GLOB_RECURSE sources "*.c")`.
- That glob currently includes `ansi/*.c` and generated files under `build/`.
- Result: CMake default link step fails with multiple `main` definitions.
- Reliable workaround for now: compile targets directly with `gcc` commands below.
- `ansi/` is a nested sample/copy project and has its own `.git` directory.
- Avoid editing generated/cache paths (`build/`, `.cache/`) unless explicitly asked.

## Cursor / Copilot Rules
- `.cursor/rules/`: not present.
- `.cursorrules`: not present.
- `.github/copilot-instructions.md`: not present.
- If these files appear later, treat them as higher-priority instructions.

## Build, Lint, and Test Commands

### Environment
- Compiler: `gcc` (GNU 13.x observed).
- Language mode: `-std=c11`.
- Baseline warnings: `-Wall -Wextra -Wpedantic`.

### Build (Recommended)
Build the main executable directly:
```bash
mkdir -p build
gcc -Wall -Wextra -Wpedantic -std=c11 main.c ansi_escapes.c -I. -o build/iv
```
Run:
```bash
./build/iv
```

### Build (CMake, currently broken)
Configure:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```
Build:
```bash
cmake --build build
```
Known issue: recursive globbing pulls test/sample/generated `.c` files.

### Clean
Use script:
```bash
./cleanall
```
Manual equivalent:
```bash
rm -rf build/*
```

### Lint / Static Analysis
No dedicated lint target exists yet.

Use compile warnings as lint:
```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -fsyntax-only main.c ansi_escapes.c -I.
```
Optional, if installed:
```bash
clang-tidy main.c ansi_escapes.c -- -I. -std=c11
```
Optional format check, if installed:
```bash
clang-format --dry-run --Werror main.c ansi_escapes.c ansi_escapes.h
```

### Tests
- No CTest tests are currently registered.
- Files in `ansi/t0.c` ... `ansi/t3.c` are executable behavior samples.

Single-sample run (current single-test equivalent):
```bash
mkdir -p build
gcc -Wall -Wextra -Wpedantic -std=c11 ansi/t2b.c ansi/ansi_escapes.c -Iansi -o build/t2b
./build/t2b
```
To run a different sample, replace `t2b.c` and output name.

### Single Test Command (Future CTest)
When tests are added with `add_test(...)`:

Run all tests:
```bash
ctest --test-dir build --output-on-failure
```
Run one test:
```bash
ctest --test-dir build -R "^test_name$" --output-on-failure
```

## Code Style Guidelines

### Includes / Imports
- One include per line.
- Include order: C standard headers first, then project headers.
- Keep include lists minimal; remove unused headers.
- Use quoted includes for local headers, e.g. `"ansi_escapes.h"`.

### Formatting
- Indentation: 4 spaces, no tabs.
- Opening brace stays on the same line (`if (...) {`, `int f(void) {`).
- Always include a space before `{` and after control keywords.
- Prefer lines <= 100 chars.
- Use blank lines between logical blocks (not between every statement).
- Avoid trailing whitespace.

### Types and Data
- Use `size_t` for sizes/indices where appropriate.
- Use `bool` with `<stdbool.h>` for boolean state.
- Avoid implicit narrowing conversions; cast deliberately when needed.
- Prefer explicit struct initialization (designated initializers when useful).
- Keep global mutable state minimal and justified.

### Naming
- Functions: `lower_snake_case`.
- Variables: `lower_snake_case`.
- Constants/macros/enum constants: `UPPER_SNAKE_CASE`.
- Files: `lower_snake_case.c` / `lower_snake_case.h`.
- Internal helpers should be `static` in their implementation file.

### Error Handling
- Check return values from I/O, terminal, and allocation/system calls.
- Print actionable errors to `stderr`.
- Prefer returning error codes from helper functions instead of calling `exit()`.
- Ensure terminal state is restored on every exit path after `setupConsole()`.
- Validate pointer inputs before dereferencing in public/helper APIs.

### Resource Management
- Pair each acquire with a clear release path.
- Prevent leaks on early returns (cleanup label is acceptable).
- Keep buffer operations bounded.
- Avoid unchecked fixed-size buffer writes/growth.

### Portability
- Keep `_WIN32` conditionals localized in `.c` files.
- Expose a cross-platform interface in headers; hide platform details in `.c`.
- Preserve behavior parity between POSIX and Windows paths.
- Add guarded fallbacks for platform-specific behavior.

### Testing Expectations for New Code
- For new behavior, add or update at least one runnable verification path.
- Prefer CTest registration once CMake source selection is fixed.
- Use descriptive test names so `ctest -R` remains usable.
- If automated tests are absent, document manual verification commands.

## Agent Workflow Checklist
- Read relevant files first; follow local patterns.
- Make the smallest change that fully solves the task.
- Run relevant build/lint/test commands after edits.
- Prefer direct `gcc` compile checks until CMake globbing is fixed.
- In final notes, list exact commands run and outcomes.
- If a command cannot run, explain why and provide the exact fallback command.

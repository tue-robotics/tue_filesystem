# Skill: Migrate a downstream package off `tue_filesystem` to `std::filesystem`

> **Purpose:** Replace every use of the `tue_filesystem` package (C++ classes
> `tue::filesystem::Path` and `tue::filesystem::Crawler`) in a downstream
> ROS/ROS 2 package with the C++17 standard `<filesystem>` library, then remove
> the dependency. Preserve behaviour exactly. Make minimal, surgical changes.

---

## 0. Preconditions

1. The package compiles with **C++17 or later**. Verify in `CMakeLists.txt`:
   - Look for `set(CMAKE_CXX_STANDARD …)` or
     `target_compile_features(... cxx_std_…)`. If absent or set to <17, raise
     it to 17 (`set(CMAKE_CXX_STANDARD 17)` guarded by
     `if(NOT CMAKE_CXX_STANDARD)`).
2. Confirm the target toolchain ships `<filesystem>` without a separate link
   flag (GCC ≥ 9, Clang ≥ 9). No `-lstdc++fs` needed for these compilers.
3. Work on a fresh branch. Do **not** touch unrelated code.

## 1. Discover all usages

Run, from the package root, in this order:

```bash
grep -rn "tue/filesystem"  include src test \
    --include='*.h' --include='*.hpp' --include='*.cpp' --include='*.cc'
grep -rn "tue::filesystem" include src test \
    --include='*.h' --include='*.hpp' --include='*.cpp' --include='*.cc'
grep -rn "tue_filesystem"  CMakeLists.txt package.xml cmake/ 2>/dev/null
```

Build a checklist of:

- Every file with an `#include "tue/filesystem/..."` line.
- Every reference to `tue::filesystem::Path` or `tue::filesystem::Crawler`.
- Every CMake `find_package`, `target_link_libraries`,
  `ament_target_dependencies`, `catkin_package(... DEPENDS/CATKIN_DEPENDS ...)`
  mention of `tue_filesystem`.
- Every `<depend>`, `<build_depend>`, `<exec_depend>`, `<run_depend>` in
  `package.xml`.

If the package does **not** use `tue::filesystem::Path` or
`tue::filesystem::Crawler` in code but still declares the dependency, skip to
§5 (CMake/package.xml cleanup) only.

## 2. Header substitutions

Replace includes:

| Remove                                  | Add                      |
| --------------------------------------- | ------------------------ |
| `#include "tue/filesystem/path.h"`      | `#include <filesystem>`  |
| `#include "tue/filesystem/crawler.h"`   | `#include <filesystem>`  |
| `#include <tue/filesystem/...>` (any)   | `#include <filesystem>`  |

- De-duplicate `#include <filesystem>` if multiple replacements collapse to one.
- Sort includes consistent with the package's existing style (or, if the
  package uses `clang-format`, run it on the touched files after edits).
- Do **not** add `using namespace std::filesystem;`. Prefer the alias
  `namespace fs = std::filesystem;` only when it already matches local
  convention; otherwise spell out `std::filesystem::` per call site.

## 3. `tue::filesystem::Path` → `std::filesystem::path`

The wrapper stores a `std::string` and delegates to Boost. Behaviour is
identical to `std::filesystem::path` **except** for `parentPath()`,
`lastWriteTime()`, and the `join` operator (see notes). Apply these mappings
everywhere:

| `tue::filesystem::Path` API                                | `std::filesystem::path` replacement                                                                                                                                                                                                                                                                                  |
| ---------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `tue::filesystem::Path` (the type)                         | `std::filesystem::path`                                                                                                                                                                                                                                                                                              |
| `Path()` / `Path(const std::string&)` / `Path(const char*)`| Same ctors on `std::filesystem::path`                                                                                                                                                                                                                                                                                |
| `p.string()`                                               | `p.string()` (already on `std::filesystem::path`)                                                                                                                                                                                                                                                                    |
| `p.extension()` (returns `std::string`)                    | `p.extension().string()`                                                                                                                                                                                                                                                                                             |
| `p.filename()` (returns `std::string`)                     | `p.filename().string()`                                                                                                                                                                                                                                                                                              |
| `p.parentPath()`                                           | `p.parent_path()` — **but** see "parent_path quirk" below                                                                                                                                                                                                                                                            |
| `p.exists()`                                               | `std::filesystem::exists(p)`                                                                                                                                                                                                                                                                                         |
| `p.isRegularFile()`                                        | `std::filesystem::is_regular_file(p)`                                                                                                                                                                                                                                                                                |
| `p.isDirectory()`                                          | `std::filesystem::is_directory(p)`                                                                                                                                                                                                                                                                                   |
| `p.lastWriteTime()` (returns `std::time_t`)                | See "lastWriteTime quirk" below                                                                                                                                                                                                                                                                                      |
| `p.removeExtension()` (mutating, returns `Path&`)          | `p.replace_extension();` — note the return type changes from `Path&` to `path&`, which is fine for the common `p.removeExtension();` use and for chained `.string()`. If the result is assigned (`Path q = p.removeExtension();`), use `p.replace_extension(); auto q = p;`.                                         |
| `p.withoutExtension()` (non-mutating)                      | `std::filesystem::path(p).replace_extension()` — wrap in a small inline lambda only if used many times                                                                                                                                                                                                               |
| `p.join(q)`                                                | `p / q` (operator/). **Caution:** if `q` is absolute, `p / q` yields `q`. The wrapper's hand-rolled `'/'` concatenation does **not** behave this way — verify each call site uses relative right-hand operands, which is the documented intent.                                                                      |
| `out << p;` (stream insertion)                             | Same — `std::filesystem::path` defines `operator<<` (it quotes the path). If the exact unquoted output matters (it almost never does for logs), write `out << p.string();`.                                                                                                                                          |

### parent_path quirk

`tue::filesystem::Path::parentPath()` returns `"."` when the parent is empty;
`std::filesystem::path::parent_path()` returns an empty path. If the call site
relies on the `"."` (e.g. it is passed to `exists()`, used as a directory
argument to `chdir`, or printed), preserve the behaviour locally:

```cpp
auto parent = p.parent_path();
if (parent.empty()) parent = ".";
```

Only insert this when needed. For most uses (passing into other
`std::filesystem` calls) the empty path is equivalent.

### lastWriteTime quirk

`tue::filesystem::Path::lastWriteTime()` returns `std::time_t`.
`std::filesystem::last_write_time(p)` returns
`std::filesystem::file_time_type`. Replace as follows:

- **If the call site needs a `time_t`** (passed to `std::localtime`,
  `difftime`, an API expecting `time_t`, etc.), insert this conversion at the
  call site:

  ```cpp
  // C++17-portable conversion: assumes file_clock epoch maps to system_clock
  // epoch (true on glibc/libstdc++ and libc++ on Linux).
  auto ftime = std::filesystem::last_write_time(p);
  auto sctp  = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                   ftime - std::filesystem::file_time_type::clock::now()
                   + std::chrono::system_clock::now());
  std::time_t t = std::chrono::system_clock::to_time_t(sctp);
  ```

  If the package's standard is C++20 or later, prefer the cleaner:

  ```cpp
  auto t = std::chrono::system_clock::to_time_t(
               std::chrono::clock_cast<std::chrono::system_clock>(
                   std::filesystem::last_write_time(p)));
  ```

- **If the call site only compares two timestamps**, compare the
  `file_time_type` values directly — no conversion needed.

Do not introduce a global helper unless the conversion appears three or more
times in the package; otherwise inline it.

## 4. `tue::filesystem::Crawler` → `std::filesystem::recursive_directory_iterator`

The `Crawler` is a pull-style iterator with five toggles. Replace it with
`std::filesystem::recursive_directory_iterator` plus filtering. Behaviour to
preserve:

- Default: walk recursively, ignore hidden files and directories (name starts
  with `.`), yield regular files only.
- Configurable via `setRecursive`, `setIgnoreHiddenDirectories`,
  `setIgnoreHiddenFiles`, `setListDirectories`, `setListFiles`.
- Permission-denied / unreadable entries are skipped (not fatal).

### Recommended translation

Replace this pattern:

```cpp
tue::filesystem::Crawler crawler(root);
crawler.setListDirectories();   // and other toggles in whatever combination
tue::filesystem::Path p;
while (crawler.nextPath(p)) {
    // use p (a tue::filesystem::Path)
}
```

with this range-for over the standard iterator (template — keep only the
matching `if`s for the toggles that were set):

```cpp
namespace fs = std::filesystem;

auto opts = fs::directory_options::skip_permission_denied;
for (auto it = fs::recursive_directory_iterator(root, opts);
     it != fs::recursive_directory_iterator();
     ++it)
{
    const auto& entry = *it;
    const auto  name  = entry.path().filename().string();
    const bool  is_hidden = !name.empty() && name.front() == '.';

    if (entry.is_directory()) {
        if (/* ignore_hidden_dirs */ is_hidden) { it.disable_recursion_pending(); continue; }
        if (!/* recursive */ true)              { it.disable_recursion_pending(); }
        if (!/* list_dirs */ false) continue;
    } else if (entry.is_regular_file()) {
        if (!/* list_files */ true) continue;
        if (/* ignore_hidden_files */ is_hidden) continue;
    } else {
        continue;
    }

    const fs::path& p = entry.path();
    // ... use p ...
}
```

Rules when applying this template:

1. Inspect each `Crawler` instance's toggle calls; keep only the conditions
   that correspond to non-default settings (or hard-code the matching
   defaults).
2. If `setRecursive(false)` is used, prefer `std::filesystem::directory_iterator`
   (non-recursive) — it is simpler and matches intent.
3. Preserve the logging behaviour. The original logs `ex.what()` via
   `console_bridge` when an iterator step throws. The
   `directory_options::skip_permission_denied` flag handles the common case
   silently; if explicit logging is required, wrap the body in
   `try { ... } catch (const std::filesystem::filesystem_error& e) { /* package's logger */ }`
   matching the package's existing logging mechanism (do not introduce a new
   logging dependency).
4. If multiple call sites share identical filter logic, extract a small free
   function inside the package — do not create a new public API mimicking
   `Crawler` unless the package already exports such a wrapper.

## 5. CMake changes

In every `CMakeLists.txt` of the package:

1. Remove `find_package(tue_filesystem REQUIRED)` (and any
   conditional/`COMPONENTS` variants).
2. Remove `tue_filesystem` from:
   - `target_link_libraries(...)` (any form, including
     `tue_filesystem::tue_filesystem`)
   - `ament_target_dependencies(...)`
   - `catkin_package(... CATKIN_DEPENDS ...)` and `... DEPENDS ...`
   - `${catkin_LIBRARIES}` is unaffected (it's a variable, not a token to
     remove).
3. Ensure C++17 is in effect (see §0). No new library link is required for
   `<filesystem>` on GCC ≥ 9 / Clang ≥ 9. If the package supports older
   toolchains, add — guarded — `target_link_libraries(<tgt> PRIVATE stdc++fs)`
   for GCC 8 and `c++fs` for older libc++.
4. Re-run any package-local linters/formatters that the package configures
   (e.g. `ament_lint_cmake`, `clang-format`) on the modified files.

## 6. `package.xml` changes

Remove every line referencing `tue_filesystem`:

- `<depend>tue_filesystem</depend>`
- `<build_depend>tue_filesystem</build_depend>`
- `<exec_depend>tue_filesystem</exec_depend>` /
  `<run_depend>tue_filesystem</run_depend>`
- `<build_export_depend>tue_filesystem</build_export_depend>`

Do **not** add any new dependency — `<filesystem>` is part of the standard
library.

## 7. Tests

- Do not delete or relax existing tests.
- If tests use `tue::filesystem::Path` / `Crawler`, apply the same
  substitutions from §3 and §4 inside the test sources.
- Add no new tests unless the package's policy mandates regression tests for
  refactors.

## 8. Validation

Run, in order, only what the package already supports:

1. `colcon build --packages-select <pkg>` (ROS 2) **or**
   `catkin build <pkg>` / `catkin_make` (ROS 1).
2. `colcon test --packages-select <pkg> && colcon test-result --verbose` (or
   the ROS 1 equivalent).
3. Whatever linters the package configures (`ament_clang_format`,
   `ament_clang_tidy`, `ament_lint_cmake`, `ament_xmllint`, `catkin_lint`,
   etc.). Fix only issues introduced by your changes; do not fix unrelated
   pre-existing warnings.
4. Confirm no remaining matches:

   ```bash
   grep -rn "tue/filesystem\|tue::filesystem\|tue_filesystem" .
   ```

   The only acceptable hits are in changelog/release notes you are
   intentionally writing.

## 9. Commit

One commit (or small logical series), message:

> `Migrate from tue_filesystem to std::filesystem`
>
> Body: list of files touched, note any behavioural-preservation shims added
> (`parent_path "."` substitute, `time_t` conversion).

## 10. Pitfalls checklist (review before submitting)

- [ ] No remaining `tue/filesystem` includes or `tue::filesystem::` qualifiers.
- [ ] No remaining `tue_filesystem` in `CMakeLists.txt` or `package.xml`.
- [ ] All `.extension()` / `.filename()` call sites that previously got
      `std::string` still get `std::string` (added `.string()` where needed).
- [ ] `parent_path()` behaviour preserved at call sites that relied on the
      `"."` substitute.
- [ ] `last_write_time` consumers either accept `file_time_type` or have an
      inline `time_t` conversion appropriate for the package's C++ standard.
- [ ] `Path::join(absolute)` was not used (otherwise the `operator/` semantics
      differ — handle explicitly).
- [ ] `Crawler` toggle combinations preserved;
      `directory_options::skip_permission_denied` used to keep the no-throw
      walk behaviour.
- [ ] C++ standard is ≥ 17 and the package still builds on all CI
      distributions it targets.
- [ ] No new dependencies introduced. No unrelated changes.

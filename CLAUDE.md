# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Test

**Toolchain**: Visual Studio 2022 (VS 18) + ClangCL, x64, C++17. The CMake binary used throughout is:
```
C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
```

**Configure** (first time or after CMakeLists.txt changes):
```powershell
cmake.exe --preset windows-clangcl-debug
```

**Build** (incremental):
```powershell
cmake.exe --build --preset debug
```

**Clean build** (wipes `build/` and reconfigures):
```powershell
if (Test-Path build) { Remove-Item -Recurse -Force build }
cmake.exe --preset windows-clangcl-debug
cmake.exe --build --preset debug
```

**Run all tests**:
```powershell
.\build\Debug\tests.exe
```

**Run a single test suite or test**:
```powershell
.\build\Debug\tests.exe --gtest_filter="WriteCommandTest.*"
.\build\Debug\tests.exe --gtest_filter="WriteCommandTest.REQ_WRITE_013_*"
```

The VS Code default build task (`Ctrl+Shift+B`) runs **CMake Build** (configure + incremental build). The **CMake Clean Build** task is also available in the task menu.

## Project Architecture

### Source files
- `src/nvme_device.h` — All command parameter structs, `NvmeStatus` enum, and `NvmeDevice` class declaration.
- `src/nvme_device.cpp` — Fake device implementation (in-memory, no real NVMe I/O).
- `tests/test_NVM_<Command>.cpp` — One file per NVMe command, Google Test suites.
- `tests/test_main.cpp` — GTest main entry point.

### NvmeDevice (Fake Device pattern)
`NvmeDevice` is an in-memory simulation of an NVMe controller used exclusively for testing. It does not call any OS/driver APIs. Configurable state is injected via helper methods before a test:

| Helper | Purpose |
|---|---|
| `SetReadOnly(start, end)` | Mark LBA range as read-only (triggers `WriteToReadOnlyRange`) |
| `SetE2EProtection(bool)` | Enable E2E protection format (PRINFO checks) |
| `SetUncorrectable(start, end)` | Mark LBA range as uncorrectable |
| `SetCompareFailure(bool)` | Force `CompareFailure` on next Compare call |
| `SetDsmRangeLimit(max_nr)` | Cap the number of DSM ranges (triggers `CommandSizeLimitExceeded`) |

`NvmeStatus` maps to NVMe command-specific status codes (SCT=1h): `0x80` ConflictingAttributes through `0x8A` NamespaceNotReachable.

### Adding a new NVMe command
1. Add the command struct and method declaration to `src/nvme_device.h`.
2. Implement the method body in `src/nvme_device.cpp`.
3. Create `tests/test_NVM_<Command>.cpp`.
4. Register the new `.cpp` in the `add_executable(tests ...)` list in `CMakeLists.txt`.

### GTest / GMock
Pre-built static libs (`Debug`, ClangCL) live in `lib/gtest/`. They are imported directly in `CMakeLists.txt` — do not attempt to rebuild or fetch them via FetchContent.

## Multi-Agent TDD Pipeline

Five Claude sub-agents in `.claude/agents/` automate the Spec → Interface → Requirements → TC → Fake Device workflow:

| Agent | Invocation | Input → Output |
|---|---|---|
| `nvme-app-interface-generator` | `nvme-app-interface-generator <spec>.md` | Spec → `src/nvme_device.h` / `.cpp` |
| `nvme-spec-analysis` | `nvme-spec-analysis <spec>.md` | Spec → `doc/requirements/<cmd>-requirements.md` |
| `test-case-generator` | `test-case-generator doc/requirements/<cmd>-requirements.md` | Requirements → `tests/test_NVM_<Cmd>.cpp` |
| `nvme-fake-device-implementor` | `nvme-fake-device-implementor tests/test_NVM_<Cmd>.cpp` | Failing TCs → PASS (fixes device code only) |
| `orchestrator` | `orchestrator <spec>.md` | Full pipeline for all commands in a spec file |

**Role boundaries are strict**: `test-case-generator` only fixes TC code defects; `nvme-fake-device-implementor` only fixes device implementation defects. Ambiguous requirements are escalated back to `nvme-spec-analysis`.

### Requirement ID convention
`REQ-<COMMAND>-<SEQ>` (e.g., `REQ-WRITE-001`). Test names mirror these IDs: `TEST(WriteCommandTest, REQ_WRITE_001_WritesDataToLogicalBlocks)`.

## NVMe Spec documents
Source specs live in `doc/spec/nvme_io/`. Derived requirement documents are saved to `doc/requirements/`.

# AGENTS.md — Go4 Project Guide for AI Agents

## Project Overview

**Go4** (GSI Online Offline Object Oriented) is a C++ framework for experiment data processing, developed at the EE department of GSI Helmholtz Centre for Heavy Ion Research. It provides infrastructure for online and offline analysis of experimental physics data.

- **Version**: v6.4.0 (build 60400), March 2025
- **License**: GNU General Public License v2 or later
- **Language**: C++ (`.cxx` / `.h` files)
- **Build System**: CMake (minimum 3.10)
- **Dependencies**: ROOT >= 6.28, Qt5 >= 5.12 or Qt6 >= 6.4
- **Total Source Files**: ~965 (`.cxx`, `.h`, `.cpp`, `.hpp`)

## Architecture

The project follows a modular library architecture with clear dependency layers:

### Dependency Layers

```
Layer 1 (Base):     Go4LockGuard, Go4Log, Go4Exceptions, Go4CommandsBase, Go4StatusBase
                           ↓
Layer 2 (Core):     Go4Base (aggregates Layer 1 + ROOT dict)
                           ↓
Layer 3 (Thread):   Go4ThreadManager
                           ↓
Layer 4 (Task):     Go4Socket, Go4Queue, Go4TaskHandler, Go4CommandsTaskHandler
                           ↓
Layer 5 (AnalBase): Go4Event, Go4EventServer, Go4ConditionsBase, Go4DynamicList,
                    Go4StatusAnalysis, Go4Analysis, MbsAPIbase, Go4HDF5 (optional)
                           ↓
Layer 6 (Analysis): MbsAPI, RawAPI (optional), Go4HistogramServer,
                    Go4CommandsAnalysis, Go4AnalysisClient, Go4Http (optional)
                           ↓
Layer 7 (GUI):      Go4ObjectManager, Go4Display, Go4Proxies → qt4/
```

### Key Directories

| Directory | Purpose |
|-----------|---------|
| `Go4Base*` | Foundation modules (logging, exceptions, commands, status) |
| `Go4ThreadManager` | Thread pool and task scheduling |
| `Go4TaskHandler` | Client-server task management via sockets |
| `Go4Analysis*` | Core analysis framework (steps, events, conditions) |
| `Go4Commands*` | Command pattern implementations for remote control |
| `Go4ConditionsBase` | Data visualization conditions (polygon, window, etc.) |
| `Go4Dict` | ROOT dictionary linkdef headers |
| `qt4/` | Qt GUI applications (go4gui, web interface) |
| `Go4Example*/` | Example analysis implementations |
| `cmake/modules/` | Custom CMake macros and modules |
| `python/go4py/` | Python bindings |
| `MbsAPI/` | MBS event format reader API |

## Coding Conventions

### File Naming
- Headers: `TModule-name.h` (e.g., `TGo4AnalysisStep.h`)
- Sources: `TModule-name.cxx` (e.g., `TGo4AnalysisStep.cxx`)
- Class names match header file names with `T` prefix convention

### Class Naming
- All Go4 classes prefixed with `TGo4` (e.g., `TGo4Analysis`, `TGo4Command`)
- Inheritance naming: append role descriptor (e.g., `TGo4AnalysisImp`, `TGo4AnalysisManager`)
- Commander pattern: `TGo4Com<action>` (e.g., `TGo4ComStart`, `TGo4ComQuit`)

### Header Style
- Include guard format: `TMODULENAME_H` (uppercase)
- Go4 header includes use bare names: `#include "TGo4Log.h"` (no path)
- All headers are copied to `${BUILD_DIR}/include/` during build

### File Header Block
Every source and header file starts with:
```cpp
// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------
```

### Code Style
- Tabs for indentation
- Opening braces on same line for functions/classes
- Pointer/reference with type: `const char* ptr`, `int& ref`
- No trailing semicolons after namespace blocks

## Build System

### CMake Macros (from `cmake/modules/Go4Macros.cmake`)

| Macro | Purpose |
|-------|---------|
| `GO4_SOURCES(libname HEADERS h1 SOURCES s1)` | Register source/header files for a library |
| `GO4_STANDARD_LIBRARY(name LINKDEF ... DEPENDENCIES ...)` | Create a Go4 shared library with ROOT dictionary |
| `GO4_USER_ANALYSIS(HEADERS h1 SOURCES s1 LINKDEF l)` | Build user analysis library |
| `GO4_INSTALL_HEADERS(h1 h2 ...)` | Copy headers to build include directory |
| `GO4_LINK_LIBRARY(name SOURCES s1 LIBRARIES l1 DEFINITIONS d1)` | Create a shared library without ROOT dict |

### Common CMake Options
```bash
cmake - Dhdf5=ON    # Enable HDF5 support
cmake -Dqt6=ON      # Use Qt6 instead of Qt5
cmake -Dgui=ON      # Build GUI components
cmake -Dexamples=ON # Build example projects
cmake -Ddabc=ON     # Enable DABC integration
```

### Build Commands
```bash
mkdir build && cd build
cmake <path_to_source>
make -j$(nproc)
source go4login        # Set environment variables
```

## Development Guidelines

### Adding a New Module
1. Create directory `Go4<Name>/` with `CMakeLists.txt`
2. Add `Module.mk` for legacy Make support
3. Use `GO4_SOURCES` to register files in the parent CMakeLists.txt
4. Include in appropriate layer's library build

### Adding a New Command
1. Create `TGo4Com<action>.h` and `TGo4Com<action>.cxx` in appropriate `Go4Commands*/` directory
2. Inherit from `TGo4Command` base class
3. Override `Execute()` method
4. Register in command list's `CreateCommands()` method

### Adding a New Analysis Step
1. Create step class inheriting `TGo4AnalysisStep`
2. Override `Init()`, `FillData()`, `ResetLoop()`, `ResetResults()`
3. Register in user analysis library's CMakeLists.txt

### ROOT Dictionary Generation
Headers included in `LINKDEF` files get dictionary generation via `ROOT_GENERATE_DICTIONARY`. All public API headers that need interpreter access must be listed in the corresponding `Go4Dict/Go4*LinkDef.h`.

## Platform Support

| Platform | Status |
|----------|--------|
| openSUSE Leap/Tumbleweed | Primary development |
| Debian 7-11 | Supported |
| macOS 10.14+ | Supported (requires qt5web) |
| Windows (VS2019) | Supported |
| GCC 10-13 | Tested |

## Key Environment Variables

After running `source go4login`:
- `GO4SYS` — Go4 installation/build directory
- `GO4INC` — Go4 include directory
- `GO4BIN` — Go4 binaries directory
- `GO4LIB` — Go4 libraries directory

## Git Remotes

- `origin`: git@github.com:gsi-ee/go4.git

## Useful Paths

| Path | Content |
|------|---------|
| `docs/` | PDF manuals (Go4Introduction, Go4Reference, Go4FitTutorial) |
| `etc/` | Configuration files and startup scripts |
| `macros/` | ROOT macro files (`.C`) |
| `html/`, `html5/` | Web interface assets |
| `data/test.lmd` | Test data file |
| `docker/` | Docker configurations for various distros |
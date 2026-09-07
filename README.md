# Aznora IDE

Aznora is a native C++20 desktop IDE project scaffold built with CMake.

## Build

```sh
cmake -S . -B build
cmake --build build
```

Run the initial desktop shell with:

```sh
./build/apps/aznora-desktop/Aznora
```

## Project layout

- `apps/aznora-desktop/`: desktop application entry point and executable target
- `src/core/workspace/`: workspace and workspace lifecycle abstractions
- `src/core/filesystem/`: filesystem access abstraction
- `src/core/project/`: project model abstraction
- `src/core/settings/`: application settings abstraction
- `src/services/`: future application-level services
- `src/ui/`: future user interface integrations
- `tests/`: future automated tests
- `docs/`: architecture and project documentation
- `assets/`: application assets
- `third_party/`: future vendored dependencies

The `aznora_core` static library contains only placeholder architecture types. Future Workspace, FileSystem, Editor, Terminal, Git, LSP, and Mobile modules can be added without coupling them to the desktop entry point.
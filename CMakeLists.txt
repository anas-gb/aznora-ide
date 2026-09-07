cmake_minimum_required(VERSION 3.20)

project(Aznora
    VERSION 0.1.0
    DESCRIPTION "Aznora IDE"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aznora_core
    src/core/build/BuildService.cpp
    src/core/editor/EditorManager.cpp
    src/core/filesystem/FileExplorer.cpp
    src/core/filesystem/FileSystemService.cpp
    src/core/git/GitService.cpp
    src/core/languages/LanguageDetector.cpp
    src/core/project/Project.cpp
    src/core/search/SearchEngine.cpp
    src/core/settings/Settings.cpp
    src/core/terminal/TerminalService.cpp
    src/core/workspace/Workspace.cpp
    src/core/workspace/WorkspaceManager.cpp
)

target_include_directories(aznora_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(apps/aznora-desktop)

if(MSVC)
    target_compile_options(aznora_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(aznora_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
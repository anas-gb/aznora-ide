#include <iostream>
#include "core/workspace/WorkspaceManager.hpp"

int main()
{
    WorkspaceManager manager;

    bool success =
        manager.createWorkspace(
            "TestProject",
            "/home/anas/Projects");

    std::cout << "Workspace created: "
              << success
              << std::endl;

    return 0;
}
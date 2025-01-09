#include <ConsoleHandler.h>
#include <Windows.h>
#include <iostream>

// Creates a new console if one is not active
void CreateConsole() {
    if (GetConsoleWindow() == nullptr) { // Check that a window is not already active
        AllocConsole(); // Allocate a new console window
        FILE* fpstdout; // Create pipe for stdout
        FILE* fpstderr; // Create pipe for stderr
        freopen_s(&fpstdout, "CONOUT$", "w", stdout); // Redirect stdout to new console
        freopen_s(&fpstderr, "CONOUT$", "w", stderr); // Redirect stderr to new console
        std::cout << "Console initialized!\n"; // Display success message
    } else {
        std::cout << "Cannot create another console instance since one is already active.\n"; // Display warning
    }
}

// Kills the current console
void KillConsole() {
    std::cout << "Stopping Console\n"; // Display task start

    // Close the streams first
    fclose(stdout); // Close stdout stream
    fclose(stderr); // Close stderr stream

    FreeConsole(); // Free the console
}
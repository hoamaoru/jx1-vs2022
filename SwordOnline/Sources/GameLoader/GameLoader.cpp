#include <windows.h>
#include <iostream>

// Function pointer types matching the exported functions from Game.dll
using PFN_S3Client_Initialize = BOOL (*)(HINSTANCE);
using PFN_S3Client_Run        = int (*)(void);
using PFN_S3Client_Shutdown   = void (*)(void);

static bool Set16BitColorIfNeeded(DEVMODE &prevMode, bool &changed)
{
    changed = false;
    ZeroMemory(&prevMode, sizeof(prevMode));
    prevMode.dmSize = sizeof(prevMode);
    if (!EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &prevMode))
        return false;

    // If current desktop is palettized (8-bit), try switch to 16-bit
    if (prevMode.dmBitsPerPel <= 8) {
        DEVMODE dm = prevMode;
        dm.dmFields |= DM_BITSPERPEL;
        dm.dmBitsPerPel = 16; // 16-bit (usually 565)
        LONG res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
        if (res == DISP_CHANGE_SUCCESSFUL) {
            changed = true;
            return true;
        } else {
            std::cerr << "Warning: cannot switch display to 16-bit. Code=" << res << std::endl;
            return false;
        }
    }
    return true; // already >= 16-bit
}

static void RestoreDisplayMode(const DEVMODE &prevMode, bool changed)
{
    if (changed) {
        ChangeDisplaySettings(const_cast<DEVMODE*>(&prevMode), 0);
    }
}

int main()
{
    HINSTANCE hInst = GetModuleHandle(nullptr);

    // Ensure 16-bit color depth when starting the game (some builds expect >=16bpp)
    DEVMODE prevMode{};
    bool displayChanged = false;
    Set16BitColorIfNeeded(prevMode, displayChanged);

    // Load the DLL (ensure Game.dll is in the same folder or in PATH)
    HMODULE hGame = LoadLibraryA("Game.dll");
    if (!hGame) {
        std::cerr << "Cannot load Game.dll. Error=" << GetLastError() << std::endl;
        RestoreDisplayMode(prevMode, displayChanged);
        return 1;
    }

    // Resolve the exported functions
    auto pInitialize = reinterpret_cast<PFN_S3Client_Initialize>(GetProcAddress(hGame, "S3Client_Initialize"));
    auto pRun        = reinterpret_cast<PFN_S3Client_Run>(GetProcAddress(hGame, "S3Client_Run"));
    auto pShutdown   = reinterpret_cast<PFN_S3Client_Shutdown>(GetProcAddress(hGame, "S3Client_Shutdown"));

    if (!pInitialize || !pRun || !pShutdown) {
        std::cerr << "Missing exported functions in Game.dll" << std::endl;
        FreeLibrary(hGame);
        RestoreDisplayMode(prevMode, displayChanged);
        return 2;
    }

    // Initialize
    if (!pInitialize(hInst)) {
        std::cerr << "S3Client_Initialize failed" << std::endl;
        FreeLibrary(hGame);
        RestoreDisplayMode(prevMode, displayChanged);
        return 3;
    }

    // Run (blocks until game loop exits)
    std::cout << "Starting Game..." << std::endl;
    pRun();

    // Shutdown (optional, safe even if already cleaned up)
    pShutdown();

    FreeLibrary(hGame);

    // Restore desktop display mode if we changed it
    RestoreDisplayMode(prevMode, displayChanged);

    std::cout << "Game exited." << std::endl;
    return 0;
}

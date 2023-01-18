#include <Windows.h>
#include <cstdint>
#include <map>
#include <minhook.h>
#include <string>
#include <psapi.h>
#include "MCM.h"
#include <iostream>
typedef void(__thiscall* LevelHook)();
LevelHook _ResetLevel;

void LevelDetour() {
    std::cout << "Function Called" << std::endl;
    return;
}
DWORD WINAPI Console() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    std::cout << "DLL Injected, console alive" << std::endl;
    return 0;
}

void init() {
    Console();
    uintptr_t LevelAddr = MCM::findSig("40 53 48 83 EC 20 48 8B 01 48 8D 15 ? ? ? ? 48 8B D9 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 48 8B C8");
    if (MH_Initialize() == MH_OK) {
        if (MH_CreateHook((void*)LevelAddr, &LevelDetour, reinterpret_cast<LPVOID*>(&_ResetLevel)) == MH_OK) {
            std::cout << "Creating Hook" << std::endl;
            MH_EnableHook((void*)LevelAddr);
            std::cout << "Enabling.." << std::endl;
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)init, hModule, 0, 0);
        break;
    }

    return TRUE;
}


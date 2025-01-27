#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include "memory.h"  // Include memory header
#include "build/utilities/memory.h"

// Function to get process ID by name
DWORD GetProcessIdByName(const char* processName) {
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(hSnapshot, &entry)) {
        do {
            if (strcmp(entry.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &entry));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// Function to get the module base address
uintptr_t GetModuleBaseAddress(DWORD procId, const char* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_stricmp(modEntry.szModule, modName)) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

// Function to modify ammo with user input
void ammoHack(HANDLE hProcess, uintptr_t moduleBase) {
    uintptr_t ammoAddress = moduleBase + offsets::ammoAddress;
    int rifleAmmo;

    if (ReadMemory(hProcess, ammoAddress, &rifleAmmo, sizeof(rifleAmmo))) {
        std::cout << "Current rifle ammo: " << rifleAmmo << std::endl;

        int newRifleAmmo;
        std::cout << "Enter the desired ammo amount: ";
        std::cin >> newRifleAmmo;

        if (WriteMemory(hProcess, ammoAddress, &newRifleAmmo, sizeof(newRifleAmmo))) {
            std::cout << "New rifle ammo set to: " << newRifleAmmo << std::endl;
        }
        else {
            std::cerr << "Failed to write memory." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to read memory for ammo." << std::endl;
    }
}

// Function to modify health with user input
void healthHack(HANDLE hProcess, uintptr_t moduleBase) {
    uintptr_t playerBase = moduleBase + offsets::player;
    uintptr_t healthOffset = offsets::playerHealth;
    uintptr_t playerAddress;

    if (ReadMemory(hProcess, playerBase, &playerAddress, sizeof(playerAddress))) {
        int health;
        if (ReadMemory(hProcess, playerAddress + healthOffset, &health, sizeof(health))) {
            std::cout << "Current health: " << health << std::endl;

            int newHealth;
            std::cout << "Enter the desired health amount: ";
            std::cin >> newHealth;

            if (WriteMemory(hProcess, playerAddress + healthOffset, &newHealth, sizeof(newHealth))) {
                std::cout << "New health set to: " << newHealth << std::endl;
            }
            else {
                std::cerr << "Failed to write memory." << std::endl;
            }
        }
        else {
            std::cerr << "Failed to read memory for health." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to get player base address." << std::endl;
    }
}

// Func to enable fly hack
void flyHack(HANDLE hProcess, uintptr_t moduleBase) {
    uintptr_t playerBase = offsets::player;
    uintptr_t zAxisOffset = offsets::zAxisOffset;
    uintptr_t playerAddress;

    if (ReadMemory(hProcess, playerBase, &playerAddress, sizeof(playerAddress))) {
        float zAxis;
        if (ReadMemory(hProcess, playerAddress + zAxisOffset, &zAxis, sizeof(zAxis))) {
            std::cout << "Current height: " << zAxis << std::endl;

            float newHeight;
            std::cout << "Enter your desired fly height: ";
            std::cin >> newHeight;

            if (WriteMemory(hProcess, playerAddress + zAxisOffset, &newHeight, sizeof(newHeight))) {
                std::cout << "New height set to: " << newHeight << std::endl;
            }
            else {
                std::cerr << "Failed to write memory." << std::endl;
            }
        }
        else {
            std::cerr << "Failed to read memory for health." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to get player base address." << std::endl;
    }
}

// Func for title & colours 
void tileAndColours() {
    system("title Assault Cube Internal Hack");
    std::cout << "\033[0;36m"; // Cyan colour (i think...)
}

int main() {
    tileAndColours();
    const char* processName = "ac_client.exe";
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        std::cerr << "Game not running/found." << std::endl;
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cerr << "Failed to open game. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    int option;
    while (true) {
        system("cls");
        std::cout << "\033[0;36m"; // Cyan colour (i think...)
        std::cout << "===============================\n";
        std::cout << "  Assault Cube Internal Hack\n";
        std::cout << "===============================\n";
        std::cout << "\033[0;33m"; // Yellow colour (i think...)
        std::cout << "[1] Health Hack\n";
        std::cout << "[2] Ammo Hack\n";
        std::cout << "[3] Fly Hack\n";
        std::cout << "[0] Exit\n";
        std::cout << "\033[0m"; // Reset colour
        std::cout << "Enter an option: ";
        std::cin >> option;

        if (option == 1) {
            healthHack(hProcess, moduleBase);
        }
        else if (option == 2) {
            ammoHack(hProcess, moduleBase);
        }
        else if (option == 3) {
            flyHack(hProcess, moduleBase);
        }
        else if (option == 0) {
            std::cout << "Exiting...";
            break;
        }
        else {
            std::cout << "Invalid option.";
            break;
        }
    }

    CloseHandle(hProcess);
    return 0;
}
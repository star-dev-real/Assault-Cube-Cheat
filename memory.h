#ifndef MEMORY_H
#define MEMORY_H

#include <Windows.h>

// Offsets for chaning memory
namespace offsets {
    uintptr_t player = 0x195404;
    uintptr_t gravity = 0x0048;
    uintptr_t entList = 0x0018AC04;
    uintptr_t playerAmount = 0x18AC0C;
    uintptr_t playerName = 0x205;
    uintptr_t playerHealth = 0xEC;
    uintptr_t playerPos = 0x28;
    uintptr_t playerLookX = 0x0034;
    uintptr_t playerLookY = 0x0038;
    uintptr_t ammoAddress = 0x00860408; // I need to get offset / base
    uintptr_t zAxisOffset = 0x30;
    }

    bool ReadMemory(HANDLE hProcess, uintptr_t address, void* value, size_t size) {
        return ReadProcessMemory(hProcess, (LPCVOID)address, value, size, NULL);
    }

    bool WriteMemory(HANDLE hProcess, uintptr_t address, void* value, size_t size) {
        return WriteProcessMemory(hProcess, (LPCVOID)address, value, size, NULL);
    }


#endif

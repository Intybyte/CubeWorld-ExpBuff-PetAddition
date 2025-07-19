// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"

#define noinl __declspec(noinline)
#define interpret_bytes(arg) reinterpret_cast<BYTE*>(arg)

cube::GameController* GameController = nullptr;

extern "C" __declspec(dllexport) noinl int __stdcall CalculateCorrectedXP(int added_xp, cube::Creature* pet) {
    if (GameController == nullptr) {
        GameController = cube::GetGameController();
    }
    
    if (GameController == NULL || GameController->shutdown) {
        //keep doing whatever you where doing
        return added_xp;
    }

    cube::Creature* player = GameController->GetLocalPlayer();
    if (player->GUID != pet->parent_GUID) { // not a pet
        //keep doing whatever you where doing
        return added_xp;
    }

    float difference = ((float)player->level) / ((float)pet->level);
    difference = max(difference, 1.0);
    float added_xp_corrected = ((float)added_xp) * difference;

    return (int)added_xp_corrected;
}

int jump_back;
extern "C" __declspec(dllexport) __declspec(naked) noinl void PetCalculateAddXP_Wrapper() {
    __asm {
        pushfd // Save flags

        // -- Save general-purpose registers --
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

        mov esi, esp // backup ESP originale, senza modifiche

        // -- Save full FP + XMM state --
        sub esp, 512 + 16              // Allocate 512 bytes for fxsave buffer
        and esp, 0xFFFFFFF0         // Align ESP to 16 bytes (required for FXSAVE)

        fxsave[esp]                // Save FPU, SSE, and MXCSR state

        mov ebp, esp

        push edi                  // param pet
        push eax                  // param added_xp
        call CalculateCorrectedXP

        mov esp, ebp

        //   Restore full FP + XMM state --
        fxrstor[esp]

        mov esp, esi                // Restore original ESP (before fxsave)

        // -- Restore general-purpose registers --
        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        add[edi + 0x194], eax    // Update XP with function return value

        popfd                    // Restore flags

        jmp jump_back
    }

}

intptr_t PetCalculateAddXP_ptr = (intptr_t) &PetCalculateAddXP_Wrapper;

void WriteCall(BYTE* location, BYTE* newFunction) {
    DWORD oldProtect;

    VirtualProtect(location, 6, PAGE_EXECUTE_READWRITE, &oldProtect);

    location[0] = 0xE9; // JMP

    // relative offset for JMP (dest - src - 5 bytes per istruzione)
    intptr_t relAddr = (intptr_t)(newFunction - location) - 5;

    *reinterpret_cast<int32_t*>(location + 1) = static_cast<int32_t>(relAddr);

    location[5] = 0x90; // fill spare with NOP

    VirtualProtect(location, 6, oldProtect, &oldProtect);
}

extern "C" __declspec(dllexport) bool  APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    HMODULE baseModule = GetModuleHandle(NULL);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        //OpenConsole();
        
        cube::SetBase((uint32_t)GetModuleHandle(NULL));
        jump_back = cube::Offset(0x001a0d3f);
        WriteCall(interpret_bytes(cube::Offset(0x001a0d39)), interpret_bytes(PetCalculateAddXP_ptr));
        
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) 
    {
        //FreeConsole();
    }
    return true;
}


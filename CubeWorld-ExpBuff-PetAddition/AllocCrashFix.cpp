// AllocCrashFix.cpp : Definisce le funzioni esportate per la DLL.
//

#include "pch.h"
#include "framework.h"
#include "AllocCrashFix.h"


// Esempio di variabile esportata
ALLOCCRASHFIX_API int nAllocCrashFix=0;

// Esempio di funzione esportata.
ALLOCCRASHFIX_API int fnAllocCrashFix(void)
{
    return 0;
}

// Costruttore di una classe esportata.
CAllocCrashFix::CAllocCrashFix()
{
    return;
}

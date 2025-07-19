#pragma once
#include "pch.h"
#include <fcntl.h>
#include <io.h>

void OpenConsole()
{
    AllocConsole();

    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    _setmode(_fileno(stdout), _O_TEXT);
    _setmode(_fileno(stdin), _O_TEXT);
    _setmode(_fileno(stderr), _O_TEXT);

    printf("Debug on.\n");
}
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <Windows.h>
#include <locale.h>

#include "SDK/Interfaces.h"
#include "SDK/Hooks.h"
#include "Utils/Utils.h"
#include "SDK/Config.h"
#include "Features/Visuals/Chams.h"
#include "SDK/Globals.h"
#include "Features/Visuals/SkinChanger.h"
#include "Features/Lua/Bridge/Bridge.h"


LONG __stdcall ExceptionHandler(_EXCEPTION_POINTERS* exceptionInfo) {
    std::string exceptionText;

    std::string moduleCrashed;
    uintptr_t addressCrashed;
    Memory->ModuleRelativeAddress(reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress), &moduleCrashed, &addressCrashed);
    exceptionText += std::format("Exception occured at {}+{:X}\n", moduleCrashed, addressCrashed);
    exceptionText += std::format("EAX: {:x}\n", exceptionInfo->ContextRecord->Eax);
    exceptionText += std::format("Code: {:x}\n", exceptionInfo->ExceptionRecord->ExceptionCode);
    exceptionText += "\nInformation copied to clipboard, send it Penguin#3040";

    OpenClipboard(0);
    SetClipboardData(CF_TEXT, (HANDLE)exceptionText.c_str());
    CloseClipboard();

    MessageBoxA(0, exceptionText.c_str(), "Arctic Tech", MB_OK | MB_ICONERROR);

    return 0;
}

void Initialize(HMODULE hModule) {
    setlocale(LC_ALL, "ru_RI.UTF-8");
    //PVOID exceptionHandle = AddVectoredExceptionHandler(0, ExceptionHandler);

#ifdef _DEBUG
    AllocConsole();
    AttachConsole(ATTACH_PARENT_PROCESS);
    FILE* filePointer = nullptr;
    freopen_s(&filePointer, "CONOUT$", "w", stdout);
#endif

    Interfaces::Initialize();
    Hooks::Initialize();
    Lua->Setup();
    SkinChanger->FixViewModelSequence();
    SkinChanger->GetPaintKits();


#ifdef _DEBUG
    while (!GetAsyncKeyState(VK_END)) {
        Sleep(1000);
    }

    Cheat.Unloaded = true;
    Hooks::End();

    fclose(filePointer);
    FreeConsole();
    const auto wnd = GetConsoleWindow();
    if (wnd)
        PostMessageW(wnd, WM_CLOSE, 0, 0);

    //RemoveVectoredExceptionHandler(exceptionHandle);
    FreeLibraryAndExitThread(hModule, 0);
#endif
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Initialize, hModule, 0, 0);
    return TRUE;
}
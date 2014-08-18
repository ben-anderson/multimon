#pragma once

#ifdef _WIN64
#define HOOKDLL TEXT("mmhook64.dll")
#define SHELLPROC "?ShellProc@@YA_JH_K_J@Z"
#else
#define SHELLPROC "?ShellProc@@YGJHIJ@Z"
#define HOOKDLL TEXT("mmhook32.dll")
#endif

// Hook procedures
//LRESULT WINAPI CallWndProc(int, WPARAM, LPARAM);
//LRESULT WINAPI CBTProc(int, WPARAM, LPARAM);
//LRESULT WINAPI DebugProc(int, WPARAM, LPARAM);
LRESULT WINAPI ShellProc(int, WPARAM, LPARAM);

// Initialization procedure
bool InstallShellHook();
void RemoveShellHook();

#include "stdafx.h"
#include "hook.h"
#include "log.h"

// Global variables
HINSTANCE hinstDll;

HHOOK hhShell = NULL;
HHOOK hhWnd   = NULL;

bool InstallShellHook()
{
	HOOKPROC hkprc;

	hinstDll = LoadLibrary(HOOKDLL);
	if (!hinstDll)
	{
		LogSendMessage(TEXT("Could not load 64 bit hook dll!"), LOG_ERROR);
		return FALSE;
	}

	hkprc = (HOOKPROC)GetProcAddress(hinstDll, SHELLPROC);
	if (!hkprc)
	{
		LogSendMessage(TEXT("Could not get 64 bit hook address!"), LOG_ERROR);
		return FALSE;
	}

	hhShell = SetWindowsHookEx(
		WH_SHELL,
		hkprc,
		hinstDll,
		0	// Hook all processes
	);

	if (!hhShell)
	{
		LogSendMessage(TEXT("Unable to install 64 bit app hook!"), LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

void RemoveShellHook()
{
	if (hhShell)
		UnhookWindowsHookEx(hhShell);

	if (hinstDll)
		FreeLibrary(hinstDll);
}
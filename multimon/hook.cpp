#include "stdafx.h"
#include "multimon.h"
#include "hook.h"

#define PROXYTIMEOUT 1000

#define PROXYEXITNAME TEXT("_multimon_proxy_exit")


// Global variables
HINSTANCE hinstDll;

HANDLE hProxy     = NULL;
HANDLE hProxyExit = NULL;

HHOOK hhShell = NULL;
HHOOK hhWnd   = NULL;


BOOL IsWow64()
{
	BOOL bIsWow64;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	return bIsWow64;
}

bool InstallShellHook()
{
	HOOKPROC hkprc;

	hinstDll = LoadLibrary(HOOKDLL);
	if (!hinstDll)
	{
		MessageBox(hwndMain, TEXT("Could not load hook dll!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hkprc = (HOOKPROC)GetProcAddress(hinstDll, SHELLPROC);
	if (!hkprc)
	{
		MessageBox(hwndMain, TEXT("Could not get hook address!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
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
		MessageBox(hwndMain, TEXT("Unable to install app hook!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
	if (IsWow64())
	{
		STARTUPINFO startup;
		PROCESS_INFORMATION process;

		ZeroMemory(&startup, sizeof(STARTUPINFO));
		startup.cb = sizeof(STARTUPINFO);

		if (CreateProcess(
			TEXT("mmproxy64.exe"),	// Aplication name
			NULL,					// Command line
			NULL,					// Process security attributes
			NULL,					// Thread security attributes
			FALSE,					// Inherit handles
			NULL,					// Process creation flags
			NULL,					// Environment
			NULL,					// Current directory
			&startup,				// STARTUPINFO
			&process				// PROCESS_INFORMATION
			))
		{
			hProxy     = process.hProcess;
			hProxyExit = CreateEvent(NULL, FALSE, FALSE, PROXYEXITNAME);
		}
	}

	return TRUE;
}

void RemoveShellHook()
{
	if (hhShell)
		UnhookWindowsHookEx(hhShell);

	if (hinstDll)
		FreeLibrary(hinstDll);

	if (hProxy)
	{
		if (hProxyExit)
		{
			if (SetEvent(hProxyExit))
			{
				if (WaitForSingleObject(hProxy, PROXYTIMEOUT) == WAIT_OBJECT_0)
					hProxy = NULL;
			}
		}
		
		if (hProxy)
			TerminateProcess(hProxy, -1);
	}

	if (hProxyExit)
	{
		CloseHandle(hProxyExit);
		hProxyExit = NULL;
	}
}
// mmproxy64.cpp : Proxy to load the 64 bit shell hook DLL.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#include "stdafx.h"

#include "log.h"
#include "hook.h"
#include "mmproxy64.h"

#define PROXYEXITNAME TEXT("_multimon_proxy_exit")


// Global Variables:
HINSTANCE hInst;	// Current instance

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	HANDLE hEventExit = NULL;
	
	hInst = hInstance;

	if (!InitLog())
		return -1;

	hEventExit = OpenEvent(EVENT_ALL_ACCESS, FALSE, PROXYEXITNAME);
	if (!hEventExit)
		return -1;
	
	if (!InstallShellHook())
		return -1;
	
	while (MsgWaitForMultipleObjects(1, &hEventExit, FALSE, INFINITE, QS_ALLINPUT) != WAIT_OBJECT_0)
	{
		1;
	}
	
	ShutdownLog();
	CloseHandle(hEventExit);
	return 0;
}
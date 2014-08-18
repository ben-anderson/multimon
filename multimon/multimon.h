// multimon.h : Multi Monitor Tool
// Window initialization and main message loop.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#pragma once

#define MAX_LOADSTRING 100
#include "resource.h"

#include "logserver.h"

// Global variables
#if defined(MULTIMON_CPP)
HINSTANCE	hInst;								// current instance
HWND		hwndMain;							// main window handle
TCHAR		szTitle[MAX_LOADSTRING];			// window title text
TCHAR		szWindowClass[MAX_LOADSTRING];		// the main window class name

Log *pLog = NULL;
#else
extern HWND hwndMain;
#endif

// Function declarations
ATOM				RegisterMainClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
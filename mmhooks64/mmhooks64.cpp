// mmhooks64.cpp : Shell hook DLL to support multimon.cpp
// Contains the functionality to move windows between monitors.
//
// Copyright(c) 2014 Ben Anderson
//

#include "stdafx.h"

#define MMHOOKS64_CPP
#include "mmhooks64.h"
#include "log.h"


// FUNCTION: MovePointToMonitor
// PARAMETERS:
//   POINT *pptSrc
//     point to move
//   RECT *prSrcMon
//     work area of the source monitor
//   RECT *prDstMon
//     work area of the destination monitor
//
// RETURN VALUE:
//   POINT structure with the new coordinates
//
// Maps the source point to the destination monitor.
//
// The destination point will be scaled according to any resolution differences between monitors.
//
POINT MovePointToMonitor(POINT *pptSrc, RECT *prSrcMon, RECT *prDstMon)
{
	POINT offset, srcSize, dstSize;
	POINT ptDst;

	float fx, fy;

	// Distance between the top left corner of the source monitor and the point.
	offset.x = pptSrc->x - prSrcMon->left;
	offset.y = pptSrc->y - prSrcMon->top;

	srcSize.x = prSrcMon->right  - prSrcMon->left;	// prSrcMon width
	srcSize.y = prSrcMon->bottom - prSrcMon->top;	// prSrcMon height

	dstSize.x = prDstMon->right  - prDstMon->left;	// prDstMon width
	dstSize.y = prDstMon->bottom - prDstMon->top;	// prDstMon height

	// Calculate the resolution scale between the source and dest monitors.
	// Do not divide by zero.
	fx = (srcSize.x) ? (float)dstSize.x / (float)srcSize.x : 0;
	fy = (srcSize.y) ? (float)dstSize.y / (float)srcSize.y : 0;

	// Translate the point, then scale it so it is in the same relative location on the
	// destination monitor.
	ptDst.x = prDstMon->left + (LONG)((float)offset.x * fx);
	ptDst.y = prDstMon->top  + (LONG)((float)offset.y * fy);
	return ptDst;
}

// FUNCTION: MoveRectToMonitor
// PARAMETERS:
//   RECT *prSrc
//     rect to move
//   RECT *prSrcMon
//     work area of the source monitor
//   RECT *prDstMon
//     work area of the destination monitor
//
// RETURN VALUE:
//   RECT structure with the new coordinates
//
// Maps the source rectangle to the destination monitor.
//
// The origin point (top left) will be scaled according to any resolution differences between monitors.
//
// The width and height will be preserved if possible. If the bottom right corner is out of bounds,
// the origin point will be moved to allow the RECT to fit on the monitor. If the RECT is too large
// for the destination monitor it will be set to the size of the destination monitor.
//
RECT MoveRectToMonitor(RECT *prSrc, RECT *prSrcMon, RECT *prDstMon)
{
	POINT ptSrc, ptDst;

	RECT rDst;
	RECT rDiff;

	// Translate and scale the top left corner
	ptSrc.x   = prSrc->left;
	ptSrc.y   = prSrc->top;
	ptDst     = MovePointToMonitor(&ptSrc, prSrcMon, prDstMon);
	rDst.left = ptDst.x;
	rDst.top  = ptDst.y;

	// Calculate the bottom right corner, preserving width and height if it will fit on
	// the destination monitor.
	rDst.right  = ptDst.x + prSrc->right  - prSrc->left;	// x + width
	rDst.bottom = ptDst.y + prSrc->bottom - prSrc->top;		// y + height

	// The right edge is off the screen. Attempt to move the window to the left to make it fit.
	if (rDst.right > prDstMon->right)
	{
		rDiff.left  = prSrc->left  - prDstMon->left;
		rDiff.right = prSrc->right - prDstMon->right;

		// It fits.
		if (rDiff.right < rDiff.left)
		{
			rDst.left  -= rDiff.right;
			rDst.right  = prDstMon->right;
		}
		// Doesn't fit. Make the window's X axis fill the screen.
		else
		{
			rDst.left  = prDstMon->left;
			rDst.right = prDstMon->right;
		}
	}

	// The bottom edge is off the screen. Attempt to move the window to the left to make it fit.
	if (rDst.bottom > prDstMon->bottom)
	{
		rDiff.top    = prSrc->top    - prDstMon->top;
		rDiff.bottom = prSrc->bottom - prDstMon->bottom;

		// It fits.
		if (rDiff.bottom < rDiff.top)
		{
			rDst.top    -= rDiff.bottom;
			rDst.bottom  = prDstMon->bottom;
		}
		// Doesn't fit. Make the window's Y axis fill the screen.
		else
		{
			rDst.top    = prDstMon->top;
			rDst.bottom = prDstMon->bottom;
		}
	}

	return rDst;
}

// FUNCTION: MoveWindowToMonitor
// PARAMETERS:
//   HWND hWnd
//     handle of the window to move
//   RECT *prSrcMon
//     work area of the source monitor
//   RECT *prDstMon
//     work area of the destination monitor
//
// RETURN VALUE:
//   none
//
// Maps the window to the destination monitor, preserving the size if possible.
//
void MoveWindowToMonitor(HWND hWnd, RECT *prSrcMon, RECT *prDstMon)
{
	WINDOWPLACEMENT wpSrc, wpDst;
	RECT rSrc, rDst;

	TCHAR szBuf[513];

	// Get normal window position. Will also tell if the window is maximized, minimized or normal.
	wpSrc.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hWnd, &wpSrc);
	CopyMemory(&wpDst, &wpSrc, sizeof(WINDOWPLACEMENT));

	// If the window is maximized, set the show flag to restore the window. If it is left unchanged,
	// the maximized window will retain its original size on the new monitor.
	if (wpSrc.showCmd == SW_MAXIMIZE)
		wpDst.showCmd = SW_SHOWNORMAL;

	rDst = MoveRectToMonitor(&wpDst.rcNormalPosition, prSrcMon, prDstMon);
	wpDst.rcNormalPosition = rDst;

	StringCbPrintf(
		szBuf, sizeof(szBuf) - 1,
		TEXT("Window was not created on mouse monitor, calling SetWindowPlacement(%s)\r\n")
		TEXT("Old Position: min:[%ld, %ld], max:[%ld, %ld], normal:[%ld, %ld, %ld, %ld]\r\n")
		TEXT("New Position: min:[%ld, %ld], max:[%ld, %ld], normal:[%ld, %ld, %ld, %ld]"),
		(wpSrc.showCmd == SW_MAXIMIZE) ? TEXT("SHOWNORMAL") : TEXT(""),
		wpSrc.ptMinPosition.x, wpSrc.ptMinPosition.y, wpSrc.ptMaxPosition.x, wpSrc.ptMaxPosition.y,
		wpSrc.rcNormalPosition.left, wpSrc.rcNormalPosition.top, wpSrc.rcNormalPosition.right, wpSrc.rcNormalPosition.bottom,
		wpDst.ptMinPosition.x, wpDst.ptMinPosition.y, wpDst.ptMaxPosition.x, wpDst.ptMaxPosition.y,
		wpDst.rcNormalPosition.left, wpDst.rcNormalPosition.top, wpDst.rcNormalPosition.right, wpDst.rcNormalPosition.bottom
		);

	LogSendMessage(szBuf, LOG_INFO);
	SetWindowPlacement(hWnd, &wpDst);

	// TODO: needed anymore?
	// The window will not actually move to its new position until MoveWindow is called
	// with its current coordinates.
	/*GetWindowRect(hWnd, &rSrc);
	rDst = rSrc;

	StringCbPrintf(
		szBuf, sizeof(szBuf) - 1,
		TEXT("Updating window, calling MoveWindow()\r\n")
		TEXT("Old Position: [x:%ld, y:%ld], [w:%ld, h:%ld]\r\n")
		TEXT("New Position: [x:%ld, y:%ld], [w:%ld, h:%ld]"),
		rSrc.left, rSrc.top, rSrc.right, rSrc.bottom,
		rDst.left, rDst.top, rDst.right, rDst.bottom
		);

	LogSendMessage(szBuf, LOG_INFO);
	MoveWindow(hWnd, rDst.left, rDst.top, rDst.right - rDst.left, rDst.bottom - rDst.top, FALSE);*/ // TODO: redraw true or false?

	// Set the window back to its maximized state if we restored it.
	if (wpSrc.showCmd == SW_MAXIMIZE)
	{
		wpDst.showCmd = SW_MAXIMIZE;
		LogSendMessage(TEXT("Maximizing restored window, calling SetWindowPlacement(MAXIMIZE)"), LOG_INFO);
		SetWindowPlacement(hWnd, &wpDst);
	}
}

// FUNCTION: PositionWindow
// PARAMETERS:
//   HWND hWnd
//     Handle to the window to position
//
// RETURN VALUE
//   none
//
// Moves a newly created window to the same window as the mouse.
//
// First checks if the window was created on the correct monitor, and if not calculates the new position.
//
void PositionWindow(HWND hWnd)
{
	TCHAR szTitle[101];
	RECT  rWnd;
	POINT p;

	TCHAR szBuf[513];

	MONITORINFO	mCurMon;
	HMONITOR	hCurMon;

	MONITORINFO	mMouseMon;
	HMONITOR	hMouseMon;

	MONITORINFO	mMainMon;
	HMONITOR	hMainMon;

	// Coordinates of the created window
	GetWindowRect(hWnd, &rWnd);
	GetWindowText(hWnd, szTitle, 100);

	// Coordinates of the created window's monitor
	mCurMon.cbSize = sizeof(MONITORINFO);
	hCurMon = MonitorFromRect(&rWnd, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hCurMon, &mCurMon);

	// Coordinates of the mouse's monitor
	GetCursorPos(&p);
	mMouseMon.cbSize = sizeof(MONITORINFO);
	hMouseMon = MonitorFromPoint(p, MONITOR_DEFAULTTONULL);
	GetMonitorInfo(hMouseMon, &mMouseMon);

	// Coordinates of the default monitor
	p.x = p.y = 9999999;	// Invalid coordinates; MonitorFromPoint will return the default monitor
	mMainMon.cbSize = sizeof(MONITORINFO);
	hMainMon = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);
	GetMonitorInfo(hMainMon, &mMainMon);

	StringCbPrintf(
		szBuf, sizeof(szBuf) - 1,
		TEXT("Window Created: [%ld, %ld, %ld, %ld] - \"%s\"\r\n")
		TEXT("Creation Monitor: [%ld, %ld, %ld, %ld]\r\n")
		TEXT("Mouse Monitor: [%ld, %ld, %ld, %ld]\r\n")
		TEXT("Default Monitor: [%ld, %ld, %ld, %ld]"),
		rWnd.left, rWnd.top, rWnd.right, rWnd.bottom, szTitle,
		mCurMon.rcWork.left, mCurMon.rcWork.top, mCurMon.rcWork.right, mCurMon.rcWork.bottom,
		mMouseMon.rcWork.left, mMouseMon.rcWork.top, mMouseMon.rcWork.right, mMouseMon.rcWork.bottom,
		mMainMon.rcMonitor.left, mMainMon.rcMonitor.top, mMainMon.rcMonitor.right, mMainMon.rcMonitor.bottom
		);

	LogSendMessage(szBuf, LOG_INFO);

	// Move the window
	if (hCurMon != hMouseMon)
		MoveWindowToMonitor(hWnd, &mCurMon.rcWork, &mMouseMon.rcWork);
}


// FUNCTION: ShellProc
//
// Hook procedure to intercept Shell events. See MSDN for further information.
// http://msdn.microsoft.com/en-ca/library/windows/desktop/ms644991%28v=vs.85%29.aspx
//
MMHOOKS64_API LRESULT WINAPI ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 /*|| bActivated*/)  // Do not process message
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	switch (nCode)
	{
	case HSHELL_WINDOWCREATED:
		PositionWindow((HWND)wParam);
		//bActivated = true;
		break;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
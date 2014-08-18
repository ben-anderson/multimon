// logserver.cpp : A multi-process log receiver.
// Requires the mmap library class.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#include "stdafx.h"
#include <chrono>
#include <ctime>

#include "multimon.h"
#include "logserver.h"


DWORD WINAPI Log::Pump(Log *pThis)
{
	std::wstring buffer;

	while (1)
	{
		// Wait for either a new message, or a quit event
		DWORD obj = WaitForMultipleObjects(NUM_LOG_EVENTS, pThis->hEvents, FALSE, INFINITE);
		if (obj >= WAIT_OBJECT_0 && obj < WAIT_OBJECT_0 + NUM_LOG_EVENTS)
		{
			obj -= WAIT_OBJECT_0;
			if (obj == LOG_EVENT_READ)
			{
				//pThis->lines.push_back(*(pThis->FormatMessage(pThis->RetrieveMessage())));
				//for (int n = 0; n < pThis->lines.size(); n++)

				buffer = *pThis->RetrieveMessage() + TEXT("\r\n") + buffer;
				SetWindowText(pThis->hWnd, buffer.c_str());
			}
			else if (obj == LOG_EVENT_EXIT)
				break;
		}
	}

	return 1;
}


std::wstring *Log::RetrieveMessage()
{
	TCHAR chBuf[LOGBUFSIZE];
	UINT32 logLevel		= -1;
	std::wstring *wsBuf = NULL;

	std::time_t t;
	struct std::tm time;
	std::chrono::time_point<std::chrono::system_clock> now;

	PLOGSHARED pLog = NULL;

	// Lock the mapped memory
	pLog = static_cast<PLOGSHARED>(mmap->Lock());
	if (!pLog)
		return NULL;

	// Copy the log message before it is overwritten
	StringCbCopy(chBuf, sizeof(chBuf) - 1, pLog->chLogMsg);
	logLevel = pLog->nLevel;

	// We're done with the memory, release it
	mmap->Release();

	// Signal the log producers they can send another message
	if (hEvents[LOG_EVENT_READ])
		ResetEvent(hEvents[LOG_EVENT_READ]);

	// Format the message
	wsBuf = new std::wstring(TEXT("\r\n"));

	now = std::chrono::system_clock::now();
	t = std::chrono::system_clock::to_time_t(now);
	localtime_s(&time, &t);
	
	TCHAR chTime[80];
	wcsftime(chTime, 80, TEXT("%m-%d-%Y %I:%M:%S"), &time);
	*wsBuf += chTime;

	switch (logLevel)
	{
	case LOG_ERROR:
		*wsBuf += TEXT(" - ERROR\r\n\r\n");
		break;
	case LOG_WARN:
		*wsBuf += TEXT(" - WARNING\r\n\r\n");
		break;
	case LOG_INFO:
		*wsBuf += TEXT(" - INFO\r\n\r\n");
		break;
	default:
		*wsBuf += TEXT(" - UNKNOWN LEVEL\r\n\r\n");
	}

	*wsBuf += chBuf;
	*wsBuf += TEXT("\r\n\r\n----------------------------------------------------------------------------------------------------");

	return wsBuf;
}

Log::Log(HWND hwnd)
{
	try
	{
		mmap = new MMap(TEXT("multimon_log"), sizeof(LOGSHARED));
	}
	catch (...)
	{
		throw TEXT("Unable to initialize shared memory!");
	}

	hEvents[LOG_EVENT_READ] = CreateEvent(
		NULL,
		TRUE,	// manual reset
		FALSE,
		LOGSIGNALNAME
		);

	if (!hEvents[LOG_EVENT_READ])
		throw TEXT("Unable to create log event!");

	hEvents[LOG_EVENT_EXIT] = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		LOGEXITNAME
		);

	if (!hEvents[LOG_EVENT_EXIT])
		throw TEXT("Unable to create exit event!");

	hPump = CreateThread(
		NULL,
		0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(&Log::Pump),
		this,
		0,
		NULL
	);

	if (hPump)
	{
		hWnd = hwnd;
		SetWindowText(hWnd, TEXT("Log initialized."));
	}
	else
		throw 0;
}

Log::~Log()
{
	SetEvent(hEvents[LOG_EVENT_EXIT]);
	DWORD obj = WaitForSingleObject(hPump, INFINITE);

	CloseHandle(hPump);
	CloseHandle(hEvents[LOG_EVENT_READ]);
	CloseHandle(hEvents[LOG_EVENT_EXIT]);

	//if (mmapLog) /* TODO: causes double delete?? */
	//	delete mmapLog;
}
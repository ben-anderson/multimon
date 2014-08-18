#include "stdafx.h"

#define LOG_CPP

#include "log.h"

#define LOGTIMEOUT 16
#define LOGRETRIES 10


PLOGSHARED WaitForLockAndQueueEmpty()
{
	PLOGSHARED pView = NULL;
	int tries = 0;

	// Try LOGRETRIES times to acquire both the shared memory mutex and the log event
	while (!pView)
	{
		if (++tries > LOGRETRIES)
			return NULL;

		// Check if there is already a log message waiting to be read
		if (WaitForSingleObject(hLogSignal, 0) != WAIT_OBJECT_0)
		{
			pView = (PLOGSHARED)mmapLog->Lock();

			// Make sure another process hasn't signalled while we were waiting for the lock mutex
			if (pView && WaitForSingleObject(hLogSignal, LOGTIMEOUT) == WAIT_OBJECT_0)
			{
				mmapLog->Release();
				pView = NULL;
			}
		}
		else
			Sleep(LOGTIMEOUT);
	}

	return pView;
}

bool LogSendMessage(LPCTSTR msg, UINT32 level)
{
	if (!bLogInit)
		if (!InitLog())
			return false;

	if (!mmapLog || lstrlen(msg) + 1 > LOGBUFSIZE - 1)
		return false;

	// Lock the memory map
	PLOGSHARED pLog = WaitForLockAndQueueEmpty();
	if (!pLog)
		return false;

	StringCchCopy(pLog->chLogMsg, LOGBUFSIZE - 1, msg);
	pLog->nLevel = level;

	// Signal the log window. If we release the lock before signalling the event, another process
	// could overwrite the message we just sent.
	if (!SetEvent(hLogSignal))
	{
		// SetEvent failed
		mmapLog->Release();
		return false;
	}

	// We're done with the memory map, release it
	mmapLog->Release();
	return true;
}

bool InitLog()
{
	try
	{
		mmapLog = new MMap(TEXT("multimon_log"), sizeof(LOGSHARED));
	}
	catch (...)
	{
		return false;
	}

	hLogSignal = OpenEvent(
		EVENT_ALL_ACCESS,
		FALSE,
		LOGSIGNALNAME
		);
	if (!hLogSignal)
	{
		MessageBox(NULL, TEXT("Unable to open log event!"), TEXT("Multimon Hook Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	bLogInit = true;
	return true;
}

bool ShutdownLog()
{
	if (hLogSignal)
		CloseHandle(hLogSignal);

	//if (mmapLog)
	//	delete mmapLog;

	return true;
}
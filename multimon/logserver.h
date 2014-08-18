// logserver.h : A multi-process log receiver.
// Requires the mmap library class.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#pragma once
#include <string>
#include <vector>

#include "mmap.h"

#define LOGBUFSIZE 513

#define LOG_ERROR 0
#define LOG_WARN 1
#define LOG_INFO 2

//#define DLLSIGNALNAME TEXT("multimon_mmap_dll_update")
#define LOGEXITNAME   TEXT("_multimon_log_pump_exit")
#define LOGSIGNALNAME TEXT("_multimon_log_message")

#define LOG_EVENT_READ 0
#define LOG_EVENT_EXIT 1
#define NUM_LOG_EVENTS 2


typedef struct _tag_LOGSHARED
{
	UINT32	nLevel;
	UINT32	cbMsgSize;
	TCHAR	chLogMsg[LOGBUFSIZE];
} LOGSHARED, *PLOGSHARED;


class Log
{
private:
	MMap *mmap;

	HANDLE hEvents[NUM_LOG_EVENTS];
	HANDLE hPump;

	HWND hWnd;

	//std::vector<std::wstring> lines;
	std::wstring buffer;

	std::wstring *RetrieveMessage();

	static DWORD WINAPI Pump(Log *pThis);

public:
	Log(HWND hwnd);
	~Log();
};
// log.h : A multi-process log sender.
// Requires the mmap library class.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#pragma once
#include "../multimon/mmap.h"

#define LOGBUFSIZE 513

#define LOG_ERROR 0
#define LOG_WARN 1
#define LOG_INFO 2

#define LOGSIGNALNAME TEXT("_multimon_log_message")
#define DLLSIGNALNAME TEXT("_multimon_mmap_dll_update")


typedef struct _tag_LOGSHARED
{
	UINT32	nLevel;
	UINT32	cbMsgSize;
	TCHAR	chLogMsg[LOGBUFSIZE];
} LOGSHARED, *PLOGSHARED;


#if defined(LOG_CPP)
bool bLogInit = false;

MMap *mmapLog = NULL;
HANDLE hLogSignal = NULL;
#else
//HANDLE hDllSignal;
#endif


bool InitLog();
bool ShutdownLog();

PLOGSHARED WaitForLockAndQueueEmpty();
bool LogSendMessage(LPCTSTR msg, UINT32 level);
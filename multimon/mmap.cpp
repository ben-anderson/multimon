// mmap.cpp : A multi-process shared memory implementation.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#include "stdafx.h"
#include <string>

#include "mmap.h"

#define MMAPTIMEOUT 100

#define MMAPPREFIX TEXT("_mmap_")
#define MMAPMUTEXPREFIX TEXT("_mmap_mutex_")


LPVOID MMap::Lock()
{
	if (lpView == NULL || hMmapMutex == NULL || WaitForSingleObject(hMmapMutex, MMAPTIMEOUT) != WAIT_OBJECT_0)
		return NULL;

	return lpView;
}

void MMap::Release()
{
	if (hMmapMutex)
		ReleaseMutex(hMmapMutex);
}


MMap::MMap(LPCTSTR name, size_t size)
{
	std::wstring mmapName(MMAPPREFIX);
	mmapName += name;

	std::wstring mutexName(MMAPMUTEXPREFIX);
	mutexName += name;

	hMmap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, mmapName.c_str());
	if (!hMmap)
		throw TEXT("Could not create file mapping!");

	// Check if we created or opened the file mapping
	DWORD dwStatus = GetLastError();

	lpView = MapViewOfFile(hMmap, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (!lpView)
		throw TEXT("Unable to map view of file!");

	// Erase the memory if we're the creator
	if (dwStatus != ERROR_ALREADY_EXISTS)
		ZeroMemory(lpView, size);

	hMmapMutex = CreateMutex(NULL, FALSE, mutexName.c_str());
	if (!hMmapMutex)
		throw TEXT("Unable to create mmap mutex!");
}

MMap::~MMap()
{
	if (hMmapMutex)
	{
		CloseHandle(hMmapMutex);
		hMmapMutex = NULL;
	}

	if (lpView)
	{
		UnmapViewOfFile(lpView);
		lpView = NULL;
	}

	if (hMmap)
	{
		CloseHandle(hMmap);
		hMmap = NULL;
	}
}
// mmap.h : A multi-process shared memory implementation.
//
// Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
//

#pragma once

class MMap
{
private:
	HANDLE hMmap;
	HANDLE hMmapMutex;

	LPVOID lpView;

public:
	MMap(LPCTSTR name, size_t size);
	~MMap();

	LPVOID Lock();
	void Release();
};

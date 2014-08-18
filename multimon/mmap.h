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

#include <windows.h>
#include "sharemem.h"

HANDLE hMapping;

MMD_CAMERA *Getshare(void)
{
//	DWORD dwLastErr;
	MMD_CAMERA *mmd_cam;

	hMapping=CreateFileMappingA(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0, (DWORD)sizeof(MMD_CAMERA),
		"Share_MMD_Camera"
	);
//	dwLastErr=GetLastError();
  
	mmd_cam = (MMD_CAMERA *)MapViewOfFile(
		hMapping,
		FILE_MAP_ALL_ACCESS,
		0, 0, 0
	);
	return mmd_cam;
}

void Closeshare(MMD_CAMERA *mmd_cam)
{
	UnmapViewOfFile(mmd_cam);
	CloseHandle(hMapping);
}

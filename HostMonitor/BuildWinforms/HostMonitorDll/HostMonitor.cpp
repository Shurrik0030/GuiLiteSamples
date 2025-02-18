// HostMonitorDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <assert.h>
#include <shellapi.h>
#include <wchar.h>
#include <malloc.h>

typedef int(*SYNC_DATA)(int hr, int spo2, int rr, int nibp_sys, int nibp_dia, int nibp_mean);
extern SYNC_DATA gSyncData;
extern void startHostMonitor(void* phy_fb, int screen_width, int screen_height, int color_bytes);
extern void* getUiOfHostMonitor(int* width, int* height, bool force_update);
extern void sendTouch2HostMonitor(int x, int y, bool is_down);

int sync_data(int hr, int spo2, int rr, int nibp_sys, int nibp_dia, int nibp_mean)
{
	return 0;
	wchar_t arguments[128];
	memset(arguments, 0, sizeof(arguments));
	swprintf(arguments, 128, L"/C sync_data.bat %d %d %d %d %d %d WinForms",
		hr, spo2, rr, nibp_sys, nibp_dia, nibp_mean);
	HINSTANCE ret = ShellExecute(NULL, L"open", L"cmd.exe", arguments, NULL, SW_HIDE);
	return 0;
}

DWORD WINAPI threadHostMonitor(LPVOID lpParam)
{
	gSyncData = sync_data;
	sync_data(60, 98, 30, 120, 80, 100);//Ping cloud
	startHostMonitor(calloc(1024 * 768, 4), 1024, 768, 4);
	return 0;
}

extern "C" __declspec(dllexport) int startHostMonitorFromDll()
{
	ShellExecute(0, L"open", L"https://github.com/idea4good/GuiLite", L"", L"", SW_SHOWNORMAL);
	DWORD pid;
	CreateThread(NULL, 0, threadHostMonitor, NULL, 0, &pid);
	return 0;
}

extern "C" __declspec(dllexport) HBITMAP getHbitmapOfHostMonitorUiFromDll(bool force_update)
{
	static HBITMAP s_bitmap;
	int width, height;
	BYTE* buffer = (BYTE*)getUiOfHostMonitor(&width, &height, force_update);
	if (buffer)
	{
		if (s_bitmap)
		{
			DeleteObject(s_bitmap);
		}
		s_bitmap = CreateBitmap(width, height, 1, 32, buffer);
		return s_bitmap;
	}
	return 0;
}

extern "C" __declspec(dllexport) void sendTouch2HostMonitorFromDll(int x, int y, bool is_down)
{
	sendTouch2HostMonitor(x, y, is_down);
}

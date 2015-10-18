//--------------------------------------------------------------------------
// dllmain.cpp - Entry point for shared library
//--------------------------------------------------------------------------
// Author: Lam H. Dao <daohailam(at)yahoo(dot)com>
//--------------------------------------------------------------------------
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//--------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#include "resource.h"
//--------------------------------------------------------------------------
namespace WinDLL {
//--------------------------------------------------------------------------
static HINSTANCE self;
//--------------------------------------------------------------------------
void About()
{
	static HWND about = 0;
	bool first_time = about == 0;
	about = CreateDialogParam(self,MAKEINTRESOURCE(IDD_ABOUT),0,(DLGPROC)DefWindowProc,0);
	if (!about)
		return;
	if (!first_time) {
		SetWindowText(about, L"About");
	}
	ShowWindow(about, SW_SHOW);
	Sleep(1500);
	DestroyWindow(about);
}
//--------------------------------------------------------------------------
} // namespace WinDLL
//-------------------------------------------------------------------------
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason){
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(WinDLL::self=hModule);
			WinDLL::About();
			break;
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}
//-------------------------------------------------------------------------
#endif

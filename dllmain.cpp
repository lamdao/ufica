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
// Windows 头文件:
#include <windows.h>
#include <commctrl.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "MoonChildApp.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if( RunDlg() == 0 )
		return 0;
	MoonChildApp theApp;

	InitCommonControls();
	if(FAILED(theApp.Create(hInstance) ) )
		return 0;
	return theApp.Run();
}
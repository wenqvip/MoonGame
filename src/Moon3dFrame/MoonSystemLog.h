#pragma once
#include "MoonUtility.h"

class MoonSystemLog
{
public:
	MoonSystemLog(void);
	virtual ~MoonSystemLog(void);
/////////////////////////////////////////
private:
	static bool m_bEnableLogging;
	static LPSYSTEMTIME m_time;

protected:

	/*D3DXVECTOR3 GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2,
		D3DXVECTOR3* vVertex3);*/

	static void StartLogging();
	static void StopLogging();
	static void LogError(char *lpszText, ...);
	static void LogInfo(char *lpszText, ...);
	static void LogWarning(char *lpszText, ...);
	static DWORD GetMemoryUsage();
	static void LogMemoryUsage();
};

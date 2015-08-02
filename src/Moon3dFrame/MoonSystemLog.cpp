#include "MoonSystemLog.h"

MoonSystemLog::MoonSystemLog(void)
{
}

MoonSystemLog::~MoonSystemLog(void)
{
}

bool MoonSystemLog::m_bEnableLogging = false;
LPSYSTEMTIME MoonSystemLog::m_time = new SYSTEMTIME();

void MoonSystemLog::LogError(char *lpszText, ...)
{
	if(m_bEnableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;
		GetLocalTime(m_time);

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen("Moon3D.log", "a+");

		if(pFile != NULL)
		{
			fprintf(pFile, "[%02d:%02d:%02d] ERROR: ",
				m_time->wHour, m_time->wMinute, m_time->wSecond);
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}

void MoonSystemLog::LogInfo(char *lpszText, ...)
{
	if(m_bEnableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;
		GetLocalTime(m_time);

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen("Moon3D.log", "a+");

		if(pFile != NULL)
		{
			fprintf(pFile, "[%02d:%02d:%02d] ",
				m_time->wHour, m_time->wMinute, m_time->wSecond);
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}

void MoonSystemLog::LogWarning(char *lpszText, ...)
{
	if(m_bEnableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;
		GetLocalTime(m_time);

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen("Moon3D.log", "a+");

		if(pFile != NULL)
		{
			fprintf(pFile, "[%02d:%02d:%02d] WORNING: ",
				m_time->wHour, m_time->wMinute, m_time->wSecond);
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}

void MoonSystemLog::StartLogging()
{
	FILE* pFile = NULL;
	GetLocalTime(m_time);

	//OPen the file and clear the contents
	pFile = fopen("Moon3D.log", "w");

	if(pFile != NULL)
	{
		fprintf(pFile, "Moon 3D log\nCreated(or opened) on %02d/%02d/%04d %02d:%02d:%02d\n\n",
			m_time->wDay, m_time->wMonth, m_time->wYear, m_time->wHour, m_time->wMinute, m_time->wSecond);
		//Close the file
		fclose(pFile);
		m_bEnableLogging = true;
	}
	else
		::MessageBox(0, "Cannot open the log file",0,0);
}

void MoonSystemLog::StopLogging()
{
	if(m_bEnableLogging)
	{
		FILE *pFile = NULL;
		GetLocalTime(m_time);

		//Open the log file for appending
		pFile = fopen("Moon3D.log", "a+");
		
		if(pFile != NULL)
		{
			fprintf(pFile, "\n[%02d:%02d:%02d] Log file closed.\n",
				m_time->wHour, m_time->wMinute, m_time->wSecond);

			//Close the file
			fclose(pFile);
		}

		m_bEnableLogging = false;
	}
}

DWORD MoonSystemLog::GetMemoryUsage()
{
	//This is slow. Don't use it inside the game loop!

	DWORD dwProcessID	= GetCurrentProcessId();
	HANDLE hSnapshot	= CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST
		|TH32CS_SNAPMODULE, dwProcessID);
	DWORD dwSize		= 0;		
	// Will contain the total memory usage when we're done!

	MODULEENTRY32 module;
	module.dwSize = sizeof(module);


	//Get memory used by modules (dlls etc)
	if (Module32First(hSnapshot, &module))
	{
  		do
  		{
   			dwSize += module.modBaseSize;
			//LogInfo("<li>%s", module.szModule);	
			//Log the module name that we are using
  		}
		while (Module32Next(hSnapshot, &module));
	}


	HEAPLIST32 heap;
	heap.dwSize = sizeof(heap); 

	//Get all memory used by the heap
	if (Heap32ListFirst(hSnapshot, &heap))
	{
		do
  		{
   			HEAPENTRY32 heapentry;
			heapentry.dwSize = sizeof(heapentry);
   			
			if (Heap32First(&heapentry,heap.th32ProcessID,heap.th32HeapID))
   			{
    			do
    			{
     				if (heapentry.dwFlags != LF32_FREE)	// If the block is currently used
					{
      					dwSize += heapentry.dwBlockSize;
					}
    			}
				while (Heap32Next(&heapentry));
   			}
  		}
		while (Heap32ListNext(hSnapshot,&heap));
	}

	CloseHandle(hSnapshot);

	return dwSize;
}


void MoonSystemLog::LogMemoryUsage()
{
	DWORD dwMemoryUsage = GetMemoryUsage();
	float rMemoryUsage = 0.0f;
	
	if(dwMemoryUsage < 1024)
	{
		LogInfo("Memory Usage: %d Bytes", dwMemoryUsage);
	}
	else if(dwMemoryUsage < 1048576)
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1024.0f);
		LogInfo("Memory Usage: %f KB", rMemoryUsage);
	}
	else if(dwMemoryUsage < 1073741824)
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1048576.0f);
		LogInfo("Memory Usage: %f MB", rMemoryUsage);
	}
	else
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1073741824.0f);
		LogInfo("Memory Usage: %f GB", rMemoryUsage);
	}
}

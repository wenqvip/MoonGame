#pragma once
#include <windows.h>

class MoonTimer
{
public:
	MoonTimer(void);
	~MoonTimer(void);
private:
	float m_fBaseTime;
	float m_fLastElapsedTime;
	float m_fStopTime;
	bool m_bTimerStopped;
public:
	float Start(void);
	float Reset(void);
	float Stop(void);
	float GetAppTime(void);
	float GetElapsedTime(void);
};

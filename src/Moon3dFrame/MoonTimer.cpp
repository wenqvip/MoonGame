#include "MoonTimer.h"

MoonTimer::MoonTimer(void)
: m_fBaseTime(0.0f)
, m_fLastElapsedTime(0.0)
, m_fStopTime(0.0f)
, m_bTimerStopped(false)
{
}

MoonTimer::~MoonTimer(void)
{
}

float MoonTimer::Start(void)
{
	float fTime = timeGetTime() * 0.001f;
	if(m_bTimerStopped)
		m_fBaseTime += fTime - m_fStopTime;
	m_fStopTime = 0.0f;
	m_fLastElapsedTime = fTime;
	m_bTimerStopped = false;
	return 0.0;
}

float MoonTimer::Reset(void)
{
	float fTime = timeGetTime() * 0.001f;
	m_fBaseTime = fTime;
	m_fLastElapsedTime = fTime;
	m_fStopTime = 0;
	m_bTimerStopped = false;
	return 0;
}

float MoonTimer::Stop(void)
{
	float fTime = timeGetTime() * 0.001f;
	if(!m_bTimerStopped)
	{
		m_fStopTime = fTime;
		m_fLastElapsedTime = fTime;
		m_bTimerStopped = true;
	}
	return 0;
}

float MoonTimer::GetAppTime(void)
{
	float fTime = timeGetTime() * 0.001f;
	return fTime - m_fBaseTime;
}

float MoonTimer::GetElapsedTime(void)
{
	float fTime = timeGetTime() * 0.001f;
	float fElapsedTime = fTime - m_fLastElapsedTime;
	m_fLastElapsedTime = fTime;
	return fElapsedTime;
}

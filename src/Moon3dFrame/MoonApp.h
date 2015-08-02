#pragma once
#include "MoonSystemLog.h"
#include "MoonTimer.h"
#include "MoonSetting.h"
#include "MoonFont.h"

class MoonApp : public MoonSystemLog
{
public:
	MoonApp(void);
	virtual ~MoonApp(void);
	HRESULT Create(HINSTANCE hInst);
	int Run(void);
	virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Pause(bool bPause);
	/*void ShowCursor(bool show);*/
protected:
	// 显卡编号
	UINT					m_nAdapter;
	MoonSetting				m_cMoonSetting;
	D3DPRESENT_PARAMETERS	m_d3dpp;
	HWND					m_hWnd;
	LPDIRECT3D9				m_pD3D;
	LPDIRECT3DDEVICE9		m_pDevice;
	D3DCAPS9				m_d3dCaps;
	LPDIRECTINPUT8			m_pDirectInput; 
	LPDIRECTINPUTDEVICE8	m_pKeyboard;
	LPDIRECTINPUTDEVICE8	m_pMouse; 
	DWORD					m_dwCreationWidth;
	DWORD					m_dwCreationHeight;
	DWORD					m_dwWindowStyle;
	string					m_strWindowTitle;
	RECT					m_rcWindowBounds;
	RECT					m_rcWindowClient;
	bool					m_bIgnoreSizeChange;//全屏时忽视窗口大小的改变
	MoonFont*				m_pFont;

	bool m_bWindowed;
	bool m_bActive;
	bool m_bDeviceLost;
	bool m_bMinimized;
	bool m_bMaximized;
	bool m_bDeviceObjectsRestored;
	bool m_bDeviceObjectsInited;
	/*bool m_bShowCursor;*/

	char			m_KeyboardState[256];
	DIMOUSESTATE	m_MouseState;
	
	bool m_bFrameMoving;
	
	MoonTimer*	m_pTimer;
	float		m_fStartTime;
	float		m_fTime;
	float		m_fElapsedTime;
	float		m_fFPS;
	TCHAR		m_strFrameStats[90];

	HRESULT				Render3DEnvironment(void);
	HRESULT				Reset3DEnvironment(void);
	HRESULT				Initialize3DEnvironment(void);
	HRESULT				InitializeDirectinput(HINSTANCE hInst);
	void				CleanupDirectinput(void);
	void				Cleanup3DEnvironment(void);
	HRESULT				HandlePossibleSizeChange(void);
	HRESULT				ToggleFullscreen(void);
	HRESULT				AdjustWindowForChange(void);
	void				UpdateSetting(void);
	HRESULT				OnChangeSetting(void);
	void				ProcessKeyboard(void);
	void				ProcessMouse(void);

	virtual void		UpdateStats(void);
	virtual HRESULT FrameMove(void)					{	return S_OK;	}
	virtual HRESULT Render(void)					{	return S_OK;	}
	virtual HRESULT OneTimeSceneInit(void)			{	return S_OK;	}
	virtual HRESULT FinalCleanup(void)				{	return S_OK;	}
	virtual HRESULT InitDeviceObjects(void)			{	return S_OK;	}
	virtual HRESULT RestoreDeviceObjects(void)		{	return S_OK;	}
	virtual HRESULT InvalidateDeviceObjects(void)	{	return S_OK;	}
	virtual HRESULT DeleteDeviceObjects(void)		{	return S_OK;	}
};

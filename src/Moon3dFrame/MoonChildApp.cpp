#include "MoonChildApp.h"

//mydata g_data;

MoonChildApp::MoonChildApp(void)
{
	_AppState = MAINMENU;
	char ch[256];
	GetPrivateProfileString("scene","scene","default",ch,256,"./start.cfg");
	_scnname = ch;
	m_strWindowTitle = "UnrealWorld";
	m_strWindowTitle += "  SceneName: ";
	m_strWindowTitle += ch;
	_normalCursor = LoadCursorFromFile("../media/cursors/normal.ani");
	the3DText = NULL;
	theScene = NULL;
	_StartPanel = NULL;
	_LoadingPanel = NULL;
}

MoonChildApp::~MoonChildApp(void)
{
}

HRESULT MoonChildApp::FrameMove(void)
{
	POINT pos;
	GetCursorPos(&pos);
	bool bTemp;
	if(MOUSEBUTTONDOWN(m_MouseState.rgbButtons[0]))
		bTemp = true;
	else
		bTemp = false;
	if(m_bWindowed)
	{
		int ntemp = (m_rcWindowBounds.right-m_rcWindowClient.right-m_rcWindowBounds.left)/2;
		pos.x-=(ntemp+m_rcWindowBounds.left);
		pos.y+=(m_rcWindowClient.bottom-m_rcWindowBounds.bottom+ntemp);
	}

	if(_AppState == MAINMENU && _StartPanel)
	{
		DWORD value = _StartPanel->Test(pos.x,pos.y,bTemp);
		if(value == ID_EXIT)
		{
			SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
		}
		else if(value == ID_OK)
		{
			_AppState=LOADING;
			/*g_data.theapp=this;
			g_data.state = INSCENE;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LoadScene,
				NULL, 0, NULL);*/
		}
		else if(value == ID_ABOUT)
		{
			_AppState=ABOUT;
		}
	}
	else if(_AppState == INSCENE)
	{
		if(theScene)
		{
			if(theScene->GetKeyboardInput(m_KeyboardState, m_fElapsedTime)==ID_EXIT)
			{
				theScene->InvalidateDeviceObjects();
				theScene->DeleteDeviceObjects();
				theScene->Cleanup();
				_AppState=GAMETOMENU;
				return S_OK;
			}
			theScene->GetMouseInput(&m_MouseState, m_fElapsedTime);
			theScene->FrameMove(m_fElapsedTime, m_fTime);
		}
	}
	else if(_AppState == LOADING)
	{
		_StartPanel->Destroy();
		theScene->Initialize(&m_d3dCaps, _scnname.c_str());
		theScene->InitDeviceObjects(m_pDevice);
		theScene->RestoreDeviceObjects();
		_AppState = INSCENE;
	}
	else if(_AppState == GAMETOMENU)
	{
		if(m_bWindowed)
		{
			_StartPanel->Create(m_pDevice,"start.pnl",m_rcWindowClient.right,m_rcWindowClient.bottom,_Font);
		}
		else
		{
			_StartPanel->Create(m_pDevice,"start.pnl",m_cMoonSetting.m_nWidth,m_cMoonSetting.m_nHeight,_Font);
		}
		_AppState = MAINMENU;
	}
	else if(_AppState == ABOUT)
	{
		if(_AboutPanel->Test(pos.x,pos.y,bTemp)==ID_EXIT)
			_AppState = MAINMENU;
	}

	return S_OK;
}

HRESULT MoonChildApp::Render(void)
{
    //Clear the back buffer and depth buffer
	if(m_pDevice==NULL)
		return E_FAIL;
    m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);
    if( SUCCEEDED( m_pDevice->BeginScene() ) )
    {
		if(_AppState == INSCENE)
		{
			if(theScene)
				theScene->Draw();

			if(the3DText)
				the3DText->DrawTextx(D3DXVECTOR3(-90, 200, 300), D3DXCOLOR(1.0f,0.0f,0.0f,1.0f), 40, 0.0f);
		}
		else if(_AppState == MAINMENU)
		{
			_StartPanel->Draw();
		}
		else if(_AppState == LOADING)
			_LoadingPanel->Draw();
		else if(_AppState == ABOUT)
			_AboutPanel->Draw();

		if(m_pFont)
			m_pFont->DrawTextx(m_strFrameStats, 0, 0, D3DCOLOR_XRGB(64, 64, 255));
		// End the scene.
		m_pDevice->EndScene();
    }

	return S_OK;
}

HRESULT MoonChildApp::OneTimeSceneInit(void)
{
	SetCursor(_normalCursor);
	_LoadingPanel = new MoonUIPanel();
	_AboutPanel = new MoonUIPanel();
	theScene = new MoonScene();
	/*if(!theScene->Initialize(&m_d3dCaps, _scnname.c_str()))
	{
		LogError("ChildMoonApp::OneTimeSceneInit() Initialize MoonScene Failed!");
		SAFE_DELETE(theScene);
		return E_FAIL;
	}*/
	_StartPanel = new MoonUIPanel();

	return S_OK;
}

HRESULT MoonChildApp::FinalCleanup(void)
{
	if(_AppState==INSCENE && theScene)
		theScene->Cleanup();
	SAFE_DELETE(theScene);
	SAFE_DELETE(_StartPanel);
	SAFE_DELETE(_LoadingPanel);

	return S_OK;
}

HRESULT MoonChildApp::InitDeviceObjects(void)
{
	if(_AppState==INSCENE && theScene)
		theScene->InitDeviceObjects(m_pDevice);

	the3DText = new Moon3DFont(m_pDevice, "Snap ITC", "Unreal World", 2.0f);
	LogInfo("ChildMoonApp::InitDeviceObjects() Create Moon3DFont OK!");

	return S_OK;
}

HRESULT MoonChildApp::DeleteDeviceObjects(void)
{
	if(_AppState==INSCENE && theScene)
		theScene->DeleteDeviceObjects();
	SAFE_DELETE(the3DText);
	return S_OK;
}

HRESULT MoonChildApp::RestoreDeviceObjects(void)
{
	HRESULT hr;

	m_pFont = new MoonFont(m_pDevice, "Verdana", 20, false, false, false);
	_Font = new MoonFont(m_pDevice,"Î¢ÈíÑÅºÚ",20,true,false,false);

	if(_AppState==INSCENE && theScene)
		hr = theScene->RestoreDeviceObjects();

	if(m_bWindowed)
	{
		_StartPanel->Create(m_pDevice,"start.pnl",m_rcWindowClient.right,m_rcWindowClient.bottom,_Font);
		_LoadingPanel->Create(m_pDevice,"load.pnl",m_rcWindowClient.right,m_rcWindowClient.bottom,_Font);
		_AboutPanel->Create(m_pDevice,"about.pnl",m_rcWindowClient.right,m_rcWindowClient.bottom,_Font);
	}
	else
	{
		_StartPanel->Create(m_pDevice,"start.pnl",m_cMoonSetting.m_nWidth,m_cMoonSetting.m_nHeight,_Font);
		_LoadingPanel->Create(m_pDevice,"load.pnl",m_cMoonSetting.m_nWidth,m_cMoonSetting.m_nHeight,_Font);
		_AboutPanel->Create(m_pDevice,"about.pnl",m_cMoonSetting.m_nWidth,m_cMoonSetting.m_nHeight,_Font);
	}
	
    D3DXMATRIX matProj;
    FLOAT fAspect = m_d3dpp.BackBufferWidth / (float)m_d3dpp.BackBufferHeight;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, fAspect, 0.1f, 3000.0f );
	hr = m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	if(SUCCEEDED(hr))
		LogInfo("ChildMoonApp::RestoreDeviceObjects() Set projection OK!");
	else
		LogError("ChildMoonApp::RestoreDeviceObjects() Set projection Failed!");

	return S_OK;
}

HRESULT MoonChildApp::InvalidateDeviceObjects(void)
{
	if(_AppState==INSCENE && theScene)
		theScene->InvalidateDeviceObjects();
	if(_StartPanel)
		_StartPanel->Destroy();
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(_Font);
	return S_OK;
}

//void LoadScene()
//{
//	if(g_data.theapp->_AppState == g_data.state)
//		return;
//	g_data.theapp->theScene->Initialize(&g_data.theapp->m_d3dCaps, g_data.theapp->_scnname.c_str());
//	g_data.theapp->theScene->InitDeviceObjects(g_data.theapp->m_pDevice);
//	g_data.theapp->theScene->RestoreDeviceObjects();
//	g_data.theapp->_AppState=g_data.state;
//}
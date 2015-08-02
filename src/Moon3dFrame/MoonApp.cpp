#include "MoonApp.h"


static MoonApp* g_pMoonApp = NULL;

MoonApp::MoonApp(void)
: m_nAdapter(0)
{
	g_pMoonApp		=	this;

	m_pD3D			=	NULL;
	m_pDevice		=	NULL;
	m_pKeyboard		=	NULL;
	m_pMouse		=	NULL;
	m_hWnd			=	NULL;

	m_dwWindowStyle			=	0;
	m_bMinimized			=	false;
	m_bMaximized			=	false;
	m_bIgnoreSizeChange		=	false;
	/*m_bShowCursor			=	true;*/
	m_strFrameStats[0]		=	0;

	m_bActive		=	true;
	m_bFrameMoving	=	true;
	m_bDeviceLost	=	false;
	m_bDeviceObjectsRestored	=	false;
	m_bDeviceObjectsInited		=	false;

	m_pTimer = new MoonTimer();
	m_fStartTime = 0;
	m_fTime = 0;
	m_fElapsedTime = 0;
	m_fFPS = 0.0;
}

MoonApp::~MoonApp(void)
{
	LogMemoryUsage();
	StopLogging();
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return g_pMoonApp->MsgProc( hWnd, uMsg, wParam, lParam );
}

HRESULT MoonApp::Create(HINSTANCE hInst)
{
	HRESULT hr;
	StartLogging();

	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(m_pD3D == NULL)
		return E_NOTIMPL;

	UpdateSetting();

	//创建窗口
	if(m_hWnd == NULL)
	{
		WNDCLASS wndClass = { 0, WndProc, 0, 0, hInst,
                              LoadIcon( hInst, MAKEINTRESOURCE(IDI_MOON) ),
                              LoadCursorFromFile("../media/cursors/normal.ani"),
                              (HBRUSH)GetStockObject(WHITE_BRUSH),
                              NULL, _T("Moon 3d Frame") };
        RegisterClass( &wndClass );
		
        // Set the window's initial style
        m_dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

        // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, m_dwCreationWidth, m_dwCreationHeight );
        AdjustWindowRect( &rc, m_dwWindowStyle, true );

        // Create the render window
        m_hWnd = CreateWindowA( _T("Moon 3d Frame"), m_strWindowTitle.c_str(), m_dwWindowStyle,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               (rc.right-rc.left), (rc.bottom-rc.top), 0,
                               NULL,
                               hInst, 0 );
	}
	
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
	GetClientRect( m_hWnd, &m_rcWindowClient );
	//Adjust the Client Rect of the window
	SetWindowPos(m_hWnd, HWND_NOTOPMOST,
		m_rcWindowBounds.left, m_rcWindowBounds.top, 
		( m_rcWindowBounds.right - m_rcWindowBounds.left +m_dwCreationWidth-m_rcWindowClient.right),
		( m_rcWindowBounds.bottom - m_rcWindowBounds.top +m_dwCreationHeight-m_rcWindowClient.bottom),
		SWP_SHOWWINDOW );

    GetWindowRect( m_hWnd, &m_rcWindowBounds );
	GetClientRect( m_hWnd, &m_rcWindowClient );
	
	AdjustWindowForChange();

	LogInfo("MoonApp::Create(): Window is Created.");

	// Initialize Directinput
	InitializeDirectinput(hInst);

    // Initialize the app's custom scene stuff
    if( FAILED( hr = OneTimeSceneInit() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return E_FAIL;
    }

    // Initialize the 3D environment for the app
    if( FAILED( hr = Initialize3DEnvironment() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return E_FAIL;
    }
	
	m_pTimer->Start();

	return S_OK;
}

int MoonApp::Run(void)
{
	// Load keyboard accelerators
	HACCEL hAccel = LoadAccelerators( NULL, MAKEINTRESOURCE(IDR_ACCEL) );

	// Now we're ready to recieve and process Windows messages.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  )
	{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		static long input = 0, render = 0;
		if( m_bActive )
		{
            bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );
		}
        else
            bGotMsg = ( GetMessage( &msg, NULL, 0U, 0U ) != 0 );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            if( hAccel == NULL || m_hWnd == NULL || 
                0 == TranslateAccelerator( m_hWnd, hAccel, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
            //if( m_bDeviceLost )
            //{
            //    // Yield some CPU time to other processes
            //    Sleep( 100 ); // 100 milliseconds
            //}
            // Render a frame during idle time (no messages are waiting)
            if( m_bActive )
            {
                if( FAILED( Render3DEnvironment() ) )
                    SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
            }
        }
    }
    if( hAccel != NULL )
        DestroyAcceleratorTable( hAccel );

    return (INT)msg.wParam;
}

LRESULT MoonApp::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
		case WM_SETFOCUS:
			if(m_pKeyboard)
				m_pKeyboard->Acquire();
			if(m_pMouse)
				m_pMouse->Acquire();
			break;
		case WM_KILLFOCUS:
			if(m_pKeyboard)
				m_pKeyboard->Unacquire();
			if(m_pMouse)
				m_pMouse->Unacquire();
			break;
		case WM_COMMAND:
			switch( LOWORD(wParam) )
            {
				case IDM_FULLSCREEN:
					if(FAILED(ToggleFullscreen()))
						SendMessage( hWnd, WM_CLOSE, 0, 0 );
					break;
			}
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_F2:
					if(!m_bWindowed)
						if(FAILED(ToggleFullscreen()))
							SendMessage( hWnd, WM_CLOSE, 0, 0 );
					Pause(true);
					if(RunDlg()==1)
						OnChangeSetting();
					Pause(false);
					break;
			}
			break;

        case WM_SIZE:
            // Pick up possible changes to window style due to maximize, etc.
            if( m_bWindowed && m_hWnd != NULL )
                m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );

            if( SIZE_MINIMIZED == wParam )
            {
                //if( m_bClipCursorWhenFullscreen && !m_bWindowed )
                //    ClipCursor( NULL );
                Pause( true ); // Pause while we're minimized
                m_bMinimized = true;
                m_bMaximized = false;
            }
            else if( SIZE_MAXIMIZED == wParam )
            {
                if( m_bMinimized )
                    Pause( false ); // Unpause since we're no longer minimized
                m_bMinimized = false;
                m_bMaximized = true;
                if(FAILED(HandlePossibleSizeChange()))
					SendMessage( hWnd, WM_CLOSE, 0, 0 );
            }
            else if( SIZE_RESTORED == wParam )
            {
                if( m_bMaximized )
                {
                    m_bMaximized = false;
					if(FAILED(HandlePossibleSizeChange()))
						SendMessage( hWnd, WM_CLOSE, 0, 0 );
                }
                else if( m_bMinimized)
                {
                    Pause( false ); // Unpause since we're no longer minimized
                    m_bMinimized = false;
					if(FAILED(HandlePossibleSizeChange()))
						SendMessage( hWnd, WM_CLOSE, 0, 0 );
                }
                else
                {
                    // If we're neither maximized nor minimized, the window size 
                    // is changing by the user dragging the window edges.  In this 
                    // case, we don't reset the device yet -- we wait until the 
                    // user stops dragging, and a WM_EXITSIZEMOVE message comes.
                }
            }
            break;
			
        case WM_EXITSIZEMOVE:
            Pause( false );
            if(FAILED(HandlePossibleSizeChange()))
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
            break;

        case WM_ENTERSIZEMOVE:
            // Halt frame movement while the app is sizing or moving
            Pause( true );
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
			Cleanup3DEnvironment();
            SAFE_RELEASE( m_pD3D );
            FinalCleanup();
            DestroyWindow( hWnd );
			m_bActive = false;
            m_hWnd = NULL;
            return 0;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

HRESULT MoonApp::InitializeDirectinput(HINSTANCE hInst)
{
	HRESULT hr;

	//Create the DirectInput object
	if(FAILED(hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, 
			  IID_IDirectInput8, (void**)&m_pDirectInput, NULL))) 
	{ 
		LogError("MoonApp::InitializeDirectinput() Unable to create DirectInput interface.");
		return hr;
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() DirectInput interface created OK");
	}
	
	//KEYBOARD =======================================================================
	//Create the keyboard device object
	if(FAILED(hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL))) 
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to create DirectInput keyboard device interface.");
		return hr; 
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() DirectInput keyboard device interface created OK.");
	}

	//Set the data format for the keyboard
	if(FAILED(hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to set the keyboard data format.");
		return hr; 
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() Set the keyboard data format OK.");
	}

	//Set the cooperative level for the keyboard
	if(FAILED(hr = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to set the keyboard cooperative level.");
		return hr;
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() Set the keyboard cooperative level OK.");
	}

	//Acquire the keyboard
	if(m_pKeyboard)
	{
		hr = m_pKeyboard->Acquire(); 
	}


	//MOUSE =======================================================================
	//Create the mouse device object
	if(FAILED(hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to create DirectInput mouse device interface.");
		return hr; 
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() DirectInput mouse device interface created OK.");
	}

	//Set the data format for the mouse
	if(FAILED(hr = m_pMouse->SetDataFormat(&c_dfDIMouse)))
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to set the mouse data format.");
		return hr; 
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() Set the mouse data format OK.");
	}

	//Set the cooperative level for the mouse
	if(FAILED(hr = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{ 
		CleanupDirectinput();
		LogError("MoonApp::InitializeDirectinput() Unable to set the mouse cooperative level.");
		return hr;
	}
	else
	{
		LogInfo("MoonApp::InitializeDirectinput() Set the mouse cooperative level OK.");
	}

	//Acquire the mouse
	if(m_pMouse)
	{
		hr = m_pMouse->Acquire(); 
	}

	return hr;
}

void MoonApp::CleanupDirectinput()
{
	if(m_pKeyboard)
	{
		m_pKeyboard->Unacquire(); 
	}

	if(m_pMouse)
	{
		m_pMouse->Unacquire();
	}

	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pKeyboard);
	SAFE_RELEASE(m_pDirectInput);
	LogInfo("MoonApp::CleanupDirectinput() Success.");
}

void MoonApp::Pause(bool bPause)
{
    static DWORD dwAppPausedCount = 0;

    dwAppPausedCount += ( bPause ? +1 : -1 );
    m_bActive         = ( dwAppPausedCount ? false : true );

    // Handle the first pause request (of many, nestable pause requests)
    if( bPause && ( 1 == dwAppPausedCount ) )
    {
        // Stop the scene from animating
        if( m_bFrameMoving )
			m_pTimer->Stop();
    }

    if( 0 == dwAppPausedCount )
    {
        // Restart the timers
        if( m_bFrameMoving )
			m_pTimer->Start();
    }
}

HRESULT MoonApp::Render3DEnvironment(void)
{
    HRESULT hr;

    if( m_bDeviceLost )
    {
        // Test the cooperative level to see if it's okay to render
        if( FAILED( hr = m_pDevice->TestCooperativeLevel() ) )
        {
            // If the device was lost, do not render until we get it back
            if( D3DERR_DEVICELOST == hr )
                return S_OK;

            // Check if the device needs to be reset.
            if( D3DERR_DEVICENOTRESET == hr )
            {
                // If we are windowed, read the desktop mode and use the same format for
                // the back buffer
                if( m_bWindowed )
                {
                    //D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
                    //m_pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &m_d3dSettings.Windowed_DisplayMode );
                    //m_d3dpp.BackBufferFormat = m_d3dSettings.Windowed_DisplayMode.Format;
                }

                if( FAILED( hr = Reset3DEnvironment() ) )
                    return hr;
            }
            return hr;
        }
        m_bDeviceLost = false;
    }

	float fAppTime = m_pTimer->GetAppTime();
	float fElapsedTime = m_pTimer->GetElapsedTime();

	if( ( 0.0f == fElapsedTime )&& m_bFrameMoving )
	{
		return S_OK;
	}

	if(m_bFrameMoving)
	{
        // Store the time for the app
        m_fTime        = fAppTime;
        m_fElapsedTime = fElapsedTime;

		ProcessKeyboard();
		ProcessMouse();

		if(FAILED(hr=FrameMove()))
			return hr;
	}
	if(FAILED(hr=Render()))
		return hr;

	UpdateStats();

    hr = m_pDevice->Present( NULL, NULL, NULL, NULL );
    if( D3DERR_DEVICELOST == hr )
        m_bDeviceLost = true;

	return S_OK;
}

HRESULT MoonApp::Initialize3DEnvironment(void)
{
	HRESULT hr;

	hr = m_pD3D->CreateDevice(m_nAdapter, m_cMoonSetting.m_DeviceType, m_hWnd,
					m_cMoonSetting.m_nVertexProcessing, &m_d3dpp, &m_pDevice);
	if(FAILED(hr))
	{
		LogError("MoonApp::Initialize3DEnvironment() Cannot create D3D device.");
		return E_FAIL;
	}
	LogInfo("MoonApp::Initialize3DEnvironment() Create D3D device OK.");
	m_pDevice->GetDeviceCaps(&m_d3dCaps);
	
	// Initialize the app's device-dependent objects
	hr = InitDeviceObjects();
	if( FAILED(hr) )
	{
		DeleteDeviceObjects();
	}
	else
	{
		m_bDeviceObjectsInited = true;
		hr = RestoreDeviceObjects();
		if( FAILED(hr) )
		{
			InvalidateDeviceObjects();
		}
		else
		{
			m_bDeviceObjectsRestored = true;
			return S_OK;
		}
	}
	return E_FAIL;
}

void MoonApp::Cleanup3DEnvironment(void)
{

	if( m_pDevice != NULL)
	{
		if( m_bDeviceObjectsRestored )
		{
			m_bDeviceObjectsRestored = false;
			InvalidateDeviceObjects();
		}
        if( m_bDeviceObjectsInited )
        {
            m_bDeviceObjectsInited = false;
            DeleteDeviceObjects();
        }

		CleanupDirectinput();
		m_pDevice->Release();
		m_pDevice=NULL;
	}
	LogInfo("MoonApp::Cleanup3DEnvironment() All cleaned.");
}

void MoonApp::UpdateStats(void)
{
	static DWORD dwLastTime = 0;
	static DWORD dwFrames = 0;
	DWORD dwTime = timeGetTime();
	++dwFrames;

	if( dwTime - dwLastTime > 1000 )
	{
		m_fFPS = dwFrames * 1000.0f / (float)(dwTime - dwLastTime);
		dwLastTime = dwTime;
		dwFrames = 0;
		
        const int cchMaxFrameStats = sizeof(m_strFrameStats) / sizeof(TCHAR);
		sprintf_s(m_strFrameStats,"FPS:%.02f\0",m_fFPS);
        m_strFrameStats[cchMaxFrameStats - 1] = TEXT('\0');
	}
}

HRESULT MoonApp::HandlePossibleSizeChange(void)
{
    HRESULT hr = S_OK;
    RECT rcClientOld;
    rcClientOld = m_rcWindowClient;
	
    if( m_bIgnoreSizeChange )
        return S_OK;

    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

	if( rcClientOld.right - rcClientOld.left !=
        m_rcWindowClient.right - m_rcWindowClient.left ||
        rcClientOld.bottom - rcClientOld.top !=
        m_rcWindowClient.bottom - m_rcWindowClient.top)
    {
        Pause( true );

        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

        if( m_pDevice != NULL )
        {
            // Reset the 3D environment
            if( FAILED( hr = Reset3DEnvironment() ) )
            {
                //if( hr != D3DERR_OUTOFVIDEOMEMORY )
                //    hr = D3DAPPERR_RESETFAILED;
                //DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
				LogError("MoonApp::HandlePossibleSizeChange() Cannot reset device, Error code: 0x%X",hr);
				::MessageBox(m_hWnd, "Reset device failed, The game will shutdown.",0,0);
            }
        }
		LogInfo("Window size has changed to %d x %d.",m_d3dpp.BackBufferWidth,m_d3dpp.BackBufferHeight);
        Pause( false );
	}

	return hr;
}

HRESULT MoonApp::Reset3DEnvironment(void)
{
	HRESULT hr;

	if( m_bDeviceObjectsRestored )
	{
		m_bDeviceObjectsRestored = false;
		InvalidateDeviceObjects();
	}

	if( FAILED(hr = m_pDevice->Reset(&m_d3dpp)) )
	{
		LogError("MoonApp::Reset3DEnvironment() Reset device failed.");
		return hr;
	}
	LogInfo("MoonApp::Reset3DEnvironment() Reset device OK.");
	hr = RestoreDeviceObjects();
	if( FAILED(hr) )
	{
		InvalidateDeviceObjects();
		return hr;
	}
	m_bDeviceObjectsRestored = true;

	return S_OK;
}

HRESULT MoonApp::ToggleFullscreen(void)
{
	HRESULT hr;

	Pause(true);
	m_bIgnoreSizeChange = true;

	m_bWindowed = !m_bWindowed;
	m_d3dpp.Windowed = m_bWindowed;

	AdjustWindowForChange();

	if(m_bWindowed==false)
	{
		m_d3dpp.BackBufferWidth = m_dwCreationWidth;
		m_d3dpp.BackBufferHeight = m_dwCreationHeight;
	}
	else
	{
		m_d3dpp.BackBufferWidth = m_rcWindowClient.right;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom;
	}

	hr = Reset3DEnvironment();
	if(FAILED(hr))
	{
		m_bWindowed = !m_bWindowed;
		m_d3dpp.Windowed = m_bWindowed;

		AdjustWindowForChange();

		if(m_bWindowed==false)
		{
			m_d3dpp.BackBufferWidth = m_dwCreationWidth;
			m_d3dpp.BackBufferHeight = m_dwCreationHeight;
		}
		else
		{
			::MessageBox(m_hWnd, "不能转成全屏模式！", 0,0);
			m_d3dpp.BackBufferWidth = m_rcWindowClient.right;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom;
		}
		if( m_bDeviceObjectsRestored )
		{
			m_bDeviceObjectsRestored = false;
			InvalidateDeviceObjects();
		}
		if( m_bDeviceObjectsInited )
		{
			m_bDeviceObjectsInited = false;
			DeleteDeviceObjects();
		}
		hr = Initialize3DEnvironment();
		if(SUCCEEDED(hr))
		{
			m_bIgnoreSizeChange = false;
			Pause(false);
		}
		return hr;
	}

	if(m_bWindowed)
	{
		SetWindowPos(m_hWnd, HWND_NOTOPMOST,
			m_rcWindowBounds.left, m_rcWindowBounds.top,
            ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
            ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
            SWP_SHOWWINDOW );
		LogInfo("Toggle to window mode on %d x %d.",m_d3dpp.BackBufferWidth,m_d3dpp.BackBufferHeight);
	}
	else
		LogInfo("Toggle to fullscreen on %d x %d.",m_dwCreationWidth,m_dwCreationHeight);

	m_bIgnoreSizeChange = false;
	Pause(false);

	return hr;
}

HRESULT MoonApp::AdjustWindowForChange(void)
{
	if( m_bWindowed )
	{
        // Set windowed-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, m_dwWindowStyle );
	}
	else
	{
        // Set fullscreen-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
	}
	return S_OK;
}

void MoonApp::UpdateSetting(void)
{
	m_cMoonSetting.GetSetting();
	m_nAdapter = m_cMoonSetting.m_nAdapter;
	m_bWindowed = !m_cMoonSetting.m_bFullscreen;
	m_dwCreationWidth = m_cMoonSetting.m_nWidth;
	m_dwCreationHeight = m_cMoonSetting.m_nHeight;

	ZeroMemory(&m_d3dpp,sizeof(m_d3dpp));
	m_d3dpp.Windowed = m_bWindowed;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.BackBufferFormat = m_cMoonSetting.m_BufferFormat;
	m_d3dpp.BackBufferWidth = m_dwCreationWidth;
	m_d3dpp.BackBufferHeight = m_dwCreationHeight;
	m_d3dpp.hDeviceWindow = m_hWnd;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval = m_cMoonSetting.m_nPresentInterval;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	m_d3dpp.EnableAutoDepthStencil = true;

	if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_cMoonSetting.m_nAdapter, 
				m_cMoonSetting.m_DeviceType, m_cMoonSetting.m_BufferFormat, m_bWindowed, 
				D3DMULTISAMPLE_4_SAMPLES, NULL)))
	{
		m_d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
		LogInfo("MoonApp::UpdateSetting() MultisampleType: %dX.", (int)m_d3dpp.MultiSampleType);
	}
}

HRESULT MoonApp::OnChangeSetting(void)
{
	HRESULT hr;
	UINT nAdapterOld = m_nAdapter;
	D3DDEVTYPE DevtypeOld = m_cMoonSetting.m_DeviceType;
	UINT vpOld = m_cMoonSetting.m_nVertexProcessing;

	UpdateSetting();
	GetClientRect(m_hWnd,&m_rcWindowClient);
	if( m_rcWindowClient.right != m_dwCreationWidth 
			&& m_rcWindowClient.bottom != m_dwCreationHeight )
	{
		GetWindowRect(m_hWnd,&m_rcWindowBounds);
		//Adjust the Client Rect of the window
		SetWindowPos(m_hWnd, HWND_NOTOPMOST,
			m_rcWindowBounds.left, m_rcWindowBounds.top, 
			( m_rcWindowBounds.right - m_rcWindowBounds.left +m_dwCreationWidth-m_rcWindowClient.right),
			( m_rcWindowBounds.bottom - m_rcWindowBounds.top +m_dwCreationHeight-m_rcWindowClient.bottom),
		SWP_SHOWWINDOW );
	}

	if( nAdapterOld == m_nAdapter && DevtypeOld == m_cMoonSetting.m_DeviceType
			&& vpOld == m_cMoonSetting.m_nVertexProcessing )
	{
		hr = Reset3DEnvironment();
	}
	else
	{
		Cleanup3DEnvironment();
		hr = Initialize3DEnvironment();
	}
	return hr;
}

void MoonApp::ProcessKeyboard(void)
{
	HRESULT hr;
	hr = m_pKeyboard->GetDeviceState(sizeof(m_KeyboardState),(LPVOID)&m_KeyboardState);
    if( hr == DIERR_INPUTLOST )
    { 
		m_pKeyboard->Acquire() ; 
    }
}

void MoonApp::ProcessMouse(void)
{
	HRESULT hr;
	hr = m_pMouse->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState);
	if( hr == DIERR_INPUTLOST )
    { 
		m_pMouse->Acquire() ; 
    }
}

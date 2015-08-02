#pragma once
#include "MoonApp.h"
#include "MoonCamera.h"
#include "Moon3DFont.h"
#include "MoonScene.h"
#include "MoonUIPanel.h"

//struct mydata;

class MoonChildApp : public MoonApp
{
public:
	MoonChildApp(void);
	virtual ~MoonChildApp(void);

protected:
	AppState _AppState;
	MoonUIPanel* _StartPanel;
	MoonUIPanel* _LoadingPanel;
	MoonUIPanel* _AboutPanel;
	MoonScene * theScene;
	Moon3DFont * the3DText;
	string _scnname;
	HCURSOR _normalCursor;
	MoonFont* _Font;
	int x,y;

	virtual HRESULT FrameMove(void);
	virtual HRESULT Render(void);
	virtual HRESULT OneTimeSceneInit(void);
	virtual HRESULT FinalCleanup(void);
	virtual HRESULT InitDeviceObjects(void);
	virtual HRESULT RestoreDeviceObjects(void);
	virtual HRESULT InvalidateDeviceObjects(void);
	virtual HRESULT DeleteDeviceObjects(void);

	/*friend void LoadScene();*/
};

//struct mydata
//{
//	MoonChildApp* theapp;
//	AppState state;
//};
#include "MoonScene.h"
DWORD MoonScene::_ID = 0;

MoonScene::MoonScene()
{
}

MoonScene::~MoonScene(void)
{
}

void MoonScene::ReadCollisionBox(const char* appname, const char* scnpath, MoonObject* pObj)
{
	char ch[256];
	char c[4];
	int nCollisionBox;
	string cb;
	D3DXVECTOR3 cbmin,cbmax,cbpos;
	GetPrivateProfileString(appname,"collisionbox","0",ch,256,scnpath);
	nCollisionBox = CharToInt(ch);
	for(int i=0; i<nCollisionBox; i++)
	{
		sprintf_s(c,"%03d",i);
		cb = "cb";
		cb+=c; cb+="min";
		GetPrivateProfileString(appname,cb.c_str(),NULL,ch,256,scnpath);
		cbmin = CharToVector3(ch);
		cb = "cb";
		cb+=c; cb+="max";
		GetPrivateProfileString(appname,cb.c_str(),NULL,ch,256,scnpath);
		cbmax = CharToVector3(ch);
		cb = "cb";
		cb+=c; cb+="pos";
		GetPrivateProfileString(appname,cb.c_str(),"(0,0,0)",ch,256,scnpath);
		cbpos = CharToVector3(ch);
		_CollisionTS->Add(pObj->GetID(),pObj,cbmin,cbmax,cbpos);
	}
	LogInfo("MoonScene::ReadCollisionBox() Create all the collisionboxes of a object.");
}

bool MoonScene::Initialize(D3DCAPS9* d3dCaps, const char* scenename)
{
	_device = NULL;
	_d3dCaps = d3dCaps;
	//检查场景配置文件是否存在
	string scnpath(SCENE_DIR(scenename));
	scnpath += ".scn";
	if(!PathFileExists(scnpath.c_str()))
	{
		LogError("MoonScene::Initialize() Cannot find scene configure file!");
		::MessageBox(0,"Cannot find scene configure file, please check bin/start.cfg.",0,0);
		return false;
	}

	char ch[256];
	string temp;
	char c[5];
	//读取场景配置信息
	GetPrivateProfileString("configure","bRenderCollisionBox","false",ch,256,scnpath.c_str());
	temp = ch;
	if(temp == "true")
		_CollisionTS = new MoonCollision(true);
	else
		_CollisionTS = new MoonCollision();

	//初始化摄像机
	_ThirdCamera = new MoonCamera(MoonCamera::RPG);
	_ThirdCamera->setPosition(&D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	_ThirdCamera->setLook(&D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	LogInfo("MoonScene::Initialize() Initialize CameraOnPlayer OK!");
	_FirstCamera = new MoonCamera(MoonCamera::LANDOBJECT);
	_FirstCamera->setPosition(&D3DXVECTOR3(0.0f, 150.0f, 0.0f));
	_FirstCamera->setLook(&D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	LogInfo("MoonScene::Initialize() Initialize FreeCamera OK!");
	_Camera = _ThirdCamera;

	//读取地形和天空的配置名称
	GetPrivateProfileString("environment","terrain",NULL,ch,256,scnpath.c_str());
	_terrainname = ch;
	GetPrivateProfileString("environment","skybox",NULL,ch,256,scnpath.c_str());
	_skyboxname = ch;

	//读取环境光照参数
	D3DXVECTOR3 v;
	GetPrivateProfileString("environmentlight","range",NULL,ch,256,scnpath.c_str());
	_environmentLight.Range=CharToFloat(ch);
	GetPrivateProfileString("environmentlight","diffuse",NULL,ch,256,scnpath.c_str());
	v = CharToVector3(ch);
	_environmentLight.Diffuse.r=v.x;
	_environmentLight.Diffuse.b=v.y;
	_environmentLight.Diffuse.g=v.z;
	GetPrivateProfileString("environmentlight","ambient",NULL,ch,256,scnpath.c_str());
	v = CharToVector3(ch);
	_environmentLight.Ambient.r=v.x;
	_environmentLight.Ambient.b=v.y;
	_environmentLight.Ambient.g=v.z;
	GetPrivateProfileString("environmentlight","specular",NULL,ch,256,scnpath.c_str());
	v = CharToVector3(ch);
	_environmentLight.Specular.r=v.x;
	_environmentLight.Specular.b=v.y;
	_environmentLight.Specular.g=v.z;
	GetPrivateProfileString("environmentlight","direction",NULL,ch,256,scnpath.c_str());
	v = CharToVector3(ch);
	D3DXVec3Normalize((D3DXVECTOR3*)&(_environmentLight.Direction),&v);
	_environmentLight.Type = D3DLIGHT_DIRECTIONAL;

	GetPrivateProfileString("ambient","ambient",NULL,ch,256,scnpath.c_str());
	_ambient = CharToVector3(ch);

	//读取雾化参数
	GetPrivateProfileString("fog","fog",NULL,ch,256,scnpath.c_str());
	temp = ch;
	if(temp == "true")
	{
		_dwFog = TRUE;
		GetPrivateProfileString("fog","fogcolor",NULL,ch,256,scnpath.c_str());
		v = CharToVector3(ch);
		_dwFogColor = D3DCOLOR_XRGB((int)v.x,(int)v.y,(int)v.z);
		GetPrivateProfileString("fog","fogtablemode",NULL,ch,256,scnpath.c_str());
		temp = ch;
		float f;
		if(temp == "linear")
		{
			_dwFogTableMode = D3DFOG_LINEAR;
			GetPrivateProfileString("fog","fogstart",NULL,ch,256,scnpath.c_str());
			f = CharToFloat(ch);
			_dwFogStart = *(DWORD*)&f;
			GetPrivateProfileString("fog","fogend",NULL,ch,256,scnpath.c_str());
			f = CharToFloat(ch);
			_dwFogEnd = *(DWORD*)&f;
		}
		else
		{
			if(temp == "exp")
				_dwFogTableMode = D3DFOG_EXP;
			else if(temp == "exp2")
				_dwFogTableMode = D3DFOG_EXP2;
			GetPrivateProfileString("fog","fogdensity",NULL,ch,256,scnpath.c_str());
			f = CharToFloat(ch);
			_dwFogDensity = *(DWORD*)&f;
		}
	}
	else
	{
		_dwFog = FALSE;
	}

	//读取角色相关参数
	string strPlayerName;
	D3DXVECTOR3 PlayerPos;
	D3DXVECTOR3 PlayerDir;
	D3DXVECTOR3 PlayerScl;
	GetPrivateProfileString("player","player",NULL,ch,256,scnpath.c_str());
	strPlayerName = ch;
	GetPrivateProfileString("player","position",NULL,ch,256,scnpath.c_str());
	PlayerPos = CharToVector3(ch);
	GetPrivateProfileString("player","direction",NULL,ch,256,scnpath.c_str());
	PlayerDir = CharToVector3(ch);
	GetPrivateProfileString("player","scale",NULL,ch,256,scnpath.c_str());
	PlayerScl = CharToVector3(ch);
	_thePlayer = new MoonCharacter(_d3dCaps,strPlayerName.c_str(), _ID++ ,PlayerPos,PlayerDir, PlayerScl);
	LogInfo("MoonScene::Initialize() Create MoonCharacter OK!");
	ReadCollisionBox("player",scnpath.c_str(),_thePlayer);

	//读取地面物体参数
	_blockcount = 0;
	_pBlock = NULL;
	GetPrivateProfileString("block","blockcount","0",ch,256,scnpath.c_str());
	_blockcount = CharToInt(ch);
	if(_blockcount>0)
		_pBlock = new MoonBlock[_blockcount];
	string str="block";
	string blockname;
	D3DXVECTOR3 blockpos,blockscl,blockrot;
	for(int i = 0; i < _blockcount; i++)
	{
		sprintf_s(c,"%04d",i);
		str+=c;
		GetPrivateProfileString(str.c_str(),"name",NULL,ch,256,scnpath.c_str());
		blockname = ch;
		GetPrivateProfileString(str.c_str(),"pos",NULL,ch,256,scnpath.c_str());
		blockpos = CharToVector3(ch);
		GetPrivateProfileString(str.c_str(),"scl",NULL,ch,256,scnpath.c_str());
		blockscl = CharToVector3(ch);
		GetPrivateProfileString(str.c_str(),"rot",NULL,ch,256,scnpath.c_str());
		blockrot = CharToVector3(ch);
		(_pBlock[i]).Initialize(_d3dCaps,blockname.c_str(), _ID++ ,blockpos,blockrot,blockscl);
		LogInfo("MoonScene::Initialize() Create new MoonBlock, name: %s.",blockname.c_str());
		ReadCollisionBox(str.c_str(),scnpath.c_str(),&_pBlock[i]);
		str="block";
	}

	//是否下雨
	GetPrivateProfileString("rain","brainy","false",ch,256,scnpath.c_str());
	temp = ch;
	if(temp=="true")
		_pRain = new MoonRain();
	else
		_pRain = NULL;

	D3DXVECTOR3 vec1,vec2;
	//是否下雪
	GetPrivateProfileString("snow","bsnow","false",ch,256,scnpath.c_str());
	temp = ch;
	if(temp=="true")
	{
		GetPrivateProfileString("snow","boundingmin","(0,0,0)",ch,256,scnpath.c_str());
		vec1 = CharToVector3(ch);
		GetPrivateProfileString("snow","boundingmax","(1,1,1)",ch,256,scnpath.c_str());
		vec2 = CharToVector3(ch);
		GetPrivateProfileString("snow","size","0.5",ch,256,scnpath.c_str());
		float fsize = CharToFloat(ch);
		GetPrivateProfileString("snow","texture","snow.tga",ch,256,scnpath.c_str());
		_SnowTex = ch;
		GetPrivateProfileString("snow","numparticles","100",ch,256,scnpath.c_str());
		int num = CharToInt(ch);
		_SnowSystem = new MoonSnow(&BoundingBox(&vec1, &vec2), fsize, num);
	}
	else
		_SnowSystem = NULL;

	//是否绘制焰火
	GetPrivateProfileString("firework","bfirework","false",ch,256,scnpath.c_str());
	temp = ch;
	if(temp=="true")
	{
		GetPrivateProfileString("firework","position","(0,0,0)",ch,256,scnpath.c_str());
		vec1 = CharToVector3(ch);
		GetPrivateProfileString("firework","texture","fire.tga",ch,256,scnpath.c_str());
		_FireTex = ch;
		GetPrivateProfileString("firework","numparticles","100",ch,256,scnpath.c_str());
		int num = CharToInt(ch);
		_Firework = new MoonFirework(&vec1,num);
	}
	else
		_Firework = NULL;

	// 读取水体配置信息
	GetPrivateProfileString("water","water","false",ch,256,scnpath.c_str());
	temp = ch;
	if(temp == "true")
		_pWater = new MoonWater();
	else
		_pWater = NULL;
	if(_pWater)
	{
		GetPrivateProfileString("water","height","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","flx","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","flz","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","brx","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","brz","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","waveheight","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","flowspeedu","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
		GetPrivateProfileString("water","flowspeedv","0",ch,256,scnpath.c_str());
		_WaterArg.push_back(CharToFloat(ch));
	}

	_SoundManager = new MoonSoundManager();
	if(_SoundManager->Initialize())
		LogInfo("MoonScene::Initialize() SoundManager initialized OK!");

	return true;
}

HRESULT MoonScene::InitDeviceObjects(IDirect3DDevice9* device)
{
	_device = device;
	_CollisionTS->InitDevice(device);
	HRESULT hr;
	_theTerrain = new MoonTerrain();
	if(_theTerrain->initialize(_device, _terrainname.c_str(), _environmentLight.Direction) == false)
	{
		LogWarning("MoonScene::InitDeviceObjects() Initialize the MoonTerrain failed!");
		SAFE_DELETE(_theTerrain);
	}

	_theSkybox = new MoonSky(_device, _skyboxname.c_str(),
		3000.0f, 3000.0f, 300.0f);
	if(_theSkybox->initialize() == false)
	{
		LogWarning("MoonScene::InitDeviceObjects() Initialize the MoonSky failed!");
		SAFE_DELETE(_theSkybox);
	}

	_environmentLight.Range = (float)_theTerrain->getwidth();

	hr = _thePlayer->InitDeviceObjects(_device);
	for(int i = 0; i < _blockcount; i++)
	{
		if(FAILED(hr = _pBlock[i].InitDeviceObjects(_device)))
		{
			_CollisionTS->Delete(_pBlock[i].GetID());
			LogInfo("MoonScene::InitDeviceObjects() Block%04d initializing Failed!");
		}
	}

	if(_pRain && _pRain->Initialize(_device) == false)
	{
		LogWarning("MoonScene::InitDeviceObjects() Initialize the MoonRain failed!");
		SAFE_DELETE(_pRain);
	}

	if(_pWater && !_pWater->Initialize(_device, _WaterArg[0], _WaterArg[1],_WaterArg[2],
		_WaterArg[3],_WaterArg[4],_WaterArg[5],_WaterArg[6],_WaterArg[7]))
	{
		LogWarning("MoonScene::InitDeviceObjects() Initialize the MoonWater failed!");
		SAFE_DELETE(_pWater);
	}

	return hr;
}

HRESULT MoonScene::RestoreDeviceObjects()
{
	HRESULT hr = E_FAIL;

	if(_SnowSystem && _SnowSystem->init(_device,const_cast<char*>(IMAGE_DIR(_SnowTex.c_str()))) == false)
	{
		LogWarning("MoonScene::RestoreDeviceObjects() Initialize the MoonSnow failed!");
		SAFE_DELETE(_SnowSystem);
	}
	if(_Firework && _Firework->init(_device,const_cast<char*>(IMAGE_DIR(_FireTex.c_str()))) == false)
	{
		LogWarning("MoonScene::RestoreDeviceObjects() Initialize the MoonFirework failed!");
		SAFE_DELETE(_Firework);
	}

    // Material
    D3DMATERIAL9 Material;
    Moon_InitMaterial( Material, 1.0f, 1.0f, 1.0f, 1.0f );
    hr = _device->SetMaterial( &Material );
	if(SUCCEEDED(hr))
		LogInfo("MoonScene::RestoreDeviceObjects() SetMaterial OK!");
	else
		LogError("MoonScene::RestoreDeviceObjects() SetMaterial Failed!");

	if(FAILED(hr = _device->SetLight(0, &_environmentLight)))
		return hr;
	if(FAILED(hr = _device->LightEnable(0, true)))
		return hr;

	if(FAILED(hr = _device->SetRenderState(D3DRS_AMBIENT,
			D3DCOLOR_XRGB((int)_ambient.x, (int)_ambient.y, (int)_ambient.z))))
		return hr;

	//设置雾化效果
	if(_dwFog == TRUE)
	{
		_device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		_device->SetRenderState(D3DRS_FOGCOLOR, _dwFogColor);
		switch(_dwFogTableMode)
		{
			case D3DFOG_LINEAR:
				_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
				_device->SetRenderState(D3DRS_FOGSTART, _dwFogStart);
				_device->SetRenderState(D3DRS_FOGEND, _dwFogEnd);
				break;
			case D3DFOG_EXP:
				_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
				_device->SetRenderState(D3DRS_FOGDENSITY, _dwFogDensity);
				break;
			case D3DFOG_EXP2:
				_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
				_device->SetRenderState(D3DRS_FOGDENSITY, _dwFogDensity);
				break;
		}
		D3DCAPS9 d3dCaps;
		_device->GetDeviceCaps(&d3dCaps);
		if(d3dCaps.RasterCaps&D3DPRASTERCAPS_FOGRANGE)
			_device->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	}

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//允许使用alpha混合
	_device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//Turn on lighting
    _device->SetRenderState(D3DRS_LIGHTING, TRUE);
    _device->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    _device->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
	_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	_device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	_device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	if(FAILED(_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE)))
		LogError("MoonScene::RestoreDeviceObjects() SetRenderState(AntiAlias) Failed!");

	_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	_device->SetSamplerState( 6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	return hr;
}

HRESULT MoonScene::InvalidateDeviceObjects()
{
	if(_SnowSystem)
		_SnowSystem->reset();
	if(_Firework)
		_Firework->reset();
	//---------将雾化效果相关参数保存起来---------------------------
	_device->GetRenderState(D3DRS_FOGCOLOR, &_dwFogColor);
	_device->GetRenderState(D3DRS_FOGTABLEMODE, &_dwFogTableMode);
	_device->GetRenderState(D3DRS_FOGSTART, &_dwFogStart);
	_device->GetRenderState(D3DRS_FOGEND, &_dwFogEnd);
	_device->GetRenderState(D3DRS_FOGDENSITY, &_dwFogDensity);
	//-----------------------------------------------------------

	return S_OK;
}

HRESULT MoonScene::DeleteDeviceObjects()
{
	HRESULT hr;

	SAFE_DELETE(_theTerrain);
	SAFE_DELETE(_theSkybox);

	if(_thePlayer)
		hr = _thePlayer->DeleteDeviceObjects();
	for(int i = 0; i < _blockcount; i++)
	{
		hr = _pBlock[i].DeleteDeviceObjects();
	}
	_device = NULL;
	return hr;
}

void MoonScene::Cleanup()
{
	SAFE_DELETE(_SoundManager);
	SAFE_DELETE(_CollisionTS);
	SAFE_DELETE_ARRAY(_pBlock);
	SAFE_DELETE(_thePlayer);
	SAFE_DELETE(_pRain);
	SAFE_DELETE(_SnowSystem);
	SAFE_DELETE(_Firework);
	SAFE_DELETE(_pWater);
	SAFE_DELETE(_ThirdCamera);
	SAFE_DELETE(_FirstCamera);
}

MoonTerrain* MoonScene::getTerrain()
{
	return _theTerrain;
}

MoonCharacter* MoonScene::getPlayer()
{
	return _thePlayer;
}

void MoonScene::ClipPosition(D3DXVECTOR3* pos)
{
	if((*pos).x > _theTerrain->getwidth()/2.0f-81)
		(*pos).x = _theTerrain->getwidth()/2.0f-81;
	else if((*pos).x < -_theTerrain->getwidth()/2.0f+81)
		(*pos).x = -_theTerrain->getwidth()/2.0f+81;

	if((*pos).z > _theTerrain->getdepth()/2.0f-81)
		(*pos).z = _theTerrain->getdepth()/2.0f-81;
	else if((*pos).z < -_theTerrain->getdepth()/2.0f+81)
		(*pos).z = -_theTerrain->getdepth()/2.0f+81;
}

void MoonScene::FrameMove(float fElapsedTime, float fTime)
{
	_SoundManager->SwapBackgroundMusic();

	static bool bHanging = false;
	float fHeight;
	static float sfTime = 0.0;
	D3DXVECTOR3 pos,posCamera;
	D3DXMATRIX matView;
	pos = _thePlayer->GetPos();
	ClipPosition(&pos);
	fHeight = _theTerrain->getHeight(pos.x,pos.z) + 0.5f;
	WORD value = _CollisionTS->GetCollisionType(_thePlayer->GetID());
	if((value & 0x0001)!=0x0001)
	{
		if(pos.y > fHeight+0.5f)
		{
			if(!bHanging)
				bHanging = true;
			else
			{
				sfTime += fElapsedTime;
				pos.y -= 40.8f * sfTime * sfTime;
				if(pos.y < fHeight)
					pos.y = fHeight;
				_thePlayer->setPos(&pos);
				/*pos = _thePlayer->GetPos();*/
			}
		}
		/*else if (pos.y < fHeight -0.5f)
		{
			bHanging = false;
			sfTime = 0.0f;
			_thePlayer->setBack();
		}*/
		else
		{
			bHanging = false;
			sfTime = 0.0f;
			D3DXVECTOR3 posi = _thePlayer->GetPosIncre();
			if(sqrt(posi.x*posi.x+posi.z*posi.z)>1.732f*(fHeight-pos.y))
			{
				pos.y = fHeight;
				_thePlayer->setPos(&pos);
			}
			else
				_thePlayer->setBack();
		}
	}
	else
	{
		bHanging = false;
		sfTime = 0.0f;
	}

	_CollisionTS->Test();

	pos = _thePlayer->GetPos();
	if(_Camera==_FirstCamera)
	{
		D3DXVECTOR3 look;
		pos.y+=12.0f;
		_Camera->getLook(&look);
		look.y = 0.0f;
		D3DXVec3Normalize(&look,&look);
		pos += look*3;
		_Camera->setPosition(&pos);
	}
	else
		pos.y +=10.0f;
		_Camera->setLookAtPos(&pos);
	/*--------使摄像机的位置不会没入在地形下造成视觉错误-----------------*/
	_Camera->getPosition(&posCamera);
	float height = _theTerrain->getHeight(posCamera.x, posCamera.z);
	if(posCamera.y < height+2.0f)
	{
		posCamera.y = height+2.0f;
		D3DXMatrixLookAtLH(&matView, &posCamera, &pos, &D3DXVECTOR3(0,1,0));
	}
	else
		_Camera->getViewMatrix(&matView);
	/*-------------------------------------------------------------*/
	_device->SetTransform(D3DTS_VIEW, &matView);

	if(_theSkybox)
		_theSkybox->FrameMove(posCamera.x, posCamera.y, posCamera.z,fElapsedTime);
	if(_thePlayer)
		_thePlayer->FrameMove(fElapsedTime);
	for(int i = 0; i < _blockcount; i++)
	{
		_pBlock[i].FrameMove(fElapsedTime);
	}
	if(_pWater)
		_pWater->FrameMove(fElapsedTime, fTime, &posCamera);
	if(_pRain)
		_pRain->FrameMove(posCamera.x, 0, posCamera.z,fElapsedTime);
	if(_SnowSystem)
		_SnowSystem->update(fElapsedTime);
	if(_Firework)
		_Firework->update(fElapsedTime);
}

void MoonScene::Draw(void)
{
	if(_theSkybox)
		_theSkybox->draw();
	if(_theTerrain)
		_theTerrain->draw();
	if(_thePlayer)
		_thePlayer->Draw();
	for(int i = 0; i < _blockcount; i++)
	{
		_pBlock[i].Draw();
	}
	if(_pWater)
		_pWater->Render();
	if(_pWater && _pWater->InWater())
		return;
	if(_pRain)
		_pRain->Render();
	if(_SnowSystem)
		_SnowSystem->render();
	if(_Firework)
		_Firework->render();
	_CollisionTS->Draw();
}

DWORD MoonScene::GetKeyboardInput(const char* KeyboardState, float fElapsedTime)
{
	static bool bCheck1 = true;
	static bool bCheck2 = true;
	if(KEYDOWN(KeyboardState,DIK_ESCAPE))
		return ID_EXIT;
	if(KEYDOWN(KeyboardState,DIK_LSHIFT))
	{
		if(bCheck1)
		{
			_thePlayer->ToggleRunning();
			bCheck1 = false;
		}
	}
	else
		bCheck1 = true;
	if(KEYDOWN(KeyboardState,DIK_TAB))
	{
		if(bCheck2)
		{
			if(_Camera==_ThirdCamera)
				_Camera=_FirstCamera;
			else
				_Camera=_ThirdCamera;
			bCheck2 = false;
		}
	}
	else
		bCheck2 = true;

	float unit = fElapsedTime * 15 ;

	bool bIdle = true;
	UINT Direction = 0;
	if(KEYDOWN(KeyboardState, DIK_1))
	{
		_thePlayer->Skill1();
		bIdle = false;
	}
	else if(KEYDOWN(KeyboardState, DIK_2))
	{
		_thePlayer->Skill2();
		bIdle = false;
	}
	else if(KEYDOWN(KeyboardState, DIK_3))
	{
		_thePlayer->Skill3();
		bIdle = false;
	}
	else
	{
		if(KEYDOWN(KeyboardState, DIK_W))
			Direction |= 1;
		if(KEYDOWN(KeyboardState, DIK_S))
			Direction |= 2;
		if(KEYDOWN(KeyboardState, DIK_A))
			Direction |= 4;
		if(KEYDOWN(KeyboardState, DIK_D))
			Direction |= 8;

		D3DXVECTOR3 frontVector;
		_Camera->getLook(&frontVector);
		switch(Direction)
		{
		case 1:
			_thePlayer->TurnFront(frontVector,unit);
			bIdle = false;
			break;
		case 2:
			_thePlayer->TurnBack(frontVector,unit);
			bIdle = false;
			break;
		case 4:
			_thePlayer->TurnLeft(frontVector,unit);
			bIdle = false;
			break;
		case 8:
			_thePlayer->TurnRight(frontVector,unit);
			bIdle = false;
			break;
		case 5:
			_thePlayer->TurnLF(frontVector,unit);
			bIdle = false;
			break;
		case 9:
			_thePlayer->TurnRF(frontVector,unit);
			bIdle = false;
			break;
		case 6:
			_thePlayer->TurnLB(frontVector,unit);
			bIdle = false;
			break;
		case 10:
			_thePlayer->TurnRB(frontVector,unit);
			bIdle = false;
			break;
		}
	}
	if(bIdle)
		_thePlayer->Idle();

	return NULL;
}

void MoonScene::GetMouseInput(DIMOUSESTATE* MouseState, float fElapsedTime)
{
	// if the rightbutton of mouse is down
	if(MOUSEBUTTONDOWN(MouseState->rgbButtons[MOUSEBUTTON_RIGHT]))
	{
		MoonShowCursor(false);

		int tempx = MouseState->lX%2000;
		int tempy = MouseState->lY%2000;
		D3DXVECTOR3 pos, up, look;

		_Camera->yaw(tempx/200.0f);

		// 将上下旋转的范围控制在一定程度，当摄像机从头顶上往下看时，此时再接收正向输入（即tempy>0），摄像机就会从头顶上这一边
		// 滑到另一边。而当摄像机从脚底往上看时，再接收负向输入，摄像机就会从脚底这一边滑到另一边，因此这两情况都要阻止发生。此外
		// 还不能让摄像机没入地形下。
		_Camera->getPosition(&pos);
		_Camera->getUp(&up);
		_Camera->getLook(&look);
		if(((pos.y-2.0f) <= _theTerrain->getHeight(pos.x, pos.z))
			&& (tempy <= 0))
			;//在摄像机快要没入地形下时不再接收tempy的负向输入
		else if((up.y <= 0.02f) && (look.y > 0.0f) && (tempy <= 0))
			;//在摄像机快要完全朝上时不再接收tempy的负向输入
		else if((up.y <= 0.02f) && (look.y < 0.0f) && (tempy >= 0))
			;//在摄像机快要完全朝下时不再接收tempy的正向输入
		else
			_Camera->pitch(tempy/200.0f);
	}
	else
	{
		MoonShowCursor(true);
	}
	if(_Camera->getType() == MoonCamera::RPG && MouseState->lZ != 0)
	{
		// 要将视距的远近控制在一定范围内
		float distance = _Camera->getDistance();
		if(distance >= 6.0f && distance <= 80.0f)
			_Camera->adjustDistance(MouseState->lZ/30.0f);

		distance = _Camera->getDistance();
		if(distance > 80.0f)
			_Camera->setDistance(80.0f);
		else if(distance < 6.0f)
			_Camera->setDistance(6.0f);
	}
}
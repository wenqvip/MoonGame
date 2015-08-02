#pragma once
#include "MoonSystemLog.h"
#include "MoonUtility.h"
#include "MoonTerrain.h"
#include "MoonSky.h"
#include "MoonCharacter.h"
#include "MoonBlock.h"
#include "MoonCamera.h"
#include "MoonRain.h"
#include "MoonWater.h"
#include "MoonCollision.h"
#include "MoonSoundManager.h"
#include "MoonSnow.h"
#include "MoonFirework.h"

class MoonScene : public MoonSystemLog
{
	IDirect3DDevice9*	_device;
	D3DCAPS9*			_d3dCaps;
	MoonTerrain*		_theTerrain;
	MoonSky*			_theSkybox;
	MoonCharacter*		_thePlayer;
	MoonBlock*			_pBlock;
	MoonRain*			_pRain;
	MoonWater*			_pWater;
	vector<float>		_WaterArg;// 水面效果参数
	MoonCollision*		_CollisionTS;
	MoonSoundManager*	_SoundManager;
	MoonCamera*			_Camera;
	MoonCamera*			_ThirdCamera;
	MoonCamera*			_FirstCamera;
	MoonSnow*			_SnowSystem;
	MoonFirework*		_Firework;

	string				_FireTex;
	string				_SnowTex;

	//------环境参数-----------------------
	int					_blockcount;
	D3DLIGHT9			_environmentLight;
	string				_terrainname;
	string				_skyboxname;
	D3DXVECTOR3			_ambient;
	//------------------------------------
	//------雾化效果相关参数----------------
	DWORD				_dwFog;
	DWORD				_dwFogColor;
	DWORD				_dwFogTableMode;
	DWORD				_dwFogStart;
	DWORD				_dwFogEnd;
	DWORD				_dwFogDensity;
	//------------------------------------
	static DWORD _ID;
	void ClipPosition(D3DXVECTOR3* pos);
public:
	MoonScene();
	~MoonScene(void);
	void ReadCollisionBox(const char* appname, const char* scnpath, MoonObject* pObj);
	bool Initialize( D3DCAPS9* d3dCaps, const char* scenename);
	HRESULT InitDeviceObjects(IDirect3DDevice9* device);
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	void Cleanup();
	MoonTerrain* getTerrain();
	MoonCharacter* getPlayer();
	void FrameMove(float fElapsedTime, float fTime);
	void Draw(void);
	DWORD GetKeyboardInput(const char* KeyboardState, float fElapsedTime);
	void GetMouseInput(DIMOUSESTATE* MouseState, float fElapsedTime);
};

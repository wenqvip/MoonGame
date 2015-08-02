#pragma once
#include "MoonUtility.h"

class MoonWater
{
	LPDIRECT3DDEVICE9 _device;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	LPDIRECT3DINDEXBUFFER9 _ib;
	LPDIRECT3DTEXTURE9 _tex;
	struct WATERVERTEX
	{
		float x, y, z;
		float n1, n2, n3;
		float u, v;
		static const DWORD FVF;
	};
	float _fHeight;
	/*float _fRealHeight;*///因为水面会有波动，水面时高时低，因而_fHeight的值就不再准确，所以用这个值来表示准确的水面高度
	float _flx,_flz;
	float _brx,_brz;
	float _height[4];
	float _waveheight;
	float _flowspeedu;
	float _flowspeedv;

	bool _bInWater;
	bool _bChange;
	struct Fog
	{
		DWORD fogenable;
		DWORD fogmode;
		DWORD fogstart;
		DWORD fogend;
		DWORD fogdensity;
		DWORD fogcolor;
	};
	Fog _OrinFog;
	bool InWaterChecking(D3DXVECTOR3* pos);
public:
	MoonWater(void);
	~MoonWater(void);
	bool Initialize(LPDIRECT3DDEVICE9 device, float height, float flx, float flz, 
		float brx, float brz, float waveheight, float flowspeedu, float flowspeedv);
	void FrameMove(float fElapsedTime, float fTime, D3DXVECTOR3* pos);
	void Render();
	bool InWater(){return _bInWater;};
};

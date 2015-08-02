#pragma once
#include "MoonUtility.h"
#define PARTICLENUM 2000

class MoonRain
{
	struct RAINVERTEX
	{
		D3DXVECTOR3 vPos;
		DWORD dwColor;
		static const DWORD FVF;
	};
	struct RainParticle
	{
		float x, y, z;
		float fDspeed;
	};
	RainParticle _rain[PARTICLENUM];
	LPDIRECT3DDEVICE9 _device;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	D3DXVECTOR3 _RainDir;
	D3DXMATRIX _matWorld;
public:
	MoonRain(void);
	~MoonRain(void);
	bool Initialize(LPDIRECT3DDEVICE9 device);
	void FrameMove(float x, float y, float z, float ftime);
	void Render();
};

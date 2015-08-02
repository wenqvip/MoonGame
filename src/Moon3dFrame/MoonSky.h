#pragma once
#include "MoonUtility.h"

class MoonSky
{
	struct SkyboxVertex
	{
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};

	IDirect3DDevice9* _device;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DTexture9* _text[6];
	IDirect3DVertexBuffer9* _vbCloud;
	IDirect3DTexture9* _texCloud;
	float _cloudSpeedU,_cloudSpeedV;
	D3DXMATRIX _matWorld;
	float _width, _depth, _height;
	string _east;
	string _south;
	string _west;
	string _north;
	string _sky;
	string _earth;
	string _cloud;
public:
	MoonSky(IDirect3DDevice9* device, const char* str,
		float width, float depth, float height);
	~MoonSky(void);
	bool initialize(void);
	void draw(void);
	void FrameMove(float x, float y, float z, float fElapsedTime);
};

#pragma once
#include "MoonUtility.h"

class MoonAABBbox
{
	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;
	D3DXVECTOR3 _pos;
	LPDIRECT3DDEVICE9 _device;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	LPDIRECT3DINDEXBUFFER9 _ib;
	bool _bDrawBox;

	struct AABBVertex
	{
		float x, y, z;
		DWORD color;
		static const DWORD FVF;
	};
public:
	MoonAABBbox(D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 pos, bool bDraw = false);
	~MoonAABBbox(void);
	void InitVertexBuffer(LPDIRECT3DDEVICE9 device);
	D3DXVECTOR3 getMin();
	D3DXVECTOR3 getMax();
	void Update(D3DXVECTOR3 pos);
	void draw();
};

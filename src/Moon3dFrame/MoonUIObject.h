#pragma once
#include "MoonUtility.h"
#include "MoonFont.h"

struct UIVertex
{
	float x, y, z, rhw;
	float u, v;
	static const DWORD FVF;
};

class MoonUIObject
{
protected:
	DWORD _ID;
public:
	MoonUIObject(void);
	virtual ~MoonUIObject(void);

	virtual bool Create(DWORD ID, LPDIRECT3DDEVICE9 device,float width,float height,D3DXVECTOR3 pos,
				const char* str,MoonFont* pen) { return true; };
	virtual void Destroy() {};
	virtual DWORD OnClick(float x, float y, bool bDown) { return 0; };
	virtual void Paint() {};
};

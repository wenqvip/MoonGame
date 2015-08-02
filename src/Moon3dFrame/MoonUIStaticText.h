#pragma once
#include "MoonUIObject.h"

class MoonUIStaticText : public MoonUIObject
{
	MoonFont* _pen;
	D3DXVECTOR3 _pos;
	string _str;
	float _fWidth;
	float _fHeight;
	D3DXVECTOR3 _vPos;

public:
	MoonUIStaticText();
	~MoonUIStaticText(void);
	virtual bool Create(DWORD ID, LPDIRECT3DDEVICE9 device,float width,float height,D3DXVECTOR3 pos,
				const char* str,MoonFont* pen);
	virtual void Paint();
};

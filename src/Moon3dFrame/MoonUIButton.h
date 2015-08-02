#pragma once
#include "MoonUIObject.h"

class MoonUIButton : public MoonUIObject
{
	LPDIRECT3DDEVICE9 _device;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	/*LPDIRECT3DINDEXBUFFER9 _ib;*/
	LPDIRECT3DTEXTURE9 _tex, _texNormal, _texOver, _texDown;
	float _fWidth;
	float _fHeight;
	D3DXVECTOR3 _vPos;
	string _strCaption;
	MoonFont* _pen;

public:
	MoonUIButton(void);
	~MoonUIButton(void);
	virtual bool Create(DWORD ID, LPDIRECT3DDEVICE9 device,float width,float height,D3DXVECTOR3 pos,
				const char* str,MoonFont* pen);
	virtual void Destroy();
	virtual DWORD OnClick(float x, float y, bool bDown);
	virtual void Paint();
};

#pragma once
#include "MoonUIObject.h"
#include "MoonUIStaticText.h"
#include "MoonUIButton.h"
#include "MoonFont.h"

class MoonUIPanel
{
	LPDIRECT3DDEVICE9 _device;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	LPDIRECT3DTEXTURE9 _tex;
	bool _bFullWindow;
	float _fWidth;
	float _fHeight;
	D3DXVECTOR3 _vPos;
	MoonFont* _pen;
	bool bFocus;
	int _nCount;
	vector<MoonUIObject*> _UIObj;
public:
	MoonUIPanel(void);
	~MoonUIPanel(void);
	bool Create(LPDIRECT3DDEVICE9 device, const char* panelname, float Width, float Height, MoonFont* pen);
	void Destroy();
	DWORD Test(int x, int y, bool bDown);
	void Draw();
};

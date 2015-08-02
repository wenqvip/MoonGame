#pragma once
#include "MoonUtility.h"

class Moon3DFont
{
	LPDIRECT3DDEVICE9 _pDevice;
	LPD3DXMESH _pMesh;

public:
	Moon3DFont(LPDIRECT3DDEVICE9 device, LPSTR pFontFace, LPSTR str,
		float Extrusion, UINT uWeight = 500, bool bItalic = false,
		bool bUnderline = false, bool bStrikeOut = false);
	~Moon3DFont(void);

	void DrawTextx(D3DXVECTOR3 pos, D3DXCOLOR color, float scale = 10,
			float angle = 0.0f, D3DXVECTOR3 axis = D3DXVECTOR3(0.0f,1.0f,0.0f));
};

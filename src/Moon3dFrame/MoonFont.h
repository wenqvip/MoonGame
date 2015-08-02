#pragma once
#include<d3dx9.h>

class MoonFont
{
public:
	MoonFont(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFontFace, int nHeight,
		bool fBold, bool fItalic, bool fUnderlined);
	~MoonFont(void);
	void DrawTextx(LPSTR pText, int x, int y, D3DCOLOR rgbFontColour, DWORD format=DT_LEFT);
	void DrawTextx(LPSTR pText, RECT rect, D3DCOLOR rgbFontColour, DWORD format=DT_LEFT);

private:
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPD3DXFONT m_pFont;
};

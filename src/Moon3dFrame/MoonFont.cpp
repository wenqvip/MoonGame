#include "MoonFont.h"

MoonFont::MoonFont(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFontFace, int nHeight,
		bool fBold, bool fItalic, bool fUnderlined)
{
	m_pD3DDevice = pD3DDevice;
	
	int nWeight = FW_NORMAL;

	if(fBold)
		nWeight = FW_BOLD;
	
	D3DXCreateFont(m_pD3DDevice, nHeight, nHeight / 2, nWeight, 0, fItalic, DEFAULT_CHARSET, 0, 0, 0, "Arial", &m_pFont);
}

MoonFont::~MoonFont(void)
{
	m_pFont->Release();
	m_pFont=NULL;
}

void MoonFont::DrawTextx(LPSTR pText, int x, int y, D3DCOLOR rgbFontColour, DWORD format)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = 0;
	rect.bottom = 0;
	//Calculate the size of the rect needed
	m_pFont->DrawText(NULL, pText, -1, &rect, DT_CALCRECT, 0);
	//Draw the text
	m_pFont->DrawText(NULL, pText, -1, &rect, format, rgbFontColour);
}

void MoonFont::DrawTextx(LPSTR pText, RECT rect, D3DCOLOR rgbFontColour, DWORD format)
{
	//Calculate the size of the rect needed
	m_pFont->DrawText(NULL, pText, -1, &rect, DT_CALCRECT | format, 0);
	//Draw the text
	m_pFont->DrawText(NULL, pText, -1, &rect, format, rgbFontColour);
}
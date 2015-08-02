#include "MoonUIStaticText.h"

MoonUIStaticText::MoonUIStaticText()
{
}

MoonUIStaticText::~MoonUIStaticText(void)
{
}

bool MoonUIStaticText::Create(DWORD ID, LPDIRECT3DDEVICE9 device, float width, float height, D3DXVECTOR3 pos,
							  const char *str, MoonFont *pen)
{
	_ID = ID;
	_fWidth = width;
	_fHeight = height;
	_pen = pen;
	_pos = pos;
	_str = str;
	return true;
}

void MoonUIStaticText::Paint()
{
	RECT rect;
	rect.left=_pos.x;
	rect.top=_pos.y;
	rect.right=_pos.x+_fWidth;
	rect.bottom=_pos.y+_fHeight;
	_pen->DrawTextx(const_cast<char*>(_str.c_str()), rect, 0xff000000,DT_WORDBREAK);
}
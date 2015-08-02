#include "Moon3DFont.h"

Moon3DFont::Moon3DFont(LPDIRECT3DDEVICE9 pDevice, LPSTR pFontFace, LPSTR str,
		float fExtrusion, UINT uWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
{
	LOGFONT lf;
	HDC hdc;
	HFONT hFont, hFontOld;

	hdc = CreateCompatibleDC(0);
	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight			= 25;
	lf.lfWeight			= 12;
	lf.lfEscapement		= 0;
	lf.lfOrientation	= 0;
	lf.lfWeight			= uWeight;
	lf.lfItalic			= bItalic;
	lf.lfUnderline		= bUnderline;
	lf.lfStrikeOut		= bStrikeOut;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= 0;
	lf.lfClipPrecision	= 0;
	lf.lfQuality		= CLEARTYPE_QUALITY;
	lf.lfPitchAndFamily	= 0;
	strcpy_s(lf.lfFaceName, pFontFace);

	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);

	_pDevice = pDevice;
	_pMesh = NULL;

	D3DXCreateText(_pDevice, hdc, str, 0.001f, fExtrusion, &_pMesh, 0, 0);

	SelectObject(hdc, hFontOld);
	DeleteObject( hFont );
	DeleteDC(hdc);
}

Moon3DFont::~Moon3DFont(void)
{
	SAFE_RELEASE(_pMesh);
}

void Moon3DFont::DrawTextx(D3DXVECTOR3 pos, D3DXCOLOR color, float scale,
			float angle, D3DXVECTOR3 axis)
{
	D3DXMATRIX matworld, mattrans, matrot, matscale, matold;
	_pDevice->GetTransform(D3DTS_WORLD, &matold);

	/*D3DXMatrixIdentity(&matworld);
	D3DXMatrixMultiply(&matworld, &matworld, &matold);*/

	D3DXMatrixScaling(&matscale, scale, scale, 1);
	D3DXMatrixRotationAxis(&matrot, &axis, angle);
	D3DXMatrixTranslation(&mattrans, pos.x, pos.y, pos.z);

	/*D3DXMatrixMultiply(&matworld, &matworld, &matscale);*/
	D3DXMatrixMultiply(&matworld, &matscale, &matrot);
	D3DXMatrixMultiply(&matworld, &matworld, &mattrans);

	_pDevice->SetTransform(D3DTS_WORLD, &matworld);

	D3DMATERIAL9 mtrl, mtrlold;
	_pDevice->GetMaterial(&mtrlold);
	_pDevice->SetTexture(0, NULL);
	Moon_InitMaterial(mtrl, color.r, color.g, color.b);
	_pDevice->SetMaterial(&mtrl);

	_pMesh->DrawSubset(0);

	_pDevice->SetMaterial(&mtrlold);
	_pDevice->SetTransform(D3DTS_WORLD, &matold);
}
#include "MoonUIButton.h"

MoonUIButton::MoonUIButton(void)
{
	_device = NULL;
	_vb = NULL;
	/*_ib = NULL;*/
	_texNormal = NULL;
	_texOver = NULL;
	_texDown = NULL;
}

MoonUIButton::~MoonUIButton(void)
{
}

bool MoonUIButton::Create(DWORD ID, LPDIRECT3DDEVICE9 device, float width, float height, D3DXVECTOR3 pos, 
						  const char* str, MoonFont* pen)
{
	_ID = ID;
	_device = device;
	_fWidth = width;
	_fHeight = height;
	_vPos = pos;
	_strCaption = str;
	_pen = pen;
	V_RETURN(_device->CreateVertexBuffer(4*sizeof(UIVertex),D3DUSAGE_WRITEONLY,UIVertex::FVF,
		D3DPOOL_MANAGED, &_vb,0));
	UIVertex Vertices[]=
	{
		{_vPos.x,			_vPos.y,			_vPos.z, 1.0f, 0.0f, 0.0f,},
		{_vPos.x+_fWidth,	_vPos.y,			_vPos.z, 1.0f, 1.0f, 0.0f,},
		{_vPos.x,			_vPos.y+_fHeight,	_vPos.z, 1.0f, 0.0f, 1.0f,},
		{_vPos.x+_fWidth,	_vPos.y+_fHeight,	_vPos.z, 1.0f, 1.0f, 1.0f,},
	};
	void* pVertices;
	V_RETURN(_vb->Lock(0,sizeof(Vertices),&pVertices,0));
	memcpy(pVertices,Vertices,sizeof(Vertices));
	V_RETURN(_vb->Unlock());

	Moon_CreateTexture(_device, const_cast<TCHAR*>(GUI_DIR("button.tga")), &_texNormal);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(GUI_DIR("button_over.tga")), &_texOver);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(GUI_DIR("button_down.tga")), &_texDown);
	_tex = _texNormal;
	
	return true;
}

void MoonUIButton::Destroy()
{
	SAFE_RELEASE(_vb);
	SAFE_RELEASE(_texNormal);
	SAFE_RELEASE(_texOver);
	SAFE_RELEASE(_texDown);
}

DWORD MoonUIButton::OnClick(float x, float y, bool bDown)
{
	if(x>_vPos.x&&x<(_vPos.x+_fWidth)&&y>_vPos.y&&y<(_vPos.y+_fHeight))
	{
		if(_tex==_texDown&&!bDown)
			return _ID;

		if(bDown)
			_tex = _texDown;
		else
			_tex = _texOver;
	}
	else
	{
		_tex = _texNormal;
	}
	return 0;
}

void MoonUIButton::Paint()
{
	_device->SetTexture(0,_tex);
	_device->SetStreamSource(0, _vb, 0, sizeof(UIVertex));
	_device->SetFVF(UIVertex::FVF);
	_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	RECT rect;
	rect.left=_vPos.x;
	rect.top=_vPos.y;
	rect.right=_vPos.x+_fWidth;
	rect.bottom=_vPos.y+_fHeight;
	_pen->DrawTextx(const_cast<char*>(_strCaption.c_str()), rect, 0xff000000, DT_CENTER|DT_VCENTER);
}
#include "MoonUIPanel.h"

MoonUIPanel::MoonUIPanel(void)
{
}

MoonUIPanel::~MoonUIPanel(void)
{
}

bool MoonUIPanel::Create(LPDIRECT3DDEVICE9 device, const char *panelname, float Width, float Height,
						 MoonFont* pen)
{
	_device = device;
	_pen = pen;
	string panelpath;
	panelpath=GUI_DIR(panelname);
	char ch[260];
	string temp;
	GetPrivateProfileString("panel","bfullwindow","true",ch,260,panelpath.c_str());
	temp = ch;
	if(temp == "true")
	{
		_bFullWindow = true;
		_fWidth = Width;
		_fHeight = Height;
		_vPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	else
	{
		_bFullWindow = false;
		GetPrivateProfileString("panel","width","0.2",ch,260,panelpath.c_str());
		_fWidth = Width * CharToFloat(ch);
		GetPrivateProfileString("panel","height","0.2",ch,260,panelpath.c_str());
		_fHeight = Height * CharToFloat(ch);
		GetPrivateProfileString("panel","position","(0,0,0)",ch,260,panelpath.c_str());
		_vPos = CharToVector3(ch);
		_vPos.x *= Width;
		_vPos.y *= Height;
	}
	GetPrivateProfileString("panel","controlnum","0",ch,260,panelpath.c_str());
	_nCount = CharToInt(ch);
	char num3[4];
	string ctrl;
	for(int i=0;i<_nCount; i++)
	{
		sprintf_s(num3,"%03d",i);
		ctrl="control";
		ctrl += num3;
		GetPrivateProfileString(ctrl.c_str(),"type",NULL,ch,260,panelpath.c_str());
		temp = ch;
		if(temp == "statictext")
		{
			MoonUIStaticText* stc = new MoonUIStaticText();
			_UIObj.push_back(stc);
		}
		else if(temp == "button")
		{
			MoonUIButton* btn = new MoonUIButton();
			_UIObj.push_back(btn);
		}
		else
			return false;
		GetPrivateProfileString(ctrl.c_str(),"width",NULL,ch,260,panelpath.c_str());
		float width = CharToFloat(ch);
		width = width * _fWidth;
		GetPrivateProfileString(ctrl.c_str(),"height",NULL,ch,260,panelpath.c_str());
		float height = CharToFloat(ch);
		height = height * _fHeight;
		GetPrivateProfileString(ctrl.c_str(),"position",NULL,ch,260,panelpath.c_str());
		D3DXVECTOR3 position = CharToVector3(ch);
		position.x = position.x * _fWidth + _vPos.x;
		position.y = position.y * _fHeight + _vPos.y;
		GetPrivateProfileString(ctrl.c_str(),"caption",NULL,ch,260,panelpath.c_str());
		string caption = ch;
		GetPrivateProfileString(ctrl.c_str(),"action",NULL,ch,260,panelpath.c_str());
		temp = ch;
		if(temp=="ok")
			_UIObj[i]->Create(ID_OK, _device,width,height,position,caption.c_str(),_pen);
		else if(temp == "about")
			_UIObj[i]->Create(ID_ABOUT, _device,width,height,position,caption.c_str(),_pen);
		else if(temp == "exit")
			_UIObj[i]->Create(ID_EXIT, _device,width,height,position,caption.c_str(),_pen);
		else if(temp == "static")
			_UIObj[i]->Create(ID_STATIC, _device,width,height,position,caption.c_str(),_pen);
	}

	V_RETURN(_device->CreateVertexBuffer(4*sizeof(UIVertex),D3DUSAGE_WRITEONLY,UIVertex::FVF,
		D3DPOOL_MANAGED, &_vb,0));
	UIVertex Vertices[]=
	{
		{_vPos.x,			_vPos.y,			0, 1.0f, 0.0f, 0.0f,},
		{_vPos.x+_fWidth,	_vPos.y,			0, 1.0f, 1.0f, 0.0f,},
		{_vPos.x,			_vPos.y+_fHeight,	0, 1.0f, 0.0f, 1.0f,},
		{_vPos.x+_fWidth,	_vPos.y+_fHeight,	0, 1.0f, 1.0f, 1.0f,},
	};
	void* pVertices;
	V_RETURN(_vb->Lock(0,sizeof(Vertices),&pVertices,0));
	memcpy(pVertices,Vertices,sizeof(Vertices));
	V_RETURN(_vb->Unlock());

	GetPrivateProfileString("panel","background",NULL,ch,260,panelpath.c_str());
	temp = ch;
	Moon_CreateTexture(_device, const_cast<TCHAR*>(GUI_DIR(temp.c_str())), &_tex);

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//允许使用alpha混合
	_device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return true;
}

void MoonUIPanel::Destroy()
{
	SAFE_RELEASE(_vb);
	SAFE_RELEASE(_tex);
}

DWORD MoonUIPanel::Test(int x, int y, bool bDown)
{
	DWORD value;
	for(int i=0;i<_nCount;i++)
	{
		value = _UIObj[i]->OnClick((float)x,(float)y,bDown);
		if(value!=0)
			return value;
	}
	return 0;
}

void MoonUIPanel::Draw()
{
	_device->SetTexture(0,_tex);
	_device->SetStreamSource(0, _vb, 0, sizeof(UIVertex));
	_device->SetFVF(UIVertex::FVF);
	_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	for(int i=0;i<_nCount;i++)
		_UIObj[i]->Paint();
}
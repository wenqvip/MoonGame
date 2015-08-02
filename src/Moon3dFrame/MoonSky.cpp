#include "MoonSky.h"

const DWORD MoonSky::SkyboxVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

MoonSky::MoonSky(IDirect3DDevice9* device, const char* str,
		float width, float depth, float height)
{
	_device = device;

	string skybox(str);
	string skyboxcfgname(IMAGE_DIR("Skybox.cfg"));
	char ch[256];

	GetPrivateProfileString(skybox.c_str(), "East", NULL, ch, 256, skyboxcfgname.c_str());
	_east = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "South", NULL, ch, 256, skyboxcfgname.c_str());
	_south = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "West", NULL, ch, 256, skyboxcfgname.c_str());
	_west = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "North", NULL, ch, 256, skyboxcfgname.c_str());
	_north = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "Sky", NULL, ch, 256, skyboxcfgname.c_str());
	_sky = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "Earth", NULL, ch, 256, skyboxcfgname.c_str());
	_earth = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "Cloud", NULL, ch, 256, skyboxcfgname.c_str());
	_cloud = IMAGE_DIR(ch);
	GetPrivateProfileString(skybox.c_str(), "CloudSpeedU", "0", ch, 256, skyboxcfgname.c_str());
	_cloudSpeedU = CharToFloat(ch);
	GetPrivateProfileString(skybox.c_str(), "CloudSpeedV", "0", ch, 256, skyboxcfgname.c_str());
	_cloudSpeedV = CharToFloat(ch);

	_width = width;
	_depth = depth;
	_height = height;
}

MoonSky::~MoonSky(void)
{
	SAFE_RELEASE(_vb);
	for(int i = 0; i < 6; i++)
		SAFE_RELEASE(_text[i]);
	SAFE_RELEASE(_vbCloud);
	SAFE_RELEASE(_texCloud);
}

bool MoonSky::initialize(void)
{
	VOID* pVertices;
	_device->CreateVertexBuffer(36*sizeof(SkyboxVertex), D3DUSAGE_WRITEONLY,
				SkyboxVertex::FVF, D3DPOOL_MANAGED, &_vb, 0);

	SkyboxVertex Vertices[] = 
	{
		// left(east)
		{-_width/2.0f, _height/2.0f, -_depth/2.0f, 0.0f, 0.0f,},
		{-_width/2.0f, _height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, _height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, -_height/2.0f, _depth/2.0f, 1.0f, 1.0f,},

		// front(south)
		{-_width/2.0f, _height/2.0f, _depth/2.0f, 0.0f, 0.0f,},
		{_width/2.0f, _height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, -_height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, -_height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, _height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, _depth/2.0f, 1.0f, 1.0f,},

		// right(west)
		{_width/2.0f, _height/2.0f, _depth/2.0f, 0.0f, 0.0f,},
		{_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, -_height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, -_depth/2.0f, 1.0f, 1.0f,},

		// back(north)
		{_width/2.0f, _height/2.0f, -_depth/2.0f, 0.0f, 0.0f,},
		{-_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, -_height/2.0f, -_depth/2.0f, 1.0f, 1.0f,},

		// up(sky)
		{-_width/2.0f, _height/2.0f, -_depth/2.0f, 0.0f, 0.0f,},
		{_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, _height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, _height/2.0f, _depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, _height/2.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, _height/2.0f, _depth/2.0f, 1.0f, 1.0f,},

		// down(earth)
		{-_width/2.0f, -_height/2.0f, _depth/2.0f, 0.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, -_height/2.0f, -_depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, -_height/2.0f, _depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, -_height/2.0f, -_depth/2.0f, 1.0f, 1.0f,},
	};

	if(FAILED(_vb->Lock(0, sizeof(Vertices), &pVertices, 0)))
		return false;
	memcpy(pVertices, Vertices, sizeof(Vertices));
	_vb->Unlock();

	_device->CreateVertexBuffer(6*sizeof(SkyboxVertex), D3DUSAGE_WRITEONLY,
				SkyboxVertex::FVF, D3DPOOL_MANAGED, &_vbCloud, 0);
	SkyboxVertex VerticesCloud[]=
	{
		{-_width/2.0f, _height/2.0f-1.0f, -_depth/2.0f, 0.0f, 0.0f,},
		{_width/2.0f, _height/2.0f-1.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{-_width/2.0f, _height/2.0f-1.0f, _depth/2.0f, 0.0f, 1.0f,},
		{-_width/2.0f, _height/2.0f-1.0f, _depth/2.0f, 0.0f, 1.0f,},
		{_width/2.0f, _height/2.0f-1.0f, -_depth/2.0f, 1.0f, 0.0f,},
		{_width/2.0f, _height/2.0f-1.0f, _depth/2.0f, 1.0f, 1.0f,},
	};
	if(FAILED(_vbCloud->Lock(0, sizeof(VerticesCloud), &pVertices, 0)))
		return false;
	memcpy(pVertices, VerticesCloud, sizeof(VerticesCloud));
	_vbCloud->Unlock();

	Moon_CreateTexture(_device, const_cast<TCHAR*>(_east.c_str()), &_text[0]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_south.c_str()), &_text[1]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_west.c_str()), &_text[2]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_north.c_str()), &_text[3]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_sky.c_str()), &_text[4]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_earth.c_str()), &_text[5]);
	Moon_CreateTexture(_device, const_cast<TCHAR*>(_cloud.c_str()), &_texCloud);

	return true;
}

void MoonSky::draw(void)
{
	D3DXMATRIX matOld;
	_device->GetTransform(D3DTS_WORLD, &matOld);
	_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	_device->SetTransform(D3DTS_WORLD, &_matWorld);
	_device->SetStreamSource(0, _vb, 0, sizeof(SkyboxVertex));
	_device->SetFVF(SkyboxVertex::FVF);

	for(int i = 0; i<6; i++)
	{
		if(_text[i])
			_device->SetTexture(0, _text[i]);
		_device->DrawPrimitive(D3DPT_TRIANGLELIST, i*6, 2);
	}
	// draw the cloud.
	_device->SetStreamSource(0,_vbCloud,0,sizeof(SkyboxVertex));
	_device->SetFVF(SkyboxVertex::FVF);
	_device->SetTexture(0, _texCloud);
	_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	_device->SetTransform(D3DTS_WORLD, &matOld);
}

void MoonSky::FrameMove(float x, float y, float z, float fElapsedTime)
{
	D3DXMatrixTranslation(&_matWorld, x, y, z);
	SkyboxVertex* pVertices;
	_vbCloud->Lock(0, 0, (void**)&pVertices, 0);
	for(int i=0; i<6; i++)
	{
		pVertices[i]._u -= _cloudSpeedU * fElapsedTime;
		pVertices[i]._v -= _cloudSpeedV * fElapsedTime;
	}
	_vbCloud->Unlock();
}

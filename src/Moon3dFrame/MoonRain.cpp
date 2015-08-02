#include "MoonRain.h"

const DWORD MoonRain::RAINVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;

MoonRain::MoonRain(void)
{
}

MoonRain::~MoonRain(void)
{
}

bool MoonRain::Initialize(LPDIRECT3DDEVICE9 device)
{
	if(FAILED(device->CreateVertexBuffer(2*sizeof(RAINVERTEX), 0, RAINVERTEX::FVF, D3DPOOL_MANAGED, 
			&_vb, NULL)))
		return false;
	_device = device;
	RAINVERTEX* pRainVertices;
	_vb->Lock(0, 0, (void**)&pRainVertices, 0);
	pRainVertices[0].vPos = D3DXVECTOR3(1,3,0);
	pRainVertices[0].dwColor = 0x7744aaff;
	pRainVertices[1].vPos = D3DXVECTOR3(2,1,0);
	pRainVertices[1].dwColor = 0x7744aaff;
	_vb->Unlock();

	for(int i=0; i<PARTICLENUM; i++)
	{
		_rain[i].x = float(rand()%600-300);
		_rain[i].z = float(rand()%600-300);
		_rain[i].y = 0.0f + float(rand()%300);
		_rain[i].fDspeed = 100.0f + (rand()%40);
	}

	_RainDir = D3DXVECTOR3(3,1,0) - D3DXVECTOR3(1,5,0);
	D3DXVec3Normalize(&_RainDir,&_RainDir);

	return true;
}

void MoonRain::FrameMove(float x, float y, float z, float ftime)
{
	D3DXMatrixTranslation(&_matWorld, x, y, z);
	for(int i=0; i<PARTICLENUM; i++)
	{
		if(_rain[i].y<0)
			_rain[i].y = 300.0f;
		if(_rain[i].x>300)
			_rain[i].x = -300.0f;
		if(_rain[i].z>300)
			_rain[i].z = float(rand()%600-300);
		_rain[i].x += _rain[i].fDspeed * _RainDir.x * ftime;
		_rain[i].y += _rain[i].fDspeed * _RainDir.y * ftime;
		_rain[i].z += _rain[i].fDspeed * _RainDir.z * ftime;
	}
}

void MoonRain::Render()
{
	_device->SetTexture(0, NULL);
	D3DMATERIAL9 mtrl;
	Moon_InitMaterial(mtrl, 1,1,1);
	_device->SetMaterial(&mtrl);
	_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	for(int i=0; i<PARTICLENUM; i++)
	{
		D3DXMATRIX matRain;
		D3DXMatrixTranslation(&matRain, _rain[i].x, _rain[i].y, _rain[i].z);
		D3DXMatrixMultiply(&matRain, &matRain, &_matWorld);
		_device->SetTransform(D3DTS_WORLD, &matRain);
		_device->SetStreamSource(0, _vb, 0, sizeof(RAINVERTEX));
		_device->SetFVF(RAINVERTEX::FVF);
		_device->DrawPrimitive(D3DPT_LINELIST, 0, 1);
	}
	_device->SetRenderState(D3DRS_LIGHTING, TRUE);
}
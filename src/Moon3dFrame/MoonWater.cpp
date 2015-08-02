#include "MoonWater.h"
const DWORD MoonWater::WATERVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

MoonWater::MoonWater(void)
{
}

MoonWater::~MoonWater(void)
{
	SAFE_RELEASE(_vb);
	SAFE_RELEASE(_ib);
	SAFE_RELEASE(_tex);
}

bool MoonWater::Initialize(LPDIRECT3DDEVICE9 device, float height, float flx, float flz, 
		float brx, float brz, float waveheight, float flowspeedu, float flowspeedv)
{
	_device = device;
	_fHeight = height;
	_flx = flx, _flz = flz;
	_brx = brx, _brz = brz;
	_waveheight = waveheight;
	_flowspeedu = flowspeedu;
	_flowspeedv = flowspeedv;
	_bInWater = false;
	_bChange = false;

	V_RETURN(_device->CreateVertexBuffer(4*sizeof(WATERVERTEX), D3DUSAGE_WRITEONLY, 
		WATERVERTEX::FVF, D3DPOOL_MANAGED, &_vb, NULL));
	WATERVERTEX Vertices[]=
	{
		{_flx, _fHeight, _flz, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,},
		{_brx, _fHeight, _flz, 0.0f, 1.0f, 0.0f, 0.0f, 3.0f,},
		{_flx, _fHeight, _brz, 0.0f, 1.0f, 0.0f, 3.0f, 0.0f,},
		{_brx, _fHeight, _brz, 0.0f, 1.0f, 0.0f, 3.0f, 3.0f,},
	};
	VOID* pVertices;
	V_RETURN(_vb->Lock(0,sizeof(WATERVERTEX),&pVertices, 0));
	memcpy(pVertices, Vertices, sizeof(Vertices));
	V_RETURN(_vb->Unlock());

	V_RETURN(_device->CreateIndexBuffer(6*sizeof(WORD), 0, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &_ib, NULL));
	WORD Indices[] = {0,1,2,2,1,3};
	VOID* pIndices;
	V_RETURN(_ib->Lock(0, sizeof(Indices), (void**)&pIndices, 0));
	memcpy(pIndices, Indices, sizeof(Indices));
	V_RETURN(_ib->Unlock());

	Moon_CreateTexture(_device, const_cast<TCHAR*>(IMAGE_DIR("water.tga")), &_tex);

	return true;
}

bool MoonWater::InWaterChecking(D3DXVECTOR3* pos)
{
	if(pos->x>_flx && pos->x<_brx && pos->z>_brz && pos->z<_flz)
	{
		float dx = pos->x-_flx;
		float dz = pos->z-_brz;
		float Ax = 0.0f,Az = 0.0f,Dx = _brx-_flx, Dz = _flz-_brz;
		float height;
		if((dx)/(dz) >= (Dx)/(Dz))
		{
			height = _height[3] + (_height[2]-_height[3])*(Dx-dx)/Dx + (_height[1]-_height[3])*(dz)/Dz;
		}
		else
		{
			height = _height[0] + (_height[1]-_height[0])*(dx/Dx) + (_height[2]-_height[0])*(Dz-dz)/Dz;
		}
		if(pos->y > height)
			return false;
		else
			return true;
	}
	else 
		return false;
}

void MoonWater::FrameMove(float fElapsedTime, float fTime, D3DXVECTOR3* pos)
{
	WATERVERTEX* pVertices;
	/*_fRealHeight = _fHeight + sin(fTime) * _waveheight;*/
	_vb->Lock(0,0,(void**)&pVertices,0);
	for(int i=0; i<4; i++)
	{
		pVertices[i].y = _fHeight + sin(fTime+i*1.57f) * _waveheight;
		_height[i] = pVertices[i].y;
		pVertices[i].u += fElapsedTime * _flowspeedu;
		pVertices[i].v += fElapsedTime * _flowspeedv;
	}
	_vb->Unlock();

	if(InWaterChecking(pos) && !_bInWater)
	{
		_bInWater = true;
		_bChange = true;
		_device->GetRenderState(D3DRS_FOGCOLOR, &_OrinFog.fogcolor);
		_device->GetRenderState(D3DRS_FOGTABLEMODE, &_OrinFog.fogmode);
		_device->GetRenderState(D3DRS_FOGSTART, &_OrinFog.fogstart);
		_device->GetRenderState(D3DRS_FOGEND, &_OrinFog.fogend);
		_device->GetRenderState(D3DRS_FOGDENSITY, &_OrinFog.fogdensity);
		_device->GetRenderState(D3DRS_FOGENABLE, &_OrinFog.fogenable);
		/*return;*/
	}
	else if(!InWaterChecking(pos) && _bInWater)
	{
		_bInWater = false;
		_bChange = true;
		//_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		/*return;*/
	}
	if(_bChange&&_bInWater)
	{
		_bChange = false;
		float density = 0.008f;
		_device->SetRenderState(D3DRS_FOGCOLOR, 0xff3399ff);
		_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
		_device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&density);
		_device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	}
	else if(_bChange&&!_bInWater)
	{
		_bChange = false;
		_device->SetRenderState(D3DRS_FOGCOLOR, _OrinFog.fogcolor);
		_device->SetRenderState(D3DRS_FOGTABLEMODE, _OrinFog.fogmode);
		_device->SetRenderState(D3DRS_FOGDENSITY, _OrinFog.fogdensity);
		_device->SetRenderState(D3DRS_FOGSTART, _OrinFog.fogstart);
		_device->SetRenderState(D3DRS_FOGEND, _OrinFog.fogend);
		_device->SetRenderState(D3DRS_FOGENABLE, _OrinFog.fogenable);
	}
}

void MoonWater::Render()
{
	if(_bInWater)
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	else
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	/*_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);*/
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	_device->SetTransform(D3DTS_WORLD, &matWorld);
	_device->SetTexture(0, _tex);
	_device->SetStreamSource(0,_vb,0,sizeof(WATERVERTEX));
	_device->SetIndices(_ib);
	_device->SetFVF(WATERVERTEX::FVF);
	_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	if(_bInWater)
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
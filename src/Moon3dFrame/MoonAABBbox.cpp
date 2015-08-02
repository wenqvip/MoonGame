#include "MoonAABBbox.h"
const DWORD MoonAABBbox::AABBVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

MoonAABBbox::MoonAABBbox(D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 pos, bool bDraw)
{
	_bDrawBox = bDraw;
	_min = min;
	_max = max;
	_pos = pos;
}

MoonAABBbox::~MoonAABBbox(void)
{
}

void MoonAABBbox::InitVertexBuffer(LPDIRECT3DDEVICE9 device)
{
	_device = device;
	if(_bDrawBox)
	{
		_device->CreateVertexBuffer(8*sizeof(AABBVertex), 0, AABBVertex::FVF, D3DPOOL_MANAGED, &_vb, 0);
		AABBVertex Vertices[]=
		{
			{ _min.x, _min.y, _min.z, 0xff00ff00, },
			{ _min.x, _min.y, _max.z, 0xff00ff00, },
			{ _max.x, _min.y, _max.z, 0xff00ff00, },
			{ _max.x, _min.y, _min.z, 0xff00ff00, },
			{ _min.x, _max.y, _min.z, 0xff00ff00, },
			{ _min.x, _max.y, _max.z, 0xff00ff00, },
			{ _max.x, _max.y, _max.z, 0xff00ff00, },
			{ _max.x, _max.y, _min.z, 0xff00ff00, },
		};
		VOID* pVertices;
		_vb->Lock(0, 0, (void**)&pVertices, 0);
		memcpy(pVertices, Vertices, sizeof(Vertices));
		_vb->Unlock();

		_device->CreateIndexBuffer(24*sizeof(WORD),0,D3DFMT_INDEX16, D3DPOOL_MANAGED, &_ib, 0);
		WORD* pIndices;
		_ib->Lock(0,0,(void**)&pIndices,0);
		pIndices[0] = 0;	pIndices[1] = 1;
		pIndices[2] = 1;	pIndices[3] = 2;
		pIndices[4] = 2;	pIndices[5] = 3;
		pIndices[6] = 3;	pIndices[7] = 0;
		pIndices[8] = 0;	pIndices[9] = 4;
		pIndices[10] = 1;	pIndices[11] = 5;
		pIndices[12] = 2;	pIndices[13] = 6;
		pIndices[14] = 3;	pIndices[15] = 7;
		pIndices[16] = 4;	pIndices[17] = 5;
		pIndices[18] = 5;	pIndices[19] = 6;
		pIndices[20] = 6;	pIndices[21] = 7;
		pIndices[22] = 7;	pIndices[23] = 4;
		_ib->Unlock();
	}
}

void MoonAABBbox::draw()
{
	if(_bDrawBox&&_device)
	{
		D3DXMATRIX M;
		D3DXMatrixTranslation(&M,_pos.x,_pos.y,_pos.z);
		_device->SetTexture(0, NULL);
		_device->SetTransform(D3DTS_WORLD, &M);
		_device->SetRenderState(D3DRS_LIGHTING, FALSE);
		_device->SetFVF(AABBVertex::FVF);
		_device->SetIndices(_ib);
		_device->SetStreamSource(0,_vb,0,sizeof(AABBVertex));
		_device->DrawIndexedPrimitive(D3DPT_LINELIST,0,0,8,0,12);
		_device->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}

void MoonAABBbox::Update(D3DXVECTOR3 pos)
{
	_pos = pos;
}
D3DXVECTOR3 MoonAABBbox::getMin() 
{
	return (_min+_pos);
}
D3DXVECTOR3 MoonAABBbox::getMax()
{
	return (_max+_pos);
}
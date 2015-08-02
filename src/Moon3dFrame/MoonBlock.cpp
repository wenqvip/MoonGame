#include "MoonBlock.h"

MoonBlock::MoonBlock()
{
}

MoonBlock::~MoonBlock(void)
{
}

void MoonBlock::Initialize(D3DCAPS9* d3dCaps, const char* blockname, DWORD ID , D3DXVECTOR3 pos,
				D3DXVECTOR3 rot, D3DXVECTOR3 scl)
{
	_ID = 0x00050000|ID;
	_pCaps = d3dCaps;
	_strBlockName = blockname;

	D3DXMATRIX matPos,matScl,matRotX,matRotY,matRotZ;
	D3DXMatrixTranslation(&matPos, pos.x, pos.y, pos.z);
	D3DXMatrixScaling(&matScl, scl.x, scl.y, scl.z);
	D3DXMatrixRotationX(&matRotX, rot.x);
	D3DXMatrixRotationY(&matRotY, rot.y);
	D3DXMatrixRotationZ(&matRotZ, rot.z);
	D3DXMatrixMultiply(&_matWorld, &matRotX, &matRotY);
	D3DXMatrixMultiply(&_matWorld, &_matWorld, &matRotZ);
	D3DXMatrixMultiply(&_matWorld, &_matWorld, &matScl);
	D3DXMatrixMultiply(&_matWorld, &_matWorld, &matPos);
}

HRESULT MoonBlock::InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
	_pDevice = pDevice;
	_pBlock = new MoonSkinmesh(_pDevice, _pCaps, _strBlockName.c_str());
	if(FAILED(_pBlock->InitDeviceObjects()))
	{
		SAFE_DELETE(_pBlock);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT MoonBlock::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	if(_pBlock)
	{
		hr = _pBlock->DeleteDeviceObjects();
		SAFE_DELETE(_pBlock);
	}
	_pDevice = NULL;
	return hr;
}

void MoonBlock::FrameMove(float ftime)
{
	if(_pBlock)
		_pBlock->FrameMove(ftime,_matWorld);
}

void MoonBlock::Draw()
{
	if(_pBlock)
		_pBlock->Draw();
}

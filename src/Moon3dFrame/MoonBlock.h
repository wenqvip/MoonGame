#pragma once
#include "MoonUtility.h"
#include "MoonSkinmesh.h"
#include "MoonObject.h"

class MoonBlock : public MoonObject
{	
	LPDIRECT3DDEVICE9 _pDevice;
	D3DCAPS9* _pCaps;
	string _strBlockName;
	D3DXVECTOR3 _pos;
	D3DXVECTOR3 _scl;
	D3DXVECTOR3 _rot;
	D3DXMATRIX _matWorld;
	MoonSkinmesh * _pBlock;
public:
	MoonBlock();
	~MoonBlock(void);
	void		Initialize(D3DCAPS9* d3dCaps, const char* blockname, DWORD ID ,D3DXVECTOR3 pos,
					D3DXVECTOR3 rot=D3DXVECTOR3(0,0,0), D3DXVECTOR3 scl=D3DXVECTOR3(1,1,1));

	HRESULT		InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
	HRESULT		DeleteDeviceObjects(void);
	/*HRESULT		RestoreDeviceObjects(void);
	HRESULT		InvalidateDeviceObjects(void);*/

	void		FrameMove(float ftime);
	void		Draw(void);

	virtual D3DXVECTOR3 GetPos() { return _pos; };
	virtual void Back(D3DXVECTOR3 back)	{};
};

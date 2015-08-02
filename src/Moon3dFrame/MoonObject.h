#pragma once
#include "MoonUtility.h"

class MoonObject
{
protected:
	DWORD _ID;
public:
	MoonObject(void);
	~MoonObject(void);
	DWORD GetID() { return _ID; };

	virtual void Back(D3DXVECTOR3 back) {};
	virtual D3DXVECTOR3 GetPos() { return D3DXVECTOR3(0,0,0); };
};

#pragma once
#include "MoonUtility.h"
#include "MoonAABBbox.h"
#include "MoonObject.h"

class MoonCollision
{
	struct Collision
	{
		Collision(DWORD id, MoonObject* pObj, MoonAABBbox* pB)
		{
			ID = id; pObject = pObj; pBox = pB; wCollisionType = 0;
		};
		DWORD ID;
		WORD wCollisionType;
		MoonObject* pObject;
		MoonAABBbox* pBox;
	};
	vector<Collision*> _MovingObjects;
	vector<Collision*> _StaticObjects;
	bool _bDrawAABBbox;
	/*LPDIRECT3DDEVICE9 _device;*/

	WORD CollisionTestAABB(D3DXVECTOR3 A1, D3DXVECTOR3 A2,
				D3DXVECTOR3 B1, D3DXVECTOR3 B2, D3DXVECTOR3* feedback);
public:
	MoonCollision(bool bDraw = false);
	~MoonCollision(void);
	void Add(DWORD id, MoonObject* pObj, D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 pos);
	void Delete(DWORD id);
	void InitDevice(LPDIRECT3DDEVICE9 device);
	void Test();
	void Draw();
	WORD GetCollisionType(DWORD id);
};

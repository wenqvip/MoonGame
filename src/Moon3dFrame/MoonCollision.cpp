#include "MoonCollision.h"

MoonCollision::MoonCollision(bool bDraw)
{
	_bDrawAABBbox = bDraw;
}

MoonCollision::~MoonCollision(void)
{
	while(!_MovingObjects.empty())
		_MovingObjects.pop_back();
	while(!_StaticObjects.empty())
		_StaticObjects.pop_back();
}

void MoonCollision::Add(DWORD id, MoonObject *pObj, D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 pos)
{	
	MoonAABBbox *box = new MoonAABBbox(min,max,pos,_bDrawAABBbox);
	Collision* col = new Collision(id,pObj,box);
	if((id&0x000f0000)==0x00040000)
	{
		_MovingObjects.push_back(col);
	}
	else if((id&0x000f0000)==0x00050000)
	{
		_StaticObjects.push_back(col);
	}
}

void MoonCollision::Delete(DWORD id)
{
	vector<Collision*>::const_iterator Iter;
	if((id&0x000f0000)==0x00040000)
	{
		for(Iter = _MovingObjects.begin(); Iter != _MovingObjects.end(); Iter++)
		{
			if((*Iter)->ID == id)
			{
				delete (*Iter);
				_MovingObjects.erase(Iter);
			}
		}
	}
	else if((id&0x000f0000)==0x00050000)
	{
		for(Iter = _StaticObjects.begin(); Iter != _StaticObjects.end(); Iter++)
		{
			if((*Iter)->ID == id)
			{
				delete (*Iter);
				_StaticObjects.erase(Iter);
				return;
			}
		}
	}
}

void MoonCollision::InitDevice(LPDIRECT3DDEVICE9 device)
{
	for(int i=0; i<_MovingObjects.size(); i++)
		_MovingObjects[i]->pBox->InitVertexBuffer(device);
	for(int j=0; j<_StaticObjects.size(); j++)
		_StaticObjects[j]->pBox->InitVertexBuffer(device);
}

void MoonCollision::Test()
{
	D3DXVECTOR3 feedback;
	WORD value;
	for(int i=0; i<_MovingObjects.size(); i++)
	{
		_MovingObjects[i]->pBox->Update(_MovingObjects[i]->pObject->GetPos());
		_MovingObjects[i]->wCollisionType = 0;
		for(int j=0; j<_StaticObjects.size(); j++)
		{
			feedback = _MovingObjects[i]->pObject->GetPos();
			if(value = CollisionTestAABB( _MovingObjects[i]->pBox->getMin(),_MovingObjects[i]->pBox->getMax(),
				_StaticObjects[j]->pBox->getMin(),_StaticObjects[j]->pBox->getMax(), &feedback) )
			{
				_MovingObjects[i]->pObject->Back(feedback);
				_MovingObjects[i]->wCollisionType |= value;
				WORD temp = _MovingObjects[i]->wCollisionType;
				_MovingObjects[i]->pBox->Update(_MovingObjects[i]->pObject->GetPos());
			}
		}
	}
}

void MoonCollision::Draw()
{
	for(int i=0; i<_MovingObjects.size(); i++)
		_MovingObjects[i]->pBox->draw();
	for(int j=0; j<_StaticObjects.size(); j++)
		_StaticObjects[j]->pBox->draw();
}

WORD MoonCollision::CollisionTestAABB(D3DXVECTOR3 A1, D3DXVECTOR3 A2, 
									  D3DXVECTOR3 B1, D3DXVECTOR3 B2, D3DXVECTOR3* feedback)
{
	if(A1.x<B2.x && A2.x>B1.x && A1.y<B2.y && A2.y>B1.y && A1.z<B2.z && A2.z>B1.z)
	{
		float x, z;
		if(A1.y>(B2.y-3.0f))
		{
			feedback->y = A1.y - B2.y;
			feedback->x = 0.0f;
			feedback->z = 0.0f;
			return 0x0001;
		}
		else
			feedback->y = 0.0f;
		if(feedback->x < B2.x && feedback->z < B2.z)
		{
			feedback->x = 0.0f;
			feedback->z = 0.0f;
			x = A2.x - B1.x;
			z = A2.z - B1.z;
			if(x<=z)
				feedback->x = x;
			else
				feedback->z = z;
		}
		else if(feedback->x > B1.x && feedback->z > B1.z)
		{
			feedback->x = 0.0f;
			feedback->z = 0.0f;
			x = A1.x - B2.x;
			z = A1.z - B2.z;
			if(x>=z)
				feedback->x = x;
			else
				feedback->z = z;
		}
		else
		{
			feedback->x = 0.0f;
			feedback->z = 0.0f;
		}
		return 0x0002;
	}
	else
	{
		return 0x0000;
	}
}

WORD MoonCollision::GetCollisionType(DWORD id)
{
	for(int i=0;i<_MovingObjects.size();i++)
	{
		if(_MovingObjects[i]->ID == id)
			return _MovingObjects[i]->wCollisionType;
	}
	return 0;
}
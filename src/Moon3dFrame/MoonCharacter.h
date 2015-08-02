#pragma once
#include "MoonUtility.h"
#include "MoonSkinmesh.h"
#include "d3dx9math.h"
#include "MoonObject.h"

class MoonCharacter : public MoonObject
{
	LPDIRECT3DDEVICE9 _pDevice;
	D3DCAPS9* _d3dCaps;
	MoonSkinmesh * _pCharacter;
	string _strCharacterName;
	D3DXVECTOR3 _pos;
	D3DXVECTOR3 _scale;
	D3DXVECTOR3 _origenalDir;
	/*D3DXVECTOR3 _currentDir;*/
	D3DXMATRIX _matRot;
	D3DXMATRIX _matScl;
	bool _bRunning;
	string _strAnimSet[6];
	D3DXVECTOR3 _posIncrement;

	void		MoveOnVec(D3DXVECTOR3 vec, float unit);

public:
	/*bool _bHanging;*/
	MoonCharacter( D3DCAPS9* d3dCaps, const char* CharacterName, DWORD ID,
				D3DXVECTOR3 pos, D3DXVECTOR3 direction, D3DXVECTOR3 scale = D3DXVECTOR3(1,1,1));
	~MoonCharacter(void);

	virtual void		Back(D3DXVECTOR3 back);
	virtual D3DXVECTOR3 GetPos() { return _pos; };
	D3DXVECTOR3 GetPosIncre() { return _posIncrement; };

	HRESULT		InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
	HRESULT		DeleteDeviceObjects(void);
	/*HRESULT		RestoreDeviceObjects(void);
	HRESULT		InvalidateDeviceObjects(void);*/

	void		FrameMove(float ftime);
	void		Draw(void);

	void		TurnRight(D3DXVECTOR3 frontVector, float unit);
	void		TurnLeft(D3DXVECTOR3 frontVector, float unit);
	void		TurnFront(D3DXVECTOR3 frontVector, float unit);
	void		TurnBack(D3DXVECTOR3 frontVector, float unit);
	void		TurnRF(D3DXVECTOR3 frontVector, float unit);
	void		TurnLF(D3DXVECTOR3 frontVector, float unit);
	void		TurnRB(D3DXVECTOR3 frontVector, float unit);
	void		TurnLB(D3DXVECTOR3 frontVector, float unit);
	void		Idle();
	void		Skill1();
	void		Skill2();
	void		Skill3();

	void ToggleRunning(void);
	void setPos(D3DXVECTOR3* pos);
	void getPos(D3DXVECTOR3* pos);
	void setBack() {_pos -= _posIncrement;};
};

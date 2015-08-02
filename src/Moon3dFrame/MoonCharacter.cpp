#include "MoonCharacter.h"

MoonCharacter::MoonCharacter( D3DCAPS9* d3dCaps, const char* CharacterName, DWORD ID,
				D3DXVECTOR3 pos, D3DXVECTOR3 direction, D3DXVECTOR3 scale)
{
	/*_bHanging = true;*/
	_ID = 0x00040000|ID;
	_pDevice = NULL;
	_d3dCaps = d3dCaps;
	_pCharacter = NULL;
	_strCharacterName = CharacterName;
	_pos = pos;
	_scale = scale;//角色的缩放
	_origenalDir = direction;//为角色设定的原始朝向，也就是说这个向量指向的方向是角色开始时所面朝的方向。
	D3DXMatrixIdentity(&_matRot);
	D3DXMatrixScaling(&_matScl, _scale.x, _scale.y, _scale.z);

	_bRunning = true;

	string cfgpath = MODEL_DIR("character.cfg");
	char ch[256];
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_idle", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[0] = ch;
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_walk", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[1] = ch;
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_run", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[2] = ch;
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_skill1", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[3] = ch;
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_skill2", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[4] = ch;
	GetPrivateProfileString(_strCharacterName.c_str(), "AnimSet_skill3", NULL, ch, 256, cfgpath.c_str());
	_strAnimSet[5] = ch;
}

MoonCharacter::~MoonCharacter(void)
{
}

HRESULT MoonCharacter::InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
	_pDevice = pDevice;
	_pCharacter = new MoonSkinmesh(_pDevice, _d3dCaps, _strCharacterName.c_str());
	return _pCharacter->InitDeviceObjects();
}

HRESULT MoonCharacter::DeleteDeviceObjects()
{
	HRESULT hr;
	hr = _pCharacter->DeleteDeviceObjects();
	SAFE_DELETE(_pCharacter);
	_pDevice = NULL;
	return hr;
}

//HRESULT MoonCharacter::RestoreDeviceObjects()
//{
//	return _pCharacter->RestoreDeviceObjects();
//}
//
//HRESULT MoonCharacter::InvalidateDeviceObjects()
//{
//	return _pCharacter->InvalidateDeviceObjects();
//}

void MoonCharacter::FrameMove(float ftime)
{
	//类自己的_matWorld记录着角色的旋转和缩放
	D3DXMATRIX M;
	D3DXMatrixIdentity(&M);
	D3DXMatrixMultiply(&M, &M, &_matScl);
	D3DXMatrixMultiply(&M, &M, &_matRot);

	//M_adjust是为了矫正原始文件中的朝向问题，只针对hero.x文件，可以注释掉。
	D3DXMATRIX M_adjust;
	D3DXMatrixRotationY(&M_adjust,-0.2f);
	D3DXMatrixMultiply(&M, &M, &M_adjust);

	D3DXMATRIX M_trans;
	D3DXMatrixTranslation(&M_trans, _pos.x, _pos.y, _pos.z);
	D3DXMatrixMultiply(&M,&M,&M_trans);

	_pCharacter->FrameMove(ftime, M);
}

void MoonCharacter::Draw()
{
	_pCharacter->Draw();
}

void MoonCharacter::TurnRight(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXVECTOR3 vec;
	vec.x = frontVector.z;
	vec.y = 0;
	vec.z = -frontVector.x;
	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnLeft(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXVECTOR3 vec;
	vec.x = -frontVector.z;
	vec.y = 0;
	vec.z = frontVector.x;
	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnFront(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXVECTOR3 vec;
	vec.x = frontVector.x;
	vec.y = 0;
	vec.z = frontVector.z;
	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnBack(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXVECTOR3 vec;
	vec.x = -frontVector.x;
	vec.y = 0.0f;
	vec.z = -frontVector.z;
	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnRF(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXMATRIX M;
	D3DXMatrixRotationY(&M, D3DX_PI/4.0f);
	D3DXVECTOR3 vec = frontVector;
	D3DXVec3TransformNormal(&vec,&vec,&M);

	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnLF(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXMATRIX M;
	D3DXMatrixRotationY(&M, -D3DX_PI/4.0f);
	D3DXVECTOR3 vec = frontVector;
	D3DXVec3TransformNormal(&vec,&vec,&M);

	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnRB(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXMATRIX M;
	D3DXMatrixRotationY(&M, D3DX_PI/4.0f*3.0f);
	D3DXVECTOR3 vec = frontVector;
	D3DXVec3TransformNormal(&vec,&vec,&M);

	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::TurnLB(D3DXVECTOR3 frontVector, float unit)
{
	D3DXMatrixIdentity(&_matRot);

	D3DXMATRIX M;
	D3DXMatrixRotationY(&M, -D3DX_PI/4.0f*3.0f);
	D3DXVECTOR3 vec = frontVector;
	D3DXVec3TransformNormal(&vec,&vec,&M);

	D3DXVec3Normalize(&vec, &vec);
	MoveOnVec(vec,unit);
}

void MoonCharacter::MoveOnVec(D3DXVECTOR3 vec, float unit)
{
	float angle = acos(D3DXVec3Dot(&_origenalDir,&vec));
	if(vec.x>=0.0f)
		D3DXMatrixRotationY(&_matRot, -angle);
	else
		D3DXMatrixRotationY(&_matRot, angle);

	/*if(_bHanging == true)
		return;*/
	//计算角色位置
	vec.y = 0.0f;
	D3DXVec3Normalize(&vec,&vec);
	if(_bRunning)
	{
		_pCharacter->SetAnimByName(_strAnimSet[2].c_str());
		_posIncrement = unit * vec * 3.0f;
		_pos += _posIncrement;
	}
	else
	{
		_pCharacter->SetAnimByName(_strAnimSet[1].c_str());
		_posIncrement = unit * vec;
		_pos += _posIncrement;
	}
}

void MoonCharacter::Back(D3DXVECTOR3 back)
{
	_pos -= back;
}

void MoonCharacter::Idle()
{
	_pCharacter->SetAnimByName(_strAnimSet[0].c_str());
}

void MoonCharacter::Skill1()
{
	if(!_strAnimSet[3].empty())
		_pCharacter->SetAnimByName(_strAnimSet[3].c_str());
}

void MoonCharacter::Skill2()
{
	if(!_strAnimSet[4].empty())
		_pCharacter->SetAnimByName(_strAnimSet[4].c_str());
}

void MoonCharacter::Skill3()
{
	if(!_strAnimSet[5].empty())
		_pCharacter->SetAnimByName(_strAnimSet[5].c_str());
}

void MoonCharacter::ToggleRunning(void)
{
	_bRunning = !_bRunning;
}

void MoonCharacter::setPos(D3DXVECTOR3* pos)
{
	_pos.x = pos->x;
	_pos.y = pos->y;
	_pos.z = pos->z;
}

void MoonCharacter::getPos(D3DXVECTOR3* pos)
{
	*pos = _pos;
}

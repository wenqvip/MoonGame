#include "MoonCamera.h"

//MoonCamera::MoonCamera(void)
//{
//	_cameraType = LANDOBJECT;
//
//	_pos = D3DXVECTOR3(0, 0, 0);
//	_look = D3DXVECTOR3(0, 0, 1);
//	_up = D3DXVECTOR3(0, 1, 0);
//	_right = D3DXVECTOR3(1, 0, 0);
//	_distance = 0.0f;
//	_angle = 0.0f;
//}

MoonCamera::MoonCamera(CameraType ct)
{
	_cameraType = ct;

	_pos = D3DXVECTOR3(0, 0, 0);
	_look = D3DXVECTOR3(0, 0, 1);
	_up = D3DXVECTOR3(0, 1, 0);
	_right = D3DXVECTOR3(1, 0, 0);

	if( _cameraType == RPG )
		_distance = 50.0f;
	else
		_distance = 0.0f;
}

MoonCamera::~MoonCamera(void)
{
}

// left/right
void MoonCamera::strafe(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT || _cameraType == RPG )
		_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _right * units;
}

// up/down
void MoonCamera::fly(float units)
{
	// move only on y-axis for land object
	if( _cameraType == LANDOBJECT || _cameraType == RPG )
		_pos.y += units;

	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

// forward/backward
void MoonCamera::walk(float units)
{
	/*int i = _pos.x;*/
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT || _cameraType == RPG )
	{
		D3DXVECTOR3 temp;
		D3DXVec3Normalize(&temp,&D3DXVECTOR3(_look.x, 0.0f, _look.z));
		_pos += temp * units;
	}

	if( _cameraType == AIRCRAFT )
		_pos += _look * units;
	/*if((_pos.x - i) > 2 || (_pos.x - i) < -2)
		i = 0;*/
}

// rotate on right vector
void MoonCamera::pitch(float angle)
{
	if(angle == 0.0f)
		return;
	/*if(_cameraType == LANDOBJECT || _cameraType == RPG)
		if((_look.y < -0.999f && angle > 0.0f)||(_look.y > 0.999f && angle < 0.0f))
			return;*/
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_right, angle);

	if( _cameraType == RPG )
		_pos += _look * _distance;

	/*D3DXVECTOR3 v = _look;*/
	// rotate _up and _look around -right vector
	D3DXVec3TransformCoord(&_up, &_up, &T);
	D3DXVec3TransformCoord(&_look, &_look, &T);

	/*if(D3DXVec3Dot(&_look, &D3DXVECTOR3(v.x, 0.0f, v.z))<0.0f)
	{
		_look.x = 0.0f; _look.z = 0.0f;
		_up.y = 0.0f;
		if(_look.y < 0.0f)
			_look.y = -1.0f;
		else
			_look.y = 1.0f;
	}*/

	if( _cameraType == RPG )
		_pos -= _look * _distance;
}

// rotate on up vector
void MoonCamera::yaw(float angle)
{
	D3DXMATRIX T;
	
	//rotate around world y (0,1,0) always for land object
	if( _cameraType == LANDOBJECT || _cameraType == RPG )
		D3DXMatrixRotationY(&T, angle);

	// rotate around own up vector for aircraft
	else if( _cameraType == AIRCRAFT )
		D3DXMatrixRotationAxis(&T, &_up, angle);


	if( _cameraType == RPG )
		_pos += _look * _distance;

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord(&_right, &_right, &T);
	D3DXVec3TransformCoord(&_look, &_look, &T);

	if( _cameraType == RPG )
		_pos -= _look * _distance;
}

// rotate on look vector
void MoonCamera::roll(float angle)
{
	// only roll for aircraft type
	if( _cameraType == AIRCRAFT )
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_look, angle);

		// rotate _up and _right around _look vector
		D3DXVec3TransformCoord(&_right, &_right, &T);
		D3DXVec3TransformCoord(&_up, &_up, &T);
	}
}

void MoonCamera::getViewMatrix(D3DXMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother
	D3DXVec3Normalize(&_look,&_look);
	// ��������������LANDOBJECT��RPG����ô��ʹ����������ˮƽ
	if(_cameraType == LANDOBJECT || _cameraType == RPG)
		_right.y = 0.0f;

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x;
	(*V)(0,1) = _up.x;
	(*V)(0,2) = _look.x;
	(*V)(0,3) = 0.0f;

	(*V)(1,0) = _right.y;
	(*V)(1,1) = _up.y;
	(*V)(1,2) = _look.y;
	(*V)(1,3) = 0.0f;

	(*V)(2,0) = _right.z;
	(*V)(2,1) = _up.z;
	(*V)(2,2) = _look.z;
	(*V)(2,3) = 0.0f;

	(*V)(3,0) = x;
	(*V)(3,1) = y;
	(*V)(3,2) = z;
	(*V)(3,3) = 1.0f;
}

//void MoonCamera::setCameraType(CameraType ct)
//{
//	_cameraType = ct;
//
//	if( _cameraType == RPG )
//		_distance = 20.0f;
//	else
//		_distance = 0.0f;
//}

void MoonCamera::getPosition(D3DXVECTOR3* pos)
{
	*pos = _pos;
}

void MoonCamera::setPosition(D3DXVECTOR3* pos)
{
	_pos = *pos;
}

void MoonCamera::getRight(D3DXVECTOR3* right)
{
	*right = _right;
}

void MoonCamera::getUp(D3DXVECTOR3* up)
{
	*up = _up;
}

void MoonCamera::getLook(D3DXVECTOR3* look)
{
	*look = _look;
}

void MoonCamera::setLook(D3DXVECTOR3* look)
{
	_look = *look;

	if( _look == D3DXVECTOR3(0, 1, 0) )
		_up = D3DXVECTOR3(0, 0, -1);
	if( _look == D3DXVECTOR3(1, 0, 0) )
		_right = D3DXVECTOR3(0, 0, -1);
}

// only be used in RPG type
void MoonCamera::getLookAtPos(D3DXVECTOR3* pos)
{
		*pos = _pos + (_look * _distance);
}

// only be used in RPG type
void MoonCamera::setLookAtPos(D3DXVECTOR3* pos)
{
		_pos = *pos - (_look * _distance);
}

// only be used in RPG type
void MoonCamera::adjustDistance(float unit)
{
	if(_cameraType == RPG)
	{
		_pos += _look * _distance;
		_distance += unit;
		_pos -= _look * _distance;
	}
}

// only be used in RPG type
void MoonCamera::setDistance(float distance)
{
	if(_cameraType == RPG)
	{
		_pos += _look * _distance;
		_distance = distance;
		_pos -= _look * _distance;
	}
}

MoonCamera::CameraType MoonCamera::getType(void)
{
	return _cameraType;
}

// only be used in RPG type
float MoonCamera::getDistance(void)
{
	return _distance;
}

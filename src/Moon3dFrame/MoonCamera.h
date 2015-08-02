#pragma once
#include "d3dx9math.h"

class MoonCamera
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT, RPG };

	/*MoonCamera(void);*/
	MoonCamera(CameraType ct);
	~MoonCamera(void);

	// left/right
	void strafe(float units);
	// up/down
	void fly(float units);
	// forward/backward
	void walk(float units);

	// rotate on right vector
	void pitch(float angle);
	// rotate on up vector
	void yaw(float angle);
	// rotate on look vector
	void roll(float angle);

	void getViewMatrix(D3DXMATRIX* V);
	/*void setCameraType(CameraType ct);*/
	void getPosition(D3DXVECTOR3* pos);
	void setPosition(D3DXVECTOR3* pos);
	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);
	void setLook(D3DXVECTOR3* look);
	MoonCamera::CameraType getType(void);

	// the next only be used in RPG type
	void getLookAtPos(D3DXVECTOR3* pos);
	void setLookAtPos(D3DXVECTOR3* pos);
	void adjustDistance(float unit);
	float getDistance(void);
	void setDistance(float distance);

private:
	CameraType _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
	// the distance from camera to the target character
	float _distance;
};

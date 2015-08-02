#pragma once
#include "moonparticle.h"

class MoonFirework :
	public MoonParticle
{
	D3DXVECTOR3 _tempPos;
	D3DXCOLOR _tempColor;
	void newPosition();
public:
	MoonFirework(D3DXVECTOR3* origin, int numParticles);
	~MoonFirework(void);
	void resetParticle(Attribute* attribute);
	void update(float time);
	void preRender(void);
	void postRender(void);
};

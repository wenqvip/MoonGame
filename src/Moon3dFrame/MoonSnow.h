#pragma once
#include "moonparticle.h"

class MoonSnow :
	public MoonParticle
{
public:
	MoonSnow(BoundingBox* boundingBox, float size, int numParticles);
	~MoonSnow(void);
	virtual void resetParticle(Attribute* attribute);
	virtual void update(float time);
};

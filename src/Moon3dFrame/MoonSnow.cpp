#include "MoonSnow.h"

MoonSnow::MoonSnow(BoundingBox* boundingBox, float size, int numParticles)
{
	_boundingBox		= *boundingBox;
	_size				= size;
	_vbSize				= 2048;
	_vbOffset			= 0;
	_vbBatchSize		= 512;

	for(int i = 0; i< numParticles; i++)
		addParticle();
}

MoonSnow::~MoonSnow(void)
{
}

void MoonSnow::resetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;

	GetRandomVector(&attribute->_position, &_boundingBox.min, &_boundingBox.max);

	attribute->_position.y = _boundingBox.max.y;

	attribute->_velocity.x = GetRandomFloat(0.0f,1.0f)*-3.0f;
	attribute->_velocity.y = GetRandomFloat(0.0f,1.0f)*-10.0f;
	attribute->_velocity.z = 0.0f;

	attribute->_color = D3DXCOLOR(D3DCOLOR_XRGB(255,255,255));
}

void MoonSnow::update(float time)
{
	list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * time;
		if(_boundingBox.isPointInside(i->_position) == false)
		{
			resetParticle(&(*i));
		}
	}
}

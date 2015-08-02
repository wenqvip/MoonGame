#include "MoonFirework.h"

MoonFirework::MoonFirework(D3DXVECTOR3* origin, int numParticles)
{
	_size			= 2.0f;
	_vbSize			= 2048;
	_vbOffset		= 0;
	_vbBatchSize	= 512;
	_origin			= *origin;

	_tempPos		= _origin;
	_tempColor = D3DXCOLOR(	GetRandomFloat(0.0f,1.0f),
							GetRandomFloat(0.0f,1.0f),
							GetRandomFloat(0.0f,1.0f),
							1.0f);

	for(int i = 0; i < numParticles; i++)
		addParticle();
}

MoonFirework::~MoonFirework(void)
{
}

void MoonFirework::newPosition()
{
	D3DXVECTOR3 min = _origin - D3DXVECTOR3( 100, 25, 100);
	D3DXVECTOR3 max = _origin + D3DXVECTOR3( 100, 25, 100);
	GetRandomVector(&_tempPos, &min, &max);
	_tempColor = D3DXCOLOR(	GetRandomFloat(0.0f,1.0f),
							GetRandomFloat(0.0f,1.0f),
							GetRandomFloat(0.0f,1.0f),
							1.0f);
}

void MoonFirework::resetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;
	attribute->_position = _tempPos;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);
	attribute->_velocity *= 80.0f;
	attribute->_color = _tempColor;
	attribute->_age = 0.0f;
	attribute->_lifeTime = 5.0f;
}

void MoonFirework::update(float time)
{
	if(_particles.begin()->_isAlive == false)
		newPosition();
	list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		if(i->_isAlive)
		{
			i->_position += i->_velocity * time;
			D3DXVECTOR3 temp = i->_velocity * (time / 1.5f);
			i->_velocity -= temp;
			i->_age += time;
			i->_color.r -= (time / 1.5f);
			i->_color.g -= (time / 1.5f);
			i->_color.b -= (time / 1.5f);
			if(i->_age > i->_lifeTime)
				i->_isAlive = false;
		}
		else
			resetParticle(&(*i));
	}
}

void MoonFirework::preRender(void)
{
	MoonParticle::preRender();
	_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void MoonFirework::postRender(void)
{
	MoonParticle::postRender();
	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
}

#pragma once
#include "MoonUtility.h"

struct Particle
{
	D3DXVECTOR3 _position;
	D3DCOLOR _color;
	static const DWORD FVF;
};
struct Attribute
{
	D3DXVECTOR3 _position;//粒子的位置
	D3DXVECTOR3 _velocity;//粒子的速度
	D3DXVECTOR3 _acceleration;//粒子的加速度
	float _lifeTime;//粒子的自诞生到消亡所需的时间
	float _age;//粒子的当前年龄
	D3DXCOLOR _color;//粒子的颜色
	D3DXCOLOR _colorFade;//粒子颜色如何随时间渐弱
	bool _isAlive;//粒子是否处于活动状态
};

class MoonParticle
{
public:
	MoonParticle(void);
	virtual ~MoonParticle(void);

	virtual bool init(LPDIRECT3DDEVICE9 device, char* texFileName);
	virtual void reset();

	virtual void resetParticle(Attribute* attribute) = 0;
	virtual void addParticle();
	virtual void update(float time) = 0;

	virtual void preRender();
	virtual void render();
	virtual void postRender();

	bool isEmpty();
	bool isDead();

protected:
	virtual void removeDeadParticle();

protected:
	LPDIRECT3DDEVICE9			_device;
	D3DXVECTOR3					_origin;
	BoundingBox					_boundingBox;
	float						_emitRate;
	float						_size;
	LPDIRECT3DTEXTURE9			_tex;
	LPDIRECT3DVERTEXBUFFER9		_vb;
	list<Attribute>				_particles;
	int							_maxParticles;

	// Following three data elements used for rendering the Particle-System efficiently
	DWORD _vbSize;     // size of vb
	DWORD _vbOffset;    // offset in vb to lock
	DWORD _vbBatchSize;  // number of vertices to lock starting at _vbOffset
};

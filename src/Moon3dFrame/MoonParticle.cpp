#include "MoonParticle.h"
const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

MoonParticle::MoonParticle(void)
{
}

MoonParticle::~MoonParticle(void)
{
	_particles.clear();
}

bool MoonParticle::init(LPDIRECT3DDEVICE9 device, char *texFileName)
{
	_device = device;
	HRESULT hr;
	hr = _device->CreateVertexBuffer(
			_vbSize * sizeof(Particle),
			D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
			Particle::FVF,
			D3DPOOL_DEFAULT,
			&_vb,
			0);
	if(FAILED(hr))
		return false;
	hr = Moon_CreateTexture(_device,texFileName,&_tex);
	if(FAILED(hr))
		return false;
	return true;
}

void MoonParticle::reset()
{
	list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
		resetParticle(&(*i));
	SAFE_RELEASE(_vb);
	SAFE_RELEASE(_tex);
}

void MoonParticle::addParticle()
{
	Attribute attribute;
	resetParticle(&attribute);
	_particles.push_back(attribute);
}

void MoonParticle::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSIZE,FtoDw(_size));
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));
	
	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	_device->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	_device->SetTransform(D3DTS_WORLD, &matWorld);
}

void MoonParticle::render()
{
	if(!_particles.empty())
	{
		// set render states
		preRender();

		_device->SetTexture(0, _tex);
		_device->SetFVF(Particle::FVF);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		// Start at beginning if we're at the end of the vb
		if(_vbOffset >= _vbSize)
			_vbOffset = 0;

		Particle* v = 0;
		_vb->Lock(_vbOffset*sizeof(Particle),_vbBatchSize*sizeof(Particle),
			(void**)&v,_vbOffset ? D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		// 
		// Until all particles have been rendered
		//
		list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end(); i++)
		{
			if(i->_isAlive)
			{
				//
				// copy a batch of the living particles to the 
				// next vertex buffer segment.
				//
				v->_position = i->_position;
				v->_color = i->_color;
				v++;

				numParticlesInBatch++; // increase batch counter

				// if this batch full?
				if(numParticlesInBatch == _vbBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer.
					//
					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, _vbBatchSize);

					//
					// While that batch is drawing, start filling the 
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					_vbOffset += _vbBatchSize;

					// don't offset into memory thats outside the vb's range
					// If we're at the end, start at the beginning.
					if(_vbOffset >= _vbSize)
						_vbOffset = 0;

					_vb->Lock(
						_vbOffset*sizeof(Particle),
						_vbBatchSize*sizeof(Particle),
						(void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}
		_vb->Unlock();

		// its possible that the LAST batch being filled never
		// got rendered because the condition
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied. We draw the last partially filled batch now.
		if(numParticlesInBatch)
		{
			_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, numParticlesInBatch);
		}

		// next block
		_vbOffset += _vbBatchSize;

		//
		// reset render states
		//
		postRender();
	}// end if
}

void MoonParticle::postRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	//_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool MoonParticle::isEmpty()
{
	return _particles.empty();
}

bool MoonParticle::isDead()
{
	list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		if(i->_isAlive)
			return false;
	}
	return true;
}

void MoonParticle::removeDeadParticle()
{
	list<Attribute>::iterator i;
	i = _particles.begin();
	while(i != _particles.end() )
	{
		if( i->_isAlive == false )
		{
			// erase returns the next iterator, so no need to 
			// increment to the next one ourselves.
			i = _particles.erase(i);
		}
		else
			i++;
	}
}
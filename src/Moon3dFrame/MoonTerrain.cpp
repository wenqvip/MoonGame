#include "MoonTerrain.h"

const DWORD MoonTerrain::TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX3;

MoonTerrain::MoonTerrain()
{
}

MoonTerrain::~MoonTerrain(void)
{
	SAFE_RELEASE(_vb);
	SAFE_RELEASE(_ib);
	SAFE_RELEASE(_tex[0]);
	SAFE_RELEASE(_tex[1]);
	SAFE_RELEASE(_tex[2]);
	SAFE_RELEASE(_tex[3]);
	SAFE_RELEASE(_tex[4]);
	SAFE_RELEASE(_tex[5]);
	SAFE_RELEASE(_detail);
}

bool MoonTerrain::initialize(IDirect3DDevice9* device, string TerrainName, D3DVECTOR v)
{
	_vb = NULL;
	_ib = NULL;
	_tex[0] = NULL;
	_tex[1] = NULL;
	_tex[2] = NULL;
	_tex[3] = NULL;
	_tex[4] = NULL;
	_tex[5] = NULL;

	_TerrainName = TerrainName;
	char ch[256];
	string cfgpath = TERRAIN_DIR("Terrain.cfg");

	GetPrivateProfileString(TerrainName.c_str(),"filename",NULL,ch,256,cfgpath.c_str());
	if(ch[0] == NULL)
	{
		LogError("MoonTerrain::initialize() Cannot find Terrain's configure data.");
		return false;
	}
	else
		_filepath = TERRAIN_DIR(ch);

	GetPrivateProfileString(TerrainName.c_str(),"numVertsPerRow","128",ch,256,cfgpath.c_str());
	_numVertsPerRow = CharToInt(ch);

	GetPrivateProfileString(TerrainName.c_str(),"numVertsPerCol","128",ch,256,cfgpath.c_str());
	_numVertsPerCol = CharToInt(ch);

	GetPrivateProfileString(TerrainName.c_str(),"cellSpacing","10",ch,256,cfgpath.c_str());
	_cellSpacing = CharToInt(ch);

	GetPrivateProfileString(TerrainName.c_str(),"heightScale","1",ch,256,cfgpath.c_str());
	_heightScale = CharToFloat(ch);
	
	GetPrivateProfileString(TerrainName.c_str(),"texture1","",ch,256,cfgpath.c_str());
	_strTexture1 = ch;
	GetPrivateProfileString(TerrainName.c_str(),"texture2","",ch,256,cfgpath.c_str());
	_strTexture2 = ch;
	GetPrivateProfileString(TerrainName.c_str(),"texture3","",ch,256,cfgpath.c_str());
	_strTexture3 = ch;
	GetPrivateProfileString(TerrainName.c_str(),"detail","",ch,256,cfgpath.c_str());
	_strDetail = ch;

	_device				= device;

	_numCellsPerRow		= _numVertsPerRow - 1;
	_numCellsPerCol		= _numVertsPerCol - 1;
	_width				= _numCellsPerRow * _cellSpacing;
	_depth				= _numCellsPerCol * _cellSpacing;
	_numVertices		= _numVertsPerRow * _numVertsPerCol;
	_numTriangles		= _numCellsPerRow * _numCellsPerCol * 2;

	readRawFile(_filepath);
	computeVertices();
	computeIndices();
	
	string strTemp;
	if(!_strTexture1.empty())
	{
		strTemp = _TerrainName + "_tex1.jpg";
		loadTexture(TERRAIN_DIR(strTemp.c_str()), 0);
		lightTerrain(&D3DXVECTOR3(-v.x, -v.y, -v.z),0);
		loadTexture(TERRAIN_DIR(_strTexture1.c_str()), 1);
	}
	else
	{
		genTexture();
		lightTerrain(&D3DXVECTOR3(-v.x, -v.y, -v.z),0);
	}

	if(!_strTexture2.empty())
	{
		strTemp = _TerrainName + "_tex2.jpg";
		loadTexture(TERRAIN_DIR(strTemp.c_str()), 2);
		lightTerrain(&D3DXVECTOR3(-v.x, -v.y, -v.z),2);
		loadTexture(TERRAIN_DIR(_strTexture2.c_str()), 3);
	}
	if(!_strTexture3.empty())
	{
		strTemp = _TerrainName + "_tex3.jpg";
		loadTexture(TERRAIN_DIR(strTemp.c_str()), 4);
		lightTerrain(&D3DXVECTOR3(-v.x, -v.y, -v.z),4);
		loadTexture(TERRAIN_DIR(_strTexture3.c_str()), 5);
	}
	if(!_strDetail.empty())
	{
		D3DXCreateTextureFromFile(_device, TERRAIN_DIR(_strDetail.c_str()), &_detail);
	}
	else
		_detail = NULL;

	return true;
}

bool MoonTerrain::readRawFile(string filename)
{
	// Restriction: RAW file dimensions must be >= to the dimensions of the terrain.
	// That is a 128x128 RAW file can only be used with a terrain constructed with
	// at most 128x128 vertices.

	// A height for each vertex
	vector<BYTE> in ( _numVertices );
	ifstream inFile(filename.c_str(), ios_base::binary);

	if( inFile.eof() )
	{
		LogWarning("MoonTerrain::readRawFile() donot find the raw file!");
		return false;
	}
	inFile.read(
		(char*) &in[0],		// buffer
		in.size());			// number of bytes to read into buffter

	inFile.close();

	// copy BYTE vector to int vector
	_heightmap.resize( _numVertices );

	for(int i = 0; i < in.size(); i++)
		_heightmap[i] = in[i] * _heightScale;

	return true;
}

bool MoonTerrain::computeVertices(void)
{
	HRESULT hr = 0;

	hr = _device->CreateVertexBuffer(
		_numVertices * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&_vb,
		0);

	if(FAILED(hr))
	{
		LogWarning("MoonTerrain::computeVertices() CreateVertexBuffer failed, error code: 0x%X.",
			hr);
		return false;
	}

	// coordinates to start generating vertices at 
	int startX = -_width / 2;
	int startZ = _depth / 2;

	// coordinates to end generating vertices at 
	int endX = _width / 2;
	int endZ = -_depth / 2;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	float uCoordIncrementSize = 1.0f / (float)_numCellsPerRow;
	float vCoordIncrementSize = 1.0f / (float)_numCellsPerCol;

	TerrainVertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int i = 0;
	for(int z = startZ; z >= endZ-1; z -= _cellSpacing)
	{
		int j = 0;
		for(int x = startX; x <= endX+1; x += _cellSpacing)
		{
			// compute the correct index into the vertex buffter and heightmap
			// based on where we are in the nested loop.
			int index = i * _numVertsPerRow + j;

			v[index] = TerrainVertex(
				(float)x,
				(float)_heightmap[index],
				(float)z,
				(float)j * uCoordIncrementSize,
				(float)i * vCoordIncrementSize,
				(float)j * 0.25f,
				(float)i * 0.25f,
				(float)j * 2.0f,
				(float)i * 2.0f);

			j++;	// next column
		}
		i++;	// next row
	}

	_vb->Unlock();

	return true;
}

bool MoonTerrain::computeIndices(void)
{
	HRESULT hr = 0;

	hr = _device->CreateIndexBuffer(
		_numTriangles * 3 * sizeof(WORD),	// 3 indices per triangle
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib,
		0);

	if(FAILED(hr))
	{
		LogWarning("MoonTerrain::computeIndices() CreateIndexBuffer failed, error code: 0x%X.",
			hr);
		return false;
	}

	WORD* indices = 0;
	_ib->Lock(0, 0, (void**)&indices, 0);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0;

	// loop through and compute the triangles of each quad
	for(int i = 0; i < _numCellsPerCol; i++)
	{
		for(int j = 0; j < _numCellsPerRow; j++)
		{
			indices[baseIndex]			= i * _numVertsPerRow + j;
			indices[baseIndex + 1]		= i * _numVertsPerRow + j + 1;
			indices[baseIndex + 2]		= (i+1) * _numVertsPerRow + j;

			indices[baseIndex + 3]		= (i+1) * _numVertsPerRow + j;
			indices[baseIndex + 4]		= i * _numVertsPerRow + j + 1;
			indices[baseIndex + 5]		= (i+1) * _numVertsPerRow + j + 1;

			// next quad
			baseIndex += 6;
		}
	}

	_ib->Unlock();

	return true;
}

bool MoonTerrain::loadTexture(const char* filename, UINT n)
{
	HRESULT hr = 0;
	string str;

	hr = D3DXCreateTextureFromFile(
		_device,
		filename,
		&_tex[n]);

	if(FAILED(hr))
	{
		str = "Cannot load texture: ";
		str += filename;
		LogWarning(const_cast<char*>(str.c_str()));
		return false;
	}
	str = "Load texture: ";
	str += filename;
	LogInfo(const_cast<char*>(str.c_str()));

	return true;
}

bool MoonTerrain::genTexture()
{
	// Method files the top surface of a texture procedurally. Then
	// lights the top surface. Finally, it fills the other mipmap
	// surfaces based on the top surface data using D3DXFilterTexture.

	HRESULT hr = 0;

	// texel for each quad cell
	int texWidth = _numCellsPerRow;
	int texHeight = _numCellsPerCol;

	// create an empty texture
	hr = D3DXCreateTexture(
		_device,
		texWidth, texHeight, 
		0, // Create a complete mipmap chain
		0, // usage
		D3DFMT_X8R8G8B8, // 32 bit XRGB format
		D3DPOOL_MANAGED, &_tex[0]);

	if(FAILED(hr))
	{
		LogWarning("MoonTerrain::genTexture() Cannot create texture, error code 0x%X.",hr);
		return false;
	}

	D3DSURFACE_DESC textureDesc;
	_tex[0]->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;

	D3DLOCKED_RECT lockedRect;
	_tex[0]->LockRect(0/*lock top surface*/, &lockedRect,
		0 /* lock entire tex*/, 0/*flags*/);

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(int i = 0; i < textureDesc.Height; i++)
	{
		for(int j = 0; j < textureDesc.Width; j++)
		{
			DWORD c;

			//get height of upper left vertex of quad.
			float height = (float)getHeightmapEntry(i, j) / _heightScale;

			if( height < 42.5f)				c = D3DCOLOR_XRGB(0,40,128);
			else if( height < 64.5f && height > 42.5f)		c = D3DCOLOR_XRGB(128, 128, 0);
			else if( height < 127.5f && height > 64.5f)		c = D3DCOLOR_XRGB(0, 192, 0);
			else if( height < 150.5f && height > 127.5f)		c = D3DCOLOR_XRGB(0, 100, 0);
			else if( height < 212.5f && height > 150.5f)		c = D3DCOLOR_XRGB(72,64,0);
			else							c = D3DCOLOR_XRGB(255, 255, 255);

			// fill locked data, note we divide the pitch by four because the 
			// pitch is given in byte and there are 4 bytes per DWORD.
			imageData[i * lockedRect.Pitch / 4 + j] = c;
		}
	}

	_tex[0]->UnlockRect(0);

	/*if(!lightTerrain(directionToLight, 0))
	{
		::MessageBox(0, "lightTerrain() - FAILED", 0, 0);
		return false;
	}*/

	//hr = D3DXFilterTexture(
	//	_tex,
	//	0, // default palette
	//	0, // use top level as source level
	//	D3DX_DEFAULT); // default filter

	//if(FAILED(hr))
	//{
	//	::MessageBox(0, "D3DXFilterTexture() - FAILED", 0 ,0);
	//	return false;
	//}

	return true;
}

int MoonTerrain::getHeightmapEntry(int row, int col)
{
	return _heightmap[row * _numVertsPerRow + col];
}

void MoonTerrain::setHeightmapEntry(int row, int col, int value)
{
	_heightmap[row * _numVertsPerRow + col] = value;
}

bool MoonTerrain::lightTerrain(D3DXVECTOR3 * directionToLight, UINT n)
{
	D3DLOCKED_RECT lockedRect;

	_tex[n]->LockRect(0, &lockedRect, 0, 0);

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(int i = 0; i < _numCellsPerRow; i++)
	{
		for(int j = 0; j < _numCellsPerCol; j++)
		{
			// index into texture, note we use the pitch and divide by four since
			// the pitch is given in bytes and there are 4 bytes per DWORD.
			int index = i * lockedRect.Pitch / 4 + j;

			//get current color of quad
			D3DXCOLOR c( imageData[index] );

			// shade current quad
			c *= computeShade(i, j, directionToLight);

			//save shaded color
			imageData[index] = (D3DCOLOR)c;
		}
	}
	_tex[n]->UnlockRect(0);
	return true;
}

float MoonTerrain::computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight)
{
	// get heights of three vertices on the quad
	float heightA = getHeightmapEntry(cellRow, cellCol);
	float heightB = getHeightmapEntry(cellRow, cellCol+1);
	float heightC = getHeightmapEntry(cellRow+1, cellCol);

	// build two vector on the quad
	D3DXVECTOR3 u(_cellSpacing, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f, heightC - heightA, -_cellSpacing);

	// find the normal by taking the cross product of two vector on the quad.
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	float cosine = D3DXVec3Dot(&n, directionToLight);

	if(cosine < 0.0f)
		cosine = 0.0f;

	return cosine;
}

float MoonTerrain::getHeight(float x, float z)
{
	if(x < (-_width / 2.0f + 1) 
			|| x > (_width / 2.0f - 1) 
			|| z < (-_depth / 2.0f + 1) 
			|| z > (_depth / 2.0f - 1) )
		return 0.0f;
	// Translate on xz-plane by the transformation that takes 
	// the terrain START point to the origin
	x = ((float)_width / 2.0f) + x;
	z = ((float)_depth / 2.0f) - z;

	// Scale down by the transformation that makes the cellspacing equal
	// to one. This is given by 1 / cellspacing since; 
	// cellspacing * 1 / cellspacing = 1.
	x /= (float)_cellSpacing;
	z /= (float)_cellSpacing;

	float col = ::floorf(x);
	float row = ::floorf(z);

	// get the heights of the quad we're in:
	// 
		// A   B
		// *---*
		// | / |
		// *---*
		// C   D
	float A = getHeightmapEntry(row, col);
	float B = getHeightmapEntry(row, col+1);
	float C = getHeightmapEntry(row+1, col);
	float D = getHeightmapEntry(row+1, col+1);

	float dx = x - col;
	float dz = z - row;

	float height = 0.0f;
	if(dz < 1.0f - dx)	// upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		height = A + uy * dx + vy * dz;
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		height = D + uy * (1.0f - dx) + vy * (1.0f - dz);
	}

	return height;
}

int MoonTerrain::draw(void)
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	_device->SetTransform(D3DTS_WORLD, &matWorld);
	_device->SetStreamSource(0, _vb, 0, sizeof(TerrainVertex));
	_device->SetFVF(TerrainVertex::FVF);

	_device->SetTexture(0, _tex[0]);
	_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	/*_device->SetRenderState(D3DRS_LIGHTING, FALSE);*/

	if(_tex[1])
	{
		_device->SetTexture(1, _tex[1]);
		_device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		_device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		_device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	}

	if(_tex[2])
	{
		_device->SetTexture(2, _tex[2]);
		_device->SetTextureStageState(2, D3DTSS_COLORARG1,D3DTA_TEXTURE );
		_device->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		_device->SetTextureStageState(2, D3DTSS_RESULTARG, D3DTA_TEMP);
		_device->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 0);
	}

	if(_tex[3])
	{
		_device->SetTexture(3, _tex[3]);
		_device->SetTextureStageState(3, D3DTSS_COLORARG1,D3DTA_TEMP);
		_device->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		_device->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
		_device->SetTextureStageState(3, D3DTSS_COLORARG0, D3DTA_CURRENT);
		_device->SetTextureStageState(3, D3DTSS_RESULTARG, D3DTA_CURRENT);
		_device->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 1);
	}

	if(_tex[4])
	{
		_device->SetTexture(4, _tex[4]);
		_device->SetTextureStageState(4, D3DTSS_COLORARG1,D3DTA_TEXTURE );
		_device->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		_device->SetTextureStageState(4, D3DTSS_RESULTARG, D3DTA_TEMP);
		_device->SetTextureStageState(4, D3DTSS_TEXCOORDINDEX, 0);
	}

	if(_tex[5])
	{
		_device->SetTexture(5, _tex[5]);
		_device->SetTextureStageState(5, D3DTSS_COLORARG1,D3DTA_TEMP);
		_device->SetTextureStageState(5, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		_device->SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
		_device->SetTextureStageState(5, D3DTSS_COLORARG0, D3DTA_CURRENT);
		_device->SetTextureStageState(5, D3DTSS_RESULTARG, D3DTA_CURRENT);
		_device->SetTextureStageState(5, D3DTSS_TEXCOORDINDEX, 1);
	}
	if(_detail)
	{
		_device->SetTexture(6, _detail);
		_device->SetTextureStageState(6, D3DTSS_TEXCOORDINDEX, 2);
		_device->SetTextureStageState(6, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_device->SetTextureStageState(6, D3DTSS_COLORARG2, D3DTA_CURRENT);
		_device->SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X);
		_device->SetTextureStageState(6, D3DTSS_RESULTARG, D3DTA_CURRENT);
	}

	_device->SetIndices(_ib);

	_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _numVertices,
		0 , _numTriangles);

	/*_device->SetRenderState(D3DRS_LIGHTING, TRUE);*/
	_device->SetTexture(1, NULL);
	_device->SetTexture(2, NULL);
	_device->SetTexture(3, NULL);
	_device->SetTexture(4, NULL);
	_device->SetTexture(5, NULL);
	_device->SetTexture(6, NULL);

	return _numTriangles;
}

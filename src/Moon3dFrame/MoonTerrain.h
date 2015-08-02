#pragma once
#include "MoonUtility.h"
#include "MoonSystemLog.h"

class MoonTerrain : public MoonSystemLog
{
	struct TerrainVertex
	{
		TerrainVertex(){}
		TerrainVertex(float x, float y, float z, float u1, float v1, float u2, float v2, float u3, float v3)
		{
			_x = x; _y = y; _z = z; _u1 = u1; _v1 = v1; _u2 = u2; _v2 = v2; _u3 = u3; _v3 = v3;
		}
		float _x, _y, _z;
		float _u1, _v1;
		float _u2, _v2;
		float _u3, _v3;

		static const DWORD FVF;
	};
public:
	MoonTerrain();
	~MoonTerrain(void);
private:
	IDirect3DDevice9*			_device;
	IDirect3DVertexBuffer9*		_vb;
	IDirect3DIndexBuffer9*		_ib;
	IDirect3DTexture9*			_tex[6];
	IDirect3DTexture9*			_detail;
	string	_filepath;
	string _TerrainName;
	string _strTexture1;
	string _strTexture2;
	string _strTexture3;
	string _strDetail;
	int		_numVertsPerRow;
	int		_numVertsPerCol;
	int		_cellSpacing;

	int		_numCellsPerRow;
	int		_numCellsPerCol;
	int		_width;
	int		_depth;
	int		_numVertices;
	int		_numTriangles;

	float	_heightScale;
	vector<int> _heightmap;

	bool	readRawFile(string filename);
	bool	computeVertices(void);
	bool	computeIndices(void);
	float	computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);
	bool	lightTerrain(D3DXVECTOR3 * directionToLight, UINT n);

public:
	bool	initialize(IDirect3DDevice9* device, string TerrainName, D3DVECTOR v);
	bool	loadTexture(const char* filename, UINT n);
	bool	genTexture();
	int		getHeightmapEntry(int row, int col);
	void	setHeightmapEntry(int row, int col, int value);
	float	getHeight(float x, float z);
	int		draw(void);
	int		getwidth()		{return	_width;}
	int		getdepth()		{return _depth;}
};

// Cuboid.h: interface for the CCuboid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_)
#define AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "MoonUtility.h"

//Define a FVF for our cubiods
#define CUBOID_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CCuboid  
{
private:
	//Define a custom vertex for our cubiods
	struct CUBOID_CUSTOMVERTEX
	{
		FLOAT x, y, z;		//Position of vertex in 3D space
		FLOAT nx, ny, nz;	//Lighting Normal
		FLOAT tu, tv;		//Texture coordinates
	};

public:
	bool SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower);
	bool SetTexture(const char* szTextureFilePath);
	bool SetPosition(float x, float y, float z);
	bool SetSize(float rWidth, float rHeight, float rDepth);
	// 设置正文体的旋转轴
	void SetRotationAxis(D3DXVECTOR3* V);
	bool Render();
	CCuboid(LPDIRECT3DDEVICE9 pD3DDevice);
	virtual ~CCuboid();
	/*void GetPosition(D3DXVECTOR3* pos);*/

private:
	D3DVECTOR GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);
	bool UpdateVertices();
	HRESULT CreateVertexBuffer();
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9 m_pTexture;
	D3DMATERIAL9 m_matMaterial;
		
	float m_rWidth;
	float m_rHeight;
	float m_rDepth;
	float m_rX;
	float m_rY;
	float m_rZ;
	D3DXVECTOR3 m_vRotAxis;
};

#endif // !defined(AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_)

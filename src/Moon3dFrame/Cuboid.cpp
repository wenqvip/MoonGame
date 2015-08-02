// Cuboid.cpp: implementation of the CCuboid class.
//
//////////////////////////////////////////////////////////////////////

#include "Cuboid.h"
#include <stdio.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCuboid::CCuboid(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
	m_pVertexBuffer = NULL;
	m_pTexture = NULL;


	//Set a default size and position
	m_rWidth = 10.0;
	m_rHeight = 10.0;
	m_rDepth = 10.0;
	m_rX = 0.0;
	m_rY = 0.0;
	m_rZ = 0.0;
	m_vRotAxis.x=0;
	m_vRotAxis.y=0;
	m_vRotAxis.z=0;


	//Set material default values (R, G, B, A)
	D3DCOLORVALUE rgbaDiffuse  = {1.0, 1.0, 1.0, 0.5,};
	D3DCOLORVALUE rgbaAmbient  = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaSpecular = {0.0, 0.0, 0.0, 0.0,};
	D3DCOLORVALUE rgbaEmissive = {0.0, 0.0, 0.0, 0.0,};
	
	SetMaterial(rgbaDiffuse, rgbaAmbient, rgbaSpecular, rgbaEmissive, 0);


	//Initialize Vertex Buffer
    if(SUCCEEDED(CreateVertexBuffer()))
	{
		UpdateVertices();
	}
}

CCuboid::~CCuboid()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVertexBuffer);
}

bool CCuboid::Render()
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer,0, sizeof(CUBOID_CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(CUBOID_D3DFVF_CUSTOMVERTEX);

	if(m_pTexture != NULL)
	{
		//A texture has been set. We want our texture to be shaded based
		//on the current light levels, so used D3DTOP_MODULATE.
		m_pD3DDevice->SetTexture(0, m_pTexture);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
	else
	{
		//No texture has been set
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	if(FAILED(m_pD3DDevice->SetMaterial(&m_matMaterial)))
	{
		//Set material failed
		OutputDebugString("SetMaterial Failed!\n");
	}

	if(m_vRotAxis == D3DXVECTOR3(0,0,0))
	{
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
	}
	else
	{
		float time = timeGetTime()/800.0f;
		D3DXVECTOR3 V(m_rX,m_rY,m_rZ);
		D3DXMATRIX M, M1, oldM;
		m_pD3DDevice->GetTransform(D3DTS_WORLD, &oldM);
		D3DXMatrixIdentity(&M);
		D3DXMatrixRotationAxis(&M1, &m_vRotAxis, time);
		D3DXVec3TransformCoord(&V,&V,&M1);
		D3DXMatrixMultiply(&M, &M, &M1);
		D3DXMatrixTranslation(&M1, m_rX-V.x, m_rY-V.y, m_rZ-V.z);
		D3DXMatrixMultiply(&M, &M, &M1);
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &M);

		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

		m_pD3DDevice->SetTransform(D3DTS_WORLD, &oldM);
	}

	return true;
}

HRESULT CCuboid::CreateVertexBuffer()
{
    //Create the vertex buffer from our device.
    if(FAILED(m_pD3DDevice->CreateVertexBuffer(36 * sizeof(CUBOID_CUSTOMVERTEX),
                                               0, CUBOID_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_MANAGED, &m_pVertexBuffer,0)))
    {
        return E_FAIL;
    }

    return S_OK;
}

D3DVECTOR CCuboid::GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3)
{
	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 v1;
	D3DXVECTOR3 v2;

	D3DXVec3Subtract(&v1, vVertex2, vVertex1);
	D3DXVec3Subtract(&v2, vVertex3, vVertex1);

	D3DXVec3Cross(&vNormal, &v1, &v2);

	D3DXVec3Normalize(&vNormal, &vNormal);

	return vNormal;
}

bool CCuboid::UpdateVertices()
{
	VOID* pVertices;
	D3DVECTOR vNormal;

	CUBOID_CUSTOMVERTEX cvVertices[] =
	{	
		//Top Face
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 0
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 1
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 2
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 3
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 4
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 5

		//Face 1
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 6
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 7
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 8
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 9
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 10
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 11


		//Face 2
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 12
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 13
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 14
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 15
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 16
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 17
		

		//Face 3
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 18
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 19
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 20
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 21
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 22
		{m_rX + (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 23


		//Face 4
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 24
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 25
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 26
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 27
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 28
		{m_rX - (m_rWidth / 2), m_rY + (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 29


		//Bottom Face
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 30
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 31
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 32
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 33
		{m_rX - (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 34
		{m_rX + (m_rWidth / 2), m_rY - (m_rHeight / 2), m_rZ + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 35
	};



	//Set all vertex normals
	int i;
	CHAR DEBUG[255];

	for(i = 0; i < 36; i += 3)
	{
		vNormal = GetTriangeNormal(&D3DXVECTOR3(cvVertices[i].x, cvVertices[i].y, cvVertices[i].z), &D3DXVECTOR3(cvVertices[i + 1].x, cvVertices[i + 1].y, cvVertices[i + 1].z), &D3DXVECTOR3(cvVertices[i + 2].x, cvVertices[i + 2].y, cvVertices[i + 2].z));

		cvVertices[i].nx = vNormal.x;
		cvVertices[i].ny = vNormal.y;
		cvVertices[i].nz = vNormal.z;

		cvVertices[i + 1].nx = vNormal.x;
		cvVertices[i + 1].ny = vNormal.y;
		cvVertices[i + 1].nz = vNormal.z;

		cvVertices[i + 2].nx = vNormal.x;
		cvVertices[i + 2].ny = vNormal.y;
		cvVertices[i + 2].nz = vNormal.z;

		sprintf(DEBUG, "Vertices %d - %d: x = %f, y = %f, z = %f\n", i, i + 2, vNormal.x, vNormal.y, vNormal.z);
		OutputDebugString(DEBUG);
	}


	//Get a pointer to the vertex buffer vertices and lock the vertex buffer
    if(FAILED(m_pVertexBuffer->Lock(0, sizeof(cvVertices), (void**)&pVertices, 0)))
    {
        return false;
    }

    //Copy our stored vertices values into the vertex buffer
    memcpy(pVertices, cvVertices, sizeof(cvVertices));

    //Unlock the vertex buffer
    m_pVertexBuffer->Unlock();

	return true;
}

bool CCuboid::SetSize(float rWidth, float rHeight, float rDepth)
{
	m_rWidth = rWidth;
	m_rHeight = rHeight;
	m_rDepth = rDepth;

	UpdateVertices();

	return true;
}

bool CCuboid::SetPosition(float x, float y, float z)
{
	m_rX = x;
	m_rY = y;
	m_rZ = z;

	UpdateVertices();

	return true;
}

//void CCuboid::GetPosition(D3DXVECTOR3* pos)
//{
//	pos->x = m_rX;
//	pos->y = m_rY;
//	pos->z = m_rZ;
//}

bool CCuboid::SetTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, szTextureFilePath, &m_pTexture)))
	{
		return false;
	}

	UpdateVertices();

	return true;
}

bool CCuboid::SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower)
{
	//Set the RGBA for diffuse light reflected from this material. 
	m_matMaterial.Diffuse = rgbaDiffuse; 

	//Set the RGBA for ambient light reflected from this material. 
	m_matMaterial.Ambient = rgbaAmbient; 

	//Set the color and sharpness of specular highlights for the material. 
	m_matMaterial.Specular = rgbaSpecular; 
	m_matMaterial.Power = rPower;

	//Set the RGBA for light emitted from this material. 
	m_matMaterial.Emissive = rgbaEmissive;

	return true;
}

// 设置正文体的旋转轴
void CCuboid::SetRotationAxis(D3DXVECTOR3* V)
{
	m_vRotAxis.x = V->x;
	m_vRotAxis.y = V->y;
	m_vRotAxis.z = V->z;
}

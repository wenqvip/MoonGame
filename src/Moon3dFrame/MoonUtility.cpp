
#include "MoonUtility.h"

AttachPathFile::AttachPathFile(const char* path, const char* file)
{
	/*str.clear();*/
	str = path;
	str += file;
}
AttachPathFile::~AttachPathFile()
{
}

const char* AttachPathFile::Return()
{
	return str.c_str();
}

D3DFORMAT StringToFmt(char* str)
{
	D3DFORMAT format;
	if(str[7]=='A'&&str[8]=='8')
		format=D3DFMT_A8R8G8B8;
	else if(str[7]=='X'&&str[8]=='8')
		format=D3DFMT_X8R8G8B8;
	else if(str[7]=='R'&&str[8]=='5')
		format=D3DFMT_R5G6B5;
	else if(str[7]=='X'&&str[8]=='1')
		format=D3DFMT_X1R5G5B5;
	else if(str[7]=='A'&&str[8]=='1')
		format=D3DFMT_A1R5G5B5;
	else if(str[7]=='A'&&str[8]=='2')
		format=D3DFMT_A2R10G10B10;
	else
		format=D3DFMT_X8R8G8B8;
	return format;
}

D3DDEVTYPE StringToDevtype(char *str)
{
	if(str[11]=='H'&&str[12]=='A')
		return D3DDEVTYPE_HAL;
	else if(str[11]=='R'&&str[12]=='E')
		return D3DDEVTYPE_REF;
	else if(str[11]=='S'&&str[12]=='W')
		return D3DDEVTYPE_SW;
	else
		return D3DDEVTYPE_FORCE_DWORD;
}

UINT StringToUINT(char* str)
{
	UINT n=0;
	if(str[0]=='S'&&str[1]=='O')
		n=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if(str[0]=='H'&&str[1]=='A')
		n=D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if(str[0]=='P'&&str[1]=='U')
		n=D3DCREATE_PUREDEVICE;
	else if(str[0]=='M'&&str[1]=='I')
		n=D3DCREATE_MIXED_VERTEXPROCESSING;
	else if(str[20]=='D'&&str[21]=='E')
		n=D3DPRESENT_INTERVAL_DEFAULT;
	else if(str[20]=='O'&&str[21]=='N')
		n=D3DPRESENT_INTERVAL_ONE;
	else if(str[20]=='T'&&str[21]=='W')
		n=D3DPRESENT_INTERVAL_TWO;
	else if(str[20]=='T'&&str[21]=='H')
		n=D3DPRESENT_INTERVAL_THREE;
	else if(str[20]=='F'&&str[21]=='O')
		n=D3DPRESENT_INTERVAL_FOUR;
	else if(str[20]=='I'&&str[21]=='M')
		n=D3DPRESENT_INTERVAL_IMMEDIATE;
	return n;
}

bool isNum(char ch)
{
	if(ch>47&&ch<58)
		return true;
	return false;
}

HRESULT Moon_CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strTexture,
                               LPDIRECT3DTEXTURE9* ppTexture, D3DFORMAT d3dFormat )
{
    // Create the texture using D3DX
    return D3DXCreateTextureFromFileEx( pd3dDevice, strTexture, 
                D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, d3dFormat, 
                D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, ppTexture );
}

VOID Moon_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
                        FLOAT x, FLOAT y, FLOAT z )
{
    D3DXVECTOR3 vecLightDirUnnormalized(x, y, z);
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type        = ltType;
    light.Diffuse.r   = 1.0f;
    light.Diffuse.g   = 1.0f;
    light.Diffuse.b   = 1.0f;
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized );
    light.Position.x   = x;
    light.Position.y   = y;
    light.Position.z   = z;
    light.Range        = 1000.0f;
}

VOID Moon_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r, FLOAT g, FLOAT b,
                           FLOAT a)
{
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = r;
    mtrl.Diffuse.g = mtrl.Ambient.g = g;
    mtrl.Diffuse.b = mtrl.Ambient.b = b;
    mtrl.Diffuse.a = mtrl.Ambient.a = a;
}

int CharToInt(const char* ch)
{
	int value = 0;
	int flag = 0;
	for(int i = 0; ; i++)
	{
		if(isNum(ch[i]))
		{
			if(flag == 0)
				flag = 1;
			value = value * 10 + (int)(ch[i]-48);
		}
		else if(ch[i]=='-'&&flag==0)
			flag = -1;
		else
			break;
	}
	return value * flag;
}

float CharToFloat(const char * ch)
{
	float value = 0.0f;
	int j = -1;
	int i;
	int flag = 0;
	for(i = 0; ; i++)
	{
		if(isNum(ch[i+j+1]))
		{
			if(flag==0)
				flag=1;
			value = value * 10 + (float)(ch[i+j+1]-48);
		}
		else if (ch[i+j+1] == '.'&&j==-1)
		{
			if(flag==0)
				flag=1;
			j = i-1; i = 0;
		}
		else if(ch[i+j+1] =='-'&&flag==0)
		{
			flag=-1;
		}
		else
		{
			if(j==-1)
				i = 1;
			break;
		}
	}
	return flag * value / (float)pow(10.0f,i-1);
}

D3DXVECTOR3 CharToVector3(const char * ch)
{
	float f[3] = {0.0f,0.0f,0.0f};
	int n=0, m=-1;
	for(int i=0; ; i++)
	{
		if(ch[i]=='\0'||n>2)
			break;
		if(isNum(ch[i])||ch[i]=='-'||ch[i]=='.')
		{
			if(m!=n)
			{
				f[n]=CharToFloat(&ch[i]);
				m=n;
			}
		}
		else
			if(n==m)
				n++;
	}
	return D3DXVECTOR3(f[0],f[1],f[2]);
}

D3DXVECTOR3 GetTriangeNormal(D3DXVECTOR3* vVertex1,
									D3DXVECTOR3* vVertex2, 
									D3DXVECTOR3* vVertex3)
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

void MoonShowCursor(bool show)
{
	static bool bShowCursor = true;
	if(show == true && bShowCursor == false)
	{
		bShowCursor = true;
		::ShowCursor(true);
	}
	else if(show == false && bShowCursor == true)
	{
		bShowCursor = false;
		::ShowCursor(false);
	}
}

float GetRandomFloat(float lowBound, float highBound)
{
	if(lowBound >= highBound) //bad input
		return lowBound;

	float f = (rand() % 10000) * 0.0001f;
	return (f* (highBound-lowBound)) +lowBound;
}

void GetRandomVector(D3DXVECTOR3* out,D3DXVECTOR3* min,D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x,max->x);
	out->y = GetRandomFloat(min->y,max->y);
	out->z = GetRandomFloat(min->z,max->z);
}
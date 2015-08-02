#pragma once

#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"strmiids.lib")

//#pragma comment(lib,"..\\..\\bin\\RenderSetup.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <d3dx9.h>
#include <tlhelp32.h>
#include <windows.h>
#include <shlwapi.h>
#include <WindowsX.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <fstream>
#include <dinput.h>
#include <dsound.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <dshow.h>
#include "resource.h"
using namespace std;

#define KEYDOWN(name, key) (name[key] & 0x80) 
#define MOUSEBUTTONDOWN(key) (key & 0x80)

//Mouse buttons
#define MOUSEBUTTON_LEFT 0
#define MOUSEBUTTON_RIGHT 1
#define MOUSEBUTTON_MIDDLE 2

//从RenderSetup.dll中引入RunDlg()运行一个对话框，它将询问有关设置项
extern "C" __declspec(dllimport) int RunDlg(void);


//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define V_RETURN(p) {if(FAILED(p)) return false;}

class BoundingBox
{
public:
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
	BoundingBox(void) {};
	BoundingBox(D3DXVECTOR3* vec1, D3DXVECTOR3* vec2)
	{
		min = *vec1;
		max = *vec2;
	};
	BoundingBox(float x, float y, float z, float u, float v, float w)
	{
		min.x = x;   min.y = y;   min.z = z;
		max.x = u;   max.y = v;   max.z = w;
	};
	BoundingBox(BoundingBox &bb)
	{
		min = bb.min;
		max = bb.max;
	};
	~BoundingBox() {};
	bool isPointInside(D3DXVECTOR3 pos)
	{
		if(pos.x>min.x&&pos.y>min.y&&pos.z>min.z
						&&pos.x<max.x&&pos.y<max.y&&pos.z<max.z)
			return true;
		return false;
	};
};

enum AppState
{
	MAINMENU		= 0,
	INSCENE,
	LOADING,
	GAMETOMENU,
	ABOUT,

	NOSTATE			= 999
};

class AttachPathFile
{
public:
	AttachPathFile(const char* path, const char* file);
	~AttachPathFile();
	const char * Return();
private:
	string str;
};

// filename should add quotes.
#define IMAGE_DIR(filename)			AttachPathFile("..\\media\\images\\", filename).Return()
#define MODEL_DIR(filename)			AttachPathFile("..\\media\\models\\", filename).Return()
#define GUI_DIR(filename)			AttachPathFile("..\\media\\gui\\", filename).Return()
#define TERRAIN_DIR(filename)		AttachPathFile("..\\media\\terrain\\", filename).Return()
#define SCENE_DIR(filename)			AttachPathFile("..\\media\\scenes\\", filename).Return()


//Input string like "D3DFMT_X8R8G8B8" and return a D3DFORMAT value
D3DFORMAT StringToFmt(char* str);

//
D3DDEVTYPE StringToDevtype(char * str);

//
UINT StringToUINT(char* str);

// read a int from a char*
int CharToInt(const char * ch);

// read a float from a char*
float CharToFloat(const char * ch);

// read a vector3 from a char*
D3DXVECTOR3 CharToVector3(const char * ch);

//To judge whether a char is a number or not
bool isNum(char ch);

//Help to Create a Texture from file
HRESULT Moon_CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strTexture,
                               LPDIRECT3DTEXTURE9* ppTexture,
                               D3DFORMAT d3dFormat = D3DFMT_UNKNOWN );

void Moon_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
                        FLOAT x, FLOAT y, FLOAT z );

void Moon_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r, FLOAT g, FLOAT b,
                           FLOAT a =1.0f);

D3DXVECTOR3 GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);

void MoonShowCursor(bool show);

float GetRandomFloat(float lowBound, float highBound);

void GetRandomVector(D3DXVECTOR3* out,D3DXVECTOR3* min,D3DXVECTOR3* max);

inline DWORD FtoDw(float f)
{
	return *((DWORD*)&f);
};
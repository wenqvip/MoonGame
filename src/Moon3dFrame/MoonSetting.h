#pragma once
#include <d3d9.h>
#include "MoonUtility.h"

class MoonSetting
{
public:
	MoonSetting(void);
	~MoonSetting(void);
	void GetSetting();

	UINT m_nAdapter;
	D3DDEVTYPE m_DeviceType;
	bool m_bFullscreen;
	D3DFORMAT m_DisplayFormat;
	D3DFORMAT m_BufferFormat;
	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nVertexProcessing;
	UINT m_nPresentInterval;
};

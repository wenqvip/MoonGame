#include "MoonSetting.h"

MoonSetting::MoonSetting(void)
: m_nAdapter(0)
, m_bFullscreen(false)
, m_nWidth(0)
, m_nHeight(0)
, m_nVertexProcessing(0)
, m_nPresentInterval(0)
{
}

MoonSetting::~MoonSetting(void)
{
}

void MoonSetting::GetSetting()
{
	char str[32];	
	GetPrivateProfileString("MoonRenderSetup","DeviceType","0",str,32,"./moon.ini");
	m_DeviceType=StringToDevtype(str);
	GetPrivateProfileString("MoonRenderSetup","VertexProcessing","0",str,32,"./moon.ini");
	m_nVertexProcessing=StringToUINT(str);
	GetPrivateProfileString("MoonRenderSetup","PresentInterval","0",str,32,"./moon.ini");
	m_nPresentInterval=StringToUINT(str);
	GetPrivateProfileString("MoonRenderSetup","DisplayFormat","0",str,32,"./moon.ini");
	m_DisplayFormat = StringToFmt(str);
	GetPrivateProfileString("MoonRenderSetup","FullScreen","0",str,32,"./moon.ini");
	if( str[0] == '0' )
		m_bFullscreen = false;
	else
		m_bFullscreen = true;
	GetPrivateProfileString("MoonRenderSetup","BufferFormat","D3DFMT_UNKNOWN",str,32,"./moon.ini");
	m_BufferFormat = StringToFmt(str);
	GetPrivateProfileString("MoonRenderSetup","Adapter","0",str,32,"./moon.ini");
	m_nAdapter = (int)(str[0]-48);
	GetPrivateProfileString("MoonRenderSetup","Resolution","800 * 600",str,32,"./moon.ini");
	
	//将读进来的字符串如“800 * 600”变成数，使nWidth=800,nHeight=600
	m_nWidth = 0;
	m_nHeight = 0;
	for(int i=0;i<32;i++)
	{
		if(isNum(str[i])&&m_nWidth==0)
		{
			for(;i<32;i++)
			{
				if(isNum(str[i]))
					m_nWidth=m_nWidth*10+(int)(str[i]-48);
				else
					break;
			}
		}
		else if(isNum(str[i])&&m_nWidth!=0)
		{
			for(;i<32;i++)
			{
				if(isNum(str[i]))
					m_nHeight=m_nHeight*10+(int)(str[i]-48);
				else
					break;
			}
			break;
		}
	}
}

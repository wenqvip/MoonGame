// RenderSetup.h : RenderSetup DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRenderSetupApp
// �йش���ʵ�ֵ���Ϣ������� RenderSetup.cpp
//

class CRenderSetupApp : public CWinApp
{
public:
	CRenderSetupApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

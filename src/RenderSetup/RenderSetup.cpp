// RenderSetup.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "RenderSetup.h"
#include "RenderSetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CRenderSetupApp

BEGIN_MESSAGE_MAP(CRenderSetupApp, CWinApp)
END_MESSAGE_MAP()


// CRenderSetupApp ����

CRenderSetupApp::CRenderSetupApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CRenderSetupApp ����

CRenderSetupApp theApp;


// CRenderSetupApp ��ʼ��

BOOL CRenderSetupApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



extern "C" __declspec(dllexport) int RunDlg(void)
{
	RenderSetupDlg dlg;
	if(dlg.DoModal()==IDOK)
		return 1;
	return 0;
}
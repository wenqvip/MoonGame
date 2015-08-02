// RenderSetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RenderSetup.h"
#include "RenderSetupDlg.h"


// RenderSetupDlg 对话框

IMPLEMENT_DYNAMIC(RenderSetupDlg, CDialog)

RenderSetupDlg::RenderSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RenderSetupDlg::IDD, pParent)
	, m_pBox(NULL)
	, m_pLBox(NULL)
	, m_nListBoxIndex(-1)
{
	m_nAdapter = 0;
	m_DeviceType = D3DDEVTYPE_HAL;
	m_bFullScreen = false;
	m_strDisplayFormat = FmtToString(D3DFMT_X8R8G8B8);
	m_strBufferFormat = FmtToString(D3DFMT_X8R8G8B8);
	m_strResolution = "800 * 600";
	m_strVertexProcessing = UINTToString(D3DCREATE_SOFTWARE_VERTEXPROCESSING);
	m_strPresentInterval = UINTToString(D3DPRESENT_INTERVAL_DEFAULT);
}

RenderSetupDlg::~RenderSetupDlg()
{
	//if(m_pBox!=NULL)
	//	delete m_pBox;
	if(m_pD3D!=NULL){
		m_pD3D->Release();
		m_pD3D=NULL;
	}
}

void RenderSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RenderSetupDlg, CDialog)
	//ON_BN_CLICKED(IDC_CHECK1, &RenderSetupDlg::OnBnClickedCheck1)
	//ON_CBN_SELCHANGE(IDC_ADAPTER, &RenderSetupDlg::OnCbnSelchangeAdapter)
	ON_LBN_SELCHANGE(IDC_LIST, &RenderSetupDlg::OnLbnSelchangeList)
	ON_CBN_SELCHANGE(IDC_COMBO, &RenderSetupDlg::OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// RenderSetupDlg 消息处理程序

BOOL RenderSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//创建D3D对象
	if(NULL==(m_pD3D=Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(_T("Cannot create D3D object!"));
		return FALSE;
	}
	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1),TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1),FALSE);
	m_pBox = (CComboBox*) GetDlgItem(IDC_COMBO);
	m_pLBox = (CListBox*) GetDlgItem(IDC_LIST);

	if(!InitCheckfromfile())
		if(!InitCheckDefault())
			return FALSE;

	//CheckVideoMode(0,m_bFullScreen);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void RenderSetupDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	WritePrivateProfileSection("MoonRenderSetup","","./moon.ini");

	//
	CString str;
	str.Format("%d",m_nAdapter);
	if(str.GetLength()==0||m_strResolution.GetLength()==0
		||m_strBufferFormat.GetLength()==0 
		||m_strDisplayFormat.GetLength()==0
		||m_strVertexProcessing.GetLength()==0
		||m_strPresentInterval.GetLength()==0)
	{
		MessageBox("有些选项还未设置！");
		return;
	}
	WritePrivateProfileString("MoonRenderSetup","Adapter",str,"./moon.ini");
	WritePrivateProfileString("MoonRenderSetup","DeviceType",DevtypeToString(m_DeviceType),"./moon.ini");

	if(m_bFullScreen)
		WritePrivateProfileString("MoonRenderSetup","FullScreen","1","./moon.ini");
	else
		WritePrivateProfileString("MoonRenderSetup","FullScreen","0","./moon.ini");

	WritePrivateProfileString("MoonRenderSetup","DisplayFormat",m_strDisplayFormat,"./moon.ini");
	WritePrivateProfileString("MoonRenderSetup","BufferFormat",m_strBufferFormat,"./moon.ini");
	WritePrivateProfileString("MoonRenderSetup","Resolution",m_strResolution,"./moon.ini");
	WritePrivateProfileString("MoonRenderSetup","VertexProcessing",m_strVertexProcessing,"./moon.ini");
	WritePrivateProfileString("MoonRenderSetup","PresentInterval",m_strPresentInterval,"./moon.ini");

	CDialog::OnOK();
}

CString RenderSetupDlg::FmtToString(D3DFORMAT format)
{
	CString str;
	if(format==D3DFMT_A8R8G8B8)
		str="D3DFMT_A8R8G8B8";
	else if(format==D3DFMT_X8R8G8B8)
		str="D3DFMT_X8R8G8B8";
	else if(format==D3DFMT_R5G6B5)
		str="D3DFMT_R5G6B5";
	else if(format==D3DFMT_X1R5G5B5)
		str="D3DFMT_X1R5G5B5";
	else if(format==D3DFMT_A1R5G5B5)
		str="D3DFMT_A1R5G5B5";
	else if(format==D3DFMT_A2R10G10B10)
		str="D3DFMT_A2R10G10B10";
	else
		str="D3DFMT_X8R8G8B8";
	return str;
}

HRESULT RenderSetupDlg::CheckFormat(int nAdapter, D3DDEVTYPE devtype, bool bFullscreen)
{
	SetDlgItemText(IDC_STATIC1,_T("Display Format: "));
	DeleteAllItem(m_pBox);
	if(bFullscreen==false)
	{
		D3DDISPLAYMODE d3ddm;
		m_pD3D->GetAdapterDisplayMode(nAdapter,&d3ddm);
		m_strDisplayFormat = FmtToString(d3ddm.Format);
		m_pBox->AddString(m_strDisplayFormat);
		m_pBox->SetCurSel(0);
		SetListBox(3,m_strDisplayFormat);
		return S_OK;
	}
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,true)))
		m_pBox->AddString("D3DFMT_X8R8G8B8");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_X1R5G5B5,D3DFMT_X1R5G5B5,true)))
		m_pBox->AddString("D3DFMT_X1R5G5B5");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_R5G6B5,D3DFMT_R5G6B5,true)))
		m_pBox->AddString("D3DFMT_R5G6B5");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_A2R10G10B10,D3DFMT_A2R10G10B10,true)))
		m_pBox->AddString("D3DFMT_A2R10G10B10");
	CString str;
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==m_strDisplayFormat){
			m_pBox->SetCurSel(i);
			return S_OK;
		}
	}
	m_pBox->SetCurSel(0);
	return S_OK;
}

HRESULT RenderSetupDlg::CheckResolution(int nAdapter, D3DFORMAT format)
{
	SetDlgItemText(IDC_STATIC1,_T("Resolution: "));
	DeleteAllItem(m_pBox);
	CString str;
	D3DDISPLAYMODE d3ddm;
	int w=0,h=0;
	for(int i=0;i<m_pD3D->GetAdapterModeCount(nAdapter,format);i++)
	{
		m_pD3D->EnumAdapterModes(nAdapter,format,i,&d3ddm);
		if(d3ddm.Width<640)
			continue;
		if(w!=d3ddm.Width&&h!=d3ddm.Height)
			w=d3ddm.Width,h=d3ddm.Height;
		else
			continue;
		str.Format("%d * %d",d3ddm.Width,d3ddm.Height);
		m_pBox->AddString(str);
	}
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==m_strResolution){
			m_pBox->SetCurSel(i);
			break;
		}
	}
	return S_OK;
}

HRESULT RenderSetupDlg::CheckFullscreen(void)
{
	SetDlgItemText(IDC_STATIC1,_T("FullScreen: "));
	DeleteAllItem(m_pBox);
	m_pBox->AddString("False");
	m_pBox->AddString("True");
	if(m_bFullScreen)
		m_pBox->SetCurSel(1);
	else
		m_pBox->SetCurSel(0);
	return S_OK;
}

HRESULT RenderSetupDlg::CheckAdapter(void)
{
	SetDlgItemText(IDC_STATIC1,_T("Adapter: "));
	DeleteAllItem(m_pBox);
	CString str;
	D3DADAPTER_IDENTIFIER9 pIdentifier;
	ZeroMemory(&pIdentifier,sizeof(pIdentifier));
	for(int i=0;i< m_pD3D->GetAdapterCount();i++)
	{
		if(m_pD3D->GetAdapterIdentifier(i,0,&pIdentifier)==E_FAIL){
			MessageBox(_T("Cannot get adapter count!"));
			return E_FAIL;
		}
		str+=pIdentifier.Description;
		m_pBox->AddString(str);
	}
	m_pBox->SetCurSel(m_nAdapter);
	return S_OK;
}

void RenderSetupDlg::DeleteAllItem(CComboBox* pBox)
{
	//删掉IDC_VIDEOMODE的原有项
	if(pBox !=NULL){
		int t=pBox->GetCount();
		for(int i=1;i<=t;i++)
			pBox->DeleteString(t-i);
	}
	pBox->SetCurSel(0);
}

HRESULT RenderSetupDlg::CheckFormatBuffer(int nAdapter, D3DDEVTYPE devtype, D3DFORMAT format, bool bFullscreen)
{
	SetDlgItemText(IDC_STATIC1,_T("Buffer Format: "));
	DeleteAllItem(m_pBox);
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_X8R8G8B8,!bFullscreen)))
		m_pBox->AddString("D3DFMT_X8R8G8B8");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A8R8G8B8,!bFullscreen)))
		m_pBox->AddString("D3DFMT_A8R8G8B8");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_X1R5G5B5,!bFullscreen)))
		m_pBox->AddString("D3DFMT_X1R5G5B5");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A1R5G5B5,!bFullscreen)))
		m_pBox->AddString("D3DFMT_A1R5G5B5");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_R5G6B5,!bFullscreen)))
		m_pBox->AddString("D3DFMT_R5G6B5");
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A2R10G10B10,!bFullscreen)))
		m_pBox->AddString("D3DFMT_A2R10G10B10");
	CString str;
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==m_strBufferFormat){
			m_pBox->SetCurSel(i);
			return S_OK;
		}
	}
	m_pBox->SetCurSel(0);
	m_pBox->GetLBText(0,m_strBufferFormat);
	SetListBox(m_nListBoxIndex,m_strBufferFormat);
	return S_OK;
}

D3DFORMAT RenderSetupDlg::StringToFmt(CString str)
{
	D3DFORMAT format;
	if(str=="D3DFMT_A8R8G8B8")
		format=D3DFMT_A8R8G8B8;
	else if(str=="D3DFMT_X8R8G8B8")
		format=D3DFMT_X8R8G8B8;
	else if(str=="D3DFMT_R5G6B5")
		format=D3DFMT_R5G6B5;
	else if(str=="D3DFMT_X1R5G5B5")
		format=D3DFMT_X1R5G5B5;
	else if(str=="D3DFMT_A1R5G5B5")
		format=D3DFMT_A1R5G5B5;
	else if(str=="D3DFMT_A2R10G10B10")
		format=D3DFMT_A2R10G10B10;
	else
		format=D3DFMT_X8R8G8B8;
	return format;
}

void RenderSetupDlg::OnLbnSelchangeList()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nListBoxIndex=m_pLBox->GetCurSel();
	if(m_nListBoxIndex==0)
		CheckAdapter();
	else if(m_nListBoxIndex==1)
		CheckDevtype();
	else if(m_nListBoxIndex==2)
		CheckFullscreen();
	else if(m_nListBoxIndex==3)
		CheckFormat(m_nAdapter, m_DeviceType, m_bFullScreen);
	else if(m_nListBoxIndex==4)
		CheckFormatBuffer(m_nAdapter,m_DeviceType, StringToFmt(m_strDisplayFormat),m_bFullScreen);
	else if(m_nListBoxIndex==5)
		CheckResolution(m_nAdapter,StringToFmt(m_strDisplayFormat));
	else if(m_nListBoxIndex==6)
		CheckVP(m_nAdapter, m_DeviceType);
	else if(m_nListBoxIndex==7)
		CheckPI(m_nAdapter, m_DeviceType);
	else
		DeleteAllItem(m_pBox);
}

void RenderSetupDlg::OnCbnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	int i=m_pBox->GetCurSel();
	m_pBox->GetWindowText(temp);
	if(m_nListBoxIndex==0){
		m_nAdapter=i;
	}
	else if(m_nListBoxIndex==1){
		m_DeviceType=StringToDevtype(temp);
		SetListBox(4,"");
		m_strBufferFormat = "";
		SetListBox(3,"");
		m_strDisplayFormat = "";
		SetListBox(6,"");
		m_strVertexProcessing = "";
		SetListBox(7,"");
		m_strPresentInterval = "";
	}
	else if(m_nListBoxIndex==2){
		if(i==1)
		{
			m_bFullScreen=true;
			SetListBox(4,"");
			m_strBufferFormat="";
		}
		else{
			m_bFullScreen=false;
			m_strDisplayFormat="";
			m_strBufferFormat="";
			SetListBox(4,"");
			SetListBox(3,"");
		}
	}
	else if(m_nListBoxIndex==3){
		m_strDisplayFormat=temp;
		m_strBufferFormat="";
		SetListBox(4,"");
	}
	else if(m_nListBoxIndex==4){
		m_strBufferFormat=temp;
	}
	else if(m_nListBoxIndex==5){
		m_strResolution=temp;
	}
	else if (m_nListBoxIndex==6){
		m_strVertexProcessing=temp;
	}
	else if (m_nListBoxIndex==7){
		m_strPresentInterval=temp;
	}
	SetListBox(m_nListBoxIndex,temp);
}

bool RenderSetupDlg::InitCheckDefault(void)
{
	D3DADAPTER_IDENTIFIER9 pIdentifier;
	ZeroMemory(&pIdentifier,sizeof(pIdentifier));
	CString str;
//	D3DDISPLAYMODE d3ddm;

	str="DisplayAdapter: ";
	if(m_pD3D->GetAdapterIdentifier(0,0,&pIdentifier)!=S_OK)
		return false;
	m_nAdapter=0;
	str+=pIdentifier.Description;
	m_pLBox->AddString(str);
	
	str = "DeviceType: ";
	str+=DevtypeToString(m_DeviceType);
	m_pLBox->AddString(str);

	str="Fullscreen: ";
	if(m_bFullScreen)
		str+="True";
	else
		str+="False";
	m_pLBox->AddString(str);

	str="DisplayFormat: ";
	str+=m_strDisplayFormat;
	m_pLBox->AddString(str);

	str="BufferFormat: ";
	str+=m_strBufferFormat;
	m_pLBox->AddString(str);

	str="Resolution: ";
	str+=m_strResolution;
	m_pLBox->AddString(str);
	
	str="VertexProcessing: ";
	str+=m_strVertexProcessing;
	m_pLBox->AddString(str);
	
	str="PresentInterval: ";
	str+=m_strPresentInterval;
	m_pLBox->AddString(str);

	return true;
}

bool RenderSetupDlg::InitCheckfromfile(void)
{
	D3DADAPTER_IDENTIFIER9 pIdentifier;
	ZeroMemory(&pIdentifier,sizeof(pIdentifier));
	CString str;
//	D3DDISPLAYMODE d3ddm;
	char ch[32];
	GetPrivateProfileString("MoonRenderSetup","Adapter","n",ch,32,"./moon.ini");
	if(ch[0]=='n')
		return false;
	str="DisplayAdapter: ";
	m_nAdapter=(UINT)(ch[0]-48);
	if(m_pD3D->GetAdapterIdentifier(m_nAdapter,0,&pIdentifier)!=S_OK)
		return false;
	str+=pIdentifier.Description;
	m_pLBox->AddString(str);

	GetPrivateProfileString("MoonRenderSetup","DeviceType","n",ch,32,"./moon.ini");
	str = "DeviceType: ";
	m_DeviceType = StringToDevtype(CString(ch));
	str+=ch;
	m_pLBox->AddString(str);

	GetPrivateProfileString("MoonRenderSetup","FullScreen","0",ch,32,"./moon.ini");
	str="Fullscreen: ";
	if(ch[0]=='1'){
		m_bFullScreen=true;
		str+="True";
	}
	else{
		m_bFullScreen=false;
		str+="False";
	}
	m_pLBox->AddString(str);

	GetPrivateProfileString("MoonRenderSetup","DisplayFormat","D3DFMT_UNKOWN",ch,32,"./moon.ini");
	str="DisplayFormat: ";
	m_strDisplayFormat=ch;
	str+=m_strDisplayFormat;
	m_pLBox->AddString(str);

	GetPrivateProfileString("MoonRenderSetup","BufferFormat","D3DFMT_UNKOWN",ch,32,"./moon.ini");
	str="BufferFormat: ";
	m_strBufferFormat=ch;
	str+=m_strBufferFormat;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString("MoonRenderSetup","Resolution","800 * 600",ch,32,"./moon.ini");
	str="Resolution: ";
	m_strResolution=ch;
	str+=ch;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString("MoonRenderSetup","VertexProcessing","SOFTWARE_VP",ch,32,"./moon.ini");
	str="VertexProcessing: ";
	m_strVertexProcessing=ch;
	str+=ch;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString("MoonRenderSetup","PresentInterval","D3DPRESENT_INTERVAL_DEFAULT",ch,32,"./moon.ini");
	str="PresentInterval: ";
	m_strPresentInterval=ch;
	str+=ch;
	m_pLBox->AddString(str);

	return true;
}

void RenderSetupDlg::SetListBox(int i, CString str)
{
	CString string;
	m_pLBox->GetText(i,string);
	string=string.Left(string.ReverseFind(':')+2);
	string+=str;
	m_pLBox->DeleteString(i);
	m_pLBox->InsertString(i,string);
	m_pLBox->SetCurSel(i);
}

D3DDEVTYPE RenderSetupDlg::StringToDevtype(CString str)
{
	if(str == "D3DDEVTYPE_HAL")
		return D3DDEVTYPE_HAL;
	else if(str == "D3DDEVTYPE_REF")
		return D3DDEVTYPE_REF;
	else if(str == "D3DDEVTYPE_SW")
		return D3DDEVTYPE_SW;
	else
		return D3DDEVTYPE_FORCE_DWORD;
}

CString RenderSetupDlg::DevtypeToString(D3DDEVTYPE devtype)
{
	if(devtype == D3DDEVTYPE_HAL)
		return CString("D3DDEVTYPE_HAL");
	else if(devtype == D3DDEVTYPE_REF)
		return CString("D3DDEVTYPE_REF");
	else if(devtype == D3DDEVTYPE_SW)
		return CString("D3DDEVTYPE_SW");
	else
		return CString("D3DDEVTYPE_FORCE_DWORD");
}

CString RenderSetupDlg::UINTToString(UINT n)
{	
	CString str="";
	if(n==D3DCREATE_SOFTWARE_VERTEXPROCESSING)
		str = "SOFTWARE_VP";
	else if(n==D3DCREATE_HARDWARE_VERTEXPROCESSING)
		str = "HARDWARE_VP";
	else if(n==D3DCREATE_PUREDEVICE)
		str = "PURE_VP";
	else if(n==D3DCREATE_MIXED_VERTEXPROCESSING)
		str = "MIXED_VP";
	else if(n==D3DPRESENT_INTERVAL_DEFAULT)
		str = "D3DPRESENT_INTERVAL_DEFAULT";
	else if(n==D3DPRESENT_INTERVAL_ONE)
		str = "D3DPRESENT_INTERVAL_ONE";
	else if(n==D3DPRESENT_INTERVAL_TWO)
		str = "D3DPRESENT_INTERVAL_TWO";
	else if(n==D3DPRESENT_INTERVAL_THREE)
		str = "D3DPRESENT_INTERVAL_THREE";
	else if(n==D3DPRESENT_INTERVAL_FOUR)
		str = "D3DPRESENT_INTERVAL_FOUR";
	else if(n==D3DPRESENT_INTERVAL_IMMEDIATE)
		str = "D3DPRESENT_INTERVAL_IMMEDIATE";
	return str;
}

HRESULT RenderSetupDlg::CheckDevtype(void)
{
	SetDlgItemText(IDC_STATIC1,_T("Device Type: "));
	DeleteAllItem(m_pBox);
	m_pBox->AddString("D3DDEVTYPE_HAL");
	m_pBox->AddString("D3DDEVTYPE_REF");
	CString str;
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==DevtypeToString(m_DeviceType)){
			m_pBox->SetCurSel(i);
			return S_OK;
		}
	}
	m_pBox->SetCurSel(0);
	return S_OK;
}

HRESULT RenderSetupDlg::CheckVP(UINT nAdapter, D3DDEVTYPE devtype)
{
	SetDlgItemText(IDC_STATIC1,_T("Vertex Processing: "));
	DeleteAllItem(m_pBox);
	D3DCAPS9 caps;
	m_pD3D->GetDeviceCaps(nAdapter, devtype, &caps);

	m_pBox->AddString("SOFTWARE_VP");
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		m_pBox->AddString("HARDWARE_VP");
	}
	CString str;
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==m_strVertexProcessing){
			m_pBox->SetCurSel(i);
			return S_OK;
		}
	}
	m_pBox->SetCurSel(0);
	m_pBox->GetLBText(0,m_strVertexProcessing);
	SetListBox(m_nListBoxIndex,m_strVertexProcessing);
	return S_OK;
}

HRESULT RenderSetupDlg::CheckPI(UINT nAdapter, D3DDEVTYPE devtype)
{
	SetDlgItemText(IDC_STATIC1,_T("Present Interval: "));
	DeleteAllItem(m_pBox);
	D3DCAPS9 caps;
	m_pD3D->GetDeviceCaps(nAdapter, devtype, &caps);

	m_pBox->AddString("D3DPRESENT_INTERVA_DEFAULT");
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
		m_pBox->AddString("D3DPRESENT_INTERVAL_IMMEDIATE");
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE)
		m_pBox->AddString("D3DPRESENT_INTERVAL_ONE");
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO)
		m_pBox->AddString("D3DPRESENT_INTERVAL_TWO");
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE)
		m_pBox->AddString("D3DPRESENT_INTERVAL_THREE");
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR)
		m_pBox->AddString("D3DPRESENT_INTERVAL_FOUR");
	CString str;
	for(int i=0;i<m_pBox->GetCount();i++){
		m_pBox->GetLBText(i,str);
		if(str==m_strPresentInterval){
			m_pBox->SetCurSel(i);
			return S_OK;
		}
	}
	m_pBox->SetCurSel(0);
	m_pBox->GetLBText(0,m_strPresentInterval);
	SetListBox(m_nListBoxIndex,m_strPresentInterval);
	return S_OK;
}

UINT RenderSetupDlg::StringToUINT(CString str)
{
	UINT n=0;
	if(str == "SOFTWARE_VP")
		n=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if(str == "HARDWARE_VP")
		n=D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if(str == "PURE_VP")
		n=D3DCREATE_PUREDEVICE;
	else if(str == "MIXED_VP")
		n=D3DCREATE_MIXED_VERTEXPROCESSING;
	else if(str == "D3DPRESENT_INTERVAL_DEFAULT")
		n=D3DPRESENT_INTERVAL_DEFAULT;
	else if(str == "D3DPRESENT_INTERVAL_ONE")
		n=D3DPRESENT_INTERVAL_ONE;
	else if(str == "D3DPRESENT_INTERVAL_TWO")
		n=D3DPRESENT_INTERVAL_TWO;
	else if(str == "D3DPRESENT_INTERVAL_THREE")
		n=D3DPRESENT_INTERVAL_THREE;
	else if(str == "D3DPRESENT_INTERVAL_FOUR")
		n=D3DPRESENT_INTERVAL_FOUR;
	else if(str == "D3DPRESENT_INTERVAL_IMMEDIATE")
		n=D3DPRESENT_INTERVAL_IMMEDIATE;
	return n;
}

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
	WritePrivateProfileSection(_T("MoonRenderSetup"),_T(""),_T("./moon.ini"));

	//
	CString str;
	str.Format(_T("%d"),m_nAdapter);
	if(str.GetLength()==0||m_strResolution.GetLength()==0
		||m_strBufferFormat.GetLength()==0 
		||m_strDisplayFormat.GetLength()==0
		||m_strVertexProcessing.GetLength()==0
		||m_strPresentInterval.GetLength()==0)
	{
		MessageBox(_T("有些选项还未设置！"));
		return;
	}
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("Adapter"),str,_T("./moon.ini"));
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("DeviceType"),DevtypeToString(m_DeviceType),_T("./moon.ini"));

	if(m_bFullScreen)
		WritePrivateProfileString(_T("MoonRenderSetup"),_T("FullScreen"),_T("1"),_T("./moon.ini"));
	else
		WritePrivateProfileString(_T("MoonRenderSetup"),_T("FullScreen"),_T("0"),_T("./moon.ini"));

	WritePrivateProfileString(_T("MoonRenderSetup"),_T("DisplayFormat"),m_strDisplayFormat,_T("./moon.ini"));
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("BufferFormat"),m_strBufferFormat,_T("./moon.ini"));
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("Resolution"),m_strResolution,_T("./moon.ini"));
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("VertexProcessing"),m_strVertexProcessing,_T("./moon.ini"));
	WritePrivateProfileString(_T("MoonRenderSetup"),_T("PresentInterval"),m_strPresentInterval,_T("./moon.ini"));

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
		m_pBox->AddString(_T("D3DFMT_X8R8G8B8"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_X1R5G5B5,D3DFMT_X1R5G5B5,true)))
		m_pBox->AddString(_T("D3DFMT_X1R5G5B5"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_R5G6B5,D3DFMT_R5G6B5,true)))
		m_pBox->AddString(_T("D3DFMT_R5G6B5"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,D3DFMT_A2R10G10B10,D3DFMT_A2R10G10B10,true)))
		m_pBox->AddString(_T("D3DFMT_A2R10G10B10"));
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
		str.Format(_T("%d * %d"),d3ddm.Width,d3ddm.Height);
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
	m_pBox->AddString(_T("False"));
	m_pBox->AddString(_T("True"));
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
		m_pBox->AddString(_T("D3DFMT_X8R8G8B8"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A8R8G8B8,!bFullscreen)))
		m_pBox->AddString(_T("D3DFMT_A8R8G8B8"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_X1R5G5B5,!bFullscreen)))
		m_pBox->AddString(_T("D3DFMT_X1R5G5B5"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A1R5G5B5,!bFullscreen)))
		m_pBox->AddString(_T("D3DFMT_A1R5G5B5"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_R5G6B5,!bFullscreen)))
		m_pBox->AddString(_T("D3DFMT_R5G6B5"));
	if(SUCCEEDED(m_pD3D->CheckDeviceType(nAdapter,devtype,format,D3DFMT_A2R10G10B10,!bFullscreen)))
		m_pBox->AddString(_T("D3DFMT_A2R10G10B10"));
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
	if(str==_T("D3DFMT_A8R8G8B8"))
		format=D3DFMT_A8R8G8B8;
	else if(str==_T("D3DFMT_X8R8G8B8"))
		format=D3DFMT_X8R8G8B8;
	else if(str==_T("D3DFMT_R5G6B5"))
		format=D3DFMT_R5G6B5;
	else if(str==_T("D3DFMT_X1R5G5B5"))
		format=D3DFMT_X1R5G5B5;
	else if(str==_T("D3DFMT_A1R5G5B5"))
		format=D3DFMT_A1R5G5B5;
	else if(str==_T("D3DFMT_A2R10G10B10"))
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
		SetListBox(4,_T(""));
		m_strBufferFormat = _T("");
		SetListBox(3,_T(""));
		m_strDisplayFormat = _T("");
		SetListBox(6,_T(""));
		m_strVertexProcessing = _T("");
		SetListBox(7,_T(""));
		m_strPresentInterval = _T("");
	}
	else if(m_nListBoxIndex==2){
		if(i==1)
		{
			m_bFullScreen=true;
			SetListBox(4,_T(""));
			m_strBufferFormat=_T("");
		}
		else{
			m_bFullScreen=false;
			m_strDisplayFormat=_T("");
			m_strBufferFormat=_T("");
			SetListBox(4,_T(""));
			SetListBox(3,_T(""));
		}
	}
	else if(m_nListBoxIndex==3){
		m_strDisplayFormat=temp;
		m_strBufferFormat=_T("");
		SetListBox(4,_T(""));
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

	str=_T("DisplayAdapter: ");
	if(m_pD3D->GetAdapterIdentifier(0,0,&pIdentifier)!=S_OK)
		return false;
	m_nAdapter=0;
	str+=pIdentifier.Description;
	m_pLBox->AddString(str);
	
	str = _T("DeviceType: ");
	str+=DevtypeToString(m_DeviceType);
	m_pLBox->AddString(str);

	str=_T("Fullscreen: ");
	if(m_bFullScreen)
		str+=_T("True");
	else
		str+=_T("False");
	m_pLBox->AddString(str);

	str=_T("DisplayFormat: ");
	str+=m_strDisplayFormat;
	m_pLBox->AddString(str);

	str=_T("BufferFormat: ");
	str+=m_strBufferFormat;
	m_pLBox->AddString(str);

	str=_T("Resolution: ");
	str+=m_strResolution;
	m_pLBox->AddString(str);
	
	str=_T("VertexProcessing: ");
	str+=m_strVertexProcessing;
	m_pLBox->AddString(str);
	
	str=_T("PresentInterval: ");
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
	wchar_t ch[32];
	GetPrivateProfileString(_T("MoonRenderSetup"),_T("Adapter"),_T("n"),ch,32,_T("./moon.ini"));
	if(ch[0]=='n')
		return false;
	str=_T("DisplayAdapter: ");
	m_nAdapter=(UINT)(ch[0]-48);
	if(m_pD3D->GetAdapterIdentifier(m_nAdapter,0,&pIdentifier)!=S_OK)
		return false;
	str+=pIdentifier.Description;
	m_pLBox->AddString(str);

	GetPrivateProfileString(_T("MoonRenderSetup"),_T("DeviceType"),_T("n"),ch,32,_T("./moon.ini"));
	str = "DeviceType: ";
	m_DeviceType = StringToDevtype(CString(ch));
	str+=ch;
	m_pLBox->AddString(str);

	GetPrivateProfileString(_T("MoonRenderSetup"),_T("FullScreen"),_T("0"),ch,32,_T("./moon.ini"));
	str=_T("Fullscreen: ");
	if(ch[0]=='1'){
		m_bFullScreen=true;
		str+=_T("True");
	}
	else{
		m_bFullScreen=false;
		str+=_T("False");
	}
	m_pLBox->AddString(str);

	GetPrivateProfileString(_T("MoonRenderSetup"),_T("DisplayFormat"),_T("D3DFMT_UNKOWN"),ch,32,_T("./moon.ini"));
	str=_T("DisplayFormat: ");
	m_strDisplayFormat=ch;
	str+=m_strDisplayFormat;
	m_pLBox->AddString(str);

	GetPrivateProfileString(_T("MoonRenderSetup"),_T("BufferFormat"),_T("D3DFMT_UNKOWN"),ch,32,_T("./moon.ini"));
	str=_T("BufferFormat: ");
	m_strBufferFormat=ch;
	str+=m_strBufferFormat;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString(_T("MoonRenderSetup"),_T("Resolution"),_T("800 * 600"),ch,32,_T("./moon.ini"));
	str=_T("Resolution: ");
	m_strResolution=ch;
	str+=ch;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString(_T("MoonRenderSetup"),_T("VertexProcessing"),_T("SOFTWARE_VP"),ch,32,_T("./moon.ini"));
	str=_T("VertexProcessing: ");
	m_strVertexProcessing=ch;
	str+=ch;
	m_pLBox->AddString(str);
		
	GetPrivateProfileString(_T("MoonRenderSetup"),_T("PresentInterval"),_T("D3DPRESENT_INTERVAL_DEFAULT"),ch,32,_T("./moon.ini"));
	str=_T("PresentInterval: ");
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
	if(str == _T("D3DDEVTYPE_HAL"))
		return D3DDEVTYPE_HAL;
	else if(str == _T("D3DDEVTYPE_REF"))
		return D3DDEVTYPE_REF;
	else if(str == _T("D3DDEVTYPE_SW"))
		return D3DDEVTYPE_SW;
	else
		return D3DDEVTYPE_FORCE_DWORD;
}

CString RenderSetupDlg::DevtypeToString(D3DDEVTYPE devtype)
{
	if(devtype == D3DDEVTYPE_HAL)
		return CString(_T("D3DDEVTYPE_HAL"));
	else if(devtype == D3DDEVTYPE_REF)
		return CString(_T("D3DDEVTYPE_REF"));
	else if(devtype == D3DDEVTYPE_SW)
		return CString(_T("D3DDEVTYPE_SW"));
	else
		return CString(_T("D3DDEVTYPE_FORCE_DWORD"));
}

CString RenderSetupDlg::UINTToString(UINT n)
{	
	CString str=_T("");
	if(n==D3DCREATE_SOFTWARE_VERTEXPROCESSING)
		str = _T("SOFTWARE_VP");
	else if(n==D3DCREATE_HARDWARE_VERTEXPROCESSING)
		str = _T("HARDWARE_VP");
	else if(n==D3DCREATE_PUREDEVICE)
		str = _T("PURE_VP");
	else if(n==D3DCREATE_MIXED_VERTEXPROCESSING)
		str = _T("MIXED_VP");
	else if(n==D3DPRESENT_INTERVAL_DEFAULT)
		str = _T("D3DPRESENT_INTERVAL_DEFAULT");
	else if(n==D3DPRESENT_INTERVAL_ONE)
		str = _T("D3DPRESENT_INTERVAL_ONE");
	else if(n==D3DPRESENT_INTERVAL_TWO)
		str = _T("D3DPRESENT_INTERVAL_TWO");
	else if(n==D3DPRESENT_INTERVAL_THREE)
		str = _T("D3DPRESENT_INTERVAL_THREE");
	else if(n==D3DPRESENT_INTERVAL_FOUR)
		str = _T("D3DPRESENT_INTERVAL_FOUR");
	else if(n==D3DPRESENT_INTERVAL_IMMEDIATE)
		str = _T("D3DPRESENT_INTERVAL_IMMEDIATE");
	return str;
}

HRESULT RenderSetupDlg::CheckDevtype(void)
{
	SetDlgItemText(IDC_STATIC1,_T("Device Type: "));
	DeleteAllItem(m_pBox);
	m_pBox->AddString(_T("D3DDEVTYPE_HAL"));
	m_pBox->AddString(_T("D3DDEVTYPE_REF"));
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

	m_pBox->AddString(_T("SOFTWARE_VP"));
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		m_pBox->AddString(_T("HARDWARE_VP"));
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

	m_pBox->AddString(_T("D3DPRESENT_INTERVA_DEFAULT"));
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
		m_pBox->AddString(_T("D3DPRESENT_INTERVAL_IMMEDIATE"));
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE)
		m_pBox->AddString(_T("D3DPRESENT_INTERVAL_ONE"));
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO)
		m_pBox->AddString(_T("D3DPRESENT_INTERVAL_TWO"));
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE)
		m_pBox->AddString(_T("D3DPRESENT_INTERVAL_THREE"));
	if(caps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR)
		m_pBox->AddString(_T("D3DPRESENT_INTERVAL_FOUR"));
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
	if(str == _T("SOFTWARE_VP"))
		n=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if(str == _T("HARDWARE_VP"))
		n=D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if(str == _T("PURE_VP"))
		n=D3DCREATE_PUREDEVICE;
	else if(str == _T("MIXED_VP"))
		n=D3DCREATE_MIXED_VERTEXPROCESSING;
	else if(str == _T("D3DPRESENT_INTERVAL_DEFAULT"))
		n=D3DPRESENT_INTERVAL_DEFAULT;
	else if(str == _T("D3DPRESENT_INTERVAL_ONE"))
		n=D3DPRESENT_INTERVAL_ONE;
	else if(str == _T("D3DPRESENT_INTERVAL_TWO"))
		n=D3DPRESENT_INTERVAL_TWO;
	else if(str == _T("D3DPRESENT_INTERVAL_THREE"))
		n=D3DPRESENT_INTERVAL_THREE;
	else if(str == _T("D3DPRESENT_INTERVAL_FOUR"))
		n=D3DPRESENT_INTERVAL_FOUR;
	else if(str == _T("D3DPRESENT_INTERVAL_IMMEDIATE"))
		n=D3DPRESENT_INTERVAL_IMMEDIATE;
	return n;
}

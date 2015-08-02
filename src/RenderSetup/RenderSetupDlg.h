#pragma once
#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")


// RenderSetupDlg 对话框

class RenderSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(RenderSetupDlg)

public:
	RenderSetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RenderSetupDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnCbnSelchangeCombo();

protected:
	virtual void OnOK();

private:
	CListBox* m_pLBox;
	CComboBox* m_pBox;
	LPDIRECT3D9 m_pD3D;
	int m_nListBoxIndex;

	UINT m_nAdapter;
	D3DDEVTYPE m_DeviceType;
	bool m_bFullScreen;
	CString m_strDisplayFormat;
	CString m_strBufferFormat;
	CString m_strResolution;
	CString m_strVertexProcessing;
	CString m_strPresentInterval;

	CString FmtToString(D3DFORMAT format);
	D3DFORMAT StringToFmt(CString str);
	CString DevtypeToString(D3DDEVTYPE devtype);
	D3DDEVTYPE StringToDevtype(CString str);
	CString UINTToString(UINT n);
	UINT StringToUINT(CString str);

	HRESULT CheckAdapter(void);
	HRESULT CheckDevtype(void);
	HRESULT CheckFullscreen(void);
	HRESULT CheckFormat(int nAdapter, D3DDEVTYPE m_DeviceType, bool bFullscreen);
	HRESULT CheckFormatBuffer(int nAdapter, D3DDEVTYPE devtype, D3DFORMAT format, bool bFullscreen);
	HRESULT CheckResolution(int nAdapter, D3DFORMAT format);
	HRESULT CheckVP(UINT nAdapter, D3DDEVTYPE devtype);
	HRESULT CheckPI(UINT nAdapter, D3DDEVTYPE devtype);

	void DeleteAllItem(CComboBox* pBox);
	bool InitCheckDefault(void);
	bool InitCheckfromfile(void);
	void SetListBox(int i, CString str);
};

#pragma once


// CDlgLogFilter �Ի���

class CDlgLogFilter : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogFilter)

public:
	CDlgLogFilter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogFilter();

// �Ի�������
	enum { IDD = IDD_DLG_LOG_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnLbnDblclkListStationName();
	afx_msg void OnLbnDblclkListUsefulKeys();
	CString m_szFilterKey;
	afx_msg void OnBnClickedClearKey();
	afx_msg void OnBnClickedOk();
};

#pragma once


// CAbsStopDlg �Ի���

class CAbsStopDlg : public CDialog
{
	DECLARE_DYNAMIC(CAbsStopDlg)

public:
	CAbsStopDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAbsStopDlg();

// �Ի�������
	enum { IDD = IDD_DLG_ABS_STOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_reason;
	DWORD m_abs_stop_time;
};

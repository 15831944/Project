#pragma once
#include "afxwin.h"


// CMessageDlg �Ի���

class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMessageDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
	void ShowMessage(CStringArray& text);
	CEdit m_textmsg;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};

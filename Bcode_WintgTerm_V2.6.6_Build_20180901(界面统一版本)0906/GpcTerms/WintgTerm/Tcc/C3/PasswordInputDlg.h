#pragma once


#include "afxwin.h"
#include "resource.h"

namespace TCC_C3
{

class CPasswordInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordInputDlg)

public:
	CPasswordInputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordInputDlg();

// �Ի�������
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
	CString m_szPasswd;

public:
	virtual BOOL OnInitDialog();

	CString GetPassword() { return m_szPasswd; }
};

}
// PasswordInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PasswordInputDlg.h"

namespace TCC_C3
{
// CPasswordInputDlg �Ի���

IMPLEMENT_DYNAMIC(CPasswordInputDlg, CDialog)
CPasswordInputDlg::CPasswordInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordInputDlg::IDD, pParent)
{
}

CPasswordInputDlg::~CPasswordInputDlg()
{
}

void CPasswordInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_szPasswd);
}

BEGIN_MESSAGE_MAP(CPasswordInputDlg, CDialog)
END_MESSAGE_MAP()

// CPasswordInputDlg ��Ϣ�������
BOOL CPasswordInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_szPasswd = "";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPasswordInputDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}



}
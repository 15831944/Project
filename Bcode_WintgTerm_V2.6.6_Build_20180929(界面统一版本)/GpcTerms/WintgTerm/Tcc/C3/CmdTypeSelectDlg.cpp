// PasswordInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CmdTypeSelectDlg.h"

namespace TCC_C3
{
// CPasswordInputDlg �Ի���

IMPLEMENT_DYNAMIC(CCmdTypeSelectDlg, CDialog)
CCmdTypeSelectDlg::CCmdTypeSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdTypeSelectDlg::IDD, pParent)
{
}

CCmdTypeSelectDlg::~CCmdTypeSelectDlg()
{
}

BEGIN_MESSAGE_MAP(CCmdTypeSelectDlg, CDialog)
END_MESSAGE_MAP()


// CCmdTypeSelectDlg ��Ϣ�������
BOOL CCmdTypeSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if(CWnd *pParent = GetParent())
	{
		CRect rcParentWnd;
		pParent->GetWindowRect(&rcParentWnd);

		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CPoint pt;
		pt.x = rcParentWnd.left + (rcParentWnd.Width() - rcWnd.Width())/2;
		pt.y = rcParentWnd.top  + (rcParentWnd.Height()- rcWnd.Height())/2;

		SetWindowPos(NULL, pt.x, pt.y, rcWnd.Width(), rcWnd.Height(), SWP_NOSIZE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


}
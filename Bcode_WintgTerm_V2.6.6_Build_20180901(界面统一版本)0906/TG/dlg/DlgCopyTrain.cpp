// DlgCopyTrain.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "DlgCopyTrain.h"
#include ".\dlgcopytrain.h"


// CDlgCopyTrain �Ի���

IMPLEMENT_DYNAMIC(CDlgCopyTrain, CDialog)
CDlgCopyTrain::CDlgCopyTrain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCopyTrain::IDD, pParent)
	, m_trainId(_T(""))
{
}

CDlgCopyTrain::~CDlgCopyTrain()
{
}

void CDlgCopyTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHANGE_NEWID, m_trainId);
	DDV_MaxChars(pDX, m_trainId, 9);
}


BEGIN_MESSAGE_MAP(CDlgCopyTrain, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCopyTrain ��Ϣ�������

void CDlgCopyTrain::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd* pWnd = GetDlgItem(IDC_CHANGE_NEWID);
	if(pWnd != NULL)
	{
		pWnd->GetWindowText(m_trainId);
	}

	if(m_trainId.GetLength()>0)
		OnOK();
}

// TGColorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "TGColorDlg.h"
#include ".\tgcolordlg.h"


// CTGColorDlg �Ի���

IMPLEMENT_DYNAMIC(CTGColorDlg, CDialog)
CTGColorDlg::CTGColorDlg(CWnd* pParent)
	: CDialog(CTGColorDlg::IDD, pParent)
{
}

CTGColorDlg::~CTGColorDlg()
{
}

void CTGColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
}


BEGIN_MESSAGE_MAP(CTGColorDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	
END_MESSAGE_MAP()


// CTGColorDlg ��Ϣ�������

BOOL CTGColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_slider.SetRange(0, 50);
	m_slider.SetPos(mColor);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTGColorDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mColor = m_slider.GetPos();
	OnOK();
}


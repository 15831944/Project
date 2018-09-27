// JiaoluTrainSelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "JiaoluTrainSelDlg.h"


// CJiaoluTrainSelDlg �Ի���

IMPLEMENT_DYNAMIC(CJiaoluTrainSelDlg, CDialog)
CJiaoluTrainSelDlg::CJiaoluTrainSelDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex)
	: CDialog(CJiaoluTrainSelDlg::IDD, pParent)
{
	m_return_trainindex=0;
	m_list_trainno.RemoveAll();
	m_list_trainindex.RemoveAll();

	m_list_trainno.Append(list_trainno);
	m_list_trainindex.Append(list_trainindex);
}

CJiaoluTrainSelDlg::~CJiaoluTrainSelDlg()
{
}

void CJiaoluTrainSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_list);
}


BEGIN_MESSAGE_MAP(CJiaoluTrainSelDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CJiaoluTrainSelDlg ��Ϣ�������
BOOL CJiaoluTrainSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i=0; i<m_list_trainno.GetCount()&&m_list_trainindex.GetCount(); i++)
	{
		int index=m_list.AddString(m_list_trainno.GetAt(i));
		m_list.SetItemData(index, m_list_trainindex.GetAt(i));
	}

	m_list.SetCurSel(-1);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CJiaoluTrainSelDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel=m_list.GetCurSel();
	if(nSel==-1)
	{
		m_return_trainindex=0;
		AfxMessageBox("��ѡ��·�г�!!");
	}
	else
	{
		m_return_trainindex=m_list.GetItemData(nSel);
		OnOK();
	}
}

void CJiaoluTrainSelDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_return_trainindex=0;
	OnCancel();
}

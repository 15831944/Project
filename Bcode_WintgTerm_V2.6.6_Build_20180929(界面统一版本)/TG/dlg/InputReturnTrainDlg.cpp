// ReturnTrainSelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "InputReturnTrainDlg.h"
#include ".\InputReturnTrainDlg.h"
#include "JiaoluTrainSelDlg.h"

// CInputReturnTrainDlg �Ի���

IMPLEMENT_DYNAMIC(CInputReturnTrainDlg, CDialog)
CInputReturnTrainDlg::CInputReturnTrainDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex)
	: CDialog(CInputReturnTrainDlg::IDD, pParent)
{
	m_return_trainindex=0;
	m_list_trainno.RemoveAll();
	m_list_trainindex.RemoveAll();

	m_list_trainno.Append(list_trainno);
	m_list_trainindex.Append(list_trainindex);
}

CInputReturnTrainDlg::~CInputReturnTrainDlg()
{
}

void CInputReturnTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputReturnTrainDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

// CInputReturnTrainDlg ��Ϣ�������
BOOL CInputReturnTrainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CInputReturnTrainDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTrainID="";
	CWnd* pWnd=(CWnd*)GetDlgItem(IDC_EDIT1);
	if(pWnd)
		pWnd->GetWindowText(strTrainID);

	WORD count=0;
	CStringArray m_list_trainno1;
	CDWordArray m_list_trainindex1;
	for(int i=0; i<m_list_trainno.GetCount()&&m_list_trainindex.GetCount(); i++)
	{
		CString str=m_list_trainno.GetAt(i), str2;
		int pos=str.Find('(');
		if(pos>=0)
			str=str.Left(pos);
		else
			str=str2;

		if(str==strTrainID)
		{
			m_list_trainno1.Add(m_list_trainno.GetAt(i));
			m_list_trainindex1.Add(m_list_trainindex.GetAt(i));
		}
	}
	if(m_list_trainno1.GetCount()==0)
	{
		m_return_trainindex=0;
		AfxMessageBox("�Ҳ���������ĺ���ǰ���г�!!");
	}
	else if(m_list_trainno1.GetCount()==1)
	{
		m_return_trainindex=m_list_trainindex1.GetAt(0);
		OnOK();
	}
	else
	{
		AfxMessageBox("����������ĳ���, �ҵ����ǰ���г�, ��ѡ��!!");
		CJiaoluTrainSelDlg dlg(this, m_list_trainno1, m_list_trainindex1);
		if(dlg.DoModal()==IDOK)
		{
			m_return_trainindex=dlg.m_return_trainindex;
			OnOK();
		}
		else
		{
			OnCancel();
		}
	}
}

void CInputReturnTrainDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_return_trainindex=0;
	OnCancel();
}

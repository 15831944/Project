// ReturnTrainidInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "ReturnTrainidInfo.h"
#include ".\returntrainidinfo.h"


// CReturnTrainidInfo �Ի���

IMPLEMENT_DYNAMIC(CReturnTrainidInfo, CDialog)
CReturnTrainidInfo::CReturnTrainidInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CReturnTrainidInfo::IDD, pParent)
{
}

CReturnTrainidInfo::~CReturnTrainidInfo()
{
}

void CReturnTrainidInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST4, m_list);
	DDX_Control(pDX, IDC_PROGRESS1, m_pross);
	DDX_Control(pDX, IDC_EDIT1, m_detailText);
}


BEGIN_MESSAGE_MAP(CReturnTrainidInfo, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST4, OnNMClickList4)
END_MESSAGE_MAP()

BOOL CReturnTrainidInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//IDD_DLG_INPUT
	DWORD dwStyle =m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);
	
	m_list.InsertColumn(0, "���",LVCFMT_CENTER,60);
	m_list.InsertColumn(1, "�����",LVCFMT_LEFT,120);
	m_list.InsertColumn(2, "����",LVCFMT_LEFT,150);
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// CReturnTrainidInfo ��Ϣ�������
void CReturnTrainidInfo::SetTotalProcessNum(int all)
{
	m_list.Invalidate();
	m_total=all;
	m_list.DeleteAllItems();
	m_pross.SetRange(0, m_total);
}

void CReturnTrainidInfo::StepAt(int step)
{
	m_pross.StepIt();
	if(m_total==step)
	{
		m_pross.SetPos(0);
	}
}

void CReturnTrainidInfo::AddGenMessage(CString text, CString czh)
{
	int nRow = m_list.GetItemCount();
	CString itemno;
	itemno.Format("%d",nRow+1) ;
	
	m_list.InsertItem(nRow, itemno);   
	m_list.SetItemText(nRow,1,czh);
	m_list.SetItemText(nRow,2,text);
}

void CReturnTrainidInfo::OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int i=0; i<m_list.GetItemCount(); i++)
	{
		if(m_list.GetItemState(i, LVIS_SELECTED)==LVIS_SELECTED)
		{
			CString text=m_list.GetItemText(i, 2);
			m_detailText.SetWindowText(text);
		}
	}

	*pResult = 0;
}

void CReturnTrainidInfo::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DestroyWindow();
}

void CReturnTrainidInfo::PostNcDestroy() 
{
	delete this;
}

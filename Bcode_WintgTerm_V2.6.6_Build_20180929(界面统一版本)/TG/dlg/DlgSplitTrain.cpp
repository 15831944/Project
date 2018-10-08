// DlgSplitTrain.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "..\tgForm.h"
#include ".\dlgsplittrain.h"

// CDlgSplitTrain �Ի���

IMPLEMENT_DYNAMIC(CDlgSplitTrain, CDialog)
CDlgSplitTrain::CDlgSplitTrain(CWnd* pParent)
	: CDialog(CDlgSplitTrain::IDD, pParent)
{
}

CDlgSplitTrain::~CDlgSplitTrain()
{
}

void CDlgSplitTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainAttribute)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSplitTrain, CDialog)
	ON_BN_CLICKED(IDC_BTN_SPLIT_OK, OnBtnSplitOk)
	ON_BN_CLICKED(IDC_BTN_SPLIT_CANCEL, OnBtnSplitCancel)
END_MESSAGE_MAP()


// CDlgSplitTrain ��Ϣ�������

BOOL CDlgSplitTrain::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strSplitTrainId1.Empty();
	m_strSplitTrainId2.Empty();

	CWnd* pWnd = GetDlgItem(IDC_SPLITED_TRAIN);
	if(pWnd != NULL)
	{
		pWnd->SetWindowText(m_strOrigTrainId);
	}
	
	return TRUE; // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgSplitTrain::OnBtnSplitOk()
{
	m_strSplitTrainId1.Empty();
	m_strSplitTrainId2.Empty();

	CWnd* pWnd = GetDlgItem(IDC_SPLIT_TRAINID1);
	if(pWnd != NULL)
	{
		pWnd->GetWindowText(m_strSplitTrainId1);
	}

	pWnd = GetDlgItem(IDC_SPLIT_TRAINID2);
	if(pWnd != NULL)
	{
		pWnd->GetWindowText(m_strSplitTrainId2);
	}

	if( m_strSplitTrainId1.IsEmpty() || m_strSplitTrainId2.IsEmpty())
	{
		MessageBox("������ֽ����г�����","��ʾ",MB_ICONQUESTION|MB_OK);
		return;
	}
	if( m_strSplitTrainId1 == m_strSplitTrainId2)
	{
		MessageBox("�ֽ����г����β�����ͬ","��ʾ",MB_ICONQUESTION|MB_OK);
		return;
	}
	CDialog::OnOK();
}

void CDlgSplitTrain::OnBtnSplitCancel()
{
	CDialog::OnCancel();
}
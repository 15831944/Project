// DlgCombineTrain.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgCombineTrain.h"
#include ".\dlgcombinetrain.h"

// CDlgCombineTrain �Ի���

IMPLEMENT_DYNAMIC(CDlgCombineTrain, CDialog)
CDlgCombineTrain::CDlgCombineTrain(CWnd* pParent)
	: CDialog(CDlgCombineTrain::IDD, pParent)
{
}

CDlgCombineTrain::~CDlgCombineTrain()
{
}

void CDlgCombineTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCombineTrain, CDialog)
	ON_BN_CLICKED(IDC_BTN_COMBINE_OK, OnCombineOk)
	ON_BN_CLICKED(IDC_BTN_COMBINE_CANCEL, OnCombineCancel)
END_MESSAGE_MAP()


// CDlgCombineTrain ��Ϣ�������
BOOL CDlgCombineTrain::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strTrainId1.Empty();
	m_strTrainId2.Empty();

	CWnd* pWnd = GetDlgItem(IDC_COMBINE_FINISHED_TRAIN);
	if(pWnd)
	{
		pWnd->SetWindowText(m_strNewTrainId);
	}

	return TRUE;
}

void CDlgCombineTrain::OnCombineOk()
{
	m_strTrainId1.Empty();
	m_strTrainId2.Empty();

	CWnd* pWnd = GetDlgItem(IDC_COMBINED_FIRST_TRAIN);
	if(pWnd)
	{
		pWnd->GetWindowText(m_strTrainId1);
	}

	pWnd = GetDlgItem(IDC_COMBINED_SECOND_TRAIN);
	if(pWnd)
	{
		pWnd->GetWindowText(m_strTrainId2);
	}

	if( m_strTrainId1.IsEmpty() || m_strTrainId2.IsEmpty())
	{
		MessageBox("����������г�����","��ʾ",MB_ICONQUESTION|MB_OK);
		return;
	}

	if(m_strTrainId1 == m_strNewTrainId || m_strTrainId2 == m_strNewTrainId)
	{
		MessageBox("����г����β�����ͬ","��ʾ",MB_ICONQUESTION|MB_OK);
		return;
	}
	
	CDialog::OnOK( );
}

void CDlgCombineTrain::OnCombineCancel()
{
	CDialog::OnCancel( );
}

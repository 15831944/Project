#pragma once

//add this class to split train by zzg in ty 2008.05.08
#include "resource.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory dialog
// CDlgSplitTrain �Ի���

class CDlgSplitTrain : public CDialog
{
	DECLARE_DYNAMIC(CDlgSplitTrain)

public:
	CDlgSplitTrain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSplitTrain();

// �Ի�������
	//{{AFX_DATA(CTrainAttribute)
	enum { IDD = IDD_SPLIT_TRAIN };
	//}}AFX_DATA
	
public:
	CString m_strOrigTrainId;
	CString m_strSplitTrainId1;
	CString m_strSplitTrainId2;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSplitOk();
	afx_msg void OnBtnSplitCancel();
};

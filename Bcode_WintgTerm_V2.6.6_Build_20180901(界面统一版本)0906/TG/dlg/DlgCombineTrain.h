#pragma once

#include "resource.h"
#include "tgdoc.h"

#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory dialog
// CDlgCombineTrain �Ի���

class CDlgCombineTrain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCombineTrain)

public:
	CDlgCombineTrain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCombineTrain();

// �Ի�������
	enum { IDD = IDD_COMBINE_TRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNewTrainId;
	CString m_strTrainId1;
	CString m_strTrainId2;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCombineOk();
	afx_msg void OnCombineCancel();
};

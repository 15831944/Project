#pragma once

#include "tgdoc.h"
// CDlgCheZuHao �Ի���

class CDlgCheZuHao : public CDialog
{
	DECLARE_DYNAMIC(CDlgCheZuHao)

public:
	CDlgCheZuHao(CTG_TRAIN* train, CTGDoc *pDoc, CWnd* pParent);   // ��׼���캯��
	virtual ~CDlgCheZuHao();

// �Ի�������
	enum { IDD = IDD_DLG_CZH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	// Implementation
protected:
   
	// Generated message map functions
	//{{AFX_MSG(CTransportInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString	m_strCheZuHao;
	CString m_origStStation;
	CString m_origEdStation;
		
	CTGDoc* m_pDoc;
	CTG_TRAIN m_train;
public:
	afx_msg void OnBnClickedOk();
};

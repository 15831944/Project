#pragma once
#include "afxdtctl.h"
#include "tgdoc.h"

// CFenjiekouTimeDlg �Ի���

class CFenjiekouTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(CFenjiekouTimeDlg)

public:
	CFenjiekouTimeDlg(CTGDoc *pDoc, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFenjiekouTimeDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_FENJIEKOU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_sborder_plan;
	CDateTimeCtrl m_eborder_plan;
	CDateTimeCtrl m_sborder_real;
	CDateTimeCtrl m_eborder_real;

public:
	ULONG m_trainIndex;
	CTGDoc *m_pDoc;
	ULONG m_sborder_plan_tm;
	ULONG m_eborder_plan_tm;
	ULONG m_sborder_real_tm;
	ULONG m_eborder_real_tm;
	
};

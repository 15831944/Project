#pragma once

#include "resource.h"
#include "gridctrl.h"
#include "gridcellcheck.h"
#include "tgdoc.h"
#include "uneditableGridCell.h"
#include "afxdtctl.h"
// CDlgSelMutiMark �Ի���

class CDlgSelMutiMark : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelMutiMark)

public:
	CDlgSelMutiMark(CTGDoc *pDoc, CWnd* pParent);   // ��׼���캯��
	virtual ~CDlgSelMutiMark();

// �Ի�������
	enum { IDD = IDD_DLG_SEL_MUTI_MARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
	
	CDateTimeCtrl m_btm;
	CDateTimeCtrl m_etm;
private:
	CGridCtrl * m_pGridCtrl;
	CTGDoc *m_pDoc;
public:
	afx_msg void OnBnClickedBtnFind();

public:
	ULONG mSelMarkIndex[2048], mSelMarkCount;
	afx_msg void OnBnClickedOk();
};

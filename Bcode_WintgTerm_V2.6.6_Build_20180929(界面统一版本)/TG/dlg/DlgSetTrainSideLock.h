#pragma once

#include "resource.h"
#include "gridctrl.h"
#include "gridcellcheck.h"
#include "tgdoc.h"
#include "uneditableGridCell.h"
#include "afxdtctl.h"
// CDlgSetTrainSideLock �Ի���

class CDlgSetTrainSideLock : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetTrainSideLock)

public:
	CDlgSetTrainSideLock(CTGDoc *pDoc, CWnd* pParent);   // ��׼���캯��
	virtual ~CDlgSetTrainSideLock();

	// �Ի�������
	enum { IDD = IDD_DLG_SET_MULTI_TRAIN_SIDE_LOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CDateTimeCtrl m_btm;
	CDateTimeCtrl m_etm;
	CButton m_selall;
	CButton m_selnone;
private:
	CGridCtrl * m_pGridCtrl;
	CTGDoc *m_pDoc;
public:
	afx_msg void OnBnClickedBtnFind();

public:
	ULONG mSelTrainIndex[2048], mSelTrainCount;

	afx_msg void OnBnClickedLock();
	afx_msg void OnBnClickedUnLock();
	afx_msg void OnBtnClose();
	afx_msg void OnBtnSelAll();
	afx_msg void OnBtnSelNone();
};

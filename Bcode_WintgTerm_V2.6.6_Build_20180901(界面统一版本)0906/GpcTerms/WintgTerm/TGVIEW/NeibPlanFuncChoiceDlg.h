#pragma once

#include "resource.h"
// CNeibPlanFuncChoiceDlg �Ի���

class CNeibPlanFuncChoiceDlg : public CDialog
{
	DECLARE_DYNAMIC(CNeibPlanFuncChoiceDlg)

public:
	CNeibPlanFuncChoiceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNeibPlanFuncChoiceDlg();

// �Ի�������
	enum { IDD = IDD_DLG_NEIG_PLAN_CHOICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMSG);
	DECLARE_MESSAGE_MAP()
public:
	int nRet;
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

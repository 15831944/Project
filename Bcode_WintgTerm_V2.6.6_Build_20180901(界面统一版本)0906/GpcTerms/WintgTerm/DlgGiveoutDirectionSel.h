#pragma once

#include "GridCtrl_src\gridcellcheck.h"
#include "wintgdoc.h"
#include "uneditableGridCell.h"

// CDlgGiveoutDirectionSel �Ի���

class CDlgGiveoutDirectionSel : public CDialog
{
	DECLARE_DYNAMIC(CDlgGiveoutDirectionSel)

public:
	CDlgGiveoutDirectionSel(CWintgDoc *pDoc, CSingleTrain *single,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGiveoutDirectionSel();

// �Ի�������
	enum { IDD = IDD_DLG_GIVEOUT_DIRECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

public:
	int m_nCount;
	CGridCtrl * m_pGridCtrl;
	CWintgDoc *m_pDoc;
	CSingleTrain *m_nTrain;

	void initialGridCtrl();
	void fillGrid();
    int abs_suited ;
	int abs_line_suited[32];
	CString sLineName[32];
	int exit_list[32];
	int nSelExit;
	afx_msg void OnBnClickedOk();
};

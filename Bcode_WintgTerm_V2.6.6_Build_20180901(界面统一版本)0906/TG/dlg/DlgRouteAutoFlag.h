#pragma once

#include "resource.h"
#include "gridcellcheck.h"
#include "tgdoc.h"
#include "uneditableGridCell.h"
#include "gridctrl.h"


// CDlgRouteAutoFlag �Ի���

class CDlgRouteAutoFlag : public CDialog
{
	DECLARE_DYNAMIC(CDlgRouteAutoFlag)

public:
	CDlgRouteAutoFlag(CTGDoc *pDoc, const CTG_TRAIN *single,CWnd* pParent = NULL);   // ��׼���캯��
	int m_nCount;
	CGridCtrl * m_pGridCtrl;
	CTGDoc *m_pDoc;
	CTG_TRAIN m_nTrain;

	void initialGridCtrl();
	void fillGrid();

	virtual ~CDlgRouteAutoFlag();

// �Ի�������
	enum { IDD = IDD_DLG_ROUTE_AUTO_FLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
private:
	void SetRouteAutoFlag(void);
public:
	afx_msg void OnBnClickedArrivalAll();
	afx_msg void OnBnClickedDepartAll();
};

#pragma once
#include "utility/xmlview.h"  //�������ؼ�����

// CPropertyPageJiaoBan �Ի���
class CTgDocument;
class CPropertyPageJiaoBan : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyPageJiaoBan)
	void initialGridCtrl();
//	CCFTimeEdit  m_cfTime;
	CXMLDataGrid   m_GridCtrl;
	CListBox       m_PlanList;
	int            m_nSectionNo;
	CTgDocument   *m_pDoc;
public:
	CPropertyPageJiaoBan(CTgDocument *pdoc);
	virtual ~CPropertyPageJiaoBan();
    BOOL    Add1Train(long train_index,const char *train_no);
// �Ի�������
	enum { IDD = IDD_DLG_JIAOBAN };
//{{AFX_VIRTUAL(CPageNotepad)
	virtual BOOL  OnInitDialog(); 
//}}AFX_VIRTUAL
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	CWnd *m_pNotifyWindow;
public:
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void InformParent();
};

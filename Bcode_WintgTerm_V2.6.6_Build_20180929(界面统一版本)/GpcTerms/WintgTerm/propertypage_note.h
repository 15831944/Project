#pragma once
#include "tgmsg.h"                  //��Ϣ�����ݽṹ����
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //�������ؼ�����

#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "Tooltipwnd.h"
#include ".\ext_src\CJFlatComboBox.h"  //office ����COMBOX
#include "Dlgmainfrm.h"
#include "afxdtctl.h"
#include "editex.hxx"
#include ".\GridCtrl_src\gridcellcheck.h"

class CTgDocument;
class CPageNotepad : public CPropertyPage
{
	//DECLARE_DYNCREATE(CPageNotepad)

// Construction
private:
	// Э��
	BOOL Query_Notebook();
	BOOL Set_Notebook();
public:
	void PrintNotepadDirectly();
	void initStationList();
	int nStationList[64];
	int stationCount;

	void GetGridData(CString &note_content);
	CString parsecontent[19];//��������ݶ�һ��
	void fillGrid(int start_pos);
	int m_nRowCount;
	void parseNotes();
	int m_nTimeValue_JiaoBan;
	void initialGridCtrl();
//	CCFTimeEdit  m_cfTime;
	CGridCtrl   *m_pGridCtrl;

	CEditEx   m_edit_jiaoban;
	CEditEx   m_edit; 
	CString  print_content;
	unsigned long select_shift_id;
	void OnPrint(CDC* pDC,CPrintInfo* pInfo);
	int schd_type;
	CTgDocument *m_pDoc;
	void InitIdentify();
	CPageNotepad(CTgDocument *pdoc);
	virtual ~CPageNotepad();

// Dialog Data
	//{{AFX_DATA(CPageNotepad)
	enum { IDD = IDD_PROPPAGE_NOTE };
	CString	m_notebook;
	CString	m_cfTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageNotepad)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageNotepad)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNotebook();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnSetfocusNotebook();
	afx_msg void OnSetfocusEditContent();
	afx_msg void OnSetfocusEditTime();
	afx_msg void  OnDestroy();
	//}}AFX_MSG
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CWnd *m_pNotifyWindow;
public:
	BOOL  ProcessMessage(const MsgNotePadFormat *pBuf);
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void InformParent();
};


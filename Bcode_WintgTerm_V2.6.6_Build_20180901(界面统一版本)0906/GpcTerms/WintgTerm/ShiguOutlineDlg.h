#if !defined(AFX_SHIGUOUTLINEDLG_H__324C8278_7966_4C7A_A7C3_C604B66A802C__INCLUDED_)
#define AFX_SHIGUOUTLINEDLG_H__324C8278_7966_4C7A_A7C3_C604B66A802C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShiguOutlineDlg.h : header file
//
//
#include "tgmsg.h"                  //��Ϣ�����ݽṹ����
#include "GridCtrl_src/GridCtrl.h"  //�������ؼ�����
#include "messages.h"
#include "function.h"
#include "memshare.h"

/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg dialog
// CDispatcherCommandModule dialog
 
#define ACCIDENT_MODULE_NUM		256
struct _AccidentModuleDetails
{
	char no[8];//���ͱ��
	char type_name[256];//��������
	char module_content[1800];//ģ������
};
struct _AccidentModule
{
	int module_num;//ģ���е���
	struct _AccidentModuleDetails details[ACCIDENT_MODULE_NUM];
};
#define MAX_ACCIDENT 10
 

class CShiguOutlineDlg : public CDialog
{
// Construction
public:
	CShiguOutlineDlg(int section_id,int shiftid,int schd_type,CWnd* pParent = NULL);   // standard constructor
	CGridCtrl *m_pGridCtrl;
    int		m_nFixCols;   //�̶���
	int		m_nFixRows;   //�̶���
	int		m_nCols;      //��
	int		m_nRows;      //��
	int     nAccident;
	int     CurrentRow;
	int     schdtype;   
	int 	m_RecordsPerPrintPage ;
	int     m_nSectionID,m_nShiftID;

// Dialog Data
	//{{AFX_DATA(CShiguOutlineDlg)
	enum { IDD = IDD_DLG_SHIGU_OUTLINE };
	CComboBox	m_Ctrlmodule;
	CEdit	m_EditDetail;
	CEdit	m_Newword;
	CListBox	m_List;
	CStatic	m_Frame;
	CEdit* g_curEdit;//��ǰ���ôʻ�Ҫ����ı༭��
	CString	m_title;
	CString	m_detail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShiguOutlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
		void InitIdentify();
	bool   ReadModuleFile(char *lpszFileName);
	void SortModuleFile();
	void PrintEveryPageInfo(CDC *pDC, CPrintInfo *pInfo);
	int GetMaxIndex();
    void Query_Accident();
	void Del_Accident(ushort nIndex);
	void Add_OR_Change_Accident(int type,ushort nIndex);
	// Generated message map functions
	//{{AFX_MSG(CShiguOutlineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddNewDetail();
	afx_msg void OnWordEdit();
	afx_msg void OnWordDelete();
	afx_msg void OnDblclkList();
	afx_msg void OnWordAdd();
	afx_msg LRESULT OnAccidentMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnPrint();
	afx_msg void OnSelendokCmbModule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SendMsgAccidentOper(ushort nOpetype,ushort nIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHIGUOUTLINEDLG_H__324C8278_7966_4C7A_A7C3_C604B66A802C__INCLUDED_)

#pragma once
// CPropertyPageNotepad.h : header file
//
#include "tgmsg.h"                  //��Ϣ�����ݽṹ����
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //�������ؼ�����
#include ".\StplanGrid.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "Tooltipwnd.h"
#include ".\ext_src\CJFlatComboBox.h"  //office ����COMBOX

#include "afxdtctl.h"
#include "editex.hxx"
#include ".\GridCtrl_src\gridcellcheck.h"
#include "utility/pspview.h"
#include "propertypage_shigu.h"
#include "propertypage_note.h"
#include "propertypage_phasenoteplan.h"
#include "PropertyPageJiaoBan.h"
#include "sendstationplandlgex.h"
#include "propertypage_subao.h"
#include "PropertyPage_ShuaiGua.h"
class CTgDocument;
//#include "GridLCBZ.h"	//�̳���CGRIDCTRL���࣬�����г�������

//CCJFlatComboBox	m_sideCtrl;
//CToolTipWnd m_ctrlTooltipWnd;

//��̬���ɵ�5���ؼ������ڳ��ôʻ������
#define IDC_EDITNEWWORD 5000 //�´ʻ������
#define IDC_BUTTONADD 5001	//���Ӱ�ť
#define IDC_BUTTONDEL 5002	//ɾ����ť
#define IDC_LISTALLWORDS 5003	//�б��
#define IDC_BUTTONEDIT 5004	//�ֹ��༭��ť
#define IDC_LISTSCHDINFO 5005
#define IDC_WORD_TABCTRL 5006
#define IDC_STPLAN_WARN 5025

#define IDC_BTN_CHAOXIAN 5007
#define IDC_BTN_QUJIANZUOYE   5008
#define IDC_BTN_JUNTEYUN    5009
#define IDC_CBO_STARTTIME   5010
#define IDC_CBO_ENDTIME   5011
#define IDC_BTN_SHUAIGUA    5012
#define IDC_BTN_SHIGONG     5013
#define IDC_BTN_MY_CANCEL   5014
#define IDC_BTN_ALL         5015

#define PHASENOTE_PAGE 0
#define PHASENOTE_SEND_PAGE 1
#define SHIGU_PAGE 2
#define JIAOBAN_PAGE  3
#define STPLAN_PAGE   4
#define SUBAO_PAGE    5
#define SHUAIGUA_PAGE    6

class CPropertySheetNotepad : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetNotepad)

	int   m_nLoadSchdInfoFlag;
	void  LoadSchdInfo(int flag,TIME starttm=0,TIME endtm=0); 

	int m_nCurActivePage;

	int m_aPageID[20];
	int m_nPageCount;
	int GetCurActiveUserPage();
public :
    CPageNotepad        m_page_notepad;
	CPageNotepadPlan    m_page_phasenote_plan;
	CShiguOutlineDlg    m_page_shigu_outline;
	CPropertyPageJiaoBan m_page_jiaoban;
	CSendStationPlanDlgEx m_page_send_stplan;
	CPageSUBAO            m_page_subao;
	CPropertyPage_ShuaiGua m_page_shuaigua;
	CTabCtrl            m_tab; // �����л����ôʻ�͵��������Ϣ
	CTgDocument *m_pDoc;
	
	BOOL                m_bEnableWordSelected;
	BOOL                m_bEnableScheduleInfoSelected;
	BOOL				m_bEnableStPlanWaring;
	// Construction
public:
	void SetBasicSchduleInfo();  // ���ɻ���ͼ��Ϣ
	void SendStplanOnlySelLIRC();
	void SelAllStplanRecEntity();
	void SendNoticeToDCmd();
	CPropertySheetNotepad(LPCTSTR pszCaption,CTgDocument *pDoc,int schdtype);
	void  AddUserPage(int index);
	CPropertySheetNotepad(int page_choice,LPCTSTR pszCaption,CTgDocument *pDoc,int schdtype);
	CPropertyPage *GetUserPage(int page_id);
	BOOL  m_bModaless; // ��ģʽ�Ի���
// Attributes
public:
	CEdit m_newword; //�����´ʻ�
	CListBox m_listallwords;//���ôʻ��б��
	CButton m_buttonadd;//��Ӱ�Ŧ
	CButton m_buttondel;//ɾ����Ŧ
	CButton m_buttonedit;//�༭��ť

	CComboBox m_cboStartTime,m_cboEndTime;
	CStatic m_txtStartTime;
	CStatic m_txtEndTime;
    CButton m_buttonCHAOXIAN;//������Ϣ��Ŧ
	CButton m_buttonQUJIANZUOYE;//������ҵ
	CButton m_buttonJUNTEYUN;//���ص���Ϣ
	CButton m_buttonSHUAIGUA;//������ҵ
	CButton m_buttonSHIGONG;//���ص���Ϣ
	CButton m_buttonAll;// ȫ���Զ�����



	CEdit* m_focusedit;	//Ҫ�����ı����Ǹ��ؼ���ָ��

	CListBox m_listSchdInfo;//������Ϣ�б��
	
	CButton m_buttonCancel;//���ص���Ϣ

	CStplanGrid m_stplangrid; // �ƻ�����
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetNotepad)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation
public:
	unsigned long select_shift_id;
	
	void OnRecievePlanDCmdResult(int result, CString warning, CString id);
	void InitIdentify();
	unsigned long GetToday();
	unsigned long Getnextday();
	virtual ~CPropertySheetNotepad();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySheetNotepad)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT ChangeStationReceiveStatus(WPARAM wParam,LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkListallwords();
	afx_msg void OnDblclkSchdInfo();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnSelchangeSbtEndtime(); 
	afx_msg void OnSelchangeSbtStarttime(); 
	afx_msg LRESULT OnSchdTrainSelected(WPARAM wp,LPARAM lp);
	//}}AFX_MSG
	void OnQujianZuoyeInfo();
	void OnJunteYunInfo();
	void OnChaoXianInfo();
	void OnShiGongInfo();
	void OnShuaiGuaInfo();
	void OnAllInfo();
	
	DECLARE_MESSAGE_MAP()
	TIME  m_nStarttime, m_nEndtime;
	BOOL  m_bLoadPassengerTrain;
	BOOL  m_bLoadFreightTrain;
	int   m_nStationNo,m_nSectionNo;
	enum{INFO_TYPE_TEYUN=1,INFO_TYPE_CHAOXIAN=2,INFO_TYPE_QUJIANZUOYE=3,INFO_TYPE_SHUAIGUA=4,INFO_TYPE_SHIGONG=5};
	void  GenSchInfoString(CString &dest,int flag,TIME starttm,TIME endtm);
	bool m_bReSize;

public:
	bool m_HideWnd;
};

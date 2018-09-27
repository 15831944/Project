
#ifndef __TCC_DLG_H__
#define __TCC_DLG_H__

#include "tcc_base.h"
#include "afxwin.h"
//#include "atltime.h"
#include "afxdtctl.h"
#include "afxcmn.h"

#include <Tcc\Grid.h>
#include <Tcc\TabSheet.h>

#include <list>
using namespace std;

#pragma once

namespace TCC_C3 {

class CStccStatusGrid : public CGridCtrl {
	DECLARE_DYNAMIC(CStccStatusGrid)

public:
	CStccStatusGrid();
	virtual ~CStccStatusGrid();

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

public:
	void SetStatus(TCC_STATUS& rec);
	void ClearStatus();

private:
	// Generated message map functions
	//{{AFX_MSG(CStccStatusGrid)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CTccDlg;

// CLocalCommandPage �Ի���
class CLocalCommandPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLocalCommandPage)

public:
	CLocalCommandPage();
	virtual ~CLocalCommandPage();

// �Ի�������
	enum { IDD = IDD_TCC_LOCALCMD_MGR };

public:
	void Tcc_SetCmdStatus(DWORD nSysID, DWORD nCmdID, DWORD nTccID, DWORD nStatus);
	void Refresh();
	RAW_STOCK_COMMAND* GetRawCommand(sysid_t& raw_sysid);
	void AppendCommand(RAW_STOCK_COMMAND& cmd);

private:
	typedef list<RAW_STOCK_COMMAND> TCC_CMD_LIST;
	TCC_CMD_LIST listTcc;
	TCC_CMD_LIST& GetTccCmdList();
	ITccAdviseSink* GetTccAdviseSink();
	ITccClientInfoProvider* GetTccClientInfoProvider();
	WORD GetCurrentTccID();
	CTccDlg* GetTccDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	void InitListCtrl(CGridCtrl& grid);

	void ShowCmd(RAW_STOCK_COMMAND *pcmd = NULL);

	BOOL Load(LPCSTR szFilename);

private:
	RAW_STOCK_COMMAND *m_pCurCmd;	// ��ǰ����

private:
	// �����б�
	CGridCtrl m_CmdList;
	CString m_CmdString;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDelRawStockCommand();
	afx_msg void OnBtnClickedRefresh();
	afx_msg void OnNMClickTccCmdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnDestroy();
};

// CConfirmCommandPage �Ի���
class CConfirmCommandPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfirmCommandPage)

public:
	CConfirmCommandPage();
	virtual ~CConfirmCommandPage();

// �Ի�������
	enum { IDD = IDD_TCC_CONFIRM_COMMAND };

public:
	void AppendCommand(STOCK_COMMAND& cmd);
	void Refresh();
	STOCK_COMMAND* GetCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID);
	void SetCommandOperResult(TCC_COMMAND& cmd, DWORD nStatus, LPCSTR strComment);

private:
	bool GetExecuteRight();
	bool GetEditRight();
	ITccAdviseSink* GetTccAdviseSink();
	ITccClientInfoProvider* GetTccClientInfoProvider();
	stano_t GetCurrentTccID();
	CTccDlg* GetTccDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	void ShowCmd(STOCK_COMMAND *pcmd = NULL);

private:
	//��ѯ��Ŀ��վ
	//CComboBox m_TccList;

	//��ѯ��ʱ�䷶Χ
	// ��ʼʱ��
	INT m_bTimeBegin;
	CDateTimeCtrl m_tmBegin;
	CButton m_BeginRadio;
	CButton m_BeginRadio2;
	// ����ʱ��
	INT m_bTimeEnd;
	CDateTimeCtrl m_tmEnd;
	CButton m_EndRadio;
	CButton m_EndRadio2;

	// �����б�
	CGridCtrl m_CmdList;
	STOCK_COMMAND *m_pCurCmd;	// ��ǰ����
	CString m_CmdString;
	CEdit m_OperPrompt;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickTccCmdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSendTccCommand();
	afx_msg void OnBtnClickedTccRefresh();
	afx_msg void OnDelPartialStockCommand();
	afx_msg void OnDelStockCommand();
	afx_msg void OnCancel();
	afx_msg void OnBtnClickedTccCmdTimeendRadio2();
	afx_msg void OnBtnClickedTccCmdTimeendRadio();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio2();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio();
	afx_msg void OnTccCommandStatusChange();
};

// CHistoryCommandPage �Ի���
class CHistoryCommandPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CHistoryCommandPage)

public:
	CHistoryCommandPage();
	virtual ~CHistoryCommandPage();

// �Ի�������
	enum { IDD = IDD_TCC_HISTORY_COMMAND };

public:
	void AppendCommand(STOCK_COMMAND& cmd);
	void Refresh();
	STOCK_COMMAND* GetCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID);

private:
	ITccAdviseSink* GetTccAdviseSink();
	ITccClientInfoProvider* GetTccClientInfoProvider();
	stano_t GetCurrentTccID();
	CTccDlg* GetTccDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	void InitListCtrl(CGridCtrl& grid);
	void ShowCmd(STOCK_COMMAND *pcmd = NULL);

private:
	//��ѯ��Ŀ��վ
	//CComboBox m_TccList;

	//��ѯ��ʱ�䷶Χ
	// ��ʼʱ��
	INT m_bTimeBegin;
	CDateTimeCtrl m_tmBegin;
	CButton m_BeginRadio;
	CButton m_BeginRadio2;
	// ����ʱ��
	INT m_bTimeEnd;
	CDateTimeCtrl m_tmEnd;
	CButton m_EndRadio;
	CButton m_EndRadio2;

	// �����б�
	CGridCtrl m_CmdList;
	STOCK_COMMAND *m_pCurCmd;	// ��ǰ����
	CString m_CmdString;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickTccCmdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnBtnClickedTccRefresh();
	afx_msg void OnDelPartialStockCommand();
	afx_msg void OnDelStockCommand();
	afx_msg void OnCancel();
	afx_msg void OnBtnClickedTccCmdTimeendRadio2();
	afx_msg void OnBtnClickedTccCmdTimeendRadio();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio2();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio();
	afx_msg void OnTccCommandStatusChange();
};


// CLircCommandPage �Ի���

class CLircCommandPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLircCommandPage)

public:
	CLircCommandPage();
	virtual ~CLircCommandPage();

// �Ի�������
	enum { IDD = IDD_TCC_LIRC_CMD_TABLE };

public:
	bool GetExecuteRight();
	bool GetCancelRight();
	void AppendCommand(TCC_COMMAND& cmd, DWORD nStatus);
	void Tcc_Report(DWORD nTccID, TCC_STATUS& status);
	void Refresh();

private:
	ITccAdviseSink* GetTccAdviseSink();
	ITccClientInfoProvider* GetTccClientInfoProvider();
	stano_t GetCurrentTccID();
	CTccDlg* GetTccDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	enum CMDLIST_COL {
		COL_CMDID = 0,
		COL_BEGINSTA,
		COL_ENDSTA,
		COL_ABSLINEID,
		COL_START_KM,
		COL_END_KM,
		COL_SPEEDVAL,
		COL_START_TIME,
		COL_END_TIME,
	};
	void InitListCtrl(CListCtrl *pListCtrl);

private:
	// �����б�
	CHeaderList m_CmdList;
	CStccStatusGrid m_StccStatusGrid;
	BOOL m_bInitComplete;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnRequestTccCmdList();
	afx_msg void OnTccInitComplete();
	afx_msg void OnTccCommand_FullCancel();
	afx_msg void OnCancelLimitSpeed();
	afx_msg void OnCancelLimitSpeedDirect();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnNMDBClickCmdList(NMHDR *pNMHDR, LRESULT *pResult);

	void TccInitComplete(stano_t nTccID);
};


#define UM_RECEIVE_TCC_COMMAND			(WM_USER + 202)
#define UM_RECEIVE_TCC_REPORT           (WM_USER + 203)
#define UM_RECEIVE_CONFIRM_COMMAND		(WM_USER + 204)
#define UM_CLIENTENV_CHANGED			(WM_USER + 205)
#define UM_RECEIVE_TCC_COMMAND_RESULT	(WM_USER + 206)
#define UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT	(WM_USER + 207)
#define UM_TCC_INITCOMPLETE             (WM_USER + 208)

class CTccDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccDlg)
public:
	CTccDlg();
	CTccDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTccDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CTccDlg();
	virtual BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1,
		DWORD dwExStyle = 0);
	enum { IDD = IDD_TCC };

private:
	CLircCommandPage	m_LircCommandPage;
	CConfirmCommandPage m_ConfirmCommandPage;
	CHistoryCommandPage m_HistoryCommandPage;
	CLocalCommandPage	m_LocalCommandPage;
	BOOL m_bAutoDelete;

	CTabSheet m_sheet;
	CComboBox m_TccList;
	stano_t  m_nCurrentTccID;

private:
	ITccAdviseSink *m_pTccAdviseSink;
	ITccClientInfoProvider *m_pTccClientInfoProvider;
	//friend class CLocalCommandPage;
	//friend class CLircCommandPage;
	//friend class CConfirmCommandPage;

public:
	virtual BOOL OnInitDialog();

public:
	void SetInterface(ITccAdviseSink *pTccAdviseSink, ITccClientInfoProvider *pInfoProvider);
	ITccClientInfoProvider* GetTccClientInfoProvider() { return m_pTccClientInfoProvider; }
	ITccAdviseSink* GetTccAdviseSink() { return m_pTccAdviseSink; }
	stano_t GetCurrentTccID() { return m_nCurrentTccID; }

	STOCK_COMMAND* GetConfirmCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID);
	STOCK_COMMAND* GetLocalCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID);

public:
	// �յ�ʵʱ�п����������б�
	afx_msg LRESULT OnReceiveTccCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnReceiveTccLineInitialCommandResult(WPARAM wParam, LPARAM lParam);

	// ���ɻ�������п�״̬
	afx_msg LRESULT OnReceiveTccReport(WPARAM wParam, LPARAM lParam);
	// �յ����������ص�ȷ�������б�
	afx_msg LRESULT OnReceiveConfirmCommand(WPARAM wParam, LPARAM lParam);

	// ���ػ��������仯�����л������ε�
	afx_msg LRESULT OnClientEnvChanged(WPARAM wParam, LPARAM lParam);

	//����������Ҫ��ʼ���п�
	afx_msg LRESULT OnTccInitComplete(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void PostNcDestroy();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
public:
	afx_msg void OnDestroy();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSelectTcc();
	afx_msg void OnRefreshTcc();
	afx_msg void OnDirectLimit();
};

};

#endif

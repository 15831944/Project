#if !defined(AFX_ABSELEFIXMANAGE_H__3C27F1B3_0823_4B2A_A0B3_1F97B286B297__INCLUDED_)
#define AFX_ABSELEFIXMANAGE_H__3C27F1B3_0823_4B2A_A0B3_1F97B286B297__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsEleFixManage.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixManage dialog
#define MAX_TIANCHUANG_NUM		256
#define MAX_ABS_NUM		64
struct _tagAbsEleFixDetails
{
	int nType;//���� 11
	int nTianChuangNo;//�촰���
	char tianchuang_title_name[256];//�촰��������
	
	int nNode1Index;
	int nNode2Index;
	int nNode1StationNo;
	int nNode2StationNo;
	int nSelectFenXiangFlag;//�û�ѡ��ķ���
	int nRealBlockFlag;     //��ʵ�ķ�����־
	int nSelectBeginAbsIndex;
	int nSelectEndAbsIndex;
	int nRealBeginAbsIndex;
	int nRealEndAbsIndex;

	int nFaceSelectBlockFlag;
};
struct _tagAbsEleFixModule
{
	int tianchuang_num;//�촰��
	struct _tagAbsEleFixDetails tianchuang_details[MAX_TIANCHUANG_NUM];
};

struct _tagAbsInfo
{
	int nAbsIndex;
	int nNode1;
	int nNode1StationNo;
	char szNode1Name[32];
	int nNode2;
	int nNode2StationNo;
	char szNode2Name[32];

	char szAbsName[64];//szNode1Name+szNode1Nam2
};

//bool ReadTianChuangModuleFile(char *lpszFileName,struct _tagAbsEleFixModule *t);
//void SortTianChuangModuleFile(struct _tagAbsEleFixModule *t);

class CAbsEleFixManage : public CDialog
{
// Construction
public:
	void LoadTianChuangFirst();
	bool ReadFromSpecifyFile(char *filename,struct _tagAbsEleFixDetails *t);
	bool CreateTwoModuleFile(char *filename,struct _tagAbsEleFixDetails *item);
	BOOL ChangeFilem_typelistWithNeighbour(int nDirect);

	struct _tagAbsEleFixModule m_AbsEleFixModule;

	CAbsEleFixManage(CTGDoc *pDoc,CWnd* pParent);   // standard constructor
	CTGDoc *m_pDoc;

// Dialog Data
	//{{AFX_DATA(CAbsEleFixManage)
	enum { IDD = IDD_ABS_ELE_FIX_MANAGE };
	/*CDateTimeCtrl	m_ctlEndTime;
	CDateTimeCtrl	m_ctlStartTime;
	CComboBox	m_comboEndAbs;
	CComboBox	m_comboBeginAbs;*/
	CString	m_strTianChuangName;
	CString	m_strTianChuangNo;
	//}}AFX_DATA
	char* CheckResult(int row);
	void SetControlEnable(BOOL bFlag, BOOL bTianChuangNoFlag = TRUE);
	bool ReadTianChuangModuleFile(char *lpszFileName,struct _tagAbsEleFixModule *t);
	bool WriteTianChuangModuleFile(char *lpszFileName);
	void SortTianChuangModuleFile(struct _tagAbsEleFixModule *t);
	BOOL InitAbsInfo();
	BOOL InitListInfo();
	void InitControl();
	void ShowRecord(int nCurrentSel);//��ʾ�б��е�ǰ��ѡ����
	BOOL SaveTianChuangInfoToFile(int nAddOrModify);
	int m_nCurrentSel;//��ʾ�б��е�ǰ��ѡ����

	int m_nTianChuangFileNum;
	USHORT m_usCommQueueHandle;

	int m_nAbsNum;
	struct _tagAbsInfo m_absInfo[MAX_ABS_NUM];

	int nFenXiangPos;//����λ��.1 �Ͻ��½���2�Ͻ���Զ��3 ��Զ�½���4 ��Զ��Զ
	int nCheckUpAndDownFlag;//�Ƿ����·�վ. 0 ��������1 ���Ϸ�վ��2 ���·�վ��3 ���¶���

	long usStartDateValue,usEndDateValue, usStartTimeValue,usEndTimeValue;//�û�ѡ��ʱ��
	int station,index;//�ڵ�
    int station_no1, station_no2; //վ��
	//�����Ƕ�д�ļ�ʱ���ֶ�
	int m_FileNode1Index;
	int m_FileNode2Index;
	int m_FileNode1StationNo;
	int m_FileNode2StationNo;
	int m_FileSelectFenXiangFlag;//�û�ѡ��ķ���
	int m_FileRealBlockFlag;//��ʵ�ķ�����־
	int m_FileSelectBeginAbsIndex;
	int m_FileSelectEndAbsIndex;
	int m_FileRealBeginAbsIndex;
	int m_FileRealEndAbsIndex;

	int m_FileFaceSelectBlockFlag;//����ѡ��ĸ�ѡ��(���Ϸ�վ,���·�վ)�ı�־
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsEleFixManage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsEleFixManage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAddSave();
	afx_msg void OnRadioSjxj();
	afx_msg void OnRadioSjxy();
	afx_msg void OnRadioSyxj();
	afx_msg void OnRadioSyxy();
	afx_msg void OnCheckIncludeDown();
	afx_msg void OnCheckIncludeUp();
	afx_msg void OnBtnInsertToGraph();
	afx_msg void OnBtnModifySave();
	afx_msg void OnBtnCancelOper();
	afx_msg void OnBtnDelete();
	afx_msg void OnSelchangeComboBeginAbs();
	afx_msg void OnSelchangeComboEndAbs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnClose();
};
//extern CWintgForm *m_pWintgForm;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSELEFIXMANAGE_H__3C27F1B3_0823_4B2A_A0B3_1F97B286B297__INCLUDED_)

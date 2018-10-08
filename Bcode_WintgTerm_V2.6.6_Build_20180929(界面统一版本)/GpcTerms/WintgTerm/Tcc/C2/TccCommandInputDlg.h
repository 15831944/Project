#include "tcc.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <Tcc\Grid.h>
#include "afxwin.h"

#pragma once

namespace TCC {

// CTccCommandInputDlg �Ի���

class CTccCommandInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccCommandInputDlg)

public:
	CTccCommandInputDlg(RAW_STOCK_COMMAND *pcmd, 
		BOOL bCheckCommand,
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTccCommandInputDlg();

// �Ի�������
	enum { IDD = IDD_TCC_COMMAND_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	RAW_STOCK_COMMAND m_cmd;
	BOOL  m_bCheckCommand;
	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;
	
	BOOL  m_bManualChangeCommandID;
	DWORD dwServerConfirmCookie;
	bool  m_bCmdIDConfirmed;	//�������Ƿ���ȷ��
public:
	RAW_STOCK_COMMAND* GetCommand() { return &m_cmd; }

private:

	enum {
		STATION_LIMIT,	//վ������
		ABSAPP_LIMIT,	//��������
		ABSAPP_FULL_LIMIT,	//����ȫ������
		BLOCK_LIMIT,	//��������
	};
	// ��������
	CComboBox m_CmdType;

	// ���������
	DWORD m_nCmdID;
	CEdit m_CmdID;
	CButton m_btnChangeCmdID;
	CButton m_btnAutoComputeCmdID;//�Զ�ѡ��

	// �Ƿ�ȡ������
	BOOL m_bCancelLimit;
	CButton m_CancelLimit;
	// �Ƿ�����ȡ������
	BOOL m_bGenCancelCommand;
	CButton m_GenCancelCommand;
	// ��ʼվ, ����վ
	CComboBox m_StaBegin, m_StaEnd;
	// ��·, ����ֵ
	CComboBox m_AbsLine;
	// ��ʼ�����
	int m_nKmBegin;
	int m_nMBegin;
	CEdit m_KmBegin;
	CEdit m_MBegin;
	// ���������
	int m_nKmEnd;
	int m_nMEnd;
	CEdit m_KmEnd;
	CEdit m_MEnd;
	CComboBox m_StartLongKM;
	CComboBox m_StartLongM;
	CComboBox m_EndLongKM;
	CComboBox m_EndLongM;
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

	// ����ֵ
	CComboBox m_LimitVal[5];
	CButton   m_Limit[5];	//m_Limit[0]û��,������Ϊ����������
	CEdit     m_TrainID[5];

	// ��������
	CString m_CmdString;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	afx_msg void OnCbnSelchangeTccCmdSta();
	afx_msg void OnCbnSelchangeTccCmdSta2();

	afx_msg void OnBtnClickedTccCmdTimeendRadio2();
	afx_msg void OnBtnClickedTccCmdTimeendRadio();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio2();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio();
	afx_msg void OnCbnSelchangeTccCmdType();
	afx_msg void OnCbnSelchangeTccLine();
	afx_msg void OnBtnClickedLimit2();
	afx_msg void OnBtnClickedLimit3();
	afx_msg void OnBtnClickedLimit4();
	afx_msg void OnBtnClickedLimit5();
	afx_msg void OnSelCancelLimit();
	afx_msg void OnChangeTccCmdID();
	afx_msg void OnBtnClickedAutoComputeCmdID();

	afx_msg LRESULT OnServerConfirmCommandID(WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnOK();
	virtual void OnCancel();

private:
	void RequestCommandID(cmdid_t nCmdID);

	CGridCtrl m_TccList;
	void InitListCtrl(CGridCtrl& grid);
};


};
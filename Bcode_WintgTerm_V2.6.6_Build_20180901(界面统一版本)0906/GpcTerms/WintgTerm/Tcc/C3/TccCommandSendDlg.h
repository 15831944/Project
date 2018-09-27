#if !defined(TccCommandSendDlg_H)
#define TccCommandSendDlg_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TccCommandSendDlg.h : header file
//

#include "tcc.h"

namespace TCC_C3
{

// CTccCommandSendDlg �Ի���

class CTccCommandSendDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccCommandSendDlg)

public:
	enum {
		TCC_COMMAND_DIRECT,	//ֱ������
		TCC_COMMAND_CANCEL,	//ȡ������
		TCC_COMMAND_FULLCANCEL,		//ǿ��ȫ��������
		TCC_COMMAND_INITCOMPLETE,	//��ʼ�����
		TCC_COMMAND_CONFIRM,//ȷ���б�������
	};
	CTccCommandSendDlg(TCC_COMMAND& cmd, int op_flag,
		STOCK_COMMAND *pStockCmd,
		ITccClientInfoProvider *pInfoProvider,
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTccCommandSendDlg();

// �Ի�������
	enum { IDD = IDD_DLG_TCC_COMMAND_SEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	int m_opflag;
	STOCK_COMMAND *m_pStockCmd;
	TCC_COMMAND m_TccCommand;
	BOOL m_bCanExecuteTccCommand;

	BOOL m_bCanClose;

	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;

	DECLARE_MESSAGE_MAP()
	LRESULT OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam);
public:
	CString m_sContent,m_sStatus;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnTryExecute();
};

}

#endif //if !defined(TccCommandSendDlg_H)
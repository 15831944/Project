#pragma once
#include "afxwin.h"


// CInputReturnTrainDlg �Ի���

class CInputReturnTrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputReturnTrainDlg)

public:
	CInputReturnTrainDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex);   // ��׼���캯��
	virtual ~CInputReturnTrainDlg();

// �Ի�������
	enum { IDD = IDD_DLG_INPUT_RETURN_ID };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	CStringArray m_list_trainno;
	CDWordArray m_list_trainindex;

public:
	long m_return_trainindex;
};

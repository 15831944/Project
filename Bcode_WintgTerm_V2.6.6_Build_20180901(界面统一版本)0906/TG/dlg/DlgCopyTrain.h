#pragma once


// CDlgCopyTrain �Ի���

class CDlgCopyTrain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCopyTrain)

public:
	CDlgCopyTrain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCopyTrain();

// �Ի�������
	enum { IDD = IDD_DLG_COPY_TRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_trainId;
};

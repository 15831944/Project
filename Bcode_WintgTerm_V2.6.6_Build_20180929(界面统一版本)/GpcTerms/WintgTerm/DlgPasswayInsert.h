#pragma once


// CDlgPasswayInsert �Ի���

class CDlgPasswayInsert : public CDialog
{
	DECLARE_DYNAMIC(CDlgPasswayInsert)

public:
	CDlgPasswayInsert(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPasswayInsert();

// �Ի�������
	enum { IDD = IDD_DLG_PASSWAY_INSERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��·��
	short m_nPasswayId;
	// ��·����
	CString  m_nPasswayName;
	afx_msg void OnBnClickedOk();
};

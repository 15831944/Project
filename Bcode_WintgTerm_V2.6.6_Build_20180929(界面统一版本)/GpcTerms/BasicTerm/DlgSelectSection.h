#pragma once


// CDlgSelectSection �Ի���

class CDlgSelectSection : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectSection)

public:
	CDlgSelectSection(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSelectSection();
	int   m_nNeighborOfSection;
	int   m_nSectionNo;
// �Ի�������
	enum { IDD = IDD_SELECT_SECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog(void);
public:
	afx_msg void OnBnClickedOk();
};

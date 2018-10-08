#pragma once


// CDlgWordMaintain �Ի���

class CDlgWordMaintain : public CDialog
{
	DECLARE_DYNAMIC(CDlgWordMaintain)

public:
	CDlgWordMaintain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWordMaintain();

// �Ի�������
	enum { IDD = IDD_DLG_WORD_MAINTAIN };
	CEdit m_newword; //�����´ʻ�
	CListBox m_listallwords;//���ôʻ��б��
	CButton m_buttonadd;//��Ӱ�Ŧ
	CButton m_buttondel;//ɾ����Ŧ
	CButton m_buttonedit;//�༭��ť
	CComboBox m_cbotype;

	CString   m_szFileName;
	BOOL      m_bEdited;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnWordAdd();
	afx_msg void OnBnClickedBtnWordEdit();
	afx_msg void OnBnClickedBtnWordDelete();
	afx_msg void OnSelTypeChange();
	afx_msg void OnSelWordChange();
	afx_msg void OnSave();
	BOOL OnInitDialog();
	void RefreshList();
	void OnOK();
};

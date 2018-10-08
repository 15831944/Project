#pragma once


// CDlgMarkResult �Ի���

class CDlgMarkResult : public CDialog
{
	DECLARE_DYNAMIC(CDlgMarkResult)
	CString  m_szCaption;
	CString  m_szInform;
public:
	CDlgMarkResult(const char *caption,const char *inform,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMarkResult();
	const char *GetInputValue();
	// �Ի�������
	enum { IDD = IDD_DLG_MMPLAN_MARK_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();
	void OnOK();
	DECLARE_MESSAGE_MAP()
};

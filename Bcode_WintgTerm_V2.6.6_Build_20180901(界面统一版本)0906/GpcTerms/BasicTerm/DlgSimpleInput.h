#pragma once


// CDlgSimpleInput �Ի���

class CDlgSimpleInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgSimpleInput)
	CString  m_szCaption;
	CString  m_szInform;
	CString  m_szValue;
public:
	CDlgSimpleInput(const char *caption,const char *inform,const char *defvalue,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSimpleInput();
	const char *GetInputValue();
// �Ի�������
	enum { IDD = IDD_DLG_SIMPINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();
	void OnOK();
	DECLARE_MESSAGE_MAP()
};

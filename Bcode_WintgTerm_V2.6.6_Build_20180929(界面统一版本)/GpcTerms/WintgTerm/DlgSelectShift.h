#pragma once


// CDlgSelectShift �Ի���
class CTgDocument;
class CDlgSelectShift : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectShift)
	CTgDocument *m_pDoc;
	int m_nNextShiftID;
public:
	CDlgSelectShift(CWnd* pParent, int nShiftID);   // ��׼���캯��
	virtual ~CDlgSelectShift();


// �Ի�������
	enum { IDD = IDD_DLG_SELECT_SHIFT };
 
	int   m_nShiftID;
	
	TIME  m_ntStartTime;
	TIME  m_ntEndTime;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
public:
	void OnOK();
};

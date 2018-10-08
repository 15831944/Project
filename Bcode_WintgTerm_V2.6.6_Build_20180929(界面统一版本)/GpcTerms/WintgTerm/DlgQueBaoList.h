#pragma once
#include "utility/xmlview.h"

// CDlgQueBaoList �Ի���
class CWorkScheduleHolder;
//class CTG_TrainScheduleBase;
class CTgDocument;
class CDlgQueBaoList : public CDialog
{
	DECLARE_DYNAMIC(CDlgQueBaoList)
	CXMLDataGrid m_grid;
	CXMLDataGrid m_gridHeader,m_gridZhongche,m_gridQingche;
	CString m_szTrainNo;
	DWORD   m_TrainGTID;
	int     m_nStationID;
public:

	CDlgQueBaoList(int stno,const char *trainno,DWORD gtid,CWnd* pParent = NULL);   // ��׼���캯��
	 virtual ~CDlgQueBaoList();

// �Ի�������
	enum { IDD = IDD_DLG_QUE_BAO_LIST };
   
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	void         OnOK();
	void    OnBnClickedReq();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
    void OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRequestDepartData();
};

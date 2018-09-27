#pragma once
#include "markgrid.h"

// CDlgMarkList �Ի���
class CWorkScheduleHolder;
class CTG_TrainScheduleBase;
class CDlgMarkList : public CDialog
{
	DECLARE_DYNAMIC(CDlgMarkList)
	CMarkGridView m_grid;
	CWorkScheduleHolder *m_pHolder;
	CTG_TrainScheduleBase *m_pData;
	int    m_nCurrentSel;
	CString m_szCaption;
public:
	int    m_nScheduleType;
public:
	CDlgMarkList(const char *caption,CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CDlgMarkList();

	void  SetScheduleHolder(CWorkScheduleHolder *p) {m_pHolder=p;}
	void  SetScheduleType(int tp) {m_nScheduleType=tp;}
// �Ի�������
	enum { IDD = IDD_DLG_MARK_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	void         OnOK();
	void    OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
	DECLARE_MESSAGE_MAP()
};

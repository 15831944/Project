#pragma once

#include "traingrid.h"
#include "tg_schedule_base.h"
// CDlgMultiAdjust �Ի���

class CDlgMultiAdjust : public CDialog
{
	DECLARE_DYNAMIC(CDlgMultiAdjust)

	CTrainGridView  m_grid;
	
	BOOL            m_bAllSelected;
public:
	CDlgMultiAdjust(CTG_TrainScheduleBase *psourcebase, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMultiAdjust();

// �Ի�������
	enum { IDD = IDD_DLG_AUTO_ADJUST };
	CTG_TrainScheduleBase  *m_pSourceDataBase;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnQuery();
	afx_msg void OnBnClickedChkAll();
	afx_msg void OnBnClickedOk();
	BOOL    OnInitDialog(); 

public:
	int    GetSelectedTrains(TRAIN_INDEX  buffer[],int buffersize);
};

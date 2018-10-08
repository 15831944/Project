#pragma once

#include "tg_message_define.h"
#include ".\tgframe.h"

// CDlgFindTrainResult �Ի���

class CDlgFindTrainResult : public CDialog
{
	DECLARE_DYNAMIC(CDlgFindTrainResult)

public:
	CDlgFindTrainResult(int tg_type, int TrainCnt, CString TrainId, long PostionList[],long PTrainIdList[],long PTimeList[], CWnd* pParent);   // ��׼���캯��
	virtual ~CDlgFindTrainResult();

// �Ի�������
	enum { IDD = IDD_DLG_FIND_TRAIN_RESULT };
private:
	int m_tg_type;
	int m_nTrainCnt;
	CString m_strTrainId;
	CListCtrl m_listResult;
	long m_nlistPosition[MAX_SAME_TRAIN_ID_NUM];
	long m_nlistTrainId[MAX_SAME_TRAIN_ID_NUM];
	long m_nlistDepartTime[MAX_SAME_TRAIN_ID_NUM];

	CTGFrame* pMainFrme;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBntClose();
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
};

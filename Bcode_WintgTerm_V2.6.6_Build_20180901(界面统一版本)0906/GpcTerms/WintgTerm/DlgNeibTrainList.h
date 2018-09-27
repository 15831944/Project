#pragma once
#include "neibtraingrid.h"
#include "tg_schedule_base.h"
#include "td/td_frame.h"
#include "td/td_data_def.h"
#include "tg_schedule_packdata.h"
#include "schd_newserver.h"
// CDlgNeibTrainList �Ի���
class CWorkScheduleHolder;
//class CTG_TrainScheduleBase;
class CTgDocument;
class CWintgViewMMI;
class CDlgNeibTrainList : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeibTrainList)
	CNeibTrainGridView m_grid;
	CWorkScheduleHolder *m_pHolder;
	CTG_TrainScheduleBase *m_pSourceData;
	CTG_TrainScheduleBase *m_pData;
	CTG_TrainScheduleBase *m_pOtherData;   // �����ݴ水����̨�г����ɱ�̨����г�ʱ, ���������������̨�����ظ�����, ����m_pData�Ĳ���
	CTG_ScheduleInfo       m_schd_info;
	CWnd                  *m_pWaitWnd;
	CTgDocument   *m_pDoc;
	CWintgViewMMI *m_pTgMMI;
	CTG_SchedulePackData  m_LocalScheduleData;
	CWintgNewServer      *m_pWintgNewServerWork;
	std::map<TRAIN_INDEX, CTG_SingleTrain> m_mapTrains;  // �����̨�г�
	CString m_szCaption;

	CTD_PlanInform  m_plan_inform; // from opms

	int    m_nStatus; //0 IDLE, >1: wait for NEIBPLAN, -1 wait for OPMS
	int nMiddleStaNum, nMiddleStaList[64];

public:
	int    m_nScheduleType;
	BOOL   m_bLoadFromSchedule;
	int    m_nDataChanged;
	int    m_nCurrentSel;
public:
	void   SetTgMMI(CWintgViewMMI *pmmi) {m_pTgMMI=pmmi;}
	CDlgNeibTrainList(const char *caption,CTgDocument *pdoc,int sche_type,int type,CWnd* pParent = NULL);   // ��׼���캯��
	CDlgNeibTrainList(const char *caption, CTgDocument *pdoc,int sche_type,int type, int hour, int nSection, CWnd* pParent);   // ��׼���캯��
    virtual ~CDlgNeibTrainList();

// �Ի�������
	enum { IDD = IDD_DLG_NEIB_TRAIN_LIST };
   
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	void    OnOK();
	void    OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
	void    OnGridSelectChangeCheck(NMHDR* pNMHDR, LRESULT* pResult); 
	void    OnBnClickedReq();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
    DECLARE_MESSAGE_MAP()

	BOOL ProcessTrainMsgFromOPMS(const char *token,const char *content);
	void ProcessTrainMsgFromOPMS_NEW(const char *token,const char *content);
	
	BOOL ProcessShiftInfoMsgFromOPMS(const char *token,const char *content);
	void RequestNewYLFromOPMS();
	void RefreshTrainList();
	void RefreshTrain(int index);
	void RequestShiftInfoFromOPMS(DWORD gtid);

	void RefreshPlanList();
	void RequestPlanDetail();
	void OnTimer(UINT_PTR IDEvent);
	void GetRecvCountRate(int &recved,int &tatol);
	void OnDestroy();
	
	BOOL AddTrainIDToPlanInform(const CTG_SingleTrain *ptrain);
	void GenTrainsFromNeib();
	void FindNeibNotExistTrains(CStringArray& m_warning_ex);

	CTG_SingleTrain *GetFromTmpMemoryTrain(TRAIN_INDEX index);
	CTG_SingleTrain *GetSingleTrainByShiftIndex(TRAIN_INDEX shift_index, STATION_NO station, CTG_TrainScheduleBase** pData);

private:
	// �Ϻ���������ʾ��̨�ƻ�
	void DisplayTrainAcceptOrder();
	BOOL GetGridXmlShowNeibStation(CTG_SingleTrain *ptrain, CString& xml, CTG_TrainRecord *pneibrec);
	BOOL GetTrainCompareResult(CTG_SingleTrain *srctrain, CTG_SingleTrain *desttrain, int neib_station, CString& xml);

	BOOL GetGridXml(CTG_SingleTrain *ptrain, CString& xml);
	bool IsTrainChange(CTG_SingleTrain *psrctrain, CTG_SingleTrain *desttrain, CString& strPath, CString& xml);
	void SetTrainStartOrAccept(const CTG_SingleTrain* ptrain, CTG_SingleTrain& newtrain, int sectionid,bool accept_start_flag);
private:
	int m_nHour;
	int m_nSectionNo;
	bool m_bSelAll;
	CStringArray m_warning;
	CStringArray tdms_ctc_alarm;
};

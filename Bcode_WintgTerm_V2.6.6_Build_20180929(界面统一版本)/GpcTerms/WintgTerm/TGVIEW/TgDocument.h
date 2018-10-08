#pragma once
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_work_schdule_holder.h"
#include "distance.h"  // ��Ҫ��ȡ�ƻ����͵����ݶ���
#include "tg_other_structdef.h"
#include "../td/td_data_def.h"
#include "../td/td_frame.h"
#include "./predict/TrainDepartPredictHolder.h"
#include "DlgCrewNotice.h"
#include <list>
#include "DlgTYMsg.h"
// CTgDocument �ĵ�
class CWintgNewServer;
class IScheduleComm;
class CSchduleDatabaseInterface;
class cStPlan;
class CDlgTYMsg;

#define  TGDATA_ALL_CHANGE    1
#define  TGDATA_CHANGE        2
#define  TGDATA_NEIGHBOR_PLAN 3
#define  TGDATA_NEIGHBOR_PLAN_TRAIN_UPDATE 4
#define  TGDATA_PHASE_NOTE    5
#define  TGDATA_SHIFT_SCHEDULE 6
#define  TGDATA_MPLAN_UPDATED  7
#define  TGDATA_XML_INFORM  8
#define  TGDATA_STATION_STATUS_CHANGE   9
#define  TGDATA_NOTEPAD    10
#define  TGDATA_TYPLAN_UPDATED		12

#define  TGSHIFT_TRAIN_GEN_TO_WORK  15

#define  TD_GPC_ACK_MARK		0x70
#define  TD_GPC_ACK_FTN			0x71
#define  TD_GPC_ACK_STN			0x72
#define  TD_GPC_ACK_JIWU		0x73
#define  TD_GPC_ACK_SPEC		0x74
#define  TD_GPC_ACK_PLAN_INFORM	0x76
const int  EVENT_RECIEVE_DATA_END = WM_USER+5000;

// Ӧ�ú�td_frame.h ����ı���һ��
#define XMLTYPE_MPLAN_LIST  TOKEN_14_ACK_RSINFO
#define XMLTYPE_MPLAN_MARK  TOKEN_13_ACK_WKINFO
#define XMLTYPE_MPLAN_INFO  TOKEN_17_ACK_SG_WX_INFO
#define XMLTYPE_MPLAN_JIESHI TOKEN_18_ACK_WKDES
#define XMLTYPE_MPLAN_DCMD   TOKEN_12_ACK_CMD
#define XMLTYPE_MPLAN_TRNINFO "TRNINFO"

#define GET_JHH_FROM_PLANID(x) x.Right(6)
#define GET_RQSTR_FROM_PLANID(x) x.Left(8)
struct ScheduleDataLoad:public CObject{
 public:
    int  status; // 0: begin 1:end
	CTG_ScheduleInfo::SCHEDULE_TYPE  schdeule_type;
};

struct StationStatusInform:public CObject{
	int  station_no;
};

struct XMLInform:public CObject{
	   CString  token;
	   CString  content;
};

struct MMplanInform:public CObject{
	enum{
		MMPLAN_LIST_CHANGED,
		MMPLAN_INFO_CHANGED,
		MMPLAN_DCMD_CHANGED,
		MMPLAN_JIESHI_CHANGED,
		MMPLAN_SELECT_CHANGE,
		MMPLAN_CANCEL_WORK,
		MMPLAN_WORK_STATE_CHANGE}
	  change_flag;
      int index;
	  CString id;
	  CString token;
	  int nResult;
};

struct TYplanInform:public CObject{
	enum{
		TYPLAN_LIST_CHANGED,
		TYPLAN_INFO_CHANGED,
		TYPLAN_DCMD_CHANGED,}
	  change_flag;
      int index;
	  CString id;
	  CString token;
	  int nResult;
};


struct PhaseNoteInform:public CObject{
	enum {DATA_CHANGED=1,RECEIVE_STATUS_CHANGED=2};
	int  nType;
	long nEntityID;
	int  nStatus;
	PhaseNoteInform(int type,long entity_id,int status)
	{
		nType=type;
		nEntityID=entity_id;
		nStatus=status;
	}
};

struct NeighborPlanInform : public CObject{
	enum {NEIB_PLAN_SEND_STATUS_CHANGE=1,NEIB_PLAN_UPDATE_TRAIN=2,NEIB_PLAN_RECV_STATUS_CHANGE=3};
	int   nType;
	int   nNeighberNo;
	int   nRecIndex;
	NeighborPlanInform(int type,int neib_no,int index)
	{
		nType=type;
		nNeighberNo=neib_no;
		nRecIndex=index;
	}
};

class CMMPlanListInfo{
	CString m_id;
	CString m_currentID; // ��ǰѡ���ID,��һ����m_id
public:
	int  m_nStatus; // 0:����״̬ 1:ȡ��״̬
	CString m_stTm; // ʩ����ʼʱ��
	CString m_edTm; // ʩ������ʱ��
	
	BOOL m_bUpdatedFromMsg;
	CString m_guid;
	CString m_workpos;
	CString m_szType;
	int     m_nFlag;
	CString m_topic;
	CString m_trninfo;
	time_t  m_worktime;
	time_t GetWorkDate() {return m_worktime;}
	CMMPlanListInfo()
	{
		m_bUpdatedFromMsg=TRUE;
		m_nFlag=0;
		m_nStatus=0;
	}
	void  GenListInfoXML(CString &dest,const char *node);
	
	int   GetSubPlanIDs(CStringArray &subids);
	CString GetMainPlanID() const;
	CString GetCurrentSubID() {return m_currentID;}
	void    SetCurrentSubID(const char *s) {m_currentID=s;}
	void    SetID(const char *s);
	CString GetID() {return m_id;}
	const char * GetSubIDByJHH(long jhh);
};

class CTYPlanListInfo{
public:
	int m_id;
	DWORD m_nWorkTm; // ʩ��ʱ��
	int m_nCount;
	void Clear()
	{
		m_id=0;
		m_nCount=0;
		m_nWorkTm=0;
	};
};

class CPlanInformListItem{
public:
	DWORD    m_id;
	time_t   m_recvtime;
	time_t   m_signtime;
	CString  m_token;
	CString  m_content;
public:
	CPlanInformListItem()
	{
		m_recvtime=0;
		m_signtime=0;
		m_id=0;
	}
	CPlanInformListItem(const CPlanInformListItem &src)
	{
		m_token=src.m_token;
		m_content=src.m_content;
		m_recvtime=src.m_recvtime;
		m_signtime=src.m_signtime;
		m_id=src.m_id;
	}
	CPlanInformListItem &operator=(const CPlanInformListItem &src)
	{
		m_token=src.m_token;
		m_content=src.m_content;
		m_recvtime=src.m_recvtime;
		m_signtime=src.m_signtime;
		m_id=src.m_id;
		return *this;
	}
};

class RecvNeigPlan
{
public:
	int  nHour;
	int  nSendSectionNo;
	char strSendSectionName[33];

	RecvNeigPlan()
	{
		nHour = 0;
		nSendSectionNo = 0;
		memset(strSendSectionName, 0, sizeof(strSendSectionName));
	}
};

struct NODE_STATION_INFO
{
	WORD station;
	TRAIN_NO arrive_trainid;
	TRAIN_NO depart_trainid;
	BYTE arrive_side;
	BYTE depart_side;
	BYTE tech_stop;
};

struct NODE_TRAIN
{
	WORD num;
	char name[16];

	NODE_STATION_INFO node[128];

	NODE_TRAIN()
	{
		num=0;
		memset(name, 0, 16);
		memset(node, 0, sizeof(node));
	}
};

struct SINGLE_TRAIN_GEN_TO_WORK
{
	TRAIN_INDEX train_index;
	int section;
	int source_schd_type;
	char train_id[32];
};

class CTgDocument : public CDocument
{
	DECLARE_DYNCREATE(CTgDocument)

public:
	CTgDocument();
	virtual ~CTgDocument();
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);

	DECLARE_MESSAGE_MAP()
public:
	CTrainDepartPredictHolder *GetTrainDepartPredictHold() {return m_pTrainDepartPredictData;}
private:
	CTrainDepartPredictHolder *m_pTrainDepartPredictData;
	CWorkScheduleHolder  *m_pCurrentWorkScheduleData;
	CWintgNewServer      *m_pWintgNewServerWork;
	CWintgNewServer      *m_pWintgNewServerShift;
	CWintgNewServer      *m_pWintgNewServerBasic;
	CWintgNewServer      *m_pWintgNewServerBasicPrev;
	CWintgNewServer      *m_pWintgNewServerBasicBackup;
	cStPlan             *m_pStplan;
    BOOL         m_bAutoDelete;
	BOOL         m_bQueryMode;
///////////////////////////��ǰѡ�ж��� ////////////////
	CTG_LogicObject  *m_pActiveObject;
	TRAIN_INDEX       m_nActiveObjectIndex;
	TG_OBJECT_TYPE    m_nActiveObjectType;
	CTG_ScheduleInfo::SCHEDULE_TYPE m_nActiveObjectScheduleType;
	int   m_nActiveObjectStationNo; // ѡ�е�վ��
	int   m_nActiveObjectStationIndex; // ѡ�е�վ��
//////////////////////////////////////////�йؼƻ��Ķ���
	CWnd *m_pWaitWnd;

	//BOOL m_bEnableProcessShiftInfo;
	//BOOL m_bEnableProcessShiftTrains;
public:
	int m_nCurrentSectionNo;
	int m_tmPlanSendRange;
	
	int m_nCurrentSectionList[32];
    int m_nCurrNeighberSectionCount;
	int m_nCurrNeighberSectionList[10];
    int m_nCurrNeighberGpcCount;
	int m_nCurrNeighberGpcList[10];
    int m_nCurrNeighberSectionPropertyList[10];  //��̨�����ԣ�0����ϵͳ��DMIS��̨,1-����ϵͳ��DMIS��̨
	int m_nCurrNeighberViewPlanSectionCount;
	int m_nCurrNeighberViewPlanSectionList[10];
    int m_nCurrNeighberViewPlanGpcCount;
	int m_nCurrNeighberViewPlanGpcList[10];
    int m_nCurrentNeighberViewPlanSectionNo;
	
	int   m_nNeibPlanTotalCount[6];
	int   m_nReceiveNeighberLastActivePage;
    TG_STATIONPLANINFO_IMPORTANTNEW m_StationPlanInfo_ImportantNew; //new defiend

	
	TG_NOTEBOOK m_cPhaseNotes; //����


	int m_aStationList[128];
	int m_nStationCount;

	cStPlan *GetSTPLAN() {return m_pStplan;}

#define MAX_REGISTERED_WND_NUM 24
	struct{
		HWND    hWnd;
		UINT    nMsgID;
	}m_aRegisterdWnd[MAX_REGISTERED_WND_NUM];
 
public:
////  STATIC STUFF /////////
static 	CTgDocument *CreateTgDyncDocument(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,int shift_id,BOOL localmode,BOOL newserver);  // STATIC!!
static 	CTgDocument *CreateTgDocument(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,int shift_id,BOOL localmode,BOOL newserver);
static  void DeleteAllTgDocuments();
static  BOOL InitTGDocuments();
static  BOOL GlobalProcessMessage(long eid,BaseMsg *pBuf);
static  BOOL GlobalAutoShiftSchedule();
static  BOOL InitWS();
static  BOOL DeleteWS();
static  BOOL GlobalLoadFromLocalFiles();
static  BOOL GlobalSaveToLocalFiles();
static  BOOL GlobalProcessStationStatusInform(int station);
static  void SetTrainReverseFlag(CTG_SingleTrain *ptrain); // ���÷����г�
static  void SetTrainFromToConid(int conid, CTG_SingleTrain *ptrain); // ���ý��뽻��̨

static  bool ResetGoodsTrainStopTech(CTG_SingleTrain *ptrain); // ȥ�������Ǽ���ͣ��
static  void CheckTdmsSideByStationside(int iSection, CTG_SingleTrain& train);// ���TDMS�ɵ��Ƿ�����վϸ
static  BOOL IsSendCloseWithSvr(int con_id, CTG_ScheduleInfo::SCHEDULE_TYPE type);

// ����-�ص㾯��-����-����
static void SetTrainSpaceGaurdMilitaryRank(CTG_SingleTrain& train); 
////////////////////////////
	void  OpenWaitWindow(const char *cap, const char *text=NULL);	
	CString  IsWaitWindowExist();
	BOOL  InitLogicData(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL  localmode,BOOL newserver);
	BOOL  InitLogicDataDync(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL  localmode,BOOL newserver); //type
	BOOL  DeinitData(void);
	BOOL  PollingData(void);
	BOOL  LoadSchedule(int shift_id);
	CWorkScheduleHolder  *GetTGDataHolder(){return m_pCurrentWorkScheduleData;}
	BOOL  GetScheduleInfo(CTG_ScheduleInfo &info);
	BOOL  GetShiftScheduleInfo(CTG_ScheduleInfo &info);
	BOOL  GetBasicScheduleInfo(CTG_ScheduleInfo &info);
	BOOL  GetBasicPrevScheduleInfo(CTG_ScheduleInfo &info);
	CTG_ScheduleInfo::SCHEDULE_TYPE  GetScheduleType();
	CTG_TrainScheduleBase *GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE type);
	long  GetCurrentShiftID();
	int   GetCurrentSectionNo();
	void  SetAutoDelete(BOOL flag) {m_bAutoDelete=flag;}
	BOOL  SetLocalDatabaseMode(BOOL flag);
//////////////////////////////////
	BOOL  InitPlanInfo(int section_id);
	BOOL  InitStationPlanInfo(int nFlag=0);
	
	int   GetCurrentTGDate();
	int   GetCurrentTGTime();
	TIME  GetCurrentTGDLongTime();

	
	
	BOOL  ProcessGenMessage(long eid,BaseMsg *pBuf);
	BOOL  ProcessMessage( MsgStringMessage *pmsg);

	BOOL  ProcessStationStatusInform( int station);
/////////////  ���� ///////////////////////////
	 BOOL AutoShiftSchedule();
	 BOOL ShiftSchedule(ULONG newShiftID, bool nIgnore=false);
/////////////////////////////////////
	 CTG_LogicObject  *GetCurrentActiveObject(int *st=NULL, int *rec_index=NULL) 
	 {
		 if(st) 
			 *st=m_nActiveObjectStationNo;
		 if(rec_index) 
			 *rec_index=m_nActiveObjectStationIndex;
		 if(m_nActiveObjectScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
			 return NULL;
	 	 CTG_TrainScheduleBase *pdata=GetScheduleData(m_nActiveObjectScheduleType);
		 if(!pdata)
			 return NULL;
		 if(m_nActiveObjectType==TG_OBJ_TRAIN)
			 return pdata->GetSingleTrain(m_nActiveObjectIndex);
		 else
		 if(m_nActiveObjectType==TG_OBJ_MARK)
			 return pdata->GetSingleMark(m_nActiveObjectIndex);	
		 else
			return NULL;
	 }
	 
	 CTG_SingleTrain  *GetCurrentActiveTrain(CTG_ScheduleInfo::SCHEDULE_TYPE schd_type) 
	 {
		 if(m_nActiveObjectScheduleType!=schd_type || m_nActiveObjectType!=TG_OBJ_TRAIN)
			 return NULL;
	 	 CTG_TrainScheduleBase *pdata=GetScheduleData(m_nActiveObjectScheduleType);
		 if(!pdata)
			 return NULL;
		 return pdata->GetSingleTrain(m_nActiveObjectIndex);
	 }

	 void GetCurrentActiveObject(CTG_ScheduleInfo::SCHEDULE_TYPE  &schdtp,TG_OBJECT_TYPE &objtype,TRAIN_INDEX &index)
	 { 
		 objtype=m_nActiveObjectType;
		 index=m_nActiveObjectIndex;
		 schdtp=m_nActiveObjectScheduleType;
	 }
	 void SetCurrentActiveObject(CTG_ScheduleInfo::SCHEDULE_TYPE  tp,CTG_LogicObject  *p, int st, int rec_index) 
	 {
		 m_nActiveObjectScheduleType=tp;
		 m_pActiveObject=p;
		 m_nActiveObjectStationNo=st;
		 m_nActiveObjectStationIndex=rec_index;

		 m_nActiveObjectType=TG_OBJ_NULL;
		 m_nActiveObjectIndex=0;
		 
		 if(m_pActiveObject)
		 {
			 m_nActiveObjectType=m_pActiveObject->GetObjectType();
			 if(m_nActiveObjectType==TG_OBJ_TRAIN)
                  m_nActiveObjectIndex=((CTG_SingleTrain *)m_pActiveObject)->GetTrainIndex();
			 else
			 if(m_nActiveObjectType==TG_OBJ_MARK)
                  m_nActiveObjectIndex=((CTG_SingleMark *)m_pActiveObject)->GetBlockIndex();
		 }
	 }
////////////// ʩ���ƻ��б� //////////////
 	enum{MAX_MMPLAN_NUM=256};
	CMMPlanListInfo *m_cMMplans[MAX_MMPLAN_NUM];
	int    m_nCountOfPlan;
	list<CTD_MPLANMARK> m_cMMplanMarks;  // 2010.3.20 ʩ�����ŵ����洢
	CString m_mplan_ws_url;  // Webservice URL
	TIME  m_tMMplanStartTime,m_tMMplanEndTime; // ʩ���ƻ���ʱ�䷶Χ
	void  ClearAllMMPlan();
	void  ClearLocalXMLFile();
	int   AddMMPlan(const char *id,BOOL frommsg);
	int   FindMMPlan(const char *id);
	int   FindMMPlanByMainID(const char *id);
	int   FindMMPlanByCurrentSubID(const char *id);
	int   FindMMPlanBySubID(const char *id);
	int   GetMPlanCount();
	CMMPlanListInfo *GetMPlanPtr(int i);
	BOOL  ReadMPLANFile(CString &xml,const char *planid,const char *node);
	BOOL  WriteMPLANFile(const char *planid,const char *node,const char *xml);
	BOOL  LoadMMListFromWS();
	BOOL  LoadMMInfoFromWS(CString &dest,const char *idstr);
	BOOL  SaveMMInfoToWS(int index);
	CString GetPhasePlanFileDir();
	BOOL  LoadPhasePlanFromFile();
	void  SavePhasePlanToFile(const CPlanInformListItem* item);

	BOOL  ProcessXML_MMPlanMsgFromTD(const char *xml,const char *node);
	
    BOOL  ProcessXML_Msg_Mark(const char *xml,const char *node,bool bIsFromTD, int conid);
	BOOL  ProcessXML_Msg_SG_WX(const char *xml,const char *node, bool bIsFromTD, int conid);
	BOOL  ProcessXML_Msg_WKDES(const char *xml,const char *node, bool bIsFromTD, int conid);
	BOOL  ProcessXML_Msg_List(const char *xml,const char *node, bool bIsFromTD, int conid);
	BOOL  ProcessXML_Msg_DCMD(const char *xml,const char *node, bool bIsFromTD, int conid);
    BOOL  ProcessXML_Msg_TD_RESULT(const char *xml,const char *node);
	BOOL  ProcessXML_Msg_List_State(const char *xml,const char *node, int con_id);

	void  SaveMMPlanToWS();
	int  SaveDCMDToWS(int i);
	int  SaveDCMDToWS(const char *planid);
	void  GetMMPlanRange(TIME &st,TIME &et) {st=m_tMMplanStartTime;et=m_tMMplanEndTime;}
	BOOL  ReadDCMDFromFiles(int index,CString *pdest,CStringArray *parray=NULL);

	BOOL   ProcessXML_PlanInform(const char *xml,const char *node);
	// �����Ϻ��ֻ��˵����ƻ� 2014.5.13
	BOOL   ProcessXML_FTNMsg(const char *xml,const char *node);
	long   m_nRecvMsgId;
	int	   m_nRecvMsgCnt;
	CString m_strRecvMsg;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  

	//void   Test();
	BOOL   RequestMMPlanFromOPMS(time_t start,time_t end,CView *pv,long destid);
	BOOL   LoadMMListFromFile();
	BOOL   SaveMMListToFile();
	BOOL   SaveMMMarkToFile();
	void   SendMMPlanStateToServer(int index);
	BOOL   ReadMMPlanListStateFromFile();
	BOOL   GenMarkFromMMPlan(const CTD_MPLANMARK *pmark,int flag); // flag:  1 shift  2 work 3 all
	BOOL   RequestSPECFromCAD(time_t start,time_t end); // �����������������Ϣ
	BOOL   RequestTrainExpand_KD_YL_FromOPMS(TRAIN_INDEX GTID, int MSGID); // MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
	BOOL   RequestTrainExpand_KD_YL_CREW_FromOPMS(TRAIN_INDEX GTID, int MSGID); // MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
	BOOL   RequestTrainExpand_ENGINECREW_TICKE_FromOPMS(TRAIN_INDEX GTID, int MSGID, CString& TNO); // MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
	BOOL   RequestCrewInform(std::vector<ULONG>& crewgid); // ���������Ϣ
	BOOL   RequestMMPlanInfoFromOPMS(int index,int mask);
	
	BOOL   IsMyStation(int station) const;
	void   ClearPhasePlanInfo();
	void   SetQueryMode(BOOL va);
	BOOL   IsQueryMode() {return m_bQueryMode;}
	IScheduleComm *GetServerCommInterface() {return (IScheduleComm *)m_pWintgNewServerWork;}
	
	void   SendNeighbourPlan(const MsgNextPlanRequest *pBuf);

	void   GenDCmd(CString plan_id, long block_index, CString content, int con_id, long type);

	int    GetMPlanTime(CStringArray& planid, CStringArray& sttm, CStringArray& edtm);
	
	BOOL   SaveMemMMPlanListToFile();
	int    GetMMPlanState(CString planid);

	void   AddMPlanMark(CTD_MPLANMARK *p);
	void   MPlanMarksToXML(CString &xml,const char *node);
	void   MPlanListToXML(CString &xml);
	BOOL   GenMarkFromMMPlanByPlanId(const CString planid,int flag, int&s, int&f); // flag:  1 shift  2 work 3 all
	BOOL   GenMarkFromMMPlanByPlanId(int flag); // flag:  1 shift  2 work 3 all
	
	time_t GetMMPlanStartTime(const CMMPlanListInfo *mPlan) ;
	time_t GetMMPlanEndTime(const CMMPlanListInfo *mPlan);
	BOOL   GetMarkDesc(const char *planid,CStringArray &result, int type);
	void   DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE sche_type,const char *planid);
public: 
	///////////// �յ��ĵ����ƻ� ///////////////
	list<CPlanInformListItem>  m_listPlanInform;
	CPlanInformListItem *GetPlanInformByID(DWORD id);
	//void  EnableProcessShiftInfo(BOOL va){ m_bEnableProcessShiftInfo=va;} // �Ƿ�����Ϣ�����ڽ��յ����ƻ�ʱ����Ҫ��TgDocument���ظ�����
	//void  EnableProcessShiftTrain(BOOL va) {m_bEnableProcessShiftTrains=va;} 

	
	//��ԭϵͳinput���ݵķ�ʽ����
	BOOL olSaveTrainRecords(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName);
	BOOL olSaveTrainInfo(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * infoName);
	BOOL olSaveTrainSideandEntry(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName);

	BOOL olSaveTrainToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName);
public:
	int  GetTrainIDDirection(CString strTrainId);
	std::vector<RecvNeigPlan> m_vectNeigPlan;
	void NoticeNeighbourReceivePlan(CTG_ScheduleInfo& schd_info, DWORD entity);
	BOOL  ProcessMessage(const MsgNoticeNeigbourReceiveSchd *pBuf);

public:
	BOOL LoadTrainSideTechToFile(const CString& fileName);
	BOOL SaveTrainSideTechToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName);
	
	BOOL ReleaseBasicTrainInfo();
	BOOL InitBasicInfo(CTG_SingleTrain * ptrain);

private:
	WORD basic_train_info_num;
	NODE_TRAIN* basic_train_info[2048];
	CDlgCrewNotice* m_pCrewNoticeDlg;

public:
	void OnReciveOPMSCrewNotice(const char* token, const char* content);

private:
	CStringArray planidcancel;

public:
	BOOL  ProcessXML_TYPlanMsg(const char *xml,const char *node);
	BOOL  ProcessXML_Msg_TEYUN(const char *xml,const char *node);
	BOOL  ProcessXML_Msg_TEYUN_Update(const char *xml,const char *node);
	BOOL  ProcessXML_KDPlanMsg(const char *xml,const char *node);
	BOOL  WriteTYPLANFile(long date,int index,const char *node,const char *xml);
	BOOL  ReadTYPLANFile(CString &xml,long date,int index,const char *node);
	void  DeletePrevTYPLANFile(long date);
	int   FindTYPlanByMsgID(int id);
	int   AddTYPlan(int id, BOOL frommsg, DWORD tm=0);

	BOOL  LoadTYListFromFile();

	void  ClearAllTYPlan();
	int   GetTYPlanCount();
	CTYPlanListInfo *GetTYPlanPtr(int i);

private:
	CDlgTYMsg *m_pTyWaitDlg;
	std::vector<CTYPlanListInfo> mSpecPlanList;
public:
	void SendGpcAckMsg(int type, bool isOk, long src, CString msgid, CString jht, CString result="");
};


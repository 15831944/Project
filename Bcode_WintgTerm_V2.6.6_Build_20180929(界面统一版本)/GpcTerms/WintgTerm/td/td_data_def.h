#pragma once
#include <vector>
#include "tg_schedule_base.h"

#define MAX_MPLAN_NODE_NUM  256
#define MAX_MPLAN_MARK_NUM  512
#define MAX_MPLAN_DCMD_NUM  64

/////////////////////////////////////////////////////////////
class CTD_TrainInfo
{
public:
	CTD_TrainInfo();
	CTD_TrainInfo(const CTD_TrainInfo &src);
	CTD_TrainInfo& operator= (const CTD_TrainInfo &src);

private:
	void SetValue(const CTD_TrainInfo &src);

public:
	int     SCHTYP; // ��������ͼ����
	int     YLFG;  //0: ������ 1: ����
	int     LYLFG; //0: ���ڱ�̨ 1: ��̨
	int     TRAINTYPE;
	DWORD   GTID;  // *
	CString TNO; // *
	int     IFLAG; //1��ʼ��/0��ת��
	int     OFLAG; //0��ת��/1������
	CString STST; // ʼ��վ *
	CString ENDST; //�յ�վ *
	CString FROMST; //����ǰһվ
	CString TOST;   // ������һվ
	time_t LSTTM; //��̨��ʼʱ��
	time_t LEDTM; //��̨����ʱ��
	time_t STTM;  //ʼ��ʱ�� *
	time_t FJTM; // �ֽ�ڽ���ʱ�� *
	time_t FJTDTM; // �ֽ�ڽ���ʱ�� *
	int    m_nStationCount;
	int    SEQ;  // ��ǰ���
	int    COUNT; // ����

	// �Ϻ������� 2014.4.12
	DWORD   RTID;
	CString EMU;
	DWORD   DGID;
	int		HALT;
};

class CTD_TrainRecordInfo
{
public:
	CTD_TrainRecordInfo();
	CTD_TrainRecordInfo(const CTD_TrainRecordInfo &src);
	CTD_TrainRecordInfo& operator= (const CTD_TrainRecordInfo &src);

private:
	void SetValue(const CTD_TrainRecordInfo &src);

public:
	int     STID;
	int     NODESEQ; // �ڵ�˳��
	CString STNM; //վ��
	int     ARND; //����ڵ�
	int     DPND; //�����ڵ�
	int     ENTRY_LINE;//����߱�
	int     EXIT_LINE;//�����߱�
	int     SIDENO;//�ɵ���
	CString ARTNO; //���ﳵ��
	CString DPTNO; //��������
	time_t  ARTM;  //����ʱ��
	time_t  DPTM;  //����ʱ��
	time_t  SARTM; //ͼ������ʱ��
	time_t  SDPTM; //ͼ������ʱ��
	int     BSTM;  //Ӫҵʱ��
	WORD    IMPINFO; //1 ���� 2 �ص㾯�� 4 ��Ʒ 8 ����
	int     STPINFO; //��/��ͣ/����ͣ 0/1/2
	int     METINFO[20]; //���ᳵ��
	int     m_nNotMeetCount;
	CString SGINFO;
	int     SGFLAG;
	//added by zhaoliang 20100529
    int     ARR_ENGINE_NUM;//��������
	CString ARR_ENGINE_NAME;//��������=��������+������
	int     DEP_ENGINE_NUM;
	CString DEP_ENGINE_NAME;

	// �Ϻ������� 2014.4.12
	int		OPERFLAG;
	CString OPERINFO;
	int     WORKFLAG;
	time_t  CWCL;
	// ���Ͼ�Ҫ������ �ص����� 2017.01.07
	CString NOTE_TEXT; 
};

class CTD_Train
{
public:
	CTD_Train();
	CTD_Train(const CTD_Train &src);
	CTD_Train& operator= (const CTD_Train &src);

public:
	TRAIN_INDEX GetTrainShiftIndex() {return m_TDInfo.GTID;}
	void SetTrainShiftIndex(TRAIN_INDEX index) { m_TDInfo.GTID=index;}
	CTD_TrainRecordInfo* GetTrainTDRecord(WORD idx);
	int GetTrainTDRecordCount() { return m_TDRecordList.size(); }
	void PurgeTrainRecord() { m_TDRecordList.clear();}
public:
	CTD_TrainInfo                m_TDInfo;
	CTG_TrainEngineList          m_cEngineList;
	vector<CTD_TrainRecordInfo>  m_TDRecordList;
};


void SumZhongChe(const char *dir,int ld);
void SumQingChe(const char *type);
BOOL TD_BZGenHeaderXML(CString &hdr,const char *xml,const char *node);

BOOL TD_LoadTrainFromXML(const CTG_ScheduleInfo& schdinfo,CTG_TrainScheduleBase *ptrainbase,CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *pflag,time_t sttm,time_t endtm,int mode, CStringArray &train_alarm);
CTG_SingleTrain *TD_GetTrainInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node);
void TD_SetTrainTicketInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node);
BOOL TD_LoadBasicScheduleTrainFromXML(CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *ptype, CStringArray& train_alarm); // ��ȡ����ͼ������Ϣ
BOOL TD_LoadShiftScheduleTrainFromXML(const CTG_ScheduleInfo& schdinfo,CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount, int mode, CStringArray &train_alarm);

BOOL TD_LoadTrainFromXMLx(const CTG_ScheduleInfo& schdinfo,CTD_Train &tdtrain,const char *xml,const char *node,int *pflag);

BOOL TD_ImportRecordInfoFromTD(CTG_TrainRecord &ctcrecord,CTD_TrainRecordInfo &tdrecord,CTD_TrainInfo &info, bool bAcceptSide,CStringArray& list);
BOOL TD_ImportTrainInfoFromTD(CTG_SingleTrain &ctctrain,CTD_TrainInfo &tdtrain,int mode);

void SetTrainFirstStationEntry(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1�հ�ƻ�������ͼ 2����ͼ ��Ҫ��Խ���ͽ���
void SetTrainLastStationExit(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1�հ�ƻ�������ͼ 2����ͼ ��Ҫ��Խ���ͽ���

bool TD_GetKediaoInfoFromXML(CTG_TrainKediaoInfo& kd,const char *xml,const char *node);
bool TD_GetYingyeInfoFromXML(CTG_TrainYingyeList& yy,const char *xml,const char *node);
bool TD_GetTicketInfoFromXML(CTG_TrainTicketList& tk,const char *xml,const char *node);
bool TD_GetCrewInfoFromXML(CTG_TrainCrewList& tk,const char *xml,const char *node);
bool TD_LoadEngineCrewInfoFromXML(CTG_EngineCrewList& tk,const char *xml,const char *node, int* seq, int*count);

// 3. ʩ������
class CTD_MPLANMARK
{
public:
	CTD_MPLANMARK();
	CTD_MPLANMARK &operator=(const CTD_MPLANMARK &other);
	BOOL operator== (const CTD_MPLANMARK &src) const;
	
public:
	BOOL  SaveToXML(CString &dest,const char *node);
	BOOL  GenListInfoXML(CString &dest,const char *node);
	const char  *GetListTitle();
	
	BOOL     GenMark(CTG_SingleMark &mark) const;

public:
	DWORD    m_nGMID;
	int      m_nCTCType;
	int      m_nTDType;
	time_t   m_tStartTime,m_tEndTime;
	
	CString  PLANID,STST,EDST,OTHER,PLANCD;
	CString  REASON;
	int      STKM,STM,ETKM,ETM;
	int      LIM,DIR;
	CString  CONTENT;
	int      STTB, EDTB;
};

// 4. ʩ��ά��
class CTD_MPLAN_SG_WX
{
public:
	CTD_MPLAN_SG_WX();
public:
	int m_con_id;
	int m_count, m_flag;
	CString DISID;
	CString PLANID,TM,DBH,MID,WORKPOS,LINENM,DIR,WORKKM;
	CString WORKTP,PJNM,UNITNM,LEADER,RANK,WKCON,SPLIM,LIMINFO;
	CString STTM,EDTM,TELID;
public:
	BOOL  LoadSG_WXFromTDXML(const char *xml,const char *node);
	BOOL  LoadSG_WXFromCADXML(const char *xml,const char *node, int conid);
	BOOL  GenMplan_SG_WX(const char *xml,const char *node);
	BOOL  GenViewInfoXML(CString &dest,const char *node);
	BOOL  SaveSG_WXToXML(CString &dest,const char *node);
};

class CTD_MPLANDCMD
{
public:
	CTD_MPLANDCMD();

public:
	BOOL SaveToXML(CString &dest,const char *node, long create_time, CString dcmdtype, CString dcmdsubtype);
	BOOL SaveDCmdToXML(CString &dest,const char *node);
public:
	int     m_nSectionID;

	CString PLANID; //�ƻ�ID
	CString CMDINDEX; // �ڲ���ʾ
	CString CMDTP; // ��������

	CString  RECVUNIT; // �������
	
	CString TOPIC; // �������
	CString CONTENT; //��������
	CString PRNM;  // ��Ŀ����

	CString SUBTP; // ����������
	CString RECVER;
};

// 6. ʩ������
class CMPLAN_STATE
{
public:
	CMPLAN_STATE()
	{
		m_nSectionID=0;
		m_nStatus=0;
	}
	CString DISID;
	int     m_nSectionID;
	CString PLANID;
	int     m_nStatus;
	CString STTM;
	CString EDTM;
};

// 7. ʩ����ʾ
class CMPLAN_WKDES
{
public:
	CMPLAN_WKDES();

public:
	BOOL  LoadWKDESFromTDXML(const char *xml,const char *node);
	BOOL  LoadWKDESFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveWKDESToXML(CString &dest,const char *node);
public:
	CString strDisID;
	int     m_nSectionID;

	CString PLANID;
	CString CMDID;
	CString CONTENT;
};

// 8. ʩ���ܱ�
class CMPLAN_RSINFO
{
public:
	CMPLAN_RSINFO();
	
public:
	BOOL SaveRSINFOToXML(CString& dest, const char* node);

public:
	CString strDisID;
	int     m_nSectionID;

	CString PLANID;
	time_t  TM;
	CString TRNINFO;
	int     FLAG;

	CString PNM;
	CString WKDES;
};

class CTD_MPlanMidList
{
public:
	CTD_MPlanMidList();

public:
	BOOL  LoadRSINFOFromTDXML(const char *xml,const char *node, int conid);
	BOOL  LoadRSINFOFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveRSINFOToXML(CString&xml, const char* token);
public:

	int m_nCount;
	CMPLAN_RSINFO m_list[MAX_MPLAN_NODE_NUM];
};

class CMPlanListState
{
public:
	CMPlanListState();
	
public:
	BOOL LoadMPlanStateFromCADXML(const char *xml,const char *node, int con_id);

public:
	int m_nCount;
	CMPLAN_STATE m_list[MAX_MPLAN_NODE_NUM];
};

class CTD_MPlanMarkList
{
public:
	CTD_MPlanMarkList();

public:
	BOOL  LoadMarkFromTDXML(const char *xml,const char *node);
	BOOL  LoadMarkFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveXMLToString(CString&xml, const char* token);
public:
	long m_nSrccode;
	long m_nMsgid;

	int mark_con_id;
	CString strDisID;
	int m_nCount;
	CTD_MPLANMARK m_list[MAX_MPLAN_MARK_NUM];
};

class CTD_MPlanDCMDList
{
public:
	CTD_MPlanDCMDList();

public:
	BOOL  LoadDcmdFromTDXML(const char *xml,const char *node, int conid);
	BOOL  LoadDcmdFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveDCmdsToXML(CString&xml, const char* token);
public:
	CString strDisID;
	int     m_nSectionID;

public:
	int m_nCount;
	CTD_MPLANDCMD m_list[MAX_MPLAN_DCMD_NUM];
};

class CTD_TrainCrew
{
public:
	CTD_TrainCrew();
	
public:
	CString GetCrewInfo();
public:
	DWORD GID;
	CString TICKET;
	CString CREW;
	CString TNO;
};

class CTD_TrainTicketList
{
public:
	CTD_TrainTicketList();

public:
	BOOL  LoadCrewFromTDXML(const char *xml,const char *node);

public:
	int m_nCount;
	CTD_TrainCrew m_list[512];
};

class CTD_CrewNotice
{
public:
	CTD_CrewNotice();

public:
	BOOL  LoadCrewNoticeFromTDXML(const char *xml,const char *node);

public:
	UINT m_sendcount;
	UINT m_count;
	ULONG m_gtid[512];
};

class CTD_BZInfo
{
public:
	CTD_BZInfo();
public:
	CString TYPE;
	CString CNO;
	int SWT;
	int LEN;
	int LD;
	int PBCNT;
	CString FIRSTNM;
	CString OBJNM;
};

class CTD_PlanInform
{
public:
   enum{MAX_NFORM_TRAIN_NUM=512};
   enum{SHIFT_PLAN_INFORM,KD_PLAN_INFORM,JH_PLAN_INFORM,STN_PLAN_INFORM,FTN_PLAN_INFORM, AD_PLAN_INFO} inform_type;
   enum{OPER_ADD_TRAIN=1,OPER_UPDATE_TRAIN=2,OPER_DELETE_TRAIN=3,OPER_NOEXSIT=4};
	DWORD         JHID;
	CString       JHTNM;
	CString		  MSGID;
	struct{
		BYTE      OPER; //0x01 ����  0x02 �޸� 0x03 ɾ��
		DWORD     KDID;
		DWORD     GTID;
		CString   TNO;
		BOOL      LocalAdded;
		short     InfoReceivedFlag;
	}trains[MAX_NFORM_TRAIN_NUM];
	int COUNT;
	time_t  STTM,ENDTM;
	CTD_PlanInform()
	{
		JHID=0;
		COUNT=0;
		for(int i=0;i<MAX_NFORM_TRAIN_NUM;i++)
		{
			trains[i].KDID=0;
			trains[i].GTID=0;
			trains[i].InfoReceivedFlag=0;
			trains[i].LocalAdded=FALSE;
		}
		STTM=0;
		ENDTM=0;

	}
	BOOL  LoadFromXML(const char *xml,const char *node);
	BOOL  GenACKXML(CString &dest,const char *node);
};

class CTD_Ack
{
public:
	int ACKL;
	BOOL  LoadFromXML(const char *xml,const char *node);
};

#define MAX_TY_DCMD_NUM 64
#define MAX_TY_NOTIFY_NUM 64

class CTD_TYDCMD
{
public:
	CTD_TYDCMD();
	CTD_TYDCMD(const CTD_TYDCMD& rhs);

	BOOL LoadTYDcmd(const char *xml,const char *node);
	BOOL SaveTYDcmdToXML(CString &dest,const char *node);
public:
	CString CID;
	int CTYPE;

	CString CRECV;
	CString CTITLE;
	CString CCONTENT;
	CString CPNM;
	CString CSID;
	int CRCNT;
	CString CRPT;
};

class CTD_TYNOTIFY
{
public:
	CTD_TYNOTIFY();
	CTD_TYNOTIFY(const CTD_TYNOTIFY& rhs);

	BOOL LoadTYNOTIFY(const char *xml,const char *node);
	BOOL LoadTYNOTIFYX(const char *xml,const char *node);
	BOOL SaveTYNOTIFYToXML(CString &dest,const char *node);

public:
	CString NTITLE;
	CString NCONTENT;
	time_t NTM;
	DWORD NGID;
};

class CTD_SPECPLAN
{
public:
	CTD_SPECPLAN();

	BOOL LoadSPECPLAN(const char *xml,const char *node);
	BOOL LoadSPECPLANFromFile(const char *xml,const char *node);
	BOOL SaveSPECPLANToFile(CString &dest,const char *node);

public:
	long SRCCODE;
	CString MSGID;
	CString JHTNM;
	time_t DATE;
	int COUNT;
	int SEQ;
	int TRACKID;
	CString CONTENT;
	CString TNO;
	time_t  STTM;
	time_t  EDTM;
	CString STST;
	CString EDST;

	int NOTECNT;
	std::vector<CTD_TYNOTIFY> TYNOTIFY;
	int CMDCNT;
	std::vector<CTD_TYDCMD> TYDCMDLIST;
};


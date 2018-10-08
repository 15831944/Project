#ifndef __INCLUDE_TG_FUNCTION_CONFIG___
#define __INCLUDE_TG_FUNCTION_CONFIG___
#include "gpcdef.h"
#include "tg_defines.h"
#include <vector>

const int NEW_SPLIT_ST_NUM=64;

class WarnIdLeval
{
public:
	COLORREF clr;
	int id_count;
	int id[128];
	int tz_id[128];
	char text[256];
public:
	WarnIdLeval()
	{
		InitWarnIdLeval();
	}
	void InitWarnIdLeval()
	{
		id_count=0;
		clr=RGB(255,255,255);
		memset(id, 0, sizeof(id));
		memset(tz_id, 0, sizeof(tz_id));
		memset(text, 0, sizeof(text));
	}
};

struct StationDefaultElec
{
	int station;
	int entry;
	int exit;
	int flag;
	StationDefaultElec()
	{
		station=0;
		entry=0;
		exit=0;
		flag=-1;
	}
};

struct SectionDefaultElec
{
	int con_id;
	int station_count;
	StationDefaultElec default_elec[20];

	SectionDefaultElec()
	{
		con_id=0;
		station_count=0;
	}
};

struct ChangeTrainId
{
	int station;
	int pre_station;
	int next_station;
	
	int flag; // 1:Ϊ��վ 2:Ϊ���� 3:Ϊ���
	int entry_count;
	int entry[5];

	int method;
	char text[20];

	ChangeTrainId()
	{
		flag=0;
		station=0;
		pre_station=0;
		next_station=0;
		entry_count=0;
		memset(entry, 0, sizeof(entry));
		method=0;
		memset(text, 0, sizeof(text));
	}
};

struct ConsoleChangeId
{
	int con_id;
	int info_count;
	ChangeTrainId info[10];

	ConsoleChangeId()
	{
		con_id=0;
		info_count=0;
		memset(info, 0, sizeof(info));
	}
};

struct NeibPlanSY
{
	int  flag;
	int  first_station;
		
	int  fjk_station;
	
	int  exit_count;
	int  exit[10];   // ����
	
	int  station_count;
	int  station_list[10];
	int  station_index_list[20];

	int  passway;
	int  accept_side;
	int  accept_stop;
	int  accept_arrive_depart;

	NeibPlanSY()
	{
		flag=0;
		first_station=0;
		
		fjk_station=0;
		accept_side=0;

		exit_count=0;
		memset(exit, 0, sizeof(exit));
		
		station_count=0;
		memset(station_list, 0, sizeof(station_list));
		memset(station_index_list, 0, sizeof(station_index_list));

		passway=0;
		accept_stop=1;
		accept_arrive_depart=0;
	}		
};

struct NeibConsolePlanSY
{
	int  neib_section;
	int  neibplan_count;
	NeibPlanSY neib_plan[32];
	
	NeibConsolePlanSY()
	{
		neib_section=0;
		neibplan_count=0;
		memset(&neib_plan, 0, sizeof(neib_plan));
	}		
};

struct ConsoleNeibPlanSY
{
	int  this_section;
	int  neib_console_count;
	NeibConsolePlanSY neib_console[10];
	ConsoleNeibPlanSY()
	{
		this_section=0;
		neib_console_count=0;
		memset(&neib_console, 0, sizeof(neib_console));
	}
};

// ���շ����г���̨�ƻ�����
struct NeibPlanReverse
{
	int  flag_count;
	int  flag[2];

	int  station;		// Ŀ�공վ
	int  station_pos;   // Ŀ�공վ�ڱ�̨��վ���������е��±�

	int  station_count;     // ��̨��վ�����г�վ��Ŀ
	int  station_list[10]; // ��̨��վ����
	int  station_index_list[20];  // ��̨��վ�ڱ�̨�ĳ����վ��
		
	int  only_accept_depart;
	int  accept_stop;
	NeibPlanReverse()
	{
		flag_count=0;
		flag[0]=0;
		flag[1]=0;
		
		station=0;
		station_pos=0;

		station_count=0;
		memset(station_list, 0, sizeof(station_list));
		memset(station_index_list, 0, sizeof(station_index_list));
		
		only_accept_depart=1;
		accept_stop=1;
	}		
};

struct NeibConsolePlanReverse
{
	int  neib_section;
	int  neibplan_num;
	NeibPlanReverse neib_plan[32];
	
	NeibConsolePlanReverse()
	{
		neib_section=0;

		neibplan_num=0;
		memset(&neib_plan, 0, sizeof(neib_plan));
	}		
};

struct ConsoleNeibPlan
{
	int  this_section;
	int  neib_console_num;
	NeibConsolePlanReverse neib_console[10];
	ConsoleNeibPlan()
	{
		this_section=0;
		neib_console_num=0;
		memset(&neib_console, 0, sizeof(neib_console));
	}
};

struct StationSideNotChange
{
	int station;// ��վ
	int flag;   // 1:�ͳ� 2:���� 3:�ͻ���
	int side_count;
	int side[10];

	StationSideNotChange()
	{
		flag=0;
		station=0;
		side_count=0;
		memset(side, 0, sizeof(side));
	}
};

struct SectionStationNotChange
{
	int station_count;
	StationSideNotChange info[20];

	SectionStationNotChange()
	{
		station_count=0;
		memset(info, 0, sizeof(info));
	}
};
struct PlanNoteTip
{
	int conid;
	int stano;
	int nxtstano;
	PlanNoteTip()
	{
		conid = 0;
		stano = 0;
		nxtstano = 0;
	}
};
struct DcmdTypeCnt
{
	char tmisType[32];
	long cascoType;
	DcmdTypeCnt()
	{
		cascoType = 0;
		memset(tmisType, 0, sizeof(tmisType));
	}
};

struct NeibStation
{
	int nPreStation; // ����վǰһվ
	int nNeibStaNo;  // ����վ
	int nStationNo;  // ����վ��һվ
	int nNextNextStationNo;// ����վ���һվ
	
	NeibStation()
	{
		nPreStation = 0;
		nNeibStaNo = 0;
		nStationNo = 0;
		nNextNextStationNo=0;
	}
};

class NeibStationPlan
{
public:
	int nSectionNo;
	std::vector<NeibStation> vecNeibStation;

public:
	NeibStationPlan();
	~NeibStationPlan();
	NeibStationPlan(const NeibStationPlan& rhs);
	NeibStationPlan& operator=(const NeibStationPlan& rhs);
};

struct CheckTrainDepart
{
	int station_warning;
	int station_check;

	CheckTrainDepart()
	{
		station_warning=0;
		station_check=0;
	}
};

struct SectionCheckTrainDepart
{
	int con_id;
	int count;
	CheckTrainDepart check[50];
	SectionCheckTrainDepart()
	{
		con_id=0;
		count=0;
	}
};

struct FromNoCal
{
	int conid;
	int station;
	int entry;
	FromNoCal()
	{
		conid=0;
		station=0;
		entry=0;
	}
};

struct ToLimitCal
{
	int conid;
	int station;
	int nextstation;
	ToLimitCal()
	{
		conid=0;
		station=0;
		nextstation=0;
	}
};

struct Entry_Exit_Default
{
	int station;
	int nextstation;
	int lineno;
	Entry_Exit_Default()
	{
		station=0;
		nextstation=0;
		lineno=0;
	}
};

struct NeibSectionStation
{
	int station_count;
	int station_no[16];
	int this_section[16]; // �Ƿ�̨��Ͻ��־ 1:��̨��Ͻ��վ 0:�Ǳ�̨��Ͻ��վ
	NeibSectionStation()
	{
		station_count=0;
		memset(station_no, 0, sizeof(station_no));
		memset(this_section, 0, sizeof(this_section));
	}
};

struct NoLimitSectionStation
{
	int station_count;
	int station_no[64];
	NoLimitSectionStation()
	{
		station_count=0;
		memset(station_no, 0, sizeof(station_no));
	}
};

struct NeibStationList // ��̨��ȫ��������
{
	int nThisSection;
	int nNeibSection;
	int entry;
	int exit;
	int station;
	int n_PreStation;
	int n_NextStation;
	NeibStationList()
	{
		nThisSection=0;
		nNeibSection=0;
		entry=0;
		exit=0;
		station=0;
		n_PreStation=0;
		n_NextStation=0;
	}
};

struct NeibAdjustExtern // ��̨��������
{
	int nThisSection;
	int nNeibSection;
	
	int nStation;
	int exit_count;
	int exit[8];
	int nNextStation;
	int nPassway;
	int nNodeIndex;

	int neib_count;
	int neib_station[8];
	int passway_list[8];
	int index_list[8];

	NeibAdjustExtern()
	{
		Init();
	}
	void Init()
	{
		nThisSection=0;
		nNeibSection=0;
		nStation=0;
		exit_count=0;
		memset(exit, 0, sizeof(exit));
		nNextStation=0;
		
		nPassway=0;
		nNodeIndex=0;
		neib_count=0;
		memset(neib_station, 0, sizeof(neib_station));
		memset(passway_list, 0, sizeof(passway_list));
		memset(index_list, 0, sizeof(index_list));
	}
};

struct SplitTrainExtern // �ָ��г�
{
	int nThisSection;
	int position_station;

	int station_pair_count;
	int station[8];
	int station_exit[8];
	int next_station[8];
	int next_station_entry[8];
};

struct StaEntry  // ���Ͼ�,�̳�--�̳Ǳ��������������
{
	int section;
	int flag; // ƥ��������λ�� 1:��վ 2:�м�վ
	int count;
	int station[8];
	int entry[8];
	int exit[8];
	StaEntry()
	{
		count=0;
		flag=0;
		for(int i=0; i<8; i++)
		{
			station[i]=0;
			entry[i]=0;
			exit[i]=0;
		}
	}
};

struct SectionStaEntry  // ���Ͼ�,�̳�--�̳Ǳ��������������
{
	int section;
	int count;
	StaEntry entry[16];
	SectionStaEntry()
	{
		section=0;
		count=0;
	}
};

class NeibPlanParam
{
public:
	int first_station_index;
	int first_station;
	int next_station;
	int next_next_station;
	int neib_station;
	int consider_start_train; // ��̨ʼ����,�Ƿ�ȥ��̨����, 0:��ȥ��̨����, 1:ȥ��̨����
	NeibPlanParam()
	{
		first_station_index=0;
		first_station=0;
		next_station=0;
		next_next_station=0;
		neib_station=0;
		consider_start_train=0;
	}
};

struct StationCheckNode
{
	int station;
	int setbcheck;
	int setscheck;
	int setwcheck;
};

class CheckStationsOfSection
{
public:
	int count;
	StationCheckNode station_list[256];
public:
	CheckStationsOfSection()
	{
		Init();
	}
	void Init()
	{
		count=0;
		memset(station_list, 0, sizeof(station_list));
	}
};

class NodeDefine
{
public:
	NodeDefine()
	{
		Init();
	}
	void Init()
	{
		index=-1;
		station=0;
		entry_line_num=0;
		memset(entry_line, 0, sizeof(entry_line));
		exit_line_num=0;
		memset(exit_line, 0, sizeof(exit_line));
	}
public:
	int index;
	int station;
	int entry_line_num;
	int entry_line[32];
	int exit_line_num;
	int exit_line[32];
};

class SectionDefine
{
public:
	SectionDefine()
	{
		Init();
	}
	void Init()
	{
		con_id=0;
		stdef_num=0;
		for(int n=0; n<128; n++)
			stinfo[n].Init();
	}
   
public:
	int con_id;
	int stdef_num;
	NodeDefine stinfo[128];
};

class StationStplanNotWarning
{
public:
	StationStplanNotWarning()
	{
		Init();
	}
	void Init()
	{
		station_count=0;
		memset(station_lists, 0, sizeof(station_lists));
	}
public:
	int station_count;
	int station_lists[128];
};

class CalSideTrainType
{
public:
	int nSection;
	int nCount;
	int nTrainType[256];
public:
	CalSideTrainType()
	{
		Init();
	}
	void Init()
	{
		nSection=0;
		nCount=0;
		memset(nTrainType, 0, sizeof(nTrainType));
	}
};

class CTGFunctionConfig
{
public:
	enum{MAX_LOGIC_SECTION_NUM=128,
		MAX_SPLIT_DEF_NUM=64,
		MAX_NOTIFY_STRLEN=128,
		MAX_NOTIY_COUNT=24,
		MAX_FJK_STAT_NUM=64,
		MAX_NEIB_FJK_STATION_COUNT=5};

	enum{ADJUST_FLAGBIT_BLOCK=0,
		 ADJUST_FLAGBIT_TRAIN=1,
		};

	struct
	{
		short section_id;
		short tmis_id;
		char  disid[10]; // ����
		char  jhdisid[10]; // �ƻ�̨����
		char  kddisid[10]; // �͵�̨����
	}m_aSectionList[MAX_LOGIC_SECTION_NUM];
	int   m_nSectionCount;

	struct
	{
		short section_id;
		int   station1;
		int   station2;
		int   station1next;   // station1��һվ
		int   station2pre;    // station2��һվ

		int   first_entry;  // ǰ���г����
		int   first_exit;   // ǰ���г�����
		int   second_entry;  // ����г����
		int   second_exit;  // ����г�����
	}m_aSplitTrainDef[MAX_SPLIT_DEF_NUM];
	int m_nSplitDefCount;

	struct
	{
		int   day_time; //0�㿪ʼ�ķ�����
		char  notify_str[MAX_NOTIFY_STRLEN];
	}m_aAutoNotifyDef[MAX_NOTIY_COUNT];
	int m_nAutoNotifyDefCount;

	int   m_aFJKStations[MAX_FJK_STAT_NUM];
	int   m_nFJKStationCount;

	struct NeibPlanStruct
	{
		int  source_section;
		int  dest_section;
		int  prev_station;
		int  first_station;
		int  next_station;
		int  next_next_station;
		int  passway;
		int  node_index;
		
		int  nWholeMatch;
		int  accept_ternimal_train;
		int  neib_station; // ̨��վ
		int	 control_prev_abs; // �Ƿ��̨��վ��һ����
		int  consider_start_train; // ��̨ʼ����,�Ƿ�ȥ��̨����, 0:��ȥ��̨����, 1:ȥ��̨����
		NeibPlanStruct()
		{
			source_section=0;
			dest_section=0;
			prev_station=0;
			first_station=0;
			next_station=0;
			next_next_station=0;
			passway=0;
			node_index=255;
			nWholeMatch=1;
			accept_ternimal_train=0;
			neib_station=0;
			control_prev_abs=0;
			consider_start_train=0;
		}		
	};
   vector<NeibPlanStruct>  m_aNeibPlanConfig;

   struct NeibPlanEntryStruct
	{
		int  source_section;
		int  dest_section;
		int  fjk_station;
		int  passway;
		int  node_index;
		int  exit;  // ��̨fjk_station����
		int  bqd_entry; // ��̨fjk_station���,���ڱ�Ǯ��
		int	 entry;  // ��̨fjk_station���,����ɸѡ��̨�ƻ�
		NeibPlanEntryStruct()
		{
			source_section=0;
			dest_section=0;
			
			fjk_station=0;
			passway=0;
			exit=-1;
			bqd_entry=-1; //20140911
			node_index=255;
			entry=-1; 
		}		
	};
   vector<NeibPlanEntryStruct>  m_aNeibPlanEntryConfig;
	struct NeibPlanMode
	{
		int  this_section;
		int  neib_section;
		int  mode; // 1:���ճ���� 2:����ǰ��վ
		NeibPlanMode()
		{
			this_section=0;
			neib_section=0;
			mode=0;
		}
	};
	vector<NeibPlanMode>  m_aNeibPlanMode;

   // ���շ����г���̨�ƻ�����  
   std::vector<ConsoleNeibPlan> console_reverse_neibplan;

   // ����������̨�ƻ�����
   std::vector<ConsoleNeibPlanSY> console_neibplan;
   std::vector<int> m_notUseSyNeibPlan; // ���ò�������ģʽ��̨�����ļ�(neibplan.xml)���Ƶ���̨�ƻ�,�������ʹ��ͨ����̨����

   // �������
   std::map<int, ConsoleChangeId> console_change_trainid;

   // ��վ�������
   std::map<int, SectionCheckTrainDepart> section_check_depart;
   struct StationDefaultEntry
   {
	   short  station;
	   short  in_down_entry;  // Ĭ�Ͻ������г�����
	   short  in_up_entry; // Ĭ�Ͻ������г�����
	   short  out_down_entry; // Ĭ�Ͻ������г�����
	   short  out_up_entry; // Ĭ�Ͻ������г�����
	   StationDefaultEntry()
	   {
		   station=0;
		   in_down_entry=0;
		   in_up_entry=0;
		   out_down_entry=0;
		   out_up_entry=0;
      }

   };
   vector<StationDefaultEntry>  m_aStationDefaultEntryConfig;

   struct AdjustGapTime
   {
	   short              station;
	   short               act1;
	   short          act2;
	   short              type1;
	   short                  type2;
	   long                    gap_time;
	   AdjustGapTime()
	   {
		   station=0;
		   act1=-1;
		   act2=-1;
		   type1=-1;
		   type2=-1;
		   gap_time=0;
      }
   };
   vector<AdjustGapTime>  m_aAdjustGapTimeConfig;
	
   vector<int>  m_aInsideUsedTrainTypes;  // ·�ó�����鿨��
   vector<int>  m_aFreeAddTrainTypes;  // ����ֱ�����ӵ��г�����
   vector<int>  m_vecCheckPlanSection; // �¼ƻ�Ĭ�ϼ��վϸ����̨
   vector<int>  m_vecSectionThirtyByPlan;  // 30���Ӱ��ƻ������������κ�
   
   vector<DcmdTypeCnt> m_vecDCMDType;

   map<int, NeibStationPlan> m_neigStationPlan; // ̨��վ�г��ƻ���������
   struct FJK_LATE_TIME
   {
	   int station;
	   int method; // 0: ���� 1:����
	   FJK_LATE_TIME()
	   {
		   station=0;
		   method=0;
	   }
   };
   vector<FJK_LATE_TIME> m_vecFjkStatisticLateTime;

   vector<SplitTrainExtern> m_NewSplitList;
   vector<NeibAdjustExtern> m_neibAdjustStation;
   vector<NeibStationList> m_neibRecvAllStation;

   vector<long> m_GoodTrainType; //  �ϲ���,�����зǼ���ͣ��Ļ�������,���� С��ת��
   vector<long> m_PessengerTrainEndTechType; // �ϲ���,����ͳ��յ��Ǽ���ͣ��Ŀͳ�����,���� ���쳵��
   vector<long> m_PessengerTrainStartTechType; // ����ͳ�ʼ���Ǽ���ͣ��Ŀͳ�����
   vector<int> m_UseOPMSTrainType; //   �ϲ���,���������ȫʹ��OPMS����ʱ����г�����

   struct USE_TDMS_LINE_STATION
   {
		int con_id;
		int station_id1;
		int station_id2;
		int station_id1_nd;
		int station_id2_nd;
		USE_TDMS_LINE_STATION()
		{
			con_id=0;
			station_id1=0;
			station_id2=0;
			station_id1_nd=255;
			station_id2_nd=255;
		}
   };
   vector<USE_TDMS_LINE_STATION> m_UserTDMSLineStation; // ���ݾ�, �����ֳ�վ��ȫʹ��TDMS���߱�

   map<int, SectionDefine> m_tg_define;
   
   WarnIdLeval m_stplanLeval[5];
private:
	char  m_szDataPath[512];
	char  m_szTrainBZCXurl[256];
	char  m_szWsDCMDurl1[128];
	char  m_szWsDCMDurl2[128];

private:
	char m_szGSoapAuthName[256];
	char m_szGSoapUrl[256];
	char m_szGSoapUrl2[256];

public:
	void GetWarnIdLeval(int level, WarnIdLeval& cfg);
	bool GetStaEntry(int section, SectionStaEntry& entry); // ���Ͼ�,�̳�--�̳Ǳ��������������
	int  GetNewSplitStation(int nThisSection, int station_list[], int entry_list[], int max_size) const;
	int  GetNeibEqualStation(int nThisSection, int nNeibSection, NeibAdjustExtern neibPlanStation[], int max_size) const;

	int  GetNeibStationList(int nThisSection, int nNeibSection, NeibStationList ns[], int max_size) const;

	bool UseOPMSTrainTimeTrainType(int type);
	bool IsOtherBureaSection(int sectionno); // �ж��Ƿ�Ϊ���������Σ�������̨�ƻ��� true:������� false:��������
	int GetNeibScheduleMinStopTime_Shanghai(BOOL type); // ��ȡ��Сͣվʱ�� ������̨�ƻ�
	void GetNeibStructs(int this_section,int neib_section,std::vector<NeibPlanParam>& neibplan_param_vect);
	bool IsEnableExactNeibPlanMode() { return m_bEnableExactNeibPlanMode;}
	bool IsNeibPlanShowAcceptInfoOrder() { return m_bNeibPlanShowAcceptInfoOrder;}
	bool IsQuerySchdClose() { return m_bQuerySchdClose;}
	bool IsShowColorNeibPlanTip() { return m_bShowColorNeibPlanTip;}
	int  GetFjkStatisticMethod(int station);
	int  GetNeibStations(int nSection, int station, std::vector<NeibStation>& vecNeibStaPlan);
	
	CGpcdef *GetGPCDEFx();
	const char *GetDataPath() const;
	BOOL  IfCalculateBasicTrainType() const;
	BOOL  NeiConsiderPassenger()  const;
	BOOL  IfSetDefaultTechStop() const;
	int   GetSuperSpaceTrainType() const;
	BOOL  IfdChoseSideWhileAllFull() const;
	BOOL  IfChoseMainSideWhilePass() const;
	
	int   GetDefaultRunTime() const;
	int   GetDefaultStopExtra() const;
	int   GetDefaultStartExtra() const;

	BOOL  IsDefaultAccept() const;  // �˹��ӳ�Ĭ��
	BOOL  IsDefaultTerminate() const;// �˹��ӳ�Ĭ��

	BOOL  IsLocalLogSendStPlan() const;
	int   GetSendWholePlanCronTimes() const;

	
	int   GetMaxPlanReceverCount() const {return 64;}
	int   GetMaxPlanTrainCount()  const {return 512;}

	int    GetDefaultSendPlanTRange() const {return m_nDefaultSendPlanTRange;}
	int    GetDefaultNeibPlanTRange() const {return m_nDefaultNeibPlanTRange;}
	BOOL  IsDefaultSendPlanClose() const;
	WORD  GetFileSaveFlag() const;
	int   GetFileKeepTimeHour() const;

	const char *GetBZCXurl() const {return m_szTrainBZCXurl;}
	const char *GetWSDCMDurl1() const {return m_szWsDCMDurl1;}
	const char *GetWSDCMDurl2() const {return m_szWsDCMDurl2;}

	char *GetGSoapAuthority()  {return m_szGSoapAuthName;}
	const char *GetGSoapURL() const {return m_szGSoapUrl;}
	const char *GetGSoapURL2() const {return m_szGSoapUrl2;}

	int   GetAutoShift2Time() const;
	int   GetAutoShift1Time() const;
	BOOL  IsMMPlanWSEnabled() const;
    BOOL  IsMMPlanEditable() const;
	int   GetSectionIDByTmisID(int tmis_id,char *dispid);
    int   GetTmisSectionID(int con_id,char *dispid=NULL,char *jhdispid=NULL,char *kddispid=NULL);
	BOOL  IsDefaultTrigByPlan() const;

	BOOL MatchSplitTrain(int con_id,int station1,int station2, int *station1next, int *station2pre, int* first_entry, int* first_exit, int* second_entry, int* second_exit);
	void  SetCheckManualOper(BOOL va);
	BOOL  IfCheckManualOper() const;
	BOOL  IsDebugTDMsg() const {return m_bDebugTDMsg;}
	void  SetDebugTDMsg(BOOL va) {m_bDebugTDMsg=va;}
	bool IfCheckAndSetAlarm() const {return m_bCheckAndSetAlarm;}
	
	BOOL IsNeibPlanConsiderSameTrain() const;

	BOOL IsStplanTrainIndexSaveInList() const;
	BOOL IsCheckExitValueSetDefualtExit() const;
	BOOL IsCheckEntryValueSetDefualtEntry() const;
	int  GetLineNo(int station, int nextstation);
private:
	int  m_nDefaultNeibPlanTRange;
	int  m_nDefaultSendPlanTRange;
	BOOL m_IfChoseMainSideWhilePass,m_IfCalculateBasicTrainType,m_NeiConsiderPassenger;
	BOOL m_IfSetDefaultTechStop,m_IfdChoseSideWhileAllFull;
	BOOL m_bLocalLogSendStPlan;
	int  m_nSuperSpaceTrainType;
	int  m_nDefaultRunTime;
	int  m_nDefaultStopExtra;
	int  m_nDefaultStartExtra;
	int  m_SendWholePlanCronTimes;
	BOOL m_bDefaultAccept;
	BOOL m_bDefaultTerminate;
	BOOL m_bDefaultSendPlanClose;
	BOOL m_bMMPlanWSEnabled;
	BOOL m_bMMPlanEditable;
	WORD m_nFileSaveFlag;
	int m_nFileKeepTimeHour;
	BOOL m_bDefaultTrigByPlan;
	BOOL m_bCheckManualOper;
	BOOL m_bDebugTDMsg;
	bool m_bCheckAndSetAlarm;
	
	bool m_bCheckStPlanWithBasicSchd;
	bool m_bNewNeibModeAddTrain;
	bool m_bEnableOPMSNodeIndex;
	BOOL m_bStationBlockNotCheckSide;

	BOOL m_bEnablePasstoStayModifyArivalTime;
	BOOL m_bEnableTempAddTrain;
	BOOL m_bEnableSelPasswayByDirectTrain;
	BOOL m_bEnablePlanNoticeDCmd; // �Ƿ��¼ƻ�֪ͨ���������ն�

	BOOL m_bEnableOPMSBasicSchdSide; // �Ƿ�ʹ��OPMS����ͼ�ɵ�
	BOOL m_bEnableOPMSShiftSchdSide; // �Ƿ�ʹ��OPMS�հ�ƻ��ɵ�
	bool m_bIsGoodTrainUseOPMSSide;  // �����Ƿ�ʹ��opms�ɵ�
	std::vector<int> m_vConfigGoodTrainTypesOfUseOPMSSide; // ����ʹ��TDMS�ɵ��Ļ����г�����

	bool m_bIsBasicValidSideByBasicSchd;	// ��ʹ��OPMS����ͼ�ɵ�ģʽ�£�����OPMSδ���Ĺɵ����Ƿ�ʹ�ó��û���ͼ�ɵ�
	bool m_bIsBasicValidSideByCalculate;	// ��ʹ��OPMS����ͼ�ɵ�ģʽ�£�����OPMSδ���Ĺɵ����Ƿ����Ĭ�Ϲɵ�

	bool m_bIsCalBasicSchdExitEntry; // Ԥ������ͼ�Ƿ��������, Ĭ�ϼ���
	bool m_bIsCalShiftSchdExitEntry; // �հ�ƻ�ͼ�Ƿ��������, Ĭ�ϼ���

	std::vector<int> m_vNotUseBasicSideSection;
	std::vector<int> m_vNotUseShiftSideSection;

	BOOL m_bDisEnableWSSaveDCmd;
	BOOL m_bEnablePrintNoteTrainStopOneHour; // �г�ͣ������һСʱ,�ڼ����д�ӡ
	BOOL m_bEnableSelectTrainByLanzhou;
	BOOL m_bEnableInputEngineId;
	BOOL m_bEnableLanzhouEnginePlot;
	bool m_bSecretTrainToSta;
	bool m_bEnableCheckPlatform; // ���վ̨
	
	BOOL m_bTempAddTrain;// ��ʱ�ӳ�
	BOOL m_bEnableFreeDrawSetBasicTrainPlanTime; // ���ɻ����г��Ƿ���ͼ���г�����ͼ���ƻ�ʱ��
	BOOL m_bUnknowTrainStPlanRunType;  //δ֪�г��·��׶��Ƿ����еȼ��Ƿ��´�Ϊ255
	bool m_bCheckCtcModeStaCtrlStPlan; //�Ƿ���CTCģʽ��վ��ȫ���Ƽƻ�
	BOOL m_bCheckAllStationStPlan; // �Ƿ������г�վ�ƻ�
	BOOL m_bCheckTrainMeetByNoMeet; // �����г��༭�����г�������
	BOOL m_bDisenableSheetTab;  // �Ƿ�ֻ������ҳ
	BOOL m_bDisenableSheetTabExit;  // ����ҳ����һ�����Ƿ�ֻ��

	bool m_bStplanSelectAllRecEntity; // true:ÿ�δ򿪽׶μƻ���ѡ�����н��ܼƻ���λ false:�����ϴ�
	bool m_bRemoveChineseCharSetTrainType; // �ж��г������Ƿ�ȥ��ǰ׺����
	bool m_bEngineChangeGiveoutTip; // �����仯�Ƿ���ʾ
	bool m_bTrainPassToStopUseMainSide; // true: û�в��ߵ�����ʹ������ false: û�в��ߵ�������ֹʹ������

	bool m_bStplanDefaultSelLirc; // true:�·��׶μƻ�ֻѡ�����ɻ� false:�·��׶μƻ�ѡ�������豸
	bool m_bGetEngineFromPreConID; // true: ��ȫ�����ϸ�̨������ false: ��������һ��վ������
	bool m_bSendStplanUseName; // true: �·��׶μƻ�������Ա���� false: �·��׶μƻ�������Ա����
	bool m_bEnableDeleteFromTmisBlock; // true: ����ɾ��������ͼ��ʩ�� false:������ɾ��������ͼ��ʩ�� 
	bool m_bTrainIdUseArriveId; // true: �г�ʹ�õ�һ��վ���복�� false: �г�ʹ�õ�һ��վ��������
	
	bool m_bTaiYuanNeigbourPlan; // true: ����̫ԭģʽ����
	bool m_bChangeRouteTip;      // �������ص㳵�ľ�·����Ƿ���ʾ,Ĭ�ϲ���ʾ
	bool m_bGoodTrainUseBasicSide; // ����ͼ�ӳ������ɵ��ͻ���ͼ����һ�� true: ����һ�� false: ���¼���
	bool m_bSTIQueryBianzu; // �г������ѯ false:����ʹ��STI; true:ʹ��STI��ѯ
	
	bool m_bAddTrainShiftAndBasic; // true:���ɻ��ߣ�ѡ�а����հ�ӳ������ȼ���հࡢ�ټ�����ͼ
	bool m_bEnableThirtyByPlan; // ���ձ���ģʽ��,�·��׶μƻ��Ƿ��޸Ľ�·����Ϊ���ռƻ� true: ������ǰ30�����г�������Ϊ���ƻ����� false:���޸�
	
	bool m_bNeibPlanConsiderSameTrain; //2012.04.25 �ж��Ƿ�����̨�ƻ��е��س��κŵĳ� true:���� false:������
	
	bool m_bStplanTrainIndexSaveInList; //2013.02.22 �ж϶����¹��ƻ����г������߽��ж���ļ�¼ true:���� false:������
	bool m_bCheckExitValueSetDefualtExit; //2013.02.28 �����޸�,���ݾ�����������Ŀ true:��������Ч�� false:�����
	bool m_bCheckEntryValueSetDefualtEntry;//2013.05.23 �������޸�,����������Ŀ true:��������Ч�� false:�����
	// �Զ�����ʱ��
	int  m_nAutoShift1Time; // �绻��ʱ��, ��0:00�������
	int  m_nAutoShift2Time; // ����ʱ��, ��0:00�������

	char m_szTrainId[25];
	int  m_dcmd_prefix;
	std::vector<PlanNoteTip> m_vctPlanTip;

	int  m_AutoAdjustOperFlag; // Bit 0: ���Զ�����  1: �˹�����-�Զ����õ��� 
	int  m_AutoAdjustStepFlag; // Bit 0: ��������ʩ������ Bit 1:���������г���ͻ

	int  m_nDepartDelayTimeOfBlockedABS;  // ������������󣬷�����ʱ�룬�Զ�������

	bool m_nSetPassengerTrainNoTechStopLockSide;//�ÿ��г����վĬ�������ɵ�
	bool m_bIsSubaoShowSpace; // �Ƿ��ٱ���ʾ����
	bool m_bSetStplanSpackRankByStation;//�г���վ�޳���ʱ,�·��׶μƻ��Ƿ��ճ�վ���޵ȼ��·� 0:Ĭ��ֵ �������������·� 1:���ճ�վ�ȼ��·�

	// ���г�ͨ���䵽�����ߵ�����ͨ��ʱ, ���ı�ɵ��г�����
	std::vector<int> m_vecNoChangeSideWhileStopPass;
	
	bool m_bEnableLockWholeTrainSide; // true: �����������г��ɵ� false:�����������г��ɵ�

	bool m_bEnableCzh; // true:���ó������ false:���ó������
	bool m_bEnableNewCzh; // true:�����³��������  false:�����³��������
	bool m_bChangeTrainID; // �ϳ���ģʽ�� true:�޸����������� false:���޸�����������, �³���ģʽ �̶�Ϊfalse:���޸�����������
	bool m_bEnableTDMS5; // true:TDMS5.0 false:TDMS4.0
	int m_nAdjustAcceptRelation;  // �����ƻ��ϵ���ͼ��·��ϵ���¶��� 0:������ 1:����
	int m_nNeibplanAcceptRelation;  // ��̨�ƻ��ϵ���ͼ��·��ϵ���¶��� 0:������ 1:����
	// ��ȼ�����������ߵ�̨
	std::vector<int> m_vecConDieselLocomotiveDrawGreen;

	bool m_bDeleteRepeatDeleteStplan;//true:ɾ������ɾ���ƻ� false:�����

	bool m_bUseStringKiloMeter; // true:ʹ���ַ����Ĺ���� false:��ʹ�� ��20140328���̫ԭ����Ҫʹ�ò���K��ͷ�Ĺ����ָ����
	bool m_bIsNeibplanOnlyUseOPMSSchd; // true:��̨�ƻ��ӳ�ʱ����ͨ��OPMS�����������±�����ʽ��� false:Ĭ��,�����øù���
	bool m_bShowMMPlanSkyLightButton; // true:ʩ���ƻ���������������ť"ʩ��"��"�촰",�ֱ���й�����ʾ  false:Ĭ��,�����øù���
	bool m_bShowMMPlanMaintainButton; // true:ʩ���ƻ���������������ť"ʩ��"��"ά��",�ֱ���й�����ʾʩ����ά�޼ƻ�  false:Ĭ��,�����øù���

	bool m_bPlaySoundCrew;
	bool m_bPlaySoundNeiPlan;
	bool m_bBasicStopSetNoTechStop;
	bool m_bShiftStopSetNoTechStop;
	bool m_bFreedrawStopSetNoTechStop;
	
	bool m_bPassengerTrainStartSetNotTechStop; // �Ϻ���, ʼ���ͳ�,��վĬ�����óɷǼ���ͣ��
	bool m_bPassengerTrainEndSetNotTechStop; // �ϲ���, �յ��ͳ�,���һ��վĬ�����óɷǼ���ͣ��
	bool m_bNeibPlanPassengerTrainAddNotByBasic; // �ϲ���, �ͳ���̨���������ջ���ͼ�ӳ�
	bool m_bNeibPlanTrainAddOnlyByShift; // ���Ͼ�, ��̨�������ۿͻ��������Ȱ����հ�ӳ�,����������,ǰ��ʧ��Ϊ����
	int  m_nNeibPlanAddNewTrainMode;  // ��̨����ʽ0:�����г� 1:�������г�(����) 2:�г���Դ�����հ�ƻ�ʱ�������г�(�人)
	bool m_bSelBackupSideByCfg; // �ϲ���, ��ѡ�ɵ���������˳��ѡ��
	bool m_bShiftTrainSelFirstBackupSide; // �ϲ���, �հ�ƻ�ͼ�ͳ�����ɵ�,ѡ��һ���ùɵ�,�����m_bSelBackupSideByCfg����ʹ��
	bool m_bGoodTrainNotTechStopNeedPwd; // �ϲ��ֻ����г�,�Ǽ���ͣ�㵽����ͨ����Ҫ������ʾ true:��Ҫ false:ȱʡֵ,����Ҫ

	bool m_bQuebaoQueryHaveChinese; // ��ѯȷ�� �������κ��к���
	bool m_bShowArriveTrainid; // ��ʾ���ﳵ��

	bool m_bDisEnablDeleteRecordWhileLimitModifyActual;
	bool m_bClosePollingCommStatus;
	bool m_bCalExitEntryByTrainId; // opms�������г�����վ��ڣ�ĩվ���ڼ���, false:Ĭ��ֵ,���ձ�վ���η���ѡ������ true:������������ѡ������ 
	bool m_bHasNoLimitStation; // �Ƿ���ڲ��ܷ������ͼ���Ƶĳ�վ

	bool m_bTrainSideUseBasicSide; // �����г��ɵ�ʹ�û���ͼ�ɵ�, �Ϻ��� false ������ true
	
	bool m_bEnableRecvAllAdjustPlan; // �Ϻ��������Ƿ���ȫ���յ����ƻ�, true  �Ϻ���   false  ������(Ĭ��ֵ)
	bool m_bIsAlarmOnlyCheckCRHTrainSide;  // �·��׶μƻ�ʱ�����̶��ɵ���Ϣ������  true ����鶯�����г�  false ������пͳ�(Ĭ��ֵ)
	bool m_bIsAlarmNotCheckSpackTrainMeetInfo;  // �·��׶μƻ�ʱ����� ���޳���ͬһ��վ�ڵĽ��� true ����� false���(Ĭ��ֵ)
	
	int m_nCheckElecSideWhenPassToStop; // ������Ҫ�� ͨ���䵽��ʱ ���Ӽ������ɵ��������� 0:Ĭ�� ������ 1 ��������Ŀ��Ƶ�CTC��վ 2 ������г�վ
	bool m_bEnablePlotMainSide; // ������Ҫ�� ���Ӵ�ӡ����ͼʱ �Ӵ����߹ɵ�
	bool m_bQuestionStationPlanSelState; // �·��׶μƻ�,�Ƿ���ʾδѡ���ռƻ���λ����
	bool m_bQuestionStationPlanPassengerSta; // �·��׶μƻ�, ��ʾ�Ƿ��·��÷�ϵͳ
	bool m_bEnableNeibPlanFreightSameToPassenger; // ���ݾ�Ҫ�� ��̨�ƻ��л�����ͳ���ͬ��ʽ���д���
	
	bool m_bEnableLogTDMessage; // true;Ĭ��,��־��¼TD�ӿڴ���������Ϣ��Ϣ false:�����øù���
	bool mbSupportInterCityStPlan; // 20150623�Ƿ�֧�������ǳǼ��۷��ƻ� true:������ false:ͨ��·�ּƻ�
	bool m_bEnableRecvAdjustPlanTime; // ���������� �������е����ƻ���ʱ��� true ������ false ������(Ĭ��ֵ)
	bool m_bModifyTrainActAndGTIDSame; // �����ƻ���ȫ��ID��ͬ����ʵ�ʵ�ʱ�Ƿ�����г�
	bool m_bNewModeLoginRefresh; // ��ģʽ(ǰ12��12)ÿ�ζ�ˢ������
	int m_nReportEarlyTime; //���Ͼ��·��׶μƻ�ʱ�������ÿ��г�����ͼ����N���ӽ�����ʾ
	bool m_bConsiderStplanSide; // �׶μƻ���ǰ��վ�ɵ��Ƿ�����-1,false:������ true:����
	std::map<int, SectionDefaultElec> default_elec;

	std::map<int, SectionStationNotChange> side_not_change;
	std::vector<ToLimitCal> limit_section;
	std::vector<FromNoCal> notcalfromconid;
	std::map<int, NeibSectionStation> m_neibSectionStation;
	std::map<int, NoLimitSectionStation> m_noLimitStationList;
	int m_nSectionStaEntryCount;  // ���Ͼ�,�̳�--�̳Ǳ��������������
	SectionStaEntry m_specialEntry[64]; // ���Ͼ�,�̳�--�̳Ǳ��������������
	std::vector<int> m_otherBureaSection; // ������������κţ�������̨�ƻ�
	std::vector<Entry_Exit_Default> default_entry_vet;

	// �Ϻ��� ��ȷ��̨�ƻ� ģʽ
	bool m_bEnableExactNeibPlanMode;
	int m_nExactNeibPlanModeCrhStopTime;  // ������ͣ��ʱ��
	int m_nExactNeibPlanModeOtherStopTime; // �����г�ͣ��ʱ��
	bool m_bNeibPlanShowAcceptInfoOrder; // ������ʾ̨��վʱ��
	bool m_bShowColorNeibPlanTip; // �Ϻ���,��̨������ʾ��ɫ������

	// ���Ͼ�����
	bool m_bAcceptImportNote; // �Ƿ���մ���OPMS���͵��ص����� true:��ʾ false:����ʾ
	bool m_bGiveChoicePathChange; // ���յ����ƻ��󣬵��г���·�����仯���Ƿ���ʾ����Աǩ��
	
    int  m_nBasicPreHour, m_nBasicBackHour; // ������ϵ����ͼ,Ĭ�ϲ���ǰ��ʱ��Χ������ǰ��ʼ��ʱ�䣬��ǰ6Сʱ�����Һ�ʱ��Χ������ǰ���յ�ʱ�䣬���6Сʱ��
	bool m_bSetPreBasicTechStop; // false:������,ֻҪ��·һ�¾�ƥ�� true:Ԥ������ͼ�г�Ϊ����ʱ��ƥ�䳣�û���ͼ��Ӫҵ����
	bool m_bNeedCompareTrainExpand; // true:Ĭ��ֵ,�Ƚ��г����䲿�� false���Ƚ��г������г����䲿��
	bool m_bNeedWaitMarkEnd; // true:Ĭ��ֵ,�����ӳ���ȴ����Ž����ٻ��� false�����ȴ�
	bool m_bGpcSendReqOtherMsg; // �հ�ƻ��г����������͵���Ӫҵ�ͳ�����Ϣ��ô���� true:����ͼ���� false:TD�Լ�����,����ͼ���ڷ�������ӿ��ٶ�
	bool m_bSendLogToLogsvr; // �Ƿ���־���͸�logsvr true:���� false��Ĭ��ֵ,������
	bool m_bBasicDeletePassengerFreight; // ���Ͼ�����,Ԥ������ͼ�п��Ե���ɾ���ͳ����߻���
	bool m_bGenAllMarkByDeleteExist;	// ���������,ȫ��ʩ��������ͼʱ,Ӧ������ɾ���Ѿ����ڵ���ͬ����
	bool m_bConsiderTwicePassSameStation;	// �׶μƻ������г�����ͨ��ͬһ��վ�Ƚϻ���ͼ�Ƿ��Ǹ���� false:������ true:����
	bool m_bCompatibleOldStPlan; // �Ƿ�����ϵļƻ�
	bool m_bEnableModifyGID; // �Ƿ��������ȫ��ID
	bool m_bCheckShiftTrainAddTime; // �հ�ƻ������£����μ��ʱ���Ƿ�Ƚ�ʱ��
	bool m_NotSelAdjustPlan; // �����ƻ�,�������Ĭ�ϲ�ѡ��
	bool m_bUsingWorkGoodMode; // ���λͼ�Ż�ģʽ
	std::vector<int> m_NotCheckSection; // ������̨�ƻ�ʱ,����鱾̨�Ƿ���ڱ���̨��������ߵĵ�������
	bool m_bBasicShowCompareDlg; // ���ջ���ͼ���,�Ƿ���ʾ�Ƚ϶Ի���
	bool m_bShiftShowCompareDlg; // �����հ�ƻ����,�Ƿ���ʾ�Ƚ϶Ի���
	bool m_bEnableSelGenBasicTrain; // ���ݾ�,�Ƿ�����ѡ�����ͼ�г����ɻ���ͼ
	bool m_bQuerySchdClose; // ���ݾ�,��ѯ̨�л�ʱ���ر��Ѿ��򿪵�ͼ
	bool m_bDisableAutoTrainNo; // ���ݾ�,�¼ƻ�����ѡ���Ƿ��ֹ�Զ��������
	bool m_bTrainNoAutoChgChoice; // �¼ƻ�ʱ�Ƿ����ѡ���Զ��������
	bool m_bEraseBeforeModifyTrain; // ȥ��������Ӱ��
	bool m_bNotMeetBySpeed; // true:�����ٶȽ��� false:�������ͽ���
	bool m_bDisableModifyPlanTm; // �Ƿ��ֹ�޸�ͼ��ʱ��
	CalSideTrainType mCalSideTrainType; // ��̨��ʹ��TDMS�ɵ��г�����
	bool m_bCheckSideFull;
	bool m_bEnableScadElecArm; // �Ƿ�����pscad
	bool m_bCheckTDMSSide; // �Ƿ���TDMS�ɵ��Ϸ���
	bool m_bAcceptTrainID; // �����ƻ��Ƿ���յ�ǰʱ��3Сʱ�ڳ��α仯
	bool m_bZeroGIDPasswayChange; // ̫ԭ�����,���г���·�仯ʱ,ȥ��ȫ��ID
	bool m_bFormatView; // �Ƿ���ʾΪ�ܹ�˾ͳһ����
	bool m_bIsFindTrainByViewRange; // ���Ͼ�,��α����ظ�����, false:�������ڰ�η�Χ, true:�ɼ���Χ
	bool m_bBatchAddTrainFindExistTrain; // ���Ͼ�,�����ӳ�ʱ true:����ظ����� false:������ظ�����
	bool m_bCheckRelationSideCrash; // ���ݾ֣��Ƿ��齻·�ɵ���ͻ
public:
	bool IsCheckTDMSSide() { return m_bCheckTDMSSide;}
	bool IsEnableScadaElecArm() { return m_bEnableScadElecArm;}
	bool IsCheckSideFull() { return m_bCheckSideFull;}
	bool IsTrainNoAutoChgEnable() { return m_bTrainNoAutoChgChoice;}
	bool IsNotMeetBySpeed() { return m_bNotMeetBySpeed;}
	bool IsDisableModifyPlanTm() { return m_bDisableModifyPlanTm;}
	bool IsEraseBeforeModifyTrain() { return m_bEraseBeforeModifyTrain;}
	bool IsNewSetPreBasicTechStop() {return m_bSetPreBasicTechStop;}
	int GetFindBasicTrainBeforeTime() { return m_nBasicPreHour*3600;}
	int GetFindBasicTrainAfterTime() { return m_nBasicBackHour*3600;}
	
    ///////////////////////////////////
	bool   IsLimitSectionCalToConid(int conid, int station, int nextstation);
	bool   IsNotCalFromConid(int conid, int station, int entry);    

    ///////////////////////////////////
	bool   IsAcceptImportNote() {return m_bAcceptImportNote;}
	bool   IsGiveChoiceWhenPathChange() {return m_bGiveChoicePathChange;}
	bool   IsSubaoShowSpace() {return m_bIsSubaoShowSpace;}
	bool   IsStplanSpaceRankByStation() {return m_bSetStplanSpackRankByStation; }
	bool   IsShowArriveTrainid() {return m_bShowArriveTrainid;}
	bool   IsQuebaoQueryHaveChinese() {return m_bQuebaoQueryHaveChinese;}
	bool   IsBasicStopSetNoTechStop() {return m_bBasicStopSetNoTechStop;}
	bool   IsShiftStopSetNoTechStop() {return m_bShiftStopSetNoTechStop;}
	bool   IsFreedrawStopSetNoTechStop() {return m_bFreedrawStopSetNoTechStop;}

	CONID  GetTrainRunFromConid(int src_conid, int sta, int exit, int entry);
	CONID  GetTrainRunToConid(int src_conid, int sta, int exit);

	long  GetDcmdTempleteNo(CString type, CString subType);
	CString GetTdmsTrainId() { return m_szTrainId; }
	CTGFunctionConfig();
	BOOL  InitData(CString strPath);
	int   GetNoifyCount() {return m_nAutoNotifyDefCount;}
	BOOL  GetNotifyDesc(int index,int *daytime,char *desc,int desclen);
	BOOL  IsFJKStation(int station) const;
	int   MatchNeibPlan(int source_section,int dest_section,int stationlist[],int stationcount,int& matched_rec_count, bool& bHavePreStation,int& nodeindex,int& passway,bool& bWholeMatch, bool& bAcceptTerminalTrain, int &neib_station, int &control_prev_abs);
	
	int   GetNeibFJKStations(int dest_section,int source_section,int stationlist[],int limit);
	BOOL  GetDefaultEntry(int station,StationDefaultEntry &default_entry) const;
	int   GetDefaultEntry(int station, BOOL down_direction,BOOL accept_in) const;
	BOOL  NeedToCheckTGOper(int train_type) const;
	BOOL  GoodsTrainHaveTechStop(int train_type) const;
	BOOL  PassengerTrainEndTechStop(int train_type) const;
	BOOL  PassengerTrainStartTechStop(int train_type) const;
	bool  IsEnableOPMSNodeIndex() const;

	bool  IsCheckStPlanWithBasicSchd() const;
	bool  IsNewNeibModeAddTrain() const;
	bool  IsCalBasicSchdExitEntry() const;
	bool  IsCalShiftSchdExitEntry() const;

	BOOL  IsEnableOPMSBasicSchdSide(int nSection) const;
	BOOL  IsEnableOPMSShiftSchdSide(int nSection) const;
	bool  IsGoodTrainUseOPMSSide(int train_type) const;
	bool  IsPessagerTrainNotUseOPMSSide(int section, int train_type);

	bool  IsBasicValidSideByBasicSchd()	const;
	bool  IsBasicValidSideByCalculate() const;

	BOOL  IsEnableTempAddTrain() const;
	
	BOOL  IsEnableSelPasswayByDirectTrain() const;
	BOOL  IsEnablePasstoStayModifyArivalTime() const;
	BOOL  IsEnableSendPlanNoticeDCmd() const;
	BOOL  IsDisEnableWSSaveDCmd() const;
	BOOL  IsEnablePrintNoteTrainStopOneHour() const;
	BOOL  IsEnableSelectTrainByLanzhou() const;
	BOOL  MatchNeibPlanByEntry(int source_section,int dest_section,int station,int exit,int *nodeinex,int *passway, int entry); // ����ֵ�ɹ�ʧ��
	bool IsSyNeighbourPlan(int conid);
	
	
	const NeibPlanSY* GetNeibPlanSY(int this_section, int neib_section, int id);
	const NeibConsolePlanSY*  GetNeibConsolePlan(int this_section,int neib_section);
	int   GetFJKStationPreList_SY(int this_section, int neib_section,int id, int station[], int limit, int& fjk_accept, int& accept_stop, int& accept_arrive_depart);

    // ���շ����г���̨�ƻ�����
	const NeibPlanReverse*  GetStationList_Reverse(int this_section, int neib_section, int id);
	const NeibConsolePlanReverse*  GetNeibPlan_Reverse(int this_section,int neib_section);
	
	void  LoadStplanWarnCfg(const CString pathname);
	bool  LoadNeibPlanReverseXML(const CString pathname);
	bool  LoadNeibPlanSYXML(const CString pathname);
	bool  LoadChangeTrainidXML(const CString pathname);
	bool  LoadTrainDepartCheckXML(const CString pathname);

	const ConsoleChangeId* GetConsoleChangeId(int con_id);

	BOOL  IsFreeAddTrain(int train_type) const;
	BOOL  IsStationBlockNotCheckSide() const;
	BOOL  IsEnableInputEngineId() const;
	BOOL  IsEnableLanzhouEnginePlot() const;
	BOOL  IsSendPlanTip(int con_id, int sta, int nxtsta) const;
	BOOL  IsFreeDrawSetBasicTrainPlanTime() const;
	BOOL  IsEnableUnknowTrainStPlanRunType() const;
	bool  IsCheckCtcModeStaCtrlStPlan() const;
	BOOL  IsCheckAllStationStPlan() const;
	BOOL  IsCheckTrainMeetByNoMeet() const;

	bool  IsSecretTrainToStation() { return m_bSecretTrainToSta;}
	bool  IsCheckPlanValid(int con_id);
	bool  IsEnableCheckPlatform() { return m_bEnableCheckPlatform;}
	
	BOOL IsDisenableSheetTab() const;
	BOOL IsDisenableSheetTabExit() const;
	
	bool IsStplanOnlySelectLirc() {return m_bStplanDefaultSelLirc;}
	bool IsStplanSelectAllEntity() {return m_bStplanSelectAllRecEntity;}
	bool IsRemoveTrainIDChineseChar() {return m_bRemoveChineseCharSetTrainType;}
	bool IsEngineChangeGiveOutTip() {return m_bEngineChangeGiveoutTip;}

	bool IsGetEngineFromPreConID() {return m_bGetEngineFromPreConID;}
	bool IsSendStplanUseName() {return m_bSendStplanUseName;}
	bool IsEnableDeleteFromTmisBlock() {return m_bEnableDeleteFromTmisBlock;}
	bool IsTrainPassToStopUseMainSide() {return m_bTrainPassToStopUseMainSide;}
	bool IsTrainIdUseArriveId() {return m_bTrainIdUseArriveId;}
	
	bool IsTaiYuanNeigbourPlan() {return m_bTaiYuanNeigbourPlan;}
	bool IsCheckRouteforCrhAndImp() {return m_bChangeRouteTip;}
	
	bool IsGoodTrainUseBasicSide() {return m_bGoodTrainUseBasicSide;}

	bool IsSTIQueryTrainBianzu() {return m_bSTIQueryBianzu; }
	int  GetAutoAdjustOperFlag() const;
	void SetAutoAdjustOperFlag(int va);

	bool IsEnableThirtyByPlan(const int con_id);
	bool IsFreedrawAddTrainShiftAndBasic() {return m_bAddTrainShiftAndBasic;}
	int  GetAutoAdjustStepFlag(int bit) const;
	void SetAutoAdjustStepFlag(int bit,int va);

	int  GetDepartDelayTimeOfBlockedABS() {return  m_nDepartDelayTimeOfBlockedABS;}
	bool IsChangeSideWhileStopPass(int train_type);

	bool SetPassengerTrainNoTechStopLockSide() const {return m_nSetPassengerTrainNoTechStopLockSide;}

	bool IsEnableLockWholeTrainSide() {return m_bEnableLockWholeTrainSide;}
	
	bool IsDeleteRepeatDeleteStplan() {return m_bDeleteRepeatDeleteStplan;}

	bool IsDieselLocomotiveDrawGreen(int con_id);
	bool IsNeibPlanPlaySound() {return m_bPlaySoundNeiPlan;}
	bool IsCrewPlaySound() {return m_bPlaySoundCrew;}

	bool IsEnableCzh() {return m_bEnableCzh;}
	bool IsEnableNewCzh() {return m_bEnableNewCzh;}
	int  GetAdjustAcceptRelationMode() {return m_nAdjustAcceptRelation; }
	int  GetNeibplanAcceptRelationMode() { return m_nNeibplanAcceptRelation; }
	bool IsChangeTrainID() {return m_bChangeTrainID;}
	bool IsEnableTDMS5_0() {return m_bEnableTDMS5;}
	bool IsGoodTrainNotTechStopNeedPwd() { return m_bGoodTrainNotTechStopNeedPwd;}
	bool IsUseStringKiloMeter() {return m_bUseStringKiloMeter;}
	bool IsNeibplanOnlyUseOPMSSchd() {return m_bIsNeibplanOnlyUseOPMSSchd;}
	bool IsShowMMPlanSkyLightButton() {return m_bShowMMPlanSkyLightButton;}
	bool IsShowMMPlanMaintainButton() {return m_bShowMMPlanMaintainButton;}
	
	bool IsNeibPlanAddTrainOnlyByShift() { return m_bNeibPlanTrainAddOnlyByShift;}
	bool IsNeibPlanPassengerTrainAddNotByBasic(){	return m_bNeibPlanPassengerTrainAddNotByBasic;}
	bool IsPassengerTrainEndSetNotTechStop(){	return m_bPassengerTrainEndSetNotTechStop;}
	bool IsPassengerTrainStartSetNotTechStop(){	return m_bPassengerTrainStartSetNotTechStop;}
	bool IsSelBackupSideByCfgQueue() {return m_bSelBackupSideByCfg;}
	bool IsShiftTrainSelFirstBackupSide() {return m_bShiftTrainSelFirstBackupSide;}
	bool IsEnableNeibPlanFreightSameToPassenger(){return m_bEnableNeibPlanFreightSameToPassenger;}
	
	bool IsSupportInterCityStPlan() { return mbSupportInterCityStPlan;}

	int SetElecTrain(int con_id, int station, int exit, int entry);
	bool IsSideNotChange(int con_id, int station, int side, int flag);

	bool GetTrainDepartCheckInfo(int con_id, int station, std::vector<int>& liststation); 

	bool IsNotThisSectionStation(int this_section, int station);
	bool IsThisSectionStation(int this_section, int station);
	UINT GetNotThisSectionStation(int this_section, int station[], int nMax);

	//�������ͼͨ���������Ӳ���ճ�վ���ã������ļ�no_limit_station.xml
	bool LoadNoLimitStitonXml(CString filename);
	bool IsNoLimitStation(int this_section, int station);
	UINT GetNoLimitStationList(int this_section, int station[], int nMax);
	bool HasNoLimitStation() { return m_bHasNoLimitStation; }

	bool ProhibitDeleteRecordWhileLimitModifyActual() { return m_bDisEnablDeleteRecordWhileLimitModifyActual;}
	bool IsClosePollingCommStatus() { return m_bClosePollingCommStatus;}

	bool IsCalExitEntryByTrainId() { return m_bCalExitEntryByTrainId;}

	int  GetTrainDirection(CString strTrainId);
	
	int  GetNeibPlanMode(int this_section, int neib_section);

	bool IsTrainSideUseBasicSide() { return m_bTrainSideUseBasicSide;}

	bool IsEnableRecvAllAdjustPlan() { return m_bEnableRecvAllAdjustPlan;}
	bool IsAlarmOnlyCheckCRHTrainSide() {return m_bIsAlarmOnlyCheckCRHTrainSide;}
	bool IsAlarmNotCheckSpackTrainMeetInfo() {return m_bIsAlarmNotCheckSpackTrainMeetInfo;}


	int GetCheckElecSideWhenPassToStopFlag() { return m_nCheckElecSideWhenPassToStop; }
	bool IsEnablePlotMainSide() {return m_bEnablePlotMainSide;}
	bool IsQuestionStplanSelState() {return m_bQuestionStationPlanSelState;}
	bool IsQuestionStplanSelPassengerSta() {return m_bQuestionStationPlanPassengerSta;}

	bool IsEnableLogTDMessage() {return m_bEnableLogTDMessage;}
	bool IsThisSectionHasTDMSLineStation(int section);
	bool IsStationUseTDMSLine(int section, int station1, int station2, int station1_nd, int station2_nd);

	bool IsEnableRecvAdjustPlanTime() { return m_bEnableRecvAdjustPlanTime;}	
	bool IsNeibPlanNotAddNewTrainIFSourceNotShift() const;
	bool IsNeibPlanNotAddNewTrain() const;
	bool IsModifyTrainActAndGTIDSame() { return m_bModifyTrainActAndGTIDSame;}
	bool IsAcceptTrainID() { return m_bAcceptTrainID;}
	// ̫ԭ�֣�����г���·�Ƿ�����ȫ��IDΪ0
	bool IsZeroGIDPasswayChange() { return m_bZeroGIDPasswayChange;}
	// �Ƿ���ʾΪ�ܹ�˾ͳһ����
	void SetFormatView(bool b){ m_bFormatView=b;}
	bool IsFormatView(){ return m_bFormatView;	}
	// ���Ͼ�,�ظ����β��ҷ�Χ
	bool IsFindTrainByViewRange() { return m_bIsFindTrainByViewRange;}
	bool IsBatchAddTrainFindExistTrain() { return m_bBatchAddTrainFindExistTrain;}
	// ���ݾ֣��Ƿ��齻·�ɵ���ͻ
	bool IsCheckRelationSideCrash() { return m_bCheckRelationSideCrash;}
	int GetStPlanReportEarlyTimeRange() { return m_nReportEarlyTime;}
	bool IsNewModeLoginRefresh() { return m_bNewModeLoginRefresh;}
	bool IsConsiderStplanSide() { return m_bConsiderStplanSide;}
	bool IsNeedCompareTrainExpand() { return m_bNeedCompareTrainExpand;}
	bool IsNeedWaitMarkEnd() { return m_bNeedWaitMarkEnd;}
	bool IsGpcSendReqOtherMsg() { return m_bGpcSendReqOtherMsg;}
	bool IsSendLogToLogsvr() { return m_bSendLogToLogsvr;}
	bool IsBasicDeletePassengerFreight() { return m_bBasicDeletePassengerFreight;}
	bool IsGenAllMarkByDeleteExist() { return m_bGenAllMarkByDeleteExist;}
	bool IsConsiderTwicePassSameStation() { return m_bConsiderTwicePassSameStation;}
	bool IsCompatibleOldStPlan() { return m_bCompatibleOldStPlan; }
	bool IsEnableModifyGID() {return m_bEnableModifyGID; }
	bool IsShiftCompareWithBasicAddTime() { return m_bCheckShiftTrainAddTime;}
	bool IsNotSelAdjustPlan() const { return m_NotSelAdjustPlan;}
	bool IsUsingWorkGoodMode() const { return m_bUsingWorkGoodMode;}
	bool IsBasicShowCompareDlg() const { return m_bBasicShowCompareDlg;}
	bool IsShiftShowCompareDlg() const { return m_bShiftShowCompareDlg;}
	bool IsEnableSelGenBasicTrain() { return m_bEnableSelGenBasicTrain;} 
	bool checkConfig();
	BOOL LoadTgdef();
	bool VerifyStationWithIndex(int section, int station, int index);
	bool IsCheckNeibTrains(int section);
	void GetNeibPlanConfig(int neib_section,int this_section, vector<NeibPlanStruct>& neibPlanConfig);

	bool LoadCompareStationList(int section, CheckStationsOfSection& compare_stations);
	bool IsDisableAutoTrainNo() { return m_bDisableAutoTrainNo;}
	void SetDisableAutoTrainNo(bool b) { m_bDisableAutoTrainNo=b;}
	int  GetThisSectionStation(int con_id, int station_list[128], int total); // ��ȡ���������г�վ

	bool GetSectionConfig(int section, SectionDefine& node_section);
	bool LoadStplanNoWarningStation(int section, StationStplanNotWarning& stations);
	void LoadCalSideTrainType(int section);
};

CTGFunctionConfig *GetTGFunctionConfig();
// ԭ��������icommtask.h 
void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...);
void GpcLogSysMessage(short process_id, const char *fmt, ...);
void GpcLogDebugMessage(short process_id, const char *fmt, ...);
#endif
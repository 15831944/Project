#pragma once
#include "defines.h"

//////// �������ݽ��������ݽṹ ////////////////
//�ɵ��泵��ṹ
struct _tagSideStoreCarriage 
{
	ushort usCarriageNum;        //������Ŀ
	ushort usCarriageType;       //��������
	ushort usCarriageExplain;    //����ȥ��
	char lpszCarriageApply[64];  //�Ƿ�Ӧ��
	char lpszCarriageRemark[64];
	_tagSideStoreCarriage()
	{
		memset(this, 0, sizeof(_tagSideStoreCarriage));
	}
};

//�ɵ��泵�����
struct _tagSideStoredTrain 
{
	ushort usSideNo;            //�ɵ�����
	char pszSideName[20];
	ushort usRecNum;            //�ùɵ�����ļ�¼����һ����¼�����ж������
	ushort CarriageCounts;      //�ùɵ������еĳ�������
	struct _tagSideStoreCarriage rec[64];	//��ϸ�Ĵ泵���¼
	_tagSideStoredTrain()
	{
		usSideNo=0;
		usRecNum=0;
		CarriageCounts=0;
		memset(pszSideName, 0, sizeof(pszSideName));
	}
};
struct _tagStationStoreTrain
{
    unsigned long EditTime;	//���༭ʱ��
	ushort usStationNo;	//̨վ����
	ushort usSideNum;	//�ɵ���Ŀ
	struct _tagSideStoredTrain sideinfo[64];	//�ɵ���ϸ���
	_tagStationStoreTrain()
	{
		EditTime=0;
		usStationNo=0;
		usSideNum=0;
	}
};
	///////////// Oper ///////////////////   
typedef struct _tagCADDATA_PASS_WAY_INFO{
	 short Pass_way_id;          //��·��
	 char  Pass_way_name[40];    //��·����
     short Console_id;           //����̨����
	 short Begin_sta;            //��ʼ��վ
	 short End_sta;              //��ֹ��վ
}PASS_WAY_INFO;

//          2 : վ��Ϣ 
typedef struct _pass_way_sta{
	 short Pass_way_id;          //��·����
	 short Sta_lst_idx;          //��·վ���
	 short Entry;                //���
	 short Station;              //վ��
	 short Exit;                 //����
}PASS_WAY_STA;

typedef struct _pass_way_utility{
	 char PREFIX[10];                  //����ǰ׺
	 unsigned long START_TRAIN_NUM;    //��ʼ����
	 unsigned long END_TRAIN_NUM;      //��ֹ����
	 short Station;                    //վ��
	 short Entry;                       //���
	 short Exit;                        //����
     short DIRECTIONS;                  //���η���
	 short CONSOLE_ID;                  //����̨����
	 short PASS_WAY_ID;                //��·��
     BYTE  ACPT_FLAG;                   //���룬ʼ��
	 BYTE  GVT_FLAG;                    //�յ㣬����
}PASS_WAY_UTILITY;

//�г���ҵ�ƻ�
struct _tagTrainLineupSchedule 
{
    unsigned long input_time;    //  ����ʱ��
	unsigned long train_idx;     //  �г�Ψһ��ʶ

	char lpszTrainID[10];        //  ���κ�
	byte usWorkType;             //  ��ҵ����
	byte usSideNo;               //  ��ҵ�ɵ�
	char lpszWorkContent[128];   //  ��ҵ����
};

struct _tagStationTrainLineupSchedule 
{
	ushort usStationNo;           //��վ����
	ushort usRecNum;              //
	struct _tagTrainLineupSchedule rec;
};

#define CADCONST_MAX_TRAIN_ID_NUM_IN_SINGLE_TRAIN   8
#define CADCONST_MAX_ENGINEER_NUM_IN_SINGLE_TRAIN   8
#define CADCONST_MAX_STATION_TEXT_NUM_IN_SINGLE_TRAIN  32
#define CADCONST_MAX_ABS_TEXT_NUM_IN_SINGLE_TRAIN      32
#define CADCONST_MAX_LOAD_NUM_IN_SINGLE_TRAIN           8

struct  TRAIN_INFO {
	ULONG   train_index;          // index of train
	long sborder_plan_date;  /* converted from mm:dd:yy */
	long sborder_plan_time;  /* converted from hh:mm:ss */
	long sborder_actual_date;  /* converted from mm:dd:yy */
	long sborder_actual_time;  /* converted from hh:mm:ss */

	long eborder_plan_date;  /* converted from mm:dd:yy */
	long eborder_plan_time;  /* converted from hh:mm:ss */
	long eborder_actual_date;  /* converted from mm:dd:yy */
	long eborder_actual_time;  /* converted from hh:mm:ss */
	//	BYTE     spare[2];
	ushort sborder_sta;
	ushort eborder_sta;
	BYTE     start_sta_name[12]; // used while start_sta == 0
	BYTE     end_sta_name[12];   // used while end_sta == 0
    ULONG   basic_train_index;    // index of train in basic_schedule   -1 added
	   BYTE     direction;    // bit 0 - up   bit 1- down   
	   BYTE     train_id_count;
	   char     train_id[CADCONST_MAX_TRAIN_ID_NUM_IN_SINGLE_TRAIN][20];  
	   char     original_train_id[CADCONST_MAX_TRAIN_ID_NUM_IN_SINGLE_TRAIN][20];//add by cf for hat train
	   ushort     begin_sta[CADCONST_MAX_TRAIN_ID_NUM_IN_SINGLE_TRAIN];
	   ushort     end_sta[CADCONST_MAX_TRAIN_ID_NUM_IN_SINGLE_TRAIN];

	   BYTE     type;         //  detailed train type
	   BYTE     run_type;     //  main type : EXPRESS_P_TRAIN, FAST_P_TRAIN,
	  			 //  NORMAL_P_TRAIN, PACK_F_TRAIN, 
							 //  FIVE_F_TRAIN,  NORMAL_F_TRAIN,
							 //  bit  7 - 0 passenger   1 - freight train
 	   USHORT   length;
	   USHORT   train_weight;

	   BYTE     priority;
 
	// Locomotive Status:
	   BYTE     engine_count;
	   char     engine_id[CADCONST_MAX_ENGINEER_NUM_IN_SINGLE_TRAIN][20];
	   BYTE     engine_type[CADCONST_MAX_ENGINEER_NUM_IN_SINGLE_TRAIN];
	   USHORT   attach_station[CADCONST_MAX_ENGINEER_NUM_IN_SINGLE_TRAIN];
	   USHORT   unattach_station[CADCONST_MAX_ENGINEER_NUM_IN_SINGLE_TRAIN];

	   BYTE     engine_utility;                  // 0: normal
												 // 1: limited_time
												 // 2: no locomotive
	   BYTE     side_utility;       // bit0:   1 :   need high platform

	// Important Train:
	   BYTE    space_rank;       // 0: no out of space, else: rank of space
	   BYTE    length_rank;      // 0: no out of length, else: rank of length
	   BYTE    super_rank;       // 0: not super_train, else: rank of super_train
	   BYTE    military_rank;    // 0: not military_train, else: rank of military_train
	   BYTE    speed_limit;      // 0: no speed_limit, else: the max speed

	// Running Conditions:
	   BYTE    no_meet_flag;     // 0: can meet other train, else:
					 //     bit 0: can not meet EXPRESS_P_TRAIN
					 //     bit 1: can not meet FAST_P_TRAIN
					 //     bit 2: can not meet NORMAL_P_TRAIN
					 //     bit 3: can not meet PACK_F_TRAIN
					 //     bit 4: can not meet FIVE_F_TRAIN
					 //     bit 5: can not meet NORMAL_F_TRAIN
	   USHORT  super_interval_ltime;   // 0: use default interval, else: the left time (min)
	   USHORT  status;   //bit 0:  quebao flag
                                       //bit 1:  secret train


	// adjust info:
	   USHORT  adjust_begin_sta;
	   USHORT  adjust_end_sta;            // keep it for future use

	   BYTE    adjust_flag;               // bit 0: 0 - join adjust   1 - not


	   BYTE    train_source;
	   // bit 03-00:  define train infomation's source(0-7)
	   //    0 - basic schedule                  1 - addition train
	   //    2 - auto report true train create   3 - auto report fake train create
	   //    4 - dispatcher manual (free draw)   5 - pathway
	   //    6 - system probe                    7 - informal

	   BYTE    spare[3];

	   ULONG   ip_index;

	// bit 1: 0 - exist in shift schedule
       BYTE  station_text_count;
	   ushort station_text_id[CADCONST_MAX_STATION_TEXT_NUM_IN_SINGLE_TRAIN];
	   char  station_text[CADCONST_MAX_STATION_TEXT_NUM_IN_SINGLE_TRAIN][32];
     
	   BYTE  abs_text_count;
	   ushort abs_text_id[CADCONST_MAX_ABS_TEXT_NUM_IN_SINGLE_TRAIN];
	   char  abs_text[CADCONST_MAX_ABS_TEXT_NUM_IN_SINGLE_TRAIN][32];
			   //<total_len>(2)<type>(1)<sta_no>(2)<len>(2)<content>...
 	   BYTE  load_text_count;
	   ushort load_text_id[CADCONST_MAX_LOAD_NUM_IN_SINGLE_TRAIN];
	   char  load_text[CADCONST_MAX_LOAD_NUM_IN_SINGLE_TRAIN][32];
      
	   //for limite width
       ushort   width_limit;
       ushort	 over_length_flag;

     //end 
};



struct  TRAINRECORD
{ 
	BYTE     rec_index;    //only for gpc 
	ushort   station;       /* station number */
	ushort   entry;         /* entry window */
	ushort   exit;          /* exit window */

	
    ushort     arrive_side;         //ʵ�ʵ���ɵ�
    ushort     plan_arrive_side;    //�ƻ�����ɵ�
    ushort     depart_side;        //ʵ�ʳ����ɵ�
	ushort     plan_depart_side;   //�ƻ������ɵ�

	ushort     status;     //Ԥ��

	ushort   min_stop_time;

	long arrival_date;  /* converted from mm:dd:yy */
	long arrival_time;  /* converted from hh:mm:ss */
	long depart_date;   /* converted from mm:dd:yy */
	long depart_time;   /* converted from hh:mm:ss */


	ushort   additional_abs_time;

	//add 2002/4/3
	long plan_arrival_date;  /* converted from mm:dd:yy */
	long plan_arrival_time;  /* converted from hh:mm:ss */
	long plan_depart_date;   /* converted from mm:dd:yy */
	long plan_depart_time;   /* converted from hh:mm:ss */

	BYTE     stop_reason;
	BYTE     late_reason;

	SHORT   early_depart_time;   // 0: no early depart limit, 
                                      // else: the time span could be taken early
	BYTE     run_fast_slow_time;  // bit 7:   0   run too fast
             				      //          1   run too slow
                                  // bit 6-0: time (minutes)
    BYTE     stat_flag;            // bit 0 :   statisticalize arrive 
                                   // bit 1:   statisticalize depart
	BYTE     speed_limit;          // 0: no speed_limit, else: the max speed
    BYTE     stop_condition;
              // bit 2:   cannot stop without plan
              // bit 1 :   depart time lock
              // bit 0 :   arrive time lock


	USHORT     adjust_status;       
				  // bit 0:  0 : arrival time is plan
				  //         1 : arrival time is actual
				  // bit 1:  0 : depart time is plan
				  //         1 : depart time is actual

                  // bit 3-2:
				  //  00:  normal, 01:  with conflict,
				  //  10:  error,  11:  cannot adjust
				  // bit 4:    1 - arrive is not controled by this console
                  // bit 5:    1 - depart is not controled by this console
    BYTE       side_utility;        
                  // bit 3 :  changed by abs slowmove
                  // bit 2 :  changed by side slowmove 
				  // bit 1 :  cannot stop at other side
				  // bit 0 :  cannot pass by non-main line
	
    BYTE     stop_abs_position;//positon  
 	USHORT   stop_abs_s_time;// special_time1 
	USHORT   stop_abs_e_time;//special_time2
  
    ULONG    special_train_index;
	ULONG    data_source;
	ULONG       flag;   
	ULONG     spare;
  
    BYTE local_stat_arrival;
	BYTE local_stat_depart; 
};

#define IsFlagGiveout(flag)         ((flag & 0x0007) ==  0x0001)
#define IsFlagTerminal(flag)        ((flag & 0x0007) ==  0x0002 || (flag & 0x0007) ==  0x0003)
#define IsFlagEnd(flag)             ((flag & 0x0007) ==  0x0003)
#define IsFlagKeep(flag)            ((flag & 0x0007) ==  0x0004)
#define IsFlagRelay(flag)           ((flag & 0x0007) ==  0x0005)
#define IsFlagUnite(flag)           ((flag & 0x0007) ==  0x0006)
#define IsFlagDivide(flag)          ((flag & 0x0007) ==  0x0007)
#define IsFlagAccept(flag)          ((flag & 0x0018) ==  0x0008)
#define IsFlagStart(flag)           ((flag & 0x0018) ==  0x0010)
#define IsFlagReverse(flag)         (flag & 0x0020)
#define IsFlagCorner(flag)          (flag & 0x0040)
#define IsFlagStopSignal(flag)      (flag & 0x0080)
#define IsFlagStopAbs(flag)         (flag & 0x0100)
#define IsFlagAnLocReturn(flag)     (flag & 0x0200)

//�ӳ�վ���������յ�
#define IsFlagAbsTerminal(flag)             ((flag & 0x00010000) ==  0x00010000)
#define SetFlagAbsTerminal(flag)            (flag &= 0xFFF0FFFF,flag|= 0x00010000)
//�����俪��վ
//#define IsFlagStopAbs2Station(flag)          ((flag & 0x00020000) ==  0x00020000)
//#define SetFlagStopAbs2Station(flag)         (flag &= 0xFFF2FFFF,flag|= 0x00020000)
#define IsFlagAbsStart(flag)        (flag & 0x00020000)
 
#define IsFlagPass(rec)            (rec.arrival_time == rec.depart_time) 
#define IsFlagStop(rec)            (rec.arrival_time != rec.depart_time)

#define IsFlagRescue(flag)            (flag & 0x0800)//11 bit 
#define IsFlagRescued(flag)            (flag & 0x1000)//12 bit

#define IsFlagUniteTrain(flag)      (flag & 0x4000)//14 bit
#define IsFlagTrainUnited(flag)     (flag & 0x8000)//15 bit

#define SetFlagPass(flag)           (flag &= 0xFFFFFFFF)
#define SetFlagStop(flag)           (flag &= 0xFFFFFFFF)

#define SetFlagRescue(flag)           (flag |= 0x0800)
#define SetFlagRescued(flag)           (flag |= 0x1000)

#define SetFlagUniteTrain(flag)      (flag |= 0x4000)//14 bit
#define SetFlagTrainUnited(flag)     (flag |= 0x8000)//15 bit

#define SetGiveFlagClear(flag)		 (flag &= 0xFFFFFFF0)
#define SetFlagGiveout(flag)         (flag &= 0xFFFFFFF8,flag|= 0x0001)
#define SetFlagTerminal(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0002)
#define SetFlagEnd(flag)             (flag &= 0xFFFFFFF8,flag|= 0x0003)
#define SetFlagKeep(flag)            (flag &= 0xFFFFFFF8,flag|= 0x0004)
#define SetFlagRelay(flag)           (flag &= 0xFFFFFFF8,flag|= 0x0005)
#define SetFlagUnite(flag)           (flag &= 0xFFFFFFF8,flag|= 0x0006)
#define SetFlagDivide(flag)          (flag &= 0xFFFFFFF8,flag|= 0x0007)
#define SetFlagAccept(flag)          (flag &= 0xFFFFFFE7,flag|= 0x0008)
#define SetFlagStart(flag)           (flag &= 0xFFFFFFF0,flag|= 0x0010)
#define SetFlagReverse(flag)         (flag |= 0x0020)
#define SetFlagCorner(flag)          (flag |= 0x0040)
#define SetFlagStopSignal(flag)      (flag |= 0x0080)
#define SetFlagStopAbs(flag)         (flag |= 0x0100)
#define SetFlagAnLocReturn(flag)     (flag |= 0x0200)
#define SetFlagPassNonMainSide(flag) (flag |= 0x0400)



#define  IsDieselEngineExt(enginetype)		(((enginetype) & 0x0F) == 0x00) 
#define  IsElecEngineExt(enginetype)		(((enginetype) & 0x0F) == 0x01) 

#define  SetDieselEngineExt(enginetype)		((enginetype) &= 0xF0) 
#define  SetElecEngineExt(enginetype)		((enginetype) &= 0xF0, (enginetype)|= 0x01) 

#define  IsTractionEngineExt(enginetype)	    (((enginetype) & 0x30) == 0x00) 
#define  IsAttachEngineExt(enginetype)		    (((enginetype) & 0x30) == 0x10) 

#define  SetTractionEngineExt(enginetype)	((enginetype) &= 0xCF) 
#define  SetAttachEngineExt(enginetype)		((enginetype) &= 0xCF, (enginetype)|= 0x10) 

struct _tagMarkItem{
	ULONG     mark_index;
  	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
	BYTE      type;           
	BYTE      flag;  
	USHORT    line;
	USHORT    station;
	USHORT    index;
	USHORT    position;       
	USHORT    count;
	USHORT    ele_number[64];
 	char     text[256];
	//for abs
	USHORT s_meter;
	USHORT e_meter;
	USHORT e_kilo;
	USHORT slow_time;
	//end
	bool  bFlag ; // reserved for judge GRAPH_MARK ;
};


typedef struct  _tagCADDATA_ABSTIME{
	int    absnum;
	ushort index;
	ushort station_no1;
	ushort station_no2;
    byte exit;
	byte entry;
	byte direction;	
	byte train_type;
	ushort stop_time;
	ushort old_stop_time;
	ushort start_time;
	ushort old_start_time;
	ushort run_time;
	ushort old_run_time;
}ABSTIME;


#define  CADCONST_MAX_TRACT_ENGINE_COUNT  32
#define  CADCONST_MAX_ATTACH_ENGINE_COUNTS 32


typedef struct _tagCADDATA_TRAIN_SUBAO{
      unsigned long train_idx;            //train_index
      unsigned long input_time;             //����ʱ��idx       
      unsigned long shift_id;             //���
           
     //byte Train_direction;     ////�г���������
	 char Train_id[20];          //����
     char Train_describe[50];    //����
     char Orig[20];              //��Դ
	 char Dest[20];              //Ŀ�ĵ�

	 short engine_count;    // �������� , new added
	 char  Engine_type[8][20];       //��������    //ÿ��������������,ԭ������Ϊ char  Engine_type[20]
     char  Engine_id[8][20];        //������
	 

	 char Driver_name[20];       //˾��
	 char Train_leader_name[20]; //����
	 char Cover[10];             //��
	 char Tail_no[10];           //β��

	 byte Space_rank;            //���޼���
	 byte Length_rank;           //��������
	 byte Super_rank;            //ר�˼���
	 byte Military_rank;         //���˼���
	 
	 byte Speed_limit;           //����
	 byte Con_id;                //̨��
	 byte E_engine;              //������־
	 byte Print_flag;            //�Ƿ��ӡ

	 short K_count;              //�ͳ�����
     short Load_count;           //�س�����

     short PK_count;             //�񳵣��գ�����
	 short PZ_count;             //�񳵣��أ�����

	 short CK_count;             //�������գ�
	 short CZ_count;             //�������أ�

	 short PIK_count;            //ƽ�����գ�
	 short PIZ_count;            //ƽ�����أ�

     short GK_count;             //�ޣ��գ�
	 short GZ_count;             //�� (�أ�

	 short O_count;              //����
	 short S_count;              //�س�

	 short Total_count;          //�ϼ�
     short Van_count;            //���    $

	 short Cal_len;              //�ǳ�
	 long Total_weight;         //ȫ��

	 short Subao_type;          //��������
	 short Speed;               //�ٶ�
 
     short   Noutil_cnt;         //���ó���    @
     short   report_station;     //�ϱ���վ

	 short	tract_engine_count;             //ǣ�� ��������
     byte	tract_engine_type[CADCONST_MAX_TRACT_ENGINE_COUNT];           //ǣ���������� $
	 byte	tract_engine_id[CADCONST_MAX_TRACT_ENGINE_COUNT][20];         //ǣ�������� $
	 short	attach_engine_count;          // ���һ�������
	 byte	attach_engine_type[CADCONST_MAX_ATTACH_ENGINE_COUNTS];           //���һ������� $
	 byte	attach_engine_id[CADCONST_MAX_ATTACH_ENGINE_COUNTS][20];        //���һ����� $

	 unsigned long	flag;                                 //Ԥ����־
	 byte	note[60];                    //Ԥ������  

	_tagCADDATA_TRAIN_SUBAO()
	{
		memset(this, 0, sizeof(_tagCADDATA_TRAIN_SUBAO));
	}
}TRAIN_SUBAO_NEW;


struct _tagStationPlanImportant {
	
	time_t   send_time ;//
    time_t   recv_time ;//
	time_t   edit_time ;//

	long usSendDate;
	long usSendTime;
	ushort bSignInFlag;
	ushort bReceiveFlag;

	char strSignName[16];
	char strRecvName[16];
};

#define CADCONST_STAGE_PER_WORK  3


typedef Pstruct _tagStationPlan_Stage{
	int nStation;// entry no
	struct _tagStationPlanImportant m_StageInfo[CADCONST_STAGE_PER_WORK];
}STATIONPLANINFO_STAGE;

typedef struct _tagStationPlanInfo_Important{
    int nStationNum;
	CString text_content[CADCONST_STAGE_PER_WORK];
	ushort  start_st_weather[CADCONST_STAGE_PER_WORK];
	ushort  end_st_weather[CADCONST_STAGE_PER_WORK];
	CString text_commander[CADCONST_STAGE_PER_WORK];
	CString text_command_time[CADCONST_STAGE_PER_WORK];
	CString text_send_time[CADCONST_STAGE_PER_WORK];
	STATIONPLANINFO_STAGE m_StationPlan[MAX_STATIONS_PER_SECTION];
}STATIONPLANINFO_IMPORTANT;

//the folloing is for �ֽ�ڽ��ӳ�ͳ��
typedef struct train_type_info
{
	char  pre_text[10];
	ULONG start_train_num;
	ULONG end_train_num;
 
	byte type;
	byte run_type;	
	byte priority;
}TRAIN_TYPE_INFO;
	 

#define CAR_TYPE_LEN 4
  

typedef Pstruct _train_asm_format_ { // 156
      ULONG   asm_id;                     // ����ID��14��
      ULONG   asm_plan_time;              // �ƻ�����ʱ�䣨14��
      ULONG   asm_actl_time;              // ʵ�ʿ���ʱ�䣨14��

      BYTE    asm_plan_train_id[10];      // �ƻ����Σ�8��
      BYTE    asm_actl_train_id[10];      // ʵ�ʳ��Σ�8��

      USHORT  asm_dept_sta_id;            // ʼ��վ��
      BYTE    asm_dept_sta_name[14];      // ʼ��վ��12��
      BYTE    asm_dept_sta_code[4];       // ʼ��վ���루3��

      USHORT  asm_arrv_sta_id;            // �յ�վ��
      BYTE    asm_arrv_sta_name[14];      // �յ�վ��12��
      BYTE    asm_arrv_sta_code[4];       // �յ�վ���루3��

      USHORT  asm_reporter_id;            // ����վ��
      BYTE    asm_reporter_name[14];      // ����վ��12��
      BYTE    asm_reporter_code[4];       // ����վ���루3��

      USHORT  asm_sg_sta_id;              // ˦��վ��
      BYTE    asm_sg_sta_name[14];        // ˦��վ��12��
      BYTE    asm_sg_sta_code[4];         // ˦��վ���루3��

      ULONG   asm_send_time;              // �´�ʱ�䣨14��

      USHORT  asm_toal_cnt;               // ������3��
      USHORT  asm_cal_len;                // ������5��

      USHORT  asm_weight;                 // ������5��
      USHORT  asm_self_weight;            // ���أ�5��

      USHORT  asm_van_cnt;                // �����4��
      USHORT  asm_complete_sta;           // �ϱ���վ

      BYTE    asm_reporter[10];           // �����ˣ�10��
      BYTE    asm_assigner[10];           // ǩ���ˣ�10��

      BYTE    asm_ht_flag;                // ��β��־��1��
      BYTE    asm_spare0;
      USHORT  asm_load_weight;            // ����

      USHORT  asm_s_car_cnt;              // ˦����
      USHORT  asm_g_car_cnt;              // �ҳ���
      } TrainAsmFormat;
 

typedef Pstruct asm_car_format {      // 296
      USHORT car_pos;                 //     ��ţ�3���ó������г��е����򣬴�1��ʼ
      USHORT car_spare0;               //     

      BYTE   car_type[CAR_TYPE_LEN];  //     ���֣�4��

      BYTE   car_no[10];    //     ���ţ�8��
      USHORT car_weight;              //     ������4��

      USHORT car_dept_sta_id;         //     ��վվ��
      BYTE   car_dept_sta_name[14];   //     ��վ���ƣ�12��
      BYTE   car_dept_sta_code[4];    //     ��վ���루3��

      USHORT car_arrv_sta_id;         //     ��վվ��
      BYTE   car_arrv_sta_name[14];   //     ��վ���ƣ�12��
      BYTE   car_arrv_sta_code[4];    //     ��վ���루3��

      BYTE   car_bureau_name[4];      //     ���־�����2��
      BYTE   car_bureau_code[4];      //     �־ִ��루3��

      BYTE   car_freight[20];         //     Ʒ����20��
      BYTE   car_recver[100];         //     �ջ��ˣ�100��
      BYTE   car_note[100];           //     ���£�100��

      USHORT car_cal_len;             //     ������4��
      USHORT car_van_cnt;             //     �������4��

      BYTE   car_sg_flag;             //     ˦�ұ�־��1��
      BYTE   car_spare1;
      USHORT car_sg_sta;              //     ˦��վ��
      } AsmCarFormat;

typedef Pstruct accident_info{
     ushort  index;
	 char  subject[40];
	 char  detail[2400];

}ACCIDENT_INFO;

/*���±�*/

typedef Pstruct _notebook{
	 short ddt_id;               //����̨id Section_Num
	 unsigned long  shift_id;    //���Ӱ�id

     short note_len;             //note ����
	 //modified by qianguorong 2003-08-11
	 char note[2500];           //note ����
	 //modified by qianguorong 2003-08-11 ends here
     
     short note_len1;             //note1 ����
	 char   note1[800];            //note1 ����
	 char  command_info1[20];
     char  send_time1[10];
	 short start_wea1;       //      ����
	 short end_wea1;         //      ����

     short note_len2;             //note2 ����
	 char  note2[800];            //note2 ����
	 char  command_info2[20];
     char  send_time2[10];
	 short start_wea2;       //      ����
	 short end_wea2;         //      ����

     short note_len3;             //note3 ����
	 char  note3[800];            //note3 ����
	 char  command_info3[20];
     char  send_time3[10];
	 short start_wea3;       //      ����
	 short end_wea3;         //      ����

}NOTEBOOK;

//////////////////////// ������Ҫȷ��
Pstruct _tagSideInform {
	ushort usSideNo;
	ushort usStatus;
	ushort usRemarkLen;
	ushort usChangeFlag;
	char Remark[256];
};

typedef Pstruct _tagSideInform	SIDEINFORM;
Pstruct _tagWeatherInform {
	ushort usStationNo;
	short usHighTemp;
	short usLowTemp;
	ushort usCondition;
	ushort usWindDirect;
	ushort usWindPower;
	ushort usChangeFlag;
	ushort usSpared;
};

typedef Pstruct  _tagWeatherInform  WEATHERINFORM;

typedef Pstruct interval_time{
	ushort index;
	ushort station_no;
    byte   IntervalTYpe;
	char   type_str[20];
	ushort IntervalTime;
	ushort old_IntervalTime;
}INTERVALTIME;

Pstruct _tagAddTrainInfo {
	ULONG train_index;
	BYTE trainid[12];
	ushort rec_num;
	ushort usSpared;
	Pstruct TRAINRECORD rec[1];
};

typedef Pstruct _tagAddTrainInfo ADDTRAININFO;


typedef Pstruct _tagLongStopTrainItem{
    char train_id[10];
	BYTE flag;
	long train_index;
 	long usStartDate;
	long usEndDate;
}LONG_STOP_TRAIN_ITEM;

Pstruct StationEntryAbsnum{
	ushort entry_num;
	ushort abs_num;
	ushort dir;
	ushort usSpared;
};

Pstruct SpecialStation{
	ushort station;
	ushort entry_count;
	Pstruct StationEntryAbsnum entry[16];
};


Pstruct TRAINRECORD_LONG { // 16 bytes altegether
	ushort   station;       /* station number */
	ushort   spare;
	ushort   min_stop_time;
	ushort   stop_reason;
	long     arrival_time;  /* arrival time */
	long     depart_time;   /* depart time */
	ushort   entry;         /* entry window */
	ushort   exit;          /* exit window */
	ushort   primaryside;          /* selected side */
	ushort   flag;          /* train status (pass,stop,terminal) */
};

Pstruct _tagTrainMarshal {
	char trainid[12];
	char engine_number[12];
	char driver_number[12];
	char driver_name[12];
	ushort usEngineType;
	ushort usTrainLength;
	ushort usCarriageCount;
	ushort usTotalWeight;
	ushort usIsCoveredTrain;
	ushort usCalculatedLength;
};

typedef Pstruct _tagTrainMarshal TRAIN_MARSHAL;

Pstruct TRAIN_HISTORY {
	char   train_id[12];
	ushort   station_number;
	ushort   a_d_flag;
	long date;
	long time;
	ushort   entry_exit;
	ushort   side;
	ushort   late_reason;
};

Pstruct TRAIN_HISTORY_LONG {
	char   train_id[12];
	ushort   station_number;
	ushort   a_d_flag;
	long   time;
	ushort   entry_exit;
	ushort   side;
	ushort   late_reason;
};

Pstruct TrainDelay{
	char    train_id[12];
	ushort  station;
	ushort  arrive_depart;
	ushort  delay_time;
	ushort usSpared;
};

Pstruct TrainDelete{
	char    train_id[12];
};

Pstruct TrainChange{
	char    old_train_id[12];
	char    new_train_id[12];
};

Pstruct TrainStartFromMid{
	char    train_id[12];
	ushort  station;
	ushort usSpared;
};

Pstruct TrainStopInMid{
	char    train_id[12];
	ushort  station;
	ushort usSpared;
};

Pstruct TrainAdd{
	ushort  abs_number;
	ushort  direction;
	ushort  speed;
	ushort  start_time;
	ushort  end_time;
	ushort usSpared;
};

Pstruct TrainLong{
	char    train_id[12];
	ushort  type;
	ushort  priority;
	ushort  length;
	ushort usSpared;
};

Pstruct TrainOverLimit{
	char    train_id[12];
	ushort  type;
	ushort  priority;
	ushort  level;
	ushort usSpared;
};

Pstruct TrainChangePriority{
	char    train_id[12];
	ushort  priority;
	ushort usSpared;
};

Pstruct SideSpecific{
	char    train_id[12];
	ushort  station;
	ushort  side;
};
//*** lyc  2001/9/14 ****** 
Pstruct SideSlow{
	ushort  station;
	ushort  side_count;
	ushort  side[30];
	ushort  limit_speed;
	ushort  limit_distance;
	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
 	ushort usSpared;
	ushort plan_type;
	char  reason[20];
};
  
Pstruct SideInhibit{
	ushort  station;
	ushort  side_count;
	ushort  side[30];
	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
    ushort type;
 	ushort usSpared;
	ushort plan_type;
	char  reason[20];
};

Pstruct AbsSlow{
	ushort  abs_number;
	ushort  direction;
	ushort  limit_speed;
	ushort  limit_distance;
	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
	ushort usSpared;
	ushort plan_type;
	char  reason[20];
};

Pstruct AbsInhibit{
	ushort  abs_number;
	ushort  direction;
	ushort  flag;// is able to reverse run flag;
	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
	ushort type;
	ushort  plan_type;
	ushort  abs_type;
	char  reason[20];
};

Pstruct AbsTask{
	ushort  abs_number;
	ushort  direction;
	char    train1[10];
	char    train2[10];
 	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
 	ushort plan_type;
	char reason[20];
};
Pstruct _tagSideInhibitInfo {
	int count;
	Pstruct SideInhibit rec[1];
};

Pstruct _tagAbsInhibitInfo {
	int count;
	Pstruct AbsInhibit rec[1];
};

Pstruct _tagAbsSlowInfo {
	int count;
	Pstruct AbsSlow rec[1];
};

Pstruct _tagTrainAttributeInfo {
	int count;
	Pstruct TRAIN_INFO rec[1];
};

Pstruct _tagTrainMarshalInfo {
	int count;
	TRAIN_MARSHAL	   rec[1];
} ;

Pstruct _tagWeatherInfo {
	int count;
	WEATHERINFORM rec[1];
};

Pstruct _tagSelectConflict {
	int nSelectType;
	char abFirstTrain[32];
	char abSecondTrain[32];
};

Pstruct SideInform {
	ushort usStationNo;
	ushort usSideNo;
	ushort usStatus;
	ushort usRemarkLen;
	ushort usChangeFlag;
	ushort usSpared;
	char Remark[256];
};

Pstruct _tagStationInfo {
	int count;
	Pstruct SideInform rec[1];
};

Pstruct _tagTrainHistory {
	int count;
	Pstruct TRAIN_HISTORY rec[1];
};



Pstruct TextNode{
	ushort  top_pos;
	ushort  bottom_pos;
  	long  start_date;
	long  start_time;
	long  end_date;
	long  end_time;
 	ushort plan_type;
	byte  text_type;
	char text[20];
};
//����λ���õ��ĺ�
//ʹ��line�ĸ�λ�ֽ��еĵͶ�λ����������λ��
//00�Ϸ�  01Ĭ���������м�  10�·�
//����վ�ڷ��������У��촰�޵�����λ��
//����λ��Ϊ�Ϸ����·������У���.
#define SetLineLinePosTop(line) ((line) = (0xFCFF&(line))|0x0000)
#define SetLineLinePosMiddle(line) ((line) = (0xFCFF&(line))|0x0100)
#define SetLineLinePosBottom(line) ((line) = (0xFCFF&(line))|0x0200)

#define IsLineLinePosTop(line)	((0x0300&(line)) == 0x0000)
#define IsLineLinePosMiddle(line)	((0x0300&(line)) == 0x0100)
#define IsLineLinePosBottom(line)	((0x0300&(line)) == 0x0200)

//����λ��ʹ�ø�λ�ֽڵĵ͵���λ
//0��ʾ�Ϸ���ȱʡ��ʽ,1��ʾ�·�
#define SetLineTextPosTop(line) ((line) = (0xFBFF&(line))|0x0000)
#define SetLineTextPosBottom(line) ((line) = (0xFBFF&(line))|0x0400)

#define IsLineTextPosTop(line)	((0x0400&(line)) == 0x0000)
#define IsLineTextPosBottom(line)	((0x0400&(line)) == 0x0400)


//�Ƿ��Ǽ������ߵı�־ʹ�ø�λ�ֽڵĵ͵���λ
//0��ʾʵ�ߣ�ȱʡ��ʽ,1��ʾ����
#define SetLineSolid(line) ((line) = (0xF7FF&(line))|0x0000)
#define SetLineDash(line) ((line) = (0xF7FF&(line))|0x0800)

#define IsLineSolide(line)	((0x0800&(line)) == 0x0000)
#define IsLineDash(line)	((0x0800&(line)) == 0x0800)

//ʹ�õ��ֽڵĵͶ�λ
//00���У�01���У�10������ȱʡ��11�޷��򣬱�ʾѡ����һ������˵���������
#define SetLineDirectionUp(line) ((line) = (0xFFFC&(line))|0x0000)
#define SetLineDirectionDown(line) ((line) = (0xFFFC&(line))|0x0001)
#define SetLineDirectionDual(line) ((line) = (0xFFFC&(line))|0x0002)
#define SetLineDirectionNone(line) ((line) = (0xFFFC&(line))|0x0003)

#define IsLineDirectionUp(line) ((0x0003&(line)) == 0x0000)
#define IsLineDirectionDown(line) ((0x0003&(line)) == 0x0001)
#define IsLineDirectionDual(line) ((0x0003&(line)) == 0x0002)
#define IsLineDirectionNone(line) ((0x0003&(line)) == 0x0003)

//ʹ�õ��ֽڵĵ�����λ
//00���������01���ͳ�������������
#define SetLineFlagInhibit(line) ((line) = (0xFFF3&(line))|0x0000)
#define SetLineFlagLightTrain(line) ((line) = (0xFFF3&(line))|0x0004)

#define IsLineFlagInhibit(line) ((0x000C&(line)) == 0x0000)
#define IsLineFlagLightTrain(line) ((0x000C&(line)) == 0x0004)

//���ø��ֽڵĸ�һλ 11XX 1111 1111 1111(EFFF)����ʾ��ʾ����ɫ
#define SetLineColorRed(line) ((line) = (0xCFFF&(line))|0x0000)   //��ɫ
#define SetLineColorHe(line) ((line) =  (0xCFFF&(line))|0x1000)   //ź��ɫ
#define SetLineColorGreen(line) ((line) = (0xCFFF&(line))|0x2000)   //��ɫ
#define SetLineColorBlue(line) ((line) = (0xCFFF&(line))|0x3000)   //��ɫ
 
#define IsLineColorRed(line)	((0x3000&(line)) == 0x0000)
#define IsLineColorHe(line)	    ((0x3000&(line)) == 0x1000)
#define IsLineColorGreen(line)	((0x3000&(line)) == 0x2000)
#define IsLineColorBlue(line)	((0x3000&(line)) == 0x3000)


//ʹ��slow_time�ֶεͶ�λ��ʾͼ��ע������ΧԲȦ��λ��
//00��ʾû��ԲȦ��01��ʾ��Ȧ��10��ʾ��Ȧ
#define SetSLWRoundNone(slow_time) ((slow_time) = (0xFFFC&(slow_time))|0x0000)
#define SetSLWRoundBlue(slow_time) ((slow_time) = (0xFFFC&(slow_time))|0x0001)
#define SetSLWRoundRed(slow_time) ((slow_time) = (0xFFFC&(slow_time))|0x0002)
#define SetSLWRoundCircle(slow_time) ((slow_time) = (0xFFFC&(slow_time))|0x0003)//zgj add

#define IsSLWRoundNone(slow_time) ((0x0003&(slow_time)) == 0x0000)
#define IsFrameBlue(slow_time) ((0x0003&(slow_time)) == 0x0001)
#define IsFrameRed(slow_time) ((0x0003&(slow_time)) == 0x0002)
#define IsCircleRed(slow_time) ((0x0003&(slow_time)) == 0x0003)
#define IsCircleBlue(slow_time)		((0x0007&(slow_time)) == 0x0004)

typedef Pstruct _AbsInhibitElement {
	ushort nEntryNo;
	ushort usChangeFlag;
	long usOldStartDate;
	long usOldStartTime;
	long usStartDate;
	long usStartTime;
	long usEndDate;
	long usEndTime;
	ushort usDirection;
	ushort usReverseRunFlag;
} ABSINHIBITELEMENT;

typedef Pstruct _AbsInhibitInform {
	int nStationNo;
	int nInhibitNum;
	int nAllocNum;
	ABSINHIBITELEMENT *sInhibit[96];	
} ABSINHIBITINFORM;

typedef Pstruct _SideInhibitElement {
	ushort nSideNo;
	ushort usChangeFlag;
	long usOldStartDate;
	long usOldStartTime;
	long usStartDate;
	long usStartTime;
	long usEndDate;
	long usEndTime;
} SIDEINHIBITELEMENT;

typedef Pstruct _SideInhibitInform {
	int nStationNo;
	int nInhibitNum;
	int nAllocNum;
	SIDEINHIBITELEMENT *sInhibit[96];	
} SIDEINHIBITINFORM;

typedef Pstruct _tagAbsSlowElement {
	ushort nEntryNo;// entry no
	ushort usChangeFlag;
	long usOldStartDate;
	long usOldStartTime;
	long usStartDate;
	long usStartTime;
	long usEndDate;
	long usEndTime;
	ushort usSlowLength;
	ushort usSpeed;
} ABSSLOWELEMENT;

typedef Pstruct _tagAbsSlowInform {
	int nStationNo;
	int nAbsSlowNum;
	int nAllocNum;
	ABSSLOWELEMENT sAbsSlow[8];	
} ABSSLOWINFORM;

Pstruct _tagConflictElement {
	ushort usConflictid;
	ushort usTimeGap;
	char trainid[10];
	USHORT  train_idx;
	ushort usStationNo;
	ushort usElementType;
	ushort usElementId;
	ushort usFlag;
	long nStartTime;
	long nEndTime;
};

typedef  Pstruct _tagConflictElement		TRAIN_CONFLICT_ELE;


typedef Pstruct _tagConflictInform {
	Pstruct _tagConflictElement ele1;
	Pstruct _tagConflictElement ele2;
} CONFLICTINFORM;

Pstruct _tagConflictInfo {
	int count;
	CONFLICTINFORM rec[1];
};





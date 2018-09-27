#ifndef _DEF_SCHEDULE_H
#define _DEF_SCHEDULE_H

/*  this file contains schedule related data struct  */

#include "tg_defines.h"
#define  STR_CPY(x,y)   {strncpy(x,y,sizeof(x)-1);x[sizeof(x)-1]=0;}
enum TRAIN_IN_OUT 
{
	TRAIN_OUT_STATION,
	TRAIN_INTO_STATION
};
enum ARRIVE_DEPART_STATUS
{
	STATION_NULLSTATUS,
	STATION_ARRIVE,
	STATION_DEPART,
	STATION_PASS,
	MAX_ACT_TYPE_COUNT=3
};
enum OCCUPY_TYPE 
{
	OCCUPY_NULL,
	OCCUPY_ABS,
	OCCUPY_STA,
	OCCUPY_SAME_SIDE,
	OCCUPY_SIDE_AFTER_TERMINAL
};
enum GAP_TYPE 
{
	OCCUPY_SAME_SIDE_GAP=0,
	OCCUPY_SIDE_AFTER_TERMINAL_GAP=1,
	CONTINUE_DEPART_GAP=2,
	MEET_TRAIN_GAP=3,
	DIFF_ARRIVE_GAP=4,
	ABS_BLOCK_GAP=5,
	SIDE_BLOCK_GAP=6,
	TRACE_GAP=10,
	NO_GAP=19
};
enum RUN_TYPE 
{
	NULL_RUNTYPE,
	FREIGHT_TRAIN,
	PASSANGER_TRAIN,
	MAX_RUN_TYPE_COUNT=2
};
enum LINEBLOCK_TYPE 
{
	NULL_LINEBLOCK,
	AUTO_BLOCK,
	HALF_BLOCK
};

enum ELEMENT_STATUS_TYPE
{
	TG_ABS_INHIBIT=1,
	TG_ABS_FIX_MARK=2, // �ۺ��촰��
	TG_ABS_SLOW=3,
	TG_SIDE_SLOW=4,
	TG_SIDE_INHIBIT=5,
	TG_GRAPH_NOTE=7,
	TG_CHANGE_BLOCK_TYPE=9,
	TG_SIDE_TIANCHUANG=10,
	TG_SECTION_ELEC_FIX=11,
	TG_SIDE_OCCUPY=12,
	TG_SECTION_ELEC_FIX_LANZHOU=16
};

enum NOT_MEET_BIT_FLAG
{
	NOTMEET_CRH       =0x01,
	NOTMEET_SEPECIAL_Z=0x02,
	NOTMEET_PASSENGER =0x04,
	NOTMEET_FREIGHT   =0x08,
	NOTMEET_SPACE     =0x10,

	NOTMEET_200_orUp=0x0100,
	NOTMEET_120_200 =0x0200,
	NOTMEET_120_Low =0x0400,
	NOTMEET_ALL     =0x0800,
};

#define TG_IS_ABS_MARK(x)  (x==TG_ABS_INHIBIT || x==TG_ABS_SLOW || x==TG_SECTION_ELEC_FIX || x==TG_ABS_FIX_MARK || x==TG_SECTION_ELEC_FIX_LANZHOU)
#define TG_IS_INSTATION_MARK(x)  (x==TG_SIDE_SLOW || x==TG_SIDE_INHIBIT || x==TG_SIDE_TIANCHUANG || x==TG_SIDE_OCCUPY)

enum TRAIN_MEET_RELATION
{
	NO_MEET=0,
    DIFF_DIR_MEET=1,
    SAME_DIR_MEET=2
};

struct TG_MsgStamp{
	int	          con_id;
	int           schedule_type;
	TRAIN_INDEX   index;
	TIME          ltime;
	ENTITY_ID     operatr;
};
/*   �г�������Ϣ  */

struct TG_TrainEngineInfo
{
	TRAIN_NO     engine_id;            // ������
	TRAIN_NO     engine_type;          // ��������
	long         engine_status;        // bit 7-6:  from dispatcher/subao
                                      // bit 5-4:  0: traction  1 - attach
									  // bit 0-3:  0:��ȼ 1:����
	short       attach_sta;           // index of trainrecord
	short       unattach_sta;
};

/* �г�����ͣ����Ϣ */
struct TG_TrainAbsStop
{
	BYTE    flag;     // 1: stop out of station, 0: abs stop
	short   position;
	TIME    start_time;
	TIME    stop_time;
};

/* �г�ʱ����Ϣ */
struct TG_TrainRecord
{
	TRAIN_NO     arrive_train_id;                       // ���ﳵ��
	TRAIN_NO     original_arrive_train_id;              // ԭʼ���ﳵ�Σ���ñʱʹ��

	TRAIN_NO     depart_train_id;                       // ��������
	TRAIN_NO     original_depart_train_id;              // ԭʼ�������Σ���ñʱʹ��

	STATION_NO   station;                               // վ��
	ENTRY        entry;                                 // ��ں�
	ENTRY        exit;                                  // ���ں�

	BYTE     arrive_side;                               // ����ɵ�
	BYTE     plan_arrive_side;                          // ͼ������ɵ�
	BYTE     depart_side;                               // �����ɵ�
	BYTE     plan_depart_side;                          // ͼ�������ɵ�
	short    min_stop_time;                             // Ӫҵʱ��

	TIME    arrive;                                     // ����ʱ��
	TIME    depart;                                     // ����ʱ��
	TIME    plan_arrive;                                // ͼ������ʱ��
	TIME    plan_depart;                                // ͼ������ʱ��
	TIME    send_plan_time;                             // �ƻ��´�ʱ��
	long    stop_reason;
	long    late_reason;

	long    early_depart_time;                          // ������ǰ����ʱ��
						// 0x0000 -> 0x7FFF, ������ǰ����������
						// 0x8000 ������ǰ����������
						// 0x0000 ��������ǰ����
	long    run_fast_slow_time;                         // �ϻ�����Ϣ
						// bit 7:   0 - ��,   1 - ��
						// bit 6-0:   �ϻ���ʱ�� (����)

	long    route_flag;                                 // ��·����״̬
						// bit 1-0:   00/01/10/11  �ӳ���·  no(clear)/doing/OK/OCCUPY
						// bit 3-2:   00/01/10/11  ������·  no(clear)/doing/OK/OCCUPY
	long    rank;		// �ص�����Ϣ��TMIS��Ӧ
						// bit 0:  ����---��ʹ�ñ���Ϊ0�������ݼ���,��bit16-bit21���
						// bit 1:  �ص㾯��
						// bit 2:  ��Ʒ
						// bit 3:  ����
					    // bit 0-bit15���ֶκ�TMIS��Ӧ, ��������ʹ��

						// bit16-bit18: ���ﳬ�� 0:���� 1:��������  2:һ������ 3:�������� 4:��������
						// bit19-bit21: �������� 0:���� 1:��������  2:һ������ 3:�������� 4:��������

	long    speed_limit;								// �����г�������Ϣ
						// 0: ������, else: �������ֵ

	long    stop_condition;
						// bit 1:    1-ԭ����Ϊͨ���г�(����)�����г�����ʱ�Զ���·
						// bit 2:    1-�޼ƻ�����ͣ��
						// bit 3:    1-����ʹ�������ɵ�
						// bit 4:    1-���ò���ͨ��
						// bit 5:    1-�����Զ�����ӳ���·
						// bit 6:    1-�����Զ���������·
						// bit 7:    0-�������������������·,  1-���ƻ�ʱ����������·
						// bit 8:    1-��ֹͣ��
						// bit 9:    1-����ͣ��
						// bit 10:   1-ѹǰ,��ͷͣ�ڹɵ���, ��β��ȫ����ɵ�
	                    // bit 11:   1-ѹ��,��ͷ��ȫ����ɵ�, ��βͣ�ڹɵ���
						// bit 12:	 1-�˹����ý�·����ģʽ
						// bit 13:   1-OPMS�ƻ��������1:��ֹͨ��
						// bit 14:   �ɵ��ͽ�·�۷��ɵ���ͻ 1-����ʾ�ɵ� 0:������ʾ�ɵ�
						// bit 15:   �޸Ľ�·�ɵ� 1-����ʾ�ɵ� 0:������ʾ�ɵ�
	long    adjust_status;
						// bit 0:  0 : ����ʱ��Ϊ�ƻ�
						//         1 : ����ʱ��Ϊʵ��
						// bit 1:  0 : ����ʱ��Ϊ�ƻ�
						//         1 : ����ʱ��Ϊʵ��
						// bit 2:  0 : ����
						//         1 : ���ڴ�����Ϣ
						// bit 3:  0 : �����ɹ�
						//         1 : ����ʧ��
						// bit 4:  1 : ����ͣ��
						// bit 5:  1 : 
						// bit 6:  0 : arrivetime is same with the nextcon's arr
						//         1 : not same(only for inter_sta)
						// bit 7:  0 : departtime is same with the nextcon's dep
						//         1 : not same(only for inter_sta)
						// bit 8 :   ����ʱ����
						// bit 9 :   ����ʱ����
						// bit 10:  0  ʵ�ʵ���ʱ��Ϊ�˹�����
						//          1  ʵ�ʵ���ʱ��Ϊ�Զ�����
						// bit 11:  0  ʵ�ʳ���ʱ��Ϊ�˹�����
						//          1  ʵ�ʳ���ʱ��Ϊ�Զ�����
						// bit 12:  
						//          
						// bit 13,14:  0  δ���͵���̨��̨��վ��
						//                 1  ��̨�ܾ�
						//                 2  ��̨����
						//       
						// bit 15:  0  �޵���ǣ��
						//          1  ����ǣ��
						// bit 16: 1:�ֽ�ڳ�վ
	                    // bit 17: 1:̨��վ
						// bit 18: 1:�Զ������۷�����
	                   	// bit 19: 1:3.0�����ػ���ͼ�����
	                   	// bit 20: 1:3.0�����ػ���ͼ�ɵ�

	short   spec_run_time;      // ָ������ʱ��
								// �� TG_NOTMEET_xxx �ж�
								// BIT ����� NOTMEET_xxx
	long    can_not_meet;       // ���ᳵ����Ϣ
						//     bit 0: ���ᶯ����
						//     bit 1: ����ֱ��
						//     bit 2: ����ͳ�
						//     bit 3: �����г�
						//     bit 4: ����
						//     bit 5: ����

	long    super_interval_ltime; // ָ���г����
					// 3-0: 0 ʹ��Ĭ������, else: ����� (min)
					// 7-4: 0 ʹ��Ĭ������, else: �Ҳ��� (min)

	short         special_position;
	WORD         special_time_1;
	WORD         special_time_2;
	TRAIN_INDEX   special_train_index;

	long    data_source;
						// bit 03-00:  define side infomation's source(0-7)
						//   0 - basic schedule defined       1 - addition schedule defined
						//   2 - static side                  3 - dispatcher changed
						//   4 - auto adjusted                5 - sta watcher changed plan
						//   6 - sta watcher reported actual  7 - lirc changed plan
						//   8 - auto report                  9 - pathway defined

						// bit 07-04:  define arrive information's source(0-6)
						// bit 11-08:  define depart information's source(0-6)
						//   0 - default(create, dispatcher change)    1 - auto adjust
						//   2 - lirc changed plan                     3 - dispatcher turn actual
						//   4 - sta watcher changed plan              5 - sta watcher report 
						//   6 - auto report                           7 - auto report match
						//   8 - simple adjust(calculate by runtime)

						// bit 14-12:  define plan arrive information's source(0-2)
						// bit 17-15:  define plan depart information's source(0-2)
						//   0 - basic schedule               1 - addition schedule
						//   2 - auto create (the time while non-basic schedule is created )
						//   3 - manual set 

	long    flag;
						// bit  2-0:
						//            001 - ����
						//            010 - �յ�
						//            011 - ͣ��
						//            100 - ����
						//            101 - ����
						//            110 - �г��ϲ�����
						//            111 - �г������ڷֲ�����
						// bit  4-3:    01 - ����
						//              10 - ʼ��
						// bit  5:  1 - �����г�
						// bit  7:  1 - ����ͣ��
						// bit  8:  1 - ����ͣ��
						// bit  9:  1 - ����;���۷�
						// bit 10:  1 - ����ͨ��
						// bit 11:  1 - ��Ԯ�����г��� �����г�Ϊspecial_train_index
						// bit 12:  1 - �������г���Ԯ�� �����г�Ϊspecial_train_index
						// bit 13:  1 - ��������ͣ��, 0 ��������ͣ��
						// bit 14:  1 - ��������г��� �����г�Ϊspecial_train_index
						// bit 15:  1 - �������г���ϣ� �����г�Ϊspecial_train_index
						// bit 16:  1 - �����յ�
						// bit 17:  1 - ����ʼ��
};

// �г��۷�λ�� վ�� վ��
//bit1:0 ͨ�ð汾, bit2������
//bit1:1 �����ǳǼ��۷�,bit2������,����bitΪ1��վ���۷� bit2Ϊ0��ԭ���۷�
#define IsNewReturn(side_utility)		   (((side_utility)&0x2) == 0x2)	//bit1:1�Ǽ��۷� 0:������
#define IsStationOutReturn(side_utility)    (((side_utility)&0x6) == 0x6)	//bit2
#define SetStationOutReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x6)	//bit2
#define SetStationReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x2)	//bit2
#define NotStationReturn(side_utility)   ((side_utility) &= ~0x6)	//bit2

/* �г�������Ϣ */
struct TG_TrainInfo 
{
	TG_TrainInfo()
	{
		memset(this, 0, sizeof(TG_TrainInfo));
	}
/*   ������Ϣ  */
	char          train_name[MAX_TRAIN_NAME_LEN];   //  ���κ����

	TRAIN_KEY     train_key;

	TRAIN_INDEX   gpc_train_index;      // GPC����������
	TRAIN_INDEX   basic_train_index;    // ���ݻ���ͼ����ʱ, ����ͼ������
	TRAIN_INDEX   shift_train_index;    // �����հ�ƻ�����ʱ,�հ�ƻ�������
	char          shift_string[32]; // OPMS4.0��ʾ
	TIME          update_time;      // ɾ��ʱ��
	short         delete_flag;      //  -1, ��
	BYTE          type;        // �г���ϸ���� (bit 7: 0 - passenger 1 -freight)
	BYTE          run_type;    // �г����ԣ����ڼ�����������ʱ����Զ���·��ǰ����ʱ��

	TIME          start_plan_time;     // ʼ���ƻ�ʱ��
	TIME          start_actual_time;   // ʼ��ʵ��ʱ��
	TIME          sborder_plan_time;   // �ֽ�ڽ���ƻ�ʱ��
	TIME          sborder_actual_time; // �ֽ�ڽ���ʵ��ʱ��
	TIME          eborder_plan_time;   // �ֽ�ڽ����ƻ�ʱ��
	TIME          eborder_actual_time; // �ֽ�ڽ���ʵ��ʱ��
	STATION_NO    sborder_sta;         // �ֽ�ڽ���վ��
	STATION_NO    eborder_sta;         // �ֽ�ڽ���վ��
	char          start_sta_name[MAX_STA_NAME_LEN];  // ��ʼ��վ��
	char          end_sta_name[MAX_STA_NAME_LEN];    // �յ���վ��
	CONID         from_conid;          // �󷽵���̨��
	CONID         to_conid;            // ǰ������̨��

	BYTE         priority;             // �г��ȼ�
	long         train_length;         // ����
	long         train_weight;         // ����
	long         status;               
						// bit 0-2:   0 - �������ٱ�,       1 - �ٱ����ڳ�վ�ϱ� 
						//            2 - �ٱ�������̨����, 3 - �ٱ����ڵ���Ա�ֹ�
						//            4 - �ٱ��������߳��κ�
	                    
						// bit 3: 1 - �����г�
						// bit 4: 1 - TDMS�г�
	                    // bit 5: 1 - ǿ������
						// bit 16-31: ��������ֽڴ�� ����  ������ 20100715 Ӧ���ݾ�����
/*   �����г���Ϣ  */
	BYTE    space_rank;       // 0: no out of space, else: rank of space
	BYTE    length_rank;      // 0: no out of length, else: rank of length
	BYTE    super_rank;       // 0: not super_train, else: rank of super_train
	BYTE    military_rank;    // 0: not military_train, else: rank of military_train
	

/*   �г�������Ϣ  */
	long     engine_utility;  // 0: normal
								// 1: limited_time
								// 2: no locomotive
	long     side_utility;    // bit0:   1 :   need high platform
							  // bit 1-2
							  // bit 1: 1:bit2��Ч 0:bit2��Ч
							  // bit 2: 1:վ���۷� 0:ԭ���۷�	
							  // bit 4-5
							  // bit 4: 1:bit5��Ч 0:bit5��Ч
							  // bit 5: 1:վ���۷� 0:ԭ���۷�	

/*   �Զ�������Ϣ  */
	short   adjust_begin_sta;   // bit 7: 1 set by default,  0 set manual
	short   adjust_end_sta;     // keep it for future use

	long    adjust_flag;        // bit 0: 0 - join adjust           1 - not
								// bit 1: 0 - exist in shift schedule
								// bit 2: 0 - generate time_link    1 - not
								// bit 3: 0 - not in save before adjust   1 - do no use
								// bit 7-6:  0 - sborder_actual time is not set
								//           1 - sborder_actual time is set by manual
								//           2 - sborder_actual time is set by system
								// bit 10-8:  ��������ʾλ��  0-����ʾ   1-�� 2-��  4-��
								// bit 11:   �����ܹ����ݵ��г���־---������ҵ��
								// bit 12:   ����ֲ����� 1:��  0����
								// bit 13-14: ����ʼ���г� ͣ������ 01 ������ 10
								// bit 15: ����;���۷� 1���� 0:��
								// bit 16-31: ����ʼ���г� ��������ʱ��

	long    train_source;
};


struct TG_ScheduleBlock 
{
  TRAIN_INDEX   gpc_index;
  TRAIN_INDEX   basic_index;
  TRAIN_INDEX   shift_index;

  char          shift_string[32]; // OPMS4.0��ʾ
  TIME          update_time;      // ɾ��ʱ��
  short         delete_flag;      //  -1, ��Ч�� >=0 �ӳ�ɾ����¼����

  TIME     start_time;
  TIME     end_time;

  BYTE       type;           //  ABS SLOW,      ABS_INHIBIT,  ELECTRO_FIX,  SIDE_SLOW,    SIDE_INHIBIT,  GRAPH_NOTE, ABS_ELEC_FIX, REGIST_BLOCK_TYPE
  BYTE       flag;           //  limit_speed    flag          0             limit_speed   type           0,                        block_type(0-half,1-auto,2-tel)
  USHORT     line;            // used by gpc
  STATION_NO station;        //  0              0             0             station       station        top_position
  USHORT     index;          // used by gpc
  USHORT     position;       //  limit_length   position      position      limit_length  position       bottom_position
                             // bit 0:  1 -- inhibit first sta, 0 -- not;  bit 1: 1 -- inhibit second sta, 0 -- not ;  when type=SECTION_ELEC_FIX
  USHORT     direction;  // ��station & index==0xFF ʱ�����������ֶβ���Ч
  USHORT     station1;  
  USHORT     station2;
  USHORT     ele_count;
  USHORT     ele_number[MAX_SCHD_BLOCK_ELE_COUNT];
  long      adjust_status;   // bit 1-0:  0 �ƻ���1 ִ�С�2 ʵ��
  char      text[MAX_SCHD_BLOCK_TEXT_LEN];
  USHORT    s_meter;  //first sta num ,when type = SECTION_ELEC_FIX
  USHORT    e_meter;  //second sta num ,when type = SECTION_ELEC_FIX
  USHORT    e_kilo;
  USHORT    slow_time;
} ;

struct TG_TrainYingyeInfo
{
	STATION_NO	station_no;				// վ��
	I_TIME		arrive_time;			// ����ʱ��
	I_TIME		depart_time;			// ����ʱ��
	char		station_name[MAX_STA_NAME_LEN];			// վ��
	TG_TrainYingyeInfo()
	{
		station_no=0;
		arrive_time=0;
		depart_time=0;
		memset(station_name, 0, sizeof(station_name));
	}
};

struct TG_TrainCrewInfo
{
	short	crew_type;			//0:δ֪ 1��˾��	2����еʦ	3���˾�	4������	5������
	char	crew_info[64];		//�������绰
	char	crew_bureau[32];	//����·��
	TG_TrainCrewInfo()
	{
		crew_type=0;
		memset(crew_info, 0, sizeof(crew_info));
		memset(crew_bureau, 0, sizeof(crew_bureau));
	}
};

struct TG_TrainTicketInfo
{
	int			on_station_index;		// �ϳ�վ��
	STATION_NO	on_station;			// �ϳ�վ��
	int			down_station_index;		// �³�վ��
	STATION_NO  down_station;		// �³�վ��
	int			sale;					// ��������
	char		seat[32];				// ϯ��
	char	on_station_name[MAX_STA_NAME_LEN];			// �ϳ�վ��
	char	down_station_name[MAX_STA_NAME_LEN];			// �³�վ��
	TG_TrainTicketInfo()
	{
		on_station_index=0;
		on_station=0;
		down_station_index=0;
		down_station=0;
		sale=0;
		memset(seat, 0, sizeof(seat));
		memset(on_station_name, 0, sizeof(on_station_name));
		memset(down_station_name, 0, sizeof(down_station_name));
	}
};

struct TG_EngineCrewInfo
{
	STATION_NO	station_no;				// վ��
	I_TIME		overwork_time;			// ����ʱ��
	char	station_name[MAX_STA_NAME_LEN];			// վ��
	TG_EngineCrewInfo()
	{
		station_no=0;
		overwork_time=0;
		memset(station_name, 0, sizeof(station_name));
	}
};

struct TG_TrainOPMSRecordInfo
{
	STATION_NO	station_no;			// ��վվ��
	IDWORD	    work_flag;			// ��վ��ҵ
	IDWORD      oper_flag;			// ��������
	IDWORD      important_flag;		// ��Ҫ����

	char	    oper_info[256];
	char        note_text[256];
	TG_TrainOPMSRecordInfo()
	{
		station_no=0;
		work_flag=0;
		oper_flag=0;
		important_flag=0;
		memset(oper_info, 0, sizeof(oper_info));
		memset(note_text, 0, sizeof(note_text));
	}
};

/* */
#define TG_GetAnotherShiftId(shift_id)           (3 - shift_id)

#define TG_SetTccStatusNone(block)      ((block).dir = (0xCFFF&((block).dir))|0x0000)
#define TG_SetTccStatusConfirm(block)   ((block).dir = (0xCFFF&((block).dir))|0x1000)
#define TG_SetTccStatusExec(block)      ((block).dir = (0xCFFF&((block).dir))|0x2000)
#define TG_SetTccStatusCancel(block)    ((block).dir = (0xCFFF&((block).dir))|0x3000)

#define TG_IsTccStatusNone(block)	     ((0x3000&((block).dir)) == 0x0000)
#define TG_IsTccStatusConfirm(block)	 ((0x3000&((block).dir)) == 0x1000)
#define TG_IsTccStatusExec(block)	     ((0x3000&((block).dir)) == 0x2000)
#define TG_IsTccStatusCancel(block)	 ((0x3000&((block).dir)) == 0x3000)

#define TG_IsFlagGiveout(flag)         ((flag & 0x0007) ==  0x0001)
#define TG_IsFlagTerminal(flag)        (((flag & 0x0007) ==  0x0002)  || ((flag & 0x0007) ==  0x0003))
#define TG_IsFlagEnd(flag)             ((flag & 0x0007) ==  0x0003)
#define TG_IsFlagKeep(flag)            ((flag & 0x0007) ==  0x0004)
#define TG_IsFlagRelay(flag)           ((flag & 0x0007) ==  0x0005)
#define TG_IsFlagEndInAbs(flag)        ((flag & 0x0007) ==  0x0006)
#define TG_IsFlagDivide(flag)          ((flag & 0x0007) ==  0x0007)
#define TG_IsFlagAccept(flag)          ((flag & 0x0018) ==  0x0008)
#define TG_IsFlagStart(flag)           ((flag & 0x0018) ==  0x0010)
#define TG_IsFlagReverse(flag)         (flag & 0x0020)
#define TG_IsFlagCorner(flag)          (flag & 0x0040)
#define TG_IsFlagStopSignal(flag)      ((flag & 0x0080) ==  0x0080)
#define TG_IsFlagStopAbs(flag)         ((flag & 0x0100) ==  0x0100)
#define TG_IsFlagAnLocReturn(flag)     (flag & 0x0200)
#define TG_IsFlagPassNonMainSide(flag) ((flag & 0x0400) ==  0x0000)
#define TG_IsFlagStopAbsUp(flag)       (flag & 0x2000)
#define TG_IsFlagStopAbsDown(flag)     (!(flag & 0x2000))
#define TG_IsFlagUnitOtherTrain(flag)  (flag & 0x4000)   //bit 14
#define TG_IsFalgBeUnitedByOther(flag) (flag & 0x8000)   //bit 15
#define TG_IsFlagStopMainSide(flag)    (0)
#define TG_IsFlagAbsTerminal(flag)     (flag & 0x00010000)
#define TG_IsFlagAbsStart(flag)        (flag & 0x00020000)

#define TG_SetFlagGiveout(flag)         (flag &= 0xFFFFFFF8,flag|= 0x0001)
#define TG_SetFlagTerminal(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0002)
#define TG_SetFlagEnd(flag)             (flag &= 0xFFFFFFF8,flag|= 0x0003)
#define TG_SetFlagKeep(flag)            (flag &= 0xFFFFFFF8,flag|= 0x0004)
#define TG_SetFlagRelay(flag)           (flag &= 0xFFFFFFF8,flag|= 0x0005)
#define TG_SetFlagEndInAbs(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0006)
#define TG_SetFlagDivide(flag)          (flag &= 0xFFFFFFF8,flag|= 0x0007)
#define TG_SetFlagAccept(flag)          (flag &= 0xFFFFFFE7,flag|= 0x0008)
#define TG_SetFlagStart(flag)           (flag &= 0xFFFFFFE7,flag|= 0x0010)
#define TG_SetFlagReverse(flag)         (flag |= 0x0020)
#define TG_SetFlagCorner(flag)          (flag |= 0x0040)
#define TG_SetFlagStopSignal(flag)      (flag |= 0x0080)
#define TG_SetFlagStopAbs(flag)         (flag |= 0x0100)
#define TG_SetFlagAnLocReturn(flag)     (flag |= 0x0200)
#define TG_SetFlagPassNonMainSide(flag) (flag |= 0x0400)
#define TG_SetFlagStopAbsUp(flag)       (flag |= 0x2000)
#define TG_SetFlagStopAbsDown(flag)     (flag &= 0xFFFFDFFF)
#define TG_SetFlagUnitOtherTrain(flag)  (flag |= 0x4000)
#define TG_SetFlagBeUnitedByOther(flag) (flag |= 0x8000)
#define TG_SetFlagStopMainSide(flag)    (flag)
#define TG_SetFlagAbsTerminal(flag)     (flag |= 0x00010000)
#define TG_SetFlagAbsStart(flag)        (flag |= 0x00020000)

#define TG_ClearFlagGiveoutTerminal(flag) (flag &= ~0x3)
#define TG_ClearFlagStartAccept(flag)     (flag &= ~0x18)

#define TG_ClearFlagReverse(flag)         (flag &= ~0x0020)
#define TG_ClearFlagCorner(flag)          (flag &= ~0x0040)
#define TG_ClearFlagStopSignal(flag)      (flag &= ~0x0080)
#define TG_ClearFlagStopAbs(flag)         (flag &= ~0x0100)
#define TG_ClearFlagAnLocReturn(flag)     (flag &= ~0x0200)
#define TG_ClearFlagPassNonMainSide(flag) (flag &= ~0x0400)
#define TG_ClearFlagStopMainSide(flag)    (flag)
#define TG_RemoveFlagStatus(flag)         (flag &= ~0x1F)

#define TG_RemoveFlagLeaveStatus(flag)    (flag &= ((TG_IsFlagEndInAbs(flag) || TG_IsFlagDivide(flag)) ? 0xFFFFFFFF:0xFFFFFFF8))
#define TG_RemoveFlagEnterStatus(flag)    (flag &= 0xFFFFFFE7)

#define TG_ClearFlagUnitOtherTrain(flag)  (flag &= 0xFFFFBFFF)
#define TG_ClearFlagBeUnitedByOther(flag) (flag &= 0xFFFF7FFF)
#define TG_ClearFlagAbsTerminal(flag)     (flag &= ~0x00010000)
#define TG_ClearFlagAbsStart(flag)        (flag &= ~0x00020000)

#define TG_IsTrainPass(rec)                  ((rec).arrive == (rec).depart && (rec).arrive_side == (rec).depart_side && (!TG_IsFlagStart((rec).flag) && !TG_IsFlagTerminal((rec).flag)))

#define TG_IsDefinePass(rec)                 ((rec).stop_condition & 0x02)
#define TG_IsForcePass(rec)                  ((rec).stop_condition & 0x04)
#define TG_IsForceSide(rec)                  ((rec).stop_condition & 0x08)
#define TG_IsForcePassByMainSide(rec)        ((rec).stop_condition & 0x10)

#define TG_SetDefinePass(rec)                ((rec).stop_condition |= 0x02)
#define TG_SetForcePass(rec)                 ((rec).stop_condition |= 0x04)
#define TG_SetForceSide(rec)                 ((rec).stop_condition |= 0x08)
#define TG_SetForcePassByMainSide(rec)       ((rec).stop_condition |= 0x10)

#define TG_ReSetDefinePass(rec)              ((rec).stop_condition &= ~0x02)
#define TG_ReSetForcePass(rec)               ((rec).stop_condition &= ~0x04)
#define TG_ReSetForceSide(rec)               ((rec).stop_condition &= ~0x08)
#define TG_ReSetForcePassByMainSide(rec)     ((rec).stop_condition &= ~0x10)

#define TG_IsAutoMakeArriveRoute(rec)        (!((rec).stop_condition & 0x20))
#define TG_IsAutoMakeDepartRoute(rec)        (!((rec).stop_condition & 0x40))
#define TG_SetAutoMakeArriveRoute(rec)       ((rec).stop_condition &= ~0x20)
#define TG_SetAutoMakeDepartRoute(rec)       ((rec).stop_condition &= ~0x40)
#define TG_SetNotAutoMakeArriveRoute(rec)    ((rec).stop_condition |= 0x20)
#define TG_SetNotAutoMakeDepartRoute(rec)    ((rec).stop_condition |= 0x40)

#define TG_IsMakeDepartRouteByMeet(rec)      (((rec).stop_condition & 0x80) == 0x00)
#define TG_SetMakeDepartRouteByMeet(rec)     ((rec).stop_condition &= ~0x80)
#define TG_IsMakeDepartRouteByPlan(rec)      (((rec).stop_condition & 0x80) == 0x80)
#define TG_SetMakeDepartRouteByPlan(rec)     ((rec).stop_condition |= 0x80) 

// TDMSҪ��:��ֹͣ��
#define TG_IsMustNotStop(rec)     ((rec).stop_condition & 0x100) 
#define TG_SetFlagMustNotStop(rec)       ((rec).stop_condition |= 0x100) 
#define TG_ClearFlagMustNotStop(rec)     ((rec).stop_condition &= ~0x100) 

// TDMSҪ��:����ͣ��
#define TG_IsMustStop(rec)     ((rec).stop_condition & 0x200) 
#define TG_SetFlagMustStop(rec)       ((rec).stop_condition |= 0x200) 
#define TG_ClearFlagMustStop(rec)     ((rec).stop_condition &= ~0x200) 

// ѹǰ
#define TG_IsBeforePressure(rec)         (((rec).stop_condition & 0x400) == 0x400)

// ѹǰ
#define TG_IsAfterPressure(rec)         (((rec).stop_condition & 0x800) == 0x800)

// TDMSҪ��:ͣ��
#define TG_IsManualSetTrigeRoute(rec)   (((rec).stop_condition & 0x1000) == 0x1000)
#define TG_ManualSetTrigeRoute(rec)   ((rec).stop_condition |= 0x1000)

// TDMSҪ��:��ֹͨ��
#define TG_IsMustNoPass(rec)     ((rec).stop_condition & 0x2000) 
#define TG_SetFlagMustNoPass(rec)       ((rec).stop_condition |= 0x2000) 
#define TG_ClearFlagMustNoPass(rec)     ((rec).stop_condition &= ~0x2000) 

// �ɵ��ͽ�·�۷��ɵ���ͻ,�Ƿ���ʾ�ɵ�,�ص���ʾ��
#define TG_IsDispalySide(rec)     ((rec).stop_condition & 0x4000) 
#define TG_SetFlagDispalySide(rec)       ((rec).stop_condition |= 0x4000) 
#define TG_ClearFlagDispalySide(rec)     ((rec).stop_condition &= ~0x4000) 

//  �޸Ľ�·�ɵ�, �Ƿ���ʾ�ɵ�,�ص���ʾ��
#define TG_IsDispalySide2(rec)     ((rec).stop_condition & 0x8000) 
#define TG_SetFlagDispalySide2(rec)       ((rec).stop_condition |= 0x8000) 
#define TG_ClearFlagDispalySide2(rec)     ((rec).stop_condition &= ~0x8000) 

#define TG_IsArrivePlan(rec)           (!((rec).adjust_status & 0x0001))
#define TG_IsArriveActual(rec)         ((rec).adjust_status & 0x0001)
#define TG_IsDepartPlan(rec)           (!((rec).adjust_status & 0X0002))
#define TG_IsDepartActual(rec)         ((rec).adjust_status & 0x0002)
#define TG_IsStatusRight(rec)          (!((rec).adjust_status & 0x0004))
#define TG_IsStatusError(rec)          ((rec).adjust_status & 0x0004)
#define TG_IsStatusCanAdjust(rec)      (!((rec).adjust_status & 0x0008))
#define TG_IsStatusCannotAdjust(rec)   ((rec).adjust_status & 0x0008)
#define TG_SetArrivePlan(rec)          ((rec).adjust_status &= 0xFFFE)
#define TG_SetArriveActual(rec)        ((rec).adjust_status |= 0x0001)
#define TG_SetDepartPlan(rec)          ((rec).adjust_status &= 0xFFFD)
#define TG_SetDepartActual(rec)        ((rec).adjust_status |= 0x0002)
#define TG_SetStatusRight(rec)         ((rec).adjust_status &= ~0x0004)
#define TG_SetStatusError(rec)         ((rec).adjust_status |= 0x0004)
#define TG_SetStatusCanAdjust(rec)     ((rec).adjust_status &= ~0x0008)
#define TG_SetStatusCannotAdjust(rec)  ((rec).adjust_status |= 0x0008)

/// 
#define TG_IsActualArrivalTime(adjust_status)  (adjust_status & 0x0001)
#define TG_IsActualDepartTime(adjust_status)  (adjust_status & 0x0002)
#define TG_SetActualArrivalTime(adjust_status)  adjust_status|=0x0001
#define TG_SetActualDepartTime(adjust_status)   adjust_status|=0x0002

#define TG_IsTechStopTrain(rec)        (((rec).adjust_status & 0x0010))
#define TG_SetTechStopTrain(rec)       ((rec).adjust_status |= 0x0010)
#define TG_ClearTechStopTrain(rec)     ((rec).adjust_status &= ~0x0010)

#define TG_IsArriveLock(rec)           (((rec).adjust_status & 0x0100) == 0x0100)
#define TG_IsDepartLock(rec)           (((rec).adjust_status & 0x0200) == 0x0200)
#define TG_SetArriveLock(rec)          ((rec).adjust_status |= 0x0100)
#define TG_SetDepartLock(rec)          ((rec).adjust_status |= 0x0200)
#define TG_ReSetArriveLock(rec)        ((rec).adjust_status &= ~0x0100)
#define TG_ReSetDepartLock(rec)        ((rec).adjust_status &= ~0x0200)

#define TG_IsInterArriveSame (rec)     (!((rec).adjust_status & 0x0040))
#define TG_IsInterArriveDiffer (rec)   ((rec).adjust_status & 0x0040)
#define TG_SetInterArriveSame (rec)    ((rec).adjust_status &= ~0x0040))
#define TG_SetInterArriveDiffer (rec)  ((rec).adjust_status |= 0x0040))
#define TG_IsInterDepartSame (rec)     (!((rec).adjust_status & 0x0080))
#define TG_IsInterDepartDiffer (rec)   ((rec).adjust_status & 0x0080)
#define TG_SetInterDepartSame (rec)    ((rec).adjust_status &= ~0x0080))
#define TG_SetInterDepartDiffer (rec)  ((rec).adjust_status |= 0x0080)

#define TG_IsArriveActualByManual(rec) (!((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define TG_IsArriveActualByAuto(rec)   (((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define TG_IsDepartActualByManual(rec) (!((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define TG_IsDepartActualByAuto(rec)   (((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define TG_SetArriveActualByManual(rec) ((rec).adjust_status|=0x0001,(rec).adjust_status&= ~0x0400)
#define TG_SetArriveActualByAuto(rec)   ((rec).adjust_status|=0x0001,(rec).adjust_status|=0x0400)
#define TG_SetDepartActualByManual(rec) ((rec).adjust_status|=0x0002,(rec).adjust_status&= ~0x0800)
#define TG_SetDepartActualByAuto(rec)   ((rec).adjust_status|=0x0002,(rec).adjust_status|=0x0800)

#define TG_SetAdjustBeginSta(rec)          ((rec).adjust_status |= 0x1000)              //bit 12
#define TG_SetAdjustNormalSta(rec)         ((rec).adjust_status &= ~0x1000)
#define TG_IsAdjustBeginSta(rec)           ((rec).adjust_status &  0x1000)
#define TG_SetNotSendPlanToNextCon(rec)    ((rec).adjust_status &= 0x9FFF)          //bit 13,14: 00
#define TG_SetBeRejectedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x2000)   //bit 13,14: 01
#define TG_SetBeReceivedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x4000)   //bit 13,14: 10
#define TG_IsRejectedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x2000)
#define TG_IsReceivedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x4000)
#define TG_IsSendPlanToNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x6000)

/// ����ǣ��
#define TG_SetElecLocoFlag(rec)          ((rec).adjust_status |= 0x8000)              //bit 15
#define TG_SetNoElecLocoFlag(rec)        ((rec).adjust_status &= ~0x8000)              //bit 15
#define TG_IsElecLocoFlag(rec)           ((rec).adjust_status & 0x8000)              //bit 15

// �ֽ�ڳ�վ
#define TG_SetFJKStationFlag(rec)        ((rec).adjust_status |= 0x10000)              //bit 16
#define TG_SetNoFJKStationFlag(rec)        ((rec).adjust_status &= ~0x10000)              //bit 16
#define TG_IsFJKStationFlag(rec)           ((rec).adjust_status & 0x10000)              //bit 16

// ̨�䳵վ
#define TG_SetNeighbourStationFlag(rec)        ((rec).adjust_status |= 0x20000)              //bit 17
#define TG_SetNoNeighbourStationFlag(rec)        ((rec).adjust_status &= ~0x20000)        //bit 17
#define TG_IsNeighbourStationFlag(rec)           ((rec).adjust_status & 0x20000)              //bit 17

// �Զ������۷�����
#define IsAutoSetReturnTrainId(rec)     (((rec).adjust_status&0x00040000) == 0x00040000)	//bit 18
#define SetAutoReturnTrainId(rec)       ((rec).adjust_status |= 0x00040000)					//bit 18
#define ResetAutoReturnTrainId(rec)     ((rec).adjust_status &= ~0x00040000)				//bit 18

// 3.0���ػ���ͼ�����
#define TG_IsNotCheckTrainExitEntry(rec)   (((rec).adjust_status&0x80000) == 0x80000)	//bit 19
#define TG_NotCheckTrainExitEntry(rec)	   ((rec).adjust_status |= 0x80000)					//bit 19
#define TG_CheckTrainExitEntry(rec)		   ((rec).adjust_status &= ~0x80000)				//bit 19

// 3.0�����ػ���ͼ�ɵ�
#define TG_IsNotCheckTrainSide(rec)          (((rec).adjust_status & 0x100000) == 0x100000)  // bit 20
#define TG_NotCheckTrainSide(rec)            ((rec).adjust_status |= 0x100000)
#define TG_CheckTrainSide(rec)			     ((rec).adjust_status &= ~0x100000)
//TDMSҪ�� ����rank
						// bit 0:  ����
						// bit 1:  �ص㾯��
						// bit 2:  ��Ʒ
						// bit 3:  ����
#define TG_IsSpaceRank(rec)               ((rec).rank & 0x01) //���ޱ�־---���޵ȼ�������bit16-bit21
#define TG_IsSpecialGaurdRank(rec)        ((rec).rank & 0x02)
#define TG_IsGoustRank(rec)               ((rec).rank & 0x04)
#define TG_IsMilitaryRank(rec)            ((rec).rank & 0x08)

#define TG_IsSpace(rec)					  (((rec).rank & 0x3F0000)>>16)
#define TG_GetArriveSpace(rec)            (((rec).rank & 0x070000)>>16) // �ӳ����޵ȼ�ֵ
#define TG_GetDepartSpace(rec)            (((rec).rank & 0x380000)>>19) // �������޵ȼ�ֵ

#define TG_SetArriveSpace(rec,  space)    ((rec).rank &= ~0x070000,  (rec).rank |= (space)<<16)
#define TG_SetDepartSpace(rec, space)     ((rec).rank &= ~0x380000,  (rec).rank |= (space)<<19)

#define TG_SetSpaceRank(rec)			   ((rec).rank |= 0x01)
#define TG_SetSpecialGaurdRank(rec)        ((rec).rank |= 0x02)
#define TG_SetGoustRank(rec)               ((rec).rank |= 0x04)
#define TG_SetMilitaryRank(rec)            ((rec).rank |=0x08)

#define TG_SetNoSpaceRank(rec)             ((rec).rank &=~0x01)
#define TG_SetNoMilitaryRank(rec)          ((rec).rank &=~0x08)

#define TG_IsSpecifyAbsRunTime(rec)              ((rec)->spec_run_time)
#define TG_SetSpecifyAbsRunTime(rec, ltime)      ((rec)->spec_run_time = ltime)

#define TG_SetRunFastSlowTime(rfstime, t)  (rfstime = (((short)(t/60)>127 ||(short)(t/60)<-127)? 0 :(((short)(t/60)<=0)?(short)(t/60)*(-1):(t/60+128))))
#define TG_InitRunFastSlowTIme(rfstime)    (rfstime = 0)

#define TG_IsPermitEarlyDepart(rec)           ((short)(rec).early_depart_time != (short)0x0000)
#define TG_SetCanNotEarlyDepart(rec)          ((rec).early_depart_time = 0x0000)
#define TG_IsNoLimitEarlyDepart(rec)          ((rec).early_depart_time == (short)0x8000)
#define TG_SetNoLimitEarlyDepart(rec)         ((rec).early_depart_time =  (short)0x8000)
#define TG_SetPermitEarlyDepart(rec, ltime)   ((rec).early_depart_time = (ltime - (rec).plan_depart)/60)
#define TG_GetEarlyDepartLimit(rec)           (!IsPermitEarlyDepart(rec)?(rec).plan_depart:IsNoLimitEarlyDepart(rec)?NULL_TIME:(rec).early_depart_time*60+(rec).plan_depart)

#define TG_SetNoConflictReason(rec)          ((rec).stop_reason &= 0x00)
#define TG_SetEntryConflictPre(rec)          ((rec).stop_reason &= 0x3F,(rec).stop_reason |= 0x40)           
#define TG_SetEntryConflictNext(rec)         ((rec).stop_reason &= 0x3F,(rec).stop_reason |= 0x80)           
#define TG_SetEntryConflictDevice(rec)       ((rec).stop_reason |= 0xC0)
#define TG_SetEntryNoConflict(rec)           ((rec).stop_reason &= 0x3F)           
#define TG_SetSideConflictPre(rec)           ((rec).stop_reason &= 0xCF,(rec).stop_reason |= 0x10)           
#define TG_SetSideConflictNext(rec)          ((rec).stop_reason &= 0xCF,(rec).stop_reason |= 0x20)           
#define TG_SetSideConflictDevice(rec)        ((rec).stop_reason |= 0x30)
#define TG_SetSideNoConflict(rec)            ((rec).stop_reason &= 0xCF)           
#define TG_SetExitConflictPre(rec)           ((rec).stop_reason &= 0xF3,(rec).stop_reason |= 0x04)           
#define TG_SetExitConflictNext(rec)          ((rec).stop_reason &= 0xF3,(rec).stop_reason |= 0x08)           
#define TG_SetExitConflictDevice(rec)        ((rec).stop_reason |= 0x0C)
#define TG_SetExitNoConflict(rec)            ((rec).stop_reason &= 0xF3)           
#define TG_SetAbsConflictPre(rec)            ((rec).stop_reason &= 0xFC,(rec).stop_reason |= 0x01)           
#define TG_SetAbsConflictNext(rec)           ((rec).stop_reason &= 0xFC,(rec).stop_reason |= 0x02)           
#define TG_SetAbsConflictDevice(rec)         ((rec).stop_reason |= 0x03)
#define TG_SetAbsNoConflict(rec)             ((rec).stop_reason &= 0xFC)           

#define TG_SetNoErrorReason(rec)             ((rec).stop_reason = 0x00)
#define TG_SetErrorStationNumber(rec)        ((rec).stop_reason = 0x01)
#define TG_SetErrorEntryNumber(rec)          ((rec).stop_reason = 0x02)
#define TG_SetErrorExitNumber(rec)           ((rec).stop_reason = 0x03)
#define TG_SetErrorSideNumber(rec)           ((rec).stop_reason = 0x04)
#define TG_SetErrorArriveDepartTime(rec)     ((rec).stop_reason = 0x05)
#define TG_SetErrorRunFlag(rec)              ((rec).stop_reason = 0x06)
#define TG_SetArriveTimeTooSmall(rec)        ((rec).stop_reason = 0x07)
#define TG_SetAbsRunTimeTooSmall(rec)        ((rec).stop_reason = 0x08)
#define TG_SetEntrySideNotConnect(rec)       ((rec).stop_reason = 0x09)
#define TG_SetSideExitNotConnect(rec)        ((rec).stop_reason = 0x0A)
#define TG_SetPrevAbsEntryNotConnect(rec)    ((rec).stop_reason = 0x0B)
#define TG_SetExitNextAbsNotConnect(rec)     ((rec).stop_reason = 0x0C)
#define TG_SetWrongSideArriveDirection(rec)  ((rec).stop_reason = 0x0D)
#define TG_SetWrongSideDepartDirection(rec)  ((rec).stop_reason = 0x0E)
#define TG_SetWrongSideArriveTrainType(rec)  ((rec).stop_reason = 0x0F)
#define TG_SetWrongSideDepartTrainType(rec)  ((rec).stop_reason = 0x10)
#define TG_SetWrongSideLength(rec)           ((rec).stop_reason = 0x11)
#define TG_SetWrongSideSpace(rec)            ((rec).stop_reason = 0x12)
#define TG_SetWrongSideWithouElec(rec)       ((rec).stop_reason = 0x13)
#define TG_SetWrongSideWithoutPlatform(rec)  ((rec).stop_reason = 0x14)
#define TG_SetWrongSideStartDifficult(rec)   ((rec).stop_reason = 0x15)
#define TG_SetStopAbsEntryError(rec)         ((rec).stop_reason = 0x16)
#define TG_SetMustStopTrain(rec)             ((rec).stop_reason = 0x17)
#define TG_SetCannotDepartEarly(rec)         ((rec).stop_reason = 0x18)
#define TG_SetSideUtilityConflict(rec)       ((rec).stop_reason = 0x19)
#define TG_SetAbsRunTimeConflict(rec)        ((rec).stop_reason = 0x1A)

#define TG_IsArriveRouteClear(rec)      (((rec).route_flag & 0x03) == 0x00)
#define TG_IsArriveRouteDoing(rec)      (((rec).route_flag & 0x03) == 0x01)
#define TG_IsArriveRouteOk(rec)         (((rec).route_flag & 0x03) == 0x02)
#define TG_IsArriveRouteOccupy(rec)     (((rec).route_flag & 0x03) == 0x03)
#define TG_IsDepartRouteClear(rec)      (((rec).route_flag & 0x0C) == 0x00)
#define TG_IsDepartRouteDoing(rec)      (((rec).route_flag & 0x0C) == 0x04)
#define TG_IsDepartRouteOk(rec)         (((rec).route_flag & 0x0C) == 0x08)
#define TG_IsDepartRouteOccupy(rec)     (((rec).route_flag & 0x0C) == 0x0C)
#define TG_IsRouteClear(rec)            (IsArriveRouteClear(rec) && IsDepartRouteClear(rec))
#define TG_IsTrainIntoSide(rec)         (((rec).route_flag & 0x10) == 0x10)

#define TG_SetRecordNormal(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x00)
#define TG_SetRecordChange(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x01)
#define TG_SetRecordInsert(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x02)
#define TG_SetRecordSendBack(rec)           ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x03)
#define TG_IsRecordNormal(rec)              (!((rec).db_flag & 0x03))
#define TG_IsRecordChange(rec)              (((rec).db_flag & 0x03) == 1)
#define TG_IsRecordInsert(rec)              (((rec).db_flag & 0x03) == 2)
#define TG_IsRecordSendBack(rec)            (((rec).db_flag & 0x03) == 3)

#define TG_SetRecordSaveIntoHo(rec)         ((rec).db_flag |= 0x04)
#define TG_SetRecordNotSaveIntoHo(rec)      ((rec).db_flag &= ~0x04)
#define TG_IsRecordSaveIntoHo(rec)          ((rec).db_flag & 0x04)

#define TG_SetSideFromBasicSchedule(rec)              ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x00)
#define TG_SetSideFromAdditionSchedule(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x01)
#define TG_SetSideFromStaticDefine(rec)               ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x02)
#define TG_SetSideFromDispatcher(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x03)
#define TG_SetSideFromAutoAdjust(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x04)
#define TG_SetSideFromStaWatcherChangePlan(rec)       ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x05)
#define TG_SetSideFromStaWatcherReportActual(rec)     ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x06)
#define TG_SetSideFromLircChangePlan(rec)             ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x07)
#define TG_SetSideFromLircReportActual(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x08)
#define TG_SetSideFromPathWay(rec)                    ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x09)

#define TG_IsSideFromBasicSchedule(rec)               (((rec).data_source & 0x0F) == 0)
#define TG_IsSideFromAdditionSchedule(rec)            (((rec).data_source & 0x0F) == 1)
#define TG_IsSideFromStaticDefine(rec)                (((rec).data_source & 0x0F) == 2)
#define TG_IsSideFromDispatcher(rec)                  (((rec).data_source & 0x0F) == 3)
#define TG_IsSideFromAutoAdjust(rec)                  (((rec).data_source & 0x0F) == 4)
#define TG_IsSideFromStaWatcherChangePlan(rec)        (((rec).data_source & 0x0F) == 5)
#define TG_IsSideFromStaWatcherReportActual(rec)      (((rec).data_source & 0x0F) == 6)
#define TG_IsSideFromLircChangePlan(rec)              (((rec).data_source & 0x0F) == 7)
#define TG_IsSideFromLircReportActual(rec)            (((rec).data_source & 0x0F) == 8)
#define TG_IsSideFromPathWay(rec)                     (((rec).data_source & 0x0F) == 9)

#define TG_SetRecTimeFromDefault(rec, ad)              ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define TG_SetRecTimeFromAutoAdjust(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define TG_SetRecTimeFromLircChangePlan(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define TG_SetRecTimeFromDispatcherTurnActual(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define TG_SetRecTimeFromStaWatcherChangePlan(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define TG_SetRecTimeFromStaWatcherReport(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define TG_SetRecTimeFromLircReportActual(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define TG_SetRecTimeFromLircReportMatch(rec, ad)      ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define TG_SetRecTimeFromSimpleAdjust(rec, ad)         ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define TG_IsRecTimeFromDefault(rec, ad)               (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define TG_IsRecTimeFromAutoAdjust(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define TG_IsRecTimeFromLircChangePlan(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define TG_IsRecTimeFromDispatcherTurnActual(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define TG_IsRecTimeFromStaWatcherChangePlan(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define TG_IsRecTimeFromStaWatcherReport(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define TG_IsRecTimeFromLircReportActual(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define TG_IsRecTimeFromLircReportMatch(rec, ad)       (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define TG_IsRecTimeFromSimpleAdjust(rec, ad)          (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define TG_SetPlanTimeFromBasicSchedule(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define TG_SetPlanTimeFromAdditionSchedule(rec, ad)    ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define TG_SetPlanTimeFromAutoCreate(rec, ad)          ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define TG_SetPlanTimeFromManualSet(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

#define TG_IsPlanTimeFromBasicSchedule(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define TG_IsPlanTimeFromAdditionSchedule(rec, ad)     (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define TG_IsPlanTimeFromAutoCreate(rec, ad)           (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define TG_IsPlanTimeFromManualSet(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

#define TG_IsSideConfirmed(rec)     ((rec).data_source & 0x80000000)
#define TG_SetSideConfirmed(rec)       ((rec).data_source |= 0x80000000)
#define TG_ClearSideConfirmed(rec)     ((rec).data_source &= ~0x80000000)

#define TG_SetTrainFromBasicSchedule(coninfo)            ((coninfo).train_source |= 0)
#define TG_SetTrainFromAdditionSchedule(coninfo)         ((coninfo).train_source |= 1)
#define TG_SetTrainFromReportTrue(coninfo)               ((coninfo).train_source |= 2)
#define TG_SetTrainFromReportFake(coninfo)               ((coninfo).train_source |= 3)
#define TG_SetTrainFromFreeDraw(coninfo)                 ((coninfo).train_source |= 4)
#define TG_SetTrainFromPathWay(coninfo)                  ((coninfo).train_source |= 5)
#define TG_SetTrainInfoFromShift(coninfo)                ((coninfo).train_source |= 6)
#define TG_SetTrainFromInformalCreate(coninfo)           ((coninfo).train_source |= 7)

#define TG_IsBasicScheduleTrain(coninfo)                 (0 == (0xffff&&(coninfo).train_source))
#define TG_IsAdditionScheduleTrain(coninfo)              (1 == (0xffff&&(coninfo).train_source))
#define TG_IsReportTrueTrain(coninfo)                    (2 == (0xffff&&(coninfo).train_source))
#define TG_IsReportFakeTrain(coninfo)                    (3 == (0xffff&&(coninfo).train_source))
#define TG_IsFreeDrawTrain(coninfo)                      (4 == (0xffff&&(coninfo).train_source))
#define TG_IsPathWayCreateTrain(coninfo)                 (5 == (0xffff&&(coninfo).train_source))
#define TG_IsProbeCreateTrain(coninfo)                   (6 == (0xffff&&(coninfo).train_source))
#define TG_IsInformalCreateTrain(coninfo)                (7 == (0xffff&&(coninfo).train_source))

#define TG_IsFreightTrainType(type)           ((type) & 0x80)
#define TG_IsPassengerTrainType(type)         (!((type) & 0x80))
#define TG_IsFreightTrain(coninfo)            (TG_IsFreightTrainType((coninfo)->type))
#define TG_IsPassengerTrain(coninfo)          (TG_IsPassengerTrainType((coninfo)->type))
#define TG_IsNullTrainType(type)     (type==255)   // type ==0 ���ؿ�!

#define TG_GetAdjustBeginSta(conptr)             ((conptr)->adjust_begin_sta & ~0x0080)
#define TG_IfAdjustBeginStaManualSet(conptr)     (!((conptr)->adjust_begin_sta & 0x0080))
#define TG_IfAdjustBeginStaAutoSet(conptr)       ((conptr)->adjust_begin_sta & 0x0080)
#define TG_SetAdjustBeginStaAutoSet(conptr)      ((conptr)->adjust_begin_sta |= 0x0080)
#define TG_GetAdjustEndSta(conptr)               ((conptr)->adjust_end_sta)

#define TG_SetConsoleTrainNotJoinAdjust(conptr)  ((conptr)->adjust_flag |= 0x01)
#define TG_SetConsoleTrainJoinAdjust(conptr)     ((conptr)->adjust_flag &= ~0x01)
#define TG_IsConsoleTrainNotJoinAdjust(conptr)   ((conptr)->adjust_flag & 0x01)
#define TG_IsConsoleTrainJoinAdjust(conptr)      (!((conptr)->adjust_flag & 0x01))

#define TG_SetConsoleTrainLink(conptr)          ((conptr)->adjust_flag |= 0x04)
#define TG_SetConsoleTrainNoLink(conptr)        ((conptr)->adjust_flag &= ~0x04)
#define TG_IsConsoleTrainLink(conptr)           ((conptr)->adjust_flag & 0x04)
#define TG_IsConsoleTrainNoLink(conptr)         (!((conptr)->adjust_flag & 0x04))

#define TG_ClearBorderTimeAuto(conptr)          ((conptr)->adjust_flag &= 0x3F)
#define TG_SetBorderTimeAuto(conptr)            ((conptr)->adjust_flag &= 0x3F, (conptr)->adjust_flag |= 0x40)
#define TG_SetBorderTimeManual(conptr)          ((conptr)->adjust_flag &= 0x3F, (conptr)->adjust_flag |= 0x80)
#define TG_IsBorderTimeNotSet(conptr)           (((conptr)->adjust_flag & 0xC0) == 0x00 ? TRUE : FALSE)
#define TG_IsBorderTimeAuto(conptr)             (((conptr)->adjust_flag & 0xC0) == 0x40 ? TRUE : FALSE)
#define TG_IsBorderTimeManual(conptr)           (((conptr)->adjust_flag & 0xC0) == 0x80 ? TRUE : FALSE)

#define TG_SetLocoPosMid(conptr)                ((conptr)->adjust_flag |= 0x200)    //bit 8

#define IsAbsStartStopUp(conptr)			(((conptr).adjust_flag&0x6000)==0x2000) // ����ʼ���г�
#define IsAbsStartStopDown(conptr)			(((conptr).adjust_flag&0x6000)==0x4000) // ����ʼ���г�

#define TG_SetConsoleTrainNotExistSubao(conptr)  ((conptr)->status &= 0xFFF8)   //bit 0-2
#define TG_IsConsoleTrainExistSubao(conptr)      ((conptr)->status & 0x0007) 

#define TG_SetTrainFromShift(status)  ( (status)|= 0x10)
#define TG_IsTrainFromShift(status) ( ((status)&0x10) == 0x10)
#define TG_ClearTrainFromShift(status)  ( (status)&= ~0x10)

#define TG_SetSubaoFromStation(conptr)           ((conptr)->status &= ~0x7,(conptr)->status |= 0x1)
#define TG_SetSubaoFromPreConsole(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x2)
#define TG_SetSubaoFromDispatcher(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x3)
#define TG_SetSubaoFromWireless(conptr)          ((conptr)->status &= ~0x7,(conptr)->status |= 0x4)
#define TG_IsSubaoFromStation(conptr)            (((conptr)->status & 0x7) == 0x1)
#define TG_IsSubaoFromPreConsole(conptr)         (((conptr)->status & 0x7) == 0x2)
#define TG_IsSubaoFromDispatcher(conptr)         (((conptr)->status & 0x7) == 0x3)
#define TG_IsSubaoFromWireless(conptr)           (((conptr)->status & 0x7) == 0x4)

#define SetConsoleTrainSecret(conptr)          ((conptr).status |= 0x8)
#define SetConsoleTrainNotSecret(conptr)       ((conptr).status &= ~0x8)
#define IsConsoleTrainSecret(conptr)           ((conptr).status & 0x8)

#define SetTrainFromShift(conptr)		((conptr).status |= 0x10)
#define ResetTrainFromShift(conptr)		((conptr).status &= ~0x10)
#define IsTrainFromShift(conptr)		(((conptr).status & 0x10) == 0x10)


#define IsConTrainUpDir(conptr)          ((conptr)->direction == 0)
#define IsConTrainDownDir(conptr)        ((conptr)->direction == 1)
#define IsTrainUpDir(dir)          		((dir) == 0)
#define IsTrainDownDir(dir)        		((dir) == 1)

#define TG_SetDieselEngineExt(enginetype)		    ((enginetype) &= 0xF0)                        //bit 0-3 
#define TG_SetElecEngineExt(enginetype)		    ((enginetype) &= 0xF0, (enginetype)|= 0x01) 
#define TG_IsDieselEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x00) 
#define TG_IsElecEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x01) 
#define TG_SetTractionEngineExt(enginetype)	    ((enginetype) &= 0xCF)                        //bit 4,5 
#define TG_SetAttachEngineExt(enginetype)		    ((enginetype) &= 0xCF, (enginetype)|= 0x10)
#define TG_IsTractionEngineExt(enginetype)	    (((enginetype) & 0x30) == 0x00) 
#define TG_IsAttachEngineExt(enginetype)		    (((enginetype) & 0x30) == 0x10) 
#define TG_SetEngineFromDispatcher(enginetype)    ((enginetype) &= 0x3F)                        //bit 6,7 
#define TG_SetEngineFromSubao(enginetype)         ((enginetype) &= 0x3F, (enginetype)|= 0x40) 
#define TG_IsEngineFromDispatcher(enginetype)     (((enginetype) & 0xC0) == 0x00) 
#define TG_TG_IsEngineFromSubao(enginetype)          (((enginetype) & 0xC0) == 0x40) 

#define TG_IsCheckNCWayStop(rec)          ((rec.flag & 0x03) == 0x01)
#define TG_IsCheckNCWayPass(rec)          ((rec.flag & 0x03) == 0x02)
#define TG_IsNoCheckNCWayStopPass(rec)    ((rec.flag & 0x03) == 0x00)
#define TG_IsCheckNCWayEntry(rec)         (rec.entry  != 0xFF)
#define TG_IsCheckNCWayExit(rec)          (rec.exit  != 0xFF)
#define TG_IsCheckNCWaySide(rec)          (rec.side  != 0xFF)

#define TG_IsCheckNCWayStart(recs)              (((recs).flag & 0x0003) == 0x0001)
#define TG_IsCheckNCWayAccept(recs)             (((recs).flag & 0x0003) == 0x0002)
#define TG_IsNoCheckNCWayStartAccept(recs)      (((recs).flag & 0x0003) == 0x0000)
#define TG_IsCheckNCWayTerminal(recs)           (((recs).flag & 0x000c) == 0x0001)
#define TG_IsCheckNCWayGiveout(recs)            (((recs).flag & 0x000c) == 0x0002)
#define TG_IsNoCheckNCWayTerminalGiveout(recs)  (((recs).flag & 0x000c) == 0x0000)
#define TG_IsCheckNCWayActual(recs)             (((recs).flag & 0x0030) == 0x0010)
#define TG_IsCheckNCWayPlan(recs)               (((recs).flag & 0x0030) == 0x0020)
#define TG_IsNoCheckNCWayActualPlan(recs)       (((recs).flag & 0x0030) == 0x0000)

#define TG_IsCheckNCWaySame(recs)               (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0040)
#define TG_ISCheckNCWayEmbed(recs)              (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0080)


#define TG_GetTrainRecordDir(rec)            ((rec).entry != 0xFF ? TrainDirection((rec).station, (rec).entry, TRAIN_INTO_STATION) : TrainDirection((rec).station, (rec).exit, TRAIN_OUT_STATION))

#define TG_GetValidADTime(rec)           ((!(rec).arrive || NULL_TIME == (rec).arrive) ? (rec).depart:(rec).arrive)
#define TG_GetValidDATime(rec)           ((!(rec).depart || NULL_TIME == (rec).depart) ? (rec).arrive:(rec).depart)
#define TG_SetArriveTimeOff(rec, toff)   ((rec).arrive += (NULL_TIME == (rec).arrive ? 0 : (int)(toff)))
#define TG_SetDepartTimeOff(rec, toff)   ((rec).depart += (NULL_TIME == (rec).depart ? 0 : (int)(toff)))
#define TG_SetTrainRecordTimeOff(rec, toff)   (SetArriveTimeOff(rec, toff), SetDepartTimeOff(rec, toff))

#define TG_GetLogoTrainId(coninfo)       (NULL == (coninfo) ? "NULL" : 0 == (coninfo)->train_id[0][0] ? "" : (coninfo)->train_id[0])
#define TG_CopyTrainId(new_t, old_t)     {strncpy((new_t), (old_t), sizeof(TRAIN_NO)-1); new_t[sizeof(TRAIN_NO)-1]=0;}

#define TG_IsInhibitStation(schdblock)                ((schdblock).position & 0x03)
#define TG_IsInhibitFirstStation(schdblock)           (((schdblock).position & 0x01) == 0x01)
#define TG_IsInhibitSecondStation(schdblock)          (((schdblock).position & 0x02) == 0x02)

#define TG_QUALIFY_TRAININDEX(tindex)    if (0 == (tindex) || (TRAIN_INDEX)-1 == (tindex)) (tindex) = NULL_TRAININDEX;
#define TG_QUALIFY_STATION(sta)          if (0 == (sta)    || (STATION_NO)-1  == (sta))    (sta)    = NULL_STATION; 
#define TG_QUALIFY_ENTRY(entry)          if (0 == (entry)  || (ENTRY)-1 == (entry))        (entry)  = NULL_ENTRY; 
#define TG_QUALIFY_SIDE(side)            if (0 == (side)   || (SIDE)-1  == (side))         (side)   = NULL_SIDE; 

// record �е�not_meet ����
#define TG_NOTMEET_CRH(x)          ((x & NOTMEET_CRH)==NOTMEET_CRH)
#define TG_NOTMEET_SEPECIAL_Z(x)   ((x & NOTMEET_SEPECIAL_Z)==NOTMEET_SEPECIAL_Z)
#define TG_NOTMEET_PASSENGER(x)    ((x & NOTMEET_PASSENGER)==NOTMEET_PASSENGER)
#define TG_NOTMEET_FREIGHT(x)      ((x & NOTMEET_FREIGHT)==NOTMEET_FREIGHT)
#define TG_NOTMEET_SPACE(x)          ((x & NOTMEET_SPACE)==NOTMEET_SPACE)

#define TG_NOTMEET_200OrUp(x)   ((x & NOTMEET_200_orUp)==NOTMEET_200_orUp)
#define TG_NOTMEET_120to200(x)    ((x & NOTMEET_120_200)==NOTMEET_120_200)
#define TG_NOTMEET_120Low(x)      ((x & NOTMEET_120_Low)==NOTMEET_120_Low)
#define TG_NOTMEET_All(x)      ((x & NOTMEET_ALL)==NOTMEET_ALL)

// shoud be same with gpc
#define TG_QUALIFY_TIME(ltime)           (ltime /60 *60)





struct TG_PassWayInfo{
	 short Pass_way_id;          //��·��
	 char  Pass_way_name[40];    //��·����
     short Console_id;           //����̨����
	 short Begin_sta;            //��ʼ��վ
	 short End_sta;              //��ֹ��վ
	 TG_PassWayInfo()
	 {
		 memset(this, 0, sizeof(TG_PassWayInfo));
	 }
};

//          2 : վ��Ϣ 
struct TG_PassWaySta
{
	 short Pass_way_id;          //��·����
	 short in_sta_idx;           //���վ���
	 short out_sta_idx;          //����վ���
	 short Entry;                //���
	 short Station;              //վ��
	 short Exit;                 //����
	 TG_PassWaySta()
	 {
		 memset(this, 0, sizeof(TG_PassWaySta));
	 }
};

struct TG_PassWayUtility{
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
	  TG_PassWayUtility()
	 {
		 memset(this, 0, sizeof(TG_PassWayUtility));
	 }
};

struct TGBasicTrainListRequest
{
	ENTITY_ID	gpc;
	short		con_id;
	STATION_NO	station;
	TIME		stime;
	TIME		etime;
	TIME		shift_begin_time;
	TIME		shift_end_time;	
	enum	INIT_TRAINTYPE {
		INIT_PASSENGER,
		INIT_FREIGHT,
		INIT_ALL_TYPE
	} train_type;
	enum	INIT_DIRECTION {
		INIT_UP,
		INIT_DOWN,
		INIT_ALL_DIR
	} direction;
	enum	INIT_START_ACCEPT {
		INIT_START,
		INIT_ACCEPT,
		INIT_ALL_SA
	} flag;
	enum	INIT_HAT_FLAG {
		INIT_NORMAL_TRAIN,
		INIT_HAT_TRAIN
	} hat_flag;
};

struct TGSelectedBasicTrain{
	TRAIN_NO train_id;
	TRAIN_INDEX train_index;
 	TIME  start_date;
	TIME  start_time;
	TIME  depart_date;
	TIME  depart_time;
	BOOL  exist;//add by cf after 9-4
	enum BASIC_TRAIN_LIST_EXIST {
		BT_EXIST,
		BT_NONE
	} exist_train;
	enum BASIC_TRAIN_LIST_DIR {
		BT_UP,
		BT_DOWN,
		BT_ALL_DIR
	}  dir;
	enum BASIC_TRAIN_LIST_TYPE {
		BT_PASSENGER,
		BT_FREIGHT,
		BT_ALL_TYPE
	} type;
	enum BASIC_TRAIN_LIST_HAT {
		BT_HAT_TRAIN,
		BT_NORMAL_TRAIN
	}  hat_train;
	enum BASIC_TRAIN_LIST_ADD {
		BT_NORMAL_SCHEDULE,
		BT_LONG_STOP,
		BT_LONG_INCREASE
	} add_train;
};

struct TGSelectedBasicMark{
	TRAIN_INDEX mark_index;
	BOOL  exist;//add by cf after 9-4
	BOOL  checked;
	int   mark_type;
	int   row_index;
};

struct TGMarkSelector{
	TIME  start_time;
	TIME  end_time;
	int   station;
};
#endif /** THE END **/


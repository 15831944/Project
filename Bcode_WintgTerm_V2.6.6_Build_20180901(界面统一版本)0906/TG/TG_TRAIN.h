//
#ifndef TG_TRAIN_H_FILE
#define TG_TRAIN_H_FILE

#include "BaseTrain.h"

#include "vector"
#include "map"

#define MINIMUMTIME	120
#define WHOLE_DAY_MINUTE	1440
#define WHOLE_DAY_SECOND	86400
#define NOTIME		0xffffffff
#define UP_MARK		1
#define DOWN_MARK	2


//adjust status
#define IsActualArrivalTime(flag)           ((flag & 0x0001) == 1)
#define IsActualDepartTime(flag)            ((flag & 0x0002) == 2)

#define SetActualArrivalTime(flag)           (flag = (flag | 0x0001) )
#define SetActualDepartTime(flag)            (flag = (flag | 0x0002) )
#define CancelActualArrivalTime(flag)           (flag = (flag & 0xFFFFfffe) )
#define CancelActualDepartTime(flag)            (flag = (flag & 0xFFFFfffd) )

#define CancelFlagStopSignal(flag)          (flag &= 0xFFFFff7f)
#define CancelFlagStopAbs(flag)             (flag &= 0xFFFFfeff)

#define CancelFlagRescue(flag)          (flag &= 0xFFFFF7FF)
#define CancelFlagRescued(flag)         (flag &= 0xFFFFefff)

#define CancelFlagUniteTrain(flag)          (flag &= 0xFFFFbfff)
#define CancelFlagTrainUnited(flag)         (flag &= 0xFFFF7fff)



//infer traintype
#define IsTrain(train_info)            (!(((train_info.type)>>7) &0x01))
#define IsTruck(train_info)            (((train_info.type)>>7) &0x01)
#define IsCrhTrain(train_info)         (((train_info.type)>=17 && (train_info.type)<=19) || (train_info.run_type)==5)

//add by cf after 11-7
#define IsRunFast(rec)                 (!(((rec->run_fast_slow_time)>>7) & 0x01))
#define GetFastMinute(rec)             ((rec->run_fast_slow_time & 0x7F))



#define   TRAIN_ARRIVE              1
#define   TRAIN_DEPART              2


//�����俪��վ


#define IsAbsDrawUp(stop_reason)                  (stop_reason & 0x04) 
#define IsAbsDrawBottom(stop_reason)              (stop_reason & 0x08)

#define SetAbsDrawUp(stop_reason)                 (stop_reason |= 0x04) 
#define SetAbsDrawBottom(stop_reason)             (stop_reason |= 0x08) 

#define ReSetAbsDraw(stop_reason)                 (stop_reason &= ~0x0C) 

#define SetDisplyTractionBottom(adjust_flag)  (adjust_flag |= 0x100)
#define SetDisplyTractionMiddle(adjust_flag)  (adjust_flag |= 0x200)
#define SetDisplyTractionTop(adjust_flag)     (adjust_flag |= 0x400)
#define SetDisplyTractionNone(adjust_flag)    (adjust_flag &= 0xfffff8ff)

#define IsDisplyTractionTop(adjust_flag)      (((adjust_flag)&0x700) == 0x400)
#define IsDisplyTractionMiddle(adjust_flag)   (((adjust_flag)&0x700) == 0x200)
#define IsDisplyTractionBottom(adjust_flag)   (((adjust_flag)&0x700) == 0x100)
#define IsDisplyTractionNone(adjust_flag)     (((adjust_flag)&0x700) == 0x000)

// �����ܹ�������������---������ҵ��
#define SetTrainFlag(adjust_flag)    (adjust_flag |= 0x800)
#define IsSetTrainFlag(adjust_flag)  (((adjust_flag)&0x800) == 0x800)

#define SetAbsPartRunTrain(adjust_flag)  (adjust_flag |= 0x1000)
#define IsAbsPartRunTrain(adjust_flag)	  ((adjust_flag&0x1000)==0x1000)

#define ReSetAbsStartStop(adjust_flag)    (adjust_flag &= ~0x6000)
#define SetAbsStartStopUp(adjust_flag)	  (adjust_flag &= ~0x6000, adjust_flag |= 0x2000)
#define SetAbsStartStopDown(adjust_flag)  (adjust_flag &= ~0x6000, adjust_flag |= 0x4000)
#define IsAbsStartStopUp(adjust_flag)	  ((adjust_flag&0x6000)==0x2000)
#define IsAbsStartStopDown(adjust_flag)	  ((adjust_flag&0x6000)==0x4000)

#define SetAbsAttainEngineReturn(adjust_flag)  (adjust_flag &= ~0x8000, adjust_flag |= 0x8000)
#define IsAbsAttainEngineReturn(adjust_flag)	  ((adjust_flag&0x8000)==0x8000)

//#define SetTrainUpdateByOtherGPC(train) ( ((train)->dwFlag) |= 0x1)
#define SetTrainDeleteByOtherGPC(train) ( ((train)->dwFlag) |= 0x2)
#define SetTrainShow(train)             ( ((train)->dwFlag) |= 0x4)
//#define IsTrainUpdateByOtherGPC(train)  ( (((train)->dwFlag)&0x1) == 0x1)
#define IsTraintDeleteByOtherGPC(train) ( (((train)->dwFlag)&0x2) == 0x2)
#define IsTrainShow(train)				( (((train)->dwFlag)&0x4) == 0x4)


#define SetAbsStopTime(abs_stop_time)  ((abs_stop_time)|0x8000)

#define TG_IsSpaceRank(rank)               ((rank) & 0x01) //���ޱ�־---���޵ȼ�������bit16-bit21
#define TG_IsSpecialGaurdRank(rank)   ((rank) & 0x02)
#define TG_IsGoustRank(rank)               ((rank) & 0x04)
#define TG_IsMilitaryRank(rank)            ((rank) & 0x08)
#define TG_IsSpace(rank)							( ((rank) & 0x3F0000)>>16)

#define TG_GetArriveSpace(rank)              ( ((rank) & 0x070000)>>16) // �ӳ����޵ȼ�ֵ
#define TG_GetDepartSpace(rank)            ( ((rank) & 0x380000)>>19)  // �������޵ȼ�ֵ

// bit 4: 1-- ֱ�Ӵ��հ�ƻ��ӳ� 0-- ����
using namespace std;
struct ABS_STOP
{
	long flag;
	long position;
	long start_time;
	long stop_time;
	ABS_STOP()
	{
		flag = 0;
		position = 0;
		stop_time = 0;
		start_time = 0;
	}

	bool IsAbsStopCoreSame(const ABS_STOP& rhs);
};

class TgTRAIN_RECORD
{
public:
	int        rec_index; 
	
	int        opmsGiveStationText; // ����Ա����ֵ 0 OPMS����ֵ 1
	CString    szOpmsStationText;
	CString    szStationText;
	
	CString    szAbsText;
	CString    szAlarmText;
	CString    szPlanNote;
    	
	CString    arrive_train_id;
	CString    original_arrive_train_id;
	CString    depart_train_id;
	CString    original_depart_train_id;

    DWORD      special_train_index;
	
	STATION_NO station;      
	ENTRY_ID   entry;       
	ENTRY_ID   exit;       

	BYTE    arrive_side;         
	BYTE    plan_arrive_side;    
	BYTE    depart_side;       
	BYTE    plan_depart_side;   

	long	arrive; 
	long	depart;
	
	long	plan_arrive;
	long	plan_depart;
	
	long	route_flag;
	long    stop_reason;
							
	long    late_reason;
						// bit 0 �Ƿ�ͳ�Ƶ����
						// bit 1 �Ƿ�ͳ�Ƴ�����
	long    early_depart_time;   
	long    run_fast_slow_time;  

	long    speed_limit;
						// 0 ������, else: �������ֵ
	long    stop_condition;
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
	                    // bit 18: �Ƿ�����·��������

	short	min_stop_time;
	short   spec_run_time; // side_utility        
	long     super_interval_ltime;  
	short   stop_abs_position;// positon --- �����۷� 
	WORD    stop_abs_s_time;//special_time1 
	WORD    stop_abs_e_time;//special_time2

	long    data_source;
	long    flag;
	long    no_meet_reason;
     
	int     modify_abs_runtime;
	int   in_station_index;
	int   out_station_index;
	int     rank;
						// bit 0:  ����---0�������� 1������ ���޵ȼ�������bit16-bit21
						// bit 1:  �ص㾯��
						// bit 2:  ��Ʒ
						// bit 3:  ����
					    // bit 0-bit15���ֶκ�TMIS��Ӧ, ��������ʹ��

						// bit16-bit18: ���ﳬ�� 0:���� 1:��������  2:һ������ 3:�������� 4:��������
						// bit19-bit21: �������� 0:���� 1:��������  2:һ������ 3:�������� 4:��������
	DWORD   display_status;
	short    in_sta_direct;
	short    out_sta_direct;
	BYTE    static_arrive;
	BYTE    static_depart;
	BYTE    static_manual; // �˹�����ͳ��

	CString str_arrive_side;
	CString str_depart_side;
	
	vector<ABS_STOP> abs_stop;
	bool    bShowSecond;   // �Ƿ���ʾ��
	WORD    line_type;
	BYTE    factor;
	DWORD   schd_color, real_color, schd_vert_color, real_vert_color;
	BYTE    schd_width, real_width;
	BYTE    nEngineCnt;
public:
	TgTRAIN_RECORD();
	TgTRAIN_RECORD(const TgTRAIN_RECORD& rhs);
	TgTRAIN_RECORD& operator=(const TgTRAIN_RECORD& rhs);
	~TgTRAIN_RECORD();

public:
	bool  IfInTimeScope(TIME stime, TIME etime) const;
	bool  IsRecordCoreSame(const TgTRAIN_RECORD& rhs);
	CString GetStaAlarmText();
	void  InitRecordBasic();
	CString toXML();
	
	BYTE GetArriveSpace() const;
	BYTE GetDepartSpace() const ;
public:
	void Clear();
	
private:
	void Copy(const TgTRAIN_RECORD& rhs);
};

class TgTRAIN_MORE_INFO
{
public: 
	// ������Ϣ
	CString  train_name;  //���κ����

	TRAIN_KEY   train_key;
	TRAIN_INDEX basic_train_index;    
    CString shift_index_string;
	BYTE     type;   
	BYTE     run_type;

	long start_actual_time; 
	long start_plan_time;

	long sborder_plan_time;  /* converted from hh:mm:ss */
	long sborder_actual_time;  /* converted from hh:mm:ss */

	long eborder_plan_time;  /* converted from hh:mm:ss */
	long eborder_actual_time;  /* converted from hh:mm:ss */

	STATION_NO sborder_sta;
	STATION_NO eborder_sta;
	CString start_sta_name;
	CString end_sta_name;

	CONID from_conid;
	CONID to_conid;

	BYTE   priority;
	long   train_length;
	long   train_weight;
	long   status;  
			// 0x08: �����г�
			// 0x10: �հ�ƻ��г�
	        // 0x20: ǿ������

	// �����г���Ϣ
	BYTE    space_rank;    
	BYTE    length_rank; 
	BYTE    super_rank;  
	BYTE    military_rank;  

	// �г�������Ϣ
	long     engine_utility;    
	long     side_utility;  
	long     no_meet_flag;
	


	// �Զ�������Ϣ
	short  adjust_begin_sta;
	short  adjust_end_sta;        
	long   adjust_flag;  
							// bit 0: 0 - �����Զ�����   1 - �������Զ����� 
							// bit 1: 
							// bit 2: 0 - generate time_link    1 - not
							// bit 3: 0 - not in save before adjust   1 - do no use
							// bit 7-6:  0 - �ֽ�ڽ���ʱ��δ����
							//           1 - �ֽ�ڽ���ʱ���ֹ�δ����
							//           2 - �ֽ�ڽ���ʱ����ϵͳ�Զ�����
							// bit 10-8:  ��������ʾλ��  0-����ʾ   1-�� 2-��  4-��
							// bit 11:   �����ܹ����ݵ��г���־---������ҵ��
	                        // bit 12:   ����ֲ����� 1:��  0����
							// bit 13-14: ����ʼ���г� ͣ������ 01 ������ 10
							// bit 15: ����;���۷� 1���� 0:��
	                        // bit 16-31: ����ʼ���г� ��������ʱ��
	
	long   train_source;
		
	// add your code here
	BYTE whole_static;
	WORD stop_abs_time; 
	bool b_elec_rank;
	bool b_self_update;
public:
	TgTRAIN_MORE_INFO();
	TgTRAIN_MORE_INFO(const TgTRAIN_MORE_INFO& rhs);
	TgTRAIN_MORE_INFO& operator=(const TgTRAIN_MORE_INFO& rhs);
	~TgTRAIN_MORE_INFO();

public:
	bool  IsInfoCoreSame(const TgTRAIN_MORE_INFO& rhs);
	CString toXML();

public:
	void Clear();
private:
	void Copy(const TgTRAIN_MORE_INFO& rhs);
};

struct TgTRAIN_ENGINE_INFO
{
	CString     engine_id;            // ������
	CString     engine_name;          // ��������
	long        engine_status;        // bit 7-6:  from dispatcher/subao
	// bit 5-4:  0 - traction  1 - attach
	short       attach_sta;           // index of trainrecord
	short       unattach_sta;

public:
	TgTRAIN_ENGINE_INFO();
	TgTRAIN_ENGINE_INFO(const TgTRAIN_ENGINE_INFO& rhs);
	TgTRAIN_ENGINE_INFO& operator=(const TgTRAIN_ENGINE_INFO& rhs);
	~TgTRAIN_ENGINE_INFO();

public:
	bool IsEngineCoreSame(const TgTRAIN_ENGINE_INFO& rhs);
	void Copy(const TgTRAIN_ENGINE_INFO& rhs);
	void Clear();
	
public:
	CString toXML();

};

class CTG_TRAIN: public CBaseTrain
{
public:
	CTG_TRAIN();
	~CTG_TRAIN();
	CTG_TRAIN(const CTG_TRAIN& rhs);
	CTG_TRAIN& operator=(const CTG_TRAIN& rhs);
public:
	bool  IfInTimeScope(TIME stime, TIME etime) const;
	bool  IsTrainIDChange(const CTG_TRAIN& rhs);
	bool  IsTrainCoreSame(const CTG_TRAIN& rhs);
	ULONG GetTrainIndex();
	CString toXML(DWORD flag, WORD station, WORD rec_index, CString trainid, BYTE nRunType);
	
	bool GetLogicInfo(CString xml);
    CString toString();
	CString toPassway();
	CString toXMLForExitEntry();
	void AddPerPointInfo(WORD nRecIndex, WORD nFlag, const CPoint& point, int nSection);
	bool IsExistActualTime();
public:
	TRAIN_INDEX m_nTrainIndex;
	TRAIN_INDEX prev_train_index;
	TgTRAIN_MORE_INFO myTrainInfo;
	vector<TgTRAIN_RECORD> myRecord;
	vector<TgTRAIN_ENGINE_INFO> myEngine;

public:
	int AddRecord(TgTRAIN_RECORD& curr, int nType);
	TgTRAIN_RECORD* GetLastRecord();
	TgTRAIN_RECORD* GetFirstRecord();
	TgTRAIN_RECORD* GetRecordByPosition(int pos);
	TgTRAIN_RECORD* GetRecordByRecIndex(int uIndex);
	TgTRAIN_RECORD* GetStationRecord(int nStationNo);
	
	TgTRAIN_RECORD* GetPrevStationRecord(const TgTRAIN_RECORD* currec);
	TgTRAIN_RECORD* GetNextStationRecord(const TgTRAIN_RECORD* currec);

public:
	void ClearEngine();
	void AppendTailEngine(const TgTRAIN_ENGINE_INFO& eng);
	
	void DeleteHeadRecord();
	void DeleteTailRecord();
	void SetTrainType();
	void SetEleLocoTrain();
	int  GetTrainType(){ return m_nTrainType;}
	bool IsCHRTrain();
public:	
	bool bChangeTrainID;
	
	DWORD mOtherTrainIndex;
	CString GetTrainId();
	CString GetTractionIds();

	UINT GetRecordNum() const;
	UINT GetIndexOfRecord(int nStationNo);
	
	int  OffsetWorkScheduleTime(int nTimeOffset);
	int  OffsetWorkScheduleTime(int nRecordNo, int nTimeOffset, int nFlag);
	int  OffsetBasicScheduleTime(int nRecordNo,int nTimeOffset,bool bMoveWhole,int nFlag); // nFlag: 0 ����� 1 ����� 2 �������
	void UnionRect(CRect &rect) 
	{ 
		if(!rect.IsRectEmpty())
			m_train_rect.UnionRect(&m_train_rect, &rect);
	}
	void EmptyRect() { m_train_rect.SetRectEmpty();}
	CRect GetTrainRect(){ return m_train_rect;}
public:
	DWORD dwFlag;		// bit0: ���г��Ѿ�������GPC���� 1:�� 0:��
					    // bit1: ���г��Ѿ�������GPCɾ�� 1:�� 0:��
						// bit2: ���г�������ʾ,�������ɻ��ߣ��޷��ж�����Ϳͻ�ʱ
	
	bool  mbDoubleDirect; // true: ˫�� false: ����
	DWORD dwLogicFlag;  // �߼���־
	                    // bit0: 1:��ˢ�½���

public:
	void InitBasicTM();
	bool IsFlash(int& times);
	bool IsWholeTrainSideLock();
private:
	CRect m_train_rect; // �г��������־��εĺϲ�����
	int m_flashTimes;
	bool bWholeTrainSideLock;
};

#endif
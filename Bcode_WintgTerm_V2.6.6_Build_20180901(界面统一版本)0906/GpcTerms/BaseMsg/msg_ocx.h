#ifndef _CLBTH_MSGDEF_OCX_SCHD_H_
#define _CLBTH_MSGDEF_OCX_SCHD_H_

#include "msg_schd.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

class MsgTrainRecordOCX: public BaseMsg
{
	DECLARE_CLBTH_OCX(MsgTrainRecordOCX)
	int m_Version;

public:
	MsgTrainRecordOCX(int version = 0);
	virtual ~MsgTrainRecordOCX();

private:
	MsgTrainRecordOCX(const MsgTrainRecordOCX& rhs);                       
	MsgTrainRecordOCX& operator=(const MsgTrainRecordOCX& rhs); 

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short		in_station_index;
	short		out_station_index;

	short    opms_station_text_state;
	char    opms_station_text[256];
	char    station_text[256];
	char	abs_text[256];
	char	alarm_text[256];
	char    plan_note[256];

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

	I_TIME    arrive;                                   // ����ʱ��
	I_TIME    depart;                                   // ����ʱ��
	I_TIME    plan_arrive;                              // ͼ������ʱ��
	I_TIME    plan_depart;                              // ͼ������ʱ��
	I_TIME    send_plan_time;                           // �ƻ��´�ʱ��    

	ISDWORD    stop_reason;                             // bit 32-16������ӵ㣬�����ӵ㣬��deleteʱ��¼ԭrec_index
	ISDWORD    late_reason;                             //                 

	ISDWORD    early_depart_time;                       // ������ǰ����ʱ��
	ISDWORD    run_fast_slow_time;                      // �ϻ�����Ϣ
	
	ISDWORD    route_flag;                              // ��·����״̬   
	ISDWORD    rank;                                    // �ص�����Ϣ
	
	ISDWORD    speed_limit;                             // �����г�������Ϣ
	ISDWORD    stop_condition;                          //ͣ������
	
	ISDWORD    adjust_status;                           // ����״̬
	short   spec_run_time;                              // ָ������ʱ��

	ISDWORD    can_not_meet;                            // ���ᳵ����Ϣ
	ISDWORD    super_interval_ltime;                    // ָ���г����
	
	short         special_position;
	short         special_time_1;
	short         special_time_2;
	TRAIN_INDEX   special_train_index;

	ISDWORD    data_source;
	ISDWORD    flag;
    
	/*   ����ͣ��  */
	struct ABS_STOP
	{
		BYTE    flag;				// ����orվ��
		short   position;           // ͣ��λ��,���ͣ��ʱʹ��
		I_TIME  start_time;         // ͣ����ʼʱ��
		I_TIME  stop_time;          // ͣ������ʱ��
	};
	short      abs_stop_count;	// ͣ������
	ABS_STOP   abs_stop[8];		// ͣ����Ϣ
	ISDWORD    display_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecordOCX)

/* ȫ���г���Ϣ */
class MsgTrainOCX: public BaseMsg
{
	DECLARE_CLBTH_OCX(MsgTrainOCX)
	int m_Version;

public:
	MsgTrainOCX();
	virtual ~MsgTrainOCX();

private:
	MsgTrainOCX(const MsgTrainOCX& rhs);                       
	MsgTrainOCX& operator=(const MsgTrainOCX& rhs); 

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgTrainInfo        info;        //�г���Ϣ

	short               engine_count;//������
	MsgTrainEngineInfo* engine_ptr;  //������Ϣ

	short				rec_count;     //�г�������
	MsgTrainRecordOCX*   rec_ptr;       //�г�������¼


	TRAIN_INDEX train_index;
	TRAIN_INDEX prev_train_index;
	IBYTE		self_update;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOCX)

/* ������Ϣ */
class MsgSchdBlockOCX: public MsgSchdBlock
{
	DECLARE_CLBTH_OCX(MsgSchdBlockOCX)
	int m_Version;

public:
	MsgSchdBlockOCX(int version = 0);
	virtual ~MsgSchdBlockOCX();

private:
	MsgSchdBlockOCX(const MsgSchdBlockOCX& rhs);                       
	MsgSchdBlockOCX& operator=(const MsgSchdBlockOCX& rhs); 

public:
	IBYTE self_update;
	TRAIN_INDEX mark_index;
	USHORT station1;
	USHORT station2;
	IBYTE  direction;

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockOCX)

CLBTH_NAMESPACE_END

#endif

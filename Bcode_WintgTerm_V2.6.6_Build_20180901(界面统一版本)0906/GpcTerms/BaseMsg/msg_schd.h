#ifndef _CLBTH_MSGDEF_MSG_SCHD_H_
#define _CLBTH_MSGDEF_MSG_SCHD_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

// ���мƻ���Ϣ��
class AFX_EXT_CLASS MsgSchdStamp: public BaseMsg
{
    DECLARE_CLBTH(MsgSchdStamp)
	int m_Version;

public:
	MsgSchdStamp(int version = 0);
	virtual ~MsgSchdStamp();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	CONID          con_id;    //����̨��
	IBYTE          schd_type; //����������
	TRAIN_INDEX    index;     //����
	I_TIME         ltime;     //����ʱ��
	ENTITY_ID      operatr;   //������
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdStamp)

/* �г�������Ϣ */
class AFX_EXT_CLASS MsgTrainInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainInfo)
	int m_Version;

public:
	MsgTrainInfo(int version = 0);
	virtual ~MsgTrainInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:

	/*   ������Ϣ  */
	char          train_name[MAX_TRAIN_NAME_LEN];    //  ���κ����
	TRAIN_KEY     train_key;                         // IP���ٵ��ڲ�����     

	TRAIN_INDEX   gpc_index;           // GPC����������
	TRAIN_INDEX   basic_index;         // ���ݻ���ͼ����ʱ, ����ͼ������
	TRAIN_INDEX   shift_index;         // �����հ�ƻ�����ʱ,�հ�ƻ�������
	char          shift_string[32];    // OPMS4.0��ʾ
	I_TIME        update_time;         // ɾ��ʱ��
	short         delete_flag;         //  -1, ��Ч�� >=0 �ӳ�ɾ����¼����,����Ч���ݼ�¼����

	IBYTE         type;                // �г���ϸ���� (bit 7: 0 - passenger 1 -freight)
	IBYTE         run_type;            // �г����ԣ����ڼ�����������ʱ����Զ���·��ǰ����ʱ��

	I_TIME        start_plan_time;     // ʼ���ƻ�ʱ��
	I_TIME        start_actual_time;   // ʼ��ʵ��ʱ��
	I_TIME        sborder_plan_time;   // �ֽ�ڽ���ƻ�ʱ��
	I_TIME        sborder_actual_time; // �ֽ�ڽ���ʵ��ʱ��
	I_TIME        eborder_plan_time;   // �ֽ�ڽ����ƻ�ʱ��
	I_TIME        eborder_actual_time; // �ֽ�ڽ���ʵ��ʱ��
	STATION_NO    sborder_sta;         // �ֽ�ڽ���վ��
	STATION_NO    eborder_sta;         // �ֽ�ڽ���վ��
	char          start_sta_name[MAX_STA_NAME_LEN];  // ��ʼ��վ��
	char          end_sta_name[MAX_STA_NAME_LEN];    // �յ���վ��
	CONID         from_conid;          // �󷽵���̨��
	CONID         to_conid;            // ǰ������̨��

	IBYTE         priority;             // �г��ȼ�
	ISDWORD       train_length;         // ����
	ISDWORD       train_weight;         // ����
	ISDWORD       status;
	// bit 0-2:   0 - �������ٱ�,       1 - �ٱ����ڳ�վ�ϱ�
	//            2 - �ٱ�������̨����, 3 - �ٱ����ڵ���Ա�ֹ�
	//            4 - �ٱ��������߳��κ�
	// bit 3: 1 - �����г�
	// bit 4: 1-- ֱ�Ӵ��հ�ƻ��ӳ� 0-- ����
	// bit 16-31: ��������ֽڴ�� ����

	/*   �����г���Ϣ  */
	IBYTE    space_rank;               //����ȼ�,�糬��
	IBYTE    length_rank;              //���ȵȼ�
	IBYTE    super_rank;               //���޵ȼ�
	IBYTE    military_rank;            //���õȼ�

	/*   �г�������Ϣ  */
	ISDWORD     engine_utility;        //������;
	//    0: normal
	//    1: limited_time
	//    2: no locomotive
	ISDWORD     side_utility;          //�ɵ���;
	//    bit0:   1 :  ��Ҫ��վ̨
	/*   �Զ�������Ϣ  */
	short   adjust_begin_sta;          // �Զ�������ʼվ    bit 7: 1 set by default,  0 set manual
	short   adjust_end_sta;            // �Զ���������վ

	ISDWORD    adjust_flag;
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
	ISDWORD    train_source;           // ������Դ

	short   comment_len;               // ע�ͳ���
	char*   comment;                   // ע��

	HRCODE SetComment(const char* c);
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainInfo)

/*   �г�������Ϣ  */
class AFX_EXT_CLASS MsgTrainEngineInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainEngineInfo)
	int m_Version;

public:
	MsgTrainEngineInfo(int version = 0);
	virtual ~MsgTrainEngineInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	TRAIN_NO     engine_id;            // ������
	TRAIN_NO     engine_type;          // ������������

	ISDWORD         engine_status;
	// bit 7-6:  0 - ����Ա�ֹ����� /  1- �����ٱ����� / 2- ���߳�����Ϣ
	// bit 5-4:  0 - ǣ��  1 - ����
	// bit 3-0:  0 - ��ȼ  1 - ����

	short        attach_sta;           // �������ҳ�վ   index of trainrecord
	short        unattach_sta;         // ����˦�ҳ�վ
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainEngineInfo)

/*   �г�����ͣ����Ϣ  */
class AFX_EXT_CLASS MsgTrainAbsStop: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainAbsStop)
	int m_Version;

public:
	MsgTrainAbsStop(int version = 0);
	virtual ~MsgTrainAbsStop();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short   position;             // ����ͣ��λ��,���ͣ��ʱʹ��
	I_TIME    start_time;         // ����ͣ����ʼʱ��
	I_TIME    stop_time;          // ����ͣ������ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainAbsStop)

/* �г�������¼ */
class AFX_EXT_CLASS MsgTrainRecord: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainRecord)
	int m_Version;

public:
	MsgTrainRecord(int version = 0);
	virtual ~MsgTrainRecord();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	TRAIN_NO     arrive_train_id;                       // ���ﳵ��
	TRAIN_NO     original_arrive_train_id;              // ԭʼ���ﳵ�Σ���ñʱʹ��

	TRAIN_NO     depart_train_id;                       // ��������
	TRAIN_NO     original_depart_train_id;              // ԭʼ�������Σ���ñʱʹ��

	STATION_NO   station;                               // վ��
	ENTRY        entry;                                 // ��ں�
	ENTRY        exit;                                  // ���ں�

	SIDE     arrive_side;                               // ����ɵ�
	SIDE     plan_arrive_side;                          // ͼ������ɵ�
	SIDE     depart_side;                               // �����ɵ�
	SIDE     plan_depart_side;                          // ͼ�������ɵ�
	short    min_stop_time;                             // Ӫҵʱ��

	I_TIME    arrive;                                   // ����ʱ��
	I_TIME    depart;                                   // ����ʱ��
	I_TIME    plan_arrive;                              // ͼ������ʱ��
	I_TIME    plan_depart;                              // ͼ������ʱ��
	I_TIME    send_plan_time;                           // �ƻ��´�ʱ��    

	ISDWORD    stop_reason;                             // bit 32-16������ӵ㣬�����ӵ㣬��deleteʱ��¼ԭrec_index
	ISDWORD    late_reason;                             //                 

	ISDWORD    early_depart_time;                       // ������ǰ����ʱ��
	// 0x0000 -> 0x7FFF, ������ǰ����������
	// 0x8000 ������ǰ����������
	// 0x0000 ��������ǰ����
	ISDWORD    run_fast_slow_time;                      // �ϻ�����Ϣ
	//    bit 7:   0 - ��,   1 - ��
	//    bit 6-0:   �ϻ���ʱ�� (����)

	ISDWORD    route_flag;                              // ��·����״̬   
	//    bit 10:   00/01/10/11  �ӳ���·  no(clear)/doing/OK/OCCUPY
	//    bit 32:   00/01/10/11  ������·  no(clear)/doing/OK/OCCUPY
	//    bit 4: 1:���ѽ��ɵ�
	ISDWORD    rank;                                    // �ص�����Ϣ
	//    bit 0:  ����
	//    bit 1:  �ص㾯��
	//    bit 2:  ��Ʒ
	//    bit 3:  ����
	ISDWORD    speed_limit;                             // �����г�������Ϣ
	//     0: ������, else: �������ֵ

	ISDWORD    stop_condition;                          //ͣ������
	//    bit 1:    1-ԭ����Ϊͨ���г�(����)�����г�����ʱ�Զ���·
	//    bit 2:    1-�޼ƻ�����ͣ��
	//    bit 3:    1-����ʹ�������ɵ�
	//    bit 4:    1-���ò���ͨ��
	//    bit 5:    1-�����Զ�����ӳ���·
	//    bit 6:    1-�����Զ���������·
	//    bit 7:    0-�������������������·,  1-���ƻ�ʱ����������·
	//    bit 8:    1-��ֹͣ��
	//    bit 9:    1-����ͣ��

	ISDWORD    adjust_status;                           // ����״̬
	//     bit 0:  0 : ����ʱ��Ϊ�ƻ�
	//             1 : ����ʱ��Ϊʵ��
	//     bit 1:  0 : ����ʱ��Ϊ�ƻ�
	//             1 : ����ʱ��Ϊʵ��
	//     bit 2:  0 : ����
	//             1 : ���ڴ�����Ϣ
	//     bit 3:  0 : �����ɹ�
	//             1 : ����ʧ��
	//     bit 4:  1 : ����ͣ��
	//     bit 5:  1 :
	//     bit 6:  0 : arrivetime is same with the nextcon's arr
	//             1 : not same(only for inter_sta)
	//     bit 7:  0 : departtime is same with the nextcon's dep
	//             1 : not same(only for inter_sta)
	//     bit 8 :   ����ʱ����
	//     bit 9 :   ����ʱ����
	//     bit 10:  0  ʵ�ʵ���ʱ��Ϊ�˹�����
	//              1  ʵ�ʵ���ʱ��Ϊ�Զ�����
	//     bit 11:  0  ʵ�ʳ���ʱ��Ϊ�˹�����
	//              1  ʵ�ʳ���ʱ��Ϊ�Զ�����
	//     bit 12:
	//     
	//     bit 13,14:  0  δ���͵���̨��̨��վ��
	//                 1  ��̨�ܾ�
	//                 2  ��̨����
	//     bit 15:  0  �޵���ǣ��
	//              1  ����ǣ��
	//     bit 16:  0 �ǽӿڳ�վ  1 �ֽ�ڳ�վ
    //     bit 17:  0 ��̨��վ 1 ̨��վ
	//     bit 18:  0 ���Զ��۷����� 1 �Զ��۷�����

	short   spec_run_time;                              // ָ������ʱ��

	ISDWORD    can_not_meet;                            // ���ᳵ����Ϣ
	//     bit 0: ���ᶯ����
	//     bit 1: ����ֱ��
	//     bit 2: ����ͳ�
	//     bit 3: �����г�
	//     bit 4: ����
	//     bit 5: ����
	// can_not_meet: ���ֽڣ� ����Ͻ����־ �ε��ֽ� ����½����־

	ISDWORD    super_interval_ltime;                    // ָ���г����
	//        3-0: 0 ʹ��Ĭ������, else: ����� (min)
	//        7-4: 0 ʹ��Ĭ������, else: �Ҳ��� (min)

	short         special_position;
	short         special_time_1;
	short         special_time_2;
	TRAIN_INDEX   special_train_index;

	ISDWORD    data_source;
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

	// bit 31:   1-��վȷ�Ϲɵ�

	ISDWORD    flag;
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

	/*   ����ͣ��  */
	MsgTrainAbsStop   stop_out_of_signal;               // ����ͣ��
	short             abs_stop_count;                   // ����ͣ������
	MsgTrainAbsStop   abs_stop[MAX_ABS_STOP_COUNT];     // ����ͣ��

	/*   �г�������Ϣ  0 - abs  1 - station  2 - alarm */
	char    text[3][MAX_TRAIN_TEXT_LENGTH];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecord)

/* ȫ���г���Ϣ */
class AFX_EXT_CLASS MsgTrain: public BaseMsg
{
	DECLARE_CLBTH(MsgTrain)
	int m_Version;

public:
	MsgTrain();
	virtual ~MsgTrain();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgTrainInfo        info;        //�г���Ϣ

	short               engine_count;//������
	MsgTrainEngineInfo* engine_ptr;  //������Ϣ

	short             rec_count;     //�г�������
	MsgTrainRecord*   rec_ptr;       //�г�������¼
};
DECLARE_SERIALIZE_OPERATOR(MsgTrain)

/* �����г���Ϣ */
class MsgTrainUpdate;
class AFX_EXT_CLASS MsgTrainAdd: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainAdd)

public:
	MsgTrainAdd();
	MsgTrainAdd(const MsgTrainUpdate & update);
	virtual ~MsgTrainAdd();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainAdd)

/* ɾ���г���Ϣ */
class AFX_EXT_CLASS MsgTrainDelete: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainDelete)
public:
	MsgTrainDelete();
	MsgTrainDelete(const MsgTrainUpdate & update);
	virtual ~MsgTrainDelete();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDelete)

/* ����ȫ���г���Ϣ */
class AFX_EXT_CLASS MsgTrainUpdate: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainUpdate)
public:
	MsgTrainUpdate();
	MsgTrainUpdate(const MsgTrainAdd& add);
	virtual ~MsgTrainUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdate)
/* ������̨�г���Ϣ,��Ӧ��̨���� */
class AFX_EXT_CLASS MsgNeibTrainUpdate: public MsgTrain
{
	DECLARE_CLBTH(MsgNeibTrainUpdate)
public:
	MsgNeibTrainUpdate();
	virtual ~MsgNeibTrainUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//����ʵ��
	ENTITY_ID   recv_entity;//����ʵ��

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//��̨��Ϣ��ʼʱ��
	I_TIME        end_time;//��̨��Ϣ����ʱ��
	I_TIME        send_time;  //����ʱ��
	USERNAME    sender_name;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgNeibTrainUpdate)

/* �����г���Ϣ */
class AFX_EXT_CLASS MsgTrainData: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainData)

public:
	MsgTrainData();
	virtual ~MsgTrainData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp  stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainData)

/* �����г�������Ϣ */
class AFX_EXT_CLASS MsgTrainUpdateInfo: public MsgTrainInfo
{
	DECLARE_CLBTH(MsgTrainUpdateInfo)
public:
	MsgTrainUpdateInfo();
	virtual ~MsgTrainUpdateInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateInfo)

/* �����г���������Ϣ */
class AFX_EXT_CLASS MsgTrainUpdateEngine: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainUpdateEngine)
public:
	MsgTrainUpdateEngine();
	virtual ~MsgTrainUpdateEngine();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short               engine_count;//������
	MsgTrainEngineInfo* engine_ptr;//������Ϣ

	MsgSchdStamp stamp;

	HRCODE SetEngine(short count, const MsgTrainEngineInfo* engine);
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateEngine)

/* �����г���������Ϣ */
class AFX_EXT_CLASS MsgTrainUpdateRecord: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainUpdateRecord)
public:
	MsgTrainUpdateRecord();
	virtual ~MsgTrainUpdateRecord();
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	short            rec_cnt;//
	MsgTrainRecord   *rec_ptr;//�г�������¼
	short            *rec_index;   // ��¼�±�

	MsgSchdStamp     stamp;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateRecord)

/* ������Ϣ */
class AFX_EXT_CLASS MsgSchdBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdBlock)
	int m_Version;

public:
	MsgSchdBlock(int version = 0);
	virtual ~MsgSchdBlock();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	TRAIN_INDEX   gpc_index;         //gpc����������
	TRAIN_INDEX   basic_index;       //���ݻ���ͼ����������
	TRAIN_INDEX   shift_index;       //�����հ�ƻ�����������
	char          shift_string[32];  // OPMS4.0��ʾ
	I_TIME        update_time;       // ɾ��ʱ��
	short         delete_flag;       //  -1, ��Ч�� >=0 �ӳ�ɾ����¼����

	I_TIME        start_time;        //������ʼʱ��
	I_TIME        end_time;          //��������ʱ��

	IBYTE         type;           //  ABS SLOW,      ABS_INHIBIT,  ELECTRO_FIX,  SIDE_SLOW,    SIDE_INHIBIT,  GRAPH_NOTE, ABS_ELEC_FIX, REGIST_BLOCK_TYPE
	IBYTE         flag;           //  limit_speed    flag          0             limit_speed   type           0,                        block_type(0-half,1-auto,2-tel)
	IWORD         dir;            // used by gpc
	STATION_NO    station;        //  0              0             0             station       station        top_position
	IWORD         index;          // used by gpc
	IWORD         position;       //  limit_length   position      position      limit_length  position       bottom_position
	//     bit 0:  1 -- inhibit first sta, 0 -- not;  bit 1: 1 -- inhibit second sta, 0 -- not ;  when type=SECTION_ELEC_FIX
	IWORD     ele_count;                                 //  Ԫ�ظ���
	IWORD     ele_number[MAX_SCHD_BLOCK_ELE_COUNT];      //  Ԫ���б�

	short     text_len;
	char      text[MAX_SCHD_BLOCK_TEXT_LEN];
	IWORD     s_meter;   //first sta num ,when type = SECTION_ELEC_FIX
	IWORD     e_meter;   //second sta num ,when type = SECTION_ELEC_FIX
	IWORD     e_kilo;
	IWORD     slow_time;
	ISDWORD   adjust_status;   // bit 1-0:  0 �ƻ���1 ִ�С�2 ʵ��

	short     comment_len;    //ע�ͳ���
	char*     comment;        //ע��

	HRCODE SetComment(const char* c);
	void Set(const MsgSchdBlock & msg);
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlock)

/* ���ӷ�����Ϣ */
class MsgSchdBlockUpdate;
class AFX_EXT_CLASS MsgSchdBlockAdd: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockAdd)
public:
	MsgSchdBlockAdd();
	MsgSchdBlockAdd(const MsgSchdBlockUpdate & msg);
	virtual ~MsgSchdBlockAdd();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockAdd)

/* ���ķ�����Ϣ */
class AFX_EXT_CLASS MsgSchdBlockUpdate: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockUpdate)
public:
	MsgSchdBlockUpdate();
	MsgSchdBlockUpdate(const MsgSchdBlockAdd & msg);
	virtual ~MsgSchdBlockUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockUpdate)

/* ɾ��������Ϣ */
class AFX_EXT_CLASS MsgSchdBlockDelete: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockDelete)
public:
	MsgSchdBlockDelete();
	virtual ~MsgSchdBlockDelete();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockDelete)

/* ���ط�����Ϣ */
class AFX_EXT_CLASS MsgSchdBlockData: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockData)
public:
	MsgSchdBlockData();
	virtual ~MsgSchdBlockData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp    stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockData)

// gpc��Ϣ���������
class AFX_EXT_CLASS MsgSchdMsgEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdMsgEcho)
	int m_Version;

public:
	MsgSchdMsgEcho(int version = 0);
	virtual ~MsgSchdMsgEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short msgid;//��Ϣ��
	ISDWORD  ecode;//������

	MsgSchdStamp stamp;

	void SetSchdStamp(const MsgSchdStamp& s);
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdMsgEcho)

// ����ͼ������Ϣ�ͷ���
class AFX_EXT_CLASS MsgConvertBasicSchd: public BaseMsg
{
	DECLARE_CLBTH(MsgConvertBasicSchd)
	int m_Version;

public:
	MsgConvertBasicSchd(int version = 0);
	virtual ~MsgConvertBasicSchd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID conid;//���κ�
	enum FLAG
	{
		CONVERT,
		RESTORE,
		ECHO_OK,
		ECHO_FAIL,
	}  flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgConvertBasicSchd)


class AFX_EXT_CLASS MsgSchdLoad: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdLoad)
	int m_Version;

public:
	MsgSchdLoad(int version = 0);
	virtual ~MsgSchdLoad();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	CONID         con_id;//����̨��
	IBYTE          schd_type; //����������

	I_TIME          start_time;//��ʼʱ��
	I_TIME          stop_time;//����ʱ��

	int           sta_count;//������վ��
	STATION_NO    station[128];//վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdLoad)

class AFX_EXT_CLASS MsgNoticeNeigbourReceiveSchd: public BaseMsg
{
	DECLARE_CLBTH(MsgNoticeNeigbourReceiveSchd)
	int m_Version;

public:
	MsgNoticeNeigbourReceiveSchd(int version = 0);
	virtual ~MsgNoticeNeigbourReceiveSchd();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	ISDWORD         send_id;//����ʵ���
	ISDWORD         receive_id;//����ʵ���
	IBYTE         schd_type;  //����������
	ISDWORD         nShiftId;//�����հ�ƻ����ɵ�id��
	int          hour;//
	ISDWORD         con_id;//����̨��
};
DECLARE_SERIALIZE_OPERATOR(MsgNoticeNeigbourReceiveSchd)

/* ����վ���� */
class AFX_EXT_CLASS MsgTrainRecordData: public MsgTrainUpdateRecord
{
	DECLARE_CLBTH(MsgTrainRecordData)
public:
	MsgTrainRecordData();
	virtual ~MsgTrainRecordData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgTrainInfo info;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecordData)

const int MAX_SEARCH_TRAIN_STA = 24;
/* �ظ������ж� */
class AFX_EXT_CLASS MsgCheckDupilicateTrainID: public BaseMsg
{
	DECLARE_CLBTH(MsgCheckDupilicateTrainID)
public:
	MsgCheckDupilicateTrainID();
	virtual ~MsgCheckDupilicateTrainID();
private:
protected:
	virtual size_t countObjectLength();
	virtual HRCODE doSerialize(Archive& ar);

public:
	TRAIN_NO  szTrainNo;//���κ�
	CONID   nCon_Id;  // ���Ϊ0�����ʾ��������̨����
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// ���Ϊ0�����ʾ��������վ���ݣ�Ԥ��
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];
};
DECLARE_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainID)

/* ���ظ������Ƽ� */
class AFX_EXT_CLASS MsgFindUniqeTrainID: public BaseMsg
{
	DECLARE_CLBTH(MsgFindUniqeTrainID)
public:
	MsgFindUniqeTrainID();
	virtual ~MsgFindUniqeTrainID();
private:
protected:
	virtual size_t countObjectLength();
	virtual HRCODE doSerialize(Archive& ar);

public:
	char    szPrefix[8];   // ����������ؿ죬��˴�Ϊ ��T��
	CONID   nCon_Id;  // ���Ϊ0�����ʾ��������̨����
	IDWORD    nTrainIDFrom;  // ���κ���ֵ������Сֵ
	IDWORD    nTrainIDTo;    // ���κ���ֵ�������ֵ
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// ���Ϊ0�����ʾ��������վ���ݣ�Ԥ��
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];
};
DECLARE_SERIALIZE_OPERATOR(MsgFindUniqeTrainID)

/* �ظ������жϽ�� */
class AFX_EXT_CLASS MsgCheckDupilicateTrainIDResult: public MsgCheckDupilicateTrainID
{
	DECLARE_CLBTH(MsgCheckDupilicateTrainIDResult)
public:
	MsgCheckDupilicateTrainIDResult();
	virtual ~MsgCheckDupilicateTrainIDResult();
	MsgCheckDupilicateTrainIDResult(const MsgCheckDupilicateTrainID & msg);
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_BOOL           bResult;  // �˴������Ƿ��ظ�
	I_TIME           tResultTime; // �˴������ϴ��ظ����ε�ʱ��
	CONID nResultConID;
};
DECLARE_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainIDResult)

/* ���ظ������Ƽ���� */
class AFX_EXT_CLASS MsgFindUniqeTrainIDResult: public MsgFindUniqeTrainID
{
	DECLARE_CLBTH(MsgFindUniqeTrainIDResult)
public:
	MsgFindUniqeTrainIDResult();
	virtual ~MsgFindUniqeTrainIDResult();
	MsgFindUniqeTrainIDResult(const MsgFindUniqeTrainID & msg);
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD          nResultTrainID;   // ��ָ����Χ�ڲ��ظ��ĳ�����ֵ������Ҳ�������Ϊ0
};
DECLARE_SERIALIZE_OPERATOR(MsgFindUniqeTrainID)

/* ��ѯ���κ��б� */
const int MAX_TRAIN_ID_BUFFER_LEN_PER_PACK=2048;
class AFX_EXT_CLASS MsgGetTrainIDList: public BaseMsg
{
	DECLARE_CLBTH(MsgGetTrainIDList)
public:
	MsgGetTrainIDList();
	virtual ~MsgGetTrainIDList();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   nCon_Id;  // ���Ϊ0�����ʾ��������̨����
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// ���Ϊ0�����ʾ��������վ���ݣ�Ԥ��
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];//վ��
	int     nTrainIDBufferLen;    //����ʱΪ0������ʱΪ�������
	char  szTrainNoBuffer[MAX_TRAIN_ID_BUFFER_LEN_PER_PACK];
};
DECLARE_SERIALIZE_OPERATOR(MsgGetTrainIDList)

// ����ͼת����Ϣ�ͷ���
class AFX_EXT_CLASS MsgConvertBasicSchdNew: public BaseMsg
{
	DECLARE_CLBTH(MsgConvertBasicSchdNew)

public:
	MsgConvertBasicSchdNew(int version = 0);
	virtual ~MsgConvertBasicSchdNew();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int m_Version;
	CONID conid;
	SCHEDULE_ID from_schd_type, to_schd_type;
	enum FLAG
	{
		CONVERT,
		ECHO_OK,
		ECHO_FAIL,
	}  flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgConvertBasicSchdNew)

// ���ݳ��κŷ��ض�̨������
class AFX_EXT_CLASS MsgQueryTrainByID: public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainByID)

public:
	MsgQueryTrainByID();
	virtual ~MsgQueryTrainByID();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_NO  szTrainNo;//���κ�
	I_TIME    tBeginTm;//
	I_TIME    tEndTm;//
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainByID)
/*
class AFX_EXT_CLASS MsgBasicScheduleInfoUpdate: public BaseMsg
{
DECLARE_CLBTH(MsgBasicScheduleInfoUpdate)
int m_Version;

public:
MsgBasicScheduleInfoUpdate(int version = 0);
virtual ~MsgBasicScheduleInfoUpdate();

virtual HRCODE doSerialize(Archive& ar);
virtual size_t countObjectLength();

public:
CONID       con_id;             // ���κ�
short       flag;               // 1: ����Ԥ��ͼ�汾��2�����»���ͼ�汾��3�����ڸ���Ԥ��ͼ��4�����ڸ��»���ͼ
char        ver_info[256];      // �汾��Ϣ
I_TIME        set_time;           // ����ʱ��
USERNAME    set_name;           // ��������

};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoUpdate)

class AFX_EXT_CLASS MsgBasicScheduleInfoInform : public MsgBasicScheduleInfoUpdate
{
DECLARE_CLBTH(MsgBasicScheduleInfoInform)
int m_Version;

public:
MsgBasicScheduleInfoInform(int version = 0);
virtual ~MsgBasicScheduleInfoInform();

};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoInform)

class AFX_EXT_CLASS MsgBasicScheduleInfoQuery: public BaseMsg
{
DECLARE_CLBTH(MsgBasicScheduleInfoQuery)
int m_Version;

public:
MsgBasicScheduleInfoQuery(int version = 0);
virtual ~MsgBasicScheduleInfoQuery();

virtual HRCODE doSerialize(Archive& ar);
virtual size_t countObjectLength();

public:
CONID   con_id;            // ���κ�
short   flag;               // 1: ����Ԥ��ͼ�汾��2�����»���ͼ�汾
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoQuery)
*/

// �г������߿͵�����
class AFX_EXT_CLASS MsgTrainKediaoInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainKediaoInfo)
public:
	MsgTrainKediaoInfo();
	virtual ~MsgTrainKediaoInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short	type;			// ���� 1:�͵� 2���ص�

	short   text_len;       // ��Ϣ����
	char*   text;           // ��Ϣ����, ��󳤶�Ϊ MAX_KEDIAO_LENGTH
	
	HRCODE SetKediao(const char* c);

};
DECLARE_SERIALIZE_OPERATOR(MsgTrainKediaoInfo)

// �г�������Ӫҵ����
class AFX_EXT_CLASS MsgTrainYingyeInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainYingyeInfo)
public:
	MsgTrainYingyeInfo();
	virtual ~MsgTrainYingyeInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO	station_no;				// վ��
	I_TIME		arrive_time;			// ����ʱ��
	I_TIME		depart_time;			// ����ʱ��
	STAT_NAME	station_name;			// վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainYingyeInfo)

// �г������߳�������
class AFX_EXT_CLASS MsgTrainCrewInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainCrewInfo)
public:
	MsgTrainCrewInfo();
	virtual ~MsgTrainCrewInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	short	crew_type;			//0:δ֪ 1��˾��	2����еʦ	3���˾�	4������	5������
	char	crew_info[64];		//�������绰
	char	crew_bureau[32];	//����·��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainCrewInfo)

// �г������߻�������
class AFX_EXT_CLASS MsgEngineCrewInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgEngineCrewInfo)
public:
	MsgEngineCrewInfo();
	virtual ~MsgEngineCrewInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO	station_no;				// վ��
	STAT_NAME	station_name;			// վ��
	I_TIME		overwork_time;			// ����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgEngineCrewInfo)

// �г������߿�Ʊ����
class AFX_EXT_CLASS MsgTrainPassgenerTicketInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainPassgenerTicketInfo)
public:
	MsgTrainPassgenerTicketInfo();
	virtual ~MsgTrainPassgenerTicketInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	short		on_station_index;		// �ϳ�վ��
	STATION_NO	on_station;				// �ϳ�վ��
	short		down_station_index;		// �³�վ��
	STATION_NO  down_station;			// �³�վ��
	int			sale;					// ��������
	char		seat[32];				// ϯ��
	STAT_NAME	on_station_name;		// �ϳ�վ��
	STAT_NAME	down_station_name;		// �³�վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainPassgenerTicketInfo)

// �г�������OPMS��Ϣ
class AFX_EXT_CLASS MsgTrainOPMSInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainOPMSInfo)
public:
	MsgTrainOPMSInfo();
	virtual ~MsgTrainOPMSInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO	station_no;			// ��վվ��
	IDWORD	    work_flag;			// ��վ��ҵ
	IDWORD      oper_flag;			// ��������
	IDWORD      important_flag;		// ��Ҫ����

	char	    oper_info[256];
	char        note_text[256];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOPMSInfo)

// �г������߶������ԣ�Ӫҵ�����񡢿͵�����Ʊ������
class AFX_EXT_CLASS MsgTrainExpand: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainExpand)

public:
	MsgTrainExpand();
	virtual ~MsgTrainExpand();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp	stamp;
	int				m_Version;			//�汾
	char			chezu_no[32];		// �����
	TRAIN_INDEX		prev_shift_index;	//ǰ�˳�ȫ��ID	 
	TRAIN_INDEX		next_shift_index;	//ֱͨID	 
	TRAIN_INDEX		prev_train_index;	//ǰ�˳�
	TRAIN_INDEX		prev_basic_index;	//ǰ�˳�����ͼID
	STATION_NO		prev_dest_sta;		//ǰһ������վ 
	STATION_NO		next_dest_sta;		//��һ������վ 
	IDWORD			halt_state;         //ͣ�˱�־
	MsgTrainKediaoInfo*        kd_ptr;        //�͵���Ϣ

	int		yingye_count;
	MsgTrainYingyeInfo*		  yy_ptr;		  //Ӫҵ��Ϣ

	int		crew_count;					
	MsgTrainCrewInfo*		crew_ptr;		  //������Ϣ
	
	int		engine_crew_count;					
	MsgEngineCrewInfo*	  engine_crew_ptr;		  //������Ϣ	

	int		ticket_count;
	MsgTrainPassgenerTicketInfo*	  ticket_ptr;		  //��Ʊ��Ϣ

	int opms_count;
	MsgTrainOPMSInfo* opms_ptr;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainExpand)


// �г�����������������Ϣ
#define MsgTrainCmdInfo_Version 0
class AFX_EXT_CLASS MsgTrainCmdInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainCmdInfo)
public:
	MsgTrainCmdInfo(int version = 0);
	virtual ~MsgTrainCmdInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int			m_Version;			//�汾
	STATION_NO	station_no			;	// ��վվ��
	int		    rec_index			;	// ��վ���
	ENTITY_ID	send_entity;
	TIME		create_time;
	int			sign_status;	//0: not sent	1:sent	2:locomotive received	3:signed
	TIME		sign_time	;
	char		dcmd_id[64];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainCmdInfo)

// �г������߸������ԣ��������
#define MsgTrainExtent_Version 0
class AFX_EXT_CLASS MsgTrainExtent: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainExtent)

public:
	MsgTrainExtent(int version = MsgTrainExtent_Version);
	virtual ~MsgTrainExtent();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp	stamp;
	int				m_Version;			//�汾
	
	char			chezu_no[32];		// �����
	TRAIN_INDEX		prev_shift_index;	//ǰ�˳�ȫ��ID	 
	TRAIN_INDEX		next_shift_index;	//ֱͨID	 
	TRAIN_INDEX		prev_train_index;	//ǰ�˳�
	TRAIN_INDEX		prev_basic_index;	//ǰ�˳�����ͼID
	STATION_NO		prev_dest_sta;		//ǰһ������վ 
	STATION_NO		next_dest_sta;		//��һ������վ 
	IDWORD			halt_state;         //ͣ�˱�־
	MsgTrainKediaoInfo*        kd_ptr;        //�͵���Ϣ

	int		yingye_count;
	MsgTrainYingyeInfo*		  yy_ptr;		  //Ӫҵ��Ϣ

	int		crew_count;					
	MsgTrainCrewInfo*		crew_ptr;		  //������Ϣ
	
	int		engine_crew_count;					
	MsgEngineCrewInfo*	  engine_crew_ptr;		  //������Ϣ	

	int		ticket_count;
	MsgTrainPassgenerTicketInfo*	  ticket_ptr;		  //��Ʊ��Ϣ

	int opms_count;
	MsgTrainOPMSInfo* opms_ptr;

	int		cmd_count;
	MsgTrainCmdInfo*	  cmd_ptr;		  //����������Ϣ
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainExtent)

CLBTH_NAMESPACE_END

#endif

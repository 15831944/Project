#ifndef _CLBTH_MSGDEF_MSG_TCC_H_
#define _CLBTH_MSGDEF_MSG_TCC_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

//GSM-R��Ϣ����

// �п�������Ϣ���忪ʼ
// ��ʱ���������´�
class AFX_EXT_CLASS MsgStccCmdSend: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCmdSend)
		
public:
	MsgStccCmdSend();
	virtual ~MsgStccCmdSend();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum CMD_TYPE
	{
		SPEED_LIMIT = 0,	// ��ʱ��������
		SPEED_LIMIT_CANCEL,	// ȡ�� ��ʱ��������
		STATUS_REQUST,		// ��ʱ����״̬����
		CMD_REQUEST			// ��ʱ����״̬����
	};

	ENTITY_ID source;	// �����豸
	IBYTE cmd_type;	// ��������
	IDWORD sys_id;		// ctc�ڲ���, ����ȡ����������
	IWORD cmd_no;		// �����(0~9999)
	ISDWORD start_tm;		// ��ʼʱ��(��)
	ISDWORD end_tm;		// ����ʱ��(��)
	IBYTE line_no;		// ��·��(����涨)
	ISDWORD start_pos;		// ��ʼ�����(��)
	ISDWORD end_pos;		// ���������(��)
	IBYTE speed;			// ����ֵ(KM/H)
	STATION_NO start_sta_no; //��ʼվ��
	STATION_NO end_sta_no;	//����վ��
	IBYTE confirm;		// ȷ�ϴ���
	
	IBYTE packet_sum;	// �������������(1~100)
	IBYTE packet_no;		// ��ǰ����(1~100)
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCmdSend)

// ��ʱ��������ִ�н��
class AFX_EXT_CLASS MsgStccCmdStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCmdStatus)
		
public:
	MsgStccCmdStatus();
	virtual ~MsgStccCmdStatus();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum STATUS_TYPE
	{
		// ����ͻ�ִ
		LIRC_RECEIVED		= 0x01,	// ���ɻ��յ�
		LIRC_SEND_FAIL		= 0x02,	// ���ɻ�����ʧ��(Э��֡����)
		STCC_RECEIVED		= 0x07,	// �п��յ�(Э��֡����)
		STCC_ACCEPT			= 0x08,	// �޴���

		// ����״̬
		STCC_WAITFOR_EXEC	= 0x09,	// �Ŷӣ��ȴ�ִ��
		STCC_EXECUTING		= 0x0a,	// ����ִ��
		STCC_ZOMBIE			= 0x0b,	// ִ�н���(����ʱ�䵽)
		STCC_CANCELED		= 0x0c,	// ��ȡ��(�˹�����������)

		// ������ȷ��״̬
		STCC_CONFIRM_OK         = 0x0010,	// ��ȷ��
		STCC_CONFIRM_CANCEL     = 0x0011,	// ��ȡ��
		STCC_CONFIRM_TIMEOUT    = 0x0012,	// ��ʱ��δȷ��

		// ��Ч�Լ���������
		ERR_START_KM		= 0x20,	// ��ʼ�����Ƿ�
		ERR_END_KM			= 0x21,	// ���������Ƿ�
		ERR_ALL_KM			= 0x22,	// ��ʼ�����������Ƿ�
		ERR_SPEED			= 0x23,	// ����ֵ�Ƿ�
		ERR_REGION_LONG		= 0x24,	// ��������̫��
		ERR_REGION_EXIST	= 0x25,	// ���������Ѵ���
		ERR_REGION_INVALID	= 0x26,	// ��������Ƿ�
		ERR_DUP_CMDNO		= 0x27,	// ������ظ�
		ERR_TIME_FAIL		= 0x28,	// ʱ��У��ʧ��
		ERR_UNKNOWN			= 0xff,	// δ֪����
	};

	enum ACTION_TYPE {
		COMMAND_REPLY = 0,	// ������Ļ�ִ
		STCC_REPORT,	// �����б�
	};

	IBYTE action_type;
	IBYTE status_type;	// ״̬����

	STATION_NO sta_no;	// վ��
	IDWORD sys_id;		// ctc�ڲ���, ����ȡ����������
	IWORD cmd_no;		// �����(0~9999)
	ISDWORD start_tm;		// ��ʼʱ��(��)
	ISDWORD end_tm;		// ����ʱ��(��)
	IBYTE line_no;		// ��·��(����涨)
	ISDWORD start_pos;		// ��ʼ�����(��)
	ISDWORD end_pos;		// ���������(��)
	IWORD speed;			// ����ֵ(KM/H)

	IBYTE packet_sum;	// �������������(1~100)
	IBYTE packet_no;		// ��ǰ����(1~100)
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCmdStatus)

// �п�����״̬
class AFX_EXT_CLASS MsgStccReport: public BaseMsg
{
	DECLARE_CLBTH(MsgStccReport)
		
public:
	MsgStccReport();
	virtual ~MsgStccReport();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO sta_no;		// վ��
	IBYTE comm_active;		// �п�ͨ��״̬	(0=��;1=����;2=˫��)
	IBYTE stcc_master[2];		// �п�����״̬
	IBYTE ilock_channel[2];	// ����ͨ��״̬
	IBYTE ctc_channel[2];		// CTCͨ��״̬
	IBYTE leu_channel[4];	// LEU ͨ��״̬
	IBYTE leu1_ans[4];	// LEU1 ��Ӧ����״̬
	IBYTE leu2_ans[4];	// LEU2 ��Ӧ����״̬
	IBYTE leu3_ans[4];	// LEU3 ��Ӧ����״̬
	IBYTE leu4_ans[4];	// LEU4 ��Ӧ����״̬
	IBYTE init_section_num;	// ��ʼ���������(���Ϊ8)
	IBYTE init_line_no[8];		// ����������·��
	IBYTE init_direction[8]; 	// ��Ӧ����·����
};
DECLARE_SERIALIZE_OPERATOR(MsgStccReport)

// ����������STPC, GPC��ȷ���б�
class AFX_EXT_CLASS MsgTccCommand: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommand)
public:
	MsgTccCommand();
	virtual ~MsgTccCommand();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IBYTE    raw_sysid[16];	// ԭʼ����ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	IDWORD   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	I_TIME    time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ
	I_BOOL    bCancelLimit;	// �Ƿ� ȡ����������

	ENTITY_ID  creator;		// ԭʼ���������
	ENTITY_ID  checkor;		// ����У����
	ENTITY_ID  confirmor;	// ȷ�Ϸֽ���

	IWORD       alarm_num;   // ���ı�������
	ENTITY_ID  alarm[4];    // ���Ľ��ܱ�����

	IWORD    tcc_num;		// ����վ����
	IWORD    tcc_no[8];		// ����վ���пأ�
	IWORD    tcc_line_no[8]; // ����վ��·
	IWORD    tcc_status[8];	// ����վ����״̬
	I_TIME    tcc_time[8];	// ����վ������Чʱ��
	ENTITY_ID  tcc_checkor[8]; // ����վУ����
	ENTITY_ID  tcc_sender[8];	// ����վ�����ˣ���վ���������е���

	IWORD       tcc_alarm_num[8];   // ��վ��������
	ENTITY_ID  tcc_alarm[8][4];    // ��վ���ܱ�����

	IDWORD   nMarkIndex;		// ����ͼ�������б�־
	I_TIME    create_time;    // ����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommand)

// ����������
class AFX_EXT_CLASS MsgTccCommandAdd: public MsgTccCommand
{
	DECLARE_CLBTH(MsgTccCommandAdd)
public:
	MsgTccCommandAdd();
	virtual ~MsgTccCommandAdd();
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandAdd)

class AFX_EXT_CLASS MsgTccCommandDel: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandDel)
public:
	MsgTccCommandDel();
	virtual ~MsgTccCommandDel();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��

	IWORD    tcc_no;			// ����վ��0Ϊɾ����ͬ����ŵ���������վ���
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandDel)

class AFX_EXT_CLASS MsgTccCommandDump: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandDump)
public:
	MsgTccCommandDump();
	virtual ~MsgTccCommandDump();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	// ����̨ ̨��
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandDump)

class AFX_EXT_CLASS MsgTccCommandQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandQuery)
public:
	MsgTccCommandQuery();
	virtual ~MsgTccCommandQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	// ����̨ ̨��
	IWORD    tcc_no;		// ����վ
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandQuery)

// ��ʱ���������´�
class AFX_EXT_CLASS MsgStccCommandSend: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandSend)
public:
	MsgStccCommandSend();
	virtual ~MsgStccCommandSend();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	IDWORD   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	I_TIME    time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������

	I_BOOL    bCancelLimit;	// �Ƿ�ȡ����������
	I_BOOL	bInitOpration;			// �Ƿ��ǳ�ʼ������
	IBYTE	section_dir;	// ������������ڳ�վ�ķ���0:���з���, 1:���з���
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSend)

// LiRC��GPC���������������״̬�ı�(???)
class AFX_EXT_CLASS MsgTccCommandStatusChanged: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandStatusChanged)
public:
	MsgTccCommandStatusChanged();
	virtual ~MsgTccCommandStatusChanged();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��

	IWORD    tcc_no;			// ����վ���пأ�

	I_BOOL    b_status_changed;
	IWORD    tcc_status;		// ����վ����״̬(�����С�ִ�С�ʧ��)
	I_BOOL       b_sender_set;
	ENTITY_ID  tcc_sender;	// ����վ������
	I_BOOL    b_time_set;
	I_TIME    tcc_time;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandStatusChanged)


// LiRC������������ķ���
class AFX_EXT_CLASS MsgStccCommandSendResult: public MsgStccCommandSend
{
	DECLARE_CLBTH(MsgStccCommandSendResult)
public:
	MsgStccCommandSendResult();
	virtual ~MsgStccCommandSendResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD   result;			// ����ִ�н��

	void SetComment(const char* pComment, INT len);
	void GetComment(char buf[], INT len);
private:
	IWORD    comment_length;	// �������ֳ���
	char    *strComment;	// ����
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult)

// ��LiRC����ʵʱ�����б�
class AFX_EXT_CLASS MsgStccCommandListQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandListQuery)
public:
	MsgStccCommandListQuery();
	virtual ~MsgStccCommandListQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    tcc_no;			// ����վ���пأ�
	ENTITY_ID  inquirer;	// ������
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandListQuery)

// LiRC�����ʵʱ�����б�
class AFX_EXT_CLASS MsgStccCommandList: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandList)
public:
	MsgStccCommandList();
	virtual ~MsgStccCommandList();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	IDWORD   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	IDWORD   time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandList)

// LiRC�����ʵʱ�����б�(���豸Ԫ���б�)
class AFX_EXT_CLASS MsgStccCommandDetail: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandDetail)
public:
	MsgStccCommandDetail();
	virtual ~MsgStccCommandDetail();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	IDWORD   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	IDWORD   time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������

	I_TIME	work_time;	// ����ִ��ʱ��

	enum { MAIN_LINE_LMT, SIDE_LINE_LMT };
	IBYTE	line_type;

	enum { MAX_CMD_ELEM_COUNT = 128 };
	struct
	{
		STATION_NO	sta_no;
		DEVICE_TYPE	elem_type;
		DMIS_ID		elem_id;
	}	elements[MAX_CMD_ELEM_COUNT];
	IWORD	elem_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandDetail)

// �п�������Ϣ�������
// ��վ�п�������������Ϣ
// CTC��TCC���Ͳ���������Ϣ
class AFX_EXT_CLASS MsgTccThreePointCheckRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgTccThreePointCheckRequest)
public:
	MsgTccThreePointCheckRequest();
	virtual ~MsgTccThreePointCheckRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	IBYTE    ctc_request[20];	//������CTC������Ϣ��
	//	IDWORD	 tcc_no;			//TCCվ���
	//	IBYTE	 command_type;		//��������
	//	IWORD	 deviece_id;		//�����������
	//	IBYTE	 train_id;			//�г����
	//	IWORD	 ctc_sender;		//������ʵ���	 
	//	IDWORD	 crc1;				//CRC32У��ֵ1
	//	IDWORD	 crc2;				//CRC32У��ֵ2
	IDWORD	 tcc_data_vebose;	//�п����ݰ汾��
};
DECLARE_SERIALIZE_OPERATOR(MsgTccThreePointCheckRequest)


// TCC��CTC����ȷ�ϻ�ִ��Ϣ
class AFX_EXT_CLASS MsgTccThreePointCheckConfirm: public BaseMsg
{
	DECLARE_CLBTH(MsgTccThreePointCheckConfirm)
public:
	MsgTccThreePointCheckConfirm();
	virtual ~MsgTccThreePointCheckConfirm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	IBYTE    tcc_confrim[20];    //tcc������ȷ����Ϣ��
	//	IDWORD	 tcc_no;			//TCCվ���
	//	IBYTE	 command_type;		//��������
	//	IWORD	 deviece_id;		//�����������
	//	IBYTE	 result_id;			//ִ�н��
	//	IWORD	 ctc_sender;		//������ʵ���	 
	//	IDWORD	 crc1;				//CRC32У��ֵ1
	//	IDWORD	 crc2;				//CRC32У��ֵ2
	IDWORD	 tcc_data_vebose;	//�п����ݰ汾��
	
};
DECLARE_SERIALIZE_OPERATOR(MsgTccThreePointCheckConfirm)
//-----------------------------------------------------------------------------
//
//C2�п���Ϣ
//
//-----------------------------------------------------------------------------

typedef struct KM_MARK_STRUCT {
		IWORD  nKmsysCode;	//�����ϵ
		IWORD  k_cl;	//������
		IWORD  m_cl;
		IDWORD val;	//�����0-9999 999
	} kmmark_t;
	EXTERN Archive& operator<<(Archive& ar, kmmark_t& m); 
	EXTERN Archive& operator>>(Archive& ar, kmmark_t& m); 


// ��ר�п����ٳ�ʼ�����
class AFX_EXT_CLASS MsgStccInitComplete: public BaseMsg
{
	DECLARE_CLBTH(MsgStccInitComplete)
public:
	MsgStccInitComplete();
	virtual ~MsgStccInitComplete();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������

	kmmark_t   tcc_km_marker[2];	// ����վ����귶Χ
};
DECLARE_SERIALIZE_OPERATOR(MsgStccInitComplete)

// ��ר�п�����״̬
class AFX_EXT_CLASS MsgStccReport2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccReport2)
		
public:
	MsgStccReport2();
	virtual ~MsgStccReport2();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IBYTE init_status;	//��ʼ��״̬: δ��ʼ��, ��ʼ��������, ��ʼ�������
    STATION_NO sta_no;      //����վ��
    IBYTE comm_active;	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
	IBYTE nStaNum;// վ��
	struct {
    		STATION_NO nStaID;// վ��
        IBYTE stby_status;       // 1: ������ 2��˫��
		IBYTE master_status;     // 1: a�� 2  b: ��	
		IBYTE ilock_channel[2];  // ����������ͨ��״̬ //=10������ =00���� =01����
		IBYTE ctc_channel[2];    // ��CTC����ͨ��״̬  //=10������ =00���� =01���� 
		IBYTE switch_channel[2];  //�뽻��������ͨ��״̬ // =00���� =03����
		struct {
			I_BOOL bValid;	//�Ƿ���Ч  2: ��Ч  1����Ч
			IBYTE leu_channel[2];// LEU ͨ��״̬ 0 ���� 1 ����
			IBYTE leu_ans[4];//LEUӦ����״̬, һ��leu��4��Ӧ���� 0 ���� 1 ����
		} leu[16];
		IBYTE nZCNum;
		struct {
	    	STATION_NO nStaID;// �м�վ վ��
			IBYTE channel[2];//ֻ�е�һ����վ���� ���м�վͨ��������״̬
		} ZC[8];
	} sta[32];
};
DECLARE_SERIALIZE_OPERATOR(MsgStccReport2)


// ��ר LiRC�����ʵʱ�����б�
class AFX_EXT_CLASS MsgStccCommandList2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandList2)
public:
	MsgStccCommandList2();
	virtual ~MsgStccCommandList2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	IDWORD   time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ
    IDWORD   set_time;               // ������Чʱ��   time_t, 0��ʾ�޷�ȷ��ʱ��(���ɻ�������)

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������
	kmmark_t   tcc_km_marker[2];	// ����վ����귶Χ
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandList2)


// ��ʱ���������´�
class AFX_EXT_CLASS MsgStccCommandSend2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandSend2)
public:
	MsgStccCommandSend2();
	virtual ~MsgStccCommandSend2();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	I_TIME    time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������

	I_BOOL    bCancelLimit;	// �Ƿ�ȡ����������
	I_BOOL	bInitOpration;			// �Ƿ��ǳ�ʼ������
	IBYTE	section_dir;	// ������������ڳ�վ�ķ���0:���з���, 1:���з���

	kmmark_t   tcc_km_marker[2];	// ����վ����귶Χ
	IBYTE    op;				// �������� 1:У��  2:ִ��
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSend2)


// ��ר ����������STPC, GPC��ȷ���б�
class AFX_EXT_CLASS MsgTccCommand2: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommand2)
public:
	MsgTccCommand2();
	virtual ~MsgTccCommand2();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    dcmd_no[16];    // �����������
	ENTITY_ID  dcmd_entity; // ������������
	I_TIME       dcmd_time;   // ������������

	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IBYTE    raw_sysid[16];	// ԭʼ����ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	I_TIME    time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ
	I_BOOL    bCancelLimit;	// �Ƿ� ȡ����������

	ENTITY_ID  creator;		// ԭʼ���������
	ENTITY_ID  checkor;		// ����У����
	ENTITY_ID  confirmor;	// ȷ�Ϸֽ���

	IWORD       alarm_num;   // ���ı�������
	ENTITY_ID  alarm[4];    // ���Ľ��ܱ�����

	IWORD    tcc_num;		      // ����վ����
	IWORD    tcc_no[128];	          // ����վ���пأ�
	IWORD    tcc_line_no[128];      // ����վ��·
	IWORD    tcc_status[128];	      // ����վ����״̬
	I_TIME    tcc_time[128];	      // ����վ������Чʱ��
	ENTITY_ID  tcc_checkor[128];   // ����վУ����
	ENTITY_ID  tcc_sender[128];	  // ����վ�����ˣ���վ���������е���
	kmmark_t   tcc_km_marker[128][2]; // ����վ����귶Χ

	IWORD       tcc_alarm_num[128];   // ��վ��������
	ENTITY_ID  tcc_alarm[128][4];    // ��վ���ܱ�����

	IDWORD   nMarkIndex;		// ����ͼ�������б�־
	I_TIME    create_time;		// �����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommand2)

class AFX_EXT_CLASS MsgTccCommandAdd2: public MsgTccCommand2
{
	DECLARE_CLBTH(MsgTccCommandAdd2)
public:
	MsgTccCommandAdd2();
	virtual ~MsgTccCommandAdd2();
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandAdd2)

class AFX_EXT_CLASS MsgTccCommandData2: public MsgTccCommand2
{
	DECLARE_CLBTH(MsgTccCommandData2)
public:
	MsgTccCommandData2();
	virtual ~MsgTccCommandData2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum QUERY_TYPE
	{
		QUERY_CURRENT,
		QUERY_HISTORY
	} type;

};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandData2)

class AFX_EXT_CLASS MsgTccCommandQuery2: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandQuery2)
public:
	MsgTccCommandQuery2();
	virtual ~MsgTccCommandQuery2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	  // ����̨̨��
	IWORD    tcc_no;		  // ����վ
	IBYTE    dcmd_no[16];  // �����������
	
	I_TIME    time_scope[2];	//ʱ��η�Χ, ƥ�������ʱ��, ����򲻿���ʱ��, time_scope[0]==0��ʾ����time_scope[1]֮ǰ��, time_scope[1]==0��ʾ����time_cope[0]֮���

	enum QUERY_MODE 
	{
		QUERY_ALL,	//��ѯȫ��
		QUERY_BY_CONID,//������̨id��ѯ
		QUERY_BY_TCCNO,//������վ��ѯ
		QUERY_BY_DCMDID//����������Ų�ѯ
	} flag;

	enum QUERY_TYPE
	{
		QUERY_CURRENT,//��ѯ��ǰ
		QUERY_HISTORY//��ѯ��ʷ
	} type;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandQuery2)

class AFX_EXT_CLASS MsgTccCommandSet: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandSet)
public:
	MsgTccCommandSet();
	virtual ~MsgTccCommandSet();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    tcc_no;			// ����վ��0Ϊ��ͬ����ŵ���������վ���

	IWORD    tcc_status;         // �����MsgTccCommand��tcc_status��ͬ
	I_TIME    tcc_time;	    // ����վ������Чʱ��

	ENTITY_ID  tcc_checkor; // ����վУ����
	ENTITY_ID  tcc_sender;	// ����վ�����ˣ���վ���������е���
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandSet)

class AFX_EXT_CLASS MsgTccCommandOperEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandOperEcho)
public:
	MsgTccCommandOperEcho();
	virtual ~MsgTccCommandOperEcho();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��

	IWORD code;        //  0(������),    1,             2,           3,          4,
	enum TccOper
	{
		ADD,          // OK����������̫��,  �����Ѵ���,   ������ظ�
		DEL,          // OK�����ݲ�����
		SET           // OK�����ݲ�����
	} oper;

	IWORD para;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandOperEcho)


// LiRC������������ķ���
class AFX_EXT_CLASS MsgStccCommandSendResult2: public MsgStccCommandSend2
{
	DECLARE_CLBTH(MsgStccCommandSendResult2)
public:
	MsgStccCommandSendResult2();
	virtual ~MsgStccCommandSendResult2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD   result;			// ����ִ�н��  1: ���ɻ��յ� 2:�п�У��ɹ� 3:�п�У��ʧ�� 4:�п�ִ�гɹ� 5:�п�ִ��ʧ�� 6:�п�ϵͳ�쳣 7:���ɻ����ʧ�� 

	void SetComment(const char* pComment, INT len);
	void GetComment(char buf[], INT len);
private:
	IWORD    comment_length;	// �������ֳ���
	char    *strComment;	// ����
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult2)

// LiRC�����ʵʱ�����б�(���豸Ԫ���б�)
class AFX_EXT_CLASS MsgStccCommandDetail2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandDetail2)
public:
	MsgStccCommandDetail2();
	virtual ~MsgStccCommandDetail2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// ����̨ ̨��
	IBYTE    sys_id[16];		// ϵͳ��ʶ
	IWORD    cmd_no;			// ����ţ�0~9999��
	IWORD    sta_no[2];		// ��ʼ������վ��
	IWORD    line_no;		// ��·��
	I_BOOL    bFullLimit;		// �Ƿ�ȫ������
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL    bStartNow;		// �Ƿ�������ʼ
	I_BOOL    bEndless;		// �Ƿ�������Ч
	IDWORD   time[2];		// ��ʼ������ʱ��
	IWORD    speed;			// ����ֵ

	IWORD    tcc_no;			// ����վ���пأ�
	IWORD    tcc_line_no;	// ����վ��·
	ENTITY_ID  tcc_sender;	// ����վ������

	I_TIME	work_time;	// ����ִ��ʱ��

	enum { MAIN_LINE_LMT, SIDE_LINE_LMT };
	IBYTE	line_type;

	enum { MAX_CMD_ELEM_COUNT = 128 };
	struct
	{
		STATION_NO	sta_no;//վ��
		DEVICE_TYPE	elem_type;//�豸����
		DMIS_ID		elem_id;//�豸id
	}	elements[MAX_CMD_ELEM_COUNT];
	IWORD	elem_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandDetail2)

// �п�������Ϣ�������


// ��������� ����
class AFX_EXT_CLASS MsgStccCommandIDRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandIDRequest)
public:
	MsgStccCommandIDRequest();
	virtual ~MsgStccCommandIDRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum {	GET_CMDID = 1,	SET_CMDID = 2,	};
	IBYTE    oper_mode;		// GET��������ţ�SET��������ŵķ���

	IWORD    console_no;		// ����̨ ̨��
	IWORD    cmd_no;			// ��ǰ����ţ�0~9999��
	IWORD    min_cmd_no;		// ��С�����
	IWORD    max_cmd_no;		// ��������
	IDWORD   dwCookie;		// �����߱�����У������

	enum {
		TCC_CMDID_REQUEST_SUCCESS = 1,
		TCC_CMDID_REQUEST_FAIL = 2,
	};
	IWORD    nStatus;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandIDRequest)

//-----------------------------------------------------------------------------
//
//TSR �п���Ϣ
//
//-----------------------------------------------------------------------------

struct tsr_cmd_t
{
	IBYTE	reason;			// ����ԭ��(1:ʩ��; 2:����ѩ; 3:ͻ���ֺ�)
	char		cmd_no[16];		// ����ţ�0~9999��
	IWORD	sta_no;			// վ��(���Բ���������Ч)
	IWORD	line_no;			// ��·��
	kmmark_t   km_marker[2];	// ��ʼ����������꣨��mΪ��λ��
	I_BOOL	bStartNow;		// �Ƿ�������ʼ
	I_BOOL	bEndless;		// �Ƿ�������Ч
	I_TIME	time[2];			// ��ʼ������ʱ��
	IWORD	speed;			// ����ֵ

	size_t	countObjectLength();
};
EXTERN Archive& operator<<(Archive& ar, tsr_cmd_t& m); 
EXTERN Archive& operator>>(Archive& ar, tsr_cmd_t& m); 


// TSR��������ͣ��ⶨ��DCMD->TSRi
class AFX_EXT_CLASS MsgTSRCmdSend: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdSend)
public:
	MsgTSRCmdSend();
	virtual ~MsgTSRCmdSend();
private:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	char	szGUID[64];
	char	dcmd_no[16];		// ���������
	char	m_cContent[2048];//������������
	IWORD	m_nContentLen;//�����������ݳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdSend)

// TSR���������״̬�Ļ�ִ���ⶨ�Ļ�ִ��TSRi->DCMD
class AFX_EXT_CLASS MsgTSRCmdSendResult: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdSendResult)
public:
	MsgTSRCmdSendResult();
	virtual ~MsgTSRCmdSendResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	char			szGUID[64];//��������id
	tsr_cmd_t	sTsrCmd;//��������
	IBYTE		nResult;	//��������ͽ��
	enum	// ״̬����
	{
		CMD_CTC_RECEIVED = 11,	// TSRͨ�Ż����յ�
		CMD_CTC_SEND,			// TSRͨ�Ż��ѷ�����TSR������
		CMD_TSR_CHECKOK,		// TSR��������У��ͨ��
		CMD_TSR_CHECKFAIL,	// TSR������У��ʧ��
		CMD_TSR_TIMEOUT,		// TSR��������Ӧ��ʱ
		CMD_TSR_DEAD,			// TSR����������Ӧ
		CMD_CTC_REJECTED		// TSRͨ�Ż��ڲ����󲻽�������
	};
	char			strResult[128];	// ��������ַ���
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdSendResult)

// ���ټ�����á���������ʼ����TSR������˹�������TSRTerm->TSRi
class AFX_EXT_CLASS MsgTSRCmdOperation : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdOperation)
public:
	MsgTSRCmdOperation();
	virtual ~MsgTSRCmdOperation();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;	// ��������
	CONID		nConID;		// ���κ�
	IBYTE		nOper;		// ��������
	enum
	{
		TSR_INIT	= 1,	// ��ʼ��
		TSR_ACT		= 2,	// ����
		TSR_SET		= 3,	// ����
		TSR_REVOKE	= 4		// ����
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdOperation)

// ���ٲ���������TSR��������Ļ�ִ��TSRi->TSRTerm
class AFX_EXT_CLASS MsgTSRCmdOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdOperResult)
public:
	MsgTSRCmdOperResult();
	virtual ~MsgTSRCmdOperResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd; 	// ��������
	CONID		nConID;		// ���κ�
	IBYTE		nOper; 		// ��������
	enum
	{
		TSR_INIT	= 1,	// ��ʼ��
		TSR_ACT		= 2,	// ����
		TSR_SET		= 3,	// ����
		TSR_REVOKE	= 4		// ����
	};
	IBYTE		nResult;	// �������
	enum
	{
		CMD_CTC_RECEIVED = 1,	// TSRͨ�Ż����յ�
		CMD_CTC_SEND,			// TSRͨ�Ż��ѷ�����TSR������
		CMD_TSR_OK,				// TSR�������ѳɹ�����
		CMD_TSR_FAIL,			// TSR���������ش���
		CMD_TSR_TIMEOUT,		// TSR��������Ӧ��ʱ
		CMD_TSR_DEAD,			// TSR����������Ӧ
		CMD_CTC_REJECTED		// TSRͨ�Ż��ڲ����󲻽�������
	};
	char		strResult[128];		// ��������ַ���	
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdOperResult)

// ������ʾ,TSRi->TSRTerm������TSRTerm��������list��ʾ������ʾ
class AFX_EXT_CLASS MsgTSRCmdAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdAlarm)
public:
	MsgTSRCmdAlarm();
	virtual ~MsgTSRCmdAlarm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;		// ��������
	CONID		nConID;			// ���κ�
	IBYTE		nAlarmLevel;	// ��ʾ�ȼ�
	enum
	{
		ALARM_EMERGENCY	= 1,	// ������ʾ������ȷ��ʾ�˹�����
		ALARM_NORMAL = 2		// һ����ʾ
	};
	IBYTE		nAlarmType;		// ��ʾ����
	enum
	{
		ST_ALARM_STOP		= 0,	// ֹͣ��ʾ
		ST_CANCEL			= 2,	// ִ��ȡ���ɹ�
		ST_REVOKE			= 4,	// ִ�г����ɹ�
		ST_ALL_SET			= 6,	// ִ�����óɹ�
		ST_PART_SET			= 14,	// ��ʾ����ִ��
		ST_NOT_SET_ALARM	= 16,	// ��ʾ��ʱδ����
		ST_SET_ALARM		= 18,	// ��ʾ������
		ST_ACTIVE_ALARM		= 20	// ��ʾ�ɼ���
	};
	IWORD		nValidTime;		// ��ʾ��Чʱ�䣨��λ�룩
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdAlarm)

// TSR��������ʵʱ״̬���Ǳ�ʾ��TSRi->TSRTerm��
class AFX_EXT_CLASS MsgTSRCmdStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdStatus)
public:
	MsgTSRCmdStatus();
	virtual ~MsgTSRCmdStatus();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;	//��������
	CONID		nConID;	//���κ�
	IBYTE		nStatus;		// TSR����״̬
	enum	// ����״̬����
	{
		CMD_CHECKOK = 1,	// �������ⶨ
		CMD_ACTIVE,			// �����Ѽ���
		CMD_SETALLOK,		// ������ִ��
		CMD_CANCEL,			// ȡ�����������Ѽ����ִ�У�
		CMD_SETPARTOK		// �������ã���ִ�У�
	};
	IBYTE		nDetailStatus;
	enum	// ״̬��ϸ����
	{
		ST_PROCESS			= 0,	// ���ڴ�����
		ST_CANCEL			= 2,	// ִ��ȡ���ɹ�
		ST_REVOKE			= 4,	// ִ�г����ɹ�
		ST_ALL_SET			= 6,	// ִ�����óɹ�
		ST_PREPARE_CANCEL	= 8,	// ��֤ȡ���ɹ�
		ST_PREPARE_SET		= 10,	// ��֤���óɹ�
		ST_NOT_ACTIVE		= 12,	// ��δ����
		ST_PART_SET			= 14,	// ��ʾ����ִ��
		ST_NOT_SET_ALARM	= 16,	// ��ʾ��ʱδ����
		ST_SET_ALARM		= 18,	// ��ʾ������
		ST_ACTIVE_ALARM		= 20	// ��ʾ�ɼ���
	};

	friend class MsgTSRCmdDisplay;
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdStatus)

// TSR���������ѯ,1: �����ʾLiRC->TSRi,nQueryType==QUERY_DISP    = 0x0200,	2:��������TSRTerm->TSRi��nQueryType==QUERY_CMD    = 0x0400   
class AFX_EXT_CLASS MsgTSRCmdQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdQuery)
public:
	MsgTSRCmdQuery();
	virtual ~MsgTSRCmdQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD		nQueryType;
	enum
	{
		QUERY_CHECKOK = 0x0001,	// ������У��ͨ��
		QUERY_ACTIVE  = 0x0002,	// �����Ѽ���
		QUERY_SETOK   = 0x0004,	// ����������
		QUERY_CANCEL  = 0x0008,	// ȡ�����������Ѽ���
		QUERY_ALL     = 0x000F,	// ����ȫ��
		QUERY_COUNT	  = 0x0100,	// �������ٸ���
		QUERY_DISP    = 0x0200,	// �����ʾ
		QUERY_CMD    = 0x0400    // ��������
	};
	STATION_NO	nzStations[256];
	int			nStaCount;
	CONID		nConID;			// ��������κ�
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdQuery)

// TSRS״̬��ѯ��TSRTerm->TSRi��
class AFX_EXT_CLASS MsgTSRServerRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRServerRequest)
public:
	MsgTSRServerRequest();
	virtual ~MsgTSRServerRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { MAX_REQ_CON_COUNT = 32 };
	CONID	nzConIDs[MAX_REQ_CON_COUNT];	// ���κ��б�
	IBYTE	nConCount;
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRServerRequest)

// TSRS״̬���棬TSRi->TSRTerm,����MsgTSRServerRequest����
class AFX_EXT_CLASS MsgTSRServerStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRServerStatus)
public:
	MsgTSRServerStatus();
	virtual ~MsgTSRServerStatus();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IDWORD	nTsrsID;		// TSRS���
	CONID	nConID;			// ���κ�
	IBYTE	nStatus;		// ״̬
	enum
	{
		TSRS_DOWN	= 0,	// �ж�
		TSRS_UNINIT,		// δ��ʼ��
		TSRS_INIT			// �ѳ�ʼ��
	};
	IBYTE	nTimeFlag;		// ʱ��ͬ����־��1��TSRS��CTCͬ����0����ͬ����
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRServerStatus)

// TSR��������������TSRi->TSRTerm,����MsgTSRCmdStatusʱ����֮ǰ�ͷ�֮���ͣ���Ϊ��ʼ�ͽ�����ʶ
class AFX_EXT_CLASS MsgTSRCmdCount: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdCount)
public:
	MsgTSRCmdCount();
	virtual ~MsgTSRCmdCount();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID	nConID;	      //���κ�
	IWORD	nCheckCount;  //��������У����
	IWORD	nActiveCount; //�����������
	IWORD	nSetCount;    //��������������
	IWORD	nCancelCount; //��������ȡ����
	IBYTE	nReplyStatus; //���������ִ״̬
	enum { RPL_BEGIN, RPL_END }; //
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdCount)

// TSR��������ʵʱ״̬��ʾ��TSRi->LiRC,
class AFX_EXT_CLASS MsgTSRCmdDisplay: public MsgTSRCmdStatus
{
	DECLARE_CLBTH(MsgTSRCmdDisplay)
public:
	MsgTSRCmdDisplay();
	virtual ~MsgTSRCmdDisplay();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { MAX_ELEM_COUNT = 256 };
	struct
	{
		short nStation;    //��վ
		short nDmisType;   //dmis����
		short nDmisID;     //dmis��
	}		szElems[MAX_ELEM_COUNT];
	int		nElemCount;     //Ԫ����
	char	szComment[256]; //ע��
	IBYTE	nRemoveFlag;//
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdDisplay)

CLBTH_NAMESPACE_END

#endif

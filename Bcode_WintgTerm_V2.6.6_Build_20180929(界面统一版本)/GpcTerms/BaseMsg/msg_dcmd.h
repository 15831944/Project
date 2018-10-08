#ifndef _CLBTH_MSGDEF_MSG_DCMD_H_
#define _CLBTH_MSGDEF_MSG_DCMD_H_

#include "msg_tcc.h" //for import kmmark_t
#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

class AFX_EXT_CLASS MsgDispatchCommandSend : public BaseMsg
{
	DECLARE_CLBTH(MsgDispatchCommandSend)
		
public:
	MsgDispatchCommandSend();
	virtual ~MsgDispatchCommandSend();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	
	ENTITY_ID   send_entity;   //����ʵ��
	ENTITY_ID   recv_entity;   //����ʵ��
	I_TIME        send_time;	
    I_TIME        edit_time;

	CONID		con_id;			// GPC����ʱʹ��
	STATION_NO	source_station;	// ��վPC����ʱʹ��
	 
	IDWORD unique_cmdindex;  //��������Ψһʶ��
	short bianMa;  //�������ͱ���
	short cmdNo;   //�������
	char cmdName[512];  //������������,���Ը������ͱ�������֪ // 2010.2.26 �Ϻ��ֵ���������ⳤ�������޷�����
	char rereader[32];  //����������
	char dispatcher[32]; //����Ա����
	char unit[32];  //���λ�����λ
	char reader[32];  //�Ķ�������
 
	short       recv_count;     //���͵���Ŀ
	struct RECV_LIST{
		ENTITY_ID	recv_entity; //���͵�ʵ��
		char        chaosongMemo[64]; //����ע��		
	}recv_list[64];
	

	short		chaosong_recv_count;     //������Ŀ
	ENTITY_ID	chaosong_recv_list[128]; //�����б�
	char        chaosongMemo[128]; //����ע��

	short chaosong_train_list_count;    //��STPC֪�����͵��г� 
	struct CHAOSONG_TRAIN_LIST{
	   TRAIN_NO    train_id; //���Σ�for wireless
	   TRAIN_NO    locomotive; //������,for wireless
	   ENTITY_ID   recv_lirc;   //����ʵ��
	}chaosong_train_list[32];
   
	enum CMD_TYPE {
		    DISPATCH_COMMAND = 0x01, //��������    
			ROUTE_TICKET,	  //·Ʊ
			GREEN_ADMISSION,     //��ɫ���֤
			RED_ADMISSION,       //��ɫ���֤
			SPY_ON_SHUNT_NOTE,   //��վ���ٵ���֪ͨ��
			USE_LIGHT_CAR_ADMISSION, //�����г�ʹ��˵����
			FORETELL_TRAIN_ROUTE,    //�г���·Ԥ����Ϣ
			JUST_LINE_PASS,   //����ͨ����
			BY_LINE_PASS,  //����ͨ����
			JUST_LINE_SLOW_PASS,  //���߻���ͨ����
			ENTER_JUST_LINE_STOP,//��վ����ͣ����
			ENTER_BY_LINE_STOP,  //��վ����ͣ���� 
			SHUNTING_JOB = 0x11, //������ҵ
			SHUNTING_REQUEST_RECEIVED = 0x12, //�����������յ�
			IN_OUT_CHECK = 0x20,      //�������
			OTHER_COMMAND,    //��������
	}cmd_type;

	//���������ߵ�������
	enum   SEND_WAY {
		    SEND_INTELLIGENT,                     //���ܷ���
			SEND_IMMEDIATELY                      //��������
	}send_way;                             //   ���ͷ�ʽ
	
	enum CMD_DESTINATION{
		GPC_2_DIRECOR_ON_DUTY,
		DIRECOR_ON_DUTY_2_GPC,			
        GPC_2_GPC,
		GPC_2_STPC,
		GPC_2_LIRC,
		GPC_2_STPC_AND_LIRC, //û���õ���ʵ���Ϸ����η��͵�
        STPC_2_LIRC,	
		STPC_2_GPC,  //������������
	}cmd_destination;

	short ContentLength;  //����
	char content[4000];

	TRAIN_NO    train_id; //���Σ�for wireless
	TRAIN_NO    locomotive; //������,for wireless

//�п����
	IBYTE	bTccCmd;//
	IWORD	nCmdID;			//�����
	IWORD    nStaID[2];		//��ʼ������վ
	IWORD	nTccLineID;	//��·,Ԥ��Ӧ����Ҫ��
	IWORD	nTccNum;//����վ����
	struct TccLine
	{
		IWORD	nTccID;			//����վ
		IWORD	nTccLineID;		//��·
	}tccline[8];

	IBYTE	bFullLimit;		//�Ƿ�ȫ������
	IDWORD	nKmMark[2];		//ʼĩ�����(����Ϊ��λ)
	IBYTE	bStartNow;		//�Ƿ�������ʼ
	IBYTE	bEndless;		//�Ƿ�������Ч
	IDWORD	nTime[2];		//��ʼ������ʱ��

	IBYTE	bCancelLimit;	//�Ƿ�ȡ������

	IWORD	nTrainID;	//���ٸ�����
	struct {
		TRAIN_NO szTrainID;	//���κ�
		IWORD	nSpeedMaxVal;	//����ֵ
	} train_cmd[5];
 //�п�end
};

DECLARE_SERIALIZE_OPERATOR(MsgDispatchCommandSend)


class AFX_EXT_CLASS MsgDispatchCommandStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgDispatchCommandStatus)
		
public:
	MsgDispatchCommandStatus();
	virtual ~MsgDispatchCommandStatus();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	
	ENTITY_ID   send_entity;
	ENTITY_ID   recv_entity;
	I_TIME        send_time;	
    I_TIME        recv_time;
	
	IDWORD unique_cmdindex;  //��������Ψһʶ��
	short bianMa;  //�������ͱ���
	short cmdNo;   //�������
 
	enum CMD_TYPE {
		    DISPATCH_COMMAND = 0x01, //��������    
			ROUTE_TICKET,	  //·Ʊ
			GREEN_ADMISSION,     //��ɫ���֤
			RED_ADMISSION,       //��ɫ���֤
			SPY_ON_SHUNT_NOTE,   //��վ���ٵ���֪ͨ��
			USE_LIGHT_CAR_ADMISSION, //�����г�ʹ��˵����
			FORETELL_TRAIN_ROUTE,    //�г���·Ԥ����Ϣ
			JUST_LINE_PASS,   //����ͨ����
			BY_LINE_PASS,  //����ͨ����
			JUST_LINE_SLOW_PASS,  //���߻���ͨ����
			ENTER_JUST_LINE_STOP,//��վ����ͣ����
			ENTER_BY_LINE_STOP,  //��վ����ͣ���� 
			SHUNTING_JOB = 0x11, //������ҵ
			SHUNTING_REQUEST_RECEIVED = 0x12, //�����������յ�
			IN_OUT_CHECK = 0x20,      //�������
			OTHER_COMMAND,    //��������
	}cmd_type;
	
 
    TRAIN_NO    train_id; //���Σ�for wireless
	TRAIN_NO    locomotive; //������,for wireless
	
	enum RECV_DESTIN{
		 STATUS_GPC_DIRECOR_ON_DUTY,
		 STATUS_GPC,
	     STATUS_STPC,
		 STATUS_LIRC,
		 STATUS_LOCOMOTIVE,
	}recv_destin;
	enum RECV_STATUS {
		    COMMAND_RECV,
			COMMAND_SIGN,
			COMMAND_LOCOMOTIVE_RECV,
			COMMAND_LOCOMOTIVE_SIGN,	
			COMMAND_SENDED,            //���ɻ��ѷ���
			COMMAND_ENTITY_DEAD,
			COMMAND_ERROR,
			COMMAND_DENY, //�ܾ�
			COMMAND_INIT,
			COMMAND_DEV_RECV, 	// ����ת������ִ
			COMMAND_AGREE = 21,			//ͬ��
			COMMAND_DISAGREE = 23		//��ͬ��			
    } recv_status;
	
    char    sign_name[32];
	I_TIME    sign_time;
	
	
};

DECLARE_SERIALIZE_OPERATOR(MsgDispatchCommandStatus)

class AFX_EXT_CLASS MsgDCmdTcc : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTcc)
		
public:
	MsgDCmdTcc();
	virtual ~MsgDCmdTcc();
	
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	//���������STPCУ���п�����ʱ��Ч
	IBYTE    nCmdType;
	IBYTE    nDirection;		//���ٷ���	1����  2����  3˫��

	IWORD    nConsoleID;		//����̨ ̨��
	IBYTE    raw_sysid[16];	//ԭʼ�����ʶ

	IWORD	nCmdID;			//�����
	IWORD    nStaID[2];		//��ʼ������վ
	IWORD	nLineID;	    //��·,Ԥ��

	I_BOOL	bFullLimit;		//�Ƿ�ȫ������
	kmmark_t nKmMark[2];		//ʼĩ�����(����Ϊ��λ)

	I_BOOL	bStartNow;		//�Ƿ�������ʼ
	I_BOOL	bEndless;		//�Ƿ�������Ч
	IDWORD	nTime[2];		//��ʼ������ʱ��
	I_BOOL	bCancelLimit;	//�Ƿ�ȡ������

	I_BOOL	bChecked;		//�Ƿ��Ѿ�У�飬����nCheckorID
	IDWORD   nCreatorID;		//������
	IDWORD	nCheckorID;		//����У����
	IDWORD   nConfirmorID;	//ȷ�Ϸֽ���
	IDWORD   nCreateTime;	//�����ʱ��

	IWORD	nTccNum;		//����վ����
	struct TCC 
	{
		IWORD	nTccID;		//����վ
		IWORD	nTccLineID;	//����վ��·
		IDWORD   nCheckorID;	//��վУ����(����վΪȷ�Ϸֽ���)
		kmmark_t   nKmMark[2];	//����վ����귶Χ
		I_BOOL    bNeedCheck;	//�Ƿ���ҪУ��
	} tcc[128];

	IWORD	nTrainNum;	    //���ٸ�����
	struct TRAIN_CMD
	{
		TRAIN_NO szTrainID;	    //���κ�
		IWORD	 nSpeedMaxVal;	//����ֵ, �����пص�ֵ
		IWORD     nSpeed;		//����ֵ, �˹�ѡ���ֵ, ����45��45�����п�
	} train_cmd[5];
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTcc)



class AFX_EXT_CLASS MsgDCmdTcc2 : public MsgDCmdTcc
{
	DECLARE_CLBTH(MsgDCmdTcc2)
		
public:
	MsgDCmdTcc2();
	virtual ~MsgDCmdTcc2();
	
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	IBYTE reason;	// ����ԭ��(1:ʩ��; 2:����ѩ; 3:ͻ���ֺ�)
	
	// Ԥ��
	IWORD nReserveLen;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTcc2)



CLBTH_NAMESPACE_END

#endif

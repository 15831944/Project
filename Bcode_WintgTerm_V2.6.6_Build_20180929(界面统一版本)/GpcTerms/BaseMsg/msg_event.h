#ifndef _CLBTH_MSGDEF_MSG_EVENT_H_
#define _CLBTH_MSGDEF_MSG_EVENT_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN


//---------------------------- �¼���� -------------------------------
#define MAX_EVENT_DATA_LEN 40960
#define MAX_EVENT_KEY_COUNT	128

enum EVENT_ID							// �¼�����ID
{
//TDCS3.0	
	NULL_EVENT_ID,						       // ������  ����ʵ��ҵ���滻���滻Ϊ����	EVENT_TRAIN_ARRVIE�� ÿ�����µ��¼����ͣ����޸Ĵ����Ͷ���
	EVENT_COMMON_APP_STOP,                     //�����˳�ʱ����          
	EVENT_COMMON_APP_START,                    //�������������          
	EVENT_COMMON_USER_LOGIN,                   //���û���¼ʱ����        
	EVENT_COMMON_USER_LOGOUT,                  //���û�ע��ʱ����        
	EVENT_STPC_ACK_DEPART,                     //ͬ����վ����            
	EVENT_STPC_REQ_DEPART,                     //����վ��������          
	EVENT_STPC_INFORM_DEPART,                  //����վ����Ԥ��          
	EVENT_STPC_ACK_SIDE,                       //ȷ�Ͻӳ��ɵ�            
	EVENT_STPC_REPORT_TRAIN_ARRIVE,            //ȷ���г�����,�˹�����   
	EVENT_STPC_REPORT_TRAIN_DEPART,            //ȷ���г�����,�˹�����   
	EVENT_STPC_ACK_SHUNT_STOP,                 //ȷ�ϵ�����ҵֹͣ        
	EVENT_STPC_ACK_TRAIN_INT,                  //ȷ���г�����            
	EVENT_STPC_SIGN_PLAN,                      //ǩ�ս׶μƻ�            
	EVENT_STPC_RECV_PLAN,                      //�յ��׶μƻ�            
	EVENT_STPC_SIGN_PLANNOTE,                  //ǩ�ս׶μ���            
	EVENT_STPC_RECV_PLANNOTE,                  //�յ��׶μ���            
	EVENT_STPC_SIGN_DCMD,                      //ǩ�յ�������            
	EVENT_STPC_RECV_DCMD,                      //�յ���������            
	EVENT_STPC_SEND_DCMD_REQ,                  //�����������            
	EVENT_STPC_SEND_RADIO_DCMD,                //�������ߵ�������        
	EVENT_STPC_RECV_RADIO_DCMD_ACK,            //���ߵ����������״̬    
	EVENT_STPC_REPORT_CURRENT_CAR,             //�ϱ����ڳ�              
	EVENT_STPC_REPORT_TRAIN_SUBAO,             //�ϱ��г��ٱ�            
	EVENT_LIRC_REPORT_ROUTE_STATUS,            //��⵽��·����,�źſ��� 
	EVENT_LIRC_REPORT_TRAIN_ENTERING,          //��⵽�г�ѹ����վ�źŻ�
	EVENT_LIRC_REPORT_TRAIN_EXITING,           //��⵽�г�ѹ����վ�źŻ�
	EVENT_LIRC_REPORT_SIDE_OCC,                //��⵽�ɵ�ռ��          
	EVENT_LIRC_REPORT_SIDE_UNOCC,              //��⵽�ɵ�����    
	EVENT_STPC_CHOOSE_SIDE,                    //ѡ��ӳ��ɵ�      added by wuchunbo 
	EVENT_STPC_AGREE_OPEN_ARSIG,               //�����Žӳ��ź�
	EVENT_STPC_AGREE_OPEN_DPTSIG,              //�����ŷ����ź�
	EVENT_STPC_PREPARE_OPEN_ARSIG,             //׼�����Žӳ��ź�
	EVENT_STPC_PREPARE_OPEN_DPTSIG,             //׼�����ŷ����ź�
	EVENT_STPC_TRAIN_APPROACH,					//�г��ӽ�
	EVENT_STPC_DPT_CONDITION_OK,				//������������
	EVENT_STPC_OPEN_DPTSIG_DONE,                //�����ź����
	EVENT_STPC_TRAIN_CONTACTED,					//�����г�
	EVENT_STPC_PREPARE_CONTACT_TRAIN,			//׼���ӳ�
	EVENT_LIRC_REPORT_TRAIN_ARRIVE,            //ȷ���г�����,�Զ�����   
	EVENT_LIRC_REPORT_TRAIN_DEPART,            //ȷ���г�����,�Զ�����  
	EVENT_LIRC_AR_ROUTE_CREATE,				   //�ӳ���·����
	EVENT_LIRC_AR_ROUTE_CANCEL,				   //�ӳ���·ȡ��
	EVENT_LIRC_PS_ROUTE_CREATE,				   //ͨ����·����
	EVENT_LIRC_PS_ROUTE_CANCEL,				   //ͨ����·ȡ��
	EVENT_LIRC_DP_ROUTE_CREATE,				   //������·����
	EVENT_LIRC_DP_ROUTE_CANCEL,				   //������·ȡ��
	EVENT_STPC_AR_ROUTE_CREATING,				   //���ڰ���ӳ���·
	EVENT_STPC_DP_ROUTE_CREATING,				   //���ڰ�������·
	EVENT_STPC_PREPARE_DPT,							//׼������
	EVENT_STPC_ACK_SIDE_CANCEL,                       //�ӳ��ɵ�������ӳ�����
	EVENT_LIRC_REPORT_SIDE_WHOLE_OCC,			//��⵽��ȫ�ɵ�ռ��
	EVENT_STPC_REPORT_TRAIN_PASS,            //�г�ͨ��,�˹�����   
	EVENT_LIRC_REPORT_TRAIN_PASS,            //�г�ͨ��,�Զ����� 
	EVENT_COMMON_TRAIN_ALARM,				//�г�����  
	EVENT_COMMON_SYS_ALARM,					//ϵͳ���� 
	EVENT_STPC_OPEN_DPTSIG_CANCEL,           //�����㷢������
	EVENT_STPC_CHECK_LINE,                   //�����·
	EVENT_STPC_ST_HOME_RT,					 //׼���ӳ���·
	EVENT_STPC_HOME_TRAIN,					 //�ӳ�
	EVENT_STPC_SET_DEPART_RT,				 //׼��������·
	EVENT_STPC_SET_TOKEN,					//����ƾ֤
	EVENT_STPC_ACK_DPT_CDT,					//ȷ�Ϸ�������
	EVENT_STPC_ACK_DPT_TRAIN,				//����
	EVENT_STPC_NEIB_AR,						//��վ����
	EVENT_STPC_OP_RECORD,				   //��վ�˹������¼�
	EVENT_STPC_FLOW_STATUS,				   //��վ���̱仯�¼�

	//���������¼����� ռ��Ԥ��600~799
	EVENT_DCMD_CREATE_COMMOND=600,	//���������		
	EVENT_DCMD_SAVE_COMMOND,	//���������		
	EVENT_DCMD_SEND_COMMOND,	//���������·�		
	EVENT_DCMD_RECV_COMMOND,	//�����������		
	EVENT_DCMD_SIGN_COMMOND,//��������ǩ��		
	EVENT_DCMD_LOGIN,//���������¼
	EVENT_DCMD_FEEDBACK_COMMOND,//���������յ����ܻ�ִ	
	
	//TSRTERM�¼����� ռ��Ԥ��800~899
	EVENT_TSRTERM_CMD_OPER=800, //�����������
	EVENT_TSRTERM_CMD_RESULT,	//��������������
	
	//���ɻ��¼����� ռ��Ԥ��900~999
	EVENT_LIRC_ROUTE_STATUS=900, //��·״̬�仯	
	
	//վ��ͼ�¼����� ռ��Ԥ��1000~1999
	EVENT_LAYOUTTERM_SM_REQUEST=1000,     //	��վ����ģʽ����
	EVENT_LAYOUTTERM_RM_REQUEST,     //	��վ��·ģʽ����
	EVENT_LAYOUTTERM_TRAININDEX_OPER,//	���κŲ���
	EVENT_LAYOUTTERM_TRAINSTOP,//	�����г�ͣ�ȡ�����      
	EVENT_LAYOUTTERM_TRAINATTR_OPER, //	�޸��г�����
	EVENT_LAYOUTTERM_MANUAL_OPER,//�˹���������    
	EVENT_LAYOUTTERM_RBCTRAIN_OPER, //���κŲ��� 
	EVENT_LAYOUTTERM_STATIONCTCFLAG, //��վCTC������־����		
	EVENT_LAYOUTTERM_ROUTETRIG,//��·�˹�����
	EVENT_LAYOUTTERM_TRIGFLAGUPDATE,//�޸��Դ����
	
	//GPC�¼����� ռ��Ԥ��2000~2999
	EVENT_GPC_LOGIN=2000,					//����ͼ��½
	EVENT_GPC_SHIFT_BANBIE,					//����ͼ���Ӱ�
	EVENT_GPC_MODIFY_SIDE,					//�޸Ĺɵ�
	EVENT_GPC_SEND_STPLAN,					//�·��׶μƻ�
	EVENT_GPC_PASS_TO_STAY,					//ͨ���䵽��
	EVENT_GPC_STAY_TO_PASS,					//������ͨ��
	EVENT_GPC_STOP_ABS,						//����ͣ��
	EVENT_GPC_STOP_OUT_SIGNED,				//����ͣ��
	EVENT_GPC_CHANG_TRAINID,				//�޸ĳ���	
	EVENT_GPC_MODIFY_DEPART,				//�޸ķ���ʱ��
	EVENT_GPC_TRAIN_PLAN_PARA,				//�޸ļƻ��߲���
	EVENT_GPC_RECV_NEIBPLAN,				//����̨�ƻ�
	EVENT_GPC_SEND_NEIBPLAN,				//����̨�ƻ�
	EVENT_GPC_CHANGE_TRAIN_REAL,			//תʵ��
	EVENT_GPC_MOUSE_REAL,					//����յ�
	EVENT_GPC_NEW_TRAIN,					//�����г�
	EVENT_GPC_DELETE_TRAIN,					//ɾ���г�
	EVENT_GPC_ELE_ARM,						//�޸Ĺ����״̬
	EVENT_GPC_HELP_TRAIN,					//��Ԯ�г�
	EVENT_GPC_MODIFY_ABSLINE,				//�޸������߱�
	EVENT_GPC_NEW_MARK,						//����ʩ��
	EVENT_GPC_DELETE_MARK,					//ɾ��ʩ��
	EVENT_GPC_MODIFY_MARK,					//�޸�ʩ��
	EVENT_GPC_TRAIN_ATTR_SPACE,				//�޸��г�����
	EVENT_GPC_INHIBIT_TRAINS_MEET,			//�޸��г�����
	EVENT_GPC_MODIFY_TRAIN_TIME,			//�г�ʱ���޸�
	EVENT_GPC_ZHEFAN_TRAIN,					//�޸��۷�����
	EVENT_GPC_HAT_TRAIN,					//�޸Ĵ�ñ����
		
};

enum EVENT_KEY_ID
{
	NULL_KEY_ID,
	KeyTrainIndex,            //�ƻ�INDEX      
	KeyTrainKey,				//�г�KEY                      
	KeyStationId,             //��վվ��                      
	KeyConId,                 //�������κ�                        
	KeySideId,                //�ɵ����,��STATIONSIDE����һ��
	KeyEntryId,               //��ں�                        
	KeyExitId,                //���ں�                      
	KeyArriveTime,            //����ʱ��                      
	KeyDepartTime,            //����ʱ��                      
	KeyPhaseId,               //�׶ε�                        
	KeyRouteStatus,           //��·״̬
	KeyShiftId,               //���    
	KeySourceEntity,          //�����Դ
	KeyDestEntity,            //���ȥ��
	KeyTrainNo,               //���κ�
	KeyLocomotiveNo,          //������
	KeyUserName,              //�û���ʵ����
	KeyUserId,	               //�û�����    
	KeyDcmdNo,                //���������
	KeyDcmdKey,               //��������ؼ���
	KeyRecvStatus,            //����״̬
	KeySourceType,			 //��Դ��GPC/���ɻ�/stpc/manual
	KeyRecindex,					
	KeyStationName,			  //վ��
	KeyArrTrainNo,			  //���ﳵ��
	KeyDptTrainNo,			  //��������
	KeyRouteType,             //��·���ͣ�1:�ӳ�2:����3:ͨ����
	KeyRouteFlag,				// ��·�����־(bit0:�г�������־, bit1:��·������־, bit2:��·��·������־)
	KeyMinSwitch,				// ��·;����С�޲��	
	KeyMinDypassSwitch,		// ��·;��������С�޲��
	KeyFromStationId,             //ǰ����վվ��                      
	KeyToStationId,             //�󷽳�վվ��                        
	KeyDelayTime,             //�����   
	KeyAlarmId,             //����id
	KeyFlowId,				//����id
	KeyFlowName,			//������
	KeyFlowTskId,          //����id
	KeyFlowTskName,        //������
	KeyFlowTskStatus,     //����״̬
	KeyVerbose,                   //��ϸ����
	
	//layoutterm
	KeyZoneID,//�����
	KeyStationMode,//վ��ģʽ0��ɢ����1������ȫ����2��վ��ȫ����3�ǳ�վ��4�������Ŀ���
	KeyRouteMode,//��·����ģʽ,0�ƻ�����;1�洢��·;2��ֹ�Զ�
	KeyDmisID,//dims��
	KeyIconName,//�豸����
	KeyOperType,//0��ӣ�1�޸ģ�2ɾ��
	KeyTrainType,//�������ͣ�0�г���1������3����
	KeyTrainPos,//0վ�ڣ�1����
	KeyWindowID,//���δ�id
	KeyWindowName,//���δ�����
	KeyStopInfo,//ͣ�ȱ�ǣ�1����,2ͣ�ȣ�
	KeyTrainRate,//�г�����
	KeyManualOperID,//�˹��������
	KeyManualOperType,//�˹���������, �μ�ctc_defines.h��MANUAL_OPER_TYPE���Ͷ���
	KeyDeviceType,//�豸����
	KeyOperTime,//����ʱ��
	KeyOperPara,//��������
	KeyForceFlag,//1:ǿ��ִ��(�����������߼����); 0:�����վϸ�ȼ��,
	KeyUniqueID,//RBC�ṩ���г�Ψһʶ��ţ�
	KeyRBCOper,//�������ͣ�0��ѯ��1ɾ��
	KeyCTCFlag ,//0�ƻ����ƣ�1�Ǽƻ�����
	KeyRouteHandle,//��·���DWORD����
	KeyListID,//��·���к�DWORD����
	KeyTrigTime,//����ʱ��
	KeyRouteForceFlag,//�Ƿ�ǿ�ƴ��� 0:ǿ�ƣ� 1����ǿ��
	KeyTrigFlag,//�������(0:�˹�����, 1:�Զ�����)

	//tsrterm
	KeyTsrCmdNo,//tsr�����    
	KeyTsrCmdOperID,//��������,��ʼ��1,����2,����3,����4
	KeyReason,// ����ԭ��(1:ʩ��; 2:����ѩ; 3:ͻ���ֺ�);
	KeyTsrNum,//TSR�����
	KeyTsrLineNo,//Tsr��·��
	KeyStartNow,//�Ƿ�������ʼ
	KeyEndless,//�Ƿ�������Ч
	KeyStartTime,//
	KeyEndTime,//
	KeySpeed,//����ֵ
	KeyTsrResult,//1:TSRͨ�Ż����յ�,2��TSRͨ�Ż��ѷ�����TSR������,3��TSR�������ѳɹ�����,4��TSR���������ش���,5��TSR��������Ӧ��ʱ,6��TSR����������Ӧ,7��TSRͨ�Ż��ڲ����󲻽�������
	KeyKmsysCode,//�����ϵ
	KeyKCL,//������
	KeyKmValue,//�����ֵ
	//dcmd
	KeyDcmdID,//��������Ψһid
	KeyDcmdCreateTime,//����ʱ��
	KeyDcmdCreateEntityID,//����ʵ���
	KeyDcmdType,//������������ 0��ͨ���1�п����2ʩ�����3ȡ�����
	KeySaveType,//������������� 1ֻ�洢����,2ֻ�洢���ݿ⣬3�洢���غ����ݿ�
	KeyConName,//������������
	KeyDcmdSendResult,//���ͽ��
	KeySendTime,//����ʱ��
	KeyReceiveTime,//����ʱ��
	KeySourceEntityID,//Դʵ���
	KeySourceName,//Դ����
	KeySignTime,//ǩ��ʱ��
	KeySignName,//ǩ��������
	KeySignEntityID,//ǩ��ʵ���
	KeySignResult,//ǩ�ս��1ǩ�գ�2��ǩ��3��ǩ
	KeyFeedBackEntityID,//��ִ��Դʵ���
	KeyFeedBackName,//��ִ��Դ����
	KeyFeedBackType,//���ͻ�ִ��Ϣ���ն����ͣ�0��վ��1����̨��2ֵ�����Σ�3������4�ӿڻ���
	KeyFeedBackResult,//0���գ�1ǩ�գ�2��ǩ   
		
	KeyLoginTime,				//��½ʱ��
	KeyOrlSideId,				//ԭ�ɵ�
	KeyOrlBusinessTime,			//ԭӪҵʱ��
	KeyBusinessTime,			//Ӫҵʱ��
	KeyOrlLineNo,				//ԭ�����߱�
	KeyLineNo,					//�����߱�
	KeyLocoStopStation,			//����ͣ����վ
	KeyOldTrainNo,				//ԭ�г����κ�
	KeyNeibConId,				//��̨��������
	KeyEleArm,					//����������
	KeyEleArmStatus,			//���������޵�״̬
	KeyHelpTrain,				//��Ԯ�г�
	KeyOrlDepartMoment,			//ԭ����ʱ��
	KeyDepartMoment,			//����ʱ��
	KeyMarkType,				//ʩ������
	KeyMarkStartTime,			//ʩ����ʼʱ��
	KeyMarkEndTime,				//ʩ������ʱ��
	KeyMarkAbsNo,				//��������
	KeyMarkSideNo,				//�����ɵ�
	KeyTrainSpaceInfo,			//������Ϣ
	KeyTrainMeetInfo,			//������Ϣ
	KeyRankType,				//�г��������� 1������ 2������ 3���ص� 4������
	KeyArrOrlSideId,
	KeyDptOrlSideId,
	KeyArrSideId,
	KeyDptSideId,

	KeyRouteAttribute,			//��·���ͣ��μ�msg_def.h��ROUTE_ATTRIBUTE���Ͷ���
	KeyRouteTrigStatus,			//��·����״̬���μ�ctc_defines.h��ROUTE_TRIG_STATUS���Ͷ���
};

enum EVENT_RANK
{
	IMPORTANT_EVENT,
	NORMAL_EVENT,
	MISC_EVENT
};

#define EXTERN_EVENT_DEFINE_INT(name)       extern void EventSet##name(MsgEventData&, int );           extern int EventGet##name(MsgEventData& );
#define EXTERN_EVENT_DEFINE_STR(name)       extern void EventSet##name(MsgEventData&, const char* );   extern const char* EventGet##name(MsgEventData& );

struct EVENT_KEY_DATA					// �ؼ���
{
	EVENT_KEY_ID    sKeyId;		        //  �¼��ؼ���
	char			sKeyWord[64];		// �¼��ؼ�������
};

// �¼�ע����Ϣ
class AFX_EXT_CLASS MsgEventReg : public BaseMsg
{
	DECLARE_CLBTH(MsgEventReg)

public:
	MsgEventReg();
	virtual ~MsgEventReg();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	ENTITY_ID		nSrcEntityID;		// �¼�Դʵ���
	EVENT_ID		nEventID;			// �¼�����ID
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};
	ReqStatus  nStatus;					// ע���ر�ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgEventReg)

// �¼�������Ϣ
class AFX_EXT_CLASS MsgEventData : public BaseMsg
{
	DECLARE_CLBTH(MsgEventData)
		
public:
	MsgEventData();
	MsgEventData(EVENT_ID e);
	virtual ~MsgEventData();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	EVENT_ID		nEventID;			// �¼�����ID

	ENTITY_ID		nSrcEntityID;		// �¼�Դʵ���
	char			sSrcName[32];		// �¼�Դ����
	I_TIME			tmEventTime;		// �¼�����ʱ��
	char			sEventName[32];		// �¼���������
	int				nEventIdx;			// �¼����
	int             nDataLen;
	char			*nzDataBuffer;		// �¼���������	
	EVENT_RANK		nEventRank	;		// �¼�����
	short			nKeyCount	;		// �¼��ؼ��ָ���
	IBYTE			nKeySpace	;		// �¼��ؼ��ַ����С
	char			sEventDesc[256];	// �¼�����

	EVENT_KEY_DATA	*struKey;		// �¼��ؼ���

	I_BOOL SetData(const char* buffer);
	const char* GetData() const;

	I_BOOL IsKeyHeld(const EVENT_KEY_DATA ek) const;
	short SetKey(EVENT_KEY_ID key, const char* ek);		//�ɹ���������KEY������ʧ��-0
	short SetKey(EVENT_KEY_ID key, int ek);		//�ɹ���������KEY������ʧ��-0
	const char* GetKey(EVENT_KEY_ID key) const;	//�ɹ����عؼ���ֵ�� ʧ�ܷ���NULL
	const char* GetEventName();	//�ɹ������¼����ƣ� ʧ�ܷ���NULL
	void SetEventName();//�����¼��� by envent id.
};
DECLARE_SERIALIZE_OPERATOR(MsgEventData)

// �¼���ѯ��Ϣ
class AFX_EXT_CLASS MsgEventReq : public BaseMsg
{
	DECLARE_CLBTH(MsgEventReq)
		
public:
	MsgEventReq();
	virtual ~MsgEventReq();
	short SetKey(EVENT_KEY_ID key, const char* ek);		//�ɹ���������KEY������ʧ��-0
	short SetKey(EVENT_KEY_ID key, int ek);		//�ɹ���������KEY������ʧ��-0
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	ENTITY_ID		nSrcEntityID;		// �¼�Դʵ���
	EVENT_ID		nEventID;			// �¼�����ID
	I_TIME			nStartTime;			// ��ѯʱ�䷶Χ��ʼ
	I_TIME			nEndTime;			// ��ѯʱ�䷶Χ����
	EVENT_RANK		nEventRank	;		// �¼�����(����ͬ�����߼���ģ�
	IBYTE			nKeyCount	;		// �¼��ؼ��ָ���
	IBYTE			nKeySpace	;		// �¼��ؼ��ַ����С
	EVENT_KEY_DATA	*struKey;		// �¼��ؼ���
};
DECLARE_SERIALIZE_OPERATOR(MsgEventReq)


EXTERN_EVENT_DEFINE_INT(KeyTrainIndex);
EXTERN_EVENT_DEFINE_INT(KeyStationId);
EXTERN_EVENT_DEFINE_INT(KeyConId);
EXTERN_EVENT_DEFINE_INT(KeySideId);
EXTERN_EVENT_DEFINE_INT(KeyEntryId);
EXTERN_EVENT_DEFINE_INT(KeyExitId);
EXTERN_EVENT_DEFINE_INT(KeyArriveTime);
EXTERN_EVENT_DEFINE_INT(KeyDepartTime);
EXTERN_EVENT_DEFINE_INT(KeyPhaseId);
EXTERN_EVENT_DEFINE_INT(KeyRouteStatus);
EXTERN_EVENT_DEFINE_INT(KeyShiftId);
EXTERN_EVENT_DEFINE_INT(KeySourceEntity);
EXTERN_EVENT_DEFINE_INT(KeyDestEntity);
EXTERN_EVENT_DEFINE_INT(KeySourceType);
EXTERN_EVENT_DEFINE_INT(KeyRecindex);
//GPC
EXTERN_EVENT_DEFINE_INT(KeyLoginTime);
EXTERN_EVENT_DEFINE_INT(KeyOrlSideId);
EXTERN_EVENT_DEFINE_INT(KeyOrlBusinessTime);
EXTERN_EVENT_DEFINE_INT(KeyBusinessTime);
EXTERN_EVENT_DEFINE_INT(KeyLocoStopStation);
EXTERN_EVENT_DEFINE_INT(KeyRouteFlag);
EXTERN_EVENT_DEFINE_INT(KeyEleArmStatus);
EXTERN_EVENT_DEFINE_INT(KeyNeibConId);
EXTERN_EVENT_DEFINE_INT(KeyOrlLineNo);
EXTERN_EVENT_DEFINE_INT(KeyLineNo);
EXTERN_EVENT_DEFINE_INT(KeyFromStationId);
EXTERN_EVENT_DEFINE_INT(KeyToStationId);

EXTERN_EVENT_DEFINE_STR(KeyTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyLocomotiveNo);
EXTERN_EVENT_DEFINE_STR(KeyUserName);
EXTERN_EVENT_DEFINE_STR(KeyUserId);	
EXTERN_EVENT_DEFINE_STR(KeyDcmdNo);
EXTERN_EVENT_DEFINE_STR(KeyDcmdKey);
EXTERN_EVENT_DEFINE_STR(KeyRecvStatus);
EXTERN_EVENT_DEFINE_STR(KeyStationName);
EXTERN_EVENT_DEFINE_STR(KeyArrTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyDptTrainNo);
//GPC
EXTERN_EVENT_DEFINE_STR(KeyHelpTrain);
EXTERN_EVENT_DEFINE_STR(KeyOrlTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyEleArm);
EXTERN_EVENT_DEFINE_STR(KeyOrlDepartMoment);
EXTERN_EVENT_DEFINE_STR(KeyDepartMoment);
EXTERN_EVENT_DEFINE_STR(KeyMarkType);
EXTERN_EVENT_DEFINE_STR(KeyTrainSpaceInfo);
EXTERN_EVENT_DEFINE_STR(KeyTrainMeetInfo);
EXTERN_EVENT_DEFINE_STR(KeyMarkStartTime);
EXTERN_EVENT_DEFINE_STR(KeyMarkEndTime);
EXTERN_EVENT_DEFINE_STR(KeyMarkAbsNo);
EXTERN_EVENT_DEFINE_STR(KeyMarkSideNo);


CLBTH_NAMESPACE_END

#endif

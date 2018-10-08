#define EXTERN_LINKAGE_DEFINE
#include <stdio.h>
#include "msg_event.h"

CLBTH_NAMESPACE_BEGIN

#define IMPLEMENT_EVENT_DEFINE_INT(name)              \
void EventSet##name(MsgEventData& msg, int data)      \
{                                                     \
	msg.SetKey(name, data);                         \
}                                                     \
int EventGet##name(MsgEventData& msg)                 \
{                                                     \
	return atol(msg.GetKey(name));                  \
}


#define IMPLEMENT_EVENT_DEFINE_STR(name)                   \
void EventSet##name(MsgEventData& msg, const char* data)   \
{                                                          \
	msg.SetKey(name, data);                              \
}                                                          \
const char* EventGet##name(MsgEventData& msg)              \
{                                                          \
	return msg.GetKey(name);                             \
}
	                              

IMPLEMENT_EVENT_DEFINE_INT(KeyTrainIndex);
IMPLEMENT_EVENT_DEFINE_INT(KeyTrainKey);
IMPLEMENT_EVENT_DEFINE_INT(KeyStationId);
IMPLEMENT_EVENT_DEFINE_INT(KeyConId);
IMPLEMENT_EVENT_DEFINE_INT(KeySideId);
IMPLEMENT_EVENT_DEFINE_INT(KeyEntryId);
IMPLEMENT_EVENT_DEFINE_INT(KeyExitId);
IMPLEMENT_EVENT_DEFINE_INT(KeyArriveTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyDepartTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyPhaseId);
IMPLEMENT_EVENT_DEFINE_INT(KeyRouteStatus);
IMPLEMENT_EVENT_DEFINE_INT(KeyShiftId);
IMPLEMENT_EVENT_DEFINE_INT(KeySourceEntity);
IMPLEMENT_EVENT_DEFINE_INT(KeyDestEntity);
IMPLEMENT_EVENT_DEFINE_INT(KeySourceType);
IMPLEMENT_EVENT_DEFINE_INT(KeyRecindex);
//GPC
IMPLEMENT_EVENT_DEFINE_INT(KeyLoginTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlSideId);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlBusinessTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyBusinessTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyLocoStopStation);
IMPLEMENT_EVENT_DEFINE_INT(KeyRouteFlag);
IMPLEMENT_EVENT_DEFINE_INT(KeyEleArmStatus);
IMPLEMENT_EVENT_DEFINE_INT(KeyNeibConId);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlLineNo);
IMPLEMENT_EVENT_DEFINE_INT(KeyLineNo);
IMPLEMENT_EVENT_DEFINE_INT(KeyFromStationId);
IMPLEMENT_EVENT_DEFINE_INT(KeyToStationId);


IMPLEMENT_EVENT_DEFINE_STR(KeyTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyLocomotiveNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyUserName);
IMPLEMENT_EVENT_DEFINE_STR(KeyUserId);	
IMPLEMENT_EVENT_DEFINE_STR(KeyDcmdNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyDcmdKey);
IMPLEMENT_EVENT_DEFINE_STR(KeyRecvStatus);
IMPLEMENT_EVENT_DEFINE_STR(KeyStationName);
IMPLEMENT_EVENT_DEFINE_STR(KeyArrTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyDptTrainNo);
//GPC
IMPLEMENT_EVENT_DEFINE_STR(KeyHelpTrain);
IMPLEMENT_EVENT_DEFINE_STR(KeyEleArm);
IMPLEMENT_EVENT_DEFINE_STR(KeyOrlDepartMoment);
IMPLEMENT_EVENT_DEFINE_STR(KeyDepartMoment);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkType);
IMPLEMENT_EVENT_DEFINE_STR(KeyTrainSpaceInfo);
IMPLEMENT_EVENT_DEFINE_STR(KeyTrainMeetInfo);
IMPLEMENT_EVENT_DEFINE_STR(KeyOldTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkStartTime);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkEndTime);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkAbsNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkSideNo);

/////////////// �¼�ע����Ϣ
IMPLEMENT_CLBTH(MsgEventReg, MSGID_MsgEventReg)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventReg)
MsgEventReg::MsgEventReg()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgEventReg::~MsgEventReg()
{
	
}
HRCODE MsgEventReg::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nEventID << nStatus;		
	}
	else
	{
		int t;
		ar >> nSrcEntityID >> t;			nEventID = (EVENT_ID)t;	
		ar >> t;							nStatus = (ReqStatus)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventReg::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(nStatus);
}

/////////////// �¼�������Ϣ
IMPLEMENT_CLBTH(MsgEventData, MSGID_MsgEventData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventData)
MsgEventData::MsgEventData(EVENT_ID e)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nDataLen = 0;
	nzDataBuffer = NULL;
	struKey = NULL;
	sSrcName[0] = sEventName[0] = sEventDesc[0] = 0;
	nEventRank = NORMAL_EVENT;
	nEventID = e;
}
MsgEventData::MsgEventData()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nDataLen = 0;
	nzDataBuffer = NULL;
	nKeyCount = nKeySpace = 0;
	struKey = NULL;
	sSrcName[0] = sEventName[0] = sEventDesc[0] = 0;
	nEventRank = NORMAL_EVENT;
	nEventID = NULL_EVENT_ID;
}
MsgEventData::~MsgEventData()
{	
	if(nzDataBuffer != NULL)
		delete []nzDataBuffer;
	if(struKey != NULL)
		delete []struKey;
}
HRCODE MsgEventData::doSerialize(Archive& ar)
{
	int t;
	if(ar.isStoring())
	{
		if (NULL_EVENT_ID == nEventID)	return HRCODE(HRCODE::HC_FAIL);
		if (0 == sEventName[0])	SetEventName();
		if (0 == sEventName[0])	return HRCODE(HRCODE::HC_FAIL);
		if (0 == sEventDesc[0])
			strcpy(sEventDesc, sEventName);
		ar << nSrcEntityID << nEventID << tmEventTime << nEventIdx << nEventRank;	
		AR_WRITE_STRING(sSrcName, t);	
		AR_WRITE_STRING(sEventName, t);	
		AR_WRITE_STRING(sEventDesc, t);	
		if(nDataLen>=MAX_EVENT_DATA_LEN) return HRCODE(HRCODE::HC_FAIL);
		ar << nDataLen;
		ar.write(nzDataBuffer, nDataLen);

		ar << nKeyCount;
		for (int i=0; i<nKeyCount; i++)
		{
			ar << struKey[i].sKeyId;
			AR_WRITE_STRING(struKey[i].sKeyWord, t);
		}
	}
	else
	{
		ar >> nSrcEntityID >> t >> tmEventTime >> nEventIdx;		
		nEventID = (EVENT_ID)t;			
		if (NULL_EVENT_ID == nEventID)	return HRCODE(HRCODE::HC_FAIL);
		ar >> t;
		nEventRank = (EVENT_RANK)t;	
		AR_READ_STRING(sSrcName, t);
		AR_READ_STRING(sEventName, t);
		AR_READ_STRING(sEventDesc, t);	

		ar >> t;
		if(t>=MAX_EVENT_DATA_LEN) return HRCODE(HRCODE::HC_FAIL);
		if(nzDataBuffer != NULL) delete []nzDataBuffer;
		nzDataBuffer = new char[t+1];
		ar.read(nzDataBuffer, t);
		nzDataBuffer[t] = 0;
		nDataLen = t;
		
		ar >> nKeyCount;
		if(nKeyCount>MAX_EVENT_KEY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);		
		nKeySpace = nKeyCount;
		struKey = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
		{
			ar >> t;
			struKey[i].sKeyId = (EVENT_KEY_ID)t;
			AR_READ_STRING(struKey[i].sKeyWord, t);
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventData::countObjectLength()
{
	//return 0;
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(tmEventTime)+sizeof(nEventIdx)+sizeof(nEventRank)+sizeof(nKeyCount)+sizeof(sSrcName)+sizeof(sEventName)+sizeof(sEventDesc)+nKeyCount*(sizeof(struKey[0].sKeyId)+sizeof(struKey[0].sKeyWord))+sizeof(nDataLen)+nDataLen;
}

I_BOOL MsgEventData::SetData(const char* buffer)
{
	if ( NULL == buffer || strlen(buffer) >= MAX_EVENT_DATA_LEN)
	{
		nDataLen = 0;
		return IFALSE;
	}
	else 
	{
		if(nzDataBuffer != NULL) delete []nzDataBuffer;
		nDataLen = strlen(buffer);

		nzDataBuffer = new char[nDataLen+1];
		memcpy(nzDataBuffer, buffer, nDataLen);
		nzDataBuffer[nDataLen] = 0;
		return ITRUE;
	}
}
const char* MsgEventData::GetData() const
{
	return nzDataBuffer;
}

I_BOOL MsgEventData::IsKeyHeld(const EVENT_KEY_DATA ek) const
{
	for (int i=0; i<nKeyCount; i++)
	{
		if (ek.sKeyId == struKey[i].sKeyId
			&& strcmp(ek.sKeyWord, struKey[i].sKeyWord)==0)
			return ITRUE;
	}
	return IFALSE;
}
short MsgEventData::SetKey(EVENT_KEY_ID key, const char* ek)
{
	if (nKeyCount >= MAX_EVENT_KEY_COUNT)
		return 0;
	
	if (nKeyCount == nKeySpace)
	{
		nKeySpace += 16;
		EVENT_KEY_DATA* tmp_key = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
			tmp_key[i] = struKey[i];
		delete[] struKey;
		struKey = tmp_key;
	}

	struKey[nKeyCount].sKeyId = key;
	memset(struKey[nKeyCount].sKeyWord, 0, sizeof(struKey[nKeyCount].sKeyWord));
	strncpy(struKey[nKeyCount].sKeyWord, ek, sizeof(struKey[nKeyCount].sKeyWord)-1);
	return ++nKeyCount;
}
short MsgEventData::SetKey(EVENT_KEY_ID key, int ek)
{
	char x[64]; sprintf(x, "%d", ek);
	return SetKey(key, x);
}
const char* MsgEventData::GetKey(EVENT_KEY_ID key) const
{
	for (int i=0; i<nKeyCount; i++)
	{
		if (key == struKey[i].sKeyId)
			return struKey[i].sKeyWord;
	}
	return NULL;
}
const char* MsgEventData::GetEventName()
{
	return sEventName;

}
void MsgEventData::SetEventName()
{
	switch (nEventID)
	{
	case EVENT_COMMON_APP_STOP: //�����˳������
		strcpy(sEventName,"�����˳�");
		break;
	case EVENT_COMMON_APP_START:                    //�������������    
		strcpy(sEventName,"��������");
		break;
	case EVENT_COMMON_USER_LOGIN:                   //���û���¼ʱ����        
		strcpy(sEventName,"�û���¼");
		break;
	case EVENT_COMMON_USER_LOGOUT:                  //���û�ע��ʱ����   
		strcpy(sEventName,"�û�ע��");
		break;
	case EVENT_STPC_ACK_DEPART:                     //ͬ����վ����   
		strcpy(sEventName,"ͬ����վ����");
		break;
	case EVENT_STPC_REQ_DEPART:                     //����վ��������      
		strcpy(sEventName,"����վ��������");
		break;
	case EVENT_STPC_INFORM_DEPART:                  //����վ����Ԥ��  
		strcpy(sEventName,"����վ����Ԥ��");
		break;
	case EVENT_STPC_ACK_SIDE:                       //ȷ�Ͻӳ��ɵ�      
		strcpy(sEventName,"ȷ�Ͻӳ��ɵ�");
		break;
	case EVENT_STPC_REPORT_TRAIN_ARRIVE:            //ȷ���г�����,�˹����� 
		strcpy(sEventName,"�г�����");
		break;
	case EVENT_STPC_REPORT_TRAIN_DEPART:            //ȷ���г�����,�˹�����   
		strcpy(sEventName,"�г�����");
		break;
	case EVENT_STPC_ACK_SHUNT_STOP:                 //ȷ�ϵ�����ҵֹͣ     
		strcpy(sEventName,"ȷ�ϵ�����ҵֹͣ");
		break;
	case EVENT_STPC_ACK_TRAIN_INT:                  //ȷ���г�����       
		strcpy(sEventName,"ȷ���г�����");
		break;
	case EVENT_STPC_SIGN_PLAN:                      //ǩ�ս׶μƻ�    
		strcpy(sEventName,"ǩ�ս׶μƻ�");
		break;
	case EVENT_STPC_RECV_PLAN:                      //�յ��׶μƻ�    
		strcpy(sEventName,"�յ��׶μƻ�");
		break;
	case EVENT_STPC_SIGN_PLANNOTE:                  //ǩ�ս׶μ���  
		strcpy(sEventName,"ǩ�ս׶μ���");
		break;
	case EVENT_STPC_RECV_PLANNOTE:                  //�յ��׶μ���   
		strcpy(sEventName,"�յ��׶μ���");
		break;
	case EVENT_STPC_SIGN_DCMD:                      //ǩ�յ�������  
		strcpy(sEventName,"ǩ�յ�������");
		break;
	case EVENT_STPC_RECV_DCMD:                      //�յ���������   
		strcpy(sEventName,"�յ���������");
		break;
	case EVENT_STPC_SEND_DCMD_REQ:                  //�����������       
		strcpy(sEventName,"�����������");
		break;
	case EVENT_STPC_SEND_RADIO_DCMD:                //�������ߵ�������
		strcpy(sEventName,"�������ߵ�������");
		break;
	case EVENT_STPC_RECV_RADIO_DCMD_ACK:            //���ߵ����������״̬   
		strcpy(sEventName,"���ߵ����������״̬");
		break;
	case EVENT_STPC_REPORT_CURRENT_CAR:             //�ϱ����ڳ�   
		strcpy(sEventName,"�ϱ����ڳ�");
		break;
	case EVENT_STPC_REPORT_TRAIN_SUBAO:             //�ϱ��г��ٱ�   
		strcpy(sEventName,"�ϱ��г��ٱ�");
		break;
	case EVENT_LIRC_REPORT_ROUTE_STATUS:            //��⵽��·����,�źſ��� 
		strcpy(sEventName,"��⵽��·����,�źſ���");
		break;
	case EVENT_LIRC_REPORT_TRAIN_ENTERING:          //��⵽�г�ѹ����վ�źŻ�
		strcpy(sEventName,"��⵽�г�ѹ����վ�źŻ�");
		break;
	case EVENT_LIRC_REPORT_TRAIN_EXITING:           //��⵽�г�ѹ����վ�źŻ�
		strcpy(sEventName,"��⵽�г�ѹ����վ�źŻ�");
		break;
	case EVENT_LIRC_REPORT_SIDE_OCC:                //��⵽�ɵ�ռ��   
		strcpy(sEventName,"��⵽�ɵ�ռ��");
		break;
	case EVENT_LIRC_REPORT_SIDE_UNOCC:              //��⵽�ɵ�����  
		strcpy(sEventName,"��⵽�ɵ�����");
		break;
	case EVENT_STPC_CHOOSE_SIDE:                    //ѡ��ӳ��ɵ�      added by wuchunbo 
		strcpy(sEventName,"ѡ��ӳ��ɵ�");
		break;
	case EVENT_STPC_AGREE_OPEN_ARSIG:               //�����Žӳ��ź�
		strcpy(sEventName,"�����Žӳ��ź�");
		break;
	case EVENT_STPC_AGREE_OPEN_DPTSIG:              //�����ŷ����ź�
		strcpy(sEventName,"�����ŷ����ź�");
		break;
	case EVENT_STPC_PREPARE_OPEN_ARSIG:             //׼�����Žӳ��ź�
		strcpy(sEventName,"׼�����Žӳ��ź�");
		break;
	case EVENT_STPC_PREPARE_OPEN_DPTSIG:             //׼�����ŷ����ź�
		strcpy(sEventName,"׼�����ŷ����ź�");
		break;
	case EVENT_STPC_TRAIN_APPROACH:					//�г��ӽ�
		strcpy(sEventName,"�г��ӽ�");
		break;
	case EVENT_STPC_DPT_CONDITION_OK:				//������������
		strcpy(sEventName,"������������");
		break;
	case EVENT_STPC_OPEN_DPTSIG_DONE:                //�����ź����
		strcpy(sEventName,"�����ź����");
		break;
	case EVENT_STPC_TRAIN_CONTACTED:					//�����г�
		strcpy(sEventName,"�����г�");
		break;
	case EVENT_STPC_PREPARE_CONTACT_TRAIN:			//׼���ӳ�
		strcpy(sEventName,"׼���ӳ�");
		break;
	case EVENT_LIRC_REPORT_TRAIN_ARRIVE:			//�г�����(�Զ�����
		strcpy(sEventName,"�г�����(�Զ�����)");
		break;
	case EVENT_LIRC_REPORT_TRAIN_DEPART:			//�г�����(�Զ�����
		strcpy(sEventName,"�г�����(�Զ�����)");
		break;
	case EVENT_LIRC_AR_ROUTE_CREATE:			//�ӳ���·����
		strcpy(sEventName,"�ӳ���·����");
		break;
	case EVENT_LIRC_AR_ROUTE_CANCEL:			//�ӳ���·ȡ��
		strcpy(sEventName,"�ӳ���·ȡ��");
		break;
	case EVENT_LIRC_PS_ROUTE_CREATE:			//ͨ����·����
		strcpy(sEventName,"ͨ����·����");
		break;
	case EVENT_LIRC_PS_ROUTE_CANCEL:			//ͨ����·ȡ��
		strcpy(sEventName,"ͨ����·ȡ��");
		break;
	case EVENT_LIRC_DP_ROUTE_CREATE:			//������·����
		strcpy(sEventName,"������·����");
		break;
	case EVENT_LIRC_DP_ROUTE_CANCEL:			//������·ȡ��
		strcpy(sEventName,"������·ȡ��");
		break;
	case EVENT_STPC_AR_ROUTE_CREATING:
		strcpy(sEventName,"����ӳ���·");
		break;
	case EVENT_STPC_DP_ROUTE_CREATING:
		strcpy(sEventName,"��������·");
		break;
	case EVENT_STPC_PREPARE_DPT:							//׼������
		strcpy(sEventName,"׼������");
		break;
	case EVENT_STPC_ACK_SIDE_CANCEL:                     //�ӳ��ɵ�������ӳ�����
		strcpy(sEventName,"�ӳ��ɵ�������ӳ�����");
		break;
	case EVENT_LIRC_REPORT_SIDE_WHOLE_OCC:							//��⵽��ȫ�ɵ�ռ��
		strcpy(sEventName,"��⵽��ȫ�ɵ�ռ��");
		break;
	case EVENT_STPC_REPORT_TRAIN_PASS:							//�г�ͨ��,�˹�����
		strcpy(sEventName,"�г�ͨ��(�˹�����)");
		break;
	case EVENT_LIRC_REPORT_TRAIN_PASS:							//�г�ͨ��,�Զ�����
		strcpy(sEventName,"�г�ͨ��(�Զ�����)");
		break;
	case EVENT_STPC_OPEN_DPTSIG_CANCEL:          //�����㷢������
		strcpy(sEventName,"�����㿪���ź�����");
		break;
	case EVENT_COMMON_TRAIN_ALARM:				//�г�����  
		strcpy(sEventName,"�г�����");
		break;
	case EVENT_COMMON_SYS_ALARM:				//ϵͳ����  
		strcpy(sEventName,"ϵͳ����");
		break;
	case EVENT_STPC_CHECK_LINE:                  //�����·
		strcpy(sEventName,"�����·");
		break;
	case EVENT_STPC_ST_HOME_RT:					 //׼���ӳ���·
		strcpy(sEventName,"׼���ӳ���·");
		break;
	case EVENT_STPC_HOME_TRAIN:					 //�ӳ�
		strcpy(sEventName,"�ӳ�");
		break;
	case EVENT_STPC_SET_DEPART_RT:				 //׼��������·
		strcpy(sEventName,"׼��������·");
		break;
	case EVENT_STPC_SET_TOKEN:					 //����ƾ֤
		strcpy(sEventName,"����ƾ֤");
		break;
	case EVENT_STPC_ACK_DPT_CDT:				 //ȷ�Ϸ�������
		strcpy(sEventName,"ȷ�Ϸ�������");
		break;
	case EVENT_STPC_ACK_DPT_TRAIN:				 //����
		strcpy(sEventName,"����");
		break;
	case EVENT_STPC_NEIB_AR:					 //��վ����
		strcpy(sEventName,"��վ����");
		break;
	case EVENT_STPC_OP_RECORD:				   //��վ�˹������¼�
		strcpy(sEventName,"�˹�����");
		break;
	case EVENT_STPC_FLOW_STATUS:				   //��վ���̱仯�¼�
		strcpy(sEventName,"���̱仯");
		break;
		
	case EVENT_GPC_LOGIN:							//����ͼ��½
		strcpy(sEventName,"����ͼ��½");
		break;
	case EVENT_GPC_SHIFT_BANBIE:					 //����ͼ���Ӱ�
		strcpy(sEventName,"����ͼ���Ӱ�");
		break;
	case EVENT_GPC_MODIFY_SIDE:						 //�޸Ĺɵ�
		strcpy(sEventName,"�޸Ĺɵ�");
		break;
	case EVENT_GPC_SEND_STPLAN:						 //�·��׶μƻ�
		strcpy(sEventName,"�·��׶μƻ�");
		break;
	case EVENT_GPC_PASS_TO_STAY:					 //ͨ���䵽��
		strcpy(sEventName,"ͨ���䵽��");
		break;
	case EVENT_GPC_STAY_TO_PASS:					 //������ͨ��
		strcpy(sEventName,"������ͨ��");
		break;
	case EVENT_GPC_STOP_ABS:						 //����ͣ��
		strcpy(sEventName,"����ͣ��");
		break;
	case EVENT_GPC_STOP_OUT_SIGNED:					 //����ͣ��
		strcpy(sEventName,"����ͣ��");
		break;
	case EVENT_GPC_CHANG_TRAINID:					 //�޸ĳ���
		strcpy(sEventName,"�޸ĳ���");
		break;
	case EVENT_GPC_MODIFY_DEPART:					 //�޸ķ���ʱ��
		strcpy(sEventName,"�޸ķ���ʱ��");
		break;
	case EVENT_GPC_TRAIN_PLAN_PARA:					 //�޸ļƻ��߲���
		strcpy(sEventName,"�޸ļƻ��߲���");
		break;
	case EVENT_GPC_RECV_NEIBPLAN:					 //����̨�ƻ�
		strcpy(sEventName,"����̨�ƻ�");
		break;
	case EVENT_GPC_SEND_NEIBPLAN:					 //����̨�ƻ�
		strcpy(sEventName,"����̨�ƻ�");
		break;
	case EVENT_GPC_CHANGE_TRAIN_REAL:				 //תʵ��
		strcpy(sEventName,"תʵ��");
		break;
	case EVENT_GPC_MOUSE_REAL:						 //����յ�
		strcpy(sEventName,"����յ�");
		break;
	case EVENT_GPC_NEW_TRAIN:						 //�����г�
		strcpy(sEventName,"�����г�");
		break;
	case EVENT_GPC_DELETE_TRAIN:					 //ɾ���г�
		strcpy(sEventName,"ɾ���г�");
		break;	
	case EVENT_GPC_ELE_ARM:							 //�޸Ĺ����״̬
		strcpy(sEventName,"�޸Ĺ����״̬");
		break;
	case EVENT_GPC_MODIFY_ABSLINE:					 //�޸������߱�
		strcpy(sEventName,"�޸������߱�");
		break;
	case EVENT_GPC_NEW_MARK:						 //����ʩ��
		strcpy(sEventName,"����ʩ��");
		break;
	case EVENT_GPC_DELETE_MARK:						 //ɾ��ʩ��
		strcpy(sEventName,"ɾ��ʩ��");
		break;
	case EVENT_GPC_MODIFY_MARK:						 //�޸�ʩ��
		strcpy(sEventName,"�޸�ʩ��");
		break;
	case EVENT_GPC_TRAIN_ATTR_SPACE:				 //�޸��г�����
		strcpy(sEventName,"�޸��г�����");
		break;
	case EVENT_GPC_INHIBIT_TRAINS_MEET:				 //�޸��г�����
		strcpy(sEventName,"�޸��г�����");
		break;
	case EVENT_GPC_MODIFY_TRAIN_TIME:				 //�г�ʱ���޸�
		strcpy(sEventName,"�г�ʱ���޸�");
		break;
	case	EVENT_DCMD_CREATE_COMMOND://	���������
		strcpy(sEventName,"���������");
		break;		
	case	EVENT_DCMD_SAVE_COMMOND://	���������
		strcpy(sEventName,"���������");
		break;			
	case	EVENT_DCMD_SEND_COMMOND://���������·�
		strcpy(sEventName,"���������·�");
		break;			
	case	EVENT_DCMD_RECV_COMMOND://	�����������
		strcpy(sEventName,"�����������");
		break;			
	case	EVENT_DCMD_SIGN_COMMOND://��������ǩ��
		strcpy(sEventName,"��������ǩ��");
		break;			
	case	EVENT_DCMD_LOGIN://���������¼
		strcpy(sEventName,"���������¼");
		break;	
	case	EVENT_DCMD_FEEDBACK_COMMOND://���������յ����ܻ�ִ
		strcpy(sEventName,"���������յ����ܻ�ִ");
		break;	
	case	EVENT_LAYOUTTERM_SM_REQUEST://��վ����ģʽ����
		strcpy(sEventName,"��վ����ģʽ����");
		break;	
	case	EVENT_LAYOUTTERM_RM_REQUEST:// ��վ��·ģʽ����
		strcpy(sEventName,"��վ��·ģʽ����");
		break;
	case	EVENT_LAYOUTTERM_TRAININDEX_OPER://���κŲ���
		strcpy(sEventName,"���κŲ���");
		break;	
	case	EVENT_LAYOUTTERM_TRAINSTOP://�����г�ͣ�ȡ�����
		strcpy(sEventName,"�����г�ͣ�ȡ�����");
		break;	     
	case	EVENT_LAYOUTTERM_TRAINATTR_OPER:// �޸��г�����
		strcpy(sEventName,"�޸��г�����");
		break;	
	case	EVENT_LAYOUTTERM_MANUAL_OPER://�˹���������
		strcpy(sEventName,"�˹���������");
		break;	    
	case	EVENT_LAYOUTTERM_RBCTRAIN_OPER://���κŲ���
		 strcpy(sEventName,"���κŲ���");
		 break;	 
	case	EVENT_LAYOUTTERM_STATIONCTCFLAG:// ��վCTC������־����
		strcpy(sEventName,"��վCTC������־����");
		break;
	case	EVENT_LAYOUTTERM_ROUTETRIG:// ��·�˹�����
		strcpy(sEventName,"��·�˹�����");
		break;
	case	EVENT_LAYOUTTERM_TRIGFLAGUPDATE:// �޸��Դ����
		strcpy(sEventName,"�޸��Դ����");
		break;
	case	EVENT_TSRTERM_CMD_OPER://�����������
		strcpy(sEventName,"�����������");
		break;
	case	EVENT_TSRTERM_CMD_RESULT://��������������
		strcpy(sEventName,"��������������");
		break;
	case	EVENT_LIRC_ROUTE_STATUS://��·״̬�仯
		strcpy(sEventName,"��·״̬�仯");
		break;
	default:
		strcpy(sEventName,"");
		break;
	}	
}
/////////////// �¼���ѯ��Ϣ
IMPLEMENT_CLBTH(MsgEventReq, MSGID_MsgEventReq)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventReq)
MsgEventReq::MsgEventReq()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nKeyCount = nKeySpace = 0;
	struKey = NULL;
	nEventRank = NORMAL_EVENT;
}
MsgEventReq::~MsgEventReq()
{	
	if(struKey != NULL)
		delete []struKey;
}
HRCODE MsgEventReq::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nEventID << nStartTime << nEndTime << nEventRank << nKeyCount;	
		for (int i=0; i<nKeyCount; i++)
		{
			ar << struKey[i].sKeyId;
			ar.write(struKey[i].sKeyWord, sizeof(struKey[i].sKeyWord));
		}
	}
	else
	{
		int t;
		ar >> nSrcEntityID >> t >> nStartTime >> nEndTime;	
		nEventID = (EVENT_ID)t;		
		ar >> t >> nKeyCount;
		nEventRank = (EVENT_RANK)t;	
		if(nKeyCount>MAX_EVENT_KEY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		nKeySpace = nKeyCount;
		struKey = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
		{
			ar >> t;
			struKey[i].sKeyId = (EVENT_KEY_ID)t;
			ar.read(struKey[i].sKeyWord, sizeof(struKey[i].sKeyWord));
		}	
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventReq::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(nStartTime)+sizeof(nEndTime)+sizeof(nEventRank)+sizeof(nKeyCount)+nKeyCount*(sizeof(struKey[0].sKeyId)+sizeof(struKey[0].sKeyWord));
}
short MsgEventReq::SetKey(EVENT_KEY_ID key, const char* ek)
{
	if (nKeyCount >= MAX_EVENT_KEY_COUNT)
		return 0;

	if (nKeyCount == nKeySpace)
	{
		nKeySpace += 16;
		EVENT_KEY_DATA* tmp_key = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
			tmp_key[i] = struKey[i];
		delete[] struKey;
		struKey = tmp_key;
	}

	struKey[nKeyCount].sKeyId = key;
	memset(struKey[nKeyCount].sKeyWord, 0, sizeof(struKey[nKeyCount].sKeyWord));
	strncpy(struKey[nKeyCount].sKeyWord, ek, sizeof(struKey[nKeyCount].sKeyWord)-1);
	return ++nKeyCount;
}
short MsgEventReq::SetKey(EVENT_KEY_ID key, int ek)
{
	char x[64]; sprintf(x, "%d", ek);
	return SetKey(key, x);
}

///////////// END /////////////
CLBTH_NAMESPACE_END

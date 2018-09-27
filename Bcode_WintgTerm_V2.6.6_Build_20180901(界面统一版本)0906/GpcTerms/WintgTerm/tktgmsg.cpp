#include "stdafx.h"
#include "widgets.h"  //wangjinhai
#include "tg.h"
#include "oscalls.h"
#include "sysshare.h"
#include "messages.h"
#include "dbfunc.h"
#include "history.h"
#include "msg_id.h"
#include "mainframe.h"

#include "tgmsg.h"
#include "function.h"
#include "plotdll/plotview.h"
//#include "NewDispatcherCommand.h"
#include "MessageDlg.h"

#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
//#include "MacroSwitch.h"
#include "icommtask.h"
#include "tg.h"
#include "db_msgproc.h"
#include "tgdocument.h"
#include "MainFrame.h"
#define HEAD_LEN   64  // 
#include "DlgMessageBox.h"



static int nSchdType;
static long nCurrentTime,nCurrentDate;

short InitStationEntryAbsnumConvertTable(void);
int OsPurgeAllConflictInform( );
static void	terminate_task(void);
static int	process_msg(void);


DECLARE_C_VISITOR(visitorofWintg);

//Ĭ����Ϣ����
DEFINE_BASEMSG_DEFPROC(vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

//ʵ������״̬
DEFINE_BASEMSG_PROC(MsgStPlanStatusAnswer, pBufStPlanStatusAnswer, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

// �г��¹ʸ���
DEFINE_BASEMSG_PROC(MsgAccidentOper, pBufMsgAccidentOper, vpParam, visitorofWintg)
{  
	CWnd * pwnd=CWnd::FindWindow(NULL,"�г��¹ʸſ�");
	if(pwnd)
		pwnd->SendMessage(GIVE_ACCIDENT, (WPARAM)pBufMsgAccidentOper, (LPARAM)0);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

}  

DEFINE_BASEMSG_PROC(MsgUserManager, pBufMsgUserManager, vpParam, visitorofWintg)
{  
	if(pBufMsgUserManager->oper == CLBTH::MsgUserManager::USERMGR_CHANGE_PROP)
	{  
		CWnd * pwnd=CWnd::FindWindow(NULL,"�޸�����");
		if(pwnd)
		{ 
			pwnd->SendMessage(UM_CHANGE_PASSWORD_MSG, (WPARAM)pBufMsgUserManager, (LPARAM)0);
			return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS); 
		}

	}
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

// �г�˦��
DEFINE_BASEMSG_PROC(MsgGroupTrainWorkFormat, pBufMsgGroupTrainWorkFormat, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

//GPC����Ϣȷ��
DEFINE_BASEMSG_PROC(MsgGpcMessage, pBuf0, vpParam, visitorofWintg)
{ 
	CLBTH::MsgGpcMessage  *pBuf  = (CLBTH::MsgGpcMessage *)pBuf0;
	if(pBuf->cmd_type == CLBTH::MsgGpcMessage::Msg_REQUEST)
	{
		char pack[1025];
		memcpy(pack, pBuf->package, pBuf->pack_len);
		pack[pBuf->pack_len] = 0;

		CString strText;
		strText.Format("��ȷ�ϣ�%s", pack);
		strText.TrimLeft(); strText.TrimRight();
		CDlgMessageBox dlg(strText,AfxGetMainWnd());

		CLBTH::MsgGpcMessage  mMsgGpcMessage ;
		if(dlg.DoModal() != IDOK)
		{
			GpcLogTrainMessage(0,0,"[Answer],�Ҿܾ�ȷ����Ϣ�� %s,����%d, Ŀ�꣺%d,����ʱ�䣺%d", 
				strText,pBuf->source_id, pBuf->dest_id,pBuf->dest_id, pBuf->tm_request );
			mMsgGpcMessage.answer_type = CLBTH::MsgGpcMessage::ANSWER_DENAY;

		}
		else
		{
			GpcLogTrainMessage(0,0,"[Answer],��ȷ����Ϣ�� %s,����%d, Ŀ�꣺%d,����ʱ�䣺%d", 
				strText,pBuf->source_id, pBuf->dest_id,pBuf->dest_id, pBuf->tm_request );
			mMsgGpcMessage.answer_type = CLBTH::MsgGpcMessage::ANSWER_OK;
		}



		CTime tCurrent = CTime::GetCurrentTime();
		GPCENTITY  entityInfo;
		gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo);

		mMsgGpcMessage.label = pBuf->label;            
		mMsgGpcMessage.source_id = entityInfo.nEntityId;  
		mMsgGpcMessage.dest_id =  pBuf->source_id;  
		mMsgGpcMessage.cmd_type = CLBTH::MsgGpcMessage::Msg_REPLY;



		mMsgGpcMessage.tm_request = pBuf->tm_request;  
		mMsgGpcMessage.tm_reply = tCurrent.GetTime();


		mMsgGpcMessage.pack_len = pBuf->pack_len;

		strncpy(mMsgGpcMessage.package, pBuf->package, mMsgGpcMessage.pack_len );


		SendBMsg(&mMsgGpcMessage,mMsgGpcMessage.dest_id); 

		GpcLogTrainMessage(0,0,"[PMTG]:��Ϣ����,�·�Ŀ��:%d",mMsgGpcMessage.dest_id);
	}
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

}  

bool TGMessageProcessNew(long entity_id,unsigned char *ptr,int length)
{
 	CLBTH::Archive RecDataBuffervar( CLBTH::Archive::load,length, ptr);
	CLBTH::EventSink::PostEvent(RecDataBuffervar, GET_C_VISITOR(visitorofWintg));
	DB_ProcessServerTGMessage(entity_id,ptr,length); // �������ݿ⴦��
	return true;
}

bool LocalTGRequestMessageProcess(unsigned char *ptr,int length)// ԭ���ɽ�����������
{
 	CLBTH::Archive RecDataBuffervar( CLBTH::Archive::load,length, ptr);
	CLBTH::EventSink::PostEvent(RecDataBuffervar, GET_C_VISITOR(visitorofWintg));
//	DB_ProcessTGRequestMessage(ptr,length); // �������ݿ⴦��
	return true;
}

//// ���������
DEFINE_BASEMSG_PROC(MsgElecArmAnswer, pBuf, vpParam, visitorofWintg)
{
    CLBTH::MsgElecArmAnswer  *pArm = ( CLBTH::MsgElecArmAnswer *)pBuf;

	CFrameWnd *pWnd = ((CWintg *)AfxGetApp())->GetMainFrame();
	if(pWnd == NULL)
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

	if(pArm->status==CLBTH::MsgElecArmAnswer::ELEC_ENABLE)
		pWnd->SendMessage(UM_ARMS_STATUS_INFORM, (pArm->elec_arm_id<<16)+pArm->elec_sta,3);
	else
		pWnd->SendMessage(UM_ARMS_STATUS_INFORM, (pArm->elec_arm_id<<16)+pArm->elec_sta,1);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

static int	AddStationList(int nStationNo,int nAllStationNum,int *nAllStationList)
{
	int i;
	for(i=0;i < nAllStationNum;i++)
	{
		if(nAllStationList[i] == nStationNo)
			break;
	}
	if(i >= nAllStationNum)
	{
		nAllStationList[nAllStationNum] = nStationNo;
		return 0;
	}
	return -1;
}

int SendTGCommMessage(int section_id,ushort usMsgSize,const char *abBufferMsg,long entity_id)
{
	if(entity_id==0)
	    entity_id=GetSpecialEntityID(SERVER_TYPE_TG,section_id);
	if(entity_id>=0)
	{
		SendCommMessage(entity_id,0,(BYTE *)abBufferMsg,usMsgSize); //entityInfo.nEntityId
		//DB_ProcessTGCommSendMessage((BYTE *)abBufferMsg,usMsgSize);// �ػ�������������Ϣ���ڱ������ݿ⴦��
		return RET_SUCCESS;
	}
	return -2;
}

bool SendTGBMsg(int section_id,CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag) // Ĭ���Ա���ID,�Զ�LABEL,������ŷ���
{
	if(dest_entity==0)
		dest_entity=GetSpecialEntityID(SERVER_TYPE_TG,section_id);
	return SendBMsg(pOneMsg,dest_entity,flag); // Ĭ���Ա���ID,�Զ�LABEL,������ŷ���	
}

//////////////////////////////////////////////////////////////////////////////
int	AddTGSectionList(int nSectionNo,int nTgSectionNum,int *nTgSectionList, int max)
{
	int i;
	for(i=0; i<nTgSectionNum&&i<max; i++)
	{
		if(nTgSectionList[i] == nSectionNo)
			break;
	}
	if(i >= nTgSectionNum)
	{
		nTgSectionList[nTgSectionNum] = nSectionNo;
		return 0;
	}

	return -1;
}

void AddEntityToMsg(MsgEntityConnStatusOrder &msg,ENTITY_ID eid)
{
	ASSERT(msg.entity_count<LOGIN_MAX_ENTITY_COUNT);
	for(int i=0; i<msg.entity_count; i++)
	{
		if(msg.entity_list[i]==eid)
		{
			return;
		}
	}
	msg.entity_list[msg.entity_count++]=eid;
}

void  RequestAllEntityStatus()
{
    MsgEntityConnStatusOrder msg;

	GPCENTITY entityInfo;
	
	// get all station inform
	int nTgSectionNum=0, nTgSectionList[64];
	int nSectionNum=0,nSectionList[64];
	int nStationNum=0,nStationList[512];
    int nAllStationNum=0,nAllEntityNum=0,nAllStationList[4096],nAllStationEntityList[4096];
	memset(nAllStationList,0,sizeof(nAllStationList));
	memset(nAllStationEntityList,0,sizeof(nAllStationEntityList));

        
	memset(nTgSectionList, 0, sizeof(nTgSectionList));
	int max=sizeof(nTgSectionList)/sizeof(nTgSectionList[0]);
	nSectionNum = sizeof(nSectionList)/sizeof(nSectionList[0]);
	int rs=gpcdef.GetSectionList(nSectionNum,nSectionList);
	ASSERT( rs > 0);

	// svr_td
	AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_TDCOMM,0));
	GpcLogTrainMessage(0, 0, "��ѯ td svrid=%d", GetSpecialEntityID(SERVER_TYPE_TDCOMM,0));

	// mmplan
	if(gpcdef.IsEnableMMPlanFromCAD())
	{
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_MMPLAN,0));
		GpcLogTrainMessage(0, 0, "��ѯ mmplan svrid=%d", GetSpecialEntityID(SERVER_TYPE_MMPLAN,0));
	}
	if (gpcdef.IsOtherMsgToGateway())
	{
		// svr_gateway
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_gateway svrid=%d", GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
	}
	else
	{
		// svr_db
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_DB,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_db svrid=%d", GetSpecialEntityID(SERVER_TYPE_DB,0));
		// svr_mdb
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_DATASTORE,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_mdb svrid=%d", GetSpecialEntityID(SERVER_TYPE_DATASTORE,0));
		// svr_basic
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_basic svrid=%d", GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0));
		// svr_shift
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_SHIFT_TG,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_shift svrid=%d", GetSpecialEntityID(SERVER_TYPE_SHIFT_TG,0));
		// svr_history
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_HISTDATA,0));
		GpcLogTrainMessage(0, 0, "��ѯ svr_history svrid=%d", GetSpecialEntityID(SERVER_TYPE_HISTDATA,0));
	}

	// svr_sdb
	AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_PARA,0));
	GpcLogTrainMessage(0, 0, "��ѯ svr_sdb svrid=%d", GetSpecialEntityID(SERVER_TYPE_PARA,0));

	// query all station in wintg section
	nAllStationNum = 0;
	nTgSectionNum = 0;
	for(int i=0;i < nSectionNum;i++)
	{
		if(AddTGSectionList(nSectionList[i], nTgSectionNum, nTgSectionList, max)==0)
			nTgSectionNum++;
		
		nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
		rs=gpcdef.GetStationList(nSectionList[i],nStationNum,nStationList);
		ASSERT(rs > 0);

		for(int j=0;j < nStationNum;j++)
		{
			if(AddStationList(nStationList[j],nAllStationNum,nAllStationList) == 0)
			   nAllStationNum++;
		}

		// �շ���̨TG_SVR
		int nNeibNum=64, nNeibSectionNo[64];
		nNeibNum=gpcdef.GetNeighberSectionList(nSectionList[i], nNeibNum, nNeibSectionNo);
		for(int k=0; k<nNeibNum; k++)   // �����ѭ��Ϊi����ѭ����k
		{
			if(AddTGSectionList(nNeibSectionNo[k], nTgSectionNum, nTgSectionList, max)==0)
				nTgSectionNum++;
		}

		// ������̨TG_SVR
		int nViewNeibNum=64, nViewNeibSectionNo[64];
		nViewNeibNum=gpcdef.GetNeighberViewPlanSectionList(nSectionList[i], nViewNeibNum, nViewNeibSectionNo);
		for(int k=0; k<nViewNeibNum; k++)
		{
			if(AddTGSectionList(nViewNeibSectionNo[k], nTgSectionNum, nTgSectionList, max)==0)
				nTgSectionNum++;
		}
	}

	if (gpcdef.IsTgMsgToGateway())
	{
		if (!gpcdef.IsOtherMsgToGateway())
		{
			// svr_gateway
			AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
			GpcLogTrainMessage(0, 0, "��ѯ svr_gateway svrid=%d", GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
		}
	}
	else
	{
		for(int i=0; i<nTgSectionNum; i++)
		{
			long tg_server_id=GetSpecialEntityID(SERVER_TYPE_TG,nTgSectionList[i]);
			AddEntityToMsg(msg,tg_server_id);
			GpcLogTrainMessage(0, 0, "��ѯ conid=%d svrtg=%d", nTgSectionList[i], tg_server_id);
		}
	}

	nAllEntityNum = 0;
	for(i=0;i<nAllStationNum;i++)
	{
		if(!gpcdef.GetEntityInfo(STPC_WACHER,nAllStationList[i],entityInfo))
			continue;

		ASSERT(nAllEntityNum < sizeof(nAllStationEntityList)/sizeof(nAllStationEntityList[0]));
		nAllStationEntityList[nAllEntityNum++] = entityInfo.nEntityId;
	} 
	for(i=nAllStationNum;i<nAllStationNum*2;i++)
	{
		int nStationNo = nAllStationList[i-nAllStationNum];
		if(!gpcdef.GetEntityInfo(EQUIP_LIRC,nStationNo,entityInfo))
			continue;

		ASSERT(nAllEntityNum < sizeof(nAllStationEntityList)/sizeof(nAllStationEntityList[0]));
		nAllStationEntityList[nAllEntityNum++] = entityInfo.nEntityId;
	}
	// 2009.11.21 �����в���Ȩ�޵��û�,���ٶ�COMMSVR����
	if(gpcdef.GetUserIdentify() == GPC_DISPATCHER 
		|| gpcdef.GetUserIdentify() == GPC_OPERATOR 
		|| gpcdef.GetUserIdentify() == GPC_MAINTAIN_DISPATCH 
		|| gpcdef.GetUserIdentify() == GPC_SUPER_OPER 
		|| gpcdef.GetUserIdentify() == GPC_TEST)
	{
		for(int i=0;i<nAllEntityNum;i++)
			AddEntityToMsg(msg,nAllStationEntityList[i]);
	}
	msg.flag=MsgEntityConnStatusOrder::CONN_INIT; 
	SendBMsg(&msg,0,0);
}

///////////////////////// ����״̬����
void  InitBeforeCommStart(void)
{
}

void  LayoutProcessEntityStatus(long entity_id, int status)
{
	/*
	GPCENTITY entityInfo;

	if(gpcdef.GetEntityInfo(entity_id,entityInfo))
	{
	if(entityInfo.nGroupId == 101) // ������
	{
	if(status==2)
	DB_ProcessServerCommStatus(entity_id,TRUE);
	else
	DB_ProcessServerCommStatus(entity_id,FALSE);
	}
	}	  */
}
void SetDisconnectWithCommsvr();

bool  NetworkEntityStatusProcessNew(int evt_type,long entity_id, int status)
{
	GpcLogTrainMessage(0, 0, "�յ�unicomm״̬֪ͨ, evt_type=%d, entity_id=%d, status=%d", evt_type, entity_id, status);

	if(evt_type==0) // connect status
	{
	  if(IsInCommServerMode()) // ��ͨ�ŷ�����ģʽ�£���ʱ����������ͨ��״̬��2009.01.20
	  {
		  if(status==0) // network is broken
		  {
			  GpcLogTrainMessage(0, 0, "WintgTerm��Commsvr���������ж�");// 2013.05.07��������ͼ��Commsvr����
			  SetDisconnectWithCommsvr();
		  }
		  else if (status==1) // ��������
		  {
			  GpcLogTrainMessage(0, 0, "WintgTerm��Commsvr��������");
		  }
		  else if (status==2)
		  {
			  GpcLogTrainMessage(0, 0, "WintgTerm��Commsvr��������");
		  }
		  else if(status==3) // network is ok
		  {
			 GpcLogTrainMessage(0, 0, "WintgTerm��½Commsvr�ɹ�");// 2013.05.07��������ͼ��Commsvr����
			 RequestAllEntityStatus();  // ��ͨ�ŷ�������ȡEntity״̬
 			 if (gpcdef.IsMSSAlarmEnable())
			 {
				 SetMSCapacityInit();
			 }
		  }
		  else if(status==254) // ͨ�����񼴽����� ����relaentity
		  {
			  GpcLogTrainMessage(0, 0, "WintgTerm��������ͨ������");
			  InitBeforeCommStart();
		  }
		  else if (status==255) // network is off
		  {
			  GpcLogTrainMessage(0, 0, "WintgTerm��Commsvrͨ��������ֹ,unicomm�����Զ�����");
			  SetDisconnectWithCommsvr();
		  }

		  return true;
	  }
	}
	else if(evt_type==1)// entity status
	{
		if(IsInCommServerMode()) // ��ͨ�ŷ�����ģʽ�£���ʱ����������ͨ��״̬��2009.01.20
		{
			LayoutProcessEntityStatus(entity_id,status);
		}
	}
	return true;
}

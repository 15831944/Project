#include "stdafx.h"
#include "tg.h"
#include "..\basemsg\clbth.h"
#include "..\basemsg\msg_schd.h"
#include "tg_schedule_base.h"
#include "tg_addtrain_logic.h"
#include "schd_newserver.h"
#include "function.h" 
#include "messages.h"
#include "gpcdef.h"
#include "memshare.h"
#include "icommtask.h"
#include "tg_msgcopy_newctcmsg.h"
#include "db_msgproc.h"
#include "TgDocument.h"
#include "basic_schd_config.h"

extern basic_schd_config* GetBasicConfig();
extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
		
#define MAX_STORED_SERVER_NUM  128
#define MAX_STORED_EVENT_NUM  32

static struct{
	CWintgNewServer *pServerInt;
	CTG_ScheduleInfo cScheduleInfo;        
}g_servers[MAX_STORED_SERVER_NUM];

static struct{
	HWND hWnd;
	CTG_ScheduleInfo cScheduleInfo;
}g_event[MAX_STORED_EVENT_NUM];

void  RegisterDataEndEvent(HWND hWnd, const CTG_ScheduleInfo & schd)
{
	int i;
	for(i=0;i<MAX_STORED_EVENT_NUM;i++)
	{
		if(g_event[i].hWnd==NULL)
			break;
	}
	if(i<MAX_STORED_EVENT_NUM)
	{
		g_event[i].hWnd=hWnd;
		g_event[i].cScheduleInfo=schd;
	}
}

void  UnRegisterDataEndEvent(HWND hWnd)
{
	int i;
	for(i=0;i<MAX_STORED_EVENT_NUM;i++)
	{
		if(g_event[i].hWnd==hWnd)
		{
			g_event[i].hWnd=NULL;
		}
	}
}

static BOOL RegisterStyleServer(CWintgNewServer *p)
{
	int i;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt==NULL)
			break;
	}
	if(i>=MAX_STORED_SERVER_NUM)
		return FALSE;
	g_servers[i].pServerInt=p;
	return TRUE;
}
static BOOL UnRegisterStyleServer(CWintgNewServer *p)
{
	int i;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt==p)
			break;
	}
	if(i>=MAX_STORED_SERVER_NUM)
		return FALSE;
	g_servers[i].pServerInt=NULL;
	return TRUE;
}

BOOL  CWintgNewServer::InitSetvers()
{
	int i;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		g_servers[i].pServerInt=NULL;
	}
	
	for(i=0;i<MAX_STORED_EVENT_NUM;i++)
	{
		g_event[i].hWnd=NULL;
	}
	return TRUE;
}

//��鵽����ϵ
void CWintgNewServer::CheckStayPassRelation(const CTG_SingleTrain  &srctrain)
{
	if(!srctrain.IsPassengerTrain())  // 2012.05.02 ������ͳ���������
		return;

	CString text="";
	for (int i= 0 ;i<srctrain.GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *rec = srctrain.GetTrainRecordByRecIndex_Const(i);
		if(rec == NULL)
			continue;

		if (!rec->IsPassRec())
		{
			if (abs(rec->m_data.depart - rec->m_data.arrive)<60)
			{
				CString tmp;
				tmp.Format(" sta:%d:%d-%d=%d", rec->GetStationNo(), rec->m_data.depart, rec->m_data.arrive, rec->m_data.depart - rec->m_data.arrive);
				text+=tmp;
			}
		}
	}

	if(text!="")
		GpcLogTrainMessage(0, 0,"[CheckStayPass]:%s index:%d %s",  srctrain.GetTrainID(), srctrain.GetTrainIndex(), text);
}

BOOL  CWintgNewServer::ProcessServerCommStatus(long server_id,BOOL  active_flag)   // STATIC!!!
{
	int i;
	BOOL rc=FALSE;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt && (g_servers[i].pServerInt->m_nServerEntityID==server_id || server_id==0))
		{
		  g_servers[i].pServerInt->SetCommReady(active_flag);
		  // 2010.06.28 ��������״̬���ٱ仯ʱ��Tg��ⲻ�����������ݲ�����
		  CWorkScheduleHolder::GlobalSetCommInterfaceStatus(g_servers[i].pServerInt,active_flag);
		  rc=TRUE;
		}
	}
	return TRUE;
}

void  CWintgNewServer::GlobalTrainExtent(const TG_MsgStamp& stamp, const CTG_TrainExtent& trainExtent)
{
	for(int i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->UpdateTrainExtent(stamp,trainExtent);
		}
	}
}

BOOL  CWintgNewServer::GlobalUpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train)
{
	int i;
	BOOL rc=FALSE;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->UpdateData(stamp,op_type,train);
		  rc=TRUE;
		}
	}
	return TRUE;
}

BOOL  CWintgNewServer::GlobalUpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark)	
{
	int i;
	BOOL rc=FALSE;
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->UpdateData(stamp,op_type,mark);
		  rc=TRUE;
		}
	}
	return TRUE;
}
BOOL  CWintgNewServer::GlobalInitAppendData(const TG_MsgStamp &stamp,const CTG_SingleMark  &mark)
{
	int i;
	BOOL rc=FALSE;
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->InitAppendData(schd,mark);
		  rc=TRUE;
		}
	}
	return TRUE;
}
BOOL  CWintgNewServer::GlobalInitAppendData(const TG_MsgStamp &stamp,const CTG_SingleTrain  &train)
{
	int i;
	BOOL rc=FALSE;
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 
	
	GpcLogTrainMessage(0, 0, "���շ������������� trainID %s index %u con_id %d, schedule_type %d, m_nType %d", 
			  train.GetTrainID(), train.GetTrainIndex(), stamp.con_id, stamp.schedule_type, schd.m_nType);
	
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->InitAppendData(schd,train);
		  rc=TRUE;
		}
	}
	return TRUE;
}

int  CWintgNewServer::GetServerEntityIDs(long entitys[],int limit)
{
	int count=0;
	int i;
	for(i=0;i<MAX_STORED_SERVER_NUM && count<limit;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  entitys[count++]=g_servers[i].pServerInt->m_nServerEntityID;
		}
	}
	return count;
}

BOOL  CWintgNewServer::GlobalProcessServerErrorEcho(const TG_MsgStamp &stamp,TG_OBJECT_TYPE obj_type,long err_code)
{
	int i;
	BOOL rc=FALSE;
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
		  g_servers[i].pServerInt->ProcessServerErrorEcho(schd,obj_type,stamp.index,err_code);
		  rc=TRUE;
		}
	}
	return TRUE;
}

extern DWORD AddToMemoryNotify(const char *desc,time_t begintime,int repeat,int type,DWORD objindex, int flag=1);
BOOL   CWintgNewServer::GlobalSetInitStatus(const TG_MsgStamp &stamp,TG_OBJECT_TYPE obj_type,BOOL va,int obj_count)
{
	int i;
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 

	if(va==TRUE)
		GpcLogTrainMessage(0,0,"�յ������� MsgHugeDataCtrl con_id=%d schd_type=%d obj_type=%d END",stamp.con_id, stamp.schedule_type, obj_type);
	else
		GpcLogTrainMessage(0,0,"�յ������� MsgHugeDataCtrl con_id=%d schd_type=%d obj_type=%d BEGIN",stamp.con_id, stamp.schedule_type, obj_type);
	for(i=0;i<MAX_STORED_SERVER_NUM;i++)
	{
		if(g_servers[i].pServerInt)
		{
			CTG_TrainScheduleBase *pdata=g_servers[i].pServerInt->GetDataStore(schd);
			if(pdata)
			{
				if(obj_type==TG_OBJ_TRAIN)
				{
					if (va==TRUE)
					{
						int passcnt=0;
						int frecnt=0;
						int total=pdata->GetTrainCount1(&passcnt,&frecnt);
						if (obj_count!=total)
						{
							GpcLogTrainMessage(0,0,"�յ������� MsgHugeDataCtrl �������������߳��ֲ�ͬ��,con_id=%d, schd_type=%d, ����=%d, �ڴ��г�����=%d",stamp.con_id, schd.m_nType, obj_count, total);
							CString str;
							
						}
					}	
							
					pdata->SetTrainDataInited(va);
					g_servers[i].pServerInt->SetTrainDataInited(va); // ע���ʱû�п����ж��data�����

					if(va==FALSE)
					{
						for(i=0;i<MAX_STORED_EVENT_NUM;i++)
						{
							if(g_event[i].cScheduleInfo.m_nSectionID==schd.m_nSectionID && g_event[i].cScheduleInfo.m_nType==schd.m_nType)
							{
								::SendMessage(g_event[i].hWnd, EVENT_RECIEVE_DATA_END, 0, 1);
							}
						}
					}

					if(!GetTGFunctionConfig()->IsNeedWaitMarkEnd())
					{
						if(va==TRUE)
						{
							for(i=0;i<MAX_STORED_EVENT_NUM;i++)
							{
								if(g_event[i].cScheduleInfo.m_nSectionID==schd.m_nSectionID && g_event[i].cScheduleInfo.m_nType==schd.m_nType)
								{
									::SendMessage(g_event[i].hWnd, EVENT_RECIEVE_DATA_END, 0, 0);
								}
							}
						}
					}
				}
				else
				if(obj_type==TG_OBJ_MARK)
				{
					if (va==TRUE)
					{
						if (obj_count!=pdata->GetMarkCount())
						{
							GpcLogTrainMessage(0,0,"�յ������� MsgHugeDataCtrl ��������ʩ�����ų��ֲ�ͬ��,con_id=%d, schd_type=%d, ����=%d, �ڴ�ʩ����������=%d",stamp.con_id, schd.m_nType, obj_count, pdata->GetMarkCount());
						}
					}

					pdata->SetMarkDataInited(va);
					g_servers[i].pServerInt->SetMarkDataInited(va);

					if(GetTGFunctionConfig()->IsNeedWaitMarkEnd())
					{
						if(va==TRUE)
						{
							for(i=0;i<MAX_STORED_EVENT_NUM;i++)
							{
								if(g_event[i].cScheduleInfo.m_nSectionID==schd.m_nSectionID && g_event[i].cScheduleInfo.m_nType==schd.m_nType)
								{
									::SendMessage(g_event[i].hWnd, EVENT_RECIEVE_DATA_END, 0, 0);
								}
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
CWintgNewServer::CWintgNewServer()
{
	RegisterStyleServer(this);
	m_nOperEntityID=0;
	m_nServerEntityID=0;
	m_bQueryMode=TRUE;
}

CWintgNewServer::~CWintgNewServer()
{
	UnRegisterStyleServer(this);
}

BOOL  CWintgNewServer::Init(int  con_id,int server_type)
{
	m_nOperEntityID=GetLocalEntityID();
	m_nServerEntityID=GetSpecialEntityID(server_type,con_id);
	if(m_nOperEntityID>0 && m_nServerEntityID>0)
	{
		return TRUE;
	}
	else
		return FALSE;
}

CTG_TrainScheduleBase *CWintgNewServer::GetDataStore(const CTG_ScheduleInfo &source_info)
	{
		int i;
		for(i=0;i<DATA_SOURCE_NUM;i++)
		{
		  if(m_pDataStore[i])
		  {
			  CTG_ScheduleInfo info;
			  m_pDataStore[i]->GetScheduleInfo(info); 
			  if(info.m_nSectionID ==source_info.m_nSectionID  && info.m_nType==source_info.m_nType)
				  return m_pDataStore[i];
		  }
		}
		return NULL;
	}


void  CWintgNewServer::PollingAndCheckStatus()
{
	if(DB_GetEntityStatus(m_nServerEntityID)==1)
	{
		SetCommReady(TRUE);
	}
	else
	{
		SetCommReady(FALSE);
	}
}

BOOL CWintgNewServer::UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	//if(srctrain.GetGPCTrainIndex() & 0x80000000)
		return SendMessageToServer(schd,TG_OPER_UPDATE,srctrain);
	// else
	//return SendMessageToServer(schd,TG_OPER_ADD,srctrain);
}

BOOL CWintgNewServer::UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
	if(block_info.GetGPCIndex() & 0x80000000)
		return SendMessageToServer(schd,TG_OPER_UPDATE,block_info);
	else
	{
		if(block_info.m_data.direction == 0xFF)  // direction == 0xFF��ʾʩ��������ͼ����ʩ��,��ֱ�������������,������TGOCX��ɲ�����ֵ���ɱ���TGOCX�ٷ�������
		{
			GpcLogTrainMessage(0, 0, "ʩ��������ͼ ʩ�����Ų���ֱ�ӷ��͸�������index %d, direct %d",
				block_info.GetBlockIndex(), block_info.m_data.direction);
			return TRUE;
		}

		return SendMessageToServer(schd,TG_OPER_UPDATE,block_info);
	}
}


	///////// DELETE
BOOL CWintgNewServer::DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return SendMessageToServer(schd,TG_OPER_DELETE,train);
	
}

BOOL CWintgNewServer::DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &srcmark,int source_flag)
{
	return SendMessageToServer(schd,TG_OPER_DELETE,srcmark);
}

BOOL CWintgNewServer::DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag)
{
	return FALSE;
}


	///////// APPEND
TRAIN_INDEX CWintgNewServer::AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	if(SendMessageToServer(schd,TG_OPER_ADD,srctrain))
		return srctrain.GetTrainIndex();
	else
		return NULL_TRAININDEX;

}
TRAIN_INDEX  CWintgNewServer::AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &srcmark,int source_flag)
{

	if(SendMessageToServer(schd,TG_OPER_ADD,srcmark))
		return srcmark.GetBlockIndex();
	else
		return NULL_TRAININDEX;
};

BOOL CWintgNewServer::DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag)
{
	CTG_SingleTrain train;
	train.SetTrainIndex(index); 
	return SendMessageToServer(schd,TG_OPER_DELETE,train);
}


BOOL CWintgNewServer::DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag){return FALSE;};
	////////  LOAD
	//////////////////////////// ��� ////////////////////////////
	////////  COPY
BOOL CWintgNewServer::CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag)
{
	 CTG_TrainScheduleBase *pData=GetDataStore(schd);
	 if(pData!=pOut)  // �Ҳ�����Ӧ���ݴ洢
		return FALSE;

	if(pData!=(CTG_TrainScheduleBase *)pOut)
		return FALSE;
	SendScheduleLoadMessage_ToServer(schd,FALSE);	
	return TRUE;
}

void CWintgNewServer::Close()
{
	int i;
	for(i=0;i<DATA_SOURCE_NUM;i++)
	{
		if(m_pDataStore[i])
		{
			CTG_ScheduleInfo info;
			m_pDataStore[i]->GetScheduleInfo(info); 
			
			SendScheduleLoadMessage_ToServer(info,TRUE);
		}
	}
}

BOOL  CWintgNewServer::SendScheduleLoadMessage_ToServer(const CTG_ScheduleInfo &schd_info,BOOL unload)
{
	DWORD style=0;
		
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		style=MsgDataStyleReq2::SCHD_TRAIN|MsgDataStyleReq2::SCHD_BLOCK;
	else
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		style=MsgDataStyleReq2::BASC_TRAIN|MsgDataStyleReq2::BASC_BLOCK;
	else
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
		style=MsgDataStyleReq2::HSTY_TRAIN|MsgDataStyleReq2::HSTY_BLOCK;
	else
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		style=MsgDataStyleReq2::SHIFT_TRAIN|MsgDataStyleReq2::SHIFT_BLOCK;
	else
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		style=MsgDataStyleReq2::PREV_TRAIN| MsgDataStyleReq2::PREV_BLOCK;
	else if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		style=MsgDataStyleReq2::BASIC_ARCHIVE;
	else
		return FALSE;

	if(schd_info.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		MsgDataStyleReq4 msg;
		msg.nStatCount= 1;
		msg.nStno[0]= schd_info.m_nSectionID; 
		msg.nStyle[0]=style;
		msg.nScheduleId[0]=GetBasicConfig()->GetCurSelBackupBasicType(schd_info.m_nSectionID);
		if(unload)
			msg.nStatus[0]= MsgDataStyleReq2::CLOSE; 
		else
			msg.nStatus[0]= MsgDataStyleReq2::LOAD; 
		msg.nStartTime =schd_info.m_tStartTime;
		msg.nEndTime = schd_info.m_tEndTime;

		GpcLogTrainMessage(0, 0, "�����������(MsgDataStyleReq4) con_id %d nScheduleId %d style %0.4X load(1:Close 0:Open) %d nStartTime %d nEndTime %d",
						msg.nStno[0], msg.nScheduleId[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, msg.nEndTime);

		if(SendBMsg(&msg,m_nServerEntityID,0))
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		MsgDataStyleReq2 msg;
		msg.nStatCount= 1;
		msg.nStno[0]= schd_info.m_nSectionID; 
		msg.nStyle[0]=style;
		if(unload)
			msg.nStatus[0]= MsgDataStyleReq2::CLOSE; 
		else
			msg.nStatus[0]= MsgDataStyleReq2::LOAD; 
		
		// 20171218�˷������ظ�
		TIME nStartTime=0, nEndTime=0;
		schd_info.GetTermAcceptSchdTimeRange(nStartTime, nEndTime);
		msg.nStartTime=nStartTime;
		msg.nEndTime=nEndTime;
		
		CTime btm(msg.nStartTime), etm(msg.nEndTime);
		if(SendBMsg(&msg,m_nServerEntityID,0))
		{
			GpcLogTrainMessage(0, 0, "�����������(MsgDataStyleReq2) con_id %d, style %0.4X load(1:Close 0:Open) %d nStartTime %d(%s) nEndTime %d(%s)",
					msg.nStno[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, btm.Format("%Y-%m-%d %H:%M:%S"), msg.nEndTime, etm.Format("%Y-%m-%d %H:%M:%S"));
			return TRUE;
		}
		else
		{
			GpcLogTrainMessage(0, 0, "�����������(MsgDataStyleReq2) con_id %d, style %0.4X load(1:Close 0:Open) %d nStartTime %d(%s) nEndTime %d(%s) ʧ��",
					msg.nStno[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, btm.Format("%Y-%m-%d %H:%M:%S"), msg.nEndTime, etm.Format("%Y-%m-%d %H:%M:%S"));
			return FALSE;
		}
	}
    return FALSE;
}

BOOL  CWintgNewServer::SendScheduleLoadMessage_ToServer(const CTG_ScheduleInfo &schd_info,int stations[],int statcnt)
{
	MsgSchdLoad msg;
	for(int i=0;i<statcnt;i++)
		msg.station[i]=stations[i];
	msg.sta_count=statcnt;
	msg.con_id=schd_info.m_nSectionID;
	msg.schd_type=schd_info.m_nType;
	msg.start_time=schd_info.m_tStartTime;
	msg.stop_time=schd_info.m_tEndTime; 
	
	SetTrainDataInited(FALSE);
	SetMarkDataInited(FALSE);
	
	if(SendBMsg(&msg,m_nServerEntityID,0))
		return TRUE;
	else
		return FALSE;
}

BOOL  CWintgNewServer::SendMessageToServer(const CTG_ScheduleInfo &schd,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train)
{
	if(m_bQueryMode)
		return FALSE;

	bool rc=false;
	TIME tm=time(NULL);
	TG_MsgStamp stamp;
	stamp.con_id=schd.m_nSectionID;
	stamp.index=train.GetTrainIndex();
	stamp.ltime=tm;
	stamp.operatr=m_nOperEntityID;
	stamp.schedule_type =schd.m_nType; 
	
	switch(op_type)
	{
	case TG_OPER_ADD:
		{
			MsgTrainAdd msg;
			MsgTrainExtent msg_extent;
			if(TrainCopy(msg,train) && TrainExtentCopy(msg_extent,train))
			{
				CopyStamp(msg.stamp,stamp);
				CopyStamp(msg_extent.stamp,stamp);

				CLBTH::BaseMsg* pMsgs[2];
				pMsgs[0]=&msg_extent;
				pMsgs[1]=&msg;

				rc=SendBMsg(pMsgs,2,m_nServerEntityID,0);
				GpcLogTrainMessage(0, 0, "����������͸���(TG_OPER_ADD) trainID %s con_id %d, index %u prev_train_index %u ltime %ld entity %d schdule_type %d list: %s",
					train.GetTrainID(),stamp.con_id, stamp.index, msg_extent.prev_train_index, stamp.ltime, stamp.operatr, stamp.schedule_type,train.toString());
			}
		}
		break;
	case TG_OPER_DELETE:
		{
			MsgTrainDelete msg;
			if(TrainCopy(msg,train))
			{
				CopyStamp(msg.stamp,stamp);
				rc=SendBMsg(&msg,m_nServerEntityID,0);
				GpcLogTrainMessage(0, 0, "����������͸���(TG_OPER_DELETE) trainID %s con_id %d, index %u ltime %ld entity %d schdule_type %d list: %s",
					train.GetTrainID(), stamp.con_id, stamp.index, stamp.ltime, stamp.operatr, stamp.schedule_type,train.toString());
			}
		}
		break;
	case TG_OPER_UPDATE:
		{
			MsgTrainUpdate msg;
			MsgTrainExtent msg_extent;
			if(TrainCopy(msg,train) && TrainExtentCopy(msg_extent,train))
			{
				CopyStamp(msg.stamp,stamp);
				CopyStamp(msg_extent.stamp,stamp);

				CLBTH::BaseMsg* pMsgs[2];
				pMsgs[0]=&msg_extent;
				pMsgs[1]=&msg;

				rc=SendBMsg(pMsgs,2,m_nServerEntityID,0);
				GpcLogTrainMessage(0, 0, "����������͸���(TG_OPER_UPDATE) trainID %s con_id %d, index %u prev_train_index %u ltime %ld entity %d schdule_type %d list: %s",
					train.GetTrainID(),stamp.con_id, stamp.index, msg_extent.prev_train_index, stamp.ltime, stamp.operatr, stamp.schedule_type,train.toString());
			}
		}
		break;
	}
	return TRUE;
}

BOOL  CWintgNewServer::SendMessageToServer(const CTG_ScheduleInfo &schd,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark)	
{
	if(m_bQueryMode)
		return FALSE;

	bool rc=false;
	TIME tm=time(NULL);
	TG_MsgStamp stamp;
	stamp.con_id=schd.m_nSectionID;
	stamp.index=mark.GetBlockIndex();
	stamp.ltime=tm;
	stamp.operatr=m_nOperEntityID;
	stamp.schedule_type =schd.m_nType; 
	switch(op_type)
	{
	case TG_OPER_ADD:
		{
			MsgSchdBlockAdd msg;
			if(BlockCopy(msg,mark))
			{
				CopyStamp(msg.stamp,stamp);
				rc=SendBMsg(&msg,m_nServerEntityID,0);
			}
		}
		break;
	case TG_OPER_DELETE:
		{
			MsgSchdBlockDelete msg;
			if(BlockCopy(msg,mark))
			{
				CopyStamp(msg.stamp,stamp);
				rc=SendBMsg(&msg,m_nServerEntityID,0);
			}
		}
		break;
	case TG_OPER_UPDATE:
		{
			MsgSchdBlockUpdate msg;
			if(BlockCopy(msg,mark))
			{
				CopyStamp(msg.stamp,stamp);
				rc=SendBMsg(&msg,m_nServerEntityID,0);
			}
		}
		break;
	}
	return TRUE;
}

BOOL  CWintgNewServer::UpdateTrainExtent(const TG_MsgStamp& stamp, const CTG_TrainExtent& trainExtent)
{
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 
	CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;
	pData->SaveTrainExtent(trainExtent);
	return TRUE;
}
	
    /// Process Server message Input 
BOOL  CWintgNewServer::UpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train)
{
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 
	CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;

	if(op_type==TG_OPER_ADD || op_type==TG_OPER_UPDATE)
	{
		if(!pData->SetTrainExtent(const_cast<CTG_SingleTrain&>(train)))
		{
			if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
			{
				GpcLogTrainMessage(0, 0, "�ϲ�TrainExtentʧ��. trainID %s con_id %d schedule_type %d train_index %u ltime %u operatr %u m_nType %d", 
					train.GetTrainID(), stamp.con_id, stamp.schedule_type, stamp.index, stamp.ltime, stamp.operatr, schd.m_nType);
			}
		}
	}
	pData->FinishSetTrainExtent();

	CTG_SingleTrain *pold;
	switch(op_type)
	{
		case TG_OPER_ADD:
		case TG_OPER_UPDATE:
			{
				pold=pData->GetSingleTrain(train.GetTrainIndex());
				if(pold)
				{
					BOOL core_same=FALSE; // ��������һ��,�������tg.ocx
					core_same=pold->CompareCoreData(train);
					// ����������(<1s)�޸���������,��ɵڶ��ν����޸����ݶ�ʧ
					if(stamp.operatr==m_nOperEntityID && FALSE == core_same)
					{
						GpcLogTrainMessage(0, 0, "�������������ظ��� trainID %s con_id %d, index %u prev_train_index %u ltime %ld entity %d schdule_type %d",
							train.GetTrainID(), stamp.con_id, stamp.index, train.GetPrevTrainIndex(), stamp.ltime, stamp.operatr, stamp.schedule_type);

						pold->StopSyncing(GetInterfaceFlag()); // ���ñ�־,���·��������� 2010.6.24,������֮�����������ĸ���ʱ��Ӧ����������������
						pold->SetSyncStatus(GetInterfaceFlag(),FALSE);  // ���ñ�־,���·��������� 2010.6.24,������֮�����������ĸ���ʱ��Ӧ����������������
						break;
					}

					///////////////// ����Ϊ�������� 2010.4.7 ////
					// ����δ�仯�����Ѿ��ͷ�����ͬ����ô�Ƿ������������ظ���Ϣ
					if(*pold==train && pold->CheckSyncStatus(GetInterfaceFlag()))  
					{
						GpcLogTrainMessage(0, 0, "�������������ظ���-�ظ���Ϣ!  trainID %s con_id %d, index %u prev_train_index %u ltime %ld entity %d schdule_type %d",
							train.GetTrainID(), stamp.con_id, stamp.index, train.GetPrevTrainIndex(), stamp.ltime, stamp.operatr, stamp.schedule_type);
						break;
					}

					// �޻�2010.6.18 �������������, ��������δͬ��ʱ�����������ٷ��غ�ʹͬ����־����ˣ����½���δ����
				    pold->SetSyncStatus(GetInterfaceFlag(),TRUE);  // ���÷������Ѿ�ͬ��

					BOOL all_synced=pData->CheckAllSynced(pold);  // ���ش���: ԭ�������������ã� 2010.6.23
                  
					if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
					{
						if(!((stamp.operatr==m_nOperEntityID || core_same) && all_synced))
						{
							CTG_SingleTrain* pTrain = (CTG_SingleTrain*)(&train);
							CaculateTrainSlowTime(pTrain);// 20090917 ����ϻ�����
						}
					}

					// �������������г��ĵ�����ϵ
					CheckStayPassRelation(train);
					CTG_SingleTrain* pTrain = (CTG_SingleTrain*)(&train);
					pTrain->SetTrainStSpace();

                    pData->UpdateTrain(schd,train,GetInterfaceFlag());
				    if((stamp.operatr==m_nOperEntityID || core_same) && all_synced)   // ����һ����������ԭ����������Ѿ���ͬ����
					{
					    pold->SetSyncStatus(SYCFLAG_ALL,TRUE);  // �Լ������ĸ��±�����������,���߻��������ޱ仯,������Tg.ocx
						GpcLogTrainMessage(0, 0, "���շ��������ظ��� trainID %s con_id %d, index %u prev_train_index %u,���������ޱ仯������TG", train.GetTrainID(), stamp.con_id, stamp.index, train.GetPrevTrainIndex());
					}
					else
					{
						GpcLogTrainMessage(0, 0, "���շ��������ظ���(TG_OPER_UPDATE) all_synced %d trainID %s con_id %d, index %u  prev_train_index %u ltime %ld entity %d schdule_type %d %s",
							all_synced, train.GetTrainID(), stamp.con_id, stamp.index, train.GetPrevTrainIndex(), stamp.ltime, stamp.operatr, stamp.schedule_type,train.toString());
					}
					
				}
				else
				{
					// �������������г��ĵ�����ϵ
					CheckStayPassRelation(train);
					CTG_SingleTrain* pTrain = (CTG_SingleTrain*)(&train);
					pTrain->SetTrainStSpace();

					pData->AppendTrain(schd,train,GetInterfaceFlag());
					GpcLogTrainMessage(0, 0, "���շ��������ظ���(TG_OPER_ADD) trainID %s con_id %d, index %u  prev_train_index %u ltime %ld entity %d schdule_type %d %s",
						train.GetTrainID(), stamp.con_id, stamp.index, train.GetPrevTrainIndex(), stamp.ltime, stamp.operatr, stamp.schedule_type,train.toString());

				}
			}
			break;
		case TG_OPER_DELETE:
			{
				pData->DeleteTrain(schd,train,GetInterfaceFlag());
			}
			break;
		default:
			;
	}
	return TRUE;
}
BOOL  CWintgNewServer::UpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark)	
{
	CTG_ScheduleInfo schd;
	schd.m_nSectionID = stamp.con_id;  
	schd.m_nType = CTG_ScheduleInfo::ScheduleTypeConvert(stamp.schedule_type); 

	CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;
	switch(op_type)
	{
		case TG_OPER_ADD:
		case TG_OPER_UPDATE:
			{
			CTG_SingleMark *pold=pData->GetSingleMark(mark.GetBlockIndex());
			if(pold)
			{
                pData->UpdateMark(schd,mark,GetInterfaceFlag());
				if(stamp.operatr==m_nOperEntityID)
					pold->SetSyncStatus(SYCFLAG_ALL,TRUE);  // �Լ������ĸ��±�����������
			}
			else
				pData->AppendMark(schd,mark,GetInterfaceFlag());
			}
			break;
		case TG_OPER_DELETE:
			pData->DeleteMark(schd,mark,GetInterfaceFlag());
			break;
	}
	return TRUE;
}
BOOL  CWintgNewServer::InitAppendData(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark)
{
	CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;
	CTG_SingleMark *prev;
	if(prev=pData->GetSingleMark(mark.GetBlockIndex(),TRUE))  
	{
		//if(prev->GetUpdateTime()>=mark.GetUpdateTime()) // �������ݱȽ���!
		//	return FALSE;
		if(!prev->CheckSyncStatus(this->GetInterfaceFlag()))  // ����������Ҫ���µ�������,���ܷ��򸲸�
			return FALSE;
		else
			return pData->UpdateMark(schd,mark,GetInterfaceFlag())?TRUE:FALSE; 
	}
	return pData->AppendMark(schd,mark,GetInterfaceFlag())?TRUE:FALSE; 
}

BOOL  CWintgNewServer::InitAppendData(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train)
{
    CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;

	if(!pData->SetTrainExtent(const_cast<CTG_SingleTrain&>(train)))
	{
		if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
		{
			GpcLogTrainMessage(0, 0, "�ϲ�TrainExtentʧ��. trainID %s train_index %u SectionID %d  m_nType %d", 
				 train.GetTrainID(), train.GetTrainIndex(), schd.m_nSectionID, schd.m_nType);
		}
	}
	pData->FinishSetTrainExtent();
	
	CTG_SingleTrain *prev;
	if(prev=pData->GetSingleTrain(train.GetTrainIndex(),TRUE))  
	{
		if(!prev->CheckSyncStatus(this->GetInterfaceFlag()))  // ����������Ҫ���µ�������,���ܷ��򸲸�
			return FALSE;
		else
			return pData->UpdateTrain(schd,train,GetInterfaceFlag())?TRUE:FALSE;
	}
	CTG_SingleTrain* pTrain = (CTG_SingleTrain*)(&train);
	pTrain->SetTrainStSpace();
	return pData->AppendTrain(schd,train,GetInterfaceFlag())?TRUE:FALSE;
}

BOOL  CWintgNewServer::ProcessServerErrorEcho(const CTG_ScheduleInfo &schd,TG_OBJECT_TYPE ob_type,TRAIN_INDEX train_index,long err_code)
{
	CTG_TrainScheduleBase *pData=GetDataStore(schd);
	if(!pData)
		return FALSE;
	if(ob_type==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=pData->GetSingleTrain(train_index);
		if(!ptrain)
			return FALSE;
		if(err_code!=0)
			ptrain->SetAlarmStatus(ALARM_FLAG_SERVER_NOT_ACK,TRUE); 
		else
			ptrain->SetAlarmStatus(ALARM_FLAG_SERVER_NOT_ACK,FALSE); 
		ptrain->SetSyncStatus(GetInterfaceFlag(),TRUE);
		return TRUE;
	}
	else
	if(ob_type==TG_OBJ_MARK)
	{
		CTG_SingleMark *pmark=pData->GetSingleMark(train_index);
		if(!pmark)
			return FALSE;
		if(err_code!=0)
			pmark->SetAlarmStatus(ALARM_FLAG_SERVER_NOT_ACK,TRUE); 
		else
			pmark->SetAlarmStatus(ALARM_FLAG_SERVER_NOT_ACK,FALSE); 
		pmark->SetSyncStatus(GetInterfaceFlag(),TRUE);
		return TRUE;
	}
	return FALSE;
}

void   CWintgNewServer::PollingForReqData(const CTG_ScheduleInfo& schd_info)
{
	if(!gpcdef.IsNewAutoMoveMode())
		return;

	DWORD style=0;
		
	if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		style=MsgDataStyleReq2::SCHD_TRAIN|MsgDataStyleReq2::SCHD_BLOCK;
	else if(schd_info.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		style=MsgDataStyleReq2::SHIFT_TRAIN|MsgDataStyleReq2::SHIFT_BLOCK;
	else
		return;

	MsgDataStyleReq2 msg;
	msg.nStatCount= 1;
	msg.nStno[0]= schd_info.m_nSectionID; 
	msg.nStyle[0]=style;
	msg.nStatus[0]= MsgDataStyleReq2::LOAD; 

	schd_info.GetViewScopeRange(msg.nStartTime, msg.nEndTime);
	msg.nStartTime=msg.nEndTime;
	msg.nEndTime += 3600;
		
	CTime btm(msg.nStartTime), etm(msg.nEndTime);
	if(SendBMsg(&msg,m_nServerEntityID,0))
	{
		GpcLogTrainMessage(0, 0, "����������ͽ׶�����(MsgDataStyleReq2) con_id %d, style %0.4X load(1:Close 0:Open) %d nStartTime %d(%s) nEndTime %d(%s)",
			msg.nStno[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, btm.Format("%Y-%m-%d %H:%M:%S"), msg.nEndTime, etm.Format("%Y-%m-%d %H:%M:%S"));
	}
	else
	{
		GpcLogTrainMessage(0, 0, "����������ͽ׶�����ʧ��(MsgDataStyleReq2) con_id %d, style %0.4X load(1:Close 0:Open) %d nStartTime %d(%s) nEndTime %d(%s)",
			msg.nStno[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, btm.Format("%Y-%m-%d %H:%M:%S"), msg.nEndTime, etm.Format("%Y-%m-%d %H:%M:%S"));
	}
}

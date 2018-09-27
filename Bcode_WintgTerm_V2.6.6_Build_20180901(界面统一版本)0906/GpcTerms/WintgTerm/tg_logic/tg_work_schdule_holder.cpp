#include "tg_work_schdule_holder.h"
#include "tg_addtrain_logic.h"
#include "tg_autoadjust.h"
#include "function.h"
#include "tg_filebase.h"
#include "sysshare.h"
#include "tgx.h"
#include "tg_BinXml.h"
#include "tg_other_structdef.h"
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp
extern void  RemoveTrainAbsTimeType(CTG_SingleTrain* ptrain);
extern void  SetTrainAbsTimeType(CTG_SingleTrain* ptrain, BYTE run_type);
extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);
extern BOOL  SetNewTrainAbsRunTime(CTG_SingleTrain& train, int rec_index, BYTE run_type);
extern BOOL  ReqBasicName_ToServer(int con_id, bool bload);
#define MAX_STORED_HOLDER_NUM  24
static struct{
	CWorkScheduleHolder *pDataHolder;
}g_data_holders[MAX_STORED_HOLDER_NUM];

static BOOL RegisterHolder(CWorkScheduleHolder *p)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder==NULL)
			break;
	}
	if(i>=MAX_STORED_HOLDER_NUM)
		return FALSE;
	g_data_holders[i].pDataHolder=p;
	return TRUE;
}

static BOOL UnRegisterHolder(CWorkScheduleHolder *p)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder==p)
			break;
	}
	if(i>=MAX_STORED_HOLDER_NUM)
		return FALSE;
	g_data_holders[i].pDataHolder=NULL;
	return TRUE;
}

static int  GetEntryByTmisAbsNumber(int station,int tmisabsnum)
{
	long absnum=GetTGBasicParam()->GetAbsNumByTmisNum(tmisabsnum);
	if(absnum<0)
		absnum=tmisabsnum;
	ushort st1=0,etr1=255,st2=0,etr2=255;
	GetTGBasicParam()->ConvertAbsnumToStationEntry(&st1,&etr1,&st2,&etr2,absnum);
	if(station==st1)
		return etr1;
	if(station==st2)
		return etr2;
	return -1;
}

BOOL  CWorkScheduleHolder::InitTGHolders()
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		g_data_holders[i].pDataHolder=NULL;
	}
	return TRUE;
}

static TIME g_polling_time=0;
BOOL  CWorkScheduleHolder::Polling(TIME curtm)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
		{
			g_data_holders[i].pDataHolder->PollingData(curtm); 	
		}
	}

	if(gpcdef.IsNewAutoMoveMode() && g_polling_time!=curtm)
	{
		g_polling_time=curtm;
		for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
		{
			if(g_data_holders[i].pDataHolder)
			{
				g_data_holders[i].pDataHolder->PollingAutoMove(curtm);
			}
		}
	}

	return TRUE;
}

// 2013.05.07��������ͼ��Commsvr����
void CWorkScheduleHolder::GlobalRefreshCommStatus()
{
	if(GetTGFunctionConfig()->IsClosePollingCommStatus())
		return;

	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
			g_data_holders[i].pDataHolder->RefreshCommStatus(); 	
	}
}

BOOL  CWorkScheduleHolder::GlobalSetCommInterfaceStatus(IScheduleComm *p,BOOL va)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
		{
			g_data_holders[i].pDataHolder->InitCommInterfaceStatus(p,va);
			g_data_holders[i].pDataHolder->PostCommStatus(p,va);
		}
	}
	return TRUE;
}

CWorkScheduleHolder::CWorkScheduleHolder(CWorkScheduleHolder::DATA_RUN_MODE mode)
{
	CTime tm=CTime::GetCurrentTime();
	m_nPollingMiniute = tm.GetMinute();
	m_nPollingHour = tm.GetHour();
	
    m_nMode=mode;
	RegisterHolder(this); 
	
	m_pMasterServerCommWork=NULL;
	m_pMasterServerCommShift=NULL;
	m_pMasterServerCommBasic=NULL;
	m_pMasterServerCommBasicPrev=NULL;
	m_pMasterServerCommBasicBackup=NULL;
	m_pMasterDatabaseComm=NULL;
	m_bQueryMode=TRUE;
	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		m_aCommArray[i].nSchType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL; 
        m_aCommArray[i].nSectionID=0;
		m_aCommArray[i].pCommInterface=NULL;
		m_aCommArray[i].bActive=FALSE;
	}
	mNeibPlanCount=0;
}
CWorkScheduleHolder::~CWorkScheduleHolder()
{
    UnRegisterHolder(this);
}

BOOL  CWorkScheduleHolder::SwitchToLocalMode()
{
	if(m_nMode==LOCAL_MODE)
		return FALSE;
	IScheduleComm *pcomm=m_pMasterDatabaseComm;
	if(!pcomm)  // �������ݿ�����
		return FALSE;
	if(!pcomm->CommReady())  // ���ݿ����ӿ���
		return FALSE;
	CString tmp;
	if(m_cWorkSchduleData.GetNoSyncTrainCount(m_pMasterDatabaseComm->GetInterfaceFlag(),tmp)>0)  // �������ݿ���ȫͬ�����������ݿⲻ�������
		return FALSE;
	if(m_cWorkSchduleData.GetNoSyncMarkCount(m_pMasterDatabaseComm->GetInterfaceFlag(),tmp)>0)
		return FALSE;
	m_nMode=LOCAL_MODE;
	return TRUE;
}

BOOL  CWorkScheduleHolder::SwitchToServerMode()
{
	if(m_nMode==SERVER_MODE)
		return FALSE;
	if(!m_pMasterServerCommWork || !m_pMasterServerCommWork->CommReady()) 
		return FALSE;
	if(!m_pMasterServerCommShift || !m_pMasterServerCommShift->CommReady()) 
		return FALSE;
	if(!m_pMasterServerCommBasic || !m_pMasterServerCommBasic->CommReady()) 
		return FALSE;
	
	m_nMode=SERVER_MODE;
	return TRUE;
}

BOOL  CWorkScheduleHolder::InitScheduleData(int section,CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	CTG_ScheduleInfo work_schd;

	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
		work_schd.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_HIST;
	else
		work_schd.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;
	work_schd.m_nSectionID=section;
	m_cWorkSchduleData.SetScheduleInfo(work_schd); 
	m_cWorkSchduleData.DeleteAllData(work_schd,SYCFLAG_NULL);
	
	
	CTG_ScheduleInfo basic_schd;
	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	CTG_ScheduleInfo shift_schd;
	shift_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT; 
	shift_schd.m_nSectionID=section;
	m_cShiftSchduleData.SetScheduleInfo(shift_schd); 
	m_cShiftSchduleData.DeleteAllData(shift_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicPrevSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicPrevSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicBackupSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicBackupSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);
	

	return TRUE;
}

BOOL CWorkScheduleHolder::LoadDataFromLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	//DeleteAllData();
	CTG_ScheduleInfo info;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		m_cBasicSchduleData.GetScheduleInfo(info);
	    m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		m_cBasicPrevSchduleData.GetScheduleInfo(info);
	    m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicPrevSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		m_cBasicBackupSchduleData.GetScheduleInfo(info);
	    m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicBackupSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		m_cWorkSchduleData.GetScheduleInfo(info);
	    m_cWorkSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cWorkSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		m_cShiftSchduleData.GetScheduleInfo(info);
	    m_cShiftSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cShiftSchduleData);
	}
	return TRUE;
}
BOOL CWorkScheduleHolder::SaveDataToLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE  type)
{
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
		TG_SaveToFiles(&m_cBasicSchduleData);
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
		TG_SaveToFiles(&m_cWorkSchduleData);
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
		TG_SaveToFiles(&m_cShiftSchduleData);
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
		TG_SaveToFiles(&m_cBasicPrevSchduleData);
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP || type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
		TG_SaveToFiles(&m_cBasicBackupSchduleData);
		
	return TRUE;
}

void  CWorkScheduleHolder::SetUserID(int id)
{
	m_cWorkSchduleData.SetUserSpeical(id);
	m_cShiftSchduleData.SetUserSpeical(id);
	m_cBasicSchduleData.SetUserSpeical(id);
	m_cBasicPrevSchduleData.SetUserSpeical(id);
}

BOOL CWorkScheduleHolder::ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE  tp,BOOL delfirst)  // ���»�ȡ
{
	IScheduleComm *pcomm;
	if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		if(delfirst)
			DeleteAllData();// 20090704
		
		if(m_nMode==LOCAL_MODE)
		{
			pcomm=m_pMasterDatabaseComm;
			ReadSchedule(&m_cBasicSchduleData,pcomm);  // �������ͼ
			ReadSchedule(&m_cWorkSchduleData,pcomm);
			ReadSchedule(&m_cShiftSchduleData,pcomm);
			pcomm->SetCommReady(TRUE); 
		}
		else if(m_nMode==SERVER_MODE)
		{
			InitCommInterfaceStatus(m_pMasterServerCommBasic,FALSE);
			InitCommInterfaceStatus(m_pMasterServerCommWork,FALSE);
			InitCommInterfaceStatus(m_pMasterServerCommShift,FALSE);
			if(m_pMasterServerCommBasic)
			{
				m_pMasterServerCommBasic->SetTrainDataInited(FALSE);
			}
			if(m_pMasterServerCommWork)
			{
				m_pMasterServerCommWork->SetTrainDataInited(FALSE);
				m_pMasterServerCommWork->SetMarkDataInited(FALSE);
			}
			if(m_pMasterServerCommShift)
			{
				m_pMasterServerCommShift->SetTrainDataInited(FALSE);
				m_pMasterServerCommShift->SetMarkDataInited(FALSE);
			}
		}
		else
			return FALSE;
	}
	else
	{
		// ע��: ����û�п��� Database���������ΪDatabase Comm ������û��ʲôǰ;
		CTG_ScheduleInfo info;
		if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			m_cBasicSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC ������");
			}
			ReadSchedule(&m_cBasicSchduleData,m_pMasterServerCommBasic);  // �������ͼ
		}
		else if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			m_cBasicPrevSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC_PREV ������");
			}
			ReadSchedule(&m_cBasicPrevSchduleData,m_pMasterServerCommBasicPrev);  // �������ͼ

		}
		else if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			m_cBasicBackupSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC_BACKUP ������");
			}
			ReadSchedule(&m_cBasicBackupSchduleData,m_pMasterServerCommBasicBackup);  // �������ͼ
		}
		else if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			m_cShiftSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cShiftSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_SHIFT ������");
			}
			ReadSchedule(&m_cShiftSchduleData,m_pMasterServerCommShift);  // �������ͼ
		}
		else
		{
			m_cWorkSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cWorkSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] schd_type %d ������", tp);
			}
			ReadSchedule(&m_cWorkSchduleData,m_pMasterServerCommWork);  // �������ͼ
		}
	}
	return TRUE;
}

BOOL  CWorkScheduleHolder::ShiftWorkSchedule(const CTG_ScheduleInfo &info)
{
	CTG_ScheduleInfo prev_info;
	m_cWorkSchduleData.GetScheduleInfo(prev_info);
	 
	//m_cWorkSchduleData.DeleteAllData(prev_info,SYCFLAG_NULL);//����ȫ��ɾ���������ڱ���ģʽ�����ݶ�ʧ
	//
	m_cWorkSchduleData.DeleteDataInMemory(0,info.m_tStartTime,FALSE,TRUE);
	prev_info.m_nShiftID=info.m_nShiftID;
	prev_info.m_tStartTime=info.m_tStartTime;
	prev_info.m_tEndTime=info.m_tEndTime;
	m_cWorkSchduleData.SetScheduleInfo(prev_info);
	  
 
	prev_info.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	 
	//m_cShiftSchduleData.DeleteAllData(prev_info,SYCFLAG_NULL); //����ȫ��ɾ���������ڱ���ģʽ�����ݶ�ʧ

    m_cShiftSchduleData.DeleteDataInMemory(0,info.m_tStartTime,FALSE,TRUE);
	m_cShiftSchduleData.SetScheduleInfo(prev_info);
	 

	return TRUE;
}

BOOL  CWorkScheduleHolder::RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE sch_type,IScheduleComm *pcomm, bool bMMI)
{
	int i;
	for(i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface==NULL)
			break;
	}
	if(i>=MAX_COMM_INTF_NUM)
		return FALSE;

	 m_aCommArray[i].pCommInterface=pcomm;
	 m_aCommArray[i].nSchType=sch_type;
	 pcomm->m_bForMMI=bMMI;
	 
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	 {
		 m_cBasicSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	 {
		 m_cBasicPrevSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicPrevSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	 {
		 m_cBasicBackupSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicBackupSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	 {
		 m_cShiftSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cShiftSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	 {
		m_cBasicSchduleData.SetNeedSyncFlag(i,TRUE);
		m_cWorkSchduleData.SetNeedSyncFlag(i,TRUE);
		m_cShiftSchduleData.SetNeedSyncFlag(i,TRUE);
		m_cBasicPrevSchduleData.SetNeedSyncFlag(i,TRUE);
		pcomm->SetDataStore(&m_cWorkSchduleData);
		pcomm->SetDataStore(&m_cBasicSchduleData);
		pcomm->SetDataStore(&m_cShiftSchduleData);
		pcomm->SetDataStore(&m_cBasicPrevSchduleData);
	 }
	 else
	 {
     	m_cWorkSchduleData.SetNeedSyncFlag(i,TRUE);
		pcomm->SetDataStore(&m_cWorkSchduleData);
	 }
     
	 if(pcomm->GetInterfaceFlag()==SYCFLAG_NULL)
		pcomm->SetInterfaceFlag(i); 
	 return TRUE;
}

BOOL  CWorkScheduleHolder::UnRegisterCommInterface(IScheduleComm *pcomm)
{
	int i;
	for(i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface && m_aCommArray[i].pCommInterface==pcomm)
		{
			m_aCommArray[i].pCommInterface=NULL;
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			{
				 pcomm->ClearDataStore(&m_cBasicSchduleData);
				 m_cBasicSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			{
				 pcomm->ClearDataStore(&m_cBasicPrevSchduleData);
				 m_cBasicPrevSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
			{
				pcomm->ClearDataStore(&m_cBasicBackupSchduleData);
				m_cBasicBackupSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			{
				 pcomm->ClearDataStore(&m_cShiftSchduleData);
				 m_cShiftSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
			{
				pcomm->ClearDataStore(&m_cShiftSchduleData);
				pcomm->ClearDataStore(&m_cBasicSchduleData);
				pcomm->ClearDataStore(&m_cWorkSchduleData);
				pcomm->ClearDataStore(&m_cBasicPrevSchduleData);
				pcomm->ClearDataStore(&m_cBasicBackupSchduleData);
				m_cBasicSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cWorkSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cShiftSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cBasicPrevSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cBasicBackupSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			{
				pcomm->ClearDataStore(&m_cWorkSchduleData);
				m_cWorkSchduleData.SetNeedSyncFlag(i,FALSE);
			}
	 		m_aCommArray[i].nSchType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL;
		}
	}
	return TRUE;
}



void  CWorkScheduleHolder::DeleteAllData()
{
	CTG_ScheduleInfo info;
	m_cWorkSchduleData.GetScheduleInfo(info);
	m_cWorkSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
	m_cBasicSchduleData.GetScheduleInfo(info);
	m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	m_cShiftSchduleData.GetScheduleInfo(info);
	m_cShiftSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	m_cBasicPrevSchduleData.GetScheduleInfo(info);
	m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	m_cBasicBackupSchduleData.GetScheduleInfo(info);
	m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	GpcLogTrainMessage(0, 0, "[deleteall] all schd_type ������");
}

BOOL  CWorkScheduleHolder::ReadSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm)
{
	 if(!pcomm)
		 return FALSE;
	 CTG_ScheduleInfo info;
	 pData->GetScheduleInfo(info);
	 if(info.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL) 
		return pcomm->CopyAllData(info,pData,TRUE);
	 else
		 return FALSE;
}

BOOL  CWorkScheduleHolder::WriteSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL allwrite)
{
	 if(!pcomm || !pData)
		 return FALSE;

	 CTG_ScheduleInfo info;
	 pData->GetScheduleInfo(info);
	 return pData->CopyAllData(info,pcomm,allwrite);
}

BOOL  CWorkScheduleHolder::WriteSchedule(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,BOOL allwrite)
{
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
		 return WriteSchedule(&m_cBasicSchduleData,pcomm,allwrite);
	 else
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
		 return WriteSchedule(&m_cShiftSchduleData,pcomm,allwrite);
	 else
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
		 return WriteSchedule(&m_cBasicPrevSchduleData,pcomm,allwrite);
	 else
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		 return WriteSchedule(&m_cBasicBackupSchduleData,pcomm,allwrite);
	 else
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	 {
		 WriteSchedule(&m_cBasicBackupSchduleData,pcomm,allwrite);
		 WriteSchedule(&m_cBasicPrevSchduleData,pcomm,allwrite);
		 WriteSchedule(&m_cBasicSchduleData,pcomm,allwrite);
		 WriteSchedule(&m_cShiftSchduleData,pcomm,allwrite);
		 return WriteSchedule(&m_cWorkSchduleData,pcomm,allwrite);
	 }
	 else
	 {
		 return WriteSchedule(&m_cWorkSchduleData,pcomm,allwrite);
	 }
	 
	
}

IScheduleComm  *CWorkScheduleHolder::GetServerCommByScheduleType(CTG_ScheduleInfo::SCHEDULE_TYPE schd_type)
{
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		return m_pMasterServerCommBasic;
	else
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		return m_pMasterServerCommShift;
	else // WORK & HIST
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return m_pMasterServerCommBasicPrev;
	else
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		return m_pMasterServerCommBasicBackup;
	else
		return m_pMasterServerCommWork;

}
void  CWorkScheduleHolder::PostCommStatus(IScheduleComm *pcomm,BOOL active)
{
		if(pcomm==m_pMasterDatabaseComm  && m_nMode==LOCAL_MODE)
		{
			if(active)
			{  // ���»�ȡ����
				
				ReadSchedule(&m_cWorkSchduleData,pcomm);
				ReadSchedule(&m_cShiftSchduleData,pcomm);	
				ReadSchedule(&m_cBasicSchduleData,pcomm);  // �������ͼ

				ReqBasicScheduleName();
			}	
		}
		else
        if(pcomm==m_pMasterServerCommWork && m_nMode==SERVER_MODE)
		{
			if(active)
				 ReadSchedule(&m_cWorkSchduleData,pcomm);
		}
		else
		if(pcomm==m_pMasterServerCommShift && m_nMode==SERVER_MODE)
		{
			if(active)
				 ReadSchedule(&m_cShiftSchduleData,pcomm);

		}
		else
		if(pcomm==m_pMasterServerCommBasic && m_nMode==SERVER_MODE)
		{
			if(active)
			{
				ReadSchedule(&m_cBasicSchduleData,pcomm);
				ReqBasicScheduleName();
			}
		}
		else
		if(pcomm==m_pMasterServerCommBasicPrev && m_nMode==SERVER_MODE)
		{
			if(active)
				 ReadSchedule(&m_cBasicPrevSchduleData,pcomm);
		}
		return;
}
void  CWorkScheduleHolder::InitCommInterfaceStatus(IScheduleComm *pcomm,BOOL va)
{
	if(!pcomm)
		return;
	int i=pcomm->GetInterfaceFlag();
	if(i>=0 && i<MAX_COMM_INTF_NUM)
	{
		if(m_aCommArray[i].pCommInterface==pcomm)
			m_aCommArray[i].bActive=va;
	}
}

// 2013.05.07��������ͼ��Commsvr����
void CWorkScheduleHolder::RefreshCommStatus()
{
	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface)
		{
			m_aCommArray[i].pCommInterface->PollingAndCheckStatus(); //�������״̬
			PostCommStatus(m_aCommArray[i].pCommInterface,FALSE);
			m_aCommArray[i].bActive=FALSE;
		}
	}
}

BOOL  CWorkScheduleHolder::PollingData(TIME curtm)
{
	m_cWorkSchduleData.PollingForUpdate(TRUE,curtm);
	m_cShiftSchduleData.PollingForUpdate(TRUE,curtm);
	m_cBasicSchduleData.PollingForUpdate(TRUE,curtm);
	m_cBasicPrevSchduleData.PollingForUpdate(TRUE,curtm);
    for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		//20110420
		if(m_pMasterServerCommWork!=NULL && m_pMasterServerCommBasic!=NULL && m_pMasterServerCommShift!=NULL)
		{
			if((m_aCommArray[i].pCommInterface==m_pMasterServerCommShift) &&
				( (!m_pMasterServerCommWork->DataOK()  && m_pMasterServerCommWork->CommReady())
				||(!m_pMasterServerCommBasic->DataOK() && m_pMasterServerCommBasic->CommReady()) ))
			{
				continue;
			}
		}

		if(m_aCommArray[i].pCommInterface)
		{
			m_aCommArray[i].pCommInterface->PollingAndCheckStatus(); //�������״̬
			if(m_aCommArray[i].pCommInterface->CommReady() && !m_aCommArray[i].bActive)  // ״̬���
			{
				PostCommStatus(m_aCommArray[i].pCommInterface,TRUE);
				m_aCommArray[i].bActive=TRUE;
			}
			else
			if(!m_aCommArray[i].pCommInterface->CommReady() && m_aCommArray[i].bActive)  // ״̬����
			{
				PostCommStatus(m_aCommArray[i].pCommInterface,FALSE);
				m_aCommArray[i].bActive=FALSE;
			}
		}
	}

	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface)
		{
			if(m_aCommArray[i].pCommInterface->CommReady())
			{
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
				{
					PollingForUpdate(&m_cBasicSchduleData,m_aCommArray[i].pCommInterface,FALSE);
					PollingForUpdate(&m_cWorkSchduleData,m_aCommArray[i].pCommInterface,FALSE);
					PollingForUpdate(&m_cShiftSchduleData,m_aCommArray[i].pCommInterface,FALSE);
					PollingForUpdate(&m_cBasicPrevSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
				{
					PollingForUpdate(&m_cBasicSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				{
					PollingForUpdate(&m_cBasicPrevSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
				{
					PollingForUpdate(&m_cBasicBackupSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
				{
					PollingForUpdate(&m_cShiftSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				{
					PollingForUpdate(&m_cWorkSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
			}
		}
	}
	return TRUE;
}

BOOL   CWorkScheduleHolder::PollingForUpdate(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL all_flag)
{
	CTG_SingleTrain *ptrain;
	CTG_SingleMark  *pmark;
	BOOL rc=FALSE;
	int sync_flag;
	if(!pcomm || !pData)
		return FALSE;

	int nCountOnceSend=99999;
	if((all_flag==FALSE) && pcomm->m_bForMMI) // �����������ͬ��ÿ������10��
	{
		all_flag=TRUE;
		nCountOnceSend=20;
	}

	sync_flag=pcomm->GetInterfaceFlag();
	do
	{
		CTG_ScheduleInfo schd;
		CTG_SingleTrain  srctrain;
		CTG_SingleMark   srcmark;
		BOOL   bSyncComm=pcomm->IsAsyncMode();
		pData->Lock(); 
		ptrain=pData->Find1NoSyncTrain(&schd,sync_flag,!bSyncComm); //�����첽COMM,���������ڵȴ�Ӧ��Ķ���
		pmark=pData->Find1NoSyncMark(&schd,sync_flag,!bSyncComm);
		if(ptrain)
		{
			srctrain=*ptrain;
			if(pcomm->IsAsyncMode()) // �첽ģʽ
				ptrain->StartSyncing(sync_flag);
			else
				ptrain->SetSyncStatus(sync_flag,TRUE); 
		}
		
		if(pmark)
		{
			srcmark=*pmark;
			if(pcomm->IsAsyncMode()) // �첽ģʽ		
				pmark->StartSyncing(sync_flag);
			else
				pmark->SetSyncStatus(sync_flag,TRUE); 
		}
		pData->UnLock(); 

		if(ptrain)
		{

			if(srctrain.IsDeletedObject())
			{
				pcomm->DeleteTrain(schd,srctrain,SYCFLAG_MEMORY);
			}
			else
			{
				pcomm->UpdateTrain(schd,srctrain,SYCFLAG_MEMORY);
			}

			rc=TRUE;
		}

		if(pmark)
		{
			if(srcmark.IsDeletedObject())
				pcomm->DeleteMark(schd,srcmark,SYCFLAG_MEMORY);
			else
				pcomm->UpdateMark(schd,srcmark,SYCFLAG_MEMORY);
			rc=TRUE;
		}
		if(!ptrain && !pmark)
		{
			break;
		}
		else 
		{
			//GpcLogTrainMessage(0, 0, "ͬ������[%d] nCountOnceSend %ld ", schd.m_nType, nCountOnceSend);
			nCountOnceSend--;
			if(nCountOnceSend<0)
			{
				break;
			}
		}
	}while(all_flag);
	return rc;
}

BOOL  CWorkScheduleHolder::GenTrain_FreeDraw(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL qualify_runtime,BYTE run_type) // train���ǽ������ɵĳ�ʼ����
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)   
		pCurData=&m_cBasicPrevSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)   
		pCurData=&m_cBasicBackupSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)   // ����ͼ����
	{
		pCurData=&m_cBasicSchduleData;
		pBasicData=NULL;
	}
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
    CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrain_FreeDraw(&srctrain,qualify_runtime,run_type))
		return FALSE;
	
	if(schd.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)  //��WORK_SCHD�Զ������ͳ�ͻ���
		return TRUE;
	
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
			AdjustTrainForCollision(srctrain,0,schd);
	adjust_logic.CheckAndSetAlarmInfo(&srctrain);
	CTGAddTrainLogic::AutoChangeTrainID(&srctrain, schd.m_nSectionID, 1);
	CTGAddTrainLogic::ChangeTrainElec(&srctrain, schd.m_nSectionID);
	return TRUE;
}

bool CWorkScheduleHolder::TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG)
{
	GpcLogTrainMessage(0, 0, "TGOCXSetTrainExitEntry ��ͼ�ؼ���ʼ��������");
	if(pTG==NULL)
	{
		GpcLogTrainMessage(0, 0, "�ؼ�ָ��Ϊ��");
		return false;
	}
	
	// ������վ����ڡ�δվ������
	CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
	CTG_TrainRecord *plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
	if(pfirst_rec==NULL || plast_rec==NULL)
	{
		GpcLogTrainMessage(0, 0, "�г���¼�쳣");
		return false;
	}
	int entry=pfirst_rec->GetEntry();
	int	exit=plast_rec->GetExit();

	bool bResult = TGOCXSetTrainExitEntry(train, pTG, entry, exit);
	return bResult;
}

BOOL  CWorkScheduleHolder::GenTrain_OPMS(CTG_SingleTrain &srctrain,CTG_SingleTrain &othertrain,const CTG_ScheduleInfo &schd,BOOL newflag)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrain_OPMS(&srctrain,newflag))
		return FALSE;

	srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());  
	for(int i=0;i<srctrain.GetTrainRecordCount()-1;i++)
	{
		CTG_TrainRecord *prec=srctrain.GetTrainRecord(i);
		CTG_TrainRecord *nextprec=srctrain.GetTrainRecord(i+1);
		int station1next=-1;
		int station2pre=-1;
		int first_entry=-1;
		int first_exit=-1;
		int second_entry=-1;
		int second_exit=-1;
		if(GetTGFunctionConfig()->MatchSplitTrain(schd.m_nSectionID,prec->GetStationNo(),nextprec->GetStationNo(),&station1next,&station2pre,&first_entry,&first_exit,&second_entry,&second_exit))
		{
			//yuzhu begin �۷��г��ֳ�����
			int absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(prec->GetStationNo(),station1next,srctrain.IsDownDirection()?1:0);
			if(absnum>0)
				prec->m_data.exit=GetEntryByTmisAbsNumber(prec->GetStationNo(),absnum);

			if(station1next == station2pre)
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),!srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			else
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			//yuzhu end
			 logic.SplitTrain(&srctrain,&othertrain,i+1,SHIFT,first_entry,first_exit,second_entry,second_exit);
			 break;
		}
	}

	// 20090931 qiaoyt
	othertrain.SetShiftTrainIndex(othertrain.GetShiftTrainIndex() | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
	othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());
	if(othertrain.GetTrainRecordCount()>0)
	{
		adjust_logic.CacuTrainSlowTime(&othertrain); // 20100813 ����ϻ�����
		TRAIN_INDEX tidx1=srctrain.GetShiftTrainIndex();
		TRAIN_INDEX tidx2=othertrain.GetShiftTrainIndex();
		if(tidx1==tidx2)
		{
			GpcLogTrainMessage(0, 0, "�հ�ƻ����ηָ�, ��ʧ������ %s, ����OPMSȫ��ID %d ��bit30λΪ1", srctrain.GetTrainID(), tidx1);
		}
	}

	// �����г���Դ��shift
    srctrain.SetFromShift(TRUE);

	adjust_logic.CacuTrainSlowTime(&srctrain); // 20100813 ����ϻ�����
	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrainBasicSchedule_OPMS(CTG_SingleTrain &srctrain,CTG_SingleTrain &othertrain,const CTG_ScheduleInfo &schd)
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		pCurData=&m_cBasicSchduleData;
		pBasicData=NULL;
	}
	else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		pCurData=&m_cBasicBackupSchduleData;
		pBasicData=NULL;
	}
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		pCurData=&m_cBasicPrevSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
    CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrainBasic_OPMS(&srctrain))
		return FALSE;
	srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex()); 
	for(int i=0;i<srctrain.GetTrainRecordCount()-1;i++)
	{
		CTG_TrainRecord *prec=srctrain.GetTrainRecord(i);
		CTG_TrainRecord *nextprec=srctrain.GetTrainRecord(i+1);
		int station1next=-1;
		int station2pre=-1;
		int first_entry=-1;
		int first_exit=-1;
		int second_entry=-1;
		int second_exit=-1;
		if(GetTGFunctionConfig()->MatchSplitTrain(schd.m_nSectionID,prec->GetStationNo(),nextprec->GetStationNo(),&station1next,&station2pre,&first_entry,&first_exit,&second_entry,&second_exit))
		{
			//yuzhu begin �۷��г��ֳ�����
			int absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(prec->GetStationNo(),station1next,srctrain.IsDownDirection()?1:0);
			if(absnum>0)
				prec->m_data.exit=GetEntryByTmisAbsNumber(prec->GetStationNo(),absnum);

			if(station1next == station2pre)
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),!srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			else
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			//yuzhu end
			 logic.SplitTrain(&srctrain,&othertrain,i+1,BASIC,first_entry,first_exit,second_entry,second_exit);
			 break;
		}
	}

	othertrain.SetShiftTrainIndex(othertrain.GetShiftTrainIndex() | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
	othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());
	if(othertrain.GetTrainRecordCount()>0)
	{
		TRAIN_INDEX tidx1=srctrain.GetShiftTrainIndex();
		TRAIN_INDEX tidx2=othertrain.GetShiftTrainIndex();
		if(tidx1==tidx2)
		{
			GpcLogTrainMessage(0, 0, "����ͼ���ηָ�, ��ʧ������ %s, ����OPMSȫ��ID %d ��bit30λΪ1", srctrain.GetTrainID(), tidx1);
		}
	}

	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrainTmp_Passway(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CString train,int  pathway, BYTE start_flag, BYTE end_flag,BYTE start_index, BYTE end_index,TIME ltime,BYTE run_type)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
    if(!logic.GenTrainTmp_Passway(&srctrain,train,schd.m_nSectionID,pathway,start_flag,end_flag, start_index, end_index,ltime,ltime,run_type))
		return FALSE;
	if(schd.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)  //��WORK_SCHD�Զ������ͳ�ͻ���
		return TRUE;
	
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
			AdjustTrainForCollision(srctrain,0,schd);
	adjust_logic.CheckAndSetAlarmInfo(&srctrain);
	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrain_Passway(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const TRAIN_NO train,int  pathway, BYTE start_flag, BYTE end_flag ,STATION_NO station,TIME ltime,BYTE run_type)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
    if(!logic.GenTrain_Passway(&srctrain,train,schd.m_nSectionID,pathway,start_flag,end_flag,station,ltime,ltime,run_type))
		return FALSE;
	if(schd.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)  //��WORK_SCHD�Զ������ͳ�ͻ���
		return TRUE;
	
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
			AdjustTrainForCollision(srctrain,0,schd);
	adjust_logic.CheckAndSetAlarmInfo(&srctrain);
	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrain_BasicSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,TRAIN_INDEX index,STATION_NO station, TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	CTG_SingleTrain  *pbasic_train;
	pbasic_train=m_cBasicSchduleData.GetSingleTrain(index);
	if(!pbasic_train)
	{
		GpcLogTrainMessage(0, 0, "����ͼ���Ҳ��� basic_index=%d ���г�", index);
		return FALSE;
	}
	srctrain.SetPrevTrainIndex(0);
	return logic.GenTrain_BasicSchedule(&srctrain, *pbasic_train,station,bm,em,NULL,NULL);  
}
BOOL  CWorkScheduleHolder::GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const char *train_no,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);

	CTG_SingleTrain  *pbasic_train;
	pbasic_train=m_cShiftSchduleData.GetSingleTrain(train_no,bm,em,FALSE);
	if(!pbasic_train)
	{
		GpcLogTrainMessage(0, 0, "�հ�ƻ����Ҳ��� %s",train_no);
		return FALSE;
	}
	srctrain=*pbasic_train;
	srctrain.SetPrevTrainIndex(0);
	logic.GenTrain_ShiftSchedule(&srctrain);
	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,TRAIN_INDEX index,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	CTG_SingleTrain  *pbasic_train;
	pbasic_train=m_cShiftSchduleData.GetSingleTrain(index);
	if(!pbasic_train)
	{
		GpcLogTrainMessage(0, 0, "�հ�ƻ����Ҳ�������Ϊ %d���г�",index);
		return FALSE;
	}
	srctrain=*pbasic_train;
	srctrain.SetPrevTrainIndex(0);
	logic.GenTrain_ShiftSchedule(&srctrain);
	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrain_BasicSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const TRAIN_NO trainno,STATION_NO station, TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
       
	CTG_SingleTrain  *pbasic_train;
	pbasic_train=m_cBasicSchduleData.GetSingleTrain(trainno);
	if(!pbasic_train)
	{
		GpcLogTrainMessage(0, 0, "����ͼ���Ҳ��� trainname=%s ���г�", trainno);
		return FALSE;
	}
	srctrain.SetPrevTrainIndex(0);
	return logic.GenTrain_BasicSchedule(&srctrain, *pbasic_train,station,bm,em,trainno,NULL);  
}

BOOL  CWorkScheduleHolder::TrainAlreadyExsitInWorkSchedule(const TRAIN_NO trainno)
{
 	int i=0;
	CTG_ScheduleInfo schd;
	m_cWorkSchduleData.GetScheduleInfo(schd);
	CTG_TrainPtrGroup group;
	m_cWorkSchduleData.FindTrainsNotOrder(group);
	time_t curtm=time(NULL)-CTGAddTrainLogic::ADDTRAIN_BASIC_SCHEDULE_MAX_BEFORE_TIME;
	for(int train_iter=0;train_iter<group.GetTrainCount();train_iter++)
	{
		CTG_SingleTrain &train=*group.GetTrainPtr(train_iter);
		if(train.MatchTrainID(trainno))
		{
				CTG_TrainRecord *prec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
				if(prec)
				{
					if(prec->m_data.arrive>curtm    // ͬ���ĳ��Σ����ڽϽ���ʱ��
						|| prec->m_data.depart>curtm)
						break;
				}
		}
	}
	if(train_iter<group.GetTrainCount())
		return TRUE;
	else
		return FALSE;
}

const char *CWorkScheduleHolder::CheckTrainManualUpdateOper(CTG_SingleTrain *pnewtrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,DWORD& status)// return NULL ok,or ERROR info,status errcode
{
	status=0;

	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return NULL;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	return adjust_logic.CheckTrainManualUpdateOper(pnewtrain,oper_flag,status);// return NULL ok,or ERROR info,status errcode
}

BOOL  CWorkScheduleHolder::GenTrain_AutoAdjustManualUpdate(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,CWnd* pParentWnd)
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)   
		pCurData=&m_cBasicPrevSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		pCurData=&m_cBasicBackupSchduleData;
		pBasicData=NULL;
	}
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)   // ����ͼ����
	{
		pCurData=&m_cBasicSchduleData;
		pBasicData=NULL;
	}
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
	if(!adjust_logic.AdjustTrainForManualUpdate(&srctrain,oper_flag,schd.m_nSectionID,pParentWnd))
		return FALSE;

	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV ||
		schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		int offset=0;
		CTG_TrainRecord *rec=srctrain.GetTrainRecord(oper_flag.m_nRecIndex);
		CTG_TrainRecord *pnext=srctrain.GetTrainRecord(oper_flag.m_nRecIndex+1);
		if(rec!=NULL && pnext!=NULL)
			offset=rec->m_data.depart-pnext->m_data.arrive;

		if(offset>0)
		{
			offset+=60;
			for(int i=oper_flag.m_nRecIndex+1;i<srctrain.GetTrainRecordCount();i++)
			{
				CTG_TrainRecord *prec=srctrain.GetTrainRecord(i);
				if(!prec)
					continue; 
				prec->m_data.arrive+=offset;
				prec->m_data.depart+=offset;
			}
		}
	}

	if(oper_flag.nFlag & CMMIOperFlag::CONTINUE_DRAW_TRAIN)  // ��������
	{
		if(oper_flag.m_nRecIndex>=0) // ��ֹ�ӵ�һ����������ߺ��ʼ������ȷ 2014.4.30
		{
			CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
			logic.SetDefaultAttribForNewRecords(&srctrain,oper_flag.m_nRecIndex);
			CTGAddTrainLogic::AutoChangeTrainID(&srctrain, schd.m_nSectionID);
		} 
	}
	if(oper_flag.nFlag & CMMIOperFlag::ENGINE_INFO_CHANGED)  // ����ǻ�����Ϣ�仯�����õ���
	{
		srctrain.SetElecFlagByEngineInfo(); 
	}

	CTG_SingleTrain *pold=pCurData->GetSingleTrain(srctrain.GetTrainIndex());
	if(NULL != pold)
	{	
		// 2014.05.27 ���� ����ʱ������
		bool enable=GetTGBasicParam()->IsEnableABSTimeType();
		if(oper_flag.nFlag & CMMIOperFlag::TRAIN_ATTRIB_CHANGED)
		{
			if(enable)
			{
				BYTE nOldAbsTimeType=GetTrainAbsTimeType(pold);
				BYTE nNewAbsTimeType=GetTrainAbsTimeType(&srctrain);
				
				if(nNewAbsTimeType!=nOldAbsTimeType)
				{
					if(SetNewTrainAbsRunTime(srctrain,oper_flag.m_nRecIndex,nNewAbsTimeType))
					{
						SetTrainAbsTimeType(&srctrain, nNewAbsTimeType);
						GpcLogTrainMessage(0,0,"����Ա�޸����Դ����趨 train:%s ʹ������ʱ������ %d ���¼�������ʱ��", srctrain.GetTrainID(), nNewAbsTimeType);
					}
					else
					{
						SetTrainAbsTimeType(&srctrain, nOldAbsTimeType);
					}
				}
			}
		}

		if(srctrain.GetTrainType()!=pold->GetTrainType())  // ���͸ı� 20140918
		{
			BYTE run_type=0,prio=0,locotype=0;
			BOOL elec=TRUE;
			if(GetTGBasicParam()->GetTrainTypeDef(srctrain.GetTrainType(),&run_type,&prio,&locotype))
			{
				if(!enable)
				{
					if(oper_flag.m_nStation==0) // 0���޸�run_type 1�����޸�
						srctrain.GetTrainInfo()->m_data.run_type= run_type;
					srctrain.GetTrainInfo()->m_data.priority = prio;
				}
				srctrain.SetDefaultAttribByDefinedFlag(locotype);
			}
		}

		if(oper_flag.nFlag & CMMIOperFlag::TRAIN_ATTRIB_CHANGED)
		{
			// ������OCX���ó��ޱ�־
			if(srctrain.GetTrainInfo()->m_data.space_rank!=pold->GetTrainInfo()->m_data.space_rank)
				srctrain.SetSpaceRank(srctrain.GetTrainInfo()->m_data.space_rank, 0, true, true); // ���ü�¼�ĳ��ޱ�־ 
		}

		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			srctrain.ComparePrevAndUpateFlags(*pold);   // ���ڼ�¼ɾ��������Ᵽ������  //// 521�ƻ��Ż��޸�
	}

	if(schd.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return TRUE;

    if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
		AdjustTrainForCollision(srctrain,oper_flag.m_nRecIndex,schd);

	adjust_logic.CheckAndSetAlarmInfo(&srctrain);
	
	adjust_logic.CacuTrainSlowTime(&srctrain); // 20090917 ����ϻ�����

	return TRUE;
}

BOOL  CWorkScheduleHolder::CheckStplan(CTG_SingleTrain* ptrain, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup, int rec_index)
{
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,&m_cWorkSchduleData);
  	adjust_logic.CheckStplan(ptrain, start_time, end_time, group, markgroup, rec_index);
	return TRUE;
}

BOOL  CWorkScheduleHolder::IsSyncedWithServer(CTG_ScheduleInfo::SCHEDULE_TYPE  schd_type,const CTG_LogicObject *pobj)
{
	IScheduleComm *pcomm=GetServerCommByScheduleType(schd_type);
	if(pcomm)
		return pobj->CheckSyncStatus(pcomm->GetInterfaceFlag());
	else
		return FALSE;
}

BOOL  CWorkScheduleHolder::IsSyncedWithDatabase(const CTG_LogicObject *pobj)
{
	if(!m_pMasterDatabaseComm)
		return pobj->CheckSyncStatus(m_pMasterDatabaseComm->GetInterfaceFlag());
	else
		return FALSE;
}

int   CWorkScheduleHolder::GetNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,CString &Info)
{
	if(!pcomm)
		return -1;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK || type==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
		return (m_cWorkSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cWorkSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
		
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		return (m_cBasicSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return (m_cBasicPrevSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicPrevSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		return (m_cBasicBackupSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicBackupSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		return (m_cShiftSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cShiftSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
		return -2;
}

int   CWorkScheduleHolder::GetServerNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type, CString &Info)
{
	IScheduleComm *pcomm;
	pcomm=GetServerCommByScheduleType(type);
	if(pcomm)
		return GetNoSyncedObjectCount(type,pcomm,Info);
	else
		return -2;
}
int   CWorkScheduleHolder::GetDatabaseNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type)
{
	CString tmp="";
	return GetNoSyncedObjectCount(type,m_pMasterDatabaseComm,tmp);
}

// ����ͼ���հ�ƻ���ͼʱ���г���Ŀ���������Ŀ�жϴ���-1���г������������ 0��û�ж�ȡ���г� 1����ȡ�ɹ�
int  CWorkScheduleHolder::SelectSchdTrain(const TGBasicTrainListRequest &msg,TGSelectedBasicTrain info[],int *limit,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL IsStart)
{
	int startdate=0;
	if(msg.etime==msg.stime)
	{
		*limit=0;
		return 0;
	}
	CTG_TrainScheduleBase *pDataBase;

	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		pDataBase=&m_cBasicSchduleData;
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pDataBase=&m_cShiftSchduleData;
	else
		pDataBase=&m_cWorkSchduleData;

	CTG_TrainPtrGroup group;
	pDataBase->FindTrains(group);
	int i=0;
	bool bErr=false;  // �����г���Ŀ���������Ŀ�жϴ���
	for(int train_iter=0;train_iter<group.GetTrainCount();train_iter++)
	{
		CTG_SingleTrain &train=*group.GetTrainPtr(train_iter);
		do
		{
			bErr=false;
			const CTG_TrainRecord *rec_temp = NULL;
			if(msg.station >0)
			{ 
				rec_temp = train.GetTrainRecordByStation_Const(msg.station); // �ж�;����վ
				if(rec_temp == NULL)
					break;
			}
			//ʼ���г�
			if(msg.flag==TGBasicTrainListRequest::INIT_START)
			{
				if(rec_temp != NULL)
				{
					if(!rec_temp->IsStartRec())
						break;
					else
					{
						rec_temp = train.GetTrainRecord(0); // �жϵ�һ����վ
						if(rec_temp != NULL)
						{
							if(!rec_temp->IsStartRec())
								break;
						}
					}
				}
				else
				{
					rec_temp = train.GetTrainRecord(0); // �жϵ�һ����վ
					if(rec_temp != NULL)
					{
						if(!rec_temp->IsStartRec())
							break;
					}
				}
			}
			//��ʼ���г�
			if(msg.flag==TGBasicTrainListRequest::INIT_ACCEPT)
			{				
				rec_temp = train.GetTrainRecord(0); // �жϵ�һ����վ
				if(rec_temp != NULL)
				{
					if(rec_temp->IsStartRec())
						break;
				}				
			}
			if(msg.etime>msg.stime)
			{
				if(!train.IfInTimeScope(msg.stime*3600,msg.etime*3600,IsStart))
					break;
			}
			else if(!train.IfInTimeScope(msg.stime*3600,24*3600,IsStart)	&& !train.IfInTimeScope(0,msg.etime*3600,IsStart))
				break;

			if(msg.train_type==TGBasicTrainListRequest::INIT_PASSENGER
				&& !TG_IsPassengerTrainType(train.GetTrainType()))
				break;
			if(msg.train_type==TGBasicTrainListRequest::INIT_FREIGHT
				&& !TG_IsFreightTrainType(train.GetTrainType()))
				break;
			if(msg.direction==TGBasicTrainListRequest::INIT_DOWN
				&& !train.IsDownDirection())
				break;
			if(msg.direction==TGBasicTrainListRequest::INIT_UP
				&& !train.IsUpDirection())
				break;

			CTG_TrainRecord *prec=train.GetTrainRecord(0);
			if(!prec)
				break;

			if(i>=*limit)
			{
				bErr=true; // �����������,�˳�
				break;
			}

			info[i].train_index = train.GetTrainIndex();
			strcpy((char*)info[i].train_id,train.GetTrainID());
			if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			{
				if(TrainAlreadyExsitInWorkSchedule(info[i].train_id))
				{
					info[i].exist_train=TGSelectedBasicTrain::BT_EXIST; 
				}
				else
				{
					info[i].exist_train=TGSelectedBasicTrain::BT_NONE; 
				}
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			{
				CTG_SingleTrain *ptrain2;
				CTG_TrainRecord *pfirrec=train.GetTrainRecord(0); 
				int station=NULL_STATION;
				if(pfirrec)
					station=pfirrec->GetStationNo();
				ptrain2=m_cWorkSchduleData.GetSingleTrainByShiftIndex(train.GetShiftTrainIndex(),station);
				if(ptrain2!=NULL) 
				{
					if(ptrain2->GetTrainRecordCount()>0)
						info[i].exist_train=TGSelectedBasicTrain::BT_EXIST;
					else
						info[i].exist_train=TGSelectedBasicTrain::BT_NONE;
				}
				else
				{
					info[i].exist_train=TGSelectedBasicTrain::BT_NONE;
				}
			}
			else
			{
				info[i].exist_train=TGSelectedBasicTrain::BT_NONE;
			}

			/*
			if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			{
			info[i].start_time = prec->m_data.arrive;
			info[i].depart_time =prec->m_data.depart;
			}
			else*/
			{
				info[i].start_time=minute_time(prec->m_data.arrive,&info[i].start_date);
				info[i].depart_time=minute_time(prec->m_data.depart,&info[i].depart_date);
			}
			info[i].add_train = TGSelectedBasicTrain::BT_NORMAL_SCHEDULE;
			info[i].hat_train = TGSelectedBasicTrain::BT_NORMAL_TRAIN;
			info[i].dir =  train.IsDownDirection()?TGSelectedBasicTrain::BT_DOWN:TGSelectedBasicTrain::BT_UP;
			info[i].type = TG_IsPassengerTrainType(train.GetTrainType())?TGSelectedBasicTrain::BT_PASSENGER:TGSelectedBasicTrain::BT_FREIGHT;
			i++;
		}while(0);

		if(bErr)
		{
			break;
		}
	}

	if(bErr)
	{
		*limit=0;
		return -1;
	}
	else
	{
		*limit=i;
		return 1;
	}
}


BOOL  CWorkScheduleHolder::GenMark_ShiftSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index)
{
		CTG_SingleMark *pshift_mark=m_cShiftSchduleData.GetSingleMark(index);
		if(!pshift_mark)
			return FALSE;
		mark=*pshift_mark;
		return TRUE;
}
BOOL  CWorkScheduleHolder::GenMark_BasicSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index)
{
		CTG_SingleMark *pshift_mark=m_cBasicSchduleData.GetSingleMark(index);
		if(!pshift_mark)
			return FALSE;
		mark=*pshift_mark;
		return TRUE;
}

void CWorkScheduleHolder::SetFileSaveFlag(WORD va)
{
	m_cWorkSchduleData.SetFileSaveFlag(va);
	m_cShiftSchduleData.SetFileSaveFlag(va);
	m_cBasicSchduleData.SetFileSaveFlag(va);
}
void CWorkScheduleHolder::SetQueryMode(BOOL va)
{
	m_bQueryMode=va;
}

BOOL  CWorkScheduleHolder::GenTrain_ChangeRoute(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,int route_no)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	return logic.GenTrain_ChangeRoute(srctrain,schd.m_nSectionID,route_no);
}

int  CWorkScheduleHolder::CheckNeedSendToNeibSectionByEntry(const CTG_SingleTrain *single,int source_section,int dest_section,int *matched_rec_count,int *nodeindex,int *passway, int* exit_entry)
{
	*exit_entry=0;
	const CTG_TrainRecord *rec_temp= NULL;
	
	int station_list[128];
	// ���Ȱ��ճ���ڷ�ʽƥ��
	int fjk_station_count=GetTGFunctionConfig()->GetNeibFJKStations(dest_section,source_section,station_list,128);
	for(int i=0;i<fjk_station_count;i++)
	{
		// ��̨�ƻ���̨�г�Ӧ�ð��մӺ���ǰ���ҳ�վ��¼,��ֹͬһ��վ�ظ�
		//rec_temp=single->GetTrainRecordByStation_Const(station_list[i]);
		rec_temp=single->GetTrainRecordByStation_Reverse_Const(station_list[i]);  
		if(rec_temp==NULL)
			continue;

		int entry_exit=0;
		if(rec_temp->IsTerminalRec())
		{
			entry_exit= -1*rec_temp->GetEntry();
			*exit_entry=1; // �ж����
		}
		else
		{
			entry_exit=rec_temp->GetExit();
			*exit_entry=2; // �жϳ���
		}

		if(GetTGFunctionConfig()->MatchNeibPlanByEntry(source_section,dest_section,station_list[i],entry_exit,nodeindex,passway,rec_temp->GetEntry()))
		{
			*matched_rec_count=single->GetTrainRecordCount()-rec_temp->GetIndex();
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, exit %d ����������̨����", single->GetTrainID(), single->GetTrainIndex(), entry_exit);
			return rec_temp->GetIndex();
		}
	}
    return -1;
}
// ���ճ�վ�б�ƥ�䷽ʽ
int  CWorkScheduleHolder::CheckNeedSendToNeibSection(const CTG_SingleTrain *single,
													 int source_section,
													 int dest_section,
													 int& matched_rec_count, 
													 bool& bHavePreStation,
													 int& nodeindex,
													 int& passway, 
													 bool& bWholeMatch,
													 bool& bAcceptTerminalTrain,
													 int &neib_station,
													 int &control_prev_abs)
{
	const CTG_TrainRecord *rec_temp= NULL;
	
	int station_list[128];
	int station_count=single->GetTrainRecordCount();
	for(int i=0;i<station_count&&i<128;i++)
	{
		rec_temp=single->GetTrainRecordNoDelete_Const(i);
		if(rec_temp==NULL)
			break;
		station_list[i]=rec_temp->GetStationNo(); 
	}

	return GetTGFunctionConfig()->MatchNeibPlan(source_section,dest_section,station_list,station_count,matched_rec_count,bHavePreStation,nodeindex,passway,bWholeMatch,bAcceptTerminalTrain,neib_station,control_prev_abs);
}

BOOL  CWorkScheduleHolder::GenTrain_NeibSchedule_ConsiderTrainID(CTG_SingleTrain &newtrain,
																 vector<CTG_SingleTrain>& othertrain_list,
																 const CTG_SingleTrain &neibtrain, 
																 int& nNeibStaNo,
																 int neib_section, 
																 int *add_or_update,
																 CTG_TrainScheduleBase* pData, 
																 CStringArray& m_warning)
{
	othertrain_list.clear();
	newtrain.m_strSource="--";
	nNeibStaNo=0;
	CTG_ScheduleInfo schdinfo;
	m_cWorkSchduleData.GetScheduleInfo(schdinfo); 
	
	int this_section=schdinfo.m_nSectionID;
	int matched_rec_count=0;
	int passway=0;
	int nodeindex=0;
	int recindex=0;
	int exit_entry=0;
	int neib_station=0;
	int control_prev_abs=0;
	int nMode=GetTGFunctionConfig()->GetNeibPlanMode(this_section, neib_section);
	// �����ģʽ����̨
	if(nMode==1)
	{
		recindex=CheckNeedSendToNeibSectionByEntry(&neibtrain,neib_section,this_section,&matched_rec_count,&nodeindex,&passway,&exit_entry);
		if(recindex<0)
		    return FALSE; 
	}
	// ǰ��վģʽ����̨
	else if(nMode==2)
	{
		bool bWholeMatch=true;
		bool bHavePreStation=true;
		bool bAcceptTerminalTrain=false;
		recindex=CheckNeedSendToNeibSection(&neibtrain,neib_section,this_section,matched_rec_count,bHavePreStation,nodeindex,passway,bWholeMatch,bAcceptTerminalTrain,neib_station,control_prev_abs);
		if(recindex<0)
			return FALSE; 

		if(matched_rec_count==1) // ��ǰվ �޺�վ ���㳵
		{
			// �ֽ�վ����̨�����߱����ǵ��㳵
			if((1!=neibtrain.GetTrainRecordCount()) && bWholeMatch)
				return FALSE;
		}
		else if(matched_rec_count==2)  
		{
			// ��ǰһվ
			if(!bHavePreStation)
			{
				// �ֽ�վ��������̨�����ߵ����ڶ�վ
				if((recindex!=neibtrain.GetTrainRecordCount()-2)&& bWholeMatch)
					return FALSE;
			}
			else// �޺�һվ
			{
				recindex++;
				const CTG_TrainRecord *ptmprec=neibtrain.GetTrainRecordNoDelete_Const(recindex);
				if(ptmprec->IsTerminalRec())
				{
					if(!bAcceptTerminalTrain)
						return FALSE;
				}
				// �ֽ�վ��������̨���������һվ
				if(recindex!=neibtrain.GetTrainRecordCount()-1)
					return FALSE;
			}
		}
		else if(matched_rec_count==3) // ǰվ �ֽ�վ ��վ ��ƥ��
		{
			if(!bHavePreStation) // �ֽ�վ ��վ ���վ
			{
				// �ֽ�վ��������̨�����ߵ�������վ
				if((recindex!=neibtrain.GetTrainRecordCount()-3) && bWholeMatch)
					return FALSE;
			}
			else // ǰվ �ֽ�վ ��վ
			{
				recindex++;
				// �ֽ�վ��������̨�����ߵ����ڶ�վ
				if((recindex!=neibtrain.GetTrainRecordCount()-2) && bWholeMatch)
					return FALSE;
			}
		}
		else if(matched_rec_count==4) // ǰվ �ֽ�վ ��վ ���վ
		{
			// �ֽ�վ��������̨�����ߵ�������վ
			recindex++;
			if((recindex!=neibtrain.GetTrainRecordCount()-3) && bWholeMatch)
				return FALSE;
		}
		else
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, matched_rec_count=%d ƥ���������", 
				neibtrain.GetTrainID(), neibtrain.GetTrainIndex(),matched_rec_count);
			return FALSE;
		}
		
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d,fjkrecindex=%d,matched_rec_count=%d ���ճ�վ�б�ƥ�� �Ƿ���ȫƥ�� %d", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(),recindex,matched_rec_count,bWholeMatch);
	}
	else if(nMode==3)
	{
		// �ȼ۳�վ�ж�
		return AdjustNeibPlanByEqualStation(pData, add_or_update, newtrain, neibtrain, othertrain_list, m_warning);
	}
	else if(nMode==4)
	{
		// ��ȫ����
		return RecvNeibPlanAll(pData, add_or_update, newtrain, neibtrain, othertrain_list, m_warning);
	}
	else
	{
		GpcLogTrainMessage(0,0,"��̨ģʽ����:this_section:%d �� neib_section:%d mode:%d (mode:1 ����� 2:ǰ��վ)", 
											this_section, neib_section, nMode);
		return FALSE;
	}

	const CTG_TrainRecord *prec=neibtrain.GetTrainRecordNoDelete_Const(recindex);
	if(prec == NULL)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u,fjkrecindex=%d, ���ǲ����ڸü�¼,ʧ��", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(),recindex);
		return FALSE;
	}

	// ̨�䳵վ
	nNeibStaNo = prec->m_data.station;
	GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, ƥ�䳵վ %d arrive=%d depart=%d", 
		neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(),
		prec->m_data.station, prec->m_data.arrive, prec->m_data.depart);
	
	// ����ʹ��shiftindex ����
	bool bNeibIsOtherBurea=false;
	CTG_SingleTrain *prev=NULL;
	if(GetTGFunctionConfig()->IsOtherBureaSection(neib_section))
	{
		GpcLogTrainMessage(0,0,"��̨ %d ���Ǳ��ֵ�������,����trainid=%s (%u) ��ȫ��ID %u ���ڱ���������,ֻ�ܰ��ճ��β���", 
			neib_section, neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex());

		bNeibIsOtherBurea=true;
		prev=NULL; // �����̨������ʹ��ȫ��ID����
	}
	else
	{
		GpcLogTrainMessage(0,0,"��̨ %d �Ǳ��ֵ�������,����trainid=%s (%u) ��ȫ��ID %u ���ڱ���������,���Ȱ���ȫ��ID����", 
			neib_section, neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex());
		
		bNeibIsOtherBurea=false;
		prev=GetTrainByNeibTrainShiftID(neibtrain, prec);
	}
	
	// �����ճ��κŲ���
	if(!prev)  
	{
		bool bFind=false;
		prev=GetTrainByNeibTrainID(neibtrain, prec, pData,m_warning,bFind);
		if(bFind)
		{
			return FALSE;
		}
	}

	if(prev)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, �ҵ���̨�г� trainid=%s,index=%u, shiftindex=%u", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(), prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());

		if(!prev->MatchTrainID(prec->GetArriveTID()) && !prev->MatchTrainID(prec->GetDepartTID()))  // �ҵ����г����복��һ��
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s ����̨���� arrive_id=%s depart_id=%s��ƥ��", 
				prev->GetTrainID(), prec->GetArriveTID(), prec->GetDepartTID());
			prev=NULL;
		}
	}
	newtrain.m_nCurrConTrainIndex = 0;
	if(prev && prev->GetTrainRecordCount()>0)
	{
		*add_or_update = 1;
		// �����Ѿ����ڵ��г���ֻҪƽ��ʱ��,
		newtrain=*prev;

		CTG_TrainRecord *pfirst_newtrain_record=newtrain.GetTrainRecord(0);

		// ����ֽ�վ�ǵ�һ��վ������ͼʹ�õ�һ��վ��Ϊƽ��
		if(pfirst_newtrain_record->GetStationNo()!=prec->GetStationNo())
		{
             const  CTG_TrainRecord *ptmp_record=neibtrain.GetTrainRecordByStation_Const(pfirst_newtrain_record->GetStationNo());
			 if(ptmp_record)
			 {
				 prec=ptmp_record;
				 GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ��һ��վ�Ƿֽ�վ,����ͼʹ�õ�һ��վ%d��Ϊƽ��", 
					 prev->GetTrainID(), ptmp_record->GetStationNo());
			 }
		}

		// ��̨�ƻ�,��̨�г����ҳ�վӦ�ô�ǰ�����,�ظ���վ����
		CTG_TrainRecord *ptrain_record= newtrain.GetTrainRecordByStation(prec->GetStationNo());
		
		short this_index =ptrain_record->GetIndex();
		//for(int x=this_index; x<newtrain.GetTrainRecordCount(); x++)
		//{
		//	CTG_TrainRecord *ptmprec=newtrain.GetTrainRecord(x);
		//	if(TG_IsArriveActual(ptmprec->m_data) || TG_IsDepartActual(ptmprec->m_data))
		//	{
		//		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ����ʵ�ʵ㲻��ƽ��", prev->GetTrainID());
		//		return FALSE;
		//	}
		//}
		time_t next_time = prec->GetDepartTime();
		time_t ltime = ptrain_record->GetDepartTime(); 
		long time_off = next_time - ltime;
		if (abs(time_off) > 8*3600)
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ʱ����8Сʱ����ƽ��", prev->GetTrainID());
			return FALSE;
		}

		if(GetTGFunctionConfig()->IsEnableExactNeibPlanMode() && neib_station!=0)
		{
			BOOL bResult = NeibScheduleUpdate_Shanghai(newtrain, neibtrain, neib_station, control_prev_abs);
			if(bResult)
			{
				// ��̨���ݻ�����
				if(GetTGFunctionConfig()->IsGetEngineFromPreConID())
				{
					PassTrainEngineNo(newtrain, &neibtrain);
				}
				//////////////////////////////////////
				newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
				newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
			}
			else
			{
				CString str;
				str.Format("��̨�г�:%s�ͱ�̨�г�:%s ̨�����о�·��һ��,�޷�����,�����Աȷ�ϴ���", 
					neibtrain.GetTrainID(), newtrain.GetTrainID());
				GpcLogTrainMessage(0,0,"%s", str);
				m_warning.Add(str);
			}

			return bResult;
		}

		// give fjk station not this section, recieve neib side and time
		if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  ptrain_record->m_data.station))
		{
			GpcLogTrainMessage(0,0,"�г�trainid=%s,����̨�䳵վ%d�����ڱ�̨%d��Ͻ, ������̨�ɵ���������ϵ����ҵʱ��",  
					newtrain.GetTrainID(), ptrain_record->m_data.station, schdinfo.m_nSectionID);

			ptrain_record->m_data.arrive_side = prec->m_data.arrive_side;
			ptrain_record->m_data.depart_side = prec->m_data.depart_side;
			ptrain_record->m_data.min_stop_time = prec->m_data.min_stop_time;

			time_off = prec->GetDepartTime() - ptrain_record->GetDepartTime();

			ptrain_record->m_data.arrive = prec->GetArriveTime();
			ptrain_record->m_data.depart = prec->GetDepartTime(); 
			newtrain.OffsetScheduleTime(this_index+1, time_off, FALSE, 1);
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ��վ %d �����ƽ��ʱ��%d", 
					prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
		}
		else
		{
			if(newtrain.IsPassengerTrain() || GetTGFunctionConfig()->IsEnableNeibPlanFreightSameToPassenger())
			{
				time_off = prec->GetArriveTime() - ptrain_record->GetArriveTime();
				newtrain.OffsetScheduleTime(this_index, time_off, FALSE, 1);
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s �ǿͳ�, ������̨����,��վ %d �����ƽ��ʱ��%d", 
							prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
			}
			else
			{
				short stop_pass_change = 0;
				if (time_off<0 && ptrain_record->IsPassRec()) 
					stop_pass_change = -1;  // pass to stop			

				if (stop_pass_change)
				{
					ptrain_record->m_data.arrive_side = 255;
					ptrain_record->m_data.depart_side = 255;
					CMMIOperFlag oper_flag;
					oper_flag.m_nRecIndex=this_index;
					if(stop_pass_change==1)
						oper_flag.nFlag=CMMIOperFlag::STAY_TO_PASS;
					else
						oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
					GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
				}
				time_off = prec->GetDepartTime() - ptrain_record->GetDepartTime();

				if(time_off<0)
				{
					if(this_index==0 && ptrain_record->IsStartRec())
						ptrain_record->m_data.depart = prec->GetDepartTime();
					ptrain_record->m_data.arrive = prec->GetArriveTime();
				}
				else
				{
					newtrain.OffsetScheduleTime(this_index, time_off, FALSE, 1);
					GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ����վ %d ����ƽ��ʱ��%d", 
							prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
				}
			}
		}
		
		// ��̨���ݻ�����
		if(GetTGFunctionConfig()->IsGetEngineFromPreConID())
		{
			PassTrainEngineNo(newtrain, &neibtrain);
		}
		//////////////////////////////////
		newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
	}
	//����֧����ϲ��� ������ 2011927
	else if(GetTGFunctionConfig()->IsNeibPlanPassengerTrainAddNotByBasic())
	{
		// ���ڿͳ�, �հ�ƻ��ӳ�->����ӳ�
		// ���ڻ���, �հ�ƻ��ӳ�->����ͼ�ӳ�->��·�ӳ�->����ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 
		
		// �����հ�ƻ��в���
		bool bFind=false;
		int offset=0;
		DWORD train_shift_index=neibtrain.GetShiftTrainIndex();
		if(train_shift_index>0)
		{
			CTG_SingleTrain* pshifttrain=m_cShiftSchduleData.GetSingleTrainByShiftIndex(neibtrain.GetShiftTrainIndex(), prec->GetStationNo());
			if(NULL != pshifttrain)
			{
				newtrain=*pshifttrain;
				newtrain.SetTrainRecordCountToDefault();
				GpcLogTrainMessage(0,0,"���� shift_index %d �ڱ�̨�հ�ƻ����ҵ� trainid=%s", 
														train_shift_index, neibtrain.GetTrainID());

				// ������̨�ֽ�վ��Ϊ��̨��վ���
				CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
				if(pfirst_rec)
				{
					if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  pfirst_rec->m_data.station))
					{
						offset=prec->m_data.depart-pfirst_rec->m_data.depart;
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						pfirst_rec->m_data.min_stop_time = prec->m_data.min_stop_time;
						pfirst_rec->m_data.arrive_side = prec->m_data.arrive_side;
						pfirst_rec->m_data.depart_side = prec->m_data.depart_side; 
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);

						GpcLogTrainMessage(0,0,"�г�trainid=%s,����̨�䳵վ%d�����ڱ�̨%d��Ͻ, ������̨�ɵ���������ϵ����ҵʱ��",  
								newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
					}
					else
					{
						GpcLogTrainMessage(0,0,"�г�trainid=%s, ̨�䳵վ%d �Ƿ�Ϊ��̨%d��Ͻδ֪",  
								newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
						if(newtrain.IsPassengerTrain()) 
						{
							offset=prec->m_data.arrive-pfirst_rec->m_data.arrive;
							pfirst_rec->m_data.arrive= prec->m_data.arrive;
							
							newtrain.OffsetScheduleTime(pfirst_rec->GetIndex(),offset,FALSE,0);

							GpcLogTrainMessage(0,0,"��̨�����հ�ӳ�trainid=%s, shiftindex %u, %dվά�ֱ�̨�ɵ��͵�����ϵ,ƽ��ʱ��%d", 
								newtrain.GetTrainID(), newtrain.GetShiftTrainIndex(), pfirst_rec->GetStationNo(), offset);
						}
						else
						{
							offset=prec->m_data.depart-pfirst_rec->m_data.depart;

							pfirst_rec->m_data.depart= prec->m_data.depart;
							pfirst_rec->m_data.arrive= prec->m_data.arrive;
							newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						}
					}
					newtrain.m_strSource="��ƻ�";
					bFind=true;
				}
			}
			else
			{
				GpcLogTrainMessage(0,0,"���� shift_index %d �ڱ�̨�հ�ƻ����Ҳ��� trainid=%s", 
														train_shift_index, neibtrain.GetTrainID());
			}
		}
	
		if(bFind)
		{
			GpcLogTrainMessage(0,0,"���� shift_index %d, �ڱ�̨�հ�ƻ����ҵ� trainid=%s, �����հ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
						train_shift_index, neibtrain.GetTrainID(), prec->GetStationNo(), offset);
		}
		else if(neibtrain.IsPassengerTrain())
		{
			newtrain.m_strSource="��ʱ����";
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetRecordList()->PurgeRecords(); 
			
			const CTG_TrainRecord *prec_neib=neibtrain.GetTrainRecordNoDelete_Const(recindex);
			if(!prec_neib)
				return FALSE; // ������
			CTG_TrainRecord train_record=*prec_neib;
			train_record.rec_index=0; 
			TG_SetFlagAccept(train_record.m_data.flag);
			TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
			train_record.SetArriveTGIndex(nodeindex);
			train_record.SetDepartTGIndex(nodeindex);
			train_record.SetPlanSendTime(0); //��Ҫ! 
			newtrain.GetRecordList()->AddRecord(train_record);

			GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
				neibtrain.GetTrainID(), nodeindex);
			
			newtrain.SetTrainRecordCountToDefault(); 
		}
		else
		{
			if(GenTrain_BasicSchedule(newtrain,schdinfo,prec->GetDepartTID(),prec->GetStationNo(),NULL_TIME,NULL_TIME))
			{
				newtrain.m_strSource="����ͼ";

				TRAIN_INDEX basic = newtrain.GetBasicTrainIndex();
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.GetTrainInfo()->m_data.basic_train_index = basic;
				newtrain.SetTrainRecordCountToDefault();

				//�޸����� CUIHU  2009.11.20 ������̨�ֽ�վ��Ϊ��̨��վ���
				CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
				if(pfirst_rec)
				{
					int offset=prec->m_data.depart-pfirst_rec->m_data.depart;
					if(GetTGFunctionConfig()->IsTaiYuanNeigbourPlan())
					{
						newtrain.OffsetScheduleTime(0,offset,FALSE,1);//̫ԭ20101018�����̨�����������
						GpcLogTrainMessage(0,0,"̫ԭģʽ,��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ����ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), offset);
					}
					else
					{
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						
						GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), offset);
					}
				}
			}
			else
			{
				// �������ջ���ͼ�ӳ�ʧ���� ���վ�·�ӳ�
				if(!neibtrain.IsPassengerTrain() && passway>0 && GenTrain_Passway(newtrain,schdinfo,prec->GetDepartTID(),passway,0,0,prec->GetStationNo(),prec->GetDepartTime(),255))
				{
					newtrain.m_strSource="Ĭ�Ͼ�·";
					*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
					RemoveTrainAbsTimeType(&newtrain);
					newtrain.SetTrainRecordCountToDefault();

					GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���վ�· %d �ӳ�", 
							neibtrain.GetTrainID(), passway);
				}
				// ���㳵
				else
				{
					newtrain.m_strSource="��ʱ����";
					*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
					RemoveTrainAbsTimeType(&newtrain);
					newtrain.GetRecordList()->PurgeRecords(); 
					
					const CTG_TrainRecord *prec_neib=neibtrain.GetTrainRecordNoDelete_Const(recindex);
					if(!prec_neib)
						return FALSE; // ������
					CTG_TrainRecord train_record=*prec_neib;
					train_record.rec_index=0; 
					TG_SetFlagAccept(train_record.m_data.flag);
					TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
					train_record.SetArriveTGIndex(nodeindex);
					train_record.SetDepartTGIndex(nodeindex);
					train_record.SetPlanSendTime(0); //��Ҫ! 
					newtrain.GetRecordList()->AddRecord(train_record);

					GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
						neibtrain.GetTrainID(), nodeindex);
					
					newtrain.SetTrainRecordCountToDefault(); 
				}
			}
		}
		
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
	}
	//����֧��Լ��Ͼ� ������ 2017823
	else if(GetTGFunctionConfig()->IsNeibPlanAddTrainOnlyByShift())
	{
		// ���ۿͻ���, �����հ�ƻ��ӳ�->����ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 
		
		// ֻ�����հ�ƻ��в���
		CTG_SingleTrain* pshifttrain=NULL;
		if(bNeibIsOtherBurea) // ��̨Ϊ���ֻ�ܰ��ճ��β���
		{
			pshifttrain=m_cShiftSchduleData.GetSingleTrain(prec->GetDepartTID());
		}
		else // ��̨Ϊ�������Ȱ���ȫ��ID����,ʧ�ܺ��ճ��β���
		{
			// ���Ȱ���shift_index����
			DWORD train_shift_index=neibtrain.GetShiftTrainIndex();
			if(train_shift_index!=NULL_TRAIN_INDEX) 
			{
				pshifttrain=m_cShiftSchduleData.GetSingleTrainByShiftIndex(train_shift_index, prec->GetStationNo());
				if(NULL == pshifttrain)
				{
					GpcLogTrainMessage(0,0,"���� shift_index %d �ڱ�̨�հ�ƻ����Ҳ��� trainid=%s", 
															train_shift_index, prec->GetDepartTID());

					pshifttrain=m_cShiftSchduleData.GetSingleTrain(prec->GetDepartTID());
				}
			}
			else
			{
				pshifttrain=m_cShiftSchduleData.GetSingleTrain(prec->GetDepartTID());
			}
		}

		bool bFind=false;
		if(NULL != pshifttrain)
		{
			int offset=0;
			
			newtrain=*pshifttrain;
			newtrain.SetTrainRecordCountToDefault();
			GpcLogTrainMessage(0,0,"���� shift_index %u �ڱ�̨�հ�ƻ����ҵ� trainid=%s shift_index %u", 
													neibtrain.GetShiftTrainIndex(), newtrain.GetTrainID(), newtrain.GetShiftTrainIndex());

			// ������̨�ֽ�վ��Ϊ��̨��վ���
			CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
			if(pfirst_rec)
			{
				if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  pfirst_rec->m_data.station))
				{
					offset=prec->m_data.depart-pfirst_rec->m_data.depart;
					pfirst_rec->m_data.depart= prec->m_data.depart;
					pfirst_rec->m_data.arrive= prec->m_data.arrive;
					pfirst_rec->m_data.min_stop_time = prec->m_data.min_stop_time;
					pfirst_rec->m_data.arrive_side = prec->m_data.arrive_side;
					pfirst_rec->m_data.depart_side = prec->m_data.depart_side; 
					newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);

					GpcLogTrainMessage(0,0,"�г�trainid=%s,����̨�䳵վ%d�����ڱ�̨%d��Ͻ, ������̨�ɵ���������ϵ����ҵʱ��",  
							newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
				}
				else
				{
					GpcLogTrainMessage(0,0,"�г�trainid=%s, ̨�䳵վ%d �Ƿ�Ϊ��̨%d��Ͻδ֪",  
							newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
					if(newtrain.IsPassengerTrain()) 
					{
						offset=prec->m_data.arrive-pfirst_rec->m_data.arrive;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex(),offset,FALSE,0);

						GpcLogTrainMessage(0,0,"��̨�����հ�ӳ�trainid=%s, shiftindex %u, %dվά�ֱ�̨�ɵ��͵�����ϵ,ƽ��ʱ��%d", 
							newtrain.GetTrainID(), newtrain.GetShiftTrainIndex(), pfirst_rec->GetStationNo(), offset);
					}
					else
					{
						offset=prec->m_data.depart-pfirst_rec->m_data.depart;

						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
					}
				}
				newtrain.m_strSource="��ƻ�"; //��ƻ�
				bFind=true;
				GpcLogTrainMessage(0,0,"trainid=%s, �����հ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
					newtrain.GetTrainID(), prec->GetStationNo(), offset);
			}
		}
		// ���㳵
		if(!bFind)
		{
			GpcLogTrainMessage(0,0,"���� ��̨���� %s �ڱ�̨�հ�ƻ����Ҳ��� trainid=%s, �������㳵", 
														prec->GetDepartTID(), neibtrain.GetTrainID());

			newtrain.m_strSource="��ʱ����";
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetRecordList()->PurgeRecords();

			if(bNeibIsOtherBurea)// �Ǳ�����̨���,��̨�ӹ�����ȫ��ID
			{
				newtrain.SetShiftTrainIndex(0);
			}
			newtrain.SetFromShift(FALSE); // ȥ���հ�ƻ���Դ
			
			const CTG_TrainRecord *prec_neib=neibtrain.GetTrainRecordNoDelete_Const(recindex);
			if(!prec_neib)
				return FALSE; // ������
			CTG_TrainRecord train_record=*prec_neib;
			train_record.rec_index=0; 
			TG_SetFlagAccept(train_record.m_data.flag);
			TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
			train_record.SetArriveTGIndex(nodeindex);
			train_record.SetDepartTGIndex(nodeindex);
			train_record.SetPlanSendTime(0); //��Ҫ! 
			newtrain.GetRecordList()->AddRecord(train_record);

			GpcLogTrainMessage(0,0,"��̨�հ�ƻ����Ҳ����г� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
				neibtrain.GetTrainID(), nodeindex);
			
			newtrain.SetTrainRecordCountToDefault(); 
		}
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
	}
	else
	{
		// �����������Σ�1.���� traininfo, ��deleteflag��SetTrainRecordCountToDefault��������
		// 2.����ǣ������
		// ���ڿͳ�������ͼ�ӳ�->����ӳ�
		// ���ڻ���, ��·�ӳ�->����ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 

		if(GetTGFunctionConfig()->IsEnableExactNeibPlanMode() && neib_station!=0)
		{
			BOOL bResult = NeibScheduleAppend_Shanghai(newtrain, neibtrain, nNeibStaNo, neib_station, neib_section, m_warning, nodeindex, control_prev_abs);
			if(bResult)
			{
				// ��̨���ݻ�����
				PassTrainEngineNo(newtrain, &neibtrain);
			}
			else
			{
				GpcLogTrainMessage(0,0,"������̨����=%s, ��������ʧ��", neibtrain.GetTrainID());
			}
			
			return bResult;
		}
		// �ͳ�--ֻ�ܰ��ջ���ͼ�ӳ�
		BOOL IsCanGenNei = TRUE;//YUZHU BEGIN
		if(GetTGFunctionConfig()->NeiConsiderPassenger())
		{
			if(neibtrain.IsPassengerTrain())
				IsCanGenNei = TRUE;
			else
				IsCanGenNei=FALSE;
		}//YUZHU END

		GpcLogTrainMessage(0,0,"��̨�ƻ� trainid=%s, �Ƿ��ջ���ͼ�ӳ���־Ϊ%d(1:���ջ���ͼ�ӳ� 0:�����ջ���ͼ�ӳ�)", 
						neibtrain.GetTrainID(), IsCanGenNei);

		if(IsCanGenNei && GenTrain_BasicSchedule(newtrain,schdinfo,prec->GetDepartTID(),prec->GetStationNo(),NULL_TIME,NULL_TIME))
		{
			newtrain.m_strSource="����ͼ";
			GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�",  neibtrain.GetTrainID());

			TRAIN_INDEX basic = newtrain.GetBasicTrainIndex();
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetTrainInfo()->m_data.basic_train_index = basic;
			newtrain.SetTrainRecordCountToDefault();

			//�޸����� CUIHU  2009.11.20 ������̨�ֽ�վ��Ϊ��̨��վ���
			CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
			if(pfirst_rec)
			{
				int offset=prec->m_data.depart-pfirst_rec->m_data.depart;
				if(GetTGFunctionConfig()->IsTaiYuanNeigbourPlan())
				{
					newtrain.OffsetScheduleTime(0,offset,FALSE,1);//̫ԭ20101018�����̨�����������
					GpcLogTrainMessage(0,0,"̫ԭģʽ,��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ����ƽ��ʱ��%d", 
						neibtrain.GetTrainID(), offset);
				}
				else
				{
					if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  pfirst_rec->m_data.station))
					{
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						pfirst_rec->m_data.arrive_side= prec->m_data.arrive_side;
						pfirst_rec->m_data.depart_side= prec->m_data.depart_side;

						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						GpcLogTrainMessage(0,0,"��̨���ջ���ͼ�ӳ�trainid=%s, ��վ%d�����ڵ�������%d��Ͻ, ������̨������͹ɵ�,������������ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), schdinfo.m_nSectionID, offset);
					}
					else
					{
						offset=prec->m_data.arrive-pfirst_rec->m_data.arrive;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex(),offset,FALSE,0);

						GpcLogTrainMessage(0,0,"��̨���ջ���ͼ�ӳ�trainid=%s, ά�ֳ�վ%d�ɵ��͵�����ϵ,ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), offset);
					}
				}
			}
		}
		else
		{
			// �������ջ���ͼ�ӳ�ʧ���� ���վ�·�ӳ�
			if(!neibtrain.IsPassengerTrain() && passway>0 && GenTrain_Passway(newtrain,schdinfo,prec->GetDepartTID(),passway,0,0,prec->GetStationNo(),prec->GetDepartTime(),255))
			{
				newtrain.m_strSource="Ĭ�Ͼ�·";
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.SetTrainRecordCountToDefault();

				GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���վ�· %d �ӳ�", 
						neibtrain.GetTrainID(), passway);
			}
			// ���㳵
			else
			{
				newtrain.m_strSource="��ʱ����";
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.GetRecordList()->PurgeRecords(); 
				
				const CTG_TrainRecord *prec_neib=neibtrain.GetTrainRecordNoDelete_Const(recindex);
				if(!prec_neib)
					return FALSE; // ������
				CTG_TrainRecord train_record=*prec_neib;
				train_record.rec_index=0; 
				TG_SetFlagAccept(train_record.m_data.flag);
				TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
				train_record.SetArriveTGIndex(nodeindex);
				train_record.SetDepartTGIndex(nodeindex);
				train_record.SetPlanSendTime(0); //��Ҫ! 
				newtrain.GetRecordList()->AddRecord(train_record);

				GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
					neibtrain.GetTrainID(), nodeindex);
				
				newtrain.SetTrainRecordCountToDefault(); 
			}
		}
		
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
	}
	return TRUE;
}

BOOL CWorkScheduleHolder::NeibScheduleUpdate_Shanghai(CTG_SingleTrain &newtrain, const CTG_SingleTrain &neibtrain, const int neib_station, const int control_prev_abs)
{
	if(neib_station==0)
		return FALSE;
	int offset=0;
	// Ѱ���ظ�����
	CTG_TrainRecord *pthis_rec=NULL;
	const CTG_TrainRecord *pneib_rec=NULL;
	int neibtrain_count=neibtrain.GetTrainRecordCount();
	int newtrain_count=newtrain.GetTrainRecordCount();
	int nSameCount=0;
    // �����ظ���վ���10��
	for(int i=1; i<=10; i++)
	{
		if((i>newtrain_count) || (i>neibtrain_count))
			break;
		for(int n=0; n<i; n++)
		{
			pthis_rec=newtrain.GetTrainRecord(n);
			pneib_rec=neibtrain.GetTrainRecordByRecIndex_Const(neibtrain_count-i+n);
			if (pthis_rec==NULL || pneib_rec==NULL)
				break;
				
			if(pneib_rec->m_data.station!=pthis_rec->m_data.station)
			{
				break;
			}
		}
		if(n>=i)
		{
			nSameCount=i;
		}
	}
	if(nSameCount==0)
	{
		return FALSE;
	}

	// ����Ƿ���̨��վ
	int real_neib_station=abs(neib_station);
	int nSameNeibIndex=neibtrain_count-nSameCount, m=0;
	bool bFindNeibStation=false;
	for(m=0; m<nSameCount; m++)
	{
		pthis_rec=newtrain.GetTrainRecord(m);
		pneib_rec=neibtrain.GetTrainRecordByRecIndex_Const(nSameNeibIndex+m);
		if(pneib_rec==NULL || pthis_rec==NULL)
		{
			break;
		}
		if(pneib_rec->m_data.station!=pthis_rec->m_data.station)
		{
			break;
		}

		if(pthis_rec->m_data.station==real_neib_station)
		{
			bFindNeibStation=true;
		}
	}
	if(m<nSameCount)
	{
		return FALSE;
	}

	if(!bFindNeibStation)
	{
		GpcLogTrainMessage(0,0,"�ظ���վ����:%d,����û��̨�䳵վ:%d,ʧ��", 
					nSameCount,real_neib_station);
		return FALSE;
	}

	GpcLogTrainMessage(0,0,"��̨�г�:%s(%u)����̨�г�:%s(%u)�ظ���վ����:%d,��̨�г���ʼ����:%d", 
		newtrain.GetTrainID(),newtrain.GetTrainIndex(), 
		neibtrain.GetTrainID(),neibtrain.GetTrainIndex(), nSameCount, nSameNeibIndex);

	CString str=gpcdef.GetStationName(real_neib_station);
	BOOL bResult=FALSE;
	for(int m=0; m<nSameCount; m++)
	{
		pthis_rec=newtrain.GetTrainRecord(m);
		pneib_rec=neibtrain.GetTrainRecordByRecIndex_Const(nSameNeibIndex+m);
		if(pneib_rec==NULL || pthis_rec==NULL)
		{
			break;
		}
		if(pneib_rec->m_data.station!=pthis_rec->m_data.station)
		{
			break;
		}

		if(neib_station>0) // ̨��վ��̨��Ͻ
		{
			if(pthis_rec->m_data.station==real_neib_station)						
			{
				// ��̨����ʱ��������Сͣվʱ��
				if (pthis_rec->m_data.depart != pthis_rec->m_data.arrive) 
				{
					int stop_time = GetTGFunctionConfig()->GetNeibScheduleMinStopTime_Shanghai(neibtrain.IsCRHTrain());
					// ��ֹ��������,�����г��ɵ�����Ϊͨ��
					if(stop_time==0)
						stop_time=60;
					if (pneib_rec->m_data.arrive + stop_time > pthis_rec->m_data.depart)
					{
						pthis_rec->m_data.arrive = pneib_rec->m_data.arrive;
						int depart = pthis_rec->m_data.arrive + stop_time;
						offset=depart-pthis_rec->m_data.depart;
						newtrain.OffsetScheduleTime(m, offset, FALSE, 0);

						GpcLogTrainMessage(0,0,"̨��վ:%s(%d)�鱾̨��Ͻ,��̨�г����ֵ���,ƽ��:%s �����㼰��������", 
							str, real_neib_station,str);
					}
					else
					{
						pthis_rec->m_data.arrive = pneib_rec->m_data.arrive;	
						GpcLogTrainMessage(0,0,"̨��վ:%s(%d)�鱾̨��Ͻ,��̨�г����ֵ���,��̨:%s��������ڱ�̨������,�����ճ�����,�������㼰��������", 
							str, real_neib_station,str);
					}
				}
				else
				{
					offset=pneib_rec->m_data.arrive-pthis_rec->m_data.arrive;
					newtrain.OffsetScheduleTime(m, offset, FALSE, 1);

					GpcLogTrainMessage(0,0,"̨��վ:%s(%d)�鱾̨����,��̨�г�����ͨ��,������̨:%s�ĵ����ƽ�Ʊ�̨������", 
							str, real_neib_station,str);
				}

				bResult=TRUE;
				break;
			}
			else 
			{
				pthis_rec->m_data.arrive=pneib_rec->m_data.arrive;

				if (control_prev_abs>0 && (m+1)<nSameCount)
				{
					CTG_TrainRecord *pthis_next_rec=newtrain.GetTrainRecord(m+1);
					const CTG_TrainRecord *pneib_next_rec=neibtrain.GetTrainRecordByRecIndex_Const(nSameNeibIndex+m+1);
					if(pthis_next_rec==NULL || pneib_next_rec==NULL)
					{
						break;
					}
					if(pthis_next_rec->m_data.station!=pneib_next_rec->m_data.station)
					{
						break;
					}
					if (pthis_next_rec->m_data.station==real_neib_station)
					{
						offset=pneib_rec->m_data.depart-pthis_rec->m_data.depart;
						newtrain.OffsetScheduleTime(m, offset, FALSE, 0);

						GpcLogTrainMessage(0,0,"̨��վ:%s(%d)����һ����鱾̨����,��̨�����߰���:%s(%d)�ķ������ƽ��", 
							str, real_neib_station, gpcdef.GetStationName(pthis_rec->m_data.station), pthis_rec->m_data.station);

						bResult=TRUE;
						break;
					}
					else
					{
						pthis_rec->m_data.depart=pneib_rec->m_data.depart;
					}
				}
				else
				{
					pthis_rec->m_data.depart=pneib_rec->m_data.depart;
				}
			}
		}
		else if(neib_station<0) // ̨��վ��̨��Ͻ
		{
			if(pthis_rec->m_data.station==real_neib_station)						
			{
				offset=pneib_rec->m_data.depart-pthis_rec->m_data.depart;

				pthis_rec->m_data.arrive=pneib_rec->m_data.arrive;
				newtrain.OffsetScheduleTime(m, offset, FALSE, 0);

				GpcLogTrainMessage(0,0,"̨��վ����̨����,��̨�г� %s(%d) վ���������������̨��վ���������ƽ��", 
					str, real_neib_station);

				bResult=TRUE;
				break;
			}
			else
			{
				pthis_rec->m_data.arrive=pneib_rec->m_data.arrive;
				pthis_rec->m_data.depart=pneib_rec->m_data.depart;
			}
		}

		if(bResult)
		{
			break;
		}
	}

	return bResult;
}

BOOL CWorkScheduleHolder::NeibScheduleAppend_Shanghai(CTG_SingleTrain &newtrain, const CTG_SingleTrain &neibtrain, int first_station,int neib_station, int neib_section, CStringArray& m_warning, int nodeindex, int control_prev_abs)
{
	DWORD train_shift_index=neibtrain.GetShiftTrainIndex();
	CTG_SingleTrain *prev=NULL;
	bool need_add_nwyl=false;
	if(GetTGFunctionConfig()->IsOtherBureaSection(neib_section))
	{
		// ��̨����֣�ֻ��ʹ�ó��κ����հ�ƻ��в��������г�
		bool bFindMore=false;
		prev=GetTrainByTrainNoFromShiftSchedule(neibtrain, first_station, m_warning, bFindMore);
		if(bFindMore)
		{
			CString str;
			str.Format("�ּ���̨���� %s ���հ�ƻ����ҵ��������,�����г�ʧ��",neibtrain.GetTrainID());
			m_warning.Add(str);
			GpcLogTrainMessage(0,0,"%s", str);
			return FALSE; // �ҵ�������޷�����
		}

		if (prev==NULL)
		{
			// ֱ�ӱ���
			CString str;
			str.Format("�ּ���̨���� %s ���հ�ƻ����޷��ҵ�,�����г�ʧ��",neibtrain.GetTrainID());
			m_warning.Add(str);
			GpcLogTrainMessage(0,0,"%s", str);
			return FALSE;	
		}
	}
	else // ����
	{
		if(train_shift_index!=NULL_TRAIN_INDEX)
		{
			prev=GetTrainByShiftIDFromShiftSchedule(train_shift_index, first_station);
			if(prev)
			{
				const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByStation_Reverse_Const(first_station);
				if(prec!=NULL)
				{
					if(!prev->MatchTrainID(prec->GetArriveTID()) && !prev->MatchTrainID(prec->GetDepartTID()))
					{
						// �����г�train_shift_indexһ��,���ǳ��β�һ��
						CString str;
						str.Format("��̨�г�%s�ͱ�̨�հ��г�%sȫ��IDһ��,���ǳ��β�ͬ,�޷������г�%s", 
							neibtrain.GetTrainID(), prev->GetTrainID(),neibtrain.GetTrainID());
						GpcLogTrainMessage(0,0,"%s", str);
						prev=NULL;
					}
				}
			}
			
			if(prev==NULL)
			{
				bool bFindMore=false;
				prev=GetTrainByTrainNoFromShiftSchedule(neibtrain, first_station, m_warning, bFindMore);
				if(bFindMore)
				{
					CString str;
					str.Format("������̨���� %s ���հ�ƻ����ҵ��������,�����г�ʧ��",neibtrain.GetTrainID());
					m_warning.Add(str);
					GpcLogTrainMessage(0,0,"%s", str);
					return FALSE;
				}
			}

			//���ڻ���ʹ�õ����ƻ�
			if(prev==NULL)
			{
				if(neibtrain.IsPassengerTrain())
				{
					CString text;
					text.Format("������̨�ͳ� %s(%u) �ڱ�̨�հ�ƻ�����ƥ�䳵��,�����г�ʧ��", neibtrain.GetTrainID(),train_shift_index);
					m_warning.Add(text);
					GpcLogTrainMessage(0,0,"%s", text);
					return FALSE;
				}
				else
				{
					newtrain.m_strSource="��������";
					*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
					RemoveTrainAbsTimeType(&newtrain);
					newtrain.GetRecordList()->PurgeRecords(); 
			
					const CTG_TrainRecord *prec_first=neibtrain.GetTrainRecordByStation_Reverse_Const(first_station);
					if(!prec_first)
					{
						CString text;
						text.Format("��̨�г� %s ��¼�쳣,û�г�վ%d,�����г�ʧ��", neibtrain.GetTrainID(),first_station);
						m_warning.Add(text);
						GpcLogTrainMessage(0,0,"%s", text);

						return FALSE; // ������
					}

					newtrain.GetRecordList()->PurgeRecords(); 
					CTG_TrainRecord train_record=*prec_first;
					train_record.rec_index=0; 
					TG_SetFlagAccept(train_record.m_data.flag);
					TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
					train_record.SetArriveTGIndex(nodeindex);
					train_record.SetDepartTGIndex(nodeindex);
					train_record.SetPlanSendTime(0); //��Ҫ! 
					newtrain.GetRecordList()->AddRecord(train_record);

					GpcLogTrainMessage(0,0,"������̨�г� %s,�ڱ�̨��ʱ���ӵ����յ���, ��վ %d ���� %d", 
									neibtrain.GetTrainID(), first_station, nodeindex);
					
					newtrain.SetTrainRecordCountToDefault(); 
				
					return TRUE;
				}
			}
		}
		else
		{
			if(neibtrain.IsPassengerTrain()) //ok
			{
				//������train_shift_index�ͳ�,Ҫ���ճ�����
				bool bFindMore=false;
				prev=GetTrainByTrainNoFromShiftSchedule(neibtrain, first_station, m_warning, bFindMore);
				if(bFindMore)
				{
					CString text;
					text.Format("������̨�ͳ� %s ��ȫ��ID,���հ�ƻ����ҵ��������,�����г�ʧ��", neibtrain.GetTrainID());
					m_warning.Add(text);
					GpcLogTrainMessage(0,0,"%s", text);
					return FALSE;
				}

				if(NULL==prev)
				{
					CString text;
					text.Format("������̨�ͳ� %s ��ȫ��ID,���հ�ƻ�����ƥ�䳵��,�����г�ʧ��",neibtrain.GetTrainID());
					m_warning.Add(text);
					GpcLogTrainMessage(0,0,"%s", text);
					return FALSE;
				}
			}
			else
			{
				// ������train_shift_index����,ֱ�ӱ���
				CString str;
				str.Format("������̨���� %s ��ȫ��ID,�޷���ȡ���¼ƻ���Ϣ,�����г�ʧ��", neibtrain.GetTrainID());
				m_warning.Add(str);
				GpcLogTrainMessage(0,0,"%s", str);
				return FALSE;
			}
		}
	}

	if(NULL==prev)
	{
		CString text;
		text.Format("��̨���� %s �ڱ�̨�հ�ƻ�����ƥ�䳵��,�����г�ʧ��",	neibtrain.GetTrainID());
		m_warning.Add(text);
		GpcLogTrainMessage(0,0,"%s", text);
		return FALSE;
	}
	else
	{
		newtrain=*prev;
		newtrain.SetTrainRecordCountToDefault();
		GpcLogTrainMessage(0,0,"�ڱ�̨�հ�ƻ����ҵ� trainid=%s (%u)", 
						newtrain.GetTrainID(), newtrain.GetTrainIndex());

		BOOL bSuccess=FALSE;
		bSuccess = NeibScheduleUpdate_Shanghai(newtrain, neibtrain, neib_station, control_prev_abs);
		if(bSuccess)
		{
			GpcLogTrainMessage(0,0,"��̨���ձ�̨�հ�ƻ�Ĭ���г� trainid=%s shift_index=%u �ɹ�,̨�䳵վ %d", 
					newtrain.GetTrainID(), train_shift_index, neib_station);

			if (newtrain.IsPassengerTrain())
				newtrain.m_strSource="�հ�ƻ�";
			else
				newtrain.m_strSource="��ʱ�հ�";
			return TRUE;
		}
		else 
		{
			GpcLogTrainMessage(0,0,"trainid=%s ������̨���³���ʧ��", neibtrain.GetTrainID());
		
			CString text;
			text.Format("��̨���� %s(%u) ���հ�ƻ��г��� %s(%u)��������̨����,�����г�ʧ��", 
					neibtrain.GetTrainID(),train_shift_index,newtrain.GetTrainID(),newtrain.GetTrainIndex());
			m_warning.Add(text);
			GpcLogTrainMessage(0,0,"%s", text);
			return FALSE;
		}
	}
}

int  CWorkScheduleHolder::CheckSidePlatform(const CTG_SingleTrain& train, CString& text)
{
	text="";
	    
	CTime tm = CTime::GetCurrentTime();
	for(int i=0; i<train.GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord* rec = train.GetTrainRecordNoDelete_Const(i);
		int station = rec->GetStationNo();
		if(!IsCheckStationSide(station))
			continue;

		if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
			continue;
		
		if(rec->GetArriveTime() > 3*3600+tm.GetTime())
			continue;

		if(!TG_IsTechStopTrain(rec->m_data) && (!rec->IsPassRec()))
		{
			const CStationDef* pstationside=GetTGBasicParam()->GetStationDef(station);
			if(pstationside)
			{
				if(STA_FAILED == pstationside->isPlatform(rec->m_data.arrive_side))
				{
					SIDE_NAME side_name;
					pstationside->GetSideName(rec->m_data.arrive_side, side_name);

					CString strText;
					strText.Format("��ҵ��վ %s %s û��վ̨\n", gpcdef.GetStationName(station), side_name);
					text += strText;
				}
			}
		}
	}

	if(text.IsEmpty())
		return 0;

	text = "��ע��," + text;
		
	return 1;
}

// ��ȡ��վ����ģʽ,�·��׶μƻ����ʹ��
bool CWorkScheduleHolder::IsNeedCheckCtcModePlan(int station)
{
	bool bret = false;
	TG_STATION_STATUS * pst = GetStationStatusPtr(station);
	if(NULL != pst)
	{
		switch(pst->ctrl_mode)
		{
		case SMODE_DISTRIBUTED: // ��ɢ����
		case SMODE_CENTER_CTRL: // ������ȫ����
			bret = true;
			break;
		case SMODE_STATION_CTRL: // ��վ��ȫ����
			bret = GetTGFunctionConfig()->IsCheckCtcModeStaCtrlStPlan();
			break;
		case SMODE_EMERGENCY: // �ǳ�վ��		
			break;
		default:
			;
		}
	}

	return bret;
}

int  CWorkScheduleHolder::IsCheckStationSide(int station)
{
	if(GetTGFunctionConfig()->IsCheckAllStationStPlan())
		return true;
	
	// ֻ���CTC��վ
	if(gpcdef.IsCtcStation(station))
		return IsNeedCheckCtcModePlan(station);
	else
		return false;
}

int  CWorkScheduleHolder::CheckNeedSendToNeibSectionByEntry_SY(const CTG_SingleTrain *single,int neib_section,int this_section,int& nodeindex,int& passway,int& id, int& flag)
{
	const CTG_TrainRecord *rec_temp= NULL;
	flag=0;
	const NeibConsolePlanSY* pNeibConsole=GetTGFunctionConfig()->GetNeibConsolePlan(this_section,neib_section);
	if(pNeibConsole==NULL)
		return -1;

	for(int i=0;i<pNeibConsole->neibplan_count;i++)
	{
		// ����̨�г��в��ҳ�վ������վ
		rec_temp=single->GetTrainRecordByStation_Const(pNeibConsole->neib_plan[i].station_list[0]);
		if(rec_temp==NULL)
			continue;

		const int recindex = rec_temp->GetIndex();
		
		bool bmatch=true;
		int nindex=recindex;
		for(int k=0; k<pNeibConsole->neib_plan[i].station_count; k++)
		{
			nindex = recindex + k;
			rec_temp=single->GetTrainRecordNoDelete_Const(nindex);
			if(rec_temp == NULL)
			{
				bmatch=false;
				break;
			}
			if(rec_temp->GetStationNo()!=pNeibConsole->neib_plan[i].station_list[k])
			{
				bmatch=false;
				break;
			}
		}
		
		if(!bmatch || rec_temp==NULL)
			continue;

		if((pNeibConsole->neib_plan[i].flag==0) && (nindex < single->GetTrainRecordCount()-1))
		{
			GpcLogTrainMessage(0,0,"��̨���� %d Ҫ�����ó�վ�������һվΪ�г�βվ", i+1);
			continue;
		}

		if (pNeibConsole->neib_plan[i].exit_count==1 && pNeibConsole->neib_plan[i].exit[0]==-1)
		{
			flag=-2;
			id=i;
			nodeindex=pNeibConsole->neib_plan[i].station_index_list[0];
			passway=pNeibConsole->neib_plan[i].passway;

			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, ����������̨�ƻ�ƥ��(exit=-1) id=%d �ɹ� flag=%d nodeindex=%d passway=%d", 
				single->GetTrainID(), single->GetTrainIndex(),
				id, flag, nodeindex, passway);
			return recindex;
		}

		int entry_exit=0;
		if(rec_temp->IsTerminalRec())
			entry_exit= 0;
		else
			entry_exit=rec_temp->GetExit();

		for(int c=0; c<pNeibConsole->neib_plan[i].exit_count; c++)
		{
			if(pNeibConsole->neib_plan[i].exit[c]==entry_exit)
			{
				id=i;
				if(entry_exit==0)
					flag=-1;
				
				nodeindex=pNeibConsole->neib_plan[i].station_index_list[0];
				passway=pNeibConsole->neib_plan[i].passway;

				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, ����������̨�ƻ�ƥ�� id=%d �ɹ� flag=%d nodeindex=%d passway=%d", 
							single->GetTrainID(), single->GetTrainIndex(),
							id, flag, nodeindex, passway);
				return recindex;
			}
		}
	}
    return -1;
}

BOOL  CWorkScheduleHolder::GenTrain_NeibSchedule_SY(CTG_SingleTrain &newtrain,
													const CTG_SingleTrain &neibtrain, 
													int& nNeibStaNo, 
													int neib_section, 
													int *add_or_update,
													CTG_TrainScheduleBase* pData, 
													CStringArray& m_warning)
{
	newtrain.m_strSource="--";
	nNeibStaNo=0;
	CTG_ScheduleInfo schdinfo;
	m_cWorkSchduleData.GetScheduleInfo(schdinfo); 
	
	int this_section=schdinfo.m_nSectionID;
	int passway=0;
	int nodeindex=0;
	int recindex=0;
	int id=0, flag=0;

	// ���ճ����׼ȷƥ��
	recindex=CheckNeedSendToNeibSectionByEntry_SY(&neibtrain,neib_section,this_section,nodeindex,passway,id, flag);
	if(recindex<0)
		 return FALSE; 
	
	const CTG_TrainRecord *prec=neibtrain.GetTrainRecordNoDelete_Const(recindex);
	if(prec == NULL)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, recindex=%d�ļ�¼������,��̨����ʧ��", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), recindex);
		return FALSE;
	}

	// ̨�䳵վ
	nNeibStaNo = prec->m_data.station;
	GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d shiftindex=%d station=%d arrive=%d depart=%d id=%d(%d)", 
		neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(),
		prec->m_data.station, prec->m_data.arrive, prec->m_data.depart, id, flag);
	
	// ����ʹ��shiftindex ����
	CTG_SingleTrain *prev=GetTrainByNeibTrainShiftID(neibtrain, prec);

	// �����ճ��κŲ���
	if(!prev)  
	{
		bool bFind=false;
		prev=GetTrainByNeibTrainID(neibtrain, prec, pData,m_warning,bFind);
		if(bFind)
		{
			return FALSE;
		}
	}
	
	// �ҵ����г����복��һ��
	if(prev)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, �ҵ���̨�г� trainid=%s,index=%u, shiftindex=%u", 
			neibtrain.GetTrainID(), 
			neibtrain.GetTrainIndex(), 
			neibtrain.GetShiftTrainIndex(), 
			prev->GetTrainID(), 
			prev->GetTrainIndex(), 
			prev->GetShiftTrainIndex());

		if(!prev->MatchTrainID(prec->GetArriveTID()) && !prev->MatchTrainID(prec->GetDepartTID()))  // �ҵ����г����복��һ��
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s ����̨��¼���� arrive_id=%s depart_id=%s��ƥ��", 
				prev->GetTrainID(), prec->GetArriveTID(), prec->GetDepartTID());
			prev=NULL;
		}
	}
	newtrain.m_nCurrConTrainIndex = 0;
	if(prev && prev->GetTrainRecordCount()>0)
	{
		*add_or_update = 1;
		// �����Ѿ����ڵ��г���ֻҪƽ��ʱ��,
		newtrain=*prev;
		
		CTG_TrainRecord *ptrain_record=newtrain.GetTrainRecord(0);
		// ��վ�����Ǳ�̨��һ��վ
		if(ptrain_record->GetStationNo()!=prec->GetStationNo())
		{
			GpcLogTrainMessage(0,0,"��̨�Ѵ����г� trainid=%s, ��һ��վ�� %d �Ƿֽ�վ %d, ����ƽ��", 
					 prev->GetTrainID(), ptrain_record->GetStationNo(), prec->GetStationNo());
			return FALSE;
		}

		time_t next_time = prec->GetDepartTime();
		time_t ltime = ptrain_record->GetDepartTime(); 
		long time_off = next_time - ltime;
		if (abs(time_off) > 8*3600) 
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ʱ����8Сʱ����ƽ��", prev->GetTrainID());
			return FALSE;
		}
		
		// �ص�վ����
		CTG_TrainRecord *ptmprec=NULL;
		const CTG_TrainRecord *precneigb=NULL;

		int accept_arrive_depart=0;
		int accept_stop=1;
		int accept_side=0;
		int stationno_list[64];
		int station_count = GetTGFunctionConfig()->GetFJKStationPreList_SY(this_section, neib_section,id, stationno_list, 64, accept_side, accept_stop, accept_arrive_depart);
		for(int k=0; k<station_count; k++)
		{
			ptmprec=newtrain.GetTrainRecordNoDelete(k);
			precneigb=neibtrain.GetTrainRecordNoDelete_Const(recindex+k);
			if(ptmprec == NULL)
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����ϳ�վ����, ��������̨", prev->GetTrainID());
				return FALSE;
			}
			else
			{
				if(ptmprec->GetStationNo()!=stationno_list[k])
				{
					GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ���������ó�վ���� ���ó�վ%d, ��������̨", prev->GetTrainID(), stationno_list[k]);
					return FALSE;
				}
			}

			if(precneigb == NULL)
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����ϳ�վ����, ��������̨", prev->GetTrainID());
				return FALSE;
			}
			else
			{
				if(precneigb->GetStationNo()!=stationno_list[k])
				{
					GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ���������ó�վ���� ���ó�վ%d, ��������̨", prev->GetTrainID(), stationno_list[k]);
					return FALSE;
				}
			}			
		}

		if(ptmprec==NULL || precneigb == NULL)
			return FALSE;

		// �����ص�վ
		for(int k=0; k<station_count-1; k++)
		{
			ptmprec=newtrain.GetTrainRecordNoDelete(k);
			precneigb=neibtrain.GetTrainRecordNoDelete_Const(recindex+k);
			
			if(ptmprec!=NULL && precneigb!=NULL)
			{
				if((ptmprec->m_data.arrive != precneigb->GetArriveTime()) || 
					(ptmprec->m_data.depart != precneigb->GetDepartTime()))
				{
					ptmprec->m_data.arrive = precneigb->GetArriveTime();
					ptmprec->m_data.depart = precneigb->GetDepartTime();
				}
				if((ptmprec->m_data.arrive_side != precneigb->m_data.arrive_side) ||
					(ptmprec->m_data.depart_side != precneigb->m_data.depart_side))
				{
					ptmprec->m_data.arrive_side = precneigb->m_data.arrive_side;
					ptmprec->m_data.depart_side = precneigb->m_data.depart_side;
				}
			}
			else
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����ص�վʱ��ʧ��, ��������̨", prev->GetTrainID());
				return FALSE;
			}
		}
		
		ptmprec=newtrain.GetTrainRecordNoDelete(station_count-1);
		precneigb=neibtrain.GetTrainRecordNoDelete_Const(recindex+station_count-1);
		if(ptmprec==NULL || precneigb == NULL)
			return FALSE;

		const short this_index = ptmprec->GetIndex();
		if(accept_arrive_depart==1)
		{
			//2012.02.08 ��������
			/*time_off = precneigb->m_data.arrive - ptmprec->m_data.arrive;
			int tmp_time_off = precneigb->m_data.depart - ptmprec->m_data.depart;
				
			if(time_off!=0 || tmp_time_off!=0)
			{
				newtrain.m_syNeibFlag|=SY_FLAG_TIME_CHANGE;

				ptmprec->m_data.arrive = precneigb->m_data.arrive;
				ptmprec->m_data.depart = precneigb->m_data.depart;
			}*/
			if(precneigb->IsPassRec())
			{
				if(!ptmprec->IsPassRec())
				{
					CMMIOperFlag oper_flag;
					oper_flag.m_nRecIndex=this_index;
					oper_flag.nFlag=CMMIOperFlag::STAY_TO_PASS;
					GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
				}
			}
			else
			{
				if(ptmprec->IsPassRec())
				{
					CMMIOperFlag oper_flag;
					oper_flag.m_nRecIndex=this_index;
					oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
					GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
				}
			}
			int tmp_time_off = precneigb->m_data.arrive - ptmprec->m_data.arrive;
			time_off = precneigb->m_data.depart - ptmprec->m_data.depart;
				
			if(time_off!=0 || tmp_time_off!=0)
			{
				ptmprec->m_data.arrive = precneigb->m_data.arrive;
				ptmprec->m_data.depart = precneigb->m_data.depart;
			}
		}
		else
		{
			if(ptmprec->IsPassRec())
			{
				if(precneigb->IsPassRec())
				{
					time_off = precneigb->m_data.depart - ptmprec->m_data.depart;
				
					if(time_off!=0)
					{
						ptmprec->m_data.arrive = precneigb->m_data.arrive;
						ptmprec->m_data.depart = precneigb->m_data.depart;
					}
				}
				else
				{
					if(accept_stop)
					{
						int depart=ptmprec->m_data.depart;
						CMMIOperFlag oper_flag;
						oper_flag.m_nRecIndex=this_index;
						oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
						GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
			
						if(depart>=precneigb->m_data.arrive+60)
						{
							ptmprec->m_data.depart=depart;
							ptmprec->m_data.arrive = precneigb->m_data.arrive;
							time_off = -300;
						}
						else
						{
							time_off = precneigb->m_data.arrive - ptmprec->m_data.arrive;
							if(time_off!=0)
							{
								ptmprec->m_data.arrive = precneigb->m_data.arrive;
								ptmprec->m_data.depart += time_off;
							}
						}
						CTG_TrainRecord* pprec=newtrain.GetTrainRecordNoDelete(station_count);
						if(pprec==NULL)
						{
							if(ptmprec->IsTerminalRec())
								ptmprec->m_data.depart=ptmprec->m_data.arrive;
						}
					}
					else
					{
						time_off = precneigb->m_data.arrive - ptmprec->m_data.arrive;
						if(time_off!=0)
						{
							ptmprec->m_data.arrive = precneigb->m_data.arrive;
							ptmprec->m_data.depart = ptmprec->m_data.arrive;
						}
					}
				}
			}
			else
			{
				time_off = precneigb->m_data.arrive - ptmprec->m_data.arrive;
				if(time_off!=0)
				{
					ptmprec->m_data.arrive = precneigb->m_data.arrive;
					ptmprec->m_data.depart += time_off;
				}
			}
		}

		if(accept_side) // �Ƿ���շֽ糵վ�ɵ�
		{
			//time_off = precneigb->m_data.depart - ptmprec->m_data.depart;
			
			if((ptmprec->m_data.arrive_side != precneigb->m_data.arrive_side) ||
				(ptmprec->m_data.depart_side != precneigb->m_data.depart_side))
			{
				ptmprec->m_data.arrive_side = precneigb->m_data.arrive_side;
				ptmprec->m_data.depart_side = precneigb->m_data.depart_side;
			}
		}
		for(int x=this_index; x<newtrain.GetTrainRecordCount(); x++)
		{
			CTG_TrainRecord *ptmprec=newtrain.GetTrainRecord(x);
			if(TG_IsArriveActual(ptmprec->m_data) || TG_IsDepartActual(ptmprec->m_data))
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ����ʵ�ʵ㲻��ƽ��", prev->GetTrainID());
				return FALSE;
			}
		}
		
		newtrain.OffsetScheduleTime(this_index+1, time_off, FALSE, 1);
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, %d�����ƽ��ʱ��%d", prev->GetTrainID(), this_index, time_off);
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);

		//////////////////////////////////
		newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
	}
	else
	{
		*add_or_update = 0;
		
		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 

		// �ͳ�--ֻ�ܰ��ջ���ͼ�ӳ�, ����--�������þ����Ƿ��ջ���ͼ�ӳ�
		BOOL IsCanGenNei = TRUE;//YUZHU BEGIN
		if(GetTGFunctionConfig()->NeiConsiderPassenger()){
			if(neibtrain.IsPassengerTrain())
				IsCanGenNei = TRUE;
			else
				IsCanGenNei=FALSE;
		}//YUZHU END


		// �ͳ����ջ���ͼ�ӳ�
		if(IsCanGenNei && GenTrain_BasicSchedule(newtrain,schdinfo,prec->GetDepartTID(),NULL_STATION,NULL_TIME,NULL_TIME))
		{
			newtrain.m_strSource="����ͼ";

			TRAIN_INDEX basic = newtrain.GetBasicTrainIndex();
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetTrainInfo()->m_data.basic_train_index = basic;
			newtrain.SetTrainRecordCountToDefault();

			//�޸����� CUIHU  2009.11.20 ������̨�ֽ�վ��Ϊ��̨��վ���
			CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
			if(pfirst_rec)
			{
				int offset=prec->m_data.depart-pfirst_rec->m_data.depart;
				pfirst_rec->m_data.depart= prec->m_data.depart;
				pfirst_rec->m_data.arrive= prec->m_data.arrive;
				newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
				
				GpcLogTrainMessage(0,0,"��̨�Ҳ����г� trainid=%s, ���ջ���ͼ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
					neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), offset);

				return TRUE;
			}
			else
			{
				GpcLogTrainMessage(0,0,"��̨�Ҳ����г� trainid=%s, ���ջ���ͼ�ӳ�, ���ǻ���ͼ��������û�г�վ %d, ��̨�����г�ʧ��", 
					neibtrain.GetTrainID(), prec->GetStationNo());
			}
		}
		
		bool bGenok=false;
		// �������ջ���ͼ�ӳ�ʧ���� ���վ�·�ӳ�
		if(flag == -1)
		{
			CTG_TrainRecordList* list_rec = newtrain.GetRecordList();
			list_rec->PurgeRecords();
			
			const NeibPlanSY* pNeibPlan = GetTGFunctionConfig()->GetNeibPlanSY(this_section, neib_section, id);
			if(pNeibPlan)
			{
				for(int k=0; k<pNeibPlan->station_count; k++)
				{
					const CTG_TrainRecord* prec=neibtrain.GetTrainRecordNoDelete_Const(recindex+k);
					if(prec==NULL)
						break;
					if(pNeibPlan->station_list[k]!=prec->GetStationNo())
						break;

					CTG_TrainRecord precneigb=*prec;

					if(pNeibPlan->station_index_list[k*2]>=0)
						precneigb.SetArriveTGIndex(pNeibPlan->station_index_list[k*2]);
					if(pNeibPlan->station_index_list[k*2+1]>=0)
						precneigb.SetDepartTGIndex(pNeibPlan->station_index_list[k*2+1]);

					precneigb.SetStationText("",STATION_TXT_INFO_TYPE_SHUAIGUA);
					precneigb.SetOpmsSationTextState("0");
					if(k == 0)
						TG_SetFlagAccept(precneigb.m_data.flag);
					
					if(k==pNeibPlan->station_count-1)
						TG_SetFlagTerminal(precneigb.m_data.flag);
					
					list_rec->AddRecord(precneigb);
				}
				if(k>=pNeibPlan->station_count)
					bGenok=true;
			}

			if(bGenok)
			{
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.SetTrainRecordCountToDefault();
				newtrain.m_strSource="������̨";
				GpcLogTrainMessage(0,0,"��̨�Ҳ����г� trainid=%s, ��̨�г��յ�, ����neibplan.xml�� %d̨ �� %d̨���� %d, �����ص����ֳ�վ�г� %s", 
					neibtrain.GetTrainID(), this_section, neib_section, id, newtrain.toString());
			}
		}
		else if(flag == -2)
		{
			CTG_TrainRecordList* list_rec = newtrain.GetRecordList();
			list_rec->PurgeRecords();

			const NeibPlanSY* pNeibPlan = GetTGFunctionConfig()->GetNeibPlanSY(this_section, neib_section, id);
			if(pNeibPlan)
			{
				for(int k=0; k<pNeibPlan->station_count; k++)
				{
					const CTG_TrainRecord* prec=neibtrain.GetTrainRecordNoDelete_Const(recindex+k);
					if(prec==NULL)
						break;
					if(pNeibPlan->station_list[k]!=prec->GetStationNo())
						break;

					CTG_TrainRecord precneigb=*prec;

					if(pNeibPlan->station_index_list[k*2]>=0)
						precneigb.SetArriveTGIndex(pNeibPlan->station_index_list[k*2]);
					if(pNeibPlan->station_index_list[k*2+1]>=0)
						precneigb.SetDepartTGIndex(pNeibPlan->station_index_list[k*2+1]);
					
					precneigb.SetStationText("",STATION_TXT_INFO_TYPE_SHUAIGUA);
					precneigb.SetOpmsSationTextState("0");
					if(recindex > 0)
						TG_SetFlagAccept(precneigb.m_data.flag);

					list_rec->AddRecord(precneigb);
				}
				if(k>=pNeibPlan->station_count)
					bGenok=true;
			}

			if(bGenok)
			{
				newtrain.m_strSource="������̨";
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.SetTrainRecordCountToDefault();

				GpcLogTrainMessage(0,0,"��̨�Ҳ����г� trainid=%s, ����neibplan.xml�� %d̨ �� %d̨���� %d �ĳ�վ���и�����̨�г���Ӧ��վ,�����г� %s", 
					neibtrain.GetTrainID(), this_section, neib_section, id, newtrain.toString());
			}
		}
		else if(!neibtrain.IsPassengerTrain()&& passway>0 && GenTrain_Passway(newtrain,schdinfo,prec->GetArriveTID(),passway,0,0,prec->GetStationNo(),prec->GetDepartTime(),255))
		{	
			const NeibPlanSY* pNeibPlan = GetTGFunctionConfig()->GetNeibPlanSY(this_section, neib_section, id);
			if(pNeibPlan)
			{
				for(int k=0; k<pNeibPlan->station_count; k++)
				{
					CTG_TrainRecord* pnewtrainRec = newtrain.GetTrainRecord(k);
					const CTG_TrainRecord* precneigb=neibtrain.GetTrainRecordNoDelete_Const(recindex+k);

					if(pnewtrainRec==NULL || precneigb==NULL)
						continue;

					if(pnewtrainRec->GetStationNo()!=precneigb->GetStationNo())
						continue;
					
					if(precneigb->GetStationNo()==pNeibPlan->station_list[k])
					{
						STR_CPY(pnewtrainRec->m_data.arrive_train_id, precneigb->m_data.arrive_train_id);
						STR_CPY(pnewtrainRec->m_data.depart_train_id, precneigb->m_data.depart_train_id);
						STR_CPY(pnewtrainRec->m_data.original_arrive_train_id, precneigb->m_data.original_arrive_train_id);
						STR_CPY(pnewtrainRec->m_data.original_depart_train_id, precneigb->m_data.original_depart_train_id);
						if(!precneigb->IsPassRec())
						{
							CMMIOperFlag oper_flag;
							oper_flag.m_nRecIndex=k;
							oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
							GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
							
							int time_off = precneigb->m_data.depart-pnewtrainRec->m_data.depart;
							newtrain.OffsetScheduleTime(k+1, time_off, FALSE, 1);
							pnewtrainRec->m_data.arrive = precneigb->m_data.arrive;
							pnewtrainRec->m_data.depart = precneigb->m_data.depart;
							pnewtrainRec->m_data.arrive_side = precneigb->m_data.arrive_side;
							pnewtrainRec->m_data.depart_side = precneigb->m_data.depart_side;
							pnewtrainRec->m_data.entry = precneigb->m_data.entry;
							pnewtrainRec->m_data.exit = precneigb->m_data.exit;
						}
						else
						{
							pnewtrainRec->m_data.arrive = precneigb->m_data.arrive;
							pnewtrainRec->m_data.depart = precneigb->m_data.depart;
							pnewtrainRec->m_data.arrive_side = precneigb->m_data.arrive_side;
							pnewtrainRec->m_data.depart_side = precneigb->m_data.depart_side;
							pnewtrainRec->m_data.entry = precneigb->m_data.entry;
							pnewtrainRec->m_data.exit = precneigb->m_data.exit;
						}
					}
				}

				CTG_TrainRecord* preclast = newtrain.GetTrainRecord(pNeibPlan->station_count-1);
				if(preclast)
				{
					for(int k=pNeibPlan->station_count; k<newtrain.GetTrainRecordCount(); k++)
					{
						CTG_TrainRecord* pnewtrainRec = newtrain.GetTrainRecord(k);
						if(pnewtrainRec==NULL)
							continue;

						STR_CPY(pnewtrainRec->m_data.arrive_train_id, preclast->m_data.depart_train_id);
						STR_CPY(pnewtrainRec->m_data.depart_train_id, preclast->m_data.depart_train_id);
						STR_CPY(pnewtrainRec->m_data.original_arrive_train_id, preclast->m_data.original_depart_train_id);
						STR_CPY(pnewtrainRec->m_data.original_depart_train_id, preclast->m_data.original_depart_train_id);
					}
				}
			}

			newtrain.m_strSource="Ĭ�Ͼ�·";
			bGenok=true;
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.SetTrainRecordCountToDefault();

			GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���վ�· %d �ӳ�", 
					neibtrain.GetTrainID(), passway);
		}

		// ���㳵
		if(!bGenok)
		{
			newtrain.m_strSource="��ʱ����";
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetRecordList()->PurgeRecords(); 
			
			const CTG_TrainRecord *prec_neib=neibtrain.GetTrainRecordNoDelete_Const(recindex);
			if(!prec_neib)
				return FALSE; // ������
			CTG_TrainRecord train_record=*prec_neib;
			train_record.SetStationText("",STATION_TXT_INFO_TYPE_SHUAIGUA);
			train_record.SetOpmsSationTextState("0");
			train_record.rec_index=0; 
			TG_SetFlagAccept(train_record.m_data.flag);
			TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
			train_record.SetArriveTGIndex(nodeindex);
			train_record.SetDepartTGIndex(nodeindex);
			train_record.SetPlanSendTime(0); //��Ҫ! 
			newtrain.GetRecordList()->AddRecord(train_record);

			GpcLogTrainMessage(0,0,"��̨�Ҳ����г� trainid=%s, ���ɵ����յ���, ��վ���� %d", neibtrain.GetTrainID(), nodeindex);
			
			newtrain.SetTrainRecordCountToDefault(); 
		}

		CTGAddTrainLogic::AutoChangeTrainID(&newtrain, this_section);
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
		CTGAddTrainLogic::ChangeTrainElec(&newtrain, this_section);
	}
	return TRUE;
}

// ���ݻ�����
void CWorkScheduleHolder::PassTrainEngineNo(CTG_SingleTrain& newtrain, const CTG_SingleTrain* train)
{
	if(NULL == train)
		return;

	// ����old
	CTG_TrainEngineList oldEngineList=*newtrain.GetEngineList();
	
	newtrain.GetEngineList()->PurgeRecords();

	for(int i=0;i<train->GetEngineCount();i++)
	{
		CTG_TrainEngineInfo newengine=*train->GetEngine_Const(i);
		if(newengine.m_data.unattach_sta < train->GetTrainRecordCount()-1)
			continue;
		newengine.m_data.attach_sta=0;
		newengine.m_data.unattach_sta=newtrain.GetTrainRecordCount()-1;
		newtrain.GetEngineList()->AddRecord(newengine);  
	}
	newtrain.SetElecFlagByEngineInfo(); // ���ݻ����������õ�����־

	return;
}

BOOL   CWorkScheduleHolder::AdjustTrainForCollision(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	
	adjust_logic.AdjustTrainForCollision(&train,recIndex);

	adjust_logic.CacuTrainSlowTime(&train); // 20090917 ����ϻ�����
	return TRUE;
}

BOOL   CWorkScheduleHolder::AdjustTrainForBasicSchedule(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd)
{
    CTG_SingleTrain newtrain;
	if(!GenTrain_BasicSchedule(newtrain, schd, train.GetTrainID(), NULL_STATION, NULL_TIME, NULL_TIME))
		return FALSE;
	for(int i=recIndex;i<train.GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=train.GetTrainRecord(i);
		CTG_TrainRecord *pbasic_rec=newtrain.GetTrainRecord(i);
		if(!prec || !pbasic_rec)
			return FALSE;
		if(prec->GetStationNo()!=pbasic_rec->GetStationNo())
			return FALSE;
		*prec=*pbasic_rec;
	}

	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	adjust_logic.CacuTrainSlowTime(&train); // 20090917 ����ϻ�����

	return TRUE;
}

BOOL   CWorkScheduleHolder::AdjustTrainForMutiPass(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	adjust_logic.AdjustTrainForMutiQuickPass(&train,recIndex,schd.m_nSectionID);

	adjust_logic.CacuTrainSlowTime(&train); // 20090917 ����ϻ�����

	return TRUE;
}
BOOL  CWorkScheduleHolder::GetRelaTrainsOfMark(const CTG_SingleMark *pmark,CTG_ObjectPtrGroup &rela_trains,const CTG_ScheduleInfo &schd) //������mark�����ĳ���
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	return adjust_logic.GetRelaTrainsOfMark(pmark,rela_trains);
}

BOOL  CWorkScheduleHolder::AdjustSingleTrainForMark(CTG_SingleTrain &train,int recIndex,const CTG_SingleMark *pmark,const CTG_ScheduleInfo &schd)
{
    CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
	return adjust_logic.AdjustSingleTrainForMark(train,recIndex,pmark);
}
 
int  CWorkScheduleHolder::CheckNeedSendToNeibSectionByReverse(const CTG_SingleTrain *single,int this_section,int neib_section,int& matched_start,int& matched_count,int& id)
{
	const NeibConsolePlanReverse* pNeibConsole=GetTGFunctionConfig()->GetNeibPlan_Reverse(this_section,neib_section);
	if(pNeibConsole==NULL)
		return -1;

	int nTrainRecordCount=single->GetTrainRecordCount();

	id=-1;
	int flag=-1;

	// �ж���վ
	const CTG_TrainRecord *rec_temp=single->GetTrainRecordByRecIndex_Const(0);
	if(rec_temp!=NULL)
	{
		if(rec_temp->IsStartRec())
			flag=1;// ʼ�����
		else
			flag=2;// �������

		for(int a=0; a<pNeibConsole->neibplan_num && a<sizeof(pNeibConsole->neib_plan)/sizeof(pNeibConsole->neib_plan[0]); a++)
		{
			const NeibPlanReverse* pPlanRev= &pNeibConsole->neib_plan[a];
			if(pPlanRev==NULL)
				continue;

			if((pPlanRev->flag[0]==flag) || (pPlanRev->flag[1]==flag))
			{
				for(int b=0; b<pPlanRev->station_count; b++)
				{
					rec_temp=single->GetTrainRecordByRecIndex_Const(b);
					if(rec_temp==NULL)
						break;

					if(pPlanRev->station_list[b]!=rec_temp->m_data.station)
						break;
				}

				if(b>=pPlanRev->station_count)
				{
					matched_start=0;
					matched_count=pPlanRev->station_count;
					id=a;
				
					break;
				}
			}
		}
	}
	
	if(id>=0)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, ���շ���վ��ƥ�� id=%d �ɹ� flag=%d matched_start_index=%d matched_rec_count=%d", 
			single->GetTrainID(), single->GetTrainIndex(),
			id, flag, matched_start, matched_count);

		return flag;
	}
	return -1;
}

BOOL  CWorkScheduleHolder::GenTrain_NeibSchedule_Reverse(CTG_SingleTrain &newtrain,const CTG_SingleTrain &neibtrain, int& nNeibStaNo, int neib_section, int *add_or_update)
{
	nNeibStaNo=0;
	CTG_ScheduleInfo schdinfo;
	m_cWorkSchduleData.GetScheduleInfo(schdinfo); 
	
	int this_section=schdinfo.m_nSectionID;

	int matched_start=0;
	int matched_count=0;

	int id=0;

	// ���շ���վ����ƥ�� 
	int flag=CheckNeedSendToNeibSectionByReverse(&neibtrain,this_section, neib_section, matched_start, matched_count, id);
	if(flag<0)
		 return FALSE; 

	const NeibConsolePlanReverse* pNeibConsole=GetTGFunctionConfig()->GetNeibPlan_Reverse(this_section,neib_section);
	if(pNeibConsole==NULL)
		return -1;

	const NeibPlanReverse* pNeibStation=&pNeibConsole->neib_plan[id];

	// �ж�Ŀ�공վ
	const CTG_TrainRecord *precord=neibtrain.GetTrainRecordByStation_Const(pNeibStation->station);
	if(precord==NULL)
		return FALSE;
	
	CTG_SingleTrain *prev=NULL;
	CTG_TrainPtrGroup  trains_in_range;
	CTime tm=CTime::GetCurrentTime();
	m_cWorkSchduleData.FindTrains(trains_in_range, precord->m_data.station);			

	for(int i=0;i<trains_in_range.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=trains_in_range.GetTrainPtr(i);
		const CTG_TrainRecord *prev_rec= ptrain->GetTrainRecordByStation_Const(precord->m_data.station);
		if(!prev_rec)
			continue;
		if(!ptrain->MatchTrainID(precord->GetArriveTID()) && !ptrain->MatchTrainID(precord->GetDepartTID()))
			continue;
		if(prev_rec->GetArriveTime()>0 && precord->GetArriveTime()>0 && abs( prev_rec->GetArriveTime()- precord->GetArriveTime())<3*3600) 
		{
			prev=ptrain;
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, ���ճ����ҵ���̨�г� trainid=%s %d arrive=%d depart=%d", 
				neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), 
				prev->GetTrainID(), prev->GetTrainIndex(), prev_rec->m_data.arrive, prev_rec->m_data.depart);
			break;
		}
		if(prev_rec->GetDepartTime()>0 && precord->GetDepartTime()>0 && abs( prev_rec->GetDepartTime()- precord->GetDepartTime())<3*3600) 
		{
			prev=ptrain;
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%d, ���ճ����ҵ���̨�г� trainid=%s %d arrive=%d depart=%d", 
				neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), 
				prev->GetTrainID(), prev->GetTrainIndex(), prev_rec->m_data.arrive, prev_rec->m_data.depart);
			break;
		}
	}	
	
	if(prev)
	{
		if(!prev->MatchTrainID(precord->GetArriveTID()) && !prev->MatchTrainID(precord->GetDepartTID()))  // �ҵ����г����복��һ��
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s ����̨��¼���� arrive_id=%s depart_id=%s��ƥ��", 
				prev->GetTrainID(), precord->GetArriveTID(), precord->GetDepartTID());
			prev=NULL;
		}
	}

	GpcLogTrainMessage(0,0,"��̨�ƻ� trainid=%s, flag=%d", neibtrain.GetTrainID(), flag);

	newtrain.m_nCurrConTrainIndex = 0;
	if(prev && prev->GetTrainRecordCount()>0)
	{
		*add_or_update = 1;
		// �����Ѿ����ڵ��г���ֻҪƽ��ʱ��,
		newtrain=*prev;
	
		int nNewTrainRecordNum=newtrain.GetTrainRecordCount();
		for(int s=0; s<nNewTrainRecordNum; s++)
		{
			CTG_TrainRecord* prec=newtrain.GetTrainRecordByRecIndex(s);
			if(prec==NULL)
				return FALSE;

			prec->rec_index=s;
		}

		CTG_TrainRecord *precthis=NULL;
		const CTG_TrainRecord *precneigb=NULL;

		precthis=newtrain.GetTrainRecordByStation(pNeibStation->station_list[0]);
		if(precthis==NULL)
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����ڳ�վ %d", 
						newtrain.GetTrainID(), pNeibStation->station_list[0]);
			return FALSE;
		}

		// ��̨ ʼ�� ���� Ҫ��̨�г���վΪ���һվ
		if((precthis->rec_index+1)!=nNewTrainRecordNum)
		{
			if(pNeibStation->station_count > 1)
			{
				CTG_TrainRecord *pprec=newtrain.GetTrainRecordByStation(pNeibStation->station_list[pNeibStation->station_count-1]);
				if(pprec==NULL)
				{
					GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����ڳ�վ %d", 
								newtrain.GetTrainID(), pNeibStation->station_list[pNeibStation->station_count-1]);
					return FALSE;
				}
				else 
				{
					if((pprec->rec_index+1)!=nNewTrainRecordNum)
					{
						GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����Գ�վ %d ����", 
							newtrain.GetTrainID(), pNeibStation->station_list[0]);
						return FALSE;
					}
					else
					{
						if(pprec->IsTerminalRec())
						{
							GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �Գ�վ %d �յ���û�н�����̨", 
								newtrain.GetTrainID(), pNeibStation->station_list[pNeibStation->station_count-1]);
							return FALSE;
						}
					}
				}
			}
			else
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �����Գ�վ %d ����", 
					newtrain.GetTrainID(), pNeibStation->station_list[0]);
				return FALSE;
			}
		}
		else
		{
			if(precthis->IsTerminalRec())
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, �Գ�վ %d �յ���û�н�����̨", 
					newtrain.GetTrainID(), pNeibStation->station_list[0]);
				return FALSE;
			}
		}

		int this_start=precthis->GetIndex();
		for(int k=0; k<matched_count; k++)
		{
			precthis=newtrain.GetTrainRecordNoDelete(this_start+k);
			precneigb=neibtrain.GetTrainRecordNoDelete_Const(matched_start+k); // ������
			if(precthis==NULL||precneigb==NULL)
				return FALSE;

			if(precthis->m_data.station!=precneigb->m_data.station)
				return FALSE;
		}

		precthis=newtrain.GetTrainRecordByStation(pNeibStation->station_list[0]);
		precneigb=neibtrain.GetTrainRecordNoDelete_Const(matched_start); // ������

		time_t next_time = precneigb->GetDepartTime();
		time_t ltime = precthis->GetDepartTime(); 
		long time_off = next_time - ltime;
		if (abs(time_off) > 8*3600) 
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ʱ����8Сʱ����ƽ��", prev->GetTrainID());
			return FALSE;
		}
		
		for(int k=0; k<matched_count; k++)
		{
			precthis=newtrain.GetTrainRecordNoDelete(this_start+k);
			precneigb=neibtrain.GetTrainRecordNoDelete_Const(matched_start+k); // ������
			
			if(precthis==NULL||precneigb==NULL)
				return FALSE;

			if(TG_IsArriveActual(precthis->m_data) || TG_IsDepartActual(precthis->m_data))
			{
				continue;
			}
			
			// ������̨���� ��һ��վ�ĵ���, ����վ������
			if(k==0 && pNeibStation->only_accept_depart==1 && flag==2)
			{
				if(precthis->m_data.depart != precneigb->m_data.depart&&precthis->m_data.arrive+60 <= precneigb->m_data.depart)
				{
					if(precthis->IsPassRec())
					{
						if(!precneigb->IsPassRec() && pNeibStation->accept_stop) // �Ƿ���յ�����ϵ
						{
							CMMIOperFlag oper_flag;
							oper_flag.m_nRecIndex=this_start;
							oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
							GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);

							precthis->m_data.depart = precneigb->m_data.depart;
						
							if(precthis->rec_index==0)
							{
								if(precthis->IsStartRec())
									precthis->m_data.arrive=precthis->m_data.depart;
							}
						}
					}
					else
					{
						precthis->m_data.depart = precneigb->m_data.depart;
					
						if(precthis->rec_index==0)
						{
							if(precthis->IsStartRec())
								precthis->m_data.arrive=precthis->m_data.depart;
						}
					}
				}
			}
			else
			{
				if((precthis->m_data.arrive != precneigb->m_data.arrive) 
					||(precthis->m_data.depart != precneigb->m_data.depart))
				{
					precthis->m_data.arrive = precneigb->m_data.arrive;
					precthis->m_data.depart = precneigb->m_data.depart;
				}
			}
		}

		GpcLogTrainMessage(0,0,"��̨�ƻ� trainid=%s, flag=%d ������̨ʱ���޸ı�̨ʱ��", prev->GetTrainID(), flag);
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
		//////////////////////////////////
		newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		return TRUE;
	}
	else if(flag==1)
	{
		// ��ȫ���հ�����̨�ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 

		CTG_TrainRecordList* list_rec = newtrain.GetRecordList();
		list_rec->PurgeRecords();

		CTG_TrainRecord ptrainrec;
		for(int k=0; k<matched_count; k++)
		{
			ptrainrec=*neibtrain.GetTrainRecordNoDelete_Const(matched_start+k);
			
			ptrainrec.rec_index=k;
			ptrainrec.SetArriveTGIndex(pNeibStation->station_index_list[2*k]);
			ptrainrec.SetDepartTGIndex(pNeibStation->station_index_list[2*k+1]);
			ptrainrec.SetPlanSendTime(0); //��Ҫ! 
			ptrainrec.SetStationText("",STATION_TXT_INFO_TYPE_SHUAIGUA);
			ptrainrec.SetOpmsSationTextState("0");

			list_rec->AddRecord(ptrainrec);
		}
		const CTG_TrainRecord* prec=neibtrain.GetTrainRecordNoDelete_Const(matched_start+k);
		if(prec)
		{
			CTG_TrainRecord* preclast=newtrain.GetTrainRecord(matched_count-1);
			preclast->SetGiveoutRec();
		}
		prec=neibtrain.GetTrainRecordNoDelete_Const(matched_start-1);
		if(prec)
		{
			CTG_TrainRecord* precfirst=newtrain.GetTrainRecord(0);
			precfirst->SetAcceptRec();
		}

		*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
		RemoveTrainAbsTimeType(&newtrain);
		newtrain.SetTrainRecordCountToDefault();
		
		GpcLogTrainMessage(0,0,"��̨�����г� trainid=%s, stalist:%s", 
						newtrain.GetTrainID(), newtrain.toString());
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);

		return TRUE;
	}

	return FALSE;
}

BOOL CWorkScheduleHolder::CheckTrainDepartStplan(int con_id, CTG_SingleTrain &srctrain, CTG_ObjectPtrGroup& markgroup, int rec_index, CString& str)
{
	str="";
	if(!srctrain.IsElectricTrain())
		return FALSE;

	CTG_TrainRecord *rec = srctrain.GetTrainRecordNoDelete(rec_index);
	if(NULL == rec)
		return FALSE;

	std::vector<int> list;
	if(!GetTGFunctionConfig()->GetTrainDepartCheckInfo(con_id, rec->m_data.station, list))
		return FALSE;

	int count=srctrain.GetTrainRecordCount();
	for(int i=rec_index; i<count; i++)
	{
		CTG_TrainRecord *prec = srctrain.GetTrainRecordNoDelete(i);
		if(NULL==prec)
			break;

		for(int c=0; c<list.size(); c++)
		{
			if(list[c]==prec->m_data.station)
			{
				ushort linono=0,st_dir=0;
				if(!GetTGBasicParam()->ConvertStationEntryToAbsnum(prec->m_data.station,prec->m_data.exit,&linono,&st_dir))
					continue;

				std::vector<int> list_mark_abs;
				for(int i=0;i<markgroup.GetCount();i++)
				{
					const CTG_SingleMark *iter_mark=(const CTG_SingleMark *)markgroup.GetPtr(i);
					if(iter_mark==NULL)
						continue;
					
					const CTG_SingleMark& mark = *iter_mark;
					if(mark.GetMarkType()!=TG_SECTION_ELEC_FIX)
						continue;

					if(prec->m_data.depart<mark.m_data.start_time||mark.m_data.end_time<prec->m_data.depart)
						continue;

					for(int s=0; s<mark.m_data.ele_count; s++)
						list_mark_abs.push_back(mark.m_data.ele_number[s]);
				}

				for(int q=0; q<list_mark_abs.size(); q++)
				{
					if(linono==list_mark_abs[q])
					{
						str.Format("%s����%sվ�����޵���,%sվ���ܷ���", srctrain.GetTrainID(),
							gpcdef.GetStationName(prec->m_data.station), gpcdef.GetStationName(rec->m_data.station));
						break;
					}
				}
				
				if(str!="")
					break;
			}
		}

		if(str!="")
			break;
	}

	if(str!="")
		return TRUE;

	return FALSE;
}

CTG_SingleTrain* CWorkScheduleHolder::GetTrainByShiftIDFromShiftSchedule(const DWORD nShiftTrainIndex, const int nStation)
{
	CTG_SingleTrain *prev=NULL;
	if(nShiftTrainIndex!=NULL_TRAIN_INDEX)
	{
		prev=m_cShiftSchduleData.GetSingleTrain(nShiftTrainIndex); 
		if(NULL != prev)
		{
			GpcLogTrainMessage(0,0,"����shiftindex=%d �ҵ���̨�հ�ƻ��г� %s %d", 
					nShiftTrainIndex, prev->GetTrainID(), prev->GetTrainIndex());

			const CTG_TrainRecord *prev_rec= prev->GetTrainRecordByStation_Const(nStation);
			if(NULL==prev_rec)
			{
				GpcLogTrainMessage(0,0,"��̨�հ�ƻ��г� %s %d, ���Ҳ���ƥ�䳵վ %d", 
								prev->GetTrainID(), prev->GetTrainIndex(), nStation);
				prev=NULL;
			}
		}
	}

	return prev;
}

CTG_SingleTrain* CWorkScheduleHolder::GetTrainByTrainNoFromShiftSchedule(const CTG_SingleTrain &neibtrain,int neib_station,CStringArray& m_warning, bool& bFind)
{
	bFind=false;
	GpcLogTrainMessage(0,0,"���ճ��κ��ڱ�̨�հ�ƻ��в����г� trainid=%s,index=%u, shiftindex %u, ���ճ�����ǰ3Сʱ��3Сʱ��������", 
		neibtrain.GetTrainID(), neibtrain.GetTrainIndex(),  neibtrain.GetShiftTrainIndex() );
	
	const CTG_TrainRecord  *prec = neibtrain.GetTrainRecordByStation_Reverse_Const(neib_station);
	if (!prec)
		return NULL;

	long tStartTime=time(NULL);
	long tEndTime=tStartTime+9*3600;
	if(prec->GetArriveTime()>0)
	{
		long tNeibTime=prec->GetArriveTime();
		tStartTime=tNeibTime-3*3600;
		tEndTime=tNeibTime+6*3600;
	}
	else if(prec->GetDepartTime()>0)
	{
		long tNeibTime=prec->GetDepartTime();
		tStartTime=tNeibTime-3*3600;
		tEndTime=tNeibTime+6*3600;
	}
		
	CTG_TrainPtrGroup  trains_in_range;
	m_cShiftSchduleData.FindTrains(trains_in_range,prec->GetStationNo(), tStartTime, tEndTime);			

	BYTE trainindex_num=0;
	DWORD train_index[64];
	for(int i=0;i<trains_in_range.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=trains_in_range.GetTrainPtr(i);
		if(NULL==ptrain)
			continue;
		const CTG_TrainRecord *prev_rec= ptrain->GetTrainRecordByStation_Const(prec->GetStationNo());
		if(!prev_rec)
			continue;
		
		if(!ptrain->MatchTrainID(prec->GetArriveTID()) && !ptrain->MatchTrainID(prec->GetDepartTID()))
			continue;
		
		train_index[trainindex_num]=ptrain->GetTrainIndex();
		trainindex_num++;
	}

	CString strtt, tmp;
	for(int n=0; n<trainindex_num; n++)
	{
		CTG_SingleTrain* pTrain=m_cShiftSchduleData.GetSingleTrain(train_index[n]);
		if(pTrain)
		{
			tmp.Format("����%s(%d),", pTrain->GetTrainID(), pTrain->GetTrainIndex());
			strtt+=tmp;
			GpcLogTrainMessage(0,0,"�ҵ���̨�հ�ƻ����г� trainid=%s, index=%u, shiftindex %u", 
				pTrain->GetTrainID(), pTrain->GetTrainIndex(),  pTrain->GetShiftTrainIndex());
		}
	}
	if(trainindex_num>1)
	{
		bFind=true;
		
		CString str;
		str.Format("�հ�ƻ��� %s ���� %d ��%s �޷�����̨���� %s(%d)ƥ��,���˹���������", 
				gpcdef.GetStationName(prec->GetStationNo()),
				trainindex_num, 
				strtt,
				neibtrain.GetTrainID(),
				neibtrain.GetTrainIndex());

		m_warning.Add(str);
		GpcLogTrainMessage(0,0,"%s", str);
	}
	
	if(trainindex_num==1)
	{
		return m_cShiftSchduleData.GetSingleTrain(train_index[0]);
	}
	
	return NULL;
}

CTG_SingleTrain* CWorkScheduleHolder::GetTrainByNeibTrainShiftID(const CTG_SingleTrain& neibtrain, const CTG_TrainRecord* pneib_rec)
{
	CTG_SingleTrain *prev=NULL;
	const DWORD nShiftTrainIndex=neibtrain.GetShiftTrainIndex();
	if(nShiftTrainIndex!=NULL_TRAIN_INDEX && pneib_rec!=NULL)
	{
		const int nStation=pneib_rec->GetStationNo();
		prev=m_cWorkSchduleData.GetSingleTrainByShiftIndex(nShiftTrainIndex, nStation); 
		if(NULL != prev)
		{
			GpcLogTrainMessage(0,0,"����shiftindex=%d �ҵ���̨�г� %s %d", 
					nShiftTrainIndex, prev->GetTrainID(), prev->GetTrainIndex());

			if(!prev->MatchTrainID(pneib_rec->GetArriveTID()) && !prev->MatchTrainID(pneib_rec->GetDepartTID()))  // �ҵ����г����복��һ��
			{
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, �ҵ���̨�г� trainid=%s,index=%u, shiftindex=%u,���ǳ��β�ƥ��", 
						neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(), 
						prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());
				prev=NULL;
			}
		}
	}

	return prev;
}

CTG_SingleTrain* CWorkScheduleHolder::GetTrainByNeibTrainID(const CTG_SingleTrain &neibtrain, 
															const CTG_TrainRecord *prec, 
															CTG_TrainScheduleBase* pData, 
															CStringArray& m_warning,
															bool& bFind)
{
	bFind=false;
	GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex %u, ���ճ�����ǰ3Сʱ��3Сʱ�������ұ�̨�г�", 
		neibtrain.GetTrainID(), neibtrain.GetTrainIndex(),  neibtrain.GetShiftTrainIndex() );
	
	long tStartTime=time(NULL);
	long tEndTime=tStartTime+9*3600;
	if(prec->GetArriveTime()>0)
	{
		long tNeibTime=prec->GetArriveTime();
		tStartTime=tNeibTime-3*3600;
		tEndTime=tNeibTime+3*3600;
	}
	else if(prec->GetDepartTime()>0)
	{
		long tNeibTime=prec->GetDepartTime();
		tStartTime=tNeibTime-3*3600;
		tEndTime=tNeibTime+3*3600;
	}
		
	CTG_TrainPtrGroup  trains_in_range;
	m_cWorkSchduleData.FindTrains(trains_in_range,prec->GetStationNo(), tStartTime, tEndTime);			

	BYTE trainindex_num=0;
	DWORD train_index[64];
	for(int i=0;i<trains_in_range.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=trains_in_range.GetTrainPtr(i);
		if(NULL==ptrain)
			continue;
		const CTG_TrainRecord *prev_rec= ptrain->GetTrainRecordByStation_Const(prec->GetStationNo());
		if(!prev_rec)
			continue;
		CTG_SingleTrain *ptmp1train=pData->GetSingleTrainForNeib(ptrain->GetTrainIndex());
		if(ptmp1train)
			continue;

		if(!ptrain->MatchTrainID(prec->GetArriveTID()) && !ptrain->MatchTrainID(prec->GetDepartTID()))
			continue;
		//if(ptrain->HasActualRecord()>=0)
		//{
		//	GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, index=%u, shiftindex %u, ����ʵ�ʵ�,����Ϊ��ѡ�г�", 
		//		ptrain->GetTrainID(), ptrain->GetTrainIndex(),  ptrain->GetShiftTrainIndex());
		//	continue;
		//}
		train_index[trainindex_num]=ptrain->GetTrainIndex();
		trainindex_num++;
	}

	CString strtt, tmp;
	for(int n=0; n<trainindex_num; n++)
	{
		CTG_SingleTrain* pTrain=m_cWorkSchduleData.GetSingleTrain(train_index[n]);
		if(pTrain)
		{
			tmp.Format("����%s(%d),", pTrain->GetTrainID(), pTrain->GetTrainIndex());
			strtt+=tmp;
			GpcLogTrainMessage(0,0,"�ҵ���̨�г� trainid=%s, index=%u, shiftindex %u", 
				pTrain->GetTrainID(), pTrain->GetTrainIndex(),  pTrain->GetShiftTrainIndex());
		}
	}
	if(trainindex_num>1)
	{
		bFind=true;
		
		CString str;
		str.Format("��̨�� %s ���� %d ��%s �޷�����̨���� %s(%d)ƥ��,���˹���������", 
				gpcdef.GetStationName(prec->GetStationNo()),
				trainindex_num, 
				strtt,
				neibtrain.GetTrainID(),
				neibtrain.GetTrainIndex());

		m_warning.Add(str);
		GpcLogTrainMessage(0,0,"%s", str);
	}
	
	if(trainindex_num==1)
	{
		return m_cWorkSchduleData.GetSingleTrain(train_index[0]);
	}
	
	return NULL;
}

void CWorkScheduleHolder::ReqBasicScheduleName()
{
	int nSectionList[64];
	int nSectionNum=sizeof(nSectionList)/sizeof(nSectionList[0]);
	nSectionNum=gpcdef.GetSectionList(nSectionNum, nSectionList);
	for(int ns=0; ns<nSectionNum; ns++)
	{
		ReqBasicName_ToServer(nSectionList[ns], true);
	}
}

BOOL  CWorkScheduleHolder::GenTrain_OPMS(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL newflag)
{
	CTG_TrainScheduleBase *pCurData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pCurData=&m_cWorkSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pCurData=&m_cShiftSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
    CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrain_OPMS(&srctrain,newflag))
		return FALSE;

	// �����г���Դ��shift
    srctrain.SetFromShift(TRUE);

	adjust_logic.CacuTrainSlowTime(&srctrain); // 20100813 ����ϻ�����
	return TRUE;
}

void CWorkScheduleHolder::GetNeibPlanEqualStation(int nThisSection, int nNeibSection)
{
	mNeibAllStationCount=GetTGFunctionConfig()->GetNeibStationList(nThisSection, nNeibSection, m_ns, 20);

	GpcLogTrainMessage(0,0,"��̨ %d ��̨ %d, [NEIBPLAN6]����=%d,��������:", 
						nThisSection, nNeibSection, mNeibAllStationCount);

	for(int n=0;n<mNeibAllStationCount;n++)
	{
		GpcLogTrainMessage(0,0,"%d=%d, %d, %d, %d, %d, %d/%d", n, nThisSection, nNeibSection,
			m_ns[n].n_PreStation, m_ns[n].station, m_ns[n].n_NextStation, m_ns[n].entry, m_ns[n].exit);
	}

	mNeibPlanCount=GetTGFunctionConfig()->GetNeibEqualStation(nThisSection, nNeibSection, 
			mNeibPlanStation, NEIBPLAN_ADJUST_NUM);

	for(int i=mNeibPlanCount; i<NEIBPLAN_ADJUST_NUM; i++)
	{
		mNeibPlanStation[i].Init();
	}

	GpcLogTrainMessage(0,0,"[BEGIN]��̨ %d ��̨ %d, [NEIBPLAN5] %d,��������:", 
							nThisSection, nNeibSection, mNeibPlanCount);
	for(int i=0; i<NEIBPLAN_ADJUST_NUM; i++)
	{
		NeibAdjustExtern x=mNeibPlanStation[i];
		if(x.nThisSection==0 || x.nNeibSection==0)
		{
			continue;
		}

		CString str;
		str.Format(", %d",x.exit_count);
		for(int n=0;n<x.exit_count;n++)
		{
			CString tmp;
			tmp.Format("/%d", x.exit[n]);
			str+=tmp;
		}
		for(int n=0;n<x.neib_count;n++)
		{
			CString tmp;
			tmp.Format(", %d*%d*%d", x.neib_station[n], x.passway_list[n], x.index_list[n]);
			str+=tmp;
		}
		GpcLogTrainMessage(0,5, "%d=%d, %d, %d*%d*%d*%d  %s", 
			i,x.nThisSection, x.nNeibSection,
			x.nStation, x.nNextStation, x.nPassway, x.nNodeIndex, 
			str);
	}
}

bool CWorkScheduleHolder::UpdateOrAddTrainForNeibPlan6(CTG_TrainScheduleBase* pData, int *add_or_update, CTG_SingleTrain& newtrain,
										   const CTG_SingleTrain &neibtrain, int rec_index, int index, CStringArray& m_warning)
{
	const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByRecIndex_Const(rec_index);
	if(!prec)
		return false; // ������

	CTG_ScheduleInfo schdinfo;
	m_cWorkSchduleData.GetScheduleInfo(schdinfo); 
	// ����ʹ��shiftindex ����
	CTG_SingleTrain *prev=GetTrainByNeibTrainShiftID(neibtrain, prec);
	
	// �����ճ��κŲ���
	if(!prev)  
	{
		bool bFind=false;
		prev=GetTrainByNeibTrainID(neibtrain, prec, pData, m_warning, bFind);
		if(bFind)
		{
			return false;
		}
	}

	if(prev)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, �ҵ���̨�г� trainid=%s,index=%u, shiftindex=%u", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(), prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());

		if(!prev->MatchTrainID(prec->GetArriveTID()) && !prev->MatchTrainID(prec->GetDepartTID()))  // �ҵ����г����복��һ��
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s ����̨���� arrive_id=%s depart_id=%s��ƥ��", 
				prev->GetTrainID(), prec->GetArriveTID(), prec->GetDepartTID());
			prev=NULL;
		}
	}
	newtrain.m_nCurrConTrainIndex = 0;
	if(prev && prev->GetTrainRecordCount()>0)
	{
		*add_or_update = 1;
		// �����Ѿ����ڵ��г���ֻҪƽ��ʱ��,
		newtrain=*prev;

		// ��̨�ƻ�,��̨�г����ҳ�վӦ�ôӺ���ǰ��,�ظ���վ����
		CTG_TrainRecord *ptrain_record= newtrain.GetTrainRecordByStation_Reverse(prec->GetStationNo());
		if(ptrain_record==NULL)
			return false;
		int this_index =ptrain_record->GetIndex();

		time_t next_time = prec->GetDepartTime();
		time_t ltime = ptrain_record->GetDepartTime(); 
		long time_off = next_time - ltime;
		if (abs(time_off) > 8*3600)
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ʱ����8Сʱ����ƽ��", prev->GetTrainID());
			return false;
		}

		if(index>=0 && index<mNeibAllStationCount)
		{
			CTG_TrainRecord *px1=newtrain.GetTrainRecordByRecIndex(this_index-1);
			CTG_TrainRecord *px2=newtrain.GetTrainRecordByRecIndex(this_index+1);
			int station1=0, station2=0;
			if(IsMatchConfigure(px1, px2, index, false, station1, station2))
			{
				const CTG_TrainRecord* precord1=neibtrain.GetTrainRecordByRecIndex_Const(rec_index-1);
				const CTG_TrainRecord* precord2=neibtrain.GetTrainRecordByRecIndex_Const(rec_index+1);
				if(px1 && precord1)
				{
					if((px1->m_data.station==precord1->m_data.station)&&(px1->m_data.station==station1))
					{
						px1->m_data.arrive = precord1->GetArriveTime();
						px1->m_data.depart = precord1->GetDepartTime();
					}
				}
				int station=ptrain_record->m_data.station;
				int time_off = prec->GetDepartTime() - ptrain_record->GetDepartTime();
				int off_rec_index=this_index;
				ptrain_record->m_data.arrive = prec->GetArriveTime();
				ptrain_record->m_data.depart = prec->GetDepartTime(); 
				if(px2 && precord2)
				{
					if((px2->m_data.station==precord2->m_data.station)&&(px2->m_data.station==station2))
					{
						time_off = precord2->GetDepartTime() - px2->GetDepartTime();
						off_rec_index=this_index+1;
						px2->m_data.arrive = precord2->GetArriveTime();
						px2->m_data.depart = precord2->GetDepartTime(); 
						station=px2->m_data.station;
					}
				}

				newtrain.OffsetScheduleTime(off_rec_index+1, time_off, FALSE, 1);
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ��վ %d �����ƽ��ʱ��%d", 
									prev->GetTrainID(), station, time_off);
			
				// ��̨���ݻ�����
				if(GetTGFunctionConfig()->IsGetEngineFromPreConID())
				{
					PassTrainEngineNo(newtrain, &neibtrain);
				}
				//////////////////////////////////
				newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
				newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
				return true;
			}
		}
	}
	else
	{
		if(index>=0 && index<mNeibAllStationCount)
		{
			int station1=m_ns[index].n_PreStation; // pre_station ����Ϊ0,��ʾ������,���������
			int station2=abs(m_ns[index].n_NextStation); // next_station ����Ϊ����,��ʾ������,���������
			if(0>=station1)
			{
				return false;
			}
						
			*add_or_update = 0;

			newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
			newtrain.GetRecordList()->PurgeRecords(); 

			newtrain.m_strSource="������̨";
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.SetTrainRecordCountToDefault();
			
			const CTG_TrainRecord* precord1=neibtrain.GetTrainRecordByRecIndex_Const(rec_index-1);
			const CTG_TrainRecord* precord2=neibtrain.GetTrainRecordByRecIndex_Const(rec_index+1);
			if(precord1 && (precord1->m_data.station==station1))
			{
				CTG_TrainRecord train_record=*precord1;
				train_record.SetPlanSendTime(0); //��Ҫ! 
				newtrain.GetRecordList()->AddRecord(train_record);
			}
			CTG_TrainRecord train_record=*prec;
			if(m_ns[index].entry>0)
				train_record.m_data.entry=m_ns[index].entry;
			if(m_ns[index].exit>0)
				train_record.m_data.exit=m_ns[index].exit;
			train_record.SetArriveTGIndex(0);
			train_record.SetDepartTGIndex(0);
			train_record.SetPlanSendTime(0); //��Ҫ! 
			newtrain.GetRecordList()->AddRecord(train_record);

			if(precord2 && (precord2->m_data.station==station2))
			{
				CTG_TrainRecord train_record=*precord2;
				train_record.SetPlanSendTime(0); //��Ҫ! 
				newtrain.GetRecordList()->AddRecord(train_record);
			}
			newtrain.SetTrainRecordCountToDefault(); 
			
			// ��̨���ݻ�����
			PassTrainEngineNo(newtrain, &neibtrain);
			return true;
		}
	}
	return false;
}

bool CWorkScheduleHolder::IsMatchConfigure(const CTG_TrainRecord *prec_before, const CTG_TrainRecord *prec_next, int index, bool bFlag, int& station1, int& station2)
{
	bool bResult=false;
	station1=m_ns[index].n_PreStation;
	if(bFlag)
		station2=abs(m_ns[index].n_NextStation);
	else
		station2=m_ns[index].n_NextStation;

	if(m_ns[index].n_PreStation>0)
	{
		if(prec_before==NULL)
			return false;
		else
		{
			if(prec_before->m_data.station!=m_ns[index].n_PreStation)
			{
				return false;
			}
			if(m_ns[index].n_NextStation>0)
			{
				if(prec_next==NULL)
					return false;
				else
				{
					if(prec_next->m_data.station==m_ns[index].n_NextStation)
					{
						return true;
					}
				}
			}
			else
			{
				return true;
			}
		}
	}
	else
	{
		if(m_ns[index].n_NextStation>0)
		{
			if(prec_next==NULL)
				return false;
			else
			{
				if(prec_next->m_data.station==m_ns[index].n_NextStation)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// [NEIBPLAN6]
BOOL CWorkScheduleHolder::RecvNeibPlanAll(CTG_TrainScheduleBase* pData, 
													   int *add_or_update, 
													   CTG_SingleTrain &newtrain, 
													   const CTG_SingleTrain &neibtrain, 
													   vector<CTG_SingleTrain>& othertrain_list,
													   CStringArray& m_warning)
{
	if(mNeibAllStationCount<=0)
		return FALSE;
	
	BOOL bAdjust=FALSE;
	int  idx=-1;
	for(int n=0; n<neibtrain.GetTrainRecordCount();n++)
	{
		const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			break;
		
		for(int i=0; i<mNeibAllStationCount; i++)
		{
			if(prec->m_data.station==m_ns[i].station)
			{
				const CTG_TrainRecord *prec_before=neibtrain.GetTrainRecordByRecIndex_Const(n-1);
				const CTG_TrainRecord *prec_next=neibtrain.GetTrainRecordByRecIndex_Const(n+1);
				int station1, station2;
				if(IsMatchConfigure(prec_before, prec_next, i, true, station1, station2))
				{
					if(UpdateOrAddTrainForNeibPlan6(pData,add_or_update,newtrain,neibtrain,n,i,m_warning))
					{
						bAdjust=TRUE;
						break;
					}
				}
			}
		}
		if(bAdjust)
		{
			break;
		}
	}

	for(int m=n+2; m<neibtrain.GetTrainRecordCount();m++)
	{
		const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByRecIndex_Const(m);
		if(prec==NULL)
			break;
		
		for(int i=0; i<mNeibAllStationCount; i++)
		{
			if(prec->m_data.station==m_ns[i].station)
			{
				const CTG_TrainRecord *prec_before=neibtrain.GetTrainRecordByRecIndex_Const(m-1);
				const CTG_TrainRecord *prec_next=neibtrain.GetTrainRecordByRecIndex_Const(m+1);
				int station1, station2;
				if(IsMatchConfigure(prec_before, prec_next, i, true, station1, station2))
				{
					CTG_SingleTrain srctrain;
					int add_update=0;
					if(UpdateOrAddTrainForNeibPlan6(pData,&add_update,srctrain,neibtrain,m,i,m_warning))
					{
						srctrain.m_nAddOrUpdate=add_update;
						othertrain_list.push_back(srctrain);
						break;
					}
				}
			}
		}
	}

	return bAdjust;
}

BOOL CWorkScheduleHolder::UpdateOrAddTrain(CTG_TrainScheduleBase* pData, int *add_or_update, CTG_SingleTrain& newtrain,
										   const CTG_SingleTrain &neibtrain, const CTG_TrainRecord *prec, int passway,
										   int nodeindex, CStringArray& m_warning)
{
	CTG_ScheduleInfo schdinfo;
	m_cWorkSchduleData.GetScheduleInfo(schdinfo); 
	// ����ʹ��shiftindex ����
	CTG_SingleTrain *prev=GetTrainByNeibTrainShiftID(neibtrain, prec);
	
	// �����ճ��κŲ���
	if(!prev)  
	{
		bool bFind=false;
		prev=GetTrainByNeibTrainID(neibtrain, prec, pData, m_warning, bFind);
		if(bFind)
		{
			return FALSE;
		}
	}

	if(prev)
	{
		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s,index=%u, shiftindex=%u, �ҵ���̨�г� trainid=%s,index=%u, shiftindex=%u", 
			neibtrain.GetTrainID(), neibtrain.GetTrainIndex(), neibtrain.GetShiftTrainIndex(), prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());

		if(!prev->MatchTrainID(prec->GetArriveTID()) && !prev->MatchTrainID(prec->GetDepartTID()))  // �ҵ����г����복��һ��
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s ����̨���� arrive_id=%s depart_id=%s��ƥ��", 
				prev->GetTrainID(), prec->GetArriveTID(), prec->GetDepartTID());
			prev=NULL;
		}
	}
	newtrain.m_nCurrConTrainIndex = 0;
	if(prev && prev->GetTrainRecordCount()>0)
	{
		*add_or_update = 1;
		// �����Ѿ����ڵ��г���ֻҪƽ��ʱ��,
		newtrain=*prev;

		CTG_TrainRecord *pfirst_newtrain_record=newtrain.GetTrainRecord(0);

		// ����ֽ�վ�ǵ�һ��վ������ͼʹ�õ�һ��վ��Ϊƽ��
		if(pfirst_newtrain_record->GetStationNo()!=prec->GetStationNo())
		{
             const  CTG_TrainRecord *ptmp_record=neibtrain.GetTrainRecordByStation_Const(pfirst_newtrain_record->GetStationNo());
			 if(ptmp_record)
			 {
				 prec=ptmp_record;
				 GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ��һ��վ�Ƿֽ�վ,����ͼʹ�õ�һ��վ%d��Ϊƽ��", 
					 prev->GetTrainID(), ptmp_record->GetStationNo());
			 }
		}

		// ��̨�ƻ�,��̨�г����ҳ�վӦ�ô�ǰ�����,�ظ���վ����
		CTG_TrainRecord *ptrain_record= newtrain.GetTrainRecordByStation(prec->GetStationNo());
		
		short this_index =ptrain_record->GetIndex();
		//for(int x=this_index; x<newtrain.GetTrainRecordCount(); x++)
		//{
		//	CTG_TrainRecord *ptmprec=newtrain.GetTrainRecord(x);
		//	if(TG_IsArriveActual(ptmprec->m_data) || TG_IsDepartActual(ptmprec->m_data))
		//	{
		//		GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ����ʵ�ʵ㲻��ƽ��", prev->GetTrainID());
		//		return FALSE;
		//	}
		//}
		time_t next_time = prec->GetDepartTime();
		time_t ltime = ptrain_record->GetDepartTime(); 
		long time_off = next_time - ltime;
		if (abs(time_off) > 8*3600)
		{
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ʱ����8Сʱ����ƽ��", prev->GetTrainID());
			return FALSE;
		}

		// give fjk station not this section, recieve neib side and time
		if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  ptrain_record->m_data.station))
		{
			GpcLogTrainMessage(0,0,"�г�trainid=%s,����̨�䳵վ%d�����ڱ�̨%d��Ͻ, ������̨�ɵ���������ϵ����ҵʱ��",  
					newtrain.GetTrainID(), ptrain_record->m_data.station, schdinfo.m_nSectionID);

			ptrain_record->m_data.arrive_side = prec->m_data.arrive_side;
			ptrain_record->m_data.depart_side = prec->m_data.depart_side;
			ptrain_record->m_data.min_stop_time = prec->m_data.min_stop_time;

			time_off = prec->GetDepartTime() - ptrain_record->GetDepartTime();

			ptrain_record->m_data.arrive = prec->GetArriveTime();
			ptrain_record->m_data.depart = prec->GetDepartTime(); 
			newtrain.OffsetScheduleTime(this_index+1, time_off, FALSE, 1);
			GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ��վ %d �����ƽ��ʱ��%d", 
					prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
		}
		else
		{
			if(newtrain.IsPassengerTrain() || GetTGFunctionConfig()->IsEnableNeibPlanFreightSameToPassenger())
			{
				time_off = prec->GetArriveTime() - ptrain_record->GetArriveTime();
				newtrain.OffsetScheduleTime(this_index, time_off, FALSE, 1);
				GpcLogTrainMessage(0,0,"��̨�г� trainid=%s �ǿͳ�, ������̨����,��վ %d �����ƽ��ʱ��%d", 
							prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
			}
			else
			{
				short stop_pass_change = 0;
				if (time_off<0 && ptrain_record->IsPassRec()) 
					stop_pass_change = -1;  // pass to stop			

				if (stop_pass_change)
				{
					ptrain_record->m_data.arrive_side = 255;
					ptrain_record->m_data.depart_side = 255;
					CMMIOperFlag oper_flag;
					oper_flag.m_nRecIndex=this_index;
					if(stop_pass_change==1)
						oper_flag.nFlag=CMMIOperFlag::STAY_TO_PASS;
					else
						oper_flag.nFlag=CMMIOperFlag::PASS_TO_STAY;
					GenTrain_AutoAdjustManualUpdate(newtrain,schdinfo,oper_flag);
				}
				time_off = prec->GetDepartTime() - ptrain_record->GetDepartTime();

				if(time_off<0)
				{
					if(this_index==0 && ptrain_record->IsStartRec())
						ptrain_record->m_data.depart = prec->GetDepartTime();
					ptrain_record->m_data.arrive = prec->GetArriveTime();
				}
				else
				{
					newtrain.OffsetScheduleTime(this_index, time_off, FALSE, 1);
					GpcLogTrainMessage(0,0,"��̨�г� trainid=%s, ����վ %d ����ƽ��ʱ��%d", 
							prev->GetTrainID(), ptrain_record->GetStationNo(), time_off);
				}
			}
		}

		// ��̨���ݻ�����
		if(GetTGFunctionConfig()->IsGetEngineFromPreConID())
		{
			PassTrainEngineNo(newtrain, &neibtrain);
		}
		//////////////////////////////////
		newtrain.m_nCurrConTrainIndex = prev->GetTrainIndex();
		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
	}
	//����֧����ϲ��� ������ 2011927
	else if(GetTGFunctionConfig()->IsNeibPlanPassengerTrainAddNotByBasic())
	{
		// ���ڿͳ�, �հ�ƻ��ӳ�->����ӳ�
		// ���ڻ���, ����ͼ�ӳ�->��·�ӳ�->����ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 
		
		// �����հ�ƻ��в���
		bool bFind=false;
		int offset=0;
		DWORD train_shift_index=neibtrain.GetShiftTrainIndex();
		if(train_shift_index>0)
		{
			CTG_SingleTrain* pshifttrain=m_cShiftSchduleData.GetSingleTrainByShiftIndex(neibtrain.GetShiftTrainIndex(), prec->GetStationNo());
			if(NULL != pshifttrain)
			{
				newtrain=*pshifttrain;
				newtrain.SetTrainRecordCountToDefault();
				GpcLogTrainMessage(0,0,"���� shift_index %u �ڱ�̨�հ�ƻ����ҵ� trainid=%s", 
														train_shift_index, neibtrain.GetTrainID());

				// ������̨�ֽ�վ��Ϊ��̨��վ���
				CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
				if(pfirst_rec)
				{
					if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  pfirst_rec->m_data.station))
					{
						offset=prec->m_data.depart-pfirst_rec->m_data.depart;
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						pfirst_rec->m_data.min_stop_time = prec->m_data.min_stop_time;
						pfirst_rec->m_data.arrive_side = prec->m_data.arrive_side;
						pfirst_rec->m_data.depart_side = prec->m_data.depart_side; 
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);

						GpcLogTrainMessage(0,0,"�г�trainid=%s,����̨�䳵վ%d�����ڱ�̨%d��Ͻ, ������̨�ɵ���������ϵ����ҵʱ��",  
								newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
					}
					else
					{
						GpcLogTrainMessage(0,0,"�г�trainid=%s, ̨�䳵վ%d �Ƿ�Ϊ��̨%d��Ͻδ֪",  
								newtrain.GetTrainID(), pfirst_rec->m_data.station, schdinfo.m_nSectionID);
						if(newtrain.IsPassengerTrain()) 
						{
							offset=prec->m_data.arrive-pfirst_rec->m_data.arrive;
							pfirst_rec->m_data.arrive= prec->m_data.arrive;
							
							newtrain.OffsetScheduleTime(pfirst_rec->GetIndex(),offset,FALSE,0);

							GpcLogTrainMessage(0,0,"��̨�����հ�ӳ�trainid=%s, shiftindex %u, %dվά�ֱ�̨�ɵ��͵�����ϵ,ƽ��ʱ��%d", 
								newtrain.GetTrainID(), newtrain.GetShiftTrainIndex(), pfirst_rec->GetStationNo(), offset);
						}
						else
						{
							offset=prec->m_data.depart-pfirst_rec->m_data.depart;

							pfirst_rec->m_data.depart= prec->m_data.depart;
							pfirst_rec->m_data.arrive= prec->m_data.arrive;
							newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						}
					}
					newtrain.m_strSource="��ƻ�";
					bFind=true;
				}
			}
			else
			{
				GpcLogTrainMessage(0,0,"���� shift_index %d �ڱ�̨�հ�ƻ����Ҳ��� trainid=%s", 
														train_shift_index, neibtrain.GetTrainID());
			}
		}
	
		if(bFind)
		{
			GpcLogTrainMessage(0,0,"���� shift_index %u, �ڱ�̨�հ�ƻ����ҵ� trainid=%s, �����հ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
						train_shift_index, neibtrain.GetTrainID(), prec->GetStationNo(), offset);
		}
		else if(neibtrain.IsPassengerTrain())
		{
			newtrain.m_strSource="��ʱ����";
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetRecordList()->PurgeRecords(); 
			
			if(!prec)
				return FALSE; // ������
			CTG_TrainRecord train_record=*prec;
			train_record.rec_index=0; 
			TG_SetFlagAccept(train_record.m_data.flag);
			TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
			train_record.SetArriveTGIndex(nodeindex);
			train_record.SetDepartTGIndex(nodeindex);
			train_record.SetPlanSendTime(0); //��Ҫ! 
			newtrain.GetRecordList()->AddRecord(train_record);

			GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
				neibtrain.GetTrainID(), nodeindex);
			
			newtrain.SetTrainRecordCountToDefault(); 
		}
		else
		{
			if(GenTrain_BasicSchedule(newtrain,schdinfo,prec->GetDepartTID(),prec->GetStationNo(),NULL_TIME,NULL_TIME))
			{
				newtrain.m_strSource="����ͼ";

				TRAIN_INDEX basic = newtrain.GetBasicTrainIndex();
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.GetTrainInfo()->m_data.basic_train_index = basic;
				newtrain.SetTrainRecordCountToDefault();

				//�޸����� CUIHU  2009.11.20 ������̨�ֽ�վ��Ϊ��̨��վ���
				CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
				if(pfirst_rec)
				{
					int offset=prec->m_data.depart-pfirst_rec->m_data.depart;
					if(GetTGFunctionConfig()->IsTaiYuanNeigbourPlan())
					{
						newtrain.OffsetScheduleTime(0,offset,FALSE,1);//̫ԭ20101018�����̨�����������
						GpcLogTrainMessage(0,0,"̫ԭģʽ,��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ����ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), offset);
					}
					else
					{
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						
						GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ��վ %d �����ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), offset);
					}
				}
			}
			else
			{
				// �������ջ���ͼ�ӳ�ʧ���� ���վ�·�ӳ�
				if(!neibtrain.IsPassengerTrain() && passway>0 && GenTrain_Passway(newtrain,schdinfo,prec->GetDepartTID(),passway,0,0,prec->GetStationNo(),prec->GetDepartTime(),255))
				{
					newtrain.m_strSource="Ĭ�Ͼ�·";
					*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
					RemoveTrainAbsTimeType(&newtrain);
					newtrain.SetTrainRecordCountToDefault();

					GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���վ�· %d �ӳ�", 
							neibtrain.GetTrainID(), passway);
				}
				// ���㳵
				else
				{
					newtrain.m_strSource="��ʱ����";
					*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
					RemoveTrainAbsTimeType(&newtrain);
					newtrain.GetRecordList()->PurgeRecords(); 
					
					if(!prec)
						return FALSE; // ������
					CTG_TrainRecord train_record=*prec;
					train_record.rec_index=0; 
					TG_SetFlagAccept(train_record.m_data.flag);
					TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
					train_record.SetArriveTGIndex(nodeindex);
					train_record.SetDepartTGIndex(nodeindex);
					train_record.SetPlanSendTime(0); //��Ҫ! 
					newtrain.GetRecordList()->AddRecord(train_record);

					GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
						neibtrain.GetTrainID(), nodeindex);
					
					newtrain.SetTrainRecordCountToDefault(); 
				}
			}
		}
		
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
	}
	else
	{
		// �����������Σ�1.���� traininfo, ��deleteflag��SetTrainRecordCountToDefault��������
		// 2.����ǣ������
		// ���ڿͳ�������ͼ�ӳ�->����ӳ�
		// ���ڻ���, ��·�ӳ�->����ӳ�
		*add_or_update = 0;

		newtrain.SetTrainIndex(neibtrain.GetTrainIndex());
		newtrain.GetRecordList()->PurgeRecords(); 

		// �ͳ�--ֻ�ܰ��ջ���ͼ�ӳ�
		BOOL IsCanGenNei = TRUE;//YUZHU BEGIN
		if(GetTGFunctionConfig()->NeiConsiderPassenger())
		{
			if(neibtrain.IsPassengerTrain())
				IsCanGenNei = TRUE;
			else
				IsCanGenNei=FALSE;
		}//YUZHU END

		GpcLogTrainMessage(0,0,"��̨�ƻ� trainid=%s, �Ƿ��ջ���ͼ�ӳ���־Ϊ%d(1:���ջ���ͼ�ӳ� 0:�����ջ���ͼ�ӳ�)", 
						neibtrain.GetTrainID(), IsCanGenNei);

		if(IsCanGenNei && GenTrain_BasicSchedule(newtrain,schdinfo,prec->GetDepartTID(),prec->GetStationNo(),NULL_TIME,NULL_TIME))
		{
			newtrain.m_strSource="����ͼ";
			GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�",  neibtrain.GetTrainID());

			TRAIN_INDEX basic = newtrain.GetBasicTrainIndex();
			*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
			RemoveTrainAbsTimeType(&newtrain);
			newtrain.GetTrainInfo()->m_data.basic_train_index = basic;
			newtrain.SetTrainRecordCountToDefault();

			//�޸����� CUIHU  2009.11.20 ������̨�ֽ�վ��Ϊ��̨��վ���
			CTG_TrainRecord *pfirst_rec=newtrain.GetTrainRecordByStation(prec->GetStationNo());
			if(pfirst_rec)
			{
				int offset=prec->m_data.depart-pfirst_rec->m_data.depart;
				if(GetTGFunctionConfig()->IsTaiYuanNeigbourPlan())
				{
					newtrain.OffsetScheduleTime(0,offset,FALSE,1);//̫ԭ20101018�����̨�����������
					GpcLogTrainMessage(0,0,"̫ԭģʽ,��̨�г��Ҳ��� trainid=%s, ���ջ���ͼ�ӳ�, ����ƽ��ʱ��%d", 
						neibtrain.GetTrainID(), offset);
				}
				else
				{
					if(GetTGFunctionConfig()->IsNotThisSectionStation(schdinfo.m_nSectionID,  pfirst_rec->m_data.station))
					{
						pfirst_rec->m_data.depart= prec->m_data.depart;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						pfirst_rec->m_data.arrive_side= prec->m_data.arrive_side;
						pfirst_rec->m_data.depart_side= prec->m_data.depart_side;

						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex()+1,offset,FALSE,1);
						GpcLogTrainMessage(0,0,"��̨���ջ���ͼ�ӳ�trainid=%s, ��վ%d�����ڵ�������%d��Ͻ, ������̨������͹ɵ�,������������ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), schdinfo.m_nSectionID, offset);
					}
					else
					{
						offset=prec->m_data.arrive-pfirst_rec->m_data.arrive;
						pfirst_rec->m_data.arrive= prec->m_data.arrive;
						
						newtrain.OffsetScheduleTime(pfirst_rec->GetIndex(),offset,FALSE,0);

						GpcLogTrainMessage(0,0,"��̨���ջ���ͼ�ӳ�trainid=%s, ά�ֳ�վ%d�ɵ��͵�����ϵ,ƽ��ʱ��%d", 
							neibtrain.GetTrainID(), pfirst_rec->GetStationNo(), offset);
					}
				}
			}
		}
		else
		{
			// �������ջ���ͼ�ӳ�ʧ���� ���վ�·�ӳ�
			if(!neibtrain.IsPassengerTrain() && passway>0 && GenTrain_Passway(newtrain,schdinfo,prec->GetDepartTID(),passway,0,0,prec->GetStationNo(),prec->GetDepartTime(),255))
			{
				newtrain.m_strSource="Ĭ�Ͼ�·";
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.SetTrainRecordCountToDefault();

				GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, ���վ�· %d �ӳ�", 
						neibtrain.GetTrainID(), passway);
			}
			// ���㳵
			else
			{
				newtrain.m_strSource="��ʱ����";
				*newtrain.GetTrainInfo() =*neibtrain.GetTrainInfo_Const(); // ���� traininfo
				RemoveTrainAbsTimeType(&newtrain);
				newtrain.GetRecordList()->PurgeRecords(); 
				
				if(!prec)
					return FALSE; // ������
				CTG_TrainRecord train_record=*prec;
				train_record.rec_index=0; 
				TG_SetFlagAccept(train_record.m_data.flag);
				TG_SetFlagTerminal(train_record.m_data.flag); // ����ӳ�Ĭ�ϰ����յ�
				train_record.SetArriveTGIndex(nodeindex);
				train_record.SetDepartTGIndex(nodeindex);
				train_record.SetPlanSendTime(0); //��Ҫ! 
				newtrain.GetRecordList()->AddRecord(train_record);

				GpcLogTrainMessage(0,0,"��̨�г��Ҳ��� trainid=%s, �������ӵ����յ���, ��վ���� %d", 
					neibtrain.GetTrainID(), nodeindex);
				
				newtrain.SetTrainRecordCountToDefault(); 
			}
		}
		
		// ��̨���ݻ�����
		PassTrainEngineNo(newtrain, &neibtrain);
	}
	return TRUE;
}

// �ȼ۳�վ�ж� [NEIBPLAN5]
BOOL CWorkScheduleHolder::AdjustNeibPlanByEqualStation(CTG_TrainScheduleBase* pData, 
													   int *add_or_update, 
													   CTG_SingleTrain &newtrain, 
													   const CTG_SingleTrain &neibtrain, 
													   vector<CTG_SingleTrain>& othertrain_list,
													   CStringArray& m_warning)
{
	if(mNeibPlanCount<=0)
		return FALSE;
	
	BOOL bAdjust=FALSE;
	int  idx=-1;
	for(int n=0; n<neibtrain.GetTrainRecordCount();n++)
	{
		int station=0,equal_station=0;
		const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByRecIndex_Const(n);
		const CTG_TrainRecord *prec_next=neibtrain.GetTrainRecordByRecIndex_Const(n+1);
		if(prec==NULL || prec_next==NULL)
			continue;
		for(int p=0; p<mNeibPlanCount; p++)
		{
			if(prec->m_data.station==mNeibPlanStation[p].nStation &&
				prec_next->m_data.station==mNeibPlanStation[p].nNextStation)
			{
				bool bMeetExit=false;
				if(mNeibPlanStation[p].exit_count>0)
				{
					for(int q=0; q<mNeibPlanStation[p].exit_count; q++)
					{
						if(mNeibPlanStation[p].exit[q] == prec->m_data.exit)
						{
							bMeetExit=true;
							break;
						}
					}
				}
				else
				{
					bMeetExit=true; // ���Գ���
				}

				if(bMeetExit)
				{
					idx=p;
					
					int passway=mNeibPlanStation[p].nPassway;
					int nodeindex=mNeibPlanStation[p].nNodeIndex;
					if(UpdateOrAddTrain(pData,add_or_update,newtrain,neibtrain,prec,passway,nodeindex,m_warning))
					{
						bAdjust=TRUE;
						break;
					}
				}
			}
		}
		if(bAdjust==TRUE)
		{
			break;
		}
	}

	int next_idx=0;
	for(int m=n+1; m<neibtrain.GetTrainRecordCount();m++)
	{
		if(next_idx>=mNeibPlanStation[idx].neib_count)
			break;

		const CTG_TrainRecord *prec=neibtrain.GetTrainRecordByRecIndex_Const(m);
		if(prec==NULL)
			continue;

		int equal_station=mNeibPlanStation[idx].neib_station[next_idx];
		if(prec->m_data.station==equal_station)
		{
			int passway=mNeibPlanStation[idx].passway_list[next_idx];
			int nodeindex=mNeibPlanStation[idx].index_list[next_idx];
			CTG_SingleTrain srctrain;
			int add_update=0;
			if(UpdateOrAddTrain(pData,&add_update,srctrain,neibtrain,prec,passway,nodeindex,m_warning))
			{
				srctrain.m_nAddOrUpdate=add_update;
				othertrain_list.push_back(srctrain);

				next_idx++;
			}
		}
	}
	
	return bAdjust;
}

BOOL CWorkScheduleHolder::GenTrain_OPMSx(CTG_SingleTrain &srctrain,std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd,BOOL newflag,bool& bOldSplitTrain)
{
	train_list.clear();
	int station_list[NEW_SPLIT_ST_NUM], entry_list[NEW_SPLIT_ST_NUM];
	int nStationCount=GetTGFunctionConfig()->GetNewSplitStation(schd.m_nSectionID, station_list, entry_list, NEW_SPLIT_ST_NUM);
	if(nStationCount<=0)
	{
		CTG_SingleTrain othertrain;
		if(FALSE==GenTrain_OPMS(srctrain, othertrain, schd, newflag))
		{
			return FALSE;
		}
		if(othertrain.GetTrainRecordCount()>0)
		{
			train_list.push_back(othertrain);
		}
		bOldSplitTrain=true;  // ��ģʽ����Ϊ[TD_SPLIT]��Ч���г��и�ģʽ
	}
	else
	{
		bOldSplitTrain=false;  // ��ģʽ����Ϊ������ʹ�õ�[TD_SPLIT_EXTERN]��Ч���г��и�ģʽ
		CTG_SingleTrain othertrain;
		CTG_TrainScheduleBase *pCurData;
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			pCurData=&m_cWorkSchduleData;
		else
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			pCurData=&m_cShiftSchduleData;
		else
			return FALSE;
	
		CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),&m_cBasicSchduleData,pCurData);
		CTGAddTrainLogic  logic(&m_cBasicSchduleData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
		
		vector<CTG_SingleTrain> list;
		srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());
		TRAIN_INDEX shift_index=srctrain.GetShiftTrainIndex();
		srctrain.SetFromShift(TRUE);
		CTG_SingleTrain newtrain=srctrain;
		// һ�ηָ�
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,SHIFT,-1,-1,-1,-1);
					srctrain=newtrain;
					srctrain.SetFromShift(TRUE);

					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=srctrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					srctrain.SetShiftTrainIndex(shift_index); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());
					
					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetFromShift(TRUE);
					othertrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		// ���ηָ�
		newtrain=othertrain;
		newtrain.SetFromShift(TRUE);
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,SHIFT,-1,-1,-1,-1);
					
					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=newtrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					newtrain.SetFromShift(TRUE);
					newtrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					newtrain.SetTrainIndex(newtrain.GetShiftTrainIndex());
					list.push_back(newtrain);

					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetFromShift(TRUE);
					othertrain.SetShiftTrainIndex(shift_index | 0x80000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		if(othertrain.GetTrainRecordCount()>0)
		{
			othertrain.SetFromShift(TRUE);
			list.push_back(othertrain);
		}

		// ���ڵ�һ��������
		if(!logic.GenTrain_OPMS(&srctrain,newflag))
		{
			return FALSE;
		}
		adjust_logic.CacuTrainSlowTime(&srctrain); // 20100813 ����ϻ�����
		
		int tmarrive=0;
		if(srctrain.IsPassengerTrain()) 
		{
			tmarrive=0;
		}
		else// �����г�,��¼��һ���������һվʱ��
		{
			tmarrive=CalcTrainLastSt_NextStTime(srctrain);
		}

		for(int i=0; i<list.size(); i++)
		{
			CTG_SingleTrain othertrain=list[i];
			if(othertrain.GetTrainRecordCount()>0)
			{
				// ��ͼ������վ����ʱ��Ϊ0�Ļ���
				if(FALSE==srctrain.IsPassengerTrain()) // ��һ���г�Ϊ�����г�
				{
					SetTrainFirstStationZeroTMToArriveTm(othertrain, tmarrive);
				}

				logic.GenTrain_OPMS(&othertrain,newflag);
				adjust_logic.CacuTrainSlowTime(&othertrain); // 20100813 ����ϻ�����
				train_list.push_back(othertrain);
				
				tmarrive=0;
				if(srctrain.IsPassengerTrain()) 
				{
					tmarrive=0;
				}
				else// �����г�,��¼��һ���������һվʱ��
				{
					tmarrive=CalcTrainLastSt_NextStTime(othertrain);
				}
			}
		}
	}

	return TRUE;
}

int CWorkScheduleHolder::CalcTrainLastSt_NextStTime(CTG_SingleTrain &train)
{
	int tmarrive=0;
	// �����г�,��¼��һ���������һվʱ��
	CTG_TrainRecord *pLast=train.GetTrainLastRecord();
	if(pLast)
	{
		tmarrive=pLast->m_data.depart;

		STATION_NO station=pLast->GetStationNo();
		ENTRY_ID entry=pLast->GetEntryNumber(TRAIN_OUT_STATION);
		BYTE  trn_type=train.GetRunType();

		tmarrive +=GetTGBasicParam()->CalculateTrainRunTime(station, entry, TRAIN_OUT_STATION, trn_type);
	}

	return tmarrive;
}

// ��������ʱ������ʱ,��վʱ�䲻��Ϊ��,�ú�����ͼ������վʱ��
void CWorkScheduleHolder::SetTrainFirstStationZeroTMToArriveTm(CTG_SingleTrain &train, int arrivetm)
{
	CTG_TrainRecord *pFirst=train.GetTrainFirstRecord();
	if(pFirst && arrivetm>0)
	{
		if(pFirst->m_data.arrive==0 || pFirst->m_data.depart==0)
		{
			pFirst->m_data.arrive=arrivetm;
			if(pFirst->m_data.min_stop_time>=0)
				pFirst->m_data.depart=pFirst->m_data.arrive+pFirst->m_data.min_stop_time;
			else
				pFirst->m_data.depart=pFirst->m_data.arrive;
		}
	}
}

BOOL CWorkScheduleHolder::GenTrainBasicSchedule_OPMSx(CTG_SingleTrain &srctrain, std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd, bool& bOldSplitTrain)
{
	train_list.clear();
	int station_list[NEW_SPLIT_ST_NUM], entry_list[NEW_SPLIT_ST_NUM];
	int nStationCount=GetTGFunctionConfig()->GetNewSplitStation(schd.m_nSectionID, station_list, entry_list, NEW_SPLIT_ST_NUM);
	if(nStationCount<=0)
	{
		CTG_SingleTrain othertrain;
		if(FALSE==GenTrainBasicSchedule_OPMS(srctrain, othertrain, schd))
		{
			return FALSE;
		}
		
		if(othertrain.GetTrainRecordCount()>0)
		{
			train_list.push_back(othertrain);
		}
		bOldSplitTrain=true;  // ��ģʽ����Ϊ[TD_SPLIT]��Ч���г��и�ģʽ
	}
	else
	{
		bOldSplitTrain=false;  // ��ģʽ����Ϊ������ʹ�õ�[TD_SPLIT_EXTERN]��Ч���г��и�ģʽ
		CTG_TrainScheduleBase *pCurData;
		CTG_TrainScheduleBase *pBasicData;
		pBasicData=&m_cBasicSchduleData;
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			pCurData=&m_cBasicSchduleData;
			pBasicData=NULL;
		}
		else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			pCurData=&m_cBasicBackupSchduleData;
			pBasicData=NULL;
		}
		else
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			pCurData=&m_cBasicPrevSchduleData;
		else
			return FALSE;
		
		CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
		CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
		if(!logic.GenTrainBasic_OPMS(&srctrain))
			return FALSE;

		srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());
		TRAIN_INDEX shift_index=srctrain.GetShiftTrainIndex();

		CTG_SingleTrain othertrain;
		CTG_SingleTrain newtrain=srctrain;
		// һ�ηָ�
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,BASIC,-1,-1,-1,-1);
					srctrain=newtrain;

					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=srctrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					srctrain.SetShiftTrainIndex(shift_index); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());

					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		// ���ηָ�
		newtrain=othertrain;
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,BASIC,-1,-1,-1,-1);

					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=newtrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					newtrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					newtrain.SetTrainIndex(newtrain.GetShiftTrainIndex());
					train_list.push_back(newtrain);

					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetShiftTrainIndex(shift_index | 0x80000000L); //20141204 ���ηָ� 30bitΪ1,�������ȫ��ID�ָ��ظ�
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		if(othertrain.GetTrainRecordCount()>0)
		{
			train_list.push_back(othertrain);
		}
	}
	return TRUE;
}

// 20171218�˷������ظ�
void  CWorkScheduleHolder::PollingDataForAutoMove()
{
	GpcLogTrainMessage(0, 0, "[PollingDataForAutoMove] ʱ��ƽ�ƴ��� BEGIN");
	if(m_pMasterServerCommWork!=NULL)
	{
		CTG_ScheduleInfo schd_info;
		m_cWorkSchduleData.GetScheduleInfo(schd_info);

		if(schd_info.m_nAutoMove==FALSE)
			return;

		TIME start=0,end=0;
		schd_info.GetViewScopeRange(start,end);

		CTime btm(start), etm(end);
		GpcLogTrainMessage(0, 0, "����ͼ���ӷ�Χ %s -- %s.",
				btm.Format("%Y-%m-%d %H:%M:%S"), etm.Format("%Y-%m-%d %H:%M:%S"));

		// �ƶ�OCX����
		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK))
			{
				if(pcomm->m_bForMMI)
				{
					pcomm->PollingForMoveTGOCX();
				}
			}
		}

		std::vector<TRAIN_INDEX> listTrain, listBlock;

		// delete schd
		m_cWorkSchduleData.GetHistoryData(start, listTrain, listBlock);
		GpcLogTrainMessage(0, 0, "ɾ������ͼ�ؼ�������(%d)��ʩ������(%d)", listTrain.size(), listBlock.size());

		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK))
			{
				if(pcomm->m_bForMMI)
				{
					for(int i=0; i<listTrain.size(); i++)
					{
						m_cWorkSchduleData.DirectDeleteTrain(listTrain[i]);
						pcomm->PollingForDeleteTrain(listTrain[i]);
					}
					for(int i=0; i<listBlock.size(); i++)
					{
						m_cWorkSchduleData.DirectDeleteBlock(listBlock[i]);
						pcomm->PollingForDeleteBlock(listBlock[i]);
					}
				}
			}
		}

		// append schd
		m_cWorkSchduleData.GetNewlyData(end, listTrain, listBlock);
		GpcLogTrainMessage(0, 0, "���ӵ���ͼ�ؼ�������(%d)��ʩ������(%d)", listTrain.size(), listBlock.size());

		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK))
			{
				if(pcomm->m_bForMMI)
				{
					for(int i=0; i<listTrain.size(); i++)
					{
						CTG_SingleTrain* ptrain=m_cWorkSchduleData.GetSingleTrain(listTrain[i]);
						pcomm->PollingForAppendTrain(ptrain);
					}
					for(int i=0; i<listBlock.size(); i++)
					{
						CTG_SingleMark* pblock=m_cWorkSchduleData.GetSingleMark(listBlock[i]);
						pcomm->PollingForAppendBlock(pblock);
					}
				}
			}
		}

	}
	if(m_pMasterServerCommShift!=NULL)
	{
		CTG_ScheduleInfo schd_info;
		m_cShiftSchduleData.GetScheduleInfo(schd_info);

		if(schd_info.m_nAutoMove==FALSE)
			return;

		TIME start=0,end=0;
		schd_info.GetViewScopeRange(start,end);

		CTime btm(start), etm(end);
		GpcLogTrainMessage(0, 0, "�հ�ͼ���ӷ�Χ %s -- %s.",
				btm.Format("%Y-%m-%d %H:%M:%S"), etm.Format("%Y-%m-%d %H:%M:%S"));

		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm)&& (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT))
			{
				if(pcomm->m_bForMMI)
				{
					pcomm->PollingForMoveTGOCX();
				}
			}
		}

		std::vector<TRAIN_INDEX> listTrain, listBlock;

		// delete schd
		m_cShiftSchduleData.GetHistoryData(start, listTrain, listBlock);
		GpcLogTrainMessage(0, 0, "ɾ���հ�ͼ�ؼ�������(%d)��ʩ������(%d)", listTrain.size(), listBlock.size());

		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm)&& (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT))
			{
				if(pcomm->m_bForMMI)
				{
					for(int i=0; i<listTrain.size(); i++)
					{
						m_cShiftSchduleData.DirectDeleteTrain(listTrain[i]);
						pcomm->PollingForDeleteTrain(listTrain[i]);
					}
					for(int i=0; i<listBlock.size(); i++)
					{
						m_cShiftSchduleData.DirectDeleteBlock(listBlock[i]);
						pcomm->PollingForDeleteBlock(listBlock[i]);
					}
				}
			}
		}

		// append schd
		m_cShiftSchduleData.GetNewlyData(end, listTrain, listBlock);
		GpcLogTrainMessage(0, 0, "�����հ�ͼ�ؼ�������(%d)��ʩ������(%d)", listTrain.size(), listBlock.size());

		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT))
			{
				if(pcomm->m_bForMMI)
				{
					for(int i=0; i<listTrain.size(); i++)
					{
						CTG_SingleTrain* ptrain=m_cShiftSchduleData.GetSingleTrain(listTrain[i]);
						pcomm->PollingForAppendTrain(ptrain);
					}
					for(int i=0; i<listBlock.size(); i++)
					{
						CTG_SingleMark* pblock=m_cShiftSchduleData.GetSingleMark(listBlock[i]);
						pcomm->PollingForAppendBlock(pblock);
					}
				}
			}
		}

	}

	GpcLogTrainMessage(0, 0, "[PollingDataForAutoMove] ʱ��ƽ�ƴ��� END");
}

void  CWorkScheduleHolder::PollingForReqData()
{
	if(m_pMasterServerCommWork!=NULL)
	{
		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK))
			{
				if(!pcomm->m_bForMMI) 
				{
					CTG_ScheduleInfo schd_info;
					m_cWorkSchduleData.GetScheduleInfo(schd_info);
					pcomm->PollingForReqData(schd_info);
				}
			}
		}
	}
	if(m_pMasterServerCommShift!=NULL)
	{
		for(int i=0;i<MAX_COMM_INTF_NUM;i++)
		{
			IScheduleComm *pcomm=m_aCommArray[i].pCommInterface;
			if((NULL != pcomm) && (m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT))
			{
				if(!pcomm->m_bForMMI) 
				{
					CTG_ScheduleInfo schd_info;
					m_cShiftSchduleData.GetScheduleInfo(schd_info);
					pcomm->PollingForReqData(schd_info);
				}
			}
		}
	}
}

BOOL  CWorkScheduleHolder::InitScheduleData_NEW(int section,CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	// 1. ����ͼ
	CTG_ScheduleInfo work_schd;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
	{
		work_schd.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_HIST;
	}
	else
	{
		work_schd.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;
	}
	work_schd.m_nSectionID=section;
	m_cWorkSchduleData.SetScheduleInfo(work_schd); 
	m_cWorkSchduleData.DeleteAllData(work_schd,SYCFLAG_NULL);

	// 2.�հ�ͼ
	CTG_ScheduleInfo shift_schd;
	shift_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT; 
	shift_schd.m_nSectionID=section;
	
	m_cShiftSchduleData.SetScheduleInfo(shift_schd); 
	m_cShiftSchduleData.DeleteAllData(shift_schd,SYCFLAG_NULL);

	// 3.����ͼ
	CTG_ScheduleInfo basic_schd;
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	basic_schd.m_nAutoMove=FALSE;
	basic_schd.m_nTotelLen=24;
	basic_schd.m_nHistoryLen=0;
	
	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	m_cBasicSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV; 
	m_cBasicPrevSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicPrevSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP; 
	m_cBasicBackupSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicBackupSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	return TRUE;
}

BOOL  CWorkScheduleHolder::ShiftWorkSchedule_NEW(const CTG_ScheduleInfo &info)
{
	// 0. ɾ����ǰ����
	m_cWorkSchduleData.DeleteDataInMemory(0,info.m_tStartTime,FALSE,TRUE);
	m_cShiftSchduleData.DeleteDataInMemory(0,info.m_tStartTime,FALSE,TRUE);

	// 1. ����ͼ
	CTG_ScheduleInfo prev_info;
	m_cWorkSchduleData.GetScheduleInfo(prev_info);
	
	prev_info.m_nShiftID=info.m_nShiftID;
	prev_info.m_tStartTime=info.m_tStartTime;
	prev_info.m_tEndTime=info.m_tEndTime;
	prev_info.m_nAutoMove=info.m_nAutoMove;
	prev_info.m_nTotelLen=info.m_nTotelLen;
	prev_info.m_nHistoryLen=info.m_nHistoryLen;

	m_cWorkSchduleData.SetScheduleInfo(prev_info);
	
	// 2. �հ�ͼ
	m_cShiftSchduleData.GetScheduleInfo(prev_info);
	prev_info.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	prev_info.m_nShiftID=info.m_nShiftID;
	prev_info.m_tStartTime=info.m_tStartTime;
	prev_info.m_tEndTime=info.m_tEndTime;
	prev_info.m_nAutoMove=info.m_nAutoMove;
	prev_info.m_nTotelLen=info.m_nTotelLen;
	prev_info.m_nHistoryLen=info.m_nHistoryLen;
	
	m_cShiftSchduleData.SetScheduleInfo(prev_info);
	
	GpcLogTrainMessage(0, 0, "[���Ӱ�] WorkSchduleData=%d, ShiftSchduleData=%d BasicSchduleData=%d",
		m_cWorkSchduleData.GetTrainAndMarkCount(),
		m_cShiftSchduleData.GetTrainAndMarkCount(),
		m_cBasicSchduleData.GetTrainAndMarkCount());

	return TRUE;
}

void  CWorkScheduleHolder::PollingAutoMove(TIME curtm)
{
	CTime ctm(curtm);
	
	if(ctm.GetMinute()!=m_nPollingMiniute || ctm.GetHour()!=m_nPollingHour)
	{
		m_nPollingMiniute=ctm.GetMinute();
		PollingDataForAutoMove();
	}
	
	if(ctm.GetHour()!=m_nPollingHour)
	{
		m_nPollingHour=ctm.GetHour();
		PollingForReqData();
	}
}

bool  CWorkScheduleHolder::TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG, int first_entry, int last_exit)
{
	bool bSuccess=false;
	CString newxml=pTG->SendTrainMsgToOCX(train, CTgX::SEND_FLAG::Gen_ExitEntry); // 0615
	if(newxml!="")
	{
		TrainForExitEntry train_tmp;
		if(fromXML(train_tmp,newxml))
		{
			if(SetTrainExitEntry(train, train_tmp))
			{
				GpcLogTrainMessage(0, 0, "�������ó���ڳɹ�");
				bSuccess = true;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "�������ó����ʧ��");
				bSuccess = false;
			}

			if (IS_VALID_ENTRY(first_entry))
			{
				CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
				if(pfirst_rec)
					pfirst_rec->m_data.entry=first_entry;
			}
			if (IS_VALID_ENTRY(last_exit))
			{
				CTG_TrainRecord *plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
				if(plast_rec)
					plast_rec->m_data.exit=last_exit;
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "[Error] OCX���ɳ���ڳɹ�,���ǽ���ʧ��");
			bSuccess = false;
		}
	}
	else
	{
		GpcLogTrainMessage(0, 0, "[Error] OCX���ɳ����ʧ��");
		bSuccess = false;
	}

	return bSuccess;
}

void CWorkScheduleHolder::CheckTrainNo(CTG_SingleTrain& newtrain, CStringArray& train_alarm)
{
	for (int idx =0; idx< newtrain.GetTrainRecordCount(); idx++)
	{
		CTG_TrainRecord *rec = newtrain.GetTrainRecordByRecIndex(idx);
		CTG_TrainRecord *next = newtrain.GetTrainRecordByRecIndex(idx+1);
		if (NULL!=rec && NULL!=next)
		{
			if (strcmp(rec->GetDepartTID(), next->GetArriveTID()))
			{
				CString str1=gpcdef.GetStationName(rec->m_data.station);
				CString str2=gpcdef.GetStationName(next->m_data.station);
				CString temp;
				temp.Format("%s �������� %s �� %s ���ﳵ�� %s ��ͬ,��ȷ��",
					str1, rec->GetDepartTID(), str2, next->GetArriveTID());
				train_alarm.Add(temp);
			}
		}
	}
}

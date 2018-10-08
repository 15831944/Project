// TgDocument.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sysshare.h"
#include "tg.h"
#include "TgDocument.h"
#include "schd_newserver.h"
#include "schd_database.h"
#include "function.h"
#include "icommtask.h"
#include "utility/xml_utility.h"
#include "dlgwait.h"
#include "tgmsg.h"

#define MAX_STORED_DOC_NUM 128

#import  <msxml.dll> named_guids
using namespace MSXML;

static struct
{
	int  nSectionID;
	CTG_ScheduleInfo::SCHEDULE_TYPE  nScheduleType;
	CTgDocument *pTgDoc;
}g_tgdocs[MAX_STORED_DOC_NUM];

extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp


BOOL CTgDocument::InitTGDocuments()
{
	for(int i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		g_tgdocs[i].pTgDoc=NULL;
	}
	return TRUE;
}

CTgDocument *CTgDocument::CreateTgDocument(int section_id)  // STATIC!!
{
	int i;
	CTgDocument *pRet=NULL;
	int  noused=-1;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc!=NULL)
		{
			if(g_tgdocs[i].nSectionID==section_id)
			{
				pRet=g_tgdocs[i].pTgDoc;
				break;
			}
		}

		if(g_tgdocs[i].pTgDoc==NULL && noused==-1)
			noused=i;
	}
	if(pRet)
		return pRet;
	if(noused<0)
		return NULL;
	pRet=new CTgDocument();
	if(!pRet)
		return NULL;

	if(!pRet->InitLogicData(section_id,CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC))
	{
		delete pRet;
		return NULL;
	}
	
	g_tgdocs[noused].pTgDoc=pRet;
	g_tgdocs[noused].nSectionID=section_id;
	g_tgdocs[noused].nScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	return pRet;
}

void CTgDocument::DeleteAllTgDocuments()
{
	for(int i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			delete g_tgdocs[i].pTgDoc;
			g_tgdocs[i].pTgDoc=NULL;
		}
	}
}

IMPLEMENT_DYNCREATE(CTgDocument, CDocument)

CTgDocument::CTgDocument()
{
	m_bAutoDelete=FALSE;

	m_pCurrentWorkScheduleData=NULL;
	m_pWintgNewServerBasic=NULL;
	m_pWintgNewServerBasicPrev=NULL;
	m_pWintgNewServerBasicBackup=NULL;
	m_tmPlanSendRange=3;
	m_nReceiveNeighberLastActivePage=0;
	memset(&m_cPhaseNotes,0,sizeof(m_cPhaseNotes));
	m_pActiveObject=NULL;
	m_nCountOfPlan=0;
	m_cMMplanMarks.clear();
	m_pWaitWnd=NULL;
	m_bQueryMode=TRUE;

	m_nActiveObjectIndex=0;
	m_nActiveObjectType=TG_OBJ_NULL;
	m_nActiveObjectScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL; 
	
	m_nRecvMsgId=0;
	m_nRecvMsgCnt=0;

	for(int i=0;i<MAX_REGISTERED_WND_NUM;i++)
	{
		m_aRegisterdWnd[i].nMsgID=0;
		m_aRegisterdWnd[i].hWnd=NULL;
	}

	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));
}

BOOL CTgDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTgDocument::~CTgDocument()
{
	DeinitData();
}

BEGIN_MESSAGE_MAP(CTgDocument, CDocument)
END_MESSAGE_MAP()


// CTgDocument ���

#ifdef _DEBUG
void CTgDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CTgDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTgDocument ���л�
void CTgDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}
// override
void CTgDocument::OnCloseDocument() 
{
	if(m_bAutoDelete)
		CDocument::OnCloseDocument();
}
BOOL CTgDocument::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class

	//return CDocument::CanCloseFrame(pFrame);
	return TRUE;
}

// CTgDocument ����

BOOL  CTgDocument::InitLogicData(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type) //type
{
	do
	{
		m_pCurrentWorkScheduleData=new CWorkScheduleHolder();
		if(!m_pCurrentWorkScheduleData)
			break;
		
		m_pWintgNewServerBasic=new CWintgNewServer();
		ASSERT(m_pWintgNewServerBasic!=NULL);
		m_pWintgNewServerBasic->Init(section_id,SERVER_TYPE_BASIC_TG);
		m_pWintgNewServerBasic->PollingAndCheckStatus(); //��Ĭ��״̬�û�

		m_pWintgNewServerBasicPrev=new CWintgNewServer();
		ASSERT(m_pWintgNewServerBasicPrev!=NULL);
		m_pWintgNewServerBasicPrev->Init(section_id,SERVER_TYPE_BASIC_TG);
		m_pWintgNewServerBasicPrev->PollingAndCheckStatus(); // �õ�ʱ���ټ��

		m_pWintgNewServerBasicBackup=new CWintgNewServer();
		ASSERT(m_pWintgNewServerBasicBackup!=NULL);
		m_pWintgNewServerBasicBackup->Init(section_id,SERVER_TYPE_BASIC_TG);
		m_pWintgNewServerBasicBackup->PollingAndCheckStatus(); // �õ�ʱ���ټ��

		if(m_pWintgNewServerBasic)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,m_pWintgNewServerBasic); 
			m_pCurrentWorkScheduleData->SetServerCommBasic(m_pWintgNewServerBasic); 
		}
		if(m_pWintgNewServerBasicPrev)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV,m_pWintgNewServerBasicPrev); 
			m_pCurrentWorkScheduleData->SetServerCommBasicPrev(m_pWintgNewServerBasicPrev); 
			m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicPrev,m_pWintgNewServerBasicPrev->CommReady()); 
		}
		if(m_pWintgNewServerBasicBackup)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP,m_pWintgNewServerBasicBackup); 
			m_pCurrentWorkScheduleData->SetServerCommBasicBackup(m_pWintgNewServerBasicBackup); 
			m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicBackup,m_pWintgNewServerBasicBackup->CommReady()); 
		}
	
		/////2012.04.13 nc�����ӷ�ֹʧЧGPC����trainindex�ظ�  hjz
		int tdid = gpcdef.GetTdIdByUserID(gpcdef.GetUserGpcId());
		if(tdid>0)
		{
			m_pCurrentWorkScheduleData->SetUserID(tdid+1);
			int tmp=tdid%64;
			sysprint(5,0,"[USER]=gpc%d �г��������ɺ� :%d ",   gpcdef.GetUserGpcId(), (tmp==0)?64:tmp);
		}
		else
		{
			m_pCurrentWorkScheduleData->SetUserID(gpcdef.GetUserGpcId()+1);
			int tmp=gpcdef.GetUserGpcId()%64;
			sysprint(5,0,"[USER]=gpc%d �г��������ɺ� :%d ",   gpcdef.GetUserGpcId(), (tmp==0)?64:tmp);
		}

		m_pCurrentWorkScheduleData->InitScheduleData(section_id);

		m_nCurrentSectionNo=section_id;
		////////// �йؼƻ��б�
		InitPlanInfo(section_id);
		/////////////////////////
		m_pCurrentWorkScheduleData->SetFileSaveFlag();

		return TRUE;
		// break;
	}while(0);
	return TRUE;
}

BOOL  CTgDocument::DeinitData()
{
	if(m_pWintgNewServerBasic)
	{
		m_pWintgNewServerBasic->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasic); 
		delete m_pWintgNewServerBasic;
	}
	if(m_pWintgNewServerBasicPrev)
	{
		m_pWintgNewServerBasicPrev->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasicPrev); 
		delete m_pWintgNewServerBasicPrev;
	}
	if(m_pWintgNewServerBasicBackup)
	{
		m_pWintgNewServerBasicBackup->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasicBackup); 
		delete m_pWintgNewServerBasicBackup;
	}

	if(m_pCurrentWorkScheduleData)
		delete m_pCurrentWorkScheduleData;
	
	m_pWintgNewServerBasic=NULL;
	m_pWintgNewServerBasicPrev=NULL;
	m_pWintgNewServerBasicBackup=NULL;
	m_pCurrentWorkScheduleData=NULL;
	return TRUE;
}

BOOL  CTgDocument::PollingData(void)
{
	return TRUE;
}

BOOL  CTgDocument::GetScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetBasicScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}

long  CTgDocument::GetCurrentShiftID()
{
	CTG_ScheduleInfo info;
	m_pCurrentWorkScheduleData->GetBasicScheduleInfo(info);
	return info.m_nShiftID;
}

int   CTgDocument::GetCurrentSectionNo()
{
	CTG_ScheduleInfo info;
	m_pCurrentWorkScheduleData->GetBasicScheduleInfo(info);
	return info.m_nSectionID;
}

BOOL  CTgDocument::SetLocalDatabaseMode(BOOL flag)
{
	return TRUE;
}

/////////////////////// ���´�WINTGDOC ��ֲ
BOOL CTgDocument::InitPlanInfo(int section_id)
{
	m_nStplanStationCount=0;
	memset(m_nStplanStation, 0, sizeof(m_nStplanStation));
	
	//��ǰ���ε�ʵ��������б�
	int nSendPlanIndexList[128],nEntityNum;
	nEntityNum=gpcdef.GetSpecialGPCList(section_id, nSendPlanIndexList);
	if (nEntityNum==0)
		nEntityNum = gpcdef.GetSectionPlanReceivers(section_id,nSendPlanIndexList);

	////�·��г��ƻ�������     
	GPCPLANRECEIVER rec;
	for(int k=0;k < nEntityNum;k++)
	{
		memset(&rec, 0x00, sizeof(rec));     // wx@ty
		gpcdef.GetReceiverInfo(nSendPlanIndexList[k],rec);

		for(int i=0; i<m_nStplanStationCount; i++)
		{
			if(m_nStplanStation[i]==rec.nstationNo)
			{
				break;
			}
		}
		if(i>=m_nStplanStationCount)
		{
			m_nStplanStation[m_nStplanStationCount]=rec.nstationNo;
			m_nStplanStationCount++;
		}
	}
	return TRUE;
}

void CTgDocument::ClearPhasePlanInfo()
{
	InitStationPlanInfo(GetCurrentSectionNo());
}

int CTgDocument::InitStationPlanInfo(int section_id)
{
	m_StationPlanInfo_ImportantNew.init();
	GPCENTITY entityInfo;
	GPCPLANRECEIVER rec; CString Entityname;

	int nEntityList[256];
	int nEntityNum = sizeof(nEntityList)/sizeof(nEntityList[0]);
	nEntityNum=gpcdef.GetPhaseNoteReceiverList(section_id,nEntityList,nEntityNum);


	////�·����µ�����
	//�����ε�ʵ������Ŀ

	for(int m=0;m<STAGE_PER_WORK;m++)
	{
		m_StationPlanInfo_ImportantNew.start_st_weather[m] = 0;
		m_StationPlanInfo_ImportantNew.end_st_weather[m] = 0;
	}	    	
	m =0;

	CString strText;
	for(int j= 0;j<nEntityNum;j++)
	{
		gpcdef.GetEntityInfo(nEntityList[j],entityInfo);
		strText.Format("%s",rec.sentity);

		if(entityInfo.nGroupId ==  EQUIP_LIRC)
			continue;
		strText.Format("%s",gpcdef.GetStationName(entityInfo.nSpecial1));//rec.sentity);		 		 
		//վ��
		m_StationPlanInfo_ImportantNew.m_StationPlan[m].nStation = rec.nstationNo;			
		strcpy(m_StationPlanInfo_ImportantNew.m_StationPlan[m].EntityName, strText);//nStationList[k];

		m_StationPlanInfo_ImportantNew.m_StationPlan[m].Entityid = entityInfo.nEntityId;
		m_StationPlanInfo_ImportantNew.m_StationPlan[m].nEntityIndex = nEntityList[j];   // BUG!!! k

		for(int l=0;l<STAGE_PER_WORK;l++)
		{
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].usSendDate = 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].usSendTime= 0;

			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].edit_time = 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].send_time= 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].recv_time= 0;


			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].bReceiveFlag =  CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].bSignInFlag =  CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].strSignName[0] = 0x00;
		}
		m++;	
	}		
	m_StationPlanInfo_ImportantNew.nEntityNum = m;
	
	return TRUE;
}

TIME  CTgDocument::GetCurrentTGDLongTime()
{
	CTime t = CTime::GetCurrentTime( );
	return t.GetTime(); 
}

int CTgDocument::GetCurrentTGTime()
{
	CTime t = CTime::GetCurrentTime( );
	int m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( ))*60 + t.GetSecond( );
	return m_usCurrentTime;
}
int CTgDocument::GetCurrentTGDate()
{
	CTime t = CTime::GetCurrentTime( );
	int m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1)* 31 + t.GetDay( ) - 1;
	return m_usCurrentDate;
}


CString  CTgDocument::IsWaitWindowExist()
{
	CString str="";
	if(m_pWaitWnd)
	{
		if(::IsWindow(m_pWaitWnd->m_hWnd))
		{
			str=((CDlgWait *)m_pWaitWnd)->GetDisplayStr();
		}
	}

	return str;
}

void CTgDocument::OpenWaitWindow(const char *cap, const char *text)
{
	if(cap)
	{
		if(m_pWaitWnd)
		{
			((CDlgWait *)m_pWaitWnd)->UpdateDisplay(cap, text); 
		}
		else
			m_pWaitWnd=new CDlgWait(cap);
	}
	else
	{
		if(m_pWaitWnd)
		{
			delete m_pWaitWnd;
			m_pWaitWnd=NULL;
		}
	}
}

void CTgDocument::SetQueryMode(BOOL va)
{
	m_bQueryMode=va;
	if(m_pCurrentWorkScheduleData)
		m_pCurrentWorkScheduleData->SetQueryMode(va); 
	if(m_pWintgNewServerBasic)
		m_pWintgNewServerBasic->SetQueryMode(va); 
	if(m_pWintgNewServerBasicPrev)
		m_pWintgNewServerBasicPrev->SetQueryMode(va); 
	if(m_pWintgNewServerBasicBackup)
		m_pWintgNewServerBasicBackup->SetQueryMode(va); 
}


CTG_TrainScheduleBase *CTgDocument::GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
		return m_pCurrentWorkScheduleData->GetBasicScheduleData();
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
		return m_pCurrentWorkScheduleData->GetBasicPrevScheduleData();
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP) 
		return m_pCurrentWorkScheduleData->GetBasicBackupScheduleData();
	else
		return NULL;
}

BOOL  CTgDocument::GetBasicScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetBasicScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}
BOOL  CTgDocument::GetBasicPrevScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetBasicPrevScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}

//��ԭϵͳinput���ݵķ�ʽ����



//�ѻ���ͼ��Ϣ��Ϊbsinfo�ļ�
/*
[BASIC_INFO]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID_COUNT=1, ADJUST_FLAG=0, TRAIN_TYPE=  0, RUN_TYPE=1, DIRECTION=1, TRAIN_PRIORITY=1, TRAIN_ID_1=      Z3, ORIG_TRAIN_ID_1=        , BEGIN_STA_1=  0, END_STA_1=  0, TRAIN_ID_2=        , ORIG_TRAIN_ID_2=        , BEGIN_STA_2=  0, END_STA_2=  0, TRAIN_ID_3=        , ORIG_TRAIN_ID_3=        , BEGIN_STA_3=  0, END_STA_3=  0, TRAIN_ID_4=        , ORIG_TRAIN_ID_4=        , BEGIN_STA_4=  0, END_STA_4=  8
[BASIC_INFO_END]
*/

// ��ʱ���δ洢
#define MAX_IDS_NUM_PER_TRAIN  4
struct{
	char  train_id[24];
	short start_index;
	short end_index;
}ol_trainids[MAX_IDS_NUM_PER_TRAIN];
int ol_trainids_count=0;;

static void CacuOlTrainIDs(const CTG_SingleTrain* single)
{
	ol_trainids_count=0;
	for(int i=0;i<single->GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *prec=single->GetTrainRecord_Const(i);
		if(prec)
		{
			if(ol_trainids_count==0)
			{
				strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetArriveTID());
				ol_trainids[ol_trainids_count].start_index=i;
				ol_trainids[ol_trainids_count].end_index=i;
				ol_trainids_count++;

				if (stricmp(prec->GetArriveTID(),prec->GetDepartTID())) // by wuxiang
				{
					strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetDepartTID());
					ol_trainids[ol_trainids_count].start_index=i;
					ol_trainids[ol_trainids_count].end_index=i;
					ol_trainids_count++;
				}
			}
			else if(!stricmp(ol_trainids[ol_trainids_count-1].train_id,prec->GetDepartTID()))
			{
				ol_trainids[ol_trainids_count-1].end_index=i;
			}
			else
			{
				if(ol_trainids_count>=MAX_IDS_NUM_PER_TRAIN)
					return;
				strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetDepartTID());
				ol_trainids[ol_trainids_count].start_index=i;
				ol_trainids[ol_trainids_count].end_index=i;
				ol_trainids_count++;
			}
		}
	}
}

BOOL CTgDocument::olSaveTrainInfo(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * infoName)
{
	CStdioFile infoFile;
	if(!infoFile.Open(infoName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("�����ļ�ʧ��!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("�ڲ����󣺷Ƿ�����ͼ����");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainInfo * traininfo = NULL;
	infoFile.WriteString("[BASIC_INFO]\r\n");
	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		traininfo =&single->GetTrainInfo()->m_data;

		CacuOlTrainIDs(single);

		txt_eachline.Empty();

		//CON_ID= 1, 
		eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
		txt_eachline += eachcol;

		//TRAIN_INDEX=   0, 
		eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
		txt_eachline += eachcol;

		//TRAIN_ID_COUNT=1, 
		eachcol.Format("TRAIN_ID_COUNT=%d, ",ol_trainids_count);
		txt_eachline += eachcol;
		//ADJUST_FLAG=0, 
		eachcol.Format("ADJUST_FLAG=%d, ",traininfo->adjust_flag);
		txt_eachline += eachcol;
		//TRAIN_TYPE=  0, 
		eachcol.Format("TRAIN_TYPE=%3d, ",traininfo->type);
		txt_eachline += eachcol;
		//RUN_TYPE=1, 
		eachcol.Format("RUN_TYPE=%d, ",traininfo->run_type);
		txt_eachline += eachcol;
		//DIRECTION=1, 
		eachcol.Format("DIRECTION=%d, ",single->IsDownDirection()?1:0);
		txt_eachline += eachcol;
		//TRAIN_PRIORITY=1, 
		eachcol.Format("TRAIN_PRIORITY=%d, ",traininfo->priority);
		txt_eachline += eachcol;



		for(int j=0; j<MAX_IDS_NUM_PER_TRAIN;j++)
		{
			if(j>=ol_trainids_count)
			{
				//TRAIN_ID_1=      Z3, 
				eachcol.Format("TRAIN_ID_%d=        , ",j+1);
				txt_eachline += eachcol;
				//ORIG_TRAIN_ID_1=        , 
				eachcol.Format("ORIG_TRAIN_ID_%d=        , ",j+1);
				txt_eachline += eachcol;
				//BEGIN_STA_1=  0, 
				eachcol.Format("BEGIN_STA_%d=   , ",j+1);
				txt_eachline += eachcol;
				//END_STA_1=  0, 
				eachcol.Format("END_STA_%d=   , ",j+1);					
				txt_eachline += eachcol;			

			}
			else
			{
				//TRAIN_ID_1=      Z3, 
				eachcol.Format("TRAIN_ID_%d=%8s, ",j+1,ol_trainids[j].train_id);
				txt_eachline += eachcol;
				//ORIG_TRAIN_ID_1=        , 
				eachcol.Format("ORIG_TRAIN_ID_%d=%8s, ",j+1,"");//ol_trainids[j].train_id);
				txt_eachline += eachcol;
				//BEGIN_STA_1=  0, 
				eachcol.Format("BEGIN_STA_%d=%3d, ",j+1,ol_trainids[j].start_index);
				txt_eachline += eachcol;
				//END_STA_1=  0, 
				eachcol.Format("END_STA_%d=%3d, ",j+1,ol_trainids[j].end_index);
				txt_eachline += eachcol;
			}

		}
		txt_eachline +="\r\n";
		infoFile.WriteString(txt_eachline);
	}
	infoFile.WriteString("[BASIC_INFO_END]\r\n");
	infoFile.Close();
	return TRUE;
}

// �ѻ���ͼ��Ϊ�����ı�
BOOL CTgDocument::olSaveTrainToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(fileName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("���������ı��ļ�ʧ��!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("�ڲ����󣺷Ƿ�����ͼ����");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	CString tmp;
	tmp = "<?xml version=\"1.0\" encoding=\"GB2312\" ?>";
	infoFile.WriteString(tmp);
	infoFile.WriteString("\n<TRAINS>");
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		tmp.Format("\n<TRAIN name=\'%s\'>",  single->GetTrainID());
		infoFile.WriteString(tmp);
		recNum = single->GetTrainRecordCount();
		for(int j=0; j<recNum;j++)
		{
			rec = &single->GetTrainRecord(j)->m_data;
			if(j == 0)
			{
				CString intext = "";
				if(single->GetTrainRecord(j)->IsStartRec())
					intext = "ʼ��";
				else
					intext = "����";

				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), intext);
			}
			if(j == recNum-1)
			{
				CString outtext = "";
				if(single->GetTrainRecord(j)->IsTerminalRec())
					outtext = "�յ�";
				else
					outtext = "����";

				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), outtext);
			}
			if(j > 0 && j < recNum-1)
			{
				CString passtext = "";
				if(single->GetTrainRecord(j)->IsPassRec())
					passtext = "ͨ��";
				else
					passtext = "����";
				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), passtext);
			}

			infoFile.WriteString(tmp);

			const CStationDef* pstationside=GetTGBasicParam()->GetStationDef(rec->station);
			if(pstationside)
			{
				SIDE_NAME sideName;
				memset(sideName, 0, sizeof(sideName));
				if(rec->arrive_side == rec->depart_side)
				{
					if(STA_SUCCESS == pstationside->GetSideName(rec->arrive_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
				else
				{
					if(STA_SUCCESS == pstationside->GetSideName(rec->arrive_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
					infoFile.WriteString(tmp);
					memset(sideName, 0, sizeof(sideName));
					if(STA_SUCCESS == pstationside->GetSideName(rec->depart_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
			}
			else
			{
				if(rec->arrive_side == rec->depart_side)
				{
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
				else
				{
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
					infoFile.WriteString(tmp);
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
			}
			infoFile.WriteString(tmp);

			tmp.Format("\n<ARRIVE_TIME>%d:%d:%d</ARRIVE_TIME>", rec->arrive/3600, (rec->arrive%3600)/60, rec->arrive%60);
			infoFile.WriteString(tmp);

			tmp.Format("\n<DEPART_TIME>%d:%d:%d</DEPART_TIME>", rec->depart/3600, (rec->depart%3600)/60, rec->depart%60);
			infoFile.WriteString(tmp);

			infoFile.WriteString("\n</STATION>");
		}
		infoFile.WriteString("\n</TRAIN>");
	}
	infoFile.WriteString("\n</TRAINS>");
	infoFile.Close();
	return TRUE;	
}
//�ѻ���ͼ��Ϣ��Ϊtntime�ļ�
/*
[BASIC_SCHEDULE]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID=      Z3, INDEX=  0, STATION=  1, ENTRY= 1, EXIT= 6, SIDE= 1, PLAN_SIDE=0, ARRIVE_TIME= 12195, DEPART_TIME= 12195, ARRIVE_DATE=0, PLAN_ARRIVE=03:23:15, DEPART_DATE=0, PLAN_DEPART=03:23:15, MIN_STOP_TIME=    0, FLAG=8, STOP_CONDITION=0, ADJUST_STATUS=0
[BASIC_SCHEDULE_END]
*/
BOOL CTgDocument::olSaveTrainRecords(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(recordsName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("�����ļ�ʧ��!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("�ڲ����󣺷Ƿ�����ͼ����");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;
	CTime adtime;
	infoFile.WriteString("[BASIC_SCHEDULE]\r\n");

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);

		ushort uOne_day = 0;
		recNum = single->GetTrainRecordCount();
		if(recNum==1){
			rec = &single->GetTrainRecord(0)->m_data;
			//July 30,2005 lyc
			if(rec->arrive > rec->depart){
				rec->depart = rec->depart+24*3600;
			}

			//end 

			txt_eachline.Empty();
			//CON_ID= 1, 
			eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
			txt_eachline += eachcol;			
			//TRAIN_INDEX=   0, 
			eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
			txt_eachline += eachcol;
			//TRAIN_ID=      Z3, 
			eachcol.Format("TRAIN_ID=%8s, ",single->GetTrainID());
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("INDEX=%3d, ",0);
			txt_eachline += eachcol;

			//STATION=  1, 
			eachcol.Format("STATION=%3d, ",rec->station);
			txt_eachline += eachcol;

			//ENTRY= 1, 
			eachcol.Format("ENTRY=%2d, ", short(rec->entry));
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("EXIT=%2d, ",short(rec->exit));
			txt_eachline += eachcol;

			//SIDE= 1, 
			eachcol.Format("SIDE=%2d, ",short(rec->arrive_side));
			txt_eachline += eachcol;

			//PLAN_SIDE=0, 
			eachcol.Format("PLAN_SIDE=%2d, ",short(rec->plan_arrive_side));
			txt_eachline += eachcol;

			//ARRIVE_TIME= 12195, 
			eachcol.Format("ARRIVE_TIME=%6d, ",rec->arrive);
			txt_eachline += eachcol;

			//DEPART_TIME= 12195, 
			eachcol.Format("DEPART_TIME=%6d, ",rec->depart);
			txt_eachline += eachcol;

			//ARRIVE_DATE=0, 
			eachcol.Format("ARRIVE_DATE=%d, ",0);
			txt_eachline += eachcol;

			//PLAN_ARRIVE=03:23:15, 
			eachcol.Format("PLAN_ARRIVE=%s, ",""); //OsConvertTimetoString(rec->arrival_time));
			txt_eachline += eachcol;

			//DEPART_DATE=0, 
			eachcol.Format("DEPART_DATE=%d, ",0);
			txt_eachline += eachcol;

			//PLAN_DEPART=03:23:15, 
			eachcol.Format("PLAN_DEPART=%s, ",""); //OsConvertTimetoString(rec->depart_time));
			txt_eachline += eachcol;

			//MIN_STOP_TIME=    0, 
			eachcol.Format("MIN_STOP_TIME=%5d, ",rec->min_stop_time);
			txt_eachline += eachcol;

			//FLAG=8, 
			eachcol.Format("FLAG=%d, ",rec->flag);
			txt_eachline += eachcol;

			//STOP_CONDITION=0, 
			eachcol.Format("STOP_CONDITION=%d, ",rec->stop_condition);
			txt_eachline += eachcol;

			//ADJUST_STATUS=0
			eachcol.Format("ADJUST_STATUS=%d",rec->adjust_status);
			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			infoFile.WriteString(txt_eachline); // by wuxiang
			continue;
		}
		for(int j=0; j<recNum;j++)
		{
			rec = &single->GetTrainRecord(j)->m_data;
			//July 30,2005 lyc
			if(j == 0){
				if(rec->arrive > rec->depart){
					rec->depart += 24*3600;
				}
			}else{
				pre_rec = &single->GetTrainRecord(j-1)->m_data;
				if(rec->arrive < pre_rec->depart){
					{
						rec->depart+=24*3600;
						rec->arrive+=24*3600;
					}
				}else if((rec->arrive > pre_rec->depart)&&
					(rec->arrive > rec->depart))
				{
					rec->depart+=24*3600;
				}
			}
			// end 

			txt_eachline.Empty();
			//CON_ID= 1, 
			eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
			txt_eachline += eachcol;			
			//TRAIN_INDEX=   0, 
			eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
			txt_eachline += eachcol;
			//TRAIN_ID=      Z3, 
			eachcol.Format("TRAIN_ID=%8s, ",single->GetTrainID());
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("INDEX=%3d, ",j);
			txt_eachline += eachcol;

			//STATION=  1, 
			eachcol.Format("STATION=%3d, ",rec->station);
			txt_eachline += eachcol;

			//ENTRY= 1, 
			eachcol.Format("ENTRY=%2d, ",short(rec->entry));
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("EXIT=%2d, ",short(rec->exit));
			txt_eachline += eachcol;

			//SIDE= 1, 
			eachcol.Format("SIDE=%2d, ",short(rec->arrive_side));
			txt_eachline += eachcol;

			//PLAN_SIDE=0, 
			eachcol.Format("PLAN_SIDE=%2d, ",short(rec->plan_arrive_side));
			txt_eachline += eachcol;
			//�˵��Խ��0��,wangjinhai,wuhan
			/*		    if(last_date == rec->arrival_date && last_time >rec->arrival_time  || rec->arrival_date ==1)
			uOne_day= 1;
			if(uOne_day == 1)
			{
			rec->arrival_date =1;
			rec->depart_date = 1;
			}
			//ARRIVE_TIME= 12195, 
			*/			eachcol.Format("ARRIVE_TIME=%6d, ",rec->arrive);
			txt_eachline += eachcol;

			//DEPART_TIME= 12195, 
			eachcol.Format("DEPART_TIME=%6d, ",rec->depart);
			txt_eachline += eachcol;


			//ARRIVE_DATE=0, 
			eachcol.Format("ARRIVE_DATE=%d, ",0);//rec->arrival_date);
			txt_eachline += eachcol;

			//PLAN_ARRIVE=03:23:15, 
			eachcol.Format("PLAN_ARRIVE=%s, ",""); //OsConvertTimetoString(rec->arrival_time));
			txt_eachline += eachcol;

			//DEPART_DATE=0, 
			eachcol.Format("DEPART_DATE=%d, ",0); //rec->depart_date);
			txt_eachline += eachcol;

			//PLAN_DEPART=03:23:15, 
			eachcol.Format("PLAN_DEPART=%s, ",""); //OsConvertTimetoString(rec->depart_time));
			txt_eachline += eachcol;

			//MIN_STOP_TIME=    0, 
			eachcol.Format("MIN_STOP_TIME=%5d, ",rec->min_stop_time);
			txt_eachline += eachcol;

			//FLAG=8, 
			eachcol.Format("FLAG=%d, ",rec->flag);
			txt_eachline += eachcol;

			//STOP_CONDITION=0, 
			eachcol.Format("STOP_CONDITION=%d, ",rec->stop_condition);
			txt_eachline += eachcol;

			//ADJUST_STATUS=0
			eachcol.Format("ADJUST_STATUS=%d",rec->adjust_status);
			txt_eachline += eachcol;

			txt_eachline +="\r\n";

			infoFile.WriteString(txt_eachline);

		}		
	}
	infoFile.WriteString("[BASIC_SCHEDULE_END]\r\n");
	infoFile.Close();
	return TRUE;	
}
//�ѻ���ͼ��Ϣ��Ϊtntime�ļ�
/*
[BASIC_SCHEDULE]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID=      Z3, INDEX=  0, STATION=  1, ENTRY= 1, EXIT= 6, SIDE= 1 
[BASIC_SCHEDULE_END]
*/
BOOL CTgDocument::olSaveTrainSideandEntry(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName)
{
#if 0
	CStdioFile infoFile;
	CStdioFile logFile;
	bool isSucess=true;
	if(!infoFile.Open(recordsName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("�����ļ�"+recordsName+"ʧ��!");
		return FALSE;
	}
	if(!logFile.Open(recordsName+".log",CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("�����ļ�"+recordsName+".logʧ��!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("�ڲ����󣺷Ƿ�����ͼ����");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainInfo * traininfo = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;
	CTime adtime;
	infoFile.WriteString("[BASIC_SCHEDULE]\r\n");
	CTG_TrainPtrGroup  group;
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = m_sBasicTrain[i];
		traininfo = &single->m_nTrainInfo;

		ushort uOne_day = 0;
		recNum = single->GetRecordNum();
		if(recNum==1)
		{
			rec = single->GetIndexRecord(0);
			//July 30,2005 lyc
			if(rec->arrival_time > rec->depart_time)
			{
				rec->depart_date = rec->arrival_date + 1;					
			}
			//end 

			txt_eachline.Empty();


			//TRAIN_ID=   Z3, 
			eachcol.Format("%8s, ",traininfo->train_id[0]); //TRAINID=
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("%3d, ",0); //INDEX=
			txt_eachline += eachcol;

			//PASS OR STAY=  1, 
			if(rec->arrival_time == rec->depart_time)
				eachcol.Format("%1d, ",1); //IS_PASS=
			else
				eachcol.Format("%1d, ",0);//IS_PASS=
			txt_eachline += eachcol;

			//STATION=  2, 
			eachcol.Format("%3d, ",rec->station);//STATION=
			txt_eachline += eachcol;

			//STATION name 3
			eachcol.Format("%10s, ",gpcdef.GetStationName(rec->station)); //STATION_NAME=
			txt_eachline += eachcol;

			//SIDE= 4, 
			eachcol.Format("%2d, ",short(rec->arrive_side)); //SIDE=
			txt_eachline += eachcol;

			//ENTRY=5, 
			eachcol.Format("%2d, ", short(rec->entry)); //ENTRY=
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("%2d, ",short(rec->exit)); //EXIT=
			txt_eachline += eachcol;

			//TENICAL STOP= 7, 
			if(IsTechStopTrain(*rec))
				eachcol.Format("%2d, ",short(1));  
			else
				eachcol.Format("%2d, ",short(0));  

			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			logFile.WriteString(txt_eachline);
			isSucess=false;
			continue;
		}
		for(int j=0; j<recNum;j++)
		{
			rec = single->GetIndexRecord(j);

			if(j == 0){
				if(rec->arrival_time > rec->depart_time){
					rec->depart_date = rec->arrival_date + 1;					
				}
			}else{
				pre_rec = single->GetIndexRecord(j-1);
				if(rec->arrival_time < pre_rec->depart_time){
					rec->depart_date = rec->arrival_date = pre_rec->depart_date + 1;
				}else if((rec->arrival_time > pre_rec->depart_time)&&
					(rec->arrival_time > rec->depart_time)){
						rec->depart_date = rec->arrival_date + 1;
					}else{
						rec->depart_date = rec->arrival_date = pre_rec->depart_date;
					}

			}
			// end 

			txt_eachline.Empty();		

			//TRAIN_ID=      Z3, 
			eachcol.Format("%8s, ",traininfo->train_id[0]); //TRAIN_ID=
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("%3d, ",j); //INDEX=
			txt_eachline += eachcol;

			//PASS OR STAY=  1, 
			if(rec->arrival_time == rec->depart_time)
				eachcol.Format("%1d, ",1);//IS_PASS=
			else
				eachcol.Format("%1d, ",0); //IS_PASS=
			txt_eachline += eachcol;

			//STATION=  2, 
			eachcol.Format("%3d, ",rec->station);//STATION=
			txt_eachline += eachcol;

			//STATION= 3, 
			eachcol.Format("%10s, ",gpcdef.GetStationName(rec->station));//STATION_NAME=
			txt_eachline += eachcol;

			//SIDE= 4, 
			eachcol.Format("%2d, ",short(rec->arrive_side));//SIDE=
			txt_eachline += eachcol;

			//ENTRY= 5, 
			eachcol.Format("%2d, ",short(rec->entry)); //ENTRY=
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("%2d, ",short(rec->exit)); //EXIT=
			txt_eachline += eachcol;

			//TENICAL STOP= 7, 
			if(IsTechStopTrain(*rec))
				eachcol.Format("%2d, ",short(1));  
			else
				eachcol.Format("%2d, ",short(0));  

			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			CString tmpstr(traininfo->train_id[0]);

			if(tmpstr==""){				
				logFile.WriteString(txt_eachline);
				isSucess=false;
			}else{
				infoFile.WriteString(txt_eachline);
			}


		}
		infoFile.WriteString("\r\n");
	}
	infoFile.WriteString("[BASIC_SCHEDULE_END]\r\n");
	infoFile.Close();
	logFile.Close();
	if(isSucess)
		logFile.Remove(recordsName+".log");
#endif
	return TRUE;	
}

void CTgDocument::NoticeNeighbourReceivePlan(CTG_ScheduleInfo& schd_info, DWORD entity)
{
	return;
}

// ���ý��뽻��̨
void CTgDocument::SetTrainFromToConid(int conid, CTG_SingleTrain *ptrain)
{
	if(NULL == ptrain)
		return;

	int rec_count=ptrain->GetTrainRecordCount();
	if(rec_count<=0)
		return;

	CTG_TrainRecord* pRec = NULL;

	// ���̨��վ��־
	for(int k=0; k<rec_count; k++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(k);
		if(NULL != pRec)
			TG_SetNoNeighbourStationFlag(pRec->m_data);
	}
	
	pRec = NULL;
	CONID from_conid = NULL_CONID;
	int i=0;
	for(i=0; i<rec_count; i++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			if(GetTGFunctionConfig()->IsNotCalFromConid(conid, pRec->GetStationNo(), pRec->GetEntry()))
			{
				from_conid=NULL_CONID;
				break;
			}
			from_conid = GetTGFunctionConfig()->GetTrainRunFromConid(conid, pRec->GetStationNo(), pRec->GetExit(), pRec->GetEntry());

			if(from_conid>0 && from_conid != NULL_CONID)
			{
				if((i==0) && pRec->IsStartRec())
					from_conid=NULL_CONID;
				else
				{
					GpcLogTrainMessage(0, 0, "[FROM_TO_SET] %s FROM CONID:%d(this_conid %d fjksta %d entry %d exit %d)", 
						ptrain->GetTrainID(), from_conid, conid, pRec->GetStationNo(), pRec->GetEntry(), pRec->GetExit());
				}
				break;
			}
		}
	}

	if(from_conid>0 && from_conid != NULL_CONID)
	{
		// ���ƥ�䳵վ�ȽϿ���,ʹ����վ
		if(i>3)
			pRec = ptrain->GetTrainRecordByRecIndex(0);
	
		if(NULL != pRec)
			TG_SetNeighbourStationFlag(pRec->m_data);
	}

	pRec = NULL;
	CONID to_conid = NULL_CONID;
	for(i=rec_count-1; i>=0; i--)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			CTG_TrainRecord* pPreRec = ptrain->GetTrainRecordByRecIndex(i-1);
			if(pPreRec)
			{
				if(GetTGFunctionConfig()->IsLimitSectionCalToConid(conid, pPreRec->GetStationNo(), pRec->GetStationNo()))
				{
					break;
				}
			}
			to_conid = GetTGFunctionConfig()->GetTrainRunToConid(conid, pRec->GetStationNo(), pRec->GetExit());

			if(to_conid>0 && to_conid != NULL_CONID)
			{
				if((i==rec_count-1) && pRec->IsTerminalRec())
					to_conid=NULL_CONID;
				else
				{
					GpcLogTrainMessage(0, 0, "[FROM_TO_SET] %s TO CONID:%d(this_conid %d fjksta %d exit %d)", 
						ptrain->GetTrainID(), to_conid, conid, pRec->GetStationNo(), pRec->GetExit());
				}
				break;
			}
		}
	}

	if(to_conid>0 && to_conid != NULL_CONID)
	{
		// ���ƥ�䳵վ�ȽϿ�ǰ,ʹ��βվ
		if(rec_count-i>3)
			pRec = ptrain->GetTrainRecordByRecIndex(rec_count-1);
		
		if(NULL != pRec)
			TG_SetNeighbourStationFlag(pRec->m_data);
	}

	ptrain->SetFromToConId(from_conid, to_conid);
	return;
}

// ȥ�������Ǽ���ͣ��
bool CTgDocument::ResetGoodsTrainStopTech(CTG_SingleTrain *ptrain)
{
	if(ptrain->IsPassengerTrain())
		return false;

	if(GetTGFunctionConfig()->GoodsTrainHaveTechStop(ptrain->GetTrainInfo()->m_data.type))
		return false;

	CTG_TrainRecord* pRec = NULL;
	int rec_count=ptrain->GetTrainRecordCount();
	for(int i=0; i<rec_count; i++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			if(TG_IsTechStopTrain(pRec->m_data))
				continue;

			// ���� Ӧ����Ϊ����ͣ��
			TG_SetTechStopTrain(pRec->m_data);
			GpcLogTrainMessage(0, 0, "tg_logic.cfg��GoodTrainHaveTechStopû�����ƻ���type:%d �����Ǽ���ͣ��, �������Ϊ����ͣ�� %s ��վ%d", 
				ptrain->GetTrainInfo()->m_data.type, ptrain->GetTrainID(),  pRec->GetStationNo());
		}
	}

	return true;
}

// ���÷����г�
void CTgDocument::SetTrainReverseFlag(CTG_SingleTrain *ptrain)
{
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return;

	for (int i=0; i<rec_count; i++) 
	{
		CString trainid = ptrain->GetTrainID();
		if(rec[i].m_data.plan_arrive <= 0)
		{
			GpcLogTrainMessage(0, 0, "�г�: %s plan_arrive %d arrive %d, ��ʵ�ʵ��ʼ���ƻ��� %d", 
				trainid, rec[i].m_data.plan_arrive, rec[i].m_data.arrive,i);
			rec[i].m_data.plan_arrive = rec[i].m_data.arrive;
		}
		if(rec[i].m_data.plan_depart <= 0)
		{
			GpcLogTrainMessage(0, 0, "�г�: %s plan_depart %d depart %d, ��ʵ�ʵ��ʼ���ƻ��� %d", 
				trainid, rec[i].m_data.plan_depart, rec[i].m_data.depart,i);
			rec[i].m_data.plan_depart = rec[i].m_data.depart;
		}

		if (i<rec_count-1) 
		{
			BOOL bIsDownAbs = GetTGBasicParam()->IsAbsDirectionDown(rec[i].m_data, TRAIN_OUT_STATION);
			BOOL bIsUpTrain = GetTGBasicParam()->IsTrainDirectionUp(rec[i].m_data, TRAIN_OUT_STATION);

			if(bIsDownAbs && bIsUpTrain)
			{
				TG_SetFlagReverse(rec[i].m_data.flag);
				GpcLogTrainMessage(0, 0, "%s �ڳ�վ%d�����г�", ptrain->GetTrainID(), rec[i].m_data.station);
				GpcLogTrainMessage(0, 0, "���䷽��: %s", GetTGBasicParam()->GetAbsDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
				GpcLogTrainMessage(0, 0, "�г�����: %s", GetTGBasicParam()->GetTrainDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));

			}
			else
			{
				BOOL bIsUpAbs     = GetTGBasicParam()->IsAbsDirectionUp(rec[i].m_data, TRAIN_OUT_STATION);
				BOOL bIsDownTrain = GetTGBasicParam()->IsTrainDirectionDown(rec[i].m_data, TRAIN_OUT_STATION);
				if(bIsUpAbs && bIsDownTrain)
				{
					TG_SetFlagReverse(rec[i].m_data.flag);
					GpcLogTrainMessage(0, 0, "%s �ڳ�վ%d�����г�", ptrain->GetTrainID(), rec[i].m_data.station);
					GpcLogTrainMessage(0, 0, "���䷽��: %s", GetTGBasicParam()->GetAbsDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
					GpcLogTrainMessage(0, 0, "�г�����: %s", GetTGBasicParam()->GetTrainDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
				}
				else
				{
					TG_ClearFlagReverse(rec[i].m_data.flag);
				}
			}
		}
	}
}

int  CTgDocument::GetTrainIDDirection(CString strTrainId)
{
	// ȥ��������
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		return UP_DIRECT;
	else
		return DOWN_DIRECT;
}

BOOL CTgDocument::ReleaseBasicTrainInfo()
{
	for(int i=0; i<basic_train_info_num; i++)
	{
		delete basic_train_info[i];
	}
	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));
	return TRUE;
}

BOOL CTgDocument::LoadTrainSideTechToFile(const CString& fileName)
{
	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{
		CString  prompt;
		prompt.Format("�ɵ���ҵ�ļ� %s�����ڻ��߲�����xml�﷨(���������������Ƿ��ܴ�)", fileName);
		AfxMessageBox(prompt);

		return FALSE; 
	}

	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode,pNodeTrain,pNodeStation;
	MSXML::IXMLDOMNodeListPtr list, liststation;

	pRootNode = pXMLDoc->selectSingleNode( "TRAINS");
	if(pRootNode == NULL)
		return FALSE;

	list = pRootNode->selectNodes( "TRAIN");
	if(NULL == list)
		return FALSE;


	for(int i=0; i<list->Getlength() && i < 2048; i++)
	{
		pNodeTrain = list->Getitem(i);

		liststation = pNodeTrain->selectNodes( "STATION");
		if(NULL == liststation)
			continue;

		pNode = pNodeTrain->selectSingleNode("@name");
		if(NULL == pNode)
			continue;

		NODE_TRAIN* ptrain = new NODE_TRAIN();
		if(ptrain == NULL)
			continue;

		_variant_t v;
		v = pNode->GetnodeTypedValue();
		CString name(v.bstrVal);			
		SysFreeString(v.bstrVal);
		v.Clear();

		memset(ptrain->name, 0, 16);
		strncpy(ptrain->name, name.GetBuffer(), 15);

		ptrain->num=0;
		for(int i=0; i<liststation->Getlength(); i++)
		{
			pNodeStation = liststation->Getitem(i);

			pNode = pNodeStation->selectSingleNode("@station");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].station = atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@arrive_trainid");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				CString name(v.bstrVal);
				memset(ptrain->node[ptrain->num].arrive_trainid, 0, sizeof(TRAIN_NO));
				strncpy(ptrain->node[ptrain->num].arrive_trainid, name.GetBuffer(), sizeof(TRAIN_NO)-1);			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@depart_trainid");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				CString name(v.bstrVal);
				memset(ptrain->node[ptrain->num].depart_trainid, 0, sizeof(TRAIN_NO));
				strncpy(ptrain->node[ptrain->num].depart_trainid, name.GetBuffer(), sizeof(TRAIN_NO)-1);			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@arrive_side");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].arrive_side = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@depart_side");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].depart_side = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@tech_stop");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].tech_stop = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			ptrain->num++;
		}
		basic_train_info[basic_train_info_num++] = ptrain;
	}

	return TRUE;
}

BOOL CTgDocument::SaveTrainSideTechToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(fileName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("���������ı��ļ�ʧ��!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("�ڲ����󣺷Ƿ�����ͼ����");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	const CTG_TrainRecord * prec = NULL;

	int recNum = 0;

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	CString tmp;
	tmp = "<?xml version=\"1.0\" encoding=\"GB2312\" ?>";
	infoFile.WriteString(tmp);
	infoFile.WriteString("\n<TRAINS>");
	CDlgWait *pw=new CDlgWait("���ڱ������ͼ��Ϣ,������Ҫ1-2����ʱ��,���Ժ�...");
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		if(NULL == single)
			continue;

		pw->UpdateDisplay(single->GetTrainID());
		tmp.Format("\n<TRAIN name=\"%s\">",  single->GetTrainID());
		infoFile.WriteString(tmp);
		recNum = single->GetTrainRecordCount();
		for(int j=0; j<recNum;j++)
		{
			prec = single->GetTrainRecord_Const(j);
			if(prec == NULL)
				continue;

			int tech_stop=0;
			if(TG_IsTechStopTrain(prec->m_data))
				tech_stop=1;

			tmp.Format("\n<STATION station =\"%d\" arrive_trainid=\"%s\" depart_trainid=\"%s\" arrive_side=\"%d\" depart_side=\"%d\" tech_stop=\"%d\"/>", 
				prec->GetStationNo(), prec->GetArriveTID(), prec->GetDepartTID(), prec->m_data.arrive_side,  prec->m_data.depart_side, tech_stop);

			infoFile.WriteString(tmp);
		}
		infoFile.WriteString("\n</TRAIN>");
	}
	infoFile.WriteString("\n</TRAINS>");
	infoFile.Close();

	delete pw;
	return TRUE;	
}



BOOL CTgDocument::InitBasicInfo(CTG_SingleTrain * ptrain)
{
	if(ptrain==NULL)
		return FALSE;

	CString trainid= ptrain->GetTrainID();
	// ���ճ��β����г�
	int i=0;
	for(i=0; i<basic_train_info_num&&i<2048; i++)
	{
		if(NULL == basic_train_info[i])
			continue;

		if(trainid == basic_train_info[i]->name)
			break;
	}

	if(i>=basic_train_info_num||i>=2048)
		return FALSE;

	CTG_TrainRecord* prec = NULL;
	int station;
	CString arrive_trainid,depart_trainid;

	for(int k=0; k<ptrain->GetTrainRecordCount(); k++)
	{
		prec = ptrain->GetTrainRecordByRecIndex(k);

		if(NULL == prec)
			continue;

		station = prec->GetStationNo();
		arrive_trainid = prec->GetArriveTID();
		depart_trainid = prec->GetDepartTID();
		if(arrive_trainid.IsEmpty() && depart_trainid.IsEmpty())
			continue;
		for(int n=0; n<basic_train_info[i]->num&&n<128; n++)
		{
			if(basic_train_info[i]->node[n].station == station)
			{
				if( (arrive_trainid.IsEmpty() || (!arrive_trainid.IsEmpty() && arrive_trainid == basic_train_info[i]->node[n].arrive_trainid))
					&&(depart_trainid.IsEmpty() || (!depart_trainid.IsEmpty() && depart_trainid == basic_train_info[i]->node[n].depart_trainid)))
				{
					if(basic_train_info[i]->node[n].arrive_side != 255)
					{
						prec->m_data.arrive_side=basic_train_info[i]->node[n].arrive_side;
						prec->m_data.plan_arrive_side=prec->m_data.arrive_side;
					}

					if(basic_train_info[i]->node[n].depart_side != 255)
					{
						prec->m_data.depart_side=basic_train_info[i]->node[n].depart_side;
						prec->m_data.plan_depart_side=prec->m_data.depart_side;
					}

					if(basic_train_info[i]->node[n].tech_stop==1)
						TG_SetTechStopTrain(prec->m_data);
					else
						TG_ClearTechStopTrain(prec->m_data);
					break;
				}
			}
		}
	}

	return TRUE;
}

void CTgDocument::SendGpcAckMsg(int type, bool isOk, long src, CString msgid, CString jht, CString result)
{
	CString token,content;
	if(isOk)
		TD_GenAckMessage_GPCACKINFO(token, content, GetCurrentSectionNo(), type, 1,gpcdef.GetCurrentUsername(),src,msgid,jht, result);
	else
		TD_GenAckMessage_GPCACKINFO(token, content, GetCurrentSectionNo(), type, 0,gpcdef.GetCurrentUsername(),src,msgid,jht, result);
	if(!token.IsEmpty())
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); // ��TD��ȡ,��ִTD
		if(gpcdef.IsEnableMMPlanFromCAD()) // �ӷ�������ȡ,��ִ������
			eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 

		TGSendStringMessage(token,content,eid);

		CString text;
		text.Format("�� TD ���ͻ�ִ��Ϣ: ����:%d  ״̬:%d Դ��:%d ��Ϣ��:%u �ƻ�̨��:%s ��Ϣ����: %s", type, isOk, src, msgid, jht, content);
		GpcLogTrainMessage(0,0,text);
	}

	
}
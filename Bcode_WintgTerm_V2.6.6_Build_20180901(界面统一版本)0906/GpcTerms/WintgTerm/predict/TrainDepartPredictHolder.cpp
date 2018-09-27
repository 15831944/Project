#include "StdAfx.h"
#include "sysshare.h"
#include "tg_schedule_base.h"
#include "tgdocument.h"
#include ".\DepartPredictDlg.h"
#include "Mmsystem.h"
#include ".\TrainDepartPredictHolder.h"
extern bool CreateRecursiveDir(const char* filename);
extern CString GetRecordXML(TrainPredict& predict);
// ��ȡ��վ����ģʽ,�·��׶μƻ����ʹ��
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp

CTrainDepartPredictHolder::CTrainDepartPredictHolder(CTgDocument* pDoc, int con_id)
{
	for(int i=0; i<15; i++)
	{
		for(int j=0; j<3; j++)
		{
			CDlgTimeMessageBox::WindowPosition[i][j]=0;
		}
	}
	m_pDoc=pDoc;
	m_nShiftID=0;
	m_conid=con_id;
	m_pMMI=NULL;
	m_pWorkSchedule=NULL;

	CWorkScheduleHolder* pSchdHoder=m_pDoc->GetTGDataHolder();
	if(pSchdHoder)
	{
		m_pWorkSchedule = pSchdHoder->GetWorkScheduleData();
	}
	
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
		m_pStPlans[i]=NULL;
	
	LogPredict(0,"[�Զ�Ԥ��] ����conid:%d Ԥ����� TrainDepartPredictHolder %X", m_conid, this);
	
	STATION_NO list_station[256];
	STATION_NO list_neib_station[256];
	memset(list_station, 0, sizeof(list_station));
	memset(list_neib_station, 0, sizeof(list_neib_station));

	int stationnum=gpcdef.GetYugaoStation(m_conid, 256, list_station, list_neib_station);
	for(int i=0; i<stationnum; i++)
	{
		STATION_NO  station		=list_station[i];
		STATION_NO  neib_station	=list_neib_station[i];
		if(station==0 || neib_station==0) 
			continue;
		
		for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
		{
			if(m_pStPlans[i])
			{
				if(m_pStPlans[i]->GetStationNo()==station)
				{
					m_pStPlans[i]->AddNeibStation(neib_station);
					break;
				}
			}
			else
			{
				m_pStPlans[i] =new CStationPredict(station, neib_station);
				if(m_pStPlans[i])
				{
					LogPredict(0,"[�Զ�Ԥ��] ע�ᳵվ:%s(%d)�ɹ�,��վΪ:%s(%d)", 
						gpcdef.GetStationName(station), station, 
						gpcdef.GetStationName(neib_station), neib_station);
				}
				else
				{
					LogPredict(0,"[�Զ�Ԥ��] ע�ᳵվ:%s(%d)ʧ��,��վΪ:%s(%d), ԭ���������ڴ�ʧ��", 
						gpcdef.GetStationName(station), station, 
						gpcdef.GetStationName(neib_station), neib_station);
				}
				break;
			}
		}
	}

	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			m_pStPlans[i]->LoadStplanFromFile();
		}
	}
}

CTrainDepartPredictHolder::~CTrainDepartPredictHolder(void)
{
	LogPredict(0,"[�Զ�Ԥ��] ɾ��conid:%d Ԥ����� TrainDepartPredictHolder %X", m_conid, this);
	
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			delete m_pStPlans[i];
			m_pStPlans[i]=NULL;
		}
	}
}

CStationPredict* CTrainDepartPredictHolder::GetStPredictPlan(STATION_NO this_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==this_station)
			{
				return m_pStPlans[i];
			}
		}
	}

	return NULL;
}

CStationPredict* CTrainDepartPredictHolder::GetStPredictPlan(STATION_NO this_station, STATION_NO neib_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==this_station && m_pStPlans[i]->IsMyNeibStation(neib_station))
			{
				return m_pStPlans[i];
			}
		}
	}

	return NULL;
}

bool CTrainDepartPredictHolder::IsNeedDowithDepartPredict(STATION_NO station)
{
	TG_STATION_STATUS * pst = GetStationStatusPtr(station);
	if(NULL != pst)
	{
		if(pst->ctrl_mode==SMODE_DISTRIBUTED ||
			pst->ctrl_mode==SMODE_CENTER_CTRL)
		{
			return true;
		}
	}

	return false;
}

void CTrainDepartPredictHolder::SendRevYugao(MsgTrainDepartPredict *pBuf)
{
	MsgTrainDepartPredict msg;

	msg.nSourceStno = pBuf->nDestStno;		// ������վ
	msg.nDestStno = pBuf->nSourceStno;		// ����Ŀ�공վ
	msg.nDestEntry = pBuf->nDestEntry;		// Ŀ�공վ�ӳ����
	strcpy(msg.strTid, pBuf->strTid);			// ���κ�
	msg.nTrainIndex = pBuf->nTrainIndex;	// Train Index
	msg.nRecIndex = pBuf->nRecIndex;
	
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("�����ļ������Ҳ������û���ENTITY!");
		return;
	}
	msg.nSrcEntity = entityInfo.nEntityId;		// ����Ԥ���ִ��ʵ���
	msg.nSrcType=MsgTrainDepartPredict::GPC;// ����Ԥ���ʵ������	
	msg.nOperType=MsgTrainDepartPredict::SHOUDAO;	// Ԥ������

	CTime tm=CTime::GetCurrentTime();
	msg.nOperTime = tm.GetTime();				// Ԥ��ʱ��

	long eid=GetSpecialEntityID(SERVER_TYPE_PREDICT);
	SendBMsg(&msg, eid);

	LogPredict(0, "�����յ�Ԥ���ִ train %s SourceStno %d DestStno %d entry %d trainIndex %d recIndex %d", 
			msg.strTid, msg.nSourceStno, msg.nDestStno, 
			msg.nDestEntry, msg.nTrainIndex, msg.nRecIndex);

}

BOOL  CTrainDepartPredictHolder::ProcessPredictByNeibMsg(MsgTrainDepartPredict *pBuf)
{
	if(pBuf->nOperType != MsgTrainDepartPredict::YUGAO && pBuf->nOperType != MsgTrainDepartPredict::QUXIAO &&
		pBuf->nOperType != MsgTrainDepartPredict::SHOUDAO && pBuf->nOperType != MsgTrainDepartPredict::TONGYI)
	{
		LogPredict(0, "nSrcEntity %d ���͵�Ԥ����Ϣ,����nOperType����ȷ: trainid %s nSourceStno %d nDestEntry %d nDestStno %d nSrcType %d nOperType %d trainIndex %d recIndex %d nOperTime %d", 
					pBuf->nSrcEntity, pBuf->strTid, pBuf->nSourceStno, pBuf->nDestEntry, pBuf->nDestStno, pBuf->nSrcType, pBuf->nOperType, 
					pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->nOperTime);

		return FALSE;
	}
	LogPredict(0, "***************************");

	LogPredict(0, "�����յ�Ԥ����Ϣ:trainid %s nSourceStno %d nDestEntry %d nDestStno %d nSrcType %d nOperType %d trainIndex %d recIndex %d nOperTime %d", 
					pBuf->strTid, pBuf->nSourceStno, pBuf->nDestEntry, pBuf->nDestStno, pBuf->nSrcType, pBuf->nOperType, 
					pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->nOperTime);
	
	
	LogPredict(0, "Ŀ��վ %d �ǵ���̨����վ ԴվΪ��վ %d", pBuf->nDestStno,pBuf->nSourceStno);
		
	bool bUpdateUI=false;
	if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO || pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
	{
		if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
		{
			SendRevYugao(pBuf);
			LogPredict(0,"�յ���վԤ��,���ұ��ؼƻ�");
		}
		else
			LogPredict(0,"�յ���վȡ������,���ұ��ؼƻ�");

		STATION_NO station = pBuf->nDestStno;
		STATION_NO neib_station = pBuf->nSourceStno;
		TRAIN_INDEX train_index = pBuf->nTrainIndex;
		short rec_index = pBuf->nRecIndex;
		CString arrive_trainid=pBuf->strTid;

		CStationPredict* pstation = GetStPredictPlan(station, neib_station);
		if(NULL == pstation)
		{
			LogPredict(0, "û�б�վ %d ����վ %d ��Ԥ������, �޷�����Ԥ�����ʱ��", station, neib_station);
			return false;
		}
			
		std::vector<int> list_idx;
		pstation->GetStplanByTrainNoAndPreStation(arrive_trainid, neib_station, list_idx);
		if(list_idx.size()<=0)
		{
			LogPredict(0,"����Ԥ�泵վ���� %s �Ҳ�����Ӧ�ƻ�, �޷�������վ��������Ԥ��.", arrive_trainid);
			return false;
		}
		else
		{
			LogPredict(0,"����Ԥ�泵վ���� %s �ҵ� %d ���ƻ�:", arrive_trainid, list_idx.size());
		}
			
		int idx = -1;
		if(list_idx.size()>1)
		{
			for(int n=0; n<list_idx.size(); n++)
			{
				int no = list_idx[n];
				if(pstation->m_stplan[no].train_index==train_index && pstation->m_stplan[no].rec_index==rec_index)
				{
					idx = no;
					LogPredict(0,"����train_index %u rec_index %d ��λ������ƻ�", train_index, rec_index);
					break;
				}
			}

			if(idx==-1)
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
						pMMI->SaveTrainNo(station, neib_station, arrive_trainid, TrainPredict::NEIB_YUGAO, pBuf->nOperTime, 1);
					else
						pMMI->SaveTrainNo(station, neib_station, arrive_trainid, TrainPredict::NEIB_CANCEL, pBuf->nOperTime, 1);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
		else
		{
			idx=list_idx[0];
		}
		
		if(idx!=-1)
		{
			if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
			{
				bUpdateUI = true;

				pstation->m_stplan[idx].neib_yugao_time  = pBuf->nOperTime;
				pstation->m_stplan[idx].neib_cancel_time = 0;
				pstation->SaveStplanToFile();
				LogPredict(0,"������վԤ�� �ɹ�, train_index %u rec_index %d trainid %s Ԥ��ʱ��:%d.", 
					pstation->m_stplan[idx].train_index, pstation->m_stplan[idx].rec_index, pBuf->strTid, pBuf->nOperTime);
			}

			if(pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
			{
				bUpdateUI = true;

				pstation->m_stplan[idx].tongyi_neib_time =0;
				pstation->m_stplan[idx].neib_yugao_time = 0;
				pstation->m_stplan[idx].neib_cancel_time = pBuf->nOperTime;
				pstation->SaveStplanToFile();
				LogPredict(0,"������վȡ������ �ɹ�, train_index %u rec_index %d trainid %s ��վȡ������ʱ�� %d", 
					pstation->m_stplan[idx].train_index, pstation->m_stplan[idx].rec_index, pBuf->strTid, pBuf->nOperTime);
			}
		}
	}
	else if(pBuf->nOperType == MsgTrainDepartPredict::SHOUDAO)
	{
		LogPredict(0,"���յ� Ԥ���յ���ִ��train_index %d rec_index %d trainid %s tm %d", 
				pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->strTid, pBuf->nOperTime);
	}
	else if(pBuf->nOperType == MsgTrainDepartPredict::TONGYI)
	{
		STATION_NO station = pBuf->nDestStno;
		STATION_NO neib_station = pBuf->nSourceStno;
		TRAIN_INDEX train_index = pBuf->nTrainIndex;
		short rec_index = pBuf->nRecIndex;
		CString depart_trainid=pBuf->strTid;

		LogPredict(0,"�յ���վͬ��,���ұ��ؼƻ�");

		CStationPredict* pstation = GetStPredictPlan(station, neib_station);
		if(NULL == pstation)
		{
			LogPredict(0, "û�б�վ %d ����վ %d ��Ԥ������, �޷�����Ԥ�����ʱ��", station, neib_station);
			return false;
		}
			
		std::vector<int> list_idx;
		pstation->GetStplanByTrainNoAndNextStation(depart_trainid, neib_station, list_idx);
		if(list_idx.size()<=0)
		{
			LogPredict(0,"����Ԥ�泵�� %s �Ҳ�����Ӧ�ƻ�, �޷�������վ��������ͬ��.", depart_trainid);
			return false;
		}
		else
		{
			LogPredict(0,"����Ԥ�泵�� %s �ҵ� %d ���ƻ�:", depart_trainid, list_idx.size());
		}
			
		int idx = -1;
		if(list_idx.size()>1)
		{
			for(int n=0; n<list_idx.size(); n++)
			{
				int no = list_idx[n];
				if(pstation->m_stplan[no].train_index==train_index && pstation->m_stplan[no].rec_index==rec_index)
				{
					idx = no;
					LogPredict(0,"����train_index %u rec_index %d ��λ������ƻ�", train_index, rec_index);
					break;
				}
			}

			if(idx==-1)
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					pMMI->SaveTrainNo(station, neib_station, depart_trainid, TrainPredict::NEIB_TONGYI, pBuf->nOperTime, 2);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
		else
		{
			idx=list_idx[0];
		}
		
		if(idx>=0)
		{
			if(pstation->m_stplan[idx].yugao_neib_time>0)
			{
				pstation->m_stplan[idx].neib_tongyi_time = pBuf->nOperTime;
				pstation->SaveStplanToFile();
				bUpdateUI = true;

				LogPredict(0,"������վͬ�� �ɹ�, train_index %d rec_index %d trainid %s tm %d", 
							pBuf->nTrainIndex, pBuf->nRecIndex, pBuf->strTid, pBuf->nOperTime);
			}
			else
			{
				CDepartPredictDlg* pMMI = GetMMI();
				if(pMMI!=NULL)
				{
					pMMI->SaveTrainNo(station, neib_station, depart_trainid, TrainPredict::NEIB_TONGYI, pBuf->nOperTime, 2);
					pMMI->ShowWindow(SW_SHOW);
					return TRUE;
				}
			}
		}
	}
		
	if(bUpdateUI==true)
	{
		CString path;
		path.Format("%s\\Sound\\alarm\\", gpcdef.GetDataPathName());
		
		CString text;
		if(pBuf->nOperType == MsgTrainDepartPredict::YUGAO)
		{
			text.Format("%s �� %s Ԥ�� %s�� ����", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);

			path+="��վ����Ԥ��.wav";
		}
		else if(pBuf->nOperType == MsgTrainDepartPredict::QUXIAO)
		{
			text.Format("%s ȡ���� %s ���� %s��", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);
			path+="��վȡ������.wav";
		}
		else if(pBuf->nOperType == MsgTrainDepartPredict::TONGYI)
		{
			text.Format("%s �ѽ��� %s %s�� ����Ԥ��", 
				gpcdef.GetStationName(pBuf->nSourceStno), 
				gpcdef.GetStationName(pBuf->nDestStno), 
				pBuf->strTid);
			path+="��վͬ�ⷢ��.wav";
		}
		else
		{
			text="";
			path="";
		}
				
		if(!text.IsEmpty())
		{
			int pX=0, pY=0;
			for(int y=0; y<3; y++)
			{
				bool bfind=false;
				for(int x=0; x<12; x++)
				{
					if(CDlgTimeMessageBox::WindowPosition[x][y]==0)
					{
						pX=x;
						pY=y;
						bfind=true;
						break;
					}
				}
				if(bfind)
					break;
			}
	
			CDlgTimeMessageBox* m_pMessageDlg = new CDlgTimeMessageBox(text, 30, pX, pY, NULL);
			if(m_pMessageDlg)
			{
				m_pMessageDlg->Create(IDD_DLG_TIME_MESSAGE);
				m_pMessageDlg->ShowWindow(SW_SHOW);
				CRect sz;
				m_pMessageDlg->GetWindowRect(&sz);
				m_pMessageDlg->MoveWindow(pY*50, pX*80, sz.Width(), sz.Height());

				if(path!="")
					PlaySound(path, NULL, SND_FILENAME|SND_ASYNC);
			}
		}
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}
	
	return TRUE;
}

void CTrainDepartPredictHolder::LogPredict(short process_id, const char *fmt, ...)
{
	char tmp[1024];
	memset(tmp, 0, 1024);

	va_list va;
	va_start(va,fmt);
	_vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[1023]=0;
	
	CTime tm=CTime::GetCurrentTime();
	SaveLogMessageStr("predict", tm.GetTime(), tmp);
}

bool CTrainDepartPredictHolder::IsMyStation(STATION_NO station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==station)
				return true;
		}
	}

	return false;
}

bool CTrainDepartPredictHolder::IsMyNeibStation(STATION_NO station, STATION_NO neib_station)
{
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			if(m_pStPlans[i]->GetStationNo()==station)
			{
				if(m_pStPlans[i]->IsMyNeibStation(neib_station))
					return true;
				else
					return false;
			}
		}
	}

	return false;
}

void CTrainDepartPredictHolder::SaveStplanToHistory()
{
	ULONG nEndTime=0;
	CTime curtm=CTime::GetCurrentTime();
	int  nHour=curtm.GetHour();
	if(nHour>=18 && nHour<20)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 18,0,0);
		nEndTime=tm.GetTime();
	}
	else if(nHour>=6 && nHour<8)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 6,0,0);
		nEndTime=tm.GetTime();
	}
	else
	{
		return;
	}

	long shiftid=CTG_ScheduleInfo::Get_CurrentShiftid();
	long preshiftid=CTG_ScheduleInfo::Get_PreShiftid(shiftid);
	CString fileName;
	fileName.Format("%s\\predict\\%d_%d.xml", gpcdef.GetWorkPathName(), m_conid, preshiftid);
	
	// �ж���ʷ�ļ��Ƿ����
	CFileFind finder;

	// ����ļ����ڣ�������ת��
	if(finder.FindFile(fileName))
		return;
	
	CreateRecursiveDir(fileName);

	CStdioFile file;
	if(!file.Open(fileName, CFile::modeCreate|CFile::modeWrite))
		return;

	file.WriteString("<PREDICT_PLAN>\n");
	// ת��
	for(int i=0;i<MAX_PREDICT_STATION_PLAN;i++)
	{
		if(m_pStPlans[i])
		{
			for(UINT n=0; n<MAX_STPLAN_NUM; n++)
			{
				if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==0)
					break;
				
				if(m_pStPlans[i]->m_stplan[n].train_index==0 && m_pStPlans[i]->m_stplan[n].station_no==65535)
					continue;
				
				if(m_pStPlans[i]->m_stplan[n].start==1)
				{
					if(m_pStPlans[i]->m_stplan[n].depart_time>=nEndTime)
						continue;
				}
				else if(m_pStPlans[i]->m_stplan[n].start==2)
				{
					if(m_pStPlans[i]->m_stplan[n].arrive_time>=nEndTime)
						continue;
				}
				else
				{
					if(m_pStPlans[i]->m_stplan[n].depart_time>=nEndTime)
						continue;
				}

				file.WriteString(GetRecordXML(m_pStPlans[i]->m_stplan[n]));
				m_pStPlans[i]->m_stplan[n].train_index=0;
				m_pStPlans[i]->m_stplan[n].station_no=65535;
			}
			file.Flush();
			
			m_pStPlans[i]->SaveStplanToFile();
		}
	}

	file.WriteString("</PREDICT_PLAN>\n");
	file.Close();

	NoticeMMIUpdateUI(WM_USER+1, 3, 0);
}

// �������
CDepartPredictDlg* CTrainDepartPredictHolder::GetMMI()
{
	if(m_pMMI==NULL)
		return NULL;
	
	if(::IsWindow(m_pMMI->m_hWnd))
		return m_pMMI;
	return NULL;
}

int  CTrainDepartPredictHolder::GetPredictStationList(STATION_NO list[], int num)
{
	int count=0;
	for(int i=0;i<MAX_PREDICT_STATION_PLAN&&count<num;i++)
	{
		if(m_pStPlans[i])
		{
			list[count]= m_pStPlans[i]->GetStationNo();
			count++;
		}
	}

	return count;
}

void CTrainDepartPredictHolder::NoticeMMIUpdateUI(int msgid, int type, int flag)
{
	CDepartPredictDlg* pMMI = GetMMI();
	if(pMMI!=NULL)
	{
		pMMI->SendMessage(msgid, type, flag);
	}
	return;
}

void CTrainDepartPredictHolder::SendStplan(std::vector<stplannotice>& predict_notice, STATION_NO station)
{	
	CStationPredict* pStplanStation = GetStPredictPlan(station);
	if(pStplanStation==NULL)
		return;

	LogPredict(0,"==============================================");
	LogPredict(0,"���ռƻ� ��վ:%s(%d) �ƻ���Ŀ:%d", gpcdef.GetStationName(station), station, predict_notice.size());
	if(predict_notice.size()<=0)
	{
		return;
	}

	for(int i=0; i<predict_notice.size(); i++)
	{
		if(predict_notice[i].flag==1)
			LogPredict(0,"ɾ���ƻ� sta %d train_index %d rec_index %d", station, predict_notice[i].train_index, predict_notice[i].rec_index);
		else
			LogPredict(0,"���¼ƻ� sta %d train_index %d rec_index %d", station, predict_notice[i].train_index, predict_notice[i].rec_index);
	}

	std::vector<stplannotice> notice;
	for(int i=predict_notice.size()-1; i>=0; i--)
	{
		int k=0;
		for(; k<notice.size(); k++)
		{
			if(notice[k].train_index==predict_notice[i].train_index && notice[k].rec_index==predict_notice[i].rec_index)
			{
				break;
			}
		}

		if(k>=notice.size())
		{
			notice.push_back(predict_notice[i]);
		}
	}
		
	if(m_pWorkSchedule==NULL)
		return;

	// ����ƻ�
	for(int i=0; i<notice.size(); i++)
	{
		if(notice[i].flag==1)
		{
			TrainPredict predict;
			pStplanStation->DeletePredict(notice[i].train_index, notice[i].rec_index, predict);
			LogPredict(0,"ɾ���ƻ�station %d train_index %u rec_index %d", station, notice[i].train_index, notice[i].rec_index);
			this->OutputPredict(predict);
		}
		else
		{
			TrainPredict predict;
			if (OrganizeStplan(predict, notice[i].train_index, notice[i].rec_index, station))
			{
				int nResult=pStplanStation->UpdatePredictPlan(predict);
				if(nResult==0)
				{
					LogPredict(0,"�����ƻ�ʧ��station %d train_index %u rec_index %d ǰһվ:%d�ͺ�һվ:%d��������������ҪԤ�����վ", 
					predict.station_no, predict.train_index, predict.rec_index, 
					predict.pre_station_no,	predict.next_station_no);
				}
				else if(nResult==1)
				{
					LogPredict(0,"���¼ƻ��ɹ�station %d train_index %u rec_index %d", 
						predict.station_no, predict.train_index, predict.rec_index);
					this->OutputPredict(predict);
				}
				else if(nResult==2)
				{
					LogPredict(0,"�����ƻ��ɹ�station %d train_index %d rec_index %d", 
						predict.station_no, predict.train_index, predict.rec_index);
					this->OutputPredict(predict);
				}
				else
				{
					LogPredict(0,"�����ƻ�ʧ��station %d train_index %d rec_index %d ���������ɼƻ�����", 
						predict.station_no, predict.train_index, predict.rec_index);
				}
			}
			else
			{
				LogPredict(0,"�����ƻ�ʧ��station %d train_index %u rec_index %d", 
									station, notice[i].train_index, notice[i].rec_index);
			}
		}
	}
	pStplanStation->SaveStplanToFile();
	NoticeMMIUpdateUI(WM_USER+1, 3, 0);
	return;	
}

bool CTrainDepartPredictHolder::OrganizeStplan(TrainPredict& predict, TRAIN_INDEX train_index, short rec_index, STATION_NO station)
{
	CTG_SingleTrain *ptrain=m_pWorkSchedule->GetSingleTrain(train_index);
	if(NULL == ptrain)
	{
		LogPredict(0,"����train_index %d �Ҳ����г�", train_index);
		return false;
	}
	const CTG_TrainRecord *prec = ptrain->GetTrainRecordNoDelete_Const(rec_index);
	if(NULL == prec) 
	{
		LogPredict(0,"����rec_index %d �Ҳ�����վ", rec_index);
		return false;
	}
	if(prec->GetStationNo() != station) 
	{
		LogPredict(0,"����rec_index %d �ҵ��ĳ�վ%d��ʵ�ʳ�վ%d��ͬ", 
			rec_index,prec->GetStationNo(),station);
		return false;
	}

	const CTG_TrainRecord* prerec = NULL;
	if(rec_index>0)
		prerec	= ptrain->GetTrainRecordNoDelete_Const(rec_index-1);
	const CTG_TrainRecord *precnext = ptrain->GetTrainRecordNoDelete_Const(rec_index+1);
	
	predict.train_index=ptrain->GetTrainIndex();
	predict.rec_index=prec->GetIndex();
	predict.station_no=prec->GetStationNo();
	
	if(ptrain->IsPassengerTrain())
		predict.passenger=1;
	else
		predict.passenger=2;

	if(prec->IsActualDepartTimeRecord())
		predict.depart_real=1;
	else
		predict.depart_real=0;

	if(prec->IsActualArriveTimeRecord())
		predict.arrive_real=1;
	else
		predict.arrive_real=0;

	predict.start=0;
	if(prerec)
	{
		predict.pre_station_no = prerec->GetStationNo();
		
		strcpy(predict.arrive_train_id, prec->m_data.arrive_train_id);
		predict.entry=prec->m_data.entry;
		
		predict.plan_arrive_time=prec->m_data.plan_arrive;
		predict.arrive_time=prec->m_data.arrive;

		predict.sort=prec->m_data.arrive;
	}
	else
	{
		if(prec->IsStartRec())
		{
			predict.start=1;

			predict.pre_station_no = 0;

			memset(predict.arrive_train_id, 0, sizeof(predict.arrive_train_id));
			predict.entry=0;

			predict.plan_arrive_time=0;
			predict.arrive_time=0;

			predict.sort=prec->m_data.depart;
		}
		else
		{
			WORD pre_station_no=0;
			WORD abs_num=0, dir;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(prec->GetStationNo(), prec->GetEntry(), &abs_num, &dir))
			{
				WORD station1, station2, entry1, entry2;
				if(GetTGBasicParam()->ConvertAbsnumToStationEntry(&station1, &entry1,
					&station2, &entry2, abs_num))
				{
					if(station1 == prec->GetStationNo())
						pre_station_no = station2;
					else if(station2 == prec->GetStationNo())
						pre_station_no = station1;
				}
			}

			predict.pre_station_no=pre_station_no;

			strcpy(predict.arrive_train_id, prec->m_data.arrive_train_id);
			predict.entry=prec->m_data.entry;
			
			predict.plan_arrive_time=prec->m_data.plan_arrive;
			predict.arrive_time=prec->m_data.arrive;

			predict.sort=prec->m_data.arrive;
		}
	}
	predict.neib_yugao_time=0;
	predict.tongyi_neib_time=0;
	predict.neib_cancel_time=0;

	if(precnext)
	{
		predict.next_station_no = precnext->GetStationNo();
		predict.next_entry      = precnext->GetEntry();

		strcpy(predict.depart_train_id, prec->m_data.depart_train_id);
		predict.exit=prec->m_data.exit;
		
		predict.plan_depart_time=prec->m_data.plan_depart;
		predict.depart_time=prec->m_data.depart;
	}
	else
	{
		if(prec->IsTerminalRec())
		{
			predict.start=2;
			predict.next_station_no = 0;
			predict.next_entry =0;
			memset(predict.depart_train_id, 0, sizeof(predict.depart_train_id));
			predict.exit				=	0;
			predict.plan_depart_time	=	0;
			predict.depart_time			=	0;
		}
		else
		{
			WORD abs_num=0, dir;
			if(GetTGBasicParam()->ConvertStationEntryToAbsnum(prec->GetStationNo(), prec->GetExit(), &abs_num, &dir))
			{
				WORD next_station_no=0;
				int  next_entry=0; 
				WORD station1, station2, entry1, entry2;
				if(GetTGBasicParam()->ConvertAbsnumToStationEntry(&station1, &entry1,
					&station2, &entry2, abs_num))
				{
					if(station1 == prec->GetStationNo())
					{
						next_station_no = station2;
						next_entry=entry2;
					}
					else if(station2 == prec->GetStationNo())
					{
						next_station_no = station1;
						next_entry=entry1;
					}
				}

				predict.next_station_no	=	next_station_no;
				predict.next_entry      =	next_entry;

				strcpy(predict.depart_train_id, prec->m_data.depart_train_id);
				predict.exit=prec->m_data.exit;
		        
				predict.plan_depart_time=prec->m_data.plan_depart;
				predict.depart_time=prec->m_data.depart;
			}
		}
	}
	predict.yugao_neib_time=0;
	predict.neib_tongyi_time=0;
	predict.cancel_neib_time=0;

	return true;
}

void CTrainDepartPredictHolder::ProcessStationReportTime(TRAIN_INDEX train_index, short rec_index, STATION_NO station, ULONG adjust_status, TIME arrive, TIME depart)
{
	CStationPredict* pstation = NULL;
	pstation = GetStPredictPlan(station);
	if(NULL == pstation)
	{
		LogPredict(0, "�Ҳ�����վ %d ��Ԥ��ƻ�, �޷�����ʵ�ʵ�", station);
		return;
	}
	TrainPredict predict;
	int idx=pstation->GetStplanByTrainIndex(train_index, rec_index, predict);
	if(idx==-1)
	{
		LogPredict(0,"����train_index %u rec_index %d �Ҳ����ƻ�, �޷�����ʵ�ʵ�", train_index, rec_index);
		return;
	}

	if(TG_IsActualArrivalTime(adjust_status) && pstation->m_stplan[idx].arrive_real==0)
	{
		pstation->m_stplan[idx].arrive_time = arrive;
		pstation->m_stplan[idx].arrive_real = 1;

		LogPredict(0,"[�Զ�Ԥ��] ���յ���վ %d ���ﱨ��, ���õ���Ϊʵ�ʵ�ɹ�, train_index %d rec_index %d arrive %d", 
				station, train_index, rec_index, arrive);

		pstation->SaveStplanToFile();
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}

	if(TG_IsActualDepartTime(adjust_status) && pstation->m_stplan[idx].depart_real==0)
	{
		pstation->m_stplan[idx].depart_time = depart;
		pstation->m_stplan[idx].depart_real = 1;

		LogPredict(0,"[�Զ�Ԥ��] ���յ���վ %d ��������, ���÷���Ϊʵ�ʵ�ɹ�, train_index %d rec_index %d depart %d", 
				station, train_index, rec_index, depart);

		pstation->SaveStplanToFile();
		NoticeMMIUpdateUI(WM_USER+1, 2, 0);
	}
}

void CTrainDepartPredictHolder::OutputPredict(const TrainPredict& predict)
{
	LogPredict(0,"�ƻ���ϸ����:"); 

	LogPredict(0,"��վ:%d �г�����:%u վ��:%d ʼ���յ�:%d �ͻ�:%d",
		predict.station_no, predict.train_index, predict.rec_index, predict.start, predict.passenger);
		
	LogPredict(0,"���:%d ����:%d ���ﳵ��:%s ��������:%s ͼ������:%u ����:%u ͼ������:%u ����:%u �����Ƿ�ʵ��:%d �����Ƿ�ʵ��:%d",
		predict.entry, predict.exit, predict.arrive_train_id, predict.depart_train_id, 
		predict.plan_arrive_time, predict.arrive_time, 
		predict.plan_depart_time, predict.depart_time, predict.arrive_real, predict.depart_real); 

	LogPredict(0,"ǰ��վ:%d ��վͬ��ʱ��:%u ��վԤ��ʱ��:%u ��վȡ��ʱ��:%u",
		predict.pre_station_no, predict.neib_tongyi_time, predict.neib_yugao_time, predict.neib_cancel_time); 
	
	LogPredict(0,"��վ:%d ��վ���:%d ͬ����վʱ��:%u Ԥ����վʱ��:%u ȡ����վʱ��:%u",
		predict.next_station_no, predict.next_entry, predict.tongyi_neib_time, predict.yugao_neib_time, predict.cancel_neib_time); 
}

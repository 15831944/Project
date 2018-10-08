#include "StdAfx.h"

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include ".\chezuhaomanger.h"
#include "TGDoc.h"
#include ".\dlg\CZHMultiEditDlg.h"

TRAIN_CHEZUHAO::TRAIN_CHEZUHAO()
{
	InitCheZuHao();
}

TRAIN_CHEZUHAO::~TRAIN_CHEZUHAO()
{
	InitCheZuHao();
}

void TRAIN_CHEZUHAO::InitCheZuHao()
{
	czh="";
	trains.clear();
	rect.SetRectEmpty();
}

bool TRAIN_CHEZUHAO::TrainIsJiaoLu(TRAIN_INDEX index)
{
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it = trains.begin();
	while(it != trains.end())
	{
		if(it->second.train_index==index)
			return true;
	}
	return false;
}

void TRAIN_CHEZUHAO::SetRect(int l, int t, int r, int b)
{
	if(l<rect.left)
		rect.left=l;
	if(t<rect.top)
		rect.top=t;
	if(r>rect.right)
		rect.right=r;
	if(b>rect.bottom)
		rect.bottom=b;
}

CCheZuHaoManger::CCheZuHaoManger(void)
{
	m_TrainCheZuHaoNum=0;
	for(int i=0; i<MAX_CZH_NUM; i++)
	{
		m_TrainCheZuHao[i].InitCheZuHao();
	}
	m_pDoc=NULL;
}

CCheZuHaoManger::~CCheZuHaoManger(void)
{
	m_TrainCheZuHaoNum=0;
	for(int i=0; i<MAX_CZH_NUM; i++)
	{
		m_TrainCheZuHao[i].InitCheZuHao();
	}
}

void CCheZuHaoManger::ClearAllTrainCheHao()
{
	m_TrainCheZuHaoNum=0;
	for(int i=0; i<MAX_CZH_NUM; i++)
	{
		m_TrainCheZuHao[i].InitCheZuHao();
	}
}

void CCheZuHaoManger::SetCZhRight(CTGDoc* pDoc)
{
	m_pDoc=pDoc;
}

bool CCheZuHaoManger::AppendCheZuHao(CTG_TRAIN& train)
{
	if(train.myTrainInfo.shift_index_string=="")
		return false;

	ULONG key=0;
	CString czh="";
	TRAIN_CZH_INFO info;
	if(!GenCZHInfoFromTrain(train, info, key, czh))
		return false;

	if(czh.IsEmpty())  
		return false;

	TRAIN_CHEZUHAO* pTrainCZH = GetCzhByCheZuHao(czh);
	// ���ճ���Ų��ҵ��ó����ڳ����
	if(pTrainCZH)
	{
		m_pDoc->sysprint(10, 0, "�г� %s(%u) ���ӵ� ����� %s", info.trainid, info.train_index, czh);
	
		pTrainCZH->trains.insert(make_pair(key, info));
	}
	else
	{
		pTrainCZH = GetNewCzh();
		if(pTrainCZH)
		{
			m_pDoc->sysprint(10, 0, "�����³���� %s.", czh);
			pTrainCZH->czh=czh;
			pTrainCZH->trains.insert(make_pair(key, info));
			m_pDoc->sysprint(10, 0, "�г� %s(%u) ���ӵ� ����� %s", info.trainid, info.train_index, czh);
		}
		else
		{
			m_pDoc->sysprint(10, 0, "�����³���� %s ʧ��, ���ܴ�� %s(%u)", czh, info.trainid, info.train_index);
			return false;
		}
	}
	
	return true;
}

void CCheZuHaoManger::NoticeCzhDlg()
{
	CString title1, title2;
	m_pDoc->GetChezuhaoDlgName(title1, title2);
	CWnd * pWnd=CWnd::FindWindow(NULL,title1);
	if(pWnd)
	{
		pWnd->SendMessage(MSG_UPDATE_CZH);
	}

	pWnd=CWnd::FindWindow(NULL,title2);
	if(pWnd)
	{
		pWnd->SendMessage(MSG_UPDATE_CZH);
	}
}

bool CCheZuHaoManger::DeleteCheZuHao(CTG_TRAIN& train)
{
	ULONG key=0;
	CString czh=train.myTrainInfo.shift_index_string;
	CString trainid=m_pDoc->GetTrainShowId(train);

	TRAIN_CHEZUHAO* pTrainCZH = GetCzhByTrainIndex(train.m_nTrainIndex);
	
	if(pTrainCZH!=NULL)
	{
		m_pDoc->sysprint(10, 0, "�����г����� %d �ҵ��г� %s ���ڳ���� %s", 
								  train.m_nTrainIndex, trainid, pTrainCZH->czh);

		if(DeleteFromCZHQueue(pTrainCZH, train.m_nTrainIndex))
		{
			m_pDoc->sysprint(10, 0, "�ӳ���� %s ��ɾ���г�:%s index:%d czh:%s, �ɹ�", 
								pTrainCZH->czh, trainid, train.m_nTrainIndex, czh);
		}
		else
		{
			m_pDoc->sysprint(10, 0, "�ӳ���� %s ��ɾ���г�:%s index:%d czh:%s, ʧ��", 
								pTrainCZH->czh, trainid, train.m_nTrainIndex, czh);
		}
		
		if(pTrainCZH->trains.size()<=0)
		{
			m_pDoc->sysprint(10, 0, "����� %s Ϊ��,���ճ����", pTrainCZH->czh);
			pTrainCZH->czh="";
			pTrainCZH->rect.SetRectEmpty();
		}
	}
	else
	{
		m_pDoc->sysprint(10, 0, "�����г����� %d �Ҳ����г� %s ���ڳ���� %s.�ӳ���ɾ���г�ʧ��", 
								  train.m_nTrainIndex, trainid, czh);
	}

	NoticeCzhDlg();
	
	return true;
}

bool CCheZuHaoManger::UpdateCheZuHao(CTG_TRAIN& train)
{
	ULONG key=0;
	CString czh="";
	TRAIN_CZH_INFO info;
	if(!GenCZHInfoFromTrain(train, info, key, czh))
		return true;

	TRAIN_CHEZUHAO* pTrainCZH = NULL;
	pTrainCZH = GetCzhByTrainIndex(train.GetTrainIndex());
	// �����г������ҵ��ó���ɾ��
	if(pTrainCZH)
	{
		m_pDoc->sysprint(10, 0, "�����г����� %d �ҵ��г� %s ���ڳ���� %s", 
								  info.train_index, info.trainid, pTrainCZH->czh);

		if(DeleteFromCZHQueue(pTrainCZH, train.GetTrainIndex()))
		{
			m_pDoc->sysprint(10, 0, "�ӳ���� %s ��ɾ�� %s �ɹ�", pTrainCZH->czh, info.trainid);
		}
		else
		{
			m_pDoc->sysprint(10, 0, "�ӳ���� %s ��ɾ�� %s ʧ��", pTrainCZH->czh, info.trainid);
		}
	
		if(pTrainCZH->trains.size()<=0)
		{
			m_pDoc->sysprint(10, 0, "����� %s Ϊ��,���ճ����", pTrainCZH->czh);
			pTrainCZH->czh="";
			pTrainCZH->rect.SetRectEmpty();
		}
	}
		
	// ����Ų�Ϊ��, ���´���ó�
	if(!czh.IsEmpty())
	{
		pTrainCZH = GetCzhByCheZuHao(czh);
		// ���ճ���Ų��ҵ��ó����ڳ����
		if(pTrainCZH)
		{
			m_pDoc->sysprint(10, 0, "�г� %s ���ӵ� ����� %s", info.trainid, czh);
			
			pTrainCZH->trains.insert(make_pair(key, info));
		}
		else
		{
			pTrainCZH = GetNewCzh();
			if(pTrainCZH)
			{
				m_pDoc->sysprint(10, 0, "�����³���� %s ��� %s", czh, info.trainid);

				pTrainCZH->czh=czh;
				pTrainCZH->trains.insert(make_pair(key, info));
			
			}
			else
			{
				m_pDoc->sysprint(10, 0, "�����³���� %s ʧ��, ���ܴ�� %s", czh, info.trainid);
				return false;
			}
		}
	}

	NoticeCzhDlg();
		
	return true;
}

bool CCheZuHaoManger::DeleteFromCZHQueue(TRAIN_CHEZUHAO* pTrainCZH, long train_index)
{
	if(NULL==pTrainCZH)
		return false;

	std::map<ULONG, TRAIN_CZH_INFO>::iterator it = pTrainCZH->trains.begin();

	while(it!=pTrainCZH->trains.end())
	{
		if(it->second.train_index == train_index)
		{
			pTrainCZH->trains.erase(it);
			return true;
		}
		it++;
	}

	return false;
}

// ���ó����۷�����
bool CCheZuHaoManger::SetTrainReturnID(std::map<ULONG, TRAIN_CZH_INFO>::iterator it, std::map<ULONG, TRAIN_CZH_INFO>::iterator next_it, CString& info)
{
	info="";
	bool bResult=false;
	CTG_TRAIN* pTrain=NULL, *pNextTrain=NULL;
	TgTRAIN_RECORD* last=NULL, *first=NULL;
	do
	{
		if(it->second.end_station != next_it->second.begin_station)
		{
			info.Format("�г� %s �յ���վ ���г� %s ʼ����վ ����ͬһ��վ�����������۷�����", it->second.trainid, next_it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
			break;
		}

		pTrain=m_pDoc->GetTrainByIndex(it->second.train_index);
		if(pTrain==NULL)
		{
			info.Format("�������� %d ���Ҳ����г� %s���޷��ó������۷�����", it->second.train_index, it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
			break;
		}
		
		pNextTrain=m_pDoc->GetTrainByIndex(next_it->second.train_index);
		if(pNextTrain==NULL)
		{
			info.Format("�������� %d ���Ҳ��������г� %s��%s �޷������۷�����", next_it->second.train_index, next_it->second.trainid, it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
			break;
		}

		last=pTrain->GetLastRecord();
		if(last==NULL)
			break;

		first=pNextTrain->GetFirstRecord();
		if(first==NULL)
			break;

		bResult=true;
	}while(0);
		
	if(!bResult)
	{
		if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
			return false;

		pTrain=m_pDoc->GetTrainByIndex(it->second.train_index);
		if(pTrain==NULL)
			return false;

		CTG_TRAIN train=*pTrain;  // ����ֱ�Ӹ��ڴ�����
		last=train.GetLastRecord();
		if(last==NULL)
			return false;
			
		if(IsFlagTerminal(last->flag))
		{
			if(last->arrive_train_id!=last->depart_train_id)
			{
				last->depart_train_id=last->arrive_train_id;
				m_newtrain_list.push_back(train);
			}

			CString text;
			text.Format("�г� %s �յ� �۷��������� %s", it->second.trainid, last->depart_train_id);
			m_pDoc->sysprint(10, 0, "%s", text);
		}
		return false;
	}

	if(IsFlagTerminal(last->flag) && IsFlagStart(first->flag))
	{
		if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
			return true;

		CTG_TRAIN train=*pTrain;   // ����ֱ�Ӹ��ڴ�����
		last=train.GetLastRecord();
		if(last==NULL)
			return false;

		if(m_pDoc->m_ptgctrl->mConfigTg.IsCrhStopStation(it->second.end_station))
		{
			if(last->arrive_train_id!=last->depart_train_id)
			{
				last->depart_train_id=last->arrive_train_id;
				m_newtrain_list.push_back(train);
			}
			CString text;
			text.Format("�г� %s �ڶ������յ� �����۷���������", it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", text);
		}
		else
		{
			if(last->depart_train_id!=first->depart_train_id)
			{
				last->depart_train_id=first->depart_train_id;
				m_newtrain_list.push_back(train);
			}

			CString text;
			text.Format("�г� %s ���յ��۷�������������Ϊ %s", it->second.trainid, last->depart_train_id);
			m_pDoc->sysprint(10, 0, "%s", text);
		}
	}
	else
	{
		if(IsFlagTerminal(last->flag))
		{
			info.Format("�г� %s ���յ�, �����г� %s ����ʼ��, �۷���������ʧ��", it->second.trainid, next_it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
		}
		else if(IsFlagStart(first->flag))
		{
			info.Format("�г� %s ��ʼ���������г� %s �����յ����۷���������ʧ��", next_it->second.trainid, it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
		}
		else
		{
			info.Format("�г� %s �����յ����г� %s ����ʼ�����������۷�����", it->second.trainid, next_it->second.trainid);
			m_pDoc->sysprint(10, 0, "%s", info);
		}
	}
	
	return true;
}

bool CCheZuHaoManger::SetReturnTrainId(TRAIN_CHEZUHAO* pTrainCZH, CStringArray& text)
{
	if(NULL==pTrainCZH)
		return false;

	text.RemoveAll();
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it, next_it;
	
	it = pTrainCZH->trains.begin();
	for(; it!=pTrainCZH->trains.end(); it++)
	{
		CString result="";
		next_it = it;
		next_it++;
		if(next_it!=pTrainCZH->trains.end())
		{
			if(next_it->second.stTime<=it->second.edTime)
			{
				result.Format("�г� %s �� %s �ĵ���ʱ�䲻�����۷��г�Ҫ���޷������۷�����", 
					it->second.trainid, next_it->second.trainid);
	
				m_pDoc->sysprint(10, 0, "%s", result);
				text.Add(result);
			}
		}
	}
	if(text.GetCount())
		return false;

	
	m_newtrain_list.clear();

	it = pTrainCZH->trains.begin();
	for(; it!=pTrainCZH->trains.end(); it++)
	{
		CString result="";

		next_it = it;

		next_it++;
		if(next_it!=pTrainCZH->trains.end())
		{
			SetTrainReturnID(it, next_it, result);
			if(!result.IsEmpty())
				text.Add(result);
		}
		else
		{
			if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
			{
				next_it=pTrainCZH->trains.begin();
				if(it != next_it)
				{
					SetTrainReturnID(it, next_it, result);
					if(!result.IsEmpty())
						text.Add(result);
				}
			}
			else if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
			{
				;//���ı䳵��ʲôҲ����
			}
			else
			{
				CTG_TRAIN* pObjTrain=m_pDoc->GetTrainByIndex(it->second.train_index);
				if(pObjTrain==NULL)
					break;
				
				CTG_TRAIN train=*pObjTrain;   // ����ֱ�Ӹ��ڴ�����
				TgTRAIN_RECORD* last=train.GetLastRecord();
				if(last==NULL)
					break;
				if(IsFlagTerminal(last->flag))
				{
					if(last->arrive_train_id!=last->depart_train_id)
					{
						last->depart_train_id=last->arrive_train_id;
						
						m_newtrain_list.push_back(train);
					}

					result.Format("�г� %s �յ� ���һ�г������ó������� %s", m_pDoc->GetTrainShowId(train), 
												last->depart_train_id);
					m_pDoc->sysprint(10, 0, "%s", result);
				}
			}
			break;
		}
	}

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
	{
		for(int i=0; i<m_newtrain_list.size(); i++)
		{
			m_pDoc->SendUpdateTrain(m_newtrain_list[i]);
		}
	}
	m_newtrain_list.clear();

	return true;
}

bool CCheZuHaoManger::GenCZHInfoFromTrain(CTG_TRAIN& train_org, TRAIN_CZH_INFO& info, ULONG& key, CString& czh)
{
	CTG_TRAIN train=train_org;
	if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		m_pDoc->SetBasicTrainDate(&train);

	const TgTRAIN_RECORD* first=train.GetFirstRecord();
	const TgTRAIN_RECORD* last=train.GetLastRecord();
	if(first==NULL || last==NULL)
		return false;

	key = 0;
	if(IsFlagStart(first->flag))
		key=first->depart;
	else
		key=first->arrive;

	czh=train.myTrainInfo.shift_index_string;
	CString trainid=first->depart_train_id;
	memset(info.trainid, 0, 10);
	strncpy(info.trainid, trainid, 9);
	info.trainid[9]=0;

	info.train_index=train.GetTrainIndex();
	info.begin_station=first->station;
	info.end_station=last->station;
	
	int bstation=0, estation=0;
	info.stTime=0;
	if(IsFlagStart(first->flag))
	{
		info.stTime=first->depart;
		bstation=first->station;
	}
	else
	{
		info.stTime=first->arrive;
		bstation=0;
	}
		
	info.edTime=0;
	if(IsFlagTerminal(last->flag))
	{
		info.edTime=last->arrive;
		estation=last->station;
	}
	else
	{
		info.edTime=last->depart;
		estation=0;
	}

	if(TG_TYPE::BASIC_TG != m_pDoc->GetTgType())
	{
		TIME nBeginTime=m_pDoc->GetBeginTime()+3*3600;  // ����Χ ԭΪ12*3600
		if(info.edTime<nBeginTime && info.stTime<nBeginTime)
			return false;
	}
	return true;
}

TRAIN_CHEZUHAO* CCheZuHaoManger::GetCzhByCheZuHao(const CString& czh)
{
	if(czh.IsEmpty())
		return NULL;
	
	for(UINT i=0; i<GetTrainCheZuHaoNum(); i++)
	{
		if(m_TrainCheZuHao[i].czh == czh)
		{
			return &m_TrainCheZuHao[i];
		}
	}
	
	return NULL;
}

void CCheZuHaoManger::DeleteCzhByCheZuHao(const CString& czh)
{
	if(czh!="")
	{
		for(UINT i=0; i<MAX_CZH_NUM; i++)
		{
			if(m_TrainCheZuHao[i].czh == czh)
			{
				m_TrainCheZuHao[i].InitCheZuHao();
			}
		}
	}
}

TRAIN_CHEZUHAO* CCheZuHaoManger::GetCzhByTrainIndex(TRAIN_INDEX trainIndex)
{
	for(UINT i=0; i<GetTrainCheZuHaoNum(); i++)
	{
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it = m_TrainCheZuHao[i].trains.begin();
		while(it!=m_TrainCheZuHao[i].trains.end())
		{
			if(trainIndex == it->second.train_index)
			{
				return &m_TrainCheZuHao[i];
			}
			it++;
		}
	}
	
	return NULL;
}

UINT CCheZuHaoManger::GetTrainCheZuHaoNum()
{
	return m_TrainCheZuHaoNum;
}

int  CCheZuHaoManger::GetTrainCZHText(CStringArray& array)
{
	array.RemoveAll();
	for(UINT i=0; i<GetTrainCheZuHaoNum(); i++)
	{
		CString czh=m_TrainCheZuHao[i].czh;
		if(czh.IsEmpty())
			continue;
		
		array.Add(czh);
	}

	return array.GetCount();
}

TRAIN_CHEZUHAO* CCheZuHaoManger::GetNewCzh()
{
	for(int i=0; i<MAX_CZH_NUM; i++)
	{
		if(m_TrainCheZuHao[i].czh=="")
		{
			m_TrainCheZuHao[i].trains.clear();
			m_TrainCheZuHao[i].rect.SetRectEmpty();

			if(i>=m_TrainCheZuHaoNum)
				m_TrainCheZuHaoNum++;

			return &m_TrainCheZuHao[i];
		}
	}

	return NULL;
}


// ��鳵��������ϵ
bool CCheZuHaoManger::CheckTrainCZHValid(CTG_TRAIN* train, CString& str)
{
	CString czh=train->myTrainInfo.shift_index_string;
	if(czh.IsEmpty() || train->GetRecordNum()<=0)
		return true;

	ULONG key=0;
	TRAIN_CZH_INFO info;
	if(!GenCZHInfoFromTrain(*train, info, key, czh))
		return true;

	TIME begin=0, end=0;
	TRAIN_KEY train_index=train->GetTrainIndex();

	TRAIN_CHEZUHAO* pTrainCZH = GetCzhByCheZuHao(czh);
	if(pTrainCZH==NULL)
		return true;
	
	CStringArray arytrainid;
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it, preit, nextit;
	
	for(it = pTrainCZH->trains.begin();it!=pTrainCZH->trains.end(); it++)
	{
		if(it->second.train_index==train_index)
		{
			preit = it;
			nextit = it;
			nextit++;

			if(it != pTrainCZH->trains.begin())
			{
				preit--;
				// �Ƚ�ǰһ��
				if(preit->second.edTime >= info.stTime)
				{
					str.Format("�г� %s �� %s ����ͬһ�����, %s������Ӧ����%s", 
						info.trainid, preit->second.trainid, info.trainid, preit->second.trainid);
					return false;
				}
			}

			// �ȽϺ�һ��
			if(nextit != pTrainCZH->trains.end())
			{
				if(nextit->second.stTime <= info.edTime)
				{
					str.Format("�г� %s �� %s ����ͬһ�����, %s�յ���Ӧ����%s", 
						info.trainid, nextit->second.trainid, info.trainid, nextit->second.trainid);
					return false;
				}
			}
		}
	}

	return true;
}

int  CCheZuHaoManger::GetSameCZTrainIndexList(TRAIN_CHEZUHAO* pSelTrainCZH, long nCurrentSelTrainIndex, std::vector<long>& list, long& nLeftTime, int& nLeftStation, CString& trainid)
{
	nLeftTime=0;
	nLeftStation=0;
	list.clear();
	TRAIN_CHEZUHAO* pTrainCZH = pSelTrainCZH;
	if(pTrainCZH==NULL)
		return 0;
	
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it;
	
	bool bFind=false;
	for(it = pTrainCZH->trains.begin();it!=pTrainCZH->trains.end(); it++)
	{
		if(bFind)
		{
			list.push_back(it->second.train_index);
		}
		else
		{
			if(it->second.train_index==nCurrentSelTrainIndex)
			{
				bFind=true;
				list.push_back(it->second.train_index);
			}
			else
			{
				trainid=it->second.trainid;
				nLeftTime=it->second.edTime;
				nLeftStation=it->second.end_station;
			}
		}
	}

	return list.size();
}

long CCheZuHaoManger::GetCzhTrainStartTm(TRAIN_CHEZUHAO* pSelTrainCZH, long index, CString& trainid,int& nRightStation)
{
	trainid="";
	nRightStation=0;
	TRAIN_CHEZUHAO* pTrainCZH = pSelTrainCZH;
	if(pTrainCZH==NULL)
		return 0;

	std::map<ULONG, TRAIN_CZH_INFO>::iterator it;
	for(it = pTrainCZH->trains.begin();it!=pTrainCZH->trains.end(); it++)
	{
		if(it->second.train_index==index)
		{
			trainid=it->second.trainid;
			nRightStation=it->second.begin_station;
			return it->second.stTime;
		}
	}

	return 0;
}

void CCheZuHaoManger::OutputCZRelation()
{
	CString tmp;
	CStdioFile file;
	CString strFileName;
	strFileName.Format("%s\\czh_relation.xml",  m_pDoc->GetDataPath());
	DeleteFile(strFileName); // ��ɾ��
	if(file.Open(strFileName, CFile::modeCreate|CFile::modeWrite))
	{
		CTime tm=CTime::GetCurrentTime();
		file.WriteString("<?xml version=\"1.0\" encoding=\"GB2312\" ?>\n");
		tmp.Format("<root tm=\"%d\">\n", tm.GetTime());  // ��¼�����ϵʱ��
		file.WriteString(tmp);

		for(UINT i=0; i<m_TrainCheZuHaoNum; i++)
		{
			TRAIN_CHEZUHAO& czh = m_TrainCheZuHao[i];
			if(czh.czh=="")  continue;
				
			tmp.Format("\t<CZH name=\"%s\">\n", GetValidStr(czh.czh));
			file.WriteString(tmp);
			std::map<ULONG, TRAIN_CZH_INFO>::iterator it=czh.trains.begin();
			for(; it!=czh.trains.end(); it++)
			{
				tmp.Format("\t\t<TRAIN name=\"%s\" index=\"%d\"/>\n", GetValidStr(it->second.trainid), it->second.train_index);
				file.WriteString(tmp);
			}
			file.WriteString("\t</CZH>\n");
		}

		file.WriteString("</root>\n");
		file.Close();
	}
}

bool CCheZuHaoManger::GetReturnTrainID(CTG_TRAIN* single, CString& strReturnID)
{
	strReturnID="";
	if(single==NULL)
		return false;
 
	int train_index=single->m_nTrainIndex;
	CString czh=single->myTrainInfo.shift_index_string;
	TRAIN_CHEZUHAO* pTrainsCZH=NULL;
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
	{
		pTrainsCZH=GetCzhByCheZuHao(czh);
	}
	else if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		pTrainsCZH=GetCzhByTrainIndex(train_index);
	}
	if(pTrainsCZH!=NULL)
	{
		TRAIN_CHEZUHAO TrainsCZH=*pTrainsCZH;
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it = TrainsCZH.trains.begin();
		while(it!=TrainsCZH.trains.end())
		{
			if(train_index == it->second.train_index)
			{
				int station1=it->second.end_station;
				it++;
				if(it!=TrainsCZH.trains.end())
				{
					if(station1==it->second.begin_station)
					{
						strReturnID=it->second.trainid;
					}
				}
				break;
			}
			it++;	
		}
	}

	if(strReturnID!="")
		return true;
	return false;
}

bool CCheZuHaoManger::GenCZHInfoFromTrain(CTG_TRAIN& train_org, TRAIN_CZH_INFO& info)
{
	CTG_TRAIN train=train_org;
	if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		m_pDoc->SetBasicTrainDate(&train);

	const TgTRAIN_RECORD* first=train.GetFirstRecord();
	const TgTRAIN_RECORD* last=train.GetLastRecord();
	if(first==NULL || last==NULL)
		return false;

	CString trainid=first->depart_train_id;
	memset(info.trainid, 0, 10);
	strncpy(info.trainid, trainid, 9);
	info.trainid[9]=0;

	info.train_index=train.GetTrainIndex();
	info.begin_station=first->station;
	info.end_station=last->station;
	
	info.stTime=0;
	if(IsFlagStart(first->flag))
		info.stTime=first->depart;
	else
		info.stTime=first->arrive;
		
	info.edTime=0;
	if(IsFlagTerminal(last->flag))
		info.edTime=last->arrive;
	else
		info.edTime=last->depart;

	if(TG_TYPE::BASIC_TG != m_pDoc->GetTgType())
	{
		TIME nBeginTime=m_pDoc->GetBeginTime()+3*3600;  // ����Χ ԭΪ12*3600
		if(info.edTime<nBeginTime && info.stTime<nBeginTime)
			return false;
	}
	return true;
}

bool CCheZuHaoManger::AppendCheZuHaoByRelation(CTG_TRAIN& train, int key, CString& czh)
{
	TRAIN_CZH_INFO info;
	if(!GenCZHInfoFromTrain(train, info))
		return false;

	if(czh.IsEmpty())  
		return false;

	TRAIN_CHEZUHAO* pTrainCZH = GetCzhByCheZuHao(czh);
	// ���ճ���Ų��ҵ��ó����ڳ����
	if(pTrainCZH)
	{
		m_pDoc->sysprint(10, 0, "�г� %s ���ӵ� ����� %s", info.trainid, czh);
	
		pTrainCZH->trains.insert(make_pair(key, info));
	}
	else
	{
		pTrainCZH = GetNewCzh();
		if(pTrainCZH)
		{
			m_pDoc->sysprint(10, 0, "�����³���� %s ��� %s", czh, info.trainid);
			pTrainCZH->czh=czh;
			pTrainCZH->trains.insert(make_pair(key, info));
		
		}
		else
		{
			m_pDoc->sysprint(10, 0, "�����³���� %s ʧ��, ���ܴ�� %s", czh, info.trainid);
			return false;
		}
	}
	
	return true;
}
	
void CCheZuHaoManger::UpdateCheZuHaoByRelation(CTG_TRAIN& train_org)
{
	TRAIN_CHEZUHAO* pTrainCZH = GetCzhByTrainIndex(train_org.m_nTrainIndex);
	// ���ճ���Ų��ҵ��ó����ڳ����
	if(pTrainCZH)
	{
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it=pTrainCZH->trains.begin();
		for(;it!=pTrainCZH->trains.end();it++)
		{
			if(it->second.train_index==train_org.m_nTrainIndex)
			{
				CTG_TRAIN train=train_org;

				if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
					m_pDoc->SetBasicTrainDate(&train);

				const TgTRAIN_RECORD* first=train.GetFirstRecord();
				const TgTRAIN_RECORD* last=train.GetLastRecord();
				if(first==NULL || last==NULL)
				{
					m_pDoc->sysprint(10, 0, "�����г������ϵ���г� %s ��ʱ��ʧ��,�޷���ȡ��βվ��¼", 
							train_org.GetTrainId());
					return;
				}

				it->second.begin_station=first->station;
				it->second.end_station=last->station;
				
				if(IsFlagStart(first->flag))
					it->second.stTime=first->depart;
				else
					it->second.stTime=first->arrive;
				
				if(IsFlagTerminal(last->flag))
					it->second.edTime=last->arrive;
				else
					it->second.edTime=last->depart;
				
				m_pDoc->sysprint(10, 0, "�����г������ϵ %s ���г� %s ��ʱ��", 
							pTrainCZH->czh, train_org.GetTrainId());
				break;
			}
		}
		if(it==pTrainCZH->trains.end())
		{
			m_pDoc->sysprint(10, 0, "�����г������ϵ���г� %s ��ʱ��ʧ��,�����ϵ��û�иó�", 
				train_org.GetTrainId());
		}
	}
}

CTG_TRAIN* CCheZuHaoManger::GetNextTrain(CTG_TRAIN* ptrain)
{
	DWORD next_train_index=0U;
	DWORD train_index=ptrain->m_nTrainIndex;
	CString czh=ptrain->myTrainInfo.shift_index_string;
	TRAIN_CHEZUHAO* pTrainsCZH=NULL;
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
	{
		pTrainsCZH=GetCzhByCheZuHao(czh);
	}
	else if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		pTrainsCZH=GetCzhByTrainIndex(train_index);
	}
	if(pTrainsCZH!=NULL)
	{
		TRAIN_CHEZUHAO TrainsCZH=*pTrainsCZH;
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it = TrainsCZH.trains.begin();
		while(it!=TrainsCZH.trains.end())
		{
			if(train_index == it->second.train_index)
			{
				int station1=it->second.end_station;
				it++;
				if(it!=TrainsCZH.trains.end())
				{
					if(station1==it->second.begin_station)
					{
						next_train_index=it->second.train_index;
					}
				}
				break;
			}
			it++;	
		}
	}

	if(0U != next_train_index)
		return m_pDoc->GetTrainByIndex(next_train_index);
	return NULL;
}

CTG_TRAIN* CCheZuHaoManger::GetPreTrain(CTG_TRAIN* ptrain)
{
	DWORD pre_train_index=0U;
	DWORD train_index=ptrain->m_nTrainIndex;
	CString czh=ptrain->myTrainInfo.shift_index_string;
	TRAIN_CHEZUHAO* pTrainsCZH=NULL;
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
	{
		pTrainsCZH=GetCzhByCheZuHao(czh);
	}
	else if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		pTrainsCZH=GetCzhByTrainIndex(train_index);
	}
	if(pTrainsCZH!=NULL)
	{
		TRAIN_CHEZUHAO TrainsCZH=*pTrainsCZH;
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it = TrainsCZH.trains.begin();
		std::map<ULONG, TRAIN_CZH_INFO>::iterator pre_it = it;
		while(it!=TrainsCZH.trains.end())
		{
			if(train_index == it->second.train_index)
			{
				if(it!=TrainsCZH.trains.begin())
				{
					int station1=pre_it->second.end_station;
					if(station1==it->second.begin_station)
					{
						pre_train_index=pre_it->second.train_index;
					}
				}
				break;
			}
			pre_it=it;
			it++;	
		}
	}
	if(0U != pre_train_index)
		return m_pDoc->GetTrainByIndex(pre_train_index);
	return NULL;
}


#include "stdafx.h"
#include "..\basemsg\clbth.h"
#include "tgx.h"
#include "if.h"
#include "sysshare.h"
#include "tg_schedule_base.h"
#include "schd_tgmmi.h"
#include "tg_binxml.h"
#include "tg_work_schdule_holder.h"
#include "tgmsg.h"
#include "function.h"
#include "tg_addtrain_logic.h"
#include "TgDocument.h"
extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
CWintgViewMMI::CWintgViewMMI()
{
	m_nTGUpdateFlag=0;
	m_pTGCtrl=NULL;
	m_pSchdHolder=NULL;
	m_bEnableFreeTrainOper=TRUE;
}

void CWintgViewMMI::SetPredictHolder(CTrainDepartPredictHolder *pPredictServer)
{
	m_pPredictServer = pPredictServer;
	if(m_pPredictServer==NULL)
		return;

	STATION_NO predict_station[65];
	memset(predict_station, 0, sizeof(predict_station));
	int station_count=m_pPredictServer->GetPredictStationList(predict_station, 64);
	
	for(int i=0; i<station_count; i++)
	{
		if(predict_station[i]==0)
			continue;
		map_predict_station[predict_station[i]]=1;
	}
}

bool CWintgViewMMI::IsPredictStation(STATION_NO station)
{
	std::map<STATION_NO, int>::iterator it= map_predict_station.find(station);
	if(it!= map_predict_station.end())
		return true;
	return false;
}

void CWintgViewMMI::ReportTime(const CTG_SingleTrain& srctrain)
{
	if(m_pPredictServer==NULL)
		return;

	TRAIN_INDEX train_index=srctrain.GetTrainIndex();
	for(int rec_index=0; rec_index<srctrain.GetTrainRecordCount(); rec_index++)
	{
		const CTG_TrainRecord *prec = srctrain.GetTrainRecordByRecIndex_Const(rec_index);
		if(prec==NULL)
			continue;

		STATION_NO station = prec->m_data.station;
		if(IsPredictStation(station))
		{
			ULONG adjust_status=prec->m_data.adjust_status;
			if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
			{
				TIME arrive=prec->GetArriveTime();
				TIME depart=prec->GetDepartTime();
				
				m_pPredictServer->ProcessStationReportTime(train_index, rec_index, station, adjust_status, arrive, depart);
			}
		}
	}
}

void  CWintgViewMMI::OutprintCZHRelation()
{
	m_pTGCtrl->SetTrainDisplayFlag(TRAIN_DISPLY::GEN_TRAINS_RELATION, 0);
}

BOOL WriteGenernalXMLFile(const char *xml,const char *filepath);
BOOL CWintgViewMMI::UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	if(srctrain.GetTrainRecordCount()<=0)  // 521�ƻ��Ż��޸�
		return DeleteTrain(schd,srctrain,source_flag);

	m_pTGCtrl->UpdateTrainOCX(srctrain);
	ReportTime(srctrain);
	if(m_pTGCtrl->IsBatchStatus()==FALSE)
		m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_TRAIN,(WPARAM)srctrain.GetTrainIndex());
	return TRUE;
}


BOOL CWintgViewMMI::UpdateTrainInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	return UpdateTrain(schd,srctrain,source_flag);
}


BOOL CWintgViewMMI::UpdateTrainRecords(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	return UpdateTrain(schd,srctrain,source_flag);
}


BOOL CWintgViewMMI::UpdateEngineInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	return UpdateTrain(schd,srctrain,source_flag);
}

BOOL CWintgViewMMI::UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag)
{
	m_pTGCtrl->UpdateMarkOCX(mark);
	if(m_pTGCtrl->IsBatchStatus()==FALSE)
		m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_MARK,(WPARAM)mark.GetBlockIndex());

	// �¼�����18: ����ʩ������
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;
		data.nEventID =  EVENT_GPC_MODIFY_MARK;
		data.SetKey(KeyTrainIndex, mark.GetBlockIndex());
		data.SetKey(KeyMarkType, mark.GetMarkType());
		data.SetKey(KeyMarkStartTime, mark.GetStartTime());
		data.SetKey(KeyMarkEndTime, mark.GetEndTime());

		
		SendBMsg(&data, eventEntity.nEntityId);
	}
	return TRUE;
}


///////// DELETE
BOOL CWintgViewMMI::DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	m_pTGCtrl->DeleteTrainOCX(srctrain.GetTrainIndex(),m_nTGUpdateFlag);
	m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_TRAIN,(WPARAM)srctrain.GetTrainIndex());

	// �¼�����18: ɾ���г�
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;
		data.nEventID =  EVENT_GPC_DELETE_TRAIN;
		data.SetKey(KeyTrainNo, srctrain.GetTrainID());
		data.SetKey(KeyTrainIndex, srctrain.GetTrainIndex());

		SendBMsg(&data, eventEntity.nEntityId);
	}
	return TRUE;
}

BOOL CWintgViewMMI::DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag)
{
	m_pTGCtrl->DeleteMarkOCX(mark.GetBlockIndex());
	m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_MARK,(WPARAM)mark.GetBlockIndex());

	// �¼�����18: ɾ��ʩ������
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;
		data.nEventID =  EVENT_GPC_DELETE_MARK;
		data.SetKey(KeyTrainIndex, mark.GetBlockIndex());

		SendBMsg(&data, eventEntity.nEntityId);
	}
	return TRUE;
}

BOOL CWintgViewMMI::DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag)
{
	return FALSE;
}


///////// APPEND
TRAIN_INDEX CWintgViewMMI::AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	m_pTGCtrl->AppendTrainOCX(srctrain);
	if(m_pTGCtrl->IsBatchStatus()==FALSE)
		m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_TRAIN,(WPARAM)srctrain.GetTrainIndex());

	// �¼�����18: �����г�
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;
		data.nEventID =  EVENT_GPC_NEW_TRAIN;
		data.SetKey(KeyTrainNo, srctrain.GetTrainID());
		data.SetKey(KeyTrainIndex, srctrain.GetTrainIndex());

		SendBMsg(&data, eventEntity.nEntityId);
	}
	return srctrain.GetTrainIndex();
}


TRAIN_INDEX CWintgViewMMI::AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag)
{
	m_pTGCtrl->AppendMarkOCX(mark);
	if(m_pTGCtrl->IsBatchStatus()==FALSE)
		m_pParentWnd->PostMessage(UM_TG_OBJECT_CHANGED,(WPARAM)TG_OBJ_MARK,(WPARAM)mark.GetBlockIndex());

	// �¼�����18: ����ʩ������
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;
		data.nEventID =  EVENT_GPC_NEW_MARK;
		data.SetKey(KeyTrainIndex, mark.GetBlockIndex());
		data.SetKey(KeyMarkType, mark.GetMarkType());
		data.SetKey(KeyMarkStartTime, mark.GetStartTime());
		data.SetKey(KeyMarkEndTime, mark.GetEndTime());

		SendBMsg(&data, eventEntity.nEntityId);
	}
	return mark.GetBlockIndex();
};

BOOL CWintgViewMMI::DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag){return FALSE;};
BOOL CWintgViewMMI::DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag){return FALSE;};
////////  LOAD
BOOL CWintgViewMMI::LoadAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pIn){return FALSE;};
//////////////////////////// ��� ////////////////////////////
////////  COPY
BOOL CWintgViewMMI::CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag){return FALSE;};

CTG_TrainScheduleBase *CWintgViewMMI::GetDataStore(const CTG_ScheduleInfo &source_info)
{
	int i;
	BOOL bOnly=FALSE;
	CTG_TrainScheduleBase *ponly=NULL;
	for(i=0;i<DATA_SOURCE_NUM;i++)
	{
		if(m_pDataStore[i])
		{
			CTG_ScheduleInfo info;
			m_pDataStore[i]->GetScheduleInfo(info); 
			if(info.m_nSectionID ==source_info.m_nSectionID  && info.m_nType==source_info.m_nType)
				return m_pDataStore[i];
			if(!bOnly)
			{
				ponly=m_pDataStore[i];
				bOnly=TRUE;
			}
			else
				ponly=NULL;
		}
	}
	if(ponly)  // ��ʹ�ÿղ�������ʱ������Ψһ��һ��data, һ�����ڽ���
	{
		if(source_info.m_nSectionID==0 &&  source_info.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
			return ponly;
	}
	return NULL;
}

BOOL   CWintgViewMMI::UpdateTrain_ByBasicInfo(CTG_SingleTrain& train)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;

	TRAIN_INDEX index=train.GetTrainIndex(); 
	CTG_SingleTrain *prev=pData->GetSingleTrain(index);
	if(prev)
	{
		prev->SetUpdateTime(time(NULL));
		
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(m_schd_info.m_nSectionID, &train);
		CTgDocument::ResetGoodsTrainStopTech(&train);

		pData->UpdateTrain(m_schd_info, train,SYCFLAG_NULL);
		return TRUE;
	}
	return FALSE;
}

BOOL   CWintgViewMMI::UpdateObject_ByProperty(const CTG_LogicObject *pobj) // Ŀǰ�������Դ����޸�
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CMMIOperFlag oper_flag;
		CTG_SingleTrain train=*(const CTG_SingleTrain *)pobj;
		ProcessMMIOperation(TG_OPER_UPDATE, train, oper_flag, train.GetTrainIndex());
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		TRAIN_INDEX index=((const CTG_SingleMark *)pobj)->GetBlockIndex(); 
		CTG_SingleMark *prev=pData->GetSingleMark(index);
		if(prev)
		{
			prev->SetUpdateTime(time(NULL));
			pData->UpdateMark(m_schd_info, *(CTG_SingleMark *)pobj,SYCFLAG_NULL);
		}
	}
	return TRUE;
}

bool CWintgViewMMI::IsTrainHasRelation(TRAIN_INDEX train_index, TRAIN_INDEX prev_train_index, CTG_TrainScheduleBase *pData)
{
	if(!GetTGFunctionConfig()->IsEnableNewCzh() || pData==NULL)
		return false;
		
	CTG_SingleTrain *pnexttrain=pData->GetNextTrainByPrevTrainIndex(train_index);
	if((pnexttrain!=NULL) || (prev_train_index!=0))
	{
		return true;
	}
	return false;
}

void CWintgViewMMI::RemoveTrainRelations(CTG_SingleTrain& train, CTG_TrainScheduleBase *pData)
{
	if(!GetTGFunctionConfig()->IsEnableNewCzh() || pData==NULL)
		return;

	// ���������ǰ������
	train.SetPrevTrainIndex(0);
	train.SetPrevShiftTrainIndex(0);
	train.SetPrevBasicTrainIndex(0);

	GpcLogTrainMessage(0, 0, "��� %s(%u) ��ǰ���Ĺ�����ϵ.", 
					train.GetTrainID(), train.GetTrainIndex());
	// ���������󳵹���
	CTG_SingleTrain *pnexttrain=pData->GetNextTrainByPrevTrainIndex(train.GetTrainIndex());
	if(pnexttrain)
	{
		CTG_SingleTrain nexttrain=*pnexttrain;
		
		GpcLogTrainMessage(0, 0, "��� %s(%u)�ͺ� %s(%u)�Ĺ�����ϵ.", 
		train.GetTrainID(), train.GetTrainIndex(),
		nexttrain.GetTrainID(), nexttrain.GetTrainIndex());

		CTime cur_tm=CTime::GetCurrentTime(); 
		nexttrain.SetPrevTrainIndex(0);
		nexttrain.SetUpdateTime(cur_tm.GetTime());
		pData->UpdateTrain(m_schd_info,nexttrain,SYCFLAG_NULL);
	}
}

BOOL CWintgViewMMI::ProcessMMIOperation(TG_OPER_TYPE op_type,CTG_SingleTrain &train,CMMIOperFlag &oper_flag,TRAIN_INDEX train_index)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;

	
	// ���÷����г�
	if(op_type == TG_OPER_ADD || op_type == TG_OPER_UPDATE)
	{
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(m_schd_info.m_nSectionID, &train);
		CTgDocument::ResetGoodsTrainStopTech(&train);
		if(m_schd_info.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			train.SetPrevBasicTrainIndex(train.GetPrevTrainIndex()); // �˹���������ͼ�г�,���û���ͼǰ���г�����
		}
	}

	///////////////////////////////////////////////////////////////////////////
	BOOL qualify_runtime=FALSE; // �Զ�У������ʱ��
	CTime cur_tm=CTime::GetCurrentTime(); 
	switch(op_type)
	{
	case TG_OPER_ADD:
		{
			train.SetTrainIndex(train_index); //��������,Ĭ��INDEX
			train.SetGPCTrainIndex(pData->GetAvaGPCIndex());
			train.SetTrainRecordCount(train.GetTrainRecordCount());
			m_pSchdHolder->GenTrain_FreeDraw(train,m_schd_info,qualify_runtime,oper_flag.m_nRun_type); // train���ǽ������ɵĳ�ʼ����
			this->SetFreedrawTrainPlanTime(train);
			train.SetUpdateTime(cur_tm.GetTime()); 
			
			{
				TRAIN_INDEX newtrainindex=pData->AppendTrain(m_schd_info,train,SYCFLAG_NULL);//GetInterfaceFlag());
				if(newtrainindex!=NULL_TRAININDEX)
				{
					CTG_SingleTrain *prev=pData->GetSingleTrain(newtrainindex);
					if(prev)
					{
						// ��ʱ�����ϲ�,��ʱ���߲���ˢ�µ�����
						if(m_pTGCtrl)
						{
							GpcLogTrainMessage(0, 0, "��TGOCX�������ɻ������Ӹ��� trainID %s index %ld", prev->GetTrainID(), prev->GetTrainIndex());
						
							m_pTGCtrl->AppendTrainOCX(*prev);
						}
						PushOper(prev,op_type);
					}
				}
			}
		}
		break;
	case TG_OPER_UPDATE:
		{
			CTG_SingleTrain *prev=pData->GetSingleTrain(train.GetTrainIndex());
			if(!prev)
				break;

			//////// �����ʱ�ڴ��������Ѿ������������£�����ʾ�û������޸ģ�������ܶ��� 2010.6.22
			if(!prev->CheckSyncStatus(GetInterfaceFlag()))
			{
				 //m_pParentWnd->MessageBox("���ղŲ���ʱ�������Ѿ������������£������²���");
				 GpcLogTrainMessage(0, 0, "�û�����ʱ���������������ݣ���Ҫ���²��� trainID %s index %ld", prev->GetTrainID(), prev->GetTrainIndex());
				 prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
				 break;
			}

			/// �����վ��һ��,��ɾ�������! 20090818
			if(prev->GetTrainRecord(0) && train.GetTrainRecord(0) && prev->GetTrainRecord(0)->GetStationNo()!=train.GetTrainRecord(0)->GetStationNo())
			{
				ProcessMMIOperation(TG_OPER_DELETE,train,oper_flag,prev->GetTrainIndex()); // ���ҵݹ���� 20090818

				// ÿ���˲���֮���жϾ�·�Ƿ���ͬ
				if(GetTGFunctionConfig()->IsZeroGIDPasswayChange() && (train.GetShiftTrainIndex()!=0))
				{
					if(!prev->IsSamePathway(train))
					{
						GpcLogTrainMessage(0, 0, "�г�%s(%u)�ľ�·��ԭ����ͬ���ȫ��ID.", train.GetTrainID(), train.GetTrainIndex());
						train.SetShiftTrainIndex(0);
						train.SetFromShift(FALSE);
					}
				}
				ProcessMMIOperation(TG_OPER_ADD,train,oper_flag,NULL_TRAININDEX); // ���ҵݹ���� 20090818
				break;
			}
			//////////////////////////////////////////
			if(oper_flag.nFlag == CMMIOperFlag::CHANGE_TRAINID || oper_flag.nFlag == CMMIOperFlag::HAT_TRAINID)
			{
				if(!m_bEnableFreeTrainOper)
				{
					// �����޸��հ�ƻ�����
					if(prev->GetShiftTrainIndex()!=NULL_TRAININDEX)
					{
						m_pParentWnd->MessageBox("�����޸��հ�ƻ�����!");
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
				}

				if(IsTrainHasRelation(train.GetTrainIndex(), train.GetPrevTrainIndex(), pData))
				{
					CString str="�޸ĵĳ���ϵͳ������г��Ĺ�����ϵ,��ȷʵҪ�޸ĳ�����?\nȷ���޸ĳ���,���������ʵ�µĹ�����ϵ!!!";
					GpcLogTrainMessage(0, 0, "%s", str);
					if(m_pParentWnd->MessageBox(str,"��ȷ�ϱ���",MB_YESNO)==IDYES)
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȷ���޸ĳ���");
					}
					else
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȡ���޸ĳ���");
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
				}

				BYTE train_type,run_type,prio;
				if(GetTGBasicParam()->GetTrainPrio(train.GetTrainID(), &train_type, &run_type, &prio))
				{
					if(!m_bEnableFreeTrainOper)
					{
						// ����·���г��޸ĳ���Ϊ�����г�
						if(!GetTGFunctionConfig()->IsFreeAddTrain(train_type)
							&& GetTGFunctionConfig()->IsFreeAddTrain(prev->GetTrainType()))
						{
							if(oper_flag.nFlag == CMMIOperFlag::HAT_TRAINID)
								m_pParentWnd->MessageBox("�����޸��г���ñ����!");
							else
								m_pParentWnd->MessageBox("�����޸��г�����!");
							prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
							break;
						}
					}

					if(train_type!=prev->GetTrainType())
					{
						CString strNew, strOld;
						const char* p=GetTGBasicParam()->m_cTrainTypeInfo.GetTrainTypeName(train_type);
						if(p)
							strNew = p;
						p = GetTGBasicParam()->m_cTrainTypeInfo.GetTrainTypeName(prev->GetTrainType());
						if(p)
							strOld = p;
						CString msg;
						if(oper_flag.nFlag == CMMIOperFlag::CHANGE_TRAINID)
						{
							msg.Format("�³��� %s Ϊ %s\nԭ���� %s Ϊ %s \n�޸�ǰ�����Ͳ�ͬ, �Ƿ���ԭ�г�����?", 
								oper_flag.m_trainId, strNew, prev->GetTrainID(), strOld);
						}
						else
						{
							msg.Format("��ñ���� %s Ϊ %s\nԭ���� %s Ϊ %s \n�޸�ǰ�����Ͳ�ͬ, �Ƿ���ԭ�г�����?", 
								oper_flag.m_trainId, strNew, prev->GetTrainID(), strOld);
						}
						if(m_pParentWnd->MessageBox(msg,"��ȷ�ϲ���",MB_YESNO)==IDYES)
						{
							GpcLogTrainMessage(0, 0, "%s ����Աѡ��:����ԭ�г�����.", msg);
						}
						else
						{
							train.GetTrainInfo()->m_data.type=train_type;
							GpcLogTrainMessage(0, 0, "%s ����Աѡ��:������ԭ�г�����,�޸�Ϊ������", msg);
						}
					}
				}

				// �¼�����18: �����޸ĳ���&��ñ
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					if (oper_flag.nFlag == CMMIOperFlag::HAT_TRAINID)
					{
						data.nEventID =  EVENT_GPC_HAT_TRAIN;
						data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);
					}
					else if (oper_flag.nFlag == CMMIOperFlag::CHANGE_TRAINID)
						data.nEventID = EVENT_GPC_CHANG_TRAINID;
					
					data.SetKey(KeyOldTrainNo, prev->GetTrainID());
					data.SetKey(KeyTrainIndex, prev->GetTrainIndex());
					data.SetKey(KeyTrainNo, oper_flag.m_trainId);

					SendBMsg(&data, eventEntity.nEntityId);
				}
				train.SetShiftTrainIndex(0);
				train.SetFromShift(FALSE);
				RemoveTrainRelations(train, pData);
			}
			else if(oper_flag.nFlag == CMMIOperFlag::ZHEFAN_TRAINID)
			{
				if(!m_bEnableFreeTrainOper)
				{
					// �����޸��հ�ƻ�����
					if(prev->GetShiftTrainIndex()!=NULL_TRAININDEX)
					{
						m_pParentWnd->MessageBox("�����޸�, �հ�ƻ����۷�����!");
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
				}

				if(IsTrainHasRelation(train.GetTrainIndex(), train.GetPrevTrainIndex(), pData))
				{
					CString str="�޸��۷�����ϵͳ������г��Ĺ�����ϵ,��ȷʵҪ�޸��۷�������?\nȷ���޸��۷�����,���������ʵ�µĹ�����ϵ!!!";
					GpcLogTrainMessage(0, 0, "%s", str);
					if(m_pParentWnd->MessageBox(str,"��ȷ�ϱ���",MB_YESNO)==IDYES)
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȷ���޸��۷�����");
					}
					else
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȡ���޸��۷�����");
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
				}

				BYTE train_type,run_type,prio;
				if(GetTGBasicParam()->GetTrainPrio(oper_flag.m_trainId, &train_type, &run_type, &prio))
				{
					if(!m_bEnableFreeTrainOper)
					{
						// ����·���г��޸ĳ���Ϊ�����г�
						if(!GetTGFunctionConfig()->IsFreeAddTrain(train_type)
							&& GetTGFunctionConfig()->IsFreeAddTrain(prev->GetTrainType()))
						{
							m_pParentWnd->MessageBox("�����޸�, �г��۷�����!");
							prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
							break;
						}
					}

					if(train_type!=prev->GetTrainType())
					{
						CString strNew, strOld;
						const char* p=GetTGBasicParam()->m_cTrainTypeInfo.GetTrainTypeName(train_type);
						if(p)
							strNew = p;
						p = GetTGBasicParam()->m_cTrainTypeInfo.GetTrainTypeName(prev->GetTrainType());
						if(p)
							strOld = p;

						CString msg;
						msg.Format("�۷����� %s Ϊ %s \nԭ���� %s Ϊ %s \n�޸�ǰ�����Ͳ�ͬ,�Ƿ���ԭ�г�����?", 
							oper_flag.m_trainId, strNew, prev->GetTrainID(), strOld);
						
						if(m_pParentWnd->MessageBox(msg,"��ȷ�ϲ���",MB_YESNO)==IDYES)
						{
							GpcLogTrainMessage(0, 0, "%s ����Աѡ��:����ԭ�г�����.", msg);
						}
						else
						{
							train.GetTrainInfo()->m_data.type=train_type;
							GpcLogTrainMessage(0, 0, "%s ����Աѡ��:������ԭ�г�����,�޸�Ϊ������", msg);
						}
					}
				}
				// �¼�����18: �����޸��۷�
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_ZHEFAN_TRAIN;
					data.SetKey(KeyOldTrainNo, prev->GetTrainID());
					data.SetKey(KeyTrainIndex, prev->GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);
					data.SetKey(KeyTrainNo, oper_flag.m_trainId);
					
					SendBMsg(&data, eventEntity.nEntityId);
				}

				train.SetShiftTrainIndex(0);
				train.SetFromShift(FALSE);
				RemoveTrainRelations(train, pData);
			}

			if((oper_flag.nFlag&3) == CMMIOperFlag::STAY_TO_PASS || (oper_flag.nFlag&3) == CMMIOperFlag::PASS_TO_STAY)
			{
				if(GetTGFunctionConfig()->IsNotThisSectionStation(m_schd_info.m_nSectionID,  oper_flag.m_nStation))
				{
					CString text;
					text.Format("��ע��, %s �����ڱ�̨��Ͻ,�����޸ĵ�����ʽ", gpcdef.GetStationName(oper_flag.m_nStation));
					m_pParentWnd->MessageBox(text);
					break;
				}

				// �¼�����18: �����޸�ͨ���䵽��& ������ͨ��
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;

					data.SetKey(KeyTrainNo,oper_flag.m_trainId);
					data.SetKey(KeyTrainIndex, prev->GetTrainIndex());
					data.SetKey(KeyStationId, oper_flag.m_nStation);
					data.SetKey(KeyStationName, gpcdef.GetStationName(oper_flag.m_nStation));
					if((oper_flag.nFlag&3) == CMMIOperFlag::STAY_TO_PASS)
						data.nEventID =  EVENT_GPC_STAY_TO_PASS;
					else if ((oper_flag.nFlag&3) == CMMIOperFlag::PASS_TO_STAY)
						data.nEventID =  EVENT_GPC_PASS_TO_STAY;

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}		
			if(oper_flag.nFlag == CMMIOperFlag::TRAIN_ATTRIB_CHANGED)
			{
			 	if (train.GetSpaceRank()  != prev->GetSpaceRank())
			 	{
			 		// �¼�����18: �����޸��г���������(����)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
			 			MsgEventData data;
			 			data.nSrcEntityID = GetLocalEntityID();
			 			data.tmEventTime = time(NULL);
			 			data.nEventRank = NORMAL_EVENT;
			 			data.nEventID =  EVENT_GPC_TRAIN_ATTR_SPACE;
			 			data.SetKey(KeyTrainNo, train.GetTrainID());
			 			data.SetKey(KeyTrainIndex, train.GetTrainIndex());
			 			data.SetKey(KeyRankType, 1);
			 			data.SetKey(KeyTrainSpaceInfo, train.GetTrainInfo()->m_data.space_rank);

			 			SendBMsg(&data, eventEntity.nEntityId);
					}
			 	}
			 	if (train.GetTrainInfo()->m_data.length_rank != prev->GetTrainInfo()->m_data.length_rank)
			 	{
			 		// �¼�����18: �����޸��г�����(����)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
			 			MsgEventData data;
			 			data.nSrcEntityID = GetLocalEntityID();
			 			data.tmEventTime = time(NULL);
			 			data.nEventRank = NORMAL_EVENT;
			 			data.nEventID =  EVENT_GPC_TRAIN_ATTR_SPACE;
			 			data.SetKey(KeyTrainNo, train.GetTrainID());
			 			data.SetKey(KeyTrainIndex, train.GetTrainIndex());
			 			data.SetKey(KeyRankType, 2);
			 			data.SetKey(KeyTrainSpaceInfo, train.GetTrainInfo()->m_data.length_rank);
				
			 			SendBMsg(&data, eventEntity.nEntityId);
					}
			 	}
			
			 	if (train.GetTrainInfo()->m_data.super_rank != prev->GetTrainInfo()->m_data.super_rank)
			 	{
			 		// �¼�����18: �����޸��г�����(�ص�)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
			 			MsgEventData data;
			 			data.nSrcEntityID = GetLocalEntityID();
			 			data.tmEventTime = time(NULL);
			 			data.nEventRank = NORMAL_EVENT;
			 			data.nEventID =  EVENT_GPC_TRAIN_ATTR_SPACE;
			 			data.SetKey(KeyTrainNo, train.GetTrainID());
			 			data.SetKey(KeyTrainIndex, train.GetTrainIndex());
			 			data.SetKey(KeyRankType, 3);
			 			data.SetKey(KeyTrainSpaceInfo, train.GetTrainInfo()->m_data.super_rank);
				
			 			SendBMsg(&data, eventEntity.nEntityId);
					}
			 	}
			
			 	if (train.GetTrainInfo()->m_data.military_rank != prev->GetTrainInfo()->m_data.military_rank)
			 	{
			 		// �¼�����18: �����޸��г�����(����)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
			 			MsgEventData data;
			 			data.nSrcEntityID = GetLocalEntityID();
			 			data.tmEventTime = time(NULL);
			 			data.nEventRank = NORMAL_EVENT;
			 			data.nEventID =  EVENT_GPC_TRAIN_ATTR_SPACE;
			 			data.SetKey(KeyTrainNo, train.GetTrainID());
			 			data.SetKey(KeyTrainIndex, train.GetTrainIndex());
			 			data.SetKey(KeyRankType, 4);
			 			data.SetKey(KeyTrainSpaceInfo, train.GetTrainInfo()->m_data.military_rank);

			 			SendBMsg(&data, eventEntity.nEntityId);
					}
			 	}
			}
			
			if(oper_flag.nFlag == CMMIOperFlag::SET_ABS_TOP)
			{
				// �¼�����18: ��������ͣ��
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_STOP_ABS;
					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyStationId, oper_flag.m_nStation);
					data.SetKey(KeyStationName, gpcdef.GetStationName(oper_flag.m_nStation));
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}

			if (oper_flag.nFlag == CMMIOperFlag::MODIFY_SIDE)
			{
				// �¼�����18: �޸Ĺɵ�
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_MODIFY_SIDE;
					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);
				
					CTG_TrainRecord *newrec = train.GetTrainRecord(oper_flag.m_nRecIndex);
					CTG_TrainRecord *oldrec = prev->GetTrainRecord(oper_flag.m_nRecIndex);

					data.SetKey(KeyStationId, newrec->m_data.station);
					data.SetKey(KeyStationName, gpcdef.GetStationName(newrec->m_data.station));
					
					data.SetKey(KeyArrOrlSideId, newrec->m_data.arrive_side);
					data.SetKey(KeyDptOrlSideId, newrec->m_data.depart_side);
					data.SetKey(KeyArrSideId, oldrec->m_data.arrive_side);
					data.SetKey(KeyDptSideId, oldrec->m_data.depart_side);

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}

			if (oper_flag.nFlag == CMMIOperFlag::DEPART_BY_PLAN || oper_flag.nFlag == CMMIOperFlag::DEPART_BY_MEET)
			{
				// �¼�����18: �޸ķ���ʱ��(���ƻ�)
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_MODIFY_DEPART;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);

					CTG_TrainRecord *newrec = train.GetTrainRecord(oper_flag.m_nRecIndex);
					CTG_TrainRecord *oldrec = prev->GetTrainRecord(oper_flag.m_nRecIndex);

					data.SetKey(KeyStationId, newrec->m_data.station);
					data.SetKey(KeyStationName, gpcdef.GetStationName(newrec->m_data.station));

					//1 ���� 2 �ƻ�
					data.SetKey(KeyDepartMoment, TG_IsMakeDepartRouteByMeet(newrec->m_data));
					data.SetKey(KeyOrlDepartMoment, TG_IsMakeDepartRouteByMeet(oldrec->m_data));

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}
			if (oper_flag.nFlag == CMMIOperFlag::TRAIN_PLAN_PARA)
			{
				// �¼�����18: �޸ļƻ��߲���
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_TRAIN_PLAN_PARA;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);

					CTG_TrainRecord *newrec = train.GetTrainRecord(oper_flag.m_nRecIndex);
					data.SetKey(KeyStationId, newrec->m_data.station);
					data.SetKey(KeyStationName, gpcdef.GetStationName(newrec->m_data.station));
					data.SetKey(KeyEntryId, newrec->m_data.entry);
					data.SetKey(KeyExitId, newrec->m_data.exit);
					data.SetKey(KeyBusinessTime, newrec->m_data.min_stop_time);
					data.SetKey(KeySideId, newrec->m_data.arrive_side);

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}
			if (oper_flag.nFlag == CMMIOperFlag::HELP_TRAIN)
			{
				// �¼�����18: �޸ļƻ��߲���
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_HELP_TRAIN;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyHelpTrain, oper_flag.m_trainId);

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}

			if (oper_flag.nFlag == CMMIOperFlag::TURN_REALITY)
			{
				// �¼�����18: �˹�תʵ��
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_CHANGE_TRAIN_REAL;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);
					data.SetKey(KeyStationId, oper_flag.m_nStation);
					data.SetKey(KeyStationName, gpcdef.GetStationName(oper_flag.m_nStation));

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}
			if (oper_flag.nFlag == CMMIOperFlag::MOUSE_TURN_REALITY)
			{
				// �¼�����18: ����յ�
				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_MOUSE_REAL;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);
					data.SetKey(KeyStationId, oper_flag.m_nStation);
					data.SetKey(KeyStationName, gpcdef.GetStationName(oper_flag.m_nStation));

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}

			if (oper_flag.nFlag == CMMIOperFlag::MODIFY_TRAIN_TIME)
			{
				// �¼�����18: ����޸ĵ���ʱ��

				GPCENTITY eventEntity;
				if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
				{
					MsgEventData data;
					data.nSrcEntityID = GetLocalEntityID();
					data.tmEventTime = time(NULL);
					data.nEventRank = NORMAL_EVENT;
					data.nEventID =  EVENT_GPC_MODIFY_TRAIN_TIME;

					data.SetKey(KeyTrainNo, train.GetTrainID());
					data.SetKey(KeyTrainIndex, train.GetTrainIndex());
					data.SetKey(KeyRecindex, oper_flag.m_nRecIndex);

					CTG_TrainRecord *rec = train.GetTrainRecord(oper_flag.m_nRecIndex);
					data.SetKey(KeyStationId, rec->GetStationNo());
					data.SetKey(KeyStationName, gpcdef.GetStationName(rec->GetStationNo()));
					data.SetKey(KeyArriveTime, rec->m_data.arrive);
					data.SetKey(KeyDepartTime, rec->m_data.depart);

					SendBMsg(&data, eventEntity.nEntityId);
				}
			}
			
			// �����·��ϵ
			if (oper_flag.nFlag == CMMIOperFlag::JIE_JIAOLUT_RELATION)
			{
				train.SetPrevShiftTrainIndex(0);
				train.SetPrevBasicTrainIndex(0);
				GpcLogTrainMessage(0, 0, "����Ա�����·:����PrevShiftTrainIndex=%u PrevBasicTrainIndex=%u.", 
					train.GetPrevTrainShiftTrainIndex(), train.GetPrevTrainBasicTrainIndex());
			}

			// 2018-08-28���ڳ��鹴����ϵ,���ɵ��Ƿ���۷��ɵ���ͻ
			if(GetTGFunctionConfig()->IsEnableNewCzh())
			{
				CStringArray ary;
				vector<int> list_station, list_rec_index;
				CString strXml=m_pTGCtrl->SendTrainMsgToOCX(train, CTgX::SEND_FLAG::Check_Relation);
				AlarmfromXml(strXml, ary, list_station, list_rec_index);
				
				CString str;
				for(int n=0; n<ary.GetCount(); n++)
				{
					str += ary.GetAt(n);
					str += "\n";
				}
				if(str=="")
				{
					for(int rec_index=0; rec_index<train.GetTrainRecordCount(); rec_index++)
					{
						CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
						if(prec==NULL)
							continue;
						
						TG_ClearFlagDispalySide(prec->m_data);
					}
				}
				else
				{
					CString strErr;
					strErr.Format("%s�α�������:\n%s", train.GetTrainID(), str);
					GpcLogTrainMessage(0, 0, "%s", strErr);
					if(m_pParentWnd->MessageBox(strErr,"��ȷ�ϲ���",MB_YESNO)!=IDYES)
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȡ������");
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
					else
					{
						GpcLogTrainMessage(0, 0, "����Աѡ��ȷ��");
						for(int rec_index=0; rec_index<train.GetTrainRecordCount(); rec_index++)
						{
							CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
							if(prec==NULL)
								continue;
							
							bool bErr=false;
							for(int n=0; n<list_station.size()&&n<list_rec_index.size(); n++)
							{
								if(prec->m_data.station==list_station[n] && rec_index==list_rec_index[n])
								{
									bErr=true;
									break;
								}
							}
							if(bErr)
							{
								TG_SetFlagDispalySide(prec->m_data);
							}
							else
							{
								TG_ClearFlagDispalySide(prec->m_data);
							}
						}
					}
				}
			}

			DWORD errcode=0;
			const char *errinfo=m_pSchdHolder->CheckTrainManualUpdateOper(&train,m_schd_info,oper_flag,errcode);
			if(errinfo && errcode!=0)
			{
				GpcLogTrainMessage(0, 0, "�޸ĳ���%s(%u)����:%s", train.GetTrainID(), train.GetTrainIndex(), errinfo);
				if(m_pParentWnd->MessageBox(errinfo,"��ȷ�ϲ���",MB_YESNO)!=IDYES)
				{
					GpcLogTrainMessage(0, 0, "����Աѡ��ȡ������");
					prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
					break;
				}
				else
				{
					GpcLogTrainMessage(0, 0, "����Աѡ����");
				}
			}

			if(GetTGFunctionConfig()->IsEnableCheckPlatform())
			{
				CString strErr;
				int nCodeErr = m_pSchdHolder->CheckSidePlatform(train, strErr);
				if(nCodeErr>0)
				{
					if(m_pParentWnd->MessageBox(strErr,"��ȷ�ϲ���",MB_YESNO)!=IDYES)
					{
						prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
						break;
					}
				}
			}

			m_pSchdHolder->GenTrain_AutoAdjustManualUpdate(train,m_schd_info,oper_flag,m_pParentWnd); // 20090617 ��ȫ����
			
			if(oper_flag.nFlag & CMMIOperFlag::CONTINUE_DRAW_TRAIN)  // ��������
			{
				if(oper_flag.m_nRecIndex>0)
				{
					SetFreedrawTrainPlanTime(train, oper_flag.m_nRecIndex);
				} 
			}

			if(prev)
			{
				PushOper(prev,op_type);
				// ÿ���˲���֮���жϾ�·�Ƿ���ͬ
				if(GetTGFunctionConfig()->IsZeroGIDPasswayChange() && (train.GetShiftTrainIndex()!=0))
				{
					if(!prev->IsSamePathway(train))
					{
						GpcLogTrainMessage(0, 0, "�г�%s(%u)�ľ�·��ԭ����ͬ���ȫ��ID.", train.GetTrainID(), train.GetTrainIndex());
						train.SetShiftTrainIndex(0);
						train.SetFromShift(FALSE);
					}
				}
			}

			train.SetUpdateTime(cur_tm.GetTime());
			BOOL bUpdateSuccess=pData->UpdateTrain(m_schd_info,train,SYCFLAG_NULL);

			// ��ʱ�����ϲ�,��ʱ���߲���ˢ�µ�����
			if(m_pTGCtrl && bUpdateSuccess)
			{
				GpcLogTrainMessage(0, 0, "��TGOCX���������߸��� trainID %s index %u", train.GetTrainID(), train.GetTrainIndex());
				m_pTGCtrl->UpdateTrainOCX(train);
			}
			/////////  ǿ�Ƹ���, ������OCX��ˢ��������
			//prev=pData->GetSingleTrain(train.GetTrainIndex());
			//if(prev)
			//{
			//	prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
			//}
		}
		break;
	case TG_OPER_DELETE://// 521�ƻ��Ż��޸�
		{
			if(train_index==NULL_TRAININDEX)
				train_index=train.GetTrainIndex();
			CTG_SingleTrain *prev=pData->GetSingleTrain(train_index);
			if(!prev)
				break;
			if(prev->GetTrainRecordCount()==0)  // �Ѿ���ɾ��,һ�㲻�ᷢ��������,����ֹ�û�����ɾ��,�������������� 2010.6.22
			{
                GpcLogTrainMessage(0, 0, "�ظ�ɾ���г��� %s, train_index %u shift_index %u", prev->GetTrainID(),
					prev->GetTrainIndex(), prev->GetShiftTrainIndex());
				m_pParentWnd->MessageBox("���г��Ѿ�ɾ��,��Ⱥ�ˢ��");
				break;
			}
			PushOper(prev,op_type);
			if(GetTGFunctionConfig()->IsEnableNewCzh())
			{
				// ���ɾ���г��ͺ����г���·��ϵ
				CTG_SingleTrain *prev_train=pData->GetNextTrainByPrevTrainIndex(train_index);
				if(prev_train)
				{
					prev_train->SetPrevTrainIndex(0);
					GpcLogTrainMessage(0, 0, "�г� %s(%u)��ǰ���г�Ϊ��Ҫɾ�����г� %s(%u), ���� %s ǰ���г�prev_train_index %u", 
						prev_train->GetTrainID(), prev_train->GetTrainIndex(), prev->GetTrainID(),
						prev->GetTrainIndex(), prev_train->GetTrainID(),prev_train->GetPrevTrainIndex());

					CTG_SingleTrain newtrain=*prev_train;
					m_pTGCtrl->UpdateTrainOCX(newtrain);

					prev_train->SetUpdateTime(cur_tm.GetTime());
					prev_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
				}
			}

			if(prev->HasSentPlan() || pData->IsTrainInSendPlanList(prev)) // �����¹��ƻ�
			{
				prev->SetTrainRecordCount(0);
				prev->SetUpdateTime(cur_tm.GetTime());
				prev->SetSyncStatusX(SYCFLAG_NULL,TRUE);
				GpcLogTrainMessage(0, 0, "ɾ�������¹��ƻ����г� %s, train_index %u shift_index %u", prev->GetTrainID(),
					prev->GetTrainIndex(), prev->GetShiftTrainIndex());
			}
			else
			{
				GpcLogTrainMessage(0, 0, "ɾ��δ�¹��ƻ����г� %s, train_index %u shift_index %u", prev->GetTrainID(),
					prev->GetTrainIndex(), prev->GetShiftTrainIndex());
				pData->DeleteTrain(m_schd_info,train_index,SYCFLAG_NULL); //δ�¹��ƻ�,ֱ��ɾ��
			}
		}
		break;
	}
	return TRUE;
}

BOOL   CWintgViewMMI::ProcessMMIOperation(TG_OPER_TYPE op_type,CTG_SingleMark &mark,TRAIN_INDEX mark_index)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	///////// ���ڻ���ͼ�޸ĵ����⴦��ȥ��OCX�е����� 20090827 //////////////
	if(IS_BASIC_SCHD(m_schd_info.m_nType))
	{
		long shift_date=CTG_ScheduleInfo::Get_ShiftDate(m_schd_info.m_nShiftID);

		long curr_date=0;
		long curr_time=minute_time(mark.m_data.start_time,&curr_date);
		curr_date-=shift_date;
		mark.m_data.start_time=long_time(curr_date,curr_time);

		curr_time=minute_time(mark.m_data.end_time,&curr_date);
		curr_date-=shift_date;
		mark.m_data.end_time=long_time(curr_date,curr_time);
	}
	///////////////////////////////////////////////////////////////////////////
	switch(op_type)
	{
	case TG_OPER_ADD:
		mark.SetBlockIndex(mark_index); //��������,Ĭ��INDEX
		mark.SetGPCIndex(pData->GetAvaGPCIndex());
		{
			TRAIN_INDEX  newtrainindex=pData->AppendMark(m_schd_info,mark,SYCFLAG_NULL);
			if(newtrainindex!=NULL_TRAININDEX)
			{
				CTG_SingleMark *prev=pData->GetSingleMark(newtrainindex);
				if(prev)
				{
					if(m_schd_info.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC
						|| m_schd_info.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
						sprintf(prev->m_data.shift_string,"B_%d",newtrainindex); // Ĭ��PLANID
					else
						if(m_schd_info.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
							sprintf(prev->m_data.shift_string,"S_%d",newtrainindex); // Ĭ��PLANID
						else
							sprintf(prev->m_data.shift_string,"W_%d",newtrainindex); // Ĭ��PLANID

					PushOper(prev,op_type);
				}
			}
		}
		break;
	case TG_OPER_UPDATE:
		{
			CTG_SingleMark *prev=pData->GetSingleMark(mark.GetBlockIndex());
			if(prev)
			{
				PushOper(prev,op_type);
			}
			pData->UpdateMark(m_schd_info,mark,SYCFLAG_NULL);
		}
		break;
	case TG_OPER_DELETE:
		{
			CTG_SingleMark *prev=pData->GetSingleMark(mark_index);
			if(prev)
			{
				if(!GetTGFunctionConfig()->IsEnableDeleteFromTmisBlock())
				{
					if(prev->GetShiftIndexString()
						&& strlen(prev->GetShiftIndexString())>0 
						&& !strstr(prev->GetShiftIndexString(),"W_"))  // 20101015 ��ʱ����W_��ͷ����������ɵ�
					{
						m_pParentWnd->MessageBox("��ѡ���Ҽ��˵�'ȡ��ʩ��'!");					
						break;
					}
				}

				PushOper(prev,op_type);
				GpcLogTrainMessage(0, 0, "ɾ��ʩ������, mark_index %d shift_index %d", 
					mark_index, prev->GetShiftIndex());
			}
			if(mark_index!=NULL_TRAININDEX)
				pData->DeleteMark(m_schd_info,mark_index,SYCFLAG_NULL);
			else
				pData->DeleteMark(m_schd_info,mark_index,SYCFLAG_NULL);
		}
		break;
	}
	return TRUE;
}

BOOL   CWintgViewMMI::ProcessMMIOperation(TG_OBJECT_TYPE ob_type,TG_OPER_TYPE op_type,TRAIN_INDEX index,const char *xml)
{

	BOOL qualify_runtime=TRUE; // �Զ�У������ʱ��
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	if(ob_type==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain train;
		CTG_SingleTrain *pold=NULL;
		if(op_type==TG_OPER_ADD)
			index=NULL_TRAININDEX;
		else if(op_type==TG_OPER_UPDATE)
		{
			pold=pData->GetSingleTrain(index);
			if(!pold)
				return FALSE;
			train=*pold;
		}
		CMMIOperFlag  oper_flag;
		if(!fromXML(train,oper_flag,xml,false))
			return FALSE;
		else
		{
			train.SetSelfUpdate(TRUE); // // ���������޸ģ�������º����ѡ������ 20090921 XIAN
			return ProcessMMIOperation(op_type,train,oper_flag,index);
		}
	}
	else if(ob_type==TG_OBJ_MARK)
	{
		CTG_SingleMark mark;
		CTG_SingleMark *pold=NULL;
		if(op_type==TG_OPER_ADD)
			index=NULL_TRAININDEX;
		else
			if(op_type==TG_OPER_UPDATE)
			{
				pold=pData->GetSingleMark(index);
				if(!pold)
					return FALSE;
				mark=*pold;
			}
			if(!fromXML(mark,xml))
				return FALSE;
			else
			{
				mark.SetSelfUpdate(TRUE); // ���������޸ģ�������º����ѡ������ 20090921 XIAN
				return ProcessMMIOperation(op_type,mark,index);
			}

	}
	return TRUE;
}

BOOL  CWintgViewMMI::AddTrain_Passway(bool secret,const TRAIN_NO train_no,const TRAIN_NO chg_train,int  pathway, BYTE start_flag, BYTE end_flag ,TIME ltime,BYTE run_type)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	int station=NULL_STATION;
	CTG_SingleTrain srctrain;
	if(m_pSchdHolder->GenTrain_Passway(srctrain,m_schd_info,train_no,pathway, start_flag, end_flag ,station,ltime,run_type))
	{
		// ����passway���ô����α仯--Ŀǰ����̨������ʹ��
		CString trainid = chg_train;
		if(!trainid.IsEmpty())
		{
			CSingle_Passway passway_info;
			if(GetTGBasicParam()->m_cPasswayInfo.GetSinglePassway(m_schd_info.m_nSectionID, pathway, passway_info))
			{
				int rec_count=passway_info.m_vector_Passway_Sta_info.size();

				CTG_TrainRecordList* rec_list=srctrain.GetRecordList();
				int real_cpunt = rec_list->GetRecordCount();

				int num = (real_cpunt<=rec_count) ? real_cpunt : rec_count;

				for(int i=0;i<num;i++)
				{
					CTG_TrainRecord& rec = rec_list->m_listRecords[i];
					const Passway_Sta &p_sta= passway_info.m_vector_Passway_Sta_info[i];
					
					if(p_sta.change_trainid == 1) // �仯���복��
					{
						TG_CopyTrainId(rec.m_data.arrive_train_id, chg_train);
					}
					else if(p_sta.change_trainid == 2) // �仯��������
					{
						TG_CopyTrainId(rec.m_data.depart_train_id, chg_train);
					}
					else if(p_sta.change_trainid == 3) // �仯���뽻������
					{
						TG_CopyTrainId(rec.m_data.arrive_train_id, chg_train);
						TG_CopyTrainId(rec.m_data.depart_train_id, chg_train);
					}
				}
			}
		}
		CTGAddTrainLogic::AutoChangeTrainID(&srctrain, m_schd_info.m_nSectionID, 1);
		CTGAddTrainLogic::ChangeTrainElec(&srctrain, m_schd_info.m_nSectionID);
		SetFreedrawTrainPlanTime(srctrain);

		if(secret)
		{
			SetConsoleTrainSecret(srctrain.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "���վ�·�� %d �����г� %s Ϊ�����г�", pathway,srctrain.GetTrainID());
		}

		return AddTrain_Directly(srctrain);
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_BasicSchedule(TRAIN_INDEX basic_index,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;

	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_BasicSchedule(train,m_schd_info,basic_index,station,bm,em))
	{
		GpcLogTrainMessage(0, 0, "�������� basic_index=%d, ���ݻ���ͼ�г��� train=%s, ���복��",basic_index,train.GetTrainID());
		BOOL ret = AddTrain_Directly(train);
		if(!ret)
		{
			GpcLogTrainMessage(0, 0, "�������� basic_index=%d, ���ݻ���ͼ�г��ε��복��, ʧ��",basic_index);
		}

		return ret;
	}
	else
	{
		GpcLogTrainMessage(0, 0, "�������� basic_index=%d, ���ݻ���ͼ�г��ε��복��, ʧ��",basic_index);
	}
	return FALSE;
}

// ���ɻ��߼ӻ�����
void  CWintgViewMMI::SetEngineIds(CTG_SingleTrain &srctrain, const CDWordArray& aryStatus, const CStringArray& engineId, int conid)
{
	CTG_TrainEngineList* engineinfo=srctrain.GetEngineList();
	engineinfo->PurgeRecords();

	CTG_TrainEngineInfo rec;
	for(int i=0; i<engineId.GetCount(); i++)
	{
		CString engineNo = engineId.GetAt(i);
		memset(&rec.m_data, 0, sizeof(rec.m_data));

		CString locoType;
		CString locoNo;
		if(engineNo.GetLength()<=3)
		{
			locoType = "";
			locoNo = engineNo;
		}
		else
		{
			locoType = engineNo.Left(3);
			CString name = GetTGBasicParam()->GetLocoPreName(conid, locoType);
			strncpy(rec.m_data.engine_type, name, sizeof(rec.m_data.engine_type));

			int cascoType = GetTGBasicParam()->GetCascoIdByLocoId(conid, atoi(locoType));
			if(cascoType >= 128)
				SetElecEngineExt(rec.m_data.engine_status);
			else
				SetDieselEngineExt(rec.m_data.engine_status);
		}
		strncpy(rec.m_data.engine_id,  engineNo, sizeof(rec.m_data.engine_id)-1);
		rec.m_data.engine_id[sizeof(rec.m_data.engine_id)-1]=0;

		if(aryStatus.GetCount()>0)
		{
			DWORD nItemData = aryStatus.GetAt(i);
			rec.m_data.attach_sta = nItemData&0xFF;
			rec.m_data.unattach_sta = (nItemData&0xFF00)>>8;
			
			int nSel = (nItemData&0xFF0000)>>16;
			if(nSel==0)
				SetTractionEngineExt(rec.m_data.engine_status);
			else
				SetAttachEngineExt(rec.m_data.engine_status);
		}
		else
		{
			rec.m_data.attach_sta=0;
			rec.m_data.unattach_sta=srctrain.GetTrainRecordCount()-1;
		}
		engineinfo->AddRecord(rec);
	}
}

BOOL  CWintgViewMMI::AddTrainTmp_Passway(bool secret, CStringArray& aryEngineId, CDWordArray& status, CString trainId, int  pathway, 
										 BYTE start_flag, BYTE end_flag, BYTE start_index, BYTE end_index, TIME ltime,BYTE run_type)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain srctrain;
	if(m_pSchdHolder->GenTrainTmp_Passway(srctrain,m_schd_info,trainId,pathway, start_flag, end_flag, start_index, end_index,ltime,run_type))
	{
		SetEngineIds(srctrain, status, aryEngineId, m_schd_info.m_nSectionID);
		SetFreedrawTrainPlanTime(srctrain);

		if(secret)
		{
			SetConsoleTrainSecret(srctrain.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "���ճ��κ� %s �����г� %s Ϊ�����г�", trainId,srctrain.GetTrainID());
		}

		return AddTrain_Directly(srctrain);
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_Passway(bool secret, const TRAIN_NO train_no, const CStringArray& engineId, const CDWordArray& status,int  pathway, BYTE start_flag, BYTE end_flag ,TIME ltime,BYTE run_type)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	int station=NULL_STATION;
	CTG_SingleTrain srctrain;
	if(m_pSchdHolder->GenTrain_Passway(srctrain,m_schd_info,train_no,pathway, start_flag, end_flag ,station,ltime,run_type))
	{
		SetEngineIds(srctrain, status, engineId, m_schd_info.m_nSectionID);
		SetFreedrawTrainPlanTime(srctrain);
		if(secret)
		{
			SetConsoleTrainSecret(srctrain.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "���վ�·�� %d �����г� %s Ϊ�����г�", pathway,srctrain.GetTrainID());
		}
		return AddTrain_Directly(srctrain);
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_BasicSchedule(bool secret, const TRAIN_NO orig_trainno, const CStringArray& engineId, const CDWordArray& status)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_BasicSchedule(train, m_schd_info, orig_trainno, NULL_STATION, NULL_TIME, NULL_TIME))
	{
		GpcLogTrainMessage(0, 0, "���ճ��κ� %s ���ݻ���ͼ�г��� %s(%d), ���복��",orig_trainno,train.GetTrainID(),train.GetTrainIndex());
		SetEngineIds(train, status, engineId, m_schd_info.m_nSectionID);
		if(secret)
		{
			SetConsoleTrainSecret(train.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "���ճ��κ� %s �����г� %s Ϊ�����г�", orig_trainno,train.GetTrainID());
		}
		BOOL ret=AddTrain_Directly(train);
		if(!ret)
			GpcLogTrainMessage(0, 0, "%s ���복��ʧ��",orig_trainno);
		return ret;
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_ShiftSchedule(bool secret, TRAIN_INDEX shift_index,const CStringArray& engineId, const CDWordArray& status, STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_ShiftSchedule(train,m_schd_info,shift_index,station,bm,em))
	{
		CTG_SingleTrain *prev;
		shift_index &=0x7FFFFFF;
		CTG_TrainRecord *pfirrec=train.GetTrainRecord(0); 
		int station=NULL_STATION;
		if(pfirrec)
			station=pfirrec->GetStationNo();
		prev=pData->GetSingleTrainByShiftIndex(shift_index,station);  // �����ظ���� 20090702

		SetEngineIds(train, status, engineId, m_schd_info.m_nSectionID);
		if(secret)
		{
			SetConsoleTrainSecret(train.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "�����հ�ƻ��� %d �����г� %s Ϊ�����г�", shift_index,train.GetTrainID());
		}
		if(prev)
		{
			prev->SetShiftTrainIndex(0);
			prev->SetFromShift(FALSE);
			if(GetTGFunctionConfig()->IsEnableNewCzh())
			{
				prev->SetPrevTrainIndex(0);
				prev->SetPrevShiftTrainIndex(0);
				prev->SetPrevBasicTrainIndex(0);
			}
			prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
			BOOL ret = AddTrain_Directly(train);
			GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��ʱ�����ظ�,shift_id=%d train=%s previndex=%d",shift_index,train.GetTrainID(),prev->GetTrainIndex());
			if(!ret)
				GpcLogTrainMessage(0, 0, "���복��,shift_id=%d train=%sʧ��",shift_index,train.GetTrainID());

			return ret;
		}
		else
		{
			GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��,shift_id=%d train=%s",shift_index,train.GetTrainID());
			BOOL ret = AddTrain_Directly(train);
			if(!ret)
				GpcLogTrainMessage(0, 0, "���복��,shift_id=%d train=%sʧ��",shift_index,train.GetTrainID());
			return ret;
		}
	}

	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_BasicSchedule(bool secret, const TRAIN_NO orig_trainno,const TRAIN_NO hat_trainno,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_BasicSchedule(train,m_schd_info,orig_trainno,station,bm,em))
	{
		GpcLogTrainMessage(0, 0, "���ճ��κ� %s ���ݻ���ͼ�г��� %s(%d), ���복��",orig_trainno,train.GetTrainID(),train.GetTrainIndex());
		if(strlen(hat_trainno)>0)
		{
			for(int i=0;i<train.GetTrainRecordCount();i++)
			{
				CTG_TrainRecord *prec=train.GetTrainRecord(i);
				STR_CPY(prec->m_data.original_arrive_train_id,orig_trainno); 
				STR_CPY(prec->m_data.original_depart_train_id,orig_trainno); 
				STR_CPY(prec->m_data.arrive_train_id,hat_trainno); 
				STR_CPY(prec->m_data.depart_train_id,hat_trainno); 
			}
		}

		if(secret)
		{
			SetConsoleTrainSecret(train.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "���ճ��κ� %s �����г� %s Ϊ�����г�", orig_trainno,train.GetTrainID());
		}

		BOOL ret = AddTrain_Directly(train);

		if(!ret)
		{
			GpcLogTrainMessage(0, 0, "���ݳ��κ� %s �ӻ���ͼ�е��복��ʧ��",orig_trainno);
		}

		return ret;
	}
	else
	{
		CString text;
		text.Format("����ͼ��û�г��� %s, ����ͼ���ӳ�ʧ��", orig_trainno);
		AfxMessageBox(text);

		GpcLogTrainMessage(0, 0, "���ճ��κ� %s �ӻ���ͼ�е��복��ʧ��: %s",orig_trainno, text);

	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_ShiftSchedule(const char *train_no,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_ShiftSchedule(train,m_schd_info,train_no,station,bm,em))
	{
		BOOL ret=AddTrain_Directly(train);
		if(!ret)
			GpcLogTrainMessage(0, 0, "���ճ��κ� %s ���հ�ƻ��е��복��ʧ��",train_no);
		return ret;
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddTrain_ShiftSchedule(bool secret,TRAIN_INDEX shift_index,STATION_NO station,TIME bm,TIME em)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��,shift_id=%d station_no=%d",shift_index,station);
	CTG_SingleTrain train;
	if(m_pSchdHolder->GenTrain_ShiftSchedule(train,m_schd_info,shift_index,station,bm,em))
	{
		if(secret)
		{
			SetConsoleTrainSecret(train.GetTrainInfo()->m_data);
			GpcLogTrainMessage(0, 0, "�����հ�ƻ��� %d �����г� %s Ϊ�����г�", shift_index,train.GetTrainID());
		}

		CTG_SingleTrain *prev;
		shift_index &=0x7FFFFFF;
		CTG_TrainRecord *pfirrec=train.GetTrainRecord(0); 
		int station=NULL_STATION;
		if(pfirrec)
			station=pfirrec->GetStationNo();
		prev=pData->GetSingleTrainByShiftIndex(shift_index,station);  // �����ظ���� 20090702
		if(prev)
		{
			GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��ʱ�����ظ�,shift_id=%d train=%s previndex=%d",shift_index,train.GetTrainID(),prev->GetTrainIndex());
			
			prev->SetShiftTrainIndex(0);
			prev->SetFromShift(FALSE);
			if(GetTGFunctionConfig()->IsEnableNewCzh())
			{
				prev->SetPrevTrainIndex(0);
				prev->SetPrevShiftTrainIndex(0);
				prev->SetPrevBasicTrainIndex(0);
			}
			prev->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
			BOOL ret = AddTrain_Directly(train);
			if(!ret)
				GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��ʧ��, shift_id=%d train=%s previndex=%d",shift_index,train.GetTrainID(),prev->GetTrainIndex());

			return ret;
		}
		else
		{
			GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��,shift_id=%d train=%s",shift_index,train.GetTrainID());
			BOOL ret = AddTrain_Directly(train);
			if(!ret)
				GpcLogTrainMessage(0, 0, "���հ�ƻ��е��복��ʧ��, shift_id=%d train=%s",shift_index,train.GetTrainID());
			return ret;
		}
	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddMark_ShiftSchedule(TRAIN_INDEX shift_index)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleMark mark;
	if(m_pSchdHolder->GenMark_ShiftSchedule(mark,m_schd_info,shift_index))
	{
		CTG_SingleMark *prev=pData->GetSingleMarkByShiftString(mark.GetShiftIndexString()); // �����ظ���� 20090702
		if(prev)
		{
			mark.SetBlockIndex(prev->GetBlockIndex());  
			mark.SetGPCIndex(prev->GetGPCIndex()); 
			return pData->UpdateMark(m_schd_info,mark,SYCFLAG_NULL);//GetInterfaceFlag());
		}
		else
		{
			mark.SetBlockIndex(NULL_TRAININDEX); //��������,Ĭ��INDEX
			mark.SetGPCIndex(pData->GetAvaGPCIndex()); 
			return pData->AppendMark(m_schd_info,mark,SYCFLAG_NULL);//GetInterfaceFlag());
		}

	}
	return FALSE;
}

BOOL  CWintgViewMMI::AddMark_BasicSchedule(TRAIN_INDEX shift_index)
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleMark mark;
	if(m_pSchdHolder->GenMark_BasicSchedule(mark,m_schd_info,shift_index))
	{
		CTG_SingleMark *prev=pData->GetSingleMarkByShiftString(mark.GetShiftIndexString()); // �����ظ���� 20090702
		if(prev)
		{
			mark.SetBlockIndex(prev->GetBlockIndex());  
			mark.SetGPCIndex(prev->GetGPCIndex()); 
			return pData->UpdateMark(m_schd_info,mark,SYCFLAG_NULL);//GetInterfaceFlag());
		}
		else
		{
			mark.SetBlockIndex(NULL_TRAININDEX); //��������,Ĭ��INDEX
			mark.SetGPCIndex(pData->GetAvaGPCIndex()); 
			return pData->AppendMark(m_schd_info,mark,SYCFLAG_NULL);//GetInterfaceFlag());
		}

	}
	return FALSE;
}

void   CWintgViewMMI::OnClearTimeoutOper()
{
	time_t curtm=time(NULL);
	list<BackupOper>::iterator iter=m_listBackupOper.begin();
	while(iter!=m_listBackupOper.end())
	{
		if(iter->m_opertm<(curtm-MAX_BACKUP_TIMERANGE))
			iter=m_listBackupOper.erase(iter);
		else
			break;
	}
}

void   CWintgViewMMI::PushOper(const CTG_LogicObject *pobj,TG_OPER_TYPE oper)
{
	if(m_listBackupOper.size()>MAX_OPER_BACKUP_NUM)
		m_listBackupOper.pop_front();

	BackupOper  x;
	time_t curtm=time(NULL);
	x.SetValue(pobj,oper,curtm);
	m_listBackupOper.push_back(x);

}

BOOL   CWintgViewMMI::PopOper(const CTG_ScheduleInfo &info)
{
	BackupOper x;
	if(m_listBackupOper.size()>0)
	{
		x=m_listBackupOper.back();
		m_listBackupOper.pop_back();
	}
	else
		return FALSE;

	CTG_TrainScheduleBase *pData=GetDataStore(info);
	if(!pData)
		return FALSE;

	// �����������ܳ�����ʵ�ʵ�
	if(x.m_pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		TRAIN_INDEX train_index=((CTG_SingleTrain *)x.m_pobj)->GetTrainIndex();
		CTG_SingleTrain* ptrain=pData->GetSingleTrain(train_index);
		if(ptrain!=NULL)
		{
			for(int m=0; m<ptrain->GetTrainRecordCount(); m++)
			{
				CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(m);
				if(prec==NULL)
					continue;

				if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
				{
					GpcLogTrainMessage(0,0,"����ʧ��,�г�train=%s(%u)��%dվ����ʵ�ʵ�",
						ptrain->GetTrainID(),train_index,prec->GetStationNo());
					return TRUE;
				}
			}
		}
	}

	if(x.m_oper==TG_OPER_ADD)
	{
		if(x.m_pobj->GetObjectType()==TG_OBJ_TRAIN)
		{
			GpcLogTrainMessage(0,0,"���� AddTrain, train=%s index=%d",((CTG_SingleTrain *)x.m_pobj)->GetTrainID(),((CTG_SingleTrain *)x.m_pobj)->GetTrainIndex());
			pData->DeleteTrain(info,*(CTG_SingleTrain *)x.m_pobj,SYCFLAG_NULL);
		}
		else
			if(x.m_pobj->GetObjectType()==TG_OBJ_MARK)
			{
				GpcLogTrainMessage(0,0,"���� AddMark, markindex=%d",((CTG_SingleMark *)x.m_pobj)->GetBlockIndex());
				pData->DeleteMark(info,*(CTG_SingleMark *)x.m_pobj,SYCFLAG_NULL);
			}
	}
	else
		if(x.m_oper==TG_OPER_UPDATE)
		{
			if(x.m_pobj->GetObjectType()==TG_OBJ_TRAIN)
			{
				GpcLogTrainMessage(0,0,"���� UpdateTrain, train=%s index=%d",((CTG_SingleTrain *)x.m_pobj)->GetTrainID(),((CTG_SingleTrain *)x.m_pobj)->GetTrainIndex());
				pData->UpdateTrain(info,*(CTG_SingleTrain *)x.m_pobj,SYCFLAG_NULL);
			}
			else
				if(x.m_pobj->GetObjectType()==TG_OBJ_MARK)
				{
					GpcLogTrainMessage(0,0,"���� UpdateMark, markindex=%d",((CTG_SingleMark *)x.m_pobj)->GetBlockIndex());
					pData->UpdateMark(info,*(CTG_SingleMark *)x.m_pobj,SYCFLAG_NULL);
				}
		}
		else
			if(x.m_oper==TG_OPER_DELETE)
			{
				if(x.m_pobj->GetObjectType()==TG_OBJ_TRAIN)
				{
					GpcLogTrainMessage(0,0,"���� DeleteTrain, train=%s index=%d",((CTG_SingleTrain *)x.m_pobj)->GetTrainID(),((CTG_SingleTrain *)x.m_pobj)->GetTrainIndex());
					pData->UpdateTrain(info,*(CTG_SingleTrain *)x.m_pobj,SYCFLAG_NULL);
				}
				else
					if(x.m_pobj->GetObjectType()==TG_OBJ_MARK)
					{
						GpcLogTrainMessage(0,0,"���� DeleteMark, markindex=%d",((CTG_SingleMark *)x.m_pobj)->GetBlockIndex());
						pData->UpdateMark(info,*(CTG_SingleMark *)x.m_pobj,SYCFLAG_NULL);
					}

			}
			return TRUE;

}

BOOL   CWintgViewMMI::CanBackOper()
{
	if(m_listBackupOper.size()>0)
		return TRUE;
	else
		return FALSE;
}

bool SetTrainInOutIndex(CTG_SingleTrain& objtrain, const TrainForExitEntry& train)
{
	if(objtrain.GetTrainRecordCount()!=train.GetTrainRecordCount())
		return false;
	for(int i=0; i<objtrain.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord* prec=objtrain.GetTrainRecordByRecIndex(i);
		const TmpRecord* pold=train.GetTrainRecord(i);
		if(prec!=NULL && pold!=NULL)
		{
			if(prec->m_data.station == pold->station)
			{
				if(pold->in_index>=0 && pold->out_index>=0)
				{
					prec->SetArriveTGIndex(pold->in_index);
					prec->SetDepartTGIndex(pold->out_index);
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

BOOL   CWintgViewMMI::AddTrain_Directly(CTG_SingleTrain &train)  // ������̨�ƻ����ߵ����ƻ���������ʽ���޸Ĳ������ǽ���ֱ�Ӳ���
{
	CaculateTrainSlowTime(&train); // 20170903 ����ϻ�����
	if(m_pTGCtrl!=NULL)
	{
		CString newxml=m_pTGCtrl->SendTrainMsgToOCX(train, CTgX::SEND_FLAG::Set_TrainDefault);
		if(newxml!="")
		{
			TrainForExitEntry train_tmp;
			if(fromXML(train_tmp,newxml))
			{
				if(SetTrainInOutIndex(train, train_tmp))
					GpcLogTrainMessage(0, 0, "��������վ��ɹ�");
				else
					GpcLogTrainMessage(0, 0, "��������վ��ʧ��");
			}
			else
			{
				GpcLogTrainMessage(0, 0, "[Error] OCX����վ��ɹ�,���ǽ���ʧ��");
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "[Error] OCX����վ��ʧ��");
		}
	}

	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	train.SetTrainIndex(NULL_TRAININDEX); //��������,Ĭ��INDEX
	train.SetGPCTrainIndex(pData->GetAvaGPCIndex());
	train.SetUpdateTime(time(NULL));
	if(GetTGFunctionConfig()->IsEnableNewCzh())
	{
		TRAIN_INDEX prev_train_index=0;
		TRAIN_INDEX shift_index=train.GetShiftTrainIndex();
		TRAIN_INDEX prev_shift_index=train.GetPrevTrainShiftTrainIndex();
		if(shift_index != 0 && prev_shift_index != 0)
		{
			CTG_TrainRecord* pFirst=train.GetTrainFirstRecord();
			if(pFirst)
			{
				int station=pFirst->GetStationNo();
				CTG_SingleTrain *prev_train=pData->GetPrevTrainByShiftIndex(prev_shift_index,station);
				if(prev_train)
					prev_train_index=prev_train->GetTrainIndex();
				else
					prev_train_index=0;
			}
		}
		else
		{
			TRAIN_INDEX basic_index=train.GetBasicTrainIndex();
			TRAIN_INDEX prev_basic_index=train.GetPrevTrainBasicTrainIndex();
			if(basic_index != 0 && prev_basic_index != 0)
			{
				CTG_TrainRecord* pFirst=train.GetTrainFirstRecord();
				if(pFirst)
				{
					int station=pFirst->GetStationNo();
					int tm=pFirst->GetDepartTime();
								
					CTG_SingleTrain *prev_train=pData->GetPrevTrainByBasicIndex(prev_basic_index,station,tm,train.GetTrainID());
					if(prev_train)
						prev_train_index=prev_train->GetTrainIndex();
					else
						prev_train_index=0;
				}
			}
		}
		train.SetPrevTrainIndex(prev_train_index);
	}
	TRAIN_INDEX	 trainindex=pData->AppendTrain(m_schd_info,train,SYCFLAG_NULL); // �������κθ��£��ȴ�OCX���ò�������
	if(trainindex==NULL_TRAININDEX)
		return FALSE;
	train.SetTrainIndex(trainindex);
	
	if(GetTGFunctionConfig()->IsEnableNewCzh())
	{
		TRAIN_INDEX shift_index=train.GetShiftTrainIndex();
		if(shift_index != 0)
		{
			CTG_TrainRecord* pLast=train.GetTrainLastRecord();
			if(pLast)
			{
				int station=pLast->GetStationNo();
				CTG_SingleTrain *pnext_train=pData->GetNextTrainByShiftIndex(shift_index, station);
				if(pnext_train)
				{
					pnext_train->SetPrevTrainIndex(train.GetTrainIndex());
					pData->SaveExpandTrain(m_schd_info, pnext_train);
					//������
					pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
				}
			}
		}
		else
		{
			TRAIN_INDEX basic_index=train.GetBasicTrainIndex();
			if(basic_index != 0)
			{
				CTG_TrainRecord* pLast=train.GetTrainLastRecord();
				if(pLast)
				{
					int station=pLast->GetStationNo();
					int tm=pLast->GetArriveTime();
				
					CTG_SingleTrain *pnext_train=pData->GetNextTrainByBasicIndex(basic_index, station, tm, train.GetTrainID());
					if(pnext_train)
					{
						pnext_train->SetPrevTrainIndex(train.GetTrainIndex());
						pData->SaveExpandTrain(m_schd_info, pnext_train);
						//������
						pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
					}
				}
			}
		}
	}

	CTG_SingleTrain *prev=pData->GetSingleTrain(trainindex);
	if(prev)
		PushOper(prev,TG_OPER_ADD);
	return TRUE;
}

// �·��׶μƻ�ֱ�Ӹ���TG����
BOOL   CWintgViewMMI::UpdateMMITrain_Directly(ULONG train_index) 
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain *prev=pData->GetSingleTrain(train_index);
	if(!prev)
		return FALSE;

	m_pTGCtrl->UpdateTrainOCX(*prev);
	
	return TRUE;
}

BOOL   CWintgViewMMI::UpdateTrain_Directly(CTG_SingleTrain &train, bool bCheckRelation, bool bMoveNextTrain) // ������̨�ƻ����ߵ����ƻ���������ʽ���޸Ĳ������ǽ���ֱ�Ӳ���
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain *prev=pData->GetSingleTrain(train.GetTrainIndex());
	if(!prev)
		return FALSE;
	PushOper(prev,TG_OPER_UPDATE);
	train.ComparePrevAndUpateFlags(*prev);   // ���ڼ�¼ɾ��������Ᵽ������  //// 521�ƻ��Ż��޸�
	
	// ����20180904��̨�ƻ��н�·��ϵʱ,��������,ͬ��·�����г�Ҳƽ��	
	if(GetTGFunctionConfig()->IsEnableNewCzh() && bMoveNextTrain && (m_pTGCtrl!=NULL))
	{
		GpcLogTrainMessage(0,0,"��ͼƽ�Ƴ���%s(%u)�ĺ�����·�г�", train.GetTrainID(), train.GetTrainIndex());

		CTG_TrainRecord *pold=prev->GetTrainLastRecord();
		CTG_TrainRecord *pnew=train.GetTrainLastRecord();
		int offset=0;
		if(pold!=NULL && pnew!=NULL)
		{
			if(pold->GetStationNo()==pnew->GetStationNo() && pold->IsTerminalRec() && pnew->IsTerminalRec())
			{
				offset = pnew->GetArriveTime()-pold->GetArriveTime();
			}
		}

		if(offset>0)
		{
			TRAIN_INDEX prev_train_index=0, next_train_index=0, this_train_index=train.GetTrainIndex();
			do
			{
				CString xml=m_pTGCtrl->GetTrainJiaoRelation(this_train_index);
				prev_train_index=0;
				next_train_index=0;
				this_train_index=0; // ׼����ֹѭ��
				if(GetPrevAndAfterTrainIndex(xml, prev_train_index, next_train_index))
				{
					if(next_train_index!=0)
					{
						CTG_SingleTrain *pnext=pData->GetSingleTrain(next_train_index);
						if(NULL!=pnext)
						{
							CTG_SingleTrain next_train=*pnext;
							next_train.OffsetScheduleTime(0, offset, false, 1);
							next_train.SetUpdateTime(time(NULL));
							pData->UpdateTrain(m_schd_info,next_train,SYCFLAG_NULL);

							this_train_index = next_train_index;
						}
					}
				}
			}while(this_train_index!=0);
		}
	}

	if(GetTGFunctionConfig()->IsEnableNewCzh() && bCheckRelation)
	{
		TRAIN_INDEX prev_train_index=0;
		TRAIN_INDEX shift_index=train.GetShiftTrainIndex();
		TRAIN_INDEX prev_shift_index=train.GetPrevTrainShiftTrainIndex();
		if(shift_index != 0 && prev_shift_index != 0)
		{
			CTG_TrainRecord* pFirst=train.GetTrainFirstRecord();
			if(pFirst)
			{
				int station=pFirst->GetStationNo();
				CTG_SingleTrain *prev_train=pData->GetPrevTrainByShiftIndex(prev_shift_index,station);
				if(prev_train)
				{
					prev_train_index=prev_train->GetTrainIndex();
					// �Ƿ��г��뼴�������ĳ��й�����ϵ
					CTG_SingleTrain *pnext_train=pData->GetNextTrainByPrevTrainIndex(prev_train_index);
					if(pnext_train)
					{
						pnext_train->SetPrevTrainIndex(0);
						pData->SaveExpandTrain(m_schd_info, pnext_train);
						//������
						pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
					}
				}
				else
					prev_train_index=0;
			}
			train.SetPrevTrainIndex(prev_train_index);
		}
		else
		{
			TRAIN_INDEX basic_index=train.GetBasicTrainIndex();
			TRAIN_INDEX prev_basic_index=train.GetPrevTrainBasicTrainIndex();
			if(basic_index != 0 && prev_basic_index != 0)
			{
				CTG_TrainRecord* pFirst=train.GetTrainFirstRecord();
				if(pFirst)
				{
					int station=pFirst->GetStationNo();
					TIME tm=pFirst->GetDepartTime();
				
					CTG_SingleTrain *prev_train=pData->GetPrevTrainByBasicIndex(prev_basic_index,station,tm,train.GetTrainID());
					if(prev_train)
					{
						prev_train_index=prev_train->GetTrainIndex();
						// �Ƿ��г��뼴�������ĳ��й�����ϵ
						CTG_SingleTrain *pnext_train=pData->GetNextTrainByPrevTrainIndex(prev_train_index);
						if(pnext_train)
						{
							pnext_train->SetPrevTrainIndex(0);
							pData->SaveExpandTrain(m_schd_info, pnext_train);
							//������
							pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
						}
					}
					else
						prev_train_index=0;
				}
				train.SetPrevTrainIndex(prev_train_index);
			}
		}
	
		shift_index=train.GetShiftTrainIndex();
		if(shift_index != 0)
		{
			CTG_TrainRecord* pLast=train.GetTrainLastRecord();
			if(pLast)
			{
				int station=pLast->GetStationNo();
				CTG_SingleTrain *pnext_train=pData->GetNextTrainByShiftIndex(shift_index, station);
				if(pnext_train)
				{
					pnext_train->SetPrevTrainIndex(train.GetTrainIndex());
					pData->SaveExpandTrain(m_schd_info, pnext_train);
					//������
					pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
				}
			}
		}
		else
		{
			TRAIN_INDEX basic_index=train.GetBasicTrainIndex();
			if(basic_index != 0)
			{
				CTG_TrainRecord* pLast=train.GetTrainLastRecord();
				if(pLast)
				{
					int station=pLast->GetStationNo();
					TIME tm=pLast->GetArriveTime();
				
					CTG_SingleTrain *pnext_train=pData->GetNextTrainByBasicIndex(basic_index, station, tm, train.GetTrainID());
					if(pnext_train)
					{
						pnext_train->SetPrevTrainIndex(train.GetTrainIndex());
						pData->SaveExpandTrain(m_schd_info, pnext_train);
						//������
						pnext_train->SetSyncStatusX(SYCFLAG_NULL,TRUE);
					}
				}
			}
		}
	}

	train.SetUpdateTime(time(NULL));
	pData->UpdateTrain(m_schd_info,train,SYCFLAG_NULL);

	return TRUE;
}

BOOL   CWintgViewMMI::DeleteTrain_Directly(TRAIN_INDEX train_index)  // ������̨�ƻ����ߵ����ƻ���������ʽ���޸Ĳ������ǽ���ֱ�Ӳ���
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	if(train_index==NULL_TRAININDEX)
		return FALSE;
	CTG_SingleTrain *prev=pData->GetSingleTrain(train_index);
	if(!prev)
		return FALSE;
	PushOper(prev,TG_OPER_DELETE);

	if (!prev->GetTrainRecordCount() && prev->GetTrainRecordCountWithDeleted())
		return TRUE;

	if(prev->HasSentPlan() || pData->IsTrainInSendPlanList(prev)) // �����¹��ƻ�
	{
		GpcLogTrainMessage(0,0,"���0��¼���� ���� %s ID=%u OPMSID=%u %d staList %s",
			prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), 
			prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());

		prev->SetTrainRecordCount(0);
		prev->SetUpdateTime(time(NULL));
		prev->SetSyncStatusX(SYCFLAG_NULL,TRUE);			
	}
	else
	{
		GpcLogTrainMessage(0,0,"ֱ��ɾ�� ���� %s ID=%u OPMSID=%u %d staList %s",
			prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), 
			prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());
		pData->DeleteTrain(m_schd_info,train_index,SYCFLAG_NULL); //δ�¹��ƻ�,ֱ��ɾ��
	}

	return TRUE;
}

BOOL   CWintgViewMMI::GenTrainNodeIndex(TRAIN_INDEX train_index)  // ȫ������index
{
	m_pTGCtrl->TGGenTrainNodeIndex("");
	return TRUE;
}

BOOL  CWintgViewMMI::SetFreedrawTrainPlanTime(CTG_SingleTrain& train, int rec_index)
{
	if(!GetTGFunctionConfig()->IsFreeDrawSetBasicTrainPlanTime())
		return TRUE;

	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return FALSE;
	CTG_SingleTrain srctrain;
	if(!m_pSchdHolder->GenTrain_BasicSchedule(srctrain,m_schd_info,train.GetTrainID(),NULL,-1,-1))
	{
		GpcLogTrainMessage(0, 0, "[ƥ��ͼ���ƻ�ʱ��]: %s,�����ڻ���ͼ���Ҳ����ó�,�޷�ƥ��", train.GetTrainID());
		return FALSE;
	}
	
	int offset_time = 0;
	// ƥ��ͼ���ƻ�ʱ��
	for(int i=rec_index;i<train.GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=train.GetTrainRecord(i);
		CTG_TrainRecord *prec_next=train.GetTrainRecord(i+1);
		if(prec==NULL)
			continue;

		const CTG_TrainRecord *prec_basic=srctrain.GetTrainRecordNoDelete_Const(prec->GetIndex());
		if(!prec_basic)  // ��·�ͻ���ͼ��һ��,��ֹƥ��
			break;
		if(prec->GetStationNo()!=prec_basic->GetStationNo()) // ��·�ͻ���ͼ��һ��,��ֹƥ��
			break;

		const CTG_TrainRecord *prec_basic_next=srctrain.GetTrainRecordNoDelete_Const(prec_basic->GetIndex()+1);
		if(prec_next && !prec_basic_next)  // ��·�ͻ���ͼ��һ��,��ֹƥ��
			break;
		if(prec_next && prec_basic_next && prec_next->GetStationNo()!=prec_basic_next->GetStationNo()) // ��·�ͻ���ͼ��һ��,��ֹƥ��
			break;
		
		
		if(i==rec_index)
		{
			offset_time = prec_basic->m_data.plan_arrive-prec->m_data.arrive;
			GpcLogTrainMessage(0, 0, "[ƥ��ͼ���ƻ�ʱ��]: %s ��վ %d, origin_offset_time=%d", train.GetTrainID(), prec->m_data.station, offset_time);

			if(offset_time>=18*3600)
				offset_time -= 24*3600;
			else if(offset_time<=-18*3600)
				offset_time += 24*3600;
			else
				offset_time=0;

			GpcLogTrainMessage(0, 0, "[ƥ��ͼ���ƻ�ʱ��]: %s ��վ %d, offset_time=%d", train.GetTrainID(), prec->m_data.station, offset_time);
		}
		
		prec->m_data.plan_arrive = prec_basic->m_data.plan_arrive+offset_time;
		prec->m_data.plan_depart = prec_basic->m_data.plan_depart+offset_time;
		
		GpcLogTrainMessage(0, 0, "[ƥ��ͼ���ƻ�ʱ��]: %s ��վ %d, plan_arrival: %d(basic_plan_arrival:%d) plan_depart: %d(basic_plan_depart:%d)", 
						train.GetTrainID(), prec->m_data.station,
						prec->m_data.plan_arrive, prec_basic->m_data.plan_arrive, 
						prec->m_data.plan_depart, prec_basic->m_data.plan_depart);

	}

	if(i<train.GetTrainRecordCount())
		return FALSE;
	else
		return TRUE;
}

void  CWintgViewMMI::UpdateTGFromData()
{
	CTG_TrainScheduleBase *pData=GetDataStore(m_schd_info);
	if(!pData)
		return;
	m_pSchdHolder->PollingForUpdate(pData,this,TRUE);
}

BOOL   CWintgViewMMI::AdjustTrainForCollision(CTG_SingleTrain &train,int recIndex)
{
	CString tmp;
	tmp.Format("�ò������޸��г� %s �� %s �������ĵ���ʱ�̺͹ɵ�,������Ӧ���˶�,ȷ�ϼ�����?",
		train.GetTrainID(),gpcdef.GetStationName(train.GetTrainRecord(recIndex)->GetStationNo()));

	if(m_pParentWnd->MessageBox(tmp,"����",MB_YESNO)!=IDYES)
		return FALSE;
	GpcLogTrainMessage(0, 0, "[����Ա����] ͨ��ѡ���г��������Զ���������,�޸��г� %s(%d) ��վ %d",
		train.GetTrainID(),train.GetTrainIndex(), train.GetTrainRecord(recIndex)->GetStationNo());
	
	// ��ȡ�ó�ʵ�ʵ���һ���ƻ�������
	int nRecIndex=train.GetFirstPlanRecordIndex();
	
	int nIndex = (nRecIndex>recIndex)?nRecIndex:recIndex;
	if(nIndex>=train.GetTrainRecordCount()-1)
		return FALSE;

	if(m_pSchdHolder->AdjustTrainForCollision(train,nIndex,m_schd_info))
	{
		return UpdateTrain_Directly(train);
	}
	return FALSE;
}
BOOL  CWintgViewMMI::AdjustTrainForMutiPass(CTG_SingleTrain &train,int recIndex)
{
	CString tmp;
	tmp.Format("�ò��������޸��г� %s �� %s �������ĵ���ʱ�̺͹ɵ�,������Ӧ���˶�,ȷ�ϼ�����?",
		train.GetTrainID(),gpcdef.GetStationName(train.GetTrainRecord(recIndex)->GetStationNo()));

	if(m_pParentWnd->MessageBox(tmp,"����",MB_YESNO)!=IDYES)
		return FALSE;
	GpcLogTrainMessage(0, 0, "[����Ա����] ͨ��ѡ���г������߲����õ�������,�޸��г� %s(%d) ��վ %d",
		train.GetTrainID(),train.GetTrainIndex(), train.GetTrainRecord(recIndex)->GetStationNo());

	// ��ȡ�ó�ʵ�ʵ���һ���ƻ�������
	int nRecIndex=train.GetFirstPlanRecordIndex();

	int nIndex = (nRecIndex>recIndex)?nRecIndex:recIndex;
	if(nIndex>=train.GetTrainRecordCount()-1)
		return FALSE;

	if(m_pSchdHolder->AdjustTrainForMutiPass(train,nIndex,m_schd_info))
	{
		return UpdateTrain_Directly(train);
	}
	return FALSE;
}

BOOL CWintgViewMMI::AdjustTrainForBasicSchedule(CTG_SingleTrain &train,int recIndex)
{
	// ��ȡ�ó�ʵ�ʵ���һ���ƻ�������
	int nRecIndex=train.GetFirstPlanRecordIndex();

	int nIndex = (nRecIndex>recIndex)?nRecIndex:recIndex;
	if(nIndex>=train.GetTrainRecordCount()-1)
		return FALSE;

	if(m_pSchdHolder->AdjustTrainForBasicSchedule(train,nIndex,m_schd_info))
	{
		return UpdateTrain_Directly(train);
	}
	return FALSE;
}

BOOL   CWintgViewMMI::AdjustTrainsForMark(const CTG_SingleMark *pmark) // ����mark ������س���
{
	if(m_pParentWnd->MessageBox("�ò������޸����ʩ��������������ߵĵ���ʱ�̺͹ɵ�,������Ӧ���˶�,ȷ�ϼ�����?","����",MB_YESNO)!=IDYES)
		return FALSE;
	GpcLogTrainMessage(0, 0, "[����Ա����] ѡ��ʩ������ %d ͨ���Զ���������,�޸���������ߵĵ���ʱ�̺͹ɵ�",pmark->GetBlockIndex());
	CTG_ObjectPtrGroup relatrains;
	m_pSchdHolder->GetRelaTrainsOfMark(pmark,relatrains,m_schd_info);

	for(int pri=0;pri<=MAX_PRIORITY_VALUE;pri++)  // ���ȸ����ȼ��г�,priorityֵС���г�
	for(int i=0;i<relatrains.GetCount();)
	{
		 CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)relatrains.GetPtr(i);
		 if(pRelaTrain->pTrain->GetPriority()!=pri)
		 {
			  i++;
			  continue;
		 }
		 CTG_SingleTrain newtrain=*pRelaTrain->pTrain;
		 GpcLogTrainMessage(0, 0, "[����Ա����] ʩ�����ŵ��Զ�����, �漰�г������� %s(%d)",newtrain.GetTrainID(),newtrain.GetTrainIndex());

		 if(m_pSchdHolder->AdjustSingleTrainForMark(newtrain,pRelaTrain->nRecIndex,pmark,m_schd_info))
			 UpdateTrain_Directly(newtrain);
		 relatrains.RemovePtr(i);  // 2011.01.06 ���BUG,�����ɾ���ڵ㣬��ô����ѭ���������ͷŵ�ָ��
	}
	return TRUE;
}

void  CWintgViewMMI::PollingForDeleteTrain(TRAIN_INDEX train_index)
{
	if(m_pTGCtrl)
	{
		if(1==m_pTGCtrl->DataIsExistInOCX(train_index, 1))
		{
			m_pTGCtrl->DeleteTrainOCX(train_index,1);
		}
	}
}

void  CWintgViewMMI::PollingForDeleteBlock(TRAIN_INDEX index)
{
	if(m_pTGCtrl)
	{
		if(1==m_pTGCtrl->DataIsExistInOCX(index, 2))
		{
			m_pTGCtrl->DeleteMarkOCX(index);
		}
	}
}

void   CWintgViewMMI::PollingForAppendTrain(CTG_SingleTrain* ptrain)
{
	if(ptrain && m_pTGCtrl)
	{
		ULONG train_index=ptrain->GetTrainIndex();
		if(1!=m_pTGCtrl->DataIsExistInOCX(train_index, 1))
		{
			m_pTGCtrl->AppendTrainOCX(*ptrain);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "%s(%u)��OCX���Ѵ���,����Ҫ��ӵ�OCX", ptrain->GetTrainID(),train_index);
		}
	}
}

void   CWintgViewMMI::PollingForAppendBlock(CTG_SingleMark* pblock)
{
	if(pblock && m_pTGCtrl)
	{
		ULONG block_index=pblock->GetBlockIndex();
		if(1!=m_pTGCtrl->DataIsExistInOCX(block_index, 2))
		{
			m_pTGCtrl->AppendMarkOCX(*pblock);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "��ʩ�������Ѵ���,����Ҫ����ʩ������ %u", block_index);
		}
	}
}

void   CWintgViewMMI::PollingForMoveTGOCX(void)
{
	if(m_pTGCtrl)
		m_pTGCtrl->MoveTGOCXOneStep();
}


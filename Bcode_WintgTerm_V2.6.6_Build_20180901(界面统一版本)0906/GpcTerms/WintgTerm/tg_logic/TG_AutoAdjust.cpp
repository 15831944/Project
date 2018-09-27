#include "stdafx.h"
#include "tg_work_schdule_holder.h"
#include "tg_defines.h"
#include "tg_autoadjust.h"
#include "gpcdef.h"
#include "sysshare.h"
extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);
extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
const short  NEAR_TRAIN_TIME_RANGE=1800;

CTGAutoAdjustLogic::CTGAutoAdjustLogic(const CTGFunctionConfig  *pConfig,CTGBasicParam *pparam,CTG_TrainScheduleBase *pbasic_schd,CTG_TrainScheduleBase *pwork_schd)
{
	if(NULL!=pwork_schd)
	{
		CTG_ScheduleInfo info;
		pwork_schd->GetScheduleInfo(info);
		m_nAdjustSchdType=info.m_nType;
	}
	else
	{
		m_nAdjustSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;
	}

	m_pBasicSchedule=pbasic_schd;
	m_pWorkSchedule=pwork_schd;
	m_pBasicPara=pparam;
	m_pConfig=pConfig;

}
CTGAutoAdjustLogic::~CTGAutoAdjustLogic(void)
{
}

TIME CTGAutoAdjustLogic::CacuTrainDepartTime(const CTG_SingleTrain *psourcetrain,int rec_index, TIME ltime)
{
	const CTG_TrainRecord *ori_rec=psourcetrain->GetTrainRecordNoDelete_Const(rec_index);
	if(!ori_rec)
		return NULL_TIME;

	TIME default_return = ori_rec->GetDepartTime();
	if (ori_rec->IsStartRec() || ori_rec->IsTerminalRec())
		return default_return;

	if (NULL_TIME == ltime)
		ltime = TG_GetValidADTime(ori_rec->m_data);

	STATION_NO station = ori_rec->GetStationNo();
	ENTRY_ID   exit_id = ori_rec->GetExit();

	TIME near_time=ltime+NEAR_TRAIN_TIME_RANGE;

	short flag=0;

	ARRIVE_DEPART_STATUS status;

	RUN_TYPE type;


	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group);
	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if (ptrain->GetTrainIndex()==psourcetrain->GetTrainIndex())
			continue;

		CTG_TrainRecord *prec=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
		if(!prec)
			continue;

		if(prec->GetArriveTime()< ltime)   // ֮ǰ���г����� 
			continue;
		prec=ptrain->GetTrainRecord(0);
		if(!prec)
			continue;
		if(prec->GetArriveTime() > ltime && (prec->GetDepartTime() - ltime)> NEAR_TRAIN_TIME_RANGE) // ֮����г�����
			continue;

		if (ptrain->IsPassengerTrain())
			type = PASSANGER_TRAIN;
		else 
			type = FREIGHT_TRAIN;

		prec=ptrain->GetTrainRecordByStation(station);
		if(!prec)
			continue;
		if(prec->GetArriveTime() >= ltime && prec->GetEntry()== exit_id) 
		{ // diff dir
			if(prec->GetArriveTime()< near_time)
			{
				near_time = prec->GetArriveTime();
				flag = 1;
				status = prec->IsPassRec() ? STATION_PASS : STATION_ARRIVE;
			}
		} 
		else if (prec->GetDepartTime() > ltime && prec->GetExit() == exit_id) 
		{ // same dir
			if (prec->GetDepartTime()< near_time)
			{
				near_time = prec->GetDepartTime();
				flag = 0;
				status = prec->IsPassRec() ? STATION_PASS : STATION_DEPART;
			}
		}
	}

	if (near_time >= ltime+NEAR_TRAIN_TIME_RANGE || flag == 0)

		//next train too far,or nearest train is same dir

		return default_return;

	//get time gap

	ARRIVE_DEPART_STATUS ori_status=STATION_DEPART;
	RUN_TYPE ori_type=PASSANGER_TRAIN;
	if (ori_rec->IsPassRec())  status = STATION_PASS;
	if (!psourcetrain->IsPassengerTrain()) ori_type = FREIGHT_TRAIN;   
	return near_time + m_pBasicPara->GetTrainGapTime(station, MEET_TRAIN_GAP, ori_status, status, ori_type, type);
}

BOOL  CTGAutoAdjustLogic::CacuDefaultSide(CTG_SingleTrain *ptrain)
{
	int rec_count=ptrain->GetTrainRecordCount();
	for(int i=0;i<rec_count;i++)
	{
		CacuDefaultSide(ptrain,i);
	}
	return TRUE;
}

BOOL  CTGAutoAdjustLogic::CacuDefaultSide(CTG_SingleTrain *ptrain,int rec_index)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	
	// side cannot be changed
	if (TG_IsForceSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)�ɵ����ɸı�", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	

	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) ��վ %d û������", ptrain->GetTrainID(),ptrain->GetTrainIndex(),station);
			break;
		}

		BOOL if_pass = FALSE;
		if (pRec->IsPassRec() && !pRec->IsStartRec() && !pRec->IsTerminalRec()) //����վͨ�����Ȳ���ʼ��վҲ�����յ�վ
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				break;
			}
			else if (m_pConfig->IfChoseMainSideWhilePass())
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		
		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)��StationSide.def��û�����ÿ�ѡ�ɵ�", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		if(selected_side_count<=0)
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)��StationSide.def�����ÿ�ѡ�ɵ�Ϊ0", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		
		ori_rec.arrive_side = order_side[0];
		ori_rec.depart_side = order_side[0];
		

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (!m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;

				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)�ɵ�ռ������,���ɷ���", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::OnlyUseFirstStopSideForNanchang(CTG_SingleTrain *ptrain, int rec_index)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	
	// side cannot be changed
	if(IS_VALID_SIDE(pRec->m_data.arrive_side) || IS_VALID_SIDE(pRec->m_data.depart_side))
	{
		if (TG_IsForceSide(ori_rec))
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
	}
	// if route is making or occuped, we do not change side automaticaly
	CStringArray list;
	if (TG_IsArriveRouteDoing(ori_rec))
	{
		list.Add("�ӳ���·���ڴ���");
	}
	if (TG_IsArriveRouteOccupy(ori_rec))
	{
		list.Add("�ӳ���·ռ��");
	}
	if (TG_IsArriveRouteOk(ori_rec))
	{
		list.Add("�ӳ���·�������");
	}
	if (TG_IsDepartRouteDoing(ori_rec))
	{
		list.Add("������·���ڴ���");
	}
	if (TG_IsDepartRouteOccupy(ori_rec))
	{
		list.Add("������·ռ��");
	}
	if (TG_IsDepartRouteOk(ori_rec))
	{
		list.Add("������·�������");
	}

	if(list.GetCount()>0)
	{
		GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�,ԭ������:", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);

		for(int n=0; n<list.GetCount(); n++)
		{
			GpcLogTrainMessage(0, 0, "%s", list.GetAt(n));
		}
		return FALSE;
	}

	if (TG_IsTrainIntoSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�,�г��Ѿ���վ", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	
	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:OnlyUseFirstStopSideForNanchang %s(%d) ��վ %d û������", ptrain->GetTrainID(),ptrain->GetTrainIndex(), station);
			break;
		}

		BOOL if_pass = FALSE;
		if (pRec->IsPassRec() && !pRec->IsStartRec() && !pRec->IsTerminalRec()) //����վͨ�����Ȳ���ʼ��վҲ�����յ�վ
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				break;
			}
			else if (m_pConfig->IfChoseMainSideWhilePass())
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		
		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)��StationSide.def��û�����ÿ�ѡ�ɵ�", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		if(selected_side_count<=0)
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)��StationSide.def�����ÿ�ѡ�ɵ�Ϊ0", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		
		ori_rec.arrive_side = order_side[0];
		ori_rec.depart_side = order_side[0];
		

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (!m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;

				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]:CacuDefaultSide %s(%d) �ڳ�վ%d(entry %d exit %d)�ɵ�ռ������,���ɷ���", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::ChangeSideForStopOrPass(CTG_SingleTrain *ptrain,int rec_index,int enable_passenger_side_change,int only_pass_main_side,CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	TIME stime,etime;
	if(enable_passenger_side_change<0)
		enable_passenger_side_change=GetTGFunctionConfig()->IsChangeSideWhileStopPass(traininfo.type)?1:0;
	if(only_pass_main_side<0)
		only_pass_main_side=GetTGFunctionConfig()->IfChoseMainSideWhilePass()?1:0;
	
	// side cannot be changed
	if(IS_VALID_SIDE(pRec->m_data.arrive_side) || IS_VALID_SIDE(pRec->m_data.depart_side))
	{
		if (TG_IsForceSide(ori_rec))
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
		if(enable_passenger_side_change!=1 && ptrain->IsPassengerTrain())
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�,��Ϊδ������", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
	}
	// if route is making or occuped, we do not change side automaticaly
	CStringArray list;
	if (TG_IsArriveRouteDoing(ori_rec))
	{
		list.Add("�ӳ���·���ڴ���");
	}
	if (TG_IsArriveRouteOccupy(ori_rec))
	{
		list.Add("�ӳ���·ռ��");
	}
	if (TG_IsArriveRouteOk(ori_rec))
	{
		list.Add("�ӳ���·�������");
	}
	if (TG_IsDepartRouteDoing(ori_rec))
	{
		list.Add("������·���ڴ���");
	}
	if (TG_IsDepartRouteOccupy(ori_rec))
	{
		list.Add("������·ռ��");
	}
	if (TG_IsDepartRouteOk(ori_rec))
	{
		list.Add("������·�������");
	}

	if(list.GetCount()>0)
	{
		GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�,ԭ������:", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);

		for(int n=0; n<list.GetCount(); n++)
		{
			GpcLogTrainMessage(0, 0, "%s", list.GetAt(n));
		}
		return FALSE;
	}
	
	if (TG_IsTrainIntoSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) �ڳ�վ%d(entry %d exit %d)�ùɵ����ɸı�,�г��Ѿ���վ", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	
	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ���Ǹ��Ƿ���վ", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			break;
		}

		stime = ori_rec.arrive;
		etime = ori_rec.depart;

		if (NULL_TIME == stime) stime = etime;
		if (NULL_TIME == etime) etime = stime;

		BOOL if_pass = FALSE;
		if (stime == etime && !TG_IsFlagTerminal(ori_rec.flag) && !TG_IsFlagStart(ori_rec.flag)) //����վͨ�����Ȳ���ʼ��վҲ�����յ�վ
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ��StationSide.def���Ҳ������߹ɵ�", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
				break;
			}
			else if (only_pass_main_side)
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// use reg_side to record the max time of all side utility less than needed (etime) for every side
		TIME reg_time[MAX_SIDE_COUNT+1];
		memset(&reg_time, 0x00, sizeof(reg_time));
		CTG_TrainPtrGroup group;
		m_pWorkSchedule->FindTrains(group);
		for(int train_iter=0;train_iter<group.GetTrainCount();train_iter++)
		{
			CTG_SingleTrain *iter_train=group.GetTrainPtr(train_iter);
			if (ptrain->GetTrainIndex() == iter_train->GetTrainIndex())
			{
				continue;
			}
			
			if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) // 20140929 �޸��հ�ƻ� �ɵ�����
 			{
				if (ptrain->GetShiftTrainIndex() == iter_train->GetShiftTrainIndex() && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
					continue;
			}

			short nRecCount = iter_train->GetTrainRecordCount();
			for (int i=0; i<nRecCount; i++)
			{
				const TG_TrainRecord & rec = iter_train->GetTrainRecord(i)->m_data;
				if (rec.station != station)
					continue;

				TIME ltime = TG_GetValidADTime(rec);
				if (NULL_TIME == ltime)
					continue;

				if (ltime > etime) // donot care about later train
					continue;

				if (rec.arrive_side <0 || rec.arrive_side >= sizeof(reg_time)/sizeof(reg_time[0]))
					continue;

				if (rec.depart_side <0 || rec.depart_side >= sizeof(reg_time)/sizeof(reg_time[0]))
					continue;

				ltime = rec.depart;
				if (NULL_TIME == ltime) ltime = rec.arrive;
				if (reg_time[rec.arrive_side] < ltime)
					reg_time[rec.arrive_side] = ltime;

				break;
			}
		} 

		CTG_ObjectPtrGroup mark_group;
		m_pWorkSchedule->FindMarks(mark_group);
		for(int mark_iter=0;mark_iter<mark_group.GetCount();mark_iter++)
		{
			CTG_SingleMark *iter_mark=(CTG_SingleMark *)mark_group.GetPtr(mark_iter);
			if(!iter_mark) continue;
			const TG_ScheduleBlock &mark = iter_mark->m_data;
			if (TG_SIDE_INHIBIT != mark.type && TG_SIDE_TIANCHUANG != mark.type && TG_SIDE_OCCUPY != mark.type)
			{
				continue;
			}

			if (mark.station != station)
			{
				continue;
			}

			if (mark.start_time > etime)
			{
				continue;
			}

			for(int j=0; j<mark.ele_count; j++)
			{
				SIDE_ID side = (SIDE_ID) mark.ele_number[j];
				if (!m_pBasicPara->VerifyStationSide(station, side))
					continue;

				if (reg_time[side] < mark.end_time)
					reg_time[side] = mark.end_time;			
			}
		}

		// do not need to change
		if (TG_IsFlagStart(ori_rec.flag) && IS_VALID_SIDE(ori_rec.depart_side))
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if (reg_time[ori_rec.depart_side] < stime) 
					return FALSE;
		}
		else if (TG_IsFlagTerminal(ori_rec.flag) && IS_VALID_SIDE(ori_rec.arrive_side)) 
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if (reg_time[ori_rec.arrive_side] < stime) 
					return FALSE;
		}
		else if (ori_rec.arrive_side == ori_rec.depart_side && IS_VALID_SIDE(ori_rec.depart_side)) 
		{
			if (if_pass)
			{
				if (m_pBasicPara->IsMainSide(ori_rec))
					if (reg_time[ori_rec.depart_side] < stime) 
						return FALSE;
                if(only_pass_main_side) // 2011.01.01 ��ĳ�������,��������ɵ���ĵ���,Ӧ���������ͨ��
					break;
			}
			else
			{
				if (m_pBasicPara->IsStopSide(ori_rec))
					if (reg_time[ori_rec.depart_side] < stime) 
						return FALSE;
			}
		}
		else if (IS_VALID_SIDE(ori_rec.arrive_side) && IS_VALID_SIDE(ori_rec.depart_side)) 
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if ((reg_time[ori_rec.arrive_side] < stime && reg_time[ori_rec.arrive_side] < stime) 
					&& (reg_time[ori_rec.depart_side] < stime && reg_time[ori_rec.depart_side] < stime))
					return FALSE;
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		ori_rec.arrive_side = main_side;
		ori_rec.depart_side = main_side;

		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			if(GetTGFunctionConfig()->IsTrainPassToStopUseMainSide())
			{
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ��StationSide.def��û�����ÿ�ѡ�ɵ�, ʹ�����߹ɵ�", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
				return TRUE;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ChangeSideForStopOrPass ��StationSide.def��û�����ÿ�ѡ�ɵ�", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
			}
			break;
		}

		if(selected_side_count<=0)
		{
			if(GetTGFunctionConfig()->IsTrainPassToStopUseMainSide())
			{
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ��StationSide.def�����ÿ�ѡ�ɵ�Ϊ0��, ʹ�����߹ɵ�", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
				return TRUE;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), ��StationSide.def�����ÿ�ѡ�ɵ�Ϊ0��", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
			}
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		for(int i=0; i<selected_side_count; i++)
		{
			SIDE_ID side = order_side[i];
			if (side < 0 || side >= sizeof(reg_time)/sizeof(reg_time[0]))
			{
				continue;
			}

			if (reg_time[side] < stime) 
			{
				ori_rec.arrive_side = side; // �ص����
				ori_rec.depart_side = side;
				break;
			}
		}

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = order_side[0];
				ori_rec.depart_side = order_side[0];
			}
			else
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;
				GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d) ��վ%d(entry %d exit %d), �ɵ�ռ��,���ò��ܷ���ɵ�", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::TrainHasActualTimeRecord(const CTG_SingleTrain *ptrain,int index)
{
	for(int k=index; k<ptrain->GetTrainRecordCount(); k++) 
	{
		const CTG_TrainRecord *prec=ptrain->GetTrainRecordNoDelete_Const(k);
		if(!prec) continue; //NOT possible
		if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
			return TRUE;
	}
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustForStopPassChange(int con_id, CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass)
{
	bool bSideNotChange=false;
	
	CTG_TrainRecord* prec = pnewtrain->GetTrainRecordNoDelete(index);
	if(!prec)
		return FALSE;
	
	BYTE side=prec->m_data.arrive_side; // ����ԭ�ɵ�
	if(pnewtrain->IsPassengerTrain())
		bSideNotChange=GetTGFunctionConfig()->IsSideNotChange(con_id, prec->m_data.station, prec->m_data.arrive_side, 1);
	else
		bSideNotChange=GetTGFunctionConfig()->IsSideNotChange(con_id, prec->m_data.station, prec->m_data.arrive_side, 2);
	
	BOOL ret=AdjustForStopPassChange(pnewtrain,index,stop_to_pass);

	if(bSideNotChange && IS_VALID_SIDE(side))
	{
		CTG_TrainRecord* prec = pnewtrain->GetTrainRecordNoDelete(index);
		if(prec)
		{
			prec->m_data.arrive_side=side;  // �ָ�ԭ�ɵ�
			prec->m_data.depart_side=side;

			if(stop_to_pass)
				GpcLogTrainMessage(0, 0, "����tg_logic.cfg��[STATION_SIDE_NOT_CHANGE]���� %s (%d) �ڳ�վ %d �ɵ�����-->ͨ������%d %d",
					pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), prec->m_data.station,  
					prec->m_data.arrive_side, prec->m_data.depart_side);
			else
				GpcLogTrainMessage(0, 0, "����tg_logic.cfg��[STATION_SIDE_NOT_CHANGE]���� %s (%d) �ڳ�վ %d �ɵ�ͨ��-->��������%d %d",
						pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), prec->m_data.station,  
						prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}

	return ret;
}

BOOL  CTGAutoAdjustLogic::AdjustForStopPassChange(CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass) 
{
	int rec_count=pnewtrain->GetTrainRecordCount();
	CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(index);
	if(!prec)
		return FALSE;
	int station=prec->GetStationNo();
	int stop_extra_time = m_pBasicPara->CalculateTrainStopExtra(station,prec->GetEntry(),TRAIN_INTO_STATION,pnewtrain->GetRunType());
	int start_extra_time =m_pBasicPara->CalculateTrainStartExtra(station,prec->GetExit(),TRAIN_OUT_STATION,pnewtrain->GetRunType());
	if(!GetTGFunctionConfig()->IsEnablePasstoStayModifyArivalTime())
	{
		stop_extra_time=0;
	}
	int offset=0;
	if (stop_to_pass)   // stop -> pass
	{ 
		offset = -1 * (stop_extra_time + start_extra_time+prec->m_data.depart-prec->m_data.arrive);
		prec->m_data.arrive -= stop_extra_time;
		prec->m_data.depart=prec->m_data.arrive;

	}
	else // pass ->stop
	{						
		//offset=300;
		TIME prev_depart_time=prec->m_data.depart;

		prec->m_data.arrive += stop_extra_time;
		if(m_nAdjustSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			prec->m_data.depart += stop_extra_time+120;
		else
			prec->m_data.depart += stop_extra_time+300; // Ĭ��ͣ5����
		TIME depart_time=CacuTrainDepartTime(pnewtrain,index);
		prec->m_data.depart=depart_time;
		offset = prec->m_data.depart-prev_depart_time+start_extra_time;
		long t = prec->m_data.min_stop_time - (prec->m_data.depart - prec->m_data.arrive);
		if (prec->m_data.depart + t > prec->m_data.arrive)
		{
			prec->m_data.depart += t;
			offset += t;
		}
	}

	if(TrainHasActualTimeRecord(pnewtrain,index)) // added by wx 0718
		return TRUE;

	ChangeSideForStopOrPass(pnewtrain,index);
	if(m_nAdjustSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return TRUE; // ����ͼ�����޸ĺ�����ʱ��
	for(int i=index+1;i<rec_count;i++)
	{
		prec=pnewtrain->GetTrainRecord(i);
		if(!prec)
			continue; 
		prec->m_data.arrive+=offset;
		prec->m_data.depart+=offset;
	}
	return TRUE;
}

const char *CTGAutoAdjustLogic::CheckTrainManualUpdateOper(CTG_SingleTrain *pnewtrain,const CMMIOperFlag &oper_flag,DWORD& status)
{
	static char reason[2048];
	if(oper_flag.nFlag==CMMIOperFlag::MUTI_REC_STAY_TO_PASS)
	{
		;
	}//ͨ���䵽����������ͨ�����߼�ȫ������ocxʵ��
	else if((oper_flag.nFlag&3)==CMMIOperFlag::STAY_TO_PASS)
	{
		CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(oper_flag.m_nRecIndex);
		if(prec)
		{
			if(TG_IsMustStop(prec->m_data))
			{
				sprintf(reason,"�հ�ƻ��涨����ͣ��,���ܰ���ͨ��,��ȷ��Ҫͨ����?");
				status=112;
				return reason;
			}

			if(TG_IsMustNoPass(prec->m_data))
			{
				sprintf(reason,"�ƻ��涨��ֹͨ��,���ܰ���ͨ��,��ȷ��Ҫͨ����?");
				status=112;
				return reason;
			}

			if(prec->m_data.min_stop_time>0)
			{
				int minute=prec->m_data.min_stop_time/60;
				CString strStationName=gpcdef.GetStationName(prec->m_data.station);
				if(GetTGFunctionConfig()->IsGoodTrainNotTechStopNeedPwd() && !pnewtrain->IsPassengerTrain())
				{
					sprintf(reason,"%s �հ�ƻ�Ҫ����%d���� Ӫҵʱ��,���ܰ���ͨ��,��ȷʵҪ����ͨ����?", strStationName, minute);
					status=112;
					return reason;
				}
				else
				{
					sprintf(reason,"%s �հ�ƻ�Ҫ����%d���� Ӫҵʱ��,���ܰ���ͨ��,��ȷʵҪ����ͨ����?", strStationName, minute);
					status=112;
					return reason;
				}
			}

			const char *pshuaigua=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
			if(pshuaigua && strlen(pshuaigua)>0)
			{
				CString strStationName=gpcdef.GetStationName(prec->m_data.station);
				sprintf(reason,"%s �г�վ��ҵ:%s,��ȷʵҪ����ͨ����?",strStationName,pshuaigua);
				status=112;
				return reason;
			}			
		}
	}
	else if((oper_flag.nFlag&3)==CMMIOperFlag::PASS_TO_STAY)
	{
		CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(oper_flag.m_nRecIndex);
		if(prec)
		{
			if(TG_IsMustNotStop(prec->m_data))
			{
				sprintf(reason,"�հ�ƻ��涨��ֹͣ��,��ȷ��Ҫ������?");
				status=112;
				return reason;
			}
		}
	}
	else
	{
		if(!GetTGFunctionConfig()->IfCheckManualOper() || !GetTGFunctionConfig()->NeedToCheckTGOper(pnewtrain->GetTrainType()))  // 290090913����������·���г����ж�
		{
			return NULL;
		}
		if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)  // �Զ����õ���ʱ�����ж����³�ͻ
			return NULL;
		int i=0;
		int rec_count=pnewtrain->GetTrainRecordCount();
		CTG_TrainRecord *prec;
		for(i=rec_count-1;i>=0;i--)  //�ҵ����һ����ʵ�ʵ�ļ�¼
		{
			prec=pnewtrain->GetTrainRecord(i);
			if(!prec)
				continue;
			if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
				break;
		}

		
		if(i<0) // no need to do this
			i=0;
		
		if(i < oper_flag.m_nRecIndex)
			i = oper_flag.m_nRecIndex;
		
		for(;i<=rec_count-1;i++)
		{
			TIME departtm=0;
			CString err_info;
			unsigned long alarm_flag=0;
			
			if(CheckTrain_ABSBlock(pnewtrain,i,alarm_flag,err_info,departtm))
			{
				if(alarm_flag & ALARM_FLAG_LINE_BLOCK_ERROR)
				{
					strcpy(reason,err_info);
					status=101;
					return reason;
				}
			}

			if(CheckTrainABSMeetInfo(pnewtrain,i,alarm_flag,err_info))
			{
				if(alarm_flag & ALARM_FLAG_MEET_ERROR)
				{
					strcpy(reason,err_info);
					status=102;
					return reason;
				}
			}

			CTG_TrainRecord *precord=pnewtrain->GetTrainRecordNoDelete(i);
			if(precord)
			{
				precord->ClearStplanWarnInfo();
				CheckTrainSideInfoForStplan(pnewtrain,i,true);
				if(precord->HaveWarnInfo())
				{
					GpcLogTrainMessage(0, 0,"�˹��޸Ĳ�������:");

					CString strText;
					WORD id=0;
					CString text;
                    for(int index=0; index<1024; index++)
					{
						if(precord->GetStplanWarnInfo(index, id, text))
						{
							GpcLogTrainMessage(0, 0,"%s", text);
							strText += text;
							strText += "\n";
						}
						else
						{
							break;
						}
					}
					precord->ClearStplanWarnInfo();

					strncpy(reason,strText,sizeof(reason)-1);  
					reason[sizeof(reason)-1]=0;
					status=103;
					return reason;
				}
			}
		}
	}
	status=0;
	return NULL;
}

BOOL CTGAutoAdjustLogic::AdjustTrainForManualUpdate(CTG_SingleTrain *pnewtrain,const CMMIOperFlag &oper_flag,int con_id,CWnd* pParentWnd)
{
	int rec_index = oper_flag.m_nRecIndex;
	CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(rec_index);
	if (NULL == rec)
		return FALSE;

	CTG_TrainRecord old = *rec;

	if(oper_flag.nFlag & CMMIOperFlag::MUTI_REC_STAY_TO_PASS)
	{
		for(int i=oper_flag.m_nRecIndex; i<pnewtrain->GetTrainRecordCount(); i++)
		{
			CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(i);
			if (NULL == rec)
				continue;
			if(rec->IsPassRec())
				continue;
			// ����ҵʱ��
			if(rec->m_data.min_stop_time>0)
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ��С��ҵʱ��Ϊ%d, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()), rec->m_data.min_stop_time);
				continue;
			}
			if(TG_IsMustStop(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ����Ϊ����ͣ��, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			if(TG_IsMustNoPass(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ����Ϊ��ֹͨ��, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			// ʵ�ʵ�
			if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
				continue;
			if (TG_IsArriveRouteDoing(rec->m_data) || 
				TG_IsArriveRouteOk(rec->m_data) || 
				TG_IsArriveRouteOccupy(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �ӳ���·�����ų��������ų�����ռ��, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}
			if (TG_IsDepartRouteDoing(rec->m_data) || 
				TG_IsDepartRouteOk(rec->m_data) || 
				TG_IsDepartRouteOccupy(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ������·�����ų��������ų�����ռ��, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}

			if (TG_IsTrainIntoSide(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �Ѿ���վ, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}
			
			// �Ǽ���ͣ��
			if( !TG_IsTechStopTrain(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �п�����ҵ, ���ܱ�Ϊͨ��",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			
			if(GetTGFunctionConfig()->IsNotThisSectionStation(con_id,  rec->m_data.station))
			{
				GpcLogTrainMessage(0, 0, "��ע��, %s �����ڱ�̨��Ͻ,�����޸ĵ�����ʽ", gpcdef.GetStationName(rec->m_data.station));
				continue;
			}
			
			AdjustForStopPassChange(con_id, pnewtrain, i, TRUE);
		}
	}
	else if(oper_flag.nFlag & CMMIOperFlag::STAY_TO_PASS)
	{
		AdjustForStopPassChange(con_id, pnewtrain,oper_flag.m_nRecIndex,TRUE);
	}
	else if(oper_flag.nFlag & CMMIOperFlag::PASS_TO_STAY)
	{
		AdjustForStopPassChange(con_id, pnewtrain,oper_flag.m_nRecIndex,FALSE);
	}
	
	SetTrainPassNonMainSide(pnewtrain);
	//SetDefualtEntryAndExit(pnewtrain);  // ����-1��entry exist����
	
	// ���ñ�Ǯ��
	this->SetTrainFJK(pnewtrain);

	if(oper_flag.nFlag & CMMIOperFlag::STAY_TO_PASS
		|| oper_flag.nFlag & CMMIOperFlag::PASS_TO_STAY)
	{	
		// 20150121������Ҫ����� ͨ���䵽���޸Ĺɵ�ʱ ��ʻ��ǵ������εĵ����������б���
		if (TG_IsElecLocoFlag(rec->m_data))
		{
			bool bCheck=false;
			int nFlag=GetTGFunctionConfig()->GetCheckElecSideWhenPassToStopFlag();
			if(nFlag==2) // ����TDCS\CTC�����
			{
				bCheck=true;
			}
			else if(nFlag==1) // ��CTC��վ���
			{
				int station=rec->GetStationNo();
				if(gpcdef.IsCtcStation(station))
				{
					bCheck=CWorkScheduleHolder::IsNeedCheckCtcModePlan(station);
				}
				else
				{
					bCheck=false;
				}
			}
			else
			{
				bCheck=false;
			}

			if(bCheck)
			{
				const CStationDef *pStationDef=m_pBasicPara->GetStationDef(rec->GetStationNo());
				if(pStationDef==NULL)
					return FALSE;

				if (!pStationDef->isCatenary(rec->m_data.arrive_side))
				{
					SIDE_NAME arrive_side_name;
					memset(arrive_side_name, 0, sizeof(arrive_side_name));
					pStationDef->GetSideName(rec->m_data.arrive_side, arrive_side_name);
					
					CString str_arrive_side_name=arrive_side_name;
					if(str_arrive_side_name!="")
					{
						CString strText;
						strText.Format("�����˹���������, ���� %s �� %s �����޵����ɵ� %s", 
							pnewtrain->GetTrainID(), 
							gpcdef.GetStationName(rec->GetStationNo()), 
							arrive_side_name);
						
						if(pParentWnd)
						{
							pParentWnd->MessageBox("��ע��:\n"+strText, "������ʾ", MB_OK|MB_ICONHAND);
						}
						strText = "����("+strText+")";
						GpcLogTrainMessage(0,0,strText.GetBuffer());
					}
				}
				
				if ((rec->m_data.depart_side!=rec->m_data.arrive_side) && !pStationDef->isCatenary(rec->m_data.depart_side))
				{
					SIDE_NAME depart_side_name;
					memset(depart_side_name, 0, sizeof(depart_side_name));
					pStationDef->GetSideName(rec->m_data.depart_side, depart_side_name);
					
					CString str_depart_side_name=depart_side_name;
					if(str_depart_side_name!="")
					{
						CString strText;
						strText.Format("�����˹���������, ���� %s �� %s �޵����ɵ� %s ����", 
							pnewtrain->GetTrainID(), 
							gpcdef.GetStationName(rec->GetStationNo()), 
							depart_side_name);

						if(pParentWnd)
						{
							pParentWnd->MessageBox("��ע��:\n"+strText,"������ʾ", MB_OK|MB_ICONHAND);
						}
						
						strText = "����("+strText+")";
						GpcLogTrainMessage(0,0,strText.GetBuffer());
					}
				}
			}
		}

		GpcLogTrainMessage(0, 0, "�ɵ����train %s %u sta %d �ɵ�:%d %d -> %d %d, ����:%d ���:%d",
			pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), rec->GetStationNo(), 
			old.m_data.arrive_side, old.m_data.depart_side, 
			rec->m_data.arrive_side, rec->m_data.depart_side,
			rec->m_data.exit, rec->m_data.entry);
	}
	else
	{
		// �˹�����--���¼���Ƿ��ɵ�  20131204 Ų�ߵ����Ƿ��ɵ� ������
		//for(int i=0; i<pnewtrain->GetTrainRecordCount(); i++)
		//{
		//	CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(i);
		//	if (NULL == rec)
		//		continue;

		//	if(!IS_VALID_SIDE(rec->m_data.depart_side) || !IS_VALID_SIDE(rec->m_data.arrive_side))
		//	{
		//		ChangeSideForStopOrPass(pnewtrain, i);
		//	}
		//}
	}

	return TRUE;
}

//////////// �߼���ͻ��� /////////////
void  CTGAutoAdjustLogic::CheckAndSetAlarmInfo(CTG_SingleTrain *single)
{
	if(!GetTGFunctionConfig()->IfCheckAndSetAlarm())
	{
		return;
	}
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group,NULL_STATION,start_time,end_time);

	CTG_ObjectPtrGroup markgroup;
	m_pWorkSchedule->FindMarks(markgroup,start_time,end_time);

	CheckAndSetAlarmInfo(single, start_time, end_time, group, markgroup);
}

void  CTGAutoAdjustLogic::CheckAndSetAlarmInfo(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup)
{
	unsigned long alarm_flag=0;
	CString  strText;
	int alarm_count=0;
	for(int rec_index=0;rec_index<single->GetTrainRecordCount();rec_index++)
	{
		CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
		if(!precord) continue;
		do
		{
			alarm_flag=0;
			strText="";
			
			precord->ClearStplanWarnInfo();
			CheckTrainPassAndStop(single, rec_index);
			if(precord->HaveWarnInfo())
			{
				WORD id;
				CString text;
				if(precord->GetStplanWarnInfo(0, id, text))
				{
					strText=text;
					break;
				}
			}
			
			// ����г���ʩ���������ͻ
			TIME etime=0;
			if(CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup))
				break;

			// ����г������ͻ
			if(CheckTrainABSMeetInfo(single,rec_index,alarm_flag,strText,start_time,end_time,group))
			    break;
			
			// ����г�վ�ڳ�ͻ
			if(CheckTrainStaMeetInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText))
				break;

			CTG_TrainRecord *precord=single->GetTrainRecordNoDelete(rec_index);
			if(precord)
			{
				precord->ClearStplanWarnInfo();
				CheckTrainSideInfoForStplan(single,rec_index,true);
				if(precord->HaveWarnInfo())
				{
					WORD id;
					CString text;
					if(precord->GetStplanWarnInfo(0, id, text))
					{
						strText=text;
						break;
					}
				}
			}
		}while(0);

		precord->m_strAlarmText.clear();
		precord->ClearStplanWarnInfo();
		if(alarm_flag>0 || strText!="")
		{
			alarm_count++;
			precord->m_nAlarmStatus|=alarm_flag;
			precord->m_strAlarmText+=" ";
			precord->m_strAlarmText+=strText;
			precord->m_strAlarmText+=" ";
		}
		else
		{
			precord->m_nAlarmStatus=0;
			precord->m_strAlarmText="";
		}
	}
}

static BOOL CheckNotToMeetRecordFlag(const CTG_TrainRecord *prec,const CTG_SingleTrain *train)
{
	WORD nNotMeetFlag=(prec->m_data.can_not_meet&0xFFFF);
	if(TG_NOTMEET_CRH(prec->m_data.can_not_meet) && train->IsCRHTrain())
		return TRUE;
	if(TG_NOTMEET_PASSENGER(prec->m_data.can_not_meet) && train->IsPassengerTrain())
		return TRUE;
	if(TG_NOTMEET_FREIGHT(prec->m_data.can_not_meet))
		return TRUE;
	if(TG_NOTMEET_SEPECIAL_Z(prec->m_data.can_not_meet) && train->IsSpecialZ())
		return TRUE;
	if(TG_NOTMEET_SPACE(prec->m_data.can_not_meet) && (train->GetSpaceRank()>0))
		return TRUE;
	
	NotMeetInfo info;
	bool bFind=gpcdef.getNotMeetInfoByNoMeetFlag(nNotMeetFlag, info);
	if(bFind)
	{
		int train_type = train->GetTrainType();
		for (int n=0; n<info.nTrainTypeCount; n++)
		{
			if (info.nTrainTypeList[n]==train_type || info.nTrainTypeList[n]==-1)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CheckExtNotMeet(const CTG_SingleTrain *train1,const CTG_SingleTrain *train2)
{
		if(train1->IsCRHTrain() && train2->GetSpaceRank()>0)
			return TRUE;
		if(train2->IsCRHTrain() && train1->GetSpaceRank()>0)
			return TRUE;
        return FALSE;
}

// ע��: ���溯��û�п����г����ʱ�� 2010-11-20 cuihu
// ������ֻ�ж� train1�ķ�����train2�Ľӳ�\�����Ƿ��ͻ
static int CheckNotToMeetWithTime(const CTG_SingleTrain *train1,const CTG_SingleTrain *train2,int recindex1,int recindex2,TIME  rec_from_depart=0,TIME  rec_to_arrival=0) //������������train1��ʱ��,����ָ����record�е�ʱ��
{
	bool bCheckSpaceMeet=false;
	if(gpcdef.IsFormatView())
	{
		if (train1->GetSpaceRank() != 0)
			bCheckSpaceMeet=true;
	}
	else
	{
		if (GetTGFunctionConfig()->IsAlarmNotCheckSpackTrainMeetInfo())
		{
			bCheckSpaceMeet=false;
		}
		else
		{
			if (train1->GetSpaceRank() != 0)
				bCheckSpaceMeet=true;
		}
	}

	const CTG_TrainRecord *pthis_rec=train1->GetTrainRecordNoDelete_Const(recindex1);
	const CTG_TrainRecord *pthis_nextrec=train1->GetTrainRecordNoDelete_Const(recindex1+1);
	const CTG_TrainRecord *prela_rec=train2->GetTrainRecordNoDelete_Const(recindex2);
	const CTG_TrainRecord *prela_nextrec=NULL;
	const CTG_TrainRecord *prela_fromrec=NULL;
	if(!pthis_rec ||  !pthis_nextrec || !prela_rec)
		return 0;
	if(pthis_rec->GetStationNo()!=prela_rec->GetStationNo())
		return 0;
	ushort abs_num_in1=0,abs_num_in2=0,dir1,dir2,abs_num_out1=0,abs_num_out2=0;
	GetTGBasicParam()->ConvertStationEntryToAbsnum(pthis_rec->GetStationNo(),pthis_rec->GetExit(),&abs_num_out1,&dir1); //�����εķ�����

	GetTGBasicParam()->ConvertStationEntryToAbsnum(prela_rec->GetStationNo(),prela_rec->GetEntry(),&abs_num_in2,&dir2); // �������εĽӳ���
	GetTGBasicParam()->ConvertStationEntryToAbsnum(prela_rec->GetStationNo(),prela_rec->GetExit(),&abs_num_out2,&dir2); // �������εķ�����

	int meet_type=0;
	if((abs_num_out1==abs_num_out2) && (abs_num_out1!=0))  //bug
	{
		prela_nextrec =train2->GetTrainRecordNoDelete_Const(recindex2+1);
		prela_fromrec =prela_rec;
		meet_type=1;   // ͬ����������
	}
	else if((abs_num_out1==abs_num_in2) && (abs_num_out1!=0))  //bug
	{
		prela_nextrec=prela_rec;
		if(recindex2>0)
		{
			prela_fromrec=train2->GetTrainRecordNoDelete_Const(recindex2-1);
			meet_type=2; // ������������
		}
	}
	else
	{
		prela_nextrec=prela_rec;
		if(recindex2>0)
		{
			prela_fromrec=train2->GetTrainRecordNoDelete_Const(recindex2-1);
			if(prela_fromrec!=NULL && prela_nextrec!=NULL)
			{
				if(pthis_rec->GetStationNo()==prela_nextrec->GetStationNo() && pthis_nextrec->GetStationNo()==prela_fromrec->GetStationNo())
				{
					if(CheckNotToMeetRecordFlag(pthis_rec,train2)|| CheckNotToMeetRecordFlag(prela_fromrec,train1) || CheckExtNotMeet(train1,train2))	
						meet_type=3; // ���佻��
					else if(bCheckSpaceMeet)
						meet_type=3; // ���佻��
				}
			}
		}
	}

	int nResult=0;
	if((meet_type!=0) && (prela_nextrec!=NULL) && (prela_fromrec!=NULL))
	{
		if(!rec_from_depart)
			rec_from_depart= pthis_rec->GetDepartTime(); 
		if(!rec_to_arrival)
			rec_to_arrival = pthis_nextrec->GetArriveTime(); 
		
		// ����
		if ((meet_type==2 || meet_type==3) && prela_nextrec->GetArriveTime() >= rec_from_depart	&& rec_to_arrival >= prela_fromrec->GetDepartTime())
		{
			nResult = 1;
			if(bCheckSpaceMeet)
				nResult = 2;
		}
		// ͬ��
		if (meet_type==1 && (prela_nextrec->GetArriveTime() <= rec_to_arrival && prela_fromrec->GetDepartTime() >=rec_from_depart
			|| prela_nextrec->GetArriveTime() >= rec_to_arrival && prela_fromrec->GetDepartTime() <=rec_from_depart))  // ԭ��ȱ��һ�����
		{
			nResult = 1;
			if(bCheckSpaceMeet)
				nResult = 2;
		}
	}
	return nResult;	
}

BOOL  CTGAutoAdjustLogic::CheckTrainABSMeetInfo(CTG_SingleTrain *single, int rec_index, ULONG &alarm_flag, CString &strText)
{
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group,NULL_STATION,start_time,end_time);

	return CheckTrainABSMeetInfo(single, rec_index, alarm_flag, strText, start_time, end_time, group);
}

BOOL CTGAutoAdjustLogic::CheckTrainABSMeetInfo(CTG_SingleTrain *single,int rec_index,ULONG &alarm_flag,CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	alarm_flag=0;
	CTG_TrainRecord *precord_from=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *precord_to=single->GetTrainRecord(rec_index+1);

	if(!precord_from || !precord_to)
		return FALSE;
	TIME  rec_from_depart = precord_from->GetDepartTime(); 
	TIME  rec_to_arrival = precord_to->GetArriveTime(); 

	if(end_time<rec_from_depart || rec_to_arrival<start_time)
		return FALSE;

	const char* strDepartStaName = gpcdef.GetStationName(precord_from->GetStationNo());
	const char* strArrivalStaName = gpcdef.GetStationName(precord_to->GetStationNo());
	if(!strDepartStaName || !strArrivalStaName)
		return FALSE;
		
	const CTG_TrainRecord *rec=NULL;
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		for(int i=0;i<pTrain->GetTrainRecordCount();i++)  // ������ 2011-1-17, ��Ҫ-1
		{
			// �жϽ������������乲�ó�ͻ
			if (CheckNotToMeetWithTime(single,pTrain,rec_index,i))
			{
					strText.Format("�г�%s��%s��%s��%s�����ͻ", single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
					alarm_flag=ALARM_FLAG_MEET_ERROR;
					break;
			}
		}
		if(alarm_flag>0)
			break;
	}

	if(alarm_flag>0)
		return TRUE;
	else
		return FALSE;
	return TRUE;
}

BOOL  CTGAutoAdjustLogic::CheckTrainStaMeetInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& trains_in_range, unsigned long &alarm_flag,CString &strText)
{
	alarm_flag=0;
	strText="";

	if(!gpcdef.IsFormatView())
	{
		if (GetTGFunctionConfig()->IsAlarmNotCheckSpackTrainMeetInfo())
			return FALSE;
	}
		
	if (single->GetSpaceRank() == 0)
		return FALSE;

	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return FALSE;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return FALSE;

	TIME  rec_depart = precord->GetDepartTime();
	TIME rec_arrival = precord->GetArriveTime();

	if(start_time>rec_depart || rec_arrival>end_time)
		return FALSE;

	int j=0;
	for(j=0; j<trains_in_range.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = trains_in_range.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;
		if ( pTrain->GetSpaceRank() == 0 )
			continue;
		CTG_TrainRecord *thisrec = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(!thisrec)
			continue;

		TIME this_depart =  thisrec->GetDepartTime();
		TIME this_arrival = thisrec->GetArriveTime();
		if (thisrec->IsStartRec())
			this_arrival = this_depart;
		else if (thisrec->IsTerminalRec() )
			this_depart = this_arrival;

		if ( this_depart >= rec_arrival && rec_depart >= this_arrival )
		{
			if (GetTGBasicParam()->EnableStaSideLimitCfg())
			{
				if (GetTGBasicParam()->IsTwoStationSideLimit(precord->GetStationNo(), precord->m_data.arrive_side, thisrec->m_data.arrive_side))
				{
					strText.Format("�����г�%s���볬���г�%s����%sվ���벻�ܽ���ɵ�", 
								single->GetTrainID(), pTrain->GetTrainID(), strStaName);
					alarm_flag=ALARM_FLAG_MEET_ERROR;
					break;
				}
			}
			else
			{
				strText.Format("�����г�%s�κͳ����г�%s����%sվ���벻�ܽ���ɵ�", 
								single->GetTrainID(), pTrain->GetTrainID(), strStaName); // stplan warn 20161019
				alarm_flag=ALARM_FLAG_MEET_ERROR;
				break;
			}
		}
	}

	if(strText!="")
	{
		precord->AddStplanWarnInfo(1, strText);
		return TRUE;
	}
	else
		return FALSE;
}

//added by zhaoliang for same side check
//Ŀǰֻ��⵽���ɵ�һ�µ����
BOOL  CTGAutoAdjustLogic::CheckTrainStaSideInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& trains_in_range, unsigned long &alarm_flag,CString &strText)
{
	alarm_flag=0;
	strText="";

	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return FALSE;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return FALSE;

	if ( precord->IsStartRec() || precord->IsTerminalRec() )
		return FALSE;

	TIME rec_depart = precord->GetDepartTime();
	TIME rec_arrival = precord->GetArriveTime();
	BYTE rec_side_depart  = precord->m_data.depart_side;
    BYTE rec_side_arrival = precord->m_data.arrive_side;

	rec_arrival=rec_arrival-rec_arrival%60; // ȥ����
	rec_depart=rec_depart-rec_depart%60; // ȥ����

	if(end_time<rec_depart || rec_arrival<start_time)
		return FALSE;
		
	int j;
	const CTG_TrainRecord *rec=NULL;
	for(j=0; j<trains_in_range.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = trains_in_range.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;
		CTG_TrainRecord *thisrec = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(!thisrec)
			continue;

		TIME this_depart =  thisrec->GetDepartTime();
		TIME this_arrival = thisrec->GetArriveTime();
		BYTE this_side_depart  = thisrec->m_data.depart_side;
		BYTE this_side_arrival = thisrec->m_data.arrive_side;

		this_arrival=this_arrival-this_arrival%60; // ȥ����
		this_depart=this_depart-this_depart%60; // ȥ����

		if (thisrec->IsStartRec())
			this_arrival = this_depart;
		else if (thisrec->IsTerminalRec() )
			this_depart = this_arrival;

		if ( this_depart >= rec_arrival && rec_depart >= this_arrival )
		{
			if ((rec_side_depart==rec_side_arrival)&&(this_side_depart==this_side_arrival)&&(rec_side_depart==this_side_depart))
			{
				strText.Format("%s����%s����%sվ��ͬ�ɵ�����", single->GetTrainID(), pTrain->GetTrainID(), strStaName );
				alarm_flag=ALARM_FLAG_MEET_ERROR;
				
				break;
			}
		}
	}
	if(alarm_flag>0)
		return TRUE;
	else
		return FALSE;
}
////////// ʩ��������������

BOOL CTGAutoAdjustLogic::CheckTrain_SideBlock(STATION_NO  station,SIDE_ID side_id,TIME &etime,TIME &stime)
{
	TIME reg_time=etime;
	CTG_ObjectPtrGroup group;
	m_pWorkSchedule->FindMarks(group);
	for(int i=0;i<group.GetCount();i++)
	{
		const CTG_SingleMark *iter_mark=(const CTG_SingleMark *)group.GetPtr(i);
		const TG_ScheduleBlock &mark = iter_mark->m_data;
		if (TG_SIDE_INHIBIT != mark.type && TG_SIDE_TIANCHUANG != mark.type && TG_SIDE_OCCUPY != mark.type)
		{
			continue;
		}

		if (mark.station != station)
		{
			continue;
		}

		if (stime > 0)
		{
			if (stime < mark.start_time || mark.end_time < etime)
			{
				continue;
			}
		}
		else 
		{
			if (mark.start_time > etime)
			{
				continue;
			}
		}
		if(GetTGFunctionConfig()->IsStationBlockNotCheckSide()) //��������ɵ� 20091011 XIAN
		{ 
			if(reg_time< mark.end_time)
				reg_time = mark.end_time;	
		}
		else
			for(int j=0; j<mark.ele_count; j++)
			{
				SIDE_ID tmpside = (SIDE_ID) mark.ele_number[j];
				if(tmpside==side_id)
				{
					if (m_pBasicPara->VerifyStationSide(station, tmpside))
					{
						if (reg_time< mark.end_time)
							reg_time = mark.end_time;	
					}
					break;
				}
			}

	}
	if(reg_time!=etime)  
	{	
		etime=reg_time;
		return TRUE;
	}
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime)
{
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_ObjectPtrGroup markgroup;
	m_pWorkSchedule->FindMarks(markgroup,start_time,end_time);
	
	return CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup);
}

BOOL  CTGAutoAdjustLogic::CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime, CTG_ObjectPtrGroup& markgroup)
{
	TIME reg_time;

	int absnum=-1;
	int absdir=-1;
	if(rec_index<0 || rec_index>=(single->GetTrainRecordCount()-1))
		return FALSE;
	CTG_TrainRecord *prec=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *pnextrec=single->GetTrainRecord(rec_index+1);
	if(!prec || !pnextrec)
		return FALSE;
	int abs_run_time=pnextrec->GetArriveTime()-prec->GetDepartTime();
	alarm_flag=0;
	STATION_NO  station1,station2;
	ENTRY_ID exit_id,entry_id;
	station1=prec->GetStationNo();
	station2=pnextrec->GetStationNo();
	exit_id=prec->GetExit();
	entry_id=pnextrec->GetEntry();
	if(!m_pBasicPara->GetAbsNameBetweenStations(station1,station2,exit_id,entry_id ,&absnum,&absdir))
		return FALSE;
	if(etime==0)
		etime=prec->GetDepartTime();
	reg_time=etime;
	//CTG_ObjectPtrGroup group;
	//m_pWorkSchedule->FindMarks(group);
	for(int i=0;i<markgroup.GetCount();i++)
	{
		const CTG_SingleMark *iter_mark=(const CTG_SingleMark *)markgroup.GetPtr(i);
		const CTG_SingleMark  &mark = *iter_mark;
		if ( mark.GetMarkType()==TG_ABS_INHIBIT ||
			mark.GetMarkType()==TG_ABS_FIX_MARK  ||
			(mark.GetMarkType()==TG_SECTION_ELEC_FIX && single->IsElectricTrain())|| 
			(mark.GetMarkType()==TG_SECTION_ELEC_FIX_LANZHOU && single->IsElectricTrain()))
		{
			if(mark.MatchAbsNumber(absnum))
			{
				if (reg_time>= mark.GetStartTime() && reg_time<=mark.GetEndTime())
					reg_time = mark.GetEndTime();
				if ((reg_time+abs_run_time)>= mark.GetStartTime() && (reg_time+abs_run_time)<=mark.GetEndTime())
					reg_time = mark.GetEndTime();
			}

		}

	}
	if(reg_time!=etime)  
	{	
		etime=reg_time;
		alarm_flag=ALARM_FLAG_LINE_BLOCK_ERROR;
		strText.Format("%s��ͨ��������ͣ�硢���޻���ͣ����·!",single->GetTrainID());
		return TRUE;
	}
	return FALSE;
}

void  CTGAutoAdjustLogic::SetTrainPassNonMainSide(CTG_SingleTrain *ptrain)
{
	if(NULL==ptrain)
		return;

	int rec_count=ptrain->GetTrainRecordCount(); 
	for(int i=0;i<rec_count;i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		
		TG_ClearFlagPassNonMainSide(prec->m_data.flag);
		if(!prec->IsTerminalRec() && !prec->IsStartRec() && prec->IsPassRec())
		{
			SIDE_ID main_side = m_pBasicPara->GetMainSide(prec->m_data);
			SIDE_ID side = prec->m_data.arrive_side;
			if(main_side!=side)  //�ص���� 
				TG_SetFlagPassNonMainSide(prec->m_data.flag);
		}
	}

	return;
}

void  CTGAutoAdjustLogic::CheckTrainPassAndStop(CTG_SingleTrain *single, int rec_index)
{
	if(single==NULL)
		return;

	CTG_TrainRecord *prec=single->GetTrainRecordNoDelete(rec_index);
	if(prec==NULL)
		return;
	
	if(rec_index==0 && prec->IsStartRec())
		return;

	if(rec_index+1==single->GetTrainRecordCount() && prec->IsTerminalRec())
		return;

	//��鳵վ��ͣ,��ͣ�ͽ�ֹͨ��
	if(TG_IsMustNotStop(prec->m_data))  // "��ֹͣ��";
	{
		if(!prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s�ν�ֹ��%sվͣ��",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(5, strText);
		}
	}
	if(TG_IsMustStop(prec->m_data) || prec->m_data.min_stop_time>0) // "����ͣ��";
	{
		if(prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s�α�����%sվͣ��",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(6, strText);
		}
	}
	if(TG_IsMustNoPass(prec->m_data))  // "��ֹͨ��";
	{
		if(prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s�ν�ֹ��%sվͨ��",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(7, strText); // ������������
		}
	}
}

void CTGAutoAdjustLogic::CacuTrainSlowTime(CTG_SingleTrain *single)
{
	CaculateTrainSlowTime(single);
}

void  CTGAutoAdjustLogic::GetScheduleInfo(CTG_ScheduleInfo& schd)
{
	m_pWorkSchedule->GetScheduleInfo(schd);
}

// ���ñ�Ǯ��
void  CTGAutoAdjustLogic::SetTrainFJK(CTG_SingleTrain *ptrain)
{
	int flag = 0;
	int rec_count=ptrain->GetTrainRecordCount(); 
	for(int i=rec_count-2;i>=0;i--)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		if (TG_IsFJKStationFlag(prec->m_data))
		{
			int method = GetTGFunctionConfig()->GetFjkStatisticMethod(prec->m_data.station);
			ptrain->SetFJKTime(*prec, method);
			flag = 1;	
		}
	}
	if (!flag)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
		if (prec->IsStartRec())
		{
			int method = GetTGFunctionConfig()->GetFjkStatisticMethod(prec->m_data.station);
			ptrain->SetFJKTime(*prec, method);
		}
	}
}
void  CTGAutoAdjustLogic::CheckTrainABSMeetInfoForStplan(CTG_SingleTrain *single,int rec_index,ULONG &alarm_flag,CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	if(rec_index<0 || rec_index>=(single->GetTrainRecordCount()-1))
		return;

	CTG_TrainRecord *precord_from=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *precord_to=single->GetTrainRecord(rec_index+1);

	if(!precord_from || !precord_to)
		return;
	TIME  rec_from_depart = precord_from->GetDepartTime(); 
	TIME  rec_to_arrival = precord_to->GetArriveTime(); 

	if(end_time<rec_from_depart || rec_to_arrival<start_time)
		return;

	const char* strDepartStaName = gpcdef.GetStationName(precord_from->GetStationNo());
	const char* strArrivalStaName = gpcdef.GetStationName(precord_to->GetStationNo());
	if(!strDepartStaName || !strArrivalStaName)
		return;
			
	const CTG_TrainRecord *rec=NULL;
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		for(int i=0;i<pTrain->GetTrainRecordCount();i++)  // ������ 2011-1-17, ��Ҫ-1
		{
			// �жϽ������������乲�ó�ͻ
			int nResult = CheckNotToMeetWithTime(single,pTrain,rec_index,i);
			if (nResult==1)
			{
				strText.Format("%s����%s����%sվ��%sվ�����ͻ", single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
				precord_from->AddStplanWarnInfo(8, strText);
			}
			else if (nResult==2)
			{
				strText.Format("�����г�%s����%s����%sվ(��%sվ��)�����Ͻ�������", 
						single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
				
				precord_from->AddStplanWarnInfo(36, strText);
			}
		}
		if(strText != "")
			break;
	}

	return;
}

void  CTGAutoAdjustLogic::CheckTrainSideCrashForStplan(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	CString strText;
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	
	if(!precord)
		return;

	TIME  arrival = precord->GetArriveTime();
	TIME  depart = precord->GetDepartTime(); 

	if(end_time<arrival || depart<start_time)
		return;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return;

	if(!IS_VALID_SIDE(precord->m_data.arrive_side)) // δ����ͻ,ֻ���ӳ��ɵ�
		return;
		
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(pTrain==NULL)
			continue;
		if(pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		CTG_TrainRecord *pobj = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(pobj)
		{
			TIME  obj_arrival = pobj->GetArriveTime();
			TIME  obj_depart = pobj->GetDepartTime(); 

			if(end_time<obj_arrival || obj_depart<start_time)
				continue;

			if(!IS_VALID_SIDE(pobj->m_data.arrive_side))
				continue;

			if(precord->m_data.arrive_side != pobj->m_data.arrive_side)
				continue;

			if(obj_depart >= arrival && depart>=obj_arrival)
			{
				strText.Format("%s�κ�%s����%sվ�ɵ���ͻ", single->GetTrainID(), pTrain->GetTrainID(), strStaName);
				precord->AddStplanWarnInfo(10, strText);
			}
		}
		if(strText != "")
			break;
	}

	return;
}

BOOL  CTGAutoAdjustLogic::CheckStplan(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup, int rec_index)
{
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(NULL == precord) 
		return FALSE;

	precord->ClearStplanWarnInfo();

	ULONG alarm_flag=0;
	CString strText="";
	
	//��鳵վ��ͣ,��ͣ�ͽ�ֹͨ��
	CheckTrainPassAndStop(single, rec_index);
	
	// ����г���ʩ���������ͻ
	TIME etime=0;
	if(CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup))
	{
		if(!strText.IsEmpty())
		{
			precord->AddStplanWarnInfo(4, strText);
		}
	}
	
	// ����г������ͻ
	CheckTrainABSMeetInfoForStplan(single,rec_index,alarm_flag,strText,start_time,end_time,group);
	// ��鳬���г�վ�����ڹɵ����ܽӳ�
	CheckTrainStaMeetInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText);
		
	if(gpcdef.IsCtcStation(precord->m_data.station))  // ������TDCS��վ����Ҫ��������ƻ� 2014.05.19
	{
		if (!CWorkScheduleHolder::IsNeedCheckCtcModePlan(precord->m_data.station)) // �ǳ�վ�� �� ��վ����
		{
			if(precord->HaveWarnInfo())
				return TRUE;
			else
				return FALSE;
		}
	}
	else
	{
		if(precord->HaveWarnInfo())  // TDCS ��վ
			return TRUE;
		else
			return FALSE;
	} 
	// ��ɢ���� �� ���Ŀ���ģʽ ��Ҫ���վ��
	
	//���ɵ���ͻ
	if(CheckTrainStaSideInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText))
	{
		if(!strText.IsEmpty())
		{
			precord->AddStplanWarnInfo(2, strText);
		}
	}
	CheckTrainSideInfoForStplan(single,rec_index,false);
	CheckTrainSideCrashForStplan(single,rec_index,start_time,end_time,group);
	
	if(precord->HaveWarnInfo())
		return TRUE;
	else
		return FALSE;
}

void CTGAutoAdjustLogic::CheckTrainSideInfoForStplan(CTG_SingleTrain *single, int rec_index, bool bTdcsCheck)
{
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return;
	TG_TrainRecord *rec=&precord->m_data; 
	TG_TrainInfo   *traininfo=&single->GetTrainInfo()->m_data;  

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	const char* strTrainName = single->GetTrainID();
	// �ɵ�����
	if(!strStaName || !strTrainName)
		return;
	
	CString strText;
	TIME sideusedstarttm=precord->GetArriveTime(); 
	TIME sideuseendtm = precord->GetDepartTime();
	if(CheckTrain_SideBlock(precord->GetStationNo(),precord->m_data.arrive_side,sideusedstarttm, sideuseendtm))
	{
		strText.Format("%s����%sվ��������ɵ�", strTrainName,strStaName);
		precord->AddStplanWarnInfo(11, strText);
	}

	if(bTdcsCheck && !gpcdef.IsCtcStation(precord->m_data.station))  // ������TDCS��վ,���˹�����ʱ����Ҫ��������ƻ� 2014.05.19
	{
		return;
	}

	if(!CWorkScheduleHolder::IsCheckStationSide(precord->GetStationNo())) // TDCS��վ��������
		return;

	const CStationDef* pSideDef = m_pBasicPara->GetStationDef(rec->station);
	if(!pSideDef)
	{
		strText.Format("��ȡ %s վϸʧ��,��˶� %s �Ƿ�����վϸ", strStaName, strTrainName);
		precord->AddStplanWarnInfo(12, strText);
		return;
	}

	SIDE_NAME arrive_side_name, depart_side_name;
	memset(arrive_side_name, 0, sizeof(arrive_side_name));
	memset(depart_side_name, 0, sizeof(depart_side_name));

	bool bStart=false, bTerminal=false;
	bool bArriveSideOK=true, bDepartSideOK=true;

	if(precord->IsStartRec() && (0==rec_index)) // ʼ����
	{
		bStart=true;
	}
	if(precord->IsTerminalRec() && (single->GetTrainRecordCount()==rec_index+1)) // �յ���
	{
		bTerminal=true;
	}
	CStationDef::SIDE_T side;
	if(STA_FAILED == pSideDef->GetSideInfo(rec->arrive_side, side))
	{
		strcpy(arrive_side_name, "?G");
		bArriveSideOK=false;
		if(!bStart)
		{
			strText.Format("%s����%sվδ����ӳ��ɵ�", strTrainName, strStaName);
			precord->AddStplanWarnInfo(15, strText);
		}
	}
	else
	{
		pSideDef->GetSideName(rec->arrive_side, arrive_side_name);
	}

	if(STA_FAILED == pSideDef->GetSideInfo(rec->depart_side, side))
	{
		strcpy(depart_side_name, "?G");
		bDepartSideOK=false;
		if(!bTerminal)
		{
			strText.Format("%s����%sվδ���䷢���ɵ�", strTrainName, strStaName);
			precord->AddStplanWarnInfo(16, strText);
		}
	}
	else
	{
		pSideDef->GetSideName(rec->depart_side, depart_side_name);
	}

	if(bArriveSideOK && bDepartSideOK && !bStart && !bTerminal)
	{
		if(rec->arrive_side != rec->depart_side)// ��������ɵ��Ƿ�һ��
		{
			if(single->IsCRHTrain())
			{
				strText.Format("�ÿ��г�%s����%sվ�����ɵ���һ��", strTrainName, strStaName); 
				precord->AddStplanWarnInfo(23, strText);
			}
			else
			{
				strText.Format("�г�%s����%sվ�����ɵ���һ��", strTrainName, strStaName); 
				precord->AddStplanWarnInfo(9, strText);
			}
		}
	}

	if(bStart) // ʼ����鷢���ɵ�
	{
		if(bDepartSideOK)
		{
			// ��鳬�޳��̶��ɵ�
			if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, single->GetSpaceRank(rec_index)) )
			{
				strText.Format("�����г�%s����%sվ��ֹ��%s����", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(18, strText);
			}
			
			// ���ɵ�������
			if (single->IsElectricTrain())
			{
				if (STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
				{
					strText.Format("%s����%sվ��ֹ���޽Ӵ�����%s����", strTrainName, strStaName, depart_side_name);
					precord->AddStplanWarnInfo(20, strText);
				}
			}
			// ���ɵ��ӷ�������
			BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if (STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
			{
				strText.Format("%s����%sվ��ֹ��%s����,Υ���ͻ��г��ӷ�����", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(22, strText);
			}
			// ���ͳ�
			if ( single->IsPassengerTrain())
			{
				// ������ҵ
				if (!TG_IsTechStopTrain(*rec))
				{
					if ( !pSideDef->isPlatform(rec->depart_side) )
					{
						strText.Format("%sվ%s��վ̨,%s�β��ܰ��������ҵ", strStaName, depart_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
					{
						strText.Format("������%s����%sվ����Ǹ�վ̨�ɵ�", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}
	else if(bTerminal)
	{
		if(bArriveSideOK)
		{
			// ��鳬�޳��̶��ɵ�
			if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, single->GetSpaceRank(rec_index)) )
			{
				strText.Format("�����г�%s����%sվ��ֹ����%s", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(17, strText);
			}

			// ���ɵ�������
			if (single->IsElectricTrain())
			{
				if (STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
				{
					strText.Format("%s����%sվ��ֹ�����޽Ӵ�����%s", strTrainName, strStaName, arrive_side_name);
					precord->AddStplanWarnInfo(19, strText);
				}
			}

			// ���ɵ��ӷ�������
			BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if (STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
			{
				strText.Format("%s����%sվ��ֹ����%s,Υ���ͻ��г��ӷ�����", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(21, strText);
			}
			// ���ͳ�
			if ( single->IsPassengerTrain())
			{
				// ������ҵ
				if (!TG_IsTechStopTrain(*rec))
				{
					if ( !pSideDef->isPlatform(rec->arrive_side) )
					{
						strText.Format("%sվ%s��վ̨,%s�β��ܰ��������ҵ", strStaName, arrive_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
					{
						strText.Format("������%s����%sվ����Ǹ�վ̨�ɵ�", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}
	else
	{
		// ��鳬�޳��̶��ɵ�
		if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, single->GetSpaceRank(rec_index)) )
		{
			strText.Format("�����г�%s����%sվ��ֹ����%s", strTrainName, strStaName, arrive_side_name);
			precord->AddStplanWarnInfo(17, strText);
		}
		if (bDepartSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, single->GetSpaceRank(rec_index)) )
		{
			strText.Format("�����г�%s����%sվ��ֹ��%s����", strTrainName, strStaName, depart_side_name);
			precord->AddStplanWarnInfo(18, strText);
		}

		// ���ɵ�������
		if (single->IsElectricTrain())
		{
			if (bArriveSideOK && STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
			{
				strText.Format("%s����%sվ��ֹ�����޽Ӵ�����%s", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(19, strText);
			}
			if (bDepartSideOK && STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
			{
				strText.Format("%s����%sվ��ֹ���޽Ӵ�����%s����", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(20, strText);
			}
		}

		// ���ɵ��ӷ�������
		BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
		if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
		{
			strText.Format("%s����%sվ��ֹ����%s,Υ���ͻ��г��ӷ�����", strTrainName, strStaName, arrive_side_name);
			precord->AddStplanWarnInfo(21, strText);
		}
		if (bDepartSideOK && (rec->arrive_side != rec->depart_side) && STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
		{
			strText.Format("%s����%sվ��ֹ��%s����,Υ���ͻ��г��ӷ�����", strTrainName, strStaName, depart_side_name);
			precord->AddStplanWarnInfo(22, strText);
		}

		// ���ͳ�
		if ( single->IsPassengerTrain())
		{
			// ����ͨ��
			if (bArriveSideOK && bDepartSideOK && GetTGBasicParam()->IsTrainPassNotUseMainSide(precord->m_data))
			{
				strText.Format("%s����%sվ%s����ͨ��", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(24, strText);
			}
			// ������ҵ
			if (!precord->IsPassRec() && !TG_IsTechStopTrain(*rec))
			{
				if (bArriveSideOK)
				{
					if (!pSideDef->isPlatform(rec->arrive_side) )
					{
						strText.Format("%sվ%s��վ̨,%s�β��ܰ��������ҵ", strStaName, arrive_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
					{
						strText.Format("������%s����%sվ����Ǹ�վ̨�ɵ�", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
				if (bDepartSideOK && (rec->arrive_side != rec->depart_side))
				{
					if (!pSideDef->isPlatform(rec->depart_side) )
					{
						strText.Format("%s %s ��վ̨,�ͳ� %s ���ܰ��������ҵ", strStaName, depart_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
					{
						strText.Format("������%s����%sվ����Ǹ�վ̨�ɵ�", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}

	// ���ɵ��ͳ�����Ƿ���ͨ
	if(!bStart)
	{
		CStationDef::ENTRY_T entry;
		if (STA_FAILED == pSideDef->GetEntryInfo(rec->entry, entry))
		{
			strText.Format("%s����%sվ����߱����",strTrainName,strStaName); // stplan warn 20161019
			precord->AddStplanWarnInfo(13, strText);
		}
		else
		{
			if (bArriveSideOK && STA_FAILED==pSideDef->isEntrySideConnect(rec->arrive_side, rec->entry))
			{
				strText.Format("%s����%sվ���ܾ���%d�ڽ���%s",strTrainName, strStaName, rec->entry, arrive_side_name);
				precord->AddStplanWarnInfo(27, strText);
			}
		}
	}

	if(!bTerminal)
	{
		CStationDef::ENTRY_T entry;
		if (STA_FAILED == pSideDef->GetEntryInfo(rec->exit, entry))
		{
			strText.Format("%s����%sվ�����߱����",strTrainName,strStaName); // stplan warn 20161019
			precord->AddStplanWarnInfo(14, strText);
		}
		else
		{
			if (bDepartSideOK && STA_FAILED == pSideDef->isEntrySideConnect(rec->depart_side, rec->exit))
			{
				strText.Format("%s����%sվ���ܾ���%s����%d��",strTrainName, strStaName, depart_side_name, rec->exit);
				precord->AddStplanWarnInfo(28, strText);
			}
		}
	}

	CTG_TrainRecord *pnextrec=single->GetTrainRecordNoDelete(rec_index+1);
	if(NULL != pnextrec)
	{
		ushort absnum1=1,absnum2=2;
		ushort dir1=0xffff,dir2=0xffff;

		if(m_pBasicPara->ConvertStationEntryToAbsnum(precord->GetStationNo(),precord->GetExit(),&absnum1,&dir1)
			&& m_pBasicPara->ConvertStationEntryToAbsnum(pnextrec->GetStationNo(),pnextrec->GetEntry(),&absnum2,&dir2))
		{
			if(absnum1!=absnum2)
			{
				STATION_NO station1=precord->GetStationNo();
				STATION_NO station2=pnextrec->GetStationNo();
				strText.Format("%s���޷���%s����%s",strTrainName, gpcdef.GetStationName(station1), gpcdef.GetStationName(station2)); // stplan warn 20161019
				precord->AddStplanWarnInfo(3, strText);
			}
		}
	}

	return;
}

// �������������Զ��������ڲ����ã��������ҳ���ָ���ڵ�ı����г�������Ŀ��ó���ʱ��
BOOL  CTGAutoAdjustLogic::__AdjustTrainDepartTimeByTrainCollision(CTG_SingleTrain *ptrain,int rec_index,int abs_run_time,CTG_SingleTrain *&initRelaTrain,TIME  &cau_depart,TIME &cau_next_arrive)
{
	TIME  begin_time,end_time;
    CTG_ObjectPtrGroup rela_trains;
	const CTG_TrainRecord *precord=ptrain->GetTrainRecordNoDelete_Const(rec_index);
	const CTG_TrainRecord *pnextrec=ptrain->GetTrainRecordNoDelete_Const(rec_index+1);
	if(!pnextrec || !precord)  
		return FALSE;
	if(abs_run_time==0)
		abs_run_time=pnextrec->m_data.arrive-precord->m_data.depart;
    if(cau_depart==0)
       cau_depart=precord->m_data.depart;  // Ĭ�Ϸ���ʱ��
	if(cau_next_arrive==0)
	   cau_next_arrive=pnextrec->m_data.arrive; // Ĭ����վ����

	begin_time=precord->GetDepartTime()-1800;
	end_time=NULL_TIME;
	m_pWorkSchedule->FindTrainsByStation(rela_trains,precord->GetStationNo(),begin_time,end_time,TRUE,TRUE);
	CTG_StationTrainStruct *pSpecRelaTrain=NULL; // ͬ������г�
		
	BOOL bHasCollision=FALSE;
	RUN_TYPE this_train_type=ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN;
	ARRIVE_DEPART_STATUS this_train_act=precord->IsPassRec()? STATION_PASS:STATION_DEPART;
	for(int i=0;i<rela_trains.GetCount();i++)
		{
			CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
           
			if(pRelaTrain->nTrainIndex == ptrain->GetTrainIndex())  // ���Ա��г�
				continue;
			if(pRelaTrain->pTrain->GetPriority()>ptrain->GetPriority()) //���Բ��ȱ��г����ȼ��ߵ��г�
				continue;

			if((pRelaTrain->pTrain->GetPriority()==ptrain->GetPriority())
				&& pRelaTrain->pTrain->IsAdjusting()) //����ͬ�����ȼ��������ڵ����е��г��������������)
				continue;
			CTG_TrainRecord *prela_record=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
			if(!prela_record)
				continue;
			CTG_TrainRecord *pnextrecRela=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex+1);

			    // �õ�׷���г����
		 
		    ARRIVE_DEPART_STATUS rela_train_act=prela_record->IsPassRec()? STATION_PASS:STATION_DEPART;
	        RUN_TYPE rela_train_type=pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN;
	      
		   	int colli=-1;

			if(pRelaTrain->nEntry==precord->GetExit())   // �������
			{
				int meet_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), MEET_TRAIN_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
                int stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(pnextrec->GetStationNo(),pnextrec->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());

				if(cau_depart>=(pRelaTrain->tArriveTime-meet_gap_time) && cau_depart<=(pRelaTrain->tArriveTime+meet_gap_time)
					|| CheckNotToMeetWithTime(ptrain,pRelaTrain->pTrain,rec_index,pRelaTrain->nRecIndex,cau_depart,cau_next_arrive+stop_extra_time+meet_gap_time)) 
				{
					 if(cau_depart<pRelaTrain->tArriveTime+meet_gap_time)
					 {
						cau_depart=pRelaTrain->tArriveTime+meet_gap_time;
						initRelaTrain=pRelaTrain->pTrain;
					}
					 if(cau_next_arrive<cau_depart+abs_run_time)
						cau_next_arrive=cau_depart+abs_run_time;
					 colli+=2;
				}
				else
					 colli=0;
			}

			if(pRelaTrain->nExit==precord->GetExit())  // ͬ�򷢳����
			{
				// �õ����õ�ͬ�򷢳����
				int depart_before_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), CONTINUE_DEPART_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
			    int depart_after_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), CONTINUE_DEPART_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
				int arrive_gap_time=0; // ��վ�ӳ����
				if(pnextrecRela && pnextrecRela->GetStationNo()==pnextrec->GetStationNo())
					arrive_gap_time=m_pBasicPara->GetTrainGapTime(pnextrec->GetStationNo(), DIFF_ARRIVE_GAP,
						pnextrecRela->IsPassRec()? STATION_PASS:STATION_DEPART,
						pnextrec->IsPassRec()? STATION_PASS:STATION_DEPART,
						pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN,
						ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN
					);

				if(cau_depart<=(prela_record->m_data.depart+depart_after_gap_time)     
					&& cau_depart>=(prela_record->m_data.depart-depart_before_gap_time)  // ͬ�򷢳��������
					|| arrive_gap_time>0 && cau_next_arrive>=(pnextrecRela->m_data.arrive-arrive_gap_time)     // ͬ��ӳ����
					&& cau_next_arrive<=(pnextrecRela->m_data.arrive+arrive_gap_time)
					||	CheckNotToMeetWithTime(ptrain,pRelaTrain->pTrain,rec_index,pRelaTrain->nRecIndex,cau_depart,cau_next_arrive)) 

				{
				     if(cau_depart<pRelaTrain->tDepartTime+depart_after_gap_time)
					 {
						cau_depart=pRelaTrain->tDepartTime+depart_after_gap_time;
						initRelaTrain=pRelaTrain->pTrain;
					 }
					 colli+=2;
					 if(cau_next_arrive<pnextrecRela->m_data.arrive+arrive_gap_time) // ͬ��ӳ����
						  cau_next_arrive=pnextrecRela->m_data.arrive+arrive_gap_time;	
				}
				else
				if(colli==-1) colli=0;
			}
			if(colli==0 && pRelaTrain->tDepartTime>precord->GetDepartTime())  // �����ȱ����г����������޳�ͻ��ֹͣ
				break;
			
			if(colli>0)
				bHasCollision=TRUE;
		}
	 return TRUE;
}

// ���min_stop_timeΪ0, ��Ĭ��ͨ��,
// �� begin_index��¼�ĳ�����ʼ����
BOOL  CTGAutoAdjustLogic::__AdjustTrainForCollision__(CTG_SingleTrain *ptrain,int begin_index) 
{
	if(ptrain==NULL)
		return false;
	if(ptrain->IsPassengerTrain())  // 2012.05.02 �����Զ������ÿ��г�
		return false;

	CTG_SingleTrain train=*ptrain;

	// 1. �����г�����ʱ��
	int rec_index=begin_index;
	CTG_SingleTrain *pRelaTrain=NULL;
	int depart_delay_for_block_abs=GetTGFunctionConfig()->GetDepartDelayTimeOfBlockedABS();
	BOOL bAdjusted=FALSE;
	while(rec_index<ptrain->GetTrainRecordCount()-1)
	{
		CTG_TrainRecord *precord=ptrain->GetTrainRecord(rec_index);
		TIME cau_depart;
		TIME cau_next_arrive;

		// �õ���һ��վ��¼���Լ�Ĭ������ʱ��
		CTG_TrainRecord *pnextrec=ptrain->GetTrainRecord(rec_index+1);
		if(!pnextrec)  
			break;
		int abs_run_time = pnextrec->m_data.arrive-precord->m_data.depart;
        	
    
	    BOOL back_trace=FALSE;

		//�˴�Ӧ�ó�ʼ��,��������ѭ��δִ��ʱ,�쳣
		cau_depart=precord->m_data.depart;
	    cau_next_arrive=pnextrec->m_data.arrive; 

		for(int step=0;step<2;step++)
		{
			if(!GetTGFunctionConfig()->GetAutoAdjustStepFlag(step))  //�������þ���
				continue;
			cau_depart=precord->m_data.depart;
		    cau_next_arrive=pnextrec->m_data.arrive; 
			if(step==CTGFunctionConfig::ADJUST_FLAGBIT_BLOCK) // ���ȿ��Ƿ����������
			{
			  CTG_ObjectPtrGroup markgroup;
			  m_pWorkSchedule->FindMarks(markgroup);
		      unsigned long alarm_flag=0; // dumy
		      CString       strText;  // dumy
		      CheckTrain_ABSBlock(ptrain,rec_index,alarm_flag,strText,cau_depart,markgroup);
			  if(cau_depart>precord->m_data.depart)
				 cau_depart+=depart_delay_for_block_abs;
			}
			else 
				continue;

			 CTG_ObjectPtrGroup meettrains;
			if(cau_depart>precord->m_data.arrive && precord->m_data.min_stop_time==0 &&
				(cau_depart-precord->m_data.arrive<300) && // ͣ��С��5����
				!m_pWorkSchedule->GetStationTrainList()->FindMeetTrains(meettrains,rec_index,ptrain)) // ͣվû�б����г�,����Ҳû����Сͣվʱ��,��ôͨ��
			 {
				precord->m_data.arrive=cau_depart;
				back_trace=TRUE;   
		 	 }
			else if(precord->IsPassRec() && cau_depart>precord->m_data.depart)  // ���ڱ��ã�����ͨ���䵽��
		     {
			    precord->m_data.arrive+=m_pBasicPara->CalculateTrainStopExtra(precord->GetStationNo(),precord->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
				back_trace=TRUE;   
			 }
			 
			int offset=cau_depart-precord->m_data.depart;
			if(offset>0)
			{
				ptrain->OffsetScheduleTime(rec_index,offset,FALSE,0);
				bAdjusted=TRUE;
			}
		}

		int offset=cau_next_arrive-pnextrec->m_data.arrive;
		if((offset+pnextrec->m_data.arrive-precord->m_data.depart)>(abs_run_time+180))  // ���ƫ�����(����ԭ����ʱ��3���ӣ�����Ӧ����ǰ���¼������ͣվʱ��
		{
			int tmp_index=rec_index;
			while(tmp_index>begin_index)
			{
               CTG_TrainRecord *ptmp_rec=ptrain->GetTrainRecord(tmp_index);
			   if(!ptmp_rec->IsPassRec())
			       break;
			   tmp_index--;
			}
			ptrain->OffsetScheduleTime(tmp_index,offset,FALSE,0);
			rec_index=tmp_index;
			continue;  // ���·��ص���
		}
		
		if(offset>0)
		{
			ptrain->OffsetScheduleTime(rec_index+1,offset,FALSE,1);
		}
		rec_index++;
	}

	
	// 2. �����г����ú󵽷���ϵ�仯����ɵ�
	for(int i=begin_index; i<train.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *poldrecord=train.GetTrainRecord(i);
		CTG_TrainRecord *pnewrecord=ptrain->GetTrainRecord(i);

		if(poldrecord==NULL || pnewrecord==NULL)
			break;
		
		if(poldrecord->GetStationNo() != pnewrecord->GetStationNo())
			break;

		if(poldrecord->IsPassRec())
		{
			if(!pnewrecord->IsPassRec())
			{
				GpcLogTrainMessage(0, 0, "ʹ���Զ���������, �г� %s (%d)�� %d  վ��ͨ��->����,�ɵ��Զ�����",
									ptrain->GetTrainID(), ptrain->GetTrainIndex(), pnewrecord->GetStationNo());
			
				ChangeSideForStopOrPass(ptrain,i);  // �ɵ��Զ�����
			}
		}
		else
		{
			if(pnewrecord->IsPassRec())
			{
				GpcLogTrainMessage(0, 0, "ʹ���Զ���������, �г� %s (%d)�� %d   վ�ɵ���->ͨ��, �ɵ��Զ�����",
									ptrain->GetTrainID(), ptrain->GetTrainIndex(), pnewrecord->GetStationNo());
			
				ChangeSideForStopOrPass(ptrain,i); // �ɵ��Զ�����
			}
		}
	}

	if(rec_index>=ptrain->GetTrainRecordCount()-1)
		return TRUE;
	else
		return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustTrainForCollision(CTG_SingleTrain *ptrain,int begin_index) //�����Աܿ���ͻ���
{
	int rec_index=ptrain->GetTrainRecordCount()-1;
	if(rec_index<=begin_index) // ���һ���ڵ��������
		return FALSE; 
	// ��ʵ�ʵ�ļ�¼���������
	CTG_TrainRecord *precord=NULL;
	while(rec_index>begin_index)
	{
		precord=ptrain->GetTrainRecord(rec_index);
		if(precord->IsActualDepartTimeRecord() || precord->IsActualArriveTimeRecord())
			break;
		rec_index--;
	}
	if(precord==NULL)
		return FALSE;

	if(precord->IsActualDepartTimeRecord())
		rec_index++;
	
	return __AdjustTrainForCollision__(ptrain,rec_index);
}

    // ��������
	// �����ָ��ʱ���ͣվ�ɵ���arriveTime��ʼֵ��Ԥ�Ƶ���ʱ�̣�avaSide��ʼֵ��Ĭ�Ϲɵ�
	// ��� forceSide ΪTRUE,��arriveTime���� avaSide���õ�ʱ��
	// ��� forceSide ΪFALSE,��avaSide����������õĹɵ���arriveTime���ؿ��õ�ʱ��
	// �������� TRUE, ������������ֵ��Ч��������Ч

BOOL  CTGAutoAdjustLogic::CacuAvailableSideForPlanStop(CTG_SingleTrain *ptrain, int recIndex,TIME &arriveTime,int &avaSide,BOOL forceSide)
{
	int trainType,station,entry,exit,priority;
	SIDE_ID ava_sides[MAX_SIDE_COUNT];
	TIME    ava_time[MAX_SIDE_COUNT];

	short  side_hotel_count=MAX_SIDE_COUNT;

	CTG_TrainRecord *precord=ptrain->GetTrainRecord(recIndex);
	if(!precord)
		return FALSE;
    CTG_TrainInfo train_info=*ptrain->GetTrainInfo();
	if(!m_pBasicPara->GetStopOrderSide(train_info.m_data,precord->m_data,ava_sides,&side_hotel_count)) //���ҷ��ϻ��������Ĺɵ�
		return FALSE;
    if(side_hotel_count<=0)
		return FALSE;
	for(int i=0;i<side_hotel_count;i++)   // �ٶ�������
		ava_time[i]=arriveTime;

	station=precord->GetStationNo();
	trainType=ptrain->GetTrainType();
	entry=precord->GetEntry();
	exit=precord->GetExit();
	priority=ptrain->GetPriority();

	TIME begin_time,end_time;
	begin_time=arriveTime;
	end_time=begin_time+3600*3;  // �ٶ�����3Сʱ����г�������
	CTG_ObjectPtrGroup rela_trains;
	m_pWorkSchedule->FindTrainsByStation(rela_trains,station,begin_time,end_time,FALSE,TRUE);
	for(int i=0;i<rela_trains.GetCount();i++)
		{
			CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
           
			if(pRelaTrain->nTrainIndex == ptrain->GetTrainIndex())  // ���Ա��г�
				continue;
			if(pRelaTrain->pTrain->GetPriority()>priority)  // �������ȼ��͵��г�
				continue;
			const CTG_TrainRecord *prela_rec=pRelaTrain->pTrain->GetTrainRecordNoDelete_Const(pRelaTrain->nRecIndex);
			if(!prela_rec)
				continue;
			if(prela_rec->GetArriveTime()>arriveTime)  // ���ڴ˳�վ���ᣬ����
				continue;
			int x=0;
			for(x=0;x<side_hotel_count;x++)
			{
				if(ava_sides[x]==prela_rec->m_data.arrive_side)
                     break;
			}
			if(x>=side_hotel_count)
				continue;
			int depart_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), OCCUPY_SAME_SIDE_GAP,
					prela_rec->IsPassRec()? STATION_PASS:STATION_DEPART,
					precord->IsPassRec()? STATION_PASS:STATION_DEPART,
					pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN,
					ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN);
			ava_time[x]=prela_rec->m_data.depart+depart_gap_time; // ���������
		}
	arriveTime=NULL_TIME;
	for(int x=0;x<side_hotel_count;x++)
	{
		if(forceSide && ava_sides[x]!=avaSide)
			continue;
        if(ava_time[x]<arriveTime || arriveTime==NULL_TIME)
		{
			avaSide=ava_sides[x];
			arriveTime=ava_time[x];
		}
	}
	if(	arriveTime==NULL_TIME)
		return FALSE;
	else
		return TRUE;
}

BOOL  CTGAutoAdjustLogic::AdjustTrainForMutiQuickPass(CTG_SingleTrain *ptrain, int begin_index, int con_id)
{
	for(int i=begin_index; i<ptrain->GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord* rec = ptrain->GetTrainRecord(i);
		if (NULL == rec)
			continue;
		
		// ʵ�ʵ�
		if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
			continue;

		if (TG_IsArriveRouteDoing(rec->m_data) || 
			TG_IsArriveRouteOk(rec->m_data) || 
			TG_IsArriveRouteOccupy(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �ӳ���·�����ų��������ų�����ռ��, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}
		if (TG_IsDepartRouteDoing(rec->m_data) || 
			TG_IsDepartRouteOk(rec->m_data) || 
			TG_IsDepartRouteOccupy(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ������·�����ų��������ų�����ռ��, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}
		if (TG_IsTrainIntoSide(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �Ѿ���վ, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}

		bool bMustStop=false;
		if(TG_IsMustStop(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ����Ϊ����ͣ��, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}

		if(TG_IsMustNoPass(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ����Ϊ��ֹͨ��, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}

		// ����ҵʱ��
		if(rec->m_data.min_stop_time>0)
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ ��С��ҵʱ��Ϊ%d, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()), rec->m_data.min_stop_time);
			bMustStop=true;
		}

		// �Ǽ���ͣ��
		if( !TG_IsTechStopTrain(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "������, �г�%s��%sվ �п�����ҵ, ���ܱ�Ϊͨ��",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}
		
		if(GetTGFunctionConfig()->IsNotThisSectionStation(con_id,  rec->m_data.station))
		{
			GpcLogTrainMessage(0, 0, "��ע��, %s �����ڱ�̨��Ͻ,�����޸ĵ�����ʽ", gpcdef.GetStationName(rec->m_data.station));
			bMustStop=true;
		}

		CTG_TrainRecord *pprecrec=ptrain->GetTrainRecord(i-1);
		if(NULL==pprecrec)
			continue;

		int abs_run_time = m_pBasicPara->CalculateTrainRunTime(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());
		if(rec->IsPassRec())
		{
			if(bMustStop)
			{
				AdjustForStopPassChange(ptrain, i, FALSE);
			}
			else
			{
				int start_extra_time=0;
				if(!pprecrec->IsPassRec() || ((i==0)&&pprecrec->IsStartRec()))
					start_extra_time=m_pBasicPara->CalculateTrainStartExtra(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());

				rec->m_data.arrive = pprecrec->m_data.depart + start_extra_time + abs_run_time;
				rec->m_data.depart = rec->m_data.arrive;
			}
		}
		else
		{
			if(bMustStop)
			{
				int start_extra_time=0;
				if(!pprecrec->IsPassRec() || ((i==0)&&pprecrec->IsStartRec()))
					start_extra_time=m_pBasicPara->CalculateTrainStartExtra(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());

				int stop_extra_time = m_pBasicPara->CalculateTrainStopExtra(rec->GetStationNo(),rec->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
		
				TIME stop_time = rec->m_data.depart - rec->m_data.arrive;
				rec->m_data.arrive = pprecrec->m_data.depart + start_extra_time + abs_run_time + stop_extra_time;
				rec->m_data.depart = rec->m_data.arrive+stop_time;
			}
			else
			{
				AdjustForStopPassChange(ptrain, i, TRUE);
			}
		}
	}

	return TRUE;
}

BOOL  CTGAutoAdjustLogic::GetAbsMarkStation(const CTG_SingleMark *pmark,int &station1,int &station2)
{
	if(pmark->IsStationMarkType())
		return FALSE;
	if(pmark->m_data.ele_count<=0)
		return FALSE;
	int absnum = pmark->m_data.ele_number[0]; // �˴�ѡ���һ�����䣬�ٶ����������Ӧ��վ����ͬ��

	ushort stat1=0,stat2=0,entry1=0,entry2=0;
	if(!m_pBasicPara->ConvertAbsnumToStationEntry(&stat1,&entry1,&stat2,&entry2,absnum))
		return FALSE;
	station1=stat1;
	station2=stat2;
	return TRUE;
    
}
//������mark�����ĳ���,ע�ⷵ�ص��� CTG_StationTrainStruct���͵�ָ�����飬����ʱע��Ϊ�յ�Ҫ����
BOOL  CTGAutoAdjustLogic::GetRelaTrainsOfMark(const CTG_SingleMark *pmark,CTG_ObjectPtrGroup &rela_trains) 
{
	TIME  begin_time=pmark->GetStartTime();
	TIME  end_time=pmark->GetEndTime();

	int mark_type=pmark->GetMarkType();
	if(mark_type==TG_SIDE_INHIBIT || mark_type==TG_SIDE_TIANCHUANG || mark_type==TG_SIDE_OCCUPY) //�ɵ�����
	{
	   	m_pWorkSchedule->FindTrainsByStation(rela_trains,pmark->GetStation(),begin_time,end_time,TRUE,FALSE); //ֻ�ǲ��ң��ڷ�Χ�ڵ���ʱ��
       for(int i=0;i<rela_trains.GetCount();)
	   {
		   CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
		   CTG_TrainRecord *precord=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
		   if(!precord || (IS_VALID_SIDE(precord->m_data.arrive_side) && !pmark->MatchStationSide(precord->GetStationNo(),precord->m_data.arrive_side)))
				rela_trains.RemovePtr(i);
		   else
			    i++;
	   }
	}
	else
	if(mark_type==TG_ABS_INHIBIT || mark_type==TG_ABS_FIX_MARK  || mark_type==TG_SECTION_ELEC_FIX) // ��������
	{
	  int  station,station1,station2;
	  if(!GetAbsMarkStation(pmark,station1,station2))
		  return FALSE;
	  for(int step=0;step<2;step++)
	  {
	   if(step==0)
	      station=station1;
	   else
		  station=station2;
       m_pWorkSchedule->FindTrainsByStation(rela_trains,station,begin_time,end_time,FALSE,TRUE); //ֻ�ǲ��ң��ڷ�Χ�ڳ���ʱ��
      }
	   ushort abs_num=0;
	   ushort dir=0xffff;
	   for(int i=0;i<rela_trains.GetCount();)
	   {
	      CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
		   CTG_TrainRecord *precord=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
	      if(!precord 
			  || !m_pBasicPara->ConvertStationEntryToAbsnum(precord->GetStationNo(),precord->GetExit(),&abs_num,&dir)
			  || !pmark->MatchAbsNumber(abs_num))
			    rela_trains.RemovePtr(i);
		  else
			    i++;
	   }
	}
	if(rela_trains.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustSingleTrainForMark(CTG_SingleTrain &train,int recIndex,const CTG_SingleMark *pmark)
{
	int mark_type=pmark->GetMarkType();
	if(mark_type==TG_SIDE_INHIBIT || mark_type==TG_SIDE_TIANCHUANG || mark_type==TG_SIDE_OCCUPY) //�ɵ�����
	{
		ChangeSideForStopOrPass(&train,recIndex,1,0); // ���¼���ɵ���Ӧ�û�Ҫ�Ƚ�һ���Ƿ�ȷʵ�����
		CTG_TrainRecord* prec=train.GetTrainRecordByRecIndex(recIndex);
		if(prec)
		{
			GpcLogTrainMessage(0, 0, "[�ɵ����� ����]: %s(%d)��վ%d(entry %d exit %d), %d %d", 
				train.GetTrainID(),train.GetTrainIndex(),prec->m_data.station, prec->m_data.entry, prec->m_data.exit, prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}
	else
	if(mark_type==TG_ABS_INHIBIT || mark_type==TG_ABS_FIX_MARK  || mark_type==TG_SECTION_ELEC_FIX) // ��������
	{
	   AdjustTrainForCollision(&train,recIndex);
	}
	return TRUE;
}

// TestTgView.cpp : CTgEditView ���ʵ��
//

#include "stdafx.h"
#include "tg.h"
#include "resource.h"
#include "TgEditView.h"
#include "sysshare.h"
#include "if.h"
#include "NeibPlanFuncChoiceDlg.h"
#include "SelectScheduleTrain.h"
#include "SelectScheduleMark.h"

#include "function.h"
#include "CPropertysheetNotepad.h"
#include "ManualReportHistory.h"
#include "DlgTrainList.h"
#include "DlgSelectShift.h"
#include "AddNewTrainDlg.h"
#include ".\tgeditview.h"
#include "propertypage_phasenoteplan.h"
#include "InputHatTrain.h"
#include "ChangeTrainRoute.h"
#include "DlgNeibTrainList.h"
#include "DlgRestoreTrainList.h"
#include "tg_binxml.h"
#include "..\SuperPassDlg.h"
#include "utility\mmi_function_config.h"
#include "InsertTrainDlg.h"
#include "InsertTrainDlgLanzhou.h"
#include "DlgDividePortLateTime.h"
#include "DlgTrainRecord.h"
#include "dlgwait.h"
#include "DlgDirectSetTrigFlag.h"
#include "DlgBasicChange.h"
#include "DlgBasicParam.h"
#include "DlgDeleteTrainByIndex.h"
#include "DlgBasicDeleteName.h"
#include "..\Alarm\SoundPlay.h"

#include ".\TrainCrewTicket.h"
#include "DlgMultiAdjust.h"
#include "DlgTDMSBasicTip.h"
#include "DlgTDMSBasicGen.h"
#include "DlgConfirm.h"
#include "DlgStationReqChangeSide.h"
#include "DlgBasicTrainDeleteChoice.h"
#include "DlgBatchSideModify.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CLoadGPCDef m_load;
extern int DB_GetEntityStatus(long eid);
// CTgEditView
BOOL UR_CanOperateBasicSchedule();
BOOL UR_CanOperateSchedule();
IMPLEMENT_DYNCREATE(CTgEditView, CTgView)

BEGIN_EVENTSINK_MAP(CTgEditView, CTgView)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 1, APPEND_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 2, UPDATE_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 3, DELETE_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 4, APPEND_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 5, UPDATE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 6, TG_DELETE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 7, TG_LOG, VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 8, SELECT_TRAIN, VTS_I4 VTS_I4 VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 9, SELECT_MARK, VTS_I4 VTS_I1 VTS_I1 VTS_I2 VTS_I2 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 10, TG_NOTICE, VTS_I1 VTS_I4 VTS_I4 VTS_I2)
	
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 14, TG_FIRE_DELETE, VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 16, SetLateEarlyTimeRd, VTS_BSTR VTS_I1 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 18, MutiTrainOper, VTS_BSTR VTS_I4) 
END_EVENTSINK_MAP()
// CTgEditView ����/����
BEGIN_MESSAGE_MAP(CTgEditView, CTgView)
	ON_COMMAND(ID_TEMP_ADD_TRAIN,OnAddTmpTrain)
	ON_COMMAND(ID_INSERT_NEW_TRAIN, OnInsertNewTrain)
	ON_COMMAND(ID_DRAW_NEW_TRAIN_END, OnEndInsertNewTrain)
	ON_COMMAND(ID_MANUAL_ADD_NEW_TRAIN, OnManualAddNewTrain)
	ON_COMMAND(IDC_DEL_MULTI_TRAIN, OnDelMultiTrain)
	ON_COMMAND(ID_ABS_INHIBIT_G, OnAbsInhibitG)
	ON_COMMAND(ID_ABS_SLOW_G, OnAbsSlowG)
	ON_COMMAND(ID_ABS_ELE_FIX_G, OnAbsEleFixG)
	ON_COMMAND(ID_ABS_FIXED, OnAbsFixG)
	ON_COMMAND(ID_SELF_CLOSE_G, OnSelfCloseG)
	ON_COMMAND(ID_DRAW_GOON, OnDrawGoon)
	ON_COMMAND(ID_DRAW_RETURN, OnDrawReturn)
	ON_COMMAND(ID_DRAW_ABSTRAIN_A, OnDrawAbstrainA)
	ON_COMMAND(ID_DRAW_ABSTRAIN_B, OnDrawAbstrainB)

	ON_COMMAND(ID_SIDE_INHIBIT_G, OnSideInhibitG)
	ON_COMMAND(ID_STATION_SLOW_G, OnStationSlowG)
	ON_COMMAND(ID_SIDE_TIANCHUANG_G, OnSideTianchuangG)
	ON_COMMAND(ID_SIDE_OCCUPY, OnSideOccupy)
	ON_COMMAND(ID_SIDE_LOST_ELE, OnSideLostEle)
	ON_COMMAND(ID_TEXT_NOTE, OnTextNote)
	ON_COMMAND(ID_JISHI_ZHUJIE, OnJishiZhujie)
	ON_COMMAND(ID_UNITE_TRAIN, OnUniteTrain)
	ON_COMMAND(ID_RESCUE_TRAIN, OnRescueTrain)
	ON_COMMAND(ID_SPLIT_TRAIN, OnSplitTrain)
	ON_COMMAND(ID_COMBINE_TRAIN, OnCombineTrain)
	ON_COMMAND(ID_ABS_PART_RUN, OnAbsPartRun)
	ON_COMMAND(ID_ABS_WHOLE_HELP_RUN, OnAbsWholeHelpRun)
	
	ON_COMMAND(ID_ABS_RETURN_RUN, OnAttainEngineReturn)

	ON_COMMAND(ID_MANUAL_SELECT_BASIC_TRAIN, OnManualSelectBasicTrain)
	ON_COMMAND(ID_MANUAL_SELECT_SHIFT_TRAIN, OnManualSelectShiftTrain)
	ON_COMMAND(ID_MANUAL_SELECT_TRAIN, OnManualSelectTrain)

	ON_COMMAND(ID_MANUAL_SELECT_SHIFT_MARK,OnManualSelectShiftMark)
	ON_COMMAND(ID_MANUAL_SELECT_BASIC_MARK,OnManualSelectBasicMark)
	ON_COMMAND(IDC_SEND_STATION_PLAN, OnSendStationPlan)

	ON_COMMAND(ID_MANUAL_REPORT_HISTORY, OnManualReportHistory)
	ON_COMMAND(ID_SHIFT_GET_TRAIN_FROM_OPMS,OnGetTrainList)
	ON_COMMAND(ID_BASIC_GET_TRAIN_FROM_OPMS,OnGetBasicTrainFromOpms)
	ON_COMMAND(ID_SHIFT_GET_MARK_FROM_OPMS,OnGetMarkList)
	ON_COMMAND(ID_WORK_GET_TRAIN_FROM_OPMS,OnRecvTrainList)
	ON_COMMAND(ID_WORK_GET_MARK_FROM_OPMS,OnRecvMarkList)
	ON_COMMAND(ID_DELETE_TRAIN, OnDeleteTrain)
	ON_WM_DESTROY()
	ON_COMMAND(ID_GET_ENGINE_TICKET, OnGetEngineTicketInfo)
	ON_UPDATE_COMMAND_UI(ID_GET_ENGINE_TICKET, OnUpdateGetEngineTicketInfo)
	ON_COMMAND(ID_SHIFT_GET_SHIFTINFO, OnShiftGetShiftinfo)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_SHIFTINFO, OnUpdateShiftGetShiftinfo)
	ON_COMMAND(ID_HAT_TRAIN, OnHatTrain)
	ON_COMMAND(ID_ELEC_ARMS_SET, OnElecArmsSet)
	ON_UPDATE_COMMAND_UI(ID_ELEC_ARMS_SET, OnUpdateElecArmsSet)
	ON_COMMAND(ID_MANUAL_OPER_CHECK, OnManualOperCheck)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_OPER_CHECK, OnUpdateManualOperCheck)


	ON_UPDATE_COMMAND_UI(ID_BASIC_GET_TRAIN_FROM_OPMS,OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_INSERT_NEW_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NEW_TRAIN_END,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_ADD_NEW_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(IDC_DEL_MULTI_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_UNITE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_RESCUE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_SPLIT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_BASIC_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_SHIFT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_TRAIN, OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_MANUAL_REPORT_HISTORY,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_HAT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_COMBINE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(IDC_SEND_STATION_PLAN,OnUpdateSendPlan)
	ON_UPDATE_COMMAND_UI(IDC_SEND_NEIGHBER,OnUpdateSendPlan)

	ON_COMMAND(IDC_RICEIVE_PLAN_1, OnNeighberPlan)
	ON_UPDATE_COMMAND_UI(IDC_RICEIVE_PLAN_1,OnUpdateOperTGTrain)

	ON_COMMAND(IDC_SEND_NEIGHBER, OnSendNeighber)

	ON_COMMAND(ID_SEND_NEIGBOUR_PLAN, OnSendNeigbourPlan)
	ON_UPDATE_COMMAND_UI(ID_SEND_NEIGBOUR_PLAN,OnUpdateOperTGTrain)

	ON_COMMAND(IDC_RICEIVE_NEIGB_PLAN, OnRecvNeigbourPlan)
	ON_UPDATE_COMMAND_UI(IDC_RICEIVE_NEIGB_PLAN,OnUpdateOperTGTrain)

	ON_COMMAND(ID_TURN_REALITY, OnTurnReality)
	ON_UPDATE_COMMAND_UI(ID_TURN_REALITY,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_DELETE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MUTI_TRAIN_MOVE,OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_ABS_INHIBIT_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_SLOW_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SELF_CLOSE_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_ELE_FIX_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_FIXED,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_DRAW_GOON,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RETURN,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_A,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_B,OnUpdateOperAddWorkTrain)

	ON_UPDATE_COMMAND_UI(ID_SIDE_INHIBIT_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_STATION_SLOW_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_TIANCHUANG_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_OCCUPY,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_LOST_ELE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_TEXT_NOTE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_JISHI_ZHUJIE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_SHIFT_MARK,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_BASIC_MARK,OnUpdateOperTGMark)

	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_MARK_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_TRAIN_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_WORK_GET_TRAIN_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_WORK_GET_MARK_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_SHIFTINFO,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DELETE_ALL_TRAIN,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DELETE_ALL_MARK,OnUpdateOperTGOPMS)

	ON_COMMAND(ID_SHIFT_DELETE_ALL_TRAIN, OnDeleteAllShiftTrain)
	ON_COMMAND(ID_SHIFT_DELETE_ALL_MARK, OnDeleteAllShiftMark)

	ON_COMMAND(ID_BACK_OPER, OnBackOper)
	ON_UPDATE_COMMAND_UI(ID_BACK_OPER,OnUpdateBackOper)

	ON_COMMAND(ID_CHANGE_TRAINROUTE, OnChangeTrainRoute)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_TRAINROUTE,OnUpdateOperTGTrain)
	ON_COMMAND(ID_SYS_SAVE_TO_FILES,OnSaveToFiles)
	ON_COMMAND(ID_SYS_LOAD_FROM_FILES,OnLoadFromFiles)	

	ON_UPDATE_COMMAND_UI(ID_SYS_LOAD_FROM_FILES,OnUpdateOperRestore)

	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)

	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETE_ALL,OnUpdateDeleteBasic)
	ON_COMMAND(ID_BASIC_DELETE_ALL, OnDeleteAllBasic)

	ON_UPDATE_COMMAND_UI(ID_SAVE_SIDE_WORK,OnUpdateSaveBasicSideWorkInfo)
	ON_COMMAND(ID_SAVE_SIDE_WORK, OnSaveBasicSideWorkInfo)

	ON_UPDATE_COMMAND_UI(ID_LOAD_BASIC_SIDE_WORK,OnUpdateOperTGTrain)
	ON_COMMAND(ID_LOAD_BASIC_SIDE_WORK, OnLoadBasicSideWorkInfo)


	ON_UPDATE_COMMAND_UI(ID_BASIC_GRAPHSAVETOTEXT,OnUpdateOperTGTrain)
	ON_COMMAND(ID_BASIC_GRAPHSAVETOTEXT, OnBasicGraphsavetotext)

	ON_COMMAND(ID_ENABLE_FREE_ADDTRAIN,OnEnableFreeAddTrain)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_FREE_ADDTRAIN,OnUpdateEnableFreeAddTrain)

	ON_COMMAND(ID_GEN_TG_NODE_INDEX,OnGenNodeIndex)
	ON_UPDATE_COMMAND_UI(ID_GEN_TG_NODE_INDEX,OnUpdateGenNodeIndex)

	ON_COMMAND(ID_DIRECT_UPDATE_FROM_TDMS,OnDirectUpdateFromOPMS)
	ON_UPDATE_COMMAND_UI(ID_DIRECT_UPDATE_FROM_TDMS,OnUpdateOperTGTrain)

	ON_COMMAND(ID_MUTI_TRAIN_MOVE, OnMutiTrainMove)

	ON_COMMAND(ID_BASIC_NAMEING, OnBasicNameing)
	ON_UPDATE_COMMAND_UI(ID_BASIC_NAMEING, OnUpdateBasicNameing)

	ON_COMMAND(ID_BASIC_DELETENAME, OnBasicDeleteName)
	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETENAME, OnUpdateBasicDeleteName)

    ON_COMMAND(ID_MODIFY_BSCHD, OnModifyBschd)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_BSCHD, OnUpdateModifyBschd)

	ON_COMMAND(ID_TRAINPLAN_SIDE_MAN, OnTrainPlanSideMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_SIDE_MAN, OnUpdateTrainPlanSideMan)

	ON_COMMAND(ID_TRAINPLAN_ROUTE_MAN, OnTrainPlanRouteMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_ROUTE_MAN, OnUpdateTrainPlanRouteMan)

	ON_COMMAND(ID_BASIC_GRAPHCHANGE, OnBasicGraphchange)
	ON_UPDATE_COMMAND_UI(ID_BASIC_GRAPHCHANGE, OnUpdateBasicGraphchange)

	ON_COMMAND(ID_DIVIDEPORT_LATETIME, OnDivideportLatetime)
	ON_UPDATE_COMMAND_UI(ID_DIVIDEPORT_LATETIME,OnUpdateOperTGTrain)


	ON_COMMAND(ID_DELETE_MARK, OnDeleteMark)
	ON_COMMAND(ID_DELETE_TRAIN_BY_INDEX, OnDeleteTrainByIndex)
	ON_COMMAND(ID_DELETE_INVALID_TRAIN, OnDeleteInvalidTrain)

	ON_COMMAND(ID_AUTO_ADJUST_COL, OnAutoAdjustCol)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_COL,OnUpdateOperTGTrain)
	ON_COMMAND(ID_AUTO_ADJUST_PASS, OnAutoAdjustPass)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_PASS, OnUpdateOperTGTrain)
	ON_COMMAND(ID_AUTO_ADJUST_MULTI, OnAutoAdjustMulti)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_MULTI, OnUpdateOperTGTrain)
	ON_COMMAND(ID_OPT_AUTO_ADJUST_COLLISION, OnOptAutoAdjustCollision)
	ON_UPDATE_COMMAND_UI(ID_OPT_AUTO_ADJUST_COLLISION, OnUpdateOptAutoAdjustCollision)

	ON_WM_KEYDOWN()
	ON_COMMAND(ID_DIRECT_SET_AUTOTRIG, OnDirectSetAutotrig)
	ON_UPDATE_COMMAND_UI(ID_DIRECT_SET_AUTOTRIG, OnUpdateOperTGTrain)

	ON_COMMAND(ID_PREDICT_STPC, OnPredictStpc)
	ON_WM_TIMER()

	ON_COMMAND(IDC_RECEIVE_POINT, OnReceivePoint)
	ON_UPDATE_COMMAND_UI(IDC_RECEIVE_POINT, OnUpdateReceivePoint)

	ON_COMMAND(ID_EDIT_ALL_CZH, OnEditAllCzh)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ALL_CZH, OnUpdateEditAllCzh)

	ON_COMMAND(ID_GEN_ALL_RETURN_TRAINID, OnGenAllReturnTrainid)
	ON_UPDATE_COMMAND_UI(ID_GEN_ALL_RETURN_TRAINID, OnUpdateGenAllReturnTrainid)

	ON_COMMAND(ID_MODIFY_SHIFT_SCHD, OnModifyShiftschd)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_SHIFT_SCHD, OnUpdateModifyShiftschd)

	ON_COMMAND(ID_SET_MULTI_SIDE_LOCK, OnSetMultiSideLock)
	ON_UPDATE_COMMAND_UI(ID_SET_MULTI_SIDE_LOCK, OnUpdateSetMultiSideLock)

	ON_COMMAND(ID_QUICK_MODIFY_MULTI_TRAINS, OnQuickModifyMultiTrains)
	ON_UPDATE_COMMAND_UI(ID_QUICK_MODIFY_MULTI_TRAINS,OnUpdateOperTGTrain)

	ON_COMMAND(ID_SEL_TDMS_TRAIN_GEN_BASIC, OnSelTdmsTrainsGenBasic)
	ON_UPDATE_COMMAND_UI(ID_SEL_TDMS_TRAIN_GEN_BASIC,OnUpdateSelTdmsTrainsGenBasic)
	
	ON_COMMAND(ID_SHIFT_GENTRAIN_TO_WORK, OnShiftTrainToWork)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GENTRAIN_TO_WORK,OnUpdateShiftTrainToWork)

	ON_COMMAND(ID_BASIC_GENTRAIN_TO_WORK, OnBasicTrainToWork)
	ON_UPDATE_COMMAND_UI(ID_BASIC_GENTRAIN_TO_WORK,OnUpdateBasicTrainToWork)

	ON_COMMAND(ID_TRAINPLAN_COMPARE, OnTrainPlanCompareMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_COMPARE, OnUpdateTrainPlanCompareMan)
	
	ON_COMMAND(ID_BATCH_ALLOCATION_SIDE, OnBatchAllocationSide)
	ON_UPDATE_COMMAND_UI(ID_BATCH_ALLOCATION_SIDE, OnUpdateBatchAllocationSide)
END_MESSAGE_MAP()

CTG_TrainScheduleBase* CTgEditView::GetTrainScheduleBase()
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL;
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"�����հ�ͼTrainExpand");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"���µ���ͼTrainExpand");
	}

	return pData;
}

BOOL  CTgEditView::ProcessXML_Kediao(const char *xml, const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	CTG_TrainKediaoInfo kd;
	if(!TD_GetKediaoInfoFromXML(kd,xml,node))
		return FALSE;
	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� �͵���Ϣ node:%s msgid:%d", GetViewScheduleType(), node, kd.msgid);
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==kd.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==kd.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(kd.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainKediaoSame(kd))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainKediao(kd);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}		
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Yingye(const char *xml, const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	CTG_TrainYingyeList yy;
	if(!TD_GetYingyeInfoFromXML(yy,xml,node))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� Ӫҵ��Ϣ node:%s msgid:%d", GetViewScheduleType(), node, yy.msgid);

	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==yy.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==yy.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(yy.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainYingyeSame(yy))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainYingye(yy);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Crew(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;

	CTG_TrainCrewList crew;
	if(!TD_GetCrewInfoFromXML(crew,xml,node))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� ������Ϣ node:%s msgid:%d", GetViewScheduleType(), node, crew.msgid);

	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==crew.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==crew.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(crew.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainCrewSame(crew))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainCrew(crew);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Ticket(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;

	CTG_TrainTicketList tk;
	if(!TD_GetTicketInfoFromXML(tk,xml,node))
		return FALSE;
	
	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� ��Ʊ��Ϣ node:%s msgid:%d", GetViewScheduleType(), node, tk.msgid);
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==tk.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==tk.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(tk.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainTicketSame(tk))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainTicket(tk);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_TrainEngineCrew(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	// ������Ϣ
	CTG_EngineCrewList engine_crew;
	int opms_seq=0,opms_count=0;
	if(!TD_LoadEngineCrewInfoFromXML(engine_crew,xml,node,&opms_seq,&opms_count))
		return FALSE;
	
	CString waitstr;
	waitstr.Format("���ڽ���TDMS�հ�ƻ����� %s(%u), %d/%d, msgid:%d",engine_crew.trainid,engine_crew.gtid,
		opms_seq,opms_count, engine_crew.msgid); 
	GpcLogTrainMessage(0,0,"%s",waitstr);
	
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==engine_crew.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==engine_crew.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(engine_crew.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainEngineCrewSame(engine_crew))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainEngineCrew(engine_crew);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

LRESULT CTgEditView::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;
	if(pMsgStruct->msg_id == MSGID_MsgStringMessage)
	{
		CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pBase;

		const char *token=pMsg->GetToken();
		const char *content=pMsg->GetContent();
		if(token && content)
		{
			if(!stricmp(token,TOKEN_19_ACK_BASICSCHEDULE))
			{
				ProcessXML_BasicScheduleTrain(content,token);
			}
			if(	!stricmp(token,TOKEN_3_ACK_YL_BY_TIME))
			{
				ProcessXML_ShiftTrain(content,token);
			}
			else if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Kediao(content,token);
				else
					ProcessXML_YingyeKediao(content,token);
			}
			else if(!stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Yingye(content,token);
				else
					ProcessXML_YingyeKediao(content,token);
			}
			else if(!stricmp(token,TOKEN_20_ACK_CREW_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Crew(content,token);
				else
					ProcessXML_TrainCrew(content,token);
			}
			else if(!stricmp(token,TOKEN_30_ACK_PASSENGER_TICKET_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Ticket(content,token);
			}
			else if(!stricmp(token,TOKEN_31_ACK_ENGINE_CREW_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_TrainEngineCrew(content,token);
			}
			else if(!stricmp(token,"ALERT"))
			{
				ProcessXML_Alert(content, token);
			}
		}
	}
	else if (pMsgStruct->msg_id == MSGID_MsgParamentReply || pMsgStruct->msg_id == MSGID_MsgParamentOperation)
	{
		long this_entity=gpcdef.GetThisEntity();
		CLBTH::MsgParamentReply *pServerReply = NULL;
		if (pMsgStruct->msg_id == MSGID_MsgParamentReply)
			pServerReply = ( CLBTH::MsgParamentReply *)pMsgStruct->pmsg;
		else 
		{
			CLBTH::MsgParamentOperation* tmp = (CLBTH::MsgParamentOperation*) pMsgStruct->pmsg;
			if (tmp->oper == CLBTH::MsgParamentOperation::PARAOPER_INSERT_UPDATE)
				pServerReply = tmp;
		}

		if(NULL != pServerReply && this_entity==pServerReply->entity)
		{
			char *token,*ptr;
			char seps[]=" ,\t\\/=\n";
			GpcLogSysMessage(0,"����������Ȩ������:");
			for (int i=0; i<pServerReply->type_count; i++)
			{
				if (pServerReply->type[i] == (int)CLBTH::MsgParamentReply::WINTG && pServerReply->scope==gpcdef.GetDefaultSection())
				{
					if (pServerReply->int_parament[i] & 0x0020)
					{ // ��������Ȩ���޸�ʵ��ͼ����������ͼ������
						gpcdef.SetManualConvertActualTime(false);
						sysprint(0,PMTG.ThreadId,"MsgParamentReply, �������������޸�ʵ�ʵ�Ȩ��, ����ͼ�����Ƶ���Ա�޸�ʵ�ʵ�");
					}
					else
					{ // ��������Ȩ���޸�ʵ��ͼ����������ͼ ����+ʱ��
						gpcdef.SetManualConvertActualTime(true);
						sysprint(0,PMTG.ThreadId,"MsgParamentReply, ����������û���޸�ʵ�ʵ�Ȩ��,����ͼ ����+ʱ��");
					}
					
					int modify_time=0;
					ptr=pServerReply->str_parament[i];
					token = strtok(ptr, seps);
					if(token!=NULL)
					{
						modify_time=(int)atoi(token); 
					}

				
					GpcLogSysMessage(0,"���������޸�ʵ�ʵ�ʱ�䳤��:%d", modify_time);
					mTG.SetTrainDisplayFlag(TRAIN_DISPLY::RECIEVE_POINT_LIMIT_TIME, modify_time);
					gpcdef.SetManualConvertActualTimeLength(modify_time);

					if(gpcdef.IsInhibitManualConvertActualTime())
					{
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 1);
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 1);

						m_gpcContext.mReverseFlag[1][15]=0;
						GpcLogSysMessage(0,"�����޸�ʵ�ʵ�");
					}
					else
					{
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 0);
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 0);
						GpcLogSysMessage(0,"�����޸�ʵ�ʵ�");
					}
					if (pMsgStruct->msg_id == MSGID_MsgParamentOperation)
					{
						AfxMessageBox("�û�����Ȩ�ޱ��!");
					}

				}
				else if (pServerReply->scope==0 && pServerReply->entity==0)
				{	
					;
				}
				if (pServerReply->type[i] == (int)CLBTH::MsgParamentReply::MAINTN)
				{	
					if (pServerReply->int_parament[i] == 0x0001)
					{
						gpcdef.SetRightManangRight();
					}
				}
			}
		}
	}
	else if(pMsgStruct->msg_id == MSGID_MsgChangeSideReq)
	{
		long source_id=pMsgStruct->source_id;
		MsgChangeSideAck ackmsg;
		ackmsg.result = 1;
		SendBMsg(&ackmsg, source_id);
		GpcLogTrainMessage(0,0, "�յ���վ�ɵ��޸�����,��վʵ�� %u", source_id);
		CLBTH::MsgChangeSideReq *pChangeSideReq = ( CLBTH::MsgChangeSideReq *)pMsgStruct->pmsg;
		if(pChangeSideReq)
		{
			int station=pChangeSideReq->station;
			GpcLogTrainMessage(0,0, "��ʼ������վ %d �ɵ��޸�����", station);
			OnDisplaySideReqChangeDlg(pChangeSideReq, station);
		}
		else
		{
			GpcLogTrainMessage(0,0, "������վ�ɵ��޸�����,ʧ��");
		}
	}
	else if(pMsgStruct->msg_id == MSGID_MsgElecArmSet)
	{
		CLBTH::MsgElecArmSet* pArm=(CLBTH::MsgElecArmSet *)pMsgStruct->pmsg;
		GpcLogTrainMessage(0,0,"����%d�����Ĺ�������: arm_id %d sta %d set_entity %d set_time %d status %X",
				pMsgStruct->source_id,pArm->elec_arm_id, pArm->elec_sta, pArm->set_entity, pArm->set_time, pArm->status);
		if(GetTGFunctionConfig()->IsEnableScadaElecArm())
		{
			if(pArm->status==CLBTH::MsgElecArmSet::PREP_ENABLE
				|| pArm->status==CLBTH::MsgElecArmSet::PREP_DISABLE)
			{
				OnElecArmsSet();
				m_DlgArms.ReceiveMsgElecArmSet(pArm);
			}
		}
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CTgEditView::ReqParament()
{
	long this_entity=gpcdef.GetThisEntity();
	CLBTH::MsgParamentRequest msg;

	msg.type_count = 2;
	msg.type[0] = CLBTH::MsgParamentRequest::MAINTN;
	msg.type[1] = CLBTH::MsgParamentRequest::WINTG;
	msg.entity = this_entity;
	msg.scope = gpcdef.GetDefaultSection();

	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id<=0)  
	{
		GpcLogTrainMessage(0,0,"[REQ_PARAMENT]: err can't get svr SERVER_TYPE_PARA entity");
	}
	else
	{
		GpcLogTrainMessage(0,0,"[REQ_PARAMENT]:svr_entity:%d entity:%d section:%d", server_id, msg.entity, msg.scope);
		for(int i=0; i<msg.type_count; i++)
		{
			GpcLogTrainMessage(0,0,"[REQ_PARAMENT]:type[%d]:%d", i, msg.type[i]);
		}
		SendBMsg(&msg, server_id);
	}
}


extern BOOL  CheckAndLaunchMemoryNotify();
extern DWORD AddToMemoryNotify(const char *desc,time_t begintime,int repeat,int type,DWORD objindex, int flag);
#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
{MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
	if(pNode) \
{ \
	_variant_t v; \
	v = pNode->GetnodeTypedValue(); \
	dest = CString((v.bstrVal)); \
	SysFreeString(v.bstrVal); \
	v.Clear(); \
} \
			else dest.Empty();};
BOOL CTgEditView::ProcessXML_Alert(const char *xml,const char *node)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	GpcLogTrainMessage(0, 0, "����:%s", xml);

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( xml )))
	{
		return FALSE;
	}
	
	MSXML::IXMLDOMNodePtr pRoot1 = pXMLDoc->selectSingleNode( "ALERT");
	if(pRoot1)
	{
		MSXML::IXMLDOMNodePtr pAlert = pRoot1->selectSingleNode( "OVER_WORK_ALERT");
		if(pAlert)
		{
			CString str;
			GET_NODE_STRVALUE(str, pAlert, "@conid");
			int conid=atoi(str);
			GET_NODE_STRVALUE(str, pAlert, "@train_index");
			long train_index=atol(str);
			GET_NODE_STRVALUE(str, pAlert, "@flag");
			long flag=atol(str);
			if(conid==m_schdinfo.m_nSectionID)
			{
				CString strStartTm, strWorkTm, strTrainId;
				GET_NODE_STRVALUE(strStartTm, pAlert, "@start_time");
				GET_NODE_STRVALUE(strWorkTm, pAlert, "@standard_work_time");
				GET_NODE_STRVALUE(strTrainId, pAlert, "@train_id");
				
				//1. ������
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData(); 
				if(pData)
				{
					CTG_SingleTrain* ptrain=pData->GetSingleTrain(train_index);
					if(ptrain)
					{
						int nCount=ptrain->GetTrainRecordCount();
						CTG_TrainRecord* prec=ptrain->GetTrainRecordNoDelete(nCount-1);
						if(prec)
						{
							if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
							{
								GpcLogTrainMessage(0,0,"��ȡβվ, ����ʵ�ʵ�,���Ա���");
							}
							else
							{
								long tt=atol(strWorkTm);
								CString desc;
								desc.Format("���� %d Сʱ����,����:%s\t�ϸ�ʱ��:%s ��׼����ʱ��:%dʱ%d��", 
										flag, ptrain->GetTrainID(),strStartTm, tt/60, tt%60);
							
								CTime tm=CTime::GetCurrentTime();
								AddToMemoryNotify(desc,tm.GetTime(),1,0,0,flag); // flag=1 or 3
								CheckAndLaunchMemoryNotify();
							}
						}
						else
						{
							GpcLogTrainMessage(0,0,"�ҵ��г�,���ǻ�ȡβվʧ��,���Ա���");
						}
					}
					else
					{
						GpcLogTrainMessage(0,0,"����ALERT���� %d �Ҳ����г�, ���Ա���", train_index);
					}
				}
				
				//2. ��ִ����---2015-01-16��������������ִ
				//CString token, xml;
				//token="ACK_ALERT";
				//xml.Format("<ACK_ALERT> <OVER_WORK_ALERT conid=\"%d\" train_index=\"%d\" flag=\"%d\" /> </ACK_ALERT>", 
				//	conid, train_index, flag);
				
				// ��ȡ��������entity
				//long  eid=GetSpecialEntityID(SERVER_TYPE_MGATE, conid);
				//if(eid>0)
				//{
				//	GpcLogTrainMessage(0,0,"�򱨾�����e%d ���ͻ�ִtoken:%s XML:%s", eid, token, xml);
				//	TGSendStringMessage(token,xml,eid);
				//}
				//else
				//{
				//	GpcLogTrainMessage(0,0,"[Error]�򱨾������ͻ�ִʧ��,�Ҳ�����������ʵ�� token:%s XML:%s", token, xml);
				//}
			}
		}
	}
	return TRUE;
}

CTgEditView::CTgEditView()
{
	m_pSendPlanDlg=NULL;
	m_pNotepadDlg=NULL;
	m_predictDlg=NULL;
	m_pTrainSideDlg=NULL;
	m_pTrainRouteDlg=NULL;
	m_pWorkSchdCompareDlg=NULL;
	m_pShiftSchdCompareDlg=NULL;
	m_pBasicSchdCompareDlg=NULL;
	m_pPredictServer = NULL;

	m_currentOper.nOperStatus=OPER_STATUS_IDLE;
	m_bUserMemDC=TRUE;
	m_bEnableProcessTdmsBasicInfo=false;
	m_bEnableProcessTdmsShiftInfo=false;
	m_bSelTDMSTrainGenBasic=false;
	m_bEnableFreeAddTrain=TRUE;
	m_earlylatetimelog.clear();
}

CTgEditView::~CTgEditView()
{
	if(m_pTrainSideDlg)
		delete m_pTrainSideDlg;
	m_pTrainSideDlg=NULL;

	if(m_pTrainRouteDlg)
		delete m_pTrainRouteDlg;
	m_pTrainRouteDlg=NULL;

	if(m_pWorkSchdCompareDlg)
		delete m_pWorkSchdCompareDlg;
	m_pWorkSchdCompareDlg=NULL;
	
	if(m_pShiftSchdCompareDlg)
		delete m_pShiftSchdCompareDlg;
	m_pShiftSchdCompareDlg=NULL;

	if(m_pBasicSchdCompareDlg)
		delete m_pBasicSchdCompareDlg;
	m_pBasicSchdCompareDlg=NULL;
}


void CTgEditView::OnDestroy() 
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage);

	if(m_pNotepadDlg)  // �׶μ��ºͽ׶μƻ��ϲ�Ϊһ������
	{
		delete m_pNotepadDlg;
		m_pNotepadDlg=NULL;
		m_pSendPlanDlg=NULL;  
	}
	if(m_pSendPlanDlg)
	{
		delete m_pSendPlanDlg;
		m_pSendPlanDlg=NULL;
		GetDocument()->GetSTPLAN()->SetPlanMMIWindow(NULL);
	}
	if(m_predictDlg)
	{
		delete m_predictDlg;
		m_predictDlg=NULL;
	}

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // ֻ���ڵ���ͼ�в���Ҫ�����Ի���
	{
		m_DlgArms.DestroyWindow(); 
	}
}
void CTgEditView::OnInitialUpdate()
{
	CTgView::OnInitialUpdate();
}

BOOL  CTgEditView::InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode)
{
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{	
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgParamentReply,0);
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgParamentOperation,0);

		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgChangeSideReq,0);
		if(GetTGFunctionConfig()->IsEnableScadaElecArm())
			RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgElecArmSet,0);
		
		ReqParament();
	}
	if(!CTgView::InitScheduleView(type,datapath,query_mode))
		return FALSE;

	if(GetTGFunctionConfig()->IsGoodTrainNotTechStopNeedPwd())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::GOOD_TRAIN_STAY_TO_PASS_NEED_PWD);
	
	// �����������ý�ֹ�޸�ʵ�ʵ�
	if(gpcdef.IsInhibitManualConvertActualTime())
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 1);
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 1);
	}
	else
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 0);
	}

	// ����TG.OCX �е�Ȩ�ޣ���TView��ת�Ƴ���
	LoadEarlylatetimeXML();

	RIGHT right;
	int gpcid = gpcdef.GetUserGpcId();
	if(!m_load.GetUserRight(gpcid, right))
	{
		return FALSE;
	}

	if(!CheckUserTGRight())
	{
		right.train=0;
		right.mark=0;
		right.app=0;
		right.operate=0;
	}
	mTG.SetLineCtrlWord(right.train);
	mTG.SetMarkCtrlWord(right.mark);
	mTG.SetAppCtrlWord(right.app);
	mTG.SetOperateRight(right.operate);

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // ֻ���ڵ���ͼ�в���Ҫ�����Ի���
	{
		if(GetTGFunctionConfig()->IsSupportInterCityStPlan())
		{
			if(IsSendPlanToStation(right.wintg_operate))	
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::INTER_CITY_STPLAN, 1);
			else
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::INTER_CITY_STPLAN, 0);
		}
		m_pNotepadDlg=new CPropertySheetNotepad("�׶μƻ��´�",GetDocument(),m_schdinfo.m_nType);
		m_pNotepadDlg->m_bEnableStPlanWaring=TRUE;
		m_pNotepadDlg->m_bModaless=TRUE;
		m_pNotepadDlg->AddUserPage(STPLAN_PAGE); 
		m_pNotepadDlg->AddUserPage(PHASENOTE_SEND_PAGE); 
		m_pNotepadDlg->Create(this,WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_CONTEXTHELP);

		m_pSendPlanDlg=(CSendStationPlanDlgEx *)m_pNotepadDlg->GetUserPage(STPLAN_PAGE);
		GetDocument()->GetSTPLAN()->SetTgMMI(&m_tgmmi); 
		GetDocument()->GetSTPLAN()->SetPlanMMIWindow(m_pSendPlanDlg->GetSafeHwnd());   

		m_DlgArms.Create(CDlgElecArms::IDD, this);

		if(gpcdef.IsEnablePredict())
		{
			m_predictDlg=new CDepartPredictDlg();
			m_predictDlg->SetPredictInfo(GetDocument(), m_schdinfo.m_nShiftID, m_schdinfo.m_nSectionID);
			m_predictDlg->Create(CDepartPredictDlg::IDD, this);

			m_pPredictServer = GetDocument()->GetTrainDepartPredictHold();
			SetTimer(198311, 60*1000, NULL);
		}
		// 2018.7.23��ָ���õ�tgmmi��
		m_tgmmi.SetPredictHolder(m_pPredictServer);

		int bstation_list[129];
		int count=gpcdef.GetBasicPlanStation(m_schdinfo.m_nSectionID, bstation_list);
		for(int n=0; n<count; n++)
		{
			mTG.AddBStplanStation(bstation_list[n]);
		}
	}

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) // ��ȡ����
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) // ��ȡ����
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // ��ȡ������Ϣ
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);

	m_bEnableFreeAddTrain=GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_FREE_ADDTRAIN)>0?TRUE:FALSE;
	m_tgmmi.EnableFreeTrainOper(m_bEnableFreeAddTrain);
	if(m_bEnableFreeAddTrain)
		mTG.SetContrlRight(0);
	else
		mTG.SetContrlRight(1);

	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_MOVE_SAME_CZ_AFTER_TRAIN)>0)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN, 0);

	if(GetTGFunctionConfig()->IsDisenableSheetTab())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISENABLE_SHEET_TAB, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISENABLE_SHEET_TAB, 0);
	
	if(!IS_BASIC_SCHD(GetViewScheduleType()))
	{
		int stationlist[32];
		int nNum=GetTGFunctionConfig()->GetNotThisSectionStation(m_schdinfo.m_nSectionID, stationlist, sizeof(stationlist)/sizeof(stationlist[0]));
		for(int i=0; i<nNum; i++)
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_NOT_THIS_SECTION_STATION,  stationlist[i]);
	}

	if(!IS_BASIC_SCHD(GetViewScheduleType()) && GetTGFunctionConfig()->HasNoLimitStation())
	{
		int stationlist[64];
		int nNum=GetTGFunctionConfig()->GetNoLimitStationList(m_schdinfo.m_nSectionID, stationlist, sizeof(stationlist)/sizeof(stationlist[0]));
		for(int i=0; i<nNum; i++)
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_NO_LIMIT_STATION, stationlist[i]);
	}
	
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
		m_pTrainSideDlg=new CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV, this);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		m_pTrainSideDlg=new CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT, this);
	else
		m_pTrainSideDlg=NULL;

	if(m_pTrainSideDlg)
	{
		m_pTrainSideDlg->Create(CDlgTrainRecord::IDD, this);
	}

	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
		m_pTrainRouteDlg=new CDlgTrainRouteCheck(this);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		m_pTrainRouteDlg=new CDlgTrainRouteCheck(this);
	else
		m_pTrainRouteDlg=NULL;

	if(m_pTrainRouteDlg)
	{
		m_pTrainRouteDlg->Create(CDlgTrainRouteCheck::IDD, this);
	}

	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		m_pBasicSchdCompareDlg=new CDlgBasicSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pBasicSchdCompareDlg)
		{
			m_pBasicSchdCompareDlg->Create(CDlgBasicSchduleCompare::IDD, this);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		m_pShiftSchdCompareDlg=new CDlgShiftSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pShiftSchdCompareDlg)
		{
			m_pShiftSchdCompareDlg->Create(CDlgShiftSchduleCompare::IDD, this);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		m_pWorkSchdCompareDlg=new CDlgWorkSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pWorkSchdCompareDlg)
		{
			m_pWorkSchdCompareDlg->Create(CDlgWorkSchduleCompare::IDD, this);
		}
	}


	// ���������ɵ�
	if(GetTGFunctionConfig()->IsEnableLockWholeTrainSide())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE, 0);

	// �������ͼ,��ֹɾ��������
	if(GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::NOT_DELETE_REAL_TIME, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::NOT_DELETE_REAL_TIME, 0);

	return TRUE;
}
// CTgEditView ���
#ifdef _DEBUG
void CTgEditView::AssertValid() const
{
	CView::AssertValid();
}

void CTgEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

// �������¼���Ӧ
void CTgEditView::APPEND_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_ADD,nIndex,xml);
}

void CTgEditView::UPDATE_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_UPDATE,nIndex,xml);
}

void CTgEditView::DELETE_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_DELETE,nIndex,xml);
}

void CTgEditView::APPEND_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_ADD,nIndex,xml);
}

void CTgEditView::UPDATE_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_UPDATE,nIndex,xml);
}

void CTgEditView::TG_DELETE_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_DELETE,nIndex,xml);
}
void CTgEditView::SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time2)
{
	TIME curtm=time2;

	TIME cctm=time(NULL);
	if(m_currentOper.nOperStatus==OPER_STATUS_PASSWAY)
	{
		if(IS_BASIC_SCHD(GetViewScheduleType()))
		{
			MessageBox("����ͼ��֧�ִ˲���","��ʾ");
			m_currentOper.nOperStatus=OPER_STATUS_IDLE;
			return;
		}

		if(GetTGFunctionConfig()->IsEnableInputEngineId())
		{
			CDWordArray  aryStatus;
			CStringArray aryEngineId;
			aryEngineId.RemoveAll();
			for(int i=0; i<m_currentOper.nEngineCount; i++)
			{
				aryEngineId.Add(m_currentOper.engineId[i]);
				aryStatus.Add(m_currentOper.status[i]);
			}
			m_tgmmi.AddTrain_Passway(m_currentOper.bSelSecret, m_currentOper.szTrainNo,aryEngineId, aryStatus, m_currentOper.nRouteNo,m_currentOper.nStartMode,m_currentOper.nEndMode,curtm,m_currentOper.nRunType);		
		}
		else
			m_tgmmi.AddTrain_Passway(m_currentOper.bSelSecret, m_currentOper.szTrainNo,m_currentOper.szArriveTrainNo, m_currentOper.nRouteNo,m_currentOper.nStartMode,m_currentOper.nEndMode,curtm,m_currentOper.nRunType);		
		m_currentOper.nOperStatus=OPER_STATUS_IDLE;
	}
	else
	{
		CTgView::SELECT_TRAIN(train_index,nFlag,stano1,stano2,date,time2);
	}
}
void CTgEditView::SELECT_MARK(long mark_index, char type, char flag, short  stano1, short  stano2,long param)
{
	CTgView::SELECT_MARK(mark_index,type,flag,stano1,stano2,param);
}

void CTgEditView::TG_LOG(LPCTSTR log)
{
	if(log)
	{
		GpcLogTrainMessage(0, 0, "TG�ؼ�������־: %s", log);
	}
}

void CTgEditView::TG_NOTICE(BYTE type, long nFlag, long nIndex,short station)
{
	CTgView::TG_NOTICE(type,nFlag,nIndex,station);
	if(type == 1)
	{
		long flag = nFlag & 0xFF;
		if(flag & 0x08)  // ���õ���
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
			if(!pData)
				return;
			CTG_SingleMark *pselectedMark=pData->GetSingleMark(nIndex);
			if(!pselectedMark)
				return;
			m_tgmmi.AdjustTrainsForMark(pselectedMark);
		}
	}
	else if(type == 2)
	{
		long flag = nFlag & 0xFF;
		int rec_index = ((nFlag & 0xFFFFFF00) >> 24);
		if(flag & 0x4)
		{
			OnChangeTrainRoute();
		}
		if(flag & 0x8)
		{
			OnDivideportLatetime();
		}
	}
	else if(type == 3)  // ����
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;
		CTG_SingleTrain *pselectedTrain=pData->GetSingleTrain(nIndex);
		if(!pselectedTrain)
			return;
		CTG_SingleTrain train=*pselectedTrain;
		int rec_index=(nFlag>>24)&0xFF;
		int cmd_index=nFlag & 0xFF;
		if(cmd_index==1) // ��ͼ����
		{
			m_tgmmi.AdjustTrainForBasicSchedule(train,rec_index);
		}
		else if(cmd_index==2)  // ���õ���
		{

			m_tgmmi.AdjustTrainForCollision(train,rec_index);
		}
		else if(cmd_index==3) // ��㲻���õ���
		{
			m_tgmmi.AdjustTrainForMutiPass(train,rec_index);
		}
	}
	else if(type == 4)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;
		CTG_SingleTrain *pselectedTrain=pData->GetSingleTrain(nIndex);
		if(!pselectedTrain)
			return;
		CTG_SingleTrain train=*pselectedTrain;
		int rec_index=(nFlag>>24)&0xFF;
		int cmd_index=nFlag & 0xFF;
		if(cmd_index==1) // ���ý�·�Դ�
		{
			DirectSetAutotrig(train,rec_index);
		}
	}
	else if(type == 5)
	{
		BatchManualModifySide(nIndex);
	}
}


void CTgEditView::TG_FIRE_DELETE(LPCTSTR xml)
{
	std::vector<ULONG> list_index;
	CStdioFile file;
	if(file.Open("c:\\delete.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		file.WriteString(xml);
		file.Close();
	}
	int showdlg=0;
	int nFlag = fromDeleteXml(showdlg,list_index, xml);
	if(list_index.size() == 0)
		return;


	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(NULL == pData)
		return;

	// TRAIN
	if(nFlag == 1)
	{
		CTG_SingleTrain *ptrain=NULL;

		if(list_index.size() == 1)
		{
			ptrain = pData->GetSingleTrain(list_index[0]);
			if(ptrain==0)
			{
				return;
			}
			//������ɾ����ƻ��г�
			if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType()) && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
			{
				return;
			}
			
			if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
			{
				if (ptrain->HasActualRecord()>=0)
				{
					CString text;
					text.Format("�г� %s  �д���ʵ�ʵ�,���ý���ɾ������!", ptrain->GetTrainID());
					AfxMessageBox(text);
					return;
				}
			}

			CString cap;
			CMMIOperFlag operflag;
			cap.Format("ȷ��ɾ������ %s ��", ptrain->GetTrainID());  
			if(MessageBox(cap,"����",MB_YESNO)==IDYES)
			{
				GpcLogTrainMessage(0, 0, "���β��� %s ɾ��",ptrain->GetTrainID());
				m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);	
			}
		}
		else
		{
			CDlgMultiSelOperate dlg(list_index, this);
			dlg.m_pSchdBase = pData;
			if(dlg.DoModal() == IDOK)
			{
				for(int i=0; i<dlg.sel.size(); i++)
				{
					ptrain = pData->GetSingleTrain(dlg.sel[i]);
					if(ptrain == NULL)
						continue;

					//������ɾ����ƻ��г�
					if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType()) && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
					{
						continue;
					}
					
					if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
					{
						if (ptrain->HasActualRecord()>=0)
						{
							CString text;
							text.Format("�г� %s  �д���ʵ�ʵ�,���ý���ɾ������!", ptrain->GetTrainID());
							AfxMessageBox(text);
							continue;
						}
					}

					GpcLogTrainMessage(0, 0, "���β��� %s ɾ��",ptrain->GetTrainID());
					CMMIOperFlag operflag;
					m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);		
				}
			}
		}	
	}

	// BLOCK
	if(nFlag == 2)
	{
		CTG_SingleMark *pmark=NULL;

		if(list_index.size() == 1)
		{
			pmark = pData->GetSingleMark(list_index[0]);
			if(pmark==0)
			{
				return;
			}
			CString str;
			int m_type = pmark->GetMarkType();
			switch(m_type)
			{
			case ABS_SLOW_MARK:
				str.Format ("��������[%s]?", pmark->m_data.text);	
				break;
			case SELF_CLOSE_MARK:
				str.Format ("����������[%s]?", pmark->m_data.text);
				break;
			case ABS_INHIBIT_MARK:
				str.Format ("�������[%s]?", pmark->m_data.text);
				break;
			case ABS_FIX_MARK:
				str.Format ("�����ۺ��촰��[%s]?", pmark->m_data.text);
				break;
			case ABS_ELE_FIX_MARK:
				str.Format ("�����������[%s]?", pmark->m_data.text);
				break;
			case NEW_ABS_ELE_FIX_MARK:
				str.Format ("�����������[%s]?", pmark->m_data.text);
				break;
			case SIDE_SLOW_MARK:
				str.Format ("վ������[%s]?", pmark->m_data.text);
				break;
			case SIDE_INHIBIT_MARK:
				str.Format ("�ɵ�����[%s]?", pmark->m_data.text);
				break;
			case SIDE_TIANCHUANG_MARK:
				str.Format ("�ɵ��촰��[%s]?", pmark->m_data.text);
				break;
			case GRAPH_NOTE_MARK:
				str.Format ("ͼ��ע��[%s]?", pmark->m_data.text);
				break;
			case SIDE_OCCUPY_MARK:
				str.Format("�ɵ�ռ��[%s]?", pmark->m_data.text);
				break;
			case NEW_GRAPH_NOTE_MARK:
				str.Format ("ͼ��ע��[%s]?", pmark->m_data.text);
				break;

			default:
				str.Format ("��ʩ������?");
				break;
			}
			str = "��ȷ��ɾ�� " + str;
			if(showdlg==0)
			{
				if(MessageBox(str, "����", MB_YESNO|MB_ICONWARNING) != IDYES)
					return; 
			}
			CString log;
			log.Format("�û�ɾ��ʩ������ index %d gpc_index %d: ", pmark->GetBlockIndex(), pmark->GetGPCIndex());
			GpcLogTrainMessage(0, 0, log);
			pmark->m_data.text[255]=0;
			log.Format("%s",  pmark->m_data.text);
			GpcLogTrainMessage(0, 0, log);

			CMMIOperFlag operflag;
			m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*pmark,pmark->GetBlockIndex());	
		}
	}
}
////////////////////////////////////// ����������ͼ�Ĳ���
// �ƻ���

void CTgEditView::OnEndInsertNewTrain()
{
	mTG.EndNewSingleTrain();
}

void CTgEditView::OnDelMultiTrain()
{
	mTG.DeleteMutiTrain();
}

// ���
void CTgEditView::OnAbsInhibitG()
{
	mTG.NewMark(MARK::IF_ABS_INHIBIT);
}

void CTgEditView::OnAbsSlowG()
{
	mTG.NewMark(MARK::IF_ABS_SLOW);
}

void CTgEditView::OnAbsFixG()
{
	mTG.NewMark(MARK::IF_ABS_TOTAL_FIX);
}

void CTgEditView::OnAbsEleFixG()
{
	mTG.NewMark(MARK::IF_ABS_ELEM_FIX);
}

void CTgEditView::OnSelfCloseG()
{
	mTG.NewMark(MARK::IF_ABS_SELF_CLOSE);
}

void CTgEditView::OnSideInhibitG()
{
	mTG.NewMark(MARK::STATION_INHIBIT);
}
void CTgEditView::OnStationSlowG()
{
	mTG.NewMark(MARK::STATION_SLOW);
}
void CTgEditView::OnSideTianchuangG()
{
	mTG.NewMark(MARK::STATION_FIX);
}
void CTgEditView::OnSideOccupy()
{
	mTG.NewMark(MARK::STATION_SIDE_OCCUPY);
}
void CTgEditView::OnSideLostEle()
{
	mTG.NewMark(MARK::STATION_SIDE_LOST_ELE);
}
void CTgEditView::OnTextNote()
{
	mTG.NewMark(MARK::TEXT_NOTE);
}
void CTgEditView::OnJishiZhujie()
{
	mTG.NewMark(MARK::PE_NOTE);
}

// �ر��г�
void CTgEditView::OnDrawGoon()
{
	mTG.DrawSpecialTrain(TRAIN::GOON);
}

void CTgEditView::OnDrawReturn()
{
	mTG.DrawSpecialTrain(TRAIN::RETURN);
}

void CTgEditView::OnDrawAbstrainA()
{
	mTG.DrawSpecialTrain(TRAIN::TO_ABS_TRAIN);
}

void CTgEditView::OnDrawAbstrainB()
{
	mTG.DrawSpecialTrain(TRAIN::FROM_ABS_TRAIN);
}

// ��������
void CTgEditView::OnUniteTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::UNITE_TRAIN, 1);
}

void CTgEditView::OnRescueTrain()
{
	//���µľ�Ԯ�г�ȡ�� �人�����
	//mTG.TrainsOperator(TWO_TRAIN_OPERATOR::RESCUE_TRAIN, 1);
}

void CTgEditView::OnSplitTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::SPLIT_TRAIN, 1);
}

void CTgEditView::OnCombineTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::COMBINE_TRAIN, 1);
}

void CTgEditView::OnAbsPartRun()
{
	if(MessageBox("��ȷʵҪ�޸��г�����ֲ�������?", "��ʾ", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::IN_ABS_PART_RUN, 1);
	}
}
	
void CTgEditView::OnAbsWholeHelpRun()
{
	if(MessageBox("��ȷʵҪ�޸��г�����������Ԯ��?", "��ʾ", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::IN_ABS_WHOLE_HELP_RUN, 1);
	}
}

void CTgEditView::OnAttainEngineReturn()
{
	if(MessageBox("��ȷʵҪ�޸��г�����;���۷���?", "��ʾ", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::ATT_ENGINE_RETURN, 1);
	}
}

void CTgEditView::OnDeleteTrain()
{
	if(!CheckUserTGRight()) 
	{
		return;
	}
	
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
		if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType())&&ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)//������ɾ����ƻ��г�
		{
			return;
		}

		if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
		{
			if (ptrain->HasActualRecord()>=0)
			{
				AfxMessageBox("�������д���ʵ�ʵ�,���ý���ɾ������!");
				return;
			}
		}

		CString cap;
		CMMIOperFlag operflag;
		cap.Format("ȷ��ɾ������ %s ��",ptrain->GetTrainID());  
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			GpcLogTrainMessage(0, 0, "���β��� %s,���ɾ��",ptrain->GetTrainID());
			m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);		
		}
	}
	else
		if(pobj->GetObjectType()==TG_OBJ_MARK)
		{
			CTG_SingleMark *pmark=(CTG_SingleMark *)pobj;
			CString cap;
			CMMIOperFlag operflag;
			cap.Format("ȷ��ɾ������� %s ��",pmark->GetMarkInfo(MARK_TXT_INFO_TYPE_INSIDE));  
			if(MessageBox(cap,"����",MB_YESNO)==IDYES)
			{
				m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*pmark,pmark->GetBlockIndex());		
			}
			GpcLogTrainMessage(0, 0, "��ǲ��� %d,���ɾ��",pmark->GetBlockIndex());
		}
}

void CTgEditView::OnAddTmpTrain()
{
	// G12
	if(!CheckUserTGRight())
		return;

	if(IS_BASIC_SCHD(GetViewScheduleType()))
	{
		MessageBox("����ͼ��֧�ִ˲���","��ʾ");
		return;
	}

	CTempAddTrains* dlg = new CTempAddTrains(GetDocument(), m_schdinfo.m_nSectionID, this);
	if(dlg != NULL)
	{
		dlg->Create(IDD_TEMP_ADD_TRAINS, this);
		dlg->ShowWindow(SW_SHOW);
	}
}

void  CTgEditView::OnAddTmpTrainPassway(TmpTrainInfo* info)
{
	// G12
	if(!GetTGFunctionConfig()->IsEnableTempAddTrain())
		return;
	if(!CheckUserTGRight()) 
		return;
	if(IS_BASIC_SCHD(GetViewScheduleType()))
	{
		MessageBox("����ͼ��֧�ְ��վ�·ʱ�ӳ�","��ʾ");
		return;
	}
	if(info == NULL)
		return;

	CString trainId = info->trainid; 
	CStringArray aryEngineId;
	CDWordArray aryEngineStatus;
	ULONG beginTm = info->beginTime;
	BYTE nStartMode = info->start_mode;
	BYTE nEndMode = info->end_mode;
	BYTE nStartStaIndex = info->nBeginStaIndex;
	BYTE nEndStaIndex = info->nEndStaIndex;
	BYTE nRunType = info->nRunType;
	int nPasswayNo = info->nSelPasswayId;
	for(int i=0; i<info->nEngineCount&&i<5; i++)
	{
		aryEngineId.Add(info->engineId[i]);
		aryEngineStatus.Add(info->status[i]);
	}

	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(!pData)
		return;

    // �ж��Ƿ�ӳ�
	if(!IsAddTrain(trainId))
	{
		return;
	}

	m_tgmmi.AddTrainTmp_Passway(info->bSecret, aryEngineId, aryEngineStatus, trainId, nPasswayNo, nStartMode, nEndMode, nStartStaIndex, nEndStaIndex, beginTm, nRunType);
	GpcLogTrainMessage(0, 0, "����·��ʱ�ӳ�, train=%s bt=%d", trainId, beginTm);
	GpcLogTrainMessage(0, 0, "����·��ʱ�ӳ�, pasway=%d bs=%d es=%d",nPasswayNo,nStartStaIndex,nEndStaIndex);
	GpcLogTrainMessage(0, 0, "����·��ʱ�ӳ�, StartMode=%d EndMode=%d",nStartMode,nEndMode);

	return;
}


void CTgEditView::OnInsertNewTrain() 
{
	if(!CheckUserTGRight()) 
	{
		return;
	}

	if(GetTGFunctionConfig()->IsEnableInputEngineId())
		InsertNewTrainWithEngineNo();
	else
		InsertNewTrain();
	return;
}

void CTgEditView::InsertNewTrainWithEngineNo()
{
	CInsertTrainLanzhou dlg(GetDocument(), m_schdinfo.m_nSectionID);
	if(m_bEnableFreeAddTrain || (GetViewScheduleType() == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV))
		dlg.m_bAutoSelectShiftAddTrain=FALSE;
	else
		dlg.m_bAutoSelectShiftAddTrain=TRUE;

	if(dlg.DoModal()==IDOK)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;

		// �ж��Ƿ��ظ��ӳ�
		if(!IsAddTrain(dlg.m_strInsertTrainId))
		{
			return;
		}
				
		// 20091011 XIAN ��������ӳ�
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
			&& !dlg.m_bAddShift)  
		{
			BYTE run_prio=255,train_type=255,run_type=255;
			BOOL can_free_add=TRUE;
			if(GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId,&train_type,&run_type,&run_prio))
			{
				if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(train_type))
					can_free_add=FALSE;
			}

			if(!can_free_add)
			{
				MessageBox("��ʹ���հ�ƻ���ʽ�ӳ�","��ʾ");
				return;
			}
		}
		/////////////////////////////////////////////////
		// ͼ���ӳ�
		if(dlg.m_bAddGraphic == TRUE)  
		{
			if(IS_BASIC_SCHD(GetViewScheduleType()))
			{
				MessageBox("����ͼ��֧�ִ˲���","��ʾ");
				return;
			}

			m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId, dlg.m_aryEngineId, dlg.m_aryStatus);
			GpcLogTrainMessage(0, 0, "������ͼ�ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
		}
		// �հ�ƻ��ӳ�
		else if(dlg.m_bAddShift == TRUE)  
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
			BOOL  oper_rc=FALSE;
			if(pData)
			{
				TIME bm,em;
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_ScheduleInfo info;
				if(pHolder==NULL)
					return;
				pHolder->GetShiftScheduleInfo(info);
				info.GetShiftManagerRange(bm,em);

				CTG_TrainPtrGroup prev_group;
				pData->FindTrains(prev_group,NULL_STATION,bm, em,FALSE,TRUE);
				for(int idx=0;idx<prev_group.GetTrainCount();idx++)
				{
					CTG_SingleTrain *prev_tmp=prev_group.GetTrainPtr(idx);
					if(!prev_tmp->MatchTrainID(dlg.m_strInsertTrainId)) // ԭ����ʽ��ӵĳ���
						continue;

					oper_rc=m_tgmmi.AddTrain_ShiftSchedule(dlg.m_bSecret, prev_tmp->GetTrainIndex(), dlg.m_aryEngineId, dlg.m_aryStatus, NULL_STATION,bm,em);
					if(oper_rc)
					{
						GpcLogTrainMessage(0, 0, "���հ�ƻ��ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
					}
				}
			}
			if(oper_rc==FALSE)
				MessageBox("�Ҳ���ָ������");
		}
		// ���ɻ���
		else if(dlg.m_nCurSel == 0)  
		{
			BYTE train_type,run_type,prio;
			if(!GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId, &train_type, &run_type, &prio))
				train_type=255;
			CString xml, tmp;
			xml = "<NEWTRAIN>";
			tmp.Format(" <TRAINID>%s</TRAINID>", dlg.m_strInsertTrainId);
			xml+=tmp;
			tmp.Format(" <ARLTRAINID>%s</ARLTRAINID>", dlg.m_strArrivalTrainId);
			xml+=tmp;
			tmp.Format(" <START>%d</START>", dlg.m_nStartMode);
			xml+=tmp;
			tmp.Format(" <END>%d</END>", dlg.m_nEndMode);
			xml+=tmp;
			tmp.Format(" <SECRET>%d</SECRET>", dlg.m_bSecret);
			xml+=tmp;
			tmp.Format(" <RUN_TYPE>%d</RUN_TYPE>", dlg.m_nRunType);
			xml+=tmp;
			tmp.Format(" <TRAIN_TYPE>%d</TRAIN_TYPE>", train_type);
			xml+=tmp;
			xml+="<ENGINE>";
			for(int i=0; i<dlg.m_aryEngineId.GetCount(); i++)
			{
				tmp.Format("<ID id=\"%s\" status=\"%d\"/>", 
					dlg.m_aryEngineId.GetAt(i),
					dlg.m_aryStatus.GetAt(i));
				xml+=tmp;
			}
			xml+="</ENGINE>";
			xml += "</NEWTRAIN>";
			mTG.BeginNewSingleTrainXml(xml);

			GpcLogTrainMessage(0, 0, "���ɻ������ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
		}
		// ���վ�·�ӳ�
		else
		{
			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			if(pholder)
			{
				m_currentOper.nOperStatus=OPER_STATUS_PASSWAY;
				memset(m_currentOper.szTrainNo, 0, sizeof(m_currentOper.szTrainNo));
				TG_CopyTrainId(m_currentOper.szTrainNo,dlg.m_strInsertTrainId);

				m_currentOper.nRouteNo=dlg.m_nRouteNo;
				m_currentOper.nStartMode=dlg.m_nStartMode;
				m_currentOper.nEndMode=dlg.m_nEndMode;
				m_currentOper.nEngineCount=0;
				memset(m_currentOper.engineId, 0, sizeof(m_currentOper.engineId));
				m_currentOper.bSelSecret=dlg.m_bSecret;
				m_currentOper.nRunType=dlg.m_nRunType;
				for(int i=0; i<dlg.m_aryEngineId.GetCount()&&i<5; i++)
				{
					strncpy(m_currentOper.engineId[i],  dlg.m_aryEngineId.GetAt(i), sizeof(m_currentOper.engineId[i])-1);
					m_currentOper.status[i] = dlg.m_aryStatus.GetAt(i);
					m_currentOper.nEngineCount++;
				}
			}
			GpcLogTrainMessage(0, 0, "����·���ӵ�һ����, train=%s route=%d abs_run_type=%d",dlg.m_strInsertTrainId,dlg.m_nRouteNo,m_currentOper.nRunType);
		}
	}
}

void CTgEditView::InsertNewTrain()
{
	CInsertTrain dlg(m_schdinfo.m_nSectionID);
	if(m_bEnableFreeAddTrain || (GetViewScheduleType() == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV))
		dlg.m_bAutoSelectShiftAddTrain=FALSE;
	else
		dlg.m_bAutoSelectShiftAddTrain=TRUE;

	if(dlg.DoModal()==IDOK)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;

		// �ж��Ƿ�ӳ�
		if(!IsAddTrain(dlg.m_strInsertTrainId))
		{
			return;
		}
		
		// 20091011 XIAN ��������ӳ�
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
			&& !dlg.m_bAddShift)  
		{
			BYTE run_prio=255,train_type=255,run_type=255;
			BOOL can_free_add=TRUE;
			if(GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId,&train_type,&run_type,&run_prio))
			{
				if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(train_type))
					can_free_add=FALSE;
			}

			if(!can_free_add)
			{
				MessageBox("��ʹ���հ�ƻ���ʽ�ӳ�","��ʾ");
				return;
			}
		}
		/////////////////////////////////////////////////

		if(dlg.m_bAddGraphic == TRUE)  // ͼ���ӳ�
		{
			if(IS_BASIC_SCHD(GetViewScheduleType()))
			{
				MessageBox("����ͼ��֧�ִ˲���","��ʾ");
				return;
			}

			if(m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId,""))
			{
				GpcLogTrainMessage(0, 0, "������ͼ�ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
			}
		}
		else if(dlg.m_bAddShift == TRUE)  // �հ�ƻ��ӳ�
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
			BOOL  oper_rc=FALSE;
			if(pData)
			{
				TIME bm,em;
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_ScheduleInfo info;
				if(pHolder==NULL)
					return;
				pHolder->GetShiftScheduleInfo(info);
				info.GetShiftManagerRange(bm,em);
				em += 6*3600;

				CTG_TrainPtrGroup prev_group;
				pData->FindTrains(prev_group,NULL_STATION,bm, em,FALSE,TRUE);
				for(int idx=0;idx<prev_group.GetTrainCount();idx++)
				{
					CTG_SingleTrain *prev_tmp=prev_group.GetTrainPtr(idx);
					if(!prev_tmp->MatchTrainID(dlg.m_strInsertTrainId)) // ԭ����ʽ��ӵĳ���
						continue;
					oper_rc=m_tgmmi.AddTrain_ShiftSchedule(dlg.m_bSecret, prev_tmp->GetTrainIndex(),NULL_STATION,bm,em);
					if(oper_rc)
					{
						GpcLogTrainMessage(0, 0, "���հ�ƻ��ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
					}
				}
			}
			if(oper_rc==FALSE)
			{
				if(GetTGFunctionConfig()->IsFreedrawAddTrainShiftAndBasic())
				{
					if(IS_BASIC_SCHD(GetViewScheduleType()))
					{
						MessageBox("����ͼ��֧�ִ˲���","��ʾ");
						return;
					}

					if(m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId,""))
					{
						GpcLogTrainMessage(0, 0, "�հ�ƻ���û�иó�, ������ͼ�ӵ�һ����, train=%s",dlg.m_strInsertTrainId);
					}
					else
					{
						CString text;
						text.Format("�հ�ƻ�û�� ����ͼ��Ҳû�г��� %s, �ӳ�ʧ��", dlg.m_strInsertTrainId);
						AfxMessageBox(text);
					}
				}
				else
				{
					CString text;
					text.Format("�հ�ƻ���û�г��� %s, �����հ�ƻ��ӳ�ʧ��", dlg.m_strInsertTrainId);
					AfxMessageBox(text);
				}
			}
		}
		// ���ɻ���
		else if(dlg.m_nCurSel == 0)
		{
			//int nStartMode = dlg.m_nStartMode;
			//int nEndMode = dlg.m_nEndMode;
			//if(dlg.m_bSecret)
			//	nStartMode |= 0x80;
			//mTG.BeginNewSingleTrain(dlg.m_strInsertTrainId,dlg.m_strArrivalTrainId,nStartMode,nEndMode);
			BYTE train_type,run_type,prio;
			if(!GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId, &train_type, &run_type, &prio))
				train_type=255;

			CString xml, tmp;
			xml = "<NEWTRAIN>";
			tmp.Format(" <TRAINID>%s</TRAINID>", dlg.m_strInsertTrainId);
			xml+=tmp;
			tmp.Format(" <ARLTRAINID>%s</ARLTRAINID>", dlg.m_strArrivalTrainId);
			xml+=tmp;
			tmp.Format(" <START>%d</START>", dlg.m_nStartMode);
			xml+=tmp;
			tmp.Format(" <END>%d</END>", dlg.m_nEndMode);
			xml+=tmp;
			tmp.Format(" <SECRET>%d</SECRET>", dlg.m_bSecret);
			xml+=tmp;
			tmp.Format(" <RUN_TYPE>%d</RUN_TYPE>", dlg.m_nRunType);
			xml+=tmp;
			tmp.Format(" <TRAIN_TYPE>%d</TRAIN_TYPE>", train_type);
			xml+=tmp;
			xml += "</NEWTRAIN>";
			mTG.BeginNewSingleTrainXml(xml);

			GpcLogTrainMessage(0, 0, "���ɻ������ӵ�һ����, train=%s xml=%s",dlg.m_strInsertTrainId, xml);
		}
		// ���վ�·�ӳ�
		else
		{
			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			if(pholder)
			{
				m_currentOper.nOperStatus=OPER_STATUS_PASSWAY;
				memset(m_currentOper.szTrainNo, 0, sizeof(m_currentOper.szTrainNo));
				TG_CopyTrainId(m_currentOper.szTrainNo,dlg.m_strInsertTrainId);

				memset(m_currentOper.szArriveTrainNo, 0, sizeof(m_currentOper.szArriveTrainNo));
				TG_CopyTrainId(m_currentOper.szArriveTrainNo,dlg.m_strArrivalTrainId);

				m_currentOper.nRouteNo=dlg.m_nRouteNo;
				m_currentOper.nStartMode=dlg.m_nStartMode;
				m_currentOper.nEndMode=dlg.m_nEndMode;
				m_currentOper.nEngineCount=0;
				memset(m_currentOper.engineId, 0, sizeof(m_currentOper.engineId));
				m_currentOper.bSelSecret=dlg.m_bSecret;
				m_currentOper.nRunType=dlg.m_nRunType;
				GpcLogTrainMessage(0, 0, "����·���ӵ�һ����, train=%s route=%d abs_run_type=%d",dlg.m_strInsertTrainId,dlg.m_nRouteNo,m_currentOper.nRunType);
			}
		}
	}
}

void CTgEditView::OnManualSelectTrain()
{
	if(1 != GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_ADD_TRAIN_BY_BASIC))
		OnManualSelectShiftTrain();
	else
		OnManualSelectBasicTrain();
}

void CTgEditView::OnManualSelectBasicTrain() 
{//ѡ��ͼ���г�
	if(!m_bEnableFreeAddTrain)
	{
		MessageBox("��ʹ���հ�ƻ���ʽ�ӳ�","��ʾ");
		return;
	}
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleTrain dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	dlg.m_bEnableChoiceSchedule=TRUE;

	if(dlg.DoModal () == IDOK)
	{
		OnRecievedDataEnd(0,1);
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(dlg.info[i].exist_train==TGSelectedBasicTrain::BT_NONE || dlg.m_check)
			{
				if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
					m_tgmmi.AddTrain_BasicSchedule(dlg.info[i].train_index);
				else if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
					m_tgmmi.AddTrain_ShiftSchedule(false, dlg.info[i].train_index);
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

void CTgEditView::OnManualSelectShiftTrain() 
{//ѡ���հ�ƻ��г�
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleTrain dlg(pholder);

	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	if(!m_bEnableFreeAddTrain)
		dlg.m_bEnableChoiceSchedule=FALSE;
	else
		dlg.m_bEnableChoiceSchedule=TRUE;

	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		MessageBox("�������հ�ƻ���ʹ�øù���!","����");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		OnRecievedDataEnd(0,1);
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(dlg.info[i].exist_train==TGSelectedBasicTrain::BT_NONE || dlg.m_check)
			{
				if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
					m_tgmmi.AddTrain_BasicSchedule(dlg.info[i].train_index);
				else if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
					m_tgmmi.AddTrain_ShiftSchedule(false, dlg.info[i].train_index);
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

void CTgEditView::SingleTrainToWork()
{
	if(m_schdinfo.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK 
		|| m_schdinfo.m_nSectionID!=m_singleTrainToWork.section)
	{
		return;
	}

	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(!pData)
		return;

	// �ж��Ƿ�ӳ�
	if(!IsAddTrain(m_singleTrainToWork.train_id))
	{
		return;
	}
	
	if(m_singleTrainToWork.source_schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		BOOL oper_rc=m_tgmmi.AddTrain_ShiftSchedule(false, m_singleTrainToWork.train_index);
		if(oper_rc)
		{
			GpcLogTrainMessage(0, 0, "�հ�ƻ�,���˳��ӵ�һ����, train=%s(%u)",
							m_singleTrainToWork.train_id, m_singleTrainToWork.train_index);
		}
	}
	else if(m_singleTrainToWork.source_schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		BOOL oper_rc=m_tgmmi.AddTrain_BasicSchedule(m_singleTrainToWork.train_index);
		if(oper_rc)
		{
			GpcLogTrainMessage(0, 0, "����ͼ,���˳��ӵ�һ����, train=%s(%u)",
							m_singleTrainToWork.train_id,m_singleTrainToWork.train_index);
		}
	}
}

void CTgEditView::OnShiftTrainToWork()
{
	TRAIN_INDEX nTrainIndex=0;
	CString train_name="";
	CTG_SingleTrain *ptrain=GetDocument()->GetCurrentActiveTrain(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
	if(ptrain)
	{
		nTrainIndex=ptrain->GetTrainIndex();
		train_name=ptrain->GetTrainID();
	}
	if(0==nTrainIndex)
	{
		AfxMessageBox("����ѡ���հ�ƻ��г�");
		return;
	}
	CString str;
	str.Format("��ȷ���հ�ƻ��г� %s �ϵ���Ա��ǰ����ͼ��?", train_name);
	if(IDYES!=MessageBox(str, "��ʾ", MB_YESNO))
		return;
	m_singleTrainToWork.section=m_schdinfo.m_nSectionID;
	m_singleTrainToWork.train_index=nTrainIndex;
	m_singleTrainToWork.source_schd_type=m_schdinfo.m_nType;
	memset(m_singleTrainToWork.train_id, 0, sizeof(m_singleTrainToWork.train_id));
	strncpy(m_singleTrainToWork.train_id, train_name,31);
	m_singleTrainToWork.train_id[31]=0;
	GetDocument()->UpdateAllViews(this, TGSHIFT_TRAIN_GEN_TO_WORK, (CObject*)&m_singleTrainToWork);
}

void CTgEditView::OnUpdateShiftTrainToWork(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(gpcdef.GetUserIdentify()==GPC_DISPATCHER)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnBasicTrainToWork()
{
	TRAIN_INDEX nTrainIndex=0;
	CString train_name="";
	CTG_SingleTrain *ptrain=GetDocument()->GetCurrentActiveTrain(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	if(ptrain)
	{
		nTrainIndex=ptrain->GetTrainIndex();
		train_name=ptrain->GetTrainID();
	}
	if(0==nTrainIndex)
	{
		AfxMessageBox("����ѡ�л���ͼ�г�");
		return;
	}

	CString str;
	str.Format("��ȷ�ϻ���ͼ�г� %s �ϵ���Ա��ǰ����ͼ��?", train_name);
	if(IDYES!=MessageBox(str, "��ʾ", MB_YESNO))
		return;

	m_singleTrainToWork.section=m_schdinfo.m_nSectionID;
	m_singleTrainToWork.train_index=nTrainIndex;
	m_singleTrainToWork.source_schd_type=m_schdinfo.m_nType;
	memset(m_singleTrainToWork.train_id, 0, sizeof(m_singleTrainToWork.train_id));
	strncpy(m_singleTrainToWork.train_id, train_name,31);
	m_singleTrainToWork.train_id[31]=0;
	GetDocument()->UpdateAllViews(this, TGSHIFT_TRAIN_GEN_TO_WORK, (CObject*)&m_singleTrainToWork);
}

void CTgEditView::OnUpdateBasicTrainToWork(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	
	if(gpcdef.GetUserIdentify()==GPC_DISPATCHER)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnManualSelectBasicMark() 
{//ѡ���հ�ƻ��г�
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleMark dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		MessageBox("�����ڻ���ͼ��ʹ�øù���!","����");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(!dlg.info[i].checked )
				continue;
			if(!dlg.info[i].exist || dlg.m_check) // 20090702 �����ظ����
		 {
			 if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			 {
				 m_tgmmi.AddMark_BasicSchedule(dlg.info[i].mark_index);
				 GpcLogTrainMessage(0, 0, "�ӻ���ͼ�е���ʩ������,id=%d",dlg.info[i].mark_index);
			 }
		 }
		}
	}
}

void CTgEditView::OnManualSelectShiftMark() 
{//ѡ���հ�ƻ��г�
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleMark dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		MessageBox("�������հ�ƻ���ʹ�øù���!","����");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(!dlg.info[i].checked )
				continue;
			if(!dlg.info[i].exist || dlg.m_check) // 20090702 �����ظ����
		 {
			 if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			 {
				 m_tgmmi.AddMark_ShiftSchedule(dlg.info[i].mark_index);
				 GpcLogTrainMessage(0, 0, "���հ�ƻ��е���ʩ������,id=%d",dlg.info[i].mark_index);
			 }
		 }
		}

	}
}

void CTgEditView::OnSendStationPlan() 
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;
	if(m_pNotepadDlg != NULL)
	{
		m_pNotepadDlg->SetBasicSchduleInfo();  // �����¼ƻ���֮ǰ,���ɻ���ͼ��Ϣ
		m_pNotepadDlg->ShowWindow(SW_SHOW);
		m_pNotepadDlg->SetActiveWindow();
		m_pNotepadDlg->m_HideWnd=false;
		if(GetTGFunctionConfig()->IsStplanOnlySelectLirc())
			m_pNotepadDlg->SendStplanOnlySelLIRC();
		else
			m_pNotepadDlg->SelAllStplanRecEntity();

		if(GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
		{
			m_pNotepadDlg->SendNoticeToDCmd();
		}
	}
}   
void CTgEditView::OnSendNeighber() 
{
	OnSendNeigbourPlan();
}

// ֪ͨ��̨���ռƻ�
void CTgEditView::OnSendNeigbourPlan()
{
	GPCENTITY entityInfo;
	char sEntityExpress[15];
	sprintf(sEntityExpress, "gpc%d", gpcdef.GetDispatchNoFromDefaultUser());
	if(!gpcdef.GetEntityInfo(sEntityExpress, entityInfo))
	{
		MessageBox("�Ҳ�����̨entity, ���ܷ�����̨�ƻ�", "��ʾ", MB_OK|MB_ICONWARNING);
		return;
	}

	CTgDocument *pDoc = GetDocument();
	if(pDoc == NULL)
		return;

	CWorkScheduleHolder *pholder=pDoc->GetTGDataHolder();
	if(!pholder)
		return;

	CTG_ScheduleInfo  schd_info;
	pholder->GetWorkScheduleInfo(schd_info);

	pDoc->NoticeNeighbourReceivePlan(schd_info, entityInfo.nEntityId);
}

void CTgEditView::OnNeighberPlan() 
{	
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;

	CTgDocument *pDoc = GetDocument();
	if(pDoc != NULL && pDoc->m_vectNeigPlan.size()>0)
	{
		RecvNeigPlan plan = pDoc->m_vectNeigPlan.back();
		pDoc->m_vectNeigPlan.pop_back();

		GPCENTITY entityInfo;
		if(!gpcdef.GetEntityInfo(101, SERVER_TYPE_TG, plan.nSendSectionNo, entityInfo)) 
		{
			CString text;
			text.Format("�޷���ȡ %d�� srv_tg ��ʵ���", plan.nSendSectionNo);
			AfxMessageBox(text);
			return; 
		}

		if(DB_GetEntityStatus(entityInfo.nEntityId)!=1)
		{
			if(IDYES==MessageBox("��ע��:\n\t��̨tg_svr�������ͱ������ն������ж�, ���ܽ�����̨����,��ȷ���˳���?", "��ʾ", MB_YESNO|MB_ICONWARNING))
			{
				return;
			}
		}

		GpcLogTrainMessage(0, 0, "��̨����������̨�ƻ�con_name %s con_id %d  hour %d", plan.strSendSectionName, plan.nSendSectionNo, plan.nHour);
		CDlgNeibTrainList dlg(  plan.strSendSectionName,
			GetDocument(),
			GetViewScheduleType(),
			0,
			plan.nHour, 
			plan.nSendSectionNo, 
			this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
	else
	{
		CNeibPlanFuncChoiceDlg dlg;
		dlg.DoModal();
		if(dlg.nRet == IDOK)
		{
			OnSendNeigbourPlan();
		}
		else if(dlg.nRet == IDCANCEL)
		{
			GpcLogTrainMessage(0, 0, "��̨������ȡ��̨�ƻ�");
			CDlgNeibTrainList dlg("������̨�ƻ�",GetDocument(),GetViewScheduleType(),0,this);
			dlg.SetTgMMI(&m_tgmmi);
			dlg.DoModal();
		}
	}
}

void CTgEditView::OnRecvNeigbourPlan() 
{	
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;

	CTgDocument *pDoc = GetDocument();
	if(pDoc != NULL && pDoc->m_vectNeigPlan.size()>0)
	{
		RecvNeigPlan plan = pDoc->m_vectNeigPlan.back();
		pDoc->m_vectNeigPlan.pop_back();

		GpcLogTrainMessage(0, 0, "��̨����������̨�ƻ�con_name %s con_id %d  hour %d", plan.strSendSectionName, plan.nSendSectionNo, plan.nHour);
		CDlgNeibTrainList dlg(  plan.strSendSectionName,
			GetDocument(),
			GetViewScheduleType(),
			0,
			plan.nHour, 
			plan.nSendSectionNo, 
			this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
	else
	{
		GpcLogTrainMessage(0, 0, "��̨������ȡ��̨�ƻ�");
		CDlgNeibTrainList dlg("������̨�ƻ�",GetDocument(),GetViewScheduleType(),0,this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
}

void CTgEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	CTgDocument *pDoc=GetDocument();
	if(lHint==TGDATA_XML_INFORM)
	{
		XMLInform *pinform=(XMLInform *)pHint;
		m_xmlinform.content =pinform->content; 
		m_xmlinform.token =pinform->token;
		BOOL do_recv=FALSE;
		if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(pinform->token==TOKEN_SS_PLAN_INFORM)
			{
				GpcLogTrainMessage(0, 0, "�յ��հ�ƻ��´�֪ͨ");
				if(MessageBox("�յ��հ�ƻ��´�֪ͨ,���ڽ��н�����?","��ʾ",MB_YESNO)==IDYES)
				{
					OnGetTrainList();
					return;
				}
			}
		}
		else
			if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			{
				GpcLogTrainMessage(0, 0, "�յ������ƻ��´�֪ͨ");

				GetParentFrame()->SendMessage(UM_MEMORY_NOIFY,3,0);
			}
	}
	else if(lHint==TGSHIFT_TRAIN_GEN_TO_WORK)
	{
		m_singleTrainToWork=*(SINGLE_TRAIN_GEN_TO_WORK*)pHint;
		SingleTrainToWork();
	}
	else
		CTgView::OnUpdate(pSender,lHint,pHint);
}

void CTgEditView::OnManualReportHistory()
{
	GpcLogTrainMessage(0, 0, "�ֹ�����");
	CManualReportHistory* pReportDlg=new CManualReportHistory(&m_tgmmi,m_schdinfo.m_nSectionID,m_schdinfo.m_nType, this);
	if(pReportDlg)
	{
		pReportDlg->Create(IDD_DLG_MANUAL_REPORT);
		pReportDlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnDeleteAllShiftTrain()
{
	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		if(MessageBox("ȷ��ɾ���հ�ƻ��г���?","����",MB_YESNO)==IDYES)
		{
			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);

			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pholder->GetShiftScheduleData();
			if(pData)
				pData->DirectDeleteTrainsInRange(starttm,endtm,TRUE,FALSE);  // 20090628 �����հ�ƻ�ʱɾ��ԭ����,�յ���һ�������
			GpcLogTrainMessage(0, 0, "ɾ���հ�ƻ��г��� %d-%d",starttm,endtm);
		}
	}
}

void CTgEditView::OnDeleteAllShiftMark()
{
	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		if(MessageBox("ȷ��ɾ���հ�ƻ�ʩ����?","����",MB_YESNO)==IDYES)
		{
			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);

			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pholder->GetShiftScheduleData();
			if(pData)
				pData->DirectDeleteMarksInRange(starttm,endtm,TRUE,FALSE);  // 20090628 �����հ�ƻ�ʱɾ��ԭ����,�յ���һ�������
			GpcLogTrainMessage(0, 0, "ɾ���հ�ƻ�ʩ���� %d-%d",starttm,endtm);
		}
	}
}

void CTgEditView::OnGetBasicTrainFromOpms()
{
	if(mServer_nosync_count > 0 || mMmi_nosync_count > 0)
	{
		CString msg;
		msg.Format("����δͬ������, ������ %d  ����%d, ��ȴ�ͬ������Ϊ0�ٻ�ȡ", mServer_nosync_count, mMmi_nosync_count);
		AfxMessageBox(msg);
		return;
	}

	m_bEnableProcessTdmsBasicInfo=false;
	tdms_ctc_alarm.RemoveAll();
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		if(!UR_CanOperateBasicSchedule())
		{
			return;
		}
		long eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			MessageBox("��ע��:\n\t�������ն˺�TDͨ�ŷ����������ж�, ���ܻ�ȡ����ͼ�ƻ�!!", "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}

		GetDocument()->OpenWaitWindow(NULL);
		m_bSelTDMSTrainGenBasic=false;
		CDlgTDMSBasicTip dlg;
		if(dlg.DoModal()==IDOK)
		{
			m_bSelTDMSTrainGenBasic=dlg.m_bSelTDMSTrainGenBasic;

			CTG_ScheduleInfo schdinfo;
			CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pHolder->GetBasicPrevScheduleData();
			pData->GetScheduleInfo(schdinfo); 
			m_BaseOfBasicTmp.SetScheduleInfo(schdinfo);
			m_BaseOfBasicTmp.DeleteAllData(schdinfo, 0);

			recieved_tdms_trains.clear();

			if(RequestBasicScheduleFromOPMS(0))
			{
				GetDocument()->OpenWaitWindow("���ڽ���TDMS����ͼ����,���Ժ�...");
				m_bEnableProcessTdmsBasicInfo=true;
			}
			else
			{
				AfxMessageBox("��ȡTDMSԤ������ͼʧ��!");
			}
		}
	}
}

BOOL CTgEditView::RequestShiftScheduleFromOPMS(TIME starttm,TIME endtm)
{
	CString token,content;
	if(TD_GenRequestMessage_3_YL_BY_TIME(token,content,GetDocument()->GetCurrentSectionNo(),starttm,endtm,FALSE,TRUE,TRUE,""))
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
		GpcLogTrainMessage(0,0,"��TD�����հ�ƻ� %d %s %s", eid, token, content);
		return TRUE;
	}
	
	return FALSE;
}

void CTgEditView::OnGetTrainList()
{
	long eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	if(DB_GetEntityStatus(eid)!=1)
	{
		MessageBox("��ע��:\n\t�������ն˺�TDͨ�ŷ����������ж�, ���ܻ�ȡ�հ�ƻ�!!", "��ʾ", MB_OK|MB_ICONWARNING);
		return;
	}

	m_bEnableProcessTdmsShiftInfo=false;
	tdms_ctc_alarm.RemoveAll();
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

		GetDocument()->OpenWaitWindow(NULL);

		if(dlg.DoModal()==IDOK)
		{
			recieved_tdms_trains.clear();

			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);
			if(RequestShiftScheduleFromOPMS(starttm,endtm))
			{
				GpcLogTrainMessage(0, 0, "��ȡ��ƻ����Σ����κ� %d ��� %d", m_schdinfo.m_nSectionID,dlg.m_nShiftID);
				GetDocument()->OpenWaitWindow("���ڽ���TDMS�հ�ƻ�����,���Ժ�...");
				m_bEnableProcessTdmsShiftInfo=true;
			}
			else
			{
				AfxMessageBox("��ȡ��ƻ�����ʧ��!");
			}
		}
	}
}

void CTgEditView::OnGetMarkList()
{
	long  eid=0; 
	if(gpcdef.IsEnableMMPlanFromCAD())
	{
		eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			AfxMessageBox("��ע��:\n\t�������ն� �� ʩ�������� �����ж�, ���ܻ�ȡʩ���ƻ�!!", MB_OK|MB_ICONWARNING);
			return;
		}
	}
	else
	{
		eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM);
		if(DB_GetEntityStatus(eid)!=1)
		{
			AfxMessageBox("��ע��:\n\t�������ն� �� TD������ �����ж�, ���ܻ�ȡʩ���ƻ�!!", MB_OK|MB_ICONWARNING);
			return;
		}
	}

	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		CTime begintm(dlg.m_ntStartTime);
		begintm = CTime(begintm.GetYear(),begintm.GetMonth(),begintm.GetDay(),0,0,0);

		GetDocument()->RequestMMPlanFromOPMS(begintm.GetTime(),dlg.m_ntEndTime,this,eid);
	} 
}
void CTgEditView::OnRecvTrainList()
{
	GetParentFrame()->SendMessage(UM_MEMORY_NOIFY,2,0);
	CDlgTrainList dlg("���յ����ƻ�",GetDocument(),m_schdinfo.m_nType,0,NULL);
	dlg.SetTgMMI(&m_tgmmi);
	dlg.DoModal();
	GpcLogTrainMessage(0, 0, "���յ����ƻ�");
}


void CTgEditView::OnRecvMarkList()
{
	CDlgTrainList dlg("���յ���ʩ���ƻ�",GetDocument(),m_schdinfo.m_nType,1,NULL);
	dlg.DoModal();
}



void CTgEditView::OnManualAddNewTrain()
{
	if(!m_bEnableFreeAddTrain)
	{
		MessageBox("��ʹ���հ�ƻ���ʽ�ӳ�","��ʾ");
		return;
	}

	if(IS_BASIC_SCHD(GetViewScheduleType())) // Ԥ������ͼ,�����Ի���ʩ������
	{
		MessageBox("����ͼ��֧�ָ��ַ�ʽ�ӳ�","��ʾ");
		return;
	}

	CAddNewTrainDlg dlg(GetDocument(),m_schdinfo.m_nType, this);
	if(dlg.DoModal () != IDOK)
		return;

	for (int i=0;i<dlg.m_nTrainCount ;i++)
	{
		int station=dlg.m_nReportTrainInfo [i].nStation;
		int entry=-1;
		int exit=-1; 
		TRAIN_NO trainid, chgtrainid;
		memset(trainid, 0, sizeof(trainid));
		memset(chgtrainid, 0, sizeof(chgtrainid));
		strcpy(trainid,(LPCTSTR)dlg.m_nReportTrainInfo [i].strTrainId);
		int route=dlg.m_nReportTrainInfo [i].nRoute;
		TIME tm=long_time(dlg.m_nReportTrainInfo [i].usReceiveDate ,dlg.m_nReportTrainInfo [i].usReceiveTime);
		int iflag,oflag;
		if(dlg.m_nReportTrainInfo[i].fetch)
		{
			iflag= (byte)SCHD_ACCEPT;
		}
		else
		{
			iflag = (byte)SCHD_START;
		}

		if(dlg.m_nReportTrainInfo[i].giveout)
		{
			oflag = (byte)SCHD_GIVEOUT;
		}
		else
		{
			oflag = (byte)SCHD_TERMINAL;
		}
		int flag= dlg.m_nReportTrainInfo [i].nFlag ; // NA??? ����
		BYTE nRunType=dlg.m_nReportTrainInfo [i].nRunType;
		m_tgmmi.AddTrain_Passway(0, trainid, chgtrainid, route,iflag,oflag,tm,nRunType);		
		GpcLogTrainMessage(0, 0, "�г����� %s, run_type=%d�������",trainid,nRunType);

	}

}


void CTgEditView::OnManualOperCheck()
{
	if(GetTGFunctionConfig()->IfCheckManualOper())
	{
		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ������ͼ�������ذ�ť, ʹ��<����ͼ����������ģʽ>���в���");
		GetTGFunctionConfig()->SetCheckManualOper(FALSE);
	}
	else
	{
		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ������ͼ�������ذ�ť, ʹ��<����ͼ��������ģʽ>���в���");
		GetTGFunctionConfig()->SetCheckManualOper(TRUE);
	}
}

void CTgEditView::OnUpdateManualOperCheck(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->IfCheckManualOper())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnGetEngineTicketInfo()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return;

	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	if(ptrain==NULL)
		return;

	TRAIN_INDEX shift_index=0;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		shift_index=ptrain->GetShiftTrainIndex();
	}
	else
	{
		return;
	}
	if(shift_index!=0)
	{
		CTrainCrewTicket *pdlg = new CTrainCrewTicket(this, GetDocument(), shift_index, ptrain->GetTrainID()); 
		if(pdlg != NULL)
		{
			pdlg->Create(IDD_DIALOG_CREW_TICKET); 
			pdlg->ShowWindow(SW_SHOW);
		}
	}
}

void CTgEditView::OnUpdateGetEngineTicketInfo(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK!=GetViewScheduleType())
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
	{
		CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
		if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnShiftGetShiftinfo()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		{
			CTG_SingleTrain* ptrain=(CTG_SingleTrain *)pobj;
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(ptrain->GetTrainIndex(), 1);
		}
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		{
			CTG_SingleTrain* ptrain=(CTG_SingleTrain *)pobj;
			TRAIN_INDEX shift_index=ptrain->GetShiftTrainIndex();
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(shift_index, 2);
		}
	}
}

void CTgEditView::OnUpdateShiftGetShiftinfo(CCmdUI *pCmdUI)
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


void CTgEditView::OnHatTrain() 
{
	CInputHatTrain dlg(GetDocument());
	if(dlg.DoModal()==IDOK)
	{
		m_tgmmi.AddTrain_BasicSchedule(false, dlg.m_GraphicTrainId,dlg.m_HatTrainId);		
		GpcLogTrainMessage(0, 0, "�г����� %s,��ñ���� %s",dlg.m_GraphicTrainId,dlg.m_HatTrainId);
	}
}

void CTgEditView::OnUpdateElecArmsSet(CCmdUI *pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::ELE_ARMS))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnElecArmsSet()
{
	if(m_DlgArms.GetSafeHwnd() == NULL)
		return;
	CTgDocument *pdoc=GetDocument();
	m_DlgArms.SetSectionID(pdoc->GetCurrentSectionNo(),pdoc->IsQueryMode());
	m_DlgArms.OnBnClickedRefreshRemote(); // ������ʾʱ,ˢ����״̬
	m_DlgArms.ShowWindow(SW_SHOW); 
}

void CTgEditView::OnUpdateOperRestore(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
	}
	else
		if(!UR_CanOperateSchedule())
		{
			pCmdUI->Enable(FALSE); 
		}
		else
			pCmdUI->Enable(TRUE); 
}

void CTgEditView::OnUpdateSaveBasicSideWorkInfo(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		pCmdUI->Enable(TRUE);
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnUpdateDeleteBasic(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		if(gpcdef.GetOperatorPrior4Basic() && (gpcdef.GetUserIdentify() == GPC_TEST))
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnUpdateOperTGTrain(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}
	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE); 
}

// ��������������������ҵ����Ȩ��---һ������½�ֹ��������������ҵ�����ߣ���ĳ�ֿ����»���ɱ�̨�г���������������̨�ظ�
void CTgEditView::OnUpdateOperAddWorkTrain(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}

	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE); 
	else if(right.operate==2)
	{
		if(IsInhbitAddWorkTrain(right.wintg_operate))
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnUpdateOperTGMark(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(IS_BASIC_SCHD(GetViewScheduleType())) // Ԥ������ͼ,�����Ի���ʩ������
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}
	if(right.operate==2 || right.operate==3 )
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnUpdateOperTGOPMS(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}
	if(IsReceiveTMISPlan(right.wintg_operate))
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnUpdateSendPlan(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
		|| GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}
	if(IsSendPlanToStation(right.wintg_operate))
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnBackOper()
{
	m_tgmmi.PopOper(m_schdinfo);
}

void CTgEditView::OnUpdateBackOper(CCmdUI *pCmdUI)
{
	if(m_tgmmi.CanBackOper())
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnChangeTrainRoute()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
		if (gpcdef.IsInhibitManualConvertActualTime())
		{
			if(ptrain->HasActualRecord()>=0)
			{
				AfxMessageBox("��Ҫ�����·���г�����ʵ�ʵ�,��������о�·�������!\n���������ͨ�����������޸����һ��ʵ�ʵ���������.");
				return;
			}
		}

		//̫ԭ added 
		if(GetTGFunctionConfig()->IsCheckRouteforCrhAndImp())
		{
			TG_TrainInfo  traininfo=ptrain->GetTrainInfo()->m_data;
			if(ptrain->IsCRHTrain() || traininfo.super_rank>0 )
			{
				if(AfxMessageBox("ȷ�϶Դ˶���/�ص��г����о�·���ô��",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
					return;
			}
		}
		//end

		CChangeTrainRoute dlg(GetDocument()->GetCurrentSectionNo(),ptrain->GetTrainRecord(0)->GetStationNo(),0,this);  
		if(dlg.DoModal()==IDOK)
		{
			if(dlg.m_nRouteNo>0)
			{
				CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
				CTG_SingleTrain newtrain=*ptrain;
				CTG_ScheduleInfo schd;
				GetViewScheduleInfo(schd);
				if(pholder->GenTrain_ChangeRoute(newtrain,schd,dlg.m_nRouteNo))
				{
					GpcLogTrainMessage(0, 0, "�г����� %s,�����г���·,��·��:%d",ptrain->GetTrainID(),dlg.m_nRouteNo);
					// ÿ���˲���֮���жϾ�·�Ƿ���ͬ
					if(GetTGFunctionConfig()->IsZeroGIDPasswayChange() && (newtrain.GetShiftTrainIndex()!=0))
					{
						if(!ptrain->IsSamePathway(newtrain))
						{
							GpcLogTrainMessage(0, 0, "�г�%s(%u)�ľ�·��ԭ����ͬ���ȫ��ID.", newtrain.GetTrainID(), newtrain.GetTrainIndex());
							newtrain.SetShiftTrainIndex(0);
							newtrain.SetFromShift(FALSE);
						}
					}
					m_tgmmi.DeleteTrain_Directly(ptrain->GetTrainIndex());

					newtrain.SetTrainStartEndFlag();
					CTgDocument::SetTrainFromToConid(schd.m_nSectionID, &newtrain);

					m_tgmmi.AddTrain_Directly(newtrain);
				}
			}
		}
	}
}

BOOL  CTgEditView::CheckUserTGRight()
{
	if(GetDocument()->IsQueryMode()) 
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV && !UR_CanOperateBasicSchedule())
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		return FALSE;
	}
	if(!UR_CanOperateSchedule())
	{
		return FALSE;
	}
	return TRUE;
}

void CTgEditView::OnSaveToFiles()
{
	GetDocument()->GetTGDataHolder()->SaveDataToLocalFiles(GetViewScheduleType());
}

void CTgEditView::OnLoadFromFiles()
{
	/*
	if(MessageBox("ȷ�Ͻ����ر��������ͼ���ݻָ���?","����",MB_YESNO)==IDYES)
	{
	GetDocument()->GetTGDataHolder()->LoadDataFromLocalFiles(GetViewScheduleType());
	}*/
	CTG_TrainScheduleBase *pbase=GetDocument()->GetScheduleData(GetViewScheduleType());
	CDlgRestoreTrainList dlg(pbase);
	OnRecievedDataEnd(0,1);
	dlg.DoModal();
	OnRecievedDataEnd(0,0);
}

BOOL CTgEditView::RequestBasicScheduleFromOPMS(int type) // type==1 ���û���ͼ   type==0 Ԥ���䱾ͼ
{
	CString token,content;
	if(TD_GenRequestMessage_19_BASICSCHEDULE(token,content,GetDocument()->GetCurrentSectionNo(),type))
	{ 
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
		GpcLogTrainMessage(0,0,"��TD�������ͼ %d %s %s", eid, token, content);
		return TRUE;
	}
	return FALSE;
}

BOOL CTgEditView::ProcessXML_BasicScheduleTrain(const char *xml,const char *node)
{
	if(!m_bEnableProcessTdmsBasicInfo)
		return FALSE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=pHolder->GetBasicPrevScheduleData();
	
	pData->GetScheduleInfo(schdinfo); 

	CTG_SingleTrain train;
	int opms_seq=0,opms_count=0;
	int schd_type=0;
	if(!TD_LoadBasicScheduleTrainFromXML(train,xml,node,&opms_seq,&opms_count,&schd_type,tdms_ctc_alarm))
	{
		GpcLogTrainMessage(0,0, "����TDMSԤ������ͼʧ��");
		return FALSE;
	}
	
	if(opms_seq==1)
	{
		mTG.BeginBatchDraw(1,"��ʼ��TDMS���ջ���ͼ"); // 
	}

	DWORD guid = train.GetShiftTrainIndex();
	AddTdmsTrainGuid(guid);

	CString waitstr;
	waitstr.Format("����Ԥ���ƻ�:%d/%d,��ǰ����:%d ���� %s(%u)", opms_seq,opms_count,recieved_tdms_trains.size(),train.GetTrainID(),guid); 
	GpcLogTrainMessage(0,0, waitstr);
	if(opms_seq==opms_count)
	{
		if(recieved_tdms_trains.size()==opms_count)
			GetDocument()->OpenWaitWindow(NULL);
		else
		{
			CString waitstr;
			waitstr.Format("���ջ���ͼ���,�г��ζ�ʧ,��%d�� �յ�%d��,�����»�ȡ", opms_count,recieved_tdms_trains.size()); 
			GetDocument()->OpenWaitWindow(waitstr);
			tdms_ctc_alarm.Add(waitstr);
			GpcLogTrainMessage(0,0, waitstr);
		}
		m_bEnableProcessTdmsBasicInfo=false;

		GpcLogSysMessage(0,"TDMS����ͼ�������,���ý���δͬ����鶨ʱ��");
		SetTimer(995,500,NULL);
	}
	else
	{
		CString waitstr;
		waitstr.Format("���ڽ���TDMS����ͼ���� %s, %d/%d",train.GetTrainID(),opms_seq,opms_count); 
		GetDocument()->OpenWaitWindow(waitstr);
	}

	if(GetTGFunctionConfig()->IsCalBasicSchdExitEntry())
	{
		if(!pHolder->TGOCXSetTrainExitEntry(train, &mTG))
		{
			CString str;
			str.Format("����:%s ��·����ʧ��,��ȷ���г���·", train.GetTrainID());
			tdms_ctc_alarm.Add(str);
		}
	}

	bool bOldSplitTrain=false;
	std::vector<CTG_SingleTrain> othertrain_list;
	if(pHolder->GenTrainBasicSchedule_OPMSx(train, othertrain_list, schdinfo, bOldSplitTrain))
	{
		pHolder->CheckTrainNo(train,tdms_ctc_alarm);
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);

		if(m_bSelTDMSTrainGenBasic)
		{
			m_BaseOfBasicTmp.AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		}
		else
		{
			CTG_SingleTrain *pold=pData->GetSingleTrain(train.GetTrainIndex()); // OPMS�ƻ���trainindex==shift_index
			if(pold)
			{
				if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
					GpcLogTrainMessage(0,0,"��ȡTDMS����ͼʱ,�г��Ѿ�����, �����ظ�����!");
				else
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
			else
				pData->AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		}
	}
	for(int i=0; i<othertrain_list.size(); i++)
	{
		CTG_SingleTrain othertrain=othertrain_list[i];
		if(othertrain.GetTrainRecordCount()>0)
		{
			if(GetTGFunctionConfig()->IsCalBasicSchdExitEntry())
			{
				GpcLogTrainMessage(0, 0, "ProcessXML_BasicScheduleTrain ��ͼ�ؼ���ʼ��������");

				int first_entry=255, last_exit=255;
				if(bOldSplitTrain)
				{	
					// ������վ����ڡ�δվ������
					CTG_TrainRecord *pfirst_rec=othertrain.GetTrainFirstRecord();
					CTG_TrainRecord *plast_rec=othertrain.GetTrainLastRecord();
					if(pfirst_rec)
						first_entry=pfirst_rec->GetEntry();
					if(plast_rec)
						last_exit=plast_rec->GetExit();
				}
				if(!pHolder->TGOCXSetTrainExitEntry(othertrain, &mTG, first_entry, last_exit))
				{
					CString str;
					str.Format("����:%s ��·����ʧ��,��ȷ���г���·", othertrain.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
			}
			pHolder->CheckTrainNo(othertrain,tdms_ctc_alarm);
			CTgDocument::SetTrainReverseFlag(&othertrain);
			CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &othertrain);

			if(m_bSelTDMSTrainGenBasic)
			{
				m_BaseOfBasicTmp.AppendTrain(schdinfo,train,SYCFLAG_NULL); 
			}
			else
			{
			CTG_SingleTrain *pold=pData->GetSingleTrain(othertrain.GetTrainIndex()); // OPMS�ƻ���trainindex==shift_index
			if(pold)
			{
				if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
					GpcLogTrainMessage(0,0,"��ȡTDMS����ͼʱ,�г� %d �Ѿ�����, �����ظ�����!");
				else
					pData->UpdateTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
			else
				pData->AppendTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
		}
	}

	if(opms_seq==opms_count)
	{
		if(GetTGFunctionConfig()->IsBasicShowCompareDlg())
		{
			if(m_pBasicSchdCompareDlg!=NULL)
			{
				m_pBasicSchdCompareDlg->InitDlgCtrlItem();
				m_pBasicSchdCompareDlg->ShowWindow(SW_SHOW);
				GpcLogSysMessage(0,"TDMS����ͼ�������,������ʾ���ս���Ի���");
			}
		}
		
		if(tdms_ctc_alarm.GetCount()>0)
		{
			CString text="��ע��,����ͼ�������,��ȷ����������:";
			CDlgConfirm dlg(text, tdms_ctc_alarm, this);
			dlg.DoModal();
		
			GpcLogSysMessage(0,"TDMS����ͼ�������,������������:");
			for(int n=0; n<tdms_ctc_alarm.GetCount(); n++)
			{
				GpcLogSysMessage(0, "%s", tdms_ctc_alarm.GetAt(n));
			}
		}
	}

	return TRUE;
}

void CTgEditView::OnSelTdmsTrainsGenBasic()
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(pHolder==NULL)
		return;

	CDlgSelTDMSBasicGen* pdlg=new CDlgSelTDMSBasicGen(this, &m_BaseOfBasicTmp, pHolder);
	if(pdlg)
	{
		pdlg->Create(CDlgSelTDMSBasicGen::IDD, this);
		pdlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnUpdateSelTdmsTrainsGenBasic(CCmdUI *pCmdUI)
{
	if(m_bSelTDMSTrainGenBasic)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE); 
}

BOOL  CTgEditView::ProcessXML_TrainCrew(const char *xml,const char *node)
{
	if(0!=stricmp(node,TOKEN_20_ACK_CREW_INFO))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� ������Ϣ node:%s xml:%s", GetViewScheduleType(), node, xml);
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL; 

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"�����հ�ͼ�ƻ�������Ϣ");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"���µ���ͼ�ƻ�������Ϣ");
	}
	if(!pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	CTD_TrainTicketList ticketList;
	if(!ticketList.LoadCrewFromTDXML(xml, node))
		return FALSE;

	DWORD gtid;
	CString info;
	for(int i=0; i<ticketList.m_nCount; i++)
	{
		gtid=ticketList.m_list[i].GID;
		info=ticketList.m_list[i].GetCrewInfo();
		if(info.IsEmpty() || gtid<=0)
			continue;

		//////////////////////////
		CTG_SingleTrain *ptrain=pData->GetSingleTrainByShiftIndex(gtid,NULL_STATION);
		if(!ptrain)
		{
			GpcLogTrainMessage(0,0,"�Ҳ��� GTID %d ��Ӧ���г�,������Ϣ����ʧ��", gtid);
			continue;
		}
		const char *prevstr=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW);
		if(prevstr && !stricmp(prevstr,info))  // �ظ�!
			continue;
		ptrain->SetImportInfo(info,TRAIN_TXT_INFO_TYPE_CREW);
		//����������,�������½���
		ptrain->SetSyncStatus(GetDocument()->GetServerCommInterface()->GetInterfaceFlag(),FALSE);
	}

	return TRUE;
}

BOOL  CTgEditView::ProcessXML_YingyeKediao(const char *xml, const char *node)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	GpcLogTrainMessage(0,0,"CTgEditView type %d �յ�TD���� �͵���Ӫҵ��Ϣ node:%s xml:%s", GetViewScheduleType(), node, xml);

	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL; 

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"�����հ�ͼ�ƻ��͵�Ӫҵ��Ϣ");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"���µ���ͼ�ƻ��͵�Ӫҵ��Ϣ");
	}
	if(!pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	CTG_SingleTrain *ptrain=TD_GetTrainInfoFromXML(pData,xml,node);
	if(ptrain) //����������,�������½���
		ptrain->SetSyncStatus(GetDocument()->GetServerCommInterface()->GetInterfaceFlag(),FALSE);
	
	return TRUE;
}
BOOL  CTgEditView::ProcessXML_ShiftTrain(const char *xml,const char *node)
{
	if(!m_bEnableProcessTdmsShiftInfo)  
		return FALSE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		return FALSE;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=pHolder->GetShiftScheduleData(); 

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	// ������Ϣ
	CTG_SingleTrain train;
	int opms_seq=0,opms_count=0;
	if(!TD_LoadShiftScheduleTrainFromXML(schdinfo,train,xml,node,&opms_seq,&opms_count,1, tdms_ctc_alarm))
	{
		GpcLogTrainMessage(0,0, "�����հ�ƻ�ʧ��");
		return FALSE;
	}

	if(opms_count==0)
	{
		m_bEnableProcessTdmsShiftInfo=false;
		
		CString waitstr;
		waitstr.Format("�����հ�ƻ����,û���յ��հ೵��"); 
		GetDocument()->OpenWaitWindow(waitstr);
		GetDocument()->OpenWaitWindow(NULL);
		GpcLogTrainMessage(0,0, waitstr);
		return TRUE;
	}

	if(!GetTGFunctionConfig()->IsEnableNewCzh())
	{
		train.InitTrainExpand();
	}
	
	if(opms_seq==1)
	{
		mTG.BeginBatchDraw(1, "��ʼ��TDMS�����հ�ƻ�"); // 
	}
	
	DWORD guid = train.GetShiftTrainIndex();
	AddTdmsTrainGuid(guid);
	
	CString waitstr;
	waitstr.Format("�����հ�ƻ�:%d/%d,��ǰ����:%d ���� %s(%u)", opms_seq,opms_count,recieved_tdms_trains.size(),train.GetTrainID(),guid); 
	GpcLogTrainMessage(0,0, waitstr);

	if(opms_seq==opms_count)
	{
		m_bEnableProcessTdmsShiftInfo=false;
		if(recieved_tdms_trains.size()==opms_count)
			GetDocument()->OpenWaitWindow(NULL);
		else
		{
			CString waitstr;
			waitstr.Format("�����հ�ƻ����,�г��ζ�ʧ,��%d��,�յ�%d��,�����»�ȡ", opms_count,recieved_tdms_trains.size()); 
			GetDocument()->OpenWaitWindow(waitstr);
			tdms_ctc_alarm.Add(waitstr);
			GpcLogTrainMessage(0,0, waitstr);
		}
		GpcLogSysMessage(0,"TDMS�հ�ƻ��������,���ý���δͬ����鶨ʱ��");
		SetTimer(995,500,NULL);
	}
	else
	{
		CString waitstr;
		waitstr.Format("���ڽ���TDMS�հ�ƻ����� %s, %d/%d",train.GetTrainID(),opms_seq,opms_count); 
		GetDocument()->OpenWaitWindow(waitstr);
	}

	// ����,�ص㾯��,���˴���
	CTgDocument::SetTrainSpaceGaurdMilitaryRank(train);
	
	if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
	{
		if(!pHolder->TGOCXSetTrainExitEntry(train, &mTG))
		{
			CString str;
			str.Format("����:%s ��·����ʧ��,��ȷ���г���·", train.GetTrainID());
			tdms_ctc_alarm.Add(str);
		}
	}
	
	bool bOldSplitTrain=false;
	std::vector<CTG_SingleTrain> othertrain_list;
	if(pHolder->GenTrain_OPMSx(train,othertrain_list,schdinfo,TRUE,bOldSplitTrain))
	{
		pHolder->CheckTrainNo(train,tdms_ctc_alarm);
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);
		CTgDocument::CheckTdmsSideByStationside(schdinfo.m_nSectionID, train);
		
		CTG_SingleTrain *pold=pData->GetSingleTrain(train.GetTrainIndex());
		if(pold)  // ����
		{
			if(train.GetTrainRecordCount()<=0)
			{
				GpcLogTrainMessage(0,0,"������OPMS����ʱ,reccount==0");
			}

			if(!pold->CompareCoreData(train))  // ���������ޱ仯
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
		}
		else   // ����
		{
			pData->AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		} 

		if(GetTGFunctionConfig()->IsGpcSendReqOtherMsg()) //TD�Լ�����train����������͵���Ӫҵ�ͳ�������ͼ���ڷ�������
		{
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(train.GetTrainIndex(), 1);
		}
	}
	
	for(int i=0; i<othertrain_list.size(); i++) // ��������ֲ���г���θ���
	{
		CTG_SingleTrain othertrain=othertrain_list[i];
		if(othertrain.GetTrainRecordCount()>0)
		{
			if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
			{
				GpcLogTrainMessage(0, 0, "ProcessXML_ShiftTrain ��ͼ�ؼ���ʼ��������");
				
				int first_entry=255, last_exit=255;
				if(bOldSplitTrain)
				{
					// ������վ����ڡ�δվ������
					CTG_TrainRecord *pfirst_rec=othertrain.GetTrainFirstRecord();
					CTG_TrainRecord *plast_rec=othertrain.GetTrainLastRecord();
					if(pfirst_rec)
						first_entry=pfirst_rec->GetEntry();
					if(plast_rec)
						last_exit=plast_rec->GetExit();
				}
				if(!pHolder->TGOCXSetTrainExitEntry(othertrain, &mTG, first_entry, last_exit))
				{
					CString str;
					str.Format("����:%s ��·����ʧ��,��ȷ���г���·", othertrain.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
			}
			pHolder->CheckTrainNo(othertrain,tdms_ctc_alarm);

			CTgDocument::SetTrainReverseFlag(&othertrain);
			CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &othertrain);
			CTgDocument::CheckTdmsSideByStationside(schdinfo.m_nSectionID, othertrain);
			
			CTG_SingleTrain *pold=pData->GetSingleTrain(othertrain.GetShiftTrainIndex()); // ��ƻ���trainindex==shift_index
			if(pold)
			{  
				if(!pold->CompareCoreData(othertrain))  // ���������ޱ仯
					pData->UpdateTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
			else
			{
				pData->AppendTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
		}
	}
	
	if(opms_seq==opms_count)
	{		
		if(GetTGFunctionConfig()->IsShiftShowCompareDlg())
		{
			if(m_pShiftSchdCompareDlg!=NULL)
			{
				m_pShiftSchdCompareDlg->InitDlgCtrlItem();
				m_pShiftSchdCompareDlg->ShowWindow(SW_SHOW);
				GpcLogSysMessage(0,"TDMS�հ�ƻ��������,������ʾ���ս���Ի���");
			}
		}

		if(tdms_ctc_alarm.GetCount()>0)
		{
			CString text="��ע��,�հ�ƻ��������,��ȷ����������:";
			CDlgConfirm dlg(text, tdms_ctc_alarm, this);
			dlg.DoModal();
		
			GpcLogSysMessage(0,"TDMS�հ�ƻ��������,������������:");
			for(int n=0; n<tdms_ctc_alarm.GetCount(); n++)
			{
				GpcLogSysMessage(0, "%s", tdms_ctc_alarm.GetAt(n));
			}
		}
	}

	return TRUE;
}


void CTgEditView::OnDeleteAllBasic()
{
	if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
	{
		CDlgBasicTrainDeleteChoice dlg(1, this);
		if(dlg.DoModal()==IDOK)
		{
			int nResult = dlg.m_nChoice;
			if(nResult==1)
			{
				OnDeleteBasicTrains(0);
			}
			else if(nResult==2)
			{
				OnDeleteBasicTrains(1);
			}
			else if(nResult==3)
			{
				OnDeleteBasicTrains(2);
			}
		}
	}
	else
	{
		OnDeleteBasicTrains(0);
	}
}

void CTgEditView::OnDeleteBasicTrains(int type)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		MessageBox("���û���ͼ����ɾ��!","��ʾ");
		return;
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		MessageBox("�鵵����ͼ����ɾ��!","��ʾ");
		return;
	}

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return;
	
	CString str;
	if(type==0)
		str="��ע��,ȷ��ɾ��Ԥ������ͼ�г���������?";
	else if(type==1)
		str="��ע��,ȷ��ɾ��Ԥ������ͼ�пͳ�����������?";
	else if(type==2)
		str="��ע��,ȷ��ɾ��Ԥ������ͼ�л�������������?";
	else
		return;
	if(MessageBox(str,"����",MB_YESNO)==IDYES)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(pData)
			pData->DirectDeleteTrainsInRange(0,0,TRUE,FALSE,type);  // 
		if (type==0)
			GpcLogTrainMessage(0, 0, "ɾ������ͼ���Σ�schetype=%d",GetViewScheduleType());
		else if (type==1)
			GpcLogTrainMessage(0, 0, "ɾ������ͼ�ͳ����Σ�schetype=%d",GetViewScheduleType());
		else if (type==2)
			GpcLogTrainMessage(0, 0, "ɾ������ͼ�������Σ�schetype=%d",GetViewScheduleType());
		////////// �ӿ�ɾ����ʾ�����ٶ� /////////
		m_tgmmi.SetTGUpdateFlag(1);
		GetDocument()->GetTGDataHolder()->PollingForUpdate(pData,&m_tgmmi,TRUE);
		m_tgmmi.SetTGUpdateFlag(0);
		mTG.RefreshTG(0);
	}
}


void CTgEditView::OnBasicGraphsavetotext() 
{
	CTgDocument  *pDoc  = GetDocument();
	CString fileDir;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());

	CString infoName, recordsName, xmlfileName, tnfilename;
	infoName.Format("bsinfo%02d.txt",pDoc->GetCurrentSectionNo());
	recordsName.Format("tntime%02d.txt",pDoc->GetCurrentSectionNo());
	xmlfileName.Format("section%02d.xml",pDoc->GetCurrentSectionNo());
	tnfilename.Format("����ͼ�������ļ�%02d.txt",pDoc->GetCurrentSectionNo());

	bool bFindInfoFile=false, bFindRecordFile=false, 
		bFindTnFile=false, bFindXmlFile=false;

	CFileFind  fileFind;
	BOOL flag = fileFind.FindFile(fileDir+"\\*.*");
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString fileName = fileFind.GetFileName();
		if(fileName == infoName)
		{
			bFindInfoFile=true;
		}
		if(fileName == recordsName)
		{
			bFindRecordFile=true;
		}
		if(fileName == tnfilename)
		{
			bFindTnFile=true;
		}
		if(fileName == xmlfileName)
		{
			bFindXmlFile=true;
		}
	}

	//����ļ���
	if(bFindInfoFile || bFindRecordFile || bFindTnFile || bFindXmlFile)
	{
		CString cap;
		cap.Format("�ڱ���Ŀ¼%s��,������Ҫ�����ͬ���ļ�, ��ȷ�ϸ�����?",fileDir);
		if(MessageBox(cap, "��ʾ", MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			AfxMessageBox("�ļ�����ʧ��!!");
			return;
		}
	}

	CString path = fileDir + "\\" + infoName;
	pDoc->olSaveTrainInfo(GetViewScheduleType(),path);

	path = fileDir + "\\" + recordsName;
	pDoc->olSaveTrainRecords(GetViewScheduleType(),path);

	path = fileDir + "\\" + tnfilename;
	pDoc->olSaveTrainRecords(GetViewScheduleType(),path);

	path = fileDir + "\\" + xmlfileName;
	pDoc->olSaveTrainToFile(GetViewScheduleType(),path);

	CString cap;
	cap.Format("�ļ������� %s Ŀ¼��,�����ĸ��ļ� %s %s %s %s",fileDir, infoName, recordsName, tnfilename, xmlfileName);
	MessageBox(cap);

	return;
}

void CTgEditView::OnEnableFreeAddTrain()
{
	CSuperPassDlg pdlg("��������");
	if (pdlg.DoModal() == IDOK)
	{
		if (!pdlg.PasswordIsOk())
		{
			AfxMessageBox("������󣡣���"); 
			return;
		}
	}
	else
	{
		return;
	}

	if(m_bEnableFreeAddTrain)
	{
		m_bEnableFreeAddTrain=FALSE;
		mTG.SetContrlRight(1);
		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ��TDMSģʽ��ť, ʹ��<���������ɻ���ģʽ>���в���");
	}
	else
	{
		m_bEnableFreeAddTrain=TRUE;
		mTG.SetContrlRight(0);
		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ��TDMSģʽ��ť, ʹ��<�������ɻ���ģʽ>���в���");
	}
	m_tgmmi.EnableFreeTrainOper(m_bEnableFreeAddTrain);

}

void CTgEditView::OnUpdateEnableFreeAddTrain(CCmdUI *pCmdUI)
{
	if(m_bEnableFreeAddTrain)
		pCmdUI->SetCheck(0);
	else
		pCmdUI->SetCheck(1);		
}

void CTgEditView::OnGenNodeIndex()
{
	CSuperPassDlg pdlg("��������");
	if (pdlg.DoModal() == IDOK)
	{
		if ( !pdlg.PasswordIsOk())
		{
			AfxMessageBox("������󣡣���"); 
			return;
		}
	}
	else
	{
		return;
	}

	m_tgmmi.GenTrainNodeIndex();
}

void CTgEditView::OnUpdateGenNodeIndex(CCmdUI *pCmdUI)
{
	if(UR_CanOperateBasicSchedule())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnDirectUpdateFromOPMS()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
	{
		MessageBox("��ѡ�������߲���");
		return;
	}
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
	{
		MessageBox("��ѡ�������߲���");
		return;
	}

	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	if(ptrain->GetShiftTrainIndex()==NULL_TRAININDEX)
	{
		MessageBox("ѡ�е������߷��հ�ƻ�����");
		return;
	}
	CDlgTrainList dlg("����TDMS����",GetDocument(),m_schdinfo.m_nType,0,NULL);
	dlg.m_bDirectlyRequestMode=TRUE;
	dlg.AddRequestTrainGUID(ptrain->GetShiftTrainIndex(),ptrain->GetTrainID(),FALSE);
	dlg.SetTgMMI(&m_tgmmi);
	dlg.DoModal();

	CString tmp;
	tmp.Format("��TDMS���³��� %s(index: %u ,shiftindex: %u) ",
		ptrain->GetTrainID(),ptrain->GetTrainIndex(),ptrain->GetShiftTrainIndex());
	GpcLogTrainMessage(0, 0, tmp);
}

void CTgEditView::OnMutiTrainMove()
{
	// TODO: �ڴ���������������
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::MUTI_SEL, MUTI_SEL::TRAIN);
}

// ��������ͼ
extern BOOL  ReqBasicName_ToServer(int con_id, bool bload);
void CTgEditView::OnBasicNameing()
{
	if(!gpcdef.GetOperatorPrior4Basic())
		return;

	CDlgBasicParam dlg(GetDocument(), GetViewScheduleType(), this); 
	dlg.DoModal();
	ReqBasicName_ToServer(m_schdinfo.m_nSectionID, true);
}

void CTgEditView::OnUpdateBasicNameing(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnBasicDeleteName()
{
	if(!gpcdef.GetOperatorPrior4Basic())
		return;

	CDlgBasicDeleteName dlg(GetViewSectionNo(),GetDocument(),this); 
	dlg.DoModal();
}

void CTgEditView::OnUpdateBasicDeleteName(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnUpdateBasicGraphchange(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_DISPATCHER && GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			if(gpcdef.GetOperatorPrior4Basic())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnBasicGraphchange()
{
	if(gpcdef.GetUserIdentify() == GPC_DISPATCHER)
	{
		if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
			return;
		if(!gpcdef.GetOperatorPrior4Basic())
			return;
	}
	else if(gpcdef.GetUserIdentify() == GPC_TEST)
	{
		if(!gpcdef.GetOperatorPrior4Basic())
			return;
	}
	else
	{
		return;
	}

	if(mServer_nosync_count > 0 || mMmi_nosync_count > 0)
	{
		CString msg;
		msg.Format("����δͬ������ ������:%d  ����:%d, ��ȴ�ͬ������Ϊ0�ٻ�ͼ", 
			mServer_nosync_count, mMmi_nosync_count);
		AfxMessageBox(msg);
		return;
	}

	CDlgBasicChange dlg(GetDocument(), GetViewScheduleType()); 
	dlg.DoModal();
	if(dlg.m_bSucceed)
	{
		CWintg* pApp=(CWintg *)AfxGetApp();
		if(dlg.m_nObjSchdType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			pApp->ClearTGOCXSchedule(m_schdinfo.m_nSectionID, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);

			pholder->ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,TRUE); 
		}
		else if(dlg.m_nObjSchdType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			pApp->ClearTGOCXSchedule(m_schdinfo.m_nSectionID, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);

			pholder->ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV,TRUE); 
		}

	}
}

// ����ͼ�޸Ŀ���
void CTgEditView::OnModifyBschd()
{
	// TODO: �ڴ���������������
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
}

void CTgEditView::OnUpdateModifyBschd(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
	if(nCanModify > 0)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
void CTgEditView::OnDivideportLatetime()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain srctrain=*(CTG_SingleTrain *)pobj;

		CDlgDividePortLateTime dlg(&srctrain, this);

		if(dlg.DoModal()==IDOK)
		{
			m_tgmmi.UpdateTrain_Directly(srctrain);
		}
	}
}

void CTgEditView::OnUpdateTrainPlanSideMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
void CTgEditView::OnTrainPlanSideMan()
{
	if(m_pTrainSideDlg)
	{
		m_pTrainSideDlg->SetDocument(GetDocument(), 0);
		m_pTrainSideDlg->InitDlgCtrlItem();
		m_pTrainSideDlg->ShowWindow(SW_SHOW);
	}
}
void CTgEditView::OnTrainPlanRouteMan()
{
	if(m_pTrainRouteDlg)
	{
		m_pTrainRouteDlg->SetDocument(GetDocument(), GetViewScheduleType(), 0);
		m_pTrainRouteDlg->InitDlgCtrlItem();
		m_pTrainRouteDlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnUpdateTrainPlanRouteMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnTrainPlanCompareMan()
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
	{
		if(m_pBasicSchdCompareDlg!=NULL)
		{
			m_pBasicSchdCompareDlg->InitDlgCtrlItem();
			m_pBasicSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		if(m_pShiftSchdCompareDlg!=NULL)
		{
			m_pShiftSchdCompareDlg->InitDlgCtrlItem();
			m_pShiftSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		if(m_pWorkSchdCompareDlg!=NULL)
		{
			m_pWorkSchdCompareDlg->InitDlgCtrlItem();
			m_pWorkSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
}

void CTgEditView::OnUpdateTrainPlanCompareMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnDeleteTrainByIndex() //20130222hjz ����������ɾ���г�����
{
	if (GetDocument()->IsQueryMode() || !UR_CanOperateSchedule())
	{
		AfxMessageBox("û�в���Ȩ��!");
		return;
	}
	/////////////////////////////����������ɾ���г�//////////////////////////////////
	CDlgDeleteTrainByIndex dlg = new CDlgDeleteTrainByIndex();
	if (dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTrainIndex!=0)
		{
			//////////////////////////////////////////////////////
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();

			CTG_SingleTrain *pTrain;
			pTrain = pdata->GetSingleTrain(dlg.m_nTrainIndex);
			if (!pTrain)
			{
				AfxMessageBox("�����ڴ����г�,�����¼���г������ź��ٽ���ɾ������!");
				return;
			}
			else
			{
				m_tgmmi.DeleteTrain_Directly(dlg.m_nTrainIndex);
				return;
			}
		}
		else
			AfxMessageBox("δ���복��������!");
	}
}
void CTgEditView::OnDeleteMark()
{
	// TODO: �ڴ���������������
	mTG.AliveWindow(2);
}

void CTgEditView::OnDeleteInvalidTrain()
{
	// TODO: �ڴ���������������
	mTG.AliveWindow(8);
}

void CTgEditView::OnSaveBasicSideWorkInfo()
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		AfxMessageBox("��ע��, ֻ�ܱ��泣�û���ͼ��Ϣ"); 
		return;
	}

	if(!gpcdef.GetOperatorPrior4Basic())
	{
		AfxMessageBox("��û�б������ͼ��Ϣ��Ȩ��"); 
		return;
	}
	for(int i=0; i<3; i++)
	{
		CSuperPassDlg pdlg("��������");
		if (pdlg.DoModal() == IDOK)
		{
			if (!pdlg.PasswordIsOk())
			{
				AfxMessageBox("������󣡣���"); 
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			return;
		}
	}
	if(i>=3)
		return;

	CTgDocument  *pDoc  = GetDocument();

	CString fileDir, fileName;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());
	fileName.Format("section_basic%02d.xml",pDoc->GetCurrentSectionNo());

	bool bFindInfoFile=false;
	CFileFind  fileFind;
	BOOL flag = fileFind.FindFile(fileDir+"\\*.xml");
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString strName = fileFind.GetFileName();
		if(strName == fileName)
		{
			bFindInfoFile=true;
			break;
		}
	}

	//����ļ���
	if(bFindInfoFile)
	{
		CString cap;
		cap.Format("�ڱ���Ŀ¼%s��,������Ҫ�����ͬ���ļ� %s, ��ȷ�ϸ�����?",fileDir, fileName);
		if(MessageBox(cap, "��ʾ", MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			AfxMessageBox("�ļ�����ʧ��!!");
			return;
		}
	}

	if(MessageBox("��ע��, ��ȷ�ϱ��泣�û���ͼ��Ϣ��?","����",MB_YESNO)==IDYES)
	{
		fileName = fileDir + "\\" + fileName;
		if(!pDoc->SaveTrainSideTechToFile(GetViewScheduleType(),fileName))
			return;
		AfxMessageBox("�ļ������� "+fileName);
	}
}

void CTgEditView::OnLoadBasicSideWorkInfo()
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return;

	for(int i=0; i<3; i++)
	{
		CSuperPassDlg pdlg("��������");
		if (pdlg.DoModal() == IDOK)
		{
			if (!pdlg.PasswordIsOk())
			{
				AfxMessageBox("������󣡣���"); 
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			return;
		}
	}
	if(i>=3)
		return;

	if(MessageBox("��ע��, ��ȷ��ƥ��Ԥ������ͼ��Ϣ��?","����",MB_YESNO)!=IDYES)
		return;

	CDlgWait *pw=new CDlgWait("���ڼ��ػ���ͼ��Ϣ,������Ҫ1-2����ʱ��,���Ժ�...");

	CTgDocument  *pDoc  = GetDocument();

	CString fileDir, fileName;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());
	fileName.Format("section_basic%02d.xml",pDoc->GetCurrentSectionNo());
	fileName = fileDir + "\\" + fileName;

	pDoc->LoadTrainSideTechToFile(fileName);

	// ��ʼƥ��
	CString strMsg="ƥ���г�: ";
	CTG_TrainScheduleBase *pData=pDoc->GetScheduleData(GetViewScheduleType());
	if(pData)
	{
		CTG_TrainPtrGroup  group;
		pData->FindTrains(group);

		for(int i=0; i<group.GetTrainCount(); i++)
		{
			const CTG_SingleTrain* oldtrain = group.GetTrainPtr(i);
			if(NULL == oldtrain)
				continue;

			CTG_SingleTrain train = *oldtrain;
			pw->UpdateDisplay(strMsg+train.GetTrainID());

			if(pDoc->InitBasicInfo(&train))
			{
				m_tgmmi.UpdateTrain_ByBasicInfo(train);
			}
			else
			{
				GpcLogTrainMessage(0, 0, "ƥ�����ͼ��Ϣ, %s ʧ��", train.GetTrainID());
			}
		}
	}

	pDoc->ReleaseBasicTrainInfo();
	delete pw;
}

void CTgEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	short type = nChar;
	if(type == nChar) // ���ݶ�ʧ
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::KEY_DOWN, type);

	CTgView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CTgEditView::OnAutoAdjustCol()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			return;
		if(prec->GetStationNo()==station)
			m_tgmmi.AdjustTrainForCollision(train, rec_index);
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK) // �Զ����ݱ�ǵ����г�
	{
		CTG_SingleMark *pmark=(CTG_SingleMark *)pobj;
		m_tgmmi.AdjustTrainsForMark(pmark);
	}
}

void CTgEditView::OnAutoAdjustPass()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			return;
		if(prec->GetStationNo()==station)
			m_tgmmi.AdjustTrainForMutiPass(train, rec_index);
	}
}
void CTgEditView::OnAutoAdjustMulti()
{
	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	CDlgMultiAdjust dlg(pData,this);
	if(dlg.DoModal()==IDOK)
	{
		TRAIN_INDEX  trainidxs[100];
		int traincount=dlg.GetSelectedTrains(trainidxs,100);
		for(int i=0;i<traincount;i++)
		{
			CTG_SingleTrain train,*ptrain;
			ptrain=pData->GetSingleTrain(trainidxs[i]);
			if(ptrain)
				ptrain->SetAdjustingFlag(TRUE);  // ��������ʱ���ñ�־���Ա���ͬ�ȼ��г�����
		}

		for(int pri=0;pri<=MAX_PRIORITY_VALUE;pri++)  // ���ȸ����ȼ��г�,priorityֵС���г�
		{
			for(int i=0;i<traincount;i++)
			{
				if(trainidxs[i]==NULL_TRAIN_INDEX)
					continue;

				CTG_SingleTrain train,*ptrain;
				ptrain=pData->GetSingleTrain(trainidxs[i]);
				if(!ptrain)
					continue;
				if(ptrain->GetPriority()!=pri)
					continue;
				train=*ptrain;
				m_tgmmi.AdjustTrainForCollision(train,0);

				ptrain=pData->GetSingleTrain(trainidxs[i]);
				if(ptrain)
					ptrain->SetAdjustingFlag(FALSE);  // ��������ʱ���ñ�־���Ա���ͬ�ȼ��г�����

				trainidxs[i]=NULL_TRAIN_INDEX;
			}
		}
	}
}

void CTgEditView::OnOptAutoAdjustCollision()
{
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
	{
		CString tmp;
		tmp.Format("�ò��������� <���Զ�����ģʽ> ģʽ���в���,ȷ�ϼ�����?");

		if(MessageBox(tmp,"����",MB_YESNO)!=IDYES)
			return;

		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ���Զ�������ť, ʹ��<���Զ�����ģʽ>���в���");
		GetTGFunctionConfig()->SetAutoAdjustOperFlag(0);
	}
	else
	{
		CString tmp;
		tmp.Format("�ò��������� <�Զ�����ģʽ> ģʽ���в���,�����޸��г������ߵĵ���ʱ�̺͹ɵ�,���������ߺ�Ӧ���˶�,ȷ�ϼ�����?");

		if(MessageBox(tmp,"����",MB_YESNO)!=IDYES)
			return;

		GpcLogTrainMessage(0, 0, "[����Ա����] ͨ�����ѡ���Զ�������ť, ʹ��<�Զ�����ģʽ>���в���");
		GetTGFunctionConfig()->SetAutoAdjustOperFlag(1);
	}
}

void CTgEditView::OnUpdateOptAutoAdjustCollision(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnUpdateDirectSetAutotrig(CCmdUI *pCmdUI)
{

	// TODO: �ڴ������������û����洦��������
}

void CTgEditView::OnDirectSetAutotrig()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	do
	{
		if(!pobj)
			break;
		if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
			break;
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			break;
		if(prec->GetStationNo()!=station)
			break;
		
		if(GetTGFunctionConfig()->IsNotThisSectionStation(m_schdinfo.m_nSectionID, prec->m_data.station))
		{
			CString str;
			str.Format("��ע��, %s �����ڱ�̨��Ͻ,�����޸��Դ�", gpcdef.GetStationName(prec->m_data.station));
			MessageBox(str, "��ʾ", MB_OK);
			GpcLogTrainMessage(0, 0, "%s",  str);
			return;
		}
		DirectSetAutotrig(train, rec_index);
		return;
	}while(0);
	MessageBox("��ѡ�񳵴��ϵĳ�վ�ڵ�,Ȼ���ٲ���.","��ʾ");
}

void CTgEditView::DirectSetAutotrig(CTG_SingleTrain& train, const int rec_index)
{
	CDlgDirectSetTrigFlag  dlg(this);
	dlg.SetTrain(&m_tgmmi,&train,rec_index);
	if(dlg.DoModal()==IDOK)
	{
		m_tgmmi.UpdateTrain_Directly(train);
	}
}

void CTgEditView::OnPredictStpc()
{
	if(!UR_CanOperateSchedule())
	{
		MessageBox("��û��Ԥ��Ȩ��", "��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if(m_predictDlg)
	{
		m_predictDlg->ShowWindow(SW_SHOW);
	}

	return;
}

void CTgEditView::OnTimer(UINT nIDEvent)
{
	//if(nIDEvent == 198302)
	//{
	//	// ֻ�����棬���������
	//	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	//	if(!pholder)
	//	{
	//		GetDocument()->OpenWaitWindow(NULL);
	//		KillTimer(198302);
	//	}
	//	else
	//	{
	//		int m_count=pholder->GetNoSyncedObjectCount(m_schdinfo.m_nType,&m_tgmmi);
	//		if(m_count<=0)
	//		{
	//			mTG.AliveWindow(16);
	//			GetDocument()->OpenWaitWindow(NULL);

	//			KillTimer(198302);
	//		}
	//	}
	//}
	if(nIDEvent == 198311)
	{
		if(m_pPredictServer!=NULL)
			m_pPredictServer->SaveStplanToHistory();
	}
	
	CTgView::OnTimer(nIDEvent);
}

void CTgEditView::OnTurnReality()
{
	if(gpcdef.IsInhibitManualConvertActualTime())
	{
		AfxMessageBox("��ֹ�޸�ʵ�ʵ�,����תʵ��");
		GpcLogTrainMessage(0, 0, "��ֹ�޸�ʵ�ʵ�,����תʵ��");
	}
	else
	{
		mTG.TurnReality();
	}
}

void CTgEditView::OnReceivePoint()
{
	mTG.ReceivePoint();
}

void CTgEditView::OnUpdateReceivePoint(CCmdUI* pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 

	pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::RECEIVE_POINT));
}

void CTgEditView::OnGenAllReturnTrainid()
{
	// TODO: �ڴ���������������
	GpcLogTrainMessage(0, 0, "[����Ա����] ���� ���������۷����� ����");
	mTG.AliveWindow(16);
}

void CTgEditView::OnEditAllCzh()
{
	// TODO: �ڴ���������������
	GpcLogTrainMessage(0, 0, "[����Ա����] ���� ��������༭ ����");
	mTG.AliveWindow(32);
}

// �հ�ƻ��޸Ŀ���
void CTgEditView::OnModifyShiftschd()
{
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
}

void CTgEditView::OnUpdateModifyShiftschd(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR) && GetTGFunctionConfig()->IsEnableCzh())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
	if(nCanModify > 0)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnUpdateEditAllCzh(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		pCmdUI->Enable(FALSE);
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
		{
			short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
			if(nCanModify>0)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
		{
			short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
			if(nCanModify>0)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnUpdateGenAllReturnTrainid(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->IsChangeTrainID())
	{
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			pCmdUI->Enable(FALSE);
		}
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
			{
				short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
				if(nCanModify>0)
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
			{
				short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
				if(nCanModify>0)
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnSetMultiSideLock()
{
	mTG.AliveWindow(64);
}

void CTgEditView::OnUpdateSetMultiSideLock(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR))
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnQuickModifyMultiTrains()
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR))
	{
		mTG.AliveWindow(128);
	}
}

void CTgEditView::SetLateEarlyTimeRd(LPCTSTR xml, BYTE flag, ULONG shiftid)
{
	CString str= xml;
	std::map<CString, LateInfo>::iterator it = m_earlylatetimelog.find(str);

	if(it != m_earlylatetimelog.end())
	{
		it->second.flag=flag;
	}
	else
	{
		LateInfo info;
		info.flag=flag;
		info.shiftid=shiftid;

		m_earlylatetimelog[str]= info;
	}
	mTG.AppendEarlyLateInfo(str, flag, shiftid);

	if(flag&0x3)
		sysprint(10,0, "����Ա�����г� %s �����ͳ��:%s %s", str, (flag&0x01)?"����":"",  (flag&0x02)?"����":"");
	else if(flag&0x4)
		sysprint(10,0, "����Ա�����г� %s ��ͳ�������", str);

	SaveEarlylatetimeFile();
}


extern const char *TG_GetScheduleBaseFilePath(const CTG_ScheduleInfo  &schdinfo);
bool CTgEditView::LoadEarlylatetimeXML()
{
	ULONG shiftids[5];
	shiftids[0]=Get_CurrentShiftid();
	shiftids[1]=Get_PreShiftid(shiftids[0]);
	shiftids[2]=Get_PreShiftid(shiftids[1]);
	shiftids[3]=Get_NextShiftid(shiftids[0]);
	shiftids[4]=Get_NextShiftid(shiftids[3]);

	CString strFilename;
	strFilename.Format("%s\\early_late_time_set.xml", TG_GetScheduleBaseFilePath(m_schdinfo));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFilename) ) )
	{
		return false; 
	}

	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode,pChildNode, pRoot;
	pRoot=pXMLDoc->selectSingleNode("root");

	MSXML::IXMLDOMNodeListPtr pList = pRoot->selectNodes( "record");
	if(pList)
	{
		for(int idx = 0; idx < pList->Getlength(); idx++)
		{
			pNode = pList->Getitem(idx);
			if(pNode)
			{
				CString key;
				BYTE val=0;
				ULONG shiftid=0;
				pChildNode=pNode->selectSingleNode("key");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					key = v.bstrVal;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode=pNode->selectSingleNode("tm");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					shiftid = atol(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode=pNode->selectSingleNode("val");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					val = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				for(int i=0; i<5; i++)
				{
					if(shiftids[i]==shiftid)
						break;
				}
				if( i>=5)
				{
					continue;
				}

				if(key!="" && val>0 && shiftid>0)
				{
					mTG.AppendEarlyLateInfo(key, val, shiftid);

					LateInfo info;
					info.flag=val;
					info.shiftid=shiftid;
					m_earlylatetimelog.insert(make_pair(key, info));
				}
			}
		}
	}

	return true;
}

void CTgEditView::SaveEarlylatetimeFile()
{
	ULONG shiftids[5];
	shiftids[0]=Get_CurrentShiftid();
	shiftids[1]=Get_PreShiftid(shiftids[0]);
	shiftids[2]=Get_PreShiftid(shiftids[1]);
	shiftids[3]=Get_NextShiftid(shiftids[0]);
	shiftids[4]=Get_NextShiftid(shiftids[3]);

	CString strFilename;
	strFilename.Format("%s\\early_late_time_set.xml",  TG_GetScheduleBaseFilePath(m_schdinfo));
	CStdioFile file;
	if(file.Open(strFilename, CFile::modeCreate|CFile::modeWrite))
	{
		file.WriteString("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
		file.WriteString("<root>\n");
		std::map<CString, LateInfo>::iterator it = m_earlylatetimelog.begin();
		for(; it!=m_earlylatetimelog.end(); it++)
		{
			for(int i=0; i<5; i++)
			{
				if(shiftids[i]==it->second.shiftid)
					break;
			}
			if( i>=5)
			{
				continue;
			}

			CString xml;
			file.WriteString("\t<record>\n");	
			xml.Format("\t\t<key>%s</key>\n", it->first);
			file.WriteString(xml);
			xml.Format("\t\t<tm>%d</tm>\n", it->second.shiftid);
			file.WriteString(xml);
			xml.Format("\t\t<val>%d</val>\n", it->second.flag);
			file.WriteString(xml);
			file.WriteString("\t</record>\n");
		}
		file.WriteString("</root>\n");
		file.Close();
	}
}

extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
void CTgEditView::MutiTrainOper(LPCTSTR xml, ULONG nOperFlag)
{
	// ֻ�������������ͼ
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;
	CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData();
	if(NULL==pData)
		return;

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return;
	}
	
	_variant_t v;
	
    MSXML::IXMLDOMNodePtr pNode, pChildNode;
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "root");
	if(pRootNode == NULL)
		return;
	
	std::vector<DWORD> list_index;
	MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "TRAIN_INDEX" );
	for(int idx = 0; idx < pRecAbsList->Getlength(); idx++)
	{
		pNode = pRecAbsList->Getitem(idx);
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			DWORD train_index = atol(CString(v.bstrVal));
			list_index.push_back(train_index);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	if(list_index.size() == 0)
		return;

	CTG_ScheduleInfo schdinfo;	
	pData->GetScheduleInfo(schdinfo); 
	
	if(nOperFlag==0x00000001) // ���������ɵ�
	{
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				bool bSend=false;
				CTG_SingleTrain train=*pTrain1;
				for(int i=0; i<train.GetTrainRecordCount(); i++)
				{
					CTG_TrainRecord *prec = train.GetTrainRecordNoDelete(i);
					if(NULL != prec)
					{
						if(prec->IsLockSide())
							continue;
						if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
						{
							sysprint(4,0,"[�����ɵ�����] ����Ա�����г�:%s �ڳ�վ:%s ����ʵ�ʵ�, �ɵ�������, �ɵ�:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
						else
						{
							prec->LockSide();
							bSend=true;
							sysprint(4,0,"[�����ɵ�����] ����Ա�����г�:%s �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
					}
				}
				if(bSend)
				{
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
				}
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000002) // ���������ɵ�
	{
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				bool bSend=false;
				CTG_SingleTrain train=*pTrain1;
				for(int i=0; i<train.GetTrainRecordCount(); i++)
				{
					CTG_TrainRecord *prec = train.GetTrainRecordNoDelete(i);
					if(NULL != prec)
					{
						if(!prec->IsLockSide())
							continue;
						if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
						{
							sysprint(4,0,"[�����ɵ�����] ����Ա�����г�:%s �ڳ�վ:%s ����, ����ʵ�ʵ�, �ɵ�������, �ɵ�:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
						else
						{
							prec->UnLockSide();
							bSend=true;
							sysprint(4,0,"[�����ɵ�����] ����Ա�����г�:%s �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
					}
				}
				if(bSend)
				{
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
				}
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000003) // �����г�ƫ��
	{
		int moveMin=0,begin_sta_index=-1, end_sta_idx=-1, begin_tm=0, end_tm=0;
		pNode = pRootNode->selectSingleNode("movetm");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			moveMin = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			pChildNode=pNode->selectSingleNode("@begin_sta_index");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_sta_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_sta_idx");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_sta_idx = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@begin_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		if(moveMin==0 || begin_sta_index==-1 || end_sta_idx==-1 || begin_tm==0 || end_tm==0)
			return;

		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.ModifyTrainRunTimeByMinute(begin_sta_index, end_sta_idx, begin_tm, end_tm,moveMin);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000004) // ���������г�
	{
		int percent=0,begin_sta_index=-1, end_sta_idx=-1, begin_tm=0, end_tm=0;
		pNode = pRootNode->selectSingleNode("percent");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			percent = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			pChildNode=pNode->selectSingleNode("@begin_sta_index");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_sta_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_sta_idx");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_sta_idx = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@begin_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		if(percent==0 || begin_sta_index==-1 || end_sta_idx==-1 || begin_tm==0 || end_tm==0)
			return;

		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.ModifyTrainRunTimeByPercent(begin_sta_index, end_sta_idx, begin_tm, end_tm,percent);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000005) // ����ƽ���г�
	{
		int offset=0;
		pNode = pRootNode->selectSingleNode("offset");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			offset = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(offset==0)
			return;
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.OffsetScheduleTime(0,offset,FALSE,1);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

// ����shift id,��ȥ�ظ�
void CTgEditView::AddTdmsTrainGuid(DWORD guid)
{
	for(int n=0; n<recieved_tdms_trains.size(); n++)
	{
		if(recieved_tdms_trains[n]==guid)
		{
			CString waitstr;
			if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				waitstr.Format("����tdms����ͼʱ���� GTID %u �ظ�", guid);
			else
				waitstr.Format("����tdms�հ�ƻ�ʱ���� GTID %u �ظ�", guid);
			GpcLogTrainMessage(0,0, waitstr);
			break;
		}
	}
	if(n>=recieved_tdms_trains.size())
	{
		recieved_tdms_trains.push_back(guid);
	}
}

void CTgEditView::OnDisplaySideReqChangeDlg(CLBTH::MsgChangeSideReq *pChangeSideReq, int station)
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;
	CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData();
	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);
	CDlgStationReqChangeSide dlg(station);
	for(int i=0; i<pChangeSideReq->count&&i<256; i++)
	{
		NodeSideInfo info;
		info.train_index = pChangeSideReq->train_index[i];
		info.rec_index = pChangeSideReq->rec_index[i];
		info.a_side = pChangeSideReq->arrive_side[i];
		info.d_side = pChangeSideReq->depart_side[i];
		GpcLogTrainMessage(0,0, "��%d��: train_index=%u, rec_index=%d, arrive_side=%d, depart_side=%d",
			i+1, info.train_index, info.rec_index, info.a_side, info.d_side);

		CTG_SingleTrain *ptrain=pData->GetSingleTrain(info.train_index);
		if(ptrain==NULL)
		{
			GpcLogTrainMessage(0,0, "����train_index=%u �Ҳ���������, �޷��޸�", info.train_index);
			continue;
		}
		CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(info.rec_index);
		if(prec==NULL)
		{
			GpcLogTrainMessage(0,0, "����rec_index=%d �Ҳ��������߳�վ, �޷��޸�", info.rec_index);
			continue;
		}
		if(prec->GetStationNo()!=station)
		{
			GpcLogTrainMessage(0,0, "����rec_index=%d �ҵ���վ��ʵ������վ��ͬ, �޷��޸�", info.rec_index);
			continue;
		}

		info.trainid=ptrain->GetTrainID();
		const char* pstr=GetTGBasicParam()->GetSideNameByID(station, info.a_side);
		if(pstr==NULL)
			info.s_aside="?G";
		else
			info.s_aside=pstr;

		pstr=GetTGBasicParam()->GetSideNameByID(station, info.d_side);
		if(pstr==NULL)
			info.s_dside="?G";
		else
			info.s_dside=pstr;

		GpcLogTrainMessage(0,0, "�ɹ������� train_name=%s %s-%s", 
			info.trainid, info.s_aside, info.s_dside);
		dlg.AddTrainSideRecord(info);
	}

	if(dlg.DoModal()==IDOK)
	{
		GpcLogTrainMessage(0,0, "��ʼ�޸Ĺɵ�");
		for(int i=0; i<dlg.m_count; i++)
		{
			TRAIN_INDEX train_index=dlg.siderecord[i].train_index;
			int rec_index = dlg.siderecord[i].rec_index;

			CTG_SingleTrain *ptrain=pData->GetSingleTrain(train_index);
			if(ptrain==NULL)
			{
				GpcLogTrainMessage(0,0, "���� train_index=%u �Ҳ���������, �޷��޸�", train_index);
				continue;
			}
			CTG_SingleTrain train=*ptrain;
			CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
			if(prec==NULL)
			{
				GpcLogTrainMessage(0,0, "���� rec_index=%d �Ҳ��������߳�վ, �޷��޸�", rec_index);
				continue;
			}
			if(prec->GetStationNo()!=station)
			{
				GpcLogTrainMessage(0,0, "���� rec_index=%d �ҵ���վ %d ��ʵ������վ %d ��ͬ, �޷��޸�", 
					rec_index,prec->GetStationNo()!=station);
				continue;
			}

			prec->m_data.arrive_side=dlg.siderecord[i].a_side;
			prec->m_data.depart_side=dlg.siderecord[i].d_side;
			GpcLogTrainMessage(0,0, "�޸ĳ��� %s �ڳ�վ %d �ɵ� %d-%d", 
				train.GetTrainID(), prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);

			pData->UpdateTrain(schd_info, train, SYCFLAG_NULL);
		}
		GpcLogTrainMessage(0,0, "�����޸Ĺɵ�");
	}
}

// �����޸Ĺɵ�
void CTgEditView::BatchManualModifySide(TRAIN_INDEX train_index)
{
	// TODO: �ڴ���������������
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;

	CTG_TrainScheduleBase *pData = NULL;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		pData=pHolder->GetWorkScheduleData();
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
		pData=pHolder->GetBasicPrevScheduleData();
	else
		pData = NULL;

	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);

	CTG_SingleTrain *pTrain=pData->GetSingleTrain(train_index);
	if(!pTrain)
		return;
	
	CTG_SingleTrain train=*pTrain;

	GpcLogTrainMessage(0,0, "����Ա��ʼ�����޸ĳ���%s(%u)�Ĺɵ�", train.GetTrainID(), train_index);
	GpcLogTrainMessage(0,0, "�޸�ǰ%s�����ĳ�վ�ɵ�����:", train.GetTrainID());

	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		GpcLogTrainMessage(0,0, "��վ %d �ɵ� %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
	}

	// ���ݼƻ��·����������Щ��վΪ��̨��Ͻ
	std::vector<int> station_list;
	GetDocument()->GetSTPLAN()->GetSendPlanStation(station_list);
	
	CString strCaption;
	strCaption.Format("�����޸Ĺɵ�����%s", train.GetTrainID());
	DlgBatchSideModify dlg(train, strCaption, pHolder->GetBasicScheduleData(), pData, this);
	dlg.SetThisStation(station_list); // ���ñ�̨��Ͻ��վ
	if(dlg.DoModal()==IDOK)
	{
		GpcLogTrainMessage(0,0, "�޸ĺ�%s�����ĳ�վ�ɵ�����:", dlg.m_train.GetTrainID());
		for(int n=0; n<dlg.m_train.GetTrainRecordCount(); n++)
		{
			const CTG_TrainRecord *prec=dlg.m_train.GetTrainRecordByRecIndex_Const(n);
			if(prec==NULL)
				return;
			GpcLogTrainMessage(0,0, "��վ %d �ɵ� %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
		}

		pData->UpdateTrain(schd_info, dlg.m_train, SYCFLAG_NULL);
	}
	else
	{
		GpcLogTrainMessage(0,0, "����Աȡ������ %s �Ĺɵ������޸�", train.GetTrainID());
	}
}

// ��������δ֪�ɵ�
void CTgEditView::OnBatchAllocationSide()
{
	// TODO: �ڴ���������������
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
	{
		MessageBox("��ѡ�������߲���");
		return;
	}
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
	{
		MessageBox("��ѡ�������߲���");
		return;
	}

	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;

	CTG_TrainScheduleBase *pData = NULL;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		pData=pHolder->GetWorkScheduleData();
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
		pData=pHolder->GetBasicPrevScheduleData();
	else
		pData = NULL;

	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);

	CTG_SingleTrain train=*((CTG_SingleTrain*)pobj);

	GpcLogTrainMessage(0,0, "����Ա��ʼ�������ɳ���%s(%u)�Ĺɵ�", train.GetTrainID(), train.GetTrainIndex());
	GpcLogTrainMessage(0,0, "����ǰ%s�����ĳ�վ�ɵ�����:", train.GetTrainID());

	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		GpcLogTrainMessage(0,0, "��վ %d �ɵ� %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
	}

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pHolder->GetBasicScheduleData(), pData);
	bool bAlloctionSide=false;
	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		int station=prec->GetStationNo();
		int sideno=prec->m_data.arrive_side;
		int sideno2=prec->m_data.depart_side;
		
        const char* p=GetTGBasicParam()->GetSideNameByID(station, sideno);
		const char* p2=GetTGBasicParam()->GetSideNameByID(station, sideno2);
		if(n==0)
		{
			if(prec->IsStartRec())
			{	
				if(NULL == p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
			else
			{
				if(NULL==p || NULL==p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
		}
		if(n+1==train.GetTrainRecordCount())
		{
			if(prec->IsTerminalRec())
			{
				if(NULL == p)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
			else
			{
				if(NULL==p || NULL==p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
		}

		if(n>0 && n<train.GetTrainRecordCount())
		{
			if(NULL==p || NULL==p2)
			{
				bAlloctionSide=true;
				adjust_logic.ChangeSideForStopOrPass(&train, n);
			}
		}
	}
	
	// ����Զ�������,��ʾ���û�
	if(bAlloctionSide)
	{
		CString strCaption;
		strCaption.Format("����δ֪�ɵ�����ɵ�����%s", train.GetTrainID());
		DlgBatchSideModify dlg(train, strCaption, pHolder->GetBasicScheduleData(), pData, this);
		if(dlg.DoModal()==IDOK)
		{
			GpcLogTrainMessage(0,0, "���յ���Աȷ��%s�����ĳ�վ�ɵ�����:", dlg.m_train.GetTrainID());
			for(int n=0; n<dlg.m_train.GetTrainRecordCount(); n++)
			{
				const CTG_TrainRecord *prec=dlg.m_train.GetTrainRecordByRecIndex_Const(n);
				if(prec==NULL)
					return;
				GpcLogTrainMessage(0,0, "��վ %d �ɵ� %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
			}

			pData->UpdateTrain(schd_info, dlg.m_train, SYCFLAG_NULL);
		}
		else
		{
			GpcLogTrainMessage(0,0, "����Աȡ������ %s �������ɵ�����", train.GetTrainID());
		}
	}
	else
	{
		GpcLogTrainMessage(0,0, "%s(%u)������վû��δ����Ĺɵ�,����Ҫ��������ɵ�.", train.GetTrainID(), train.GetTrainIndex());
		CString str;
		str.Format("%s������վû��δ����Ĺɵ�,����Ҫ��������ɵ�",train.GetTrainID());
		MessageBox(str, "����");
	}
}

void CTgEditView::OnUpdateBatchAllocationSide(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		if(!CheckUserTGRight()) 
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			int gpcid = gpcdef.GetUserGpcId();
			RIGHT right;
			if(!m_load.GetUserRight(gpcid, right))
			{
				pCmdUI->Enable(FALSE); 
			}
			if(right.operate==1 || right.operate==3)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE); 
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

bool CTgEditView::IsAddTrain(CString trainId)
{
	CString strText, strTitle;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(pHolder)
	{
		strText = pHolder->NewTrainNoIsExist(trainId, strTitle);
	}

	if(strText != "")
	{
		strText = "����"+strText+"�Ѿ����ڣ��Ƿ��ظ��ӳ�?";
		if(MessageBox(strText,strTitle,MB_YESNO)!=IDYES)
			return false; // ���ӳ�
	}

	return true; // �ӳ�
}
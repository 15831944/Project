// TestTgView.cpp : CTgEditView ���ʵ��
//

#include "stdafx.h"
#include "tg.h"
#include "resource.h"
#include "TgEditView.h"
#include "sysshare.h"
#include "if.h"


#include "function.h"
#include ".\tgeditview.h"

#include "InputHatTrain.h"

#include "tg_binxml.h"
#include "..\SuperPassDlg.h"
#include "utility\mmi_function_config.h"
#include "InsertTrainDlg.h"
#include "InsertTrainDlgLanzhou.h"

#include "DlgTrainRecord.h"
#include "dlgwait.h"

#include "DlgBasicChange.h"
#include "DlgBasicParam.h"
#include "DlgDeleteTrainByIndex.h"
#include "DlgBasicDeleteName.h"

#include "DlgTDMSBasicTip.h"
#include "DlgTDMSBasicGen.h"
#include "DlgConfirm.h"
#include "DlgBasicTrainDeleteChoice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CLoadGPCDef m_load;
extern int DB_GetEntityStatus(long eid);

extern BOOL UR_CanOperateBasicSchedule();
extern BOOL UR_CanOperateSchedule();

// CTgEditView
IMPLEMENT_DYNCREATE(CTgEditView, CTgView)

BEGIN_EVENTSINK_MAP(CTgEditView, CTgView)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 4, APPEND_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 5, UPDATE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 6, TG_DELETE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 14, TG_FIRE_DELETE, VTS_BSTR)
END_EVENTSINK_MAP()
// CTgEditView ����/����
BEGIN_MESSAGE_MAP(CTgEditView, CTgView)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()

	ON_COMMAND(ID_INSERT_NEW_TRAIN, OnInsertNewTrain)
	

	ON_COMMAND(ID_DRAW_NEW_TRAIN_END, OnEndInsertNewTrain)
	

	ON_COMMAND(IDC_DEL_MULTI_TRAIN, OnDelMultiTrain)
	
	
	ON_COMMAND(ID_DRAW_GOON, OnDrawGoon)
	

	ON_COMMAND(ID_DRAW_RETURN, OnDrawReturn)
	

	ON_COMMAND(ID_DRAW_ABSTRAIN_A, OnDrawAbstrainA)
	

	ON_COMMAND(ID_DRAW_ABSTRAIN_B, OnDrawAbstrainB)
	

	ON_COMMAND(ID_BASIC_GET_TRAIN_FROM_OPMS,OnGetBasicTrainFromOpms)

	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)

	ON_COMMAND(ID_BASIC_DELETE_ALL, OnDeleteAllBasic)

	
	ON_COMMAND(ID_LOAD_BASIC_SIDE_WORK, OnLoadBasicSideWorkInfo)

	ON_COMMAND(ID_GEN_TG_NODE_INDEX,OnGenNodeIndex)
	ON_UPDATE_COMMAND_UI(ID_GEN_TG_NODE_INDEX,OnUpdateGenNodeIndex)

	ON_COMMAND(ID_BASIC_NAMEING, OnBasicNameing)
	

	ON_COMMAND(ID_BASIC_DELETENAME, OnBasicDeleteName)
	

    ON_COMMAND(ID_MODIFY_BSCHD, OnModifyBschd)
	

	ON_COMMAND(ID_TRAINPLAN_SIDE_MAN, OnTrainPlanSideMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_SIDE_MAN, OnUpdateEnableEdit)

	ON_COMMAND(ID_TRAINPLAN_ROUTE_MAN, OnTrainPlanRouteMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_ROUTE_MAN, OnUpdateEnableEdit)

	ON_COMMAND(ID_BASIC_GRAPHCHANGE, OnBasicGraphchange)
	
	
	ON_COMMAND(ID_EDIT_ALL_CZH, OnEditAllCzh)
	

	ON_COMMAND(ID_GEN_ALL_RETURN_TRAINID, OnGenAllReturnTrainid)
	
	
	ON_COMMAND(ID_SEL_TDMS_TRAIN_GEN_BASIC, OnSelTdmsTrainsGenBasic)
	
	
	ON_COMMAND(ID_TRAINPLAN_COMPARE, OnTrainPlanCompareMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_COMPARE, OnUpdateEnableEdit)
	
	ON_COMMAND(ID_TRAINPLAN_SIDE_YINGYE_EDIT, OnSideYingyeEdit)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_SIDE_YINGYE_EDIT, OnUpdateEnableEdit)

END_MESSAGE_MAP()


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
		}
	}
	
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

CTgEditView::CTgEditView()
{
	m_pTrainSideDlg=NULL;
	m_pTrainRouteDlg=NULL;
	m_pBasicSchdCompareDlg=NULL;
	m_pSideYingyeDlg=NULL;

	m_bEnableProcessTdmsBasicInfo=false;
	m_bEnableFreeAddTrain=TRUE;
}

CTgEditView::~CTgEditView()
{
	if(m_pTrainSideDlg)
		delete m_pTrainSideDlg;
	m_pTrainSideDlg=NULL;

	if(m_pTrainRouteDlg)
		delete m_pTrainRouteDlg;
	m_pTrainRouteDlg=NULL;

	if(m_pBasicSchdCompareDlg)
		delete m_pBasicSchdCompareDlg;
	m_pBasicSchdCompareDlg=NULL;

	if(m_pSideYingyeDlg)
		delete m_pSideYingyeDlg;
	m_pSideYingyeDlg=NULL;
}


void CTgEditView::OnDestroy() 
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage);
}
void CTgEditView::OnInitialUpdate()
{
	CTgView::OnInitialUpdate();
}

BOOL  CTgEditView::InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode)
{
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

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) // ��ȡ����
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
	
	m_pTrainSideDlg=new CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV, this);
	if(m_pTrainSideDlg)
	{
		m_pTrainSideDlg->Create(CDlgTrainRecord::IDD, this);
	}
	m_pSideYingyeDlg=new CDlgEditTrainSideYingye(this);
	if(m_pSideYingyeDlg)
	{
		m_pSideYingyeDlg->Create(CDlgEditTrainSideYingye::IDD, this);
	}
	m_pTrainRouteDlg=new CDlgTrainRouteCheck(this);
	if(m_pTrainRouteDlg)
	{
		m_pTrainRouteDlg->Create(CDlgTrainRouteCheck::IDD, this);
	}
	m_pBasicSchdCompareDlg=new CDlgBasicSchduleCompare(GetDocument()->GetTGDataHolder(), this);
	if(m_pBasicSchdCompareDlg)
	{
		m_pBasicSchdCompareDlg->Create(CDlgBasicSchduleCompare::IDD, this);
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
	mTG.UpdateWindow();
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

	CTgView::SELECT_TRAIN(train_index,nFlag,stano1,stano2,date,time2);
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
		TIME sttm,endtm;
		m_schdinfo.GetShiftManagerRange(sttm,endtm);
		if(pData->GetSingleTrain(dlg.m_strInsertTrainId,sttm,endtm,FALSE,TRUE))
		{
			if(MessageBox("�����Ѿ����ڣ��Ƿ��ظ��ӳ�?","��ʾ",MB_YESNO)!=IDYES)
				return;
		}

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
			if(IS_BASIC_SCHD(GetViewScheduleType()))
			{
				MessageBox("����ͼ��֧�ִ˲���","��ʾ");
				return;
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
		}
	}
}

void CTgEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CTgView::OnUpdate(pSender,lHint,pHint);
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
		{
			GpcLogTrainMessage(0, 0, "ɾ������ͼ���Σ�schetype=%d",GetViewScheduleType());
			m_tgmmi.GetTGCtrl()->BeginLoadData();
			m_tgmmi.GetTGCtrl()->EndLoadData();
		}
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

void CTgEditView::OnBasicDeleteName()
{
	if(!gpcdef.GetOperatorPrior4Basic())
		return;

	CDlgBasicDeleteName dlg(GetViewSectionNo(),GetDocument(),this); 
	dlg.DoModal();
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
}

void CTgEditView::OnSideYingyeEdit()
{
	if(m_pSideYingyeDlg)
	{
		m_pSideYingyeDlg->SetDocument(GetDocument());
		m_pSideYingyeDlg->InitDlgCtrlItem();
		m_pSideYingyeDlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnUpdateEnableEdit(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
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
			CTG_TrainScheduleBase *pdata=pholder->GetBasicPrevScheduleData();

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

void CTgEditView::OnDeleteInvalidTrain()
{
	// TODO: �ڴ���������������
	mTG.AliveWindow(8);
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

void CTgEditView::OnTimer(UINT nIDEvent)
{
	CTgView::OnTimer(nIDEvent);
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

/  / WintgForm.cpp : implementation file
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include <afxcoll.h>
#include "IF.h"
#include "io.h"

#include <sys/timeb.h>


#include "mmsystem.h"
#include "tg_message_define.h"
#include <algorithm>

#include "resource.h"
#include "tgForm.h"
#include "tg.h"

#include "messages.h"
#include "Shlwapi.h"
#include "time.h"

#include <vector>

#include ".\dlg\insertSimpleDlg.h"
#include ".\dlg\absinhibitproperty.h"
#include ".\dlg\absselfcloseproperty.h"
#include ".\dlg\SuperPassDlg.h"
#include ".\dlg\absfixproperty.h"
#include ".\dlg\stationslowproperty.h"
#include ".\dlg\StationSlowPropertyMiddle.h"
#include ".\dlg\stationslowpropertyLarge.h"
#include ".\dlg\stationinhibitproperty.h"
#include ".\dlg\StationInhibitPropertyMiddle.h"
#include ".\dlg\stationinhibitpropertyLarge.h"
#include ".\dlg\dlgrouteautoflag.h"

#include ".\dlg\DlgSplitTrain.h"				
#include ".\dlg\DlgCombineTrain.h"		
#include ".\dlg\DlgGiveoutDirectionSel.h"
#include ".\dlg\DlgCopyTrain.h"
#include ".\dlg\dlgnotetext.h"
#include ".\dlg\AbsEleFixProperty.h"

#include ".\dlg\findtraindlg.h"
#include ".\dlg\guajichedlg.h"		
#include ".\dlg\DlgMultiSelOperate.h"
#include ".\dlg\AbsSlowProperty.h"
#include ".\dlg\DlgChangeHatTrainId.h"
#include ".\dlg\DlgSetTrainSideLock.h"

#include ".\dlg\DrawfreewayendSelDlg.h"
#include ".\dlg\DlgMessageBox.h"
#include ".\tgform.h"
#include ".\dlg\TrainPlanParaDlg.h"
#include ".\dlg\TransportInfo.h"
#include ".\dlg\TrainAttributeSheet.h"
#include ".\dlg\TrainDataDlg.h"
#include ".\dlg\ChangeTrainID.h"
#include ".\dlg\ChangeTrackDlg.h"
#include ".\dlg\FindTrainDlg.h"
#include ".\dlg\ProhibitMeetDlg.h"
#include ".\dlg\TGColorDlg.h"
#include ".\dlg\ModifyStationIndex.h"
#include ".\dlg\AbsStopDlg.h"
#include ".\dlg\DlgShuaiGuaManage.h"
#include ".\dlg\AbsEleFixManage.h"
#include ".\dlg\AbsEleFixLanzhouProperty.h"
#include ".\dlg\dlgselmutimark.h"
#include ".\dlg\DlgCheZuHao.h"
#include ".\dlg\czhshowdlg.h"
#include ".\dlg\CZHMultiEditDlg.h"
#include ".\dlg\DlgQuickModifyMultiTrains.h"
#include "ReturnTrainidInfo.h"
#include "ReturnTrainSelDlg.h"
#include ".\dlg\InputReturnTrainDlg.h"
#include ".\dlg\JiaoLuEditDlg.h"
#include ".\dlg\JiaoLuShowDlg.h"
#include ".\dlg\DlgMessageBoxPassword.h"
#include ".\dlg\TrainPartRunDlg.h"

#include "MessageDlg.h"
#include ".\MarkTimeSpan.h"                  
#define TIMER_IDEVENT_DRAWBAR 101
#define TIMER_IDEVENT_FLASH 666
#define TIMER_CHECK 606

#define NULL_INDEX			  255
#define TIME_SHOW_SIDENO 4
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

#define DEBUG_RUN_TIME
// CTGForm
IMPLEMENT_DYNCREATE(CTGForm, CScrollView)

CTGForm::CTGForm( )
{
	m_sFocusTrain=NULL;
	m_nFocusTrainIndex=0;
	m_nFocusRectIndex=-1;
	m_nFocusRecordIndex=-1;

	m_pDoc = NULL;
	m_pBitmapLayer1 = NULL;
	m_pBitmapLayer2 = NULL;
	m_pBitmapLayer3 = NULL;
	m_pBitmapLayer4 = NULL;

	hPopupMenu = NULL;

	mHReceivePoint = AfxGetApp()->LoadCursor(IDC_RECEIVE_CURSOR); 

	m_bMouseDown = FALSE;


	m_nFocusFigure = NULL;

	m_bCaptured   = FALSE;

	m_bFirstPoint = false;
	m_bSecondPoint = false;
	m_nMarkType = 0;

	m_nMouseDownPosition = -1;


	m_pDoc = NULL;

	myPDataManage = NULL;

	m_InitColor = 0;
}

CTGForm::~CTGForm()
{
}

void CTGForm::SetMyCtrl(CTGCtrl* ctrl) 
{ 
	m_pctrl = ctrl;
}

void CTGForm::SetMyDataManage(CTG_Mange* manage) 
{ 
	myPDataManage = manage;
}

void CTGForm::OnDestroy() 
{
	KillTimer(TIMER_IDEVENT_DRAWBAR);
	KillTimer(TIMER_IDEVENT_FLASH);
	KillTimer(TIMER_CHECK);
	
	if(m_pBitmapLayer1!=NULL)
	{
		m_Layer1.SelectObject(m_pBitmapLayer1);
		m_Layer1.DeleteDC();
		m_bitmap1.DeleteObject();
	}

	if(m_pBitmapLayer2!=NULL)
	{
		m_Layer2.SelectObject(m_pBitmapLayer2);
		m_Layer2.DeleteDC();
		m_bitmap2.DeleteObject();
	}
	
	if(m_pBitmapLayer3!=NULL)
	{
		m_Layer3.SelectObject(m_pBitmapLayer3);
		m_Layer3.DeleteDC();
		m_bitmap3.DeleteObject();
	}

	if(m_pBitmapLayer4!=NULL)
	{
		m_Layer4.SelectObject(m_pBitmapLayer4);
		m_Layer4.DeleteDC();
		m_bitmap4.DeleteObject();
	}
	
	CScrollView::OnDestroy();
}

BEGIN_MESSAGE_MAP(CTGForm, CScrollView)
	//{{AFX_MSG_MAP(CTGForm)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_SYSCHAR()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()

	ON_COMMAND(ID_MARK_TIMESPAN, OnMarkTimespan)         // ̫ԭ �޸�ʩ��������ֹʱ��
	ON_COMMAND(ID_ADDTRAIN_ABS_STOP, OnAddtrainAbsStop)
	ON_COMMAND(ID_ADDTRAIN_STOP, OnAddtrainStop)
	ON_COMMAND(ID_ADDTRAIN_DELIVER, OnAddtrainDeliver)
	ON_COMMAND(ID_ADDTRAIN_CANCEL, OnAddtrainCancel)

	ON_COMMAND(ID_MARK_PROPERTY, OnMarkProperty)
	ON_COMMAND(ID_CHANGE_TRACK, OnChangeTrack)
	ON_COMMAND(ID_BATCH_MODIFY_TRACK, OnBatchChangeTrack)
	ON_COMMAND(ID_INHBIT_PASS, OnSetInhibitTrainPass)
	
	ON_COMMAND(ID_LOCK_STATION_SIDE, OnLockStationSide)
	ON_COMMAND(ID_LOCK_TRAIN_SIDE, OnLockTrainSide)

	ON_COMMAND(ID_STOP_ABS, OnStopAbs)
	ON_COMMAND(ID_STOP_ABS_CANCEL, OnStopAbsCancel)
	ON_COMMAND(ID_STOP_SIGNAL, OnStopSignal)
	ON_COMMAND(ID_STOP_SIGNAL_CANCEL, OnStopSignalCancel)

	ON_COMMAND(ID_QUERT_QUEBAO, OnQueryQuebao)
	ON_COMMAND(ID_FIND_DDML, OnFindDDML)
	ON_COMMAND(ID_MARK_CANCELWORK, OnCancelWork)

	ON_COMMAND(ID_TRAIN_ENGINE_INFO, OnTrainEngineInfo)
	
	ON_COMMAND(ID_TRAIN_BIANZU_INFO, OnTrainBianzuInfo)
	ON_COMMAND(ID_TRAIN_SUBAO_INFO, OnTrainSubaoInfo)
	ON_COMMAND(ID_TRAIN_WORK_INFO, OnTrainWorkInfo)
	ON_COMMAND(ID_MARK_WORK_INFO, OnMarkWorkInfo)
	ON_COMMAND(ID_SHUAIGUA_MANAGE, OnManageShuaigua)
	
	ON_COMMAND(ID_DIRECT_SET_TRIG, OnDirectSetTrigeRoute)
	ON_COMMAND(ID_SHOW_BASIC_INFO, OnShowBasicPlanInfo)
	ON_COMMAND(ID_CANNOT_MEET, OnCannotMeet)
	ON_COMMAND(ID_TRAINSPORT_INFO, OnTransportInfo)
	ON_COMMAND(ID_TRAIN_DATA, OnTrainData)
	ON_COMMAND(ID_MODIFY_PLAN_PARA, OnModifyPlanPara)

	ON_COMMAND(ID_SHOW_CZH_TRAIN, OnTrainsInSameCZHShow)
	ON_COMMAND(ID_MODIFY_TRAIN_CZH, OnTrainCZHEdit)
	ON_COMMAND(ID_JIECHU_TRAIN_CZH, OnTrainCZHJiechu)
	ON_COMMAND(ID_GENTRAINID_BY_CZH, OnGenReturnTrainIdByCZH)
	
	ON_COMMAND(ID_TRAIN_ATTRIBUTE, OnTrainAttribute)
	ON_COMMAND(ID_STAY_TO_PASS, OnStayToPass)
	ON_COMMAND(ID_PASS_TO_STAY, OnPassToStay)
	ON_COMMAND(ID_MUTI_STAY_TO_PASS, OnMutiTrainRecStayToPass)
	
	ON_COMMAND(ID_DELETE_TRAIN, OnDeleteTrain)

	ON_COMMAND(ID_GUA_JICHE, ManageGuaJiChe)
	ON_COMMAND(ID_SIDE_TIANCHUANG_G, OnSideTianchuangG)

	ON_COMMAND(ID_CHANGE_TRAINID, OnChangeTrainid)
	ON_COMMAND(ID_REPLACE_HATTRAINID, OnReplaceHattrainid)
	ON_COMMAND(ID_REPLACE_TRAINID, OnReplaceTrainid)

	ON_COMMAND(ID_SHOWTRACTION_NO, OnShowtractionNo)
	ON_COMMAND(ID_SHOWTRACTION_TOP, OnShowtractionTop)
	ON_COMMAND(ID_SHOWTRACTION_MIDDLE, OnShowtractionMiddle)
	ON_COMMAND(ID_SHOWTRACTION_BOTTOM, OnShowtractionBottom)

	ON_COMMAND(ID_STATISTIC_ARRIVAL, OnStatisticArrival)
	ON_COMMAND(ID_STATISTIC_DEPART, OnStatisticDepart)
	ON_COMMAND(ID_STATISTIC_BOTH, OnStatisticBoth) 
	ON_COMMAND(ID_NO_STATISTIC, OnNoStatistic)
	ON_COMMAND(ID_STATISTIC_WHOLE, OnStatisticWhole)

	ON_COMMAND(ID_STOP_DRAW_UP, OnStopDrawUp)
	ON_COMMAND(ID_STOP_DRAW_MIDDILE, OnStopDrawMiddile)
	ON_COMMAND(ID_STOP_DRAW_BOTTOM, OnStopDrawBottom)
	ON_COMMAND(ID_DELETE_ABS, OnDeleteAbs)
	ON_COMMAND(ID_BLOCK_ADJUST_TRAINS, OnBlockAdjustTrain)

	ON_COMMAND_RANGE(ID_MARK_PLAN,ID_MARK_COMPLETE, OnSetMarkActiveStatus)

	ON_COMMAND_RANGE(ID_CHANGE_TRACKS_BEGIN, ID_CHANGE_TRACKS_END, OnChangeTracks)
	ON_COMMAND_RANGE(ID_GEN_DCMD, ID_GEN_DCMD+20, OnAutoGenDCmd)

	ON_COMMAND(ID_REFUSEED_TRAIN, OnRefusedTrain)

	ON_COMMAND(ID_PRESS_BEFORE,   OnPressureBefore)
	ON_COMMAND(ID_PRESS_AFTER,    OnPressureAfter)

	ON_COMMAND(ID_STOP_DWAW, OnStopDraw)       // ���ͣ��
	ON_COMMAND(ID_STOP_DEPART, OnStopDepart)   // ��̽���
	ON_COMMAND(ID_MIDDLE_START, OnMiddleStart) // ��վʼ��
	ON_COMMAND(ID_MIDDLE_FETCH, OnMiddleFetch) // ��վ����
	ON_COMMAND(ID_STOP_TEMP, OnStopTemp)       // ��ʱͣ��
	ON_COMMAND(ID_MIDDLE_DIV_TRAIN, OnMiddleDivTrain)// �м�ָ�
	
	ON_COMMAND(ID_CONTINUE_DRAW, OnContinueDraw) // ��������
	ON_COMMAND(ID_ADJUST_SINGLE, OnChangeRoute) // �����·
	ON_COMMAND(ID_ROUTE_AUTO_MARK, OnRouteAutoMark) // �����·�Դ�
	ON_COMMAND(ID_FENJIEKOU_TIME, OnSetFenjiekouLateTime) // �ֽ�������

	ON_COMMAND(ID_DEPART_BY_PLAN,OnDepartByPlan) // ����ʱ��---���ƻ�
	ON_COMMAND(ID_DEPART_BY_MEET,OnDepartByMeet) // ����ʱ��---�����ù�ϵ
	ON_COMMAND(ID_DEFINE_PASS, OnDefinePass)
	ON_COMMAND(ID_TECH_STOP, OnTechStop)
	ON_COMMAND(ID_TRAIN_COPY, OnTrainCopy)

	ON_COMMAND(ID_SAVE_TRAIN_XML, OnSaveTrainXML)
	ON_COMMAND(ID_SET_SECRET_TRAIN, OnSetSecretTrain)
	ON_COMMAND(ID_SET_NO_LATE_TIME, OnSetNoLateEarlyTime)
	
	ON_COMMAND(ID_MARK_COPY, OnMarkCopy)
	ON_COMMAND(ID_MARK_COPY_V_DOWN, OnMarkCopyVDown)
	ON_COMMAND(ID_MARK_COPY_V_UP, OnMarkCopyVUp)
	ON_COMMAND(ID_MARK_CONTINUE_LINING, OnMarkContinueLining)
	ON_COMMAND(ID_DEL_MULTI_TRAIN, OnSetDeleteMultiTrain)
	ON_COMMAND(ID_SEL_MULTI_TRAIN, OnSetSelMultiTrain)
	ON_COMMAND(ID_MODIFY_MULTI_TRAIN, OnQuickModifyMultiTrain)

	ON_COMMAND(ID_ONLY_SHOW_MARK, OnOnlyShowMark)
	ON_COMMAND(ID_ONLY_SHOW_TRAIN, OnOnlyShowTrain)
	ON_COMMAND(ID_SHOW_MARK_AND_TRAIN, OnShowMarkAndTrain)
	ON_COMMAND(ID_SHOW_SINGLE_TRAIN, OnShowSingleTrain)

	ON_COMMAND(ID_MARK_MOVE_START, OnMarkAutoMoveStart)
	ON_COMMAND(ID_MARK_MOVE_STOP, OnMarkAutoMoveStop)
	
	ON_COMMAND(ID_DEFINE_COLOR, OnSetTGColor)

	ON_COMMAND(ID_UP_TRAIN, ShowUpDirectTrain)
	ON_COMMAND(ID_DOWN_TRAIN, ShowDownDirectTrain)
	ON_COMMAND(ID_DUAL_TRAIN, ShowDualDirectTrain)

	ON_COMMAND(ID_PASEEGE_TRAIN, ShowPassagerTrain)
	ON_COMMAND(ID_GOODS_TRAIN, ShowGoodsTrain)
	ON_COMMAND(ID_PESSGOODS_TRAIN, ShowAllTrain)

	ON_COMMAND(ID_BASIC_BEGIN_TIME_18, OnShowBeginTimePoint6)
	ON_COMMAND(ID_BASIC_BEGIN_TIME_12, OnShowBeginTimePoint12)

	ON_COMMAND(ID_EXIT_DRAW_TRAIN_STATE,	OnExitDeadState)
	
	ON_COMMAND(ID_MODIFY_TRAIN_INDEX,	    OnModifyTrainIndex)

	ON_COMMAND_RANGE(ID_ADJUST_BEGIN, ID_ADJUST_END, OnAutoAjustTrain)
	ON_COMMAND_RANGE(ID_ABS_TRAIN_BEGIN, ID_ABS_TRAIN_END, OnDrawAbsWorkTrain)
	//}}AFX_MSG_MAP

	ON_MESSAGE(MSG_MENUADD_MARK_GRAPH,	OnMenuAddMarkGraph)

	ON_MESSAGE(MSG_ADDNEW_TRAIN_GRAPH,	OnAddNewTrainGraph)
	ON_MESSAGE(MSG_ADDNEW_TRAIN_GRAPH_XML,	OnAddNewTrainGraphXml)

	ON_MESSAGE(MSG_TRAINS_OPERATOR,		OnTrainsOperator)

	ON_COMMAND(ID_IN_STATION_RETURN, OnInStationReturn) //  �յ�-ԭ���۷�
	ON_COMMAND(ID_OUT_STATION_RETURN, OnOutStationReturn) //  �յ�-վ���۷�
	ON_COMMAND(ID_NOT_STATION_RETURN, OnNotStationReturn) // �յ�-���۷�
	ON_COMMAND(ID_NOT_START_RETURN, OnNotStartReturn) // ʼ��-���۷�

	ON_COMMAND(ID_KAKONG_BASIC_SIDE, OnKKBasicSide) // 3.0����ͼ�ɵ�����
	ON_COMMAND(ID_KAKONG_BASIC_ENTRY, OnKKBasicEntry) // 3.0����ͼ����ڿ���
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()

	ON_WM_SETCURSOR()
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CTGForm::OnSetTGColor()
{
	CTGColorDlg dlg(this);
	dlg.mColor = this->m_InitColor;
	if(dlg.DoModal() == IDOK)
	{
		this->m_InitColor = dlg.mColor;
		if(m_pctrl->IsWorkMode())
			this->DisplyTrainByDirection();
		else
			this->Redraw(); // ok
	}
}

void CTGForm::CurrentSelObjectLostFoucs(int flag)
{
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	if(flag==1 || flag == 3)
	{
		m_nFocusTrainIndex=0;
		m_nFocusRectIndex=-1;
		m_nFocusRecordIndex=-1;
		if(m_sFocusTrain != NULL)
		{
			RefreshTrain(m_sFocusTrain);
			m_sFocusTrain = NULL;
		}
	}
	if(flag==2 || flag == 3)
	{
		if(m_nFocusFigure != NULL)
		{
			CTg_BLOCK* pBlock = myPDataManage->GetMark(m_nFocusFigure->mark_index);
			if(pBlock != NULL)
			{
				pBlock->RestoreDrawFlag();
				EraseBlock(*pBlock);
				m_pDoc->DrawSingleMark(*pBlock, &m_Layer1);
				RefreshRect(pBlock->myPosition);
			}
		}
			
		m_nFocusFigure = NULL;
		m_nFocusBlockIndex = 0;
	}
	return;
}

void CTGForm::OnSetSelMultiTrain()
{
	CurrentSelObjectLostFoucs(3);
	
	m_selTrains.clear();
	m_pDoc->m_bMoveMutiTrain=true;
	m_pDoc->m_nOperatorStatus = MUTI_TRAINS_SEL_INIT;
}

void CTGForm::OnSetDeleteMultiTrain()
{
	CurrentSelObjectLostFoucs(3);

	m_selTrains.clear();
	m_pDoc->m_bDeleteMutiTrain=true;
	m_pDoc->m_nOperatorStatus = MUTI_TRAINS_SEL_INIT;
}

void CTGForm::OnQuickModifyMultiTrain()
{
	CDlgQuickModifyMultiTrains *dlg = new CDlgQuickModifyMultiTrains(m_pDoc,NULL);
	if(dlg != NULL)
	{
		dlg->DoModal();
		delete dlg;
	}
}

void CTGForm::OnOnlyShowMark()
{
	if(m_pDoc->IsOnlyShowMark())
		return;
	m_pDoc->SetOnlyShowMark();

	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

void CTGForm::OnOnlyShowTrain()
{
	if(m_pDoc->IsOnlyShowTrain())
		return;
	m_pDoc->SetOnlyShowTrain();

	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

void CTGForm::OnShowMarkAndTrain()
{
	if(m_pDoc->IsShowTrainAndMark())
		return;
	m_pDoc->SetShowTrainAndMark();
	
	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

LRESULT CTGForm::OnMenuAddMarkGraph(WPARAM wParam,LPARAM lParam)
{
	short markType = (short)wParam;

	m_bFirstPoint = false;
	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	if(MARK::IF_ABS_INHIBIT == markType)
		OnAbsInhibitG();
	else if(MARK::IF_ABS_SLOW == markType)
		OnAbsSlowG();
	else if(MARK::IF_ABS_ELEM_FIX == markType)
	{
		if(this->m_pctrl->mConfigTg.m_context.IsGenLanzhouAbsEleFixMark())
			OnNewLanzhouAbsFixMark();
		else
			OnAbsEleFixG();
	}
	else if(MARK::IF_ABS_TOTAL_FIX == markType)
			OnAbsTotalFix();
	else if(MARK::IF_ABS_SELF_CLOSE == markType)
		OnSelfCloseG();

	else if(MARK::STATION_INHIBIT == markType)
		OnSideInhibitG();
	else if(MARK::STATION_SLOW == markType)
		OnStationSlowG();
	else if(MARK::STATION_FIX == markType)
		OnSideTianchuangG();
	else if(MARK::STATION_SIDE_OCCUPY == markType)
		OnSideOccupy();
	else if(MARK::STATION_SIDE_LOST_ELE == markType)
		OnSideLostEle();

	else if(MARK::TEXT_NOTE == markType)
		OnTextNote();
	else if(MARK::PE_NOTE == markType)
		OnJishiZhujie();

	return 0;
}

void CTGForm::OnNewLanzhouAbsFixMark()
{
	//added by zhaoliang begin
	CAbsEleFixManage dlg(m_pDoc,NULL);
	if (dlg.DoModal()==IDOK)
	{
		CTg_BLOCK block;

		//ѡ��ʱ��
		block.start_time = dlg.usStartTimeValue;
		block.end_time = dlg.usEndTimeValue;

		//վ����Ϣ
		block.station = dlg.station;
		block.index = dlg.index;
        
		// ����������ݺ͵�ǰ��ͼ�Ƿ�ƥ��
		if(block.station<0 || block.station>=m_pDoc->m_nDispDistanceNum
		    || block.index<0 || block.index>=m_pDoc->m_nDispDistanceNum)
		{
			MessageBox("��ע��, �������� ����վ�����!");
			return;
		}

		if(m_pDoc->m_sDispDistance[block.station].station_number!=dlg.station_no1)
		{
			MessageBox("��ע��, �������� ����վ���վ�Ų�һ��,������ͼ!");
			return;
		}
		if(m_pDoc->m_sDispDistance[block.index].station_number!=dlg.station_no2)
		{
			MessageBox("��ע��, �������� ����վ���վ�Ų�һ��,������ͼ!");
			return;
		}

        //�Ϸ��·�վ����
		if (dlg.nCheckUpAndDownFlag& 0x01)//�û�ѡ������Ϸ�վ
		{
			block.slow_time |= 0x0001;//���λ��1  �Ϸ�����
		} 
		else
		{
			block.slow_time &= 0xFFFE;//���λ��0  �Ϸ�������  
		}
		if (dlg.nCheckUpAndDownFlag & 0x02)//�·�վ����
		{
			block.slow_time |= 0x0002;//�ڶ�λ��1  �·�����
		} 
		else
		{
			block.slow_time &= 0xFFFD;//�ڶ�λ��0  �·�������
		}

		//������Ϣ
		if (dlg.nFenXiangPos == 1)//�Ͻ��½�  0 0 X X
		{
            block.slow_time &= 0xFFF3;
		} 
		else if(dlg.nFenXiangPos == 2)//�Ͻ���Զ 0 1 X X
		{
            block.slow_time &= 0xFFF7;
            block.slow_time |= 0x0004;
		}
		else if (dlg.nFenXiangPos == 3)//��Զ�½� 1 0 X X
		{
		    block.slow_time &= 0xFFFB;
		    block.slow_time |= 0x0008;
		} 
		else if(dlg.nFenXiangPos == 4)//��Զ��Զ  1 1 X X
		{
            block.slow_time |= 0x000C;
		}
		else
		{
            block.slow_time |= 0x000C;
		}
		
		// ����
        block.line=2;
		//added by zhaoliang 
		//////////////////////////////////////////////////////////////////////////
		int nMixSelectedNodeIndex = min(block.station,block.index);
		int nMaxSelectedNodeIndex =	max(block.station,block.index);

		std::vector<int> abs_list;
		m_pDoc->GetMutiNeibAbsLines(nMixSelectedNodeIndex, nMaxSelectedNodeIndex, abs_list);

		block.count = 0;
		for(int i=0; i<abs_list.size(); i++)
		{
			block.ele_number[block.count] = abs_list[i];
			block.count++;
			if(block.count>=sizeof(block.ele_number)/sizeof(block.ele_number[0]))
			{
				break;
			}
		}
		abs_list.clear();

		//////////////////////////////////////////////////////////////////////////
		strcpy(block.text,"��������");
		block.type=NEW_ABS_ELE_FIX_MARK;
		m_pDoc->SendAppendMark(block);
	}
}

void CTGForm::OnDeleteAbs()
{
	if(m_nFocusFigure != NULL) 
	{
		m_pDoc->SendDeleteBlockXml(m_nFocusFigure->mark_index, 0);
		m_nFocusFigure = NULL;
	}
}

//--------------------------------------------------------------------
void CTGForm::AutoChangeTrainID(CTG_TRAIN& single) 
{
	single.bChangeTrainID = false;

	if(m_strArrivalTrainId.IsEmpty())
		return;

	TgTRAIN_RECORD *pHeadRecord = single.GetFirstRecord();
	TgTRAIN_RECORD *pTailRecord = single.GetLastRecord();
	if(pHeadRecord == NULL)  
		return;

	// ��ʮ���� ������
	int index = pHeadRecord->in_station_index;
	if(index >= 0 && index < m_pDoc->m_nDispDistanceNum)
	{
		if(m_pDoc->m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsAutoChangeTrainId(pHeadRecord->station, index))
		{
			pHeadRecord->arrive_train_id = m_strArrivalTrainId;
			single.bChangeTrainID = true;
		}
	}

	if(pTailRecord == NULL || pTailRecord == pHeadRecord)  
		return;

	//��ʮ���� ������
	index = pTailRecord->in_station_index;
	if(index >= 0 && index < m_pDoc->m_nDispDistanceNum)
	{
		if(m_pDoc->m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsAutoChangeTrainId(pTailRecord->station, index))
		{
			pTailRecord->depart_train_id = m_strArrivalTrainId;
			single.bChangeTrainID = true;
		}
	}

	if(single.bChangeTrainID)
		return;

	// �м�������
	bool bChangeTrainID=false;
	int  nIndexPos=-1;
	for(int i=0; i<single.GetRecordNum(); i++)
	{
		TgTRAIN_RECORD *pRecord = single.GetRecordByRecIndex(i);
		if(pRecord)
		{
			index = pRecord->in_station_index;
			if(m_pDoc->m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsAutoChangeTrainId(pRecord->station, index))
			{
				bChangeTrainID=true;
				nIndexPos=i;
				break;
			}
			index = pRecord->out_station_index;
			if(m_pDoc->m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsAutoChangeTrainId(pRecord->station, index))
			{
				bChangeTrainID=true;
				nIndexPos=i;
				break;
			}
		}
	}

	if(bChangeTrainID && nIndexPos>=0)
	{
		for(int i=nIndexPos; i<single.GetRecordNum(); i++)
		{
			TgTRAIN_RECORD *pRecord = single.GetRecordByRecIndex(i);
			if(pRecord)
			{
				if(i==nIndexPos)
				{
					pRecord->depart_train_id = m_strArrivalTrainId;
				}
				else
				{
					pRecord->arrive_train_id = m_strArrivalTrainId;
					pRecord->depart_train_id = m_strArrivalTrainId;
				}
			}
		}
	}
}

#ifdef _DEBUG
void CTGForm::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTGForm::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif //_DEBUG

void CTGForm::SetReceivePointState(CTG_TRAIN& train, int sel_index)
{
	CTG_TRAIN modifyTrain = train;
	CString text;

	TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(sel_index);
	if(rec == NULL) 
		return;

	int nLen=this->m_pctrl->mConfigTg.m_context.GetLimitModifyTimeLen()*60;
	if (this->m_pctrl->mConfigTg.m_context.IsLimitModifyTime())
	{
		if(nLen<=0)
		{
			MessageBox("��ֹ�޸�ʵ�ʵ�,����������յ�!", "��ʾ", MB_OK|MB_ICONHAND);
			return;
		}
	}
	else
	{
		nLen=0;
	}

	CStringArray alarm;
	if(nLen>0 && !this->m_pctrl->mConfigTg.IsNoLimitStation(rec->station))
	{
		CTime tm=CTime::GetCurrentTime();
		long nNowTime=tm.GetTime();
		long depart=rec->depart;
		long arrive=rec->arrive;

		nNowTime=nNowTime-nNowTime%60;
		depart=depart-depart%60;
		arrive=arrive-arrive%60;

		if(m_nMouseDownPosition == DEPART_POSITION)
		{
			if(IsActualDepartTime(rec->adjust_status))
			{
				MessageBox("�������Ѿ���ʵ�ʵ㣬����������յ㣬ת�ƻ�!", "��ʾ", MB_OK|MB_ICONHAND);
				return;
			}
			else
			{
				if(abs(nNowTime-depart)>nLen)
				{
					CString str;
					str.Format("������͵�ǰʱ������ %d ���ӣ� ����������յ㣬תʵ��!", nLen/60);
					MessageBox(str, "��ʾ", MB_OK|MB_ICONHAND);
					return;
				}

				rec->adjust_status = rec->adjust_status | 0x0002;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);

				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			}
			if(IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		else if(m_nMouseDownPosition == ARRIVAL_POSITION)
		{
			if(IsActualArrivalTime(rec->adjust_status))
			{
				MessageBox("������Ѿ���ʵ�ʵ㣬����������յ㣬ת�ƻ�!", "��ʾ", MB_OK|MB_ICONHAND);
				return;
			}
			else
			{
				if(abs(nNowTime-arrive)>nLen)
				{
					CString str;
					str.Format("�����͵�ǰʱ������ %d ���ӣ� ����������յ㣬תʵ��!", nLen/60);
					MessageBox(str, "��ʾ", MB_OK|MB_ICONHAND);
					return;
				}

				rec->adjust_status = rec->adjust_status | 0x0001;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);

				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			}
			if(IsArriveRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		else
		{
			if(IsActualDepartTime(rec->adjust_status) && IsActualArrivalTime(rec->adjust_status))
			{
				if (IsFlagStart(rec->flag))
				{
					MessageBox("�������Ѿ���ʵ�ʵ㣬����������յ�, ת�ƻ�!", "��ʾ", MB_OK|MB_ICONHAND);
				}
				else if (IsFlagTerminal(rec->flag))
				{
					MessageBox("������Ѿ���ʵ�ʵ㣬����������յ�, ת�ƻ�!", "��ʾ", MB_OK|MB_ICONHAND);
				}
				else
				{
					MessageBox("ͨ�����Ѿ���ʵ�ʵ㣬����������յ�, ת�ƻ�!", "��ʾ", MB_OK|MB_ICONHAND);
				}
				return;
			}

			if((abs(nNowTime-depart)>nLen) || (abs(nNowTime-arrive)>nLen))
			{
				CString str;
				if (IsFlagStart(rec->flag))
				{
					str.Format("������͵�ǰʱ������ %d ���ӣ� �������޸�����յ㣬תʵ��!", nLen/60);
				}
				else if (IsFlagTerminal(rec->flag))
				{
					str.Format("�����͵�ǰʱ������ %d ���ӣ� �������޸�����յ㣬תʵ��!", nLen/60);				
				}
				else
				{
					str.Format("ͨ����͵�ǰʱ������ %d ���ӣ� �������޸�����յ㣬תʵ��!", nLen/60);
				}
				MessageBox(str, "��ʾ", MB_OK|MB_ICONHAND);
				return;
			}

			if(!IsActualDepartTime(rec->adjust_status))
			{
				rec->adjust_status = rec->adjust_status | 0x0002;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);
			}

			if(!IsActualArrivalTime(rec->adjust_status))
			{
				rec->adjust_status = rec->adjust_status | 0x0001;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
			}

			if (IsFlagStart(rec->flag))
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			else if (IsFlagTerminal(rec->flag))
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			else
				text.Format("�� %s ���г��� %s վ�� ͨ��ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));

			if(rec->adjust_status & 0x0001 || rec->adjust_status & 0x0002)
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;
			else
				m_pDoc->focus_point = m_pDoc->POINT_REAL;

			if(IsArriveRouteOk(*rec) || IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}

		if(MessageBox("��ȷʵҪ"+text+" ��?", "��ʾ", MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;
		m_pDoc->sysprint(4,0,"����Աѡ��:%s", text);
	}
	else
	{
		if(m_nMouseDownPosition == DEPART_POSITION)
		{
			if(rec->adjust_status & 0x0002)
			{
				rec->adjust_status = rec->adjust_status & (~0x0002);
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;
				SetRecTimeFromDefault(*rec, TRAIN_DEPART);
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));

				CheckYingyeButStationPass(alarm, sel_index, rec, modifyTrain);
			}
			else
			{
				rec->adjust_status = rec->adjust_status | 0x0002;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			}
			
			if(IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		else if(m_nMouseDownPosition == ARRIVAL_POSITION)
		{
			if(rec->adjust_status & 0x0001)
			{
				rec->adjust_status = rec->adjust_status & (~0x0001);
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;
				SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));

				CheckYingyeButStationPass(alarm, sel_index, rec, modifyTrain);
			}
			else
			{
				rec->adjust_status = rec->adjust_status | 0x0001;
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
				text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			}

			if(IsArriveRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		else
		{
			if(rec->adjust_status & 0x0001)
			{
				rec->adjust_status = rec->adjust_status & 0xfffe;
				rec->adjust_status = rec->adjust_status & 0xfffd;
				SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
				SetRecTimeFromDefault(*rec, TRAIN_DEPART);

				if (IsFlagTerminal(rec->flag))
					text.Format("�� %s ���г��� %s վ�� ����ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
				else
					text.Format("�� %s ���г��� %s վ�� ͨ��ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));

				CheckYingyeButStationPass(alarm, sel_index, rec, modifyTrain);
			}
			else if (rec->adjust_status & 0x0002)
			{
				rec->adjust_status = rec->adjust_status & 0xfffe;
				rec->adjust_status = rec->adjust_status & 0xfffd;
				SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
				SetRecTimeFromDefault(*rec, TRAIN_DEPART);

				if (IsFlagStart(rec->flag))
					text.Format("�� %s ���г��� %s վ�� ����ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
				else
					text.Format("�� %s ���г��� %s վ�� ͨ��ʱ�� ʵ�� ת �ƻ�",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));	
				
				CheckYingyeButStationPass(alarm, sel_index, rec, modifyTrain);
			}
			else
			{
				rec->adjust_status = rec->adjust_status | 0x0001;
				rec->adjust_status = rec->adjust_status | 0x0002;
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
				SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);

				if (IsFlagStart(rec->flag))
					text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
				else if (IsFlagTerminal(rec->flag))
					text.Format("�� %s ���г��� %s վ�� ����ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
				else
					text.Format("�� %s ���г��� %s վ�� ͨ��ʱ�� �ƻ� ת ʵ��",m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));
			} 

			if(rec->adjust_status & 0x0001 || rec->adjust_status & 0x0002)
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;
			else
				m_pDoc->focus_point = m_pDoc->POINT_REAL;

			if(IsArriveRouteOk(*rec) || IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		
		if(MessageBox("��ȷʵҪ" + text + " ��?", "��ʾ", MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;

		m_pDoc->sysprint(4,0,"����Աѡ��:%s", text);
	}
	
	if(alarm.GetCount()>0)
	{
		CDlgMessageBoxPassword dlg(alarm, this);
		if(dlg.DoModal()!=IDOK)
			return;
		alarm.RemoveAll();
	}
	m_pDoc->SendUpdateTrain(modifyTrain, SCHD_UPDATE, MOUSE_TURN_REALITY, rec->station, rec->rec_index);
}
// �г���Ԯ
void CTGForm::RescueTrain(CPoint point)
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		MessageBox("��ע��: ��û���޸������ߵ�Ȩ��", "��ʾ", MB_ICONWARNING|MB_OK);
		return;
	}

	if(m_sFocusTrain == NULL)
		return;

	ULONG nTmpFocusTrainIndex=0;
	m_pDoc->QueryCurrentTrain(&point,nTmpFocusTrainIndex);		
	if(nTmpFocusTrainIndex <= 0)
		return;

	if(m_sFocusTrain->GetTrainIndex() == nTmpFocusTrainIndex)
	{
		CString str = "��ѡһ�в�ͬ���г�!";
		CDlgMessageBox dlg(str);
		return;
	}
	else
	{
		CTG_TRAIN *tmpTrain = this->myPDataManage->GetTrain(nTmpFocusTrainIndex);
		if(tmpTrain == NULL) 
			return;

		CTG_TRAIN sglOldTrain = *tmpTrain; 

		TgTRAIN_RECORD *tmp = GetFoucsTrainRecord();
		if(tmp == NULL) return;
		SetFlagRescue(tmp->flag);
		tmp->special_train_index = sglOldTrain.GetTrainIndex();

		bool stop_abs_up = true;
		if(IsFlagStopAbsUp(tmp->flag))
			stop_abs_up = true;
		else
			stop_abs_up = false;

		int node_index = -1;
		node_index = tmp->in_station_index;
		if(node_index < 0)
			node_index = tmp->out_station_index;

		if(node_index < 0 || node_index >= m_pDoc->m_nDispDistanceNum) 
			return;

		TgTRAIN_RECORD *rec = NULL;
		int nRecNum = sglOldTrain.GetRecordNum();
		if(nRecNum > 0)
		{		
			UINT idx = 0;
			for(idx = 0; idx < nRecNum; idx++)
			{
				if(sglOldTrain.myRecord[idx].out_station_index == node_index && sglOldTrain.myRecord[idx].station == tmp->station)
				{
					break;
				}
			}

			if(idx >= nRecNum)
				return;

			rec = sglOldTrain.GetRecordByPosition(idx);
			if(NULL == rec)
				return;

			if(stop_abs_up)
			{
				if(rec->in_sta_direct == DOWN_DIRECT)
				{
					rec = sglOldTrain.GetRecordByRecIndex(idx-1);
					stop_abs_up = false;
				}
			}
			else
			{
				if(rec->in_sta_direct == UP_DIRECT)
				{
					rec = sglOldTrain.GetRecordByRecIndex(idx-1);
					stop_abs_up = true;
				}
			}
		}
		if(rec == NULL) 
			return;

		int nRecIndex = rec->rec_index;
		for (int j = nRecNum -1; j > nRecIndex; j--)
		{
			sglOldTrain.DeleteTailRecord();
		}
		rec = sglOldTrain.GetLastRecord();
		if(rec == NULL) 
			return;

		rec->stop_abs_position = 0;
		rec->stop_abs_s_time = 5;
		rec->stop_abs_e_time = 5;

		//2013.8.7 hjz �޸���վʼ������Ԯ��ʼ��վ���������
		if (rec->rec_index!=0)
			rec->flag = 0;

		SetFlagStopAbs(rec->flag);
		if(stop_abs_up)
			SetFlagStopAbsUp(rec->flag);
		else
			SetFlagStopAbsDown(rec->flag);			

		rec->special_train_index = m_sFocusTrain->GetTrainIndex();

		CString strText;
		strText.Format("%s��Ԯ%s", m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetTrainShowId(sglOldTrain));
		
		SetFlagAbsTerminal(rec->flag);
		SetFlagRescued(rec->flag);

		m_pDoc->sysprint(4,0,"[������ҵ] ����Ա���� ��Ԯ�г� �г� %s",strText);

		m_pDoc->SendUpdateTrain(sglOldTrain);
		m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, HELP_TRAIN, 0, 0, sglOldTrain.GetTrainId());

		m_nFocusTrainIndex=0;
		m_nFocusRectIndex=-1;
		m_nFocusRecordIndex=-1;
		m_sFocusTrain = NULL;
	}
}
// �г��ϲ�
void CTGForm::UniteTrains(CPoint point)
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		MessageBox("��ע��: ��û���޸������ߵ�Ȩ��", "��ʾ", MB_ICONWARNING|MB_OK);
		return;
	}

	if(m_sFocusTrain == NULL)
		return;

	DWORD nTmpFocusTrainIndex = 0;
	CString pbTrainId = m_pDoc->QueryCurrentTrain(&point, nTmpFocusTrainIndex);
	if(nTmpFocusTrainIndex <= 0)
		return;

	if(m_sFocusTrain->GetTrainIndex() == nTmpFocusTrainIndex)
	{
		CDlgMessageBox dlg("��ѡһ�в�ͬ���г�!");
		dlg.DoModal();
	}
	else
	{
		CTG_TRAIN *pTrain = this->myPDataManage->GetTrain(nTmpFocusTrainIndex);
		if(pTrain == NULL)	
			return;

		TgTRAIN_RECORD *after_rec=GetFoucsTrainRecord();
		if(after_rec == NULL) 
			return;

		TgTRAIN_RECORD*before_rec = pTrain->GetStationRecord(after_rec->station);
		if(before_rec == NULL) 
			return;

		// �Ƚ�
		int nX1 = m_pDoc->GetOffsetPixelFromGraphBegin(after_rec->depart);
		int nX2 = m_pDoc->GetOffsetPixelFromGraphBegin(before_rec->depart);

		if(nX1 < nX2)
		{
			AfxMessageBox("����ѡ�к����г�,Ȼ���ٵ�ǰ�汻�ϲ��г�");
			return;
		}

		CTG_TRAIN masterTrain = *m_sFocusTrain;
		CTG_TRAIN slaverTrain = *pTrain;

		int nRecNum = slaverTrain.GetRecordNum();
		for (int i = nRecNum -1 ; i > before_rec->rec_index; i--)
			slaverTrain.DeleteTailRecord();

		TgTRAIN_RECORD *pLastRec = slaverTrain.GetLastRecord();
		if(pLastRec == NULL)
			return;

		SetFlagUnitOtherTrain(pLastRec->flag);
		pLastRec->special_train_index = masterTrain.GetTrainIndex();

		CString strText;
		strText.Format("%s�ϲ�%s", m_pDoc->GetTrainShowId(masterTrain), m_pDoc->GetTrainShowId(slaverTrain));

		pLastRec->depart = pLastRec->arrive;
		SetFlagEnd(pLastRec->flag);

		if(IsFlagStopAbs(pLastRec->flag)) CancelFlagStopAbs(pLastRec->flag);
		if(IsFlagStopSignal(pLastRec->flag)) CancelFlagStopSignal(pLastRec->flag);

		// �ϲ��г�
		TgTRAIN_RECORD *pMidRec = masterTrain.GetRecordByPosition(m_nFocusRecordIndex);
		if(pMidRec == NULL)
			return;

		SetFlagBeUnitedByOther(pMidRec->flag);
		pMidRec->special_train_index = slaverTrain.GetTrainIndex();
		if(!pMidRec->szAbsText.IsEmpty())
		{
			pMidRec->szAbsText += "/" + strText;
		}
		else
		{
			pMidRec->szAbsText = strText;
		}

		m_pDoc->sysprint(4,0,"[������ҵ] ����Ա���� �ϲ��г� �г� %s",strText);

		m_pDoc->SendUpdateTrain(slaverTrain);
		m_pDoc->SendUpdateTrain(masterTrain);
	}

	return;
}

// һ���г��ֽ�
void CTGForm::OnSplitTrain()
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		MessageBox("��ע��: ��û���޸������ߵ�Ȩ��", "��ʾ", MB_ICONWARNING|MB_OK);
		return;
	}

	TgTRAIN_RECORD* prec=GetFoucsTrainRecord();
	if(prec == NULL)
		return;
	
	CDlgSplitTrain dlg(this);
	dlg.m_strOrigTrainId = m_pDoc->GetTrainShowId(m_sFocusTrain);

	if(dlg.DoModal() != IDOK)
		return;

	if(dlg.m_strSplitTrainId1 != dlg.m_strSplitTrainId2)
	{
		if(dlg.m_strOrigTrainId == dlg.m_strSplitTrainId1)
		{
			OnSplitTrain(dlg.m_strSplitTrainId2);
		}
		else if(dlg.m_strOrigTrainId == dlg.m_strSplitTrainId2)
		{
			OnSplitTrain(dlg.m_strSplitTrainId1);
		}
		else
		{
			OnSplitTrain(dlg.m_strSplitTrainId1, dlg.m_strSplitTrainId2);
		}
	}
}
void CTGForm::OnSplitTrain(const CString& trainid)
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(rec == NULL) return;

	int nSelIndex=m_nFocusRecordIndex;
	CString m_strOrigTrainId = m_pDoc->GetTrainShowId(m_sFocusTrain);

	if(trainid == m_strOrigTrainId)
	{
		AfxMessageBox("��ע��, ������ķֽ������������ͬ,���������ֽ�");
		return;
	}

	CTG_TRAIN* tmpTrain1 = NULL;

	long dlt1 = 3*3600;
	long nSelX  = rec->arrive;
	
	map<TRAIN_KEY,  CTG_TRAIN>::iterator it = this->myPDataManage->myTrain.begin();
	while(it != this->myPDataManage->myTrain.end())
	{
		if(m_pDoc->GetTrainShowId(it->second) == trainid)
		{
			TgTRAIN_RECORD *rec1 = it->second.GetStationRecord(rec->station);
			if(rec1 != NULL)
			{
				long nTmpX = rec1->arrive;
				if((nTmpX >= nSelX) && ((nTmpX - nSelX) < dlt1))
				{
					dlt1 = nTmpX - nSelX;
					tmpTrain1 = &it->second;
				}
			}
		}
		it++;
	}

	if(tmpTrain1 == NULL)
	{
		CString str;
		str.Format("��ע��, ����ѡ����г�����3Сʱ���Ҳ����г� %s,���ֽܷ�", trainid);
		AfxMessageBox(str);
		return;
	}

	TgTRAIN_RECORD *rec1 = tmpTrain1->GetStationRecord(rec->station);
	
	if(rec1 == NULL) return;

	CTG_TRAIN splitTrain1(*tmpTrain1), split(*m_sFocusTrain);

	rec1 = splitTrain1.GetFirstRecord(); 
	if(rec1 == NULL) return;
	while(rec1->station != rec->station)
	{
		splitTrain1.DeleteHeadRecord();
		rec1 = splitTrain1.GetFirstRecord();
		if(rec1 == NULL) break;
	}
	if(rec1 == NULL) 
		return;

	SetFlagStart(rec1->flag);
	
	rec1->szAbsText.Format("%s��%s�ֽ�",trainid, m_strOrigTrainId);
	
	rec = split.GetRecordByRecIndex(nSelIndex);

	SetFlagDivided(rec->flag);
	SetFlagDivide(rec1->flag);
	
	rec1->special_train_index = split.m_nTrainIndex;
	rec->special_train_index  = splitTrain1.m_nTrainIndex;

	m_pDoc->sysprint(4,0,"[������ҵ] ����Ա���� �ֽ��г� �г� %s",rec1->szAbsText);

	m_pDoc->SendUpdateTrain(splitTrain1);
	m_pDoc->SendUpdateTrain(split);
}

void CTGForm::OnSplitTrain(const CString& trainid1, const CString& trainid2)
{
	if(trainid2 == trainid1)
	{
		AfxMessageBox("��ע��, ������ķֽ������������ͬ,���������ֽ�");
		return;
	}

	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(rec == NULL) return;
	CString m_strOrigTrainId = m_pDoc->GetTrainShowId(m_sFocusTrain);
	
	long dlt1 = 3*3600;
	long nSelX  = m_pDoc->GetOffsetPixelFromGraphBegin(rec->arrive);
	CTG_TRAIN* tmpTrain1 = NULL;
	map<TRAIN_KEY,  CTG_TRAIN>::iterator it = this->myPDataManage->myTrain.begin();
	while(it != this->myPDataManage->myTrain.end())
	{
		if(m_pDoc->GetTrainShowId(it->second) == trainid1)
		{
			TgTRAIN_RECORD *rec1 = it->second.GetStationRecord(rec->station);
			if(rec1 != NULL)
			{
				long nTmpX = m_pDoc->GetOffsetPixelFromGraphBegin(rec1->arrive);
				if((nTmpX >= nSelX) && ((nTmpX - nSelX) < dlt1))
				{
					dlt1 = nTmpX - nSelX;
					tmpTrain1 = &it->second;
				}
			}
		}
		it++;
	}

	long dlt2 = 3*3600;
	CTG_TRAIN* tmpTrain2 = NULL;
	it = this->myPDataManage->myTrain.begin();
	while(it != this->myPDataManage->myTrain.end())
	{
		if(m_pDoc->GetTrainShowId(it->second) == trainid2)
		{
			TgTRAIN_RECORD *rec2 = it->second.GetStationRecord(rec->station);
			if(rec2 != NULL)
			{
				long nTmpX = m_pDoc->GetOffsetPixelFromGraphBegin(rec2->arrive);
				if((nTmpX >= nSelX) && ((nTmpX - nSelX) < dlt2))
				{
					dlt2 = nTmpX - nSelX;
					tmpTrain2 = &it->second;
				}
			}
		}
		it++;
	}

	if(tmpTrain1 == NULL)
	{
		CString str;
		str.Format("��ע��, ����ѡ����г�����3Сʱ���Ҳ����г� %s,���ֽܷ�", trainid1);
		AfxMessageBox(str);
		return;
	}
	if(tmpTrain2 == NULL)
	{
		CString str;
		str.Format("��ע��, ����ѡ����г�����3Сʱ���Ҳ����г� %s,���ֽܷ�", trainid2);
		AfxMessageBox(str);
		return;
	}
	
	TgTRAIN_RECORD *rec1 = tmpTrain1->GetStationRecord(rec->station);
	TgTRAIN_RECORD *rec2 = tmpTrain2->GetStationRecord(rec->station);

	if(rec1 == NULL || rec2 == NULL) return;

	CTG_TRAIN splitTrain1(*tmpTrain1), splitTrain2(*tmpTrain2), split(*m_sFocusTrain);

	TgTRAIN_RECORD* tmp = split.GetLastRecord(); 
	if(tmp == NULL) return;
	while(tmp->station != rec->station)
	{
		split.DeleteTailRecord();
		tmp = split.GetLastRecord();
		if(tmp == NULL) break;
	}
	if(tmp == NULL) 
		return;
	tmp->depart = tmp->arrive;
	SetFlagEnd(tmp->flag);
	if(IsFlagStopAbs(tmp->flag)) CancelFlagStopAbs(tmp->flag);
	if(IsFlagStopSignal(tmp->flag)) CancelFlagStopSignal(tmp->flag);

	rec1 = splitTrain1.GetFirstRecord(); 
	if(rec1 == NULL) return;
	while(rec1->station != rec->station)
	{
		splitTrain1.DeleteHeadRecord();
		rec1 = splitTrain1.GetFirstRecord();
		if(rec1 == NULL) break;
	}
	if(rec1 == NULL) return;

	rec2 = splitTrain2.GetFirstRecord(); 
	if(rec2 == NULL) return;
	while(rec2->station != rec->station)
	{
		splitTrain2.DeleteHeadRecord();
		rec2 = splitTrain2.GetFirstRecord(); 
		if(rec2 == NULL) break;
	}
	if(rec2 == NULL) return;

	SetFlagStart(rec1->flag);
	SetFlagStart(rec2->flag);

	rec1->szAbsText.Format("%s��%s�ֽ�",trainid1, m_strOrigTrainId);
	rec2->szAbsText.Format("%s��%s�ֽ�",trainid2, m_strOrigTrainId);

	SetFlagDivided(tmp->flag);
	SetFlagDivide(rec1->flag);
	SetFlagDivide(rec2->flag);

	rec1->special_train_index = split.m_nTrainIndex;
	rec2->special_train_index = splitTrain1.m_nTrainIndex;
	tmp->special_train_index  = splitTrain2.m_nTrainIndex;

	m_pDoc->sysprint(4,0,"[������ҵ] ����Ա���� �ֽ��г� �г� %s %s ",rec1->szAbsText,rec2->szAbsText);

	m_pDoc->SendUpdateTrain(splitTrain1);
	m_pDoc->SendUpdateTrain(split);
	m_pDoc->SendUpdateTrain(splitTrain2);
}

// �г����
void CTGForm::OnCombineTrain(void)
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		MessageBox("��ע��: ��û���޸������ߵ�Ȩ��", "��ʾ", MB_ICONWARNING|MB_OK);
		return;
	}

	TgTRAIN_RECORD *prec = GetFoucsTrainRecord();
	if(NULL == prec) 
		return;					

	CDlgCombineTrain dlg(this);
	dlg.m_strNewTrainId = m_pDoc->GetTrainShowId(m_sFocusTrain);
	if(dlg.DoModal() != IDOK)
		return;

	if(dlg.m_strTrainId1 != dlg.m_strTrainId2)
	{
		if(dlg.m_strNewTrainId != dlg.m_strTrainId1 && dlg.m_strNewTrainId != dlg.m_strTrainId2)
		{
			OnCombineTrain(dlg.m_strTrainId1, dlg.m_strTrainId2);
		}
	}
}

void CTGForm::OnCombineTrain(const CString& trainid1, const CString& trainid2)
{
	if(trainid2 == trainid1)
	{
		AfxMessageBox("��ע��, �������������ϳ�����ͬ,�����������");
		return;
	}

	TgTRAIN_RECORD* prec=GetFoucsTrainRecord();
	if(prec == NULL)
		return;

	int nSelIndex = m_nFocusRecordIndex;
	
	CString m_strNewTrainId = m_pDoc->GetTrainShowId(m_sFocusTrain);
	TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByRecIndex(nSelIndex);
	if(rec == NULL) return;

	long dlt1 = 3*3600;
	long nSelX  = rec->arrive;

	CTG_TRAIN* tmpTrain1 = NULL;
	map<TRAIN_KEY,  CTG_TRAIN>::iterator it = this->myPDataManage->myTrain.begin();
	while(it != this->myPDataManage->myTrain.end())
	{
		if(m_pDoc->GetTrainShowId(it->second) == trainid1)
		{
			TgTRAIN_RECORD *rec1 = it->second.GetStationRecord(rec->station);
			if(rec1 != NULL)
			{
				long nTmpX = rec1->arrive;
				if((nSelX >= nTmpX) && ((nSelX - nTmpX) < dlt1))
				{
					dlt1 = nSelX - nTmpX;
					tmpTrain1 = &it->second;
				}
			}
		}
		it++;
	}

	long dlt2 = 3*3600;
	CTG_TRAIN* tmpTrain2 = NULL;
	it = this->myPDataManage->myTrain.begin();
	while(it != this->myPDataManage->myTrain.end())
	{
		if(m_pDoc->GetTrainShowId(it->second) == trainid2)
		{
			TgTRAIN_RECORD *rec2 = it->second.GetStationRecord(rec->station);
			if(rec2 != NULL)
			{
				long nTmpX = rec2->arrive;
				if((nSelX >= nTmpX) && ((nSelX - nTmpX) < dlt2))
				{
					dlt2 = nSelX - nTmpX;
					tmpTrain2 = &it->second;
				}
			}
		}
		it++;
	}

	if(tmpTrain1 == NULL)
	{
		CString str;
		str.Format("��ע��, ����ѡ����г�ǰ��3Сʱ���Ҳ����г� %s,�������", trainid1);
		AfxMessageBox(str);
		return;
	}
	if(tmpTrain2 == NULL)
	{
		CString str;
		str.Format("��ע��, ����ѡ����г�ǰ��3Сʱ���Ҳ����г� %s,�������", trainid2);
		AfxMessageBox(str);
		return;
	}

	if(dlt2 > dlt1)
	{
		CTG_TRAIN* tmp = tmpTrain2;
		tmpTrain2 = tmpTrain1;
		tmpTrain1 = tmp;
	}
	CTG_TRAIN train1(*tmpTrain1), train2(*tmpTrain2), train(*m_sFocusTrain);

	TgTRAIN_RECORD* tmp = train.GetFirstRecord(); 
	if(tmp == NULL) return;
	while(tmp->station != rec->station)
	{
		train.DeleteHeadRecord();
		tmp = train.GetFirstRecord(); 
		if(tmp == NULL) break;
	}
	if(tmp == NULL) return;

	TgTRAIN_RECORD* rec1 = train1.GetLastRecord(); 
	if(rec1 == NULL) return;
	while(rec1->station != rec->station)
	{
		train1.DeleteTailRecord();
		rec1 = train1.GetLastRecord(); 
		if(rec1 == NULL) break;
	}
	if(rec1 == NULL) return;


	TgTRAIN_RECORD* rec2 = train2.GetLastRecord(); 
	if(rec2 == NULL) return;
	while(rec2->station != rec->station)
	{
		train2.DeleteTailRecord();
		rec2 = train2.GetLastRecord(); 
		if(rec2 == NULL) break;
	}
	if(rec2 == NULL) return;

	tmp->depart = tmp->arrive;
	if(IsFlagStopAbs(tmp->flag)) CancelFlagStopAbs(tmp->flag);
	if(IsFlagStopSignal(tmp->flag)) CancelFlagStopSignal(tmp->flag);
	SetFlagStart(tmp->flag);
	SetFlagCombine(tmp->flag);

	tmp->szAbsText.Format("%s��%s��ϳ�%s", trainid1, trainid2, m_strNewTrainId);
	
	rec1->depart = rec1->arrive;
	if(IsFlagStopAbs(rec1->flag)) CancelFlagStopAbs(rec1->flag);
	if(IsFlagStopSignal(rec1->flag)) CancelFlagStopSignal(rec1->flag);
	SetFlagEnd(rec1->flag);
	SetFlagCombined(rec1->flag);


	rec2->depart = rec2->arrive;
	if(IsFlagStopAbs(rec2->flag)) CancelFlagStopAbs(rec2->flag);
	if(IsFlagStopSignal(rec2->flag)) CancelFlagStopSignal(rec2->flag);
	SetFlagEnd(rec2->flag);
	SetFlagCombined(rec2->flag);

	rec1->special_train_index = train2.m_nTrainIndex;
	rec2->special_train_index = train.m_nTrainIndex;
	tmp->special_train_index = train1.m_nTrainIndex;

	m_pDoc->sysprint(4,0,"[������ҵ] ����Ա���� ����г� �г� %s",tmp->szAbsText);

	m_pDoc->SendUpdateTrain(train);
	m_pDoc->SendUpdateTrain(train1);
	m_pDoc->SendUpdateTrain(train2);
}

void CTGForm::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	m_bMouseDown = TRUE;
	int i;
	CRect rect;

	clientDC.DPtoLP(&point);

	///////////////////////////////////////////////	
	m_sFocusOtherTrain.clear();
	CPoint nPoint = point;

	ClientToScreen(&nPoint);
	m_nMouseCurDownPoint = point;
	m_nOldPoint = point;

	if(!IsCTRLpressed())
	{
		for(int idx = 0; idx < m_SelectedTrains.size(); idx++)
		{
			CTG_TRAIN* ptrain = this->myPDataManage->GetTrain(m_SelectedTrains[idx]);
			if(NULL != ptrain)
			{
				RefreshTrain(ptrain);
			}
		}
		m_SelectedTrains.clear();
	}

	//���þ���ѡ������ʼλ�ã�����ѡ�ж���г�
	m_TrainsSelectedRect.SetRect(point,point);

	BYTE timeLength = 0;
	bool bCanChangeHis = m_pDoc->m_ptgctrl->mConfigTg.m_SingleTrainCfg.CanChangeHistorySchd(timeLength);

	bool bChange = false; 
	if(bCanChangeHis)
	{
		CTime tt = CTime::GetCurrentTime();
		int nHour = tt.GetHour();
		if(nHour >= 18)
		{
			if((nHour - 18) <= timeLength)
				bChange = true;
		}
		else if(nHour >= 6)
		{
			if((nHour - 6) <= timeLength)
				bChange = true;
		}
	}

	if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
	{
		if(m_pDoc->m_nOperatorStatus == MUTI_TRAINS_SEL_INIT)
		{
			m_pDoc->m_nOperatorStatus = MUTI_TRAINS_SEL_ING;
			CScrollView::OnLButtonDown(nFlags, point);
			return;
		}

		if(m_pDoc->m_nOperatorStatus == MUTI_TRAINS_SEL_ING)
		{
			//����������ʷͼ��Χ����ôֱ�ӷ���			
			if (!m_pctrl->IsNewMode())
			{
				if(point.x <= m_pDoc->m_fPixelPerHour*12 && !bChange)
				{
					m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
					return;
				}
			}

			if(m_selTrains.size()>0)
			{
				m_pDoc->m_nOperatorStatus = MOVE_MUTI_TRAINS;
				m_bMouseDown = FALSE;
				mMutiMoveLastPoint = point;
				mOrgMutiMoveLastPoint = point;
				CScrollView::OnLButtonDown(nFlags, point);
				return;
			}
			else
			{
				m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			}
		}
		if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN) 
		{
			if (!m_pctrl->IsNewMode())
			{
				//����������ʷͼ��Χ����ôֱ�ӷ���			
				if(point.x <= m_pDoc->m_fPixelPerHour*12 && !bChange)
					return;
			}
		}
		
		// prev work schedule cann't be changed
		if (!m_pctrl->IsNewMode())
		{
			if(point.x <= m_pDoc->m_fPixelPerHour*12 && !bChange )
			{
				//������г���ѡ�У���ô�鿴���г��Ƿ��а������ʷͼ��
				if(m_sFocusTrain != NULL)
				{
					CTG_TRAIN train = *m_sFocusTrain;
					for (i=0;i<train.m_nPointCount;i++)
					{
						CPoint pt = train.m_pPerPointInfo[i].point;
						CRect rect(CPoint(pt.x - 5,pt.y - 5), CSize(10,10));
						if(rect.PtInRect(point))
						{
							break;					
						}
					}

					//������г��ĳ�վ���򱻵��У���ôӦ�ü���������ǰ������ʷͼ������û�г���ѡ��
					if(i >= train.m_nPointCount)
					{
						m_sFocusTrain = NULL;
						m_nFocusRectIndex = -1;
						m_nFocusRecordIndex = -1;

						this->DrawTrain(train, false);
						return;		
					}
				}
				else
				{
					return;
				}
			}
		}
	}
	//rescue train operate
	if(this->m_bRescueTrainStatus)
	{
		RescueTrain(point);

		m_bMouseDown = FALSE;
		this->m_bRescueTrainStatus= false;
		CScrollView::OnLButtonDown(nFlags, point);
		return;
	}
	//end of rescue train

	if(this->m_bUniteTrainStatus)
	{
		UniteTrains(point);
		this->m_bUniteTrainStatus = false;
		CScrollView::OnLButtonDown(nFlags, point);
		return;
	}

	bool bFoundMark = FALSE;
	bool bFoundNewTrainFlag = true;
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS ||
		m_pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS)
	{
		m_pDoc->nCurrentMouseX = point.x;
		if(m_sFocusTrain != NULL)
		{
			// �г����ϵ�ҿ��ش򿪵������
			if(m_pctrl->mConfigTg.m_context.m_bRelationTrainBold) 
			{
				TRAIN_CHEZUHAO* pFocusCzh = m_pDoc->GetCurrentSelTrainCZH();
				if(IsSelFocusTrainByPoint(point))
				{
					bFoundNewTrainFlag=false;
					if(pFocusCzh==NULL)
						RefreshTrain(m_sFocusTrain);
				}
				else
				{
					m_nFocusTrainIndex=0;
					m_nFocusRectIndex=-1;
					m_nFocusRecordIndex=-1;
					if(pFocusCzh==NULL)
						RefreshTrain(m_sFocusTrain);
					m_sFocusTrain = NULL;
					m_pDoc->QueryCurrentTrain(&point, m_nFocusTrainIndex);
				}
				RefreshCzhOtherTrain();
				ClearTrainCZHLine(NULL);
			}
			else
			{
				ClearTrainCZHLine(NULL);
				if(IsSelFocusTrainByPoint(point))
				{
					bFoundNewTrainFlag=false;
					RefreshTrain(m_sFocusTrain);
				}
				else
				{
					m_nFocusTrainIndex=0;
					m_nFocusRectIndex=-1;
					m_nFocusRecordIndex=-1;
					RefreshTrain(m_sFocusTrain);
					m_sFocusTrain = NULL;
					m_pDoc->QueryCurrentTrain(&point, m_nFocusTrainIndex);
				}
			}
		}
		else
		{
			ClearTrainCZHLine(NULL);
			m_pDoc->QueryCurrentTrain(&point, m_nFocusTrainIndex);
		}
			
		if(IsCTRLpressed() && m_nFocusTrainIndex > 0)
		{
			for(int idx =0; idx < m_SelectedTrains.size(); idx++)
			{
				if(m_nFocusTrainIndex == m_SelectedTrains[idx])
				{
					break;
				}
			}

			if(idx >= m_SelectedTrains.size())
				this->m_SelectedTrains.push_back(m_nFocusTrainIndex);
			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			return;
		}

		if (m_pDoc->IsOnlyShowMark()) 
		{
			m_nFocusTrainIndex=0;
			m_nFocusRectIndex=-1;
			m_nFocusRecordIndex=-1;
		}

		int tmpTrainIndex=0;
		if(m_nFocusTrainIndex > 0)
		{
			if(m_sFocusTrain)
			{
				if(m_nFocusTrainIndex != m_sFocusTrain->GetTrainIndex())
				{
					m_nFocusRectIndex=-1;
					m_nFocusRecordIndex=-1;
					RefreshTrain(m_sFocusTrain);
					m_sFocusTrain = NULL;
				}
			}

			if(bFoundNewTrainFlag)
			{
				CTG_TRAIN* tmp = myPDataManage->GetTrain(m_nFocusTrainIndex);
				if(tmp)
				{
					m_pDoc->m_myFocusTrain = *tmp;
					m_sFocusTrain = &m_pDoc->m_myFocusTrain;
					IsSelFocusTrainByPoint(point);
					RefreshTrain(m_sFocusTrain);
				}
				else
				{
					m_nFocusTrainIndex=0;
					m_nFocusRectIndex=-1;
					m_nFocusRecordIndex=-1;
					m_sFocusTrain = NULL;
				}
			}

			if(m_sFocusTrain == NULL)
				return;
			
			// ��ʾ��ǰѡ�н�·
			TRAIN_CHEZUHAO* pSelTrainCZH = NULL;
			if(m_sFocusTrain)
			{
				if(m_pctrl->mConfigTg.m_context.IsEnableCZH()) // ��ʾ
				{
					CString czh=m_sFocusTrain->myTrainInfo.shift_index_string;
					pSelTrainCZH = m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
				}
				else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
				{
					TRAIN_INDEX train_index=m_sFocusTrain->m_nTrainIndex;
					pSelTrainCZH = m_pDoc->myChezuhao.GetCzhByTrainIndex(train_index);
				}
			}
			if(pSelTrainCZH)
			{
				m_pDoc->SetCurrentSelTrainCZH(pSelTrainCZH->czh);
			}
			else
			{
				m_pDoc->SetCurrentSelTrainCZH("");
			}
			
			tmpTrainIndex = m_nFocusTrainIndex;
			
			TRAIN_CHEZUHAO* pFocusCzh = m_pDoc->GetCurrentSelTrainCZH();
			// �г����ϵ�ҿ��ش򿪵������
			if(m_pctrl->mConfigTg.m_context.m_bRelationTrainBold && (pFocusCzh!=NULL)) 
			{
				RefreshCzhOtherTrain();
			}
			else
			{
				RefreshTrain(m_sFocusTrain);
			}
			
			if(m_pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS)
			{
				MouseRecievePoint(point);
			}
		}
		else 
		{
			m_nFocusTrainIndex=0;
			m_nFocusRectIndex=-1;
			m_nFocusRecordIndex=-1;
			m_sFocusTrain = NULL;
			tmpTrainIndex = 0;
		}

		int stano1 = 0, stano2 = 0;
		
		m_pDoc->PointBetweenMidOfTwoStation(point.y, stano1, stano2);
		long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
		
		if(m_sFocusTrain!=NULL)
		{
			int rec_index=m_nFocusRecordIndex;
			this->m_pctrl->FIRE_TG_SELECT_TRAIN(tmpTrainIndex, nFlags, stano1, stano2, rec_index, usTimeValue);
		}

		if(m_nFocusFigure != NULL)
		{
			CTg_BLOCK* pBlock = myPDataManage->GetMark(m_nFocusFigure->mark_index);
			if(pBlock != NULL)
			{
				pBlock->RestoreDrawFlag();
				EraseBlock(*pBlock);
				m_pDoc->DrawSingleMark(*pBlock, &m_Layer1);
				
				RefreshRect(pBlock->myPosition);
			}
			else
			{
				m_nFocusFigure = NULL;
				m_nFocusBlockIndex = 0;
			}
		}

		if(m_sFocusTrain == NULL)
		{
			m_nFocusBlockIndex = m_pDoc->QueryCurrentBlock(point);

			if (m_pDoc->IsOnlyShowTrain()) 
			{
				m_nFocusBlockIndex = 0;
			}

			if(m_nFocusBlockIndex > 0)
			{
				CTg_BLOCK* tmp = myPDataManage->GetMark(m_nFocusBlockIndex);
				if(tmp != NULL)
				{
					m_pDoc->m_myFocusBlock = *tmp;
					m_nFocusFigure = &m_pDoc->m_myFocusBlock;

					this->m_pctrl->FIRE_TG_SELECT_MARK(m_nFocusFigure->mark_index,
						m_nFocusFigure->type,
						m_nFocusFigure->flag, 
						m_nFocusFigure->station1, m_nFocusFigure->station2,0);
				}

			}
			else
				m_nFocusFigure = NULL;

			if(m_nFocusFigure==NULL)
				this->m_pctrl->FIRE_TG_SELECT_TRAIN(0, nFlags, stano1, stano2, 0, usTimeValue);
		}

		if(m_nFocusFigure)
		{
			if(m_sFocusTrain != NULL)
				m_nFocusFigure->m_bFocus = FALSE;
			else
				m_nFocusFigure->m_bFocus = TRUE;
			
			m_pDoc->DrawSingleMark(*m_nFocusFigure, &m_Layer1);
			
			RefreshRect(m_nFocusFigure->myPosition);

			if(m_sFocusTrain != NULL)
				m_nFocusFigure = NULL;
		}	 
	}

	if(m_pDoc->m_nOperatorStatus == DRAW_MARK_ITEM_STATUS)
	{
		if(m_sFocusTrain != NULL)
			m_sFocusTrain = NULL;

		if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
		{
			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			MessageBox("û�л��Ʊ�ǵ�Ȩ��", "��ʾ", MB_ICONWARNING|MB_OK);
			return;
		}
		do
		{
			if(!m_bFirstPoint)
			{
				m_nSelectedStationList[0] = m_pDoc->ConvertYPostoStationNo(point.y,m_nSelectedNodeIndexList[0]);
				if(m_nSelectedStationList[0] == -1)
				    m_nSelectedStationList[0] = m_pDoc->ConvertYPostoSideUseStationNo(point.y,m_nSelectedNodeIndexList[0]);

				if(m_nSelectedStationList[0] == -1 && m_nMarkType != GRAPH_NOTE_MARK && m_nMarkType != NEW_GRAPH_NOTE_MARK)
					break;

				//�жϵ��г�վ�Ƿ�������վ��Χ��
				if(m_nMarkType == ABS_ELE_FIX_MARK)//�����������
				{
					
				}
				m_bFirstPoint = true;
				SetCapture();
				m_absDragRect.SetRect(point.x, point.y, point.x, point.y);
			}
			else
			{
				m_nSelectedStationList[1] = m_pDoc->ConvertYPostoStationNo(point.y,m_nSelectedNodeIndexList[1]);
				if(m_nSelectedStationList[1] == -1)
				     m_nSelectedStationList[1] = m_pDoc->ConvertYPostoSideUseStationNo(point.y,m_nSelectedNodeIndexList[1]);
				if(m_nSelectedStationList[1] == -1&& m_nMarkType != GRAPH_NOTE_MARK && m_nMarkType != NEW_GRAPH_NOTE_MARK)
					break;
				//�жϵ��г�վ�Ƿ�������վ��Χ��
				if(m_nMarkType == ABS_ELE_FIX_MARK)//�����������
				{
				}
				else if(m_nMarkType == ABS_SLOW_MARK)//ֻ�ܻ�һ������
				{
					if(!m_pDoc->JudeNodesConnected(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1])) //�ڶ������г�վ����Ϊ��һ�����г�վ����һվ������һվ
					{
						CString str;
						str = "���仺�в��ܿ����������,�����»�����������!";
						int index1 = m_nSelectedNodeIndexList[0];
						int index2 = m_nSelectedNodeIndexList[1];

						int stano1=-1,stano2=-1;
						if( 0 <= index1 && index1 < m_pDoc->m_nDispDistanceNum)
							stano1 = m_pDoc->m_sDispDistance[index1].station_number;
						if( 0 <= index2 && index2 < m_pDoc->m_nDispDistanceNum)
							stano2 = m_pDoc->m_sDispDistance[index2].station_number;

						if(stano1!=-1&&stano2!=-1)
						{
							str.Format("%s �� %s ���㻭�����䲻����", m_pDoc->GetStationName(stano1), m_pDoc->GetStationName(stano2));
						}
						MessageBox(str, "��ʾ", MB_ICONWARNING|MB_OK);
						DrawBox(clientDC, m_absDragRect);
						m_bFirstPoint = false;
						break;				
					}

				}
				else if(m_nMarkType == ABS_INHIBIT_MARK)//ֻ�ܻ�һ������
				{
					if(!m_pDoc->JudeNodesConnected(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1])) //�ڶ������г�վ����Ϊ��һ�����г�վ����һվ������һվ
					{
						CString str;
						str = "����������ܿ����������,�����»����������!";
						int index1 = m_nSelectedNodeIndexList[0];
						int index2 = m_nSelectedNodeIndexList[1];

						int stano1=-1,stano2=-1;
						if( 0 <= index1 && index1 < m_pDoc->m_nDispDistanceNum)
							stano1 = m_pDoc->m_sDispDistance[index1].station_number;
						if( 0 <= index2 && index2 < m_pDoc->m_nDispDistanceNum)
							stano2 = m_pDoc->m_sDispDistance[index2].station_number;

						if(stano1!=-1&&stano2!=-1)
						{
							str.Format("%s �� %s ���㻭�����䲻����", m_pDoc->GetStationName(stano1), m_pDoc->GetStationName(stano2));
						}
						MessageBox(str, "��ʾ", MB_ICONWARNING|MB_OK);
						DrawBox(clientDC, m_absDragRect);
						m_bFirstPoint = false;
						break;				
					}
				}

				m_bSecondPoint = true;
			}

			if (m_bFirstPoint && m_bSecondPoint) 
			{
				m_bFirstPoint = false;
				m_bSecondPoint = false;

				ReleaseCapture();

				::SetCursor(::LoadCursor (NULL,IDC_ARROW));
				// Erase the bounding box
				if(m_absDragRect.top > m_absDragRect.bottom )
				{
					int  temp = m_absDragRect.top ;
					m_absDragRect.top = m_absDragRect.bottom;
					m_absDragRect.bottom = temp;					
				} 
				if(m_absDragRect.left > m_absDragRect.right )
				{
					int  temp = m_absDragRect.left ;
					m_absDragRect.left = m_absDragRect.right;
					m_absDragRect.right = temp;					
				} 
				DrawBox(clientDC, m_absDragRect);

				CPoint ptlStart,ptlEnd;
				clientDC.DPtoLP(&ptlStart);

				ptlStart = m_absDragRect.TopLeft();
				ptlEnd = m_absDragRect.BottomRight() ;

				//lyc 2002/11/22
				int minX,minY,maxX,maxY;
				minX = min(ptlStart.x,ptlEnd.x);
				maxX = max(ptlStart.x,ptlEnd.x);
				minY = min(ptlStart.y,ptlEnd.y);
				maxY = max(ptlStart.y,ptlEnd.y);
				ptlStart.x = minX;
				ptlStart.y = minY;
				ptlEnd.x = maxX;
				ptlEnd.y = maxY;
				//end

				if(ptlStart == ptlEnd) return;
			
				long usStartTimeValue = m_pDoc->ConvertXPostoTime(ptlStart.x);
				long usEndTimeValue = m_pDoc->ConvertXPostoTime(ptlEnd.x);


				int y1=0,y2=0;

				if (m_nMarkType != GRAPH_NOTE_MARK && m_nMarkType != NEW_GRAPH_NOTE_MARK)
				{
					if(m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[0]].show_sation_sep)
					{
						int side_num = m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[0]].number_of_side;
						int side_no = m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[0]].side[side_num].side_number;
						y1 = m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[0]].nSideYPos[side_no];

					}
					else
					{
						y1 = m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[0]].nStationYPos;
					}
					y2 = m_pDoc->m_sDispDistance[m_nSelectedNodeIndexList[1]].nStationYPos;
				}

				CRect absRect;
				absRect.SetRect(ptlStart.x, y1, ptlEnd.x, y1);		

				//վ��ͼ�κ�ͼ��ע�⡢����ע�ⲻ�ÿ����Ƿ���ͬһվ��
				if(m_nMarkType == SIDE_INHIBIT_MARK 
					|| m_nMarkType == SIDE_TIANCHUANG_MARK
					|| m_nMarkType == SIDE_SLOW_MARK
					|| m_nMarkType == SIDE_OCCUPY_MARK
					|| m_nMarkType == GRAPH_NOTE_MARK || m_nMarkType == NEW_GRAPH_NOTE_MARK
					)
				{
					;
				}
				else 
				{
					if(m_nSelectedNodeIndexList[0] == m_nSelectedNodeIndexList[1])
					{			   
						m_bFirstPoint = true;	
						Invalidate(FALSE);
						return;
					}
				}

				m_nFocusFigure = new CTg_BLOCK;

				m_nFocusFigure->mark_index = 0;
				m_nFocusFigure->type = m_nMarkType;
				m_nFocusFigure->mColor = CLR_RED;
				strcpy(m_nFocusFigure->text,""); 
				m_nFocusFigure->count = 0;
				m_nFocusFigure->line = 0;
				m_nFocusFigure->e_kilo = 0;
				m_nFocusFigure->e_meter = 0;
				m_nFocusFigure->s_meter = 0;
				m_nFocusFigure->slow_time = 0;
				m_nFocusFigure->flag = 0;
				m_nFocusFigure->position = 0;

				if(m_nMarkType == SIDE_SLOW_MARK ||
					m_nMarkType == SIDE_TIANCHUANG_MARK||
					m_nMarkType == SIDE_OCCUPY_MARK||
					m_nMarkType == SIDE_INHIBIT_MARK)
				{
					m_nFocusFigure->station = m_nSelectedStationList[0];
					m_nFocusFigure->index = m_nSelectedNodeIndexList[0];
					m_nFocusFigure->flag = 0;
					if(m_nMarkType == SIDE_SLOW_MARK)
						m_nFocusFigure->flag = 10/5;
					if(m_nMarkType == SIDE_INHIBIT_MARK)
					{
						if(m_nSubMarkType==SIDE_LOST_ELE_MARK)
							m_nFocusFigure->slow_time |= 0x0001; // �ɵ�ͣ��
						else
							m_nFocusFigure->slow_time |= 0x0000; // �ɵ�����
					}
				}
				else if(m_nMarkType == ABS_ELE_FIX_MARK || m_nMarkType == ABS_FIX_MARK)//����������ޡ�
				{
					// �ڵ��
					m_nFocusFigure->station = min(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);
					m_nFocusFigure->index = max(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);

					// �Ƿ�������·���վ,0,����������1���Ϸ���վ������2���·���վ����
					m_nFocusFigure->slow_time = 0;
					m_nFocusFigure->flag = 1;// flag = 0;//����λ��,0,�Ϸ���1���м䣬2���·�
					strcpy(m_nFocusFigure->text,"��������"); 
				}
				else
				{
					m_nFocusFigure->station = min(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);
					m_nFocusFigure->index = max(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);

					if(m_nMarkType == ABS_SLOW_MARK)
						m_nFocusFigure->flag = 45/5;
				}

				if(m_nMarkType == SELF_CLOSE_MARK)
				{
					m_nFocusFigure->flag = this->m_pctrl->mConfigTg.m_SelfCloseMarkCfg.init;
				}

				m_nFocusFigure->start_time = usStartTimeValue;
				m_nFocusFigure->end_time = usEndTimeValue;
						

				if(m_nMarkType == GRAPH_NOTE_MARK)
				{
					m_nFocusFigure->m_drawRect =  m_absDragRect;
					m_nFocusFigure->station = m_absDragRect.top;
					m_nFocusFigure->position = m_absDragRect.bottom ;

					int usRectCenterY =(m_absDragRect.top +m_absDragRect.bottom)/2;
					int nStationNum;
					int  nRatio =  m_pDoc->ConvertYPostoStationNos(usRectCenterY,m_nSelectedNodeIndexList[0],nStationNum);
					if(nRatio >0 && nStationNum>=0)
					{
						m_nFocusFigure->index = m_nSelectedNodeIndexList[0]; 
						m_nFocusFigure->line =nRatio;     
						m_nFocusFigure->count = nStationNum; //��վ����Ŀ
					}
					else
					{
						m_nFocusFigure->index = m_nSelectedNodeIndexList[0];
						m_nFocusFigure->line =  0;
						m_nFocusFigure->count = 0;
					}
				}

				if(m_nMarkType == NEW_GRAPH_NOTE_MARK)
				{
					m_nFocusFigure->m_drawRect =  m_absDragRect;
					m_pDoc->SetGraphPosition(m_nFocusFigure, m_absDragRect);
				}
				
				/////////////////////////////////////////////////////////////////
				BOOL sendtrue = TRUE;
				if(m_nMarkType == SIDE_SLOW_MARK ||
					m_nMarkType == SIDE_TIANCHUANG_MARK||
					m_nMarkType == SIDE_OCCUPY_MARK||
					m_nMarkType == SIDE_INHIBIT_MARK)
				{
					m_nFocusFigure->count = 0;

					// ���÷����ɵ�
					int sideno=m_pDoc->GetSideUseByYPos(point.y, m_nSelectedNodeIndexList[0]);
					if(sideno>=0)
					{
						m_nFocusFigure->count=1;
						m_nFocusFigure->ele_number[0]=sideno;
					}
					else
					{
						int index = m_nSelectedNodeIndexList[0];
						if(index >= 0 && index < m_pDoc->m_nDispDistanceNum)
						{
							for(int i=0;i<m_pDoc->m_sDispDistance[index].number_of_side && i<MAX_SCHD_BLOCK_ELE_COUNT;i++)
							{
								m_nFocusFigure->ele_number[m_nFocusFigure->count] = m_pDoc->m_sDispDistance[index].side[i].side_number;
								m_nFocusFigure->count++;
							}
						}
					}

					if(m_nMarkType == SIDE_TIANCHUANG_MARK)
						strcpy(m_nFocusFigure->text,"վ���촰��");
					else if(m_nMarkType == SIDE_OCCUPY_MARK)
						strcpy(m_nFocusFigure->text,"վ�ڹɵ�ռ��"); 
					else if(m_nMarkType == SIDE_INHIBIT_MARK)
					{
						if(m_nSubMarkType == SIDE_LOST_ELE_MARK)
							strcpy(m_nFocusFigure->text,"�Ӵ�������");
						else
							strcpy(m_nFocusFigure->text,"վ�ڹɵ�����");
					}
					else
						strcpy(m_nFocusFigure->text,"վ�ڹɵ�����") ;

					SetLineDirectionNone(m_nFocusFigure->line);
				}	

				if(m_nMarkType == ABS_INHIBIT_MARK ||
					m_nMarkType  == ABS_SLOW_MARK ||
					m_nMarkType == SELF_CLOSE_MARK||
					m_nMarkType == ABS_ELE_FIX_MARK||
					m_nMarkType == ABS_FIX_MARK)
				{ 					
					m_nFocusFigure->count = 0;

					int nMixSelectedNodeIndex = min(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);
					int nMaxSelectedNodeIndex =	max(m_nSelectedNodeIndexList[0],m_nSelectedNodeIndexList[1]);

					
					if(m_nMarkType == SELF_CLOSE_MARK || 
						m_nMarkType == ABS_ELE_FIX_MARK || 
						m_nMarkType == ABS_FIX_MARK)
					{
						std::vector<int> abs_list;
						m_pDoc->GetMutiNeibAbsLines(nMixSelectedNodeIndex, nMaxSelectedNodeIndex, abs_list);
						
						m_nFocusFigure->count = 0;
						for(int i=0; i<abs_list.size(); i++)
						{
							m_nFocusFigure->ele_number[m_nFocusFigure->count] = abs_list[i];
							m_nFocusFigure->count++;
							if(m_nFocusFigure->count>=sizeof(m_nFocusFigure->ele_number)/sizeof(m_nFocusFigure->ele_number[0]))
							{
								break;
							}
						}
						abs_list.clear();						
					}
					else if(m_nMarkType == ABS_INHIBIT_MARK || m_nMarkType == ABS_SLOW_MARK)
					{
						m_nFocusFigure->count = 0;
						std::vector<int> abs_list;
						m_pDoc->GetNeibAbsLines(nMixSelectedNodeIndex, nMaxSelectedNodeIndex, abs_list);
						for(int i=0; i<abs_list.size(); i++)
						{
							m_nFocusFigure->ele_number[m_nFocusFigure->count] = abs_list[i];
							m_nFocusFigure->count++;
						}
						abs_list.clear();
					}


					if(m_nMarkType == ABS_ELE_FIX_MARK)
						strcpy(m_nFocusFigure->text,"��������");
					else if(m_nMarkType == ABS_FIX_MARK)
						strcpy(m_nFocusFigure->text,"�ۺ��촰��");
					else if(m_nMarkType == ABS_INHIBIT_MARK)
						strcpy(m_nFocusFigure->text,"��������");
					else
						strcpy(m_nFocusFigure->text,"") ;//ԭΪ�����¹� 

                    SetLineLinePosMiddle(m_nFocusFigure->line);
					SetLineTextPosTop(m_nFocusFigure->line);


					if(m_nMarkType == ABS_INHIBIT_MARK)
					{
						if(m_nMarkType4Inhibit == 0)
						{
							SetLineFlagInhibit(m_nFocusFigure->line);
						}
						else
						{
							SetLineFlagLightTrain(m_nFocusFigure->line);
							strcpy(m_nFocusFigure->text,"");
						}
					}

					if(!m_pDoc->SetBlockDirect(m_nFocusFigure))
					{
						MessageBox("�����䲻�ܻ���ʩ������", "��ʾ");
						sendtrue = FALSE;
					}

					// ����������ģʽ
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableDiffShowBlockPostion())
					{
						if (IsLineDirectionUp(m_nFocusFigure->line))
							SetLineLinePosTop(m_nFocusFigure->line);
						else if (IsLineDirectionDown(m_nFocusFigure->line))
							SetLineLinePosBottom(m_nFocusFigure->line);
					}
				}


				if(m_nMarkType == GRAPH_NOTE_MARK || m_nMarkType == NEW_GRAPH_NOTE_MARK)
				{
					int temp = 1;
					m_nFocusFigure->flag = 1;
					if(m_nSubMarkType == TEXT_NOTE_MARK)
					{
						m_nFocusFigure->flag = 2;
						temp = 2;
						SetCircleRed(m_nFocusFigure->slow_time);
					}

					CDlgNoteText *dlg = new CDlgNoteText(temp, m_nFocusFigure, m_pDoc, this);

					if(dlg->DoModal() == IDOK)
					{
						m_nFocusFigure->flag = dlg->m_nType;
						switch(m_nFocusFigure->flag)
						{
						case 1://����ע��
						case 3://�ص�ע��
						case 5://������
							{
								int len = dlg->m_text.GetLength();
								if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
								{
									AfxMessageBox("����̫��,���ܳ���128��");
									delete m_nFocusFigure;
									m_nFocusFigure = NULL;
									return;
								}

								strncpy(m_nFocusFigure->text,(LPCTSTR)dlg->m_text,len);
								m_nFocusFigure->text[len] = 0;

								m_nFocusFigure->slow_time &= 0xfff8;
								m_nFocusFigure->slow_time += dlg->m_roundFlag;
							}
							break;
						case 2://�¹�ע��
							{
								int len = dlg->m_text2.GetLength();
								if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
								{
									AfxMessageBox("����̫��,���ܳ���128��");
									delete m_nFocusFigure;
									m_nFocusFigure = NULL;
									return;
								}
								strncpy(m_nFocusFigure->text, dlg->m_text2.GetBuffer(0), len);
								m_nFocusFigure->text[len]=0;

								m_nFocusFigure->slow_time &= 0xfff8;
								m_nFocusFigure->slow_time += dlg->m_roundFlag;
							}
							break;
						case 4://����ע��
							{
								int len = dlg->m_text2.GetLength();
								if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
								{
									AfxMessageBox("����̫��,���ܳ���128��");
									delete m_nFocusFigure;
									m_nFocusFigure = NULL;
									return;
								}
								strncpy(m_nFocusFigure->text, dlg->m_text2.GetBuffer(0), len);
								m_nFocusFigure->text[len]=0;

								m_nFocusFigure->slow_time &= 0xfff8;
								m_nFocusFigure->slow_time += dlg->m_roundFlag;
							}
							break;					
						default:
							memset(m_nFocusFigure->text, 0, sizeof(m_nFocusFigure->text));
						}

						if (dlg->m_colorFlag==1)
							SetWordRed(m_nFocusFigure->slow_time);
						else if (dlg->m_colorFlag==2)
							SetWordBlue(m_nFocusFigure->slow_time);

						CString text = m_nFocusFigure->text;
						if(text.GetLength()>3)
						{
							CClientDC dc(this);
							CSize sz = dc.GetTextExtent(m_nFocusFigure->text);
							int CX=(ptlStart.x+ptlEnd.x)/2;
							int CY=(m_nFocusFigure->station+m_nFocusFigure->position)/2;

							usStartTimeValue = m_pDoc->ConvertXPostoTime(CX-sz.cx);
							usEndTimeValue = m_pDoc->ConvertXPostoTime(CX+sz.cx);

							m_nFocusFigure->start_time = usStartTimeValue;
							m_nFocusFigure->end_time = usEndTimeValue;
										
							m_nFocusFigure->station = CY - sz.cy;
							m_nFocusFigure->position = CY + sz.cy;
						}
					}
					else
					{
						sendtrue = FALSE;
					}
					delete dlg;
				}


				if(sendtrue)
				{
					m_pDoc->DrawSingleMark(*m_nFocusFigure, &m_Layer1);
										
					RefreshRect(m_nFocusFigure->myPosition);

					// ͼ���ע --- ���� index line e_kilo
					if(m_nFocusFigure->type == GRAPH_NOTE_MARK)
					{
						SetGraphNoteMarkPos(m_nFocusFigure);
					}
					m_pDoc->SendAppendMark(m_nFocusFigure);

					delete m_nFocusFigure;
					m_nFocusFigure = NULL;
				}
				else
				{
					delete m_nFocusFigure;
					m_nFocusFigure = NULL;
				}

				m_pDoc->m_nOperatorStatus = NORMAL_STATUS;		
			}
		}while(0);
	}

	//��������ǰ��,�����۷��ĳ�
	if( m_pDoc->m_nOperatorStatus == INSERT_TRAIN_GOON ||
		m_pDoc->m_nOperatorStatus == INSERT_TRAIN_RETURN )
	{
		BOOL returnValue = FALSE;
		returnValue = newTrainSimple(point,m_pDoc->m_nOperatorStatus);
		if(returnValue)
		{
			if(m_sFocusTrain == NULL)
				return;

			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

			CTG_TRAIN modifyTrain = *m_sFocusTrain;

			TgTRAIN_RECORD *rec;
			rec = modifyTrain.GetLastRecord();
			if(rec == NULL)
				return;

			SetFlagTerminal(rec->flag);

			rec->depart = rec->arrive;
			rec->exit = -1; 

			CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
			if(train == NULL)
			{
				delete m_sFocusTrain;
				m_pDoc->SendAppendTrain(&modifyTrain, "", 0);
				EraseTrain(modifyTrain);
			}
			else
			{
				m_pDoc->SendUpdateTrain(modifyTrain);
			}

			m_sFocusTrain = NULL;
			m_pDoc->ResetContinueDraw();
		}
		else
		{
			if(m_sFocusTrain)
			{
				m_pDoc->m_nOperatorStatus = NORMAL_STATUS; 
				delete m_sFocusTrain;

				m_sFocusTrain = NULL;	
				m_pDoc->ResetContinueDraw();
			}
		}
		Invalidate(FALSE);	
	}

	//���ƿ�������,�����俪�صĳ�
	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_A ||
		m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_B )
	{
		if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_A)
			newTrainSimple_ABS(point,m_pDoc->m_nOperatorStatus);
		else if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_B )
			newTrainSimple_ABS_B(point,m_pDoc->m_nOperatorStatus);

		if(m_sFocusTrain)
		{
			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			delete m_sFocusTrain;

			m_sFocusTrain = NULL;	
			m_pDoc->ResetContinueDraw();
		}

		Invalidate(FALSE);
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CTGForm::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	m_bMouseDown = FALSE;

	if(IsCTRLpressed())
	{
		for(int idx = 0; idx < m_SelectedTrains.size(); idx++)
		{
			CTG_TRAIN* train = this->myPDataManage->GetTrain(m_SelectedTrains[idx]);
			if(train)
			{
				CClientDC dc(this);
				OnPrepareDC(&dc);
				m_pDoc->DrawSingleTrain(&dc, train, DRAW_SCREEN);
			}
		}
		return;
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
		return;
	}

	
	CRect clientRect;
	GetClientRect(clientRect);

	clientDC.DPtoLP(&point);
	clientDC.DPtoLP(&clientRect);

	if(point.x < 0)
		point.x = 0;
	if(point.y < 0)
		point.y = 0;
	if(point.x > m_pDoc->m_nPageWidth + 30)
		point.x = m_pDoc->m_nPageWidth + 30;
	if(point.y > m_pDoc->m_nPageHeight)
		point.y = m_pDoc->m_nPageHeight;

	if(m_pDoc->m_nOperatorStatus == MOVE_MUTI_TRAINS)
	{
		long usLastTimeValue = m_pDoc->ConvertXPostoTime(mOrgMutiMoveLastPoint.x);
		long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
		int  nTimeOffset = usTimeValue-usLastTimeValue;
		nTimeOffset = (nTimeOffset/60)*60;
		if(abs(nTimeOffset) < 60)
			return;

		CString xml="<root>", str;
		str.Format("<offset>%d</offset>", nTimeOffset);
		xml+=str;

		for(int i = 0; i < m_selTrains.size(); i++)
		{
			str.Format("<TRAIN_INDEX>%u</TRAIN_INDEX>", m_selTrains[i].GetTrainIndex());
			xml+=str;
		}
		xml+="</root>";
		m_pDoc->m_ptgctrl->FIRE_MULTI_TRAIN_OPER(xml, 0x00000005);

		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
		m_selTrains.clear();
		
		CScrollView::OnLButtonUp(nFlags, point);
		return;
	}
	
	//����ֱ�������ѡ���������Ĺ��ܣ��ɿ������������Ի���,��ʾѡ�еĳ���
	if(m_pDoc->m_nOperatorStatus == MUTI_TRAINS_SEL_ING)
	{
		if(m_TrainsSelectedRect.TopLeft() != m_TrainsSelectedRect.BottomRight())//���������ʾ����ȷʵ���˾��Σ������û�������ɿ������Ҳ�����Ի���
		{
			if(m_TrainsSelectedRect.left > m_TrainsSelectedRect.right)
			{
				int temp = m_TrainsSelectedRect.left;
				m_TrainsSelectedRect.left = m_TrainsSelectedRect.right;
				m_TrainsSelectedRect.right = temp;
			}
			if(m_TrainsSelectedRect.top > m_TrainsSelectedRect.bottom)
			{
				int temp = m_TrainsSelectedRect.top;
				m_TrainsSelectedRect.top = m_TrainsSelectedRect.bottom;
				m_TrainsSelectedRect.bottom = temp;
			}
			DrawBox(clientDC, m_TrainsSelectedRect);
			OperateTrainsSecleted();
			
			CScrollView::OnLButtonUp(nFlags, point);
			return;
		}			
	}

	if(m_nFocusFigure != NULL)
	{
		if(m_nFocusFigure->type == GRAPH_NOTE_MARK || m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
		{
			m_nFocusFigure->bFlag = true;
		}
	}
	
	if(m_pDoc->m_nOperatorStatus == MODIFY_MARK )
	{
		if(m_nMouseCurDownPoint != point)
		{
			if(m_nFocusFigure == NULL) 
				return;

			// ͼ���ע --- ���� index line e_kilo
			if(m_nFocusFigure->type == GRAPH_NOTE_MARK)
			{
				SetGraphNoteMarkPos(m_nFocusFigure);
			}

			m_pDoc->m_myFocusBlock = *m_nFocusFigure;
			m_nFocusFigure = NULL;

			m_pDoc->SendUpdateMark(m_pDoc->m_myFocusBlock);	
		}
		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
	}

	if(m_pDoc->m_nOperatorStatus == MODIFY_TRAIN )
	{
		if(m_nMouseCurDownPoint != point)
		{
			if(m_sFocusTrain == NULL) 
				return;

			m_pDoc->m_myFocusTrain = *m_sFocusTrain;
			m_sFocusTrain = NULL;
			RefreshTrain(&m_pDoc->m_myFocusTrain);
			m_sFocusTrain = &m_pDoc->m_myFocusTrain;
			SendUpdateOtherTrain();
			m_pDoc->SendUpdateTrain(m_pDoc->m_myFocusTrain, SCHD_UPDATE, MODIFY_TRAIN_TIME, 0, m_nFocusRecordIndex);

			SendCzhOtherTrainsUpdate(m_pDoc->m_myFocusTrain.GetTrainIndex());
			m_focusCzhTrains.Init();
		}
		m_sFocusOtherTrain.clear();
		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
	}

	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN) 
	{
		if(m_sFocusTrain == NULL)	
		{
			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			return;
		}

		//����������ʷͼ��Χ����ôֱ�ӷ���
		if (!m_pctrl->IsNewMode())
		{
			if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
			{
				if(point.x <= 12*m_pDoc->m_fPixelPerHour)
					return;
			}
		}

		do 
		{
			// insert new train point
			int node_index;
			int fore_node_index = -1;

			TgTRAIN_RECORD rec;
			
			int nInsertResult = 0;

			static bool bFirstPoint = TRUE;

			TgTRAIN_RECORD *last = m_sFocusTrain->GetLastRecord();

			long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
			
			int nStationNo = m_pDoc->ConvertYPostoStationNo(point.y, node_index, last);
			
			// �ж�
			int nStationSideNo = -1;

			m_stop_train_abs = false;
			m_stop_train_abs_direct = -1;
			m_stop_train_abs_time = 0;
			if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
			{
				if(this->m_pctrl->mConfigTg.m_tgdef.IsCanDrawABSStart(m_pDoc->m_nCurrentSectionNo) || m_sFocusTrain->GetRecordNum()>0)
				{
					if(NULL != m_sFocusTrain && nStationNo==-1)
					{
						TgTRAIN_RECORD *last = m_sFocusTrain->GetLastRecord();
						if(m_pDoc->PointBetweenMidOfTwoStation(point.y, last, m_stop_train_abs_direct))
						{
							m_stop_train_abs = true;
							m_stop_train_abs_time = (usTimeValue - last->depart)/60;
							nStationNo = -1;
						}
					}
					
					std::vector<int> abs_line_list;
					if(m_pDoc->SetABSStart(m_sFocusTrain, m_strTrainId, usTimeValue, point.y, abs_line_list))
					{
						m_pDoc->DrawSingleTrain(&clientDC, m_sFocusTrain, DRAW_SCREEN);

						TgTRAIN_RECORD *rec = m_sFocusTrain->GetFirstRecord();
						if(abs_line_list.size()>1 && rec)
						{
							CDlgGiveoutDirectionSel dlg(m_pDoc, this, 2, abs_line_list, rec->station);
							dlg.entry_lines.assign(abs_line_list.begin(), abs_line_list.end());
							if(dlg.DoModal() == IDOK)
							{
								rec->entry = dlg.nSelDirect;  
							}
						}
						
						break;
					}
				}
			}

			// for train stop abs
			if(m_stop_train_abs)
			{
				int nRecNum = m_sFocusTrain->GetRecordNum();
				if(0 == nRecNum)
					break;

				TgTRAIN_RECORD *lastRec = m_sFocusTrain->GetLastRecord();
				if(NULL == lastRec)
					break;

				int prev_exit=255,curr_entry=255;
				if(point.y < m_pDoc->GetStationVPos(lastRec->out_station_index, PRIMARY_TRACK))
				{
					int idx = 0;
					for(int i=0; i<m_pDoc->m_nDispDistanceNum;i++)
					{
						if(m_pDoc->m_sDispDistance[i].nStationYPos > point.y)
						{
							break;
						}
						idx = i;
					}

					int replaceIndex = -1;
					if(!m_pDoc->JudgeNodesConnectedThroughThirdParty(lastRec->out_station_index,idx,replaceIndex))
						break;
					m_pDoc->GetExitEntryFromTwoNodeIndex(lastRec->out_station_index,idx,prev_exit,curr_entry, UP_DIRECT);

				}
				else
				{
					int idx = 0;
					for(int i=m_pDoc->m_nDispDistanceNum-1;i>=0;i--)
					{
						if(m_pDoc->m_sDispDistance[i].nSecondStationYPos < point.y)
						{
							break;
						}
						idx = i;
					}

					int replaceIndex = -1;
					if(!m_pDoc->JudgeNodesConnectedThroughThirdParty(lastRec->out_station_index,idx,replaceIndex))
						break;
					m_pDoc->GetExitEntryFromTwoNodeIndex(lastRec->out_station_index,idx,prev_exit,curr_entry, DOWN_DIRECT);
				}

				lastRec->exit = prev_exit;  


				int nOffsetTime = abs(usTimeValue - lastRec->depart);

				if(!IsFlagStopAbs(lastRec->flag))
				{
					if(point.y < m_pDoc->GetStationVPos(lastRec->out_station_index, PRIMARY_TRACK))
						SetFlagStopAbsUp(lastRec->flag);
					else
						SetFlagStopAbsDown(lastRec->flag);

					SetFlagStopAbs(lastRec->flag);
					lastRec->stop_abs_s_time = nOffsetTime/60;
					lastRec->stop_abs_e_time = lastRec->stop_abs_s_time+1;//nOffsetTime/60 + 3;
				}
				else
				{
					lastRec->stop_abs_e_time = nOffsetTime/60;
				}
			}
			else
			{
				if(nStationNo <=0)
				{
					break;
				}
				else
				{
					nStationSideNo = m_pDoc->GetStationSideNo(point.y, node_index);

					int uEntry=-1, uExit=-1;

					int nRecNum = m_sFocusTrain->GetRecordNum( );
					if(nRecNum <= 0)
					{
						m_sFocusTrain->m_nDirectFlag = m_pDoc->GetTrainDirectionByTrainId(m_strTrainId);
						int nDirectFlag = m_sFocusTrain->m_nDirectFlag;
						
						bFirstPoint = TRUE;

						m_sFocusTrain->myTrainInfo.type = this->m_pctrl->mConfigTg.GetTrainTypeByTrainNO(m_strTrainId);
						int abs_line[64];
						memset(abs_line, 0, sizeof(abs_line));

						WORD abs_line_num = m_pDoc->GetEntryFromNodeIndex(node_index, nDirectFlag, abs_line);
						
						if(abs_line_num == 0)
						{
							uEntry = -1;
						}
						else //if(abs_line_num == 1)
						{
							uEntry = m_pDoc->ConvertAbsnumStationToEntry(nStationNo, abs_line[0]);
						}

						// first record
						rec.Clear();

						rec.arrive_train_id = m_strTrainId;
						rec.depart_train_id = m_strTrainId;
						rec.original_arrive_train_id = "";
						rec.original_depart_train_id = "";
						rec.rec_index = nRecNum;
						rec.station = nStationNo;
						rec.arrive = usTimeValue;
						rec.depart = usTimeValue;
						
						rec.plan_arrive =  rec.arrive;
						rec.plan_depart = rec.depart;

						rec.entry = uEntry;
						rec.exit = uExit;
						rec.arrive_side = nStationSideNo; 	
						rec.depart_side = nStationSideNo;

						rec.adjust_status = 0;
						rec.flag = 0;
						rec.early_depart_time=0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)

						CancelFlagStopSignal(rec.flag);

						if(m_nStartMode == SCHD_START)
							SetFlagStart(rec.flag);
						else
							SetFlagAccept(rec.flag);
						//judge train direct

						if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetTechStop())
						{
							SetTechStopTrain(rec); //Ĭ�����ü���ͣ��
						}
						else
						{
							ClearTechStopTrain(rec);
						} 

						rec.in_station_index = node_index;
						rec.out_station_index = node_index;

						m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType());
						nInsertResult = 1;
					}
					else
					{					
						TgTRAIN_RECORD *lastRec = m_sFocusTrain->GetLastRecord();
						if(NULL == lastRec)
							break;

						// ��ͨ���ж�
						int last_node_index = lastRec->out_station_index;
						if(last_node_index == -1) 
							break;

						int replaceIndex = -1;
						if(!m_pDoc->JudgeNodesConnectedThroughThirdParty(node_index,last_node_index,replaceIndex))
							break;

						// ���ó�վ����վ��
						lastRec->out_station_index = replaceIndex;


						fore_node_index = lastRec->out_station_index;
						
						if(fore_node_index == node_index)
						{
							uEntry = lastRec->exit;
						}
						//��ǰѡ��վ����ǰ���һ��վ֮ǰ�����г�
						else 
						{
							int prev_exit=255,curr_entry=255;
							int nFlag = m_pDoc->GenExitEntryByInitABS(m_pDoc->m_nCurrentSectionNo,
															lastRec->station, nStationNo, 
															fore_node_index, node_index, 
															prev_exit, curr_entry);

							if(nFlag != 2)
							{
								bool bSucc=false;
								int nAbsLineNo=m_pDoc->GetLineNoByStations(lastRec->station,nStationNo);
								if(nAbsLineNo>0)
								{
									int exit  = m_pDoc->ConvertAbsnumStationToEntry(lastRec->station, nAbsLineNo);
									int entry = m_pDoc->ConvertAbsnumStationToEntry(nStationNo, nAbsLineNo);
									if(exit>0 && entry>0)
									{
										lastRec->exit=exit;
										uEntry=entry;
										bSucc=true;
										m_pDoc->sysprint(0, 0, "��㻭��,���� tg.def��[STATION_NEXTSTATION_LINENO]����:%d,%d,%d,���ó�վ %d ���� %d.",
											lastRec->station, nStationNo, nAbsLineNo, lastRec->station, lastRec->exit);
									}
								}
								if(!bSucc)
								{
									if(fore_node_index > node_index)
									{
										m_pDoc->GetExitEntryFromTwoNodeIndex(fore_node_index,node_index,prev_exit,curr_entry, UP_DIRECT);

										lastRec->exit = prev_exit;
										uEntry = curr_entry;						
									}
									else
									{
										m_pDoc->GetExitEntryFromTwoNodeIndex(fore_node_index,node_index,prev_exit,curr_entry, DOWN_DIRECT);

										lastRec->exit = prev_exit;
										uEntry = curr_entry;
									}
								}
							}
							else
							{
								lastRec->exit = prev_exit;
								uEntry = curr_entry;
							}
						}


						bFirstPoint = FALSE;

						if((nStationNo == lastRec->station )&&!(IsFlagStopAbs(lastRec->flag) ||	IsFlagStopSignal(lastRec->flag)))
						{
							if(nStationSideNo >= 0)
							{
								lastRec->arrive_side =  nStationSideNo;
								lastRec->depart_side =  nStationSideNo;
							}
							if(nRecNum == 1) 
							{
								if(usTimeValue <= lastRec->arrive)
								{
									if(fore_node_index == node_index)
										lastRec->arrive = usTimeValue;
									else
										lastRec->depart = lastRec->arrive;//yuzhu 
									lastRec->exit = uExit;
								}
								else if(usTimeValue >= lastRec->depart)
								{
									lastRec->depart = usTimeValue;
									lastRec->exit = uExit;
								}
								else 
								{
									if(usTimeValue <= (lastRec->arrive + lastRec->depart)/2)
									{
										lastRec->arrive = usTimeValue;
										lastRec->exit = uExit;
									}
									else 
									{
										lastRec->depart = usTimeValue;
										lastRec->exit = uExit;
									}
								}

								if(IsFlagStart(lastRec->flag))
								{
									lastRec->arrive = usTimeValue;
									lastRec->depart = usTimeValue;
								}

								nInsertResult = 2;
							}
							else
							{
								nInsertResult = 4;
								TgTRAIN_RECORD *prev = m_sFocusTrain->GetRecordByPosition(nRecNum - 2);
								if(NULL == prev)
									break;

								if(usTimeValue <= lastRec->arrive && usTimeValue > prev->depart)
								{
									if(fore_node_index == node_index)
										lastRec->arrive = usTimeValue;
									else
										lastRec->depart = lastRec->arrive;//  yuzhu
									lastRec->exit = uExit;
								}
								else if(usTimeValue >= lastRec->depart)
								{
									lastRec->depart = usTimeValue;
									lastRec->exit = uExit;
								}
								else if(usTimeValue > lastRec->arrive && usTimeValue < lastRec->depart) 
								{
									if(usTimeValue <= (lastRec->arrive + lastRec->depart)/2)
									{
										lastRec->arrive = usTimeValue;
										lastRec->exit = uExit;
									}
									else 
									{
										lastRec->depart = usTimeValue;
										lastRec->exit = uExit;
									}
								} 
								else 
								{
									nInsertResult = 0;
								}
							} 
						} 
						else 
						{
							// ������һ��������
							if(nRecNum == 1 && !IsFlagAbsStart(lastRec->flag))
							{
								int abs_line[64];
								WORD abs_line_num = 0;
								memset(abs_line, 0, sizeof(abs_line));
								//  ���г�
								if(fore_node_index > node_index)
									abs_line_num = m_pDoc->GetEntryFromNodeIndex(fore_node_index, UP_DIRECT, abs_line);
								// ���г�
								else if(fore_node_index < node_index)
									abs_line_num = m_pDoc->GetEntryFromNodeIndex(fore_node_index, DOWN_DIRECT, abs_line);
								else
									;

								if(abs_line_num == 0)
									lastRec->entry = -1;
								else //if(abs_line_num == 1)
									lastRec->entry = m_pDoc->ConvertAbsnumStationToEntry(lastRec->station, abs_line[0]);

							}

							rec.Clear();
							
							rec.arrive_train_id = lastRec->depart_train_id;//m_strTrainId;
							rec.depart_train_id = lastRec->depart_train_id;//m_strTrainId;
							rec.original_arrive_train_id = lastRec->original_depart_train_id;
							rec.original_depart_train_id = lastRec->original_depart_train_id;
							
							rec.rec_index = nRecNum;
							rec.station = nStationNo;
							rec.arrive = usTimeValue;
							rec.depart= usTimeValue;
							

							rec.plan_arrive =  rec.arrive;
							rec.plan_depart = rec.depart;
							

							rec.entry = uEntry;
							rec.exit = uExit;
							rec.adjust_status = 0;
							rec.flag = 0;
							rec.early_depart_time=0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)
							CancelFlagStopSignal(rec.flag);
							if(lastRec->station == nStationNo)
								SetFlagAnLocReturn(rec.flag);

							rec.arrive_side = nStationSideNo;
							rec.depart_side = nStationSideNo;  

							rec.in_station_index = node_index;
							rec.out_station_index = node_index;
							if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetTechStop())
							{
								SetTechStopTrain(rec); //Ĭ�����ü���ͣ��
							}
							else
							{
								ClearTechStopTrain(rec);
							}

							int recexit, recentry;
							if(SetEntryDefine(nStationNo, m_pDoc->m_nCurrentSectionNo, m_sFocusTrain, recentry, recexit))
							{
								if(recentry>0)
									rec.entry = recentry;
								if(recexit>0)
									rec.exit = recexit;
							}

							if(m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType()) < 0)
							{
								CString str;
								str.Format("�г� %s �ӳ�վ %s ����վ %s ���ʱ�䲻��",
									m_pDoc->GetTrainShowId(m_sFocusTrain),
									m_pDoc->GetStationName(lastRec->station),
									m_pDoc->GetStationName(rec.station));

								MessageBox(str, "��ʾ");
								
								nInsertResult = 0;
							}
							else 
							{
								nInsertResult = 6;
							}
						}
					} 

					m_pDoc->sysprint(5,0,"[TG]: nInsertResult = %d",nInsertResult);
				}
			}

			
			m_pDoc->SetTrainSideAndSchdReal(*m_sFocusTrain);
			m_pDoc->SetTrainWidthColor(m_sFocusTrain);
			m_pDoc->SetDirectAtStation(m_sFocusTrain);
			m_pDoc->DrawSingleTrain(&clientDC,m_sFocusTrain, DRAW_SCREEN);
			if(m_sFocusTrain->GetRecordNum () >0)
				m_pOldMousePoint = m_sFocusTrain->GetContinuePoint ();

			Invalidate(FALSE);

		}while(0);
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

// ���⾶·����ڶ���
bool CTGForm::SetEntryDefine(const int nStation, const int nSection, CTG_TRAIN* train, int& nEntry, int& nExit , int RecIndex)
{
	const SectionEntryDefine* sectionDefine = this->m_pctrl->mConfigTg.m_tgdef.GetSectionEntryDefine(nSection);

	if(NULL != sectionDefine)
	{
		int nNum = sectionDefine->defineNum;
		for(int idx = 0; idx < nNum; idx++)
		{
			SingleEntryDefine define = sectionDefine->define[idx];
			if(define.currentSta.station != nStation)
				continue;

			int nRecNum = train->GetRecordNum();
			if (RecIndex!=0)
				nRecNum = RecIndex;

			if(define.beforeNum > nRecNum)
				continue;

			int i = 0;
			for(; i<define.beforeNum; i++)
			{
				TgTRAIN_RECORD* rec = train->GetRecordByPosition(nRecNum-1-i);
				if(NULL == rec)
					break;
				if(rec->station != define.before[i].station)
					break;
			}

			if(i >= define.beforeNum)
			{
				i = 0;
				for(; i<define.beforeNum; i++)
				{
					TgTRAIN_RECORD* rec = train->GetRecordByPosition(nRecNum-1-i);
					if(NULL == rec)
						break;

					if(define.before[i].entry > 0)
						rec->entry = define.before[i].entry;
					if(define.before[i].exit > 0)
						rec->exit = define.before[i].exit;
				}
				nEntry = define.currentSta.entry;
				nExit = define.currentSta.exit;
				return true;
			}
		}
	}

	return false;
}

void CTGForm::SetGraphNoteMarkPos(CTg_BLOCK* block)
{
	if(block == NULL)
		return;

	if(block->station >= m_pDoc->m_sDispDistance[m_pDoc->m_nDispDistanceNum-1].nSecondStationYPos)
	{
		block->index  = m_pDoc->m_nDispDistanceNum-1;
		block->e_kilo = 2;
		block->line   = block->station - m_pDoc->m_sDispDistance[m_pDoc->m_nDispDistanceNum-1].nSecondStationYPos;
	}
	else if(block->position <= m_pDoc->m_sDispDistance[0].nStationYPos)
	{
		block->index  = 0;
		block->e_kilo = 1;
		block->line   = m_pDoc->m_sDispDistance[0].nStationYPos - block->position;
	}
	else
	{
		int mid_pos = (block->position + block->station) / 2;
		long index = 0, dlt = 9999999;
		for(int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
		{
			int tmp = abs(mid_pos - m_pDoc->m_sDispDistance[i].nStationYPos);
			if(tmp < dlt)
			{
				dlt = tmp;
				index = i;
			}
		}

		block->index  = index;
		if(mid_pos > m_pDoc->m_sDispDistance[index].nStationYPos)
		{
			block->e_kilo = 2;
			block->line   = mid_pos - m_pDoc->m_sDispDistance[index].nStationYPos;
		}
		else
		{
			block->e_kilo = 1;
			block->line   = m_pDoc->m_sDispDistance[index].nStationYPos - mid_pos;
		}
	}
}

void CTGForm::MouseRecievePoint(CPoint point)
{
	if(m_sFocusTrain == NULL)
		return;

	if(m_pDoc->m_nOperatorStatus != RECEIVE_POINT_STATUS)
		return;

	for (int i=0; i<m_sFocusTrain->m_nPointCount; i++)
	{
		CPoint pt = m_sFocusTrain->m_pPerPointInfo[i].point;
		CRect rect(CPoint(pt.x - 5,pt.y - 5), CSize(10,10));

		if(!rect.PtInRect(point))
			continue;

		m_nMouseDownPosition = m_sFocusTrain->m_pPerPointInfo[i].start_depart_flag;
	
		int sel_index = m_sFocusTrain->m_pPerPointInfo[i].record_index;
		const TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByPosition (sel_index);
		if(rec == NULL) 
			break;
		
		SetReceivePointState(*m_sFocusTrain, sel_index);
		return;
	}

	return;
}

bool CTGForm::IsSelFocusTrainByPoint(CPoint point)
{
	if(m_sFocusTrain == NULL)
		return false;
	
	m_nFocusRectIndex = -1;
	m_nFocusRecordIndex = -1;
	for (int i=0; i<m_sFocusTrain->m_nPointCount; i++)
	{
		CPoint pt = m_sFocusTrain->m_pPerPointInfo[i].point;
		CRect rect(CPoint(pt.x - 5,pt.y - 5), CSize(10,10));

		if(!rect.PtInRect(point))
			continue;

		m_nFocusRectIndex = i;
		m_nMouseDownPosition = m_sFocusTrain->m_pPerPointInfo[i].start_depart_flag;
		m_nFocusRecordIndex = m_sFocusTrain->m_pPerPointInfo[i].record_index;

		int sel_index = m_sFocusTrain->m_pPerPointInfo[i].record_index;
		const TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByPosition (sel_index);
		if(rec == NULL) 
			break;
		if(m_nMouseDownPosition == DEPART_POSITION)
		{
			if(rec->adjust_status & 0x0002)
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
			else
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;

			if(IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;

		}
		else if(m_nMouseDownPosition == ARRIVAL_POSITION)
		{
			if(rec->adjust_status & 0x0001)
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
			else
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;

			if(IsArriveRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}
		else 
		{
			if(rec->adjust_status & 0x0001 || rec->adjust_status & 0x0002)
				m_pDoc->focus_point = m_pDoc->POINT_REAL;
			else
				m_pDoc->focus_point = m_pDoc->POINT_PLAN;

			if(IsArriveRouteOk(*rec)	|| IsDepartRouteOk(*rec))
				m_pDoc->focus_point = m_pDoc->POINT_ROUTE_OK;
		}

		return true;
	}

	return false;
}

void CTGForm::OnRButtonDown(UINT nFlags, CPoint point) 
{

	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);
	clientDC.DPtoLP(&point);
	
	CRect rect;
	if(m_sFocusTrain != NULL) 
	{
		return;
	}

	CScrollView::OnRButtonDown(nFlags, point);
}

void CTGForm::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	ASSERT(m_pDoc != NULL);

	m_pCfWnd.Create(this, 180, 18);

	SIZE mSize;
	mSize.cx = m_pDoc->m_nPageWidth;
	mSize.cy = m_pDoc->m_nPageHeight;

	ShowNoearlylatetime();
	SetScrollSizes(MM_TEXT,mSize);

	timeb cur;
	ftime(&cur);
	m_timePress = ((short)cur.time)*1000 + cur.millitm;

	do 
	{
		CClientDC dc(this);
		OnPrepareDC(&dc);

		m_sFocusTrain = NULL;

		
		CRect rectWindow;
		GetWindowRect(&rectWindow);

		BITMAPINFOHEADER bmih;
		memset(&bmih,0,sizeof(BITMAPINFOHEADER));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		DEVMODE md;
		md.dmSize = sizeof(md);
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &md))
			bmih.biBitCount = md.dmBitsPerPel;
		else
			bmih.biBitCount =16;
		bmih.biCompression=BI_RGB;
		bmih.biPlanes=1;
		bmih.biWidth=m_pDoc->m_nPageWidth;
		bmih.biHeight=m_pDoc->m_nPageHeight;
		BITMAPINFO bmi;
		memset(&bmi,0,sizeof(BITMAPINFO));
		bmi.bmiHeader = bmih;
		void *p;
		HBITMAP bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
		if(!bitMap)
		{
			m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
			AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
			break;
		}
		m_bitmap1.Attach(bitMap);
		if(m_Layer1.CreateCompatibleDC(&dc) == 0)
		{
			m_bitmap1.DeleteObject();
			AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
			m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
			break;
		}
		m_pBitmapLayer1 = m_Layer1.SelectObject(&m_bitmap1);
		m_Layer1.SetBkColor(CLR_WHITE);

		if(m_pctrl->IsWorkMode())
		{
			bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
			if(!bitMap)
			{
				m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
				AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
				break;
			}
			m_bitmap2.Attach(bitMap);
			if(m_Layer2.CreateCompatibleDC(&dc) == 0)
			{
				m_bitmap2.DeleteObject();
				AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
				m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
				break;
			}

			bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
			if(!bitMap)
			{
				m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
				AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
				break;
			}
			m_bitmap3.Attach(bitMap);
			if(m_Layer3.CreateCompatibleDC(&dc) == 0)
			{
				m_bitmap3.DeleteObject();
				AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
				m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
				break;
			}

			bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
			if(!bitMap)
			{
				m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
				AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
				break;
			}
			m_bitmap4.Attach(bitMap);
			if(m_Layer4.CreateCompatibleDC(&dc) == 0)
			{
				m_bitmap4.DeleteObject();
				AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
				m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
				break;
			}
			
			m_pBitmapLayer2 = m_Layer2.SelectObject(&m_bitmap2);
			m_Layer2.SetBkColor(CLR_WHITE);
			m_pBitmapLayer3 = m_Layer3.SelectObject(&m_bitmap3);
			m_Layer3.SetBkColor(CLR_WHITE);
			m_pBitmapLayer4 = m_Layer4.SelectObject(&m_bitmap4);
			m_Layer4.SetBkColor(CLR_WHITE);

			m_pDoc->SetLayer(&m_Layer4, &m_Layer2, &m_Layer3);
			m_pDoc->sysprint(0,0,"[TG]: using work mode");
			TRACE("\n using work mode");
		}
		else
		{
			m_pDoc->sysprint(0,0,"[TG]: not using work mode");
			TRACE("\n not using work mode");
			m_pDoc->SetLayer(&m_Layer1, NULL, NULL);
		}
		m_pDoc->CreateGDIInfo(&dc);
		this->Redraw();
	}while(0);

	this->m_bRescueTrainStatus= false;
	this->m_bUniteTrainStatus = false;

	m_pDoc->m_nVerticalPos=0;
	m_pDoc->m_nHorizontalPos=0;
	
	SetScrollPos(SB_VERT,m_pDoc->m_nVerticalPos);
	SetScrollPos(SB_HORZ,m_pDoc->m_nHorizontalPos);
	ScrollWindow(0,0,NULL,NULL);

	if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
	{
		m_pDoc->SetSchdBeginTime();
		CTime tm=CTime::GetCurrentTime();	
		m_lastUpdateMinute=tm.GetMinute();
		m_lastUpdateHour=tm.GetHour();
		SetTimer(TIMER_IDEVENT_DRAWBAR,1000, 0);
	}

	int nTimeAdd=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetAutoAddBlockEndTime();
	if(nTimeAdd>0)
		SetTimer(TIMER_CHECK,nTimeAdd*60*1000, 0);
}

void CTGForm::SetRightScrollPosition()
{
	int nMin, nMax;
	GetScrollRange(SB_HORZ, &nMin, &nMax);

	m_pDoc->m_nVerticalPos = 0;
	SetScrollPos(SB_VERT,m_pDoc->m_nVerticalPos);
	
	if(m_pctrl->IsNewMode())
	{
		m_pDoc->m_nHorizontalPos = (nMax*5)/12;
	}
	else
	{
		m_pDoc->m_nHorizontalPos = 0;
	}
	SetScrollPos(SB_HORZ, m_pDoc->m_nHorizontalPos);
}

void CTGForm::SetScrollPosition(long pos, ULONG train_index)
{
	pos = pos - 30;
	if(pos < 0)
		pos = 0;
	SetScrollPos(SB_HORZ,pos);
	int nHorzPos = this->GetScrollPos(SB_HORZ);
	if(m_pDoc->m_nHorizontalPos != nHorzPos)
	{
		m_pDoc->m_nHorizontalPos = nHorzPos;
		m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL); // �����г���������
	}
	if(train_index > 0)
	{
		if(m_sFocusTrain != NULL)
		{
			m_nFocusTrainIndex=0;
			m_nFocusRectIndex=-1;
			m_nFocusRecordIndex=-1;
			RefreshTrain(m_sFocusTrain);
			m_sFocusTrain = NULL;
		}

		m_nFocusTrainIndex = train_index;
		CTG_TRAIN* tmp = myPDataManage->GetTrain(m_nFocusTrainIndex);
		if(tmp)
		{
			m_pDoc->m_myFocusTrain = *tmp;
			m_sFocusTrain = &m_pDoc->m_myFocusTrain;

			RefreshTrain(&m_pDoc->m_myFocusTrain);
		}
	}
	CRect regine(0, 0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight);
	this->RefreshRect(regine); // �����г������⣬����
}

void CTGForm::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(lHint == MSG_DOUBLE_CLICK_STATION)
	{
		this->Redraw();
	}
	else if(lHint == MSG_MOUSE_MOVE_OUTSIDE)
	{
		CPoint point(20,50);
		ShowTipMsg(point, ""); 
		m_pctrl->FIRE_SHOW_CREW(0, 0, 0); // �Ƴ�view������ʾ����
	}
	else if(lHint == MSG_RE_LAYOUT_BMP_SIZE)
	{
		SIZE mSize;
		mSize.cx = m_pDoc->m_nPageWidth;
		mSize.cy = m_pDoc->m_nPageHeight;
		SetScrollSizes(MM_TEXT,mSize);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		ReLayoutBmpPageSize(dc);
		Redraw();
	}
}

void CTGForm::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);

	int nVertPos = this->GetScrollPos(SB_VERT);
	if(m_pDoc->m_nVerticalPos != nVertPos)
	{
		m_pDoc->m_nVerticalPos = nVertPos;
		m_pDoc->UpdateAllViews(this,VERTICAL_POSITION_CHANGE_WORK,NULL); // �϶�������
	}
}

void CTGForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

	int nHorzPos = this->GetScrollPos(SB_HORZ);
	if(m_pDoc->m_nHorizontalPos != nHorzPos)
	{
		m_pDoc->m_nHorizontalPos = nHorzPos;
		m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL); // �϶�������
	}
}

BOOL CTGForm::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BOOL bHasHorzBar, bHasVertBar;
	CheckScrollBars(bHasHorzBar, bHasVertBar);
	if (!bHasVertBar && !bHasHorzBar)
		return TRUE;

	BOOL ret = DoMouseWheel(nFlags, zDelta, pt);
	if(bHasVertBar)
	{
		int nPos = this->GetScrollPos(SB_VERT);
		if(m_pDoc->m_nVerticalPos != nPos)
		{
			m_pDoc->m_nVerticalPos = nPos;
			m_pDoc->UpdateAllViews(this,VERTICAL_POSITION_CHANGE_WORK,NULL);
		}
	}
	else if(bHasHorzBar)
	{
		int nPos = this->GetScrollPos(SB_HORZ);
		if(m_pDoc->m_nHorizontalPos != nPos)
		{
			m_pDoc->m_nHorizontalPos = nPos;
			m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL);
		}
	}
	return ret;
}

BOOL  CTGForm::AddNewTrainGraphCommon()
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		AfxMessageBox("��ע��,��û�л��������ߵ�Ȩ��");
		return FALSE;
	}

	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
	{
		if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN)
		{
			if(MessageBox("�㵱ǰ���ڻ���״̬,ȷʵҪ���»�����", "��ʾ", MB_ICONQUESTION|MB_YESNO) == IDYES)
			{
				m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			}
			else
			{
				return FALSE;
			}
		}
		else if(m_pDoc->m_nOperatorStatus == DRAW_MARK_ITEM_STATUS)
		{
			if(MessageBox("�㵱ǰ���ڻ����״̬,ȷʵҪ������", "��ʾ", MB_ICONQUESTION|MB_YESNO) == IDYES)
			{
				m_bFirstPoint = false;
				m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			}
			else
			{
				return FALSE;
			}
		}
		else if(m_pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS)
		{
			if(MessageBox("�㵱ǰ��������յ�״̬,ȷʵҪ������", "��ʾ", MB_ICONQUESTION|MB_YESNO) == IDYES)
			{
				m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			}
			else
			{
				return FALSE;
			}
		}
		else
			return FALSE;
	}

	if(m_sFocusTrain != NULL)
	{
		CTG_TRAIN* tmp = myPDataManage->GetTrain(m_sFocusTrain->GetTrainIndex());
		if(tmp)
		{
			m_nFocusRectIndex = -1;
			m_nFocusRecordIndex = -1;
			
			RefreshTrain(m_sFocusTrain);
		}

		m_sFocusTrain = NULL;
	}

	return TRUE;
}

LRESULT CTGForm::OnAddNewTrainGraphXml(WPARAM wParam,LPARAM lParam) 
{
	LPCTSTR xml = (LPCTSTR)lParam;
	
	if(!AddNewTrainGraphCommon())
		return TRUE;

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( xml )))
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "NEWTRAIN");
	if(pRoot == NULL)
		return false;
	
	MSXML::IXMLDOMNodePtr pNode, pChildNode;

	pNode = pRoot->selectSingleNode("TRAINID");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_strTrainId = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode("ARLTRAINID");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_strArrivalTrainId = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode("START");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_nStartMode = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode("END");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_nEndMode = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	m_nSecretTrain=0;
	pNode = pRoot->selectSingleNode("SECRET");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_nSecretTrain = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	m_nRunType=255;
	pNode = pRoot->selectSingleNode("RUN_TYPE");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_nRunType = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	int nTrainType=255;
	pNode = pRoot->selectSingleNode("TRAIN_TYPE");
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		nTrainType = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	m_pDoc->m_aryEngine.RemoveAll();
	m_pDoc->m_aryStatus.RemoveAll();
	MSXML::IXMLDOMNodeListPtr pListEngine = pRoot->selectNodes( "ENGINE/ID");
	if(NULL != pListEngine)
	{
		for(int idx = 0; idx < pListEngine->Getlength(); idx++)
		{
			pNode = pListEngine->Getitem(idx);
			if(NULL == pNode)
				continue;
			
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				m_pDoc->m_aryEngine.Add(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode("@status");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				m_pDoc->m_aryStatus.Add(atol(CString(v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}

	if(m_strTrainId.GetLength() >= 20 || m_strArrivalTrainId.GetLength() >= 20)
	{
		MessageBox("���κŲ��ܳ���19λ","����", MB_ICONQUESTION|MB_OK);
		return TRUE;
	}
	m_sFocusTrain = new CTG_TRAIN();
	if(m_sFocusTrain != NULL)
	{
		m_sFocusTrain->myTrainInfo.type = nTrainType;
		if(m_nSecretTrain)
			SetTrainSecret(m_sFocusTrain->myTrainInfo);
	} 
	else 
	{
		m_pDoc->sysprint(5,0,"[TG] memory allocate failed");
		return TRUE;
	}

    m_pDoc->sysprint(4,0,"[TG] ����Ա�ֹ����ɻ���xmlģʽ:%s, %s", m_strTrainId, m_strArrivalTrainId);
	m_pDoc->sysprint(4,0,"[TG] ����Ա�ֹ����ɻ���xmlģʽ:%d, %d, ���� %d", m_nStartMode, m_nEndMode, m_nSecretTrain);
	
	SetTrainShow(m_sFocusTrain);
	m_pDoc->m_nOperatorStatus = INSERT_TRAIN;
	
	m_pDoc->ResetContinueDraw();

	m_pContinuePoint = m_pOldMousePoint = CPoint(-1,-1);

	return TRUE;
}

LRESULT CTGForm::OnAddNewTrainGraph(WPARAM wParam,LPARAM lParam) 
{
	DrawTrainInfo* pinfo = (DrawTrainInfo*)lParam;
	if(pinfo == NULL)  return TRUE;

	if(!AddNewTrainGraphCommon())
		return TRUE;

	if(pinfo->strTrainId.GetLength() >= 20 || pinfo->strArrivalTrainId.GetLength() >= 20)
	{
		MessageBox("���κŲ��ܳ���19λ","����", MB_ICONQUESTION|MB_OK);
		return TRUE;
	}
	m_sFocusTrain = new CTG_TRAIN();
	if(m_sFocusTrain != NULL)
	{
		m_sFocusTrain->myTrainInfo.type = 255;
	} 
	else 
	{
		m_pDoc->sysprint(5,0,"[TG] memory allocate failed");
		return TRUE;
	}

	m_pDoc->m_aryEngine.RemoveAll();
	m_pDoc->m_aryStatus.RemoveAll();
	m_nStartMode = pinfo->nStartMode;
	m_nEndMode = pinfo->nEndMode;
	m_nSecretTrain=pinfo->nSecretTrain;
	m_strTrainId = pinfo->strTrainId;
	m_strArrivalTrainId = pinfo->strArrivalTrainId;
	SetTrainShow(m_sFocusTrain);
	m_pDoc->m_nOperatorStatus = INSERT_TRAIN;
	
	if(m_nSecretTrain)
		SetTrainSecret(m_sFocusTrain->myTrainInfo);

	m_pDoc->sysprint(4,0,"[TG] ����Ա�ֹ����ɻ���:%s %s ���� %d", pinfo->strTrainId, pinfo->strArrivalTrainId, m_nSecretTrain);
	m_pDoc->ResetContinueDraw();

	m_pContinuePoint = m_pOldMousePoint = CPoint(-1,-1);

	return TRUE;
}

void CTGForm::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(m_pDoc->m_ptgctrl->IsFormatView())
	{
		OnContextMenuFormat(pWnd, point);
		return;
	}

	CMenu hResourceMenu;
	CRect r;

	do 
	{
		if(m_pDoc == NULL)
			break;

		GetWindowRect(&r);

		if(point.x == -1 && point.y == -1)
		{
			point.x = r.left / 3 + r.right * 2 / 3;
			point.y = r.top / 3 + r.bottom * 2 / 3;
		}

		if(! r.PtInRect(point))
			break;

		if(hResourceMenu.LoadMenu(IDR_RBUTTON_POPMENU) == 0)
			break;

		// TG menu
		if(m_sFocusTrain != NULL)
		{
			//ȡ���ֹ����������ߵ��Ҽ��˵�
			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN) 
			{  		           
				CPoint pt = CPoint(point);
				ScreenToClient(&pt);

				CClientDC clientDC(this);
				OnPrepareDC(&clientDC);
				clientDC.DPtoLP(&pt);
				if(m_sFocusTrain != NULL)
				{
					if(m_sFocusTrain->myRecord.size()>0)
					{ 	    
						hPopupMenu =  hResourceMenu.GetSubMenu(0);
						if(hPopupMenu == NULL)	
							break;

						hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

						if(m_stop_train_abs)
						{
							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_ABS_STOP, "�����յ�");
						}
						else
						{
							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_STOP, "�յ�");

							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_DELIVER, "����");
						}

						// ȡ������
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_CANCEL, "ȡ������");

						if(m_sFocusTrain->GetRecordNum() >= 2)
						{
							hPopupMenu->AppendMenu(MF_SEPARATOR);
						
							if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							{
								CMenu pTmpMenu;
								VERIFY(pTmpMenu.CreatePopupMenu());
								hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ʩ����");

								// ������ҵ��ǰ�У�
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN, "������ҵ��ǰ�У�");
								// ������ҵ���۷���
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+1, "������ҵ���۷���");
								// ������ҵ�������䣩
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+2, "������ҵ�������䣩");
								// ������ҵ�������俪�أ�
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+3, "������ҵ�������俪�أ�");
							}
						}

					}
				}
			}
			else
			{
				TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
				if (rec == NULL) 
					return;
				
				bool bNotThisSectionStation=this->m_pctrl->mConfigTg.IsNotThisSectionStation(rec->station);
				const TgTRAIN_MORE_INFO& train_info = m_sFocusTrain->myTrainInfo;
				hPopupMenu =  hResourceMenu.GetSubMenu(0);
				if(hPopupMenu == NULL)	
					break;
				// ɾ��ռλ�˵�
				hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableInterCityStplan()) // �����ǳǼ�,�����۷��˵�
				{
					// �յ��۷�
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�۷�����");
						
						// ԭ���۷�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_IN_STATION_RETURN, "�յ�-ԭ���۷�");
						// վ���۷�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_OUT_STATION_RETURN, "�յ�-վ���۷�");
						// ���۷�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_NOT_STATION_RETURN, "�յ�-���۷�");
						// ���۷�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_NOT_START_RETURN, "ʼ��-���۷�");
			
						if(IsNewReturn(train_info.side_utility))
						{
							if(IsStationOutReturn(train_info.side_utility))
							{
								pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
							}
							else
							{
								pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
							}
						}
						else
						{
							pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
						}
						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}

				// �Զ�����
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableAutoAdjust())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�Զ�����");
						
						// ��ͼ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN, "��ͼ����");
						// ���õ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN+1, "���õ���");
						// �����õ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN+2, "��㲻���õ���");

						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}


				// ��Ǯ��
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanSetFjkLateTime())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) || m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_FENJIEKOU_TIME, "��Ǯ��");
					}
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}

				// �����Դ���־
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanSetTrigeRouteFlag())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) || m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ROUTE_AUTO_MARK, "�����Դ���־");
					}
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}
				// �����·
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyRoute() && (m_pDoc->GetTgType() != TG_TYPE::BASIC_TG))
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_SINGLE, "�����·");
					}
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}
				
				// �������
				if(this->m_pctrl->mConfigTg.m_context.IsEnableCZH())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�������");

					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_CZH_TRAIN, "�г�����Ԥ��");
					
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						// ���˳�����༭
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_CZH, "����༭");
						
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						// �������������۷�����
						if(m_pctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())					
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_GENTRAINID_BY_CZH, "�����۷�����");
						else
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED, ID_GENTRAINID_BY_CZH, "�����۷�����");
					}
				}
				else if(this->m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "��·����");

					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_CZH_TRAIN, "�г���·Ԥ��");
					
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						// �����г�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_CZH, "�����г�");
						// �������
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_JIECHU_TRAIN_CZH, "�������");
					}
				}
				// �޸ļƻ��߲���
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_PLAN_PARA, "�ƻ��߲���");
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsFlashBasicTrain())
				{
					if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_BASIC_INFO, "����ͼ��Ϣ");
					}
				}
				if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
				{
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanDirectSetRouteTriger())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						{
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DIRECT_SET_TRIG, "���ý�·�Դ�");
						}
					}
				}
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				// ����ͼ����				
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				{
					TgTRAIN_RECORD *rec;
					int nFocusIndex = m_nFocusRecordIndex;
					rec = m_sFocusTrain->GetRecordByRecIndex(nFocusIndex);
					if(rec == NULL)
						return;

					if(m_pDoc->m_ptgctrl->IsBPlanStation(rec->station))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ͼ����");

						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_KAKONG_BASIC_SIDE, "ȡ������ͼ�ɵ�����");
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_KAKONG_BASIC_ENTRY, "ȡ������ͼ����ڿ���");

						if(TG_IsNotCheckTrainSide(*rec))
							pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);

						if(TG_IsNotCheckTrainExitEntry(*rec))
							pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
				} 

				// ����/ʼ��
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyTrainInOutMark())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����/ʼ��");
						
						// ��վ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_FETCH, "����");
						// ��վʼ��
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_START, "ʼ��");
					}

					
					// ID_STOP_DWAW
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						CMenu pTmpMenu1;
						VERIFY(pTmpMenu1.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu1.m_hMenu, "����/�յ�");

						// ��̽���
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DEPART, "����");
						// ���ͣ��
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DWAW, "�յ�");
						// ��ʱͣ��
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_TEMP, "��ʱͣ��");

						if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
						{
							// �м�ָ�
							pTmpMenu1.AppendMenu(MF_SEPARATOR);
							pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_DIV_TRAIN, "�м�ָ�");
						}

						// ��������
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_CONTINUE_DRAW, "��������");
					}
				}

				
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanPASSToStayCnvt())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !bNotThisSectionStation)
					{
						// ͨ���䵽��
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_PASS_TO_STAY, "ͨ���䵽��");
						// ������ͨ��
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_STAY_TO_PASS, "������ͨ��");
					}
				}
				
				// ��ѯȷ��
				if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_QUERT_QUEBAO, "��ѯȷ��");
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_WORK_INFO, "��ҵ��Ϣ");
					hPopupMenu->AppendMenu(MF_SEPARATOR);

					// ������Ϣ
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAINSPORT_INFO, "������Ϣ");
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_SUBAO_INFO, "�г��ٱ�");
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_BIANZU_INFO, "�г�����");
				}

				if(m_pDoc->m_bShowEngineCrewMenu)
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_ENGINE_INFO, "������Ϣ��ѯ");
				}

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				// ˦�ҹ���
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableShuaiguaManage())
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SHUAIGUA_MANAGE, "˦�ҹ���");
				
				// �г�����
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCannotMeetTrain())
				{
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_CANNOT_MEET, "�г�����");
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}

				// �����ͳ��
				if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
				{
					if(m_sFocusTrain == NULL) 
						return;

					TgTRAIN_RECORD *rec;
					int nFocusIndex = m_nFocusRecordIndex;
					rec = m_sFocusTrain->GetRecordByRecIndex(nFocusIndex);
					if(rec == NULL)
						return;

					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�����ͳ��");

					// ͳ�Ƶ����
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_ARRIVAL, "ͳ�Ƶ����");
					// ͳ�Ƴ�����
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_DEPART, "ͳ�Ƴ�����");
					// ͳ�Ƶ���/������
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_BOTH, "ͳ�Ƶ�����");
					// ��ͳ�Ƶ���/������
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_NO_STATISTIC, "��ͳ�Ƶ�����");

					pTmpMenu.AppendMenu(MF_SEPARATOR);
					// ����ͳ��
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_WHOLE, "����ͳ��");	

					if(m_sFocusTrain->myTrainInfo.whole_static>0)
						pTmpMenu.CheckMenuItem(5, MF_CHECKED|MF_BYPOSITION);


					if((rec->static_arrive > 0) && (rec->static_depart > 0))
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					else
					{
						if(rec->static_arrive>0)
							pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
						if(rec->static_depart>0)
							pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
						if(rec->static_arrive == 0 && rec->static_depart == 0)
							pTmpMenu.CheckMenuItem(3, MF_CHECKED|MF_BYPOSITION);
					}
				
					// ����/����ͣ��
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanStopTrainAbs())
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());

						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����/����ͣ��");

						int nFocusIndex = m_nFocusRecordIndex;
						TgTRAIN_RECORD *sel_next_rec = m_sFocusTrain->GetRecordByRecIndex(nFocusIndex+1);
					
						if(sel_next_rec==NULL)
						{
							// ����ͣ��
							if(IsFlagTrainWholeHelp(rec->flag))
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_DWAW, "ȡ������ͣ��");
							else
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DWAW, "ȡ������ͣ��");
						}
						else
						{
							bool bModify=true;
							if(IsFlagTrainPartRun(rec->flag))
							{
								bModify=false;
							}
							if(bModify&&m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							{
								// ����ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_ABS, "����ͣ��");
								// ȡ������ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_ABS_CANCEL, "ȡ������ͣ��");

								// ����ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_SIGNAL, "����ͣ��");	
								// ȡ������ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_SIGNAL_CANCEL, "ȡ������ͣ��");
							}
							else
							{
								// ����ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_ABS, "����ͣ��");
								// ȡ������ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_ABS_CANCEL, "ȡ������ͣ��");

								// ����ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_SIGNAL, "����ͣ��");	
								// ȡ������ͣ��
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_SIGNAL_CANCEL, "ȡ������ͣ��");
							}
						
							pTmpMenu.AppendMenu(MF_SEPARATOR);

							// ����ͣ��λ��					
							if(rec == NULL)
								return;

							if(bModify&&IsFlagStopAbs(rec->flag))
							{
								CMenu pTmpChildMenu;
								VERIFY(pTmpChildMenu.CreatePopupMenu());

								pTmpMenu.AppendMenu(MF_POPUP, (UINT) pTmpChildMenu.m_hMenu, "����ͣ��λ��");
								// ǰ��
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_UP, "ǰ��");	
								// �м�
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_MIDDILE, "�м�");
								// ��
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_BOTTOM, "��");

								if(IsAbsDrawUp(rec->stop_reason))
									pTmpChildMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
								else if(IsAbsDrawBottom(rec->stop_reason))
									pTmpChildMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
								else 
									pTmpChildMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
							}
						}
					}
				}
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifySide())
				{
					//��̬�����ӳ�վ�Ĺɵ�
					if(m_sFocusTrain == NULL) 
						return;

					if(rec == NULL)
						return;

					// �Ĺɵ�
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());

						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�Ĺɵ�");

						int i;
						for(i=0;i < m_pDoc->m_nDispDistanceNum;i++)
						{
							if(m_pDoc->m_sDispDistance[i].station_number  == rec->station)
								break;
						}  	
						if(i >= m_pDoc->m_nDispDistanceNum)
							return ;

						CString str;CString sStationName;

						//1 ��ʾ���κ�
						str = m_pDoc->GetTrainShowId(m_sFocusTrain);
						sStationName.Format("%s:", m_pDoc->m_sDispDistance[i].station_name);
						if(str.IsEmpty())
							str = "�޳���";
						else
							str = sStationName + str;
						str.TrimRight();
						pTmpMenu.AppendMenu(MF_STRING|MF_GRAYED,ID_CHANGE_TRACKS_BEGIN,str);

						//2 ���ӷָ���
						pTmpMenu.AppendMenu(MF_SEPARATOR);

						//3 ���ӹɵ��б�
						int index;
						for(int idx = 0; idx < m_pDoc->m_sDispDistance[i].number_of_side; idx++) 
						{
							int side_no = m_pDoc->m_sDispDistance[i].side[idx].side_number;	   
							str.Format("%s",m_pDoc->m_sDispDistance[i].side[idx].sidename);
							index = idx; 

							if(bNotThisSectionStation)
							{
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACKS_BEGIN+index, str);
							}
							else
							{
								if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
									pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_CHANGE_TRACKS_BEGIN+index, str);
								else
									pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACKS_BEGIN+index, str);
							}
							//��ǰ�ɵ���ѡ��֮
							if(rec == NULL)
								return;
							if(rec->arrive_side == side_no)
								pTmpMenu.CheckMenuItem(index+2, MF_CHECKED|MF_BYPOSITION);	

							BOOL bIsNoCrhSide = FALSE;
							//2007.04.18������ ������ɵ�����
							if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCheckCrhSide())
							{
								if(m_sFocusTrain->IsCHRTrain())
								{
									bIsNoCrhSide = m_pDoc->m_ptgctrl->mConfigTg.m_lineCfg.IsCRHSide(rec->station, side_no)?FALSE:TRUE;
								}
							}
							if(bIsNoCrhSide)
								pTmpMenu.EnableMenuItem(index+2, MF_BYPOSITION|MF_GRAYED);

						}
					}
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						if(this->m_pctrl->mConfigTg.m_context.IsEnableLockSide())
						{
							CMenu pTmpMenu;
							VERIFY(pTmpMenu.CreatePopupMenu());
							hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�����ɵ�");

							if(bNotThisSectionStation)
							{
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_LOCK_STATION_SIDE, "������վ�ɵ�");
								pTmpMenu.AppendMenu(MF_SEPARATOR);
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_LOCK_TRAIN_SIDE, "���������ɵ�");
							}
							else
							{
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_LOCK_STATION_SIDE, "������վ�ɵ�");
								pTmpMenu.AppendMenu(MF_SEPARATOR);
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_LOCK_TRAIN_SIDE, "���������ɵ�");
							}
							
							if(rec)
							{
								if(IsForceSide(*rec))
								{
									pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
								}
							}
					
							if(m_sFocusTrain->IsWholeTrainSideLock())
							{
								pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
							}
						}
					
						// �ĳ����ɵ�
						if(bNotThisSectionStation)
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACK, "�ĵ����ɵ�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_CHANGE_TRACK, "�ĵ����ɵ�");
						// �����޸Ĺɵ�
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_BATCH_MODIFY_TRACK, "�����޸Ĺɵ�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_BATCH_MODIFY_TRACK, "�����޸Ĺɵ�");
					}
				}

				// �ĳ���
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanRenameTrain())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,  ID_CHANGE_TRAINID, "�ĳ���");
					}
					
				}
				// �Ĵ�ñ����
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanHatTrain())	
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_REPLACE_HATTRAINID, "�Ĵ�ñ����");
					}
				}
				// ���۷�����
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyZFTrainId())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_REPLACE_TRAINID, "���۷�����");
					}
				}
				//if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanEngineTrain())
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
					// ǣ��/���һ���
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_GUA_JICHE, "ǣ��/���һ���");
					// ������λ��
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());

						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "������λ��");

						// �Ϸ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_TOP, "�Ϸ�");
						// �м�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_MIDDLE, "�м�");
						// �·�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_BOTTOM, "�·�");
						// ����ʾ
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_NO, "����ʾ");

						if(m_sFocusTrain == NULL) 
							return;
						//�ж��Ƿ���Ҫ��ʾ������ ��ȡ�õ�������ʱ��Ļ���λ��

						if(IsDisplyTractionTop(m_sFocusTrain->myTrainInfo.adjust_flag))
						{
							pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
						}
						else if(IsDisplyTractionMiddle(m_sFocusTrain->myTrainInfo.adjust_flag))
						{
							pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
						}
						else if(IsDisplyTractionBottom(m_sFocusTrain->myTrainInfo.adjust_flag))
						{
							pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
						}
						else
						{
							pTmpMenu.CheckMenuItem(3, MF_CHECKED|MF_BYPOSITION);
						}
					}
				}

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				// �г���ϸ��Ϣ
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_DATA, "�г���ϸ��Ϣ");
				// �г�����
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_ATTRIBUTE, "�г�����");

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				// ɾ���г�
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanDeleteTrain())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))//������ɾ���հ�ƻ��г�
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DELETE_TRAIN, "ɾ���г�");
					}
				}
				// �����г�
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanCopyTrain())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_COPY, "�����г�");
					}				
				}
			
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyDepartSJ())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ʱ��");

					if((m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT)||m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT)) && !bNotThisSectionStation)
					{
						// ���ռƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DEPART_BY_PLAN, "���ռƻ�");
						// ���ձ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DEPART_BY_MEET, "���ձ���");
					}
					else
					{
						// ���ռƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_PLAN, "���ռƻ�");
						// ���ձ���
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_MEET, "���ձ���");
					}
					if(rec == NULL)
						return;

					if(IsMakeDepartRouteByPlan(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsMakeDepartRouteByMeet(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
				}
				else
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ʱ��");

					// ���ռƻ�
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_PLAN, "���ռƻ�");
					// ���ձ���
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_MEET, "���ձ���");

					if(rec == NULL)
						return;

					if(IsMakeDepartRouteByPlan(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsMakeDepartRouteByMeet(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
				}

				// ����ͣ��
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanTechStop())
				{
					if(NULL == rec)
						return;
				
					if( IsTechStopTrain(*rec))
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !bNotThisSectionStation)
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_TECH_STOP, "����ͣ��");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_TECH_STOP, "����ͣ��");

					}
					else 
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !bNotThisSectionStation)
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TECH_STOP, "����ͣ��");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_TECH_STOP, "����ͣ��");
					}
				}
				else
				{
					if(NULL == rec)
						return;
					if( IsTechStopTrain(*rec))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_TECH_STOP, "����ͣ��");

					}
					else 
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_TECH_STOP, "����ͣ��");
					}
				}

				if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				{
					if(IsMustNoPass(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_INHBIT_PASS, "��ͨ��־");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_INHBIT_PASS, "��ͨ��־");
				}
				else
				{
					if(IsMustNoPass(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_INHBIT_PASS, "��ͨ��־");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_INHBIT_PASS, "��ͨ��־");
				}

				// �����г�
				if(IsTrainSecret(m_sFocusTrain->myTrainInfo))
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SET_SECRET_TRAIN, "�����г�");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_SET_SECRET_TRAIN, "�����г�");
				}
				else
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SET_SECRET_TRAIN, "�����г�");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_SET_SECRET_TRAIN, "�����г�");
				}
				
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetNoLateEarlyTime())
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
					std::map<ULONG, ULONG>::iterator it = m_pDoc->m_noearlylatetime.find(m_sFocusTrain->m_nTrainIndex);
					if(it != m_pDoc->m_noearlylatetime.end())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SET_NO_LATE_TIME, "ǿ������");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_SET_NO_LATE_TIME, "ǿ������");
					}
					else
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SET_NO_LATE_TIME, "ǿ������");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_SET_NO_LATE_TIME, "ǿ������");
					}
				}
				
				// �г�����XML
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SAVE_TRAIN_XML, "�����г�");

				// �˳�����״̬
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_EXIT_DRAW_TRAIN_STATE, "�˳�����״̬");

				// �޸�վ��
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyStationIdx())
				{
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_INDEX, "�޸�վ��");
				}
			}
			m_nFocusFigure = NULL;
		}
		// MARK menu
		else if(m_nFocusFigure != NULL)
		{
			m_sFocusTrain = NULL;
			hPopupMenu =  hResourceMenu.GetSubMenu(0);
			if(hPopupMenu == NULL)	
				break;
			// ɾ��ռλ�˵�
			hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

			// ������������
			hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_FIND_DDML, "������������");
			hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_WORK_INFO, "��ҵ��Ϣ");

			hPopupMenu->AppendMenu(MF_SEPARATOR);
			hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_CANCELWORK, "ȡ��ʩ��");
			hPopupMenu->AppendMenu(MF_SEPARATOR);

			bool iscopy=false;
			// ���Ʊ��
			if(m_nFocusFigure->type == SIDE_INHIBIT_MARK 
						||m_nFocusFigure->type == SIDE_TIANCHUANG_MARK 
						||m_nFocusFigure->type == SIDE_SLOW_MARK
						||m_nFocusFigure->type == SIDE_OCCUPY_MARK    //�ɵ�����,�ɵ��촰��,վ�ڻ���,�ɵ�ռ��
						||m_nFocusFigure->type == ABS_ELE_FIX_MARK 
						|| m_nFocusFigure->type == ABS_FIX_MARK 
						|| m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK
						|| m_nFocusFigure->type == SELF_CLOSE_MARK    //�����������
						|| m_nFocusFigure->type == ABS_INHIBIT_MARK 
						|| m_nFocusFigure->type == ABS_SLOW_MARK
						|| m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
			{          
				iscopy=true;
			}

			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanCopyMark() && iscopy)
			{
				CMenu pTmpMenu;
				VERIFY(pTmpMenu.CreatePopupMenu());

				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "���Ʊ��");

				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					// ����
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY, "����");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY_V_UP, "����");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY_V_DOWN, "����");

						// �ӳ�
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_CONTINUE_LINING, "�ӳ�");
					}
				}
				else
				{
					// ����
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY, "����");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY_V_UP, "����");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY_V_DOWN, "����");

						// �ӳ�
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_CONTINUE_LINING, "�ӳ�");
					}
				}

				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsAutoAddBlockEndTime(m_nFocusFigure->type))
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						// �Զ�
						if(m_pDoc->IsMarkMove(m_nFocusFigure->mark_index))
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_MOVE_START, "��ʼ�Զ��ӳ�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_MOVE_START, "��ʼ�Զ��ӳ�");
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_MOVE_STOP, "ֹͣ�Զ��ӳ�");
					}
				}
			}
			
			// ����Ҫ������ʩ������״̬
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus())
			{
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "ʩ��״̬");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// �ƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_PLAN, "ʩ���ƻ�");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_ACTIVE, "ʩ������");
						// ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COMPLETE, "ʩ�����");
					}

					if(IsBlockPlan(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsBlockActive(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsBlockComplete(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					}
				}
			}

			// ɾ��
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanDeleteMark())
			{
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DELETE_ABS, "ɾ��");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_DELETE_ABS, "ɾ��");
			}

			// ���ɵ�������
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanGenMarkDCmd())
			{
				LoadAutoGenDCmdXml();
				bool bFind = false;
				for(int idx=0;idx<m_menu.size();idx++)
				{
					if(m_nFocusFigure->type == m_menu[idx].type)
					{
						bFind = true;
						break;
					}
				}

				if(bFind)
				{
					m_map_menuid.clear();
					m_map_menuparentid.clear();
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "���ɵ�������");

					int menu_id = 0;
					for(int idx=0;idx<m_menu.size();idx++)
					{
						if(m_nFocusFigure->type == m_menu[idx].type)
						{
							m_map_menuid[ID_GEN_DCMD+menu_id] = m_menu[idx].id;
							m_map_menuparentid[ID_GEN_DCMD+menu_id] = m_menu[idx].parent_id;
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_GEN_DCMD+menu_id, m_menu[idx].text);
							menu_id++;
						}
					}
				}
			}
			
			
			// ���õ���������
			{
				if(m_nFocusFigure->type == ABS_INHIBIT_MARK ||
					m_nFocusFigure->type == ABS_FIX_MARK ||
					m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK ||
					m_nFocusFigure->type == ABS_ELE_FIX_MARK ||

					m_nFocusFigure->type == SIDE_INHIBIT_MARK ||
					m_nFocusFigure->type == SIDE_OCCUPY_MARK ||
					m_nFocusFigure->type == SIDE_TIANCHUANG_MARK)
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_BLOCK_ADJUST_TRAINS, "���õ���������");
						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}
			}
			
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsDirectModifyBlockTime())
			{
				// ̫ԭ �޸�ʩ��������ֹʱ��
				if(m_nFocusFigure->type ==ABS_INHIBIT_MARK ||m_nFocusFigure->type ==ABS_SLOW_MARK ||
					m_nFocusFigure->type == SIDE_INHIBIT_MARK ||m_nFocusFigure->type == SIDE_SLOW_MARK)
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_TIMESPAN, "�޸���ֹʱ��");
						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}
			}

			// ����
			hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_PROPERTY, "����");
		}
		// other menu
		else
		{
			m_nFocusFigure = NULL;
			m_sFocusTrain = NULL;

			hPopupMenu =  hResourceMenu.GetSubMenu(0);
			if(hPopupMenu == NULL)	
				break;
			// ɾ��ռλ�˵�
			hPopupMenu->DeleteMenu(0,MF_BYPOSITION);
			
			if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
			{
				// ��ʾ����
				// ���˳��˵�
				if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG || m_pDoc->GetTgTermType() == TERM_TG_TYPE::SHIFT_TG)
				{
					if(m_pDoc->m_bShowSingleTrain)
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SHOW_SINGLE_TRAIN, "��ʾ���˳�");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_SINGLE_TRAIN, "��ʾ���˳�");
				}
				if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "��ʾɸѡ");

					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ONLY_SHOW_MARK, "ֻ��ʾʩ������");	
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ONLY_SHOW_TRAIN, "ֻ��ʾ������");	
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_MARK_AND_TRAIN, "��ʾʩ�����ź�������");

					if (m_pDoc->IsOnlyShowMark()) 
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}
					else if (m_pDoc->IsOnlyShowTrain()) 
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
					else if (m_pDoc->IsShowTrainAndMark()) 
					{
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					}
				}

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DEFINE_COLOR, "�Զ����ͼ��ɫ");	

				hPopupMenu->AppendMenu(MF_SEPARATOR);

				CMenu pTmpMenu;
				VERIFY(pTmpMenu.CreatePopupMenu());
				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "������");
				int direct=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetShowTrainDirect();
				// ����
				if(direct==1)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_UP_TRAIN, "����");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_UP_TRAIN, "����");
				// ����
				if(direct==2)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DOWN_TRAIN, "����");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DOWN_TRAIN, "����");
				// ������
				if(direct==3)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DUAL_TRAIN, "������");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DUAL_TRAIN, "������");

				// �ͻ�
				hPopupMenu->AppendMenu(MF_SEPARATOR);

				CMenu pTmpMenu1;
				VERIFY(pTmpMenu1.CreatePopupMenu());

				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu1.m_hMenu, "�ͻ�");
				int type=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetShowTrainType();
				// �ͳ�
				if(type==1)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_PASEEGE_TRAIN, "�ͳ�");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_PASEEGE_TRAIN, "�ͳ�");
				// ����
				if(type==2)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_GOODS_TRAIN, "����");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_GOODS_TRAIN, "����");
				// �ͻ���
				if(type==3)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_PESSGOODS_TRAIN, "�ͻ���");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_PESSGOODS_TRAIN, "�ͻ���");


				// ����ͼ��ʼʱ��
				if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);

					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ͼ��ʼʱ��");

					// 18��
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_BASIC_BEGIN_TIME_18, "18��");
					// 12��
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_BASIC_BEGIN_TIME_12, "12��");
				}

				
				// ѡ�ж����г�
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.IsCanMutiMoveOrDel())
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SEL_MULTI_TRAIN, "�ƶ������г�");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_SEL_MULTI_TRAIN, "�ƶ������г�");
					
				// ɾ�������г�
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.IsCanMutiMoveOrDel())
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DEL_MULTI_TRAIN, "ɾ�������г�");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_DEL_MULTI_TRAIN, "ɾ�������г�");
			}
			else
			{
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_EXIT_DRAW_TRAIN_STATE, "״̬��λ");
			}
		}


		if(hPopupMenu == NULL) 
			break;
		hPopupMenu->TrackPopupMenu(TPM_LEFTALIGN |/* TPM_NONOTIFY |*/ TPM_RIGHTBUTTON,
			point.x,point.y,this,NULL);

		hPopupMenu = NULL;
		hResourceMenu.DestroyMenu( );
	}while(0);

}

void CTGForm::OnExitDeadState()
{
	m_bFirstPoint = false;
	m_bSecondPoint = false;
	m_selTrains.clear();
	m_pDoc->InitTGState();
}

void CTGForm::ShowUpDirectTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainDirect(1);
	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

void CTGForm::ShowDownDirectTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainDirect(2);
	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

void CTGForm::ShowDualDirectTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainDirect(3);
	if(m_pctrl->IsWorkMode())
		this->DisplyTrainByDirection();
	else
		this->Redraw();//ok
}

void CTGForm::ShowPassagerTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainType(1);
	this->Redraw();
}

void CTGForm::ShowGoodsTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainType(2);
	this->Redraw();
}

void CTGForm::ShowAllTrain() 
{
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainType(3);
	this->Redraw();
}

void CTGForm::OnShowBeginTimePoint6()
{
	if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
		return;

	m_pctrl->SetBeginHour(18);
	m_pDoc->m_TGGraphStartTime = 18*60*60;
	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);  

	Redraw();
}
void CTGForm::OnShowBeginTimePoint12()
{
	if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
		return;

	m_pctrl->SetBeginHour(12);
	m_pDoc->m_TGGraphStartTime = 12*60*60;

	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);  

	Redraw();
}
void CTGForm::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case 0x60:
		Redraw();
		break;
	default:
		break;
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

short CTGForm::OnZoomGraph(int nZoomFlag, short count)
{
	int nNewPerMiniutePix = m_pDoc->m_fPixelPerMinute;
	if(nZoomFlag == GRAPH_ZOOM::SMALL)
	{
		nNewPerMiniutePix=m_pctrl->mConfigTg.m_context.DecreateCurrentUnit(count);
	}
	else if(nZoomFlag == GRAPH_ZOOM::BIG)
	{
		nNewPerMiniutePix=m_pctrl->mConfigTg.m_context.IncreateCurrentUnit(count);
	}
	
	if(nNewPerMiniutePix == m_pDoc->m_fPixelPerMinute)
	{
		return m_pDoc->m_fPixelPerMinute;
	}
	
	int nOldPerHourPix = m_pDoc->m_fPixelPerHour;

	m_pDoc->m_fPixelPerMinute    = nNewPerMiniutePix;
	m_pDoc->m_fPixelPerTenMinute = m_pDoc->m_fPixelPerMinute * 10;
	m_pDoc->m_fPixelPerHalfHour	 = m_pDoc->m_fPixelPerMinute * 30;
	m_pDoc->m_fPixelPerHour		 = m_pDoc->m_fPixelPerMinute * 60;

	m_pDoc->m_nPageWidth = m_pctrl->GetTimeLength() * m_pDoc->m_fPixelPerHour;
	m_pDoc->m_nHorizontalPos = (m_pDoc->m_nHorizontalPos*m_pDoc->m_fPixelPerHour)/nOldPerHourPix;

	m_pDoc->SetGraphBeginEndPixel();

	SIZE mSize;
	mSize.cx = m_pDoc->m_nPageWidth;
	mSize.cy = m_pDoc->m_nPageHeight;

	SetScrollSizes(MM_TEXT,mSize);

	CClientDC dc(this);
	OnPrepareDC(&dc);
	ReLayoutBmpPageSize(dc);
	Redraw();

	SetScrollPos(SB_VERT,m_pDoc->m_nVerticalPos);
	SetScrollPos(SB_HORZ,m_pDoc->m_nHorizontalPos);
	ScrollWindow(0,0,NULL,NULL);
	int nHorzPos = this->GetScrollPos(SB_HORZ);
	if(m_pDoc->m_nHorizontalPos != nHorzPos)
	{
		m_pDoc->m_nHorizontalPos = nHorzPos;
		m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL); // �ػ�����
	}
	int nVertPos = this->GetScrollPos(SB_VERT);
	if(m_pDoc->m_nVerticalPos != nVertPos)
	{
		m_pDoc->m_nVerticalPos = nVertPos;
		m_pDoc->UpdateAllViews(this,VERTICAL_POSITION_CHANGE_WORK,NULL); // �ػ�����
	}
	m_pDoc->UpdateAllViews(NULL,GRAPH_ROOM_CHANGE,NULL);
	
	return m_pDoc->m_fPixelPerMinute;
}

void CTGForm::OnSelectSection(int nSectionId)
{	
	if(m_pDoc == NULL) 
		return;

	if (nSectionId == 0)
	{
		nSectionId = m_pDoc->m_ptgctrl->mConfigTg.GetCurrentSectionID();
	}

	m_pDoc->m_ptgctrl->mConfigTg.SetCurrentSectionID(nSectionId);

	m_pDoc->SetDocContext();

	SIZE mSize;
	mSize.cx = m_pDoc->m_nPageWidth;
	mSize.cy = m_pDoc->m_nPageHeight;

	SetScrollSizes(MM_TEXT,mSize);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	ReLayoutBmpPageSize(dc);
	
	m_pDoc->UpdateAllViews(NULL,FOCUS_SECTION_CHANGE,NULL);
	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);  

	SetRightScrollPosition();

	m_sFocusTrain = NULL;
	m_nFocusFigure = NULL;


	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainType(3);
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainDirect(DIRECT::DUAL);
	
	this->Redraw();

	return;
}

void CTGForm::OnShowWholeScreen(BYTE nFlag)
{	
	if(m_pDoc == NULL) 
		return;
	m_pDoc->SetDocContext();
	int nPageHeight = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetSectionHeight(m_pDoc->m_nCurrentSectionNo);
	m_pDoc->m_nPageHeight = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetWholeScreenHeight();

	if(m_pDoc->m_nPageHeight <= 0)
	{
		CRect rect;
		this->GetClientRect(&rect);
		m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.SetWholeScreenHeight(rect.Height());
		m_pDoc->m_nPageHeight = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetWholeScreenHeight();    
	}

	if(nFlag == 0)
	{
		m_pDoc->m_bShowWholeScreen=true;
		m_pDoc->m_nPageHeight = min(nPageHeight, m_pDoc->m_nPageHeight);
		m_pDoc->sysprint(4,0,"[OnShowWholeScreen] ������ʾ:%d m_nPageHeight: %d",
			m_pDoc->m_nCurrentSectionNo, m_pDoc->m_nPageHeight);
	}
	else
	{
		m_pDoc->m_bShowWholeScreen=false;
		m_pDoc->m_nPageHeight = nPageHeight;
		m_pDoc->sysprint(4,0,"[OnShowWholeScreen] ������ʾ:%d m_nPageHeight: %d",
			m_pDoc->m_nCurrentSectionNo, m_pDoc->m_nPageHeight);
	}

	m_pDoc->InitStationVerticalPos(false, true); 

	SIZE mSize;
	mSize.cx = m_pDoc->m_nPageWidth;
	mSize.cy = m_pDoc->m_nPageHeight;

	SetScrollSizes(MM_TEXT,mSize);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	ReLayoutBmpPageSize(dc);
	
	SetRightScrollPosition();

	m_pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);  



	m_sFocusTrain = NULL;
	m_nFocusFigure = NULL;

	this->Redraw(); // OnShowWholeScreen

	return;
}

void CTGForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRect rclScroll,rclClient;
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	if(m_pDoc == NULL)
		return;

	switch(nChar)	
	{
	case 0x30:  
	case '1':  
	case '2': 
	case '3':  
	case '4':  
	case '5': 
	case '6':  
	case '7':  
	case '8':  
	case 0x39:  
		if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
		{
			OnChangeArrivalDepartTime(nChar-0x30);
		}
		break;
	case 'C':
	case 'c':
		{
			OnThreeLine();
		}
		break;
	case VK_F7:// ʼ��/���� ��ݼ�
		{
			if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				return;
			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN)
			{
				if(m_stop_train_abs)
					OnAddtrainAbsStop();
				else
					OnAddtrainStop();
			}
			else if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
			{
				if(m_sFocusTrain == NULL)
					return;
				
				TgTRAIN_RECORD *rec = m_sFocusTrain->GetFirstRecord();
				if(rec == NULL)
					return;

				if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
				{
					if(rec->adjust_status & 0x0003)
					{
						AfxMessageBox("�Ѿ���ʵ�ʵ�,��ֹʹ�ÿ�ݼ�F7�޸�ʼ������뷽ʽ!");
						m_pDoc->sysprint(4,0,"����Աʹ�ÿ�ݼ�F7���%s���г�%sվ�����ʼ��ʧ��",m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));
						return;
					}
				}

				if(IsFlagStart(rec->flag))
					OnModifytrainAccept();
				else
					OnModifytrainStart();
			}
		}
		break;
	case VK_F8:// �յ�/���� ��ݼ�
		{
			if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				return;
			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN && !m_stop_train_abs)
			{
				OnAddtrainDeliver();
			}
			else if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
			{
				if(m_sFocusTrain == NULL)
					return;
				
				TgTRAIN_RECORD *rec = m_sFocusTrain->GetLastRecord();
				if(rec == NULL)
					return;

				if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
				{
					if(rec->adjust_status & 0x0003)
					{
						AfxMessageBox("�Ѿ���ʵ�ʵ�,��ֹʹ�ÿ�ݼ�F8�޸��յ��򽻳���ʽ!");
						m_pDoc->sysprint(4,0,"����Աʹ�ÿ�ݼ�F8���%s���г�%sվ�յ��򽻳�ʧ��",m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));
						return;
					}
				}

				if(IsFlagTerminal(rec->flag))
					OnModifytrainDeliver();
				else
					OnModifytrainTerminal();
			}
		}
		break;
	case VK_F9:// ͨ��<->����
		{
			if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				return;
			m_pDoc->sysprint(10,0, "����Աͨ�� VK_F9 ͨ������ת��");
			ChangeStationStayOrPass();
		}
		break;
	case VK_SPACE:
		{
			if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				return;

			if(m_sFocusTrain == NULL)
				return;

			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN)
			{
				OnAddtrainDeliver();  //����
				return;
			}

			if(m_nFocusRecordIndex < 0)
				return;

			timeb cur;
			ftime(&cur);
			DWORD curPress = ((short)cur.time)*1000 + cur.millitm;

			if(curPress - m_timePress > 1000)
			{
  				m_timePress = curPress;
				if(!m_pDoc->m_bInhibitEditRealTime)
				   OnTurnReality();   //תʵ��   
				m_pDoc->sysprint(10,0, "����Աͨ���ո������תʵ�� ����:%s �ڵ�:%d", m_pDoc->GetTrainShowId(m_sFocusTrain), m_nFocusRecordIndex);
			}
		}
		break;
	case VK_DELETE:
		{
			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN)
			{
				if(m_sFocusTrain != NULL)
				{
					TgTRAIN_RECORD* rec = m_sFocusTrain->GetLastRecord();
					if(rec == NULL)
						return;
					if(IsFlagStopAbs(rec->flag))
					{
						ClearFlagStopAbs(rec->flag);
					}

					if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime())
					{
						if (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
						{
							return;
						}
					}


					if(!IsFlagStopAbs(rec->flag))
					{		
						if(m_sFocusTrain->GetRecordNum() > 1)
						{
							m_sFocusTrain->DeleteTailRecord();
							m_pDoc->sysprint(5,0, "����ȡ���г� %s, ���һ����¼", m_pDoc->GetTrainShowId(m_sFocusTrain));
						}
						else
						{
							m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
							m_sFocusTrain->DeleteTailRecord();
							m_pDoc->sysprint(5,0, "���ɻ��߹�����, ɾ���г� %s", m_pDoc->GetTrainShowId(m_sFocusTrain));
						}
					}

					Invalidate(FALSE);
				}
			}
			else if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
			{
				OnDeleteTrain();
			}
		}
		break;
	case VK_UP:
		{
			if(this->m_pctrl->mConfigTg.m_context.IsEnableModifySideByKeyboard())
				ModifySideByKeyboard(0);
		}
		break;
	case VK_DOWN:
		{
			if(this->m_pctrl->mConfigTg.m_context.IsEnableModifySideByKeyboard())
				ModifySideByKeyboard(1);
		}
		break;
	case VK_LEFT:
		{
			ModifyTrainArlDepTimeByKeyboard(0);
			ModifyBlockTimeByKeyboard(0);
		}
		break;
	case VK_RIGHT:
		{
			ModifyTrainArlDepTimeByKeyboard(1);

			ModifyBlockTimeByKeyboard(1);
		}
		break;
	case VK_F1:
		{
			//20171225��--���ݾ���� ����F1�����ܹ�ƽ�ƺ�����·�г�
			m_pctrl->mConfigTg.m_context.SetMoveSameCZTrain(true);
			m_pctrl->mConfigTg.SetIsCanMoveBasciTrainWhole(true);
		}
		break;
	default:
		break;
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTGForm::OnAbsInhibitG()
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType4Inhibit = 0;
		m_nMarkType = ABS_INHIBIT_MARK;
	}
}

void CTGForm::OnAbsInhibitG2()
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType4Inhibit = 1;
		m_nMarkType = ABS_INHIBIT_MARK;
	}
}

void CTGForm::OnAbsSlowG()
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = ABS_SLOW_MARK;
	}
}

void CTGForm::DisplyTrainByDirection()
{
	if(!m_pDoc->m_bAcceptCadData)
		return;

	m_pDoc->m_nTrainLeapLineFlag = m_pctrl->mConfigTg.m_context.GetLeapLineFlag();
	if (m_sFocusTrain != NULL )
	{
		m_nFocusRectIndex = -1;
		m_nFocusRecordIndex = -1;
	}
	
	if(m_pctrl->IsWorkMode())
	{
		std::map<ULONG, CTG_TRAIN>::iterator it_train = myPDataManage->myTrain.begin();
		for(;it_train != myPDataManage->myTrain.end();++it_train)
		{
			if(it_train->second.mbDoubleDirect)
			{
				it_train->second.ptoftrainid_count=0;
			}
		}

		// ������Ϊ��ˢ�³���������
		m_Layer3.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_pDoc->m_brushWhite);
		map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
		while(it != myPDataManage->myTrain.end())
		{
			if(it->second.mbDoubleDirect)
				m_pDoc->DrawSingleTrain(&m_Layer3, &it->second, DRAW_NORMAL);
			it++;
		}
		m_pDoc->DrawAllCheZuHaoLine(&m_Layer3);
		RedrawLayer2();
	}
	else
	{
		std::map<ULONG, CTG_TRAIN>::iterator it_train = myPDataManage->myTrain.begin();
		for(;it_train != myPDataManage->myTrain.end();++it_train)
		{
			it_train->second.ptoftrainid_count=0;
		}
		RedrawLayer();
	}

	int nHorzPos = this->GetScrollPos(SB_HORZ);
	if(m_pDoc->m_nHorizontalPos != nHorzPos)
	{
		m_pDoc->m_nHorizontalPos = nHorzPos;
		m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL); // �ػ�����
	}
	int nVertPos = this->GetScrollPos(SB_VERT);
	if(m_pDoc->m_nVerticalPos != nVertPos)
	{
		m_pDoc->m_nVerticalPos = nVertPos;
		m_pDoc->UpdateAllViews(this,VERTICAL_POSITION_CHANGE_WORK,NULL); // �ػ�����
	}

	Invalidate(FALSE);
}

void CTGForm::Redraw()
{
	if(!m_pDoc->m_bAcceptCadData)
		return;

	m_pDoc->m_nTrainLeapLineFlag = m_pctrl->mConfigTg.m_context.GetLeapLineFlag();
	if (m_sFocusTrain != NULL )
	{
		m_nFocusRectIndex = -1;
		m_nFocusRecordIndex = -1;
	}
	
	std::map<ULONG, CTG_TRAIN>::iterator it_train = myPDataManage->myTrain.begin();
	for(;it_train != myPDataManage->myTrain.end();++it_train)
	{
		it_train->second.ptoftrainid_count=0;
	}

	if(m_pctrl->IsWorkMode())
	{
		m_Layer2.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_pDoc->m_brushWhite);
		m_Layer3.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_pDoc->m_brushWhite);
		m_Layer4.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_pDoc->m_brushWhite);
		
		map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
		while(it != myPDataManage->myTrain.end())
		{
			if(it->second.mbDoubleDirect)
				m_pDoc->DrawSingleTrain(&m_Layer3, &it->second, DRAW_NORMAL);
			else if(it->second.m_nDirectFlag==UP_DIRECT)
				m_pDoc->DrawSingleTrain(&m_Layer4, &it->second, DRAW_NORMAL);
			else
				m_pDoc->DrawSingleTrain(&m_Layer2, &it->second, DRAW_NORMAL);
			it++;
		}

		RedrawLayer2();

		m_pDoc->DrawAllCheZuHaoLine(&m_Layer3);
	}
	else
	{
		RedrawLayer();
	}

	int nHorzPos = this->GetScrollPos(SB_HORZ);
	if(m_pDoc->m_nHorizontalPos != nHorzPos)
	{
		m_pDoc->m_nHorizontalPos = nHorzPos;
		m_pDoc->UpdateAllViews(this,HORIZONTAL_POSITION_CHANGE,NULL); // �ػ�����
	}
	int nVertPos = this->GetScrollPos(SB_VERT);
	if(m_pDoc->m_nVerticalPos != nVertPos)
	{
		m_pDoc->m_nVerticalPos = nVertPos;
		m_pDoc->UpdateAllViews(this,VERTICAL_POSITION_CHANGE_WORK,NULL); // �ػ�����
	}

	Invalidate(FALSE);
}

void CTGForm::RedrawLayer()
{
	CBrush  m_brush;
	m_brush.CreateSolidBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor));
	m_Layer1.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_brush);
	m_brush.DeleteObject();

	CRect lastRect;
	if(FALSE==m_pDoc->m_nAutoMove)
	{
		if(m_pDoc->m_nBlockNum >= BACKGROUD::BLOCK::TWO)
		{
			// ��ʷͼ��
			lastRect = CRect(0,0,m_pDoc->m_fPixelPerMinute*12*60,m_pDoc->m_nPageHeight );

			m_brush.CreateSolidBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40));
			m_Layer1.FillRect(&lastRect,&m_brush);
			m_brush.DeleteObject();
		}
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			int nHistoryGraphWidth = m_pDoc->GetHistoryRegionWidth();
			int uStartX = nHistoryGraphWidth * m_pDoc->m_fPixelPerMinute;

			if(m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			{
				// ʵ��ͼ��
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30));
				lastRect = CRect(m_pDoc->m_fPixelPerMinute*12*60, 0, uStartX, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �ٽ�ͼ����10���ӣ�
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20));
				lastRect = CRect(uStartX, 0, uStartX + 10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight ); // �ٽ���10����,Ӧ�ø���ʵ��ÿ����������������,��ͬ
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �׶μƻ�����3Сʱ�����ٽ��ƻ�����
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10));
				lastRect = CRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX +m_pDoc->m_fPixelPerMinute*3*60, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();
			}
		}
	}
	else
	{
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::TWO)
		{
			// ��ʷͼ��
			lastRect = CRect(0,0,m_pDoc->m_fPixelPerMinute*12*60,m_pDoc->m_nPageHeight );

			m_brush.CreateSolidBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40));
			m_Layer1.FillRect(&lastRect,&m_brush);
			m_brush.DeleteObject();
		}
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			int nHistoryGraphWidth = m_pDoc->GetHistoryRegionWidth();
			int uStartX = nHistoryGraphWidth * m_pDoc->m_fPixelPerMinute;

			if(m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			{
				// ʵ��ͼ��
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30));
				lastRect = CRect(0, 0, uStartX, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �ٽ�ͼ����10���ӣ�
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20));
				lastRect = CRect(uStartX, 0, uStartX + 10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight ); // �ٽ���10����,Ӧ�ø���ʵ��ÿ����������������,��ͬ
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �׶μƻ�����3Сʱ�����ٽ��ƻ�����
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10));
				lastRect = CRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX +m_pDoc->m_fPixelPerMinute*3*60, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();
			}
		}
	}

	this->DrawStationAixis(&m_Layer1,CLR_LAWNGREEN);
	this->DrawTimeAixisExt(&m_Layer1,CLR_LAWNGREEN);

	// draw seperatorbar
	if(m_pctrl->mConfigTg.m_context.IsDrawCurrTimeBar() && m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
		m_pDoc->DrawSeperatorBar(&m_Layer1, CLR_BLUE);
	
	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	for(;it_block != myPDataManage->myMark.end();++it_block)
	{
		m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
	}

	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	while(it != myPDataManage->myTrain.end())
	{
		m_pDoc->DrawSingleTrain(&m_Layer1, &it->second, DRAW_NORMAL);
		it++;
	}

	m_pDoc->DrawAllCheZuHaoLine(&m_Layer1);

	return;
}

void CTGForm::OnTimer(UINT nIDEvent) 
{
	//ÿ15���Ӷ�ʱ���ͼƬ
	switch(nIDEvent)
	{
	case TIMER_IDEVENT_DRAWBAR:
		{
			if(!m_pDoc->m_bAcceptCadData)
				break;
			CTime tm = CTime::GetCurrentTime();
			if(m_lastUpdateMinute != tm.GetMinute() || m_lastUpdateHour != tm.GetHour())
			{
				m_lastUpdateMinute = tm.GetMinute();
				m_lastUpdateHour=tm.GetHour();
				
				if(m_pctrl->IsWorkMode())
				{
					if(FALSE==m_pDoc->m_nAutoMove)
					{
					RedrawLayer2();
					
					Invalidate(FALSE);
					}
				}
				else
				{
					if(FALSE==m_pDoc->m_nAutoMove)
					{
					int nHistoryGraphWidth = m_pDoc->GetHistoryRegionWidth();
					int uStartX = nHistoryGraphWidth * m_pDoc->m_fPixelPerMinute;

					CRect regine;
					if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
						regine=CRect(uStartX-30, 0, uStartX + m_pDoc->m_fPixelPerHour*3+10, m_pDoc->m_nPageHeight);
					else 
						regine=CRect(uStartX-30, 0, uStartX + 30, m_pDoc->m_nPageHeight);
					EraseRegion(regine);
					}
				}
			}

			break;
		}
	case TIMER_IDEVENT_FLASH:
		{
			FlashTrain();
			break;
		}

	case TIMER_CHECK:
		{
			CheckMarkMove();
			break;
		}
	}

	CScrollView::OnTimer(nIDEvent);
}

void CTGForm::SaveTGPicture(CString filePath)
{		
	CString picName;	
	if(filePath.IsEmpty())
	{
		filePath = m_pDoc->GetDataPath();
	}
	picName.Format("\\����%d",m_pDoc->m_nCurrentSectionNo);
	filePath += picName;

	CTime t = CTime::GetCurrentTime();
	picName.Format("%04d-%02d-%02d %02d-%02d-%02d.Jpg", t.GetYear(), t.GetMonth(), t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
	filePath += picName;

	m_bmpFile.SaveBitmapAsJpgFile(&m_Layer1,&m_bitmap1,(char*)(LPCTSTR)filePath, m_pDoc->m_nPageWidth);
}

void CTGForm::DrawBox(CDC &dc, CRect &rect, BOOL xor)
{
	/*
	R2_BLACK 1 ��Ϊ 0
	R2_WHITE 16 ��Ϊ 1
	R2_NOP 11 ���ֲ���
	R2_NOT 6 ��Ļ���෴��ɫ
	R2_COPYPEN 13 ������ɫ
	R2_NOTCOPYPEN 4 ���ʵ��෴��ɫ
	R2_MERGEPENNOT 14 ������ɫ����Ļ�෴��ɫ�ĸ���
	R2_MASKPENNOT 5 ���ʺ���Ļ����Ĺ�ͬɫ
	R2_MERGENOTPEN 12 ��Ļ��ɫ�뻭���෴��ɫ�ĸ���
	R2_MASKNOTPEN 3 ��Ļ�ͻ��ʷ���Ĺ�ͬɫ
	R2_MERGEPEN 15 ������ɫ����Ļ��ɫ�ĸ���
	R2_NOTMERGEPEN 2 R2_MERGEPEN �ķ�����ɫ
	R2_MASKPEN 9 ���ʺ���Ļ�Ĺ�ͬɫ
	R2_NOTMASKPEN 8 R2_MASKPEN �ķ�����ɫ
	R2_XORPEN 7 ��������Ļ��ɫ�ĸ��ϣ�����ͬʱ����
	R2_NOTXORPEN 10 R2_XORPEN �ķ�����ɫ
	*/

	CPen pen;
	// Save the device m_context
	int ret = dc.SaveDC();

	dc.SetROP2(R2_NOTXORPEN);
	pen.CreatePen(PS_DASH, 0, RGB(255, 0, 0));

	dc.SelectObject(&pen);

	dc.MoveTo(rect.left,  rect.top);
	dc.LineTo(rect.right, rect.top);
	dc.LineTo(rect.right, rect.bottom);
	dc.LineTo(rect.left,  rect.bottom);
	dc.LineTo(rect.left,  rect.top);

	dc.RestoreDC(ret);
	pen.DeleteObject();
}

void CTGForm::DrawLine (CDC &dc,CPoint &first,CPoint &second,BOOL xor)
{
	CPen pen;
	// Save the device m_context
	dc.SaveDC();
	if (xor) 
	{
		dc.SetROP2(R2_NOTXORPEN);
		pen.CreatePen(PS_DASH,  0, RGB(0, 0, 0)); // 0 width = 1 device unit
	} 
	else 
	{
		pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0)); // 0 width = 1 device unit
	}
	dc.SelectObject(&pen);
	// Draw the rect with lines (eliminate rect middle fill)
	dc.MoveTo(first);
	dc.LineTo(second);
	// Clean up
	dc.RestoreDC(-1);
	pen.DeleteObject();
}

void CTGForm::OnMarkProperty() 
{
	if (m_nFocusFigure == NULL)
		return;

	switch(m_nFocusFigure->type)
	{
	case ABS_INHIBIT_MARK:
		{
			CAbsInhibitProperty *dlg = new CAbsInhibitProperty(m_pDoc,m_nFocusFigure,this);
			if(dlg == NULL) break;

			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case NEW_ABS_ELE_FIX_MARK://���ݵ�������
		{
			CAbsEleFixLanzhouProperty *dlg = new CAbsEleFixLanzhouProperty(m_pDoc,m_nFocusFigure,this);
			if(dlg == NULL) break;

			if(dlg->DoModal() == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case ABS_ELE_FIX_MARK:
		{
			CAbsEleFixProperty* dlg = new CAbsEleFixProperty(m_pDoc,m_nFocusFigure,this);
			if(dlg == NULL) break;

			if(dlg->DoModal() == IDOK)
			{				
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case ABS_FIX_MARK:
		{
			CAbsFixProperty *dlg = new CAbsFixProperty(m_pDoc,m_nFocusFigure,this);
			if(dlg == NULL) break;
			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure); m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case ABS_SLOW_MARK:
		{
			CAbsSlowProperty *dlg = new CAbsSlowProperty(m_pDoc,m_nFocusFigure,this);

			if(dlg == NULL) break;
			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;

				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case SELF_CLOSE_MARK:
		{
			CAbsSelfCloseProperty *dlg = new CAbsSelfCloseProperty(m_pDoc,m_nFocusFigure,this);
			if(dlg == NULL) break;
			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;				
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case SIDE_OCCUPY_MARK:
	case SIDE_INHIBIT_MARK:
	case SIDE_TIANCHUANG_MARK:
		{
			int index = 0;
			for(index=0;index < m_pDoc->m_nDispDistanceNum;index++)
			{
				if(m_pDoc->m_sDispDistance[index].station_number  == m_nFocusFigure->station)
					break;
			}

			CStationInhibitProperty *dlg = NULL;
			if(index < m_pDoc->m_nDispDistanceNum)
			{
				if(m_pDoc->m_sDispDistance[index].number_of_side>100)
				{
					dlg = new CStationInhibitPropertyLarge(m_nFocusFigure,m_pDoc,this);
				}
				else if(m_pDoc->m_sDispDistance[index].number_of_side>=20)
				{
					dlg = new CStationInhibitPropertyMiddle(m_nFocusFigure,m_pDoc,this);
				}
				else
				{
					dlg = new CStationInhibitProperty(m_nFocusFigure,m_pDoc,this);
				}
			}
			else
			{
				dlg = new CStationInhibitProperty(m_nFocusFigure,m_pDoc,this);
			}

			if(dlg == NULL) break;
			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	case SIDE_SLOW_MARK:
		{
			int index = 0;
			for(index=0; index < m_pDoc->m_nDispDistanceNum; index++)
			{
				if(m_pDoc->m_sDispDistance[index].station_number  == m_nFocusFigure->station)
					break;
			}
			
			CStationSlowProperty *dlg = NULL;
			if(index < m_pDoc->m_nDispDistanceNum)
			{
				if(m_pDoc->m_sDispDistance[index].number_of_side>100)
				{
					dlg = new CStationSlowPropertyLarge(m_nFocusFigure,m_pDoc,this);
				}
				else if(m_pDoc->m_sDispDistance[index].number_of_side>=20)
				{
					dlg = new CStationSlowPropertyMiddle(m_nFocusFigure,m_pDoc,this);
				}
				else
				{
					dlg = new CStationSlowProperty(m_nFocusFigure,m_pDoc,this);
				}
			}
			else
			{
				dlg = new CStationSlowProperty(m_nFocusFigure,m_pDoc,this);
			}
			
			if(dlg == NULL) break;
			if(dlg->DoModal () == IDOK)
			{
				if(m_nFocusFigure == NULL)
					break;
				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;

	case GRAPH_NOTE_MARK:
	case NEW_GRAPH_NOTE_MARK:
		{
			int temp = 1;
			if(m_nFocusFigure->flag == 2 || m_nFocusFigure->flag == 4)
			{
				temp = 2;
			}

			CDlgNoteText *dlg = new CDlgNoteText(temp,m_nFocusFigure,m_pDoc,this);
			if(dlg == NULL) break;
			if(dlg->DoModal() == IDOK)
			{
				m_nFocusFigure->flag = dlg->m_nType;
				switch(dlg->m_nType)
				{
				case 1:
				case 3:
				case 5:
					{
						int len = dlg->m_text.GetLength();
						if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
						{
							AfxMessageBox("����̫��,���ܳ���128��");	
							return;
						}
						strncpy(m_nFocusFigure->text, (LPCTSTR)dlg->m_text, len);
						m_nFocusFigure->text[len] = 0;
						if(dlg->m_roundFlag == 1) //blue
						{
							SetRectBlue(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 2)//red
						{
							SetRectRed(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 3)//red
						{
							SetCircleRed(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 4)//blue
						{
							SetCircleBlue(m_nFocusFigure->slow_time);
						}
						else//none
						{
							SetSLWRoundNone(m_nFocusFigure->slow_time);
						}
					}
					break;

				case 2:
				case 4:
					{				
						int len = dlg->m_text2.GetLength();
						if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
						{
							AfxMessageBox("����̫��,���ܳ���128��");	
							return;
						}
						strncpy(m_nFocusFigure->text, (LPCTSTR)dlg->m_text2, len);
						m_nFocusFigure->text[len] = 0;
						if(dlg->m_roundFlag == 1) //blue
						{
							SetRectBlue(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 2)//red
						{
							SetRectRed(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 3)//red
						{
							SetCircleRed(m_nFocusFigure->slow_time);
						}
						else if(dlg->m_roundFlag == 4)//blue
						{
							SetCircleBlue(m_nFocusFigure->slow_time);
						}
						else//none
						{
							SetSLWRoundNone(m_nFocusFigure->slow_time);
						}
					}
					break;

				default:
					m_nFocusFigure->text[0] = 0x00;
					break;
				}


				if (dlg->m_colorFlag==1)
					SetWordRed(m_nFocusFigure->slow_time);
				else if (dlg->m_colorFlag==2)
					SetWordBlue(m_nFocusFigure->slow_time);

				m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
			}
			delete dlg;
		}
		break;
	default:
		break;
	}
	Invalidate(FALSE);

}

void CTGForm::OnStationSlowG() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SIDE_SLOW_MARK;
		m_nSubMarkType = 0;
	}	
}

void CTGForm::OnSideOccupy() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SIDE_OCCUPY_MARK; //�ɵ�ռ��
		m_nSubMarkType = 0;
	}	
}
void CTGForm::OnSideInhibitG() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SIDE_INHIBIT_MARK;
		m_nSubMarkType = 0;
	}		
}

void CTGForm::OnContinueDraw() 
{
	m_stop_train_abs = false;
	CClientDC dc(this);
	OnPrepareDC(&dc);

	m_strArrivalTrainId="";
	m_pDoc->sysprint(4,0,"[TG] CTGForm::OnContinueDraw");
	TgTRAIN_RECORD *rec = NULL;
	if(m_sFocusTrain == NULL)
		return;

	//20130419 �޸�"��������" ���������޸�ʵ�ʵ� hjz
	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime())
	{
		if (m_nMouseDownPosition == ARRIVAL_POSITION)
		{
			rec=m_sFocusTrain->GetRecordByRecIndex(m_nFocusRecordIndex);
			if(NULL == rec)
				return;
			int station=rec->station;
			for (int index=m_nFocusRecordIndex;index<m_sFocusTrain->GetRecordNum();index++)
			{
				rec = m_sFocusTrain->GetRecordByRecIndex(index);
				if(NULL==rec)
					return;
				if (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
				{
					CString text;
					text.Format("��ע��, %s ���г��� %s ��������, ���Ǹ��������ں���վ %s ����ʵ�ʵ�, ��ֹ��������", 
									m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(station) , m_pDoc->GetStationName(rec->station));
					m_pDoc->sysprint(4,0, "%s", text);
					AfxMessageBox(text);
					return;
				}
			}
		}
		else
		{
			rec = GetFoucsTrainRecord();
			if(NULL==rec)
					return;
			int station=rec->station;
			if (IsActualDepartTime(rec->adjust_status))
			{
					CString text;
					text.Format("��ע��, %s ���г��� %s ��������, ���Ǹ��������� %s ������Ϊʵ�ʵ�, ��ֹ��������", 
									m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station) , m_pDoc->GetStationName(rec->station));
					m_pDoc->sysprint(4,0, "%s", text);
					AfxMessageBox(text);
					return;
			}

			for (int index=m_nFocusRecordIndex+1;index<m_sFocusTrain->GetRecordNum();index++)
			{
				rec = m_sFocusTrain->GetRecordByRecIndex(index);
				if(NULL==rec)
					return;
				if (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
				{
					CString text;
					text.Format("��ע��, %s ���г��� %s ��������, ���Ǹ��������ں���վ %s ����ʵ�ʵ�, ��ֹ��������", 
									m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(station) , m_pDoc->GetStationName(rec->station));
					m_pDoc->sysprint(4,0, "%s", text);
					AfxMessageBox(text);
					return;
				}
			}
		}
	}

	EraseTrain(*m_sFocusTrain);

	int nRecNum = m_sFocusTrain->GetRecordNum() ;
	for (int i =  nRecNum -1 ; i > m_nFocusRecordIndex ;i--)
	{
		m_sFocusTrain->DeleteTailRecord();
	}
	rec = m_sFocusTrain->GetLastRecord();
	if(rec == NULL)
		return;

	if(m_nMouseDownPosition == ARRIVAL_POSITION )
	{
		rec->depart = rec->arrive;
	}

	if(rec->depart_train_id.IsEmpty())
	   rec->depart_train_id = rec->arrive_train_id;

	rec->stop_abs_e_time = 0;
	rec->stop_abs_s_time = 0;
	rec->stop_abs_position = 0;
	ClearFlagStopAbs(rec->flag);
	if(IsFlagTerminal(rec->flag))
	{	
		SetFlagGiveout(rec->flag);
	}

	if (rec->rec_index!=0)
		rec->flag=0;

	if(!IsAutoMakeArriveRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա��������%s���г� %s �ӳ���·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeArriveRoute(rec->stop_condition);
	}
	if(!IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա��������%s���г� %s ������·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeDepartRoute(rec->stop_condition);
	}

	// �յ���ͳ�������
	rec->static_arrive=0;
	rec->static_depart=0;
	
	m_pDoc->m_nOperatorStatus = INSERT_TRAIN;

	m_strTrainId = rec->depart_train_id;
	
	m_pDoc->SetContinueDraw(rec->rec_index,m_sFocusTrain->m_nTrainIndex);
	
	for(int idx = m_sFocusTrain->m_nPointCount-1; idx > 0; idx--)
	{
		if(m_sFocusTrain->m_pPerPointInfo[idx].record_index == m_nFocusRecordIndex)
			break;
		m_sFocusTrain->m_nPointCount--;
	}
	m_pOldMousePoint = m_pContinuePoint = m_sFocusTrain->GetContinuePoint();

	int node_index;
	int nStationNo = m_pDoc->ConvertYPostoStationNo(m_pContinuePoint.y, node_index);


	if(rec->out_station_index < 0)
	{
		if(nStationNo == rec->station)
			rec->out_station_index = node_index;
	}
}

void CTGForm::OnLockStationSide()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	if(IsForceSide(*rec))
	{
		ReSetForceSide(*rec);
		m_pDoc->sysprint(4,0,"[�ɵ�����] ����Աȡ���г�:%s, �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station),
			rec->arrive_side, rec->depart_side);
	}
	else
	{
		SetForceSide(*rec);
		m_pDoc->sysprint(4,0,"[�ɵ�����] ����Ա�����г�:%s, �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station),
			rec->arrive_side, rec->depart_side);
	}
	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

void CTGForm::OnLockTrainSide()
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	bool bLockWholeTrainSide=m_sFocusTrain->IsWholeTrainSideLock();

	if(bLockWholeTrainSide)
		m_pDoc->sysprint(4,0,"[���г�վ�ɵ�����] ����Աȡ���г�:%s, ���г�վ�ɵ�����", 	m_pDoc->GetTrainShowId(m_sFocusTrain));
	else
		m_pDoc->sysprint(4,0,"[���г�վ�ɵ�����] ����Ա�����г�:%s, ���г�վ�ɵ�����", 	m_pDoc->GetTrainShowId(m_sFocusTrain));

	TgTRAIN_RECORD *rec;
	for(int i=0; i<m_sFocusTrain->GetRecordNum(); i++)
	{
		rec = m_sFocusTrain->GetRecordByRecIndex(i);
		if(NULL != rec)
		{
			if(bLockWholeTrainSide)
			{
				ReSetForceSide(*rec);
				m_pDoc->sysprint(4,0,"[���г�վ�ɵ�����] ����Աȡ���г�:%s, �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
					m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station),
					rec->arrive_side, rec->depart_side);
			}
			else
			{
				SetForceSide(*rec);
				m_pDoc->sysprint(4,0,"[���г�վ�ɵ�����] ����Ա�����г�:%s, �ڳ�վ:%s �ɵ�����, �ɵ�:%d %d", 
					m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station),
					rec->arrive_side, rec->depart_side);
			}
		}
	}

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

// �ĵ����ɵ�
void CTGForm::OnChangeTrack() 
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	TgTRAIN_RECORD *rec;
	rec = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex);
	if(NULL == rec)
		return;

	if(this->m_pctrl->mConfigTg.IsNotThisSectionStation(rec->station))
	{
		return;
	}

	if(IsForceSide(*rec))
	{
		MessageBox("�ɵ�����,�������޸�", "����", MB_OK|MB_ICONHAND);
		return;
	}

	CChangeTrackDlg *dlg = new CChangeTrackDlg(this);
	if(NULL != dlg)
	{
		dlg->m_pDoc = m_pDoc;
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlg->m_rec_index = m_nFocusRecordIndex;
		
		dlg->Create(IDD_DLG_CHANGE_TRACK, this);
		dlg->ShowWindow(SW_SHOW);
	}
}

void CTGForm::OnBatchChangeTrack()
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;
	// �����޸Ĺɵ�,���뱣��
	CSuperPassDlg pdlg;
	if (pdlg.DoModal() == IDOK)
	{
		if ( pdlg.m_super_pass.Compare("1234") != 0)
		{
			CString str;
			str.Format("%s","�������!");
			CDlgMessageBox dlg(str);
			dlg.DoModal();
			return;
		}
	}
	else
		return;

	m_pDoc->m_ptgctrl->TG_NOTICE(5, 0, m_sFocusTrain->GetTrainIndex(), 0);
}

void CTGForm::OnSetInhibitTrainPass()
{
	if(m_sFocusTrain == NULL)	
		return;
	if( m_nFocusRecordIndex <0)
		return;
	CTG_TRAIN single;
	single = *m_sFocusTrain;
	TgTRAIN_RECORD *rec = single.GetRecordByPosition (m_nFocusRecordIndex);
	if(rec == NULL) 
		return; 

	CSuperPassDlg pdlg;
	if (pdlg.DoModal() == IDOK)
	{
		if ( pdlg.m_super_pass.Compare("1234") != 0)
		{
			CString str;
			str.Format("%s","�������!");
			CDlgMessageBox dlg(str);
			dlg.DoModal();
			return;
		}
	}
	else
		return;

	CString sResult;
	if(IsMustNoPass(*rec))
	{
		ClearFlagMustNoPass(*rec);
		sResult = "����ͨ��";
	}
	else
	{
		SetFlagMustNoPass(*rec);
		sResult = "��ֹͨ��";
	}

	m_pDoc->sysprint(4,0,"����Ա����:%s�� %sվ:%s", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station), sResult);
	m_pDoc->SendUpdateTrain(single);
}

// �г�����
void CTGForm::OnTrainAttribute() 
{
	if(m_sFocusTrain == NULL)
		return;
		
	CTrainAttributeSheet *nSheet = new CTrainAttributeSheet(this);
	if(NULL != nSheet)
	{
		nSheet->m_pDoc = m_pDoc;
		nSheet->m_train_index = m_sFocusTrain->m_nTrainIndex;
		nSheet->SetActivePage(0);
		nSheet->Create(this);
		//nSheet->ShowWindow(SW_SHOW);
	}
}

// �޸Ĺɵ�
void CTGForm::OnChangeTracks(UINT message) 
{
	int iSideSel = message - ID_CHANGE_TRACKS_BEGIN;

	if(iSideSel<0) return;
	if(m_sFocusTrain == NULL) 
		return;

	int nFocusIndex = m_nFocusRecordIndex;
	if(nFocusIndex < 0) 
		return;

	TgTRAIN_RECORD *rec;
	rec = m_sFocusTrain->GetRecordByPosition(nFocusIndex);
	if(NULL == rec)
		return;

	if(this->m_pctrl->mConfigTg.IsNotThisSectionStation(rec->station))
	{
		return;
	}

	if(IsForceSide(*rec))
	{
		MessageBox("�ɵ�����,�������޸�", "����", MB_OK|MB_ICONHAND);
		return;
	}

	//// ��·���޸Ĺɵ� �޸Ĺɵ���ʾ�û�
	//if(this->m_pctrl->mConfigTg.m_context.IsSwtichStation(rec->station))
	//{
	//	CSuperPassDlg pdlg;
	//	if (pdlg.DoModal() == IDOK)
	//	{
	//		if (pdlg.m_super_pass.Compare("1234") != 0)
	//		{
	//			CString str;
	//			str.Format("%s","�������!�޸Ĺɵ�ʧ��!");
	//			CDlgMessageBox dlg(str);
	//			dlg.DoModal();
	//			return;
	//		}
	//	}
	//	else
	//		return;
	//}

	if(m_sFocusTrain == NULL) 
		return;

	int i ;int side_no = 0;
	for( i=0;i < m_pDoc->m_nDispDistanceNum;i++)
	{
		if(m_pDoc->m_sDispDistance[i].station_number  == rec->station)
			break;
	}  	
	if(i >= m_pDoc->m_nDispDistanceNum)
		return ;
	if(iSideSel> m_pDoc->m_sDispDistance[i].number_of_side)
		return;

	side_no = m_pDoc->m_sDispDistance[i].side[iSideSel].side_number;

	WORD uOldSide = rec->arrive_side; 
	WORD uOldDetSide = rec->depart_side; 
	if(side_no == uOldSide && side_no == uOldDetSide)
	{
		m_pDoc->sysprint(0,0,"�Ҽ��˵��޸Ĺɵ���ԭ�ɵ���ͬ");
		return;
	}

	if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, rec, m_sFocusTrain))
	{
		return;
	}

	CTG_TRAIN* pTrain1 = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(NULL == pTrain1)
	{
		return;
	}
	CTG_TRAIN train =*pTrain1;
	TgTRAIN_RECORD *newrec = train.GetRecordByPosition(nFocusIndex);
	if(NULL == newrec || rec->station != newrec->station)
		return;
	
	if(!m_pDoc->SideSetEntry(this->m_hWnd, side_no, side_no, nFocusIndex, &train))
	{
		return;
	}
		
	SetSideFromDispatcher(*newrec);

	newrec->arrive_side = side_no;
	newrec->depart_side = side_no;

	CString arlSide = m_pDoc->GetSideName(newrec->station, newrec->arrive_side);
	CString depSide = m_pDoc->GetSideName(newrec->station, newrec->depart_side);

	//���ݾ֣��ÿ��г���ͱ�־�޸Ĺɵ���ʾ��
	if (this->m_pctrl->mConfigTg.m_context.IsShowWarningDlgByChangeSide() && train.GetTrainType() == TYPE::PASSENGER)//���ر�־λ�ҵ�ǰ�г��ǿͳ�
	{
		if ( newrec->arrive_side != newrec->plan_arrive_side || newrec->depart_side != newrec->plan_depart_side)
		{
			MessageBoxByChangeSide(rec, arlSide, depSide);
		}
	}

	CStringArray ary;
	m_pDoc->CheckTrainSideInfo(&train, nFocusIndex, ary);
	
	if(ary.GetCount()>0)
	{
		CString strTrainName = m_pDoc->GetTrainShowId(train);
		CString strStaName   = m_pDoc->GetStationName(newrec->station);
		CString strSideName  = m_pDoc->GetSideName(newrec->station, side_no);
		m_pDoc->sysprint(4,0,"����Աͨ���Ҽ��˵��޸�:%s�� %s �ɵ�Ϊ:%s, ��������:", strTrainName, strStaName, strSideName);

		CString strText, tmp;
		for(int i=0; i<ary.GetCount(); i++)
		{
			tmp=ary.GetAt(i);
			m_pDoc->sysprint(4,0,"%s", tmp);
			strText += tmp;
			strText += "\n";
		}
		CString str;
		str.Format("\n��ȷʵ�޸�:%s�� %sΪ%s��?", strTrainName, strStaName, strSideName);
		if(MessageBox(strText+str, "����", MB_YESNO|MB_ICONHAND)==IDYES)
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ����޸Ĺɵ�");
		}
		else
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸Ĺɵ�");
			return;
		}			
	}

	m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, MODIFY_SIDE, 0, nFocusIndex);
	m_pDoc->sysprint(4,0,"[TG]: ����Աͨ���Ҽ��˵��޸�%s %s�ɵ�:(%dG %dG)-->(%dG %dG)", m_pDoc->GetTrainShowId(train), m_pDoc->GetStationName(newrec->station), uOldSide, uOldDetSide, side_no, side_no);
}

// ȡ������ͣ��
void CTGForm::OnStopAbsCancel()
{
	if(m_sFocusTrain == NULL)	
		return;
	if(m_nFocusRecordIndex <0)
		return;

	TgTRAIN_RECORD *rec;
	rec = m_sFocusTrain->GetRecordByRecIndex (m_nFocusRecordIndex);
	if(rec==NULL) return;
	if(!IsFlagStopAbs(rec->flag))
	{
		return;
	}

	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		rec->szAbsText = "";
	}
	CancelFlagStopSignal(rec->flag);
	ReSetAbsDraw(rec->stop_reason);
	if(IsFlagStopAbs(rec->flag))
	{
		CancelFlagStopAbs(rec->flag);
		rec->stop_abs_s_time = 0;
		rec->stop_abs_e_time = 0;
		ReSetAbsDraw(rec->stop_reason);
	}

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

// ����ͣ��
void CTGForm::OnStopAbs() 
{
	TgTRAIN_RECORD *rec=GetFoucsTrainRecord();
	if(rec == NULL)	
		return;
	TgTRAIN_RECORD *next_rec = m_sFocusTrain->GetRecordByRecIndex (m_nFocusRecordIndex+1);
	if(next_rec == NULL)
		return;

	if(IsFlagStopAbs(rec->flag))
		return;

	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		CAbsStopDlg dlg;
		int dlt = (next_rec->arrive-rec->depart)/60;
		if(dlg.DoModal() == IDOK)
		{
			if(dlg.m_abs_stop_time-2 > dlt)
			{
				dlt -= 2;
				CString text;
				text.Format("��ע��, �����������ͣ��ʱ�ִ�����������ʱ,\n ����ͣ��ʱ���޸�Ϊ%d", dlt);
				if(IDNO == MessageBox(text, "��ʾ", MB_YESNO))
				{
					return;
				}
				rec->stop_abs_s_time = 1;
				rec->stop_abs_e_time = rec->stop_abs_s_time + dlt;
			}
			else
			{
				rec->stop_abs_s_time = 2;
				rec->stop_abs_e_time = rec->stop_abs_s_time + dlg.m_abs_stop_time;
			}
			rec->szAbsText = dlg.m_reason;
		}
		else
		{
			return;
		}
	}
	else
	{
		rec->stop_abs_s_time = 2;
		rec->stop_abs_e_time = 6;
	}

	CancelFlagStopSignal(rec->flag);

	SetFlagStopAbs(rec->flag);
	ReSetAbsDraw(rec->stop_reason);
		
	if(m_pDoc->GetStationVPos(m_sFocusTrain,rec,PRIMARY_TRACK,EXIT_POSITION) > m_pDoc->GetStationVPos(m_sFocusTrain,next_rec,PRIMARY_TRACK,ENTRY_POSITION))
		SetFlagStopAbsUp(rec->flag);
	else
		SetFlagStopAbsDown(rec->flag);
	
	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, SET_ABS_TOP, rec->station, rec->rec_index);
	}
	else
	{
		m_pDoc->SendUpdateTrain(m_sFocusTrain);
	}
}

// ȡ������ͣ��
void CTGForm::OnStopSignalCancel() 
{
	if(m_sFocusTrain == NULL)	
		return;
	
	TgTRAIN_RECORD *rec;
	rec = m_sFocusTrain->GetRecordByPosition (m_nFocusRecordIndex );
	if (rec==NULL)
		return;

	if(!IsFlagStopSignal(rec->flag))
		return;
	
	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		rec->szAbsText = "";
	}

	if(IsFlagStopAbs(rec->flag))
		CancelFlagStopAbs(rec->flag);

	ReSetAbsDraw(rec->stop_reason);
	CancelFlagStopSignal(rec->flag);// = SCHD_PASS;	

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

// ����ͣ��
void CTGForm::OnStopSignal() 
{
	if(m_sFocusTrain == NULL)	
		return;
	
	TgTRAIN_RECORD *rec,*next_rec;
	rec = m_sFocusTrain->GetRecordByPosition (m_nFocusRecordIndex );
	next_rec = m_sFocusTrain->GetRecordByPosition (m_nFocusRecordIndex +1 );
	if(next_rec == NULL || rec == NULL)
		return;

	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		CAbsStopDlg dlg;
		int dlt = (next_rec->arrive-rec->depart)/60;
		if(dlg.DoModal() == IDOK)
		{
			if(dlg.m_abs_stop_time-2 > dlt)
			{
				dlt -= 2;
				CString text;
				text.Format("��ע��, ������Ļ���ͣ��ʱ�ִ�����������ʱ,\n ����ͣ��ʱ���޸�Ϊ%d", dlt);
				if(IDNO == MessageBox(text, "��ʾ", MB_YESNO))
				{
					return;
				}
				rec->stop_abs_s_time = 1;
				rec->stop_abs_e_time = rec->stop_abs_s_time + dlt;
			}
			else
			{
				rec->stop_abs_s_time = 2;
				rec->stop_abs_e_time = rec->stop_abs_s_time + dlg.m_abs_stop_time;
			}
			rec->szAbsText = dlg.m_reason;
		}
		else
		{
			return;
		}
	}
	else
	{
		rec->stop_abs_s_time = 3;
		rec->stop_abs_e_time = 8;
	}

	ReSetAbsDraw(rec->stop_reason);
	if(IsFlagStopSignal(rec->flag))
		return;

	CancelFlagStopAbs(rec->flag);

	SetFlagStopSignal(rec->flag);
	if(m_pDoc->GetStationVPos(m_sFocusTrain,rec,PRIMARY_TRACK,EXIT_POSITION) > m_pDoc->GetStationVPos(m_sFocusTrain,next_rec,PRIMARY_TRACK,ENTRY_POSITION))
		SetFlagStopAbsUp(rec->flag);
	else
		SetFlagStopAbsDown(rec->flag);

	if(m_pctrl->mConfigTg.m_context.IsInputAbsStopInfo())
	{
		m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, SET_ABS_TOP, rec->station, rec->rec_index);
	}
	else
	{
		m_pDoc->SendUpdateTrain(m_sFocusTrain);
	}
}

// �޸�վ��
void CTGForm::OnModifyTrainIndex()
{
	if(m_sFocusTrain == NULL)	
		return;
	
	CModifyStationIndex* dlg = new CModifyStationIndex(m_pDoc, m_sFocusTrain, this);

	if(NULL != dlg)
	{
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
				
		dlg->Create(IDD_DLG_MODIFY_TRAIN_INOUT_INDEX, this);
		dlg->ShowWindow(SW_SHOW);
	}

	return;
}

// �г���ϸ��Ϣ
void CTGForm::OnTrainData()
{
	if(NULL != m_sFocusTrain)
	{
		CTrainDataDlg dlg(m_sFocusTrain, m_pDoc, m_pDoc->GetTgType(),this);
		dlg.DoModal ();
	}
}

CString GetReturnTrainid(CString strTrainId)
{
	CString tempTrainId;
	CString temp;

	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(lastchar < '0' || lastchar > '9')
			break;
	}
	tempTrainId = strTrainId.Right(len - i - 1);
	int TrainIdNum = atoi(tempTrainId);
	tempTrainId = strTrainId.Left(len - tempTrainId.GetLength());
	if((TrainIdNum%2) == 0)
	{
		if(TrainIdNum > 0)
			temp.Format("%d", TrainIdNum-1);
		else
			temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}
	else
	{
		temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}

	return tempTrainId;
}



void CTGForm::OnDrawAbsWorkTrain(UINT nID)
{
	if(m_sFocusTrain == NULL)	
		return;

	CTG_TRAIN modifyTrain = *m_sFocusTrain;
	if(modifyTrain.GetRecordNum()<2)	
		return;

	int nFlag = (nID-ID_ABS_TRAIN_BEGIN);
	if(nFlag==0) // ǰ��
	{
		int nThisExit = GetThisExitByQuestion(modifyTrain);
		if(nThisExit==0)
		{
			OnAddtrainCancel();
			return;
		}
		
		TgTRAIN_RECORD *prec = modifyTrain.GetRecordByRecIndex(modifyTrain.GetRecordNum()-2);
		TgTRAIN_RECORD *last = modifyTrain.GetLastRecord();
		if (prec == NULL || last== NULL) 
			return;

		WORD line_no=0;
		int nNextStation = m_pDoc->GetAbsLineOtherStation(prec->station, nThisExit, line_no);
		int nNextEntry   = m_pDoc->ConvertAbsnumStationToEntry(nNextStation, line_no);
		if(nNextEntry==0)
		{
			OnAddtrainCancel();
			return;
		}
		if(prec->out_station_index>last->in_station_index)
			SetFlagStopAbsUp(prec->flag); 
		else
			SetFlagStopAbsDown(prec->flag);	

		SetFlagStopAbs(prec->flag); 
		prec->stop_abs_s_time = 3;
		prec->stop_abs_e_time = 57;

		prec->exit = nThisExit;

		last->flag = 0;
		last->arrive=prec->depart+3600;
		last->depart=last->arrive;

		last->station = nNextStation;
		last->entry   = nNextEntry;
		last->exit    = 0;
		
		SetFlagTerminal(last->flag); 
	}
	if(nFlag==1) // �۷�
	{
		int nThisExit = GetThisExitByQuestion(modifyTrain);
		if(nThisExit==0)
		{
			OnAddtrainCancel();
			return;
		}

		TgTRAIN_RECORD *prec = modifyTrain.GetRecordByRecIndex(modifyTrain.GetRecordNum()-2);
		TgTRAIN_RECORD *last = modifyTrain.GetLastRecord();
		if (prec == NULL || last== NULL) 
			return;

		if(prec->out_station_index>last->in_station_index)
			SetFlagStopAbsUp(prec->flag); 
		else
			SetFlagStopAbsDown(prec->flag);	

		CString returnTrainId=m_strArrivalTrainId;
		if(returnTrainId.IsEmpty())
			returnTrainId=GetReturnTrainid(prec->arrive_train_id);
		SetFlagStopAbs(prec->flag); 
		prec->stop_abs_s_time = 3;
		prec->stop_abs_e_time = 57;

		prec->exit = nThisExit;
				
		last->arrive_train_id=returnTrainId;
		last->depart_train_id=returnTrainId;
		last->station=prec->station;
		last->flag = 0;

		last->entry=nThisExit;
		last->exit=0;
		
		last->arrive=prec->depart+3600;
		last->depart=last->arrive;
		SetFlagTerminal(last->flag); 
		m_strArrivalTrainId="";
	}
	if(nFlag==2)  // ������
	{
		int nThisExit = GetThisExitByQuestion(modifyTrain);
		if(nThisExit==0)
		{
			OnAddtrainCancel();
			return;
		}

		TgTRAIN_RECORD *prec = modifyTrain.GetRecordByRecIndex(modifyTrain.GetRecordNum()-2);
		TgTRAIN_RECORD *last = modifyTrain.GetLastRecord();
		if (prec == NULL || last== NULL) 
			return;

		bool bUp=false;
		if(prec->out_station_index>last->in_station_index)
		{
			bUp=true;
		}
		else
		{
			bUp=false;
		}
		modifyTrain.DeleteTailRecord();

        prec = modifyTrain.GetLastRecord();
		if(bUp)
			SetFlagStopAbsUp(prec->flag);
		else
			SetFlagStopAbsDown(prec->flag);
		SetFlagStopAbs(prec->flag); 
		prec->stop_abs_s_time = 10;
		prec->stop_abs_e_time = 10;
		prec->exit = nThisExit;
		SetFlagAbsTerminal(prec->flag);		
	}
	if(nFlag==3)  // ���俪��
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		TgTRAIN_RECORD *prec = modifyTrain.GetRecordByRecIndex(1);
		TgTRAIN_RECORD *last = modifyTrain.GetLastRecord();
		if (first == NULL || prec == NULL || last== NULL) 
			return;
        
		prec->flag=0;
		modifyTrain.myTrainInfo.adjust_flag=0;
		SetTrainFlag(modifyTrain.myTrainInfo.adjust_flag);
		if(first->out_station_index>prec->in_station_index)
		{
			SetFlagStopAbsDown(prec->flag);
			SetAbsStartStopDown(modifyTrain.myTrainInfo.adjust_flag);
		}
		else
		{
			SetFlagStopAbsUp(prec->flag);
			SetAbsStartStopUp(modifyTrain.myTrainInfo.adjust_flag);
		}
		
		prec->stop_abs_s_time = 0;
		prec->stop_abs_e_time = 0;
		modifyTrain.myTrainInfo.stop_abs_time=10;
		SetFlagAbsStart(prec->flag);  

		modifyTrain.DeleteHeadRecord();
		last = modifyTrain.GetLastRecord();
		if (last)
		{
			SetFlagTerminal(last->flag);
		}
	}
	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
	
	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;
		m_sFocusTrain=NULL;
		m_pDoc->SendAppendTrain(&modifyTrain, "", 0);
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}
	m_pDoc->ResetContinueDraw();

	return;
}

TgTRAIN_RECORD* CTGForm::GetFoucsTrainRecord()
{
	if((m_sFocusTrain==NULL) || (m_nFocusRecordIndex<0))	
		return NULL;
	
	return m_sFocusTrain->GetRecordByRecIndex(m_nFocusRecordIndex);
}

void CTGForm::OnAutoAjustTrain(UINT nID)
{
	if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableAutoAdjust())
		return;
	
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += (nID-ID_ADJUST_BEGIN+1);

	m_pDoc->m_ptgctrl->TG_NOTICE(3, nFlag, m_sFocusTrain->m_nTrainIndex, rec->station);
}

void CTGForm::OnCancelWork()
{
	if(m_nFocusFigure == NULL)
		return;
	m_pDoc->m_ptgctrl->TG_NOTICE(1, 0x00000004, m_nFocusFigure->mark_index, 0);
}

// ���ҹ�����������
void CTGForm::OnFindDDML()
{
	if(m_nFocusFigure == NULL)
		return;
	m_pDoc->m_ptgctrl->TG_NOTICE(1, 0x00000001, m_nFocusFigure->mark_index, 0);
}

// ��ҵ��Ϣ
void CTGForm::OnMarkWorkInfo()
{
	if(m_nFocusFigure == NULL)
		return;
	m_pDoc->m_ptgctrl->TG_NOTICE(1, 0x00000002, m_nFocusFigure->mark_index, 0);
}

void CTGForm::OnBlockAdjustTrain()
{
	if(m_nFocusFigure == NULL)
		return;

	if(m_nFocusFigure->type == ABS_INHIBIT_MARK ||
		m_nFocusFigure->type == ABS_FIX_MARK ||
		m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK ||
		m_nFocusFigure->type == ABS_ELE_FIX_MARK)
	{
		m_pDoc->sysprint(4,0,"[TG]: ����Աѡ������ʩ������ %d, %s, ����������", m_nFocusFigure->mark_index, m_nFocusFigure->text);
		m_pDoc->m_ptgctrl->TG_NOTICE(1, 0x00000008, m_nFocusFigure->mark_index, 1);
	}

	if(m_nFocusFigure->type == SIDE_INHIBIT_MARK ||
		m_nFocusFigure->type == SIDE_OCCUPY_MARK ||
		m_nFocusFigure->type == SIDE_TIANCHUANG_MARK)
	{
		m_pDoc->sysprint(4,0,"[TG]: ����Աѡ��վ��ʩ������ %d, %s, ����������", m_nFocusFigure->mark_index, m_nFocusFigure->text);
		m_pDoc->m_ptgctrl->TG_NOTICE(1, 0x00000008, m_nFocusFigure->mark_index, 0);
	}
}

// ������
void CTGForm::OnMutiTrainRecStayToPass()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, MUTI_REC_STAY_TO_PASS, rec->station, rec->rec_index);
}

// ��ѯȷ��
void CTGForm::OnQueryQuebao()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x1;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

// �����·
void CTGForm::OnChangeRoute()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x4;

	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}
void CTGForm::OnManageShuaigua()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(NULL == rec) 
		return;		

	CDlgShuaiGuaManage dlg(m_pDoc, m_sFocusTrain, this);
	dlg.sta_name = m_pDoc->GetStationName(rec->station); //added by zhaoliang for shuaigua manage 
	if(dlg.DoModal() == IDOK)
	{
		m_pDoc->SendUpdateTrain(dlg.mTrain);
		CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(dlg.mTrain.m_nTrainIndex);
		if(NULL == pTrain)
		{
			MessageBox("��ע��: �ó������Ѿ�ɾ��, �޸�˦����Ϣʧ��", "����");
			return;
		}
		
		for(int i=0; i<pTrain->GetRecordNum(); i++)
		{
			pTrain->myRecord[i].szStationText = dlg.mTrain.myRecord[i].szStationText;
			pTrain->myRecord[i].opmsGiveStationText = dlg.mTrain.myRecord[i].opmsGiveStationText;
		}
		
		m_pDoc->SendUpdateTrain(pTrain);
		m_pDoc->sysprint(4,0,"[TG]: ����Ա%s���г� �޸�˦��'", m_pDoc->GetTrainShowId(m_sFocusTrain));
	}
}

// ��ѡ����/������Ϣ--���������� 2014-09-11
void CTGForm::OnTrainEngineInfo()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag &=0xFF000000;
	nFlag += 0x40;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

// �����ѯ
void CTGForm::OnTrainBianzuInfo()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(NULL == rec)
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x20;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

// �г��ٱ�
void CTGForm::OnTrainSubaoInfo()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(NULL == rec)
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x10;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

// ��ҵ��Ϣ
void CTGForm::OnTrainWorkInfo()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(NULL == rec)
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x2;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

// �г�����
void CTGForm::OnCannotMeet()
{
	if(m_sFocusTrain == NULL)	
		return;

	CProhibitMeetDlg* m_pProhibitMeetDlg = new CProhibitMeetDlg(this);
	if(NULL != m_pProhibitMeetDlg)
	{
		m_pProhibitMeetDlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		m_pProhibitMeetDlg->m_pDoc = this->m_pDoc;

		m_pProhibitMeetDlg->Create(IDD_DLG_PROHIBIT_MEET, this);
		m_pProhibitMeetDlg->ShowWindow(SW_SHOW);
	}
}

// ������Ϣ
void CTGForm::OnTransportInfo() 
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

		CTransportInfo dlgTransportInfo(this);
	
		dlgTransportInfo.m_pDoc = m_pDoc;
		dlgTransportInfo.m_bEdit = m_pDoc->IsCanEditTrain();
		
		dlgTransportInfo.m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlgTransportInfo.m_rec_index = m_nFocusRecordIndex;
		
		dlgTransportInfo.m_strStation = rec->szStationText;
		dlgTransportInfo.m_strAbs = rec->szAbsText;
		dlgTransportInfo.m_strOpmsStationText = rec->szOpmsStationText;
		dlgTransportInfo.m_opmsSetStationText = rec->opmsGiveStationText;
		
		dlgTransportInfo.DoModal();
}

void CTGForm::OnDirectSetTrigeRoute()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x1;

	m_pDoc->m_ptgctrl->TG_NOTICE(4, nFlag, m_sFocusTrain->m_nTrainIndex, rec->station);
}

// ��ʾ����ͼ������
void CTGForm::OnShowBasicPlanInfo()
{
	if(m_sFocusTrain == NULL)	
		return;
	
	FlashTrain();
	
	int nFocusTrainIndex = m_sFocusTrain->GetTrainIndex();
	CTG_TRAIN* tmp = myPDataManage->GetTrain(nFocusTrainIndex);
	if(tmp)
	{
		m_trainbasicflash = *tmp;
		m_trainbasicflash.InitBasicTM();

		//1. ����train������
		m_pDoc->SetTrainDirect(m_trainbasicflash);
		m_pDoc->SetDirectAtStation(&m_trainbasicflash);
		m_pDoc->SetTrainRect(&m_trainbasicflash);
		m_pDoc->SetTimeValid(&m_trainbasicflash);
		m_pDoc->SetTrainSideAndSchdReal(m_trainbasicflash);
		m_pDoc->SetTrainWidthColor(&m_trainbasicflash);
		m_trainbasicflash.SetTrainType();
		m_trainbasicflash.SetEleLocoTrain();

		DrawTrain(m_trainbasicflash, false);
		SetTimer(TIMER_IDEVENT_FLASH,500,0);
	}
}

// �޸ļƻ��߲���
void CTGForm::OnModifyPlanPara() 
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(rec == NULL)	
		return;
	   
	CTrainPlanParaDlg* m_pTrainPlanParaDlg = new CTrainPlanParaDlg(this);
	if(NULL != m_pTrainPlanParaDlg)
	{
		m_pTrainPlanParaDlg->mTgType = m_pDoc->GetTgType(); 
		m_pTrainPlanParaDlg->m_pDoc = m_pDoc;
		
		m_pTrainPlanParaDlg->m_train_index= m_sFocusTrain->m_nTrainIndex;
		m_pTrainPlanParaDlg->m_rec_index = m_nFocusRecordIndex; 
		TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByRecIndex(m_nFocusRecordIndex);
		if(rec!=NULL)
		{
			m_pTrainPlanParaDlg->m_foucs_station=rec->station;
			m_pTrainPlanParaDlg->Create(IDD_DLG_PLAN_PARA, this);
			m_pTrainPlanParaDlg->ShowWindow(SW_SHOW);
		}
	}
}

bool CTGForm::FocusAfterActualPoint(UINT nFocusRecIndex, CTG_TRAIN& train)
{
	bool bFoucsAndAfterHaveActuralTimePoint = false; 
	TgTRAIN_RECORD *rec = train.GetRecordByPosition(nFocusRecIndex);
	if(rec == NULL)
		return false;

	if(m_nMouseDownPosition == ARRIVAL_POSITION)
	{
		bFoucsAndAfterHaveActuralTimePoint = IsActualDepartTime(rec->adjust_status);	    
	}
	else if(m_nMouseDownPosition == DEPART_POSITION || m_nMouseDownPosition == UNION_POSITION)
	{
		TgTRAIN_RECORD *next = train.GetRecordByPosition(nFocusRecIndex+1);
		if(next) 
		{
			bFoucsAndAfterHaveActuralTimePoint = IsActualArrivalTime(next->adjust_status);	
		}
	}
	else
	{
		bFoucsAndAfterHaveActuralTimePoint = IsActualArrivalTime(rec->adjust_status) | IsActualDepartTime(rec->adjust_status);
	}
	if(bFoucsAndAfterHaveActuralTimePoint) 
		return true; 

	return false;
}

void CTGForm::OnStayToPass() 
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(rec==NULL) 
		return;

	if(rec->arrive != rec->depart) 
	{
		ChangeStationStayOrPass();
	}
}

void CTGForm::OnPassToStay() 
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if(rec==NULL) 
		return;

	if(rec->arrive == rec->depart) 
	{
		ChangeStationStayOrPass();
	}
}

void CTGForm::ChangeStationStayOrPass()
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT)
		|| !(m_pDoc->GetOperatorPriorByDistrict(m_pDoc->nCurrentMouseX,m_pDoc->focus_point)))
	{
		return;
	}

	if(m_sFocusTrain == NULL)
		return;

	int nNum = m_sFocusTrain->GetRecordNum();
	int nCurrent = m_nFocusRecordIndex;

	if(nNum < 1 || nCurrent < 0)
		return;

	TgTRAIN_RECORD *rec=NULL;

	if(nCurrent == 0)
	{
		rec = m_sFocusTrain->GetFirstRecord();
		if(rec==NULL) 
			return;
		if(IsFlagStart(rec->flag))
		{
			return;
		}
	}

	if(nCurrent == nNum-1)
	{
		rec = m_sFocusTrain->GetLastRecord();
		if(rec==NULL) 
			return;
		if(IsFlagTerminal(rec->flag) && !IsFlagStopAbs(rec->flag))
		{
			return;
		}
	}

	rec = GetFoucsTrainRecord();
	if(rec==NULL) 
		return;

	// �Ƿ�����޸�ʵ�ʵ�
	if(!m_pDoc->m_bEditRealLine && (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status)))
		return;

	if(this->m_pctrl->mConfigTg.IsNotThisSectionStation(rec->station))
	{
		CString str;
		str.Format("��ע��, %s �����ڱ�̨��Ͻ,�����޸ĵ�����ʽ",  m_pDoc->GetStationName(rec->station));
		MessageBox(str, "��ʾ", MB_OK);
		m_pDoc->sysprint(4,0, "%s", str);
		return;
	}

	// ͨ���䵽��
	if(rec->arrive == rec->depart ) 
	{		
		CString strText="";
		if (IsArriveRouteDoing(*rec))
		{
			strText.Format("�г�%s��%sվ�����Ž�·�������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}
		
		else if (IsArriveRouteOk(*rec) || IsDepartRouteOk(*rec))
		{
			strText.Format("�г�%s��%sվ��·���ų��������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}

		else if (IsArriveRouteOccupy(*rec))
		{
			strText.Format("�г�%s��%sվ��·ռ���У������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}

		else if (TG_IsTrainIntoSide(*rec))
		{
			strText.Format("�г�%s �ѽ��� %sվ�������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}

		//�ͳ���ͨ����>������Ҫ �˹�ȷ��
		else if (m_sFocusTrain->m_nTrainType == TYPE::PASSENGER)	
		{
			strText.Format("�ͳ�%s��%sվΪ'ͨ��',ȷʵҪ���'����'��?", m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}

		if (strText != "")
		{
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsPassToStayNeedInfoWnd())
			{
				CDlgMessageBox dlg(strText);
				if(dlg.DoModal() != IDOK)
					return ; 
			}
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsNoTechPassToStayNeedPwd())
			{
				//��Ӫҵվ�ģ������������ܸ���
				if( !IsTechStopTrain(*rec))
				{
					CStringArray trainidary; 
					m_pDoc->GetTrainIds(trainidary, m_sFocusTrain);
					CString strPwd = this->m_pctrl->mConfigTg.get_train_sta_pwd(trainidary, rec->station);

					CSuperPassDlg pdlg;
					if (pdlg.DoModal() == IDOK)
					{
						if ( pdlg.m_super_pass != strPwd)
						{
							CString str;
							str.Format("%s","�������!");
							CDlgMessageBox dlg(str);
							dlg.DoModal();
							return;
						}
					}
					else
						return;
				}
			}
		}

		m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, PASS_TO_STAY, rec->station, m_nFocusRecordIndex);
		m_pDoc->sysprint(4,0,"[TG]: ����Ա%s�� �г� %sվ'ͨ��'��'����'", m_pDoc->GetTrainShowId(m_sFocusTrain),m_pDoc->GetStationName(rec->station));
		//m_sFocusTrain = NULL;
	}
	else // ������ͨ��
	{
		CString strText="";
		if (IsArriveRouteDoing(*rec))
		{
			strText.Format("�г�%s��%sվ�����Ž�·�������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}
		else if (IsArriveRouteOk(*rec) || IsDepartRouteOk(*rec))
		{
			strText.Format("�г�%s��%sվ��·���ų��������ɵ�����", m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}
		else if (IsArriveRouteOccupy(*rec))
		{
			strText.Format("�г�%s��%sվ��·ռ���У������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}
		else if (TG_IsTrainIntoSide(*rec))
		{
			strText.Format("�г�%s �ѽ��� %sվ�������ɵ�����",  m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station)); 
		}
		//�ͳ��ĵ�����ͨ����Ҫ �˹�ȷ��
		else if(m_sFocusTrain->m_nTrainType == TYPE::PASSENGER)	
		{
			strText.Format("�ͳ�%s��%sվ��'ͣ��',ȷʵҪ���'ͨ��'��", m_pDoc->GetTrainShowId(m_sFocusTrain),m_pDoc->GetStationName(rec->station)); 
		}
		else if (IsMustNoPass(*rec))
		{
			strText.Format("�г�%s��%sվ�мƻ��������'��ֹͨ��',ȷʵҪ���'ͨ��'��", m_pDoc->GetTrainShowId(m_sFocusTrain),m_pDoc->GetStationName(rec->station)); 
		}
		else
		{
			if(m_pDoc->IsGoodTrainNotTechStopNeedPwd())
			{
				if(!IsTechStopTrain(*rec) || rec->min_stop_time>0)
				{
					strText.Format("�г�%s��%sվ��'Ӫҵ��',ȷʵҪ���'ͨ��'��", m_pDoc->GetTrainShowId(m_sFocusTrain),m_pDoc->GetStationName(rec->station)); 
				}
			}
		}


		if (strText != "")
		{
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsStayToPassNeedInfoWnd())
			{ 
				CDlgMessageBox dlg(strText);
				if(dlg.DoModal() != IDOK)
					return ; 
			}

			//��Ӫҵվ�ģ������������ܸ���
			bool bNeedPwd = false;
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsStayToPassNeedPwd()) // ̫ԭ����Ҫ���ô���
				bNeedPwd=true;
			else if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsNoTechStayToPassNeedPwd())
			{
				if(!IsTechStopTrain(*rec) || IsMustNoPass(*rec))  // �Ǽ���ͣ�㣬���Ϻ���Ҫ���޸ģ�zhoujb��2008-05-05
					bNeedPwd=true;
			}

			if(bNeedPwd)
			{
				CStringArray trainidary; 
				m_pDoc->GetTrainIds(trainidary, m_sFocusTrain);
				CString strPwd = this->m_pctrl->mConfigTg.get_train_sta_pwd(trainidary, rec->station);

				CSuperPassDlg pdlg;
				if (pdlg.DoModal() == IDOK)
				{
					if ( pdlg.m_super_pass != strPwd)
					{
						CString str;
						str.Format("%s","�������!");
						CDlgMessageBox dlg(str);
						dlg.DoModal();
						return;
					}
				}
				else
					return;
			}
		}

		// �����������ʷ��,����������ͨ�� ̫ԭ
		if(TRUE==m_pDoc->m_nAutoMove)
		{
			;//������
		}
		else
		{
			if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
			{
				int nMinute = m_pDoc->GetOffsetMinuteFromGraphBegin(rec->arrive);
				if(nMinute < 720)
				{
					m_pDoc->sysprint(4,0,"��һ��: �г� %s �ڳ�վ %s ���� %d ����ʷ��,����������ͨ��", m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station),rec->arrive);
					return; 
				}
			}
		}
		
		if(IsBeforePressure(rec->stop_condition))
		{
			m_pDoc->sysprint(4,0,"[TG]: %s���г� %sվ'����'��'ͨ��', ���ѹǰ��־" , m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));
			ClearBeforePressure(rec->stop_condition);
		}
		if(IsAfterPressure(rec->stop_condition))
		{
			m_pDoc->sysprint(4,0,"[TG]: %s���г� %sվ'����'��'ͨ��', ���ѹ���־" , m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));
			ClearAfterPressure(rec->stop_condition);
		}
		m_pDoc->SendUpdateTrain(m_sFocusTrain, SCHD_UPDATE, STAY_TO_PASS, rec->station, m_nFocusRecordIndex);
		m_pDoc->sysprint(4,0,"[TG]: ����Ա%s�� �г� %sվ'����'��'ͨ��'" , m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));
		//m_sFocusTrain = NULL;
	}
}

// �������ж� --- �޸ĺ������� ԭ��������IsOnlyOneArrivalDirection
int CTGForm::GetStaStopAcceptEntry(CTG_TRAIN* train, std::vector<int>& line_vector, int& nEntry, int recindex)
{
	line_vector.clear();
	nEntry = 0;
	do
	{
		if(train == NULL) 
			break;

		int nNumRd = train->GetRecordNum();
		if(nNumRd<1)
			break;

		TgTRAIN_RECORD *rec = NULL, *next = NULL;
		if(recindex<nNumRd)
			rec = train->GetRecordByPosition(recindex);
		if(recindex+1<nNumRd)
			next = train->GetRecordByPosition(recindex+1);

		if(rec == NULL) 
			break;

		int nFocusTrainDirect = -1;
		if(next != NULL)
		{
			if(next->in_station_index > rec->out_station_index)
				nFocusTrainDirect = DOWN_DIRECT;
			else
				nFocusTrainDirect = UP_DIRECT;
		}

		if(nFocusTrainDirect == -1)
		{
			CString trainId = rec->depart_train_id;
			if(trainId.IsEmpty())
			{
				trainId = rec->arrive_train_id;
			}
		
			// 2012.01.12�г�����
			nFocusTrainDirect=m_pDoc->GetTrainDirectByTrainId(trainId);
			if(nFocusTrainDirect==-1)
			{
				if(IDYES == MessageBox("��ע��,ϵͳ�޷��ж�������г��ķ���,����ѡ��:\n���� ��'��' \n���а�'��' ", 
					"��ʾ", MB_YESNO|MB_ICONQUESTION))
					nFocusTrainDirect = UP_DIRECT;
				else
					nFocusTrainDirect = DOWN_DIRECT;
			}
		}

		// m_pDoc->m_nCurrentNodeIndex = rec->in_station_index; // 20131203 ���վ��ѡ����� ɾ��
		DispDistance *distance_node = m_pDoc->GetDispDistanceByIdx(m_pDoc->m_nCurrentNodeIndex);
		if(distance_node == NULL)
			break;

		std::vector<int> sel_line_no;
		if(nFocusTrainDirect == UP_DIRECT)
		{
			std::map<int, int> line_map;
			for(int n=m_pDoc->m_nCurrentNodeIndex+1; n<=m_pDoc->m_nDispDistanceNum; n++)
			{
				DispDistance *distance = m_pDoc->GetDispDistanceByIdx(n);
				if(distance == NULL)
					break;
				if(distance->sub_section != distance_node->sub_section)
					break;
				
				for(int k=0; k<distance->up_line_count; k++)
				{
					int line = distance->up_line_list[k];
					line_map.insert(make_pair(line, line));
				}
			}

			for(WORD i=0; i < distance_node->down_line_count; i++)
			{
				int line = distance_node->down_line_list[i];

				std::map<int, int>::iterator it = line_map.find(line);
				if(it == line_map.end())
				{
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}

			if(sel_line_no.empty())
			{
				for(WORD i=0; i < distance_node->down_line_count; i++)
				{
					int line = distance_node->down_line_list[i];
				
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}
		}
		else if(nFocusTrainDirect == DOWN_DIRECT)
		{
			std::map<int, int> line_map;
			for(int n=m_pDoc->m_nCurrentNodeIndex-1; n>=0; n--)
			{
				DispDistance *distance = m_pDoc->GetDispDistanceByIdx(n);
				if(distance == NULL)
					break;
				if(distance->sub_section != distance_node->sub_section)
					break;
				
				for(int k=0; k<distance->down_line_count; k++)
				{
					int line = distance->down_line_list[k];
					line_map.insert(make_pair(line, line));
				}
			}

			for(WORD i=0; i<distance_node->up_line_count; i++)
			{
				int line = distance_node->up_line_list[i];
				std::map<int, int>::iterator it = line_map.find(line);
				if(it == line_map.end())
				{
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}

			if(sel_line_no.empty())
			{
				for(WORD i=0; i < distance_node->up_line_count; i++)
				{
					int line = distance_node->up_line_list[i];

					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}
		}
		else
		{
			for(WORD i=0; i < distance_node->down_line_count; i++)
			{
				int line = distance_node->down_line_list[i];
				
				if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
				{
					sel_line_no.push_back(line);
				}
			}
			for(WORD i=0; i<distance_node->up_line_count; i++)
			{
				int line = distance_node->up_line_list[i];

				if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
				{
					sel_line_no.push_back(line);
				}
			}
		}

		//�ڱ�ѡ�ߺ��в��Һ����ߺ�
		for(WORD i = 0;i < sel_line_no.size(); i++)
		{		 
			long tmpLineNo = sel_line_no[i];
			const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(tmpLineNo);
			if(line == NULL)
				continue;

			if(next !=  NULL)
			{
				if ((line->nStartStation == rec->station && line->nEndStation == next->station)
					|| (line->nStartStation == next->station && line->nEndStation == rec->station))
				{
					continue;
				}
			}

			if((line->nDirect ==3) && (line->nStartStation == rec->station || line->nEndStation == rec->station))
			{
				int nTmpEntry = m_pDoc->ConvertAbsnumStationToEntry(rec->station,  tmpLineNo);
				if(nTmpEntry <= 0)
				{
					CString msg;
					msg.Format("�Ҳ�����վ%d��Ӧ�ߺ�%d�ĳ����,����ENTRY.cfg�ļ�", rec->station, tmpLineNo);
					m_pDoc->sysprint(5, 0, "%s", msg);
					//AfxMessageBox(msg, MB_ICONHAND);
				}
				else
				{
					CString msg;
					msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d ���ϳ�վ%d�Ľ�������", 
						line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
					m_pDoc->sysprint(5, 0, "%s", msg);
					line_vector.push_back(tmpLineNo);

					if (nEntry == 0) 
					{
						nEntry = nTmpEntry;
					}
				}
			}
			else if(line->nDirect == nFocusTrainDirect && (line->nEndStation == rec->station))
			{
				int nTmpEntry = m_pDoc->ConvertAbsnumStationToEntry(rec->station,  tmpLineNo);
				if(nTmpEntry <= 0)
				{
					CString msg;
					msg.Format("�Ҳ�����վ%d��Ӧ�ߺ�%d�ĳ����,����ENTRY.cfg�ļ�", rec->station, tmpLineNo);
					m_pDoc->sysprint(5, 0, "%s", msg);
					//AfxMessageBox(msg, MB_ICONHAND);
				}
				else
				{
					CString msg;
					msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d ���ϳ�վ%d�Ľ�������", 
						line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
					m_pDoc->sysprint(5, 0, "%s", msg);
					line_vector.push_back(tmpLineNo);

					if (nEntry == 0) 
					{
						nEntry = nTmpEntry;
					}
				}
			}
			else
			{
				CString msg;
				msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d �����ϳ�վ%d�Ľ�������", 
					line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
				m_pDoc->sysprint(5, 0, "%s", msg);
			}
		}

		sel_line_no.clear();

	}while(0);

	return (int)line_vector.size();
}

// ��г����ж� --- �޸ĺ������� ԭ��������IsOnlyOneGiveoutDirection
int CTGForm::GetStaStopGiveoutExit(CTG_TRAIN* train, std::vector<int>& line_vector, int& nExit, int recindex,bool& bSelAllLine)
{
	bSelAllLine=false;   // 2012.2.10 �Ϻ�����¤��
	line_vector.clear();
	nExit = 0;
	do
	{
		if(train == NULL) 
			break;

		TgTRAIN_RECORD *rec = NULL, *prev = NULL;
		if(recindex >= 0)
			rec = train->GetRecordByPosition(recindex);
		if(recindex >= 1)
			prev = train->GetRecordByPosition(recindex - 1);

		if(rec == NULL) 
			break;

		int nFocusTrainDirect = -1;
		if(prev!=NULL)
		{
			if(prev->out_station_index > rec->in_station_index)
				nFocusTrainDirect = UP_DIRECT;
			else
				nFocusTrainDirect = DOWN_DIRECT;
		}

		if(nFocusTrainDirect == -1)
		{
			CString trainId = rec->depart_train_id;
			if(trainId.IsEmpty())
			{
				trainId = rec->arrive_train_id;
			}

			// 2012.01.12�г�����
			nFocusTrainDirect=m_pDoc->GetTrainDirectByTrainId(trainId);
			if(nFocusTrainDirect==-1)
			{
				if(IDYES == MessageBox("��ע��,ϵͳ�޷��ж�������г��ķ���,����ѡ��:\n���� ��'��' \n���а�'��' ", 
					"��ʾ", MB_YESNO|MB_ICONQUESTION))
					nFocusTrainDirect = UP_DIRECT;
				else
					nFocusTrainDirect = DOWN_DIRECT;
			}
		}

		// m_pDoc->m_nCurrentNodeIndex = rec->out_station_index; // 20131203 ����վ��ѡ����� ɾ��
		if(m_pDoc->m_nCurrentNodeIndex<0 || m_pDoc->m_nCurrentNodeIndex >= m_pDoc->m_nDispDistanceNum)
			m_pDoc->m_nCurrentNodeIndex = rec->in_station_index;

		DispDistance *distance_node = m_pDoc->GetDispDistanceByIdx(m_pDoc->m_nCurrentNodeIndex);
		if(distance_node == NULL)
			break;

		std::vector<int> sel_line_no;

		if(nFocusTrainDirect == DOWN_DIRECT)
		{
			std::map<int, int> line_map;
			for(int n=m_pDoc->m_nCurrentNodeIndex+1; n<=m_pDoc->m_nDispDistanceNum; n++)
			{
				DispDistance *distance = m_pDoc->GetDispDistanceByIdx(n);
				if(distance == NULL)
					break;
				if(distance->sub_section != distance_node->sub_section)
					break;
				
				for(int k=0; k<distance->up_line_count; k++)
				{
					int line = distance->up_line_list[k];
					line_map.insert(make_pair(line, line));
				}
			}

			for(WORD i=0; i < distance_node->down_line_count; i++)
			{
				int line = distance_node->down_line_list[i];
				std::map<int, int>::iterator it = line_map.find(line);
				if(it == line_map.end())
				{
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}

			if(sel_line_no.empty())
			{
				bSelAllLine=true;
				for(WORD i=0; i < distance_node->down_line_count; i++)
				{
					int line = distance_node->down_line_list[i];
					
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}

				for(WORD i=0; i<distance_node->up_line_count; i++)
				{
					int line = distance_node->up_line_list[i];

					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}
		}
		else if(nFocusTrainDirect == UP_DIRECT)
		{
			std::map<int, int> line_map;
			for(int n=m_pDoc->m_nCurrentNodeIndex-1; n>=0; n--)
			{
				DispDistance *distance = m_pDoc->GetDispDistanceByIdx(n);
				if(distance == NULL)
					break;
				if(distance->sub_section != distance_node->sub_section)
					break;
				
				for(int k=0; k<distance->down_line_count; k++)
				{
					int line = distance->down_line_list[k];
					line_map.insert(make_pair(line, line));
				}
			}

			for(WORD i=0; i<distance_node->up_line_count; i++)
			{
				int line = distance_node->up_line_list[i];
				std::map<int, int>::iterator it = line_map.find(line);
				if(it == line_map.end())
				{
					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}

			if(sel_line_no.empty())
			{
				bSelAllLine=true;
				for(WORD i=0; i < distance_node->up_line_count; i++)
				{
					int line = distance_node->up_line_list[i];

					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}

				for(WORD i=0; i < distance_node->down_line_count; i++)
				{
					int line = distance_node->down_line_list[i];

					if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
					{
						sel_line_no.push_back(line);
					}
				}
			}
		}
		else
		{
			bSelAllLine=true;
			for(WORD i=0; i < distance_node->down_line_count; i++)
			{
				int line = distance_node->down_line_list[i];

				if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
				{
					sel_line_no.push_back(line);
				}
			}
			for(WORD i=0; i<distance_node->up_line_count; i++)
			{
				int line = distance_node->up_line_list[i];

				if(sel_line_no.end()==std::find(sel_line_no.begin(), sel_line_no.end(), line))
				{
					sel_line_no.push_back(line);
				}
			}
		}

		//�ڱ�ѡ�ߺ��в��Һ����ߺ�
		if(bSelAllLine)
		{
			line_vector.clear();
			for(WORD i = 0;i < sel_line_no.size(); i++)
			{		 
				line_vector.push_back(sel_line_no[i]);
			}
		}
		else
		{
			for(WORD i = 0;i < sel_line_no.size(); i++)
			{		 
				long tmpLineNo = sel_line_no[i];
				const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(tmpLineNo);
				if (line == NULL)
					continue;

				if  (prev !=  NULL)
				{
					if ((line->nStartStation == rec->station && line->nEndStation == prev->station)
						|| (line->nStartStation == prev->station && line->nEndStation == rec->station))
					{
						continue;
					}
				}

				if((line->nDirect ==3) && (line->nStartStation == rec->station || line->nEndStation == rec->station))
				{
					int nTmpExit = m_pDoc->ConvertAbsnumStationToEntry(rec->station,  tmpLineNo);
					if(nTmpExit <= 0)
					{
						CString msg;
						msg.Format("�Ҳ�����վ%d��Ӧ�ߺ�%d�ĳ����,����ENTRY.cfg�ļ�", rec->station, tmpLineNo);
						m_pDoc->sysprint(5, 0, "%s", msg);
						//AfxMessageBox(msg, MB_ICONHAND);
					}
					else
					{
						CString msg;
						msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d ���ϳ�վ%d�Ľ�������", 
							line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
						m_pDoc->sysprint(5, 0, "%s", msg);

						line_vector.push_back(tmpLineNo);
						if(nExit == 0)
						{
							nExit = nTmpExit;
						}
					}
				}
				else if(line->nDirect == nFocusTrainDirect && (line->nStartStation == rec->station  || line->nEndStation == rec->station))
				{
					int nTmpExit = m_pDoc->ConvertAbsnumStationToEntry(rec->station,  tmpLineNo);
					if(nTmpExit <= 0)
					{
						CString msg;
						msg.Format("�Ҳ�����վ%d��Ӧ�ߺ�%d�ĳ����,����ENTRY.cfg�ļ�", rec->station, tmpLineNo);
						m_pDoc->sysprint(5, 0, "%s", msg);
						//AfxMessageBox(msg, MB_ICONHAND);
					}
					else
					{
						CString msg;
						msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d ���ϳ�վ%d�Ľ�������", 
							line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
						m_pDoc->sysprint(5, 0, "%s", msg);

						line_vector.push_back(tmpLineNo);
						if(nExit == 0)
						{
							nExit = nTmpExit;
						}
					}
				}
				else
				{
					CString msg;
					msg.Format("�ߺ�%d: ��ʼ��վ%d ��ֹ��վ%d ���� %d, �г����� %d �����ϳ�վ%d�Ľ�������", 
						line->nLineNo, line->nStartStation, line->nEndStation, line->nDirect, nFocusTrainDirect, rec->station);
					m_pDoc->sysprint(5, 0, "%s", msg);
				}
			}
		}

		sel_line_no.clear();

	}while(0);

	return (int)line_vector.size();
}

void CTGForm::OnAddtrainAbsStop()
{
	if(m_pDoc->m_nOperatorStatus != INSERT_TRAIN &&
		m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_A &&
		m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_B )
		return;
	if(m_sFocusTrain == NULL || !m_stop_train_abs)
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
	
	TgTRAIN_RECORD *rec = NULL;
	rec = m_sFocusTrain->GetFirstRecord();
	if(rec == NULL) 
	{
		OnAddtrainCancel();
		return;
	}

	// �жϽ����
	SetTrainEntry(m_sFocusTrain, rec->entry);
	
	CTG_TRAIN modifyTrain = *m_sFocusTrain;

	rec = modifyTrain.GetLastRecord();
	if(rec == NULL)
		return;
	
	// �Զ�������κ�
	if(!m_strArrivalTrainId.IsEmpty())
	{
		AutoChangeTrainID(modifyTrain);
	}

	SetFlagAbsTerminal(rec->flag);
	SetFlagStopAbs(rec->flag);
	if(m_stop_train_abs_direct == UP_DIRECT)
		SetFlagStopAbsUp(rec->flag);
	else
		SetFlagStopAbsDown(rec->flag);

	// �����ж����������, ����ʾ�û�����ѡ��
	int nExit=0;
	std::vector<int> line_vector;
	int rs = GetAbsStopGiveoutExit(line_vector, m_stop_train_abs_direct, rec->rec_index, nExit);
	if(rs>1)
	{
		CDlgGiveoutDirectionSel dlg(m_pDoc, this, 3, line_vector, rec->station);
		if(dlg.DoModal() == IDOK)
		{
			rec->exit = dlg.nSelDirect;  
		}
	}
	else
	{
		if(nExit==0)
		{
			rec->exit = -1;
		}
		else
		{
			rec->exit =  nExit;
		}
	}

	rec->stop_abs_e_time=m_stop_train_abs_time;
	rec->stop_abs_s_time=m_stop_train_abs_time;

	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;

		m_pDoc->SendAppendTrain(&modifyTrain, m_strArrivalTrainId, m_nRunType);
		m_nRunType=0;
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}

	m_sFocusTrain = NULL;
	m_pDoc->ResetContinueDraw();
}

void CTGForm::OnAddtrainStop() 
{
	if(m_pDoc->m_nOperatorStatus != INSERT_TRAIN &&
		m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_A &&
		m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_B )
		return;
	if(m_sFocusTrain == NULL || m_stop_train_abs)
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	TgTRAIN_RECORD *rec = NULL;
	rec = m_sFocusTrain->GetFirstRecord();
	if(rec == NULL) 
	{
		OnAddtrainCancel();
		return;
	}

	// �жϽ����
	SetTrainEntry(m_sFocusTrain, rec->entry);

	CTG_TRAIN modifyTrain = *m_sFocusTrain;

	rec = modifyTrain.GetLastRecord();
	if(rec == NULL)
		return;

	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		if(IsFlagStart(first->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ�!", 
				m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(first->station));
			m_pDoc->sysprint(10,0, "%s", text);
			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			
			delete m_sFocusTrain;
			m_sFocusTrain = NULL;

			Invalidate(FALSE);
						
			m_pDoc->ResetContinueDraw();
			return;
		}
	}

	rec->stop_abs_e_time=0;
	rec->stop_abs_s_time=0;
	ClearFlagStopSignal(rec->flag);
	ClearFlagStopAbs(rec->flag);
	ClearFlagAbsTerminal(rec->flag);
	if(m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_A &&
		m_pDoc->m_nOperatorStatus != INSERT_TRAIN_ABS_B )
	{
		SetFlagTerminal(rec->flag);
	}
	
	WarningStplan(*rec);
	
	rec->depart = rec->arrive;
	rec->exit = -1;

	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_A ||
		m_pDoc->m_nOperatorStatus == INSERT_TRAIN_ABS_B )
	{
		rec->stop_abs_e_time = rec->stop_abs_s_time;
		SetFlagAbsTerminal(rec->flag); 
		SetFlagAbsStart(rec->flag); 
	}

	// �Զ�������κ�
	if(!m_strArrivalTrainId.IsEmpty())
	{
		AutoChangeTrainID(modifyTrain);
	}

	// ������·���Դ�
	if(IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�����յ��г�%s�� %s ������·�������Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeDepartRoute(rec->stop_condition);
	}

	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;

		m_pDoc->SendAppendTrain(&modifyTrain, m_strArrivalTrainId, m_nRunType);
		m_nRunType=0;
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}

	m_sFocusTrain = NULL;
	m_pDoc->ResetContinueDraw();
}
// G06
void CTGForm::OnModifytrainStart()
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;
	if(m_sFocusTrain == NULL) 
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	CTG_TRAIN modifyTrain = *m_sFocusTrain;
	TgTRAIN_RECORD *rec = NULL;
	rec = modifyTrain.GetFirstRecord();
	if(rec==NULL) 
		return;

	if(IsFlagAbsStart(rec->flag))
		return;

	if(IsFlagStart(rec->flag))
		return;

	rec->depart = rec->arrive;
	rec->entry = -1;
	if(IsFlagAbsStart(rec->flag))
	{
		ReSetFlagAbsStart(rec->flag);
		modifyTrain.myTrainInfo.stop_abs_time = 0;
		ReSetAbsStartStop(modifyTrain.myTrainInfo.adjust_flag);
	}
	SetFlagStart(rec->flag);

	// ͳ�Ʒ���
	rec->static_arrive=0;
	rec->static_depart=1;
	
	// �ӳ���·���Դ�
	if(IsAutoMakeArriveRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Աʼ��%s���г� %s �ӳ���·�������Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeArriveRoute(rec->stop_condition);
	}
	
	m_pDoc->sysprint(10,0, "��ݼ�F7 ����Ա�����г�%s��%dվʼ��", m_pDoc->GetTrainShowId(modifyTrain), rec->station);

	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		if(IsFlagTerminal(first->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ� \n�������޸� %s �� %s ʼ��ʧ��!", 
			m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(first->station));
			m_pDoc->sysprint(10,0, "��ݼ�F7 %s", text);

			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			return;
		}
	}
	m_pDoc->SendUpdateTrain(modifyTrain);    
}
// G06
void CTGForm::OnModifytrainAccept()
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;
	if(m_sFocusTrain == NULL) 
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	TgTRAIN_RECORD *rec = NULL;
	rec = modifyTrain.GetFirstRecord();
	if(rec == NULL)
		return;

	if(IsFlagAbsStart(rec->flag))
		return;

	if(IsFlagAccept(rec->flag))
		return;

	if(IsFlagAbsStart(rec->flag))
	{
		ReSetFlagAbsStart(rec->flag);
		modifyTrain.myTrainInfo.stop_abs_time = 0;
		ReSetAbsStartStop(modifyTrain.myTrainInfo.adjust_flag);
	}
	// �ӳ���·���Դ�
	if(!IsAutoMakeArriveRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա����%s���г� %s �ӳ���·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeArriveRoute(rec->stop_condition);
	}

	SetFlagAccept(rec->flag);
	SetTrainEntry(&modifyTrain, -1);

	m_pDoc->sysprint(10,0, "��ݼ�F7 ����Ա�����г�%s��%dվ����, entry=%d", m_pDoc->GetTrainShowId(modifyTrain), rec->station, rec->entry);
	m_pDoc->SendUpdateTrain(modifyTrain);   
}

// G06
void CTGForm::OnModifytrainDeliver()
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;
	if(m_sFocusTrain == NULL) 
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	CTG_TRAIN modifyTrain = *m_sFocusTrain;

	TgTRAIN_RECORD *rec = NULL;
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL) 
		return;

	if(IsFlagGiveout(rec->flag))
		return;

	if(IsFlagAbsTerminal(rec->flag))
		return;

	rec->stop_abs_e_time=0;
	rec->stop_abs_s_time=0;
	ClearFlagStopSignal(rec->flag);
	ClearFlagStopAbs(rec->flag);
	ClearFlagAbsTerminal(rec->flag);
	SetFlagGiveout(rec->flag);

	SetTrainExit(&modifyTrain, -1);
	
	// ������·�Դ�
	if(!IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա���� %s ���г� %s ������·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeDepartRoute(rec->stop_condition);
	}

	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;
		m_pDoc->SendAppendTrain(&modifyTrain, m_strArrivalTrainId, m_nRunType);
		m_nRunType=0;
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}   

	//m_sFocusTrain = NULL;
	m_pDoc->ResetContinueDraw();
}

// G06
void CTGForm::OnModifytrainTerminal()
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;
	if(m_sFocusTrain == NULL)
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	CTG_TRAIN modifyTrain = *m_sFocusTrain;

	TgTRAIN_RECORD *rec;
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL)
		return;

	if(IsFlagTerminal(rec->flag))
		return;

	if(IsFlagAbsTerminal(rec->flag))
		return;
	TgTRAIN_RECORD lastRec=*rec;

	rec->stop_abs_e_time=0;
	rec->stop_abs_s_time=0;
	ClearFlagStopSignal(rec->flag);
	ClearFlagStopAbs(rec->flag);
	ClearFlagAbsTerminal(rec->flag);
	SetFlagTerminal(rec->flag);
	
	rec->depart = rec->arrive;
	rec->exit = -1;

	// ������·���Դ�
	if(IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�յ� %s ���г� %s ������·�������Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeDepartRoute(rec->stop_condition);
	}
	m_pDoc->sysprint(10,0, "��ݼ�F8 ����Ա�����г�%s��%dվ�յ�", m_pDoc->GetTrainShowId(modifyTrain), rec->station);

	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		if(IsFlagStart(first->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ� \n�������޸� %s �� %s �յ�ʧ��!", 
			m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(first->station));
			m_pDoc->sysprint(10,0, "��ݼ�F8 %s", text);

			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			return;
		}
	}

	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;

		m_pDoc->SendAppendTrain(&modifyTrain, m_strArrivalTrainId, m_nRunType);
		m_nRunType=0;
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}

	//m_sFocusTrain = NULL;
	m_pDoc->ResetContinueDraw();

	this->WarningStplan(lastRec);
}

void CTGForm::OnAddtrainDeliver() 
{
	if(m_pDoc->m_nOperatorStatus != INSERT_TRAIN  )
		return;
	if(m_sFocusTrain == NULL) 
		return;

	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
	TgTRAIN_RECORD *rec = NULL;
	rec = m_sFocusTrain->GetFirstRecord();
	if(rec == NULL) 
	{
		OnAddtrainCancel();
		return;
	}

	// �жϽ����
	SetTrainEntry(m_sFocusTrain, rec->entry);

	CTG_TRAIN modifyTrain = *m_sFocusTrain;
	
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL) 
		return;

	SetFlagGiveout(rec->flag);

	TgTRAIN_RECORD *prev = NULL;
	prev = m_sFocusTrain->GetRecordByPosition(rec->rec_index - 1);

	int nFocusTrainDirect = -1;
	if(prev!=NULL)
	{
		if(prev->out_station_index > rec->in_station_index)
			nFocusTrainDirect = UP_DIRECT;
		else
			nFocusTrainDirect = DOWN_DIRECT;
	}

	if(nFocusTrainDirect == -1)
	{
		CString trainId = rec->depart_train_id;
		if(trainId.IsEmpty())
		{
			trainId = rec->arrive_train_id;
		}

		// 2012.01.12�г�����
		nFocusTrainDirect=m_pDoc->GetTrainDirectByTrainId(trainId);

		if(nFocusTrainDirect==-1)
		{
			if(IDYES == MessageBox("��ע��,ϵͳ�޷��ж�������г��ķ���,����ѡ��:\n���� ��'��' \n���а�'��' ", 
				"��ʾ", MB_YESNO|MB_ICONQUESTION))
				nFocusTrainDirect = UP_DIRECT;
			else
				nFocusTrainDirect = DOWN_DIRECT;
		}
	}
	modifyTrain.m_nDirectFlag = nFocusTrainDirect;

	SetTrainExit(&modifyTrain, -1);
	
	// �Զ�������κ�
	if(!m_strArrivalTrainId.IsEmpty())
	{
		AutoChangeTrainID(modifyTrain);
	}

	// ������·�Դ�
	if(!IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա���ƽ���%s���г� %s ������·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeDepartRoute(rec->stop_condition);
	}

	CTG_TRAIN* train = this->myPDataManage->GetTrain(modifyTrain.GetTrainIndex());
	if(train == NULL)
	{
		delete m_sFocusTrain;
		m_pDoc->SendAppendTrain(&modifyTrain, m_strArrivalTrainId, m_nRunType);
		m_nRunType=0;
		EraseTrain(modifyTrain);
	}
	else
	{
		m_pDoc->SendUpdateTrain(modifyTrain);
	}   

	m_sFocusTrain = NULL;
	m_pDoc->ResetContinueDraw();
}

void CTGForm::OnAddtrainCancel() 
{
	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
    m_pDoc->m_aryEngine.RemoveAll();
	m_pDoc->m_aryStatus.RemoveAll();

	int rec_index; long trainIndex;
	if(!m_pDoc->IsContinueDrawTrain(rec_index, trainIndex))
	{
		delete m_sFocusTrain;
		m_sFocusTrain = NULL;	
	}
	
	m_pDoc->ResetContinueDraw();

	if(m_sFocusTrain != NULL)
	{
		m_nFocusTrainIndex = m_sFocusTrain->GetTrainIndex();
		CTG_TRAIN* tmp = myPDataManage->GetTrain(m_nFocusTrainIndex);
		if(tmp)
		{
			m_pDoc->m_myFocusTrain = *tmp;
			m_sFocusTrain = &m_pDoc->m_myFocusTrain;
			RefreshTrain(m_sFocusTrain);
		}
		else
		{
			m_sFocusTrain = NULL;
		}
	}

	Invalidate(FALSE);
}

void CTGForm::OnTextNote() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{   
		//����ͼ��ע��
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = NEW_GRAPH_NOTE_MARK;
		m_nSubMarkType = GRAPH_NOTE_MARK;
	}
}

// ɾ���г�
void CTGForm::OnDeleteTrain() 
{
	//ͬʱ�ж��Ƿ�ɾ��ͼ��ע��,����DEL��
	if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		OnDeleteAbs();
	}

	if(m_sFocusTrain == NULL)
	{
		if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
		{
			if(this->m_SelectedTrains.size() > 0)
			{
				for(int idx = 0; idx < m_SelectedTrains.size(); idx++)
				{
					CTG_TRAIN* ptrain = this->myPDataManage->GetTrain(m_SelectedTrains[idx]);
					if(NULL != ptrain)
					{
						RefreshTrain(ptrain);
					}
				}
				m_pDoc->SendDeleteTrainXml(m_SelectedTrains);
				m_SelectedTrains.clear();
				return;
			}
		}
		return;
	}
	const TgTRAIN_MORE_INFO& train_info = m_sFocusTrain->myTrainInfo;
	if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))//������ɾ���հ�ƻ��г�
		;
	else 
		return;
	//�����ж�ɾ�����ɻ������һ����
	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN&&m_sFocusTrain!=NULL) 
	{
		CClientDC clientDC(this);
		OnPrepareDC(&clientDC);
		CRect clientRect;
		GetClientRect(clientRect);
		int nRecNum = m_sFocusTrain->GetRecordNum( );
		if(nRecNum > 0)
		{					
			TgTRAIN_RECORD* temprec = m_sFocusTrain->GetRecordByPosition (nRecNum -1);
			if(IsFlagStopAbs(temprec->flag))
			{
				CancelFlagStopAbs(temprec->flag);
			}
			else
			{
				m_sFocusTrain->DeleteTailRecord();
				if(nRecNum>1)
				{
					temprec = m_sFocusTrain->GetRecordByPosition (m_sFocusTrain->GetRecordNum( ) -1);
					if(temprec!=NULL)
					{
						if(IsFlagStopAbs(temprec->flag))
						{
							temprec->flag=0;
							SetFlagStopAbs(temprec->flag);
						}
						else if(IsFlagStopAbsUp(temprec->flag))
						{
							temprec->flag=0;
							SetFlagStopAbsUp(temprec->flag);
						}
						else if(IsFlagStopAbsDown(temprec->flag))
						{
							temprec->flag=0;
							SetFlagStopAbsDown(temprec->flag);
						}
						else
							temprec->flag=0;
					}
				}
				if(nRecNum==1)
				{
					OnAddtrainCancel();
					ReleaseCapture();
					Invalidate(FALSE);
					return;
				}
			}

			m_pDoc->DrawSingleTrain(&clientDC,m_sFocusTrain, DRAW_SCREEN);
			if(m_sFocusTrain->GetRecordNum () >0)
				m_pOldMousePoint = m_sFocusTrain->GetContinuePoint ();
			Invalidate(FALSE);
		}
		return;
	}
	
	//�������ж�ɾ���г�
	CString str;
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT)
		|| !(m_pDoc->GetOperatorPriorByDistrict(m_pDoc->nCurrentMouseX)))
	{
		return;
	}

	if(m_sFocusTrain == NULL)
		return;

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
	{
		if (m_sFocusTrain->IsExistActualTime())
		{
			AfxMessageBox("����ʵ�ʵ�,��ֹɾ���г�!");
			return;
		}
	}

	short zi;
	CDWordArray nDivideTrainIndex, nUniteTrainIndex;
	nDivideTrainIndex.RemoveAll();
	nUniteTrainIndex.RemoveAll();

	int nRecNum = m_sFocusTrain->GetRecordNum () ;
	if (nRecNum<1)	return; 
	TgTRAIN_RECORD *rec;
	CTG_TRAIN* pFirstDivideTrain, *pFirstUniteTrain;
	bool bIsDivideTrain = false;
	bool bIsUniteTrain = false;

	rec = m_sFocusTrain->GetRecordByPosition(nRecNum - 1);
	if (NULL == rec) return;

	if (!IsFlagDivide(rec->flag))
	{
		rec = m_sFocusTrain->GetRecordByPosition(0);
		if (NULL == rec) 
			return;
		if (IsFlagDivide(rec->flag))
		{
			bIsDivideTrain = true;
			nDivideTrainIndex.Add(m_sFocusTrain->GetTrainIndex());
			pFirstDivideTrain = m_pDoc->GetTrainByIndex(rec->special_train_index);
			if (NULL != pFirstDivideTrain)
			{
				nDivideTrainIndex.Add(pFirstDivideTrain->GetTrainIndex());
				for (zi=0; zi<pFirstDivideTrain->GetRecordNum(); zi++)
				{
					rec = pFirstDivideTrain->GetRecordByPosition(zi);
					if (IsFlagDivide(rec->flag))
					{
						if(zi == 0) 
						{
							nDivideTrainIndex.Add(rec->special_train_index);
						}
						else if (zi == pFirstDivideTrain->GetRecordNum()-1)
						{
							if (rec->special_train_index != m_sFocusTrain->GetTrainIndex())
							{
								nDivideTrainIndex.Add(rec->special_train_index);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if(IsFlagDivide(rec->flag))
		{
			bIsDivideTrain = true;
			nDivideTrainIndex.Add(m_sFocusTrain->GetTrainIndex());
			pFirstDivideTrain = m_pDoc->GetTrainByIndex(rec->special_train_index);
			if (NULL != pFirstDivideTrain)
			{
				nDivideTrainIndex.Add(pFirstDivideTrain->GetTrainIndex());
				rec = pFirstDivideTrain->GetRecordByPosition(0);
				if (rec != NULL)
				{
					if (m_sFocusTrain->GetTrainIndex() != rec->special_train_index)
					{
						nDivideTrainIndex.Add(rec->special_train_index);
					}						
				}
			}
		}
	}

	//���ѡ�е�������г������ж��Ƿ�ɾ�����и���ϱ�־�������г� added by zzg in ty 2008.05.12
	if (!bIsDivideTrain)
	{
		rec = m_sFocusTrain->GetRecordByPosition(nRecNum - 1);
		if (NULL == rec) return;
		if (!IsFlagEndInAbs(rec->flag))
		{
			rec = m_sFocusTrain->GetRecordByPosition(0);
			if (NULL == rec) 
				return;
			if (IsFlagEndInAbs(rec->flag))
			{
				bIsUniteTrain = true;
				nUniteTrainIndex.Add(m_sFocusTrain->GetTrainIndex());
				pFirstUniteTrain = m_pDoc->GetTrainByIndex(rec->special_train_index);
				if (NULL != pFirstUniteTrain)
				{
					nUniteTrainIndex.Add(pFirstUniteTrain->GetTrainIndex());
					rec = pFirstUniteTrain->GetRecordByPosition(pFirstUniteTrain->GetRecordNum()-1);
					if (rec != NULL)
					{
						if (m_sFocusTrain->GetTrainIndex() != rec->special_train_index)
						{
							nUniteTrainIndex.Add(rec->special_train_index);
						}						
					}


				}
			}
		}
		else
		{
			if(IsFlagEndInAbs(rec->flag))
			{
				bIsUniteTrain = true;
				nUniteTrainIndex.Add(m_sFocusTrain->GetTrainIndex());
				pFirstUniteTrain = m_pDoc->GetTrainByIndex(rec->special_train_index);
				if (NULL != pFirstUniteTrain)
				{
					nUniteTrainIndex.Add(pFirstUniteTrain->GetTrainIndex());
					for (zi=0; zi<pFirstUniteTrain->GetRecordNum(); zi++)
					{
						rec = pFirstUniteTrain->GetRecordByPosition(zi);
						if (NULL == rec) 
							return;

						if (IsFlagEndInAbs(rec->flag))
						{
							if(zi == 0) 
							{
								nUniteTrainIndex.Add(rec->special_train_index);
							}
							else if (zi == pFirstUniteTrain->GetRecordNum()-1)
							{
								if (rec->special_train_index != m_sFocusTrain->GetTrainIndex())
								{
									nUniteTrainIndex.Add(rec->special_train_index);
								}
							}
						}
					}
				}
			}
		}
	}

	CString strTrainId, strTrainIndex, strTmp;
	if (bIsDivideTrain)
	{
		for (zi=0; zi<nDivideTrainIndex.GetSize(); zi++) // wuxiang
		{
			CTG_TRAIN* train = m_pDoc->GetTrainByIndex(nDivideTrainIndex.GetAt(zi));
			if (NULL == train) continue;

			strTrainId += train->GetTrainId();
			strTmp.Format("%d", nDivideTrainIndex.GetAt(zi));
			strTrainIndex += strTmp;
			if(zi == nDivideTrainIndex.GetSize()-1) break;
			strTrainId += "��";
			strTrainIndex += "��";
		}
		str.Format("�Ƿ�ɾ�� %s ��(�ֽ�)�г�?", strTrainId);
	} 
	else if (bIsUniteTrain)
	{
		for (zi=0; zi<nUniteTrainIndex.GetSize(); zi++)
		{
			CTG_TRAIN* train = m_pDoc->GetTrainByIndex(nUniteTrainIndex.GetAt(zi));
			if (NULL == train) continue;
			
			strTrainId += train->GetTrainId();
			strTmp.Format("%d", nUniteTrainIndex.GetAt(zi));
			strTrainIndex += strTmp;
			if(zi == nUniteTrainIndex.GetSize()-1) break;
			strTrainId += "��";
			strTrainIndex += "��";
		}
		str.Format("�Ƿ�ɾ�� %s ��(���)�г�?", strTrainId);
	}

	if (strlen((LPTSTR)(LPCTSTR)strTrainId) < 1)
	{
		strTrainId.Format("%s", m_pDoc->GetTrainShowId(m_sFocusTrain));
		str.Format("�Ƿ�ɾ�� %s ���г�?", strTrainId);
	}	

	if(bIsDivideTrain)
	{
		for (int idx=0; idx < nDivideTrainIndex.GetSize(); idx++)
		{
			int train_key = nDivideTrainIndex.GetAt(idx);
			if (NULL == m_pDoc->GetTrainByIndex(train_key))
				continue; // wuxiang
			m_pDoc->SendDeleteTrainXml(train_key);
		}
		nDivideTrainIndex.RemoveAll();
	}
	else if (bIsUniteTrain)
	{
		for (int idx=0; idx<nUniteTrainIndex.GetSize(); idx++)
		{
			int train_key = nUniteTrainIndex.GetAt(idx);
			if (NULL == m_pDoc->GetTrainByIndex(train_key)) 
				continue; // wuxiang
			m_pDoc->SendDeleteTrainXml(train_key);
		}
		nUniteTrainIndex.RemoveAll();
	}
	else
	{
		m_pDoc->SendDeleteTrainXml(m_sFocusTrain->m_nTrainIndex);
	}

	//m_sFocusTrain = NULL;
}

// �޸ĳ��κ�
void CTGForm::OnChangeTrainid() 
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS || m_sFocusTrain == NULL)
		return;

	TgTRAIN_RECORD* rec = m_sFocusTrain->GetFirstRecord();
	if(rec == NULL)  
		return;

	CChangeTrainID *dlg = new CChangeTrainID(rec->arrive_train_id,"�޸ĳ���", this);
	if(dlg != NULL)
	{
		dlg->m_nFlag = 1;
		dlg->m_pDoc = m_pDoc;
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlg->DoModal();
		delete dlg;
	}
}

// ��ñ���κ�
void CTGForm::OnReplaceHattrainid() 
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS || m_sFocusTrain == NULL)
		return;

	TgTRAIN_RECORD* rec = m_sFocusTrain->GetFirstRecord();
	if(rec == NULL)  
		return;

	CString trainHatId = rec->arrive_train_id;
	if(trainHatId.IsEmpty())
	{
		trainHatId = rec->depart_train_id;
	}

	CString trainId = rec->original_arrive_train_id;
	if(trainId.IsEmpty())
		trainId = rec->original_depart_train_id;
	if(trainId.IsEmpty())
		trainId = trainHatId;

	CDlgChangeHatTrainId *dlg = new CDlgChangeHatTrainId(trainId, trainHatId, this);
	if(dlg != NULL)
	{
		dlg->m_pDoc  = m_pDoc;
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlg->DoModal();
		delete dlg;
	}
}

// �任�۷�����
void CTGForm::OnReplaceTrainid() 
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;

	TgTRAIN_RECORD* rec = GetFoucsTrainRecord();
	if(rec == NULL)  
		return;

	CChangeTrainID *dlg = new CChangeTrainID(rec->depart_train_id,"�޸��۷�����", this);
	if(dlg != NULL)
	{
		dlg->m_nFlag = 2;
		dlg->m_pDoc = m_pDoc;
		dlg->m_nSelRecIdx  = m_nFocusRecordIndex;
		dlg->m_nSelRectIdx  = m_nFocusRectIndex;
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlg->DoModal();
		delete dlg;
	}
}

void CTGForm::SetStatic(BYTE flag)
{
	if(m_sFocusTrain == NULL) 
		return;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(pTrain == NULL) 
		return;

	TgTRAIN_RECORD *rec = NULL;
	rec = pTrain->GetRecordByPosition(m_nFocusRecordIndex);	 
	if(rec == NULL) return;

	BYTE st_flag=0;
	if(flag == 1)
	{
		rec->static_arrive=1;
		rec->static_depart=0;
		rec->static_manual=1;

		st_flag|=0x01;
	}
	else if(flag == 2)
	{
		rec->static_arrive=0;
		rec->static_depart=1;
		rec->static_manual=1;

		st_flag|=0x02;
	}
	else if(flag == 3)
	{
		rec->static_arrive=1;
		rec->static_depart=1;
		rec->static_manual=1;

		st_flag|=0x01;
		st_flag|=0x02;
	}
	else
	{
		rec->static_arrive=0;
		rec->static_depart=0;
		rec->static_manual=0;

		st_flag|=0x04;
	}
	
	m_pDoc->m_myFocusTrain = *pTrain;
	m_sFocusTrain = &m_pDoc->m_myFocusTrain;

	CString trainid=rec->arrive_train_id;
	if(trainid=="")
		trainid=rec->depart_train_id;

	m_pDoc->SetLateEarlyTimeRd(m_sFocusTrain->m_nTrainIndex, rec->station, rec->rec_index, st_flag,trainid);

	// �Ż�����,����OnLButtonDown�������ػ�,����Ҫ�����ػ�
	EraseTrain(m_pDoc->m_myFocusTrain);
	DrawTrain(m_pDoc->m_myFocusTrain, false);
}

void CTGForm::OnStatisticArrival() 
{
	SetStatic(1);
}

void CTGForm::OnStatisticDepart() 
{
	SetStatic(2);
}

void CTGForm::OnStatisticBoth() 
{
	SetStatic(3);
}

void CTGForm::OnNoStatistic()
{
	SetStatic(0);
}

void CTGForm::OnStatisticWhole() 
{
	if(m_sFocusTrain == NULL) 
		return;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(pTrain == NULL) 
		return;

	int nRecNum = pTrain->GetRecordNum(); 

	if(pTrain->myTrainInfo.whole_static>0)
	{
		pTrain->myTrainInfo.whole_static=0;
		for(int i = 0; i< nRecNum; i++)
		{		
			TgTRAIN_RECORD*rec = pTrain->GetRecordByPosition(i);
			if(rec==NULL) continue;

			rec->static_arrive=0;
			rec->static_depart=0;
		}
		m_pDoc->InitEarlyLastTime(pTrain);
	}
	else
	{
		pTrain->myTrainInfo.whole_static=1;

		for(int i=0; i<nRecNum; i++)
		{
			TgTRAIN_RECORD* rec = pTrain->GetRecordByPosition(i);	 
			if(rec == NULL) 
				continue;

			if((0==i) && IsFlagStart(rec->flag))
			{
				rec->static_arrive=0;
				rec->static_depart=1;
			}
			else if((i == nRecNum-1) && IsFlagTerminal(rec->flag))
			{
				rec->static_arrive=1;
				rec->static_depart=0;
			}
			else
			{
				rec->static_arrive=1;
				rec->static_depart=1;
			}
		}
	}

	m_pDoc->m_myFocusTrain = *pTrain;
	m_sFocusTrain = &m_pDoc->m_myFocusTrain;

	// �Ż�����,����OnLButtonDown�������ػ�,����Ҫ�����ػ�
	EraseTrain(m_pDoc->m_myFocusTrain);
	DrawTrain(m_pDoc->m_myFocusTrain, false);
}

bool CTGForm::IsInhibitModifyRouteOkSchdTM()
{
	if(this->m_pctrl->mConfigTg.m_context.IsRouteOkInhibitModifySchd())
	{
		if(IsSHIFTpressed())
			return false;
		return true;
	}

	return false;
}

void CTGForm::OnChangeArrivalDepartTime(WORD nMinute)
{     
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex < 0)
		return;   

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	bool bActuralTimePoint = FocusAfterActualPoint(m_nFocusRecordIndex, modifyTrain);

	bool IsCanMoveWholeTrain = m_pDoc->m_ptgctrl->mConfigTg.IsCanMoveBasciTrainWhole();
	bool bRouteOkInhibitModifySchd=IsInhibitModifyRouteOkSchdTM();
	int nTgType = m_pDoc->GetTgType();

	TgTRAIN_RECORD *rec;
	rec = modifyTrain.GetRecordByPosition(m_nFocusRecordIndex);      
	if(rec == NULL)
		return;

	short offset = 0; 
	if(rec->arrive == rec->depart)
	{
		if(!m_pDoc->m_bEditRealLine && (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status)))
			return;

		if(nMinute == (rec->arrive/60) % 10)
			return;
		int arrival_min = (rec->arrive/60) % 10;
		if(arrival_min > 5 && nMinute == 0)
		{
			nMinute = 10;
		}

		offset = (nMinute - arrival_min)*60; 	 
		if(bActuralTimePoint)
		{
			rec->arrive = (rec->arrive/60 -(rec->arrive/60)%10)*60 + nMinute*60;
			rec->depart = rec->arrive;
		}
		else
		{
			if(nTgType == TG_TYPE::BASIC_TG)
				modifyTrain.OffsetBasicScheduleTime(m_nFocusRecordIndex, offset, IsCanMoveWholeTrain, 2);
			else
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(&modifyTrain, m_nFocusRecordIndex, offset, 3);
				else
					modifyTrain.OffsetWorkScheduleTime(m_nFocusRecordIndex, offset, 2);
			}
		}           		
	}
	else 
	{ 
		if(m_nMouseDownPosition == DEPART_POSITION)
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualDepartTime(rec->adjust_status)))
				return;
			int arrival_min = (rec->depart/60) % 10;
			if(arrival_min > 5 && nMinute == 0)
			{
				nMinute = 10;
			}

			offset = (nMinute - arrival_min)*60;	 	 
			long depart = (rec->depart/60 -(rec->depart/60)%10)*60 + nMinute*60;
			if(rec->arrive+60>depart)
				return;

			if(bActuralTimePoint)
			{
				rec->depart = depart;
				m_pDoc->sysprint(4,0,"����Աͨ�����ּ��޸��г� %s �� %s ����ʵ��ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}
			else
			{
				if(nTgType == TG_TYPE::BASIC_TG)
					modifyTrain.OffsetBasicScheduleTime(m_nFocusRecordIndex, offset, IsCanMoveWholeTrain, 1);
				else
				{
					if(bRouteOkInhibitModifySchd)
						m_pDoc->OffsetWorkScheduleTimeByRouteOk(&modifyTrain, m_nFocusRecordIndex, offset, 1);
					else
						modifyTrain.OffsetWorkScheduleTime(m_nFocusRecordIndex, offset, 1);
				}
				m_pDoc->sysprint(4,0,"����Աͨ�����ּ��޸��г� %s �� %s ƽ�Ƹ�վ����������ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}
		}
		else
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualArrivalTime(rec->adjust_status)))
				return;
			int arrival_min = (rec->arrive/60) % 10;
			if(arrival_min > 5 && nMinute == 0)
			{
				nMinute = 10;
			}
			offset = (nMinute - arrival_min)*60;
			
			if(bActuralTimePoint)
			{
				long arrive = (rec->arrive/60 -(rec->arrive/60)%10)*60 + nMinute*60;
				if(arrive+60>rec->depart)
					return;
				rec->arrive=arrive;
				m_pDoc->sysprint(4,0,"����Աͨ�����ּ��޸��г� %s �� %s ����ʵ��ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}
			else
			{
				if(nTgType == TG_TYPE::BASIC_TG)
					modifyTrain.OffsetBasicScheduleTime(m_nFocusRecordIndex, offset, IsCanMoveWholeTrain, 2);
				else
				{
					if(bRouteOkInhibitModifySchd)
						m_pDoc->OffsetWorkScheduleTimeByRouteOk(&modifyTrain, m_nFocusRecordIndex, offset, 2);
					else
						modifyTrain.OffsetWorkScheduleTime(m_nFocusRecordIndex, offset, 2);
				}

				m_pDoc->sysprint(4,0,"����Աͨ�����ּ��޸��г� %s �� %s ƽ�Ƹ�վ������ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}
		}
	} 

	m_pDoc->SendUpdateTrain(modifyTrain);	
}

// ������ҵ�г�
void CTGForm::DrawWorkTrain(CString dlgText, int flag) 
{
	if(m_pDoc->m_nOperatorStatus != NORMAL_STATUS)
		return;
	CClientDC client(this);
	OnPrepareDC(&client);

	CInsertSimpleDlg *dlg = new CInsertSimpleDlg(dlgText, this, flag);
	if(dlg == NULL)
	{
		m_pDoc->sysprint(0,0,"[TG]: alloc memory failed in file %s at line %d",
			__FILE__,__LINE__);
		return;
	}

	if(dlg->DoModal () == IDOK)
	{
		if(m_sFocusTrain != NULL)
		{
			m_nFocusRectIndex = -1;
			m_nFocusRecordIndex = -1;
			RefreshTrain(m_sFocusTrain);
			m_sFocusTrain = NULL;
			m_nFocusTrainIndex=0;
		}
		m_strTrainId = dlg->m_sStaionid;
		if(m_strTrainId.IsEmpty())
		{
			AfxMessageBox("������������ҵ���κ�");
			return;
		}
		m_strRetTrainId = dlg->m_sRetTrainId;

		m_sFocusTrain = new CTG_TRAIN();
		if (m_sFocusTrain == NULL)
		{
			m_pDoc->sysprint(0,0,"[TG] memory allocate failed");
			return;
		}

		// ���������ܹ�������־
		SetTrainFlag(m_sFocusTrain->myTrainInfo.adjust_flag);

		// 2012.01.12�г�����
		m_sFocusTrain->m_nDirectFlag=m_pDoc->GetTrainDirectByTrainId(m_strTrainId);
		if(m_sFocusTrain->m_nDirectFlag==-1)
		{
			if(IDYES == MessageBox("��ע��,ϵͳ�޷��ж�������г��ķ���,����ѡ��:\n���� ��'��' \n���а�'��' ", 
					"��ʾ", MB_YESNO|MB_ICONQUESTION))
			{
 				m_pDoc->sysprint(4,0,"[������ҵ] ����Ա�����г� %s Ϊ����", m_strRetTrainId);
				m_sFocusTrain->m_nDirectFlag = UP_DIRECT;
			}
			else
			{
 				m_pDoc->sysprint(4,0,"[������ҵ] ����Ա�����г� %s Ϊ����", m_strRetTrainId);
				m_sFocusTrain->m_nDirectFlag = DOWN_DIRECT;
			}
		}			
		
		m_pDoc->ResetContinueDraw();
		m_pDoc->m_nOperatorStatus = flag;
	}
	delete dlg; 
}

//���ƿ�������ĳ�A
void CTGForm::OnDrawAbstrainA()
{
	m_strArrivalTrainId = "";
	DrawWorkTrain("������ҵ(������)", INSERT_TRAIN_ABS_A);
}

//���ƴ����俪�صĳ�B
void CTGForm::OnDrawAbstrainB()
{
	m_strArrivalTrainId = "";
	DrawWorkTrain("������ҵ(�����俪��)", INSERT_TRAIN_ABS_B);
}

// ��������ǰ��
void CTGForm::OnDrawGoon() 
{
	m_strArrivalTrainId = "";
	DrawWorkTrain("������ҵ(ǰ��)", INSERT_TRAIN_GOON);
}

// ���������۷�
void CTGForm::OnDrawReturn() 
{
	m_strArrivalTrainId = "";
	DrawWorkTrain("������ҵ(�۷�)", INSERT_TRAIN_RETURN);
}

BOOL CTGForm::newTrainSimple(CPoint point,int insert_type)
{
	int nDirectFlag=0,nRouteSetNo=0;
	int node_index=0;

	do 
	{
		if(m_sFocusTrain == NULL)
			break;

		// 2012.01.12�г�����
		m_sFocusTrain->m_nDirectFlag=m_pDoc->GetTrainDirectByTrainId(m_strTrainId);
		if(m_sFocusTrain->m_nDirectFlag==-1)
		{
			if(IDYES == MessageBox("��ע��,ϵͳ�޷��ж�������г��ķ���,����ѡ��:\n���� ��'��' \n���а�'��' ", 
					"��ʾ", MB_YESNO|MB_ICONQUESTION))
				m_sFocusTrain->m_nDirectFlag = UP_DIRECT;
			else
				m_sFocusTrain->m_nDirectFlag = DOWN_DIRECT;
		}

		nDirectFlag = m_sFocusTrain->m_nDirectFlag;

		//���վ��
		int nStationNo = m_pDoc->ConvertYPostoStationNo(point.y,node_index);
		if(nStationNo == -1)
			break;

		//���г�
		if(nDirectFlag == DOWN_DIRECT)
		{
			if(node_index >= m_pDoc->m_nDispDistanceNum - 1)
				break;

			DispDistance *node_info1,*node_info2;

			node_info1 = &m_pDoc->m_sDispDistance[node_index];
			node_info2 = &m_pDoc->m_sDispDistance[node_index + 1];

			if(node_info1->sub_section != node_info2->sub_section)
				break;
		}
		//���г�
		else
		{
			if(node_index < 1)
				break;

			DispDistance *node_info1,*node_info2;

			node_info1 = &m_pDoc->m_sDispDistance[node_index];
			node_info2 = &m_pDoc->m_sDispDistance[node_index - 1];

			if(node_info1->sub_section != node_info2->sub_section)
				break;
		}

		//1 add first record
		int primary_side = m_pDoc->ConvertYPostoPrimarySide(point.y,nRouteSetNo);
		int nAbsList[32];
		// ��ȡ���������г�,���ó��������߱�
		int nAbsNum=m_pDoc->GetExitByNodeIndexForTo_abs(node_index,nStationNo,nDirectFlag,nAbsList);
		if(nAbsNum<=0)
		{
			break;
		}
		WORD uExit=0;
		if(nAbsNum==1)
		{
			uExit = m_pDoc->ConvertAbsnumStationToEntry(nStationNo, nAbsList[0]);
		}
		else
		{
			CDlgGiveoutDirectionSel dlg(m_pDoc, this, 9, nAbsList, nAbsNum, nStationNo);
			if(dlg.DoModal() == IDOK)
			{
				uExit = dlg.nSelDirect;
			}
			else
			{
				break;
			}
		}

		long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);

		TgTRAIN_RECORD rec;
		rec.Clear();
		rec.arrive_train_id = m_strTrainId;
		rec.depart_train_id = m_strTrainId;

		rec.rec_index = 0;
		rec.station = nStationNo;   //��վվ��
		rec.arrive = usTimeValue;
		rec.depart = usTimeValue;
	
		rec.plan_arrive =  rec.arrive;
		rec.plan_depart = rec.depart;
		
		rec.entry = 0;
		rec.exit  = uExit;
		rec.in_station_index=node_index;
		rec.out_station_index=node_index;
		rec.arrive_side = primary_side;    rec.depart_side = primary_side;
		rec.adjust_status = 0;
		rec.flag = 0;
		CancelFlagStopSignal(rec.flag);

		SetFlagStart(rec.flag);

		m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType());

		//add for stop train abs
		TgTRAIN_RECORD *temprec = m_sFocusTrain->GetLastRecord();
		if(nDirectFlag == 1)
			SetFlagStopAbsDown(temprec->flag);
		else
			SetFlagStopAbsUp(temprec->flag); 

		SetFlagStopAbs(temprec->flag);
		temprec->stop_abs_s_time = 3;
		temprec->stop_abs_e_time = 57;

		//2 next record
		WORD uEntry=0;
		WORD nStationNo_next;
		if(insert_type == INSERT_TRAIN_RETURN)
		{
			nStationNo_next = nStationNo;
			uEntry = uExit;
		}
		else if(insert_type ==  INSERT_TRAIN_GOON)
		{
			WORD line_no=0;
			nStationNo_next = m_pDoc->GetAbsLineOtherStation(nStationNo, uExit, line_no);
			uEntry =  m_pDoc->ConvertAbsnumStationToEntry(nStationNo_next, line_no);
		}		 


		rec.Clear();

		rec.arrive = temprec->depart + 3600; 
		rec.arrive_train_id = m_strTrainId;
		rec.depart_train_id = m_strTrainId;
		rec.rec_index = 1;
		rec.station   = nStationNo_next; //�¸�վ��վ��
		rec.depart    = rec.arrive;

		if(insert_type == INSERT_TRAIN_RETURN)
		{
			rec.in_station_index=node_index;
			rec.out_station_index=node_index;
		}
		else if(insert_type ==  INSERT_TRAIN_GOON)
		{
			//���г�
			if(nDirectFlag == DOWN_DIRECT)
			{
				int n = m_pDoc->GetAbsPreStationIndexByNextStationAndEntry(nStationNo, uExit, false, node_index);
				if(n!=-1)
					rec.in_station_index = n;
				else
					rec.in_station_index = node_index+1;
				
				rec.out_station_index = rec.in_station_index;
			}
			//���г�
			else
			{
				int n = m_pDoc->GetAbsPreStationIndexByNextStationAndEntry(nStationNo, uExit, true, node_index);
				if(n!=-1)
					rec.in_station_index = n;
				else
					rec.in_station_index = node_index-1;
				
				rec.out_station_index = rec.in_station_index;
			}
		}
		
		rec.plan_arrive = rec.arrive;
		rec.plan_depart = rec.depart;
		
		rec.entry = uEntry;
		rec.exit  = 0;
		rec.adjust_status = 0;
		rec.flag = 0;
		CancelFlagStopSignal(rec.flag);

		rec.arrive_side = 1;
		rec.depart_side = 1;

		SetFlagAnLocReturn(rec.flag);

		m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType());
				
		if(insert_type == INSERT_TRAIN_RETURN)
		{
			temprec = m_sFocusTrain->GetRecordByRecIndex(1);
			if(temprec == NULL) break;
			temprec->arrive_train_id = m_strRetTrainId;
			temprec->depart_train_id = "";
		}

		if (insert_type == INSERT_TRAIN_GOON)
			m_pDoc->sysprint(4,0,"[������ҵ] ����Աͨ�� ������ҵ(ǰ��) ��ʽ�����г� %s",m_sFocusTrain->GetTrainId());
		else if (insert_type == INSERT_TRAIN_RETURN)
			m_pDoc->sysprint(4,0,"[������ҵ] ����Աͨ�� ������ҵ(�۷�) ��ʽ�����г� %s",m_sFocusTrain->GetTrainId());

		return TRUE;

	}while(0);

	return FALSE;
}

BOOL CTGForm::newTrainSimple_ABS_B(CPoint point,int insert_type)
{
	int nRouteSetNo=0;
	int node_index=0;
	int uEntry=0; 
	long usTimeValue=0;

	do 
	{
		if(insert_type != INSERT_TRAIN_ABS_B)
			break;
		if(m_sFocusTrain == NULL)
			break;

		//���վ��
		int nStationNo = m_pDoc->ConvertYPostoStationNo(point.y,node_index);
		if(nStationNo == -1)
			break;

		int nDirectFlag = m_sFocusTrain->m_nDirectFlag;
		//���г�
		if(nDirectFlag == DOWN_DIRECT)
		{
			if(node_index <= 0)
				break;   
		}
		//���г�
		else
		{
			if(node_index >= m_pDoc->m_nDispDistanceNum - 1)
				break;   
		}

		//1 add first record
		int primary_side = m_pDoc->ConvertYPostoPrimarySide(point.y,nRouteSetNo);
		int nAbsList[32];
		int nAbsNum=m_pDoc->GetEntryByNodeIndexForFrom_abs(node_index,nStationNo,nDirectFlag,nAbsList);
		if(nAbsNum<=0)
		{
			break;
		}
		if(nAbsNum==1)
		{
			uEntry = m_pDoc->ConvertAbsnumStationToEntry(nStationNo, nAbsList[0]);
		}
		else
		{
			CDlgGiveoutDirectionSel dlg(m_pDoc, this, 8, nAbsList, nAbsNum, nStationNo);
			if(dlg.DoModal() == IDOK)
			{
				uEntry = dlg.nSelDirect;
			}
			else
			{
				break;
			}
		}
	
		usTimeValue = m_pDoc->ConvertXPostoTime(point.x);

		TgTRAIN_RECORD rec;
		rec.Clear();
		rec.arrive_train_id = m_strTrainId;
		rec.depart_train_id = m_strTrainId;
		rec.rec_index = 0;
		rec.station = nStationNo;   //��վվ��
		rec.arrive = usTimeValue;
		rec.depart = usTimeValue;
		
		rec.in_station_index=node_index;
		rec.out_station_index=node_index;

		rec.plan_arrive =  rec.arrive;
		rec.plan_depart = rec.depart;
	
		rec.entry =  uEntry;
		rec.exit =  0;
		rec.arrive_side = primary_side;    
		rec.depart_side = primary_side;
		rec.adjust_status = 0;
		
		m_sFocusTrain->myTrainInfo.type = 0;
		m_sFocusTrain->myTrainInfo.stop_abs_time = 20;
		rec.stop_abs_s_time = 0;
		rec.stop_abs_e_time = 0;

		rec.adjust_status = 0;
		rec.early_depart_time=0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)

		rec.flag = 0;
	
		SetFlagAbsStart(rec.flag);
		if(nDirectFlag == UP_DIRECT)
			SetAbsStartStopDown(m_sFocusTrain->myTrainInfo.adjust_flag);
		else
			SetAbsStartStopUp(m_sFocusTrain->myTrainInfo.adjust_flag);
	
		rec.static_arrive=0;
		rec.static_depart=0;
		
		SetFlagTerminal(rec.flag);  
	
		m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType());

		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

		m_pDoc->SendAppendTrain(m_sFocusTrain, "", 0);

		m_pDoc->sysprint(4,0,"[������ҵ] ����Աͨ�� ������ҵ(�����俪��) ��ʽ�����г� %s",m_sFocusTrain->GetTrainId());

		EraseTrain(*m_sFocusTrain);

		m_pDoc->ResetContinueDraw();

		return TRUE;

	}while(0);
	return FALSE;
}
BOOL CTGForm::newTrainSimple_ABS(CPoint point,int insert_type)
{
	int nRouteSetNo=0;
	int node_index=0;
	int uExit=0; 

	long usTimeValue=0;

	do 
	{
		if(insert_type != INSERT_TRAIN_ABS_A)
			break;
		if(m_sFocusTrain == NULL)
			break;

		int nDirectFlag = m_sFocusTrain->m_nDirectFlag;

		//���վ��
		int nStationNo = m_pDoc->ConvertYPostoStationNo(point.y,node_index);
		if(nStationNo == -1)
			break;

		//���г�
		if(nDirectFlag == 1)
		{
			if(node_index >= m_pDoc->m_nDispDistanceNum - 1)
				break;  //������վ̨�϶������һ����
		}
		//���г�
		else
		{
			if(node_index <= 0)
				break;  //������վ̨�϶��ǵ�һ��
		}

		//1 add first record
		int primary_side = m_pDoc->ConvertYPostoPrimarySide(point.y,nRouteSetNo);
		int nAbsList[32];
		// ��ȡ���������г�,���ó��������߱�
		int nAbsNum=m_pDoc->GetExitByNodeIndexForTo_abs(node_index,nStationNo,nDirectFlag,nAbsList);
		if(nAbsNum<=0)
		{
			break;
		}
		if(nAbsNum==1)
		{
			uExit = m_pDoc->ConvertAbsnumStationToEntry(nStationNo, nAbsList[0]);
		}
		else
		{
			CDlgGiveoutDirectionSel dlg(m_pDoc, this, 9, nAbsList, nAbsNum, nStationNo);
			if(dlg.DoModal() == IDOK)
			{
				uExit = dlg.nSelDirect;
			}
			else
			{
				break;
			}
		}

		usTimeValue = m_pDoc->ConvertXPostoTime(point.x);

		TgTRAIN_RECORD rec;
		rec.Clear();
		rec.rec_index = 0;
		rec.arrive_train_id = m_strTrainId;
		rec.depart_train_id = m_strTrainId;
		rec.station = nStationNo;   //��վվ��
		rec.arrive = usTimeValue;
		rec.depart = usTimeValue;
		rec.in_station_index=node_index;
		rec.out_station_index=node_index;
		
		rec.plan_arrive =  rec.arrive;
		rec.plan_depart = rec.depart;
		
		rec.entry =  0;
		rec.exit  =  uExit;
		rec.arrive_side = primary_side;    
		rec.depart_side = primary_side;
		rec.adjust_status = 0;

		rec.flag = 0;
		CancelFlagStopSignal(rec.flag);

		SetFlagStart(rec.flag);  

		if(nDirectFlag == 1)
			SetFlagStopAbsDown(rec.flag);
		else
			SetFlagStopAbsUp(rec.flag); 

		SetFlagStopAbs(rec.flag); 
		rec.stop_abs_s_time = rec.stop_abs_e_time = 10;

		rec.static_arrive=0;
		rec.static_depart=0;
		
		rec.depart = rec.arrive;

		SetFlagAbsTerminal(rec.flag);

		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

		m_sFocusTrain->AddRecord(rec, m_pDoc->GetTgType());

		m_pDoc->SendAppendTrain(m_sFocusTrain, "", 0);
		m_pDoc->sysprint(4,0,"[������ҵ] ����Աͨ�� ������ҵ(������) ��ʽ�����г� %s",m_sFocusTrain->GetTrainId());
		EraseTrain(*m_sFocusTrain);

		m_pDoc->ResetContinueDraw();

		return TRUE;

	}while(0);
	return FALSE;
}

BOOL CTGForm::PreTranslateMessage(MSG* pMsg) 
{

	if ((pMsg->message == WM_KEYDOWN) &&
		(pMsg->wParam == VK_F12))
	{
		return FALSE;
	}

	return CScrollView::PreTranslateMessage(pMsg);
}

void CTGForm::OnJishiZhujie() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = NEW_GRAPH_NOTE_MARK;
		m_nSubMarkType = TEXT_NOTE_MARK;
	}		

}

void CTGForm::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(m_sFocusTrain == NULL)
	{
		CScrollView::OnLButtonDblClk(nFlags, point);
		return;
	}

	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLBtnDblClkRouteSet())
		{
			if(m_nFocusRecordIndex < 0 )
			{
				OnRouteAutoMark();
				CScrollView::OnLButtonDblClk(nFlags, point);
				return;
			}
		}

		if(m_nFocusRecordIndex >= 0)
			ChangeStationStayOrPass();	
	}

	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CTGForm::OnSelfCloseG()
{
	if(m_pDoc != NULL)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SELF_CLOSE_MARK;
	}
}

void CTGForm::ManageGuaJiChe()
{
	if(m_sFocusTrain == NULL)
		return;

	CGuaJiCheDlg *dlg = new CGuaJiCheDlg(this);
	if(NULL != dlg)
	{
		dlg->m_pDoc = m_pDoc;
		dlg->m_train_index = m_sFocusTrain->m_nTrainIndex;
		dlg->m_pTrain = *m_sFocusTrain;
		
		dlg->Create(IDD_DLG_GUAJICHE, this);
		dlg->ShowWindow(SW_SHOW);
	}
}

void CTGForm::OnSideTianchuangG() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SIDE_TIANCHUANG_MARK;
		m_nSubMarkType = 0;
	}		
}

// �ɵ�ͣ���ǻ��ڹɵ��������������������־����Ƿ�������ͣ��
void CTGForm::OnSideLostEle() 
{
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = SIDE_INHIBIT_MARK; //�ɵ�����
		m_nSubMarkType = SIDE_LOST_ELE_MARK;//������-ͣ��
	}	
}

void CTGForm::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar){
	case 17://VK_CTRL:
		break;
	case VK_F1:
		{
			//20171225��--���ݾ���� ����F1�����ܹ�ƽ�ƺ�����·�г�
			m_pctrl->mConfigTg.m_context.SetMoveSameCZTrain(false);
			m_pctrl->mConfigTg.SetIsCanMoveBasciTrainWhole(false);
		}
		break;
	default:
		break;
	}

	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTGForm::OnAbsTotalFix() //�����ۺ��촰����
{
	if(m_pDoc != NULL)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = ABS_FIX_MARK;
	}
}

void CTGForm::OnAbsEleFixG() //�����������
{
	// TODO: Add your command handler code here
	if(m_pDoc != NULL)
	{
		m_pDoc->m_nOperatorStatus = DRAW_MARK_ITEM_STATUS;
		m_nMarkType = ABS_ELE_FIX_MARK;
	}
}
void CTGForm::OnMarkContinueLining()
{
	if(m_nFocusFigure == NULL)	
		return;

	//�Զ��ӳ�10����
	m_nFocusFigure->end_time  += 10*60;

	m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
}

void CTGForm::OperateTrainsSecleted()
{
	ULONG foundTrainIndex = 0;
	std::vector<ULONG> foundTrainArray;
	
	CPoint poingInRect;

	int xCoordinate = 0;
	int yCoordinate = 0;

	int flag = 0;
	int i = 0;

	CString tempstr = "";

	//Ϊ�˼ӿ�����ٶȣ�����Ϊ8
	for (xCoordinate = m_TrainsSelectedRect.left; xCoordinate < m_TrainsSelectedRect.right; xCoordinate +=8)
	{
		for (yCoordinate = m_TrainsSelectedRect.top; yCoordinate < m_TrainsSelectedRect.bottom; yCoordinate += 50)
		{
			poingInRect.x = xCoordinate;
			poingInRect.y = yCoordinate;

			m_pDoc->QueryCurrentTrain(&poingInRect, foundTrainIndex);

			if (foundTrainIndex <= 0)//û���ҵ��г�
				continue;

			//�����ҵ����г��Ƿ��Ѿ����г��б���
			for (i=0; i<foundTrainArray.size(); i++) 
			{
				if (foundTrainArray[i] == foundTrainIndex)
					break;				
			}

			if (i < foundTrainArray.size())//���г����Ѿ������б��У������ظ�����
				continue;

			// �����б�, ����, �����ʵ�ʵ�,��ô����ʾ�ڵ����Ŀ���
			CTG_TRAIN* single = m_pDoc->GetTrainByIndex(foundTrainIndex);
			if(single != NULL)
			{
				TgTRAIN_RECORD* rec = NULL;
				BOOL actualFlag = FALSE;
				for(int reccount = 0; reccount <single->GetRecordNum(); reccount++)
				{
					rec = single->GetRecordByPosition(reccount);
					if(rec != NULL)
					{
						if(IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
						{
							actualFlag = TRUE;
							break;
						}
					}
				}
				if(!actualFlag)
				{
					foundTrainArray.push_back(foundTrainIndex);
				}
			}
		}
	}

	// û�г�ѡ��, ��ô����ʾ�����Ի���
	if(m_pDoc->m_bMoveMutiTrain)
	{
		m_pDoc->m_bMoveMutiTrain=false;

		m_selTrains.clear();
		CDlgMultiSelOperate dlg(foundTrainArray, m_pDoc, this);
		if(dlg.DoModal()==IDOK)
		{
			for(int i=0; i<dlg.mSelTrainKey.size(); i++)
			{
				CTG_TRAIN* train = m_pDoc->GetTrainByIndex(dlg.mSelTrainKey[i]);
				if(NULL != train)
				{
					m_selTrains.push_back(*train);
				}
			}
		}
	}
	else if(m_pDoc->m_bDeleteMutiTrain)
	{
		m_pDoc->m_bDeleteMutiTrain=false;
		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
		if(foundTrainArray.size() > 0)
		{
			m_pDoc->SendDeleteTrainXml(foundTrainArray);
		}
	}
}

void CTGForm::OnShowtractionTop() 
{
	if(m_sFocusTrain == NULL)
		return;

	if(IsDisplyTractionTop(m_sFocusTrain->myTrainInfo.adjust_flag))
		return;

	SetDisplyTractionNone(m_sFocusTrain->myTrainInfo.adjust_flag);
	SetDisplyTractionTop(m_sFocusTrain->myTrainInfo.adjust_flag);

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

void CTGForm::OnShowtractionBottom() 
{
	if(m_sFocusTrain == NULL)
		return;

	if(IsDisplyTractionBottom(m_sFocusTrain->myTrainInfo.adjust_flag))
		return;

	SetDisplyTractionNone(m_sFocusTrain->myTrainInfo.adjust_flag);
	SetDisplyTractionBottom(m_sFocusTrain->myTrainInfo.adjust_flag);

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

void CTGForm::OnShowtractionMiddle() 
{
	if(m_sFocusTrain == NULL)
		return;

	if(IsDisplyTractionMiddle(m_sFocusTrain->myTrainInfo.adjust_flag))
		return;

	SetDisplyTractionNone(m_sFocusTrain->myTrainInfo.adjust_flag);
	SetDisplyTractionMiddle(m_sFocusTrain->myTrainInfo.adjust_flag);

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

void CTGForm::OnShowtractionNo() 
{
	if(m_sFocusTrain == NULL)
		return;

	if(IsDisplyTractionNone(m_sFocusTrain->myTrainInfo.adjust_flag))
		return;

	SetDisplyTractionNone(m_sFocusTrain->myTrainInfo.adjust_flag);

	m_pDoc->SendUpdateTrain(m_sFocusTrain);
}

void CTGForm::OnMarkCopy() 
{
	if(m_nFocusFigure == NULL)	
		return;

	CTg_BLOCK tempFigure(*m_nFocusFigure);

	int offset=tempFigure.end_time-tempFigure.start_time;
	if(offset<=0)
		return;
		
	tempFigure.mark_index = 0;
	
	tempFigure.start_time = tempFigure.end_time;
	tempFigure.end_time += offset;
	
	m_pDoc->SendAppendMark(tempFigure);
}

void CTGForm::OnDrawNewTrainEnd()
{
	ASSERT_VALID(m_pDoc);

	if(m_pDoc->m_nOperatorStatus != INSERT_TRAIN )
		return;

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEndDrawLineDlgNeed())
	{
		CDrawfreewayendSelDlg dlg;
		if(dlg.DoModal( ) == IDOK)
		{
			int nSel = dlg.nSel;
			if(nSel ==0)
				OnAddtrainDeliver();  //����
			else if(nSel ==1)
				OnAddtrainStop();     //�յ�
			else if(nSel ==2)
				OnAddtrainCancel();   //ȡ������
		}
	}
	else
	{
		if(m_nEndMode == 3)
		{
			OnAddtrainStop();     //�յ�
			m_nEndMode = 0;
		}
		else if(m_nEndMode == 4)
		{
			OnAddtrainDeliver();  //����
			m_nEndMode = 0;
		}
		else
		{
			CDrawfreewayendSelDlg dlg;
			if(dlg.DoModal( ) == IDOK)
			{
				int nSel = dlg.nSel;
				if(nSel ==0)
					OnAddtrainDeliver();  //����
				else if(nSel ==1)
					OnAddtrainStop();     //�յ�
				else if(nSel ==2)
					OnAddtrainCancel();   //ȡ������
			}
		}
	}
}

void CTGForm::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
	{
		m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
		CScrollView::OnRButtonUp(nFlags, point);
		return;
	}

	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	m_bMouseDown = FALSE;
	CRect clientRect;
	GetClientRect(clientRect);

	clientDC.DPtoLP(&point);
	clientDC.DPtoLP(&clientRect);

	if(point.x < 0)
		point.x = 0;
	if(point.y < 0)
		point.y = 0;
	if(point.x > m_pDoc->m_nPageWidth + 30)
		point.x = m_pDoc->m_nPageWidth + 30;
	if(point.y > m_pDoc->m_nPageHeight)
		point.y = m_pDoc->m_nPageHeight;


	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN) 
	{
		bShowTrainMenu = TRUE;
		::ClipCursor(NULL);

		if(m_sFocusTrain == NULL)
		{
			m_pDoc->m_nOperatorStatus = NORMAL_STATUS;
			return;
		}

		do 
		{
			// insert new train point
			int node_index,fore_node_index = -1;
			bool stop_train_abs = FALSE;
			TgTRAIN_RECORD *temprec;
			
			static bool bFirstPoint = TRUE;
			int nStationNo = m_pDoc->ConvertYPostoStationNo(point.y,node_index);
			
			//��ǰ���ѡ����ĳ����վ
			if(nStationNo == -1 && !m_stop_train_abs)
			{	
				AfxMessageBox("��ǰ���û��ѡ�г�վ,Ҳ��������ͣ��λ��");
				Invalidate(FALSE); 
				return;
			}
			int nRecNum = m_sFocusTrain->GetRecordNum();
			if(nRecNum <= 0)
				break;


			temprec = m_sFocusTrain->GetRecordByPosition (nRecNum -1);
			fore_node_index = temprec->out_station_index;

			//////////////////////////////////////////////////////////////////
			//���ӵ������������վ�Լ�����������վ֮��ļ�¼
			long curTime = m_pDoc->ConvertXPostoTime(point.x);

			if(fore_node_index == node_index || m_stop_train_abs)
			{
				// ͬһ��վ,Ҫ�����˵�,��Ҫ���� CScrollView::OnRButtonUp(nFlags, point);
			}
			else
			{
				int sectionNo = m_pDoc->m_nCurrentSectionNo;
				if(m_pDoc->GenerateRecordsBetweenNodes(sectionNo,fore_node_index,node_index,m_sFocusTrain,curTime))
				{
					for (int i=0;i<m_sFocusTrain->GetRecordNum();i++)
					{
						TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByRecIndex(i);
						if (rec==NULL)
							continue;

						int recexit, recentry;
						if(SetEntryDefine(rec->station, sectionNo, m_sFocusTrain, recentry, recexit, i))
						{
							if(recentry>0)
								rec->entry = recentry;
							if(recexit>0)
								rec->exit = recexit;
						}
					}
					
					m_pDoc->SetDirectAtStation(m_sFocusTrain);
					m_pDoc->SetTrainSideAndSchdReal(*m_sFocusTrain);
					m_pDoc->SetTrainWidthColor(m_sFocusTrain);

					m_pDoc->DrawSingleTrain(&clientDC,m_sFocusTrain, DRAW_SCREEN);
					if(m_sFocusTrain->GetRecordNum () >0)
						m_pOldMousePoint = m_sFocusTrain->GetContinuePoint();						
					Invalidate(FALSE); // ��Ҫ����������ˢ���Ҽ����ߺ������

					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsRButtonGenTg())
					{
						OnDrawNewTrainEnd();
						return;
					}
					return; // qiaoyt 20090818 �Ҽ����߲���ֹ
				}
				else 
				{
					if(fore_node_index>=0&&fore_node_index<m_pDoc->m_nDispDistanceNum
						&&node_index>=0&&node_index<m_pDoc->m_nDispDistanceNum)
					{
						CString str;
						str.Format("��ע��,��%s���͡�%s������ѡ������ͼ�ϲ���ͨ,����ֱ������������", 
							m_pDoc->GetStationName(m_pDoc->m_sDispDistance[fore_node_index].station_number), 
							m_pDoc->GetStationName(m_pDoc->m_sDispDistance[node_index].station_number));
						MessageBox(str, "��ʾ", MB_ICONWARNING|MB_OK);
					}
					return; // �����䲻��ͨ,��Ҫ�����˵�,������ CScrollView::OnRButtonUp(nFlags, point);
				}
			}
		}while(0);
	}

	CScrollView::OnRButtonUp(nFlags, point);
}

// ��̨�ܾ���־
void CTGForm::OnRefusedTrain() 
{
	m_pDoc->sysprint(0,0,"[TG] CTGForm::OnRefusedTrain");
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex < 0)
		return;

	CTG_TRAIN modifytrain = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = modifytrain.GetRecordByPosition (m_nFocusRecordIndex );
	if(rec == NULL)
		return;

	int temp =  rec->adjust_status & 0x6000;
	if((rec->adjust_status & 0x6000) == 0x00002000)
		SetNotSendPlanToNextCon(*rec);
	else
		SetBeRejectedByNextCon(*rec);

	m_pDoc->SendUpdateTrain(modifytrain);
}
//��������ͣ�����Ϸ�λ��
void CTGForm::OnStopDrawUp() 
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	CTG_TRAIN modifytrain = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = modifytrain.GetRecordByPosition (m_nFocusRecordIndex );
	if(rec == NULL)
		return;

	ReSetAbsDraw(rec->stop_reason);
	SetAbsDrawUp(rec->stop_reason);
	SetFlagStopAbs(rec->flag);

	m_pDoc->SendUpdateTrain(modifytrain);
}
//��������ͣ�����м�λ��
void CTGForm::OnStopDrawMiddile() 
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	CTG_TRAIN modifytrain = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = modifytrain.GetRecordByPosition (m_nFocusRecordIndex);
	if(rec == NULL)
		return;

	ReSetAbsDraw(rec->stop_reason);
	SetFlagStopAbs(rec->flag);

	m_pDoc->SendUpdateTrain(modifytrain);
}
//��������ͣ�����·�λ��
void CTGForm::OnStopDrawBottom() 
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	CTG_TRAIN modifytrain = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = modifytrain.GetRecordByPosition (m_nFocusRecordIndex );
	if(rec == NULL)
		return;

	ReSetAbsDraw(rec->stop_reason);
	SetAbsDrawBottom(rec->stop_reason);
	SetFlagStopAbs(rec->flag);

	m_pDoc->SendUpdateTrain(modifytrain);
}

//���ø�վ�ĵ�������·���Զ�������־
void CTGForm::OnRouteAutoMark()
{
	if(m_pDoc == NULL || m_sFocusTrain == NULL) 
		return;

	CDlgRouteAutoFlag  dlg(m_pDoc, m_sFocusTrain, this);
	if(dlg.DoModal() == IDOK)
		this->m_pDoc->SendUpdateTrain(dlg.m_nTrain);   
}

void CTGForm::OnInStationReturn()
{
	if(m_pDoc == NULL || m_sFocusTrain == NULL) 
		return;

	CTG_TRAIN train=*m_sFocusTrain;
	CString text;
	CStringArray list_trainno;
	CDWordArray list_trainindex;
	if(!m_pDoc->IsCanSetInterCityReturn(train, text, list_trainno, list_trainindex))
	{
		MessageBox(text, "��ʾ", MB_OK);
	}
	else
	{
		CReturnTrainSelDlg dlg(this,list_trainno, list_trainindex);
		if(dlg.DoModal()==IDOK)
		{
			CTG_TRAIN* tmp=m_pDoc->GetTrainByIndex(dlg.m_return_trainindex);
			if(tmp)
			{
				CTG_TRAIN next_train=*tmp;

				SetStationReturn(train.myTrainInfo.side_utility);
				SetNextTrainStationReturn(next_train.myTrainInfo.side_utility);

				m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s(%u) 0x%0.4X �������г� %s(%u) 0x%0.4XΪ'ԭ���۷�'", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility,
					m_pDoc->GetTrainShowId(next_train), next_train.GetTrainIndex(), next_train.myTrainInfo.side_utility);

				m_pDoc->SendUpdateTrain(train);
				m_pDoc->SendUpdateTrain(next_train);
			}
			else
			{
				MessageBox("�Ҳ��������г�,��������'ԭ���۷�'", "����", MB_OK|MB_ICONHAND);
			}
		}
	}
}

void CTGForm::OnOutStationReturn()
{
	if(m_pDoc == NULL || m_sFocusTrain == NULL) 
		return;
	
	CTG_TRAIN train=*m_sFocusTrain;
	CString text;
	CStringArray list_trainno;
	CDWordArray list_trainindex;
	if(!m_pDoc->IsCanSetInterCityReturn(train, text, list_trainno, list_trainindex))
	{
		MessageBox(text, "��ʾ", MB_OK);
	}
	else
	{
		CReturnTrainSelDlg dlg(this,list_trainno, list_trainindex);
		if(dlg.DoModal()==IDOK)
		{
			CTG_TRAIN* tmp=m_pDoc->GetTrainByIndex(dlg.m_return_trainindex);
			if(tmp)
			{
				CTG_TRAIN next_train=*tmp;

				SetStationOutReturn(train.myTrainInfo.side_utility);
				SetNextTrainOutReturn(next_train.myTrainInfo.side_utility);

				m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s(%u) 0x%0.4X �������г� %s(%u) 0x%0.4XΪ'վ���۷�'", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility,
					m_pDoc->GetTrainShowId(next_train), next_train.GetTrainIndex(), next_train.myTrainInfo.side_utility);

				m_pDoc->SendUpdateTrain(train);
				m_pDoc->SendUpdateTrain(next_train);
			}
			else
			{
				MessageBox("�Ҳ��������г�,��������'վ���۷�'", "����", MB_OK|MB_ICONHAND);
			}
		}
	}
}

void CTGForm::OnNotStationReturn()
{
	if(m_pDoc == NULL || m_sFocusTrain == NULL) 
		return;
	
	CTG_TRAIN train=*m_sFocusTrain;

	CString text;
	CStringArray list_trainno;
	CDWordArray list_trainindex;
	m_pDoc->IsCanSetInterCityReturn(train, text, list_trainno, list_trainindex);

	NotStationReturn(train.myTrainInfo.side_utility);
	
	if(list_trainindex.GetCount()<=0)
	{
		list_trainindex.Add(0);
		list_trainno.Add("�޺����г�");
	}
	
	CReturnTrainSelDlg dlg(this,list_trainno, list_trainindex);
	if(dlg.DoModal()==IDOK)
	{
		m_pDoc->sysprint(4,0,"ѡ���г� %u", dlg.m_return_trainindex);
		CTG_TRAIN* tmp=m_pDoc->GetTrainByIndex(dlg.m_return_trainindex);
		if(tmp)
		{
			CTG_TRAIN next_train=*tmp;
			NotNextStationReturn(next_train.myTrainInfo.side_utility);

			m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s(%u) 0x%0.4X �������г� %s(%u) 0x%0.4XΪ'���۷�'", 
				m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility,
				m_pDoc->GetTrainShowId(next_train), next_train.GetTrainIndex(), next_train.myTrainInfo.side_utility);

			m_pDoc->SendUpdateTrain(train);
			m_pDoc->SendUpdateTrain(next_train);
		}
		else
		{
			m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s(%u) 0x%0.4X ���۷�,û�к����г�", 
						m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility);
			m_pDoc->SendUpdateTrain(train);
		}
	}
}

void CTGForm::OnNotStartReturn()
{
	if(m_pDoc == NULL || m_sFocusTrain == NULL) 
		return;
	
	CTG_TRAIN train=*m_sFocusTrain;

	CString text;
	CStringArray list_trainno;
	CDWordArray list_trainindex;
	m_pDoc->IsCanSetInterCityStartReturn(train, text, list_trainno, list_trainindex);
	if(list_trainno.GetCount()>0)
	{
		m_pDoc->sysprint(4,0,"�г� %s(%u) 0x%0.4X, ����ǰ���յ��г�,�����޸�ʼ����־Ϊ'���۷�'", 
						m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility);
		CString str;
		str.Format("��ע��:�г� %s ����ǰ���յ��г�,�����޸�ʼ���г�Ϊ'���۷�'", 
			m_pDoc->GetTrainShowId(train));
		AfxMessageBox(str);
	}
	else
	{
		NotNextStationReturn(train.myTrainInfo.side_utility);
		m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s(%u) 0x%0.4X ʼ�����۷�", 
						m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.myTrainInfo.side_utility);
		m_pDoc->SendUpdateTrain(train);
	}
}

//����������ͼ��
void CTGForm::OnMarkCopyVDown()
{
	if(m_nFocusFigure == NULL)	
		return;

	CTg_BLOCK tempFigure(*m_nFocusFigure);

	// ������Ƶ��ֶ�
	for(int k=0; k<tempFigure.count; k++)
	{
		tempFigure.ele_number[k]=0;
	}
	tempFigure.count=0;
	tempFigure.mark_index = 0;

	long timewidth = m_nFocusFigure->end_time-m_nFocusFigure->start_time;
	tempFigure.start_time = tempFigure.end_time;
	tempFigure.end_time += timewidth;

	int nMaxIndex , nMinIndex;
	int m_nDispDistanceNum = m_pDoc->m_nDispDistanceNum;
	if(m_nFocusFigure->type == SIDE_INHIBIT_MARK 
		||m_nFocusFigure->type == SIDE_TIANCHUANG_MARK 
		||m_nFocusFigure->type == SIDE_SLOW_MARK
		||m_nFocusFigure->type == SIDE_OCCUPY_MARK)    //�ɵ�����,�ɵ��촰��,վ�ڻ���,�ɵ�ռ��

	{
		int m_nDispDistanceNum = m_pDoc->m_nDispDistanceNum;
		nMaxIndex = nMinIndex = m_nFocusFigure->index;
		if(nMinIndex < m_nDispDistanceNum-1) // 20161009 Խ��
		{
			tempFigure.index  = nMinIndex +1 ;

			tempFigure.station = m_pDoc->m_sDispDistance[nMinIndex+1].station_number;
		}
		else if(nMinIndex == m_nDispDistanceNum-1)
		{ 
			tempFigure.index = nMinIndex;
			tempFigure.station = m_nFocusFigure->station;
		}
		
		tempFigure.count = 0;
		int count = m_pDoc->m_sDispDistance[tempFigure.index].number_of_side;
		for(int i=0;i<count;i++)
		{
			int side_no=m_pDoc->m_sDispDistance[tempFigure.index].side[i].side_number;
			if(side_no>0)
			{
				tempFigure.ele_number[tempFigure.count]=side_no;
				tempFigure.count++;
			}
		}
	}
	else if(m_nFocusFigure->type == ABS_ELE_FIX_MARK 
		|| m_nFocusFigure->type == ABS_FIX_MARK 
		|| m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK
		|| m_nFocusFigure->type == SELF_CLOSE_MARK)//�����������
	{
		//���ɵ�ʱ���Ѿ���֤�����Ҫ��
		nMinIndex = (m_nFocusFigure->station < m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;
		nMaxIndex = (m_nFocusFigure->station > m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;
		tempFigure.station  = nMaxIndex ;
		tempFigure.index  = nMaxIndex + (nMaxIndex - nMinIndex);
		if(tempFigure.index >= m_nDispDistanceNum-1)	
			tempFigure.index  =  m_nDispDistanceNum-1;
		else
		{
			std::vector<int> list_lineno;
			m_pDoc->GetMutiNeibAbsLines(tempFigure.station, tempFigure.index, list_lineno);

		    SetBlockAbsLineByBlockDirect(list_lineno, tempFigure);
		}
	}
	else if(m_nFocusFigure->type == ABS_INHIBIT_MARK || m_nFocusFigure->type == ABS_SLOW_MARK)//����:����/����
	{                       
		nMinIndex = (m_nFocusFigure->station < m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;
		nMaxIndex = (m_nFocusFigure->station > m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;

		if(nMaxIndex <m_nDispDistanceNum-1)
		{
			//���ܲ��������ε����������Ϣ			 
			if(m_pDoc->m_sDispDistance[nMaxIndex].sub_section != m_pDoc->m_sDispDistance[nMaxIndex+1].sub_section )
			{
				nMinIndex = nMaxIndex+1;
				tempFigure.station  = nMinIndex;
				tempFigure.index  = nMinIndex + 1;
			}
			else
			{
				tempFigure.station  = nMaxIndex;
				tempFigure.index  = nMaxIndex + 1;
			}

			std::vector<int> list_lineno;
			m_pDoc->GetNeibAbsLines(tempFigure.station, tempFigure.index, list_lineno);
			
			SetBlockAbsLineByBlockDirect(list_lineno, tempFigure);
		}
		//��Ȼ���ֲ���
		else
		{  
			tempFigure.station  = nMinIndex ;
			tempFigure.index  =    nMaxIndex ;
		}
	}
	else if(m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
	{
	}
	else
	{
		return;
	}

	m_pDoc->SendAppendMark(tempFigure);
}

void CTGForm::SetBlockAbsLineByBlockDirect(std::vector<int> list_lineno, CTg_BLOCK& block)
{
	int nBlockDirect = 0x0003 & block.line;
		
	block.count=0;
	for(int i=0;i<list_lineno.size();i++)
	{
		// 0:�� 1:�� 3:����
		int lineno=list_lineno[i];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(lineno);
		if(line!=NULL)
		{
			// ���� ������
			if(nBlockDirect==2 || nBlockDirect==3)
			{
				block.ele_number[block.count]=lineno;
				block.count++;
			}
			// ����
			else if(nBlockDirect==0)
			{
				if(line->nDirect==3 || line->nDirect==0)
				{
					block.ele_number[block.count]=lineno;
					block.count++;
				}
			}
			// ����
			else if(nBlockDirect==1)
			{
				if(line->nDirect==3 || line->nDirect==1)
				{
					block.ele_number[block.count]=lineno;
					block.count++;
				}
			}
		}
	}
	int mix_index=(block.station>block.index)?block.index:block.station;
	int max_index=(block.station>block.index)?block.station:block.index;

	if(mix_index <=m_pDoc->m_nDispDistanceNum-1 && mix_index>=0)
		block.station1=m_pDoc->m_sDispDistance[mix_index].station_number;
	if(max_index <=m_pDoc->m_nDispDistanceNum-1 && max_index>=0)
		block.station2=m_pDoc->m_sDispDistance[max_index].station_number;
			
	m_pDoc->SetBlockDirect(&block);
}

void CTGForm::OnMarkCopyVUp()
{
	if(m_nFocusFigure == NULL)	
		return;

	CTg_BLOCK tempFigure(*m_nFocusFigure);
	
	// ������Ƶ��ֶ�
	for(int k=0; k<tempFigure.count; k++)
	{
		tempFigure.ele_number[k]=0;
	}
	tempFigure.count=0;
	tempFigure.mark_index = 0;

	long timewidth = m_nFocusFigure->end_time- m_nFocusFigure->start_time;
	tempFigure.start_time = tempFigure.end_time;
	tempFigure.end_time += timewidth;

	int nMaxIndex , nMinIndex;
	int m_nDispDistanceNum = m_pDoc->m_nDispDistanceNum;
	if(m_nFocusFigure->type == SIDE_INHIBIT_MARK 
		||m_nFocusFigure-> type == SIDE_TIANCHUANG_MARK 
		||m_nFocusFigure-> type == SIDE_SLOW_MARK
		||m_nFocusFigure-> type == SIDE_OCCUPY_MARK)    //�ɵ�����,�ɵ��촰��,վ�ڻ���,�ɵ�ռ��		
	{

		nMaxIndex = nMinIndex = m_nFocusFigure->index;
		if(nMinIndex >0 )
		{
			tempFigure.index  = nMinIndex -1 ;

			tempFigure.station = m_pDoc->m_sDispDistance[nMinIndex-1].station_number;
		}
		else if(nMinIndex ==0)
		{ 
			tempFigure.index = nMinIndex;
			tempFigure.station = m_nFocusFigure->station;
		}
		tempFigure.count = 0;
		int count = m_pDoc->m_sDispDistance[tempFigure.index].number_of_side;
		for(int i=0;i<count;i++)
		{
			int side_no=m_pDoc->m_sDispDistance[tempFigure.index].side[i].side_number;
			if(side_no>0)
			{
				tempFigure.ele_number[tempFigure.count]=side_no;
				tempFigure.count++;
			}
		}
	}
	else if(m_nFocusFigure->type == ABS_ELE_FIX_MARK 
		|| m_nFocusFigure->type == ABS_FIX_MARK 
		|| m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK
		|| m_nFocusFigure->type == SELF_CLOSE_MARK)//�����������
	{
		//���ɵ�ʱ���Ѿ���֤�����Ҫ��
		nMinIndex = (m_nFocusFigure->station < m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;
		nMaxIndex = (m_nFocusFigure->station > m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;

		tempFigure.station  = nMinIndex-(nMaxIndex-nMinIndex) ;
		tempFigure.index  = nMinIndex ;
		if(tempFigure.station <0 || nMinIndex ==0)
		{
			tempFigure.station  = m_nFocusFigure->station;
			tempFigure.index  =  m_nFocusFigure->index;
		}
		else
		{
			std::vector<int> list_lineno;
			m_pDoc->GetMutiNeibAbsLines(tempFigure.station, tempFigure.index, list_lineno);
		    SetBlockAbsLineByBlockDirect(list_lineno, tempFigure);
		}
	}
	else if(m_nFocusFigure->type == ABS_INHIBIT_MARK || m_nFocusFigure->type == ABS_SLOW_MARK)//����:����/����
	{                         
		nMinIndex = (m_nFocusFigure->station < m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;
		nMaxIndex = (m_nFocusFigure->station > m_nFocusFigure->index)?m_nFocusFigure->station:m_nFocusFigure->index;

		if(nMinIndex > 0)
		{
			//���ܲ��������ε����������Ϣ			 
			if(m_pDoc->m_sDispDistance[nMinIndex].sub_section != m_pDoc->m_sDispDistance[nMinIndex-1].sub_section )
			{
				if(nMinIndex-2 >0)
					nMinIndex = nMinIndex-2;
				else
					nMinIndex= 0;

				tempFigure.station  = nMinIndex ;
				tempFigure.index  = nMinIndex+1;
			}
			else
			{
				tempFigure.station  = nMinIndex-1;
				tempFigure.index    = nMinIndex ;
			}
				
			std::vector<int> list_lineno;
			m_pDoc->GetNeibAbsLines(tempFigure.station, tempFigure.index, list_lineno);
			SetBlockAbsLineByBlockDirect(list_lineno, tempFigure);
		}
		//��Ȼ���ֲ���
		else
		{  
			tempFigure.station  = nMinIndex ;
			tempFigure.index  =    nMaxIndex ;
		}
	}
	else if(m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
	{
	}
	else
	{
		return;
	}
	
	m_pDoc->SendAppendMark(tempFigure);
}

//���ݼƻ�������Ĭ�ϣ�
void CTGForm::OnDepartByPlan()
{
	if(m_sFocusTrain == NULL)	
		return;
	if(m_nFocusRecordIndex < 0)
		return;

	CTG_TRAIN train;
	train = *m_sFocusTrain;
	TgTRAIN_RECORD *rec = NULL;

	rec = train.GetRecordByPosition (m_nFocusRecordIndex);
	if(rec == NULL) 
		return; 
	
	SetMakeDepartRouteByPlan(rec->stop_condition);
	ManualSetTrigeRoute(rec->stop_condition);

	m_pDoc->SendUpdateTrain(train,SCHD_UPDATE, DEPART_BY_PLAN, rec->station,m_nFocusRecordIndex);
	m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s �� %s ����ʱ�� Ϊ���ռƻ�", 
		m_pDoc->GetTrainShowId(train), m_pDoc->GetStationName(rec->station));
}

//���ݱ��ù�ϵ����
void CTGForm::OnDepartByMeet()
{
	if(m_sFocusTrain == NULL)	
		return;
	if(m_nFocusRecordIndex < 0)
		return;

	CTG_TRAIN train;
	train = *m_sFocusTrain;
	TgTRAIN_RECORD *rec = NULL;

	rec = train.GetRecordByPosition (m_nFocusRecordIndex);
	if(rec == NULL) 
		return; 
	
	SetMakeDepartRouteByMeet(rec->stop_condition);
	ManualSetTrigeRoute(rec->stop_condition);

	m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, DEPART_BY_MEET, rec->station, m_nFocusRecordIndex);

	m_pDoc->sysprint(4,0,"����Ա�˹��趨�г� %s �� %s  ����ʱ�� Ϊ���մ���", 
		m_pDoc->GetTrainShowId(train), m_pDoc->GetStationName(rec->station));
}

void CTGForm::OnDefinePass()
{
	if(m_sFocusTrain == NULL)	
		return;
	if( m_nFocusRecordIndex <0)
		return;
	CTG_TRAIN single;
	single = *m_sFocusTrain;
	TgTRAIN_RECORD *rec = NULL;
	rec = single.GetRecordByPosition (m_nFocusRecordIndex);
	if(rec == NULL) 
		return; 
	CString sResult;
	if( IsDefinePass(*rec))
	{
		ReSetDefinePass(*rec);
		sResult = "����Ϊ��ͨ��";
	}
	else 
	{
		SetDefinePass(*rec);
		sResult = "����Ϊͨ��";

	}

	m_pDoc->SendUpdateTrain(single);
	m_pDoc->sysprint(4,0,"[TG]:����Ա�޸Ĺ涨ͨ��:%s,%sվΪ%s", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station), sResult);

}

void CTGForm::CheckYingyeButStationPass(CStringArray& alarm, int rec_index, TgTRAIN_RECORD *rec, CTG_TRAIN& train)
{
	CString str="";
	if(((rec_index==0)&&IsFlagStart(rec->flag))
			|| ((rec_index+1==train.GetRecordNum()) && IsFlagTerminal(rec->flag)))
	{
		str="";
	}
	else
	{
		if(!IsTechStopTrain(*rec))
		{
			if(rec->arrive==rec->depart)
			{
				str.Format("���� %s �� %s Ӫҵ, �ƻ�����ͨ��", 
					m_pDoc->GetTrainShowId(train),
					m_pDoc->GetStationName(rec->station));
				alarm.Add(str);
				m_pDoc->sysprint(4,0,"Ӫҵ����:%s", str);
			}
		}
	}
}

void CTGForm::OnTurnReality() 
{
	if(m_sFocusTrain == NULL) 
		return;
	if(m_nFocusRecordIndex < 0) 
		return;

	CTG_TRAIN single = *m_sFocusTrain;

	TgTRAIN_RECORD *rec = NULL;

	bool bPlanToActual = TRUE;

	m_pDoc->sysprint(4,0,"[תʵ��] ����Աѡ�м�¼%d, ͨ��תʵ�ʵ����г�%s(%d), ",
		    m_nFocusRecordIndex, m_pDoc->GetTrainShowId(single), single.GetTrainIndex());
	if(m_nMouseDownPosition == DEPART_POSITION)
	{
		for (int i = 0;i <= m_nFocusRecordIndex; i++)
		{
			rec = single.GetRecordByRecIndex(i);
			if(rec == NULL) continue;
			if(!IsActualDepartTime(rec->adjust_status) || !IsActualArrivalTime(rec->adjust_status))
				break;
			if (i == m_nFocusRecordIndex)
				bPlanToActual = FALSE;
		}
	}
	else
	{ // ARRIVE_POSITION
		for (int i = 0;i <= m_nFocusRecordIndex; i++)
		{
			rec = single.GetRecordByRecIndex(i);
			if(rec == NULL) continue;
			if(i<m_nFocusRecordIndex) 
			{
				if(!IsActualDepartTime(rec->adjust_status) || !IsActualArrivalTime(rec->adjust_status))
					break;
			}
			else
			{ 
				if(IsActualArrivalTime(rec->adjust_status))
					bPlanToActual = FALSE;
			}
		}
	}

	CStringArray alarm;
	for (int i = 0;i <= m_nFocusRecordIndex; i++)
	{
		rec = single.GetRecordByRecIndex(i);
		if(rec == NULL) 
			continue;
		if(i == m_nFocusRecordIndex)
		{
			if(m_nMouseDownPosition == ARRIVAL_POSITION)
			{
				if(bPlanToActual)
				{
					if(!IsActualArrivalTime(rec->adjust_status))
					{
						SetActualArrivalTime(rec->adjust_status);
						SetArriveActualByManual(*rec);
						SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
						m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d�����ɼƻ���Ϊʵ��", rec->station);
					}
				}
				else
				{
					CancelActualArrivalTime(rec->adjust_status);
					SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
					m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d������ʵ�ʱ�Ϊ�ƻ�", rec->station);

					CheckYingyeButStationPass(alarm, i, rec, single);
				}
			}
			else
			{              
				if(bPlanToActual)
				{
					if(!IsActualArrivalTime(rec->adjust_status))
					{
						SetActualArrivalTime(rec->adjust_status);
						SetArriveActualByManual(*rec);
						SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
						m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d�����ɼƻ���Ϊʵ��", rec->station);
					}

					if(!IsActualDepartTime(rec->adjust_status))
					{
						SetActualDepartTime(rec->adjust_status);
						SetDepartActualByManual(*rec);
						SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);
						m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d�����ɼƻ���Ϊʵ��", rec->station);
					}
				}
				else
				{
					CancelActualArrivalTime(rec->adjust_status);
					CancelActualDepartTime(rec->adjust_status);
					SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
					SetRecTimeFromDefault(*rec, TRAIN_DEPART);

					m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d��ʵ�ʱ�Ϊ�ƻ�", rec->station);

					CheckYingyeButStationPass(alarm, i, rec, single);
				}
			}
		}
		else
		{
			if(bPlanToActual)
			{
				if(!IsActualArrivalTime(rec->adjust_status))
				{
					SetActualArrivalTime(rec->adjust_status);
					SetArriveActualByManual(*rec);
					SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE);
					m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d�����ɼƻ���Ϊʵ��", rec->station);
				}
				
				if(!IsActualDepartTime(rec->adjust_status))
				{
					SetActualDepartTime(rec->adjust_status);
					SetDepartActualByManual(*rec);
					SetRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART);
					m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d�����ɼƻ���Ϊʵ��", rec->station);
				}
			}
			else
			{
				CancelActualArrivalTime(rec->adjust_status);
				CancelActualDepartTime(rec->adjust_status);
				SetRecTimeFromDefault(*rec, TRAIN_ARRIVE);
				SetRecTimeFromDefault(*rec, TRAIN_DEPART);

				m_pDoc->sysprint(4,0,"[תʵ��] ����Աͨ��תʵ�ʽ���վ%d��ʵ�ʱ�Ϊ�ƻ�", rec->station);
	
				CheckYingyeButStationPass(alarm, i, rec, single);
			}
		}
	}

	if(alarm.GetCount()>0)
	{
		CDlgMessageBoxPassword dlg(alarm, this);
		if(dlg.DoModal()!=IDOK)
			return;
		alarm.RemoveAll();
	}

	m_pDoc->SendUpdateTrain(single, SCHD_UPDATE, TURN_REALITY, rec->station, rec->rec_index);
	return;
}

void CTGForm::OnTechStop( ) 
{
	if(m_sFocusTrain == NULL) 
		return;
	int nFocusIndex =m_nFocusRecordIndex;
	if(nFocusIndex < 0) return;

	TgTRAIN_RECORD *rec;
	CTG_TRAIN single;
	single = *m_sFocusTrain;

	rec = single.GetRecordByPosition(m_nFocusRecordIndex);
	if(NULL == rec)
		return;

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsTechstopModifyNeedPwd())
	{
		CSuperPassDlg pdlg;
		if (pdlg.DoModal() == IDOK)
		{
			if ( pdlg.m_super_pass.Compare("1234") != 0)
			{
				CString str;
				str.Format("%s","�������!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return;
			}
		}
		else
			return;
	}
	//#endif

	CString sResult;
	if( IsTechStopTrain(*rec))
	{
		if(CTC_STATION == m_pDoc->GetStationProperty(rec->station))
		{
			if( (rec->arrive != rec->depart) || ((rec->rec_index==0) && IsFlagStart(rec->flag)) || 
				((rec->rec_index==m_sFocusTrain->GetRecordNum()-1) && IsFlagTerminal(rec->flag)) )
			{
				CString strText1;
				CString strText2;
				if(!this->m_pctrl->mConfigTg.isPlatform(rec->station, rec->arrive_side))
				{
					strText1.Format("%sվ%s��վ̨,%s�β��ܰ��������ҵ", 
						m_pDoc->GetStationName(rec->station),
						m_pDoc->GetSideName(rec->station,  rec->arrive_side),
						m_pDoc->GetTrainShowId(m_sFocusTrain));
				}
				else
				{
					if (IsCrhTrain(m_sFocusTrain->myTrainInfo) && !this->m_pctrl->mConfigTg.isHighPlatform(rec->station, rec->arrive_side))
					{
						strText2.Format("������%s����%sվ����Ǹ�վ̨�ɵ�",
							m_pDoc->GetTrainShowId(m_sFocusTrain),
							m_pDoc->GetStationName(rec->station));
					}
				}
				
				CString strText;
				if(strText1!=""||strText2!="")
				{
					strText="��ע��:\n"+strText1;
					strText+="\n"+strText2;
					strText+="\n\t\t��ȷ���޸���?";
				}
			
				if(strText!="")
				{
					if(MessageBox(strText, "����", MB_ICONWARNING|MB_YESNO)==IDNO)
						return;
					if(strText1!="")
						m_pDoc->sysprint(4,0,"[TG]:%s", strText1);
					if(strText2!="")
						m_pDoc->sysprint(4,0,"[TG]:%s", strText2);
					m_pDoc->sysprint(4,0,"[TG]:����Ա����޸�:%s, %s ΪӪҵ��վ,", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station));
				}
			}
		}

		ClearTechStopTrain(*rec);
		sResult = "�Ǽ���ͣ��";
	}
	else 
	{
		SetTechStopTrain(*rec);
		sResult = "����ͣ��";
	}

	m_pDoc->sysprint(4,0,"[TG]:����Ա�޸�:%s,��%sվΪ%s", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station), sResult);

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLockSideNotTechStop() && m_sFocusTrain->GetTrainType() == TYPE::PASSENGER)
	{
		if (!IsTechStopTrain(*rec))
		{
			SetForceSide(*rec);
			m_pDoc->sysprint(4,0,"[TG]:����Աͨ���޸�:%s,��%sվ�Ǽ���ͣ�㽫 %s ����", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station), m_pDoc->GetSideName(rec->station, rec->arrive_side));
		}
		else 
		{
			ReSetForceSide(*rec);
			m_pDoc->sysprint(4,0,"[TG]:����Աͨ���޸�:%s,��%sվ����ͣ�㽫 %s ������", m_pDoc->GetTrainShowId(single), m_pDoc->GetStationName(rec->station), m_pDoc->GetSideName(rec->station, rec->arrive_side));
		}
	}
	m_pDoc->SendUpdateTrain(single);
}
//added by zhaoliang for no earlylatetime
void CTGForm::CreatNoearlylatetimeFolder()
{
	CString strPath;
	strPath.Format("%s\\Noearlylatetime\\", m_pDoc->GetDataPath()); 
	if(!PathIsDirectory(strPath))   
	{   
		CreateDirectory(strPath,NULL); 
	} 
}
void CTGForm::CreatNoearlylatetimeFile()
{
	CString strPath,strFilename,Train_ID;
	strPath.Format("%s\\Noearlylatetime\\", m_pDoc->GetDataPath()); 
	if(PathIsDirectory(strPath)) //����ļ��д���  
	{  
		strFilename.Format("%s\\Noearlylatetime\\%d.nel", m_pDoc->GetDataPath(),m_pDoc->GetShiftId());
		FILE *filep = fopen(strFilename,"a+");
		if(filep == NULL)
			return;		
		CTG_TRAIN* tmp = myPDataManage->GetTrain(m_nFocusTrainIndex);
		if (tmp==NULL)
			return;
        Train_ID.Format("%d",tmp->GetTrainIndex());
		fprintf(filep,"%s,",Train_ID);
		fclose(filep);
	} 
}
void CTGForm::DeleteNoearlylatetimeFile()
{
	CString strPath,strFilename,Train_ID,TrainID;
	strPath.Format("%s\\Noearlylatetime\\", m_pDoc->GetDataPath()); 
	if(PathIsDirectory(strPath))
	{  
		 //����ļ��д���  �����ݶ��� Train_ID
		strFilename.Format("%s\\Noearlylatetime\\%d.nel", m_pDoc->GetDataPath(),m_pDoc->GetShiftId());
		CStdioFile filep;
		if(filep.Open(strFilename,CFile::modeReadWrite) == NULL)
			return;		
		CTG_TRAIN* tmp = myPDataManage->GetTrain(m_nFocusTrainIndex);
		TrainID.Format("%d,",tmp->GetTrainIndex());
		filep.ReadString(Train_ID);
        Train_ID.TrimLeft(TrainID);
		filep.Close();
        //���´�д������ �������
		strFilename.Format("%s\\Noearlylatetime\\%d.nel", m_pDoc->GetDataPath(),m_pDoc->GetShiftId());
		if(filep.Open(strFilename,CFile::modeCreate|CFile::modeReadWrite) == NULL)
			return;		
		filep.WriteString(Train_ID);
		filep.Close();
	} 
}
void CTGForm::ShowNoearlylatetime()
{
	CString strPath,strFilename,Train_ID,TrainID,deletname;
	strPath.Format("%s\\Noearlylatetime\\", m_pDoc->GetDataPath()); 
	if(PathIsDirectory(strPath))
	{  
		//����ļ��д���  �����ݶ��� Train_ID
		strFilename.Format("%s\\Noearlylatetime\\%d.nel", m_pDoc->GetDataPath(),m_pDoc->GetShiftId());
		CStdioFile filep;
		if(filep.Open(strFilename,CFile::modeRead) == NULL)
			return;		
        filep.ReadString(Train_ID);
		filep.Close();
        //��ȡ��������
		char *str;
		str = Train_ID.GetBuffer(Train_ID.GetLength());
		char seps[]=",";
		char *token;
		token = strtok(str,seps);
		while (token!=NULL)
		{
            std::map<ULONG, ULONG>::iterator it = m_pDoc->m_noearlylatetime.find(atoi(token));
            m_pDoc->m_noearlylatetime.insert(make_pair(atoi(token),atoi(token)));
			token = strtok(NULL,seps);
		}
		//ͬʱɾ��һ����ǰ����
		CFileFind fileFind;
	    CTime tm;
		tm = CTime::GetCurrentTime();
		long time ;
		if(tm.GetMonth()<=1)
         time = (tm.GetYear()-1)*1000+12*100+tm.GetDay();
		else
		 time = tm.GetYear()*100+(tm.GetMonth()-1);

		deletname.Format("%s%d*.nel",strPath,time);
		BOOL flag = fileFind.FindFile(deletname);
		while(flag)
		{
			flag = fileFind.FindNextFile();
			if(fileFind.IsDots())
				continue;
			CString filepath;
			filepath = fileFind.GetFilePath();
			DeleteFile(filepath);	
		}
		fileFind.Close();
	} 

}
// ǿ�����㡡G03
void CTGForm::OnSetNoLateEarlyTime()
{
	if(m_sFocusTrain == NULL) 
		return;

	CTG_TRAIN train, *ptrain=m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(ptrain==NULL)
		return;
	train = *ptrain;
	std::map<ULONG, ULONG>::iterator it = m_pDoc->m_noearlylatetime.find(m_sFocusTrain->m_nTrainIndex);
	
	CString trainId=m_pDoc->GetTrainShowId(m_sFocusTrain);

	//added by zhaoliang for no earlylatetime
	CreatNoearlylatetimeFolder();//�����ļ���

	if(it != m_pDoc->m_noearlylatetime.end())
	{
		m_pDoc->m_noearlylatetime.erase(it);
		m_pDoc->sysprint(10,0, "����Աȡ���г� %s ǿ������", trainId);
		DeleteNoearlylatetimeFile();//ɾ��ǿ��������г���Ϣ
	}
	else
	{
		m_pDoc->m_noearlylatetime.insert(make_pair(m_sFocusTrain->m_nTrainIndex,m_sFocusTrain->m_nTrainIndex));
		m_pDoc->sysprint(10,0, "����Ա�����г� %s ǿ������", trainId);
		CreatNoearlylatetimeFile();//��¼ǿ��������г���Ϣ
	}
	
	this->EraseTrain(train);
	DrawTrain(train, false);
}

// ���ñ����г�
void CTGForm::OnSetSecretTrain()
{
	if(m_sFocusTrain == NULL) 
		return;

	CString text;
	CString trainId=m_pDoc->GetTrainShowId(m_sFocusTrain);
	if(IsTrainSecret(m_sFocusTrain->myTrainInfo))
	{
		text.Format("��ע��, �г� %sΪ �����г�, ��ȷ��������Ϊ �Ǳ��� �г���?", trainId);
		if(MessageBox(text, "��ʾ", MB_OKCANCEL) == IDOK)
		{
			SetTrainNotSecret(m_sFocusTrain->myTrainInfo);
			m_pDoc->sysprint(10,0, "����Ա�����г� %s Ϊ�Ǳ����г�", trainId);
			m_pDoc->SendUpdateTrain(m_sFocusTrain);
		}
	}
	else
	{
		text.Format("��ע��, �������г� %sΪ ���� �г���?", trainId);
		if(MessageBox(text, "��ʾ", MB_OKCANCEL) == IDOK)
		{
			SetTrainSecret(m_sFocusTrain->myTrainInfo);
			m_pDoc->sysprint(10,0, "����Ա�����г� %s Ϊ�����г�", trainId);
			m_pDoc->SendUpdateTrain(m_sFocusTrain);
		}
	}

	return;
}

void CTGForm::OnSaveTrainXML()
{
	if(m_sFocusTrain == NULL) 
		return;

	bool bFindTrainFile=false, bFindPasswayFile=false;
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile("c:\\*.xml");
	while(flag)
	{
		flag = fileFind.FindNextFile();
		
		if(fileFind.IsDirectory())
			continue;
		
		CString fileName = fileFind.GetFileName();
		if("train_passway.xml"==fileName)
		{
			bFindPasswayFile=true;
		}
		else if("train_save.xml"==fileName)
		{
			bFindTrainFile=true;
		}

		if(bFindPasswayFile || bFindTrainFile)
			break;
	}
	fileFind.Close();

	if(bFindPasswayFile || bFindTrainFile)
	{
		CString text;
		text.Format("��C:\\�´���Ҫ���������ļ� train_save.xml��train_passway.xml, ��ȷ�ϸ�����?");
		if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONWARNING)==IDNO)
			return;
	}

	bFindTrainFile=false, bFindPasswayFile=false;
	CStdioFile file;
	if(file.Open("C:\\train_save.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		CString xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?> ";
		xml +=m_sFocusTrain->toXML(0,0,0,"",0);
		file.Write(xml.GetBuffer(), xml.GetLength());
		file.Close();
		bFindTrainFile=true;
	}

	// passway
	if(file.Open("C:\\train_passway.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		CString xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?> ";
		xml +=m_sFocusTrain->toPassway();
		file.Write(xml.GetBuffer(), xml.GetLength());
		file.Close();
		bFindPasswayFile=true;
	}

	if(bFindPasswayFile || bFindTrainFile)
	{
		CString str;
		str.Format("���������ļ� train_save.xml��train_passway.xml, ������C:\\��");
		AfxMessageBox(str);
	}
	else
	{
		AfxMessageBox("���������ļ� train_save.xml��train_passway.xml, ʧ�ܣ���");
	}

	return;
}
void CTGForm::OnTrainCopy()
{
	if(m_sFocusTrain == NULL) 
		return;

	CTG_TRAIN train;
	train = *m_sFocusTrain;
	train.m_nTrainIndex=0;
	train.OffsetWorkScheduleTime(600);
	int recNum = train.GetRecordNum();

	TgTRAIN_RECORD *rec=NULL;
	if(this->m_pctrl->mConfigTg.m_context.IsEnableTaiyuanCopyTrain())
	{
		CDlgCopyTrain dlg;
		if(dlg.DoModal() != IDOK)
			return;
		if(dlg.m_trainId.IsEmpty())
		{
			AfxMessageBox("���κ�Ϊ��");
			return;
		}

		for(int i=0;i<recNum;i++)
		{
			rec = train.GetRecordByPosition(i);
			if(rec == NULL)
				return;

			rec->original_arrive_train_id="";
			rec->original_depart_train_id="";
			rec->arrive_train_id = dlg.m_trainId;
			rec->depart_train_id = rec->arrive_train_id;
		}
	}
	
	for(int i=0;i<recNum;i++)
	{
		rec = train.GetRecordByPosition(i);
		if(rec == NULL)
			return;

		rec->modify_abs_runtime = 0;
		//�������·׼���õı��
		SetRouteClear(*rec); 
		SetArrivePlan(*rec);
		SetDepartPlan(*rec);
	}
	train.ClearEngine();  //̫ԭ 20101009  ���⸴���г�ʱ��������Ҳ������
	train.myTrainInfo.Clear();   //̫ԭ  20110316  ���ƴ��հ�ƻ���������ʱ���κŲ�Ҫ��*��
		
	m_pDoc->SendAppendTrain(&train, "", 0);

	return;
}

void CTGForm::OnStopTemp() 
{	
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

// 	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
// 	{
// 		for (int i = modifyTrain.GetRecordNum() -1 ; i >= nSelRecIndex; i--)
// 		{
// 			TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
// 			if(rec->adjust_status & 0x0003)
// 			{
// 				AfxMessageBox("����ʵ�ʵ�,��ֹ�޸�!");
// 				return;
// 			}
// 		}
// 	}

	bool bWarning=false;
	int nRecNum = modifyTrain.GetRecordNum();
	if(nSelRecIndex<nRecNum-1)
	{
		for (int i = nRecNum -1 ; i > nSelRecIndex; i--)
		{
			modifyTrain.DeleteTailRecord();
			bWarning=true;
		}
	}
	else
	{
		TgTRAIN_RECORD *lastrec = modifyTrain.GetLastRecord();
		if(lastrec)
		{
			if(!IsFlagTerminal(lastrec->flag))
			{
				bWarning=true;
			}
		}
	}

	TgTRAIN_RECORD *rec;
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL) return;
	TgTRAIN_RECORD lastRec=*rec;

	rec->stop_abs_position = 0;
	rec->stop_abs_s_time = 0;
	rec->stop_abs_e_time = 0;
	rec->depart = rec->arrive;
	

	// �յ���ͳ�Ƶ���
	rec->static_arrive=1;
	rec->static_depart=0;
	
	if(IsFlagStopAbs(rec->flag)) 
		CancelFlagStopAbs(rec->flag);
	if(IsFlagStopSignal(rec->flag)) 
		CancelFlagStopSignal(rec->flag);

	ClearFlagAbsTerminal(rec->flag);
	SetFlagEnd(rec->flag);

	// ���¼���
	// ����������̨������ȫ��ͬ�߱���ظ����Σ������¼������ڵ��򣬴���
	TgTRAIN_RECORD *prec;
	prec = modifyTrain.GetPrevStationRecord(rec);
	if (!prec || !m_pDoc->JudeNodesConnected(prec->out_station_index, rec->in_station_index))
	{
		rec->in_station_index=NULL_INDEX;
		rec->out_station_index=NULL_INDEX;	
	}
	m_pDoc->SetTrainLastRecIndex(modifyTrain, true);

	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ��ʱͣ��", m_pDoc->GetTrainShowId(m_sFocusTrain), rec->station);

	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		if(IsFlagStart(first->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ� \n�������޸� %s �� %s ��ʱ�յ�ʧ��!", 
			m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(first->station));
			m_pDoc->sysprint(10,0, "%s", text);

			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			return;
		}
	}

	// ������·���Դ�
	if(IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա���� %s���г���ʱ�յ� %s ������·�������Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeDepartRoute(rec->stop_condition);
	}
	
	m_pDoc->SendUpdateTrain(modifyTrain);

	if(bWarning)
		WarningStplan(lastRec);
}

void CTGForm::OnPressureBefore()
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	TgTRAIN_RECORD *rectmp = modifyTrain.GetRecordByPosition(m_nFocusRecordIndex);
	if(rectmp == NULL)
		return;
	
	if(IsBeforePressure(rectmp->stop_condition))
	{
		ClearBeforePressure(rectmp->stop_condition);
		m_pDoc->sysprint(10,0, "����Ա ȡ�� �г�%s��%dվ ѹǰ", m_pDoc->GetTrainShowId(m_sFocusTrain), rectmp->station);
	}
	else
	{
		SetBeforePressure(rectmp->stop_condition);
		m_pDoc->sysprint(10,0, "����Ա ���� �г�%s��%dվ ѹǰ", m_pDoc->GetTrainShowId(m_sFocusTrain), rectmp->station);
	}

	m_pDoc->SendUpdateTrain(modifyTrain); 
}

void CTGForm::OnPressureAfter()
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	TgTRAIN_RECORD *rectmp = modifyTrain.GetRecordByPosition(nSelRecIndex);
	if(rectmp == NULL)
		return;
	
	if(IsAfterPressure(rectmp->stop_condition))
	{
		ClearAfterPressure(rectmp->stop_condition);
		m_pDoc->sysprint(10,0, "����Ա ȡ�� �г�%s��%dվ ѹ��", m_pDoc->GetTrainShowId(m_sFocusTrain), rectmp->station);
	}
	else
	{
		SetAfterPressure(rectmp->stop_condition);
		m_pDoc->sysprint(10,0, "����Ա ���� �г�%s��%dվ ѹ��", m_pDoc->GetTrainShowId(m_sFocusTrain), rectmp->station);
	}

	m_pDoc->SendUpdateTrain(modifyTrain);
}

void CTGForm::OnStopDraw() 
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;
	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
	{
		for (int i = modifyTrain.GetRecordNum() -1 ; i >= nSelRecIndex; i--)
		{
			TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
			if(rec->adjust_status & 0x0003)
			{
				AfxMessageBox("����ʵ�ʵ�,��ֹ�޸�!");
				return;
			}
		}
	}

	bool bWarning=false;
	int nRecNum = modifyTrain.GetRecordNum() ;
	if(nSelRecIndex<nRecNum-1)
	{
		for (int i = nRecNum -1 ; i > nSelRecIndex; i--)
		{
			modifyTrain.DeleteTailRecord();
			bWarning=true;
		}
	}
	else
	{
		TgTRAIN_RECORD *lastrec = modifyTrain.GetLastRecord();
		if(lastrec)
		{
			if(!IsFlagTerminal(lastrec->flag))
			{
				bWarning=true;
			}
		}
	}

	TgTRAIN_RECORD *rec;
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL) return;
	TgTRAIN_RECORD lastRec=*rec;
	rec->stop_abs_position = 0;
	rec->stop_abs_s_time = 0;
	rec->stop_abs_e_time = 0;
	rec->depart = rec->arrive;
	
	//�յ���ͳ�������
	rec->static_arrive=1;
	rec->static_depart=0;
	
	rec->exit = -1;

	if(IsFlagStopAbs(rec->flag)) 
		CancelFlagStopAbs(rec->flag);
	if(IsFlagStopSignal(rec->flag)) 
		CancelFlagStopSignal(rec->flag);

	ClearFlagAbsTerminal(rec->flag);
	SetFlagTerminal(rec->flag);
	
	//�÷�����������ѡ��
	rec->arrive_side = -1;
	rec->depart_side = -1;

	// ���¼���
	TgTRAIN_RECORD *prec;
	prec = modifyTrain.GetPrevStationRecord(rec);
	if (!prec || !m_pDoc->JudeNodesConnected(prec->out_station_index, rec->in_station_index))
	{
		rec->in_station_index=NULL_INDEX;
		rec->out_station_index=NULL_INDEX;	
	}
	m_pDoc->SetTrainLastRecIndex(modifyTrain, true);
	
	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ�յ�", m_pDoc->GetTrainShowId(modifyTrain), rec->station);
	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *first = modifyTrain.GetFirstRecord();
		if(IsFlagStart(first->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ� \n�������޸� %s �� %s �յ�ʧ��!", 
				m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(first->station));
			m_pDoc->sysprint(10,0, "%s", text);
			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			return;
		}
	}

	// ������·���Դ�
	if(IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�޸ĸó���վ�յ� %s���г� %s ������·�������Դ�", 
			m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeDepartRoute(rec->stop_condition);
	}

	m_pDoc->SendUpdateTrain(modifyTrain);  

	if(bWarning)
		WarningStplan(lastRec);
}

void CTGForm::WarningStplan(TgTRAIN_RECORD& lastRec) 
{
	if(lastRec.arrive<lastRec.depart)
	{
		CString text, st=m_pDoc->GetStationName(lastRec.station), trainid=lastRec.depart_train_id;

		CTime atm=CTime(lastRec.arrive), dtm=CTime(lastRec.depart);
		text.Format("����Ա�޸��г�:%s �ڳ�վ:%s �յ�, ԭ���ƻ�Ϊ�����ƻ� ����ʱ�� %s ����ʱ�� %s.", 
			trainid, st, atm.Format("%Y-%m-%d %H:%M:%S"), dtm.Format("%Y-%m-%d %H:%M:%S"));
		
		m_pDoc->sysprint(10,0, "%s", text);

		CTime tm=CTime::GetCurrentTime();
		if(lastRec.arrive<=tm.GetTime())
		{
			CString text;
			text.Format("�޸�%s�յ���, �ó��Ѳ��ڼƻ��·�����,��ش�����������:\n1.֪ͨ %s ��վֵ��Ա�ֹ����� %s �յ�\n2.֪ͨ ���� �ֹ�ɾ�� %s �ķ�����·����!!", 
				trainid, st, trainid, trainid);
			m_pDoc->sysprint(10,0, "%s", text);
			MessageBox(text, "���ؾ���", MB_ICONWARNING|MB_OK);
		}
	}
}

void CTGForm::OnStopDepart() 
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;
	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
	{
		for (int i = modifyTrain.GetRecordNum() -1 ; i >= nSelRecIndex; i--)
		{
			TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
			if(rec->adjust_status & 0x0003)
			{
				AfxMessageBox("����ʵ�ʵ�,��ֹ�޸�!");
				return;
			}
		}
	}

	int nRecNum = modifyTrain.GetRecordNum () ;
	for (int i = nRecNum -1; i > nSelRecIndex; i--)
	{
		modifyTrain.DeleteTailRecord();
	}

	TgTRAIN_RECORD *rec;
	rec = modifyTrain.GetLastRecord();
	if(rec == NULL) return;

	if(IsFlagStopAbs(rec->flag)) 
		CancelFlagStopAbs(rec->flag);
	if(IsFlagStopSignal(rec->flag)) 
		CancelFlagStopSignal(rec->flag);

	ClearFlagAbsTerminal(rec->flag);
	SetFlagGiveout(rec->flag);

	//�Ե�ǰ���г����һ�������жϣ�����ǽ����������ж��������������ʾ�û�����ѡ��
	if(rec->station == m_pDoc->m_nCurrentStationNoOfMouse)
	{
		SetTrainExit(&modifyTrain, -1);
	}	
	rec->early_depart_time=0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)
	
	//�÷�����������ѡ��
	rec->arrive_side = -1;
	rec->depart_side = -1;

	// ���¼���
	TgTRAIN_RECORD *prec;
	prec = modifyTrain.GetPrevStationRecord(rec);
	if (!prec || !m_pDoc->JudeNodesConnected(prec->out_station_index, rec->in_station_index))
	{
		rec->in_station_index=NULL_INDEX;
		rec->out_station_index=NULL_INDEX;	
	}
	m_pDoc->SetTrainLastRecIndex(modifyTrain, true);
	
	// ������·�Դ�
	if(!IsAutoMakeDepartRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�޸ĸó���վ����, %s���г� %s ������·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeDepartRoute(rec->stop_condition);
	}

	m_pDoc->SendUpdateTrain(modifyTrain); 	
	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ���� exit=%d", m_pDoc->GetTrainShowId(m_sFocusTrain), rec->station,rec->exit);
}

// �м�ָ�
void CTGForm::OnMiddleDivTrain()
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;
	CTG_TRAIN modifyTrain1 = *(m_sFocusTrain);
	CTG_TRAIN modifyTrain2 = *(m_sFocusTrain);

	// ---------------------ǰ�����-------------------------
	vector<TgTRAIN_ENGINE_INFO> tmpEngine;
	tmpEngine.assign(modifyTrain1.myEngine.begin(), modifyTrain1.myEngine.end());

	int nRecNum = modifyTrain1.GetRecordNum();
	for (int i = nRecNum -1; i > nSelRecIndex; i--)
	{
		modifyTrain1.DeleteTailRecord();
	}

	modifyTrain1.myEngine.clear();
	for(int idx=0; idx<tmpEngine.size(); idx++)
	{
		if(tmpEngine[idx].attach_sta < nSelRecIndex)
		{
			if(tmpEngine[idx].unattach_sta <= nSelRecIndex)
			{
				modifyTrain1.myEngine.push_back(tmpEngine[idx]);
			}
			else
			{
				tmpEngine[idx].unattach_sta = nSelRecIndex;
				modifyTrain1.myEngine.push_back(tmpEngine[idx]);
			}
		}
	}

	TgTRAIN_RECORD *rec;
	rec = modifyTrain1.GetLastRecord();
	if(rec == NULL) return;

	if (!IsActualArrivalTime(rec->adjust_status))
	{
		CancelActualDepartTime(rec->adjust_status);
		CancelActualArrivalTime(rec->adjust_status);
	}

	rec->stop_abs_position = 0;
	rec->stop_abs_s_time = 0;
	rec->stop_abs_e_time = 0;
	rec->depart = rec->arrive;
	
	// �յ���ͳ�Ƶ���
	rec->static_arrive=1;
	rec->static_depart=0;

	//2013.8.7 hjz �޸���վʼ�����ϲ���ʼ��վ���������
	if (rec->rec_index!=0)
		rec->flag = 0;
	if(IsFlagStopAbs(rec->flag)) 
		CancelFlagStopAbs(rec->flag);
	if(IsFlagStopSignal(rec->flag)) 
		CancelFlagStopSignal(rec->flag);

	SetFlagTerminal(rec->flag);

	// ���¼���
	rec->out_station_index=rec->in_station_index;
	m_pDoc->SetTrainLastRecIndex(modifyTrain1, true);
	
	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ�ָ�ǰ�����", m_pDoc->GetTrainShowId(modifyTrain1), rec->station);
	m_pDoc->SendUpdateTrain(modifyTrain1);

	// --------------------������--------------------------
	tmpEngine.clear();
	tmpEngine.assign(modifyTrain2.myEngine.begin(), modifyTrain2.myEngine.end());

	modifyTrain2.myEngine.clear();
	for(int idx=0; idx<tmpEngine.size(); idx++)
	{
		if(tmpEngine[idx].unattach_sta > nSelRecIndex)
		{
			tmpEngine[idx].unattach_sta -= nSelRecIndex;
			if(tmpEngine[idx].attach_sta >= nSelRecIndex)
			{
				tmpEngine[idx].attach_sta -= nSelRecIndex;
				modifyTrain2.myEngine.push_back(tmpEngine[idx]);
			}
			else if(tmpEngine[idx].attach_sta < nSelRecIndex)
			{
				tmpEngine[idx].attach_sta = 0;
				modifyTrain2.myEngine.push_back(tmpEngine[idx]);
			}
		}
	}
	
	modifyTrain2.m_nTrainIndex = 0;
	for (int i = 0; i < nSelRecIndex; i++)
	{
		modifyTrain2.DeleteHeadRecord();
	}

	nRecNum = modifyTrain2.GetRecordNum();
	for (int i =  0 ; i < nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = modifyTrain2.GetRecordByPosition(i);
		rec->rec_index = i;
	}

	rec = modifyTrain2.GetFirstRecord();
	if(rec==NULL) return;

	if (!IsActualDepartTime(rec->adjust_status))
	{
		CancelActualDepartTime(rec->adjust_status);
		CancelActualArrivalTime(rec->adjust_status);
	}

	rec->arrive = rec->depart;

	rec->entry = -1;
	//11111111111
	rec->flag = 0;
	SetFlagStart(rec->flag);

	// ͳ�Ʒ���
	rec->static_arrive=0;
	rec->static_depart=1;
	
	// ���¼���
	rec->in_station_index=rec->out_station_index;
	m_pDoc->SetTrainFirstRecIndex(modifyTrain2, true);

	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ�ָ������", m_pDoc->GetTrainShowId(modifyTrain2), rec->station);
	m_pDoc->SendAppendTrain(&modifyTrain2, "", 0);   
}
// ��վʼ��
void CTGForm::OnMiddleStart( ) 
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;
	if(nSelRecIndex == 0)
	{
		TgTRAIN_RECORD *first = m_sFocusTrain->GetFirstRecord();
		if(IsFlagStart(first->flag))
			return;
	}

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);
	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
	{
		for (int i=0;i<=nSelRecIndex;i++)
		{
			TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
			if(rec->adjust_status & 0x0003)
			{
				AfxMessageBox("����ʵ�ʵ�,��ֹ�޸�!");
				return;
			}
		}
	}

	for (int i = 0; i < nSelRecIndex; i++)
	{
		modifyTrain.DeleteHeadRecord();
	}

	int nRecNum = modifyTrain.GetRecordNum();
	for (int i =  0 ; i < nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
		rec->rec_index = i;
	}

	TgTRAIN_RECORD *rec = NULL;
	rec = modifyTrain.GetRecordByPosition(0);
	if(rec==NULL) return;

	rec->arrive = rec->depart;
	
	rec->entry = -1;
	if(IsFlagAbsStart(rec->flag))
	{
		ReSetFlagAbsStart(rec->flag);
		modifyTrain.myTrainInfo.stop_abs_time = 0;
		ReSetAbsStartStop(modifyTrain.myTrainInfo.adjust_flag);
	}
	SetFlagStart(rec->flag);

	// ͳ�Ʒ���
	rec->static_arrive=0;
	rec->static_depart=1;

	// ���¼���
	TgTRAIN_RECORD *nrec;
	nrec = modifyTrain.GetNextStationRecord(rec);
	if (!nrec || !m_pDoc->JudeNodesConnected(rec->out_station_index, nrec->in_station_index))
	{
		rec->in_station_index=NULL_INDEX;
		rec->out_station_index=NULL_INDEX;	
	}
	m_pDoc->SetTrainFirstRecIndex(modifyTrain, true);

	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվʼ��", m_pDoc->GetTrainShowId(m_sFocusTrain), rec->station);
	// ���㳵
	if(modifyTrain.GetRecordNum()==1)
	{
		TgTRAIN_RECORD *last = modifyTrain.GetLastRecord();
		if(IsFlagTerminal(last->flag))
		{
			CString text;
			text.Format("��ע��, �г�������ͬһ��վ ��ʼ��, ���յ� \n �������޸� %s �� %s ʼ��ʧ��!", 
				m_pDoc->GetTrainShowId(modifyTrain), m_pDoc->GetStationName(last->station));
			m_pDoc->sysprint(10,0, "%s", text);
			MessageBox(text, "����", MB_ICONHAND|MB_OK);
			return;
		}
	}

	// �ӳ���·�Դ�
	if(IsAutoMakeArriveRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�޸��г���վʼ�� %s���г� %s �ӳ���·�������Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetNotAutoMakeArriveRoute(rec->stop_condition);
	}

	m_pDoc->SendUpdateTrain(modifyTrain);     
}

// ��վ����
void CTGForm::OnMiddleFetch( ) 
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	const int nSelRecIndex = m_nFocusRecordIndex;

	CTG_TRAIN modifyTrain = *(m_sFocusTrain);

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime() && m_pDoc->m_ptgctrl->mConfigTg.m_context.IsProhibitDeleteRealTime())
	{
		for (int i=0;i<=nSelRecIndex;i++)
		{
			TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
			if(rec->adjust_status & 0x0003)
			{
				AfxMessageBox("����ʵ�ʵ�,��ֹ�޸�!");
				return;
			}
		}
	}

	for (int i = 0 ; i < nSelRecIndex; i++)
	{
		modifyTrain.DeleteHeadRecord();
	}

	int nRecNum = modifyTrain.GetRecordNum() ;
	for (int i =  0 ; i < nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = modifyTrain.GetRecordByPosition(i);
		rec->rec_index = i;
	}

	TgTRAIN_RECORD *rec = NULL;
	rec = modifyTrain.GetFirstRecord();
	if(rec == NULL)
		return;

	if(IsFlagAbsStart(rec->flag))
	{
		ReSetFlagAbsStart(rec->flag);
		modifyTrain.myTrainInfo.stop_abs_time = 0;
		ReSetAbsStartStop(modifyTrain.myTrainInfo.adjust_flag);
	}

	SetFlagAccept(rec->flag);

	SetTrainEntry(&modifyTrain, rec->entry);

	// ���¼���
	TgTRAIN_RECORD *nrec;
	nrec = modifyTrain.GetNextStationRecord(rec);
	if (!nrec || !m_pDoc->JudeNodesConnected(rec->out_station_index, nrec->in_station_index))
	{
		rec->in_station_index=NULL_INDEX;
		rec->out_station_index=NULL_INDEX;	
	}

	// �ӳ���·�Դ�
	if(!IsAutoMakeArriveRoute(rec->stop_condition))
	{
		m_pDoc->sysprint(4,0,"����Ա�޸� %s���г����� %s �ӳ���·�����Դ�", 
			m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station));

		SetAutoMakeArriveRoute(rec->stop_condition);
	}

	m_pDoc->SetTrainFirstRecIndex(modifyTrain, true);
	

	m_pDoc->sysprint(10,0, "����Ա�����г�%s��%dվ����, entry=%d", m_pDoc->GetTrainShowId(m_sFocusTrain), rec->station, rec->entry);
	m_pDoc->SendUpdateTrain(modifyTrain);  
}


////end of rescue train
LRESULT CTGForm::OnTrainsOperator(WPARAM wParam,LPARAM lParam)
{
	short oper= wParam;
	short rst = lParam;
	if(oper == TWO_TRAIN_OPERATOR::UNITE_TRAIN)
	{
		if(m_sFocusTrain == NULL) 
			return 0;
		if(rst == TWO_TRAIN_OPERATOR::RESULT::OPER_OK)
		{
			this->m_bUniteTrainStatus = true;
		}
	}
	else if(oper == TWO_TRAIN_OPERATOR::RESCUE_TRAIN)
	{
		if(m_sFocusTrain == NULL) 
			return 0;
		if(rst == TWO_TRAIN_OPERATOR::RESULT::OPER_OK)
		{
			this->m_bRescueTrainStatus = true;
		}
	}
	else if(oper == TWO_TRAIN_OPERATOR::SPLIT_TRAIN)
	{
		if(rst == TWO_TRAIN_OPERATOR::RESULT::OPER_OK)
			OnSplitTrain();
	}
	else if(oper == TWO_TRAIN_OPERATOR::COMBINE_TRAIN)
	{
		if(rst == TWO_TRAIN_OPERATOR::RESULT::OPER_OK)
			OnCombineTrain();
	}
	else if(oper == TWO_TRAIN_OPERATOR::IN_ABS_PART_RUN)
	{
		OnAbsPartRun();
	}
	else if(oper == TWO_TRAIN_OPERATOR::ATT_ENGINE_RETURN)
	{
		OnAttainEngineReturn();
	}
	else if(oper == TWO_TRAIN_OPERATOR::IN_ABS_WHOLE_HELP_RUN)
	{
		OnAbsWholeHelpRun();
	}
	
	
	return 0;
}

void CTGForm::EraseTrain(const CTG_TRAIN& train)
{
	m_pDoc->EraseTrainIndex(train.m_nTrainIndex);
	CRect rangeRect, interRect;
	if(m_pctrl->IsWorkMode())
	{
		RedrawLayer2();
		
		rangeRect=train.myPosition[0];
		if(train.mbDoubleDirect)
		{
			m_Layer3.FillRect(rangeRect, &m_pDoc->m_brushWhite);
			std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
			for(;it != myPDataManage->myTrain.end();++it)
			{
				if(train.mbDoubleDirect != it->second.mbDoubleDirect)
					continue;
				
				if(it->second.GetTrainIndex() == train.m_nTrainIndex) 
					continue;

				if(interRect.IntersectRect(&rangeRect, &it->second.myPosition[0]))
				{
					m_pDoc->DrawSingleTrain(&m_Layer3, &it->second, DRAW_NORMAL);
				}
			}
		}
		else if(train.m_nDirectFlag==UP_DIRECT)
		{
			m_Layer4.FillRect(rangeRect, &m_pDoc->m_brushWhite);
			std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
			for(;it != myPDataManage->myTrain.end();++it)
			{
				if(it->second.mbDoubleDirect)
					continue;

				if(it->second.m_nDirectFlag!=UP_DIRECT)
					continue;
				
				if(it->second.GetTrainIndex() == train.m_nTrainIndex) 
					continue;

				if(interRect.IntersectRect(&rangeRect, &it->second.myPosition[0]))
				{
					m_pDoc->DrawSingleTrain(&m_Layer4, &it->second, DRAW_NORMAL);
				}
			}
		}
		else
		{
			m_Layer2.FillRect(rangeRect, &m_pDoc->m_brushWhite);
			std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
			for(;it != myPDataManage->myTrain.end();++it)
			{
				if(it->second.mbDoubleDirect)
					continue;

				if(it->second.m_nDirectFlag!=DOWN_DIRECT)
					continue;
				
				if(it->second.GetTrainIndex() == train.m_nTrainIndex) 
					continue;

				if(interRect.IntersectRect(&rangeRect, &it->second.myPosition[0]))
				{
					m_pDoc->DrawSingleTrain(&m_Layer2, &it->second, DRAW_NORMAL);
				}
			}
		}
		
		//���Ƴ��鹴����
		m_pDoc->DrawAllCheZuHaoLine(&m_Layer3);
	}
	else
	{
		for(int k = 0; k < train.days; k++)
			OnDrawBackgrondExt(&m_Layer1, train.myPosition[k]); // �߳��г�

		// ˢ��single�г�����
		std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
		for(;it != myPDataManage->myTrain.end();++it)
		{
			if(it->second.GetTrainIndex() == train.m_nTrainIndex) 
				continue;

			bool bContinue = true;
			for(int k = 0; k < train.days; k++)
			{
				rangeRect=train.myPosition[k];
				for(int i = 0; i < it->second.days; i++)
				{
					if(rangeRect.IntersectRect(&train.myPosition[k], &it->second.myPosition[i]))
					{
						bContinue = false;
						m_pDoc->DrawSingleTrain(&m_Layer1, &it->second, DRAW_NORMAL);
						break;
					}
				}
				if(!bContinue)
					break;
			}
		}

		//���Ʊ�־
		std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
		for(;it_block != myPDataManage->myMark.end();++it_block)
		{
			for(int k = 0; k < train.days; k++)
			{
				rangeRect=train.myPosition[k];
				if(rangeRect.IntersectRect(&train.myPosition[k], &it_block->second.myPosition))
				{
					m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
					break;
				}
			}
		}

		//���Ƴ��鹴����
		m_pDoc->DrawAllCheZuHaoLine(&m_Layer1);
	}

	this->RefreshTrain(&train);
	m_pDoc->EraseTrainIndex(0);
}

void CTGForm::EraseRegion(const CRect& region)
{
	OnDrawBackgrondExt(&m_Layer1, region); // ���»�������

	CRect rangeRect;
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for(;it != myPDataManage->myTrain.end();++it)
	{
		for(int i = 0; i < it->second.days; i++)
		{
			if(rangeRect.IntersectRect(&region, &it->second.myPosition[i]))
			{
				m_pDoc->DrawSingleTrain(&m_Layer1, &it->second, DRAW_NORMAL);
				break;
			}
		}
	}

	//���Ʊ�־
	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	for(;it_block != myPDataManage->myMark.end();++it_block)
	{
		if(rangeRect.IntersectRect(&region, &it_block->second.myPosition))
		{
			m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
		}
	}

	//���Ƴ��鹴����
	m_pDoc->DrawAllCheZuHaoLine(&m_Layer1);
	
	RefreshRect(region);
}

void CTGForm::DrawTrain(CTG_TRAIN& train, bool bFlag)
{
	long nFlag = bFlag ? DRAW_SCREEN : DRAW_NORMAL;

	if(m_pctrl->IsWorkMode())
	{
		if(train.mbDoubleDirect)
			m_pDoc->DrawSingleTrain(&m_Layer3, &train, nFlag);
		else if(train.m_nDirectFlag==UP_DIRECT)
			m_pDoc->DrawSingleTrain(&m_Layer4, &train, nFlag);
		else
			m_pDoc->DrawSingleTrain(&m_Layer2, &train, nFlag);
	}
	else
	{
		m_pDoc->DrawSingleTrain(&m_Layer1, &train, nFlag);
	}
	RefreshTrain(&train);
}

void CTGForm::EraseBlock(const CTg_BLOCK& block)
{
	CRect clientRect;
	clientRect = block.myPosition;
	//clientRect.InflateRect(m_pDoc->m_fPixelPerHour,m_pDoc->m_fPixelPerHour,
	//	m_pDoc->m_fPixelPerHour,m_pDoc->m_fPixelPerHour);

	if(m_pctrl->IsWorkMode())
	{
		OnDrawBackgrondExt(&m_Layer1, clientRect);
	
		//���Ʊ�־
		CRect rangeRect;
		std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
		for(;it_block != myPDataManage->myMark.end();++it_block)
		{
			if(block.mark_index == it_block->second.mark_index)
				continue;
			if(rangeRect.IntersectRect(&clientRect, &it_block->second.myPosition))
			{
				m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
			}
		}
	}
	else
	{
		OnDrawBackgrondExt(&m_Layer1, clientRect);  // �߳�ʩ������

		CRect rangeRect;
		std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
		for(; it != myPDataManage->myTrain.end(); ++it)
		{
			for(int i = 0; i < it->second.days; i++)
			{
				if(rangeRect.IntersectRect(&clientRect, &it->second.myPosition[i]))
				{
					m_pDoc->DrawSingleTrain(&m_Layer1, &it->second, DRAW_NORMAL);
					break;
				}
			}
		}

		//���Ʊ�־
		std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
		for(;it_block != myPDataManage->myMark.end();++it_block)
		{
			if(block.mark_index == it_block->second.mark_index)
				continue;
			if(rangeRect.IntersectRect(&clientRect, &it_block->second.myPosition))
			{
				m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
			}
		}

		//���Ƴ��鹴����
		m_pDoc->DrawAllCheZuHaoLine(&m_Layer1);
	}

	RefreshRect(block.myPosition);
}

void CTGForm::DrawBlock(CTg_BLOCK& block)
{
	m_pDoc->DrawSingleMark(block, &m_Layer1);
	RefreshRect(block.myPosition);
}

void CTGForm::OnDrawStationAixis(CDC* pDC, const CRect& DrawRect, ULONG lColor)
{
	if (m_pDoc->m_nDispDistanceNum <= 0) // ����Ҫ!!!
		return;
	
	if (m_pDoc->m_bShowVirtualStation)
		DrawVirtulStationAixis(pDC, lColor);

	UINT nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);

	// �ɵ���
	CPen nDotPenEleArm;
	nDotPenEleArm.CreatePen(PS_DOT,1,lColor);
	
	CPen *pOldPen = pDC->SelectObject(&nDotPenEleArm);
	UINT OldTextColor = pDC->SetTextColor(lColor);

	for(int i=0;i < m_pDoc->m_nDispDistanceNum;i++)
	{
		int nWidth = m_pDoc->m_nPageWidth;

		// 0:������(Ĭ��) 1:�� 2:�� 3:����
		if(m_pDoc->m_sDispDistance[i].electric_arms == 1)
		{
			pDC->MoveTo(X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);
			pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);
		}
		else if(m_pDoc->m_sDispDistance[i].electric_arms == 2)
		{
			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
			}
			else
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos+3);
			}
		}
		else if(m_pDoc->m_sDispDistance[i].electric_arms == 3)
		{
			pDC->MoveTo(DrawRect.left+X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos-4);
			pDC->LineTo(DrawRect.right + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);

			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
			}
			else
			{
				pDC->MoveTo(0+X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos+3);
			}
		}

		
		COLORREF tmpClr = lColor;
		if(m_pDoc->m_sDispDistance[i].clrStationLine!=-1)
		{
			tmpClr=m_pDoc->m_sDispDistance[i].clrStationLine;
		}

		// ��վ��
		CPen mStationPen;
		if(m_pDoc->m_sDispDistance[i].isNeedThick == 1)
		{
			mStationPen.CreatePen(PS_SOLID,2,tmpClr);//����
		}
		else if(m_pDoc->m_sDispDistance[i].isNeedThick == 2)
		{
			mStationPen.CreatePen(PS_DOT,1,tmpClr);//����
		}
		else
		{
			mStationPen.CreatePen(PS_SOLID,1,tmpClr);//ϸ�� 
		}

		CPen* pElePen = pDC->SelectObject(&mStationPen);

		if(DrawRect.top <= m_pDoc->m_sDispDistance[i].nStationYPos && 
			m_pDoc->m_sDispDistance[i].nStationYPos <= DrawRect.bottom)
		{
			pDC->MoveTo(DrawRect.left,  m_pDoc->m_sDispDistance[i].nStationYPos);
			pDC->LineTo(DrawRect.right, m_pDoc->m_sDispDistance[i].nStationYPos);
		}

		if(m_pDoc->m_sDispDistance[i].show_sation_sep)
		{
			if(DrawRect.top <= m_pDoc->m_sDispDistance[i].nSecondStationYPos && 
				m_pDoc->m_sDispDistance[i].nSecondStationYPos <= DrawRect.bottom)
			{
				pDC->MoveTo(DrawRect.left,  m_pDoc->m_sDispDistance[i].nSecondStationYPos);
				pDC->LineTo(DrawRect.right, m_pDoc->m_sDispDistance[i].nSecondStationYPos);
			}
			if(m_pDoc->m_sDispDistance[i].show_sides_mode ==0)  
			{			
				for (int n = 0;n < m_pDoc->m_sDispDistance[i].number_of_side;n++)
				{
					int nSide = m_pDoc->m_sDispDistance[i].side[n].side_number;
					int nProperty = m_pDoc->m_sDispDistance[i].side[n].side_property;
					int nDirect = m_pDoc->m_sDispDistance[i].side[n].direct;
					CString strSideName = m_pDoc->m_sDispDistance[i].side[n].sidename;

					COLORREF sideClr = m_pDoc->m_ptgctrl->mConfigTg.GetSideColor(nProperty, nDirect, lColor);
					CPen nSidePen;
					nSidePen.CreatePen(PS_DOT,1, sideClr); 

					CPen* pStaPen = pDC->SelectObject(&nSidePen);
					
					if(DrawRect.top <= m_pDoc->m_sDispDistance[i].nSideYPos[nSide] && 
						m_pDoc->m_sDispDistance[i].nSideYPos[nSide] <= DrawRect.bottom)
					{
						pDC->MoveTo(0,  m_pDoc->m_sDispDistance[i].nSideYPos[nSide]);
						pDC->LineTo(nWidth,  m_pDoc->m_sDispDistance[i].nSideYPos[nSide]);
					}
					
					pDC->SelectObject(pStaPen);
					nSidePen.DeleteObject();

					DrawSideNamePerFourHour(pDC,  m_pDoc->m_sDispDistance[i].nSideYPos[nSide], strSideName);
				}
			}
		}
		pDC->SelectObject(pElePen);
		mStationPen.DeleteObject();
	}
	
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(OldTextColor);

	nDotPenEleArm.DeleteObject();
}

// �ڹɵ�չ��ʽʱ��ÿ4Сʱ��ʾ�ɵ��ţ�lxl add begin 20100311
void CTGForm::DrawSideNamePerFourHour(CDC* pDC, int nY, CString& sideName)
{
	int nStep=0;
    if(m_pctrl->mConfigTg.m_context.IsDrawSideNamePerFourHours())
	{
		nStep=4;
	}
	if(m_pctrl->mConfigTg.m_context.m_bDrawSideNamePerHours)
	{
		nStep=1;
	}

	if(nStep==0)
		return;
	if(nY<=0 || nY>=m_pDoc->m_nPageHeight || sideName.IsEmpty())
		return;

	CFont m_fontSideno; 
	m_fontSideno.CreatePointFont(60,_T("Arial"));
	
	CFont *pOldFont = pDC->SelectObject(&m_fontSideno);
	UINT textAlign = pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clr = pDC->SetTextColor(CLR_GRAY);

	int nStepPix = m_pDoc->m_fPixelPerHour*nStep;
	for (int nX=10; nX<m_pDoc->m_nPageWidth; nX=nX+nStepPix)
	{
		pDC->TextOut(nX, nY+5, sideName);
	}

	pDC->SelectObject(pOldFont);
	m_fontSideno.DeleteObject();
	pDC->SetTextAlign(textAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SetTextColor(clr);
}

int CTGForm::CalcAixixY()
{
	int nAixixYNum = -1;
	int routeSetNo = 9999;
	BYTE nMax = sizeof(AixisY)/sizeof(AixisY[0]);
	for(BYTE i = 0; i < nMax; i++)
	{
		AixisY[i].nTopY = 0;
		AixisY[i].nBottomY = 0;
	}
	//memset(AixisY, 0, sizeof(AixisY));

	if (m_pDoc->m_nDispDistanceNum <= 0) // ����Ҫ!!!
		return nAixixYNum;
	
	for(int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
	{
		if(routeSetNo == m_pDoc->m_sDispDistance[i].sub_section)
		{
			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				AixisY[nAixixYNum].nBottomY = m_pDoc->m_sDispDistance[i].nStationYPos;

				if (nAixixYNum+1 > nMax)
				{
					CString str;
					str.Format("����ͼ�ֶι���>%d", sizeof(AixisY)/sizeof(AixisY[0]));
					AfxMessageBox(str);
				}

				nAixixYNum++;
				AixisY[nAixixYNum].nTopY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nBottomY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
			}
			else
			{
				AixisY[nAixixYNum].nBottomY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
			}
		} 
		else 
		{
			routeSetNo = m_pDoc->m_sDispDistance[i].sub_section;

			if (nAixixYNum+1 > nMax)
			{
				CString str;
				str.Format("����ͼ�ֶι���>%d", sizeof(AixisY)/sizeof(AixisY[0]));
				AfxMessageBox(str);
			}

			nAixixYNum++;
			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				AixisY[nAixixYNum].nBottomY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nTopY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
			}
			else
			{
				AixisY[nAixixYNum].nBottomY = m_pDoc->m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nTopY = m_pDoc->m_sDispDistance[i].nStationYPos;
			}
		}
	}

	return nAixixYNum;
}

void CTGForm::OnDrawTimeAixis(CDC* pDC, const CRect& DrawRect, ULONG lColor)
{
	int nAixixYNum = CalcAixixY();

	int nStart=0, nEnd=nAixixYNum;
	for(int k = 0; k <= nAixixYNum; k++)
	{
		if(DrawRect.top > AixisY[k].nTopY)
		{
			nStart = k;
			break;
		}
	}
	for(int k = 0; k <= nAixixYNum; k++)
	{
		if(DrawRect.bottom < AixisY[k].nBottomY)
		{
			nEnd = k;
			break;
		}
	}

	if(nStart > nEnd) return;

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  1, lColor);
	mBoldPen.CreatePen(PS_SOLID, 2, lColor);

	pOldPen = pDC->SelectObject(&mThinPen);

	int nStartX = DrawRect.left;
	if(DrawRect.left%m_pDoc->m_fPixelPerTenMinute)
		nStartX += (m_pDoc->m_fPixelPerTenMinute - DrawRect.left%m_pDoc->m_fPixelPerTenMinute);

	for(int aixisX = nStartX; aixisX < DrawRect.right; aixisX += m_pDoc->m_fPixelPerTenMinute)
	{
		if(aixisX%m_pDoc->m_fPixelPerHour == 0)
			pDC->SelectObject(&mBoldPen);
		else if(aixisX%m_pDoc->m_fPixelPerHalfHour == 0)
			pDC->SelectObject(&mDashPen);
		else
		{
			if(m_pDoc->m_nADTimeSystle == 2)
			{
				continue; // �Ż���Сʱ����Ҫ��ʮ�ָ���
			}
			pDC->SelectObject(&mThinPen);
		}

		for(int k = nStart; k <= nEnd; k++)
		{
			pDC->MoveTo(aixisX, AixisY[k].nTopY);			
			pDC->LineTo(aixisX, AixisY[k].nBottomY);
		}
	}

	pDC->SelectObject(pOldPen);
	mThinPen.DeleteObject();
	mDashPen.DeleteObject();
	mBoldPen.DeleteObject();
}

void CTGForm::RefreshCzhOtherTrain()
{
	TRAIN_CHEZUHAO* pFocusCzh = m_pDoc->GetCurrentSelTrainCZH();
	if(pFocusCzh == NULL)
		return;
	
	CRect rect;
	rect.SetRectEmpty();
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it=pFocusCzh->trains.begin();
	for( ; it!=pFocusCzh->trains.end(); it++)
	{
		CTG_TRAIN* ptrain = m_pDoc->GetTrainByIndex(it->second.train_index);
		if(ptrain)
		{
			for(int i=0; i<ptrain->days; i++)
			{
				rect.UnionRect(&rect, &(ptrain->myPosition[i]));
			}
		}
	}

	if(!rect.IsRectEmpty())
	{
		RefreshRect(rect);
	}
}

void CTGForm::DrawCzhOtherTrain(CDC* pDC, ULONG train_index)
{
	TRAIN_CHEZUHAO* pFocusCzh = m_pDoc->GetCurrentSelTrainCZH();
	if(pFocusCzh == NULL)
		return;
	
	m_pDoc->m_bLineBold=true; // �����Ӵ�
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it=pFocusCzh->trains.begin();
	for( ; it!=pFocusCzh->trains.end(); it++)
	{
		if(it->second.train_index==train_index)
		{
			continue;
		}
		CTG_TRAIN* pOtherTrain = m_pDoc->GetTrainByIndex(it->second.train_index);
		if(pOtherTrain)
		{
			m_pDoc->DrawSingleTrain(pDC, pOtherTrain, DRAW_SCREEN, false);
		}
	}
	m_pDoc->m_bLineBold=false; // �����Ӵָֻ�
}

void CTGForm::OnDraw(CDC* pDC)
{
	CRect ClipRect, tempZero(0,0,0,0);
	pDC->GetClipBox(&ClipRect);

	if(ClipRect != tempZero)
	{
		if(m_pctrl->IsWorkMode())
		{
			// ��ʾ�г�ʱ,����ͼ�г�
			if(m_pDoc->IsOnlyShowTrain() || m_pDoc->IsShowTrainAndMark())
			{
				if(m_pDoc->GetShowTrainDirect() == CTGDoc::TRAIN_DIRECT::DUAL || m_pDoc->GetShowTrainDirect() == CTGDoc::TRAIN_DIRECT::UP)
					m_Layer1.BitBlt(ClipRect.left, ClipRect.top, ClipRect.Width(), ClipRect.Height(), &m_Layer4, ClipRect.left , ClipRect.top, SRCAND);
				if(m_pDoc->GetShowTrainDirect() == CTGDoc::TRAIN_DIRECT::DUAL || m_pDoc->GetShowTrainDirect() == CTGDoc::TRAIN_DIRECT::DOWN)
					m_Layer1.BitBlt(ClipRect.left, ClipRect.top, ClipRect.Width(), ClipRect.Height(), &m_Layer2, ClipRect.left , ClipRect.top, SRCAND);
			
				m_Layer1.BitBlt(ClipRect.left, ClipRect.top, ClipRect.Width(), ClipRect.Height(), &m_Layer3, ClipRect.left , ClipRect.top, SRCAND);
			}
		}

		pDC->BitBlt(ClipRect.left, ClipRect.top, ClipRect.Width(), ClipRect.Height(), &m_Layer1, ClipRect.left , ClipRect.top, SRCCOPY);
	}

	for(int i = 0; i < m_selTrains.size(); i++)
	{
		m_pDoc->DrawSingleTrain(pDC, &m_selTrains[i], DRAW_SCREEN); 
	}
	// ���ƽ����г�
	if(m_sFocusTrain != NULL)
	{
		if( !IsTraintDeleteByOtherGPC(m_sFocusTrain))
		{
			if(m_pDoc->m_nADTimeSystle == 3)
			{
				m_pDoc->DrawSingleTrain(pDC, m_sFocusTrain, DRAW_SCREEN, true);
			}
			else
			{
				m_pDoc->DrawSingleTrain(pDC, m_sFocusTrain, DRAW_SCREEN, false);
			}
			// ����ͬ���������г�,�г����ϵ�ҿ��ش򿪵������
			if(m_pctrl->mConfigTg.m_context.m_bRelationTrainBold)
			{
				DrawCzhOtherTrain(pDC, m_sFocusTrain->m_nTrainIndex);
			}
		}

		for(int i=0; i<m_focusCzhTrains.m_focusCzhTrains.size(); i++)
		{
			if(m_sFocusTrain->GetTrainIndex()==m_focusCzhTrains.m_focusCzhTrains[i].GetTrainIndex())
				continue; // �����Ѿ����͹�
			m_pDoc->DrawSingleTrain(pDC, &m_focusCzhTrains.m_focusCzhTrains[i], DRAW_NORMAL);
		}

		for(int i=0; i<m_sFocusOtherTrain.size(); i++)
		{
			m_pDoc->DrawSingleTrain(pDC, &(m_sFocusOtherTrain[i]), DRAW_NORMAL);
		}
	}

	// ���ƽ�����
	if(m_nFocusFigure != NULL)
	{
		if(!IsUpdateByOtherGPC(m_nFocusFigure) && !IstDeleteByOtherGPC(m_nFocusFigure))
		{
			m_pDoc->DrawSingleMark(*m_nFocusFigure, pDC);
		}
	}

	//����Ǽ��������������ô�������һ��
	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN && m_sFocusTrain != NULL)
	{
	//	m_pDoc->DrawSingleTrain(pDC, m_sFocusTrain, DRAW_SCREEN | DRAW_NORMAL);  // �Ż�����ͼЧ�� --- ���໭
		if(m_sFocusTrain->m_nPointCount >= 1 
			&& m_pContinuePoint != CPoint(-1,-1)
			&& m_pOldMousePoint != CPoint(-1,-1))
		{ 	
			if(m_sFocusTrain->m_nPointCount > 0)
				m_pContinuePoint = m_sFocusTrain->m_pPerPointInfo[m_sFocusTrain->m_nPointCount-1].point;
			DrawLine(*pDC,m_pContinuePoint,m_pOldMousePoint);
		}
	}

	DrawSelTrainCZHLine();
}

void CTGForm::FlashTrain()
{
	int times=0;
	if(!m_trainbasicflash.IsFlash(times))
		KillTimer(TIMER_IDEVENT_FLASH);
	
	if(m_trainbasicflash.GetRecordNum()<=0)
		return;

	if(times%2)
		DrawTrain(m_trainbasicflash, false);
	else
		EraseTrain(m_trainbasicflash);
}

BOOL CTGForm::OnEraseBkgnd(CDC* pCDC) 
{
	return TRUE;	
}

void CTGForm::RefreshTrain(const CTG_TRAIN* ptrain)
{
	if(NULL != ptrain)
	{
		for(int i=0; i<ptrain->days; i++)
			RefreshRect(ptrain->myPosition[i]);
	}
}

void CTGForm::RefreshRect(const CRect& rect, BOOL bErase)
{
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	CRect invalidRect = rect;
	invalidRect.left -= 5;
	if(invalidRect.left < 0)
		invalidRect.left = 0;
	invalidRect.right +=5;
	clientDC.LPtoDP(&invalidRect);
	invalidRect.right += 200;
	InvalidateRect(&invalidRect, bErase);
}

void CTGForm::MouseModifyMutiTrain(const CPoint point)
{
	if(m_selTrains.size()<=0)
		return;

	long usLastTimeValue = m_pDoc->ConvertXPostoTime(mMutiMoveLastPoint.x);
	long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
	int  nTimeOffset = usTimeValue-usLastTimeValue;
	nTimeOffset = (nTimeOffset/60)*60;
	if(abs(nTimeOffset) < 60)
		return;
	mMutiMoveLastPoint = point;

	CRect rect(0,0,0,0);
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	for(int i = 0; i < m_selTrains.size(); i++)
	{
		m_selTrains[i].OffsetWorkScheduleTime(nTimeOffset);
	}

	rect.IsRectEmpty();
	for(int i = 0; i < m_selTrains.size(); i++)
	{
		rect.UnionRect(rect, m_selTrains[i].myPosition[0]);
	}
	this->RefreshRect(rect);
}

void CTGForm::MouseModifyBasicTrain(const CPoint point)
{
	if((m_sFocusTrain==NULL) || (m_nFocusRecordIndex<0))	
		return;

	int nFoucsRecordNo=m_nFocusRecordIndex;
	m_pDoc->m_myFocusTrain=*m_sFocusTrain;

	TgTRAIN_RECORD *pre  = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo-1);
	TgTRAIN_RECORD *rec  = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo);
	TgTRAIN_RECORD *next = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo+1);
	if(rec == NULL) 
		return;

	int nStationNo   = rec->station;
	int nAllRecordNo = m_sFocusTrain->GetRecordNum();
	
	bool IsCanMoveWholeTrain = this->m_pctrl->mConfigTg.IsCanMoveBasciTrainWhole();
	
	bool bLimiteMove = this->m_pctrl->mConfigTg.m_context.IsLimitNxtNodeBackPreNode();
	
	bool bCheckTrainCZHRelation=true;
	if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainCZHValid(m_sFocusTrain, str);
	}
	else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainRelationValid(m_sFocusTrain, str);
	}
	
	long usTimeValue = m_pDoc->ConvertXPostoTime(point.x)%86400;
	int time_arrival=rec->arrive%86400;
	int time_depart=rec->depart%86400;

	long nTimeOffset=0;
	if(m_nMouseDownPosition == UNION_POSITION)
	{
		if(nFoucsRecordNo==0 && IsFlagStart(rec->flag))
		{
			if(time_depart>=86100 && usTimeValue==0)
				usTimeValue=86400;
			if(time_depart<=300 && usTimeValue==0)
				usTimeValue=0;
			
			nTimeOffset = ((usTimeValue - time_depart)/60)*60;
		}
		else
		{
			if(time_arrival>=86100&&usTimeValue==0)
				usTimeValue=86400;
			if(time_arrival<=300&&usTimeValue==0)
				usTimeValue=0;

			nTimeOffset = ((usTimeValue - time_arrival)/60)*60;
		}

		if(nTimeOffset>=86100)
			nTimeOffset-=86400;

		if(nTimeOffset==0)
			return;

		long arrive=rec->arrive + nTimeOffset;
		if(pre != NULL) 
		{
			if(pre->depart>=arrive)
			{
				OnLButtonUp(0, point);
				MessageBox("��վͨ��ʱ��Ӧ����ǰһվ����ʱ��", "��ʾ", MB_OK);
				return;
			}
		}
		if(next != NULL) 
		{
			if(next->arrive<=arrive)
			{
				OnLButtonUp(0, point);
				MessageBox("��վͨ��ʱ��ӦС�ں�һվ����ʱ��", "��ʾ", MB_OK);
				return;
			}
		}

		m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
		if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
		{
			if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}
		else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}

		this->RefreshTrain(m_sFocusTrain);
	}
	else if(m_nMouseDownPosition == ARRIVAL_POSITION)
	{
		if(nFoucsRecordNo==0 && IsFlagStart(rec->flag))
		{
			if(time_depart>=86100 && usTimeValue==0)
				usTimeValue=86400;
			if(time_depart<=300 && usTimeValue==0)
				usTimeValue=0;
			
			nTimeOffset = ((usTimeValue-time_depart)/60)*60;
		}
		else
		{
			if(time_arrival>=86100&&usTimeValue==0)
				usTimeValue=86400;
			if(time_arrival<=300&&usTimeValue==0)
				usTimeValue=0;

			nTimeOffset = ((usTimeValue-time_arrival)/60)*60;
		}
		
		if(nTimeOffset>=86100)
			nTimeOffset-=86400;

		if(nTimeOffset==0)
			return;
		
		long arrive=rec->arrive + nTimeOffset;
		if(pre != NULL) 
		{
			if(pre->depart>=arrive)
			{
				OnLButtonUp(0, point);
				MessageBox("��վ����ʱ��Ӧ����ǰһվ����ʱ��", "��ʾ", MB_OK);
				return;
			}
		}
		if(arrive>(rec->depart-60))
		{
			OnLButtonUp(0, point);
			MessageBox("����ӦС�ڷ���", "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}

		if(!IsCanMoveWholeTrain)
		{
			m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 0);
		}
		else
		{
			m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
		}

		if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
		{
			if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}
		else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}

		this->RefreshTrain(m_sFocusTrain);
		
	}
	else if(m_nMouseDownPosition == DEPART_POSITION) 
	{
		if(time_depart>=86100 && usTimeValue==0)
			usTimeValue=86400;
		if(time_depart<=300 && usTimeValue==0)
			usTimeValue=0;

		nTimeOffset = ((usTimeValue-time_depart)/60)*60;		
		
		if(nTimeOffset>=86100)
			nTimeOffset-=86400;
		if(nTimeOffset==0)
			return;

		long depart=rec->depart + nTimeOffset;
		if(next != NULL) 
		{
			if(next->arrive<=depart)
			{
				OnLButtonUp(0, point);
				MessageBox("��վ����ʱ��ӦС�ں�һվ����ʱ��", "��ʾ", MB_OK);
				return;
			}
		}
		if((rec->arrive+60)>depart)
		{
			OnLButtonUp(0, point);
			MessageBox("����Ӧ���ڵ���", "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}

		m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 1);
		
		// �����г��ƶ�
		if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
		{
			if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}
		else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}
		
		this->RefreshTrain(m_sFocusTrain);
	}
}

bool CTGForm::CheckFocusCZHRelation(CString& str)
{
	str="";
	//�ж�����
	int num=m_focusCzhTrains.m_focusCzhTrains.size();
	if(num<=0)
		return false;

	TgTRAIN_RECORD* phead=m_focusCzhTrains.m_focusCzhTrains[0].GetFirstRecord();
	TgTRAIN_RECORD* ptail=m_focusCzhTrains.m_focusCzhTrains[num-1].GetLastRecord();
	if(phead==NULL || ptail==NULL)
		return false;
	
	int bstation=0, estation=0;
	int stTime=0, edTime=0;
	if(IsFlagStart(phead->flag))
	{
		stTime=phead->depart;
		bstation=phead->station;
	}
	else
	{
		stTime=phead->arrive;
		bstation=0;
	}

	if(IsFlagTerminal(ptail->flag))
	{
		edTime=ptail->arrive;
		estation=ptail->station;
	}
	else
	{
		edTime=ptail->depart;
		estation=0;
	}
	
	if(m_focusCzhTrains.mLeftTime<stTime && edTime<m_focusCzhTrains.mRightTime)
		return true;

	CString czh=m_focusCzhTrains.m_focusCzhTrains[0].myTrainInfo.shift_index_string;
	if(stTime<=m_focusCzhTrains.mLeftTime+60)
	{
		str.Format("ͬ�����г� %s �������� %s",  m_pDoc->GetTrainShowId(m_focusCzhTrains.m_focusCzhTrains[0]), m_focusCzhTrains.mStrLeftTrainID);
		return false;
	}

	if(m_focusCzhTrains.mRightTime<=edTime+60)
	{
		str.Format("ͬ�����г� %s �������� %s",  m_pDoc->GetTrainShowId(m_focusCzhTrains.m_focusCzhTrains[num-1]), m_focusCzhTrains.mStrRightTrainID);
		return false;
	}
	return false;
}

void CTGForm::OffsetFocusSameCZHTrain(long offset)
{
	if(NULL == m_sFocusTrain)
		return;
	// ƽ��ͬ��������г�
	for(int i=0; i<m_focusCzhTrains.m_focusCzhTrains.size(); i++)
	{
		if(m_focusCzhTrains.m_focusCzhTrains[i].m_nTrainIndex==m_sFocusTrain->m_nTrainIndex)
		{
			m_focusCzhTrains.m_focusCzhTrains[i]=*m_sFocusTrain;
			continue;
		}
		this->RefreshTrain(&(m_focusCzhTrains.m_focusCzhTrains[i]));
		m_focusCzhTrains.m_focusCzhTrains[i].OffsetWorkScheduleTime(offset);
	}
	
	return;
}

bool CTGForm::CheckTrainRelation(int nTimeOffset, bool bCheckTrainCZHRelation, CPoint point)
{
	if(NULL==m_sFocusTrain)
		return false;

	if(m_focusCzhTrains.m_focusCzhTrains.size()>0)
	{
		int nFoucsRecordNo = m_nFocusRecordIndex;
		if(nFoucsRecordNo < 0)
			return true;

		CString str;
		bool bValid=CheckFocusCZHRelation(str);
		OffsetFocusSameCZHTrain(nTimeOffset);
		if(!CheckFocusCZHRelation(str) && bValid)
		{
			MessageBox(str, "��ʾ", MB_OK);

			m_sFocusTrain = &m_pDoc->m_myFocusTrain;

			OffsetFocusSameCZHTrain(nTimeOffset*-1);
			if(point.x!=-1 &&point.y!=-1)
				OnLButtonUp(0, point);

			return false;
		}
	}
	else
	{
		CString str;
		if(!CheckTrainRelationValid(m_sFocusTrain, str) && bCheckTrainCZHRelation)
		{
			m_sFocusTrain = &m_pDoc->m_myFocusTrain;

			if(point.x!=-1 &&point.y!=-1)
				OnLButtonUp(0, point);
			MessageBox(str, "��ʾ", MB_OK);
			return false;
		}
	}

	return true;
}

bool CTGForm::CheckCZHRelation(int nTimeOffset, bool bCheckTrainCZHRelation, CPoint point)
{
	if(NULL==m_sFocusTrain)
		return false;

	if(m_focusCzhTrains.m_focusCzhTrains.size()>0)
	{
		int nFoucsRecordNo = m_nFocusRecordIndex;
		if(nFoucsRecordNo < 0)
			return true;

		CString str;
		bool bValid=CheckFocusCZHRelation(str);
		OffsetFocusSameCZHTrain(nTimeOffset);
		if(!CheckFocusCZHRelation(str) && bValid)
		{
			MessageBox(str, "��ʾ", MB_OK);

			m_sFocusTrain = &m_pDoc->m_myFocusTrain;

			OffsetFocusSameCZHTrain(nTimeOffset*-1);
			if(point.x!=-1 &&point.y!=-1)
				OnLButtonUp(0, point);

			return false;
		}
	}
	else
	{
		CString str;
		if(!CheckTrainCZHValid(m_sFocusTrain, str) && bCheckTrainCZHRelation)
		{
			m_sFocusTrain = &m_pDoc->m_myFocusTrain;

			if(point.x!=-1 &&point.y!=-1)
				OnLButtonUp(0, point);
			MessageBox(str, "��ʾ", MB_OK);
			return false;
		}
	}

	return true;			
}

void CTGForm::MouseModifyTrain(const CPoint point)
{
	int nTgType = m_pDoc->GetTgType();
	if(nTgType == TG_TYPE::BASIC_TG)
	{
		MouseModifyBasicTrain(point);
		return;
	}

	if(m_sFocusTrain == NULL)
		return;

	int nFoucsRecordNo = m_nFocusRecordIndex;
	if(nFoucsRecordNo < 0)
		return;

	TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo);
	if(rec == NULL) 
		return;

	int nStationNo   = rec->station;
	int nAllRecordNo = m_sFocusTrain->GetRecordNum();


	bool bActuralTimePoint = FocusAfterActualPoint(nFoucsRecordNo, *m_sFocusTrain);

	long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);

	bool IsCanMoveWholeTrain = this->m_pctrl->mConfigTg.IsCanMoveBasciTrainWhole();
	bool bLimiteMove = this->m_pctrl->mConfigTg.m_context.IsLimitNxtNodeBackPreNode();
	bool bRouteOkInhibitModifySchd=IsInhibitModifyRouteOkSchdTM();

	bool bCheckTrainCZHRelation=true;
	if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainCZHValid(m_sFocusTrain, str);
	}
	else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainRelationValid(m_sFocusTrain, str);
	}

	if(m_nMouseDownPosition == UNION_POSITION)
	{
		if(IsActualArrivalTime(rec->adjust_status) && !m_pDoc->m_bEditRealLine)
			return;

		if(nFoucsRecordNo == 0)
		{
			long nTimeOffset = 0;
			if(IsFlagStart(rec->flag))
			{
				nTimeOffset = (usTimeValue - rec->depart);
			}
			else
			{
				nTimeOffset = (usTimeValue - rec->arrive);
			}
			
			if (m_pDoc->mMouseMoveMode>0)
			{
				if(nTimeOffset == 0)
					return;
			}
			else
			{
				nTimeOffset=(nTimeOffset/60)*60;
				if(abs(nTimeOffset) < 60)
					return;
			}

			if(IsFlagAbsStart(rec->flag))
			{
				nTimeOffset = 60*(nTimeOffset/60);
			}
			// ���������ڷֲ�����
			if(IsAbsPartRunTrain(m_sFocusTrain->myTrainInfo.adjust_flag))
			{
				nTimeOffset = 60*(nTimeOffset/60);
			}

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			if(nTgType == TG_TYPE::BASIC_TG)
			{	
				m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
			}
			else
			{
				// ��������ʼ��ͣ��ʱ��
				if(IsFlagAbsStart(rec->flag) && !bActuralTimePoint)
				{
					int tmp;
					tmp = m_sFocusTrain->myTrainInfo.stop_abs_time + nTimeOffset/60;
					if (tmp <=0)
						return;
					else
						m_sFocusTrain->myTrainInfo.stop_abs_time = tmp;
				}
				// ���������ڷֲ�����
				if(IsAbsPartRunTrain(m_sFocusTrain->myTrainInfo.adjust_flag) && !bActuralTimePoint)
				{
					int tmp;
					tmp = m_sFocusTrain->myTrainInfo.stop_abs_time + nTimeOffset/60;
					if (tmp <=(rec->stop_abs_s_time+1))
						return;
					else
						m_sFocusTrain->myTrainInfo.stop_abs_time = tmp;
				}

				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 3);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo,  nTimeOffset,  2);
				TrytoModifyOtherTrain(m_sFocusTrain);
			}

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
					
			this->RefreshTrain(m_sFocusTrain);
		}
		else
		{
			TgTRAIN_RECORD *prev = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo - 1);

			if(prev == NULL) 
				return;


			if(IsFlagStopAbs(prev->flag) || IsFlagStopSignal(prev->flag))
			{
				int temp = (usTimeValue - prev->depart - prev->stop_abs_e_time*60);
				if(temp <60)
					return;
			}

			long nTimeOffset = 0;
			// ����ͼ��һ��վ����Ų��ǰһ��ռǰ
			if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
			{
				nTimeOffset = (usTimeValue - rec->arrive);
				nTimeOffset=(nTimeOffset/60)*60;
				if(abs(nTimeOffset) < 60)
					return;
			}
			else
			{
				nTimeOffset = (usTimeValue - rec->arrive);
				if (m_pDoc->mMouseMoveMode>0)
				{
					if(nTimeOffset == 0)
						return;
				}
				else
				{
					nTimeOffset=(nTimeOffset/60)*60;
					if(abs(nTimeOffset) < 60)
						return;
				}
				
				if(bLimiteMove)
				{
					if(prev->depart >= usTimeValue)
						return;
				}
			}

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			if(nTgType == TG_TYPE::BASIC_TG)
			{
				m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
			}
			else
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 3);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo, nTimeOffset, 2);
				TrytoModifyOtherTrain(m_sFocusTrain);
			}
			
			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
			
			if (m_pDoc->mMouseMoveMode==0)
				SetSlowFastTime(nTimeOffset, prev);
			else
			{
				TRAIN_INDEX nTrainIndex=m_sFocusTrain->m_nTrainIndex;
				SetSlowFastTime(nTimeOffset, prev, nTrainIndex, rec);
			}
			this->RefreshTrain(m_sFocusTrain);
		}
	}
	else if(m_nMouseDownPosition == ARRIVAL_POSITION)
	{
		if(IsActualArrivalTime(rec->adjust_status) && !m_pDoc->m_bEditRealLine)
			return;

		if(nFoucsRecordNo == 0)
		{
			long nTimeOffset = 0;
			if(IsFlagStart(rec->flag))
			{
				nTimeOffset = (usTimeValue - rec->depart);
			}
			else
			{
				nTimeOffset = (usTimeValue - rec->arrive);
			}
			if (m_pDoc->mMouseMoveMode>0)
			{
				if(nTimeOffset == 0)
					return;
			}
			else
			{
				nTimeOffset=(nTimeOffset/60)*60;
				if(abs(nTimeOffset) < 60)
					return;
			}

			if(IsFlagAbsStart(rec->flag))
			{
				nTimeOffset = 60*(nTimeOffset/60);
			}

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			
			if(nTgType == TG_TYPE::BASIC_TG)
			{
				if(m_nMouseDownPosition == ARRIVAL_POSITION && !IsCanMoveWholeTrain)
				{
					m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 0);
				}
				else
				{
					m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
				}
			}
			else
			{
				if(bActuralTimePoint)
				{
					// ��ֹ�����Ƶ����㼰�Ժ� 2012.5.2
					TgTRAIN_RECORD* tmpprec = m_sFocusTrain->GetRecordByRecIndex(nFoucsRecordNo);
					if(tmpprec)
					{
						if(tmpprec->arrive+nTimeOffset+60>tmpprec->depart)
							return;

						tmpprec->arrive+=nTimeOffset;
					}
					else
					{
						return;
					}
				}
				else
				{
					if(bRouteOkInhibitModifySchd)
						m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 2);
					else
						m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo, nTimeOffset,  2);
					TrytoModifyOtherTrain(m_sFocusTrain);
				}

				// ��������ʼ��ͣ��ʱ��
				if(IsFlagAbsStart(rec->flag) && !bActuralTimePoint)
				{
					m_sFocusTrain->myTrainInfo.stop_abs_time += nTimeOffset/60;
				}
			}
			
			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
								
			this->RefreshTrain(m_sFocusTrain);
		}
		else
		{
			TgTRAIN_RECORD *prev = m_sFocusTrain->GetRecordByPosition(nFoucsRecordNo - 1);
			if(prev == NULL) 
				return;

			if(IsFlagStopAbs(prev->flag) || IsFlagStopSignal(prev->flag))
			{
				int temp = (usTimeValue - prev->depart - prev->stop_abs_e_time*60);
				if(temp <60)
					return;
			}
			
			long nTimeOffset = 0;
			// ����ͼ��һ��վ����Ų��ǰһ��ռǰ
			if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
			{
				nTimeOffset = (usTimeValue - rec->arrive);
				nTimeOffset=(nTimeOffset/60)*60;
				if(abs(nTimeOffset) < 60)
					return;
			}
			else
			{
				nTimeOffset = (usTimeValue - rec->arrive);
				if (m_pDoc->mMouseMoveMode>0)
				{
					if(nTimeOffset == 0)
						return;
				}
				else
				{
					nTimeOffset=(nTimeOffset/60)*60;
					if(abs(nTimeOffset) < 60)
						return;
				}
				
				if(bLimiteMove)
				{
					if(prev->depart >= usTimeValue)
						return;
				}
			}

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			
			if(nTgType == TG_TYPE::BASIC_TG)
			{
				if(!IsCanMoveWholeTrain)
				{
					m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 0);
				}
				else
				{
					m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
				}
			}
			else
			{
				if(bActuralTimePoint)
				{
					// ��ֹ�����Ƶ����㼰�Ժ� 2012.5.2
					TgTRAIN_RECORD* tmpprec = m_sFocusTrain->GetRecordByRecIndex(nFoucsRecordNo);
					if(tmpprec)
					{
						if(tmpprec->arrive+nTimeOffset+60>tmpprec->depart)
							return;

						tmpprec->arrive+=nTimeOffset;
					}
					else
					{
						return;
					}
					if (m_pDoc->mMouseMoveMode==0)
					{
						SetSlowFastTime(nTimeOffset, prev);
					}
					else
					{
						TRAIN_INDEX nTrainIndex=m_sFocusTrain->m_nTrainIndex;
						SetSlowFastTime(nTimeOffset, prev, nTrainIndex, rec);
					}
				
					this->RefreshTrain(m_sFocusTrain);
					return;
				}

				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 2);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo, nTimeOffset, 2);
				TrytoModifyOtherTrain(m_sFocusTrain);
			}

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
					return;
			}
								
			if (m_pDoc->mMouseMoveMode==0)
				SetSlowFastTime(nTimeOffset, prev);
			else
			{
				TRAIN_INDEX nTrainIndex=m_sFocusTrain->m_nTrainIndex;
				SetSlowFastTime(nTimeOffset, prev, nTrainIndex, rec);
			}
			this->RefreshTrain(m_sFocusTrain);
		}
	}
	else if(m_nMouseDownPosition == DEPART_POSITION) 
	{
		if(IsActualDepartTime(rec->adjust_status) && !m_pDoc->m_bEditRealLine)
			return;

		// ��ֹ�����϶�������֮ǰ
		if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
		{
			;
		}
		else
		{
			if(usTimeValue < rec->arrive)
			{	
				usTimeValue = rec->arrive;
			}
		}
		
		if(usTimeValue < rec->arrive+60)
		{
			usTimeValue=rec->arrive+60; // ��ô����Ϊ�˺�����ʱ,����Ų��ͣ��1����
		}

		int nTimeOffset = (usTimeValue - rec->depart);
		if(nTimeOffset == 0)
			return;
		
		m_pDoc->m_myFocusTrain=*m_sFocusTrain;
		
		if(nTgType == TG_TYPE::BASIC_TG)
		{
			m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 1);
		}
		else
		{
			if(bRouteOkInhibitModifySchd)
				m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 1);
			else
				m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo, nTimeOffset, 1);
			TrytoModifyOtherTrain(m_sFocusTrain);
		}

		if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
		{
			if(!CheckCZHRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}
		else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			if(!CheckTrainRelation(nTimeOffset, bCheckTrainCZHRelation, point))
				return;
		}

		this->RefreshTrain(m_sFocusTrain);
	}
	else if(m_nMouseDownPosition == ABS_ARRIVAL_POSITION)
	{
		TgTRAIN_RECORD *next;
		next = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex+1);
		if(next != NULL)
		{
			if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime()&&IsActualArrivalTime(next->adjust_status) && !m_pDoc->m_bEditRealLine)
				return;
		}
		
		int nTimeOffset = (usTimeValue - rec->depart - rec->stop_abs_s_time*60);
		if(abs(nTimeOffset) < 60)
			return;

		int temp = (usTimeValue - rec->depart);
		//�����俪�س�վ�ĳ�, temp�ض��Ǹ�ֵ
		if(IsFlagAbsStart(rec->flag))
			temp = abs(temp);
		if(temp < 60)
			return;

		next = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex+1);
		if(bActuralTimePoint && next != NULL)
		{
			int temp = rec->stop_abs_e_time*60 + rec->depart - usTimeValue;
			if(abs(temp) < 60)
				return;
		}
		nTimeOffset = 60*(nTimeOffset/60);
		
		if((rec->stop_abs_s_time + nTimeOffset/60) >= 2880)
			return;

		if(nTgType == TG_TYPE::BASIC_TG)
		{
			rec->stop_abs_s_time += nTimeOffset/60;
			if(IsCanMoveWholeTrain)
			{
				rec->stop_abs_e_time = rec->stop_abs_e_time + nTimeOffset/60;
				m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo+1, nTimeOffset, IsCanMoveWholeTrain, 2);
			}
		}
		else
		{
			if((nAllRecordNo == m_nFocusRecordIndex+1) && (IsFlagAbsTerminal(rec->flag) || IsFlagEnd(rec->flag)))
			{
				; // �����յ�\������ʱ�յ�
			}
			else
			{
				if(rec->stop_abs_e_time <= rec->stop_abs_s_time+nTimeOffset/60)
					return;
			}

			rec->stop_abs_s_time += nTimeOffset/60;
			
			if(next != NULL)
			{
				if(!IsActualArrivalTime(next->adjust_status))
				{
					bActuralTimePoint = false;
				}
			}

			if(!bActuralTimePoint)
			{	
				rec->stop_abs_e_time = rec->stop_abs_e_time + nTimeOffset/60;
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo+1, nTimeOffset, 3);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo+1, nTimeOffset, 2);

				// ���������ڷֲ�����
				if(IsAbsPartRunTrain(m_sFocusTrain->myTrainInfo.adjust_flag))
				{
					int tmp;
					tmp = m_sFocusTrain->myTrainInfo.stop_abs_time + nTimeOffset/60;
					if (tmp <=(rec->stop_abs_s_time+1))
						return;
					else
						m_sFocusTrain->myTrainInfo.stop_abs_time = tmp;
				}
				TrytoModifyOtherTrain(m_sFocusTrain);
			}
			else  // �˷������յ�����תʵ��Ų���쳣
			{
				if((nAllRecordNo == m_nFocusRecordIndex+1) && (IsFlagAbsTerminal(rec->flag) || IsFlagEnd(rec->flag)))
				{
					rec->stop_abs_e_time=rec->stop_abs_s_time; // �����յ�\������ʱ�յ�
				}
			}
		}
		this->RefreshTrain(m_sFocusTrain);

	} 
	else if(m_nMouseDownPosition == ABS_DEPART_POSITION ) 
	{
		TgTRAIN_RECORD *next;
		next = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex+1);
		if(next != NULL)
		{
			if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime()&&IsActualArrivalTime(next->adjust_status)&&!m_pDoc->m_bEditRealLine)
				return;
		}
		else
		{
			if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLimitModifyTime()&&!m_pDoc->m_bEditRealLine)
				return;
		}

		// ��ֹ�����϶�������֮ǰ
		int nTimeOffset = (usTimeValue - rec->depart - rec->stop_abs_e_time*60);
		if(abs(nTimeOffset) < 60)
			return;

		nTimeOffset = (nTimeOffset/60)*60;
		if((rec->stop_abs_e_time + nTimeOffset/60) >= 2880)
			return;
				
		if(nTgType == TG_TYPE::BASIC_TG)
		{
			rec->stop_abs_e_time += nTimeOffset/60;
			if(IsCanMoveWholeTrain)
			{
				m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo+1, nTimeOffset, IsCanMoveWholeTrain, 2);
			}
		}
		else
		{
			if(rec->stop_abs_e_time+nTimeOffset/60 <= rec->stop_abs_s_time)
				return;

			rec->stop_abs_e_time += nTimeOffset/60;
			if(next != NULL)
			{
				if(!IsActualArrivalTime(next->adjust_status))
				{
					bActuralTimePoint = false;
				}
			}
			if(!bActuralTimePoint)
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo+1, nTimeOffset, 3);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo+1, nTimeOffset,  2);
				TrytoModifyOtherTrain(m_sFocusTrain);
			}
		}

		this->RefreshTrain(m_sFocusTrain);
	}
	else if(m_nMouseDownPosition == ABS_START_POSITION)
	{
		int nTimeOffset = (usTimeValue + m_sFocusTrain->myTrainInfo.stop_abs_time*60) - rec->arrive;
		if(abs(nTimeOffset) < 60)
			return;
			
		nTimeOffset = (nTimeOffset/60)*60;

		if(nTgType == TG_TYPE::BASIC_TG)
		{
			bActuralTimePoint = true;
		}

		if(!bActuralTimePoint)
		{
			if(nTgType == TG_TYPE::BASIC_TG)
				m_sFocusTrain->OffsetBasicScheduleTime(nFoucsRecordNo, nTimeOffset, IsCanMoveWholeTrain, 2);
			else
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, nFoucsRecordNo, nTimeOffset, 3);
				else
					m_sFocusTrain->OffsetWorkScheduleTime(nFoucsRecordNo, nTimeOffset, 2);
				TrytoModifyOtherTrain(m_sFocusTrain);
			}
		}
		
		this->RefreshTrain(m_sFocusTrain);
	}
}

// �޸ĸϻ�ʱ��
void CTGForm::SetSlowFastTime(int nTimeOffset, TgTRAIN_RECORD *prev)
{
	if(prev == NULL)
		return;

	int newSlowFastTime = 0;
	if(IsRunFast(prev))
	{
		newSlowFastTime = GetFastMinute(prev);
	}
	else
	{
		newSlowFastTime = -GetFastMinute(prev);
	}
	newSlowFastTime = newSlowFastTime-nTimeOffset/60;
	
	if(abs(newSlowFastTime) > 127)
		return;

	if(newSlowFastTime >= 0)
	{
		prev->run_fast_slow_time = newSlowFastTime;
		prev->run_fast_slow_time &= 0x7F;
	}
	else
	{
		prev->run_fast_slow_time = -1*newSlowFastTime;
		prev->run_fast_slow_time |= 0x80;
	}
}

void CTGForm::SetSlowFastTime(int nTimeOffset, TgTRAIN_RECORD *prev, TRAIN_INDEX train_index, TgTRAIN_RECORD *prev_next)
{
	if(prev == NULL || prev_next == NULL)
		return;
	
	CTG_TRAIN* ptrain = m_pDoc->GetTrainByIndex(train_index);
	if(ptrain!=NULL && prev->rec_index+1==prev_next->rec_index)
	{
		TgTRAIN_RECORD* prev_old=ptrain->GetRecordByRecIndex(prev->rec_index);
		TgTRAIN_RECORD* prev_old_next=ptrain->GetRecordByRecIndex(prev_next->rec_index);
		if(prev_old!=NULL && prev_old_next!=NULL)
		{
			if(prev->station==prev_old->station && prev_next->station==prev_old_next->station)
			{
				long dlt = prev_old_next->arrive-prev_old->depart;
				long dlt2 = prev_next->arrive-prev->depart;

				long nTimeOffset1=dlt2-dlt;
				if(abs(nTimeOffset1) < 60)
					return;
				int newSlowFastTime = 0;
				if(IsRunFast(prev_old))
				{
					newSlowFastTime = GetFastMinute(prev_old);
				}
				else
				{
					newSlowFastTime = -GetFastMinute(prev_old);
				}
				newSlowFastTime = newSlowFastTime-nTimeOffset1/60;
				
				if(abs(newSlowFastTime) > 127)
					return;

				if(newSlowFastTime >= 0)
				{
					prev->run_fast_slow_time = newSlowFastTime;
					prev->run_fast_slow_time &= 0x7F;
				}
				else
				{
					prev->run_fast_slow_time = -1*newSlowFastTime;
					prev->run_fast_slow_time |= 0x80;
				}

				return;
			}
		}
	}

	int newSlowFastTime = 0;
	if(IsRunFast(prev))
	{
		newSlowFastTime = GetFastMinute(prev);
	}
	else
	{
		newSlowFastTime = -GetFastMinute(prev);
	}
	newSlowFastTime = newSlowFastTime-nTimeOffset/60;
	
	if(abs(newSlowFastTime) > 127)
		return;

	if(newSlowFastTime >= 0)
	{
		prev->run_fast_slow_time = newSlowFastTime;
		prev->run_fast_slow_time &= 0x7F;
	}
	else
	{
		prev->run_fast_slow_time = -1*newSlowFastTime;
		prev->run_fast_slow_time |= 0x80;
	}
}

void CTGForm::MouseModifyBlock(const CPoint point)
{
	if(m_nFocusFigure == NULL)
		return;

	if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(m_nFocusFigure->adjust_status))
		return;

	CRect rect = m_nFocusFigure->m_drawRect;
	CPoint top_left_point = m_nFocusFigure->m_drawRect.TopLeft();
	CPoint bottom_right_point = m_nFocusFigure->m_drawRect.BottomRight();
	CPoint center_point = m_nFocusFigure->m_drawRect.CenterPoint();

	long usStartTime,usEndTime;

	m_nFocusFigure->start_time = (m_nFocusFigure->start_time/60)*60;
	m_nFocusFigure->end_time = (m_nFocusFigure->end_time/60)*60;;
	// -----------------------------------------------------
	if(m_nFocusFigure->type == GRAPH_NOTE_MARK)
	{
		if( m_nFocusFigure->bMoveTop == TRUE)
		{
			if(point.y + 10 < bottom_right_point.y)
			{
				m_nFocusFigure->station = point.y;
			}
		}
		else if( m_nFocusFigure->bMoveBottom == TRUE)
		{
			if(top_left_point.y+10 < point.y)
			{
				m_nFocusFigure->position = point.y;
			}
		}
		else if(m_nFocusFigure->bMoveLeft == TRUE)
		{
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
			if(point.x + 20 < x1)
			{
				usStartTime = m_pDoc->ConvertXPostoTime(point.x);
				if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
					m_nFocusFigure->start_time = usStartTime;
			}
		}
		else if(m_nFocusFigure->bMoveRight == TRUE)
		{
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
			if(x1 + 20 < point.x)
			{					
				usEndTime = m_pDoc->ConvertXPostoTime(point.x);						
				m_nFocusFigure->end_time = usEndTime;
			}
		}
		else if(m_nFocusFigure->bMoveCenter == TRUE)
		{
			int Xdlt = center_point.x - point.x;
			usStartTime = m_pDoc->ConvertXPostoTime(top_left_point.x - Xdlt);
			usEndTime = m_pDoc->ConvertXPostoTime(bottom_right_point.x - Xdlt);

			if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
				m_nFocusFigure->start_time = usStartTime;
			m_nFocusFigure->end_time = usEndTime;

			int Ydlt = 0;
			if(point.y > center_point.y)
			{
				Ydlt = point.y - center_point.y;

				m_nFocusFigure->station  += Ydlt;
				m_nFocusFigure->position += Ydlt;
			}
			else
			{
				Ydlt = center_point.y - point.y;
				m_nFocusFigure->station  -= Ydlt;
				m_nFocusFigure->position -= Ydlt;
			}
		}

		if(m_nFocusFigure->station > m_nFocusFigure->position)
		{
			int tmp = m_nFocusFigure->station;
			m_nFocusFigure->station  = m_nFocusFigure->position;
			m_nFocusFigure->position = tmp;
		}
		else if(m_nFocusFigure->station == m_nFocusFigure->position)
		{
			m_nFocusFigure->position += 10;
		}

		m_nFocusFigure->m_drawRect.top = m_nFocusFigure->station;
		m_nFocusFigure->m_drawRect.bottom = m_nFocusFigure->position;
		int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
		m_nFocusFigure->m_drawRect.left = x1;
		x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
		m_nFocusFigure->m_drawRect.right = x1;
	}  
	if(m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
	{
		if( m_nFocusFigure->bMoveTop == TRUE)
		{
			if(point.y + 10 < bottom_right_point.y)
			{
				m_nFocusFigure->m_drawRect.top = point.y;
			}
		}
		else if( m_nFocusFigure->bMoveBottom == TRUE)
		{
			if(top_left_point.y+10 < point.y)
			{
				m_nFocusFigure->m_drawRect.bottom = point.y;
			}
		}
		else if(m_nFocusFigure->bMoveLeft == TRUE)
		{
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
			if(point.x + 20 < x1)
			{
				usStartTime = m_pDoc->ConvertXPostoTime(point.x);
				if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
					m_nFocusFigure->start_time = usStartTime;
				int left = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
			    m_nFocusFigure->m_drawRect.left = left-8;
			}
		}
		else if(m_nFocusFigure->bMoveRight == TRUE)
		{
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
			if(x1 + 20 < point.x)
			{					
				usEndTime = m_pDoc->ConvertXPostoTime(point.x);						
				m_nFocusFigure->end_time = usEndTime;
				int right = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
				m_nFocusFigure->m_drawRect.right = right+8;
			}
		}
		else if(m_nFocusFigure->bMoveCenter == TRUE)
		{
			int offsetX = point.x - center_point.x;
			int offsetY = point.y - center_point.y;

			rect.OffsetRect(offsetX, offsetY);
		
			usStartTime = m_pDoc->ConvertXPostoTime(rect.left);
			usEndTime = m_pDoc->ConvertXPostoTime(rect.right);
			if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
				m_nFocusFigure->start_time = usStartTime;

			m_nFocusFigure->end_time = usEndTime;

			m_nFocusFigure->m_drawRect = rect;
			//m_nFocusFigure->m_drawRect.left -= 8;
			//m_nFocusFigure->m_drawRect.right += 8;
		}

		if(m_nFocusFigure->m_drawRect.top > m_nFocusFigure->m_drawRect.bottom)
		{
			int tmp = m_nFocusFigure->m_drawRect.top;
			m_nFocusFigure->m_drawRect.top  = m_nFocusFigure->m_drawRect.bottom;
			m_nFocusFigure->m_drawRect.bottom = tmp;
		}
		else if(m_nFocusFigure->m_drawRect.top == m_nFocusFigure->m_drawRect.bottom)
		{
			m_nFocusFigure->m_drawRect.bottom += 15;
		}

		m_pDoc->SetGraphPosition(m_nFocusFigure, m_nFocusFigure->m_drawRect);
	}  
	else
	{
		if(m_nFocusFigure->bMoveRight == TRUE)
		{		
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
			if(x1 + 6 < point.x)
			{
				usEndTime = m_pDoc->ConvertXPostoTime(point.x);	
				m_nFocusFigure->end_time = usEndTime;
			}
		}
		else if(m_nFocusFigure->bMoveLeft == TRUE)
		{
			int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
			if(point.x + 6 < x1)
			{
				usStartTime = m_pDoc->ConvertXPostoTime(point.x);
				if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
					m_nFocusFigure->start_time = usStartTime;
			}
		}
		else if(m_nFocusFigure->bMoveCenter == TRUE)
		{
			//long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
			//long usMidTime = m_nFocusFigure->start_time+(m_nFocusFigure->end_time - m_nFocusFigure->start_time)/2;
			//int nTimeOffset = (usTimeValue - usMidTime);

			int nTimeOffset = ((point.x - m_pDoc->m_selmarkpoint.x)*60)/m_pDoc->m_fPixelPerMinute;
			if(abs(nTimeOffset) < 60)
				return;
			m_pDoc->m_selmarkpoint=point;
			if (m_nFocusFigure->start_time>m_pDoc->GetBeginTime())
				m_nFocusFigure->start_time += nTimeOffset;
			else
				nTimeOffset = nTimeOffset*2;
			m_nFocusFigure->end_time += nTimeOffset;
		}

		int x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->start_time);
		m_nFocusFigure->m_drawRect.left = x1;
		x1 = m_pDoc->GetOffsetPixelFromGraphBegin(m_nFocusFigure->end_time);
		m_nFocusFigure->m_drawRect.right = x1;
	}
	this->Invalidate();

	// ͼ���ע --- ���� index line e_kilo
	if(m_nFocusFigure->type == GRAPH_NOTE_MARK)
	{
		SetGraphNoteMarkPos(m_nFocusFigure);
	}
	if(m_pDoc->mGraphEndPixel-9 <= point.x)
	{
		this->OnLButtonUp(0, point);
	}
}

void CTGForm::SelFocusCzhTrains()
{
    if(m_sFocusTrain==NULL)
		return;
	
	TRAIN_CHEZUHAO* pSelTrainCZH = NULL;
	if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
	{
		CString czh=m_sFocusTrain->myTrainInfo.shift_index_string;
		pSelTrainCZH = m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
	}
	else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		TRAIN_INDEX trainIndex=m_sFocusTrain->m_nTrainIndex;
		pSelTrainCZH = m_pDoc->myChezuhao.GetCzhByTrainIndex(trainIndex);
	}

	if(pSelTrainCZH==NULL)
		return;

	if(!m_pctrl->mConfigTg.m_context.IsMoveSameCZTrain())
		return;
	
	if(m_focusCzhTrains.m_focusCzhTrains.size()>0)
		return;

	bool bsel=true;
	if(m_nMouseDownPosition==DEPART_POSITION || m_nMouseDownPosition==UNION_POSITION)
	{
		for(int s=m_nFocusRecordIndex; s<m_sFocusTrain->GetRecordNum(); s++)
		{
			TgTRAIN_RECORD* pSelRec=m_sFocusTrain->GetRecordByRecIndex(s);
			if(pSelRec)
			{
				if(pSelRec->adjust_status&0x3)
				{
					bsel=false;
					break;
				}
			}
		}
	}
	else if(m_nMouseDownPosition==ARRIVAL_POSITION)
	{
		for(int s=m_nFocusRecordIndex; s<m_sFocusTrain->GetRecordNum(); s++)
		{
			TgTRAIN_RECORD* pSelRec=m_sFocusTrain->GetRecordByRecIndex(s);
			if(pSelRec)
			{
				if(s==m_nFocusRecordIndex)
				{
					if(pSelRec->adjust_status&0x2)
					{
						bsel=false;
						break;
					}
				}
				else
				{
					if(pSelRec->adjust_status&0x3)
					{
						bsel=false;
						break;
					}
				}
			}
		}
	}
	else
	{
		return;
	}

	m_focusCzhTrains.Init();
	if(bsel)
	{
		int nLeftStation=0, nRightStation=0;
		long nLeftTime=0, nRightTime=0;
		CString nStrLeftTrainID="";
		CString nStrRightTrainID="";
		std::vector<long> list;
		m_pDoc->myChezuhao.GetSameCZTrainIndexList(pSelTrainCZH, m_sFocusTrain->m_nTrainIndex, list, nLeftTime,  nLeftStation, nStrLeftTrainID);
		
		for(int s=0; s<list.size(); s++)
		{
			CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(list[s]);
			if(ptrain==NULL)
				break;
	
			nRightTime=m_pDoc->myChezuhao.GetCzhTrainStartTm(pSelTrainCZH, list[s], nStrRightTrainID,nRightStation);
			if(ptrain->IsExistActualTime() && s>0)
				break;
			m_focusCzhTrains.m_focusCzhTrains.push_back(*ptrain);
		}

		if(s>=list.size())
		{
			nRightTime=999999999;
			nStrRightTrainID="";
			nRightStation=0;
		}
		list.clear();
		
		if(m_focusCzhTrains.m_focusCzhTrains.size()>0)
		{
			m_focusCzhTrains.mLeftStation=nLeftStation;
			m_focusCzhTrains.mLeftTime=nLeftTime;
			m_focusCzhTrains.mRightStation=nRightStation;
			m_focusCzhTrains.mRightTime=nRightTime;
			m_focusCzhTrains.mStrLeftTrainID=nStrLeftTrainID;
			m_focusCzhTrains.mStrRightTrainID=nStrRightTrainID;
		}
	}
}

void CTGForm::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CRect clientRect;
	GetClientRect(&clientRect);
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);
	

	if(!clientRect.PtInRect(point))
	{
		ShowTipMsg(point, "");
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}

	CPoint tip = point;
	clientDC.DPtoLP(&point);
	clientDC.DPtoLP(&clientRect);

	//�ж��û�Ȩ�ޣ�����
	if(!m_pDoc->GetOperatorPriorByDistrict(m_pDoc->nCurrentMouseX,m_pDoc->focus_point))
	{
		m_pDoc->sysprint(4, 0, "û��Ȩ�޻����ڲ��ɲ�������");
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}

	//�ж���굱ǰ�����ڵĳ�վ��nodeIndex
	m_pDoc->m_nCurrentStationNoOfMouse = m_pDoc->ConvertYPostoStationNo(point.y,m_pDoc->m_nCurrentNodeIndex);

	if(m_pDoc->m_nOperatorStatus == MOVE_MUTI_TRAINS)
	{
		if(m_selTrains.size()>0)
		{
			MouseModifyMutiTrain(point);
		}
	}
	if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
	{
		CString strStaSide;
		ULONG indexoftrain;

		//��ѯ����
		m_strTimeString.Format("%s", m_pDoc->QueryCurrentTrain(&point,indexoftrain));

		if(indexoftrain > 0) 
		{	
			//��ѯ����λ�õĳ�վ�͹ɵ�
			CPoint pt = point;
			strStaSide = m_pDoc->QueryCurrentTrainStationAndside(pt,indexoftrain);
			if(!strStaSide.IsEmpty())
				m_strTimeString = m_strTimeString+":"+strStaSide;
		}

		if(this->m_pctrl->mConfigTg.m_context.IsShowStaticTimeAtMouse())
		{
			if(indexoftrain > 0)
			{
				CTG_TRAIN *pTrain = m_pDoc->GetTrainByIndex(indexoftrain);
				if(pTrain != NULL) 
				{
					long timeValue = pTrain->myTrainInfo.sborder_actual_time - pTrain->myTrainInfo.sborder_plan_time;
					timeValue = timeValue/60;
					
					CString str_text;
					if(abs(timeValue/60) < 1)
					{
						if(timeValue==0) 
							str_text.Format("");
						else 
							str_text.Format("%d",abs(timeValue%60));
					}
					else
						str_text.Format("%d.%02d",abs(timeValue/60),abs(timeValue%60));

					if(!str_text.IsEmpty())
					{
						if(timeValue>0)
							m_strTimeString = m_strTimeString + " ���:" + str_text;
						else
							m_strTimeString = m_strTimeString + " ���:" + str_text;
					}
					else
					{
						m_strTimeString += " ����";
					}
				}
			}
		}

		if(!m_strTimeString.IsEmpty())
			ShowTipMsg(tip, m_strTimeString);
		else
		{
			ShowTipMsg(tip, "");
			if(m_pctrl->mConfigTg.m_context.IsNotShowTipText())
			{
				m_pctrl->FIRE_SHOW_CREW(0, 0, 0);
			}
			else
			{
				const CTG_TRAIN* pSelTrain=m_pDoc->GetTrainByPoint(point);
				if(pSelTrain!=NULL)
				{
					CPoint wndPoint=tip;
					ClientToScreen(&wndPoint);
					m_pctrl->FIRE_SHOW_CREW(pSelTrain->m_nTrainIndex, wndPoint.x, wndPoint.y);
				}
				else
				{
					m_pctrl->FIRE_SHOW_CREW(0, 0, 0);
				}
			}
		}

		if(!IsCTRLpressed())
		{
			if(m_nOldPoint != point && m_bMouseDown)
			{
				if(m_sFocusTrain != NULL &&	m_nFocusRecordIndex >= 0 && m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				{
					if(m_nFocusRecordIndex >= 0 && m_pDoc->m_nOperatorStatus != MODIFY_TRAIN)
					{
						m_pDoc->m_nOperatorStatus = MODIFY_TRAIN;
						if(m_pDoc->m_ptgctrl->mConfigTg.IsEraseBeforeModifyTrain())
						{
							this->EraseTrain(*m_sFocusTrain);
						}

						// ��m_sFocusTrain�Ĺ����г��ŵ�m_sFocusOtherTrain
						{
							m_sFocusOtherTrain.clear();

							std::vector<CTG_TRAIN> train_list;
							// 1. ����;���۷�����
							m_pDoc->TrytoFindAbsEngineReturnTrain(m_sFocusTrain, train_list);
							for (int n=0; n<train_list.size(); n++)
							{
								m_sFocusOtherTrain.push_back(train_list[n]);
							}

							// 2. �����ڷֲ�����
							train_list.clear();
							m_pDoc->TrytoFindAbsPartRunTrain(m_sFocusTrain, train_list);
							for (int n=0; n<train_list.size(); n++)
							{
								m_sFocusOtherTrain.push_back(train_list[n]);
							}
						}
					}
				}
				if((m_nFocusFigure != NULL) && (
					m_nFocusFigure->bMoveCenter == TRUE ||
					m_nFocusFigure->bMoveLeft == TRUE ||
					m_nFocusFigure->bMoveRight == TRUE ||
					m_nFocusFigure->bMoveTop == TRUE ||
					m_nFocusFigure->bMoveBottom == TRUE)
					&& m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					m_pDoc->m_nOperatorStatus = MODIFY_MARK; 
				}
			}
		}
	}

	if(m_pDoc->m_nOperatorStatus == MODIFY_TRAIN)
	{
		// ���ý��㳵��
		SelFocusCzhTrains();
		
		// MODIFY TRAIN
		long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);

		if (m_pDoc->mMouseMoveMode==2)
			m_strTimeString.Format("%d.%d",  (usTimeValue/60)%10, usTimeValue%60);
		else
			m_strTimeString.Format("%d",  (usTimeValue/60)%10);  // ����ƶ���ʾ��
		ShowTipMsg(tip, m_strTimeString);
		MouseModifyTrain(point);
	}
	
	if(m_pDoc->m_nOperatorStatus == MODIFY_MARK)
	{
		// MODIFY BLOCK
		MouseModifyBlock(point);
	}

	if(nFlags == MK_LBUTTON && m_pDoc->m_nOperatorStatus == MUTI_TRAINS_SEL_ING) 
	{
		if(point.x <= m_pDoc->m_fPixelPerHour*12)
		{
			CScrollView::OnMouseMove(nFlags, point);
			return;
		}

		DrawBox(clientDC, m_TrainsSelectedRect);
		m_TrainsSelectedRect.BottomRight() = point;
		DrawBox(clientDC, m_TrainsSelectedRect);

		CScrollView::OnMouseMove(nFlags, point);
		return;
	}

	// ������������
	if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN)
	{
		long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
		if (m_pDoc->mMouseMoveMode==2)
			m_strTimeString.Format("%d.%d",  (usTimeValue/60)%10, usTimeValue%60);
		else
			m_strTimeString.Format("%d",  (usTimeValue/60)%10); // ����ƶ���ʾ��
		
		ShowTipMsg(tip, m_strTimeString);
		if(m_sFocusTrain != NULL)
		{
			if(m_sFocusTrain->m_nPointCount >= 1  && m_pOldMousePoint != point )
			{ 					
				if(m_pOldMousePoint != CPoint(-1,-1) && m_pContinuePoint != CPoint(-1,-1))
				{						
					DrawLine(clientDC,m_pContinuePoint,m_pOldMousePoint);
				}	

				m_pContinuePoint = m_sFocusTrain->GetContinuePoint();
				m_pOldMousePoint = point;
				DrawLine(clientDC,m_pContinuePoint,point);
			}
		}
	}
	// ���Ʊ�����߿�
	if(m_pDoc->m_nOperatorStatus== DRAW_MARK_ITEM_STATUS)
	{
		if((m_nMarkType == GRAPH_NOTE_MARK || m_nMarkType == NEW_GRAPH_NOTE_MARK) && 
			(m_nSubMarkType == GRAPH_NOTE_MARK	|| m_nSubMarkType == TEXT_NOTE_MARK))
		{
			if(m_absDragRect.left<0 || !m_bFirstPoint)
				m_absDragRect.SetRect(0,0,0,0);
			m_bFirstPoint  = true;

			DrawBox(clientDC, m_absDragRect);
			m_absDragRect.BottomRight() = point;
			CPoint pt_lefttop = CPoint(point.x-30, point.y-30);
			m_absDragRect.TopLeft() = pt_lefttop;
			DrawBox(clientDC, m_absDragRect);

			m_bSecondPoint = true;
		}
		else 
		{
			long usTimeValue = m_pDoc->ConvertXPostoTime(point.x);
			// ����ƶ���ʾ��
			if (m_pDoc->mMouseMoveMode==2)
				m_strTimeString.Format("%d.%d",  (usTimeValue/60)%10, usTimeValue%60);
			else
				m_strTimeString.Format("%d",  (usTimeValue/60)%10); 
			ShowTipMsg(tip, m_strTimeString);
			if (m_bFirstPoint)
			{
				DrawBox(clientDC, m_absDragRect);

				if(m_absDragRect.left+10 < point.x)
				{
					m_absDragRect.right = point.x;
				}

				if(m_absDragRect.top +10 < point.y)
				{
					m_absDragRect.bottom = point.y;
				}

				DrawBox(clientDC, m_absDragRect);

				// Center
				CRect invalidRect(m_absDragRect.left+2, m_absDragRect.top+2, m_absDragRect.right-2, m_absDragRect.bottom-2);
				clientDC.LPtoDP(&invalidRect);
				InvalidateRect(&invalidRect);

				// Bottom
				CRect invalidRectExt(m_absDragRect.left, m_absDragRect.bottom + 2, m_absDragRect.right, m_absDragRect.bottom + 32);
				clientDC.LPtoDP(&invalidRectExt);
				InvalidateRect(&invalidRectExt);

				// Right
				CRect invalidRectExt1(m_absDragRect.right+2, m_absDragRect.top, m_absDragRect.right+32, m_absDragRect.bottom);				
				clientDC.LPtoDP(&invalidRectExt1);
				InvalidateRect(&invalidRectExt1);
			}
		}
	}


	
	CScrollView::OnMouseMove(nFlags, point);
}

void CTGForm::ShowTipMsg(CPoint pt, CString tip)
{
	ClientToScreen(&pt);
	pt.Offset(5,-5);


	if(m_pCfWnd.m_strText != tip && !m_pCfWnd.m_strText.IsEmpty())
	{
		m_pCfWnd.HideTipWindow();
	}
	if(tip.IsEmpty())
	{
		m_pCfWnd.HideTipWindow();
	}
	else
	{
		m_pCfWnd.ShowTipWindow(pt, tip);
	}
}

void CTGForm::DrawVirtulStationAixis(CDC *pDC, ULONG lColor)  //2012.09.03 ���ӻ�������վ
{
	if (m_pDoc->m_nDispDistanceNum <= 0) // ����Ҫ!!!
		return;
		
	std::map<int , SectionVirtualStation>::iterator it = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.find(m_pDoc->m_nCurrentSectionNo);
	if(it == m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.end())
		return;

	CPen mStationPen;
	mStationPen.CreatePen(PS_DOT,1, lColor);//����

	int nWidth = m_pDoc->m_nPageWidth;

	CPen* pOldPen  = pDC->SelectObject(&mStationPen);
	UINT nOldAlign    = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);

	for(int i=0; i<it->second.count; i++)
	{
		int perStationIndex=it->second.m_virtualStation[i].perStationIndex;
		int nextStationIndex=it->second.m_virtualStation[i].nextStationIndex;
		
		if(nextStationIndex<=perStationIndex)
			continue;

		if(perStationIndex<0)
			continue;

		if(nextStationIndex>=m_pDoc->m_nDispDistanceNum)
			continue;

		int delta = m_pDoc->m_sDispDistance[nextStationIndex].nStationYPos - m_pDoc->m_sDispDistance[perStationIndex].nSecondStationYPos;


		int nPercent=it->second.m_virtualStation[i].nPercent;

		pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +3);
		pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +3);
	}

	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);

	mStationPen.DeleteObject();

    return;
}

void CTGForm::DrawStationAixis(CDC *pDC, ULONG lColor)
{
	if (m_pDoc->m_nDispDistanceNum <= 0) // ����Ҫ!!!
		return;

	if (m_pDoc->m_bShowVirtualStation)
		DrawVirtulStationAixis(pDC, lColor);

	UINT nOldAlign    = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);

	// ����������
	CPen nDotPenEleArm;
	nDotPenEleArm.CreatePen(PS_DOT,1,lColor); 
	
	CPen *pOldPen = pDC->SelectObject(&nDotPenEleArm);
	UINT OldTextColor = pDC->SetTextColor(lColor);

	int nWidth = m_pDoc->m_nPageWidth;

	for(int i=0;i < m_pDoc->m_nDispDistanceNum;i++)
	{	
		// 0:������(Ĭ��) 1:�� 2:�� 3:����
		if(m_pDoc->m_sDispDistance[i].electric_arms == 1)
		{
			pDC->MoveTo(X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);
			pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);
		}
		else if(m_pDoc->m_sDispDistance[i].electric_arms == 2)
		{
			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
			}
			else
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos+3);
			}
		}
		else if(m_pDoc->m_sDispDistance[i].electric_arms == 3)
		{
			pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos-4);
			pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos-4);

			if(m_pDoc->m_sDispDistance[i].show_sation_sep)
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSecondStationYPos+3);
			}
			else
			{
				pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos+3);
				pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos+3);
			}
		}

		COLORREF tmpClr = lColor;
		if(m_pDoc->m_sDispDistance[i].clrStationLine!=-1)
		{
			tmpClr=m_pDoc->m_sDispDistance[i].clrStationLine;
		}

		CPen mStationPen;
		if(m_pDoc->m_sDispDistance[i].isNeedThick == 1)
		{
			mStationPen.CreatePen(PS_SOLID,2,tmpClr);//����
		}
		else if(m_pDoc->m_sDispDistance[i].isNeedThick == 2)
		{
			mStationPen.CreatePen(PS_DOT,1,tmpClr);//����
		}
		else
		{
			mStationPen.CreatePen(PS_SOLID,1,tmpClr);//ϸ�� 
		}

		CPen* pElePen = pDC->SelectObject(&mStationPen);
		pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nStationYPos);
		pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nStationYPos);

		if(m_pDoc->m_sDispDistance[i].show_sation_sep)
		{
			pDC->MoveTo(X_TIME_OFFSET , m_pDoc->m_sDispDistance[i].nSecondStationYPos);
			pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSecondStationYPos);

			if(m_pDoc->m_sDispDistance[i].show_sides_mode ==0)  
			{
				for (int n = 0;n < m_pDoc->m_sDispDistance[i].number_of_side;n++)
				{
					int nSide = m_pDoc->m_sDispDistance[i].side[n].side_number;
					int nProperty = m_pDoc->m_sDispDistance[i].side[n].side_property;
					int nDirect = m_pDoc->m_sDispDistance[i].side[n].direct;
					CString strSideName = m_pDoc->m_sDispDistance[i].side[n].sidename;

					COLORREF sideClr = m_pDoc->m_ptgctrl->mConfigTg.GetSideColor(nProperty, nDirect, lColor);
					CPen nSidePen;
					nSidePen.CreatePen(PS_DOT,1, sideClr); 

					CPen* pStaPen=pDC->SelectObject(&nSidePen);

					pDC->MoveTo(X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSideYPos[nSide]);
					pDC->LineTo(nWidth + X_TIME_OFFSET, m_pDoc->m_sDispDistance[i].nSideYPos[nSide]);

					pDC->SelectObject(pStaPen);
					nSidePen.DeleteObject();

					DrawSideNamePerFourHour(pDC,  m_pDoc->m_sDispDistance[i].nSideYPos[nSide], strSideName);
				}
			}
		}
		pDC->SelectObject(pElePen);
		mStationPen.DeleteObject();
	}
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(OldTextColor);

	nDotPenEleArm.DeleteObject();
}

void CTGForm::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	return;
}

void CTGForm::OnMButtonDown(UINT nFlags, CPoint point)
{
	return;
}

void CTGForm::OnMButtonUp(UINT nFlags, CPoint point)
{
	return;
}

// G16
bool CTGForm::SetMouseSelRecord(int nFlag, int sub_section)
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return false;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(NULL == pTrain)
		return false;

	TgTRAIN_RECORD* rec = pTrain->GetRecordByPosition(m_nFocusRecordIndex);
	if(rec == NULL)
		return false;

	int nDirect = nFlag;
	if (rec->in_sta_direct == rec->out_sta_direct)
	{
		if(rec->out_sta_direct == UP_DIRECT)
		{
			if(nFlag == 0)
				nDirect = 1;
			else
				nDirect = 0;
		}
		else
		{
			if(nFlag == 0)
				nDirect = 0;
			else
				nDirect = 1;
		}
	}
	else
	{
		int m=m_nFocusRectIndex;
		if(nFlag == 0)
		{
			if (rec->in_sta_direct == UP_DIRECT)
			{
				if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m+1].record_index)
					nDirect=1;
				else if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m-1].record_index)
					nDirect=0;
			}
			else
			{
				if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m+1].record_index)
					nDirect=0;
				else if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m-1].record_index)
					nDirect=1;
			}
		}
		else if (nFlag == 1)
		{
			if (rec->in_sta_direct == UP_DIRECT)
			{
				if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m+1].record_index)
					nDirect=0;
				else if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m-1].record_index)
					nDirect=1;
			}
			else
			{
				if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m+1].record_index)
					nDirect=1;
				else if (m_sFocusTrain->m_pPerPointInfo[m].record_index == m_sFocusTrain->m_pPerPointInfo[m-1].record_index)
					nDirect=0;
			}
		}
	}

	bool bSel=false;
	if(0 == nDirect)
	{
		int m=m_nFocusRectIndex;
		if(m>=1 && m_sFocusTrain->IsSameSelRect(m, m-1))
		{
			m=m-1;
		}

		if(m>0)
		{
			if((sub_section==-1) || (m_sFocusTrain->m_pPerPointInfo[m].nSubSection==sub_section))
			{
				m_nFocusRectIndex=m-1;
				m_nFocusRecordIndex = m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].record_index;
				bSel = true;
			}
		}
	}
	else if (1 == nDirect)
	{
		int m=m_nFocusRectIndex;
		if(m<m_sFocusTrain->m_nPointCount-1 && m_sFocusTrain->IsSameSelRect(m, m+1))
		{
			m=m+1;
		}

		if(m<m_sFocusTrain->m_nPointCount-1)
		{
			if((sub_section==-1) || (m_sFocusTrain->m_pPerPointInfo[m].nSubSection==sub_section))
			{
				m_nFocusRectIndex=m+1;
				m_nFocusRecordIndex = m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].record_index;
				bSel = true;
			}
		}
	}
	
	if(bSel)
	{
		int rec_index=m_nFocusRecordIndex;
		int tmpTrainIndex=m_sFocusTrain->m_nTrainIndex;
		TgTRAIN_RECORD* rec = pTrain->GetRecordByPosition(rec_index);
		if(rec == NULL)
			this->m_pctrl->FIRE_TG_SELECT_TRAIN(tmpTrainIndex, 1, 0, 0, rec_index, 0);
		else
			this->m_pctrl->FIRE_TG_SELECT_TRAIN(tmpTrainIndex, 1, rec->station, rec->station, rec_index, 0);

		RefreshTrain(m_sFocusTrain);
	}

	return bSel;
}

void CTGForm::ModifySideByKeyboard(int nFlag)
{
	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(NULL == pTrain)
		return;

	// save
	int nFocusRecIndex = m_nFocusRecordIndex;
	int nFocusRectIndex = m_nFocusRectIndex;

	m_pDoc->m_myFocusTrain = *pTrain;
	m_sFocusTrain = &m_pDoc->m_myFocusTrain;


	TgTRAIN_RECORD* rec = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex);
	if(rec == NULL)
		return;

	if(0 <= rec->in_station_index && rec->in_station_index < m_pDoc->m_nDispDistanceNum)
	{
		if(0 <= rec->out_station_index && rec->out_station_index < m_pDoc->m_nDispDistanceNum)
		{
			if(rec->in_station_index == rec->out_station_index)
			{
				if(m_pDoc->m_sDispDistance[rec->in_station_index].show_sation_sep == 0)
				{
					SetMouseSelRecord(nFlag);
					return;
				}
			}
			else
			{
				if(m_pDoc->m_sDispDistance[rec->in_station_index].show_sation_sep == 0)
				{
					int sub_section=m_pDoc->m_sDispDistance[rec->in_station_index].sub_section;
					if(SetMouseSelRecord(nFlag, sub_section))
					{
						return;
					}
				}

				if(m_pDoc->m_sDispDistance[rec->out_station_index].show_sation_sep == 0)
				{
					int sub_section=m_pDoc->m_sDispDistance[rec->out_station_index].sub_section;
					if(SetMouseSelRecord(nFlag, sub_section))
					{
						return;
					}
				}
			}
		}
		else
		{
			if(m_pDoc->m_sDispDistance[rec->in_station_index].show_sation_sep == 0)
			{
				SetMouseSelRecord(nFlag);
				return;
			}
		}
	}
	else
	{
		if(0 <= rec->out_station_index && rec->out_station_index < m_pDoc->m_nDispDistanceNum)
		{
			if(m_pDoc->m_sDispDistance[rec->out_station_index].show_sation_sep == 0)
			{
				SetMouseSelRecord(nFlag);
				return;
			}
		}
		else
		{
			return;
		}
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
		return;

	if(m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station))
	{
		CString str;
		str.Format("��ע��, %s �����ڱ�̨��Ͻ,�����޸Ĺɵ�",  m_pDoc->GetStationName(rec->station));
		MessageBox(str, "��ʾ", MB_OK);
		m_pDoc->sysprint(4,0, "%s", str);
		return;
	}

	// �������޸Ĺɵ�
	if(IsForceSide(*rec))
	{
		MessageBox("�ɵ�����,�������޸�", "����", MB_OK|MB_ICONHAND);
		return;
	}

	LINE_INFO line;
	if(!m_pDoc->m_ptgctrl->mConfigTg.m_lineCfg.GetStaLineCfg(rec->station, line))
		return;

	int i = 0;
	if(rec->arrive_side <= 0)
	{
		if(nFlag == 0)
			i = line.vecSideInfo.size()-1;
		else
			i = 0;
	}
	else
	{
		for(i = 0; i < line.vecSideInfo.size(); i++)
		{
			if(line.vecSideInfo[i].sideno == rec->arrive_side)
				break;
		}

		if(nFlag == 0)
		{
			if(i > 0)
				i--;
		}
		else
		{
			if(i < line.vecSideInfo.size()-1)
				i++;
		}
	}

	
	int side_no = line.vecSideInfo[i].sideno;
	WORD uOldSide = rec->arrive_side; 
	WORD uOldDetSide = rec->depart_side; 
	if(side_no == uOldSide && side_no == uOldDetSide)
	{
		m_pDoc->sysprint(0,0,"�����޸Ĺɵ���ԭ�ɵ���ͬ");
		return;
	}

	if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, rec, m_sFocusTrain))
	{
		return;
	}

	if(!m_pDoc->SideSetEntry(this->m_hWnd, side_no, side_no, m_nFocusRecordIndex, m_sFocusTrain))
	{
		return;
	}

	CString arlOldSide = m_pDoc->GetSideName(rec->station, rec->arrive_side);
	CString depOldSide = m_pDoc->GetSideName(rec->station, rec->depart_side);

	rec->arrive_side = side_no;
	rec->depart_side = rec->arrive_side;

	CString arlSide = m_pDoc->GetSideName(rec->station, rec->arrive_side);
	CString depSide = m_pDoc->GetSideName(rec->station, rec->depart_side);

	//���ݾ֣��ÿ��г���ͱ�־�޸Ĺɵ���ʾ��
	if (this->m_pctrl->mConfigTg.m_context.IsShowWarningDlgByChangeSide() && m_sFocusTrain->GetTrainType() == TYPE::PASSENGER)//���ر�־λ�ҵ�ǰ�г��ǿͳ�
	{
		if ( rec->arrive_side != rec->plan_arrive_side || rec->depart_side != rec->plan_depart_side)
		{
			MessageBoxByChangeSide(rec, arlSide, depSide);
		}
	}

	CStringArray ary;
	m_pDoc->CheckTrainSideInfo(&m_pDoc->m_myFocusTrain, nFocusRecIndex, ary);
	
	if(ary.GetCount()>0)
	{
		m_pDoc->sysprint(4,0,"����Աͨ��������޸� %s %s�ɵ�, ��������:", m_pDoc->GetTrainShowId(m_pDoc->m_myFocusTrain), 
				m_pDoc->GetStationName(rec->station));

		CString strText, tmp;
		for(int i=0; i<ary.GetCount(); i++)
		{
			tmp=ary.GetAt(i);
			m_pDoc->sysprint(4,0,"%s", tmp);
			strText += tmp;
			strText += "\n";
		}

		if(MessageBox(strText+"\n��ȷʵ�޸���?", "����", MB_YESNO|MB_ICONHAND)==IDYES)
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ����޸Ĺɵ�");
		}
		else
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸Ĺɵ�");
			return;
		}			
	}

	m_pDoc->SendUpdateTrain(m_pDoc->m_myFocusTrain, SCHD_UPDATE, MODIFY_SIDE, 0, m_nFocusRecordIndex);
	m_pDoc->sysprint(4,0,"[TG]: ����Աͨ��������޸�%s %s�ɵ�:(%s %s)-->(%s %s)", 
		m_pDoc->GetTrainShowId(m_pDoc->m_myFocusTrain), m_pDoc->GetStationName(rec->station), arlOldSide, depOldSide, arlSide, depSide);
}

void CTGForm::ModifyBlockTimeByKeyboard(int nFlag)
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
		return;

	if(m_nFocusFigure == NULL || !m_nFocusFigure->m_bFocus)
		return;

	CTg_BLOCK modifyBlock(*m_nFocusFigure);

	//ѡ����߱�ǣ�������ʼʱ���һ���ӣ����Ը���ÿ���ӵ����������ڣ�Ҳ����ֱ�Ӽ�һ����
	long date = 0,time = 0;
	long totalSeconds = 0;
	int dlt = -60;
	if(nFlag == 1)
		dlt = 60;
	if(m_nFocusFigure->bMoveLeft)
	{
		if (modifyBlock.start_time>m_pDoc->GetBeginTime())
			modifyBlock.start_time += dlt;
	}
	else if(m_nFocusFigure->bMoveRight)
	{
		modifyBlock.end_time += dlt;
	}

	m_pDoc->SendUpdateMark(modifyBlock); 	
}

void CTGForm::ModifyTrainArlDepTimeByKeyboard(int nFlag)
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
		return;

	if(m_sFocusTrain == NULL || m_nFocusRecordIndex < 0)
		return;

	if(m_nFocusRectIndex<0 || m_nFocusRectIndex>=m_sFocusTrain->m_nPointCount)
		return;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->m_nTrainIndex);
	if(NULL == pTrain)
		return;

	int nTgType = m_pDoc->GetTgType();
	// save
	int nFocusRecIndex = m_nFocusRecordIndex;
	int nFocusRectIndex = m_nFocusRectIndex;
	if(nFocusRectIndex >= m_sFocusTrain->m_nPointCount || nFocusRectIndex < 0)
		return;

	int start_depart = m_sFocusTrain->m_pPerPointInfo[nFocusRectIndex].start_depart_flag;
	if(start_depart !=ARRIVAL_POSITION && start_depart !=UNION_POSITION && start_depart != DEPART_POSITION)
		return;

	m_pDoc->m_myFocusTrain = *pTrain;
	m_sFocusTrain = &m_pDoc->m_myFocusTrain;

	

	bool bActuralTimePoint = FocusAfterActualPoint(m_nFocusRecordIndex, m_pDoc->m_myFocusTrain);
	bool IsCanMoveWholeTrain = m_pDoc->m_ptgctrl->mConfigTg.IsCanMoveBasciTrainWhole();
	bool bRouteOkInhibitModifySchd=IsInhibitModifyRouteOkSchdTM();

	TgTRAIN_RECORD* rec = m_sFocusTrain->GetRecordByPosition(m_nFocusRecordIndex);
	if(rec == NULL)
		return;

	bool bCheckTrainCZHRelation=true;
	if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainCZHValid(m_sFocusTrain, str);
	}
	else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		CString str;
		bCheckTrainCZHRelation=CheckTrainRelationValid(m_sFocusTrain, str);
	}
	SelFocusCzhTrains();

	CPoint point(-1,-1);
	if(nFlag == 0)
	{
		if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == ARRIVAL_POSITION ||
			m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == UNION_POSITION)
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status)))
				return;

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			if(bActuralTimePoint && rec->arrive != rec->depart)
			{
				rec->arrive -= 60;
				m_pDoc->sysprint(4,0,"����Աͨ��<-���޸��г� %s �� %s  ����ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}
			else
			{
				if(nTgType == TG_TYPE::BASIC_TG)
					m_sFocusTrain->OffsetBasicScheduleTime(m_nFocusRecordIndex, -60, IsCanMoveWholeTrain,2);
				else
				{
					if(bRouteOkInhibitModifySchd)
					{
						if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == ARRIVAL_POSITION)
							m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, -60, 2);
						else if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == UNION_POSITION)
							m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, -60, 3);
						else
							return;
					}
					else
						m_sFocusTrain->OffsetWorkScheduleTime(m_nFocusRecordIndex, -60, 2);
				}
				m_pDoc->sysprint(4,0,"����Աͨ��<-���޸��г� %s �� %s ƽ�Ƹ�վ������ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
			}

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(-60, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(-60, bCheckTrainCZHRelation, point))
					return;
			}
		}
		else if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == DEPART_POSITION)
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualDepartTime(rec->adjust_status)))
				return;
			if(rec->arrive != rec->depart)
			{
				if(rec->depart-60 <= rec->arrive)
					return;
			}

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;

			if(nTgType == TG_TYPE::BASIC_TG)
				m_sFocusTrain->OffsetBasicScheduleTime(m_nFocusRecordIndex, -60, IsCanMoveWholeTrain,1);
			else
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, -60, 1); 
				else
					m_sFocusTrain->OffsetWorkScheduleTime(m_nFocusRecordIndex, -60, 1); 
			}
			m_pDoc->sysprint(4,0,"����Աͨ��<-���޸��г� %s �� %s ��վ����ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(-60, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(-60, bCheckTrainCZHRelation, point))
					return;
			}
		}
		else
		{
			return;
		}

		m_pDoc->SendUpdateTrain(m_pDoc->m_myFocusTrain);
		SendCzhOtherTrainsUpdate(m_pDoc->m_myFocusTrain.GetTrainIndex());
		m_focusCzhTrains.Init();
	}
	else if(nFlag == 1)
	{
		if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == ARRIVAL_POSITION ||
			m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == UNION_POSITION)
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status)))
				return;

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;
			if(bActuralTimePoint && rec->arrive != rec->depart)
			{
				if(rec->arrive+60 < rec->depart)
				{
					rec->arrive += 60;
					m_pDoc->sysprint(4,0,"����Աͨ��->���޸��г� %s �� %s ��վ����ʱ�� %d %d ",  	
						m_pDoc->GetTrainShowId(m_sFocusTrain), m_pDoc->GetStationName(rec->station), rec->arrive,rec->depart);
				}
			}
			else
			{
				if(nTgType == TG_TYPE::BASIC_TG)
					m_sFocusTrain->OffsetBasicScheduleTime(m_nFocusRecordIndex, 60, IsCanMoveWholeTrain,2);
				else
				{
					if(bRouteOkInhibitModifySchd)
					{
						if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == ARRIVAL_POSITION)
							m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, 60, 2);
						else if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == UNION_POSITION)
							m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, 60, 3);
						else
							return;
					}
					else
						m_sFocusTrain->OffsetWorkScheduleTime(m_nFocusRecordIndex, 60, 2);
				}
			}

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(60, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(60, bCheckTrainCZHRelation, point))
					return;
			}
		}
		else if(m_sFocusTrain->m_pPerPointInfo[m_nFocusRectIndex].start_depart_flag == DEPART_POSITION)
		{
			if(!m_pDoc->m_bEditRealLine && (IsActualDepartTime(rec->adjust_status)))
				return;

			m_pDoc->m_myFocusTrain=*m_sFocusTrain;

			if(nTgType == TG_TYPE::BASIC_TG)
				m_sFocusTrain->OffsetBasicScheduleTime(m_nFocusRecordIndex, 60, IsCanMoveWholeTrain,1);
			else
			{
				if(bRouteOkInhibitModifySchd)
					m_pDoc->OffsetWorkScheduleTimeByRouteOk(m_sFocusTrain, m_nFocusRecordIndex, 60, 1); 
				else
					m_sFocusTrain->OffsetWorkScheduleTime(m_nFocusRecordIndex, 60, 1); // �����60
			}

			if(m_pctrl->mConfigTg.m_context.IsEnableCZH())
			{
				if(!CheckCZHRelation(60, bCheckTrainCZHRelation, point))
					return;
			}
			else if(m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				if(!CheckTrainRelation(60, bCheckTrainCZHRelation, point))
					return;
			}
		}
		else
		{
			return;
		}

		m_pDoc->SendUpdateTrain(m_pDoc->m_myFocusTrain);
		SendCzhOtherTrainsUpdate(m_pDoc->m_myFocusTrain.GetTrainIndex());
		m_focusCzhTrains.Init();
	}
}

// �Զ���������
void CTGForm::OnAutoGenDCmd(UINT message) 
{
	if(m_nFocusFigure == NULL) 
		return;

	std::map<int, int>::iterator it = m_map_menuid.find(message);
	std::map<int, int>::iterator it_parent = m_map_menuparentid.find(message);
	
	if(it != m_map_menuid.end())
	{
		int stytle = 0; // 1:���� 2:վ��
		int parent_type = 0;
		if(it_parent != m_map_menuparentid.end())
			parent_type = it_parent->second;
		CString dcmd = GetBlockDcmdDesp(m_nFocusFigure, parent_type, it->second, stytle);
		if(!dcmd.IsEmpty())
		{
			m_pDoc->m_ptgctrl->TG_GEN_BLOCK_DCMD(dcmd, m_nFocusFigure->mark_index, stytle);
		}

		CStdioFile file;
		if(file.Open("C:\\mark_save.xml", CFile::modeCreate|CFile::modeReadWrite))
		{
			CString xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?> ";
			xml +=m_nFocusFigure->toXML();
			file.Write(xml.GetBuffer(), xml.GetLength());
			file.Close();
		}
	}
}

CString CTGForm::GetBlockDcmdDesp(const CTg_BLOCK* block, const int parent_type, const int template_type, int& stytle)
{
	stytle = 0;

	CString xml,tmp;

	CString reason, dcmdno;
	m_pDoc->ParseMarkReasonText(block->text, dcmdno, reason);

	tmp.Format("<ATTRIBUTE parent_type=\"%d\" type=\"%d\" sub_type=\"%d\" index=\"%d_%d\" dcmdno=\"%s\" text=\"%s\"/> ", 
		parent_type, template_type, 0, block->mark_index, template_type, dcmdno, reason);
	
	xml = tmp;

	tmp.Format("<TIME start_time=\"%d\" end_time=\"%d\" slow_time=\"%d\"/> ", 
		block->start_time, block->end_time, block->slow_time);
	xml += tmp;

	tmp.Format("<LIMIT s_kilo=\"%d\" s_meter=\"%d\" e_kilo=\"%d\" e_meter=\"%d\" speed=\"%d\"/> ", 
		block->position, block->s_meter, block->e_kilo, block->e_meter, block->flag);
	xml += tmp;

	CString text;
	if(block->type ==SELF_CLOSE_MARK)
	{
		if(block->flag == 0)
			text = " ���Զ�����";
		else if(block->flag == 1)
			text = " �Զ�����";
		else if(block->flag == 2)
			text = " �绰����";
		else if(block->flag == 3)
			text = " ͣ���ĵ�";
		else if(block->flag == 4)
			text = " ͣ�û�������";
		else
			text = "";

		tmp.Format("<SELF_CLOSE text=\"%s\"/>", text);
		xml += tmp;
	}


	if(block->type == ABS_INHIBIT_MARK || 
		block->type == ABS_SLOW_MARK || 
		block->type == ABS_FIX_MARK || 
		block->type == ABS_ELE_FIX_MARK || 
		block->type == NEW_ABS_ELE_FIX_MARK ||
		block->type ==SELF_CLOSE_MARK)
	{
		stytle = 1;
		int include1 = 0, include2 = 0;
		if(block->type == ABS_ELE_FIX_MARK || block->type == ABS_FIX_MARK ||block->type == NEW_ABS_ELE_FIX_MARK)
		{
			if(block->slow_time == 1)
				include1 =1;
			if(block->slow_time == 2)
				include2 =1;
			if(block->slow_time == 3)
			{
				include1 = 1;
				include2 = 1;
			}
		}

		xml += "<STATIONS>";
		tmp.Format("<station no=\"%d\" name=\"%s\" include=\"%d\"/> ", block->station1, m_pDoc->GetStationName(block->station1),include1);
		xml += tmp;
		tmp.Format("<station no=\"%d\" name=\"%s\" include=\"%d\"/> ", block->station2, m_pDoc->GetStationName(block->station2),include2);
		xml += tmp;
		xml += "</STATIONS> ";

		tmp.Format("<ABS_DESP direct=\"%d\">", (0x0003 & block->line));
		xml += tmp;
		for(int i=0; i<block->count;i++)
		{
			const ABS_LINE* pabs = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(block->ele_number[i]);
			if(NULL != pabs)
			{
				tmp.Format("<abs no=\"%d\" name=\"%s\" direct=\"%d\"/> ", block->ele_number[i], pabs->sLineName, pabs->nDirect);
				xml += tmp;
			}
		}
		xml += "</ABS_DESP> ";
	}
	else if(block->type == SIDE_INHIBIT_MARK ||
		block->type == SIDE_SLOW_MARK ||
		block->type == SIDE_OCCUPY_MARK ||
		block->type == SIDE_TIANCHUANG_MARK)
	{
		stytle = 2;
		xml += "<STATIONS>";
		tmp.Format("<station no=\"%d\" name=\"%s\" include=\"0\"/> ", block->station1, m_pDoc->GetStationName(block->station1));
		xml += tmp;
		xml += "</STATIONS>";

		xml += "<STATION_SIDE>";
		for(int i=0; i<block->count;i++)
		{
			CString sideName = m_pDoc->GetSideName(block->station1, block->ele_number[i]);
			tmp.Format("<side no=\"%d\" name=\"%s\"/>", block->ele_number[i], sideName);
			xml += tmp;
		}
		xml += "</STATION_SIDE>";
	}
	else
	{
		return "";
	}

	// GEN_BLOCK_DCMD
	return xml;
}

bool CTGForm::LoadAutoGenDCmdXml()
{
	m_menu.clear();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	CString pathname = m_pDoc->GetDataPath() + "\\AutoGenDCmdMenu.xml";
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("����%s�����ڻ��߲�����xml�﷨(���������������Ƿ��ܴ�)", pathname);
		
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "ROOT");
	if(pRootNode == NULL)
		return false;

	MSXML::IXMLDOMNodeListPtr pListCon = pRootNode->selectNodes( "BLOCK");
	if(NULL != pListCon)
	{
		for(int idx = 0; idx < pListCon->Getlength(); idx++)
		{
			MenuNode tmpMenuInfo;
			pNode = pListCon->Getitem(idx);
			if(pNode)
			{
				pChildNode = pNode->selectSingleNode("@type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmpMenuInfo.type = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				MSXML::IXMLDOMNodeListPtr pListMenu = pNode->selectNodes( "MENU");
				if(NULL != pListMenu)
				{
					for(int idx = 0; idx < pListMenu->Getlength(); idx++)
					{
						pNode = pListMenu->Getitem(idx);
						if(pNode)
						{
							tmpMenuInfo.id=0;
							tmpMenuInfo.parent_id=0;
							memset(tmpMenuInfo.text,0,sizeof(tmpMenuInfo.text));
							pChildNode = pNode->selectSingleNode("@name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								CString text = CString(v.bstrVal);

								strncpy(tmpMenuInfo.text, text.GetBuffer(), 99);
								tmpMenuInfo.text[99]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							
							pChildNode = pNode->selectSingleNode("@parent_type");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								CString text(v.bstrVal);

								tmpMenuInfo.parent_id = atoi(text);
								
								SysFreeString(v.bstrVal);
								v.Clear();
							}

							pChildNode = pNode->selectSingleNode("@dcmd_template_type");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								CString text(v.bstrVal);

								tmpMenuInfo.id = atoi(text);
								
								SysFreeString(v.bstrVal);
								v.Clear();
							}

							m_menu.push_back(tmpMenuInfo);
						}
					}
				}
			}
		}
	}

	return true;
}


void CTGForm::OnLiveDlgWindow(ULONG nFlag)
{
	if(NULL == m_pDoc)
		return;

	if(nFlag==2)
	{
		CDlgSelMutiMark dlg(m_pDoc, this);
		if(dlg.DoModal() == IDOK)
		{
			for(int i=0; i<dlg.mSelMarkCount; i++)
			{
				CTg_BLOCK* block = m_pDoc->myPDataManage->GetMark(dlg.mSelMarkIndex[i]);
				if(NULL != block)
				{
					m_pDoc->SendDeleteBlockXml(block->mark_index,0);
					m_pDoc->sysprint(4,0,"[TG]: ͨ��ϵͳ�²˵�ɾ��ʩ������ index:%d  text:%s",  block->mark_index, block->text);
				}
			}
		}
	}
	else if(nFlag==8)
	{
		int nTrainNum = 0;
		int nTrainIndex[2048];
		memset(nTrainIndex, 0, sizeof(nTrainIndex));
		
		nTrainNum = m_pDoc->myPDataManage->GetInValidTrainCount(nTrainIndex,2048);
		if(0 == nTrainNum)
		{
			AfxMessageBox("û�зǷ�����");
		}
		else
		{
			for(int i=0; i<nTrainNum; i++)
			{
				CTG_TRAIN* train = m_pDoc->myPDataManage->GetTrain(nTrainIndex[i]);
				if(NULL == train)
					continue;

				CString text;
				text.Format("���޸��г� %s ��?", m_pDoc->GetTrainShowId(train));
				int nRet = MessageBox("\n1. �޸ĸó��� ��\n2. �����ó��� �� \n3. ֹͣ�޸İ� ȡ��", text, MB_YESNOCANCEL| MB_ICONQUESTION);
				if(nRet==IDCANCEL)
					break;
				else if(nRet==IDNO)
					continue;

				CModifyStationIndex* dlg = new CModifyStationIndex(m_pDoc, train, this);

				if(NULL != dlg)
				{
					dlg->m_train_index = nTrainIndex[i];
							
					dlg->Create(IDD_DLG_MODIFY_TRAIN_INOUT_INDEX, this);
					dlg->ShowWindow(SW_SHOW);
				}
			}
		}
	}
	else if(nFlag == 16)
	{
		OnGenTrainReturnTrainIDAll();
	}
	else if(nFlag == 32)
	{
		OnMultiTrainsCZHEdit();
	}
	else if (nFlag == 64)
	{
		CDlgSetTrainSideLock *dlg = new CDlgSetTrainSideLock(m_pDoc, this);
		if (dlg!=NULL)
		{
			dlg->Create(IDD_DLG_SET_MULTI_TRAIN_SIDE_LOCK);
			dlg->ShowWindow(TRUE);
		}
	}
	else if(nFlag == 128)
	{
		OnQuickModifyMultiTrain();
	}

	return;
}

void CTGForm::OnSetFenjiekouLateTime()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	ULONG nFlag = rec->rec_index;
	nFlag <<= 24;
	nFlag += 0x8;
	m_pDoc->m_ptgctrl->TG_NOTICE(2, nFlag, m_sFocusTrain->GetTrainIndex(), rec->station);
}

void CTGForm::OnThreeLine()
{
	TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
	if (rec == NULL) 
		return;

	int station = rec->station;

	if(!m_pctrl->mConfigTg.m_SingleTrainCfg.bLoadThreeLine)
	{
		m_pctrl->mConfigTg.m_SingleTrainCfg.bLoadThreeLine=true;
		CString path;
		path.Format("%s\\ThreeLine%d.xml", m_pctrl->mConfigTg.m_context.GetDataPath(),
			m_pDoc->m_nCurrentSectionNo);
		m_pctrl->mConfigTg.m_SingleTrainCfg.LoadTgThreeLineXML(path);
	}

	std::vector<THREE_LINE>& vecThreeLine = m_pctrl->mConfigTg.m_SingleTrainCfg.vetThreeLine;
	if(vecThreeLine.size()<=0)
		return;

	for(int i=0; i<vecThreeLine.size();i++)
	{
		const THREE_LINE myThreeLine = vecThreeLine[i];
	
		int nSameWith1=0;

		int begin=-1,end=-1;
		int id=0;
		for(int n=0; n<m_sFocusTrain->GetRecordNum(); n++)
		{
			TgTRAIN_RECORD *prec=m_sFocusTrain->GetRecordByRecIndex(n);
			if(prec->station == myThreeLine.part1[id].station)
			{
				if(0 == id)
					begin=n;
				id++;
			}
			else
				id=0;

			if(id >= myThreeLine.count1)
			{
				end=n;
				break;
			}
		}
		
		if(begin!=-1 && end!=-1)
		{
			if(myThreeLine.count1 == myThreeLine.count2)
				nSameWith1=3;
			else
				nSameWith1=1;

			// �ж�ѡ��վ�Ƿ���ƥ��������
			for(int m=0; m<myThreeLine.count1; m++)
			{
				if(station==myThreeLine.part1[m].station)
				{
					break;
				}
			}
			if(m>=myThreeLine.count1)
			{
				nSameWith1=0;
			}
		}
		else
		{
			begin=-1;end=-1;
			id=0;
			for(int n=0; n<m_sFocusTrain->GetRecordNum(); n++)
			{
				TgTRAIN_RECORD *prec=m_sFocusTrain->GetRecordByRecIndex(n);
				if(prec->station == myThreeLine.part2[id].station)
				{
					if(0 == id)
						begin=n;
					id++;
				}
				else
					id=0;

				if(id >= myThreeLine.count2)
				{
					end=n;
					break;
				}
			}

			if(begin!=-1 && end!=-1)
			{
				nSameWith1=2;

				// �ж�ѡ��վ�Ƿ���ƥ��������
				for(int m=0; m<myThreeLine.count2; m++)
				{
					if(station==myThreeLine.part2[m].station)
					{
						break;
					}
				}
				if(m>=myThreeLine.count2)
				{
					nSameWith1=0;
				}
			}
			else
			{
				nSameWith1=0;
			}
		}
		
		if(nSameWith1 == 1)
		{
			CTG_TRAIN train=*m_sFocusTrain;
			// add
			if(myThreeLine.count1==2 && myThreeLine.count2==3 && end<train.GetRecordNum() && begin+1==end)
			{
				m_pDoc->SendDeleteTrain(train);

				CTG_TRAIN train_tmp=train;
				int rec_index=0;
				train.myRecord.clear();
				TIME depart=0;
				TgTRAIN_RECORD rec;
				for(int n=0; n<=begin; n++)
				{
					rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;
					depart = rec.depart;
					train.myRecord.push_back(rec);
				}

				rec.rec_index	=	rec_index;
				rec.station		=	myThreeLine.part2[1].station;
				rec.depart_side	=	myThreeLine.part2[1].sideno;
				rec.arrive_side	=	rec.depart_side;
				rec.depart		=	depart	+	myThreeLine.part2[1].run_time;
				rec.arrive		=	rec.depart;
				rec.plan_arrive =	rec.arrive;
				rec.plan_depart =	rec.depart;
				rec.in_station_index	=	myThreeLine.part2[1].arrive_index;
				rec.out_station_index	=	myThreeLine.part2[1].depart_index;
				SetTechStopTrain(rec); //Ĭ�����ü���ͣ��

				train.myRecord.push_back(rec);
				rec_index++;

				for(int n=end; n<train_tmp.GetRecordNum(); n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				end++;
			}
			else if(myThreeLine.count1==3 && myThreeLine.count2==2 && end<train.GetRecordNum() && begin+2==end)
			{
				m_pDoc->SendDeleteTrain(train);

				CTG_TRAIN train_tmp=train;
				int rec_index=0;
				train.myRecord.clear();
				for(int n=0; n<=begin; n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				for(int n=end; n<train_tmp.GetRecordNum(); n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				end--;
			}
			for(int n=begin; (n<=end)&&((n-begin)<myThreeLine.count2); n++)
			{
				TgTRAIN_RECORD *prec=train.GetRecordByRecIndex(n);
				if(NULL == prec)
					return;

				int arrive_index	=	myThreeLine.part2[n-begin].arrive_index;
				int depart_index	=	myThreeLine.part2[n-begin].depart_index;
	
				prec->entry	=	myThreeLine.part2[n-begin].entry;
				prec->exit	=	myThreeLine.part2[n-begin].exit;

				if(n==begin)
				{
					TgTRAIN_RECORD *pre=train.GetRecordByRecIndex(n-1);
					if(pre!=NULL)
					{
						//prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
				else if(n==end)
				{
					TgTRAIN_RECORD *next=train.GetRecordByRecIndex(n+1);
					if(next!=NULL)
					{
						prec->in_station_index=arrive_index;
						//prec->out_station_index=depart_index;
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
				else
				{
					prec->in_station_index=arrive_index;
					prec->out_station_index=depart_index;
				}
			}
			m_pDoc->SendAppendTrain(&train, "", 0);
			break;
		}
		else if(nSameWith1 == 2)
		{
			CTG_TRAIN train=*m_sFocusTrain;
			// add
			if(myThreeLine.count1==3 && myThreeLine.count2==2 && end<train.GetRecordNum() && begin+1==end)
			{
				m_pDoc->SendDeleteTrain(train);

				CTG_TRAIN train_tmp=train;
				int rec_index=0;
				train.myRecord.clear();
				TIME depart=0;
				TgTRAIN_RECORD rec;
				for(int n=0; n<=begin; n++)
				{
					rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					depart = rec.depart;
					train.myRecord.push_back(rec);
				}

				rec.rec_index	=	rec_index;
				rec.station		=	myThreeLine.part1[1].station;
				rec.depart_side	=	myThreeLine.part1[1].sideno;
				rec.arrive_side	=	rec.depart_side;
				rec.depart		=	depart	+	myThreeLine.part1[1].run_time;
				rec.arrive		=	rec.depart;
				rec.plan_arrive =	rec.arrive;
				rec.plan_depart =	rec.depart;
				rec.in_station_index	=	myThreeLine.part1[1].arrive_index;
				rec.out_station_index	=	myThreeLine.part1[1].depart_index;
				SetTechStopTrain(rec); //Ĭ�����ü���ͣ��
				train.myRecord.push_back(rec);

				rec_index++;

				for(int n=end; n<train_tmp.GetRecordNum(); n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				end++;
			}
			else if(myThreeLine.count1==2 && myThreeLine.count2==3 && end<train.GetRecordNum() && begin+2==end)
			{
				m_pDoc->SendDeleteTrain(train);

				CTG_TRAIN train_tmp=train;
				int rec_index=0;
				train.myRecord.clear();
				for(int n=0; n<=begin; n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				for(int n=end; n<train_tmp.GetRecordNum(); n++)
				{
					TgTRAIN_RECORD rec=train_tmp.myRecord[n];
					rec.rec_index = rec_index;
					rec_index++;

					train.myRecord.push_back(rec);
				}
				end--;
			}

			for(int n=begin; (n<=end)&&((n-begin)<myThreeLine.count1); n++)
			{
				TgTRAIN_RECORD *prec=train.GetRecordByRecIndex(n);
				if(NULL == prec)
					return;
				
				int arrive_index	=	myThreeLine.part1[n-begin].arrive_index;
				int depart_index	=	myThreeLine.part1[n-begin].depart_index;
	
				prec->entry	=	myThreeLine.part1[n-begin].entry;
				prec->exit	=	myThreeLine.part1[n-begin].exit;

				if(n==begin)
				{
					TgTRAIN_RECORD *pre=train.GetRecordByRecIndex(n-1);
					if(pre!=NULL)
					{
						//prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
				else if(n==end)
				{
					TgTRAIN_RECORD *next=train.GetRecordByRecIndex(n+1);
					if(next!=NULL)
					{
						prec->in_station_index=arrive_index;
						//prec->out_station_index=depart_index;
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
				else
				{
					prec->in_station_index=arrive_index;
					prec->out_station_index=depart_index;
				}
			}
			m_pDoc->SendAppendTrain(&train, "", 0);
			break;
		}
		else if(nSameWith1 == 3)
		{
			bool bSame=true;
			for(int n=begin; (n<=end)&&((n-begin)<myThreeLine.count1); n++)
			{
				TgTRAIN_RECORD *prec=m_sFocusTrain->GetRecordByRecIndex(n);
				if(NULL == prec)
					return;

				if(prec->station != myThreeLine.part1[n-begin].station)
					return;

				int arrive_index	=	myThreeLine.part1[n-begin].arrive_index;
				int depart_index	=	myThreeLine.part1[n-begin].depart_index;
				if(n==begin)
				{
					if(prec->out_station_index != depart_index)
						bSame=false;
				}
				else if(n==end)
				{
					if(prec->in_station_index != arrive_index)
						bSame=false;
				}
				else
				{
					if(prec->in_station_index != arrive_index)
						bSame=false;
					if(prec->out_station_index != depart_index)
						bSame=false;
				}

				if(!bSame)
					break;
			}

			for(int n=begin; (n<=end)&&((n-begin)<myThreeLine.count1); n++)
			{
				TgTRAIN_RECORD *prec=m_sFocusTrain->GetRecordByRecIndex(n);
				if(NULL == prec)
					return;
				
				if(bSame)
				{
					int arrive_index	=	myThreeLine.part2[n-begin].arrive_index;
					int depart_index	=	myThreeLine.part2[n-begin].depart_index;
	
					prec->entry	=	myThreeLine.part2[n-begin].entry;
					prec->exit	=	myThreeLine.part2[n-begin].exit;
					if(n==begin)
					{
						TgTRAIN_RECORD *pre=m_sFocusTrain->GetRecordByRecIndex(n-1);
						if(pre!=NULL)
						{
							//prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
						else
						{
							prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
					}
					else if(n==end)
					{
						TgTRAIN_RECORD *next=m_sFocusTrain->GetRecordByRecIndex(n+1);
						if(next!=NULL)
						{
							prec->in_station_index=arrive_index;
							//prec->out_station_index=depart_index;
						}
						else
						{
							prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
				else
				{
					int arrive_index	=	myThreeLine.part1[n-begin].arrive_index;
					int depart_index	=	myThreeLine.part1[n-begin].depart_index;
	
					prec->entry	=	myThreeLine.part1[n-begin].entry;
					prec->exit	=	myThreeLine.part1[n-begin].exit;
					if(n==begin)
					{
						TgTRAIN_RECORD *pre=m_sFocusTrain->GetRecordByRecIndex(n-1);
						if(pre!=NULL)
						{
							//prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
						else
						{
							prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
					}
					else if(n==end)
					{
						TgTRAIN_RECORD *next=m_sFocusTrain->GetRecordByRecIndex(n+1);
						if(next!=NULL)
						{
							prec->in_station_index=arrive_index;
							//prec->out_station_index=depart_index;
						}
						else
						{
							prec->in_station_index=arrive_index;
							prec->out_station_index=depart_index;
						}
					}
					else
					{
						prec->in_station_index=arrive_index;
						prec->out_station_index=depart_index;
					}
				}
			}
					
			m_pDoc->SendUpdateTrain(m_sFocusTrain);
			break;
		}
	}
}

void CTGForm::ClearTrainCZHLine(TRAIN_CHEZUHAO* pTrainsCZH)
{
	if(pTrainsCZH==NULL)
		pTrainsCZH = m_pDoc->GetCurrentSelTrainCZH();
	if(pTrainsCZH==NULL)
		return;

	m_pDoc->SetCurrentSelTrainCZH("");
	RefreshRect(pTrainsCZH->rect);
}

void CTGForm::DrawSelTrainCZHLine()
{
	if(!m_pDoc->m_bShowSelCZHLine)
		return;

	TRAIN_CHEZUHAO*pTrainsCZH = m_pDoc->GetCurrentSelTrainCZH();
	if(pTrainsCZH==NULL)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	CDC* pDC=&dc;

	m_pDoc->DrawTrainConnectLine(pTrainsCZH, pDC);
}

bool CTGForm::CheckTrainCZHValid(CTG_TRAIN* train, CString& str)
{
	return m_pDoc->myChezuhao.CheckTrainCZHValid(train, str);
}

bool CTGForm::CheckTrainRelationValid(CTG_TRAIN* ptrain, CString& str)
{
	if(ptrain==NULL)
		return true;

	if(ptrain->GetRecordNum()<=0)
		return true;

	TRAIN_CZH_INFO info;
	if(!m_pDoc->myChezuhao.GenCZHInfoFromTrain(*ptrain, info))
		return true;


	//m_pDoc->ReGenTrainJiaoluRelation();
	TRAIN_INDEX train_index=ptrain->GetTrainIndex();

	TRAIN_CHEZUHAO* pTrainCZH = m_pDoc->myChezuhao.GetCzhByTrainIndex(train_index);
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

// �����Ԥ��
void CTGForm::OnTrainsInSameCZHShow()
{
	if(m_sFocusTrain == NULL)	
		return;

	CString title1, title2;
	if(this->m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		m_pDoc->GetJiaoluDlgName(title1, title2);
		CWnd * pWnd=CWnd::FindWindow(NULL,title2);
		if(pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
		}
		
		CJiaoLuShowDlg* m_pCZHDlg = new CJiaoLuShowDlg(title2, m_sFocusTrain->m_nTrainIndex, m_pDoc, this);
		if(NULL != m_pCZHDlg)
		{
			m_pCZHDlg->Create(IDD_DLG_JIAOLU_VIEW, this);
			m_pCZHDlg->ShowWindow(SW_SHOW); 
		}
	}
	else
	{
		m_pDoc->GetChezuhaoDlgName(title1, title2);
		CWnd * pWnd=CWnd::FindWindow(NULL,title2);
		if(pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
		}
		
		CCZHShowDlg* m_pCZHDlg = new CCZHShowDlg(title2, m_sFocusTrain, m_pDoc, this);
		if(NULL != m_pCZHDlg)
		{
			m_pCZHDlg->Create(IDD_DLG_CZH_SHOW, this);
			m_pCZHDlg->ShowWindow(SW_SHOW); 
		}
	}
}

// �༭�����
void CTGForm::OnTrainCZHEdit()
{
	if(m_sFocusTrain == NULL)	
		return;

	if(this->m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		CTG_TRAIN train=*m_sFocusTrain;

		CString text;
		DWORD this_train_index=train.m_nTrainIndex;
		CTG_TRAIN* pPrev=m_pDoc->GetTrainByIndex(train.prev_train_index);
		if(pPrev)
		{
			text.Format("%s�κ�%s��ͬ��·, ����ı佻·, ���Ƚ���˽�·��ϵ!", 
				m_pDoc->GetTrainShowId(train),
				m_pDoc->GetTrainShowId(pPrev));
		
			m_pDoc->sysprint(4,0,"%s", text);
			MessageBox(text, "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}
		else if(train.prev_train_index!=0)
		{
			text.Format("��ע��:�г� %s ����ǰ���г�, ���Ҫ�ı� %s ��ǰ���г�,���Ƚ���˹�����ϵ!", 
				m_pDoc->GetTrainShowId(train), m_pDoc->GetTrainShowId(train));		
			m_pDoc->sysprint(4,0,"%s", text);
			MessageBox(text, "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}

		TgTRAIN_RECORD* prec=train.GetFirstRecord();
		if(prec==NULL)
		{
			return;
		}
		if(!IsFlagStart(prec->flag))
		{
			MessageBox("��ǰѡ���г�,����ʼ��,���ܹ���ǰ��·", "��ʾ", MB_OK|MB_ICONWARNING);
			return;
		}
		CStringArray list_trainno;
		CDWordArray list_trainindex;
		if(!myPDataManage->GetMaybePrevTrains(prec->station, prec->depart, text, list_trainno, list_trainindex))
		{
			MessageBox(text, "��ʾ", MB_OK);
			return;
		}
		
		CInputReturnTrainDlg dlg(this,list_trainno, list_trainindex);
		if(dlg.DoModal()!=IDOK)
			return;
		
		CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(dlg.m_return_trainindex);
		if(ptrain)
		{
			CTG_TRAIN* pothertrain=m_pDoc->GetPrevTrainByIndex(dlg.m_return_trainindex);
			if(pothertrain)
			{
				text.Format("%s�κ�%s��ͬ��·,ȷ���޸Ľ�·,���Ƚ���˽�·��ϵ!", 
					m_pDoc->GetTrainShowId(ptrain),
					m_pDoc->GetTrainShowId(pothertrain));
			
				MessageBox(text, "��ʾ", MB_OK|MB_ICONWARNING);
				return;
			}
			// ������·����
			TgTRAIN_RECORD* plast=ptrain->GetLastRecord();
			if(prec->station!=plast->station)
			{
				text.Format("����ͬһ����վ,���ܹ�����·,���� %s �� %s �յ�,���ǳ��� %s �� %s ʼ��!", 
					m_pDoc->GetTrainShowId(ptrain), m_pDoc->GetStationName(plast->station),
					m_pDoc->GetTrainShowId(train),m_pDoc->GetStationName(prec->station)
					);
			
				MessageBox(text, "��ʾ", MB_OK|MB_ICONWARNING);
				return;
			}

			if(prec->depart_side!=plast->arrive_side)
			{
				text.Format("�ɵ���ͬ�޷��۷�,%s %s��%s�յ�,%s��%sʼ��,���޸�Ϊ��ͬ�۷��ɵ�,�ٹ���·!", 
					m_pDoc->GetStationName(prec->station),
					m_pDoc->GetTrainShowId(ptrain), m_pDoc->GetSideName(plast->station,plast->arrive_side),
					m_pDoc->GetTrainShowId(train), m_pDoc->GetSideName(prec->station,prec->depart_side)
					);
			
				MessageBox(text, "��ʾ", MB_OK|MB_ICONWARNING);
				return;
			}

			m_pDoc->sysprint(4,0,"����Ա�趨�г� %s(%u)�Ĺ����г�Ϊ %s(%u)", 
				m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), 
				m_pDoc->GetTrainShowId(ptrain), ptrain->GetTrainIndex());

			train.prev_train_index=dlg.m_return_trainindex;
			if(m_pctrl->mConfigTg.m_context.m_bModifyRelationSetTrainImport)
			{
				train.myTrainInfo.super_rank  = 1;
				m_pDoc->sysprint(4,0,"����%s(%u)Ϊ�ص��г�", m_pDoc->GetTrainShowId(train), train.GetTrainIndex());
			}
			m_pDoc->SendUpdateTrain(train);

			if(m_pctrl->mConfigTg.m_context.m_bModifyRelationSetTrainImport)
			{
				CTG_TRAIN newtrain = *ptrain;
				newtrain.myTrainInfo.super_rank  = 1;
				m_pDoc->sysprint(4,0,"����%s(%u)Ϊ�ص��г�", m_pDoc->GetTrainShowId(newtrain), newtrain.GetTrainIndex());

				m_pDoc->SendUpdateTrain(newtrain);
			}
		}
		else
		{
			MessageBox("�Ҳ���ǰ���г�,��������'�����г�'", "����", MB_OK|MB_ICONHAND);
		}
	}
	else
	{
		CDlgCheZuHao dlg(m_sFocusTrain, m_pDoc, this);
		if(dlg.DoModal()!=IDOK)
			return;

		CTG_TRAIN* ptrain = m_pDoc->GetTrainByIndex(m_sFocusTrain->GetTrainIndex());
		if(NULL == ptrain)
		{
			MessageBox("��ע��: �ó������Ѿ�ɾ��, �༭�������Ϣʧ��", "����");
			return;
		}
		else
		{
			CTG_TRAIN train=*m_sFocusTrain;
			TgTRAIN_MORE_INFO& info = train.myTrainInfo;
		
			if(dlg.m_strCheZuHao != info.shift_index_string)
			{
				info.shift_index_string=dlg.m_strCheZuHao;
			    
				TgTRAIN_RECORD* last=train.GetLastRecord();
				if(last)
				{
					if(IsFlagTerminal(last->flag) && (last->depart_train_id!=last->arrive_train_id))
					{
						last->depart_train_id=last->arrive_train_id;
					}
				}

				m_pDoc->sysprint(4,0,"����Ա�༭����� �г� %s index %d �����:%s",
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), info.shift_index_string);

				m_pDoc->SendUpdateTrain(train);
			}
		}
	}
}

void CTGForm::OnTrainCZHJiechu()
{
	if(m_sFocusTrain == NULL)	
		return;
	CTG_TRAIN train=*m_sFocusTrain;
	CString text;
	DWORD this_train_index=train.m_nTrainIndex;
	CTG_TRAIN* pPrev=m_pDoc->GetTrainByIndex(train.prev_train_index);
	if(pPrev)
	{
		text.Format("��ȷʵ��� %s �� %s �Ĺ�����ϵ��?", 
						m_pDoc->GetTrainShowId(train), m_pDoc->GetTrainShowId(pPrev));
	
		if(MessageBox(text, "��ʾ", MB_YESNO)==IDYES)
		{
			m_pDoc->sysprint(4,0,"����Ա����г� %s(%u)�� %s(%u)�Ĺ�����ϵ", 
				m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), 
				m_pDoc->GetTrainShowId(pPrev), pPrev->GetTrainIndex());

			train.prev_train_index=0;
			if(m_pctrl->mConfigTg.m_context.m_bModifyRelationSetTrainImport)
			{
				train.myTrainInfo.super_rank  = 1;
				m_pDoc->sysprint(4,0,"����%s(%u)Ϊ�ص��г�", m_pDoc->GetTrainShowId(train), train.GetTrainIndex());
			}
			m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, JIE_JIAOLUT_RELATION);

			if(m_pctrl->mConfigTg.m_context.m_bModifyRelationSetTrainImport)
			{
				CTG_TRAIN newtrain = *pPrev;
				newtrain.myTrainInfo.super_rank  = 1;
				m_pDoc->sysprint(4,0,"����%s(%u)Ϊ�ص��г�", m_pDoc->GetTrainShowId(newtrain), newtrain.GetTrainIndex());
				m_pDoc->SendUpdateTrain(newtrain);
			}
		}
	}
	else
	{
		text.Format("��ȷʵ��� %s ��ǰ����·��ϵ��?", m_pDoc->GetTrainShowId(train));
		if(MessageBox(text, "��ʾ", MB_YESNO)==IDYES)
		{
			m_pDoc->sysprint(4,0,"����Ա����г� %s(%u)��ǰ����·��ϵprev_train_index=%u", 
				m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), train.prev_train_index);

			train.prev_train_index=0;
			if(m_pctrl->mConfigTg.m_context.m_bModifyRelationSetTrainImport)
			{
				train.myTrainInfo.super_rank  = 1;
				m_pDoc->sysprint(4,0,"����%s(%u)Ϊ�ص��г�", m_pDoc->GetTrainShowId(train), train.GetTrainIndex());
			}
			m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, JIE_JIAOLUT_RELATION);
		}
	}
}

void CTGForm::OnGenReturnTrainIdByCZH()
{
	if(!m_pctrl->mConfigTg.m_context.IsEnableCZH())
		return;
	if(m_sFocusTrain == NULL)	
		return;

	if(!m_pctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
		return;

	CString czh = m_sFocusTrain->myTrainInfo.shift_index_string;
	if(czh.IsEmpty())
	{
		MessageBox("��ע��, ��ѡ�е��г�û�г����, ���������۷�����", "����", MB_OK|MB_ICONWARNING);
		return;
	}
	
	m_pDoc->sysprint(4,0,"����Ա ѡ�����Ϊ:%s���г������۷�����", czh);
	
	// �������ɳ����ϵ
	m_pDoc->ReGenTrainCZHRelation(czh);

	CStringArray text;
	m_pDoc->GenTrainReturnTrainID(czh, text);
	
	// �������ʧ���򵯳��Ի���,�������ɳɹ����赯��
	if(text.GetCount()>0)
	{
		CMessageDlg *pdlg = new CMessageDlg(this);
		pdlg->Create(IDD_DIALOG_MSG);
		pdlg->ShowWindow(SW_SHOW);
		pdlg->ShowMessage(text);
	}
	
	return;
}

// �����г�����༭
void CTGForm::OnMultiTrainsCZHEdit()
{
	CString strCzh="";
	TRAIN_INDEX train_index=0;
	if(m_sFocusTrain)
	{
		strCzh=m_sFocusTrain->myTrainInfo.shift_index_string;
		train_index=m_sFocusTrain->m_nTrainIndex;
	}
	
	CString title1, title2;
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		m_pDoc->GetJiaoluDlgName(title1, title2);
		CWnd * pWnd=CWnd::FindWindow(NULL,title1);
		if(pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
		}

		CJiaoLuEditDlg* m_pJiaoluDlg = new CJiaoLuEditDlg(title1, train_index, m_pDoc, this);
		if(NULL != m_pJiaoluDlg)
		{
			m_pJiaoluDlg->Create(IDD_DLG_JIAOLU_EIDT, this);
			m_pJiaoluDlg->ShowWindow(SW_SHOW); 
		}
	}
	else
	{
		m_pDoc->GetChezuhaoDlgName(title1, title2);
		CWnd * pWnd=CWnd::FindWindow(NULL,title1);
		if(pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
		}
		
		CZHMultiEditDlg* m_pMultiCZHDlg = new CZHMultiEditDlg(title1, strCzh, m_pDoc, this, 2);
		if(NULL != m_pMultiCZHDlg)
		{
			m_pMultiCZHDlg->Create(IDD_DLG_CZH_MULTI_EDIT, this);
			m_pMultiCZHDlg->ShowWindow(SW_SHOW); 
		}
	}
}

// �����г������۷�����
void CTGForm::OnGenTrainReturnTrainIDAll()
{
	if(!m_pctrl->mConfigTg.m_context.IsEnableCZH())
		return;
	m_pDoc->ReGenTrainCZHRelation();

	CStringArray array;
	m_pDoc->myChezuhao.GetTrainCZHText(array);
	if(array.GetCount()==0)
		return;
	
	CReturnTrainidInfo* infodlg=new CReturnTrainidInfo();
	infodlg->Create(IDD_DLG_GEN_RETURNID);
	infodlg->ShowWindow(SW_SHOW);

	infodlg->SetTotalProcessNum(array.GetCount());
	infodlg->Invalidate();

	CStringArray text;
	for(int i=0; i<array.GetCount(); i++)
	{
		CString czh = array.GetAt(i);
		m_pDoc->GenTrainReturnTrainID(czh, text);
		
		infodlg->StepAt(i+1);

		CString warning;
		if(text.GetCount()>0)
		{
			for(int i=0; i<text.GetCount(); i++)
			{
				if(i==0)
					warning = text.GetAt(i);
				else
					warning += "\r\n"+text.GetAt(i);
			}
		}
		else
			warning="�ɹ�";
		
		infodlg->AddGenMessage(warning,czh);
	}
}

//̫ԭ �޸�ʩ��������ֹʱ��
void CTGForm::OnMarkTimespan() 
{
	if (m_nFocusFigure == NULL)
		return;

	switch(m_nFocusFigure->type)
	{
		case ABS_INHIBIT_MARK:   //1.�������

		case ABS_SLOW_MARK:      //2.��������

		case SIDE_INHIBIT_MARK:  //3.վ�ڷ���

		case SIDE_SLOW_MARK:     //4.վ������
			{
				CMarkTimeSpan *dlg = new CMarkTimeSpan(m_nFocusFigure->start_time, m_nFocusFigure->end_time, this);
				if(dlg == NULL) break;

				if(dlg->DoModal () == IDOK)
				{
					dlg->getTime(m_nFocusFigure->start_time,
						m_nFocusFigure->end_time);
					m_pDoc->SendUpdateMark(m_nFocusFigure);
					m_nFocusFigure = NULL;
				}
				delete dlg; 

			}
			break;
		default:
			break;
	}
	Invalidate(FALSE);

}


BOOL CTGForm::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS && mHReceivePoint)
	{
		::SetCursor(mHReceivePoint);
		return TRUE;
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

int CTGForm::GetAbsStopGiveoutExit(std::vector<int>& line_vector, int ndirect, int recIndex, int& nExit)
{
	line_vector.clear();
	do
	{
		if(m_sFocusTrain == NULL) 
			break;

		TgTRAIN_RECORD *rec = m_sFocusTrain->GetRecordByPosition(recIndex);
		if(rec == NULL)
			break;
			
		DispDistance *distance = m_pDoc->GetDispDistanceByIdx(rec->out_station_index);
		if(distance == NULL)
			break;

		std::map<int,int> line_other;
		std::vector<int> line_ready;
		if(ndirect==UP_DIRECT)
		{
			for(int k=0; k<distance->up_line_count; k++)
			{
				int line = distance->up_line_list[k];
				line_ready.push_back(line);
			}

			for(int n=rec->out_station_index-1; n>=0; n--)
			{
				DispDistance *dist = m_pDoc->GetDispDistanceByIdx(n);
				if(dist->sub_section != distance->sub_section)
					break;

				for(WORD i=0; i < dist->down_line_count; i++)
				{
					int line = dist->down_line_list[i];
					line_other.insert(make_pair(line, line));
				}
			}
		}
		else
		{
			for(WORD i=0; i < distance->down_line_count; i++)
			{
				int line = distance->down_line_list[i];
				line_ready.push_back(line);
			}
			
			for(int n=rec->out_station_index+1; n<m_pDoc->m_nDispDistanceNum; n++)
			{
				DispDistance *dist = m_pDoc->GetDispDistanceByIdx(n);
				if(dist->sub_section != distance->sub_section)
					break;

				for(WORD i=0; i < dist->up_line_count; i++)
				{
					int line = dist->up_line_list[i];
					line_other.insert(make_pair(line, line));
				}
			}
		}
		
		for(int s=0; s<line_ready.size(); s++)
		{
			std::map<int,int>::iterator it=line_other.find(line_ready[s]);
			if(it!=line_other.end())
				line_vector.push_back(line_ready[s]);	
		}
		
		for(int s=0; s<line_vector.size(); s++)
		{
			int nEntry = m_pDoc->ConvertAbsnumStationToEntry(rec->station, line_vector[s]);
			if(nEntry>0)
			{
				nExit=nEntry;
				break;
			}
		}
		
		return line_vector.size();
	}while(0);

    return 0;	
}

void CTGForm::SetTrainEntry(CTG_TRAIN* train, int default_entry)
{
	if(train == NULL)
		return;

	TgTRAIN_RECORD *rec = train->GetFirstRecord();
	if(rec == NULL)
		return;

	if(!IsFlagAccept(rec->flag))
		return;

	rec->entry = default_entry;
	//�Ե�ǰ���г��������жϣ�����ǽ��룬�����ж�����뷽������ʾ�û�����ѡ��
	int abslineno[10];
	int nAbsNum = m_pDoc->SetTrainEntryByCfgForUI(train, 0, abslineno);
	if(nAbsNum>0)
	{
		if(nAbsNum>1)
		{
			std::vector<int> line_vector;
			for(int i=0; i<nAbsNum; i++)
			{
				line_vector.push_back(abslineno[i]);
			}

			CDlgGiveoutDirectionSel dlg(m_pDoc, train, this, 5, 0);
			dlg.entry_lines.assign(line_vector.begin(), line_vector.end());
			if(dlg.DoModal() == IDOK)
			{
				rec->entry = dlg.nSelDirect;  
			}
		}
		else
		{
			int entry  = m_pDoc->ConvertAbsnumStationToEntry(rec->station, abslineno[0]);
			if(entry > 0)
			{
				rec->entry = entry;
			}
		}
	}
	else
	{
		int nEntryNo=0;
		std::vector<int> line_vector;
		int rs = GetStaStopAcceptEntry(train, line_vector, nEntryNo, 0);
		if(rs>1)
		{
			if(this->m_pctrl->mConfigTg.m_context.IsShowAcceptChioceDlg())
			{
				CDlgGiveoutDirectionSel dlg(m_pDoc, train, this, 1, 0);
				dlg.entry_lines.assign(line_vector.begin(), line_vector.end());
				if(dlg.DoModal() == IDOK)
				{
					rec->entry = dlg.nSelDirect;  
				}
			}
			else
			{
				rec->entry = nEntryNo;
			}
		}
		else
		{
			if(nEntryNo>0)
			{				
				rec->entry = nEntryNo;
			}

			int entry=m_pDoc->GetEntry(rec->in_station_index);
			if(entry>0 && nEntryNo != entry)
			{
				rec->entry=entry;
				m_pDoc->sysprint(5,0,"[TG]: ��������[SECTION_NODE_ENTRY]Ĭ�����õ�һ�������� = %d", entry);
			}
		}
	}
	
	return;
}

void CTGForm::SetTrainExit(CTG_TRAIN* train, int default_exit)
{
	if(train == NULL)
		return;

	TgTRAIN_RECORD *rec = train->GetLastRecord();
	if(rec == NULL)
		return;

	if(!IsFlagGiveout(rec->flag))
		return;

	rec->exit = default_exit;

	//�Ե�ǰ���г��������жϣ�����ǽ����������ж��������������ʾ�û�����ѡ��
	int abslineno[10];
	UINT num=train->GetRecordNum();
	int nAbsNum = m_pDoc->SetTrainExitByCfgForUI(train, num-1, abslineno);
	if(nAbsNum>0)
	{
		if(nAbsNum>1)
		{
			std::vector<int> line_vector;
			for(int i=0; i<nAbsNum; i++)
			{
				line_vector.push_back(abslineno[i]);
			}

			CDlgGiveoutDirectionSel dlg(m_pDoc, train, this, 4, num-1);
			dlg.exit_lines.assign(line_vector.begin(), line_vector.end());
			if(dlg.DoModal() == IDOK)
			{
				rec->exit = dlg.nSelDirect;
			}
		}
		else
		{
			int exit  = m_pDoc->ConvertAbsnumStationToEntry(rec->station, abslineno[0]);
			if(exit > 0)
			{
				rec->exit  = exit;
			}
		}
	}
	else
	{
		int nExit=0;
		bool bSelAllLine=false;   // 2012.2.10 �Ϻ�����¤��
		std::vector<int> line_vector;
		int rs = GetStaStopGiveoutExit(train, line_vector, nExit, num-1, bSelAllLine);
		if(rs>1)
		{
			if(bSelAllLine)
			{
				CDlgGiveoutDirectionSel dlg(m_pDoc, train, this, 6, num-1);
				dlg.exit_lines.assign(line_vector.begin(), line_vector.end());
				if(dlg.DoModal() == IDOK)
				{
					rec->exit = dlg.nSelDirect;  
				}
			}
			else
			{
				CDlgGiveoutDirectionSel dlg(m_pDoc, train, this, 0, num-1);
				dlg.exit_lines.assign(line_vector.begin(), line_vector.end());
				if(dlg.DoModal() == IDOK)
				{
					rec->exit = dlg.nSelDirect;  
				}
			}
		}
		else
		{
			if(nExit>0)
			{
				rec->exit = nExit;
			}
		}
	}

	return;
}

void CTGForm::OnMarkAutoMoveStart()
{
	if(m_nFocusFigure == NULL||m_pDoc==NULL)	
		return;

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsAutoAddBlockEndTime(m_nFocusFigure->type))
	{
		m_pDoc->AddMarkMoveIndex(m_nFocusFigure->mark_index);
	}
}

void CTGForm::OnMarkAutoMoveStop()
{
	if(m_nFocusFigure == NULL||m_pDoc==NULL)	
		return;

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsAutoAddBlockEndTime(m_nFocusFigure->type))
	{
		m_pDoc->DelMarkMoveIndex(m_nFocusFigure->mark_index);
	}
}

void CTGForm::CheckMarkMove()
{
	if(m_pDoc==NULL)
		return;

	int nTimeAdd=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetAutoAddBlockEndTime();
	if(m_pDoc->myPDataManage==NULL || nTimeAdd<=0)
		return;
	
	CTime curr_time = CTime::GetCurrentTime();
	m_pDoc->sysprint(10,0, "check mark_move_index size=%d", m_pDoc->mark_move_index.size());

	std::map<ULONG,ULONG>::iterator it=m_pDoc->mark_move_index.begin();
	for(; it!=m_pDoc->mark_move_index.end(); it++)
	{
		if(m_pDoc->mark_move_index.empty())
			break;

		if(it->second<=0)
			continue;

		CTg_BLOCK* pBlock=m_pDoc->myPDataManage->GetMark(it->first);
		if(pBlock==NULL)
		{
			m_pDoc->DelMarkMoveIndex(it->first);
			continue;
		}

		if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsAutoAddBlockEndTime(pBlock->type))
		{
			m_pDoc->DelMarkMoveIndex(it->first);
			continue;
		}

		if((pBlock->end_time>curr_time.GetTime()) &&(pBlock->end_time - curr_time.GetTime() <= 60*60*6))
		{
			CTg_BLOCK block=*pBlock;
			block.end_time += nTimeAdd*60;
			m_pDoc->SendUpdateMark(block);
		}
	}
}

///���ݾ֣��ÿ��г���ͱ�־�޸Ĺɵ���ʾ�������������� hjz 20120216
void CTGForm::MessageBoxByChangeSide(TgTRAIN_RECORD *record, CString ArrSide, CString DepSide)
{
	if(record==NULL)
		return;

	if (!IsTechStopTrain(*record))
	{
		CString PlanArrSide = m_pDoc->GetSideName(record->station, record->plan_arrive_side);
		CString PlanDepSide = m_pDoc->GetSideName(record->station, record->plan_depart_side);

		CString TrainID=record->arrive_train_id;
		if(TrainID=="")
			TrainID=record->depart_train_id;

		CString text;
		text.Format("��ʾ�� �뷢�� %s վ %s �α�� %s �����ĵ������",m_pDoc->GetStationName(record->station), TrainID, DepSide);
		if (AfxMessageBox(text,MB_OKCANCEL) != IDOK)
			return;
		if (this->m_pctrl->mConfigTg.m_context.IsGenDCMDByChangeSide())
		{
			OnAutoGenDCmd2(TrainID, record, PlanArrSide, PlanDepSide, ArrSide, DepSide);
		}
	}
}
void CTGForm::OnAutoGenDCmd2(CString TrainID, TgTRAIN_RECORD *record,CString PlanArrSide, CString PlanDepSide, CString ArrSide, CString DepSide) 
{
	if(m_sFocusTrain == NULL || record==NULL) 
		return;

	int stytle = 3; // 3:������������
	CString dcmd = GetDcmdDesp(m_sFocusTrain, record, stytle, PlanArrSide, PlanDepSide, ArrSide, DepSide);
	if(!dcmd.IsEmpty())
	{	
		m_pDoc->m_ptgctrl->TG_GEN_BLOCK_DCMD(dcmd, 1, stytle);
		m_pDoc->sysprint(4,0,"����Ա���� %s վ %s �α�� %s �����ĵ������",m_pDoc->GetStationName(record->station), TrainID, DepSide);
	}
}
CString CTGForm::GetDcmdDesp(CTG_TRAIN* train, TgTRAIN_RECORD *record, int& stytle, CString PlanArrSide, CString PlanDepSide, CString ArrSide, CString DepSide)
{
	CString tmp;
	CString xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?> ";
	
	xml += tmp;

	xml += "<TrainID>";
	tmp.Format("%s", m_pDoc->GetTrainShowId(train));
	xml += tmp;
	xml += "</TrainID>";
	
	xml += "<Stations>";
	tmp.Format("%s", m_pDoc->GetStationName(record->station));
	xml += tmp;
	xml += "</Stations>";
	
	tmp += "<PLAN_STA_ARR_SIDE>";
	tmp.Format("%s",PlanArrSide);
	xml += tmp;
	tmp += "</PLAN_STA_ARR_SIDE>";
	
	tmp += "<PLAN_STA_DEP_SIDE>";
	tmp.Format("%s",PlanDepSide);
	xml += tmp;
	tmp += "</PLAN_STA_DEP_SIDE>";
	
	tmp += "<ARR_SIDE>";
	tmp.Format("%s",ArrSide);
	xml += tmp;
	tmp += "</ARR_SIDE>";	//2012-5-2

	tmp += "<DEP_SIDE>";
	tmp.Format("%s",DepSide);
	xml += tmp;
	tmp += "</DEP_SIDE>";

	return xml;
}

void CTGForm::OnSetMarkActiveStatus(UINT nID)
{
	if(!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus())
		return;
	if(m_nFocusFigure == NULL)	
		return;
	if(m_nFocusFigure->mark_index < 0)	
		return;

	switch (nID)
	{
	case ID_MARK_PLAN:
		{
			SetBlockPlan(m_nFocusFigure->adjust_status);
			break;
		}
	case ID_MARK_ACTIVE:
		{
			SetBlockActive(m_nFocusFigure->adjust_status);
			break;
		}
	case ID_MARK_COMPLETE:
		{
			SetBlockComplete(m_nFocusFigure->adjust_status);
			break;
		}
	}
	m_pDoc->SendUpdateMark(m_nFocusFigure);	m_nFocusFigure = NULL;
}

void CTGForm::ResetFocusTrain()
{
	if((m_sFocusTrain != NULL) && (m_pDoc!=NULL))
	{
		if(m_pDoc->m_nOperatorStatus==NORMAL_STATUS)
		{
			m_nFocusTrainIndex=0;
			m_nFocusRectIndex=-1;
			m_nFocusRecordIndex=-1;
			RefreshTrain(m_sFocusTrain);
			m_sFocusTrain=NULL;
		}
	}
}

void CTGForm::OnNewShiftBanci()
{
	m_pDoc->SetDocContext();
	m_pDoc->SetSchdBeginTime();
	m_sFocusTrain = NULL;
	m_nFocusFigure = NULL;
	
	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);

	SIZE mSize;
	mSize.cx = m_pDoc->m_nPageWidth;
	mSize.cy = m_pDoc->m_nPageHeight;
	SetScrollSizes(MM_TEXT,mSize);

	CClientDC dc(this);
	OnPrepareDC(&dc);
	ReLayoutBmpPageSize(dc);

	SetRightScrollPosition();
	
	m_pDoc->m_nOperatorStatus = NORMAL_STATUS;

	Redraw();
	
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainType(3);
	m_pDoc->m_ptgctrl->mConfigTg.m_context.SetShowTrainDirect(DIRECT::DUAL);
}

void CTGForm::DrawTimeAixisExt(CDC *pDC,ULONG lColor)
{
	int nAixixYNum = CalcAixixY();

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  1, lColor);
	mBoldPen.CreatePen(PS_SOLID, 2, lColor);

	pOldPen = pDC->SelectObject(&mThinPen);
    //////////////////////////////////////////////////////////////////////
	int XX=0;
	int timeRange = m_pctrl->GetTimeLength();
	int nMiniute  = 0;
	if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
	{
		CTime tm(m_pDoc->GetBeginTime());
		nMiniute  = tm.GetMinute();
	}
	for(int k=0; k<= timeRange*60; k++)
	{
		if(nMiniute%10==0)
		{
			if(nMiniute%60 == 0)
			{
				pDC->SelectObject(&mBoldPen);
				for(int k = 0; k <= nAixixYNum; k++)
				{
					pDC->MoveTo(XX, AixisY[k].nTopY);			
					pDC->LineTo(XX, AixisY[k].nBottomY);
				}
			}
			else if(nMiniute%30 == 0)
			{
				pDC->SelectObject(&mDashPen);
				for(int k = 0; k <= nAixixYNum; k++)
				{
					pDC->MoveTo(XX, AixisY[k].nTopY);			
					pDC->LineTo(XX, AixisY[k].nBottomY);
				}
			}
			else
			{
				// �Ż���Сʱ����Ҫ��ʮ�ָ���
				if(m_pDoc->m_nADTimeSystle != 2)
				{
					pDC->SelectObject(&mThinPen);
					for(int k = 0; k <= nAixixYNum; k++)
					{
						pDC->MoveTo(XX, AixisY[k].nTopY);			
						pDC->LineTo(XX, AixisY[k].nBottomY);
					}
				}
			}
		}
		
		nMiniute += 1;
		XX += m_pDoc->m_fPixelPerMinute;
	}
	//////////////////////////////////////////////////////////////////////
	pDC->SelectObject(pOldPen);
	mThinPen.DeleteObject();
	mDashPen.DeleteObject();
	mBoldPen.DeleteObject();
}

void CTGForm::OnDrawBackgrondExt(CDC* pDC, const CRect& DrawRect)
{
	if(FALSE==m_pDoc->m_nAutoMove)
	{
		int nTmp = 12*m_pDoc->m_fPixelPerHour;
				
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			int uStartX = (int)(m_pDoc->GetHistoryRegionWidth() * m_pDoc->m_fPixelPerMinute);

			CRect InterRect;
			CRect HisRect(0, 0, nTmp, m_pDoc->m_nPageHeight);
			CRect ActRect(nTmp, 0, uStartX, m_pDoc->m_nPageHeight);
			// �ٽ���10����,Ӧ�ø���ʵ��ÿ����������������,��ͬ
			CRect NearRect(uStartX, 0, uStartX+10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight);
			CRect PartPlanRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX + m_pDoc->m_fPixelPerHour*3, m_pDoc->m_nPageHeight);
			CRect PlanRect(uStartX + m_pDoc->m_fPixelPerHour*3, 0, m_pDoc->m_fPixelPerHour*30, m_pDoc->m_nPageHeight);

			if( InterRect.IntersectRect( HisRect, DrawRect ) )
				pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)) );    // ��ʷ��

			if(m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			{
				if( InterRect.IntersectRect( ActRect, DrawRect ) )
					pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30)) );   // ʵ����

				if( InterRect.IntersectRect( NearRect, DrawRect ) )
					pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20)) );   // �ٽ���

				if( InterRect.IntersectRect( PartPlanRect, DrawRect ) )
					pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10)) );   // �׶μƻ���

				if( InterRect.IntersectRect( PlanRect, DrawRect ) )
					pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)) );    // �ƻ���
			}
			else
			{
				if(nTmp > DrawRect.left && nTmp < DrawRect.right )
				{
					CRect leftRect(DrawRect.left ,DrawRect.top, nTmp, DrawRect.bottom);
					CRect rightRect(nTmp, DrawRect.top, DrawRect.right, DrawRect.bottom);
					pDC->FillRect(leftRect,  &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
					pDC->FillRect(rightRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
				}
				else if(nTmp > DrawRect.right)
					pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
				else
					pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
			}
		}
		else if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::TWO)
		{
			if(nTmp > DrawRect.left && nTmp < DrawRect.right )
			{
				CRect leftRect(DrawRect.left ,DrawRect.top, nTmp, DrawRect.bottom);
				CRect rightRect(nTmp, DrawRect.top, DrawRect.right, DrawRect.bottom);
				pDC->FillRect(leftRect,  &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
				pDC->FillRect(rightRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
			}
			else if(nTmp > DrawRect.right)
				pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
			else
				pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
		}
		else
		{
			pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
		}

		OnDrawTimeAixis(pDC, DrawRect, CLR_LAWNGREEN);
		OnDrawStationAixis(pDC, DrawRect, CLR_LAWNGREEN);

		// draw seperatorbar
		if( m_pctrl->mConfigTg.m_context.IsDrawCurrTimeBar() && m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			m_pDoc->DrawSeperatorBar(pDC, CLR_BLUE);
	}
	else
	{
		int uStartX = m_pDoc->GetHistoryRegionWidth() * m_pDoc->m_fPixelPerMinute;

		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::TWO)
		{
			if(uStartX > DrawRect.left && uStartX < DrawRect.right )
			{
				CRect leftRect(DrawRect.left ,DrawRect.top, uStartX, DrawRect.bottom);
				CRect rightRect(uStartX, DrawRect.top, DrawRect.right, DrawRect.bottom);
				pDC->FillRect(leftRect,  &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
				pDC->FillRect(rightRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
			}
			else if(uStartX > DrawRect.right)
				pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40)));
			else
				pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
		}
		else if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			CRect InterRect;
			CRect ActRect(0, 0, uStartX, m_pDoc->m_nPageHeight);
			CRect NearRect(uStartX, 0, uStartX+10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight);
			CRect PartPlanRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX + m_pDoc->m_fPixelPerHour*3, m_pDoc->m_nPageHeight);
			CRect PlanRect(uStartX + m_pDoc->m_fPixelPerHour*3, 0, m_pDoc->m_fPixelPerHour*24, m_pDoc->m_nPageHeight);

			if( InterRect.IntersectRect( ActRect, DrawRect ) )
				pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30)) );   // ʵ����

			if( InterRect.IntersectRect( NearRect, DrawRect ) )
				pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20)) );   // �ٽ���

			if( InterRect.IntersectRect( PartPlanRect, DrawRect ) )
				pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10)) );   // �׶μƻ���

			if( InterRect.IntersectRect( PlanRect, DrawRect ) )
				pDC->FillRect( InterRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)) );    // �ƻ���
		}
		else
		{
			pDC->FillRect(DrawRect, &CBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor)));
		}

		OnDrawTimeAixisExt(pDC, DrawRect, CLR_LAWNGREEN);
		OnDrawStationAixis(pDC, DrawRect, CLR_LAWNGREEN);

		// draw seperatorbar
		if( m_pctrl->mConfigTg.m_context.IsDrawCurrTimeBar() && m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			m_pDoc->DrawSeperatorBar(pDC, CLR_BLUE);
	}
}

void CTGForm::OnDrawTimeAixisExt(CDC* pDC, const CRect& DrawRect,ULONG lColor)
{
	int nAixixYNum = CalcAixixY();

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  1, lColor);
	mBoldPen.CreatePen(PS_SOLID, 2, lColor);

	pOldPen = pDC->SelectObject(&mThinPen);
	///////////////////////////////////////////////////////////////
	int XX=0;
	int timeRange = m_pctrl->GetTimeLength();
	int nMiniute  = 0;
	if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
	{
		CTime tm(m_pDoc->GetBeginTime());
		nMiniute  = tm.GetMinute();
	}
	for(int k=0; k<= timeRange*60; k++)
	{
		if((nMiniute%10==0) && (DrawRect.left<=XX && XX<=DrawRect.right))
		{
			if(nMiniute%60 == 0)
			{
				pDC->SelectObject(&mBoldPen);
				for(int k = 0; k <= nAixixYNum; k++)
				{
					pDC->MoveTo(XX, AixisY[k].nTopY);			
					pDC->LineTo(XX, AixisY[k].nBottomY);
				}
			}
			else if(nMiniute%30 == 0)
			{
				pDC->SelectObject(&mDashPen);
				for(int k = 0; k <= nAixixYNum; k++)
				{
					pDC->MoveTo(XX, AixisY[k].nTopY);			
					pDC->LineTo(XX, AixisY[k].nBottomY);
				}
			}
			else
			{
				// �Ż���Сʱ����Ҫ��ʮ�ָ���
				if(m_pDoc->m_nADTimeSystle != 2)
				{
					pDC->SelectObject(&mThinPen);
					for(int k = 0; k <= nAixixYNum; k++)
					{
						pDC->MoveTo(XX, AixisY[k].nTopY);			
						pDC->LineTo(XX, AixisY[k].nBottomY);
					}
				}
			}
		}
		
		nMiniute += 1;
		XX += m_pDoc->m_fPixelPerMinute;
	}
	//////////////////////////////////////////////////////
	pDC->SelectObject(pOldPen);
	mThinPen.DeleteObject();
	mDashPen.DeleteObject();
	mBoldPen.DeleteObject();
}

void CTGForm::RedrawLayer2()
{
	CBrush  m_brush;
	m_brush.CreateSolidBrush(RGB(255-m_InitColor,255-m_InitColor,255-m_InitColor));
	m_Layer1.FillRect(CRect(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight),&m_brush);
	m_brush.DeleteObject();

	if(FALSE==m_pDoc->m_nAutoMove)
	{
		CRect lastRect;
		if(m_pDoc->m_nBlockNum >= BACKGROUD::BLOCK::TWO)
		{
			// ��ʷͼ��
			lastRect = CRect(0,0,m_pDoc->m_fPixelPerMinute*12*60,m_pDoc->m_nPageHeight );

			m_brush.CreateSolidBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40));
			m_Layer1.FillRect(&lastRect,&m_brush);
			m_brush.DeleteObject();
		}
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			int nHistoryGraphWidth = m_pDoc->GetHistoryRegionWidth();
			int uStartX = nHistoryGraphWidth * m_pDoc->m_fPixelPerMinute - 1;

			if(m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			{
				// ʵ��ͼ��
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30));
				lastRect = CRect(m_pDoc->m_fPixelPerMinute*12*60, 0, uStartX, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �ٽ�ͼ����10���ӣ�
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20));
				lastRect = CRect(uStartX, 0, uStartX + 10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight ); // �ٽ���10����,Ӧ�ø���ʵ��ÿ����������������,��ͬ
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �׶μƻ�����3Сʱ�����ٽ��ƻ�����
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10));
				lastRect = CRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX +m_pDoc->m_fPixelPerMinute*3*60, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();
			}
		}
		this->DrawStationAixis(&m_Layer1,CLR_LAWNGREEN);
		this->DrawTimeAixisExt(&m_Layer1,CLR_LAWNGREEN);

		// draw seperatorbar
		if(m_pctrl->mConfigTg.m_context.IsDrawCurrTimeBar() && m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			m_pDoc->DrawSeperatorBar(&m_Layer1, CLR_BLUE);	
	}
	else
	{
		int nHistoryWidth = m_pDoc->GetHistoryRegionWidth();
		int uStartX =X_TIME_OFFSET + nHistoryWidth*m_pDoc->m_fPixelPerMinute;

		CRect lastRect;
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::TWO)
		{
			// ��ʷͼ��
			m_brush.CreateSolidBrush(RGB(255-m_InitColor-40,255-m_InitColor-40,255-m_InitColor-40));
			lastRect = CRect(0, 0, uStartX, m_pDoc->m_nPageHeight);
			m_Layer1.FillRect(&lastRect,&m_brush);
			m_brush.DeleteObject();
		}
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
		{
			if(m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			{
				// ʵ��ͼ��
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-30,255-m_InitColor-30,255-m_InitColor-30));
				lastRect = CRect(0, 0, uStartX, m_pDoc->m_nPageHeight);
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �ٽ�ͼ����10���ӣ�
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-20,255-m_InitColor-20,255-m_InitColor-20));
				lastRect = CRect(uStartX, 0, uStartX + 10*m_pDoc->m_fPixelPerMinute, m_pDoc->m_nPageHeight ); // �ٽ���10����,Ӧ�ø���ʵ��ÿ����������������,��ͬ
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();

				// �׶μƻ�����3Сʱ�����ٽ��ƻ�����
				m_brush.CreateSolidBrush(RGB(255-m_InitColor-10,255-m_InitColor-10,255-m_InitColor-10));
				lastRect = CRect(uStartX + 10*m_pDoc->m_fPixelPerMinute, 0, uStartX +m_pDoc->m_fPixelPerMinute*3*60, m_pDoc->m_nPageHeight );
				m_Layer1.FillRect(&lastRect,&m_brush);
				m_brush.DeleteObject();
			}

			m_Layer1.SetBkMode(TRANSPARENT);
		}
		DrawStationAixis(&m_Layer1,CLR_LAWNGREEN);
		DrawTimeAixisExt(&m_Layer1,CLR_LAWNGREEN);

		// draw seperatorbar
		if(m_pctrl->mConfigTg.m_context.IsDrawCurrTimeBar() && m_pDoc->m_ptgctrl->mConfigTg.m_bDrawCurrentFlag)
			m_pDoc->DrawSeperatorBar(&m_Layer1, CLR_BLUE);	
	}

	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	for(;it_block != myPDataManage->myMark.end();++it_block)
	{
		m_pDoc->DrawSingleMark(it_block->second, &m_Layer1);
	}
}

// 3.0����ͼ�ɵ�����
void CTGForm::OnKKBasicSide()
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	CTG_TRAIN train = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = train.GetRecordByPosition (m_nFocusRecordIndex );
	if(rec == NULL)
		return;

	if(!m_pDoc->m_ptgctrl->IsBPlanStation(rec->station))
	{
		CString str;
		str.Format("��ע��, %s �������޸Ļ���ͼ�ɵ���������", m_pDoc->GetStationName(rec->station));
		MessageBox(str, "����", MB_OK);
		return;
	}

	do
	{
		bool bCheckTrainSide=false;
		CString str;
		if(TG_IsNotCheckTrainSide(*rec))
		{
			str.Format("��ע��, ��ȷʵҪ���� %s(%d) �� %s ���ػ���ͼ�ɵ���", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));

			m_pDoc->sysprint(4,0,"%s", str);
			if(IDYES==MessageBox(str, "����", MB_YESNO))
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��.");
				bCheckTrainSide=true;
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��."); 
				break;
			}
		}
		else
		{
			str.Format("��ע��, ��ȷʵҪ���� %s(%d) �� %s ȡ�����ػ���ͼ�ɵ���", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
			m_pDoc->sysprint(4,0,"%s", str);
			if(IDYES==MessageBox(str, "����", MB_YESNO))
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��.");
				bCheckTrainSide=false;
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��."); 
				break;
			}
		}

		CSuperPassDlg pdlg;
		if (pdlg.DoModal() == IDOK)
		{
			if ( pdlg.m_super_pass.Compare("1234") != 0)
			{
				CString str;
				str.Format("%s","�������!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				break;
			}
		}
		else
		{
			break;
		}

		if(bCheckTrainSide)
		{
			TG_CheckTrainSide(*rec);
			m_pDoc->sysprint(4,0,"����Ա���� %s(%d) �� %s ���ػ���ͼ�ɵ�.", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
		}
		else
		{
			TG_NotCheckTrainSide(*rec);
			m_pDoc->sysprint(4,0,"����Ա���� %s(%d) �� %s ȡ�����ػ���ͼ�ɵ�.", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
		}

		m_pDoc->SendUpdateTrain(train);
	}while(0);
}

// 3.0����ͼ����ڿ���
void CTGForm::OnKKBasicEntry()
{
	if(m_sFocusTrain == NULL)
		return;
	if(m_nFocusRecordIndex <0)
		return;

	CTG_TRAIN train = *m_sFocusTrain;
	TgTRAIN_RECORD *rec;
	rec = train.GetRecordByPosition (m_nFocusRecordIndex );
	if(rec == NULL)
		return;

	if(!m_pDoc->m_ptgctrl->IsBPlanStation(rec->station))
	{
		CString str;
		str.Format("��ע��, %s �������޸Ļ���ͼ����ڿ�������", m_pDoc->GetStationName(rec->station));
		MessageBox(str, "����", MB_OK);
		return;
	}

	do
	{
		bool bCheckTrainExitEntry=false;
		if(TG_IsNotCheckTrainExitEntry(*rec))
		{
			CString str;
			str.Format("��ע��, ��ȷʵҪ���� %s(%d) �� %s ���ػ���ͼ�������", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));

			m_pDoc->sysprint(4,0,"%s", str);
			if(IDYES==MessageBox(str, "����", MB_YESNO))
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��.");
				bCheckTrainExitEntry=true;
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��."); 
				break;
			}
		}
		else
		{
			CString str;
			str.Format("��ע��, ��ȷʵҪ���� %s(%d) �� %s ȡ�����ػ���ͼ�������", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
			m_pDoc->sysprint(4,0,"%s", str);
			if(IDYES==MessageBox(str, "����", MB_YESNO))
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��.");
				bCheckTrainExitEntry=false;
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Աѡ�� ��."); 
				break;
			}
		}

		CSuperPassDlg pdlg;
		if (pdlg.DoModal() == IDOK)
		{
			if ( pdlg.m_super_pass.Compare("1234") != 0)
			{
				CString str;
				str.Format("%s","�������!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				break;
			}
		}
		else
		{
			break;
		}

		if(bCheckTrainExitEntry)
		{
			TG_CheckTrainExitEntry(*rec);
			m_pDoc->sysprint(4,0,"����Ա���� %s(%d) �� %s ���ػ���ͼ�����.", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
		}
		else
		{
			TG_NotCheckTrainExitEntry(*rec);
			m_pDoc->sysprint(4,0,"����Ա���� %s(%d) �� %s ȡ�����ػ���ͼ�����.", 
					m_pDoc->GetTrainShowId(train), train.GetTrainIndex(), m_pDoc->GetStationName(rec->station));
		}

		m_pDoc->SendUpdateTrain(train);
	}while(0);
}


void CTGForm::MoveTGOCXOneStep()
{
	if(m_pctrl->IsWorkMode())
	{
		if(TRUE==m_pDoc->m_nAutoMove)
		{
			OnMoveLayerOneStep2();
		}
		RedrawLayer2();
		
		Invalidate(FALSE);
	}
	else
	{
		if(TRUE==m_pDoc->m_nAutoMove)
		{
			OnMoveLayerOneStep();
		}
		int nHistoryGraphWidth = m_pDoc->GetHistoryRegionWidth();
		int uStartX = nHistoryGraphWidth * m_pDoc->m_fPixelPerMinute;

		CRect regine;
		if(m_pDoc->m_nBlockNum == BACKGROUD::BLOCK::FIVE)
			regine=CRect(uStartX-30, 0, uStartX + m_pDoc->m_fPixelPerHour*3+10, m_pDoc->m_nPageHeight);
		else 
			regine=CRect(uStartX-30, 0, uStartX + 30, m_pDoc->m_nPageHeight);
		EraseRegion(regine);
	}
}

void CTGForm::OnMoveLayerOneStep()
{
	// 1.��������ʱ��
	TIME dltT0=m_pDoc->GetBeginTime();
	m_pDoc->SetSchdBeginTime();
	TIME dltT=m_pDoc->GetBeginTime();

	int dltMinute=(dltT-dltT0)/60;
	if(dltMinute==0)  // �����ж�
		return;
	
	TIME begin=0, end=0;
	m_pDoc->GetViewScope(begin, end);
	CTime sTime(begin), eTime(end);
	m_pDoc->sysprint(0,0, "OnMoveLayerOneStep OCX��ǰ���ӷ�Χ:%s -- %s,�г�����=%d ʩ������=%d", 
					sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"), 
					myPDataManage->myTrain.size(), myPDataManage->myMark.size());

	// 2.ƽ��ͼƬdlt����
	m_Layer1.BitBlt(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight, &m_Layer1, dltMinute*m_pDoc->m_fPixelPerMinute, 0, SRCCOPY);
	
	// 3.ˢ�¶����dlt����ͼƬ
	CRect regine(m_pDoc->m_nPageWidth-m_pDoc->m_fPixelPerMinute*dltMinute, 0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight);
	EraseRegion(regine);

	// 4.ƫ���г�
	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	while(it_block != myPDataManage->myMark.end())
	{
		if(it_block->second.m_nUpdateTime>0&&it_block->second.m_nUpdateTime!=dltT)
		{
			int dlt=it_block->second.m_nUpdateTime-dltT;
			it_block->second.OffsetPosition((dlt*m_pDoc->m_fPixelPerMinute)/60);
			it_block->second.m_nUpdateTime=dltT;
		}
		it_block++;
	}

	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	while(it != myPDataManage->myTrain.end())
	{
		if(it->second.m_nUpdateTime>0&&it->second.m_nUpdateTime!=dltT)
		{
			int dlt=it->second.m_nUpdateTime-dltT;
			it->second.OffsetPosition((dlt*m_pDoc->m_fPixelPerMinute)/60);
			it->second.m_nUpdateTime=dltT;
		}
		it++;
	}

	// 5.��ʾ
	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);
	CRect regine1(0, 0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight);
	RefreshRect(regine1);
}

void CTGForm::OnMoveLayerOneStep2()
{
	// 1.��������ʱ��
	TIME dltT0=m_pDoc->GetBeginTime();
	m_pDoc->SetSchdBeginTime();
	TIME dltT=m_pDoc->GetBeginTime();

	int dltMinute=(dltT-dltT0)/60;
	if(dltMinute==0)
		return;
	
	TIME begin=0, end=0;
	m_pDoc->GetViewScope(begin, end);
	CTime sTime(begin), eTime(end);
	m_pDoc->sysprint(0,0, "OnMoveLayerOneStep2 OCX��ǰ���ӷ�Χ:%s -- %s,�г�����=%d ʩ������=%d", 
					sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"), 
					myPDataManage->myTrain.size(), myPDataManage->myMark.size());
	
	// 2.ƽ��ͼƬdlt����
	m_Layer2.BitBlt(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight, &m_Layer2, dltMinute*m_pDoc->m_fPixelPerMinute, 0, SRCCOPY);
	m_Layer3.BitBlt(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight, &m_Layer3, dltMinute*m_pDoc->m_fPixelPerMinute, 0, SRCCOPY);
	m_Layer4.BitBlt(0,0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight, &m_Layer4, dltMinute*m_pDoc->m_fPixelPerMinute, 0, SRCCOPY);
	
	// 3.ˢ�¶����dlt����ͼƬ
	CRect newRect(m_pDoc->m_nPageWidth-m_pDoc->m_fPixelPerMinute*dltMinute, 0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight);
	m_Layer2.FillRect(&newRect, &m_pDoc->m_brushWhite);
	m_Layer3.FillRect(&newRect, &m_pDoc->m_brushWhite);
	m_Layer4.FillRect(&newRect, &m_pDoc->m_brushWhite);

	CRect rangeRect;
	std::map<ULONG, CTG_TRAIN>::iterator it_train = myPDataManage->myTrain.begin();
	for(;it_train != myPDataManage->myTrain.end();++it_train)
	{
		for(int i = 0; i < it_train->second.days; i++)
		{
			if(rangeRect.IntersectRect(&newRect, &it_train->second.myPosition[i]))
			{
				if(it_train->second.mbDoubleDirect)
					m_pDoc->DrawSingleTrain(&m_Layer3, &it_train->second, DRAW_NORMAL);
				else if(it_train->second.m_nDirectFlag==UP_DIRECT)
					m_pDoc->DrawSingleTrain(&m_Layer4, &it_train->second, DRAW_NORMAL);
				else
					m_pDoc->DrawSingleTrain(&m_Layer2, &it_train->second, DRAW_NORMAL);
				break;
			}
		}
	}
	//���Ƴ��鹴����
	m_pDoc->DrawAllCheZuHaoLine(&m_Layer3);
		
	// 4.ƫ���г�
	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	while(it_block != myPDataManage->myMark.end())
	{
		if(it_block->second.m_nUpdateTime>0&&it_block->second.m_nUpdateTime!=dltT)
		{
			int dlt=it_block->second.m_nUpdateTime-dltT;
			it_block->second.OffsetPosition((dlt*m_pDoc->m_fPixelPerMinute)/60);
			it_block->second.m_nUpdateTime=dltT;
		}
		it_block++;
	}

	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	while(it != myPDataManage->myTrain.end())
	{
		if(it->second.m_nUpdateTime>0&&it->second.m_nUpdateTime!=dltT)
		{
			int dlt=it->second.m_nUpdateTime-dltT;
			it->second.OffsetPosition((dlt*m_pDoc->m_fPixelPerMinute)/60);
			it->second.m_nUpdateTime=dltT;
		}
		it++;
	}

	// 5.��ʾ
	m_pDoc->UpdateAllViews(NULL,GRAPH_BEGIN_TIME_CHANGE,NULL);
	CRect regine(0, 0, m_pDoc->m_nPageWidth, m_pDoc->m_nPageHeight);
	RefreshRect(regine);
}

void CTGForm::OnShowSingleTrain()
{
	if(m_pDoc->m_bShowSingleTrain)
	{
		m_pDoc->m_bShowSingleTrain=false;
		m_pDoc->myPDataManage->SetSingleTrain(false);
	}
	else
	{
		m_pDoc->m_bShowSingleTrain=true;
		m_pDoc->myPDataManage->SetSingleTrain(true);
	}
	this->Redraw();
}

void CTGForm::SendCzhOtherTrainsUpdate(DWORD train_index)
{
	for(int i=0; i<m_focusCzhTrains.m_focusCzhTrains.size(); i++)
	{
		if(train_index==m_focusCzhTrains.m_focusCzhTrains[i].GetTrainIndex())
			continue; // train_index�Ѿ��������͹�
		m_pDoc->SendUpdateTrain(m_focusCzhTrains.m_focusCzhTrains[i], SCHD_UPDATE);
	}
}

// 2018.07.30�г��ֲ���Ԯ
void CTGForm::OnAbsPartRun()
{
	if(m_sFocusTrain == NULL)	
	{
		MessageBox("��ѡ��Ҫ�ֲ����е��г�!");
		return;
	}
	CTG_TRAIN newtrain = *m_sFocusTrain;
	TgTRAIN_RECORD *prec_cur = newtrain.GetRecordByPosition (m_nFocusRecordIndex);
	TgTRAIN_RECORD *prec_nxt = newtrain.GetRecordByPosition (m_nFocusRecordIndex + 1);
	if(prec_cur==NULL || prec_nxt==NULL)
	{
		MessageBox("ѡ���г����һվ(û�о�Ԯ����),�޷�ʵ�־�Ԯ!");
		return;
	}
	int station = prec_cur->station;
	int station_next = prec_nxt->station;
	
	CTrainPartRunDlg dlg(m_pDoc, this, station, station_next);
	if(dlg.DoModal () == IDOK)
	{
		if(dlg.m_start_station==station && dlg.m_end_station==station_next)
		{
			CreatePartHelpRunStation(newtrain, dlg.m_sStaionid, dlg.m_sRetTrainId, false);
		}
		if(dlg.m_start_station==station_next && dlg.m_end_station==station)
		{
			CreatePartHelpRunStation(newtrain, dlg.m_sStaionid, dlg.m_sRetTrainId, true);
		}
	}
}

// 2018.09.18�г����о�Ԯ
void CTGForm::OnAbsWholeHelpRun()
{
	if(m_sFocusTrain == NULL)	
	{
		MessageBox("��ѡ��Ҫ�ֲ����е��г�!");
		return;
	}
	CTG_TRAIN newtrain = *m_sFocusTrain;
	TgTRAIN_RECORD *prec_cur = newtrain.GetRecordByPosition (m_nFocusRecordIndex);
	TgTRAIN_RECORD *prec_nxt = newtrain.GetRecordByPosition (m_nFocusRecordIndex + 1);
	if(prec_cur==NULL || prec_nxt==NULL)
	{
		MessageBox("ѡ���г����һվ(û�о�Ԯ����),�޷�ʵ�־�Ԯ!");
		return;
	}
	int station      = prec_cur->station;
	int station_next = prec_nxt->station;
	
	CTrainPartRunDlg dlg(m_pDoc, this, station, station_next);
	if(dlg.DoModal () == IDOK)
	{
		if(dlg.m_start_station==station && dlg.m_end_station==station_next)
		{
			CreateWholeHelpRunStation(newtrain, dlg.m_sStaionid, dlg.m_sRetTrainId, false);
		}
		if(dlg.m_start_station==station_next && dlg.m_end_station==station)
		{
			CreateWholeHelpRunStation(newtrain, dlg.m_sStaionid, dlg.m_sRetTrainId, true);
		}
	}
}

// 2018.07.30����;���۷�
void CTGForm::OnAttainEngineReturn()
{
	if(m_sFocusTrain == NULL)	
	{
		MessageBox("��ѡ��Ҫ����;���۷����������ǰ��վ");
		return;
	}
	CTG_TRAIN newtrain = *m_sFocusTrain;

	TgTRAIN_RECORD *prec = newtrain.GetRecordByPosition (m_nFocusRecordIndex);
	TgTRAIN_RECORD *prec_next = newtrain.GetRecordByPosition (m_nFocusRecordIndex + 1);
	if(prec_next == NULL || prec == NULL)
	{
		MessageBox("��ѡ��Ҫ����;���۷����������ǰ��վ");
		return;
	}

	CInsertSimpleDlg *dlg = new CInsertSimpleDlg("���벹��;���۷�����", this, 0);
	if(dlg == NULL)
	{
		m_pDoc->sysprint(0,0,"[TG]: alloc memory failed in file %s at line %d",
			__FILE__,__LINE__);
		return;
	}

	if(dlg->DoModal () == IDOK)
	{
		CTG_TRAIN rescue_train;
		CString strTrainId=dlg->m_sStaionid;
		
		TgTRAIN_RECORD rec;
		rec.Clear();

		long mid_time=prec_next->arrive/2+prec->depart/2;
		rec.arrive_train_id = strTrainId;
		rec.depart_train_id = strTrainId;

		rec.rec_index			=	0;
		rec.station				=	prec->station;   //��վվ��
		rec.arrive				=	mid_time+20*60;
		rec.depart				=	rec.arrive;
		rec.plan_arrive			=	rec.arrive;
		rec.plan_depart			=	rec.depart;
		
		rec.entry				=	prec->exit;
		rec.exit				=	0;
		rec.in_station_index	=	prec->out_station_index;
		rec.out_station_index	=	prec->out_station_index;
		rec.arrive_side = 0;    
		rec.depart_side = 0;
		rec.adjust_status = 0;
		rec.flag = 0;
		SetFlagTerminal(rec.flag);
		SetFlagAbsStart(rec.flag);
		rec.special_train_index = newtrain.GetTrainIndex();
		
		if(prec->out_station_index > prec_next->in_station_index)
			SetAbsStartStopUp(rescue_train.myTrainInfo.adjust_flag); 
		else
			SetAbsStartStopDown(rescue_train.myTrainInfo.adjust_flag);
		SetAbsAttainEngineReturn(rescue_train.myTrainInfo.adjust_flag);
		rescue_train.AddRecord(rec, m_pDoc->GetTgType());
		rescue_train.myTrainInfo.stop_abs_time = 20;
		m_pDoc->SendAppendTrain(&rescue_train, "", 0);

		SetFlagAttainEngine(prec->flag);
		m_pDoc->SendUpdateTrain(newtrain);
	}
}

void CTGForm::TrytoModifyOtherTrain(CTG_TRAIN* pfocus_train)
{
	if(pfocus_train==NULL)
		return;

	for (int n=0; n<m_sFocusOtherTrain.size(); n++)
	{
		m_pDoc->TrytoMoveOtherTrain(pfocus_train, m_sFocusOtherTrain[n]);
	}
}

void CTGForm::SendUpdateOtherTrain()
{
	for(int i=0; i<m_sFocusOtherTrain.size(); i++)
	{
		m_pDoc->SendUpdateTrain(m_sFocusOtherTrain[i]);
	}
}

void CTGForm::OnContextMenuFormat(CWnd* pWnd, CPoint point)
{
	CMenu hResourceMenu;
	CRect r;

	do 
	{
		if(m_pDoc == NULL)
			break;

		GetWindowRect(&r);

		if(point.x == -1 && point.y == -1)
		{
			point.x = r.left / 3 + r.right * 2 / 3;
			point.y = r.top / 3 + r.bottom * 2 / 3;
		}

		if(! r.PtInRect(point))
			break;

		if(hResourceMenu.LoadMenu(IDR_RBUTTON_POPMENU) == 0)
			break;

		// TG menu
		if(m_sFocusTrain != NULL)
		{
			//ȡ���ֹ����������ߵ��Ҽ��˵�
			if(m_pDoc->m_nOperatorStatus == INSERT_TRAIN) 
			{  		           
				CPoint pt = CPoint(point);
				ScreenToClient(&pt);

				CClientDC clientDC(this);
				OnPrepareDC(&clientDC);
				clientDC.DPtoLP(&pt);
				if(m_sFocusTrain != NULL)
				{
					if(m_sFocusTrain->myRecord.size()>0)
					{ 	    
						hPopupMenu =  hResourceMenu.GetSubMenu(0);
						if(hPopupMenu == NULL)	
							break;

						hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

						if(m_stop_train_abs)
						{
							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_ABS_STOP, "�����յ�");
						}
						else
						{
							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_STOP, "�յ�");

							// ����������
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_DELIVER, "����");
						}

						// ȡ������
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ADDTRAIN_CANCEL, "ȡ������");

						if(m_sFocusTrain->GetRecordNum() >= 2)
						{
							hPopupMenu->AppendMenu(MF_SEPARATOR);
						
							if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							{
								CMenu pTmpMenu;
								VERIFY(pTmpMenu.CreatePopupMenu());
								hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ʩ����");

								// ������ҵ��ǰ�У�
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN, "������ҵ��ǰ�У�");
								// ������ҵ���۷���
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+1, "������ҵ���۷���");
								// ������ҵ�������䣩
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+2, "������ҵ�������䣩");
								// ������ҵ�������俪�أ�
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ABS_TRAIN_BEGIN+3, "������ҵ�������俪�أ�");
							}
						}

					}
				}
			}
			else
			{
				TgTRAIN_RECORD *rec = GetFoucsTrainRecord();
				if (rec == NULL) 
					return;
				
				bool bNotThisSectionStation=this->m_pctrl->mConfigTg.IsNotThisSectionStation(rec->station);
				const TgTRAIN_MORE_INFO& train_info = m_sFocusTrain->myTrainInfo;
				hPopupMenu =  hResourceMenu.GetSubMenu(0);
				if(hPopupMenu == NULL)	
					break;
				// ɾ��ռλ�˵�
				hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

				// ����ͼ����				
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
				{
					TgTRAIN_RECORD *rec;
					int nFocusIndex = m_nFocusRecordIndex;
					rec = m_sFocusTrain->GetRecordByRecIndex(nFocusIndex);
					if(rec == NULL)
						return;

					if(m_pDoc->m_ptgctrl->IsBPlanStation(rec->station))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ͼ����");

						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_KAKONG_BASIC_SIDE, "ȡ������ͼ�ɵ�����");
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_KAKONG_BASIC_ENTRY, "ȡ������ͼ����ڿ���");

						if(TG_IsNotCheckTrainSide(*rec))
							pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);

						if(TG_IsNotCheckTrainExitEntry(*rec))
							pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
				}

				// �����Դ���־
				if(TERM_TG_TYPE::WORK_TG==m_pctrl->GetTgTermType())
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_ROUTE_AUTO_MARK, "�����Դ���־");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_ROUTE_AUTO_MARK, "�����Դ���־");
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}
				// �������
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�������");

					if(this->m_pctrl->mConfigTg.m_context.IsEnableCZH())
					{	
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_CZH_TRAIN, "�г�����Ԥ��");

						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						{
							// ���˳�����༭
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_CZH, "����༭");
							pTmpMenu.AppendMenu(MF_SEPARATOR);
							// �������������۷�����
							if(m_pctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())					
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_GENTRAINID_BY_CZH, "�����۷�����");
							else
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED, ID_GENTRAINID_BY_CZH, "�����۷�����");
						}
						else
						{
							// ���˳�����༭
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "����༭");
							pTmpMenu.AppendMenu(MF_SEPARATOR);
							// �������������۷�����
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED, ID_GENTRAINID_BY_CZH, "�����۷�����");
						}
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						// �����г�
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "�����г�");
						// �������
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_JIECHU_TRAIN_CZH, "�������");
					}
					else if(this->m_pctrl->mConfigTg.m_context.IsEnableNewCZH())
					{
						// �г�����Ԥ��
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_CZH_TRAIN, "�г�����Ԥ��");
						
						// ���˳�����༭
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "����༭");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						// �������������۷�����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED, ID_GENTRAINID_BY_CZH, "�����۷�����");

						pTmpMenu.AppendMenu(MF_SEPARATOR);
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						{
							// �����г�
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_CZH, "�����г�");
							// �������
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_JIECHU_TRAIN_CZH, "�������");
						}
						else
						{
							// �����г�
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "�����г�");
							// �������
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_JIECHU_TRAIN_CZH, "�������");
						}
					}
					else
					{
						// �г�����Ԥ��
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_SHOW_CZH_TRAIN, "�г�����Ԥ��");
						
						// ���˳�����༭
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "����༭");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						// �������������۷�����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED, ID_GENTRAINID_BY_CZH, "�����۷�����");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						// �����г�
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MODIFY_TRAIN_CZH, "�����г�");
						// �������
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_JIECHU_TRAIN_CZH, "�������");
					}
				}

				// �޸ļƻ��߲���
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_PLAN_PARA, "�ƻ��߲���");
				hPopupMenu->AppendMenu(MF_SEPARATOR);

				// ����/ʼ��
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{	
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����/ʼ��");

						// ��վ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_FETCH, "����");
						// ��վʼ��
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_START, "ʼ��");
					}
					else
					{
						hPopupMenu->AppendMenu(MF_POPUP | MF_GRAYED, (UINT) pTmpMenu.m_hMenu, "����/ʼ��");
					}
				}

				// ����/�յ�
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					CMenu pTmpMenu1;
					VERIFY(pTmpMenu1.CreatePopupMenu());
					// ID_STOP_DWAW
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu1.m_hMenu, "����/�յ�");

						// ��̽���
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DEPART, "����");
						// ���ͣ��
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DWAW, "�յ�");
						// ��ʱͣ��
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_TEMP, "��ʱͣ��");

						// �м�ָ�
						pTmpMenu1.AppendMenu(MF_SEPARATOR);
						pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_MIDDLE_DIV_TRAIN, "�м�ָ�");

						// ��������
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_CONTINUE_DRAW, "��������");
					}
					else
					{
						hPopupMenu->AppendMenu(MF_POPUP | MF_GRAYED, (UINT) pTmpMenu1.m_hMenu, "����/�յ�");
						// ��������
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_CONTINUE_DRAW, "��������");
					}
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}
				
				// ͨ��<=>����
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					bool bEnable=false;
					if(this->m_pctrl->mConfigTg.m_context.IsCanPASSToStayCnvt())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !bNotThisSectionStation)
						{
							bEnable = true;
						}
					}
					if(bEnable)
					{
						// ͨ���䵽��
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_PASS_TO_STAY, "ͨ���䵽��");
						// ������ͨ��
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_STAY_TO_PASS, "������ͨ��");
					}
					else
					{
						// ͨ���䵽��
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_PASS_TO_STAY, "ͨ���䵽��");
						// ������ͨ��
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_STAY_TO_PASS, "������ͨ��");
					}
				}
				
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}
				//�������
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�������");
					
					// ������Ϣ
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAINSPORT_INFO, "������Ϣ");
					
					if(m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
					{
						// ��Ǯ��
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_FENJIEKOU_TIME, "��Ǯ��");
						else
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_FENJIEKOU_TIME, "��Ǯ��");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
												
						// ������Ϣ��ѯ
						if(m_pDoc->m_bShowEngineCrewMenu)
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_ENGINE_INFO, "������Ϣ��ѯ");

						// ˦�ҹ���
						if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableShuaiguaManage())		
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHUAIGUA_MANAGE, "˦�ҹ���");
					}
					
					// ��ѯȷ��
					if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
					{
						// ����ͼ��Ϣ
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_BASIC_INFO, "����ͼ��Ϣ");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_QUERT_QUEBAO, "��ѯȷ��");
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_WORK_INFO, "��ҵ��Ϣ");
						pTmpMenu.AppendMenu(MF_SEPARATOR);

						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_SUBAO_INFO, "�г��ٱ�");
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_BIANZU_INFO, "�г�����");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
											
						// �����ͳ��
						{
							CMenu pTmpMenuChild;
							VERIFY(pTmpMenuChild.CreatePopupMenu());

							pTmpMenu.AppendMenu(MF_POPUP, (UINT) pTmpMenuChild.m_hMenu, "�����ͳ��");

							// ͳ�Ƶ����
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_ARRIVAL, "ͳ�Ƶ����");
							// ͳ�Ƴ�����
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_DEPART, "ͳ�Ƴ�����");
							// ͳ�Ƶ���/������
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_BOTH, "ͳ�Ƶ�����");
							// ��ͳ�Ƶ���/������
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_NO_STATISTIC, "��ͳ�Ƶ�����");

							pTmpMenuChild.AppendMenu(MF_SEPARATOR);
							// ����ͳ��
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STATISTIC_WHOLE, "����ͳ��");	

							if(m_sFocusTrain->myTrainInfo.whole_static>0)
								pTmpMenuChild.CheckMenuItem(5, MF_CHECKED|MF_BYPOSITION);


							if((rec->static_arrive > 0) && (rec->static_depart > 0))
								pTmpMenuChild.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
							else
							{
								if(rec->static_arrive>0)
									pTmpMenuChild.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
								if(rec->static_depart>0)
									pTmpMenuChild.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
								if(rec->static_arrive == 0 && rec->static_depart == 0)
									pTmpMenuChild.CheckMenuItem(3, MF_CHECKED|MF_BYPOSITION);
							}
						}
					}
				}

				//�������
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�г�����");
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableInterCityStplan()) // �����ǳǼ�,�����۷��˵�
					{
						// �յ��۷�
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						{
							CMenu pTmpMenuChild;
							VERIFY(pTmpMenuChild.CreatePopupMenu());
							pTmpMenu.AppendMenu(MF_POPUP, (UINT) pTmpMenuChild.m_hMenu, "�۷�����");
							pTmpMenu.AppendMenu(MF_SEPARATOR);
							// ԭ���۷�
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_IN_STATION_RETURN, "�յ�-ԭ���۷�");
							// վ���۷�
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_OUT_STATION_RETURN, "�յ�-վ���۷�");
							// ���۷�
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_NOT_STATION_RETURN, "�յ�-���۷�");
							// ���۷�
							pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_NOT_START_RETURN, "ʼ��-���۷�");
				
							if(IsNewReturn(train_info.side_utility))
							{
								if(IsStationOutReturn(train_info.side_utility))
								{
									pTmpMenuChild.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
								}
								else
								{
									pTmpMenuChild.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
								}
							}
							else
							{
								pTmpMenuChild.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
							}
						}
					}
					// �����·
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
					{
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_SINGLE, "�����·");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
					}
					// ���ý�·�Դ�
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->GetTgType() == TG_TYPE::WORK_TG)
					{
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DIRECT_SET_TRIG, "���ý�·�Դ�");
						pTmpMenu.AppendMenu(MF_SEPARATOR);
					}
					// �г�����
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_CANNOT_MEET, "�г�����");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CANNOT_MEET, "�г�����");
					pTmpMenu.AppendMenu(MF_SEPARATOR);
					// ����/����ͣ��
					{
						CMenu pTmpMenuChild;
						VERIFY(pTmpMenuChild.CreatePopupMenu());
						pTmpMenu.AppendMenu(MF_POPUP, (UINT) pTmpMenuChild.m_hMenu, "����/����ͣ��");

						int nFocusIndex = m_nFocusRecordIndex;
						TgTRAIN_RECORD *sel_next_rec = m_sFocusTrain->GetRecordByRecIndex(nFocusIndex+1);

						if(sel_next_rec==NULL)
						{
							// ����ͣ��
							if(IsFlagTrainWholeHelp(rec->flag))
								pTmpMenuChild.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_DWAW, "ȡ������ͣ��");
							else
								pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED, ID_STOP_DWAW, "ȡ������ͣ��");
						}
						else
						{
							bool bModify=true;
							if(IsFlagTrainPartRun(rec->flag))
							{
								bModify=false;
							}
							if(bModify&&m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							{
								// ����ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_ABS, "����ͣ��");
								// ȡ������ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_ABS_CANCEL, "ȡ������ͣ��");
								pTmpMenuChild.AppendMenu(MF_SEPARATOR);
								// ����ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_SIGNAL, "����ͣ��");	
								// ȡ������ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_SIGNAL_CANCEL, "ȡ������ͣ��");
							}
							else
							{
								// ����ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_ABS, "����ͣ��");
								// ȡ������ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_ABS_CANCEL, "ȡ������ͣ��");

								pTmpMenuChild.AppendMenu(MF_SEPARATOR);
								// ����ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_SIGNAL, "����ͣ��");	
								// ȡ������ͣ��
								pTmpMenuChild.AppendMenu(MF_STRING | MF_GRAYED,	ID_STOP_SIGNAL_CANCEL, "ȡ������ͣ��");
							}
							// ����ͣ��λ��					
							if(rec == NULL)
								return;

							if(bModify&&IsFlagStopAbs(rec->flag))
							{
								CMenu pTmpChildMenu;
								VERIFY(pTmpChildMenu.CreatePopupMenu());

								pTmpMenuChild.AppendMenu(MF_POPUP, (UINT) pTmpChildMenu.m_hMenu, "����ͣ��λ��");
								// ǰ��
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_UP, "ǰ��");	
								// �м�
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_MIDDILE, "�м�");
								// ��
								pTmpChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_STOP_DRAW_BOTTOM, "��");

								if(IsAbsDrawUp(rec->stop_reason))
									pTmpChildMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
								else if(IsAbsDrawBottom(rec->stop_reason))
									pTmpChildMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
								else 
									pTmpChildMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
							}
						}
						pTmpMenu.AppendMenu(MF_SEPARATOR);
					}
					// �ĳ���
					bool bEnable=false;
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanRenameTrain())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
						{
							bEnable=true;
						}
					}
					if(bEnable)
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,  ID_CHANGE_TRAINID, "�ĳ���");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,  ID_CHANGE_TRAINID, "�ĳ���");

					// �Ĵ�ñ����
					bEnable=false;
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanHatTrain())	
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
						{
							bEnable=true;
						}
					}
					if(bEnable)
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_REPLACE_HATTRAINID, "�Ĵ�ñ����");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_REPLACE_HATTRAINID, "�Ĵ�ñ����");

					// ���۷�����
					bEnable=false;
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyZFTrainId())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
						{
							bEnable=true;
						}
					}
					if(bEnable)
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_REPLACE_TRAINID, "���۷�����");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_REPLACE_TRAINID, "���۷�����");

					pTmpMenu.AppendMenu(MF_SEPARATOR);

					// �Զ�����
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableAutoAdjust())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && (TG_TYPE::BASIC_TG!=m_pctrl->GetTGType()))
						{
							CMenu pChildMenu;
							VERIFY(pChildMenu.CreatePopupMenu());
							pTmpMenu.AppendMenu(MF_POPUP, (UINT) pChildMenu.m_hMenu, "�Զ�����");
							
							// ��ͼ����
							pChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN, "��ͼ����");
							// ���õ���
							pChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN+1, "���õ���");
							// �����õ���
							pChildMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ADJUST_BEGIN+2, "��㲻���õ���");

							pTmpMenu.AppendMenu(MF_SEPARATOR);
						}
					}

					// ɾ���г�
					bEnable=false;
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanDeleteTrain())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))//������ɾ���հ�ƻ��г�
						{
							bEnable=true;
						}
					}
					if(bEnable)
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DELETE_TRAIN, "ɾ���г�");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DELETE_TRAIN, "ɾ���г�");

					// �����г�
					bEnable=false;
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanCopyTrain())
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && m_pDoc->IsCanModifyTrainStation(train_info))
						{
							bEnable=true;
						}
					}
					if(bEnable)
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_COPY, "�����г�");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_TRAIN_COPY, "�����г�");
				}
				hPopupMenu->AppendMenu(MF_SEPARATOR);

				// �Ĺɵ�
				if(TERM_TG_TYPE::SHIFT_TG!=m_pctrl->GetTgTermType())
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�Ĺɵ�");

					int i;
					for(i=0;i < m_pDoc->m_nDispDistanceNum;i++)
					{
						if(m_pDoc->m_sDispDistance[i].station_number  == rec->station)
							break;
					}  	
					if(i >= m_pDoc->m_nDispDistanceNum)
						return ;

					CString str;CString sStationName;

					//1 ��ʾ���κ�
					str = m_pDoc->GetTrainShowId(m_sFocusTrain);
					sStationName.Format("%s:", m_pDoc->m_sDispDistance[i].station_name);
					if(str.IsEmpty())
						str = "�޳���";
					else
						str = sStationName + str;
					str.TrimRight();
					pTmpMenu.AppendMenu(MF_STRING|MF_GRAYED,ID_CHANGE_TRACKS_BEGIN,str);

					//2 ���ӷָ���
					pTmpMenu.AppendMenu(MF_SEPARATOR);

					//3 ���ӹɵ��б�
					int index;
					for(int idx = 0; idx < m_pDoc->m_sDispDistance[i].number_of_side; idx++) 
					{
						int side_no = m_pDoc->m_sDispDistance[i].side[idx].side_number;	   
						str.Format("%s",m_pDoc->m_sDispDistance[i].side[idx].sidename);
						index = idx; 

						if(bNotThisSectionStation)
						{
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACKS_BEGIN+index, str);
						}
						else
						{
							if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_CHANGE_TRACKS_BEGIN+index, str);
							else
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACKS_BEGIN+index, str);
						}
						//��ǰ�ɵ���ѡ��֮
						if(rec == NULL)
							return;
						if(rec->arrive_side == side_no)
							pTmpMenu.CheckMenuItem(index+2, MF_CHECKED|MF_BYPOSITION);	

						BOOL bIsNoCrhSide = FALSE;
						//2007.04.18������ ������ɵ�����
						if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCheckCrhSide())
						{
							if(m_sFocusTrain->IsCHRTrain())
							{
								bIsNoCrhSide = m_pDoc->m_ptgctrl->mConfigTg.m_lineCfg.IsCRHSide(rec->station, side_no)?FALSE:TRUE;
							}
						}
						if(bIsNoCrhSide)
							pTmpMenu.EnableMenuItem(index+2, MF_BYPOSITION|MF_GRAYED);
					}

					// �ĵ����ɵ�
					pTmpMenu.AppendMenu(MF_SEPARATOR);
					if(bNotThisSectionStation)
					{
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACK, "�ĵ����ɵ�");
					}
					else
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_CHANGE_TRACK, "�ĵ����ɵ�");
						else
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_CHANGE_TRACK, "�ĵ����ɵ�");
					}

					pTmpMenu.AppendMenu(MF_SEPARATOR);
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_BATCH_MODIFY_TRACK, "�����޸Ĺɵ�");
					else
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_BATCH_MODIFY_TRACK, "�����޸Ĺɵ�");

					// �����ɵ�
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
					{
						CMenu pTmpMenu;
						VERIFY(pTmpMenu.CreatePopupMenu());
						hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "�����ɵ�");

						if(bNotThisSectionStation)
						{
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_LOCK_STATION_SIDE, "������վ�ɵ�");
							pTmpMenu.AppendMenu(MF_SEPARATOR);
							pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_LOCK_TRAIN_SIDE, "���������ɵ�");
						}
						else
						{
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_LOCK_STATION_SIDE, "������վ�ɵ�");
							pTmpMenu.AppendMenu(MF_SEPARATOR);
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_LOCK_TRAIN_SIDE, "���������ɵ�");
						}

						if(rec)
						{
							if(IsForceSide(*rec))
							{
								pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
							}
						}

						if(m_sFocusTrain->IsWholeTrainSideLock())
						{
							pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
						}
					}
					hPopupMenu->AppendMenu(MF_SEPARATOR);
				}

				// �г���ϸ��Ϣ
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_DATA, "�г���ϸ��Ϣ");
				// �г�����
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TRAIN_ATTRIBUTE, "�г�����");
				hPopupMenu->AppendMenu(MF_SEPARATOR);

				// ǣ��/���һ���
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_GUA_JICHE, "ǣ��/���һ���");
				// ������λ��
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "������λ��");

					// �Ϸ�
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_TOP, "�Ϸ�");
					// �м�
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_MIDDLE, "�м�");
					// �·�
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_BOTTOM, "�·�");
					// ����ʾ
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOWTRACTION_NO, "����ʾ");

					if(m_sFocusTrain == NULL) 
						return;
					//�ж��Ƿ���Ҫ��ʾ������ ��ȡ�õ�������ʱ��Ļ���λ��

					if(IsDisplyTractionTop(m_sFocusTrain->myTrainInfo.adjust_flag))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}
					else if(IsDisplyTractionMiddle(m_sFocusTrain->myTrainInfo.adjust_flag))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
					else if(IsDisplyTractionBottom(m_sFocusTrain->myTrainInfo.adjust_flag))
					{
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					}
					else
					{
						pTmpMenu.CheckMenuItem(3, MF_CHECKED|MF_BYPOSITION);
					}
				}
				// ����ʱ��
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ʱ��");
					if((m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT)||m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT)) && !bNotThisSectionStation)
					{
						// ���ռƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DEPART_BY_PLAN, "���ռƻ�");
						// ���ձ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DEPART_BY_MEET, "���ձ���");
					}
					else
					{
						// ���ռƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_PLAN, "���ռƻ�");
						// ���ձ���
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DEPART_BY_MEET, "���ձ���");
					}
					if(rec == NULL)
						return;

					if(IsMakeDepartRouteByPlan(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsMakeDepartRouteByMeet(rec->stop_condition))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
				}

				// ��ͨ��־
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				{
					if(IsMustNoPass(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_INHBIT_PASS, "��ͨ��־");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_INHBIT_PASS, "��ͨ��־");
				}
				else
				{
					if(IsMustNoPass(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_INHBIT_PASS, "��ͨ��־");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_INHBIT_PASS, "��ͨ��־");
				}
				// Ӫҵͣ��
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
				{
					if( IsTechStopTrain(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_TECH_STOP, "Ӫҵͣ��");
					else 
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_TECH_STOP, "Ӫҵͣ��");
				}
				else
				{
					if( IsTechStopTrain(*rec))
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_TECH_STOP, "Ӫҵͣ��");
					else 
						hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_TECH_STOP, "Ӫҵͣ��");
				}

				// �����г�
				if(TG_TYPE::BASIC_TG!=m_pctrl->GetTGType())
				{
					if(IsTrainSecret(m_sFocusTrain->myTrainInfo))
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SET_SECRET_TRAIN, "�����г�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_SET_SECRET_TRAIN, "�����г�");
					}
					else
					{
						if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SET_SECRET_TRAIN, "�����г�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_SET_SECRET_TRAIN, "�����г�");
					}
				}
				
				// ϵͳ����
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "ϵͳ����");

					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsSetNoLateEarlyTime())
					{
						std::map<ULONG, ULONG>::iterator it = m_pDoc->m_noearlylatetime.find(m_sFocusTrain->m_nTrainIndex);
						if(it != m_pDoc->m_noearlylatetime.end())
						{
							if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SET_NO_LATE_TIME, "ǿ������");
							else
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED | MF_CHECKED,	ID_SET_NO_LATE_TIME, "ǿ������");
						}
						else
						{
							if(m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))
								pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SET_NO_LATE_TIME, "ǿ������");
							else
								pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_SET_NO_LATE_TIME, "ǿ������");
						}
						pTmpMenu.AppendMenu(MF_SEPARATOR);
					}

					// �г�����XML
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SAVE_TRAIN_XML, "�����г�");

					// �˳�����״̬
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_EXIT_DRAW_TRAIN_STATE, "�˳�����״̬");

					// �޸�վ��
					if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyStationIdx())
					{
						pTmpMenu.AppendMenu(MF_SEPARATOR);
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MODIFY_TRAIN_INDEX, "�޸�վ��");
					}
				}
			}
			m_nFocusFigure = NULL;
		}
		// MARK menu
		else if(m_nFocusFigure != NULL)
		{
			m_sFocusTrain = NULL;
			hPopupMenu =  hResourceMenu.GetSubMenu(0);
			if(hPopupMenu == NULL)	
				break;
			// ɾ��ռλ�˵�
			hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

			bool iscopy=false;
			// ���Ʊ��
			if(m_nFocusFigure->type == SIDE_INHIBIT_MARK 
						||m_nFocusFigure->type == SIDE_TIANCHUANG_MARK 
						||m_nFocusFigure->type == SIDE_SLOW_MARK
						||m_nFocusFigure->type == SIDE_OCCUPY_MARK    //�ɵ�����,�ɵ��촰��,վ�ڻ���,�ɵ�ռ��
						||m_nFocusFigure->type == ABS_ELE_FIX_MARK 
						|| m_nFocusFigure->type == ABS_FIX_MARK 
						|| m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK
						|| m_nFocusFigure->type == SELF_CLOSE_MARK    //�����������
						|| m_nFocusFigure->type == ABS_INHIBIT_MARK 
						|| m_nFocusFigure->type == ABS_SLOW_MARK
						|| m_nFocusFigure->type == NEW_GRAPH_NOTE_MARK)
			{          
				iscopy=true;
			}

			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanCopyMark() && iscopy)
			{
				CMenu pTmpMenu;
				VERIFY(pTmpMenu.CreatePopupMenu());

				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "���Ʊ��");

				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					// ����
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY, "����");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY_V_UP, "����");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COPY_V_DOWN, "����");

						// �ӳ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_CONTINUE_LINING, "�ӳ�");
					}
				}
				else
				{
					// ����
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY, "����");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY_V_UP, "����");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_COPY_V_DOWN, "����");

						// �ӳ�
						pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_CONTINUE_LINING, "�ӳ�");
					}
				}

				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsAutoAddBlockEndTime(m_nFocusFigure->type))
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						// �Զ�
						if(m_pDoc->IsMarkMove(m_nFocusFigure->mark_index))
							hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_MARK_MOVE_START, "��ʼ�Զ��ӳ�");
						else
							hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_MOVE_START, "��ʼ�Զ��ӳ�");
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_MOVE_STOP, "ֹͣ�Զ��ӳ�");
					}
				}
			}
			
			// ����Ҫ������ʩ������״̬
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus())
			{
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "ʩ��״̬");
					if(m_nFocusFigure->type != NEW_GRAPH_NOTE_MARK)
					{
						// �ƻ�
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_PLAN, "ʩ���ƻ�");
						// ����
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_ACTIVE, "ʩ������");
						// ���
						pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_COMPLETE, "ʩ�����");
					}

					if(IsBlockPlan(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsBlockActive(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}

					if(IsBlockComplete(m_nFocusFigure->adjust_status))
					{
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					}
				}
			}

			// ɾ��
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanDeleteMark())
			{
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DELETE_ABS, "ɾ��");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_DELETE_ABS, "ɾ��");
			}

			// ���ɵ�������
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsCanGenMarkDCmd())
			{
				LoadAutoGenDCmdXml();
				bool bFind = false;
				for(int idx=0;idx<m_menu.size();idx++)
				{
					if(m_nFocusFigure->type == m_menu[idx].type)
					{
						bFind = true;
						break;
					}
				}

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(bFind)
				{
					m_map_menuid.clear();
					m_map_menuparentid.clear();
					
				
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());
					
					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "���ɵ�������");

					int menu_id = 0;
					for(int idx=0;idx<m_menu.size();idx++)
					{
						if(m_nFocusFigure->type == m_menu[idx].type)
						{
							m_map_menuid[ID_GEN_DCMD+menu_id] = m_menu[idx].id;
							m_map_menuparentid[ID_GEN_DCMD+menu_id] = m_menu[idx].parent_id;
							pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_GEN_DCMD+menu_id, m_menu[idx].text);
							menu_id++;
						}
					}
				}
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,ID_GEN_DCMD, "���ɵ�������");
			}
			
			
			// ���õ���������
			{
				if(m_nFocusFigure->type == ABS_INHIBIT_MARK ||
					m_nFocusFigure->type == ABS_FIX_MARK ||
					m_nFocusFigure->type == NEW_ABS_ELE_FIX_MARK ||
					m_nFocusFigure->type == ABS_ELE_FIX_MARK ||

					m_nFocusFigure->type == SIDE_INHIBIT_MARK ||
					m_nFocusFigure->type == SIDE_OCCUPY_MARK ||
					m_nFocusFigure->type == SIDE_TIANCHUANG_MARK)
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_BLOCK_ADJUST_TRAINS, "���õ���������");
						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}
			}
			
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsDirectModifyBlockTime())
			{
				// ̫ԭ �޸�ʩ��������ֹʱ��
				if(m_nFocusFigure->type ==ABS_INHIBIT_MARK ||m_nFocusFigure->type ==ABS_SLOW_MARK ||
					m_nFocusFigure->type == SIDE_INHIBIT_MARK ||m_nFocusFigure->type == SIDE_SLOW_MARK)
				{
					if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
					{
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_TIMESPAN, "�޸���ֹʱ��");
						hPopupMenu->AppendMenu(MF_SEPARATOR);
					}
				}
			}

			// ����
			hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_PROPERTY, "����");

			// TDMS���
			hPopupMenu->AppendMenu(MF_SEPARATOR);
			{
				CMenu pTmpMenu;
				VERIFY(pTmpMenu.CreatePopupMenu());
				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "��������");
				// ������������
				pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_FIND_DDML, "������������");
				pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_WORK_INFO, "��ҵ��Ϣ");
				
				if(m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
				{
					pTmpMenu.AppendMenu(MF_SEPARATOR);
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_MARK_CANCELWORK, "ȡ��ʩ��");
				}
			}
		}
		// other menu
		else
		{
			m_nFocusFigure = NULL;
			m_sFocusTrain = NULL;

			hPopupMenu =  hResourceMenu.GetSubMenu(0);
			if(hPopupMenu == NULL)	
				break;
			// ɾ��ռλ�˵�
			hPopupMenu->DeleteMenu(0,MF_BYPOSITION);
			
			if(m_pDoc->m_nOperatorStatus == NORMAL_STATUS)
			{
				// ��ʾ����
				// ���˳��˵�
				if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG || m_pDoc->GetTgTermType() == TERM_TG_TYPE::SHIFT_TG)
				{
					if(m_pDoc->m_bShowSingleTrain)
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED | MF_CHECKED,	ID_SHOW_SINGLE_TRAIN, "��ʾ���˳�");
					else
						hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_SINGLE_TRAIN, "��ʾ���˳�");
				}
				if(m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
				{
					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "��ʾɸѡ");

					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ONLY_SHOW_MARK, "ֻ��ʾʩ������");	
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_ONLY_SHOW_TRAIN, "ֻ��ʾ������");	
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_SHOW_MARK_AND_TRAIN, "��ʾʩ�����ź�������");

					if (m_pDoc->IsOnlyShowMark()) 
					{
						pTmpMenu.CheckMenuItem(0, MF_CHECKED|MF_BYPOSITION);
					}
					else if (m_pDoc->IsOnlyShowTrain()) 
					{
						pTmpMenu.CheckMenuItem(1, MF_CHECKED|MF_BYPOSITION);
					}
					else if (m_pDoc->IsShowTrainAndMark()) 
					{
						pTmpMenu.CheckMenuItem(2, MF_CHECKED|MF_BYPOSITION);
					}
				}

				hPopupMenu->AppendMenu(MF_SEPARATOR);
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DEFINE_COLOR, "�Զ����ͼ��ɫ");	

				hPopupMenu->AppendMenu(MF_SEPARATOR);

				CMenu pTmpMenu;
				VERIFY(pTmpMenu.CreatePopupMenu());
				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "������");
				int direct=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetShowTrainDirect();
				// ����
				if(direct==1)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_UP_TRAIN, "����");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_UP_TRAIN, "����");
				// ����
				if(direct==2)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DOWN_TRAIN, "����");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DOWN_TRAIN, "����");
				// ������
				if(direct==3)
					pTmpMenu.AppendMenu(MF_STRING | MF_GRAYED,	ID_DUAL_TRAIN, "������");
				else
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_DUAL_TRAIN, "������");

				// �ͻ�
				hPopupMenu->AppendMenu(MF_SEPARATOR);

				CMenu pTmpMenu1;
				VERIFY(pTmpMenu1.CreatePopupMenu());

				hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu1.m_hMenu, "�ͻ�");
				int type=m_pDoc->m_ptgctrl->mConfigTg.m_context.GetShowTrainType();
				// �ͳ�
				if(type==1)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_PASEEGE_TRAIN, "�ͳ�");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_PASEEGE_TRAIN, "�ͳ�");
				// ����
				if(type==2)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_GOODS_TRAIN, "����");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_GOODS_TRAIN, "����");
				// �ͻ���
				if(type==3)
					pTmpMenu1.AppendMenu(MF_STRING | MF_GRAYED,	ID_PESSGOODS_TRAIN, "�ͻ���");
				else
					pTmpMenu1.AppendMenu(MF_STRING | MF_ENABLED,	ID_PESSGOODS_TRAIN, "�ͻ���");


				// ����ͼ��ʼʱ��
				if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
				{
					hPopupMenu->AppendMenu(MF_SEPARATOR);

					CMenu pTmpMenu;
					VERIFY(pTmpMenu.CreatePopupMenu());

					hPopupMenu->AppendMenu(MF_POPUP, (UINT) pTmpMenu.m_hMenu, "����ͼ��ʼʱ��");

					// 18��
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_BASIC_BEGIN_TIME_18, "18��");
					// 12��
					pTmpMenu.AppendMenu(MF_STRING | MF_ENABLED,	ID_BASIC_BEGIN_TIME_12, "12��");
				}

				// ѡ�ж����г�
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.IsCanMutiMoveOrDel())
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SEL_MULTI_TRAIN, "�ƶ������г�");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_SEL_MULTI_TRAIN, "�ƶ������г�");
					
				// ɾ�������г�
				hPopupMenu->AppendMenu(MF_SEPARATOR);
				if(m_pDoc->m_ptgctrl->mConfigTg.IsCanMutiMoveOrDel())
					hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_DEL_MULTI_TRAIN, "ɾ�������г�");
				else
					hPopupMenu->AppendMenu(MF_STRING | MF_GRAYED,	ID_DEL_MULTI_TRAIN, "ɾ�������г�");
			}
			else
			{
				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_EXIT_DRAW_TRAIN_STATE, "״̬��λ");
			}
		}


		if(hPopupMenu == NULL) 
			break;
		hPopupMenu->TrackPopupMenu(TPM_LEFTALIGN |/* TPM_NONOTIFY |*/ TPM_RIGHTBUTTON,
			point.x,point.y,this,NULL);

		hPopupMenu = NULL;
		hResourceMenu.DestroyMenu( );
	}while(0);
}

void CTGForm::ReLayoutBmpPageSize(CClientDC& dc)
{
	if(m_pBitmapLayer1!=NULL)
	{
		m_Layer1.SelectObject(m_pBitmapLayer1);
		m_Layer1.DeleteDC();
		m_bitmap1.DeleteObject();
		m_pBitmapLayer1=NULL;
	}

	if(m_pBitmapLayer2!=NULL)
	{
		m_Layer2.SelectObject(m_pBitmapLayer2);
		m_Layer2.DeleteDC();
		m_bitmap2.DeleteObject();
		m_pBitmapLayer2=NULL;
	}
	
	if(m_pBitmapLayer3!=NULL)
	{
		m_Layer3.SelectObject(m_pBitmapLayer3);
		m_Layer3.DeleteDC();
		m_bitmap3.DeleteObject();
		m_pBitmapLayer3=NULL;
	}

	if(m_pBitmapLayer4!=NULL)
	{
		m_Layer4.SelectObject(m_pBitmapLayer4);
		m_Layer4.DeleteDC();
		m_bitmap4.DeleteObject();
		m_pBitmapLayer4=NULL;
	}

	BITMAPINFOHEADER bmih;
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	DEVMODE md;
	md.dmSize = sizeof(md);
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &md))
		bmih.biBitCount = md.dmBitsPerPel;
	else
		bmih.biBitCount =16;
	bmih.biCompression=BI_RGB;
	bmih.biPlanes=1;
	bmih.biWidth=m_pDoc->m_nPageWidth;
	bmih.biHeight=m_pDoc->m_nPageHeight;
	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader = bmih;
	void *p;
	HBITMAP bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
	if(!bitMap)
	{
		m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
		AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
	}
	m_bitmap1.Attach(bitMap);
	if(m_Layer1.CreateCompatibleDC(&dc) == 0)
	{
		m_bitmap1.DeleteObject();
		AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
		m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
	}
	m_pBitmapLayer1 = m_Layer1.SelectObject(&m_bitmap1);
	m_Layer1.SetBkColor(CLR_WHITE);

	if(m_pctrl->IsWorkMode())
	{
		bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
		if(!bitMap)
		{
			m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
			AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
		}
		m_bitmap2.Attach(bitMap);
		if(m_Layer2.CreateCompatibleDC(&dc) == 0)
		{
			m_bitmap2.DeleteObject();
			AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
			m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
		}

		bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
		if(!bitMap)
		{
			m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
			AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
		}
		m_bitmap3.Attach(bitMap);
		if(m_Layer3.CreateCompatibleDC(&dc) == 0)
		{
			m_bitmap3.DeleteObject();
			AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
			m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
		}

		bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
		if(!bitMap)
		{
			m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
			AfxMessageBox("ϵͳ��Դ���㣬��������������½��");
		}
		m_bitmap4.Attach(bitMap);
		if(m_Layer4.CreateCompatibleDC(&dc) == 0)
		{
			m_bitmap4.DeleteObject();
			AfxMessageBox("���������ڴ�dcʧ�ܣ���������������½��");
			m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
		}

		m_pBitmapLayer2 = m_Layer2.SelectObject(&m_bitmap2);
		m_Layer2.SetBkColor(CLR_WHITE);
		m_pBitmapLayer3 = m_Layer3.SelectObject(&m_bitmap3);
		m_Layer3.SetBkColor(CLR_WHITE);
		m_pBitmapLayer4 = m_Layer4.SelectObject(&m_bitmap4);
		m_Layer4.SetBkColor(CLR_WHITE);

		m_pDoc->SetLayer(&m_Layer4, &m_Layer2, &m_Layer3);
		m_pDoc->sysprint(0,0,"[TG]: using work mode");
	}
	else
	{
		m_pDoc->sysprint(0,0,"[TG]: not using work mode");
		m_pDoc->SetLayer(&m_Layer1, NULL, NULL);
	}
}

CString CTGForm::CheckTrainSideByRelation(CTG_TRAIN& train)
{
	CString xml="<root>";
	for(int rec_index=0; rec_index<train.GetRecordNum(); rec_index++)
	{
		TgTRAIN_RECORD *precord=train.GetRecordByRecIndex(rec_index);
		if(NULL==precord)
			continue;
	
		int  station=precord->station;
		if(m_pDoc->GetStationProperty(station)!=CTC_STATION)
			continue;
		
		CStringArray Textary;
		m_pDoc->CheckSideRelation(&train, rec_index, Textary);
		if(Textary.GetCount()>0)
		{
			CString str;
			str.Format("<record station=\"%d\" rec_index=\"%d\">", station, rec_index);
			xml += str;
			
			for(int i=0; i<Textary.GetCount(); i++)
			{
				str.Format("<alarm>%s</alarm>", GetValidStr(Textary.GetAt(i)));
				xml += str;
			}
			xml += "</record>";
		}
	}
	xml+="</root>";
	return xml;
}

// ����ֲ���Ԯʵ��
void CTGForm::CreatePartHelpRunStation(CTG_TRAIN& newtrain, CString strTrainId, CString strRetTrainId, bool bReverse)
{
	TgTRAIN_RECORD *prec_cur = newtrain.GetRecordByPosition (m_nFocusRecordIndex);
	TgTRAIN_RECORD *prec_nxt = newtrain.GetRecordByPosition (m_nFocusRecordIndex + 1);		
	if(prec_cur == NULL || prec_nxt == NULL)
	{
		return;
	}
	
	int nStation=prec_cur->station;
	int nNextStation = prec_nxt->station;
	int nStationExit = prec_cur->exit;
	int nNextStationEntry = prec_nxt->entry;
	int station_index = prec_cur->out_station_index;
	int next_station_index = prec_nxt->in_station_index;
	prec_cur->stop_abs_s_time = 3;
	prec_cur->stop_abs_e_time = 8;

	ReSetAbsDraw(prec_cur->stop_reason);
	int nRecNum = newtrain.GetRecordNum() ;
	for (int i = nRecNum -1 ; i > m_nFocusRecordIndex+1; i--)
	{
		newtrain.DeleteTailRecord();
	}

	TgTRAIN_RECORD *plast = newtrain.GetRecordByRecIndex(newtrain.GetRecordNum()-2);
	if(plast==NULL)
		return;
	if(plast->station!=prec_cur->station)
		return;
	SetFlagTrainPartRun(plast->flag);

	CancelFlagStopSignal(plast->flag);
	SetFlagStopAbs(plast->flag);
	if(station_index<next_station_index)
		SetFlagStopAbsDown(plast->flag);
	else
		SetFlagStopAbsUp(plast->flag);
	
	plast = newtrain.GetLastRecord();
	if(plast==NULL)
		return;
	ClearFlagTrainPartRun(plast->flag);
	SetFlagEnd(plast->flag);
	plast->arrive += 5*60;
	plast->depart = plast->arrive;
	plast->out_station_index = plast->in_station_index;
	
	int	midt = prec_cur->depart + prec_cur->stop_abs_e_time*60;

	m_pDoc->SendUpdateTrain(newtrain);

	CTG_TRAIN rescue_train;
	TgTRAIN_RECORD rec, next_rec;
	rec.rec_index = 0;
	rec.arrive_train_id = strTrainId;
	rec.depart_train_id = strTrainId;
	rec.arrive_side = 0;    
	rec.depart_side = 0;
	rec.adjust_status = 0;
	rec.flag = 0;
	rec.special_train_index = newtrain.GetTrainIndex();

	rec.arrive  = prec_cur->arrive+20*60;
	rec.depart  = rec.arrive;

	rec.plan_arrive = rec.arrive;
	rec.plan_depart = rec.depart;
		
	rec.stop_abs_s_time = 10;
	rec.stop_abs_e_time = 30;


	next_rec.rec_index = 1;
	next_rec.arrive_train_id = strRetTrainId;
	next_rec.depart_train_id = strRetTrainId;
	next_rec.arrive_side = 0;    
	next_rec.depart_side = 0;
	next_rec.adjust_status = 0;
	next_rec.flag = 0;

	next_rec.arrive = rec.arrive + 40*60; 
	next_rec.depart = next_rec.arrive;
	next_rec.plan_arrive = next_rec.arrive;
	next_rec.plan_depart = next_rec.depart;
	
	SetAbsPartRunTrain(rescue_train.myTrainInfo.adjust_flag);
	if(bReverse)
	{
		rec.station = nNextStation;
		rec.entry	= 0;
		rec.exit	= nNextStationEntry;

		rec.in_station_index  = next_station_index;
		rec.out_station_index = rec.in_station_index;
		
		SetFlagStart(rec.flag);
		SetFlagRescue(rec.flag);
		SetFlagStopAbs(rec.flag);
		if(station_index > next_station_index)
			SetFlagStopAbsDown(rec.flag);
		else
			SetFlagStopAbsUp(rec.flag); 
		
		rescue_train.myTrainInfo.stop_abs_time = abs(rec.arrive-midt)/60 + 10;

		// ��һվ
		next_rec.station = rec.station; 
		next_rec.entry = rec.exit;
		next_rec.exit  = 0;

		SetFlagTerminal(next_rec.flag);
	}
	else
	{
		rec.station = nStation;
		rec.entry = 0;
		rec.exit  = nStationExit;

		rec.in_station_index  = station_index;
		rec.out_station_index = rec.in_station_index;
		
		SetFlagStart(rec.flag);
		SetFlagRescue(rec.flag);

		SetFlagStopAbs(rec.flag);
		if(station_index < next_station_index)
			SetFlagStopAbsDown(rec.flag);
		else
			SetFlagStopAbsUp(rec.flag); 
		
		rescue_train.myTrainInfo.stop_abs_time = abs(rec.arrive-midt)/60 + 10;

		// ��һվ
		next_rec.station = rec.station; 
		next_rec.entry = rec.exit;
		next_rec.exit  = 0;

		SetFlagTerminal(next_rec.flag);
	}
	next_rec.in_station_index  = rec.in_station_index;
	next_rec.out_station_index = next_rec.in_station_index;
	
	rescue_train.AddRecord(rec, m_pDoc->GetTgType());
	rescue_train.AddRecord(next_rec, m_pDoc->GetTgType());

	m_pDoc->SendAppendTrain(&rescue_train, "", 0);
}

void CTGForm::CreateWholeHelpRunStation(CTG_TRAIN& newtrain, CString strTrainId, CString strRetTrainId, bool bReverse)
{
	TgTRAIN_RECORD *prec_cur = newtrain.GetRecordByPosition (m_nFocusRecordIndex);		
	TgTRAIN_RECORD *prec_nxt = newtrain.GetRecordByPosition (m_nFocusRecordIndex + 1);		
	if(prec_cur == NULL || prec_nxt == NULL)
	{
		MessageBox("��ѡ�зֲ�����ʱ, �ֲ����п���վ");
		return;
	}
	int nStation=prec_cur->station;
	int nNextStation = prec_nxt->station;
	int nStationExit = prec_cur->exit;
	int nNextStationEntry = prec_nxt->entry;
	int station_index = prec_cur->out_station_index;
	int next_station_index = prec_nxt->in_station_index;

	int nRecNum = newtrain.GetRecordNum() ;
	for (int i = nRecNum -1 ; i > m_nFocusRecordIndex; i--)
	{
		newtrain.DeleteTailRecord();
	}

	TgTRAIN_RECORD *plast = newtrain.GetLastRecord();
	if(plast==NULL)
		return;
	if(plast->station!=prec_cur->station)
		return;
	ClearFlagTrainPartRun(plast->flag);
	SetFlagTrainWholeHelp(plast->flag);
	SetFlagGiveout(plast->flag);

	plast->stop_abs_s_time = 5;
	plast->stop_abs_e_time = 5;
	
	ReSetAbsDraw(plast->stop_reason);
	CancelFlagStopSignal(plast->flag);
	SetFlagStopAbs(plast->flag);
	if(next_station_index>station_index)
		SetFlagStopAbsDown(plast->flag);
	else
		SetFlagStopAbsUp(plast->flag);
	SetFlagAbsTerminal(plast->flag);
	plast->out_station_index = plast->in_station_index;
	
	long midt = plast->depart + plast->stop_abs_s_time*60;

	m_pDoc->SendUpdateTrain(newtrain);

	CTG_TRAIN rescue_train;
	SetAbsPartRunTrain(rescue_train.myTrainInfo.adjust_flag);
	
	TgTRAIN_RECORD rec, next_rec;
	rec.rec_index = 0;
	rec.arrive_train_id = strTrainId;
	rec.depart_train_id = strTrainId;
	rec.arrive_side = 0;    
	rec.depart_side = 0;
	rec.adjust_status = 0;
	rec.flag = 0;
	rec.arrive  = plast->arrive+20*60;
	rec.depart  = rec.arrive;
	rec.plan_arrive = rec.arrive;
	rec.plan_depart = rec.depart;

	rec.stop_abs_s_time = 10;
	rec.stop_abs_e_time = 30;

	rec.special_train_index = newtrain.GetTrainIndex();

	next_rec.rec_index = 1;
	next_rec.arrive_train_id = strRetTrainId;
	next_rec.depart_train_id = strRetTrainId;
	next_rec.arrive_side = 0;    
	next_rec.depart_side = 0;
	next_rec.adjust_status = 0;
	next_rec.flag = 0;
	next_rec.arrive = rec.arrive + 40*60; 
	next_rec.depart = next_rec.arrive;
	next_rec.plan_arrive = next_rec.arrive;
	next_rec.plan_depart = next_rec.depart;

	if(bReverse)
	{
		rec.station = nNextStation;
		rec.entry	= 0;
		rec.exit	= nNextStationEntry;

		rec.in_station_index  = next_station_index;
		rec.out_station_index = rec.in_station_index;

		SetFlagStart(rec.flag);
		SetFlagStopAbs(rec.flag);
		SetFlagRescue(rec.flag);
	
		if(station_index > next_station_index)
			SetFlagStopAbsDown(rec.flag);
		else
			SetFlagStopAbsUp(rec.flag); 
		
		rescue_train.myTrainInfo.stop_abs_time = abs(rec.arrive-midt)/60 + 10;

		// ��һվ
		next_rec.station = rec.station;
		next_rec.entry   = rec.exit;
		next_rec.exit    = 0;

		SetFlagTerminal(next_rec.flag);
	}
	else
	{
		rec.station = nStation;
		rec.entry   = 0;
		rec.exit    = nStationExit;

		rec.in_station_index  = station_index;
		rec.out_station_index = rec.in_station_index;
			
		SetFlagStart(rec.flag);
		SetFlagStopAbs(rec.flag);
		SetFlagRescue(rec.flag);
	
		if(station_index < next_station_index)
			SetFlagStopAbsDown(rec.flag);
		else
			SetFlagStopAbsUp(rec.flag); 
		
		rescue_train.myTrainInfo.stop_abs_time = abs(rec.arrive-midt)/60 + 10;

		// ��һվ
		next_rec.station = rec.station;
		next_rec.entry   = rec.exit;
		next_rec.exit    = 0;
		
		SetFlagTerminal(next_rec.flag);
	}
	next_rec.in_station_index  = rec.in_station_index;
	next_rec.out_station_index = next_rec.in_station_index;

	rescue_train.AddRecord(rec, m_pDoc->GetTgType());
	rescue_train.AddRecord(next_rec, m_pDoc->GetTgType());

	m_pDoc->SendAppendTrain(&rescue_train, "", 0);
}

int CTGForm::GetThisExitByQuestion(CTG_TRAIN& train)
{
	int nThisExit = 0;
	TgTRAIN_RECORD *prec = train.GetRecordByRecIndex(train.GetRecordNum()-2);
	TgTRAIN_RECORD *last = train.GetLastRecord();
				
	do
	{
		if(prec==NULL || last==NULL)
		{
			break;
		}

		int nAbsList[32];
		// ��ȡ���������г�,���ó��������߱�
		int nAbsNum=m_pDoc->GetExitByNodeIndex12ForTo_abs(prec->out_station_index,last->in_station_index,nAbsList);
		if(nAbsNum<=0)
		{
			break;
		}
		if(nAbsNum==1)
		{
			nThisExit = m_pDoc->ConvertAbsnumStationToEntry(prec->station, nAbsList[0]);
		}
		else
		{
			CDlgGiveoutDirectionSel dlg(m_pDoc, this, 9, nAbsList, nAbsNum, prec->station);
			if(dlg.DoModal() == IDOK)
			{
				nThisExit = dlg.nSelDirect;
			}
		}
	}while(0);

	return nThisExit;
}
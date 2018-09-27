// TGDoc.cpp : implementation file
//
#include "stdafx.h"

#include "IO.h"
#include "IF.h"

#include "tgdoc.h"
#include "resource.h"
#include "messages.h"
#include "winspool.h"
#include ".\dlg\SuperPassDlg.h"

#include "tgForm.h"
#include "math.h"

#include ".\dlg\DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MSG_UPDATE_RELATION 10012
#define HEIGHT_OFFSET   15

// �����г���ƫ��60����
const int FindTrainMoveDlt=60;
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTGDoc, CDocument)

int CTGDoc::GetShiftId()
{
	return m_ptgctrl->GetShiftId();
}

void CTGDoc::SendDeleteTrainXml(ULONG train_index)
{
	std::vector<ULONG> selectTrain;
	selectTrain.push_back(train_index);
	SendDeleteTrainXml(selectTrain);
}

void CTGDoc::SendDeleteTrainXml(std::vector<ULONG>& selectedTrains)
{
	CString xml, tmp;

	xml = "<TRAINS> ";
	for(int i = 0; i < selectedTrains.size(); i++)
	{
		tmp.Format("<train_index> %d </train_index>\n", selectedTrains[i]);
		xml += tmp;
	}
	xml += "</TRAINS>";
	sysprint(5, 0, "[TGOCX]����ɾ���г��� %s",xml);
	m_ptgctrl->FIRE_DELETE(xml);
}

 // flag:0������ʾ�Ի��� 1ֱ��ɾ��
void CTGDoc::SendDeleteBlockXml(ULONG block_index, int flag)
{
	CString xml, tmp;
	
	xml = "<BLOCKS> ";
	
		tmp.Format("<block_index flag=\"%d\"> %d </block_index>\n", flag, block_index);
		xml += tmp;
			
	xml += "</BLOCKS>";
	
	sysprint(5, 0, "[TGOCX]����ɾ��ʩ�����ţ� %s",xml);
	m_ptgctrl->FIRE_DELETE(xml);
}

void CTGDoc::SendAppendMark(CTg_BLOCK* block)
{
	if(NULL != block)
	{
		block->combstation = block->station1 << 16;
		block->combstation += block->station2;
		
		CString xml = block->toXML();
		sysprint(0,0, "[TGOCX]����Ա����ʩ������ %s", xml);
		m_ptgctrl->FIRE_TG_APPEND_MARK(block->mark_index, xml);
	}
}

void CTGDoc::SendUpdateMark(CTg_BLOCK* block)
{
	if(NULL != block)
	{
		CString xml = block->toXML();
		sysprint(5, 0, "[TGOCX]����ʩ�����Ÿ��£�%s",xml);
		m_ptgctrl->FIRE_TG_UPDATE_MARK(block->mark_index, xml);
	}
}


void CTGDoc::SendAppendMark(CTg_BLOCK& block)
{
	block.combstation = block.station1 << 16;
	block.combstation += block.station2;
	
	CString xml = block.toXML();
	sysprint(5, 0, "[TGOCX]��������ʩ�����ţ�%s",xml);
	m_ptgctrl->FIRE_TG_APPEND_MARK(block.mark_index, xml);
}

void CTGDoc::SendUpdateMark(CTg_BLOCK& block)
{
	CString xml = block.toXML();
	sysprint(5, 0, "[TGOCX]����ʩ�����Ÿ��£�%s",xml);
	m_ptgctrl->FIRE_TG_UPDATE_MARK(block.mark_index, xml);
}

void CTGDoc::ResetBasicTrainDate(CTG_TRAIN* train)
{
	if(TG_TYPE::BASIC_TG != this->GetTgType())
		return;

	for(int i=0; i<train->myRecord.size(); i++)
	{
		train->myRecord[i].arrive = train->myRecord[i].arrive%86400;
		train->myRecord[i].depart  = train->myRecord[i].depart%86400;
		train->myRecord[i].plan_arrive = train->myRecord[i].arrive;
		train->myRecord[i].plan_depart  = train->myRecord[i].depart;
		train->myRecord[i].plan_arrive_side = train->myRecord[i].arrive_side;
		train->myRecord[i].plan_depart_side = train->myRecord[i].depart_side;
	}
}

void CTGDoc::SetBasicNoStatLateTime(CTG_TRAIN* train)
{
	if(TG_TYPE::BASIC_TG != this->GetTgType())
		return;

	for(int i=0; i<train->myRecord.size(); i++)
	{
		train->myRecord[i].static_arrive=0;
		train->myRecord[i].static_depart=0;
	}
}

void CTGDoc::SetBasicTrainDate(CTG_TRAIN* train)
{
	if(TG_TYPE::BASIC_TG != this->GetTgType())
		return;

	ResetBasicTrainDate(train);

	int day = 0;
	int before = -1;
	for(int i=0; i<train->myRecord.size(); i++)
	{
		if(before > train->myRecord[i].arrive)
		{
			day++;
		}
		before = train->myRecord[i].arrive;
		train->myRecord[i].arrive += day*86400;
		train->myRecord[i].plan_arrive = train->myRecord[i].arrive;

		if(before > train->myRecord[i].depart)
		{
			day++;
		}
		before = train->myRecord[i].depart;
		train->myRecord[i].depart += day*86400;
		train->myRecord[i].plan_depart  = train->myRecord[i].depart;
	}
}

void CTGDoc::SendAppendTrain(CTG_TRAIN* train, CString strChangeTrainId, BYTE nRunType)
{
	if(NULL != train)
	{
		// �ֹ������۷��г��仯����
		if(!strChangeTrainId.IsEmpty())
		{
			int idx=-1;
			for(int i=1; i<train->GetRecordNum(); i++)
			{
				if(train->myRecord[i-1].station == train->myRecord[i].station)
				{
					idx = i;
					break;
				}
			}
			for(int i=idx; i<train->GetRecordNum(); i++)
			{
				train->myRecord[i].arrive_train_id = strChangeTrainId;
				train->myRecord[i].depart_train_id = strChangeTrainId;
			}
		}

		SetAbsReturnFlag(train);

		if(TG_TYPE::BASIC_TG == this->GetTgType())
		{
			SetBasicTrainDate(train);
			SetBasicNoStatLateTime(train);
		}
		// ������Ĭ��λ���м�
		SetDisplyTractionMiddle(train->myTrainInfo.adjust_flag);

		for(int i=0; i<train->GetRecordNum();i++)
		{
			train->myRecord[i].static_arrive=0;
			train->myRecord[i].static_depart=0;
			train->myRecord[i].plan_arrive = train->myRecord[i].arrive;
			train->myRecord[i].plan_depart = train->myRecord[i].depart;
		}

		for(int i=0; i< m_aryEngine.GetCount(); i++)
		{
			TgTRAIN_ENGINE_INFO info;

			CString strLocoId = "";
			CString engine_id = m_aryEngine.GetAt(i);
			DWORD status = m_aryStatus.GetAt(i);
			info.engine_name="";
			if(engine_id.GetLength()>3)
			{
				strLocoId = engine_id.Left(3);
				info.engine_name=GetLocoNameByLocoId(atoi(strLocoId));
			}

			int type = GetCascoIdByLocoId(atoi(strLocoId));
			if(type >= 128)
				SetElecEngineExt(info.engine_status);
			else
				SetDieselEngineExt(info.engine_status);

			
			int nSel = (status&0xFF0000)>>16;
			if(nSel==0)
				SetTractionEngineExt(info.engine_status);
			else
				SetAttachEngineExt(info.engine_status);

			
			info.attach_sta = 0;
			info.unattach_sta = train->GetRecordNum()-1;

			info.engine_id = engine_id;
			train->myEngine.push_back(info);
		}
		m_aryEngine.RemoveAll();
		m_aryStatus.RemoveAll();

		CString xml = train->toXML(0, 0, 0, "", nRunType);
		sysprint(5, 0, "[TGOCX]���������г���%s", xml);
		m_ptgctrl->FIRE_TG_APPEND_TRAIN(0, xml);
	}
}

int CTGDoc::GetCascoIdByLocoId(int id)
{
	for(int i=0; i<locoId.GetCount();i++)
	{
		if(id == locoId.GetAt(i))
		{
			return cascoId.GetAt(i);
		}
	}

	return 0;
}
CString CTGDoc::GetLocoNameByLocoId(int id)
{
	for(int i=0; i<locoId.GetCount();i++)
	{
		if(id == locoId.GetAt(i))
		{
			return locoPref.GetAt(i);
		}
	}

	CString pref;
	pref.Format("%d", id);
	return pref;
}

void CTGDoc::SendDeleteTrain(CTG_TRAIN& train)
{
	sysprint(0,0, "SendDeleteTrain:%s index %u schd %d %s", GetTrainShowId(train), train.GetTrainIndex(), GetTgTermType(), train.toString());
	CString xml = train.toXML(0, 0, 0, "", 0);
	m_ptgctrl->FIRE_TG_DELETE_TRAIN(train.GetTrainIndex(), xml);
}

void CTGDoc::SendUpdateTrain(CTG_TRAIN* ptrain, BYTE Flag, DWORD nFlagEx, WORD sta, WORD rec_index, CString trainid)
{
	if(NULL != ptrain)
	{
		SetAbsReturnFlag(ptrain);
		SetTrainRelationSideModify(ptrain);
		if(TG_TYPE::BASIC_TG == this->GetTgType())
		{
			SetBasicTrainDate(ptrain);
			SetBasicNoStatLateTime(ptrain);
		}
		
		if(ptrain->GetTrainIndex() != m_ContinueDrawInfo.m_nTrainIndex)
			ResetContinueDraw();

		if(m_ContinueDrawInfo.m_bContinueDraw)
		{
			nFlagEx = CONTINUE_DRAW_TRAIN;
			rec_index = m_ContinueDrawInfo.m_nRecIndex;
		}
		ResetContinueDraw();
		
		if(nFlagEx == PASS_TO_STAY || nFlagEx == STAY_TO_PASS)
		{
			m_pView->m_nFocusRecordIndex=-1;
			m_pView->m_nFocusRectIndex=-1;
		}

		sysprint(0,0, "SendUpdateTrain��%s index %u schd %d %s", GetTrainShowId(ptrain), ptrain->GetTrainIndex(), GetTgTermType(), ptrain->toString());
		CString xml = ptrain->toXML(nFlagEx, sta, rec_index, trainid, 0);
		m_ptgctrl->FIRE_TG_UPDATE_TRAIN(ptrain->GetTrainIndex(), xml);
	}
}

void CTGDoc::SendUpdateTrain(CTG_TRAIN& train, BYTE Flag, DWORD nFlagEx, WORD sta, WORD rec_index, CString trainid)
{
	SetAbsReturnFlag(&train);
	SetTrainRelationSideModify(&train);
	
	if(TG_TYPE::BASIC_TG == this->GetTgType())
	{
		SetBasicTrainDate(&train);
		SetBasicNoStatLateTime(&train);
	}

	if(train.GetTrainIndex() != m_ContinueDrawInfo.m_nTrainIndex)
		ResetContinueDraw();

	if(m_ContinueDrawInfo.m_bContinueDraw)
	{
		nFlagEx = CONTINUE_DRAW_TRAIN;
		rec_index = m_ContinueDrawInfo.m_nRecIndex;
	}
	ResetContinueDraw();

	if(nFlagEx == PASS_TO_STAY || nFlagEx == STAY_TO_PASS)
	{
		m_pView->m_nFocusRecordIndex=-1;
		m_pView->m_nFocusRectIndex=-1;
	}

	sysprint(0,0, "SendUpdateTrain:%s index %u schd %d %s", GetTrainShowId(train), train.GetTrainIndex(), GetTgTermType(), train.toString());
	train.myTrainInfo.b_self_update=true;
	//myPDataManage->UpdateTrain(train);
	CString xml = train.toXML(nFlagEx, sta, rec_index, trainid, 0);
	m_ptgctrl->FIRE_TG_UPDATE_TRAIN(train.GetTrainIndex(), xml);
}

void CTGDoc::SetAbsReturnFlag(CTG_TRAIN* train)
{
	if(NULL == train)
		return;

	for(int i=1; i<train->GetRecordNum();i++)
	{
		if(IsFlagStopAbs(train->myRecord[i-1].flag))
		{
			if(train->myRecord[i-1].station == train->myRecord[i].station)
				SetTrainFlag(train->myTrainInfo.adjust_flag);
		}
	}
	
}

void CTGDoc::sysprint(short msg_level,unsigned short process_id,char *format, ... )
{
	va_list marker;
	va_start(marker,format);
	CString tmpstr;
	tmpstr.FormatV(format,marker); 
	va_end( marker );
	
	this->m_ptgctrl->FIRE_TG_LOG(tmpstr);
}

bool CTGDoc::IsCanModifyTrainStation(const TgTRAIN_MORE_INFO& train_info)
{
	if(!this->m_ptgctrl->mConfigTg.m_context.IsCtrlModifyTrainStation())
		return true;

	// TMIS�������޸��г���վ
	if(IsTrainFromShift(train_info))
		return false;

	return true;
}

bool CTGDoc::GetOperatorRight(BYTE oper) 
{ 
	BYTE right_mask = 0xff;
	switch(oper)
	{
	case OPER_RIGHT::TG_EDIT: // 1
		right_mask &= 0x01;
		break;
	case OPER_RIGHT::MARK_EDIT: // 2
		right_mask &= 0x02;
		break;
	case OPER_RIGHT::TG_AND_MARK_EDIT: // 3
		right_mask &= 0x03;
		break;
	case OPER_RIGHT::NONE_EDIT: // 0
		right_mask &= 0x00;
		break;
	default:
		right_mask &= 0x00;
	}

	if(!this->m_bCalcStationIndex)
		right_mask &= 0x00;

	if ( (m_ptgctrl->mConfigTg.m_context.GetOperateRight() & right_mask) == oper)
		return true;
	return false;
}

void CTGDoc::SetTgCtrlPtr(CTGCtrl* ptgctrl)
{
	m_ptgctrl = ptgctrl;
}

void CTGDoc::SetTimeValid(CTG_TRAIN* single)
{
	if(NULL == single)
		return;

	TgTRAIN_RECORD* first = single->GetFirstRecord();
	if((NULL != first) && IsFlagStart(first->flag))
	{
		first->arrive = first->depart;
		first->plan_arrive = first->plan_depart;
		first->arrive_side = first->depart_side;
		first->plan_arrive_side = first->plan_depart_side;
	}

	TgTRAIN_RECORD* last = single->GetLastRecord();
	if((NULL != last) &&IsFlagTerminal(last->flag))
	{
		last->depart = last->arrive;
		last->plan_depart = last->plan_arrive;
		last->depart_side = last->arrive_side;
		last->plan_depart_side = last->plan_arrive_side;
	}
}

CTGDoc::CTGDoc()
{   
	m_nADTimeSystle = 0;
	m_ptgctrl = NULL;
	myPDataManage = NULL;
	m_nCurrentSectionNo = 1;
	m_bCalcStationIndex = true;

	m_bShowOPMSTrainsInfo = TRUE;
	m_bShowConflict = FALSE;
	m_bShowAdjustErrorStatus = FALSE;
	m_bShowErrorStatus =FALSE;
	
	m_bGoodTrainNotTechStopNeedPwd=FALSE;

	//��ʾ�ϻ�ʱ��
	m_bShowFastOrSlowTime = FALSE;
	//�ֽ�ڣ�����վ���ʱ��
	m_bShowAcceptTrainLate = false;
	m_bShowAcceptTruckLate = false;
	m_bIsVerifyTrainRect = false;
	m_nTrainZoomRangeX=0;
	m_nTrainZoomRangeY=0;
	//����ͨ��
	m_bByPassFlag = FALSE;

	//�����г�
	m_bShowReverse = FALSE;

	m_bInhibitEditRealTime=FALSE; // ��ֹ�޸�ʵ�ʵ�
	m_bEditRealLine = FALSE;//�Ƿ�����޸�ʵ�ʵ�
	m_bDeleteMutiTrain = false; 
	m_bMoveMutiTrain=false;
	m_bShowdistanceStone = FALSE;
	m_bShowReturnTrainid=false;
	SetShowTrainAndMark();
    
	mBlockFont.CreateFont(17, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Arial");

	mBlockFontTime.CreateFont(14, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");

	fontText.CreateFont(
				16,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,                   // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"����");                    // lpszFacename
	fontTech.CreateFont(
				14,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                   // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"����");                    // lpszFacename

	// �����ɵ�
	fontSide.CreatePointFont(SingleTrainCfg::m_sideFontSize, _T("����"));

	// ����ʱ��
	fontSecondTime.CreatePointFont(SingleTrainCfg::m_timeSecondFontSize, _T("Arial Narrow"));
	fontADTime.CreatePointFont(SingleTrainCfg::m_timeFontSize, _T("Arial Narrow"));
	fontADTimeBold.CreatePointFont(SingleTrainCfg::m_timeFontSize, _T("Arial Narrow"));
	fontAbsTime.CreatePointFont(SingleTrainCfg::m_timeFontSize, _T("Arial Narrow"));
	       
	// �����
	fontEarlyLastTime.CreateFont(
				16,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,                   // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"����");                    // lpszFacename

	
	// �������з���
	fontAnalyTime.CreatePointFont(SingleTrainCfg::m_analyFontSize,_T("Arial Narrow"));
	fontPressure.CreatePointFont(90,_T("����"));

	m_brushBlack.CreateSolidBrush(CLR_BLACK);
	m_brushWhite.CreateSolidBrush(CLR_WHITE);
	m_brushBlue.CreateSolidBrush(CLR_BLUE);
	m_nDispDistanceNum = 0;
	
	
	m_nCurrentNodeIndex = -1;
	m_nCurrentStationNoOfMouse = -1;
	
	m_nDispDistanceNum = 0;
	m_nOperatorStatus = NORMAL_STATUS;	

	m_bShowDepartFlag=true;
	m_bShowVirtualStation=false;
	m_bUseStringKiloMeter=false;
	m_nShowWorkTechFlag=TRUE;
	//�ֽ�ڣ�����վ�ͻ�����������,û��ʹ��
	m_nShowWorkSideFlag = false;
	m_nShowErrWorkSideFlag = false;
	//��ʾ����������
	m_nShowElecLocoFlag = FALSE;

	m_bShowRouteNotAutoTrigerFlag=TRUE;
	m_bShowRouteTrigerOkFlag=TRUE;
	
	m_bShowSelCZHLine=false;
	m_bShowAllCZHLine=false;
	m_bShowPlanNoteText=true;
	m_bShowWholeScreen=false;

	m_bShowEngineCrewMenu=false;
	m_bStopStationWaveLine = false;
	
	//����ʵ�����������ٽ��ƻ����Ĺ���
	m_bLimitDistrictFunction = FALSE;
	focus_point = POINT_PLAN;

	for(int i=0; i<MAX_STATION_PER_SECTION; i++)
		m_pStationDef[i] = NULL;

	x_point_offset = 0;
	y_point_offset = 0;
	ResetContinueDraw();
	m_bAcceptCadData=true;
	m_nManualRealColor=0;
	m_pLayer1=NULL;
	m_pLayer2=NULL;
	m_pLayer3=NULL;
	
	m_nEraseTrainIndex=0;
	m_nTrainNoMaxLevel=3;
	m_nAutoMove=FALSE;
	m_bShowSingleTrain=false;
	m_bLineBold=false;
}

bool CTGDoc::IsCanEditTrain()
{
	if(	!this->GetOperatorRight(OPER_RIGHT::TG_EDIT) ||
		!this->GetOperatorPriorByDistrict(nCurrentMouseX, focus_point))
	{
		return false;
	}
	return true;
}

int CTGDoc::GetTgType()
{
	return m_ptgctrl->GetTGType();
}

int CTGDoc::GetTgTermType()
{
	return m_ptgctrl->GetTgTermType();
}

int CTGDoc::GetContextLineWidth() 
{ 
	return this->m_ptgctrl->mConfigTg.m_context.GetLineWidth();
}

int CTGDoc::GetEarlyLateTime()
{
	return this->m_ptgctrl->mConfigTg.m_context.GetLateEarlyTime();
}

bool CTGDoc::IsShowLocoNo()
{
	return this->m_ptgctrl->mConfigTg.m_context.GetShowLocoNo();
}

int CTGDoc::GetShowTrainType()
{
	return this->m_ptgctrl->mConfigTg.m_context.GetShowTrainType();
}

int CTGDoc::GetShowTrainDirect()
{
	return this->m_ptgctrl->mConfigTg.m_context.GetShowTrainDirect();
}

void CTGDoc::SetDocContext()
{
	m_nOperatorStatus = NORMAL_STATUS;		

	m_nShiftId = m_ptgctrl->GetShiftId(); 

	m_nShiftDate = (m_nShiftId/1000000 - 1990) * 380 +
		(m_nShiftId/10000-(m_nShiftId/1000000)*100 - 1) * 31 + 
		m_nShiftId/100 - (m_nShiftId/10000)*100  - 1 ;
	m_nShiftNo = m_nShiftId - m_nShiftId/100*100;

	if(m_ptgctrl->IsNewMode())
		SetGraphStartTime_New(m_nShiftId);
	else
		SetGraphStartTime(m_nShiftId);

	SetGraphBeginEndPixel();
	m_nPageWidth = m_ptgctrl->GetTimeLength() * m_fPixelPerHour;
	//ReadFromFile();  �ʾ���

	mSetLineNoAry.clear();
	int nSectionId = m_ptgctrl->mConfigTg.GetCurrentSectionID();
	int from_st[32], to_st[32], lineno[32];
	int count=m_ptgctrl->mConfigTg.m_tgdef.GetLineNo(nSectionId, from_st, to_st, lineno);
	if(count>0)
	{
		for(int i=0; i<count; i++)
		{
			Entry_Exit_Set infos;
			infos.station=from_st[i];
			infos.nextstation=to_st[i];
			infos.line_num=lineno[i];

			mSetLineNoAry.push_back(infos);
		}
	}

	m_ptgctrl->mConfigTg.m_tgdef.GetSectionDirectTip(nSectionId, mDirectionTipAry);	
	m_ptgctrl->mConfigTg.m_tgdef.GetStationGiveoutTip(nSectionId, mGiveoutTipAry);
	m_ptgctrl->mConfigTg.m_tgdef.GetStationAcceptTip(nSectionId, mAcceptTipAry);
}

void CTGDoc::SetGraphBeginEndPixel()
{
	mGraphBeginPixel = 1;
	mGraphEndPixel = mGraphBeginPixel + m_ptgctrl->GetTimeLength()*this->m_fPixelPerHour;

}
BOOL CTGDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	m_nDispDistanceNum = 0;

	m_nHorizontalPos = m_nVerticalPos = 0;

	m_nOperatorStatus = NORMAL_STATUS;
	m_nViewFormat = TEN_MINUTE_GRID;
	m_nBlockNum = m_ptgctrl->mConfigTg.m_context.GetBackgroudBlock();
	m_bElecTrainDrawGreenDecorate=m_ptgctrl->mConfigTg.m_context.IsElecDrawGreenDecorate();
	decorateStyle = m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetDecorateStyle();
	trainid_style = m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetSpecialTrainIdShowStyle();
	mDisplayUnknownText = m_ptgctrl->mConfigTg.m_context.IsDisplayUnknownText();
	mMouseMoveMode = m_ptgctrl->mConfigTg.m_context.GetMouseMoveLineFlag();
	if(m_ptgctrl->GetTGType()==TG_TYPE::BASIC_TG)
		mMouseMoveMode=0;
	line_style = m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetSpecialLineStyle();
	m_bIsVerifyTrainRect = m_ptgctrl->mConfigTg.m_context.IsVerifyTrainRect();
	m_nTrainZoomRangeX = m_ptgctrl->mConfigTg.m_context.GetZoomRangeX()/2;
	m_nTrainZoomRangeY = m_ptgctrl->mConfigTg.m_context.GetZoomRangeY()/2;
	mIsSchdLineSameToActualLine=m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsSchdLineSameToActualLine();
	mIsChangeVertColor=m_ptgctrl->mConfigTg.m_SingleTrainCfg.IsChangeVertColor();
	m_ptgctrl->mConfigTg.m_context.GetDrawRouteOkJFColor(mjcolor, mfcolor);
	myChezuhao.SetCZhRight(this);
	m_nManualRealColor   = this->m_ptgctrl->mConfigTg.m_context.GetColorManualReal();
	m_nTrainLeapLineFlag = this->m_ptgctrl->mConfigTg.m_context.GetLeapLineFlag();
	m_nADTimeSystle = this->m_ptgctrl->mConfigTg.m_context.GetADTimeStytle();

	m_nCurrentSectionNo = this->m_ptgctrl->mConfigTg.GetCurrentSectionID();
	m_bCalcStationIndex = this->m_ptgctrl->mConfigTg.m_tgdef.IsCalcInOutStaSection(m_nCurrentSectionNo);
	m_nTrainNoMaxLevel = this->m_ptgctrl->GetTrainNoLevelCount();
	const SectionDisplay* disp = this->m_ptgctrl->mConfigTg.m_tgdef.GetSectionDisplay(m_nCurrentSectionNo);
	if(disp == NULL)
	{
		AfxMessageBox("������ʾ���ô���");
		return FALSE;
	}

	int nCurrent = m_ptgctrl->mConfigTg.m_context.GetPixelPerMinute();
	

    m_nPageHeight = m_ptgctrl->mConfigTg.m_tgdef.GetSectionHeight(m_nCurrentSectionNo);
	m_nPageWidth = m_ptgctrl->GetTimeLength() * nCurrent * 60;
	m_fPixelPerMinute = nCurrent;
					
	m_fPixelPerHour   = m_fPixelPerMinute*60;
	m_fPixelPerHalfHour  = m_fPixelPerMinute*30;
	m_fPixelPerTenMinute = m_fPixelPerMinute*10;

	m_ptgctrl->mConfigTg.m_locoType.GetLocoAllPref(m_nCurrentSectionNo, locoName, locoPref, locoId, cascoId);
	
	ReadStationSide();

	InitStationVerticalPos(true); 

	CString FileName = this->GetDataPath() + "\\train_type.xml";
	mTrainType.ReadTrainType(FileName);
	SetGraphBeginEndPixel();
	ReadFromFile();
	SetDocContext();
	return TRUE;
}
CTGDoc::~CTGDoc()
{
	//m_waitforupdate.clear();
	for(int i=0; i<MAX_STATION_PER_SECTION; i++)
	{
		if(m_pStationDef[i] != NULL)
			delete m_pStationDef[i];
	}
	fontTech.DeleteObject();
	fontSide.DeleteObject();
	
	fontText.DeleteObject();
	fontADTime.DeleteObject();
	fontADTimeBold.DeleteObject();
	fontAbsTime.DeleteObject();
	fontEarlyLastTime.DeleteObject();
	fontAnalyTime.DeleteObject();
	fontPressure.DeleteObject();
	fontSecondTime.DeleteObject();

	mBlockFont.DeleteObject();
	mBlockFontTime.DeleteObject();

	fontReturnTrain.DeleteObject();
	fontNomorlTrain.DeleteObject();
	fontElecTrain.DeleteObject();
	fontSpaceTrain.DeleteObject();
	fontLengthTrain.DeleteObject();
	fontMilitaryTrain.DeleteObject();
	fontSuperTrain.DeleteObject();
	fontEngine.DeleteObject();

	m_brushWhite.DeleteObject();
	m_brushBlack.DeleteObject();
	m_brushBlue.DeleteObject();

	mLeftPen.DeleteObject();
	mRightPen.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTGDoc, CDocument)
	//{{AFX_MSG_MAP(CTGDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTGDoc diagnostics

#ifdef _DEBUG
void CTGDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTGDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTGDoc commands
void CTGDoc::ReadStationSide()
{
	for(int i=0; i<MAX_STATION_PER_SECTION; i++)
	{
		if(m_pStationDef[i] != NULL)
			delete m_pStationDef[i];
		m_pStationDef[i] = NULL;
	}
	m_nStationNum = MAX_STATION_PER_SECTION;
	m_ptgctrl->mConfigTg.GetStationList(m_nCurrentSectionNo, m_nStationNum, m_nCurrentStationList);

	for(int i=0;i < m_nStationNum;i++)
	{	
		//2007.04.18 ���Ӷ�����ɵ�����
		CString station_side_def_path;
		station_side_def_path.Format("%s\\stationside.def",GetDataPath());

		if(m_pStationDef[i] != NULL)
		{	
			delete m_pStationDef[i];
			m_pStationDef[i] = NULL;
		}

		m_pStationDef[i] = new CStationDef(m_nCurrentStationList[i], station_side_def_path);

		if (m_ptgctrl->mConfigTg.m_context.IsErrorSideStation(m_nCurrentStationList[i]))
			m_pStationDef[i]->ResetStationSide();

		short  line;
		CStationDef::READ_FILE_RESULT rs = m_pStationDef[i]->GetReadFileRs(&line); 
		if(CStationDef::READ_FILE_SUCC != rs)
		{
			CString strTemp;
			strTemp.Format("��ȡ�ļ�: %s���г�վ: %d line %d %s ", station_side_def_path, m_nCurrentStationList[i], line, m_pStationDef[i]->GetReadFileReason(rs));
			AfxMessageBox(strTemp);
		}
	}

	m_ptgctrl->mConfigTg.m_lineCfg.ClearLineCfg();
			
	// ��ʼ��LineCfg
	for(int i=0;i < m_nStationNum;i++)
	{
		if(m_pStationDef[i] == NULL)
			continue;

		LINE_INFO lineInfo;
		lineInfo.stano = m_pStationDef[i]->GetStationNo();
		short side_count = m_pStationDef[i]->GetReceDeptSideCnt();
		if(MAX_SIDES<=side_count)
		{
			CString str;
			str.Format("��վ %d �ɵ�̫��,ϵͳ���֧��254", lineInfo.stano);
			AfxMessageBox(str);
			continue;
		}

		// û�йɵ�,�Ͳ�Ҫ����-----վϸ�ļ���ȱ�ٳ�վ����ʱ,���򱨴��˳�
		if (side_count<=0)
			continue;
		
		static 	SIDE_NAME sidename[MAX_SIDES];
		static  SIDE_NO    sideno[MAX_SIDES];
		short sideNum=MAX_SIDES;

		if (STA_SUCCESS == m_pStationDef[i]->GetReceDeptSideNameAndNo(sidename, sideno, sideNum))
		{
			SIDE_INFO info;
			for (int j=0; j<sideNum; j++)			
			{
				info.sideno = sideno[j];
				info.nPlatform = 0;
				if(m_pStationDef[i]->isPlatform(info.sideno))
					info.nPlatform = 1;
				if(m_pStationDef[i]->isHighPlatform(info.sideno))
					info.nPlatform = 2;

				short side_property=0, side_direct=0;
				if(STA_SUCCESS==m_pStationDef[i]->GetSideProperty(info.sideno, side_property, side_direct))
				{
					info.side_property=side_property;
					info.direct=side_direct;
				}
				else
				{
					info.side_property=21;
					info.direct=0;
				}

				info.bCrhSide = (STA_SUCCESS == m_pStationDef[i]->isCRHSide(info.sideno));
				strncpy(info.sidename, sidename[j], sizeof(info.sidename));
				info.sidename[sizeof(info.sidename)-1]=0;

				lineInfo.vecSideInfo.push_back(info);
			}
		}
		
		m_ptgctrl->mConfigTg.m_lineCfg.AddLineCfg(lineInfo);
	}
}

bool CTGDoc::InitStationVerticalPos(bool bInitStaDistance, bool bWholeScreen)
{
	if(bInitStaDistance)
	{
		LINE_INFO staline;
		const node_info *pNodeInfo;
		m_nDispDistanceNum = this->m_ptgctrl->mConfigTg.m_tgdef.GetSectionNodeCount(m_nCurrentSectionNo);
		for(int idx = 0; idx < m_nDispDistanceNum; idx++)
		{
			pNodeInfo = this->m_ptgctrl->mConfigTg.m_tgdef.GetNodeByIndex(m_nCurrentSectionNo, idx);
			if(pNodeInfo == NULL)
			{
				CString msg;
				msg.Format("InitStationVerticalPos �����ļ� TG.def ����%d���Ҳ���index=%d�ĳ�վ����",m_nCurrentSectionNo, idx);
				MessageBox(NULL, msg, "����", MB_ICONHAND|MB_OK);
				return false;
			}
			int nCurStationId = pNodeInfo->station_number;

			if(this->GetTgType() != TG_TYPE::WORK_TG)
			{
				m_sDispDistance[idx].clStpcFlag = RGB(255,255,255);
				m_sDispDistance[idx].clLircFlag = RGB(255,255,255);
			}
			m_sDispDistance[idx].clrStationLine = pNodeInfo->clrStationLine;

			m_sDispDistance[idx].station_number = nCurStationId;
			m_sDispDistance[idx].sub_section = pNodeInfo->sub_section;
			strncpy(m_sDispDistance[idx].station_name,pNodeInfo->text,31);
			m_sDispDistance[idx].station_name[31] = 0;
			m_sDispDistance[idx].deltadistance = pNodeInfo->space;
			m_sDispDistance[idx].m_nCzhPrintFlag = pNodeInfo->m_nCzhPrintFlag;
			const TGStation* pStation = this->m_ptgctrl->mConfigTg.GetStation(nCurStationId);
            if(pStation == NULL)
			{
				CString msg;
				msg.Format("�ڿͻ������ó�վ���Ҳ��� STATION %d", nCurStationId);
				MessageBox(NULL, msg, "TG����", MB_ICONHAND|MB_OK);
				return false;
			}
			
			CString kilo=this->m_ptgctrl->mConfigTg.m_tgdef.GetKiloMk(m_nCurrentSectionNo, idx+1, nCurStationId);
			if(kilo.IsEmpty())
			    m_sDispDistance[idx].distance = pStation->fKmMark;
			else
				m_sDispDistance[idx].distance = kilo;

			m_sDispDistance[idx].bShowADTime = pStation->adtime;
			m_sDispDistance[idx].nStaProperty = pStation->property;

			/* add entry&side linestatus */
			if(!this->m_ptgctrl->mConfigTg.m_lineCfg.GetStaLineCfg(nCurStationId, staline))
			{
				CString msg;
				msg.Format("�������ļ�StationSide.cfg���Ҳ��� STATION %d", nCurStationId);
				MessageBox(NULL, msg, "TG����", MB_ICONHAND|MB_OK);
				return false;
			}

			m_sDispDistance[idx].fHigh = staline.fHigh;	
			
			m_sDispDistance[idx].number_of_side = staline.vecSideInfo.size();
			for(int i=0; i < sizeof(m_sDispDistance[idx].side)/sizeof(m_sDispDistance[idx].side[0]);i++)
			{
				m_sDispDistance[idx].side[i].side_number = -1;
				m_sDispDistance[idx].side[i].sidename[0] = '\0';
				m_sDispDistance[idx].side[i].fhighlow = false;
			}

			for(int index=0; index < staline.vecSideInfo.size(); index++)
			{
				const SIDE_INFO& pSide = staline.vecSideInfo[index];
				m_sDispDistance[idx].side[index].side_number = pSide.sideno;
				strncpy(m_sDispDistance[idx].side[index].sidename,  pSide.sidename, 14);
				m_sDispDistance[idx].side[index].sidename[14] = 0;
				
				m_sDispDistance[idx].side[index].nPlatform = pSide.nPlatform;
				m_sDispDistance[idx].side[index].side_property = pSide.side_property;
				m_sDispDistance[idx].side[index].direct = pSide.direct;
			}

			m_sDispDistance[idx].up_line_count = pNodeInfo->up_line_count;
			for(int i=0; i<sizeof(m_sDispDistance[idx].up_line_list)/sizeof(m_sDispDistance[idx].up_line_list[0]); i++)
			{
				if(i < m_sDispDistance[idx].up_line_count)
					m_sDispDistance[idx].up_line_list[i] = pNodeInfo->up_line_list[i];
				else
					m_sDispDistance[idx].up_line_list[i] = 0;
			}
			m_sDispDistance[idx].down_line_count = pNodeInfo->down_line_count;
			for(int i=0; i<sizeof(m_sDispDistance[idx].down_line_list)/sizeof(m_sDispDistance[idx].down_line_list[0]); i++)
			{
				if(i < m_sDispDistance[idx].down_line_count)
					m_sDispDistance[idx].down_line_list[i] = pNodeInfo->down_line_list[i];
				else
					m_sDispDistance[idx].down_line_list[i] = 0;
			}
			m_sDispDistance[idx].show_sation_sep = pNodeInfo->show_sation_sep;
			m_sDispDistance[idx].show_sides_mode = pNodeInfo->show_sides_mode;
			m_sDispDistance[idx].inhibit_sametime_dir = pNodeInfo->inhibit_sametime_dir;
			m_sDispDistance[idx].isNeedThick = pNodeInfo->isNeedThick;
			m_sDispDistance[idx].electric_arms = pNodeInfo->electric_arms;
			
			m_sDispDistance[idx].nStationYPos = 0;
			for(int i =0; i < sizeof(m_sDispDistance[idx].nSideYPos)/sizeof(m_sDispDistance[idx].nSideYPos[0]);i++)
				m_sDispDistance[idx].nSideYPos[i] = 0;

			m_sDispDistance[idx].usStationStatus = 255;
			m_sDispDistance[idx].usLircStatus = 255;
			m_sDispDistance[idx].nRouteCtrlMode = 255;
			m_sDispDistance[idx].nStationCtrlMode = 255;
			
			if(this->m_ptgctrl->mConfigTg.bAllStationActive)
			{
				m_sDispDistance[idx].usStationStatus = 0;
				m_sDispDistance[idx].usLircStatus = STATION_STATUS::ACTIVE;
			}
			
		}
	}

	// m_nCurrentSectionNo
	for(int idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		int sub=m_sDispDistance[idx].sub_section;
		int station=m_sDispDistance[idx].station_number;

		int entry, exit;
		bool bFind = this->m_ptgctrl->mConfigTg.m_tgdef.GetEntryExit(m_nCurrentSectionNo, sub, idx+1, station, entry, exit);
		if(bFind)
		{
			m_sDispDistance[idx].entry = entry;
			m_sDispDistance[idx].exit = exit;
		}
	}
	const SectionDisplay* pdisp = this->m_ptgctrl->mConfigTg.m_tgdef.GetSectionDisplay(m_nCurrentSectionNo);
	if(pdisp == NULL)
	{
		CString msg;
		msg.Format("�����ļ� TG.def ����%d����ʾ�����Ҳ���",m_nCurrentSectionNo);
		MessageBox(NULL, msg, "����", MB_ICONHAND|MB_OK);
		return false;
	}
	SectionDisplay disp= *pdisp;
	
	int fTotalDistance = 0;
	int nTotalHightOfSides = 0;
	for (int idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		// վ���
		fTotalDistance += m_sDispDistance[idx].deltadistance;

		// �ɵ�չ��������
		if(m_sDispDistance[idx].show_sation_sep != 0)
		{
			nTotalHightOfSides += 
				m_sDispDistance[idx].number_of_side * disp.nSideHeight;
		}
	}

	// ��ȥ���¿հ�,���������μ��͹ɵ�չ����վ
	int nTotal = disp.nUpSpaceHeight + disp.nDownSpaceHeight + nTotalHightOfSides;
	for(int idx = 0; idx < disp.nSpaceSubSectionCnt; idx++)
	{
		nTotal += disp.nSpaceBetweenSubSection[idx];
	}

	if(bWholeScreen)
	{
		while(m_nPageHeight<=nTotal)
		{
			disp.nUpSpaceHeight		=	disp.nUpSpaceHeight*0.75;
			disp.nDownSpaceHeight	=	disp.nDownSpaceHeight*0.75;
			disp.nSideHeight        =   disp.nSideHeight*0.75;
			for(int idx = 0; idx < disp.nSpaceSubSectionCnt; idx++)
			{
				disp.nSpaceBetweenSubSection[idx] = disp.nSpaceBetweenSubSection[idx]*0.75;
			}
			
			nTotalHightOfSides = 0;
			for (int idx = 0; idx < m_nDispDistanceNum; idx++)
			{
				// �ɵ�չ��������
				if(m_sDispDistance[idx].show_sation_sep != 0)
				{
					nTotalHightOfSides += 
						m_sDispDistance[idx].number_of_side * disp.nSideHeight;
				}
			}
			nTotal = disp.nUpSpaceHeight + disp.nDownSpaceHeight + nTotalHightOfSides;
			for(int idx = 0; idx < disp.nSpaceSubSectionCnt; idx++)
			{
				nTotal += disp.nSpaceBetweenSubSection[idx];
			}
		}
	}
	else
	{
		this->m_bShowWholeScreen=false;
	}

	int nTotalHightOfSection = m_nPageHeight - nTotal;
	
	int* nTmpStationYPos = new int[m_nDispDistanceNum+1];
	double rate = (1.0*nTotalHightOfSection) / fTotalDistance;
	for (int idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		int fDeltaDistance = m_sDispDistance[idx].deltadistance;
		if(fDeltaDistance == 0)
		{
			// �ϱ߾��
			if(idx == 0)
				nTmpStationYPos[idx] = disp.nUpSpaceHeight;
			else
			{
				int subSectionIndex = m_sDispDistance[idx].sub_section;
				nTmpStationYPos[idx] = 
					nTmpStationYPos[idx - 1] + disp.nSpaceBetweenSubSection[subSectionIndex-2];
			}
		}
		else
		{
			nTmpStationYPos[idx] = nTmpStationYPos[idx - 1] + (int)(fDeltaDistance * rate);
		}

		m_sDispDistance[idx].nStationYPos = nTmpStationYPos[idx];
		if(m_sDispDistance[idx].show_sation_sep)
		{
			for(int n = 0; n < m_sDispDistance[idx].number_of_side; n++)
			{
				int sideno = m_sDispDistance[idx].side[n].side_number;
				m_sDispDistance[idx].nSideYPos[sideno]
				        = m_sDispDistance[idx].nStationYPos + (n+1)*disp.nSideHeight;
				nTmpStationYPos[idx] = m_sDispDistance[idx].nSideYPos[sideno];
			}
			nTmpStationYPos[idx] = nTmpStationYPos[idx] + disp.nSideHeight;
		}
		else
		{
			for(int i =0; i < sizeof(m_sDispDistance[i].nSideYPos)/sizeof(m_sDispDistance[i].nSideYPos[0]);i++)
				m_sDispDistance[idx].nSideYPos[i] = 0;
		}

		m_sDispDistance[idx].nSecondStationYPos = nTmpStationYPos[idx];
	}
	delete[] nTmpStationYPos;
	return true;
}

void CTGDoc::GetPlotParamXml(ULONG nFlag)
{
	CString xml, tmp;
	xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?>";
	xml += "<PLOT_PARAM>";
	{
		tmp.Format("<MODE leap=\"%d\" electric=\"%d\" fast_slow=\"%d\" wave_line=\"%d\" hrb=\"%d\" manualRealColor=\"%d\" draw_side_point=\"%d\"/>", 
			this->m_nTrainLeapLineFlag, 
			this->m_nShowElecLocoFlag,
			this->m_bShowFastOrSlowTime,
			this->m_bStopStationWaveLine,
			this->m_ptgctrl->mConfigTg.m_context.IsHrbStaticEarlyLateTimeColor()?1:0,
			this->m_nManualRealColor,
			this->m_ptgctrl->mConfigTg.m_context.IsDrawStopSidePoint());
		xml +=tmp;
	}
	
	xml +="<NODES>";
	{
		for(int i=0; i<this->m_nDispDistanceNum;i++)
		{
			tmp.Format("<NODE index=\"%d\" station_no=\"%d\" sep_station=\"%d\" show_side=\"%d\" czh_printflag=\"%d\">", 
					i, this->m_sDispDistance[i].station_number, 
					   this->m_sDispDistance[i].show_sation_sep, 
					   this->m_sDispDistance[i].show_sides_mode,
					   this->m_sDispDistance[i].m_nCzhPrintFlag);
			xml +=tmp;
			{
				xml +="<SIDES>";
				for(int k=0;k<this->m_sDispDistance[i].number_of_side;k++)
				{
					tmp.Format("<SIDE side_no=\"%d\" side_name=\"%s\" side_property=\"%d\"/>", 
						m_sDispDistance[i].side[k].side_number, m_sDispDistance[i].side[k].sidename, m_sDispDistance[i].side[k].side_property);
					xml +=tmp;
				}
				xml +="</SIDES>";
			}
			xml +="</NODE>";
		}
		xml +="</NODES>";
	}
	
	if(this->m_ptgctrl->mConfigTg.m_context.IsStaticsTimeCompleteByTGDEF())
		xml +="<STATICS_TIME_COMPLETE_BY_TGDEF method=\"1\"/>";
	else
		xml +="<STATICS_TIME_COMPLETE_BY_TGDEF method=\"0\"/>";

	if(this->m_ptgctrl->mConfigTg.m_context.IsTrainStayRealPassStatisticDepart())
		xml +="<STATICS_TIME method=\"1\"/>";
	else
		xml +="<STATICS_TIME method=\"0\"/>";
	
	if(this->m_ptgctrl->mConfigTg.m_context.IsEnableInhibitDrawBlackArc())
		xml +="<NOLATETRAIN draw_black_arc=\"1\">";
	else
		xml +="<NOLATETRAIN draw_black_arc=\"0\">";

	// ��ӡ�����
	if (m_bUseStringKiloMeter)
		xml +="<USE_STRING_KILO_METER method=\"1\"/>";
	else
		xml +="<USE_STRING_KILO_METER method=\"0\"/>";

	std::map<ULONG, ULONG>::iterator it = m_noearlylatetime.begin();
	while(it != m_noearlylatetime.end())
	{
		CString tmp;
		tmp.Format("<TRAININDEX id=\"%d\" />", it->first);
		xml += tmp;
		it++;
	}
	xml +="</NOLATETRAIN>";

	// ���⳵վ
	{
		if(m_bShowVirtualStation)
			xml +="<VIRTUAL_STATION show=\"1\">";
		else
			xml +="<VIRTUAL_STATION show=\"0\">";

		std::map<int , SectionVirtualStation>::iterator it = m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.find(m_nCurrentSectionNo);
		if(it != m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.end())
		{
			for(int i=0; i<it->second.count; i++)
			{
				int perStationIndex=it->second.m_virtualStation[i].perStationIndex;
				int nextStationIndex=it->second.m_virtualStation[i].nextStationIndex;
			
				if(nextStationIndex<=perStationIndex)
					continue;

				if(perStationIndex<0)
					continue;

				if(nextStationIndex>=m_nDispDistanceNum)
					continue;
		
				CString strStaName=it->second.m_virtualStation[i].strStatinName;
				int nStation=it->second.m_virtualStation[i].nStation;
				int nPercent=it->second.m_virtualStation[i].nPercent;

				if(strStaName=="")
					strStaName="δ֪����վ";

				CString tmp;
				tmp.Format("<STATION conid=\"%d\" up=\"%d\" down=\"%d\" id=\"%d\" name=\"%s\" percent=\"%d\"/>",
					m_nCurrentSectionNo, perStationIndex,  nextStationIndex, nStation, strStaName, nPercent);

				xml +=tmp;
			}
		}
		xml +="</VIRTUAL_STATION>";
	}
	xml += "</PLOT_PARAM>";

	if(m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
	{
		ReGenTrainJiaoluRelation();  // ��ӡ��������ϵ���ļ�
		myChezuhao.OutputCZRelation(); // ��ӡ��������ϵ���ļ�
	}
	else
	{
		ReGenTrainCZHRelation();   
		myChezuhao.OutputCZRelation(); // ��ӡ��������ϵ���ļ�
	}

	this->m_ptgctrl->FIRE_SET_PLOT_PARAM(xml, nFlag);
}

int CTGDoc::GetHistoryRegionWidth()
{
	if(m_ptgctrl->IsNewMode())
	{
		return 60*m_nHistoryLen;
	}
	
	CTime tm = CTime::GetCurrentTime( );
	long lCurrentTime = tm.GetHour( ) * 60 + tm.GetMinute( );
	long lCurrentDate = (tm.GetYear( ) - 1990) * 380 + (tm.GetMonth( ) - 1)* 31 + tm.GetDay( ) - 1;

    int nHistoryGraphWidth = 0;
	if(m_nShiftDate == lCurrentDate && m_nShiftNo == 2)
		nHistoryGraphWidth = lCurrentTime + 6*60;
	else if(m_nShiftDate == lCurrentDate && m_nShiftNo == 1)
		nHistoryGraphWidth =  lCurrentTime + 18*60;
	else 
		nHistoryGraphWidth =  lCurrentTime - 6*60;

	return nHistoryGraphWidth;
}

void CTGDoc::DrawSeperatorBar(CDC * pCDC, long lColor)
{ 
	int uStartX,uStartY,uEndX,uEndY;

	int nHistoryRegionWidth = GetHistoryRegionWidth();

	uStartX =X_TIME_OFFSET + (int)(nHistoryRegionWidth * m_fPixelPerMinute) - 1;
	if(uStartX<0)
		return;
	uEndX = uStartX + 1;

	uStartY = 0;
	uEndY   = m_nPageHeight;
	
	CPen mPen,*pOldPen;
	mPen.CreatePen(PS_INSIDEFRAME,2,lColor);
	pOldPen = pCDC->SelectObject(&mPen);

	CString strNowTime;
	CTime tm = CTime::GetCurrentTime();
	strNowTime.Format("%0.2d:%0.2d",tm.GetHour(),tm.GetMinute());
	
	pCDC->SetBkMode( TRANSPARENT );
	pCDC->TextOut(uStartX-20, uStartY, strNowTime);

	// ��ʾ��ǰʱ���
	pCDC->MoveTo(uStartX,uStartY + 20);
	pCDC->LineTo(uStartX,uEndY);

	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
}

//���ƴ��ܲ����ߣ���Ϊ��������
void CTGDoc::DrawChordLine(CDC* pDC,POINT pt1,POINT pt2,int dir)
{
	//����̫����ֱ������
	if(abs(pt1.x-pt2.x)<10)
	{
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		return;
	}
	int rate = 1;
	const int A = 20*rate;  
	CPoint pt_start = pt1;
	CPoint pt_end   = pt2;  
	int X_WIDTH = pt_end.x - pt_start.x;
	CPoint pt = pt_start;
	double step = 2*PI/(X_WIDTH);
	for(double i=0;i<=2*PI; i += step)
	{
		pDC->MoveTo(pt);
		pt.x ++;	 
		if(dir == DOWN_DIRECT)
			pt.y = pt_start.y+A*sin(i);
		else
			pt.y = pt_start.y-A*sin(i);
		pDC->LineTo(pt);    
		//������ߣ��������Ļ���
		if(i+step >=2*PI)
		{
			pDC->LineTo(pt2);
			break;
		}
	}
}

//�����Ǹ����г������һ�������ж�
int CTGDoc::PointBetweenMidOfTwoStation(int yPos, TgTRAIN_RECORD *rec, int& nDirection)
{
	nDirection = -1;
	if(NULL == rec) 
		return FALSE;

	int node_index = rec->out_station_index;
	if(node_index < 0 || node_index >= m_nDispDistanceNum)
		return FALSE;

	int node_index_next = -1;
	// ͣ����������
	if(yPos > m_sDispDistance[node_index].nStationYPos)
	{
		nDirection = DOWN_DIRECT;
		for(int i=node_index+1; i < m_nDispDistanceNum; i++)
		{
			int middle_pos = (m_sDispDistance[node_index].nSecondStationYPos + m_sDispDistance[i].nStationYPos)/2;
			if(abs(yPos - middle_pos) < 8 && (m_sDispDistance[node_index].sub_section == m_sDispDistance[i].sub_section))
			{
				node_index_next = i;
				break;
			}
		}
	}
	// ͣ����������
	else
	{
		nDirection = UP_DIRECT;
		for(int i=node_index-1; i >= 0; i--)
		{
			int middle_pos = (m_sDispDistance[node_index].nStationYPos + m_sDispDistance[i].nSecondStationYPos)/2;
			if(abs(yPos - middle_pos) < 8 && (m_sDispDistance[node_index].sub_section == m_sDispDistance[i].sub_section))
			{
				node_index_next = i;
				break;
			}
		}
	}
	
	if(node_index_next >= 0)
	{
		if(JudeNodesConnected(node_index, node_index_next))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//�����Ǹ����г������һ�������ж�
void CTGDoc::PointBetweenMidOfTwoStation(int yPos, int& sta1, int& sta2)
{
	int nUpY = 0, nBottomY = 99999;
	sta1 = sta2 = 0;
	for(int idx = 0; idx < m_nDispDistanceNum - 1; idx++)
	{
		nUpY = m_sDispDistance[idx].nStationYPos;
		nBottomY = m_sDispDistance[idx+1].nStationYPos;
		
		if(m_sDispDistance[idx].show_sation_sep)
		{
			int nNextY = m_sDispDistance[idx].nSecondStationYPos;
			
			if( abs(nUpY - yPos) <= 8 )
			{
				sta1 = sta2 = m_sDispDistance[idx].station_number;
				break;
			}

			if( abs(nNextY - yPos) <= 8 )
			{
				sta1 = sta2 = m_sDispDistance[idx].station_number;
				break;
			}

			if( (nUpY < yPos) && (yPos < nNextY))
			{
				sta1 = sta2 = m_sDispDistance[idx].station_number;
				break;
			}

			if( (nNextY < yPos) && (yPos < nBottomY))
			{
				sta1 = m_sDispDistance[idx].station_number;
				sta2 = m_sDispDistance[idx+1].station_number;
				if(m_sDispDistance[idx].sub_section != m_sDispDistance[idx+1].sub_section)
				{
					sta1 = sta2 = 0;
				}
				break;
			}
		}
		else
		{
			if( abs(nUpY - yPos) <= 8 )
			{
				sta1 = sta2 = m_sDispDistance[idx].station_number;
				break;
			}

			if( abs(nBottomY - yPos) <= 8 )
			{
				sta1 = sta2 = m_sDispDistance[idx+1].station_number;
				break;
			}

			if( (nUpY < yPos) && (yPos < nBottomY))
			{
				sta1 = m_sDispDistance[idx].station_number;
				sta2 = m_sDispDistance[idx+1].station_number;

				if(m_sDispDistance[idx].sub_section != m_sDispDistance[idx+1].sub_section)
				{
					sta1 = sta2 = 0;
				}
				break;
			}
		}
	}
}

int CTGDoc::GetStationSideNo(int yPos,int node_index)
{
	if(node_index < 0 || node_index >= this->m_nDispDistanceNum)
		return -1;

	if(m_sDispDistance[node_index].show_sation_sep <= 0)
		return -1;
	int dlt = 99, nSideNo = -1;
	int number_of_side = sizeof(m_sDispDistance[0].side)/sizeof(m_sDispDistance[0].side[0]);

	for(int i=0; i < number_of_side; i++)
	{
		int y1 = m_sDispDistance[node_index].nSideYPos[i];
		if(y1 <= 0)
			continue;
        
		if( abs(yPos - y1) < dlt)
		{
			dlt = abs(yPos - y1);
			nSideNo = i;
		}
	}

	return nSideNo;
}

int CTGDoc::ConvertYPostoStationNo(int yPos,int& node_index,TgTRAIN_RECORD *last)
{
	int nStationNo = -1;
	node_index = -1;
	for(int i = 0; i < m_nDispDistanceNum; i++)
	{
		int y1 = m_sDispDistance[i].nStationYPos - 6;
		int y2 = m_sDispDistance[i].nSecondStationYPos + 6;

		if( yPos > y1 && yPos < y2)
		{
			node_index = i;
			nStationNo = m_sDispDistance[i].station_number;
		}
	}

	if(NULL != last)
	{
		if(!JudeNodesConnected(node_index, last->out_station_index))
		{
			node_index = -1;
			nStationNo = -1;
		}
	}

	return nStationNo;
}

int CTGDoc::ConvertYPostoPrimarySide(int yPos,int &nRouteSetNo)
{
	int i=-1,primary_side = -1;
	if(yPos < 0)
	{
		for(i=0;i < m_nDispDistanceNum;i++)
		{         
			if(abs(yPos - m_sDispDistance[i].nStationYPos) < 8/*10*/)
				break;
			bool bFound = FALSE;

			if(m_sDispDistance[i].show_sation_sep)
			{
				int y_point_side;
				for(int ii = 1; ii < m_sDispDistance[i].number_of_side; ii ++)
				{//search every side to look for point side 
					y_point_side = m_sDispDistance[i].nSideYPos[m_sDispDistance[i].side[ii].side_number];
					if(abs(yPos - y_point_side) < 8/*10*/)
					{
						primary_side = m_sDispDistance[i].side[ii].side_number;
						bFound = TRUE;
						break;
					}
				}
			}
			if(bFound == TRUE)
				break;
		}
	}
	else
	{
		for(i=0;i < m_nDispDistanceNum;i++)
		{
			if(abs(yPos - m_sDispDistance[i].nStationYPos) < 8/*10*/)
			{
				primary_side = 1;
				break;
			}
			bool bFound = FALSE;

			if(m_sDispDistance[i].show_sation_sep)
			{
				int y_point_side;
				for(int ii = 1; ii < m_sDispDistance[i].number_of_side; ii ++)
				{//search every side to look for point side 
					y_point_side = m_sDispDistance[i].nSideYPos[m_sDispDistance[i].side[ii].side_number];
					if(abs(yPos - y_point_side) < 8/*10*/)
					{
						primary_side = m_sDispDistance[i].side[ii].side_number;
						bFound = TRUE;
						break;
					}
				}
			}
			if(bFound == TRUE)
				break;
		}
	}

	if(i<0 || i >= m_nDispDistanceNum)
		return -1;
	nRouteSetNo = m_sDispDistance[i].sub_section;

	if(!m_sDispDistance[i].fHigh && yPos < 0)
		return -1;
	return primary_side;
}


  
//��ѯ������������ߵ� ��վ�͹ɵ�
CString CTGDoc::QueryCurrentTrainStationAndside(CPoint point, const ULONG index)
{
	CString Sresult;
	CPoint pt= point;
	do
	{
		CTG_TRAIN *pTrain = myPDataManage->GetTrain(index);
		if(pTrain == NULL) 
			break;

		if(m_nOperatorStatus != NORMAL_STATUS)
			break;
		if(pTrain->m_nPointCount <0 || pTrain->m_nPointCount > 256 )
			break;
		int j;
		for ( j=0;j<pTrain->m_nPointCount;j++)
		{
			if((pt.y >= pTrain->m_pPerPointInfo[j].point.y -3 && pt.y <= pTrain->m_pPerPointInfo[j].point.y + 3)
				&& (pt.x >= pTrain->m_pPerPointInfo[j].point.x -3 && pt.x <= pTrain->m_pPerPointInfo[j].point.x + 3))

				break;
		}
		if(j >= pTrain->m_nPointCount)
			break;
		if(pTrain->m_pPerPointInfo[j].record_index <0 || pTrain->m_pPerPointInfo[j].record_index > 128)
			break;
		//��þ������һ��record
		TgTRAIN_RECORD *rec = NULL;
		rec = pTrain->GetRecordByPosition(pTrain->m_pPerPointInfo[j].record_index);
		if(NULL == rec)
			break;
		
		if(pTrain->m_pPerPointInfo[j].start_depart_flag==ABS_ARRIVAL_POSITION 
				||pTrain->m_pPerPointInfo[j].start_depart_flag==ABS_DEPART_POSITION
				|| pTrain->m_pPerPointInfo[j].start_depart_flag==ABS_START_POSITION)
		{
			break;
		}

		CString sStation,sSide="",strPath="";
		sStation = GetStationName(rec->station);
		
		if(CTC_STATION == GetStationProperty( rec->station))
		{
			if(rec->arrive_side == rec->depart_side) 
				sSide = rec->str_depart_side;
			else 
			{
				//ʼ����ֻ��ʾ�����ɵ�
				if(IsFlagStart(rec->flag))
					sSide = rec->str_depart_side;
				else if(IsFlagTerminal(rec->flag))
					sSide = rec->str_arrive_side;
				else
				{
					sSide = rec->str_arrive_side;
					CString sSideDepart = rec->str_depart_side;
					if(rec->depart_side!=rec->arrive_side && sSideDepart!="")
						sSide = sSide +"��-"+sSideDepart+"��";
				}
			}
		}

		if(IsPressureOutStation(rec->stop_condition))
		{
			CString text;
			if(IsBeforePressure(rec->stop_condition))
			{
				text="ѹǰ";
			}
			if(IsAfterPressure(rec->stop_condition))
			{
				if(!text.IsEmpty())
					text+="ѹ��";
				else
					text="ѹ��";
			}
			sSide += text;
		}

		Sresult = sStation;

		WORD nRecNum = pTrain->GetRecordNum();
		BOOL bIsErrorLine = FALSE;
		if(pTrain->m_pPerPointInfo[j].record_index== nRecNum-1 && !IsFlagTerminal(rec->flag)) 
		{
			WORD NextLine,direction;
			if(!ConvertStationEntryToAbsnum(rec->station,rec->exit,&NextLine,&direction))
				break;

			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(NextLine);
			if(line == NULL)
				break;
			
			CString strBeginStaName, strEndStaName;
			if(line->nStartStation <= 0)
				strBeginStaName = "����";
			else
				strBeginStaName = GetStationName(line->nStartStation);

			if(line->nEndStation <= 0)
				strEndStaName = "����";
			else
				strEndStaName = GetStationName(line->nEndStation);

			if(rec->station == line->nStartStation)
			    strPath = strBeginStaName + "-->" + strEndStaName;
			else if(rec->station == line->nEndStation)
				strPath = strEndStaName + "-->" + strBeginStaName;
		}
		else if(pTrain->m_pPerPointInfo[j].record_index== 0 && !IsFlagStart(rec->flag)) 
		{
			WORD NextLine,direction;
			if(!ConvertStationEntryToAbsnum(rec->station,rec->entry,&NextLine,&direction))
				break;

			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(NextLine);
			if(line == NULL)
				break;

			CString strBeginStaName, strEndStaName;
			if(line->nStartStation <= 0)
				strBeginStaName = "����";
			else
				strBeginStaName = GetStationName(line->nStartStation);

			if(line->nEndStation <= 0)
				strEndStaName = "����";
			else
				strEndStaName = GetStationName(line->nEndStation);

			if(rec->station == line->nStartStation)
			    strPath = strEndStaName + "-->" + strBeginStaName;
			else if(rec->station == line->nEndStation)
				strPath = strBeginStaName + "-->" + strEndStaName;
		}
	
		if(strPath.IsEmpty())
			Sresult = sStation + sSide;
		else
			Sresult = sSide + strPath;
		
	}while(0);
	
	return  Sresult;
}

#define IsOneBetweenTwo(a, b, c)   ((a >b && a<c) || (a >c && a<b))

// ��ѯ���
ULONG CTGDoc::QueryCurrentBlock(const CPoint& point)
{
	ULONG index = 0;
	double distance = 99999999.0;
	std::map<ULONG, CTg_BLOCK>::iterator it_block = myPDataManage->myMark.begin();
	for(;it_block != myPDataManage->myMark.end();++it_block)
	{
		CTg_BLOCK& mark = it_block->second;

		if(mark.m_drawRect.top >= mark.m_drawRect.bottom)
		{
			int tmp = mark.m_drawRect.top;
			mark.m_drawRect.top = mark.m_drawRect.bottom;
			mark.m_drawRect.bottom = tmp+5;
		}
		if(point.x > mark.m_drawRect.right+5 || point.x < mark.m_drawRect.left-5)
			continue;
		if(point.y > mark.m_drawRect.bottom+5 || point.y < mark.m_drawRect.top-5)
			continue;

		mark.RestoreDrawFlag();
		
		long dltY = abs(point.y - mark.rRect.CenterPoint().y);
		long dltX = abs(point.x - mark.rRect.CenterPoint().x);
		double Lmin = dltY*dltY + dltX*dltX;
		long Ltmp = 0;

		dltY = abs(point.y - mark.cRect.CenterPoint().y);
		dltX = abs(point.x - mark.cRect.CenterPoint().x);
		Ltmp = dltY*dltY + dltX*dltX;
		if(Ltmp < Lmin)
			Lmin = Ltmp;
		
		dltY = abs(point.y - mark.lRect.CenterPoint().y);
		dltX = abs(point.x - mark.lRect.CenterPoint().x);
		Ltmp = dltY*dltY + dltX*dltX;
		if(Ltmp < Lmin)
			Lmin = Ltmp;

		if(mark.type == GRAPH_NOTE_MARK || mark.type == NEW_GRAPH_NOTE_MARK)
		{
			dltY = abs(point.y - mark.tRect.CenterPoint().y);
			dltX = abs(point.x - mark.tRect.CenterPoint().x);
			Ltmp = dltY*dltY + dltX*dltX;
			if(Ltmp < Lmin)
				Lmin = Ltmp;

			dltY = abs(point.y - mark.bRect.CenterPoint().y);
			dltX = abs(point.x - mark.bRect.CenterPoint().x);
			Ltmp = dltY*dltY + dltX*dltX;
			if(Ltmp < Lmin)
				Lmin = Ltmp;
		}

		Lmin = sqrt(Lmin);
		if(Lmin < distance)
		{
			distance = Lmin;
			index = mark.mark_index;
		}
	}

	it_block = myPDataManage->myMark.find(index);
	if(it_block == myPDataManage->myMark.end())
	{
		return 0;
	}

	CTg_BLOCK& mark = it_block->second;
	if(mark.cRect.PtInRect(point))
	{
		mark.bMoveCenter = TRUE;
		m_selmarkpoint=point;
	}
	else
		mark.bMoveCenter = FALSE;

	if(mark.lRect.PtInRect(point))
	{
		mark.bMoveLeft = TRUE;
		m_selmarkpoint=point;
	}
	else
		mark.bMoveLeft = FALSE;

	if(mark.rRect.PtInRect(point))
	{
		mark.bMoveRight = TRUE;
		m_selmarkpoint=point;
	}
	else
		mark.bMoveRight = FALSE;

	if(mark.type == GRAPH_NOTE_MARK || mark.type == NEW_GRAPH_NOTE_MARK)
	{
		if(mark.tRect.PtInRect(point))
			mark.bMoveTop = TRUE;
		else
			mark.bMoveTop = FALSE;

		if(mark.bRect.PtInRect(point))
			mark.bMoveBottom = TRUE;
		else
			mark.bMoveBottom = FALSE;
	}
	
	return index;
}

CString CTGDoc::GetThisTrainReturnTrainID(CTG_TRAIN& train, int rec_index)
{
	TgTRAIN_RECORD* rec = train.GetRecordByPosition(rec_index);
	if(rec == NULL)
		return "";

	CString strTrainId;
	do
	{
		if(rec_index+1==train.GetRecordNum())
		{
			if(IsFlagTerminal(rec->flag))
			{
				CString strReturnTrainID;
				if(myChezuhao.GetReturnTrainID(&train, strReturnTrainID))
				{
					strTrainId = rec->arrive_train_id + "/" + strReturnTrainID;
					break;
				}
			}
		}

		if(rec->arrive_train_id != rec->depart_train_id && !rec->arrive_train_id.IsEmpty() && !rec->depart_train_id.IsEmpty())
		{
			strTrainId = rec->arrive_train_id + "/" + rec->depart_train_id;
		}
		else
		{
			if (!rec->depart_train_id.IsEmpty()) 
				strTrainId = rec->depart_train_id;
			else
				strTrainId = rec->arrive_train_id;
		}
	}while(0);

	return strTrainId;
}

// ��ѯ�г�
CString CTGDoc::QueryCurrentTrain(CPoint *point,ULONG& index)
{
	index = 0;

	CPoint ptlStart, ptlEnd;

	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); ++it)
	{
		CTG_TRAIN& myTrain = it->second;
		
		if (myTrain.myRecord.size() <= 0 ) 
			continue;
		
		if(!m_bShowSingleTrain)
		{
			//����Կͻ���ѡ����ж�
			if(GetShowTrainType() != TYPE::ALL)
			{
				if(myTrain.m_nTrainType != GetShowTrainType())
					continue;
			}

			//��������г������г����ж�
			if(GetShowTrainDirect() != TRAIN_DIRECT::DUAL)
			{
				if(!myTrain.mbDoubleDirect)
				{
					if((myTrain.m_nDirectFlag+1) != GetShowTrainDirect())
						continue;
				}
			}
		}

		bool bContinue = true;
		for(int i = 0; i < myTrain.days; i++)
		{
			if(point->x > myTrain.myPosition[i].right || point->x < myTrain.myPosition[i].left)
				continue;
			if(point->y > myTrain.myPosition[i].bottom || point->y < myTrain.myPosition[i].top)
				continue;
			bContinue = false;
			break;
		}
		if(bContinue)
			continue;

		for(int k=0; k< myTrain.m_nPointCount; k++)
		{
			ptlStart = myTrain.m_pPerPointInfo[k].point;//.rect.CenterPoint();
			if (k < myTrain.m_nPointCount-1)
				ptlEnd =  myTrain.m_pPerPointInfo[k+1].point;//.rect.CenterPoint();
			else
				ptlEnd =  myTrain.m_pPerPointInfo[k].point;//.rect.CenterPoint();

			if(CalDisp(point->x, point->y, ptlStart.x, ptlStart.y) > 8.000)
			{
				if(PointLine(m_fPixelPerHour,point->x,point->y,ptlStart.x,ptlStart.y,ptlEnd.x,ptlEnd.y) > 8.000) 
					continue;
			}
			CString strTrainId;
			
			int x1 = ptlStart.x;
			int y1 = ptlStart.y;

			int x2 = ptlEnd.x;
			int y2 = ptlEnd.y;

			int x = point->x;
			int y = point->y;

			int dlt = abs((y-y1)*(x2-x1)-(y2-y1)*(x-x1));
			
			// ��λ���ڵ�
			if(CalDisp(point->x, point->y, ptlStart.x, ptlStart.y) <= 8.000 )
			{
				int rec_index = myTrain.m_pPerPointInfo[k].record_index;
				TgTRAIN_RECORD* rec = myTrain.GetRecordByPosition(rec_index);
				if(rec == NULL)
					continue;
				
				if(m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
				{
					strTrainId = GetThisTrainReturnTrainID(myTrain, rec_index);
				}
				else
				{
					if(rec->arrive_train_id != rec->depart_train_id && !rec->arrive_train_id.IsEmpty() && !rec->depart_train_id.IsEmpty())
					{
						strTrainId = rec->arrive_train_id + "/" + rec->depart_train_id;
					}
					else
					{
						if (!rec->depart_train_id.IsEmpty()) 
							strTrainId = rec->depart_train_id;
						else
							strTrainId = rec->arrive_train_id;
					}
				}
			}
			else if (k < myTrain.m_nPointCount-1 && myTrain.m_pPerPointInfo[k].record_index == myTrain.m_pPerPointInfo[k+1].record_index)
			{
				// ��������ģʽ,�������߲�ѡ��
				if(m_nTrainLeapLineFlag != 3)
				{
					if(myTrain.m_pPerPointInfo[k].nSubSection != myTrain.m_pPerPointInfo[k+1].nSubSection)
						continue;

					if(ptlStart.y > point->y || point->y > ptlEnd.y)
						continue;
				}

				if(dlt > 64)
					continue;

				int rec_index = myTrain.m_pPerPointInfo[k].record_index;
				TgTRAIN_RECORD* rec = myTrain.GetRecordByPosition(rec_index);
				if(rec == NULL)
					continue;
				
				if(rec->arrive_train_id != rec->depart_train_id && !rec->arrive_train_id.IsEmpty() && !rec->depart_train_id.IsEmpty())
				{
					strTrainId = rec->arrive_train_id + "/" + rec->depart_train_id;
				}
				else
				{
					if (!rec->depart_train_id.IsEmpty()) 
						strTrainId = rec->depart_train_id;
					else
						strTrainId = rec->arrive_train_id;
				}
			}
			
			// ��λ������
			else if (dlt<=180) //42
			{
				// ��������ģʽ,�������߲�ѡ��
				if(m_nTrainLeapLineFlag != 3)
				{
					if(myTrain.m_pPerPointInfo[k].nSubSection != myTrain.m_pPerPointInfo[k+1].nSubSection)
						continue;
				}

				int rec_index = myTrain.m_pPerPointInfo[k].record_index;
				if(k< myTrain.m_nPointCount-1)
					rec_index = myTrain.m_pPerPointInfo[k+1].record_index;
				TgTRAIN_RECORD* rec = myTrain.GetRecordByPosition(rec_index);
				if(rec == NULL)
					continue;

				if(k< myTrain.m_nPointCount-1)
				{
					if (!rec->arrive_train_id.IsEmpty())
						strTrainId = rec->arrive_train_id;
					else
						strTrainId = rec->depart_train_id;
				}
				else
				{
					if (!rec->depart_train_id.IsEmpty())
						strTrainId = rec->depart_train_id;
					else
						strTrainId = rec->arrive_train_id;
				}
			}
			else 
				continue;

			// �����۷���ҵ�г���ʾ---- 20090725���⴦��
			if(myTrain.GetRecordNum() == 2)
			{
				TgTRAIN_RECORD* first = myTrain.GetRecordByPosition(0);
				TgTRAIN_RECORD* second = myTrain.GetRecordByPosition(1);

				if(first != NULL && second != NULL)
				{
					if(first->station == second->station)
					{
						if(!second->arrive_train_id.IsEmpty() && !first->depart_train_id.IsEmpty() && first->depart_train_id != second->arrive_train_id )
						{
							strTrainId = first->depart_train_id +"/"+ second->arrive_train_id;
						}
					}
				}

			}

			index = myTrain.m_nTrainIndex;
			if(myTrain.myTrainInfo.b_elec_rank)
				strTrainId += "(��)";
			return strTrainId;
		}
	}

	index = 0;
	return "";
}


int CTGDoc::GetAbsYPosition(CTG_TRAIN * single, const int rec_index, WORD position)
{
	const TgTRAIN_RECORD* curr_rec = single->GetRecordByRecIndex(rec_index);
	const TgTRAIN_RECORD* next_rec = single->GetRecordByRecIndex(rec_index+1);
    if(NULL == curr_rec) return -1;

	int i = curr_rec->out_station_index;	
	if(i < 0 || i >= this->m_nDispDistanceNum)
	     return -1;
    
	int j = -1;
	if(NULL != next_rec)
	{
		j = next_rec->in_station_index;
		if(j<0 || j>=m_nDispDistanceNum)
			return -1;
	}

	//for draw stop abs
	if(i == j)
	{
		if(IsFlagStopAbsUp(curr_rec->flag) && i > 0)
		    return -abs(this->m_sDispDistance[i].nStationYPos - this->m_sDispDistance[i-1].nSecondStationYPos)/2;
		else if(IsFlagStopAbsDown(curr_rec->flag) && i < this->m_nDispDistanceNum-1)
			return abs(this->m_sDispDistance[i].nSecondStationYPos - this->m_sDispDistance[i+1].nStationYPos)/2;
		else
			return abs(this->m_sDispDistance[i].nSecondStationYPos - this->m_sDispDistance[j].nStationYPos)/2;
	}
	else
	{
		if(IsFlagStopAbsUp(curr_rec->flag))
		{
			if(position == 0)
				return -abs(this->m_sDispDistance[i].nStationYPos - this->m_sDispDistance[j].nSecondStationYPos)/2;
			else if(position == 1)
				return -(abs(this->m_sDispDistance[i].nStationYPos - this->m_sDispDistance[j].nSecondStationYPos)*3)/4;
			else if(position == 2 && i > 0)
				return -abs(this->m_sDispDistance[i].nStationYPos - this->m_sDispDistance[i-1].nSecondStationYPos)/2;
			else
				return -1;
		}
		else
		{
			if(position == 0)
				return abs(this->m_sDispDistance[i].nSecondStationYPos - this->m_sDispDistance[j].nStationYPos)/2;
			else if(position == 1)
				return (abs(this->m_sDispDistance[i].nSecondStationYPos - this->m_sDispDistance[j].nStationYPos)*3)/4;
			else if(position == 2 && i < this->m_nDispDistanceNum-1)
				return abs(this->m_sDispDistance[i].nSecondStationYPos - this->m_sDispDistance[i+1].nStationYPos)/2;
			else
				return -1;
		}
	}
}

int CTGDoc::GetStationVPos(int dist_index, int usSideFlag)
{
	if(dist_index<0||dist_index>=m_nDispDistanceNum)
		return -1;

	if(m_sDispDistance[dist_index].show_sation_sep)
	{
		int first_side = m_sDispDistance[dist_index].side[0].side_number;
		int number_of_side = m_sDispDistance[dist_index].number_of_side;
		if (number_of_side == 0) 
		{
			return m_sDispDistance[dist_index].nStationYPos;
		}

		int last_side = m_sDispDistance[dist_index].side[number_of_side-1].side_number;
		switch(usSideFlag)
		{
		case TOP_TRACK://first side 
			return m_sDispDistance[dist_index].nSideYPos[first_side];
			break;
		case BOTTOM_TRACK://last side 
			return m_sDispDistance[dist_index].nSideYPos[last_side];
			break;
		default:
			return m_sDispDistance[dist_index].nStationYPos;
			break;
		} 				
	}
	else
		return m_sDispDistance[dist_index].nStationYPos;
}

int CTGDoc::GetStationSideVPos(const TgTRAIN_RECORD* rec, const int index, const int side_no)
{
	if(index<0||index>=m_nDispDistanceNum)
		return -1;

	if(side_no == -1 || side_no == 255)
	{
		if(IsFlagStart(rec->flag))
		{
			int nY = m_sDispDistance[index].nStationYPos;
			if(rec->out_sta_direct == UP_DIRECT)
			{
				nY = m_sDispDistance[index].nStationYPos;
			}
			else if(rec->out_sta_direct == DOWN_DIRECT)
			{
				nY = m_sDispDistance[index].nSecondStationYPos;
			}
			
			return nY;
		}

		if(IsFlagTerminal(rec->flag))
		{
			int nY = m_sDispDistance[index].nSecondStationYPos;
			if(rec->in_sta_direct == UP_DIRECT)
			{
				nY = m_sDispDistance[index].nSecondStationYPos;
			}
			else if(rec->in_sta_direct == DOWN_DIRECT)
			{
				nY = m_sDispDistance[index].nStationYPos;
			}
			
			return nY;
		}
	}

	if(m_sDispDistance[index].show_sation_sep)
	{
		int num_of_side = sizeof(m_sDispDistance[index].side)/sizeof(m_sDispDistance[index].side[0]);
		
		for(int i=0; i < num_of_side && i < m_sDispDistance[index].number_of_side; i++)
		{
			int sideno = m_sDispDistance[index].side[i].side_number;
			if(sideno == -1)
				continue;

			if(sideno == side_no)
			{
				return m_sDispDistance[index].nSideYPos[sideno];
			}
		}	
	}
	
	return m_sDispDistance[index].nStationYPos;
}

int CTGDoc::GetStationVPos(CTG_TRAIN* single, TgTRAIN_RECORD* rec,int usSideFlag,int usFlag)
{
	int i = -1;
	if(EXIT_POSITION == usFlag)
	{
		if(0>rec->out_station_index || rec->out_station_index >= m_nDispDistanceNum)
		{
			return -1;
		}
		else
		{
			i = rec->out_station_index;
		}
	}
	else
	if(ENTRY_POSITION == usFlag)
	{
		if(0>rec->in_station_index || rec->in_station_index >= m_nDispDistanceNum)
		{
			return -1;
		}
		else
		{
			i = rec->in_station_index;
		}
	}
	else
	{
		return -1;
	}
	
	if(i<0||i>=m_nDispDistanceNum)
		return -1;

	if(m_sDispDistance[i].show_sation_sep)
	{
		int first_side = m_sDispDistance[i].side[0].side_number;
		int last_side = m_sDispDistance[i].side[m_sDispDistance[i].number_of_side-1].side_number;
		switch(usSideFlag)
		{
		case TOP_TRACK://first side 
			return m_sDispDistance[i].nStationYPos;
			break;
		case PRIMARY_TRACK://prime side
			{
				for(int k = 0;k < m_sDispDistance[i].number_of_side;k++)
				{
					if(rec->arrive_side == m_sDispDistance[i].side[k].side_number )
						break;
				}
				if(k >= m_sDispDistance[i].number_of_side)
					return m_sDispDistance[i].nStationYPos;
				
				return m_sDispDistance[i].nSideYPos[rec->arrive_side];
				break;
			}
		case BOTTOM_TRACK://last side 
			return m_sDispDistance[i].nSecondStationYPos;
			break;
		default:
			return m_sDispDistance[i].nStationYPos;
			break;
		} 				
	}
	else
	{
		return m_sDispDistance[i].nStationYPos;
	}
}

void CTGDoc::InitEarlyLastTime(CTG_TRAIN* single)
{
	if(single == NULL)  return;

	TgTRAIN_RECORD *rec = NULL;
	int nRecNum = single->GetRecordNum();  

	for(int i = 0; i< nRecNum; i++)
	{		
		rec = single->GetRecordByPosition(i);
		if(rec==NULL) continue;

		//st_flag bit0:ͳ�Ƶ��� bit1:ͳ�Ʒ��� bit2:�˹�����
		CString trainid=rec->arrive_train_id;
		if(trainid=="")
			trainid=rec->depart_train_id;
		BYTE st_flag=GetEarlyLateTimeRd(single->m_nTrainIndex, rec->station, rec->rec_index,trainid);
		if(st_flag>0)
		{
			if((st_flag &0x1)==0x01)
				rec->static_arrive=1;
			else
				rec->static_arrive=0;
			
			if((st_flag &0x2)==0x02)
				rec->static_depart=1;
			else
				rec->static_depart=0;
			
			continue;
		}

		if(rec->static_manual==1)
			continue;
	
		int node_index = 0;
		if(i==0) 
		{
			if(IsFlagStart(rec->flag))
				node_index = rec->out_station_index;
			else
				node_index = rec->in_station_index;
		}
		else if(i==nRecNum-1)
		{
			if(IsFlagTerminal(rec->flag))
				node_index = rec->in_station_index;
			else
				node_index = rec->out_station_index;
		}
		else
		{
			node_index = rec->in_station_index;
			if(node_index == -1)
				node_index = rec->out_station_index;
		}

		const node_info *pNodeInfo = this->m_ptgctrl->mConfigTg.m_tgdef.GetNodeByIndex(m_nCurrentSectionNo, node_index);

		// �ж��Ƿ����ڱ���ֹ��ʾ���г�����
		if(pNodeInfo == NULL)
		{
			rec->static_arrive=0;
			rec->static_depart=0;
			continue;
		}

		// �˳�վ������ʾͳ��
		if(pNodeInfo->show_statistic)
		{
			int nSel = 3;

			// ����
			if(single->m_nDirectFlag == UP_DIRECT)
			{
				// �ͳ�
				if(single->m_nTrainType == TYPE::PASSENGER)
					nSel = pNodeInfo->up_person_train_flag;
				else
					nSel = pNodeInfo->up_goods_train_flag;
			}

			// ����
			if(single->m_nDirectFlag == DOWN_DIRECT)
			{
				// �ͳ�
				if(single->m_nTrainType == TYPE::PASSENGER)
					nSel = pNodeInfo->down_person_train_flag;
				else
					nSel = pNodeInfo->down_goods_train_flag;
			}

			if(nSel == 0)
			{
				rec->static_arrive=1;
				rec->static_depart=0;
			}
			else if(nSel == 1)
			{
				rec->static_arrive=0;
				rec->static_depart=1;
			}
			else if(nSel == 2)
			{
				rec->static_arrive=1;
				rec->static_depart=1;
			}
			else if(nSel == 4)
			{
				if(rec->arrive == rec->depart)
				{
					rec->static_arrive=0;
					rec->static_depart=1;
				}
				else
				{
					rec->static_arrive=1;
					rec->static_depart=0;
				}
			}
			else
			{
				rec->static_arrive=0;
				rec->static_depart=0;
			}
			
			// ͨ����ͳ�ƶ���
			if(!this->m_ptgctrl->mConfigTg.m_context.IsStaticsTimeCompleteByTGDEF())
			{
				if(rec->arrive == rec->depart && nSel == 2)
				{
					if(rec->plan_arrive != rec->plan_depart) 
					{
						if(this->m_ptgctrl->mConfigTg.m_context.IsTrainStayRealPassStatisticDepart())
						{
							// �ͳ�  ʵ��ͨ�� ͼ������ ͳ�Ʒ���
							if(single->m_nTrainType == TYPE::PASSENGER)					
							{
								rec->static_arrive=0;
								rec->static_depart=1;
							}
						}
					}
					else
					{
						rec->static_arrive=0;
						rec->static_depart=1;
					}
				}
			}
		}

		
		// ʼ�� �յ� ���
		if((i==0) && IsFlagStart(rec->flag)) 
		{
			rec->static_arrive=0;
			rec->static_depart=1;
		}
		if((i==nRecNum-1) && IsFlagTerminal(rec->flag))
		{
			rec->static_arrive=1;
			rec->static_depart=0;
		}
	}
}

void CTGDoc::SetSchdBeginTime(void) 
{
	if(TRUE==m_nAutoMove)
	{
		CTime curtm=CTime::GetCurrentTime();
		CTimeSpan span(0,m_nHistoryLen,0,0);
		curtm -= span;
		m_TGGraphStartTime = curtm.GetTime();
		m_TGGraphStartTime = m_TGGraphStartTime-m_TGGraphStartTime%60;
	}
}

void CTGDoc::SetGraphStartTime_New(ULONG nShiftId)
{
	m_ptgctrl->GetShiftID(m_nBeginTm,m_nAutoMove,m_nHistoryLen);
	if(GetTgType() == TG_TYPE::BASIC_TG)
	{
		int nHour = m_ptgctrl->GetBeginHour();
		m_TGGraphStartTime = nHour*60*60;
	}
	else
	{
		CTime tm(m_nBeginTm);
		m_ptgctrl->SetBeginHour(tm.GetHour());
		m_TGGraphStartTime = m_nBeginTm;
	}
}

void CTGDoc::SetGraphStartTime(ULONG nShiftId)
{
	int nYear, nMonth, nDay;
	int nShiftNo = nShiftId - nShiftId/100*100;

	CString str,tmp;
   	
	str.Format("%ld", nShiftId);
	tmp = str.Left(4);
	nYear = atoi(tmp);
	str = str.Right(str.GetLength() - 4);
	tmp = str.Left(2);
	nMonth = atoi(tmp);
	str = str.Right(str.GetLength() - 2);
	tmp = str.Left(2);
	nDay = atoi(tmp);
	
	if(GetTgType() == TG_TYPE::BASIC_TG)
	{
		int nHour = m_ptgctrl->GetBeginHour();
		m_TGGraphStartTime = nHour*60*60;
	}
	else
	{
		int nHour;
		if(nShiftNo == 2)
			nHour = 18;
		else
			nHour = 06;

		m_ptgctrl->SetBeginHour(nHour);
		CTimeSpan span(1,0,0,0);
		CTime tm(nYear, nMonth, nDay, nHour, 0, 0);
		tm -= span;
		m_TGGraphStartTime = tm.GetTime();
	}
}

// convert screen coordinate to time
long CTGDoc::ConvertXPostoTime(int xPos)
{
	xPos -= X_TIME_OFFSET;
	int timevalue = m_TGGraphStartTime + (xPos*60) / m_fPixelPerMinute; 
	if (mMouseMoveMode==0)
		timevalue = (timevalue/60)*60;

	return timevalue;
}

long  CTGDoc::GetXPositionConsiderSecond(long nTime)
{
	int nMinuteOffset = GetOffsetMinuteFromGraphBegin(nTime);
	int nSecondOffset = nTime%60;

	return (nMinuteOffset * m_fPixelPerMinute)+(nSecondOffset*m_fPixelPerMinute)/60;
}

long  CTGDoc::GetOffsetPixelFromGraphBegin(long nTime)
{
	int nMinuteOffset = GetOffsetMinuteFromGraphBegin(nTime);
	int nSecondOffset = nTime%60;

	return (nMinuteOffset * m_fPixelPerMinute)+(nSecondOffset*m_fPixelPerMinute)/60;
}

long  CTGDoc::GetOffsetMinuteFromGraphBegin(long nTime)
{
	int width = 0;
	
	if(GetTgType() == TG_TYPE::BASIC_TG)
	{
		if(nTime >= m_TGGraphStartTime)
			width = (nTime- m_TGGraphStartTime)%86400;
		else
			width = nTime + 86400 - m_TGGraphStartTime;
	}
	else
	{
		width = nTime - m_TGGraphStartTime;
	}

	
	return width/60;
}

CFont* CTGDoc::GetDrawTrainNoFont(const TgTRAIN_MORE_INFO& nTrainInfo, CString& strTrainId, bool& bDrawRect)
{
	CFont* pSelFont=NULL;
	bDrawRect = false;

	if(nTrainInfo.b_elec_rank)
	{
		strTrainId +=trainid_style.elec_rank.tail;

		pSelFont=&fontElecTrain;
		bDrawRect = trainid_style.elec_rank.drawrect;
	}
	if(nTrainInfo.space_rank >=1 )//���޵ȼ�
	{
		strTrainId +=trainid_style.space_rank.tail;

		pSelFont=&fontSpaceTrain;
		bDrawRect = trainid_style.space_rank.drawrect;
	}
	if(nTrainInfo.length_rank >=1) //�����ȼ�
	{
		strTrainId +=trainid_style.length_rank.tail;
		
		pSelFont=&fontLengthTrain;
		bDrawRect = trainid_style.length_rank.drawrect;
	}
	if(nTrainInfo.military_rank >=1)   //���˵ȼ�
	{
		strTrainId +=trainid_style.military_rank.tail;
		
		pSelFont=&fontMilitaryTrain;
		bDrawRect = trainid_style.military_rank.drawrect;
	}
	if(nTrainInfo.super_rank >=1)   //ר�˵ȼ����ص��г���
	{
		strTrainId +=trainid_style.super_rank.tail;

		pSelFont=&fontSuperTrain;
		bDrawRect = trainid_style.super_rank.drawrect;
	}

	return pSelFont;
}

void CTGDoc::DrawMarkOfStart(CDC *pCDC,CTG_TRAIN * single,CString strTrainId, CPoint ptlStart, int nDirect, COLORREF line_color,int  line_width, TgTRAIN_RECORD* prec)
{
	//������������С
	UINT nOldAlign;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(line_color);
	
	if(IsNewNextTrainReturn(single->myTrainInfo.side_utility))
	{
		if(IsNextTrainOutReturn(single->myTrainInfo.side_utility))
		{
			line_color=RGB(164,164,164);
		}
		else
		{
			line_color=RGB(0,0,0);
		}
	}
	
	bool bDrawRect = false;
	CFont * pOldFont = GetDrawTrainNoFont(single->myTrainInfo, strTrainId, bDrawRect);
	if(NULL != pOldFont)
		pOldFont = pCDC->SelectObject(pOldFont);
	else
		pOldFont = pCDC->SelectObject(&fontNomorlTrain);

	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;
	

	//���ƻ����ţ����¶���ʼ�����ĳ��ȣ�ռVERTICAL_SPACE��һ��
	//Ϊ�˱�������С�������·�������־�ص�����ȥ��������߶�
	CSize trainidSize = pCDC->GetTextExtent(strTrainId);
	
	int nLength = trainidSize.cy;

	//get the height of start mark
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - nLength;
		uStartX = ptlStart.x -  trainidSize.cx/2;
		uEndX = ptlStart.x + trainidSize.cx/2;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y;
	} 
	else 
	{
		ptlEnd.y = ptlStart.y + nLength;
		ptlEnd.x = ptlStart.x;
		uStartX = ptlStart.x - trainidSize.cx/2;
		uEndX = ptlStart.x + trainidSize.cx/2;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y;
	}

	int i=0;
	int stationno=ConvertYPostoStationNo(ptlStart.y,i);
	CPoint pOrg(uStartX,uStartY);
	int increaeelayer = GetIncreaseLayer(single,pOrg,nDirect,stationno,trainidSize);
	if(increaeelayer>m_nTrainNoMaxLevel)
		increaeelayer=1;

	int iDelta = trainidSize.cy*(increaeelayer-1);

	if(nDirect == UP_MARK)//�ϱ�ע
	{ 
		uStartY += -iDelta;
		uEndY += -iDelta;  
		ptlEnd.y += -iDelta;
	}
	else
	{
		uStartY += iDelta;
		uEndY += iDelta;  
		ptlEnd.y += iDelta;
	}

	// ���㳵�η���
	CRect m_rect;	
	if(1)
	{
		CPoint cPoint; CSize rectSize;
		cPoint.x = uStartX-2 ;
		rectSize = trainidSize;
		rectSize.cx +=2;

		if(nDirect == UP_MARK)
		{
			cPoint.y = uStartY  - rectSize.cy ;
		}
		else
		{ 
			cPoint.y = uStartY +2 ;

		}
		rectSize.cy -=2;

		m_rect = CRect(cPoint,rectSize);

		single->AddShowTrainIDRect(m_rect);
	}

	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
	ShowRect(pCDC, m_rect);
	#endif
	single->UnionRect(m_rect);
	}

	// select pen/brush;
	if(line_width == 0)
		line_width = 1;

	CPen mPen,*pOldPen;
	mPen.CreatePen(PS_INSIDEFRAME,line_width,line_color);   
	pOldPen = pCDC->SelectObject(&mPen);

	if(single->myTrainInfo.b_elec_rank && m_nShowElecLocoFlag)// && bDrawRect)
	{
		pCDC->MoveTo(m_rect.TopLeft());
		pCDC->LineTo(m_rect.BottomRight().x, m_rect.TopLeft().y);
		pCDC->LineTo(m_rect.BottomRight());
		pCDC->LineTo(m_rect.TopLeft().x, m_rect.BottomRight().y );
		pCDC->LineTo(m_rect.TopLeft() );
	}

	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);

	if(nDirect == UP_MARK)
	{
		pCDC->MoveTo(uStartX,uEndY);
		pCDC->LineTo(uEndX ,uEndY);

		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	}
	else
	{
		pCDC->MoveTo(uStartX,uStartY);
		pCDC->LineTo(uEndX,uStartY);

		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	}

	pCDC->SetTextColor(line_color);

	pCDC->TextOut(uStartX, uStartY, strTrainId);//m_strText

	//���ƻ����� G1
	if(this->m_ptgctrl->mConfigTg.m_context.IsDrawLanzhouEngine())
		DrawLanzhouMarkOfTractionEngine(pCDC,single,nDirect);
	else
		DrawMarkOfTractionEngine(pCDC,CPoint(uStartX+trainidSize.cx,uStartY),trainidSize,single,nDirect);

	pCDC->SelectObject(pOldFont);   
	// remove pen / brush
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
            
	mPen.DeleteObject();
}

void CTGDoc::DrawMarkOfArrive(CDC *pCDC,CTG_TRAIN * single,CString strTrainId, CPoint ptlStart, int nDirect, COLORREF line_color, int line_width, TgTRAIN_RECORD* rec)
{
	if(ptlStart.y <= 0)
		return;

	//�������壬����
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);	
	UINT nOldColor = pCDC->SetTextColor(line_color);
	UINT nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);

	bool bDrawRect = false;
	CFont * pOldFont = GetDrawTrainNoFont(single->myTrainInfo, strTrainId, bDrawRect);
	if(NULL != pOldFont)
		pOldFont = pCDC->SelectObject(pOldFont);
	else
		pOldFont = pCDC->SelectObject(&fontNomorlTrain);

	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	//���ƻ����ţ����¶���ʼ�����ĳ��ȣ�ռVERTICAL_SPACE��һ��
	//Ϊ�˱�������С�������·�������־�ص���Ϊ�˱����ʼ����Ǹ߶�һ�£���ȥ��������߶�
	//ʼ����־��ȥ����2������߶� 
	CSize trainidSize = pCDC->GetTextExtent(strTrainId);

	int nLength = trainidSize.cy;

	//get the height of start mark
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x-nLength/2;
		ptlEnd.y = ptlStart.y - nLength;
		uStartX = ptlEnd.x - trainidSize.cx; //strTrainId.GetLength()*8;// the text width is 20,50
		uEndX = ptlEnd.x;
		uStartY = ptlEnd.y;// - 4*HEIGHT_OFFSET;// the text height is 20
		uEndY = ptlEnd.y;
	} 
	else 
	{
		ptlEnd.x = ptlStart.x - nLength/2;
		ptlEnd.y = ptlStart.y + nLength;		
		uStartX = ptlEnd.x - trainidSize.cx;//strTrainId.GetLength()*8;
		uEndX = ptlEnd.x;
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y;//  - 2*HEIGHT_OFFSET;
	}


	int i=0;
	int stationno=ConvertYPostoStationNo(ptlStart.y,i);

	CPoint ptOrg(ptlEnd.x - trainidSize.cx,ptlEnd.y);
	int	increaeelayer = GetIncreaseLayer(single, ptOrg,nDirect,stationno, trainidSize);
	if(increaeelayer> m_nTrainNoMaxLevel)
		increaeelayer=1;

	int iDelta = trainidSize.cy*(increaeelayer-1);

	if(nDirect == UP_MARK)//�ϱ�ע
	{   
		ptlEnd.y += -iDelta;
	}
	else  //�±�ע
	{		
		ptlEnd.y += iDelta;
	}

	CRect m_rect;
	if(1)
	{
		//���Ƶ��������ķ���
		CPoint cPoint; CSize rectSize;
		cPoint.x = uStartX-2 ;
		rectSize = trainidSize;
		rectSize.cx +=2;

		if(nDirect == UP_MARK)
		{
			cPoint.y = ptlEnd.y  - rectSize.cy ; 
		}
		else
		{ 
			cPoint.y = ptlEnd.y +2 ;  

		}
		rectSize.cy -=2;

		m_rect = CRect(cPoint,rectSize);
		single->AddShowTrainIDRect(m_rect);
	}

	if(m_bIsVerifyTrainRect)
	{
	CRect rect=m_rect;
	#ifdef DRAW_TRAIN_RECT	
		rect.left=rect.left-5;
		ShowRect(pCDC, rect);
	#endif
	single->UnionRect(rect);
	}

	// select pen/brush;
	if(line_width == 0)
		line_width = 1;

	CPen mPen,*pOldPen;
	mPen.CreatePen(PS_INSIDEFRAME,line_width,line_color);
	pOldPen = pCDC->SelectObject(&mPen);

	if(single->myTrainInfo.b_elec_rank && m_nShowElecLocoFlag)// && bDrawRect)
	{
		pCDC->MoveTo(m_rect.TopLeft());
		pCDC->LineTo(m_rect.BottomRight().x, m_rect.TopLeft().y);
		pCDC->LineTo(m_rect.BottomRight());
		pCDC->LineTo(m_rect.TopLeft().x, m_rect.BottomRight().y );
		pCDC->LineTo(m_rect.TopLeft() );
	}

	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);
	
	TgTRAIN_RECORD *first=NULL,*second=NULL;
	int rec_count = single->GetRecordNum();
	first = single->GetRecordByPosition(0);
	if(first ==  NULL) return;
	/////////////////////////////////////////////////
	int flag = this->m_ptgctrl->mConfigTg.m_context.GetDrawRouteOkFlag();
	if(flag == 3 && m_bShowRouteTrigerOkFlag && rec)	
	{
		bool bArriveReal=(rec->adjust_status & 0x0001)>0?true:false;
		if(IsArriveRouteOk(*rec) && bArriveReal==false) 
		{
			CPen* pOldPen, pen;
			pen.CreatePen(PS_SOLID,2, mjcolor);
			pOldPen = pCDC->SelectObject(&pen);
			
			if(nDirect == UP_MARK)
			{
				pCDC->MoveTo(ptlStart.x-3,ptlStart.y);
				pCDC->LineTo(ptlEnd.x-3,ptlEnd.y);
			}
			else
			{
				pCDC->MoveTo(ptlStart.x+3,ptlStart.y);
				pCDC->LineTo(ptlEnd.x+3,ptlEnd.y);
			}

			pCDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}

	if(rec_count >1)
		second = single->GetRecordByPosition(1);

	if(nDirect == UP_MARK)
	{
		pCDC->MoveTo(ptlEnd.x,ptlEnd.y);
		pCDC->LineTo(ptlEnd.x - trainidSize.cx, ptlEnd.y);

		pCDC->MoveTo(ptlEnd.x - trainidSize.cx, ptlEnd.y);
		pCDC->LineTo(ptlEnd.x -trainidSize.cx- 7, ptlEnd.y - 7); 	 

		pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	}
	else
	{
		pCDC->MoveTo(ptlEnd.x,ptlEnd.y);
		pCDC->LineTo(ptlEnd.x - trainidSize.cx ,ptlEnd.y );

		pCDC->MoveTo(ptlEnd.x - trainidSize.cx ,ptlEnd.y);
		pCDC->LineTo(ptlEnd.x -trainidSize.cx- 7 ,ptlEnd.y + 7);  

		pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	}

	pCDC->TextOut(ptlEnd.x - trainidSize.cx,ptlEnd.y,strTrainId);

	//���ƻ����� G1
	if(this->m_ptgctrl->mConfigTg.m_context.IsDrawLanzhouEngine())
		DrawLanzhouMarkOfTractionEngine(pCDC, single,nDirect);
	else
		DrawMarkOfTractionEngine(pCDC,CPoint(ptlEnd.x,ptlEnd.y),trainidSize,single,nDirect);

	// ����ֽ�ڱ�Ǯ��
	if(nDirect == UP_MARK)
	{
		CRect tempRect(CPoint(ptlEnd.x - trainidSize.cx - 8 -12,ptlEnd.y - 13 - 24),CSize(24,24));
		DrawTrainBenqianTime(pCDC, single, first, ptlEnd, tempRect);
	}
	else
	{
		CRect tempRect(CPoint(ptlEnd.x - trainidSize.cx - 8 -12,ptlEnd.y + 13),CSize(24,24));
		DrawTrainBenqianTime(pCDC, single, first, ptlEnd, tempRect);
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	mPen.DeleteObject();
}

void CTGDoc::DrawMarkOfDepart(CDC *pCDC,CTG_TRAIN * single, CPoint ptlStart, int nDirect, COLORREF line_color,int line_width, const TgTRAIN_RECORD* rec)
{
	if(ptlStart.y <= 0)
		return;

	CPen mPen,*pOldPen;
	POINT ptl[4];
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);

	mPen.CreatePen(PS_INSIDEFRAME,line_width,line_color);

	pOldPen = pCDC->SelectObject(&mPen);
	if(nDirect == UP_MARK)
	{
		ptl[0].x = ptlStart.x;
		ptl[0].y = ptlStart.y;
		ptl[1].x = ptl[0].x;
		ptl[1].y = ptl[0].y - 6;
		ptl[2].x = ptl[1].x + 10;
		ptl[2].y = ptl[1].y;
		ptl[3].x = ptl[2].x - 3;
		ptl[3].y = ptl[2].y - 5;
	} 
	else 
	{
		ptl[0].x = ptlStart.x;
		ptl[0].y = ptlStart.y;
		ptl[1].x = ptl[0].x;
		ptl[1].y = ptl[0].y + 6;
		ptl[2].x = ptl[1].x + 10;
		ptl[2].y = ptl[1].y;
		ptl[3].x = ptl[2].x - 3;
		ptl[3].y = ptl[2].y + 5;
	}
	pCDC->Polyline(ptl,4);

	if(m_bIsVerifyTrainRect)
	{
	CRect rect;
	if(ptl[0].y>ptl[3].y)
		rect=CRect(ptl[0].x,ptl[3].y, ptl[2].x, ptl[0].y);
	else
		rect=CRect(ptl[0].x,ptl[0].y, ptl[2].x, ptl[3].y);
    #ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, rect);
	#endif
	single->UnionRect(rect);
	}

	int flag = this->m_ptgctrl->mConfigTg.m_context.GetDrawRouteOkFlag();
	if(flag == 3 && m_bShowRouteTrigerOkFlag && rec)	
	{
		bool bDepartReal=(rec->adjust_status & 0x0002)>0?true:false;
		if(IsDepartRouteOk(*rec) && bDepartReal==false) 
		{
			CPen* pOldPen, pen;
			pen.CreatePen(PS_SOLID,2, mfcolor);
			pOldPen = pCDC->SelectObject(&pen);
			
			if(nDirect == UP_MARK)
			{
				ptl[0].x = ptlStart.x-2;
				ptl[0].y = ptlStart.y;
				ptl[1].x = ptl[0].x;
				ptl[1].y = ptl[0].y - 6;
				pCDC->MoveTo(ptl[0]);
				pCDC->LineTo(ptl[1]);
			}
			else
			{
				ptl[0].x = ptlStart.x+2;
				ptl[0].y = ptlStart.y;
				ptl[1].x = ptl[0].x;
				ptl[1].y = ptl[0].y + 6;
				pCDC->MoveTo(ptl[0]);
				pCDC->LineTo(ptl[1]);
			}

			pCDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
	
	// remove pen / brush
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	BOOL ret = mPen.DeleteObject();
}


void CTGDoc::DrawMarkOfZheFanTrainID(CDC *pCDC, CPoint ptlStart, int nDirect,COLORREF color, CString& trainid,CTG_TRAIN * single)

{
	if(ptlStart.y <= 0 || trainid.IsEmpty())
		return;

	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT oldAlign=pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	UINT oldColor=pCDC->SetTextColor(color);
	CFont *pOldFont = pCDC->SelectObject(&fontReturnTrain);
	CSize sz = pCDC->GetTextExtent(trainid);
	if(nDirect == UP_MARK)
	{
		pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pCDC->TextOut(ptlStart.x+5, ptlStart.y-4, trainid);

		if(m_bIsVerifyTrainRect)
		{
		CRect rect;
		rect=CRect(ptlStart.x+5, ptlStart.y-4-sz.cy, ptlStart.x+5+sz.cx, ptlStart.y-4);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}
	}
	else 
	{
		pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlStart.x+5, ptlStart.y+4, trainid);

		if(m_bIsVerifyTrainRect)
		{
		CRect rect;
		rect=CRect(ptlStart.x+5, ptlStart.y+4, ptlStart.x+5+sz.cx, ptlStart.y+4+sz.cy);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}
	}

	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
    pCDC->SetTextAlign(oldAlign);
	pCDC->SetTextColor(oldColor);
	pCDC->SelectObject(pOldFont);
	return;
}

void CTGDoc::DrawMarkOfEnd(CDC *pCDC,CTG_TRAIN * single, CPoint ptlStart, int nDirect,COLORREF line_color, USHORT flag, int line_width)
{
	if(ptlStart.y <= 0)
		return;
	
	if(IsNewReturn(single->myTrainInfo.side_utility))
	{
		if(IsStationOutReturn(single->myTrainInfo.side_utility))
		{
			line_color=RGB(164,164,164);
		}
		else
		{
			line_color=RGB(0,0,0);
		}
	}
	
	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	int  high_offset = 10;   ////////////////changed ;
	int nLength=10; ///////////////
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - nLength;
		uStartX = ptlStart.x - 6;   //10
		uEndX = ptlStart.x + 6;     //10
		uStartY = ptlEnd.y - high_offset;
		uEndY = ptlEnd.y;
	} else {
		ptlEnd.y = ptlStart.y + nLength;
		ptlEnd.x = ptlStart.x;
		uStartX = ptlStart.x - 6;  //10
		uEndX = ptlStart.x + 6;    //10
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y + high_offset;
	}

	int Flag=0; 
	////////////////////////////////////////////////////////////////////////////////
	CRect RectTemp(uStartX-3, uStartY,  uEndX+3, uEndY);
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); it++)
	{
		if(it->second.GetTrainIndex() == single->GetTrainIndex() || m_nEraseTrainIndex==it->second.GetTrainIndex())
			continue;

		if(it->second.IsInTrainTerminalRect(RectTemp))
		{
			Flag=1;
			break;
		}
		if( Flag==1)
			break;
	}

	//have found collide rect ;
	if(it != myPDataManage->myTrain.end())
	{ 	
		if(nDirect == UP_MARK)//�ϱ�ע
		{ 
			uStartY += -15;
			uEndY += -15;  
			ptlEnd.y += -15;
		}
		else  //�±�ע
		{
			uStartY += 15;
			uEndY +=15;  
			ptlEnd.y += 15;
		}
	}
	single->AddTrainTerminalRect(uStartX, uStartY, uEndX, uEndY);
	
	POINT ptl[4];
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);

	// select pen/brush;
	// color
	CPen mPen,*pOldPen;
	mPen.CreatePen(PS_INSIDEFRAME,line_width,line_color);

	pOldPen = pCDC->SelectObject(&mPen);

	//��������
	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);

	if(nDirect == UP_MARK)
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uEndY;
		ptl[1].x = uEndX;
		ptl[1].y = uEndY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uStartY;
	} 
	else 
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uStartY;
		ptl[1].x = uEndX;
		ptl[1].y = uStartY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uEndY;
	}

	CPoint topleft,botright;
	CRect tempRect;
	if(nDirect == UP_MARK)
	{
		topleft.x = ptlEnd.x - 8;
		topleft.y = ptlEnd.y - 16;

		botright.x = ptlEnd.x + 8;
		botright.y = ptlEnd.y;			
	}
	else
	{
		topleft.x = ptlEnd.x - 8;
		topleft.y = ptlEnd.y;

		botright.x = ptlEnd.x + 8;
		botright.y = ptlEnd.y + 16;
	}
	tempRect.SetRect(topleft,botright);

	if(IsFlagEnd(flag) || IsFlagRescued(flag))//��ʱͣ��
	{
		CPoint start;
		start.x = (tempRect.left + tempRect.right) /2;
		start.y = tempRect.top;
		pCDC->Arc(tempRect,start,start);
	}
	else
	{
		pCDC->Polyline(ptl,4);
	}

	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, tempRect);
	#endif
	single->UnionRect(tempRect);
	}
	// remove pen / brush
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	BOOL ret = mPen.DeleteObject();
}

void CTGDoc::DrawMarkOfEndLittle(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect, COLORREF line_color, USHORT flag, int line_width)
{
	if(ptlStart.y <= 0)
		return;
	ptlStart.x+=3;
	int uStartX,uStartY,uEndX,uEndY;
	CPoint ptlEnd;

	int  high_offset = 5;   ////////////////changed ;
	int nLength=5; ///////////////
	if(nDirect == UP_MARK)
	{
		ptlEnd.x = ptlStart.x;
		ptlEnd.y = ptlStart.y - nLength;
		uStartX = ptlStart.x - 4;   //10
		uEndX = ptlStart.x + 4;     //10
		uStartY = ptlEnd.y - high_offset;
		uEndY = ptlEnd.y;
	} else {
		ptlEnd.y = ptlStart.y + nLength;
		ptlEnd.x = ptlStart.x;
		uStartX = ptlStart.x - 4;  //10
		uEndX = ptlStart.x + 4;    //10
		uStartY = ptlEnd.y;
		uEndY = ptlEnd.y + high_offset;
	}

	int Flag=0; 
	////////////////////////////////////////////////////////////////////////////////
	CRect RectTemp(uStartX-3, uStartY,  uEndX+3, uEndY);
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); it++)
	{
		if(it->second.GetTrainIndex() == single->GetTrainIndex() || m_nEraseTrainIndex==it->second.GetTrainIndex())
			continue;

		if(it->second.IsInTrainTerminalRect(RectTemp))
		{
			Flag=1;
			break;
		}

		if(Flag==1)
		{
			break;
		}
	}

	//have found collide rect ;
	if(it != myPDataManage->myTrain.end())
	{ 	
		if(nDirect == UP_MARK)//�ϱ�ע
		{ 
			uStartY += -15;
			uEndY += -15;  
			ptlEnd.y += -15;
		}
		else  //�±�ע
		{
			uStartY += 15;
			uEndY +=15;  
			ptlEnd.y += 15;
		}
	}

	single->AddTrainTerminalRect(uStartX, uStartY, uEndX, uEndY);

	CPen mPen,*pOldPen;
	POINT ptl[4];
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);

	// select pen/brush;
	mPen.CreatePen(PS_INSIDEFRAME,line_width,line_color);

	pOldPen = pCDC->SelectObject(&mPen);

	//��������
	pCDC->MoveTo(ptlStart.x,ptlStart.y);
	pCDC->LineTo(ptlEnd.x,ptlEnd.y);

	if(nDirect == UP_MARK)
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uEndY;
		ptl[1].x = uEndX;
		ptl[1].y = uEndY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uStartY;
	} 
	else 
	{
		ptl[3].x = ptl[0].x = uStartX;
		ptl[3].y = ptl[0].y = uStartY;
		ptl[1].x = uEndX;
		ptl[1].y = uStartY;
		ptl[2].x = ptlEnd.x;
		ptl[2].y = uEndY;
	}

	CPoint topleft,botright;
	CRect tempRect;
	if(nDirect == UP_MARK)
	{
		topleft.x = ptlEnd.x - 8;
		topleft.y = ptlEnd.y - 16;

		botright.x = ptlEnd.x + 8;
		botright.y = ptlEnd.y;			
	}
	else
	{
		topleft.x = ptlEnd.x - 8;
		topleft.y = ptlEnd.y;

		botright.x = ptlEnd.x + 8;
		botright.y = ptlEnd.y + 16;
	}
	tempRect.SetRect(topleft,botright);

	if(IsFlagEnd(flag) || IsFlagRescued(flag))//��ʱͣ��
	{
		CPoint start;
		start.x = (tempRect.left + tempRect.right) /2;
		start.y = tempRect.top;
		pCDC->Arc(tempRect,start,start);
	}
	else
	{
		pCDC->Polyline(ptl,4);
	}

	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, tempRect);
	#endif
	single->UnionRect(tempRect);
	}
	// remove pen / brush
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	BOOL ret = mPen.DeleteObject();
}

//�ж�����NODE�Ƿ����������������ֱ����������ô�������б����ж��Ƿ���
//��pre_node_index��ͬ�ĳ�վ�����cur_node_index����������У���ô int���ظó�վ��nodeindex
//�����ж�����ϵ�nodeindex�����ص�һ���ҵ���nodeindex
BOOL CTGDoc::JudgeNodesConnectedThroughThirdParty(WORD cur_node_index,WORD pre_node_index,int &replace_node_index)
{
	replace_node_index = -1;

	if(cur_node_index >= m_nDispDistanceNum || pre_node_index>= m_nDispDistanceNum)
		return FALSE;

	if(m_sDispDistance[cur_node_index].station_number == m_sDispDistance[pre_node_index].station_number)
	{
		if(cur_node_index == pre_node_index)
		{
			replace_node_index = pre_node_index;
		}
		else
		{
			replace_node_index = cur_node_index;
		}
		return TRUE;
	}
    
	// ֱ����ͨ
	if(JudeNodesConnected(cur_node_index, pre_node_index))
	{
		replace_node_index = pre_node_index;
		return TRUE;
	}
   
	// �����ͨ
	int nUpCount=0, upMaybeIndex[64];
	int nDownCount=0, downMaybeIndex[64];

	if(cur_node_index < pre_node_index)
	{
		for(int i=cur_node_index-1; i >= 0; i--)
		{
			if(m_sDispDistance[i].station_number == m_sDispDistance[pre_node_index].station_number &&
			   m_sDispDistance[i].sub_section ==  m_sDispDistance[cur_node_index].sub_section)
			{
				upMaybeIndex[nUpCount] = i;
				nUpCount++;
			}
		}

		for(int i=cur_node_index+1; i < pre_node_index; i++)
		{
			if(m_sDispDistance[i].station_number == m_sDispDistance[pre_node_index].station_number &&
			   m_sDispDistance[i].sub_section ==  m_sDispDistance[cur_node_index].sub_section)
			{
				downMaybeIndex[nDownCount] = i;
				nDownCount++;
			}
		}
	}
	else
	{
		for(int i=cur_node_index+1; i < this->m_nDispDistanceNum; i++)
		{
			if(m_sDispDistance[i].station_number == m_sDispDistance[pre_node_index].station_number &&
			   m_sDispDistance[i].sub_section ==  m_sDispDistance[cur_node_index].sub_section)
			{
				downMaybeIndex[nDownCount] = i;
				nDownCount++;
			}
		}

		for(int i=cur_node_index-1; i > pre_node_index; i--)
		{
			if(m_sDispDistance[i].station_number == m_sDispDistance[pre_node_index].station_number &&
			   m_sDispDistance[i].sub_section ==  m_sDispDistance[cur_node_index].sub_section)
			{
				upMaybeIndex[nUpCount] = i;
				nUpCount++;
			}
		}
	}

	
	int nCount=0, re_node_index[128];
	for(int i=0; i < nUpCount; i++)
	{
		if(JudeNodesConnected(cur_node_index, upMaybeIndex[i]))
		{
			re_node_index[nCount] = upMaybeIndex[i];
			nCount++;
		}
	}

	for(int i=0; i < nDownCount; i++)
	{
		if(JudeNodesConnected(cur_node_index, downMaybeIndex[i]))
		{
			re_node_index[nCount] = downMaybeIndex[i];
			nCount++;
		}
	}
	
	if(nCount == 0)
	{
		replace_node_index = -1;
		return FALSE;
	}
	else if(nCount == 1)
	{
		replace_node_index = re_node_index[0];
		return TRUE;
	}
	else
	{
		int dist = 9999, dlt = 0;
		for(int i=0; i < nCount; i++)
		{
			dlt = abs(re_node_index[i]-cur_node_index);
			if(dlt < dist)
			{
				dist = dlt;
				replace_node_index = re_node_index[i];
			}
		}
		return TRUE;
	}
}

BOOL CTGDoc::JudeNodesConnected(WORD cur_node_index,WORD pre_node_index)
{
	if(cur_node_index >= m_nDispDistanceNum || pre_node_index>= m_nDispDistanceNum)
		return FALSE;

	int i,j;
	
	if(m_sDispDistance[cur_node_index].station_number == m_sDispDistance[pre_node_index].station_number )
		return TRUE;

	for(i=0;i<m_sDispDistance[cur_node_index].up_line_count;i++)
	{
		for(j= 0;j<m_sDispDistance[pre_node_index].down_line_count ;j++)
		{
			if(m_sDispDistance[cur_node_index].up_line_list [i] == m_sDispDistance[pre_node_index].down_line_list [j])
				return TRUE;
		}
	}

	for(i=0;i<m_sDispDistance[cur_node_index].down_line_count;i++)
	{
		for(j= 0;j<m_sDispDistance[pre_node_index].up_line_count ;j++)
		{
			if(m_sDispDistance[cur_node_index].down_line_list [i] == m_sDispDistance[pre_node_index].up_line_list [j])
				return TRUE;
		}
	}

	return FALSE;	
}

WORD CTGDoc::GetEntryFromNodeIndex(int node_index, int dir, int abs_line[])
{
	if(node_index < 0 || node_index >= this->m_nDispDistanceNum)
		return 0;

	WORD abs_line_num = 0;
	WORD station = m_sDispDistance[node_index].station_number;

	if(dir == DOWN_DIRECT)
	{
		for(int i = 0; i < m_sDispDistance[node_index].up_line_count; i++)
		{
			int line_no = m_sDispDistance[node_index].up_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(line_no);
			if(line == NULL)
				continue;
	
			if(line->nDirect == dir && line->nEndStation == station)
			{
				abs_line[abs_line_num] = line_no;
				abs_line_num++;
			}
			else if((line->nDirect == 3) && (line->nStartStation == station || line->nEndStation == station))
			{
				abs_line[abs_line_num] = line_no;
				abs_line_num++;
			}
		}
	}
	else//UP_DIRECT
	{
		for(int i = 0; i < m_sDispDistance[node_index].down_line_count;i++)
		{
			int line_no = m_sDispDistance[node_index].down_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(line_no);
			if(line == NULL)
				continue;
			
			if(line->nDirect == dir && line->nEndStation == station)
			{
				abs_line[abs_line_num] = line_no;
				abs_line_num++;
			}
			else if((line->nDirect == 3) && (line->nStartStation == station || line->nEndStation == station))
			{
				abs_line[abs_line_num] = line_no;
				abs_line_num++;
			}
		}
	}

	return abs_line_num;
}

void CTGDoc::GetEntryExitFromNodeIndex(int node_index,int& entry,int& exit,int dir)
{
	entry = -1;
	exit = -1;
	
	if(node_index < 0 || node_index >= this->m_nDispDistanceNum)
		return;

	WORD abs_line,station;
	
	if(dir == DOWN_DIRECT)
	{
		for(int i = 0; i < m_sDispDistance[node_index].up_line_count; i++)
		{
			abs_line = m_sDispDistance[node_index].up_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
			if(line == NULL)
				continue;
	
			if(line->nDirect == dir || line->nDirect == 3)
			{
				station = m_sDispDistance[node_index].station_number ;
				entry = ConvertAbsnumStationToEntry(station,abs_line);
				break;
			}
		}
		
		for(int i=0;i< m_sDispDistance[node_index].down_line_count;i++)
		{
			abs_line = m_sDispDistance[node_index].down_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
			if(line == NULL)
				continue;
			
			if(line->nDirect == dir || line->nDirect == 3)
			{
				station = m_sDispDistance[node_index].station_number ;
				exit = ConvertAbsnumStationToEntry(station,abs_line);
				break;
			}
		}
	}
	else
	{//UP_DIRECT
		for(int i = 0; i < m_sDispDistance[node_index].down_line_count;i++)
		{
			abs_line = m_sDispDistance[node_index].down_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
			if(line == NULL)
				continue;
			
			if(line->nDirect == dir || line->nDirect == 3)
			{
				station = m_sDispDistance[node_index].station_number ;
				entry = ConvertAbsnumStationToEntry(station,abs_line);
				break;
			}
		}
		
		for(i=0;i< m_sDispDistance[node_index].up_line_count;i++)
		{
			abs_line = m_sDispDistance[node_index].up_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
			if(line == NULL)
				continue;
			
			if(line->nDirect == dir || line->nDirect == 3)
			{
				station = m_sDispDistance[node_index].station_number ;
				exit = ConvertAbsnumStationToEntry(station,abs_line);
				break;
			}
		}
	}
	return;
}

void CTGDoc::GetExitEntryFromTwoNodeIndex(const int prev_node_index,const int node_index,int& exit,int& entry,const int dir)
{
	int dispNum = m_nDispDistanceNum;

	int prev_station = m_sDispDistance[prev_node_index].station_number ;
	int station = m_sDispDistance[node_index].station_number;

	int nFlag = GenExitEntryByInitABS(m_nCurrentSectionNo,prev_station,station,prev_node_index,node_index,exit,entry);
	if(2 == nFlag)
	{
		return;
	}

	int nAbsLineNo=GetLineNoByStations(prev_station,station);
	if(nAbsLineNo>0)
	{
		exit  = ConvertAbsnumStationToEntry(prev_station, nAbsLineNo);
		entry = ConvertAbsnumStationToEntry(station, nAbsLineNo);
		if(exit>0 && entry>0)
		{
			sysprint(0, 0, "����Խ����,���� tg.def��[STATION_NEXTSTATION_LINENO]����:%d,%d,%d,���ó�վ %d ���� %d ��վ %d ��� %d.",
				prev_station, station, nAbsLineNo, prev_station, exit, station, entry);
			return;
		}
	}

	int small_node,large_node;
	small_node = (prev_node_index<node_index)?prev_node_index:node_index;
	large_node = (prev_node_index<node_index)?node_index:prev_node_index;

	WORD abs_line = 0;
	int i=0,k = 0,j = 0;

	bool bFound = FALSE;
	for(i=0;i<m_sDispDistance[small_node].down_line_count;i++)
	{
		for(j=0;j<m_sDispDistance[large_node].up_line_count ;j++)
		{
			if(m_sDispDistance[small_node].down_line_list [i] == m_sDispDistance[large_node].up_line_list [j])
			{
				const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(m_sDispDistance[small_node].down_line_list [i]);
				if(line)
				{
					if(line->nDirect == dir || line->nDirect & 0x02)
					{
						bFound = TRUE;
						abs_line = line->nLineNo;
					}
				}	
			}
			if(bFound) break;					    
		}
		if(bFound) break;	
	}

	if(!bFound)
	{
		for(i=0;i<m_sDispDistance[small_node].down_line_count;i++)
		{
			for(j=0;j<m_sDispDistance[large_node].up_line_count ;j++)
			{
				if(m_sDispDistance[small_node].down_line_list [i] == m_sDispDistance[large_node].up_line_list [j])
				{
					bFound = TRUE;
					abs_line = m_sDispDistance[small_node].down_line_list [i];
					break;	

				}
				if(bFound) break;					    
			}
			if(bFound) break;	
		}
	}


	if(bFound)
	{
		exit = ConvertAbsnumStationToEntry(prev_station,abs_line);
		entry = ConvertAbsnumStationToEntry(station,abs_line);
	}
}

int CTGDoc::GetEntryExitDirection(CTG_TRAIN * single,TgTRAIN_RECORD* rec,WORD usFlag,int section)
{
	if(single == NULL)
	{
		return -1;
	}

	int node_index;
	bool flag = FALSE;
	int direction = -1;

	do
	{
		if(usFlag == ENTRY_POSITION)
			node_index = rec->in_station_index;
		else
			node_index = rec->out_station_index;

		if(node_index <0 || node_index >= m_nDispDistanceNum) 
			break;
		
		
		WORD line,dir;

		if(usFlag == ENTRY_POSITION)
		{
			if(rec->entry == -1) break;
			if(!ConvertStationEntryToAbsnum(rec->station,
				rec->entry,
				&line,
				&dir)) 
				break;

		}
		else
		{
			if(rec->exit == -1) break;
			if(!ConvertStationEntryToAbsnum(rec->station,
				rec->exit,
				&line,
				&dir))
				break;

		}
		for(int j= 0;j <  m_sDispDistance[node_index].up_line_count;j++)
		{
			if(line ==  m_sDispDistance[node_index].up_line_list[j])
				direction = UP_MARK;                 					 
		}
		//looking for down line
		for(int j= 0;j <  m_sDispDistance[node_index].down_line_count;j++)
		{
			if(line ==  m_sDispDistance[node_index].down_line_list[j])
				direction = DOWN_MARK;                        					 
		}

		flag = TRUE;
	}while(0);

	if(flag && direction != -1 )
	{
		return direction;        
	}
	else
	{
		return -1;
	}
}

#define VALIDE_INDEX(a,b,index)  (index>=a && index<b)
int CTGDoc::GetNodeEntryIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section)
{
	int index = 0;
	
	BYTE nSameStationNum = 0;
	WORD nStationIndex[128];
	for(BYTE idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		if(rec->station == m_sDispDistance[idx].station_number)
		{
			nStationIndex[nSameStationNum] = idx;
			nSameStationNum++;
		}
	}
	if(nSameStationNum <= 0)
		return -1;

	if(1 == nSameStationNum)
		return nStationIndex[0];

	// ���⾶·������
	if(this->GetNodeIndexOfDefine(single, rec, ENTRY_POSITION, section, index))
		return index;
	
	TgTRAIN_RECORD* pre = single->GetRecordByPosition(rec->rec_index-1);
	
	BYTE nSel = 0;
	bool bEntryFound = false;
		
	WORD lineEntry,dirEntry;
	if(!ConvertStationEntryToAbsnum(rec->station, rec->entry, &lineEntry, &dirEntry))
		return nStationIndex[0];
	
	CString trainId = rec->arrive_train_id;
	if(trainId.IsEmpty())
		trainId = rec->depart_train_id;
	int nDirect = this->GetTrainDirectionByTrainId(trainId);

	for(BYTE k = 0; k < nSameStationNum; k++)
	{
		BYTE idx = nStationIndex[k];

		if(UP_DIRECT == nDirect)
		{
			for(BYTE i = 0; i < m_sDispDistance[idx].down_line_count; i++)
			{
				if(lineEntry == m_sDispDistance[idx].down_line_list[i])
				{
					nSel = k;
					
					if(NULL != pre && VALIDE_INDEX(0, m_nDispDistanceNum, pre->out_station_index))
					{
						if(m_sDispDistance[idx].sub_section == m_sDispDistance[pre->out_station_index].sub_section)
						{
							bEntryFound = true;
							break;
						}
					}
					else
					{
						bEntryFound = true;
						break;
					}
				}
			}
		}

		if(DOWN_DIRECT == nDirect)
		{
			for(BYTE i = 0; i < m_sDispDistance[idx].up_line_count; i++)
			{
				if(lineEntry == m_sDispDistance[idx].up_line_list[i])
				{
					nSel = k;

					if(NULL != pre && VALIDE_INDEX(0, m_nDispDistanceNum, pre->out_station_index))
					{
						if(m_sDispDistance[idx].sub_section == m_sDispDistance[pre->out_station_index].sub_section)
						{
							bEntryFound = true;
							break;
						}
					}
					else
					{
						bEntryFound = true;
						break;
					}
				}
			}
		}

		if(bEntryFound)
			break;
	}

	if(!bEntryFound)
	{
		for(BYTE k = 0; k < nSameStationNum; k++)
		{
			BYTE idx = nStationIndex[k];

			if(UP_DIRECT == nDirect)
			{
				for(BYTE i = 0; i < m_sDispDistance[idx].up_line_count; i++)
				{
					if(lineEntry == m_sDispDistance[idx].up_line_list[i])
					{
						nSel = k;
						
						if(NULL != pre && VALIDE_INDEX(0, m_nDispDistanceNum, pre->out_station_index))
						{
							if(m_sDispDistance[idx].sub_section == m_sDispDistance[pre->out_station_index].sub_section)
							{
								bEntryFound = true;
								break;
							}
						}
						else
						{
							bEntryFound = true;
							break;
						}
					}
				}
			}

			if(DOWN_DIRECT == nDirect)
			{
				for(BYTE i = 0; i < m_sDispDistance[idx].down_line_count; i++)
				{
					if(lineEntry == m_sDispDistance[idx].down_line_list[i])
					{
						nSel = k;

						if(NULL != pre && VALIDE_INDEX(0, m_nDispDistanceNum, pre->out_station_index))
						{
							if(m_sDispDistance[idx].sub_section == m_sDispDistance[pre->out_station_index].sub_section)
							{
								bEntryFound = true;
								break;
							}
						}
						else
						{
							bEntryFound = true;
							break;
						}
					}
				}
			}

			if(bEntryFound)
				break;
		}	
	}	
		
	if(bEntryFound)
		return nStationIndex[nSel];
	else
		return nStationIndex[0];
}      

int CTGDoc::GetNodeExitIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section)
{
	int index = 0;
	BYTE nSameStationNum = 0;
	WORD nStationIndex[128];
	for(BYTE idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		if(rec->station == m_sDispDistance[idx].station_number)
		{
			nStationIndex[nSameStationNum] = idx;
			nSameStationNum++;
		}
	}
	if(nSameStationNum <= 0)
		return -1;
	if(1 == nSameStationNum)
		return nStationIndex[0];

	// ���⾶·������
	if(this->GetNodeIndexOfDefine(single, rec, EXIT_POSITION, section, index))
		return index;

	BYTE nSel = 0;
	bool bExitFound = false;

	
	WORD lineExit, dirExit;
	if(!ConvertStationEntryToAbsnum(rec->station, rec->exit, &lineExit, &dirExit))
		return nStationIndex[0];

	CString trainId = rec->depart_train_id;
	if(trainId.IsEmpty())
		trainId = rec->arrive_train_id;
	int nDirect = this->GetTrainDirectionByTrainId(trainId);

	for(BYTE k = 0; k < nSameStationNum; k++)
	{
		BYTE idx = nStationIndex[k];

		if(UP_DIRECT == nDirect)
		{
			for(BYTE i = 0; i < m_sDispDistance[idx].up_line_count; i++)
			{
				if(lineExit == m_sDispDistance[idx].up_line_list[i])
				{
					nSel = k;
					bExitFound = true;
					break;
				}
			}
		}

		if(DOWN_DIRECT == nDirect)
		{
			for(BYTE i = 0; i < m_sDispDistance[idx].down_line_count; i++)
			{
				if(lineExit == m_sDispDistance[idx].down_line_list[i])
				{
					nSel = k;
					bExitFound = true;
					break;
				}
			}
		}

		if(bExitFound)
			break;
	}

	if(!bExitFound)
	{
		for(BYTE k = 0; k < nSameStationNum; k++)
		{
			BYTE idx = nStationIndex[k];

			if(UP_DIRECT == nDirect)
			{
				for(BYTE i = 0; i < m_sDispDistance[idx].down_line_count; i++)
				{
					if(lineExit == m_sDispDistance[idx].down_line_list[i])
					{
						nSel = k;
						bExitFound = true;
						break;
					}
				}
			}

			if(DOWN_DIRECT == nDirect)
			{
				for(BYTE i = 0; i < m_sDispDistance[idx].up_line_count; i++)
				{
					if(lineExit == m_sDispDistance[idx].up_line_list[i])
					{
						nSel = k;
						bExitFound = true;
						break;
					}
				}
			}

			if(bExitFound)
				break;
		}		
	}		
	
	if(bExitFound)
		return nStationIndex[nSel];
	else
		return nStationIndex[0];
}

bool CTGDoc::TwoStationSubSectionSame(int index1, int index2)
{
	if(0<=index1 && index1 < this->m_nDispDistanceNum && 0<=index2 && index2 < this->m_nDispDistanceNum)
	{
		if(m_sDispDistance[index1].sub_section == m_sDispDistance[index2].sub_section)
			return true;
	}
	return false;	
}

int CTGDoc::GetMultiNodeJudgeIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec,int& index,int forwardindex,int reverseindex,WORD usFlag,int section,BYTE bJudgeFlag)
{
	//����ǰһ����վ�жϽڵ�λ��
	TgTRAIN_RECORD* temprec=NULL;
	temprec=single->GetPrevStationRecord(rec);
	TgTRAIN_RECORD* _start_rec=rec;
	TgTRAIN_RECORD* _end_rec=rec;
	int i=0;
	while(temprec!=NULL&&i<=single->GetRecordNum()&&(bJudgeFlag&1))
	{
		i++;
		int rc=JudgeMultiNodeIndex(single,temprec,index,usFlag,section);
		if(rc==FIND_TWICE)
		{
			_start_rec=temprec;
			temprec=single->GetPrevStationRecord(temprec);
			continue;
		}
		else if(rc==FIND_ONCE)
		{
			if(GetNodeSubSectionByIndex(forwardindex)==GetNodeSubSectionByIndex(index)){
				index=forwardindex;
				return 0;
			}
			else if(GetNodeSubSectionByIndex(reverseindex)==GetNodeSubSectionByIndex(index)){
				index=reverseindex;
				return 0;
			}
			else
			{
				int tindex=GetNodeIndexBySubSection(rec->station,GetNodeSubSectionByIndex(index));
				if(tindex>=0)
				{
					index=tindex;
					return 0;
				}else
					break;
			}
		}
		else break;
	}
	//���ݺ�һ����վ�жϽڵ�λ��
	temprec=single->GetNextStationRecord(rec);
	i=0;
	while(temprec!=NULL&&i<=single->GetRecordNum()&&(bJudgeFlag&2))
	{
		i++;
		int rc=JudgeMultiNodeIndex(single,temprec,index,usFlag,section);
		if(rc==FIND_TWICE)
		{
			_end_rec=temprec;
			temprec=single->GetNextStationRecord(temprec);
			continue;
		}
		else if(rc==FIND_ONCE)
		{
			if(GetNodeSubSectionByIndex(forwardindex)==GetNodeSubSectionByIndex(index)){
				index=forwardindex;
				return 0;
			}
			else if(GetNodeSubSectionByIndex(reverseindex)==GetNodeSubSectionByIndex(index)){
				index=reverseindex;
				return 0;
			}
			else
			{
				int tindex=GetNodeIndexBySubSection(rec->station,GetNodeSubSectionByIndex(index));
				if(tindex>=0)
				{
					index=tindex;
					return 0;
				}else
					break;
				break;
			}			
		}
		else break;
	}
	//�ٲ��У���ֻ�и�����Щ��վ��˳��������ͼ�Ͻ���ƥ�䣬��ƥ�䲻�ϣ�����ʱû�취��
	if(_start_rec==NULL||_end_rec==NULL)
		return -1;
	if(abs(_start_rec->rec_index-_end_rec->rec_index)<2)
		return -1;
	CList<int,int> list;
	if(JudgeMultiNodeIndexbySeq(single,rec,list,_start_rec,_end_rec)==FIND_TWICE)
	{
		if(list.GetCount()==1)
		{
			if(list.Find(GetNodeSubSectionByIndex(forwardindex))){
				index=forwardindex;
				return 0;
			}else if(list.Find(GetNodeSubSectionByIndex(reverseindex))){
				index=reverseindex;
				return 0;
			}
		}
		else
		{//���ֶ��ƥ��
			int fsubno=-1;int rsubno=-1;
			if(list.Find(GetNodeSubSectionByIndex(forwardindex))!=NULL)
				fsubno=GetNodeSubSectionByIndex(forwardindex);
			if(list.Find(GetNodeSubSectionByIndex(reverseindex))!=NULL)
				rsubno=GetNodeSubSectionByIndex(reverseindex);
			if(fsubno>-1&&rsubno>-1)
			{
				int dist=GetNodeSubSectionByIndex(index);
				if(abs(fsubno-dist)<abs(rsubno-dist))
					index=forwardindex;
				else
					index=reverseindex;
				return 0;
			}
			else if(fsubno>-1&&rsubno==-1)
			{
				index=forwardindex;
				return 0;
			}

			if(fsubno==-1&&rsubno>-1)
			{
				index=reverseindex;
				return 0;
			}
			else
			{
				//Ŀǰ�޷��ж�
				return -1;
			}
		}
	}
	return -1;
}

int CTGDoc::JudgeMultiNodeIndexbySeq(CTG_TRAIN * single,TgTRAIN_RECORD* rec,CList<int,int>& list,TgTRAIN_RECORD* _start_rec,TgTRAIN_RECORD* _end_rec)
{
	if(_start_rec==NULL||_end_rec==NULL){
		return ERR_NOT_FIND;
	}

	//forward sequence search
	TgTRAIN_RECORD *temp=_start_rec;
	int cur_node_sub_index=-1;
	for(int i=0;i<m_nDispDistanceNum;i++){
		if(temp==NULL)
			return ERR_NOT_FIND;
		if(cur_node_sub_index!=m_sDispDistance[i].sub_section){
			cur_node_sub_index=m_sDispDistance[i].sub_section;
			temp=_start_rec;
		}
		if(temp->station == m_sDispDistance[i].station_number){
			if((temp->rec_index==_end_rec->rec_index)&&(cur_node_sub_index!=-1)&&(list.Find(cur_node_sub_index)==NULL))
				list.AddTail(cur_node_sub_index);
			if(temp->rec_index!=_end_rec->rec_index)
				temp=single->GetNextStationRecord(temp);
		}
	}

	//reverse sequence search;
	temp=_start_rec;
	cur_node_sub_index=-1;
	for(i=m_nDispDistanceNum-1;i>=0;i--)
	{
		if(temp==NULL)
			return ERR_NOT_FIND;
		if(cur_node_sub_index!=m_sDispDistance[i].sub_section)
		{
			cur_node_sub_index=m_sDispDistance[i].sub_section;
			temp=_start_rec;
		}
		if(temp->station == m_sDispDistance[i].station_number)
		{
			if((temp->rec_index==_end_rec->rec_index)&&(cur_node_sub_index!=-1)&&(list.Find(cur_node_sub_index)==NULL))
				list.AddTail(cur_node_sub_index);
			if(temp->rec_index!=_end_rec->rec_index)
				temp=single->GetNextStationRecord(temp);
		}
	}
	if(list.IsEmpty())
		return ERR_NOT_FIND;
	else{
		return FIND_TWICE;
	}
}

/*
RETURN:  -1 �޷��ҵ���index
1  �ҵ�Ψһ��index
2 �ҵ����index
����޷��ҵ�index,���޷�������һ���ڵ���ȷ�ϣ�����ҵ�Ψһ��һ���������ȷ�ϣ����ݷ��ص�index,�����������οɵ�֪�����λ�ã�
��������ж��index,��ʾ���޷�ȷ�ϣ���Ҫ��һ����Ϣ��
*/
int CTGDoc::JudgeMultiNodeIndex(CTG_TRAIN * single,TgTRAIN_RECORD* rec,int& index,WORD usFlag,int section)
{
	index = -1;
	if(rec->entry==0)rec->entry=-1;
	if(rec->exit==0)rec->exit=-1;
	if( rec->entry == -1 &&  rec->exit ==-1 ){
		return ERR_NOT_FIND;
	}
	int i,j;
	bool bFoundLine = FALSE;
	//	node_info *node_info;

	if( rec->entry != -1&&rec->exit !=-1 )
	{
		WORD lineEntry,dirEntry;
		WORD lineExit,dirExit;
		bool bEntryFound = FALSE;
		bool bExitFound = FALSE;

		ConvertStationEntryToAbsnum(rec->station,
			rec->entry,
			&lineEntry,
			&dirEntry); 
		ConvertStationEntryToAbsnum(rec->station,
			rec->exit,
			&lineExit,
			&dirExit);

		bool myFlag = FALSE;
		//��ǰ������
		for(i=0;i < m_nDispDistanceNum;i++)
		{
			//node_info = &m_sDispDistance[i].node_info;
			if(rec->station != m_sDispDistance[i].station_number)
				continue;
			bEntryFound = FALSE;                 					 
			bExitFound = FALSE;                 					 

			//looking for up line 
			for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
				if(lineEntry ==  m_sDispDistance[i].up_line_list[j])
				{
					bEntryFound = TRUE;
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
				if(lineEntry ==  m_sDispDistance[i].down_line_list[j])
				{
					bEntryFound = TRUE;                        					 
					break;
				}
			}
			//looking for up line 
			for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
				if(lineExit ==  m_sDispDistance[i].up_line_list[j])
				{
					bExitFound = TRUE;                 					 
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
				if(lineExit ==  m_sDispDistance[i].down_line_list[j])
				{
					bExitFound = TRUE;                        					 
					break;
				}
			}

			if(bEntryFound && bExitFound){
				index = i;
				myFlag = TRUE;
			}
		}

		///�Ӻ���ǰ��
		BOOL myFlag1=FALSE;
		int index1=-1;
		for(i=m_nDispDistanceNum-1;i >=0 ;i--)
		{
			//node_info = &m_sDispDistance[i].node_info;
			if(rec->station != m_sDispDistance[i].station_number)
				continue;
			bEntryFound = FALSE;                 					 
			bExitFound = FALSE;                 					 

			//looking for up line 
			for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
				if(lineEntry ==  m_sDispDistance[i].up_line_list[j])
				{
					bEntryFound = TRUE;
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
				if(lineEntry ==  m_sDispDistance[i].down_line_list[j])
				{
					bEntryFound = TRUE;                        					 
					break;
				}
			}
			//looking for up line 
			for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
				if(lineExit ==  m_sDispDistance[i].up_line_list[j])
				{
					bExitFound = TRUE;                 					 
					break;
				}

			}
			//looking for down line
			for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
				if(lineExit ==  m_sDispDistance[i].down_line_list[j])
				{
					bExitFound = TRUE;                        					 
					break;
				}
			}

			if(bEntryFound && bExitFound){
				index1 = i;
				myFlag1 = TRUE;
			}
		}
		if((index!=index1)&&(myFlag==TRUE)&&(myFlag1==TRUE)){
			return FIND_TWICE;	
		}else if((index==index1)&&(myFlag==TRUE)&&(myFlag1==TRUE)){
			return FIND_ONCE;
		}
	}

	int entry = (rec->entry!=-1) ? rec->entry : rec->exit;
	int exit = (rec->exit !=-1) ? rec->exit : rec->entry;


	WORD line,dir;
	if(usFlag == ENTRY_POSITION){
		ConvertStationEntryToAbsnum(rec->station,
			entry,
			&line,
			&dir); 
	}
	else{
		ConvertStationEntryToAbsnum(rec->station,
			exit,
			&line,
			&dir);
	}

	int myIndex = -1;
	bFoundLine = FALSE;
	for(i=0;i < m_nDispDistanceNum;i++)
	{
		//node_info = &m_sDispDistance[i].node_info;
		if(rec->station != m_sDispDistance[i].station_number)
			continue;

		//looking for up line 
		for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
			if(line ==  m_sDispDistance[i].up_line_list[j])
				bFoundLine = TRUE;                 					 
		}
		//looking for down line
		for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
			if(line ==  m_sDispDistance[i].down_line_list[j])
				bFoundLine = TRUE;                        					 
		}
		if(bFoundLine){
			myIndex = i;//break;
			bFoundLine = FALSE;
		}
	}

	int myIndex1 = -1;
	BOOL bFoundLine1 = FALSE;
	for(i=m_nDispDistanceNum-1;i >=0;i--)
	{
		//node_info = &m_sDispDistance[i].node_info;
		if(rec->station != m_sDispDistance[i].station_number)
			continue;

		//looking for up line 
		for(j= 0;j <  m_sDispDistance[i].up_line_count;j++){
			if(line ==  m_sDispDistance[i].up_line_list[j])
				bFoundLine1 = TRUE;                 					 
		}
		//looking for down line
		for(j= 0;j <  m_sDispDistance[i].down_line_count;j++){
			if(line ==  m_sDispDistance[i].down_line_list[j])
				bFoundLine1 = TRUE;                        					 
		}
		if(bFoundLine1){
			myIndex1 = i;//break;
			bFoundLine1 = FALSE;
		}
	}
	if((myIndex>=0)&&(myIndex1>=0)&&(myIndex1!=myIndex)){
		return FIND_TWICE;	
	}
	else
	{
		if((myIndex>=0)&&(myIndex1>=0)&&(myIndex1==myIndex)){
			index = myIndex;
			return FIND_ONCE;	
		}
	}
	return ERR_NOT_FIND;      
}

////////zgj add
BOOL CTGDoc::PtInRects(RECT rects, POINT pt)
{
	CPoint point1(rects.left,rects.top);
	CPoint point2(rects.right,rects.bottom);

	//for test Rect;
	CRect Rects(point1,point2); 
	if(Rects.PtInRect(pt))
	{    
		return TRUE; 
	}

	//compare with point2;
	if( (pt.x == point2.x && pt.y >=point1.y && pt.y <= point2.y ) || 
		(pt.y == point2.y && pt.x >= point1.x && pt.x <= point2.x )    )
	{   
		return TRUE;
	}
	//no 
	return FALSE;
}

void CTGDoc::DrawTrainDecorateGreenLine(CDC* pCDC, const CPoint& ptlStart, const CPoint& ptlEnd)
{
	CPen* pOldPen = pCDC->SelectObject(&mLeftPen);
		
	pCDC->MoveTo(ptlStart.x-3, ptlStart.y);
	pCDC->LineTo(ptlEnd.x-3, ptlEnd.y);

	if(decorateStyle.lcolor==decorateStyle.rcolor)
	{
		pCDC->MoveTo(ptlStart.x+3, ptlStart.y);
		pCDC->LineTo(ptlEnd.x+3, ptlEnd.y);
	}
	else
	{
		pCDC->SelectObject(&mRightPen);
	
		pCDC->MoveTo(ptlStart.x+3, ptlStart.y);
		pCDC->LineTo(ptlEnd.x+3, ptlEnd.y);
	}

	pCDC->SelectObject (pOldPen);
}

void CTGDoc::DrawNoMeetMark(CDC *pCDC, const CPoint &ptlStart, const CPoint &ptlEnd, long no_meet_flag, COLORREF clrText, CString strTrainID)
{
	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;
	CPoint ptMiddle;

	ptMiddle.x = (m_ptEnd.x + m_ptStart.x) / 2;
	ptMiddle.y = (m_ptEnd.y + m_ptStart.y) / 2;

	// ��ʾ�����ı�
	WORD nNotMeetFlag=no_meet_flag&0xffff;
	CString strShowText=m_ptgctrl->mConfigTg.getNotMeetDisplayName(nNotMeetFlag);	
	strShowText = "����"+ strShowText;

	int nOldTextColor, nOldAlign, nOldMode;
	CFont mFont,*pOldFont;

	// ��������
	LOGFONT stFont;

	memset(&stFont,0,sizeof(stFont));
	int temp = -pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(8,-pCDC->GetDeviceCaps(LOGPIXELSY),72);	 
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	
	nOldTextColor = pCDC->SetTextColor(clrText);

	if(m_ptStart.y < m_ptEnd.y)
	{
		nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	}
	else
	{
		nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_TOP);
	}
	nOldMode = pCDC->SetBkMode(TRANSPARENT);

	CPoint textpt;
	
	CSize size = pCDC->GetTextExtent(strShowText);
	if(((m_ptStart.y - m_ptEnd.y) > 0) &&(m_ptStart.x <= m_ptEnd.x))
	{
		stFont.lfEscapement = atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptEnd.x-m_ptStart.x)) * 180 / PI * 10;
		mFont.CreateFontIndirect(&stFont);
		pOldFont = pCDC->SelectObject(&mFont);

		textpt.x = ptMiddle.x;
		textpt.y = ptMiddle.y;
	}
	else if(((m_ptStart.y - m_ptEnd.y)> 0 ) &&(m_ptStart.x > m_ptEnd.x))
	{
		stFont.lfEscapement = - atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
		mFont.CreateFontIndirect(&stFont);
		pOldFont = pCDC->SelectObject(&mFont);

		textpt.x = ptMiddle.x;
		textpt.y = ptMiddle.y;
	}
	else if((m_ptStart.y <= m_ptEnd.y)&&(m_ptStart.x <= m_ptEnd.x))
	{
		stFont.lfEscapement = - atan2(double( -m_ptStart.y + m_ptEnd.y), double(-m_ptStart.x+m_ptEnd.x)) * 180 / PI * 10;
		mFont.CreateFontIndirect(&stFont);
		pOldFont = pCDC->SelectObject(&mFont);

		textpt.x = ptMiddle.x;
		textpt.y = ptMiddle.y;
	}
	else
	{
		stFont.lfEscapement = atan2(double( -m_ptStart.y + m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
		mFont.CreateFontIndirect(&stFont);
		pOldFont = pCDC->SelectObject(&mFont);

		textpt.x = ptMiddle.x;
		textpt.y = ptMiddle.y;
	}

	pCDC->TextOut(textpt.x, textpt.y, strShowText);

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldTextColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	mFont.DeleteObject();

	return;
}

// ���Ʒ����г�
void CTGDoc::DrawBadMark(CDC *pCDC, CTG_TRAIN * single, const CPoint &ptlStart, const CPoint &ptlEnd, const TgTRAIN_RECORD* first)
{
	bool reverse_Mark=false;
	if(m_bShowReverse && IsFlagReverse(first->flag)) 
	{
		reverse_Mark = true;
	}

	CString strShowText;	
	strShowText = this->m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetAbsLineDesp(
											this->m_nCurrentSectionNo,
											first->station, first->exit);

	//CString strTrainID = this->GetTrainShowId(single);
	CString strTrainID = this->GetTrainShowId(first->station, *first, single);
	if(strTrainID=="")
		strTrainID = this->GetTrainShowId(single);

	if(reverse_Mark)
	{
		if(this->m_ptgctrl->mConfigTg.m_context.IsShowReverseTrainId())
		{
			strShowText += " "+strTrainID + "(��)";
		}
		else
		{
			strShowText += " (��)";
		}
	}
	
	if(strShowText.IsEmpty())
		return;
	

	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;

	CPoint ptMiddle,ptRect[4],ptTemp;
	double nSize = 5,nSize2 = 8;
	double fDelta=0.0;

	fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
	ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
	ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

	ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
	ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

	ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
	ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

	ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
	ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

	ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
	ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;

	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int temp = -pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(10,-pCDC->GetDeviceCaps(LOGPIXELSY),72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	
	int nOldAlign;
	int nOldMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldTextColor = pCDC->SetTextColor(CLR_RED);

	if(m_ptStart.y < m_ptEnd.y)
	{
		nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	}
	else
	{
		CSize size = pCDC->GetTextExtent("A��");
		nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_TOP);
		ptMiddle.x -= size.cx/1.4;
		ptMiddle.y -= size.cy/1.4;
	}
	
	CSize size = pCDC->GetTextExtent(strShowText);
	if(((m_ptStart.y - m_ptEnd.y) > 0) &&(m_ptStart.x <= m_ptEnd.x))
	{
		stFont.lfEscapement = atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptEnd.x-m_ptStart.x)) * 180 / PI * 10;
	}
	else if(((m_ptStart.y - m_ptEnd.y)> 0 ) &&(m_ptStart.x > m_ptEnd.x))
	{
		stFont.lfEscapement = - atan2(double(m_ptStart.y- m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
	}
	else if((m_ptStart.y <= m_ptEnd.y)&&(m_ptStart.x <= m_ptEnd.x))
	{
		stFont.lfEscapement = - atan2(double( -m_ptStart.y + m_ptEnd.y), double(-m_ptStart.x+m_ptEnd.x)) * 180 / PI * 10;
	}
	else
	{
		stFont.lfEscapement = atan2(double( -m_ptStart.y + m_ptEnd.y), double(m_ptStart.x-m_ptEnd.x)) * 180 / PI * 10;
	}

	CFont mFont,*pOldFont;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	pCDC->TextOut(ptMiddle.x, ptMiddle.y, strShowText);

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldTextColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	mFont.DeleteObject();
		
	return;
}


CString CTGDoc::GetTimeString(long time, CString& strSecond, bool bShowSecond)
{
	CString time_char;
	if(time == -1)
	{
		return "";
	}
	else
	{
		if(this->GetTgType() == TG_TYPE::BASIC_TG)
		{
			int uHour = time/3600;
			int uMinute = (time/60)%60;
			int uSecond = time%60;
			// 10�ָ���ʾ ��.��
			if(m_nADTimeSystle == 1)
			{
				if(uSecond!=0)
				{
					strSecond.Format("%d", uSecond);
					time_char.Format("%d.", uMinute%10);
				}
				else
				{
					strSecond="";
					time_char.Format("%d", uMinute%10);
				}
			}
			// Сʱ����ʾ ��
			else if(m_nADTimeSystle == 2)
			{
				strSecond="";
				time_char.Format("%d", uMinute);
			}
			// 10�ָ���ʾ ѡ����ʾ��.��,��ѡ����ʾ��
			else if(m_nADTimeSystle == 3)
			{
				if(bShowSecond)
				{
					strSecond.Format("%2d", uSecond);
					time_char.Format("%d.", uMinute%10);
				}
				else
				{
					strSecond="";
					time_char.Format("%d", uMinute%10);
				}
			}
			// Ĭ����ʾ10�ָ� ��ʾ ��
			else
			{
				strSecond="";
				time_char.Format("%d", uMinute%10);
			}
		}
		else
		{
			CTime tm(time);
			int nYear = tm.GetYear();
			int nMonth = tm.GetMonth();
			int nDay = tm.GetDay();
			int uHour = tm.GetHour();
			int uMinute = tm.GetMinute();
			int uSecond = tm.GetSecond();
			// 10�ָ���ʾ ��.��
			if(m_nADTimeSystle == 1)
			{
				if(uSecond!=0)
				{
					strSecond.Format("%d", uSecond);
					time_char.Format("%d.", uMinute%10);
				}
				else
				{
					strSecond="";
					time_char.Format("%d", uMinute%10);
				}
			}
			// Сʱ����ʾ ��
			else if(m_nADTimeSystle == 2)
			{
				strSecond="";
				time_char.Format("%d", uMinute);
			}
			// 10�ָ���ʾ ѡ����ʾ��.��,��ѡ����ʾ��
			else if(m_nADTimeSystle == 3)
			{
				if(bShowSecond)
				{
					strSecond.Format("%2d", uSecond);
					time_char.Format("%d.", uMinute%10);
				}
				else
				{
					strSecond="";
					time_char.Format("%d", uMinute%10);
				}
			}
			// Ĭ����ʾ10�ָ� ��ʾ ��
			else
			{
				strSecond="";
				time_char.Format("%d", uMinute%10);
			}
		}
	}
	return  time_char;
}
void CTGDoc::DrawLine(CDC* pCDC, const DRAW_LINE_STRUCT& param, const CPoint& ptlStart,const CPoint& ptlEnd)
{
	bool bAbsLine = param.bAbsLine;
	int nEngine  = param.nEngine;
	
	BYTE line_type = param.type;
	bool bExitEngine = param.bExitEngine;

	COLORREF nPenColor = param.clrLine;
	COLORREF nPenVertColor = param.plancolor;
	BYTE nPenWidth = param.nPenWidth;

	CPoint m_ptStart = ptlStart;
	CPoint m_ptEnd = ptlEnd;

	if(param.bSchdLine && mIsSchdLineSameToActualLine)
	{
		if(mIsChangeVertColor)
			nPenVertColor=nPenColor;
	}
    if((!param.bSchdLine) || (param.bSchdLine && mIsSchdLineSameToActualLine))
	{
		switch(line_type)
		{
		case 0: //�ͳ�����
		case 5: //��������
		case 6: //ֱͨ������
		case 20://������
		case 31:// ̫ԭ�������������
		case 32:// ̫ԭ��������һ���
		case 40:// ̫ԭ�������þ�Ԯ�г�
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID, nPenWidth, nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				pCDC->MoveTo(m_ptStart.x, m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x, m_ptEnd.y);

				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 18: //��ɫ��ͷ -->-->--
			{
				CPen mPen,*pOldPen;
				CBrush mBrush(nPenColor),*pOldBrush;
				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				CPoint ptMiddle,ptRect[4],ptTemp;
				int nSize = 4,nSize2=5,nSize3 = 13;
				double fDelta;
				ptMiddle.x = m_ptStart.x *1/2 + m_ptEnd.x / 2;
				ptMiddle.y = m_ptStart.y *1/2 + m_ptEnd.y / 2;
				fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
				ptRect[0].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
				ptRect[0].x = (int)(nSize / sqrt(fDelta * fDelta + 1));
				ptRect[0].Offset(ptMiddle.x,ptMiddle.y);

				ptTemp.y = (int)(-7 * fDelta / sqrt(fDelta * fDelta + 1));
				ptTemp.x = (int)(-7 / sqrt(fDelta * fDelta + 1));
				ptTemp.Offset(ptMiddle.x,ptMiddle.y);
				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(ptTemp.x,ptTemp.y);

				ptTemp.y = (int)(-5 * fDelta / sqrt(fDelta * fDelta + 1));
				ptTemp.x = (int)(-5 / sqrt(fDelta * fDelta + 1));
				ptTemp.Offset(ptMiddle.x,ptMiddle.y);

				ptRect[1].y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
				ptRect[1].x = (int)((-nSize2) * fDelta / sqrt(fDelta * fDelta + 1));
				ptRect[1].Offset(ptMiddle.x,ptMiddle.y);

				ptRect[3].y = (int)(nSize3 * fDelta / sqrt(fDelta * fDelta + 1));
				ptRect[3].x = (int)(nSize3 / sqrt(fDelta * fDelta + 1));
				ptRect[3].Offset(ptTemp.x,ptTemp.y);
				pCDC->MoveTo(ptTemp.x,ptTemp.y);
				pCDC->LineTo(ptRect[3].x,ptRect[3].y);

				ptTemp.y = (int)((nSize3+2) * fDelta / sqrt(fDelta * fDelta + 1));
				ptTemp.x = (int)((nSize3+2) / sqrt(fDelta * fDelta + 1));
				ptTemp.Offset(ptMiddle.x,ptMiddle.y);
				pCDC->MoveTo(ptTemp.x,ptTemp.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

				ptRect[2].y = (int)((-nSize2) / sqrt(fDelta * fDelta + 1));
				ptRect[2].x = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
				ptRect[2].Offset(ptMiddle.x,ptMiddle.y);

				// draw start line
				pOldBrush=pCDC->SelectObject(&mBrush);
				pCDC->BeginPath();
				pCDC->MoveTo(ptRect[0].x,ptRect[0].y);
				pCDC->LineTo(ptRect[1].x,ptRect[1].y);
				pCDC->LineTo(ptRect[3].x,ptRect[3].y);
				pCDC->LineTo(ptRect[2].x,ptRect[2].y);

				pCDC->EndPath();
				pCDC->FillPath();
				pCDC->SelectObject (pOldPen);
				mPen.DeleteObject();
				pCDC->SelectObject (pOldBrush);
				mBrush.DeleteObject();
			}
			break;
		case 1:
		case 16:
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID,nPenWidth,  nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[4],ptTemp;
					int x,y,flag;
					double nSize = 3,nSize2 = 8;
					double fDelta,fThreeSqrt = sqrt((double)3);

					fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
					ptRect[0].y = (int)(nSize * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[0].x = (int)(nSize * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
					ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

					ptRect[3].y = (int)(nSize * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));
					ptRect[3].x = (int)(nSize * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));
					y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
					x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					flag = 0;


					// draw start line
					ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
					ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					if(flag == 0)
					{
						pCDC->MoveTo(m_ptStart.x,m_ptStart.y - 1);
						pCDC->LineTo(ptTemp.x,ptTemp.y - 1);
					} 
					else 
					{
						pCDC->MoveTo(m_ptStart.x - 1,m_ptStart.y);
						pCDC->LineTo(ptTemp.x - 1,ptTemp.y);
					}
					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
					pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);

					//draw end line
					pCDC->SelectObject(&mPen);
					mRPen.DeleteObject();
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					if(flag == 0)
					{
						pCDC->MoveTo(ptTemp.x,ptTemp.y - 1);
						pCDC->LineTo(m_ptEnd.x,m_ptEnd.y - 1);
					} 
					else 
					{
						pCDC->MoveTo(ptTemp.x - 1,ptTemp.y);
						pCDC->LineTo(m_ptEnd.x - 1,m_ptEnd.y);
					}
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}
				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 3:
		case 4:
		case 7:
		case 12: // --O--
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine )
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptTemp;
					int x,y,nSize = 10;
					double fDelta;

					// compute 4 * 4 rectangle after rotate some angle
					//			if(abs(m_ptStart.y - m_ptEnd.y) < abs(m_ptStart.x - m_ptEnd.x))
					if(TRUE)
					{
						fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
						y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						y = (int)(nSize / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
					ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);


					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->Ellipse(ptMiddle.x - nSize/2,ptMiddle.y - nSize/2,
						ptMiddle.x + nSize/2,ptMiddle.y + nSize/2);

					pCDC->SelectObject(&mPen);
					mRPen.DeleteObject();

					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}

				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 8: // �����--����
		case 41:// ������--���
			{
				int nStyleCount = 2;
				DWORD StyleArray[2];
				StyleArray[0] = 5;
				StyleArray[1] = 5;
				LOGBRUSH logBrush;
				logBrush.lbStyle = BS_SOLID;
				logBrush.lbColor = nPenColor;
				logBrush.lbHatch = 0;

				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, nPenWidth,&logBrush,nStyleCount,StyleArray);

				pOldPen = pCDC->SelectObject(&mPen);

				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

				pCDC->SelectObject (pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 2:
		case 9:  // ����+���� --��--��--
		case 10: // ֱ��+���� --��--��--
			{
				CPen mPen,*pOldPen;
				if(line_type == 9)
				{
					int nStyleCount = 2;
					DWORD StyleArray[2];
					StyleArray[0] = 5;
					StyleArray[1] = 5;
					LOGBRUSH logBrush;
					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = nPenColor;
					logBrush.lbHatch = 0;

					mPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, nPenWidth,&logBrush,nStyleCount,StyleArray);
				}
				else
					mPen.CreatePen(PS_SOLID,nPenWidth, nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[4],ptTemp;
					int x,y;
					int nSize = 4;
					double fDelta;

					if(TRUE)
					{
						fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
						ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
						y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
						x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
						y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
						x = (int)(2 * nSize * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
					ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);

					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth, nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

					pCDC->SelectObject(&mPen);
					mRPen.DeleteObject();

					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}
				pCDC->SelectObject(pOldPen);		
				mPen.DeleteObject();
			}
			break;
		case 11:// ժ���г� --+--|--+--|--
			{
				CPen mPen,*pOldPen;

				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);

				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[4],ptTemp;
					int x,y,nSize = 6,nSize2 = 8;
					double fDelta;

					if(TRUE)
					{
						fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
						ptRect[0].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)((-nSize) / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);

						ptRect[0].y = (int)(nSize / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)((-nSize) * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x *2/3 + m_ptEnd.x / 3;
					ptMiddle.y = m_ptStart.y *2/3 + m_ptEnd.y / 3;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);

					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
					// draw middle line
					pCDC->SelectObject(&mPen);
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					ptMiddle.x = m_ptStart.x / 3 + m_ptEnd.x * 2 / 3;
					ptMiddle.y = m_ptStart.y / 3 + m_ptEnd.y * 2 / 3;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);

					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);
					//draw end line
					pCDC->SelectObject(&mPen);
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
					mRPen.DeleteObject();
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}
				pCDC->SelectObject(pOldPen);

				mPen.DeleteObject();
			}
			break;
		case 13://�������
			{
				CPen mPen,*pOldPen;

				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);

				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					if(nEngine == 0)
					{
						//middle rect size is 4 * 4 pixel
						CPoint ptMiddle,ptRect[3],ptTemp;
						int x,y;
						double nSize = 6.5,nSize2 = 12;
						double fDelta,fThreeSqrt = sqrt((double)3);
						nSize = 4;

						if(m_ptStart.y > m_ptEnd.y )
						{
							fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
							ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
							ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

							ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
							ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

							ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
							ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

							y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
							x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						} 
						else 
						{
							fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
							ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
							ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

							ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
							ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

							ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
							ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

							y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
							x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
						} 

						// draw start line
						pCDC->MoveTo(m_ptStart);
						ptMiddle.x = m_ptStart.x / 2 + m_ptEnd.x / 2;
						ptMiddle.y = m_ptStart.y / 2 + m_ptEnd.y / 2;
						if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
						{
							ptTemp.x = ptMiddle.x + x;
							ptTemp.y = ptMiddle.y + y;
						}
						else
						{
							ptTemp.x = ptMiddle.x - x;
							ptTemp.y = ptMiddle.y - y;
						}
						pCDC->LineTo(ptTemp);

						pCDC->SelectObject(&mRPen);
						pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
						pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
						pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
						pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
				
						//draw end line
						pCDC->SelectObject(&mPen);
						if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
						{
							ptTemp.x = ptMiddle.x - x;
							ptTemp.y = ptMiddle.y - y;

						}
						else
						{
							ptTemp.x = ptMiddle.x + x;
							ptTemp.y = ptMiddle.y + y;
						}
						pCDC->MoveTo(ptTemp);
						pCDC->LineTo(m_ptEnd);
					}
					else
					{//�һ����ĵ���
						//middle rect size is 4 * 4 pixel
						CPoint ptMiddle,ptRect[3],ptTemp;
						int x,y;
						double nSize = 6.5,nSize2 = 12;
						double fDelta,fThreeSqrt = sqrt((double)3);
						nSize = 4;

						if(m_ptStart.y > m_ptEnd.y )
						{
							fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
							ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
							ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

							ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
							ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

							ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
							ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

							y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
							x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						} 
						else 
						{
							fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
							ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
							ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

							ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
							ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

							ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
							ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

							y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
							x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
						} 

						// draw start line
						pCDC->MoveTo(m_ptStart);
						ptMiddle.x = m_ptStart.x * 2 / 3 + m_ptEnd.x / 3;
						ptMiddle.y = m_ptStart.y * 2 / 3 + m_ptEnd.y / 3;
						if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
						{
							ptTemp.x = ptMiddle.x + x;
							ptTemp.y = ptMiddle.y + y;
						}
						else
						{
							ptTemp.x = ptMiddle.x - x;
							ptTemp.y = ptMiddle.y - y;
						}
						pCDC->LineTo(ptTemp);

						pCDC->SelectObject(&mRPen);
						pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
						pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
						pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
						pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);


						ptMiddle.x = ptMiddle.x + ptRect[0].x;
						ptMiddle.y = ptMiddle.y + ptRect[0].y;
						if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
						{
							ptMiddle.x = ptMiddle.x - x;
							ptMiddle.y = ptMiddle.y - y;
						}
						else
						{
							ptMiddle.x = ptMiddle.x + x;
							ptMiddle.y = ptMiddle.y + y;
						}


						pCDC->SelectObject(pOldPen);
						pOldPen = pCDC->SelectObject(&mRPen);
						pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
						pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
						pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
						pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

						//draw end line
						pCDC->SelectObject(&mPen);
						if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
						{
							ptTemp.x = ptMiddle.x - x;
							ptTemp.y = ptMiddle.y - y;
						}
						else
						{
							ptTemp.x = ptMiddle.x + x;
							ptTemp.y = ptMiddle.y + y;
						}
						pCDC->MoveTo(ptTemp);
						pCDC->LineTo(m_ptEnd);
					}
					mRPen.DeleteObject();
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}
				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 14:
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[3],ptTemp;
					int x,y;
					double nSize = 6.5,nSize2 = 12;
					double fDelta,fThreeSqrt = sqrt((double)3);

					nSize = 3;
					if(TRUE)
					{
						fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
						ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x / 2 + m_ptEnd.x / 2;
					ptMiddle.y = m_ptStart.y / 2 + m_ptEnd.y / 2;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);

					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->MoveTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

					pCDC->SelectObject(&mPen);
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
					//add 
					pCDC->MoveTo(m_ptStart);
					pCDC->LineTo(m_ptEnd);
					mRPen.DeleteObject();
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}

				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 15: // --X--X--
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[4],ptTemp;
					int x,y;
					double nSize = 5,nSize2 = 8;
					double fDelta;

					if(TRUE)
					{
						fDelta = (double)(m_ptEnd.y - m_ptStart.y) / (double)(m_ptEnd.x - m_ptStart.x);
						ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(nSize * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + 1) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - 1) / sqrt(fDelta * fDelta + 1));

						ptRect[3].y = (int)(nSize * (1 - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[3].x = (int)(nSize * (1 + fDelta) / sqrt(fDelta * fDelta + 1));
						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x /2 + m_ptEnd.x / 2;
					ptMiddle.y = m_ptStart.y /2 + m_ptEnd.y / 2;
					ptTemp.x = ptMiddle.x - x;
					ptTemp.y = ptMiddle.y - y;
					pCDC->LineTo(ptTemp);

					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->MoveTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[3].x + ptMiddle.x,ptRect[3].y + ptMiddle.y);

					pCDC->SelectObject(&mPen);
					ptTemp.x = ptMiddle.x + x;
					ptTemp.y = ptMiddle.y + y;
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
					mRPen.DeleteObject();
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}

				pCDC->SelectObject(pOldPen); 		
				mPen.DeleteObject();
			}
			break;
		case 44: // ˫����
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID,nPenWidth,nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				if(bAbsLine)
				{
					//middle rect size is 4 * 4 pixel
					CPoint ptMiddle,ptRect[3],ptTemp;
					int x,y;
					double nSize = 6.5,nSize2 = 12;
					double fDelta,fThreeSqrt = sqrt((double)3);
					nSize = 4;

					if(m_ptStart.y > m_ptEnd.y )
					{
						fDelta = (double)(m_ptStart.y - m_ptEnd.y) / (double)(m_ptStart.x - m_ptEnd.x);
						ptRect[0].y = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt - fDelta) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (1 + fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (1 - fThreeSqrt * fDelta) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 * fDelta / sqrt(fDelta * fDelta + 1));//fDelta *
						x = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
					} 
					else 
					{
						fDelta = (double)(m_ptStart.x - m_ptEnd.x) / (double)(m_ptStart.y - m_ptEnd.y);
						ptRect[0].y = (int)(2 * nSize / sqrt(fDelta * fDelta + 1));
						ptRect[0].x = (int)(2 * nSize * fDelta / sqrt(fDelta * fDelta + 1));

						ptRect[1].y = (int)(nSize * (fThreeSqrt * fDelta - 1) / sqrt(fDelta * fDelta + 1));
						ptRect[1].x = (int)((-nSize) * (fDelta + fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						ptRect[2].y = (int)((-nSize) * (1 + fDelta * fThreeSqrt) / sqrt(fDelta * fDelta + 1));
						ptRect[2].x = (int)((-nSize) * (fDelta - fThreeSqrt) / sqrt(fDelta * fDelta + 1));

						y = (int)(nSize2 / sqrt(fDelta * fDelta + 1));
						x = (int)(nSize2 * fDelta  / sqrt(fDelta * fDelta + 1));
					} 

					// draw start line
					pCDC->MoveTo(m_ptStart);
					ptMiddle.x = m_ptStart.x * 2 / 3 + m_ptEnd.x / 3;
					ptMiddle.y = m_ptStart.y * 2 / 3 + m_ptEnd.y / 3;
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					else
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					pCDC->LineTo(ptTemp);

					CPen mRPen;
					mRPen.CreatePen(PS_SOLID,nPenWidth,nPenVertColor);
					pCDC->SelectObject(&mRPen);
					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);


					ptMiddle.x = ptMiddle.x + ptRect[0].x;
					ptMiddle.y = ptMiddle.y + ptRect[0].y;
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptMiddle.x = ptMiddle.x - x;
						ptMiddle.y = ptMiddle.y - y;
					}
					else
					{
						ptMiddle.x = ptMiddle.x + x;
						ptMiddle.y = ptMiddle.y + y;
					}


					pCDC->MoveTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);
					pCDC->LineTo(ptRect[1].x + ptMiddle.x,ptRect[1].y + ptMiddle.y);
					pCDC->LineTo(ptRect[2].x + ptMiddle.x,ptRect[2].y + ptMiddle.y);
					pCDC->LineTo(ptRect[0].x + ptMiddle.x,ptRect[0].y + ptMiddle.y);

					//draw end line
					pCDC->SelectObject(&mPen);
					if(m_ptStart.y > m_ptEnd.y && m_ptStart.x > m_ptEnd.x)
					{
						ptTemp.x = ptMiddle.x - x;
						ptTemp.y = ptMiddle.y - y;
					}
					else
					{
						ptTemp.x = ptMiddle.x + x;
						ptTemp.y = ptMiddle.y + y;
					}
					pCDC->MoveTo(ptTemp);
					pCDC->LineTo(m_ptEnd);
					mRPen.DeleteObject();
				}
				else
				{
					pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
					pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);
				}
				pCDC->SelectObject(pOldPen);
				mPen.DeleteObject();
			}
			break;
		case 21://δ֪�г����� 
		default:
			{
				CPen mPen,*pOldPen;
				mPen.CreatePen(PS_SOLID, nPenWidth, nPenColor);
				pOldPen = pCDC->SelectObject(&mPen);

				pCDC->MoveTo(m_ptStart.x,m_ptStart.y);
				pCDC->LineTo(m_ptEnd.x,m_ptEnd.y);

				pCDC->SelectObject (pOldPen);
				mPen.DeleteObject();
			}
			break;
		}

	}
	else
	{
		CPen mPen,*pOldPen;
		mPen.CreatePen(PS_SOLID, nPenWidth, nPenColor);
		pOldPen = pCDC->SelectObject(&mPen);

		pCDC->MoveTo(m_ptStart.x, m_ptStart.y);
		pCDC->LineTo(m_ptEnd.x, m_ptEnd.y);

		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject();
	}
	if(bExitEngine)
	{
		COLORREF m_clrLine = RGB(0,0,0);
		CPen mPen,*pOldPen;
		mPen.CreatePen(PS_DOT,1,m_clrLine);

		pOldPen = pCDC->SelectObject(&mPen);
		if(m_ptEnd.y != m_ptStart.y)
		{
			int minor;
			if(m_ptEnd.y >= m_ptStart.y)
			{
				minor = -5;
			}
			else
			{
				minor = 5;

			}
			pCDC->MoveTo(m_ptStart.x +minor,m_ptStart.y);
			pCDC->LineTo(m_ptEnd.x +minor,m_ptEnd.y);
		}
		pCDC->SelectObject (pOldPen);
		mPen.DeleteObject();
	}
}
// ���Ƽƻ���
void CTGDoc::DrawSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine)
{
	if(ptlStart.y < 0 || ptlEnd.y < 0 || prec == NULL)
		return;
	
	bool bExitEngine = false;
	if(prec->factor >= 143 && prec->nEngineCnt > 0)
	{
		bExitEngine = true;
	}
	if(13 == prec->line_type)
	{
		bExitEngine = false;
	}

	DRAW_LINE_STRUCT param;
	param.bSchdLine = true;
	param.bAbsLine = bAbsLine;
	param.bExitEngine = bExitEngine;
	param.clrLine = prec->schd_color;
	param.nEngine = prec->nEngineCnt;
	param.nPenWidth = prec->schd_width;
	if(m_bLineBold)
		param.nPenWidth += 2;
	param.plancolor = prec->schd_vert_color;
	param.type = prec->line_type;

	DrawLine(pCDC, param, ptlStart, ptlEnd);
}
// ����ʵ����
void CTGDoc::DrawActualLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine)
{
	if(ptlStart.y < 0 || ptlEnd.y < 0 || prec == NULL)
		return;

	bool bExitEngine = false;
	if(prec->factor >= 143 && prec->nEngineCnt > 0)
	{
		bExitEngine = true;
	}
	if(13 == prec->line_type)
	{
		bExitEngine = false;
	}

	DRAW_LINE_STRUCT param;
	param.bSchdLine = false;
	param.bAbsLine = bAbsLine;
	param.bExitEngine = bExitEngine;
	param.clrLine = prec->real_color;
	param.nEngine = prec->nEngineCnt;
	param.nPenWidth = prec->real_width;
	if(m_bLineBold)
		param.nPenWidth += 2;
	param.plancolor = prec->real_vert_color;
	param.type = prec->line_type;

	DrawLine(pCDC, param, ptlStart, ptlEnd);
}

WORD CTGDoc::GetLineColor(const int factor, const BYTE nFlag, COLORREF& schd_color, COLORREF& real_color, COLORREF& vert_color)
{
	LINE_COLOR linecolor;
	m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetTrainLineColor(factor,linecolor);
	
	schd_color = linecolor.schd;
	real_color = linecolor.real;
	vert_color = linecolor.vert;

	SPECIAL_COLOR special_color;
	if(this->m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetTrainSpecialColor(linecolor.index,special_color))
	{
		switch(nFlag)
		{
		case 1://space_rank
			schd_color = special_color.space_rank.schd_color;
			real_color = special_color.space_rank.real_color;
			break;
		case 2://length_rank
			schd_color = special_color.length_rank.schd_color;
			real_color = special_color.length_rank.real_color;
			break;
		case 3://super_rank
			schd_color = special_color.super_rank.schd_color;
			real_color = special_color.super_rank.real_color;
			break;
		case 4://military_rank
			schd_color = special_color.military_rank.schd_color;
			real_color = special_color.military_rank.real_color;
			break;			
		}
	}
	else
	{
		schd_color = linecolor.schd;
		real_color = linecolor.real;
	}
	

	return linecolor.line_type;
}

int CTGDoc::ConvertYPostoStationNos(int yPos, int &node_index, int &nStationNum)
{
	int i;

	for(i=0;i < m_nDispDistanceNum;i++)
	{
		if(abs(yPos - m_sDispDistance[i].nStationYPos) < 3)
			break;	
	}

	if(i < m_nDispDistanceNum)
	{ 
		nStationNum = 1;
		node_index = i;
		return m_sDispDistance[i].station_number;
	}

	//�жϵ�����������վ֮�� ������
	int y1,y2,y3;
	int bFlag = -1;
	int yTop,yBottom;
	yTop    = m_sDispDistance[0].nStationYPos;
	yBottom = m_sDispDistance[m_nDispDistanceNum-1].nStationYPos;
	if(m_sDispDistance[m_nDispDistanceNum-1].show_sation_sep)
		yBottom= m_sDispDistance[m_nDispDistanceNum-1].nSideYPos[m_sDispDistance[m_nDispDistanceNum-1].side[m_sDispDistance[m_nDispDistanceNum-1].number_of_side].side_number];

	for(i=0;i < m_nDispDistanceNum-1;i++)
	{   
		if(m_sDispDistance[i].show_sation_sep){

			y1 = m_sDispDistance[i].nSideYPos[m_sDispDistance[i].side[1].side_number];
			y2 = m_sDispDistance[i].nSideYPos[m_sDispDistance[i].side[m_sDispDistance[i].number_of_side].side_number];
			y3 = m_sDispDistance[i+1].nStationYPos;
			//�ɵ�֮��
			if( yPos - y1 > 0  && yPos - y2 <0)
			{   bFlag =1;
			break;
			}
			if( yPos - y2 > 0  && yPos - y3 <0)
			{   bFlag =2;
			break;
			}
		}
		y1 = m_sDispDistance[i].nStationYPos ;
		y2 = m_sDispDistance[i+1].nStationYPos ;
		if((yPos - y1 > 0 ) &&	yPos - y2  < 0)
			break;	
	}

	if(i < m_nDispDistanceNum-1 && bFlag ==-1)
	{ 
		nStationNum = 2;
		node_index = i;
		int  nRatio = (float)(yPos-y1)/(float)(y2-yPos)*100;
		return nRatio; //m_sDispDistance[i].station_number;
	}
	//����չ���Ĺɵ�֮��
	if(i < m_nDispDistanceNum-1 && bFlag ==1)
	{
		nStationNum = 1;
		node_index = i;
		return m_sDispDistance[i].station_number;
	}
	if(i < m_nDispDistanceNum-1 && bFlag ==2)
	{
		nStationNum = 2;
		node_index = i;
		int  nRatio = (float)(yPos-y2)/(float)(y3-yPos)*100;
		return nRatio; //m_sDispDistance[i].station_number;
	}
	if( yPos < yTop )//|| rec.position > yBottom )
	{
		nStationNum = 1;
		node_index = 0;
		return m_sDispDistance[0].station_number;
	}
	if( yPos > yBottom )//|| rec.position > yBottom )
	{
		nStationNum = 1;
		node_index = m_nDispDistanceNum -1;
		return m_sDispDistance[m_nDispDistanceNum-1].station_number;
	}
	//û�з����κ������ĳ�վ
	nStationNum = -1;
	return -1;
}

void CTGDoc::DrawFastOrSlowTime(CDC *pCDC, TgTRAIN_RECORD *first, POINT &ptlStart, POINT &ptlEnd,BOOL isFast,int minute,int dir)
{
	if(NULL == first || minute == 0)
		return;

	CPoint ptMiddle;
	ptMiddle.x = (ptlStart.x + ptlEnd.x) / 2;
	ptMiddle.y = (ptlStart.y + ptlEnd.y) / 2;

	CFont *pOldFont = pCDC->SelectObject(&fontAnalyTime);
	int nOldMode = pCDC->SetBkMode(TRANSPARENT);

	CString str;
	str.Format("%d",minute);

	int nOldTextColor;
	if(isFast)
		nOldTextColor = pCDC->SetTextColor(CLR_RED);
	else
		nOldTextColor = pCDC->SetTextColor(CLR_BLUE);

	CSize sz = pCDC->GetTextExtent(str);
	if(ptlStart.y < ptlEnd.y)
	{
		UINT nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pCDC->TextOut(ptMiddle.x+5, ptMiddle.y+5, str);
		pCDC->SetTextAlign(nOldAlign);
	}
	else if(ptlStart.y > ptlEnd.y)
	{
		UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);	
		pCDC->TextOut(ptMiddle.x-5, ptMiddle.y+5, str);
		pCDC->SetTextAlign(nOldAlign);
	}
	else
	{
		if(first->out_sta_direct == UP_DIRECT)
		{
			pCDC->TextOut(ptMiddle.x-sz.cx, ptMiddle.y, str);
		}
		else
		{
			pCDC->TextOut(ptMiddle.x+sz.cx, ptMiddle.y, str);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldTextColor);
	pCDC->SetBkMode(nOldMode);
	
	return;
}

// ����ͨ�����
void CTGDoc::DrawByPassFlag(CDC *pCDC, CTG_TRAIN *single)
{
	TgTRAIN_RECORD* prec=single->GetFirstRecord();
	if(!m_bByPassFlag || prec==NULL)
		return;

	CFont font;
	font.CreateFont(
				12,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"����");                 // lpszFacename

	
	CPen nPen_red;
	nPen_red.CreatePen(PS_JOIN_ROUND, 1, CLR_RED);

	CFont* pOldFont   = pCDC->SelectObject(&font);
	CPen *nOldPen     = pCDC->SelectObject(&nPen_red);
	int  nOldColor    = pCDC->SetTextColor(prec->schd_color);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign    = pCDC->SetTextAlign(TA_CENTER);

	CPoint point;
	
	CRect rect;
	int nOffset = 10;
	int nYDis = 4;
	
	TgTRAIN_RECORD *rec=NULL;
	int nRecNum = single->GetRecordNum ( );  
	for(int i = 0; i< nRecNum; i++)
	{
		rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		if(rec->arrive != rec->depart) 
			continue;

		point.y = GetStationVPos(rec->out_station_index,PRIMARY_TRACK);
		if(point.y < 0) 
			continue;

		//�Ȼ�����ͨ��
		if(IsFlagPassNonMainSide(rec->flag))
		{
			point.x = GetXPositionConsiderSecond(rec->arrive);
			if(IsFlagTerminal(rec->flag))
				rect = CRect(point.x - 2*nOffset, point.y - nOffset, point.x, point.y + nOffset);
			else
				rect = CRect(point.x, point.y - nOffset, point.x + 2*nOffset, point.y + nOffset);
			
			pCDC->Arc(&rect,CPoint(rect.left ,(rect.top + rect.bottom )/2 ),CPoint(rect.left ,(rect.top + rect.bottom )/2 ));
			pCDC->TextOut (rect.left + nOffset,rect.top + nYDis,"��");

		}
	}
	
	pCDC->SelectObject(pOldFont);
	pCDC->SelectObject(nOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetBkColor (nOldBackMode);
	pCDC->SetTextAlign(nOldAlign);

	font.DeleteObject();            
	nPen_red.DeleteObject();
}

bool CTGDoc::JudgeNeedWaveLine(TgTRAIN_RECORD *rec,int nDirectFlag)
{
	CTG_TRAIN *train;
	int startX,endX,tempStartX,tempEndX;
	startX = GetOffsetMinuteFromGraphBegin(rec->arrive);
	endX = GetOffsetMinuteFromGraphBegin(rec->depart);
	bool flag = FALSE;

	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); ++it)
	{
		TgTRAIN_RECORD *temp;
		train = &it->second;
		//��ͬ���򳵣�������
		if(train->m_nDirectFlag != nDirectFlag)
			continue;

		temp = train->GetStationRecord(rec->station); 
		if(temp == NULL)  continue;
		if(IsFlagStart(temp->flag) || IsFlagTerminal(temp->flag))
			continue;
		if(!(temp->entry == rec->entry && temp->exit == rec->exit))
			continue;
		//���㣬����λ��
		tempStartX = GetOffsetMinuteFromGraphBegin(temp->arrive);
		tempEndX = GetOffsetMinuteFromGraphBegin(temp->depart);
		//�����������ߵĵ�������һ����������
		////if((tempStartX > startX && tempStartX < endX) 	||	(tempEndX > startX && tempEndX < endX) )
		if( (tempStartX > startX && tempStartX < endX) &&
			(tempEndX < endX) )
		{
			flag = TRUE;
			break; //add by wjh 
		}
	}

	return flag;
}

/*************************************************************************************/
//added by Qain Guorong 2003-4-27 ends here
/*************************************************************************************
*
*Purpose:��������ж�Ҫ��ʾ�������ʱ���ǲ��ǳ����˾�����ʾ��Χ,�����,��ô�����������
*Input:CRect &orignRect, CString drawText, CPoint basicpt
*		CRect &orignRect	��ʼ���Σ�û�б仯֮ǰ��
*		CString drawText	�ھ�����Ҫ��ʾ������
*		CPoint basicpt		�����ĸ��ǵ�ĳһ���㣬���ݸõ��λ�þ���Ҫ���ŵķ���
*
*Output:Crect &orignRect	�����Ժ��ı����ֵ
*
*Return: TRUE�ı���ԭʼֵ/FALSE	û�иı�ԭʼֵ
*
*************************************************************************************/

BOOL CTGDoc::AdjustRectSizeByBasicPoint(CRect &orignRect, CString drawText, CPoint basicpt,int workThread)
{
	if(drawText.GetLength()<3)//���Ȳ�����
		return FALSE;

	double scale_rate = 0.2;
	if(workThread)//��ǰΪ��ӡ�����ڵ��øó���
		scale_rate = 0.5;


	CPoint topleft,bottomright;

	if(orignRect.left == basicpt.x && orignRect.top == basicpt.y)//���Ͻǻ����㣬�����½���
	{
		topleft.x = orignRect.left;
		topleft.y = orignRect.top;
		bottomright.x = orignRect.right + abs(orignRect.Width()*scale_rate);
		bottomright.y = orignRect.bottom + abs(orignRect.Height()*scale_rate);
		orignRect.SetRect(topleft,bottomright);
	}
	else if(orignRect.right == basicpt.x && orignRect.top == basicpt.y)//���Ͻǻ����㣬�����½���
	{
		topleft.x = orignRect.left - abs(orignRect.Width()*scale_rate);
		topleft.y = orignRect.top;
		bottomright.x = orignRect.right;
		bottomright.y = orignRect.bottom + abs(orignRect.Height()*scale_rate);
		orignRect.SetRect(topleft,bottomright);

	}
	else if(orignRect.left == basicpt.x && orignRect.bottom == basicpt.y)//���½ǻ����㣬�����Ͻ���
	{
		topleft.x = orignRect.left;
		topleft.y = orignRect.top - abs(orignRect.Height()*scale_rate);
		bottomright.x = orignRect.right + abs(orignRect.Width()*scale_rate);
		bottomright.y = orignRect.bottom;
		orignRect.SetRect(topleft,bottomright);
	}
	else if(orignRect.right == basicpt.x && orignRect.bottom == basicpt.y)//���½ǻ����㣬�����Ͻ���
	{
		topleft.x = orignRect.left - abs(orignRect.Width()*scale_rate);
		topleft.y = orignRect.top - abs(orignRect.Height()*scale_rate);
		bottomright.x = orignRect.right;
		bottomright.y = orignRect.bottom;
		orignRect.SetRect(topleft,bottomright);
	}
	/*	else if(orignRect.CenterPoint().x == basicpt.x && orignRect.bottom == basicpt.y)//�ھ����·����е㣬ֱ����������
	{
	;
	}
	else if(orignRect.right == basicpt.x && orignRect.bottom == basicpt.y)//�ھ����Ϸ����е㣬ֱ����������
	{
	;
	}
	*/	else
	return FALSE;

	return TRUE;
}

int CTGDoc::GenerateRecordsBetweenNodes(int sectionNo, int preNodeIndex, int nextNodeIndex, CTG_TRAIN *single,long curTime)
{
	if(sectionNo<0)
		return 0;

	if(preNodeIndex<0 || nextNodeIndex < 0)
		return 0;

	if(preNodeIndex>MAX_STATION_PER_SECTION || nextNodeIndex >MAX_STATION_PER_SECTION)
		return 0;

	if(preNodeIndex==nextNodeIndex)
		return 0;

	if(single == NULL)
		return 0;


	//�ж��г�����
	int nDirection; //1���У�0����
	if(preNodeIndex < nextNodeIndex)//����
	{
		single->m_nDirectFlag = DOWN_DIRECT;
		nDirection = DOWN_DIRECT;
	}
	else//����
	{
		single->m_nDirectFlag = UP_DIRECT;
		nDirection = UP_DIRECT;
	}
	int firstIndex = -1;
	int secondIndex = -1;

	firstIndex = min(preNodeIndex,nextNodeIndex);
	secondIndex = max(preNodeIndex,nextNodeIndex);


	BOOL connectFlag = TRUE;
	for(int i=firstIndex; i<secondIndex; i++)
	{
		if(!JudeNodesConnected(i,i+1))
		{
			connectFlag = FALSE;
			break;
		}
	}

	//��վ֮��û��ֱ����ͨ
	if(!connectFlag)
		return 0;

	long preDate=0,preTime=0;
	TgTRAIN_RECORD *firstrec = NULL;
	firstrec = single->GetLastRecord();

	if(firstrec == NULL)
		return 0;

	TgTRAIN_RECORD rec;
	int entry = 0;
	int exit = 0;
	int recNum = 0;

	//����ǰ�г��ĵ�һ�������ڸ�ֵ����Ϊ���ǵ��г�������ʵֻ��һ����¼��
	//��ںŲ��ԣ����� 
	if(single->GetRecordNum()==1)
	{	 
		GetEntryExitFromNodeIndex(preNodeIndex,entry,exit,nDirection);
		firstrec->entry = entry;
		firstrec->exit = exit;
	}

	//���ϸ���¼firstindex��exit,����¼(Ҫ����ļ�¼)�������ֵ
	int select_index;
	for(i=firstIndex; i<secondIndex; i++)
	{
		recNum = single->GetRecordNum();
		firstrec = single->GetRecordByPosition(recNum-1);
		
		if(nDirection == DOWN_DIRECT)
			GetExitEntryFromTwoNodeIndex(i,i+1,exit,entry,nDirection);
		else
			GetExitEntryFromTwoNodeIndex(firstIndex+secondIndex-i,firstIndex+secondIndex-i-1,exit,entry,nDirection); 

		//�ϸ���¼�ĳ���
		if(recNum >=1)                
			firstrec->exit = exit;

		rec.Clear();
		rec.rec_index = recNum;
		if(nDirection == DOWN_DIRECT)
		{
			rec.station = m_sDispDistance[i+1].station_number;
			select_index=i+1;
		}
		else
		{
			rec.station = m_sDispDistance[firstIndex+secondIndex-i-1].station_number;
			select_index=firstIndex+secondIndex-i-1;
		}

		BOOL bIsStopAtStation;
		if(firstrec->arrive == firstrec->depart)
			bIsStopAtStation = FALSE;
		else
			bIsStopAtStation = TRUE;

		WORD nRuntime = 420;

		if(i == firstIndex)
		{  
			preTime = firstrec->depart + nRuntime;
		}
		else
		{
			preTime = preTime + nRuntime;
		}

		if(firstrec->depart_train_id.IsEmpty())
			firstrec->depart_train_id=firstrec->arrive_train_id;
		if(firstrec->original_depart_train_id.IsEmpty())
			firstrec->original_depart_train_id=firstrec->original_arrive_train_id;

		rec.arrive_train_id = firstrec->depart_train_id;
		rec.depart_train_id = firstrec->depart_train_id;
		rec.original_arrive_train_id = firstrec->original_depart_train_id;
		rec.original_depart_train_id = firstrec->original_depart_train_id;

		rec.arrive = preTime;
		rec.depart = preTime;
		
		rec.plan_arrive =  rec.arrive;
		rec.plan_depart = rec.depart;
		
		rec.in_station_index = select_index;
		rec.out_station_index = select_index;
		//����¼�����
		rec.entry = entry;
		rec.exit = -1;
		rec.adjust_status = 0;
		rec.flag = 0;
		rec.early_depart_time = 0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)
		CancelFlagStopSignal(rec.flag);

		rec.arrive_side = -1;
		rec.depart_side = -1; 
		rec.modify_abs_runtime = 1; // �Ƿ�����޸ĵ�

		if(this->m_ptgctrl->mConfigTg.m_context.IsSetTechStop())
		{
			SetTechStopTrain(rec); //Ĭ�����ü���ͣ��
		}
		else
		{
			ClearTechStopTrain(rec);
		}

		//����ǰ�г������һ����ĳ��ڸ�ֵ
		if(i == secondIndex-1 && single->GetRecordNum() !=1)
		{	  
			WORD abs_line;
			exit=-1;
			if(nDirection == DOWN_DIRECT)
			{
				for(int i = 0; i < m_sDispDistance[select_index].down_line_count; i++)
				{
					abs_line = m_sDispDistance[select_index].down_line_list[i];
					const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
					if(line == NULL)
						continue;
			
					if(line->nDirect == nDirection || line->nDirect == 3)
					{
						exit = ConvertAbsnumStationToEntry(rec.station,abs_line);
						if(exit > 0)
							break;
					}
				}
			}
			else
			{
				for(int i = 0; i < m_sDispDistance[select_index].up_line_count; i++)
				{
					abs_line = m_sDispDistance[select_index].up_line_list[i];
					const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line);
					if(line == NULL)
						continue;
			
					if(line->nDirect == nDirection || line->nDirect == 3)
					{
						exit = ConvertAbsnumStationToEntry(rec.station,abs_line);
						if(exit > 0)
							break;
					}
				}
			}
			rec.exit=exit;
		}
		if(rec.station==firstrec->station)
		{
			firstrec->exit=rec.exit;
			break;
		}
		if(single->AddRecord(rec, this->GetTgType()) < 0)
			break;
	}

	return i-firstIndex;
}

// ����ģʽ�������� G01
void CTGDoc::DrawLanzhouMarkOfTractionEngine(CDC* pCDC, CTG_TRAIN* single,int nDirect)
{
	//�Ƿ���ʾ����
	if(!IsShowLocoNo())
		return;

	//�ж��Ƿ���Ҫ��ʾ������ ��ȡ�õ�������ʱ��Ļ���λ��
	if(IsDisplyTractionNone(single->myTrainInfo.adjust_flag))
	{
		return;
	}


	int bNeedOutputEngine;//�Ƿ���˦����ҵ 0���� 1����

	if(single->myEngine.size() > MAX_ENGINEER_NUM)
		return;

	nDirect = single->GetFirstRecord()->out_sta_direct;//added by zl 20120418 ���ݿ����β����߳������Ŵ�ӡ����
	CFont font, font1;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(11,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_BOLD; 
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Arial Narrow");

	LOGFONT stFont1;
	memset(&stFont1,0,sizeof(stFont1));
	stFont1.lfHeight = MulDiv(11,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont1.lfWeight = FW_BOLD; 
	stFont1.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont1.lfFaceName,"Arial Narrow");

	for (int sta=0;sta<=single->GetRecordNum()-1;sta++)//�ж�ÿ��վ�Ļ������,���һ��վ���ж�
	{
		bNeedOutputEngine=0;
		CString str_engineIds,str_attach,str_unstatch,str_engine;
		for (int i = 0; i < single->myEngine.size(); i++)//�Ի������б���
		{
			if (single->myEngine[i].attach_sta == sta||single->myEngine[i].unattach_sta == sta)//��վ��˦�һ�����ҵ
			{
				bNeedOutputEngine = TRUE;
				break;
			}
		}
		
		if (bNeedOutputEngine)//�����վ��˦��
		{
			for (int i = 0; i < single->myEngine.size(); i++)//�Ի������б���
			{
				CString tmp  = single->myEngine[i].engine_id;
				CString name = single->myEngine[i].engine_name;
				if ((sta>=single->myEngine[i].attach_sta && sta<single->myEngine[i].unattach_sta)||
					(sta==single->myEngine[i].attach_sta&&sta==sta<single->myEngine[i].unattach_sta&&sta==0))
					//�����վ���ڻ���,�������߻�����
				{
					if(name.IsEmpty())
					{
						if (str_engineIds.IsEmpty())
							str_engineIds =  tmp;  
						else
							str_engineIds = str_engineIds + "/" + tmp;  
					}
					else
					{
						if (str_engineIds.IsEmpty())
							str_engineIds =  name + "-" + tmp.Right(tmp.GetLength()-3);   
						else
							str_engineIds = str_engineIds + "/" + name + "-" + tmp.Right(tmp.GetLength()-3);   
					}
				}
				if(single->myEngine[i].attach_sta == sta&&sta!=0)//�ҳ� ���ơ�+������
				{
					if (name.IsEmpty())
					{
						if (str_attach.IsEmpty())
							str_attach = "��+" + tmp;  
						else
							str_attach = str_attach + "/" + tmp;  
					}
					else
					{
						if (str_attach.IsEmpty())
							str_attach = "��+" + name + "-" + tmp.Right(tmp.GetLength()-3);   
						else
							str_attach = str_attach + "/" + name + "-" + tmp.Right(tmp.GetLength()-3);
					}
				}
				if(single->myEngine[i].unattach_sta == sta&&sta!=0)//˦�� ���ơ�-������
				{
					if (name.IsEmpty())
					{
						if (str_unstatch.IsEmpty())
							str_unstatch = "��-" + tmp;  
						else
							str_unstatch = str_unstatch + "/" + tmp;  
					}
					else
					{
						if (str_unstatch.IsEmpty())
							str_unstatch = "��-" + name + "-" + tmp.Right(tmp.GetLength()-3);   
						else
							str_unstatch = str_unstatch + "/" + name + "-" + tmp.Right(tmp.GetLength()-3);   
					}
				}
				str_engine=str_attach+str_unstatch;
			}
			
			TgTRAIN_RECORD *rec = NULL;
			rec = single->GetRecordByPosition(sta);
			if(rec == NULL)
				return ;
			int y1_depart,x1_depart,y1_arrival,x1_arrival;
			int if_need = 1;
			if (rec->out_station_index != rec->in_station_index)//modified by zl 20120418 ���ݿ����β����߳������Ŵ�ӡ����
			{
				//��һ���ڵ� ����
				y1_depart = GetStationVPos(rec->out_station_index,  PRIMARY_TRACK);
				x1_depart = GetXPositionConsiderSecond(rec->depart);
				//��һ���ڵ� ����
				y1_arrival = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK);
				x1_arrival = GetXPositionConsiderSecond(rec->arrive);
				if_need = -1;
			}
			else
			{
				//��һ���ڵ� ����
				y1_depart = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK);
				x1_depart = GetXPositionConsiderSecond(rec->depart);
				//��һ���ڵ� ����
				y1_arrival = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK);
				x1_arrival = GetXPositionConsiderSecond(rec->arrive);
			}

			double fDelta;
			double fDelcon = 0;
			double fDelsin = 1;

			if (single->GetRecordNum()>1)
			{
				rec = single->GetRecordByPosition(sta+1);
				if(rec == NULL)
					return ;
                //��һ�ڵ� ����
				int y2_arrival = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK);
				int x2_arrival  = GetXPositionConsiderSecond(rec->arrive);
				fDelta = (double)(y2_arrival - y1_depart) / (double)(x2_arrival -x1_depart);
				fDelcon = sqrt(1/(1+fDelta*fDelta));
				fDelsin = sqrt(1-fDelcon);

				if(nDirect == DOWN_DIRECT)//����б��
				{
					if(x1_depart != x2_arrival)
					{

						if(y1_depart > y2_arrival )
						{
							stFont.lfEscapement=(fDelta==0?0:(-10*atan(1/fDelta)*180/3.14));
						}
						else
						{
							stFont.lfEscapement=-10*atan(fDelta)*180/3.14;
						}
					}
				}
				else
				{
					if(x1_depart != x2_arrival)
					{

						if(y1_depart > y2_arrival )
						{
							stFont.lfEscapement=(fDelta==0?0:(900+10*atan(1/fDelta)*180/3.14));
						}
						else
						{
							stFont.lfEscapement=900+10*atan(fDelta)*180/3.14;
						}
					}
				}			
			} 

			font.CreateFontIndirect(&stFont);
			font1.CreateFontIndirect(&stFont1);
			CFont * pOldFont = pCDC->SelectObject(&font);
			COLORREF oldColor = pCDC->SetTextColor(CLR_RED);
			UINT oldAligh = pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);

			TgTRAIN_RECORD *curr_rec = single->GetRecordByPosition(sta);
			if (nDirect == DOWN_DIRECT )
			{					
				pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
				pCDC->SelectObject(&font1);
				CSize sz = pCDC->GetTextExtent("1");
				if (if_need < 0)
				{
					if (this->m_ptgctrl->mConfigTg.m_tgdef.IsLocoNoDisplay(m_nCurrentSectionNo, 
						curr_rec->station, curr_rec->in_station_index, curr_rec->out_station_index, nDirect, 1))
					{
						pCDC->TextOut(x1_arrival, y1_arrival-8,str_unstatch);//���Ǳ��

						if(m_bIsVerifyTrainRect)
						{
						sz=pCDC->GetTextExtent(str_unstatch);

						CRect rect;
						rect=CRect(x1_arrival,y1_arrival-8-sz.cy,x1_arrival+sz.cx,y1_arrival-8);
						#ifdef DRAW_TRAIN_RECT	
							ShowRect(pCDC, rect);
						#endif
						single->UnionRect(rect);
						}
					}
					if (this->m_ptgctrl->mConfigTg.m_tgdef.IsLocoNoDisplay(m_nCurrentSectionNo, 
						curr_rec->station, curr_rec->in_station_index, curr_rec->out_station_index, nDirect, 0))
					{
						pCDC->TextOut(x1_arrival, y1_depart-8,str_attach);//���Ǳ��

						if(m_bIsVerifyTrainRect)
						{
						sz=pCDC->GetTextExtent(str_attach);
						CRect rect;
						rect=CRect(x1_arrival,y1_depart-8-sz.cy,x1_arrival+sz.cx,y1_depart-8);
						#ifdef DRAW_TRAIN_RECT	
							ShowRect(pCDC, rect);
						#endif
						single->UnionRect(rect);
						}
					}
				}
				else
				{
					pCDC->TextOut(x1_arrival, y1_arrival-8,str_engine);//���Ǳ��

					if(m_bIsVerifyTrainRect)
					{
					sz=pCDC->GetTextExtent(str_engine);
					CRect rect;
					rect=CRect(x1_arrival,y1_arrival-8-sz.cy,x1_arrival+sz.cx,y1_arrival-8);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				// �����ú���������������
				if(str_engineIds.IsEmpty())
					sz=pCDC->GetTextExtent("1");
				else
					sz=pCDC->GetTextExtent(str_engineIds);
				
				pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
				pCDC->SelectObject(&font);
                pCDC->TextOut(x1_depart+8*fDelsin, y1_depart-8*fDelcon,str_engineIds);
				// ����б������
				if(m_bIsVerifyTrainRect)
				{
				CRect rect;
				int x2_depart=x1_depart+sz.cx;
				int y2_depart=y1_depart+sz.cx;
				if(x2_depart>x1_depart)
				{
					if(y2_depart>y1_depart)
					{
						rect=CRect(x1_depart,y1_depart,x2_depart,y2_depart);
					}
					else
					{
						rect=CRect(x1_depart,y2_depart,x2_depart,y1_depart+1);
					}
				}
				else
				{
					if(y2_depart>y1_depart)
					{
						rect=CRect(x2_depart,y1_depart,x1_depart+1,y2_depart);
					}
					else
					{
						rect=CRect(x2_depart,y2_depart,x1_depart+1,y1_depart+1);
					}
				}
				rect.OffsetRect(0,-sz.cy);
				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			} 
			else
			{
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->SelectObject(&font1);
				CSize sz = pCDC->GetTextExtent("1");
				if (if_need < 0)
				{
					if (this->m_ptgctrl->mConfigTg.m_tgdef.IsLocoNoDisplay(m_nCurrentSectionNo, 
						curr_rec->station, curr_rec->in_station_index, curr_rec->out_station_index, nDirect, 1))
					{
						pCDC->TextOut(x1_arrival, y1_arrival+8,str_unstatch);//���Ǳ��
						
						if(m_bIsVerifyTrainRect)
						{
						sz=pCDC->GetTextExtent(str_unstatch);
						CRect rect;
						rect=CRect(x1_arrival,y1_arrival+8,x1_arrival+sz.cx,y1_arrival+8+sz.cy);
						#ifdef DRAW_TRAIN_RECT	
							ShowRect(pCDC, rect);
						#endif
						single->UnionRect(rect);
						}
					}
					if (this->m_ptgctrl->mConfigTg.m_tgdef.IsLocoNoDisplay(m_nCurrentSectionNo, 
						curr_rec->station, curr_rec->in_station_index, curr_rec->out_station_index, nDirect, 0))
					{
						pCDC->TextOut(x1_arrival, y1_depart+8,str_attach);//���Ǳ��

						if(m_bIsVerifyTrainRect)
						{
						sz=pCDC->GetTextExtent(str_attach);
						CRect rect;
						rect=CRect(x1_arrival,y1_depart+8,x1_arrival+sz.cx,y1_depart+8+sz.cy);
						#ifdef DRAW_TRAIN_RECT	
							ShowRect(pCDC, rect);
						#endif
						single->UnionRect(rect);
						}
					}
				}
				else
				{
					pCDC->TextOut(x1_arrival, y1_arrival+8,str_engine);//���Ǳ��

					if(m_bIsVerifyTrainRect)
					{
					sz=pCDC->GetTextExtent(str_engine);
					CRect rect;
					rect=CRect(x1_arrival,y1_arrival+8,x1_arrival+sz.cx,y1_arrival+8+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				// �����ú���������������
				if(str_engineIds.IsEmpty())
					sz=pCDC->GetTextExtent("1");
				else
					sz=pCDC->GetTextExtent(str_engineIds);
				pCDC->SetTextAlign(TA_LEFT|TA_BASELINE);
				pCDC->SelectObject(&font);
				pCDC->TextOut(x1_depart-8*fDelsin, y1_depart-8*fDelcon,str_engineIds);
				// ����б������
				if(m_bIsVerifyTrainRect)
				{
				CRect rect;
				int x2_depart=x1_depart+sz.cx;
				int y2_depart=y1_depart-sz.cx;
				if(x2_depart>x1_depart)
				{
					if(y2_depart>y1_depart)
					{
						rect=CRect(x1_depart,y1_depart,x2_depart,y2_depart);
					}
					else
					{
						rect=CRect(x1_depart,y2_depart,x2_depart,y1_depart+1);
					}
				}
				else
				{
					if(y2_depart>y1_depart)
					{
						rect=CRect(x2_depart,y1_depart,x1_depart+1,y2_depart);
					}
					else
					{
						rect=CRect(x2_depart,y2_depart,x1_depart+1,y1_depart+1);
					}
				}
				rect.OffsetRect(-sz.cy,0);
				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			}
			pCDC->SelectObject(pOldFont);
			pCDC->SetTextAlign(oldAligh);
			pCDC->SetTextColor(oldColor);
			font.DeleteObject();
			font1.DeleteObject();
		}
	}
}

void CTGDoc::DrawMarkOfTractionEngine(CDC *pCDC, CPoint pBasic, CSize trainidSize, CTG_TRAIN *single,int nDirect)
{
	// �Ƿ���Ҫ��ʾ������
	if(IsDisplyTractionNone(single->myTrainInfo.adjust_flag))
	{
		return;
	}

	int nEngineNum = single->myEngine.size();
	CString engineIds[MAX_ENGINEER_NUM];//������
	
	if(nEngineNum > MAX_ENGINEER_NUM)
		nEngineNum = 8;

	int tractionCount = 0;
	for(int i = 0; i < nEngineNum; i++)
	{
		CString tmp  = single->myEngine[i].engine_id;
		CString name = single->myEngine[i].engine_name;

		if(tmp.GetLength()>3)
		{
			if(name.IsEmpty())
			{
				engineIds[tractionCount] = tmp;
			}
			else
			{
				engineIds[tractionCount] = name + "/" + tmp.Right(tmp.GetLength()-3);
			}
		}
		else
		{
			if(tmp.IsEmpty())
				continue;
			else
				engineIds[tractionCount] = tmp;
		}
	
		tractionCount++;
	}

	// ����ʾ������ʱ, �����������Ƿ���ʾ"��"
	if(!IsShowLocoNo())
	{
		if(!single->myTrainInfo.b_elec_rank)
			return;
		if(!this->m_ptgctrl->mConfigTg.m_context.IsElecEngineShowDianzi())
			return;
		
		engineIds[0] = "��";
		tractionCount=1;
	}
	
	//û�л���
	if(tractionCount<=0)
		return;

	CPoint pStart ; //�ڶ��������ŵ�λ��

	CPoint pPos[MAX_ENGINEER_NUM];//ÿ�������ŵ�λ��	
	

	pStart = pBasic;
	//pStart.x += trainidSize.cx/2;

	if(nDirect == 1)
	{
		if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
			pStart.y -= trainidSize.cy*1.5;
		else if(IsDisplyTractionBottom(single->myTrainInfo.adjust_flag))
			pStart.y += trainidSize.cy/2+3;
		else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))
			pStart.y -= trainidSize.cy*2;
		else 	
			return;
			
	}
	else
	{
		if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
			pStart.y += trainidSize.cy*1.5;
		else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))			
			pStart.y -= trainidSize.cy/2+3;
		else if(IsDisplyTractionBottom(single->myTrainInfo.adjust_flag))
			pStart.y += trainidSize.cy*2;
		else
			return;
	}
	
	CFont *pOldFont = pCDC->SelectObject(&fontEngine);
	CSize sz = pCDC->GetTextExtent("11");
	pPos[0] = pStart;
	for(int j=1; j<tractionCount; j++)
	{
		pPos[j] = pPos[j-1];
		
		if(nDirect == 1)
		{
			if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
				pPos[j].y = pPos[j-1].y - 12;
			else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))
				pPos[j].y = pPos[j-1].y - 12;
			else
				pPos[j].y = pPos[j-1].y + 12;
		}
		else
		{
			if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
				pPos[j].y = pPos[j-1].y + 12;
			else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))
				pPos[j].y = pPos[j-1].y - 12;
			else
				pPos[j].y = pPos[j-1].y + 12;	
		}
	}

	int oldAligh = 0;
	int oldColor = CLR_RED;
    bool bAlignTop=false; // true:top false:bottom
	if(nDirect == 2)
	{
		if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
			oldAligh = pCDC->SetTextAlign(TA_BOTTOM|TA_RIGHT);
		else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))
		{
			oldAligh = pCDC->SetTextAlign(TA_TOP|TA_RIGHT);
			bAlignTop=true;
		}
		else
			oldAligh = pCDC->SetTextAlign(TA_BOTTOM|TA_RIGHT);
			
	}
	else
	{
		if(IsDisplyTractionMiddle(single->myTrainInfo.adjust_flag))
		{
			oldAligh = pCDC->SetTextAlign(TA_TOP|TA_RIGHT);
			bAlignTop=true;
		}
		else if(IsDisplyTractionTop(single->myTrainInfo.adjust_flag))
		{
			oldAligh = pCDC->SetTextAlign(TA_TOP|TA_RIGHT);
			bAlignTop=true;
		}
		else
			oldAligh = pCDC->SetTextAlign(TA_BOTTOM|TA_RIGHT);
	}

	oldColor = pCDC->SetTextColor(CLR_RED);

	CRect rect_total;
	rect_total.SetRectEmpty();
	for(j=0; j<tractionCount; j++)
	{
		CRect rect;
		sz=pCDC->GetTextExtent(engineIds[j]);
		if(bAlignTop)
		{
			rect=CRect(pPos[j].x-sz.cx, pPos[j].y, pPos[j].x, pPos[j].y+sz.cy);
		}
		else
		{
			rect=CRect(pPos[j].x-sz.cx, pPos[j].y-sz.cy, pPos[j].x, pPos[j].y);
		}
		
		rect_total.UnionRect(&rect_total, &rect);
		pCDC->TextOut(pPos[j].x, pPos[j].y, engineIds[j]);
	}
	
	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, rect_total);
	#endif
	single->UnionRect(rect_total);
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(oldAligh);
	pCDC->SetTextColor(oldColor);
	
	return;
}

CTG_TRAIN* CTGDoc::GetTrainByIndex(long index)
{
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.find(index);
	if(it != myPDataManage->myTrain.end())
	{
		return &it->second;
	}
	return NULL;
}

CTG_TRAIN* CTGDoc::GetTrainByTrainId(CString trainid, UINT& nFindNum)
{
	TIME nBeginTime=0;
	if(TG_TYPE::BASIC_TG != this->GetTgType())
	{
		if(m_ptgctrl->IsNewMode())  // 2016.10.7 �ҳ�bug
		{
			nBeginTime=this->GetBeginTime();
		}
		else
		{
			nBeginTime=this->GetBeginTime()+12*3600;
		}
	}
    
	nFindNum=0;
	CTG_TRAIN* findTrainPtr=NULL;
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for(;it != myPDataManage->myTrain.end();it++)
	{
		CString findtrainid=this->GetTrainShowId(it->second);
		if(findtrainid==trainid)
		{
			TgTRAIN_RECORD* last = it->second.GetLastRecord();
			if(last != NULL)
			{
				if(IsFlagTerminal(last->flag))
				{
					if(nBeginTime>=last->arrive)
						continue;
				}
				else
				{
					if(nBeginTime>=last->depart)
						continue;
				}

				nFindNum++;
				findTrainPtr=&it->second;
			}
		}
	}

	if(nFindNum==1)
		return findTrainPtr;
	
	return NULL;
}

//���ݵ�ǰ�ڵ�ĳ����ߺ����ж���һ���������Ľڵ��
int CTGDoc::GetNodeIndexByAbs(const int curNodeIndex, const int abs_num, const int dir)
{
	int ret_node_index = -1;

	if(dir == UP_DIRECT)//���г�
	{
		int pre_node_index = curNodeIndex - 1;
		while(pre_node_index >= 0)
		{
			for(int i=0; i<m_sDispDistance[pre_node_index].down_line_count; i++)
			{
				if(m_sDispDistance[pre_node_index].down_line_list[i] == abs_num)
					break;
			}

			if(i<m_sDispDistance[pre_node_index].down_line_count)
			{
				ret_node_index = pre_node_index;
				break;
			}
			pre_node_index--;
		}

	}
	else //���г�
	{
		int next_node_index = curNodeIndex + 1;
		while(next_node_index < m_nDispDistanceNum)
		{
			for(int i=0; i<m_sDispDistance[next_node_index].up_line_count; i++)
			{
				if(m_sDispDistance[next_node_index].up_line_list[i] == abs_num)
					break;
			}

			if(i<m_sDispDistance[next_node_index].up_line_count)
			{
				ret_node_index = next_node_index;
				break;
			}
			next_node_index++;
		}

	}

	return ret_node_index;
}

int CTGDoc::GetIncreaseLayer(CTG_TRAIN* single, const CPoint& ptlOrig,int nDirect,int stationno,const CSize& trainidSize)
{
	static CTG_TRAIN* orderedList[2048];
	int orderedCount = 0;

	int cursection = 0;
	cursection = m_nCurrentSectionNo;

	int m_cur_index=-1;int i=0;

	if(single->ptoftrainid_count<0||single->ptoftrainid_count>8)
		single->ptoftrainid_count=0;
	//����ѡ����ż�¼�ĵط�
	for(i=0;i<single->ptoftrainid_count;i++)
	{
		if(stationno==single->stPttoTrainid[i].stationno)
		{
			m_cur_index=i;
			break;
		}
	}
	if(m_cur_index==-1)
	{
		if(single->ptoftrainid_count<8)
		{
			m_cur_index=single->ptoftrainid_count;
			single->ptoftrainid_count++;
		}
		else
			return 1;
	}
	if(m_cur_index<0||m_cur_index>=8)return 1;
	single->stPttoTrainid[m_cur_index].stationno=stationno;
	CPoint ptlEnd=ptlOrig;
	CSize sExtent = trainidSize;

	CTG_TRAIN* curtrain = NULL;
	int layer=1,iDelta=0;
	CRect rect;
	rect.left=ptlEnd.x-sExtent.cx;
	rect.right=ptlEnd.x+sExtent.cx;
	rect.top=ptlEnd.y-sExtent.cy*(m_nTrainNoMaxLevel+2);
	rect.bottom=ptlEnd.y+sExtent.cy*(m_nTrainNoMaxLevel+2);

	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); ++it)
	{
		curtrain = &it->second;
		if(curtrain==NULL)
			continue;
		if(curtrain->GetTrainIndex()==single->GetTrainIndex() || m_nEraseTrainIndex==it->second.GetTrainIndex())
			continue;
		for(int j=0;j<curtrain->ptoftrainid_count;j++)
		{
			if(rect.PtInRect(curtrain->stPttoTrainid[j].ptActual))
			{
				orderedList[orderedCount++]=curtrain;
				break;
			}
		}
	}
	if(orderedCount==0)
		layer=1;
	else
	{
		rect.top=ptlEnd.y-sExtent.cy/2;
		rect.bottom=ptlEnd.y+sExtent.cy/2;
		i=0;
		bool find;
		while(i<orderedCount)
		{
			curtrain=orderedList[i];find=false;
			for(int j=0;j<curtrain->ptoftrainid_count;j++)
			{
				if(rect.PtInRect((curtrain->stPttoTrainid[j]).ptActual))
				{
					find=true;
					break;
				}
			}
			if(find)
			{
				layer++;
				iDelta = sExtent.cy*(layer-1);
				if(nDirect == UP_MARK)
					ptlEnd.y += -iDelta;
				else  ptlEnd.y += iDelta;
				rect.top=ptlEnd.y-sExtent.cy/2;
				rect.bottom=ptlEnd.y+sExtent.cy/2;
				i=0;
				continue;
			}
			if(layer>8)
			{
				break;
			}
			i++;
		}
	}
	//layer=layer%5
	//������ȷ�Ĳ��
	single->stPttoTrainid[m_cur_index].ptActual=ptlEnd;
	single->stPttoTrainid[m_cur_index].layer=layer;
	return layer;
}

BOOL CTGDoc::GetOperatorPriorByDistrict(int xMouse,WORD focus_pointInfo)
{
	if(GetTgType() != TG_TYPE::WORK_TG)
		return TRUE;
	if(m_ptgctrl->IsNewMode())
	{
		return TRUE;
	}
	
	if(xMouse <=0 )
		return TRUE;
	else if(xMouse <= m_fPixelPerHour*12)   //��һ��
	{
		BYTE timeLength = 0;
		bool bCanChangeHis = this->m_ptgctrl->mConfigTg.m_SingleTrainCfg.CanChangeHistorySchd(timeLength);

		bool bChange = false; // �����޸���һ������
		if(bCanChangeHis)
		{
			// Zhoujb added, 2008-05-27,�����1Сʱ�ڿ����޸���һ������
			// �˴�����ѡ���г�
			CTime tt = CTime::GetCurrentTime();
			int nHour = tt.GetHour();
			if(nHour >= 18)
			{
				if(nHour <= timeLength+18)
					bChange = true;
			}
			else if(nHour >= 6)
			{
				if(nHour <= timeLength+6)
					bChange = true;
			}
		}
		return bChange;
	}
	else if(xMouse < nCurrentX)             //����ʵ��������
	{
		if(m_bLimitDistrictFunction && focus_pointInfo == POINT_REAL)   //����ʵ�ʵ�
			return FALSE;
		else
			return TRUE;
	}
	else if(xMouse >=nCurrentX && xMouse < m_nAdjustStartX ) //�����ٽ��ƻ�����
	{  
		if(m_bLimitDistrictFunction && 
			(focus_pointInfo == POINT_ROUTE_OK || focus_pointInfo == POINT_REAL))  //��·�Ѿ��ź�
			return FALSE;
		else
			return TRUE;
	}
	else if(xMouse >=m_nAdjustStartX && xMouse <= m_nAdjustEndX ) //�����������
		return TRUE;
	else if(xMouse >m_nAdjustEndX )  //�ƻ�����
		return TRUE;
	else return TRUE;

}

int CTGDoc::GetNodeSubSectionByIndex(int index)
{
	if(index>=0&&index<m_nDispDistanceNum)
		return m_sDispDistance[index].sub_section; 
	else
		return -1;
}

int CTGDoc::GetNodeIndexBySubSection(int station,int subsection)
{
	for(int i=0;i<m_nDispDistanceNum;i++)
	{
		if(m_sDispDistance[i].sub_section==subsection
			&&m_sDispDistance[i].station_number==station)
			return i;
	}

	return -1;
}

//��ѯ�г��Ӻγ�վ������ʲô��վȥ������: ����Aվ������Bվ
CString CTGDoc::QueryTrainFromAndToStation(CTG_TRAIN* single)
{
	CString Sresult;Sresult.Empty();
	CTG_TRAIN *pTrain = single;

	CString sStation;
	do{

		TgTRAIN_RECORD *rec = NULL;
		rec = pTrain->GetRecordByPosition(0);
		if(NULL == rec)
			break;
		if( !IsFlagStart(rec->flag))
		{		    
			//1 ��һվ
			WORD NextLine,direction,nStationNo;
			if(!ConvertStationEntryToAbsnum(rec->station,rec->entry,&NextLine,&direction))
				break;

			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(NextLine);
			if(line == NULL)
				break;
			if(rec->station == line->nStartStation)
				nStationNo = line->nEndStation;
			else
				nStationNo = line->nStartStation;	

			Sresult += ",����";  //����һ��վ����

			if(nStationNo != 0)
				sStation.Format("%s",GetStationName(nStationNo));
			else
				sStation.Format("%s",line->sLineName);
			Sresult += sStation;
		}

		//2 ��һվ
		BOOL bIsErrorLine = FALSE;
		WORD nRecNum = pTrain->GetRecordNum();
		rec = pTrain->GetRecordByPosition(nRecNum-1);
		if(NULL == rec)
			break;
		if( !IsFlagTerminal(rec->flag)) 
		{
			WORD NextLine,direction,nStationNo;
			if(!ConvertStationEntryToAbsnum(rec->station,rec->exit,&NextLine,&direction))
				break;

			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(NextLine);
			if(line == NULL)
				break;
			//��Ϊ˫���
			if( line->nDirect ==3 )
			{
				if(rec->station == line->nStartStation)
					nStationNo = line->nEndStation;
				else
					nStationNo = line->nStartStation;
			}
			//��Ϊ������,���ߺ��е�һ������Ϊ��վ
			else  
			{
				if(rec->station == line->nStartStation)
					nStationNo = line->nEndStation;
				else
					bIsErrorLine = TRUE;

			}


			Sresult += ", ����";   //������һ��վ

			if(!bIsErrorLine)
			{
				if(nStationNo != 0)
					sStation.Format("%s",GetStationName(nStationNo));
				else
					sStation.Format("%s",line->sLineName);
			}
			else
				sStation = "???";
			Sresult += sStation;

		}

	}while(0);

	return Sresult;
}

void CTGDoc::GetEntryExitFromNodeIndex_abs(int node_index, WORD& entry,WORD *exitlist,WORD& exitnum,CString *sExplain,WORD* entry_next,WORD* station_next,int dir,int next_station)
{
	WORD nEntry = 0;
	WORD nExit = 0;
	WORD nExitNum = 0;
	//int j = 0;

	if(node_index < 0)
		return;

	if(dir == DOWN_DIRECT)
	{//DOWN_DIRECT
		if(m_sDispDistance[node_index].down_line_count == 0)
		{
			CString msg;
			msg.Format("��վ %s�·�û�п����ӳ�վ", m_sDispDistance[node_index].station_name);
			AfxMessageBox(msg);
			return;
		}
		for(int i=0;i< m_sDispDistance[node_index].down_line_count;i++)
		{
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(m_sDispDistance[node_index].down_line_list [i]);
			if(line)
			{
				int abs_line = m_sDispDistance[node_index].down_line_list[i];
				int station = m_sDispDistance[node_index].station_number;

				
				// 2012.01.12 ��������Ƿ���ȷ
				int nNextStationNo=0;
				if(station == line->nStartStation)
					nNextStationNo = line->nEndStation;
				else if(station == line->nEndStation)
					nNextStationNo = line->nStartStation;
				else
					nNextStationNo=-2;

				if(nNextStationNo==-2)
				{
					this->sysprint(10,0, "��ע��:[SECTION_NODE]=%d �����õ� down_line %d ��ʼ������վ����Ϊstation %d,����!", node_index+1, abs_line, station);
					continue;
				}

				// �������ɻ��߹���δѡ��һվ
				if(next_station>0 && nNextStationNo!=next_station)
				{
					continue;
				}
				
				exitlist[nExitNum] = ConvertAbsnumStationToEntry(station,abs_line);
				entry_next[nExitNum] =  ConvertAbsnumStationToEntry(nNextStationNo,abs_line); //�·�վ����ں�
				station_next[nExitNum] = nNextStationNo; //�·�վվ��

				if(line->nDirect ==3)
					sExplain[nExitNum].Format("�������䣺��%s��%s��",GetStationName(station),GetStationName(nNextStationNo));
				else
				{
					if(line->nStartStation==station)
						sExplain[nExitNum].Format("�������䣺��%s��%s�����г���",GetStationName(station),GetStationName(nNextStationNo));
					else
						sExplain[nExitNum].Format("�������䣺��%s��%s�����г���",GetStationName(station),GetStationName(nNextStationNo));
				}
				nExitNum++;
				exitnum = nExitNum;

				if(nExitNum>=32)
					break;
			}
		}
	}
	else
	{//UP_DIRECT
		if(m_sDispDistance[node_index].up_line_count == 0)
		{
			CString msg;
			msg.Format("��վ %s�Ϸ�û�п����ӳ�վ", m_sDispDistance[node_index].station_name);
			AfxMessageBox(msg);
			return;
		}
		for(int i=0;i< m_sDispDistance[node_index].up_line_count;i++)
		{
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(m_sDispDistance[node_index].up_line_list [i]);
			if(line)
			{
				int abs_line = m_sDispDistance[node_index].up_line_list [i];
				int station = m_sDispDistance[node_index].station_number ;

				// 2012.01.12 ��������Ƿ���ȷ
				int nNextStationNo=0;
				if(station == line->nStartStation)
					nNextStationNo = line->nEndStation;
				else if(station == line->nEndStation)
					nNextStationNo = line->nStartStation;
				else
					nNextStationNo=-2;

				if(nNextStationNo==-2)
				{
					this->sysprint(10,0, "��ע��:[SECTION_NODE]=%d �����õ� up_line %d ��ʼ������վ����Ϊstation %d,����!", node_index+1, abs_line, station);
					continue;
				}

				// �������ɻ��߹���δѡ��һվ
				if(next_station>0 && nNextStationNo!=next_station)
				{
					continue;
				} 

				exitlist[nExitNum] = ConvertAbsnumStationToEntry(station,abs_line);
				entry_next[nExitNum] =  ConvertAbsnumStationToEntry(nNextStationNo,abs_line); //�·�վ����ں�
				station_next[nExitNum] = nNextStationNo; //�·�վվ��


				if(line->nDirect ==3)
					sExplain[nExitNum].Format("�������䣺��%s��%s��",GetStationName(station),GetStationName(nNextStationNo));
				else
				{
					if(line->nStartStation==station)
						sExplain[nExitNum].Format("�������䣺��%s��%s�����г���",GetStationName(station),GetStationName(nNextStationNo));
					else
						sExplain[nExitNum].Format("�������䣺��%s��%s�����г���",GetStationName(station),GetStationName(nNextStationNo));
				}

				nExitNum++;
				exitnum = nExitNum;

				if(nExitNum>=32)
					break;
			}
		}
	}
	entry = (nEntry == 0)? -1:nEntry;
}

int CTGDoc::GetAbsByStationNoAndEntry(WORD station, WORD entry)
{
	return this->m_ptgctrl->mConfigTg.m_entrycfg.GetAbsByStationNoAndEntry(station, entry);
}

bool CTGDoc::ConvertStationEntryToAbsnum(WORD station, WORD entry_num, WORD *abs_num,WORD *dir)
{
	return this->m_ptgctrl->mConfigTg.m_entrycfg.ConvertStationEntryToAbsnum(station, entry_num,
		abs_num,dir);
}

int CTGDoc::ConvertAbsnumStationToEntry(WORD station,WORD abs_num)
{
	int entry = 0;
	if(!this->m_ptgctrl->mConfigTg.m_entrycfg.ConvertAbsnumStationToEntry(station, abs_num, &entry))
		return -1;
	return entry;
}

bool CTGDoc::GetABSLine(int lineNo, ABS_LINE& lineInfo)
{
	const ABS_LINE* pAbsLine = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(lineNo);
	if(pAbsLine == NULL)
	{
		return false;
	}

	lineInfo = *pAbsLine;

	return true;
}

const DispDistance* CTGDoc::GetDispDistance(int station)
{
	for(int idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		if(m_sDispDistance[idx].station_number == station)
		{
			return &m_sDispDistance[idx];
		}
	}

	return NULL;
}

int CTGDoc::GetDispDistanceIndex(int Pointy)
{
	for(int idx = 0; idx < this->m_nDispDistanceNum; idx++)
	{
		if(abs(m_sDispDistance[idx].nStationYPos - Pointy) <= 10)
		{
			return idx; 
		}
	}

	return -1; 
}

DispDistance* CTGDoc::GetDispDistanceByIdx(UINT index)
{
	if(0 <= index && index < this->m_nDispDistanceNum)
	{
		return &m_sDispDistance[index]; 
	}

	return NULL; 
}

bool  CTGDoc::GetFlagDrawTrainTime(int stano)
{
	for(int idx = 0; idx < this->m_nDispDistanceNum; idx++)
	{
		if(m_sDispDistance[idx].station_number == stano)
		{
			return m_sDispDistance[idx].bShowADTime; 
		}
	}
	return false;
}

int CTGDoc::GetStationProperty(int stano)
{
	for(int idx = 0; idx < this->m_nDispDistanceNum; idx++)
	{
		if(m_sDispDistance[idx].station_number == stano)
		{
			return m_sDispDistance[idx].nStaProperty; 
		}
	}
	return 0;
}

CString CTGDoc::GetSideName(int stano, int sideno)
{
	LINE_INFO side;
	if(!m_ptgctrl->mConfigTg.m_lineCfg.GetStaLineCfg(stano, side))
		return "?G";

	for(int i=0; i<side.vecSideInfo.size(); i++)
	{
		if(side.vecSideInfo[i].sideno == sideno)
			return side.vecSideInfo[i].sidename;
	}
	return "?G";
}

CString CTGDoc::GetStationName(int stano)
{
	std::map<int,TGStation>::const_iterator it = 
				m_ptgctrl->mConfigTg.station.find(stano);
	if(it != m_ptgctrl->mConfigTg.station.end())
	{
		return it->second.name;
	}

	return "����վ";
}

CString CTGDoc::GetTrainShowId(const int station, const TgTRAIN_RECORD& rec, CTG_TRAIN* train, bool bStar)
{	
	int  method = -1;

	CString trainId="�޳���";
	const TRAINID_DEFINE* define = this->m_ptgctrl->mConfigTg.m_SingleTrainCfg.GetTrainIdDefine(station);
	if(NULL == define)
	{
		trainId = rec.arrive_train_id;
		if(trainId.IsEmpty() || ((rec.rec_index==0) && IsFlagStart(rec.flag)))
			trainId = rec.depart_train_id;

		if(!this->m_ptgctrl->mConfigTg.m_context.IsShowMutiTrainCenter())
		{
			if(NULL != train)
			{
				int i=0;
				CString trainIdTmp=trainId;
				for(i=0; i<train->GetRecordNum(); i++)
				{
					TgTRAIN_RECORD& rec = train->myRecord[i];
					if(rec.arrive_train_id != trainId)
					{
						trainIdTmp = rec.arrive_train_id;
						break;
					}


					if(i+1==train->GetRecordNum())
					{
						if(IsFlagGiveout(rec.flag))
						{
							if(rec.depart_train_id != trainId)
							{
								trainIdTmp = rec.depart_train_id;
							}
						}
						break;
					}
					else
					{
						if(rec.depart_train_id != trainId)
						{
							trainIdTmp = rec.depart_train_id;
							break;
						}
					}
				}
				if(trainId != trainIdTmp)
				{
					for(i=0; i<trainId.GetLength()&&i<trainIdTmp.GetLength(); i++)
					{
						if(trainId.GetAt(i) != trainIdTmp.GetAt(i))
						{
							break;
						}
					}
					
					trainId.Format("%s/%s", trainId, trainIdTmp.Right(trainIdTmp.GetLength()-i));
				}
			}
		}
	}
	else
	{
		method = define->arrival_method;		
		if(method == TRAINID_DEFINE::TRAINID::DEPART)
		{
			trainId = rec.depart_train_id;
		}
		else
		{
			trainId = rec.arrive_train_id;
		}
	}
	
	if(bStar)
	{
		trainId += "*";
	}

	return trainId;
}

CString CTGDoc::GetTrainShowId(CTG_TRAIN& train)
{
	if(train.GetRecordNum() <= 0)
		return "�޳���";

	TgTRAIN_RECORD* rec = train.GetFirstRecord();
	if(rec == NULL)
		return "�޳���";

	return GetTrainShowId(rec->station, *rec, &train);
}

CString CTGDoc::GetTrainShowId(CTG_TRAIN* train)
{
	if(train == NULL || train->GetRecordNum() <= 0)
		return "�޳���";
	TgTRAIN_RECORD rec = *train->GetFirstRecord();
	return GetTrainShowId(rec.station, rec, train);
}

bool CTGDoc::TrainHaveActualPoint(CTG_TRAIN* train)
{
	TgTRAIN_RECORD *rec = NULL;

	int nAllRecordNo = train->GetRecordNum();
	for(int i = 0;i < nAllRecordNo; i++)
	{
		rec = train->GetRecordByPosition(i);
		if(rec == NULL) continue;
		
		if(IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
		    return true; 
	}
	return false;
}

// ���Ƽƻ���
void CTGDoc::DrawBasicSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine)
{
	if(ptlStart.x == ptlEnd.x && ptlStart.y == ptlEnd.y)
		return;

	CPoint ptTmpStart(ptlStart.x, ptlStart.y);
	CPoint ptTmpEnd(ptlEnd.x,ptlEnd.y);

	int nWholeDayPixel = this->m_fPixelPerHour*24;
	ptTmpStart.x = ptTmpStart.x%nWholeDayPixel;
	ptTmpEnd.x = ptTmpEnd.x%nWholeDayPixel;
    
	int nHour = m_ptgctrl->GetBeginHour();
	int nReturnPos = nHour*this->m_fPixelPerHour;
	if(ptTmpStart.x <= nReturnPos && ptTmpEnd.x <= nReturnPos ||
		ptTmpStart.x >= nReturnPos && ptTmpEnd.x >= nReturnPos)
	{
		DrawActualLine(pCDC, ptTmpStart, ptTmpEnd, prec, bAbsLine);
	}
	else
	{
		bool bdrawtwice = false;
		if(ptTmpEnd.x < ptTmpStart.x)
		{
			bdrawtwice = true;
			ptTmpEnd.x = ptTmpEnd.x + nWholeDayPixel;
		}
		DrawActualLine(pCDC, ptTmpStart, ptTmpEnd, prec, bAbsLine);
		
		if(bdrawtwice)
		{
			CPoint ptBegin = ptTmpStart;
			CPoint ptEnd = ptTmpEnd;

			ptBegin.x -= nWholeDayPixel;
			ptEnd.x -= nWholeDayPixel;

			DrawActualLine(pCDC, ptBegin, ptEnd, prec, bAbsLine);
		}
	}
}

// ��վͣ����
void CTGDoc::DrawBasicStationStopLine(CDC* pDC,POINT ptlStart,POINT ptlEnd)
{
	if(ptlStart.x == ptlEnd.x && ptlStart.y == ptlEnd.y)
		return;

	CPoint ptTmpStart(ptlStart.x, ptlStart.y);
	CPoint ptTmpEnd(ptlEnd.x,ptlEnd.y);

	int nWholeDayPixel = this->m_fPixelPerHour*24;

	ptTmpStart.x = ptTmpStart.x%nWholeDayPixel;
	ptTmpEnd.x = ptTmpEnd.x%nWholeDayPixel;

	bool bdrawtwice = false;
	if(ptTmpEnd.x < ptTmpStart.x)
	{
		bdrawtwice = true;
		ptTmpEnd.x = ptTmpEnd.x + nWholeDayPixel;
	}

	pDC->MoveTo(ptTmpStart);  pDC->LineTo(ptTmpEnd);

	if(bdrawtwice)
	{
		CPoint ptBegin = ptTmpStart;
		CPoint ptEnd = ptTmpEnd;

		ptBegin.x -= nWholeDayPixel;
		ptEnd.x -= nWholeDayPixel;

		pDC->MoveTo(ptBegin);  pDC->LineTo(ptEnd);
	}
}

// ����:0 ����:1 ������:2
int CTGDoc::GetTrainDirectionByTrainId(CString strTrainId)
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

void CTGDoc::SetTrainFirstRecIndex(CTG_TRAIN& train, bool bFlag)
{
	int index = -1;
	UINT nCountNum = train.GetRecordNum();
	TgTRAIN_RECORD *first = train.GetFirstRecord();
	if(first == NULL) 
		return;

	if(first->in_station_index < 0 || first->in_station_index >= m_nDispDistanceNum)
	{
		index = GetNodeEntryIndex(&train,first,m_nCurrentSectionNo);
		if(index >=0)
			first->in_station_index = index;
		else
		{
			if(nCountNum > 1)// ������̨�ƻ�
				first->in_station_index = -1;
		}
	}
	else
	{
		if(m_sDispDistance[first->in_station_index].station_number != first->station)
		{
			index = GetNodeEntryIndex(&train,first,m_nCurrentSectionNo);
			if(index >=0)
				first->in_station_index = index;
			else
				first->in_station_index = -1;
		}
	}
	

	if(nCountNum > 1 || (bFlag && nCountNum == 1))
	{
		if(first->out_station_index < 0 || first->out_station_index >= m_nDispDistanceNum)
		{
			index = GetNodeExitIndex(&train,first,m_nCurrentSectionNo);
			if(index >=0)
				first->out_station_index = index;
			else
			{
				if(nCountNum > 1) // ������̨�ƻ�
					first->out_station_index = -1;
			}
		}
		else
		{
			if(m_sDispDistance[first->out_station_index].station_number != first->station)
			{
				index = GetNodeExitIndex(&train,first,m_nCurrentSectionNo);
				if(index >=0)
					first->out_station_index = index;
				else
					first->out_station_index = -1;
			}
		}
	}
	
	if(bFlag)
	{
		CheckFirstLast(train);

		// [A-B+B'-C] ��ϵ���
		CheckABC(train);
	}
	return;
}

void CTGDoc::SetTrainMidRecIndex(CTG_TRAIN& train, int rec_index)
{
	int index = -1;
	
	TgTRAIN_RECORD *mid = train.GetRecordByPosition(rec_index);
	if(mid == NULL) 
		return;

	if(mid->in_station_index < 0 || mid->in_station_index >= m_nDispDistanceNum)
	{
		index = GetNodeEntryIndex(&train,mid,m_nCurrentSectionNo);
		if(index >=0)
			mid->in_station_index = index;
		else
			mid->in_station_index = -1;
	}
	else
	{
		if(m_sDispDistance[mid->in_station_index].station_number != mid->station)
		{
			index = GetNodeEntryIndex(&train,mid,m_nCurrentSectionNo);
			if(index >=0)
				mid->in_station_index = index;
			else
				mid->in_station_index = -1;
		}
	}

	if(mid->out_station_index < 0 || mid->out_station_index >= m_nDispDistanceNum)
	{
		index = GetNodeExitIndex(&train,mid,m_nCurrentSectionNo);
		if(index >=0)
			mid->out_station_index = index;
		else
			mid->out_station_index = -1;
	}
	else
	{
		if(m_sDispDistance[mid->out_station_index].station_number != mid->station)
		{
			index = GetNodeExitIndex(&train,mid,m_nCurrentSectionNo);
			if(index >=0)
				mid->out_station_index = index;
			else
				mid->out_station_index = -1;
		}
	}

	return;
}

void CTGDoc::SetTrainLastRecIndex(CTG_TRAIN& train, bool bFlag)
{
	int index = -1;
	UINT nCountNum = train.GetRecordNum();
	TgTRAIN_RECORD *last = train.GetLastRecord();
	if(last == NULL) 
		return;

	if(last->out_station_index < 0 || last->out_station_index >= m_nDispDistanceNum)
	{
		index = GetNodeExitIndex(&train,last,m_nCurrentSectionNo);
		if(index >=0)
			last->out_station_index = index;
		else
		{
			if(nCountNum > 1) // ������̨�ƻ�
				last->out_station_index = -1;
		}
	}
	else
	{
		if(m_sDispDistance[last->out_station_index].station_number != last->station)
		{
			index = GetNodeExitIndex(&train,last,m_nCurrentSectionNo);
			if(index >=0)
				last->out_station_index = index;
			else
				last->out_station_index = -1;
		}
	}
	
	if(nCountNum > 1 || (bFlag && nCountNum == 1))
	{
		if(last->in_station_index < 0 || last->in_station_index >= m_nDispDistanceNum)
		{
			index = GetNodeEntryIndex(&train,last,m_nCurrentSectionNo);
			if(index >=0)
				last->in_station_index = index;
			else
			{
				if(nCountNum > 1) // ������̨�ƻ�
					last->in_station_index = -1;
			}
		}
		else
		{
			if(m_sDispDistance[last->in_station_index].station_number != last->station)
			{
				index = GetNodeEntryIndex(&train,last,m_nCurrentSectionNo);
				if(index >=0)
					last->in_station_index = index;
				else
					last->in_station_index = -1;
			}
		}
	}

	if(bFlag)
	{
		CheckFirstLast(train);

		// [A-B+B'-C] ��ϵ���
		CheckABC(train);
	}
	return;
}

void CTGDoc::CheckFirstLast(CTG_TRAIN& train)
{
	int index = -1;
	UINT nCountNum = train.GetRecordNum();

	TgTRAIN_RECORD *first = train.GetFirstRecord();
	if(IsFlagStart(first->flag))
	{
		first->in_station_index = first->out_station_index;
	}

	TgTRAIN_RECORD *last = train.GetLastRecord();
	if(IsFlagTerminal(last->flag))
	{
		last->out_station_index = last->in_station_index;
	}


	if(nCountNum > 1 
		&& m_ptgctrl->mConfigTg.m_tgdef.m_nCheckMinLate 
		&& !m_ptgctrl->mConfigTg.m_tgdef.IsNoCheckMinLateSection(m_nCurrentSectionNo))
	{
		// ��һ����¼����ǽ��벻�ֱܷ���ʾ�ڲ�ͬ�ֶ�
		TgTRAIN_RECORD *first = train.GetFirstRecord();
		if(NULL != first && IsFlagAccept(first->flag) && !IsFlagAbsStart(first->flag))
		{
			if(!TwoStationSubSectionSame(first->in_station_index, first->out_station_index))
				first->in_station_index = first->out_station_index;
		}
		// ���һ����¼����ǽ������ֱܷ���ʾ�ڲ�ͬ�ֶ�
		TgTRAIN_RECORD *last = train.GetLastRecord();
		if(NULL != last && IsFlagGiveout(last->flag) && !IsFlagAbsTerminal(last->flag))
		{
			if(!TwoStationSubSectionSame(last->in_station_index, last->out_station_index))
				last->out_station_index = last->in_station_index;
		}
	}

	return;
}

// [A-B+B'-C] ��ϵ���
void CTGDoc::CheckABC(CTG_TRAIN& train)
{
	UINT nCountNum = train.GetRecordNum();

	for(UINT nRecIndex=0; nRecIndex<nCountNum; nRecIndex++)
	{
		TgTRAIN_RECORD *pre = train.GetRecordByPosition(nRecIndex-1);
		TgTRAIN_RECORD *curr = train.GetRecordByPosition(nRecIndex);
		TgTRAIN_RECORD *next = train.GetRecordByPosition(nRecIndex+1);
		
		int curr_in  = curr->in_station_index;
		int curr_out = curr->out_station_index;

		if(curr_in == -1 || curr_out == -1)
			continue;

		if(m_sDispDistance[curr_in].sub_section != m_sDispDistance[curr_out].sub_section)
		{
			if(pre != NULL && next != NULL)
			{
				int pre_out = pre->out_station_index;
				int next_in = next->in_station_index;
				
				if(pre_out == -1 || next_in == -1)
					continue;

				if(m_sDispDistance[pre_out].sub_section == m_sDispDistance[next_in].sub_section)
				{
					if(m_sDispDistance[pre_out].sub_section == m_sDispDistance[curr_in].sub_section)
					{
						curr->out_station_index = curr->in_station_index;
					}
					else if(m_sDispDistance[pre_out].sub_section == m_sDispDistance[curr_out].sub_section)
					{
						curr->in_station_index = curr->out_station_index;
					}
				}
			}
		}
	}

	return;
}

// �����г�ÿ��վ�������
void CTGDoc::SetStationInOutIndex(CTG_TRAIN& train)
{
	int index = -1;
	UINT nCountNum = train.GetRecordNum();
	
	if(this->m_bCalcStationIndex) 
	{	
		for(UINT nRecIndex=0; nRecIndex<nCountNum; nRecIndex++)
		{
			if(nRecIndex==0)
			{
				SetTrainFirstRecIndex(train, false);
			}

			if(nRecIndex==nCountNum-1)
			{
				SetTrainLastRecIndex(train, false);
			}

			if(0 < nRecIndex && nRecIndex < nCountNum-1)
			{
				SetTrainMidRecIndex(train, nRecIndex);
			}
		}

		CheckFirstLast(train);
		
		// [A-B+B'-C] ��ϵ���
		CheckABC(train);
	}
	else
	{
		for(UINT nRecIndex=0; nRecIndex<nCountNum; nRecIndex++)
		{
			TgTRAIN_RECORD *first = train.GetRecordByPosition(nRecIndex);
			if(first == NULL) 
				continue;

			if(nRecIndex==0 && IsFlagStart(first->flag))
			{
				first->in_station_index = -1;
			}

			if(nRecIndex==nCountNum-1 && IsFlagTerminal(first->flag))
			{
				first->out_station_index = -1;
			}
		}
	}
}

bool CTGDoc::IsMutilStation(int station)
{
	return this->m_ptgctrl->mConfigTg.m_tgdef.IsMutilStation(m_nCurrentSectionNo, station);
}

// ���վ���Ƿ�Ϸ�
bool CTGDoc::CheckStationInOutIndex(CTG_TRAIN& train)
{
	// ���ͷ���
	if(train.myRecord.size() == 0)
	{
		return false;
	}

	CString alarm;
	CString trainId = this->GetTrainShowId(train);
	for(int idx = 0; idx < train.myRecord.size(); idx++)
	{
		TgTRAIN_RECORD *rec = train.GetRecordByPosition(idx);
		
		if(idx==0)
		{
			if(IsFlagStart(rec->flag))
			{
				if(rec->out_station_index < 0 || rec->out_station_index >= this->m_nDispDistanceNum)
				{
					alarm.Format("[���ó���� ���վ��]ʼ���� %s �ĵ�%d��¼�ĳ���վ��%d����(0-%d)", trainId, idx, rec->out_station_index, m_nDispDistanceNum);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}

				if(this->m_sDispDistance[rec->out_station_index].station_number != rec->station)
				{
					alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�ĳ���վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
								trainId, idx, rec->out_station_index, this->m_sDispDistance[rec->out_station_index].station_number,
								rec->station);			
					this->sysprint(10,0, "%s", alarm);
					return false;
				}
			}
			else
			{
				if(rec->in_station_index < 0 || rec->in_station_index >= this->m_nDispDistanceNum)
				{
					alarm.Format("[���ó���� ���վ��]���복 %s �ĵ�%d��¼�����վ��%d����(0-%d)", trainId, idx, rec->in_station_index, m_nDispDistanceNum);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}

				if(this->m_sDispDistance[rec->in_station_index].station_number != rec->station)
				{
					alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�����վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
									trainId, idx, rec->in_station_index, this->m_sDispDistance[rec->in_station_index].station_number,
									rec->station);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}

				if(train.myRecord.size() > 1)
				{
					if(rec->out_station_index < 0 || rec->out_station_index >= this->m_nDispDistanceNum)
					{
						alarm.Format("[���ó���� ���վ��]ʼ���� %s �ĵ�%d��¼�ĳ���վ��%d����(0-%d)", trainId, idx, rec->out_station_index, m_nDispDistanceNum);
						this->sysprint(10,0, "%s", alarm);
						return false;
					}

					if(this->m_sDispDistance[rec->out_station_index].station_number != rec->station)
					{
						alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�ĳ���վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
									trainId, idx, rec->out_station_index, this->m_sDispDistance[rec->out_station_index].station_number,
									rec->station);			
						this->sysprint(10,0, "%s", alarm);
						return false;
					}
				}
			}
		}
	
		if(idx==train.myRecord.size()-1)
		{
			if(train.myRecord.size() > 1)
			{
				if(rec->in_station_index < 0 || rec->in_station_index >= this->m_nDispDistanceNum)
				{
					alarm.Format("[���ó���� ���վ��]�յ��� %s �ĵ�%d��¼�����վ��%d����(0-%d)", trainId, idx, rec->in_station_index, m_nDispDistanceNum);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}

				if(this->m_sDispDistance[rec->in_station_index].station_number != rec->station)
				{
					alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�����վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
									trainId, idx, rec->in_station_index, this->m_sDispDistance[rec->in_station_index].station_number,
									rec->station);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}
			}
			
			if(IsFlagGiveout(rec->flag))
			{
				if(rec->out_station_index < 0 || rec->out_station_index >= this->m_nDispDistanceNum)
				{
					alarm.Format("[���ó���� ���վ��]������ %s �ĵ�%d��¼�ĳ���վ��%d����(0-%d)", trainId, idx, rec->out_station_index, m_nDispDistanceNum);
					this->sysprint(10,0, "%s", alarm);
					return false;
				}

				if(this->m_sDispDistance[rec->out_station_index].station_number != rec->station)
				{
					alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�ĳ���վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
								trainId, idx, rec->out_station_index, this->m_sDispDistance[rec->out_station_index].station_number,
								rec->station);			
					this->sysprint(10,0, "%s", alarm);
					return false;
				}
			}
		}

		if(idx > 0 && idx < train.myRecord.size()-1)
		{
			if(rec->in_station_index < 0 || rec->in_station_index >= this->m_nDispDistanceNum)
			{
				alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�����վ��%d����(0-%d)", trainId, idx, rec->in_station_index, m_nDispDistanceNum);
				this->sysprint(10,0, "%s", alarm);
				return false;
			}
			if(rec->out_station_index < 0 || rec->out_station_index >= this->m_nDispDistanceNum)
			{
				alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�ĳ���վ��%d����(0-%d)", trainId, idx, rec->out_station_index, m_nDispDistanceNum);
				this->sysprint(10,0, "%s", alarm);
				return false;
			}

			if(this->m_sDispDistance[rec->in_station_index].station_number != rec->station)
			{
				alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�����վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
								trainId, idx, rec->in_station_index, this->m_sDispDistance[rec->in_station_index].station_number,
								rec->station);
				this->sysprint(10,0, "%s", alarm);
				return false;
			}

			if(this->m_sDispDistance[rec->out_station_index].station_number != rec->station)
			{
				alarm.Format("[���ó���� ���վ��]%s �ĵ�%d��¼�ĳ���վ����%d, ��������ͼTG.def�и�վ��ĳ�վ��%d���������г�վ%d��һ��", 
							trainId, idx, rec->out_station_index, this->m_sDispDistance[rec->out_station_index].station_number,
							rec->station);			
				this->sysprint(10,0, "%s", alarm);
				return false;
			}
		}	
	}

	return true;
}

// ����վ�����ɳ����
void CTGDoc::GenTrainExitEntryByStationIndex(CTG_TRAIN& train)
{
	CString alarm;
	CString trainId = this->GetTrainShowId(train);
	if(train.myRecord.size() == 1)
	{
		int nDirect = UP_DIRECT;
		TgTRAIN_RECORD *rec = train.GetFirstRecord();
		
		if(IsFlagStart(rec->flag))
		{
			rec->entry = -1;
		}
		else
		{
			nDirect = this->GetTrainDirectionByTrainId(train.myRecord[0].arrive_train_id);
			int uEntry,uExit;
			this->GetEntryExitFromNodeIndex(rec->in_station_index,uEntry,uExit, nDirect);
					
			rec->entry = uEntry;		
		}

		if(IsFlagGiveout(rec->flag))
		{
			if(rec->in_station_index == rec->out_station_index)
			{
				nDirect = this->GetTrainDirectionByTrainId(train.myRecord[0].depart_train_id);
			}
			else
			{
				if(rec->in_station_index > rec->out_station_index)
					nDirect = UP_DIRECT;
				else
					nDirect = DOWN_DIRECT;
			}

			int uEntry,uExit;
			this->GetEntryExitFromNodeIndex(rec->out_station_index,uEntry,uExit,nDirect);
			rec->exit = uExit;
		}
		else
		{
			rec->exit = -1;
		}
	}
	else
	{
		int nDirect = 0;
		int prev_exit=-1, curr_entry=-1;

		int cur_index, nxt_index;
		for(UINT idx = 0; idx < train.myRecord.size(); idx++)
		{
			TgTRAIN_RECORD *rec = train.GetRecordByPosition(idx);
			TgTRAIN_RECORD *nxt = train.GetRecordByPosition(idx+1);
			
			// 1. ���Ȱ����ü�������
			if(NULL != nxt && NULL != rec)
			{
				int exit, entry;
				int nFlag = GenExitEntryByInitABS(m_nCurrentSectionNo,
												  rec->station, nxt->station, 
												  rec->out_station_index, nxt->in_station_index, 
												  exit, entry);

				if(nFlag == 1)
				{
					alarm.Format("[����վ�����ó����] %s �ĵ�%d��%d��¼����ͨ, ���ɳ����ʧ��", trainId, idx, idx+1);
					this->sysprint(10,0, "%s", alarm);
				}
				else if(nFlag == 2)
				{
					rec->exit  = exit;
					nxt->entry = entry;
					nxt->exit = -1;
					alarm.Format("[����վ�����ó����] %s ����[REVERSE_ABS]����, %d ���� ��վ %d �� %d ==> cur_exit %d, next_entry %d", trainId, 
															m_nCurrentSectionNo,rec->station, nxt->station, rec->exit, nxt->entry);
					this->sysprint(10,0, "%s", alarm);
					continue;
				}
				else if(nFlag == 3)
				{
					alarm.Format("[����վ�����ó����] %s �ĵ�%d��%d��¼,entry.cfg��TG.def���ò���Ӧ, ����������������", trainId, idx, idx+1);
					this->sysprint(10,0, "%s", alarm);
				}

				int nAbsLineNo=GetLineNoByStations(rec->station,nxt->station);
				if(nAbsLineNo>0)
				{
					exit  = ConvertAbsnumStationToEntry(rec->station, nAbsLineNo);
					entry = ConvertAbsnumStationToEntry(nxt->station, nAbsLineNo);
					if(exit>0 && entry>0)
					{
						rec->exit  = exit;
						nxt->entry = entry;
						sysprint(0, 0, "[����ǰ��վ˳�����ó����]���� tg.def��[STATION_NEXTSTATION_LINENO]����:%d,%d,%d,���ó�վ %d ���� %d ��վ %d ��� %d.",
							rec->station, nxt->station, nAbsLineNo, rec->station, exit, nxt->station, entry);
						continue;
					}
				}
			}
			
			// 2. ������վ���������
			if (rec !=NULL && nxt !=NULL)
			{
				if(idx == 0)
				{
					if(IsFlagStart(rec->flag))
					{
						cur_index = rec->out_station_index;
						nxt_index = nxt->in_station_index;

						if(cur_index > nxt_index)
							nDirect = UP_DIRECT;
						else
							nDirect = DOWN_DIRECT;
						
						if(!this->JudeNodesConnected(cur_index, nxt_index))
						{
							alarm.Format("[����վ�����ó����] %s �ĵ�%d��%d��¼����ͨ, ���ɳ����ʧ��", trainId, idx, idx+1);
							this->sysprint(10,0, "%s", alarm);
							continue;
						}
						this->GetExitEntryFromTwoNodeIndex(cur_index, nxt_index, prev_exit, curr_entry, nDirect);
					
						rec->entry = -1;
						rec->exit  = prev_exit;
						nxt->entry = curr_entry;
						nxt->exit = -1;
					}
					else
					{
						if(rec->in_station_index == rec->out_station_index)
						{
							if(rec->out_station_index > nxt->in_station_index)
								nDirect = UP_DIRECT;
							else
								nDirect = DOWN_DIRECT;
						}
						else
						{
							if(rec->in_station_index > rec->out_station_index)
								nDirect = UP_DIRECT;
							else
								nDirect = DOWN_DIRECT;
						}

						int uEntry,uExit;
						this->GetEntryExitFromNodeIndex(rec->in_station_index,uEntry,uExit, nDirect);
						
						cur_index = rec->out_station_index;
						nxt_index = nxt->in_station_index;
						if(cur_index > nxt_index)
							nDirect = UP_DIRECT;
						else
							nDirect = DOWN_DIRECT;
						
						if(!this->JudeNodesConnected(cur_index, nxt_index))
						{
							alarm.Format("[����վ�����ó����] %s �ĵ�%d��%d��¼����ͨ, ���ɳ����ʧ��", trainId, idx, idx+1);
							this->sysprint(10,0, "%s", alarm);
							continue;
						}

						this->GetExitEntryFromTwoNodeIndex(cur_index, nxt_index, prev_exit, curr_entry, nDirect);
					
						rec->entry = uEntry;
						rec->exit  = prev_exit;
						nxt->entry = curr_entry;
						nxt->exit = -1;
					}
				}
				else if(idx == train.myRecord.size()-1)
				{
					if(IsFlagGiveout(rec->flag))
					{
						if(rec->in_station_index == rec->out_station_index)
						{
							; // ���ϴεķ���
						}
						else
						{
							if(rec->in_station_index > rec->out_station_index)
								nDirect = UP_DIRECT;
							else
								nDirect = DOWN_DIRECT;
						}

						int uEntry,uExit;
						this->GetEntryExitFromNodeIndex(rec->out_station_index,uEntry,uExit,nDirect);
						rec->exit = uExit;
					}
					else
					{
						rec->exit = -1;
					}
				}
				else
				{
					cur_index = rec->out_station_index;
					nxt_index = nxt->in_station_index;
					if(cur_index > nxt_index)
						nDirect = UP_DIRECT;
					else
						nDirect = DOWN_DIRECT;
					
					if(!this->JudeNodesConnected(cur_index, nxt_index))
					{
						alarm.Format("[����վ�����ó����] %s �ĵ�%d��%d��¼����ͨ, ���ɳ����ʧ��", trainId, idx, idx+1);
						this->sysprint(10,0, "%s", alarm);
						continue;
					}
					this->GetExitEntryFromTwoNodeIndex(cur_index, nxt_index, prev_exit, curr_entry, nDirect);
					
					rec->exit  = prev_exit;
					nxt->entry = curr_entry;
					nxt->exit = -1;
				}
			}
		}

		// �����Ƿ�ʹ��TG����ĳ���ڱ�־
		TgTRAIN_RECORD *first = train.GetFirstRecord();
		TgTRAIN_RECORD *last = train.GetLastRecord();
		if(first)
		{
			first->speed_limit=0;
			if(SetTrainEntryByCfg(&train, 0)>0)
				first->speed_limit=1;
		}
		if(last)
		{
			last->speed_limit=0;
			if(SetTrainExitByCfg(&train, train.GetRecordNum()-1)>0)
				last->speed_limit=1;
		}
	}

	alarm.Format("[����վ�����ó����] %s ���ɳ���ڳɹ�", trainId);
	this->sysprint(10,0, "%s", alarm);
	return;	
}

// �����г�����
void CTGDoc::SetTrainDirect(CTG_TRAIN& train)
{
	int nDirect = -1;

	// �г��Ƿ�仯����
	int mChangeDirectFlag = 0;
	UINT nRecNum = train.GetRecordNum();  
	for(UINT i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = train.GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		TgTRAIN_RECORD *nxt = NULL;
		if(i < nRecNum-1)
			nxt = train.GetRecordByPosition(i+1);
		else
			nxt = NULL;

		if(nxt != NULL && rec != NULL)
		{
			int Ynxt = GetStationVPos(nxt->in_station_index,   PRIMARY_TRACK);
			int Yrec = GetStationVPos(rec->out_station_index,  PRIMARY_TRACK);
			
			if (Ynxt == Yrec)
			{
				mChangeDirectFlag = 1;
				break;
			}
			else if(Ynxt > Yrec)
			{
				if(nDirect != -1 && nDirect == TRAIN_DIRECT::UP)
				{
					mChangeDirectFlag = 1;
					break;
				}
				nDirect = TRAIN_DIRECT::DOWN;
			}
			else if(Ynxt < Yrec)
			{
				if(nDirect != -1 && nDirect == TRAIN_DIRECT::DOWN)
				{
					mChangeDirectFlag = 1;
					break;
				}
				nDirect = TRAIN_DIRECT::UP;
			}
		}
	}

	// �г�����
	train.m_nDirectFlag = UP_DIRECT;
	if(nRecNum == 1)
	{
		TgTRAIN_RECORD *first;
		first = train.GetRecordByPosition(0);
		if(first == NULL) 
			return;

		if(IsFlagStopAbs(first->flag))
		{
			if(IsFlagStopAbsUp(first->flag))
			{
				train.m_nDirectFlag = UP_DIRECT;
			}
			else
			{
				train.m_nDirectFlag = DOWN_DIRECT;
			}
		}
		else if(IsFlagAbsStart(first->flag))
		{
			if(IsAbsStartStopDown(train.myTrainInfo.adjust_flag))
			{
				train.m_nDirectFlag = UP_DIRECT;
			}
			else
			{
				train.m_nDirectFlag = DOWN_DIRECT;
			}
		}
		else 
		{
			CString strTrainId = first->depart_train_id;
			if(strTrainId.IsEmpty())
			{
				strTrainId = first->arrive_train_id;
			}
			if(strTrainId.IsEmpty())
			{
				strTrainId = GetTrainShowId(train);
			}
			train.m_nDirectFlag = GetTrainDirectionByTrainId(strTrainId);
		}
	}
	else
	{
		TgTRAIN_RECORD *first,*second;
		first = train.GetRecordByPosition(0);
		if(first == NULL) 
			return;
		second = train.GetRecordByPosition(1);
		if(second == NULL) 
			return;
		
		if(first->station == second->station)
		{
			if(IsFlagStopAbs(first->flag))
			{
				if(IsFlagStopAbsUp(first->flag))
				{
					train.m_nDirectFlag = UP_DIRECT;
				}
				else
				{
					train.m_nDirectFlag = DOWN_DIRECT;
				}
			}
			else
			{
				int len = first->depart_train_id.GetLength();
				for(int i= len-1;i>=0;i--)
				{
					char lastchar = first->depart_train_id.GetAt(i);
					if(lastchar < 0x31 || lastchar > 0x39)
						break;
				}
				CString tempTrainId = first->depart_train_id.Right(len - i - 1);
				len = atoi(tempTrainId);
				if(len%2 == 0)
					train.m_nDirectFlag = UP_DIRECT;
				else
					train.m_nDirectFlag = DOWN_DIRECT;
			}
		}
		else
		{
			int Ynxt = GetStationVPos(second->in_station_index,  PRIMARY_TRACK);
			int Yrec = GetStationVPos(first->out_station_index,  PRIMARY_TRACK);
			
			if(Ynxt > Yrec)
			{
				train.m_nDirectFlag = DOWN_DIRECT;
			}
			else
			{
				train.m_nDirectFlag = UP_DIRECT;
			}
		}
	}

	// ����г�����仯,�������������г�����
	if(1 == mChangeDirectFlag)
	{
		TRAIN_DIRECT_DEFINE define;
		if(this->m_ptgctrl->mConfigTg.m_tgdef.GetTrainDirectDefine(this->m_nCurrentSectionNo, define))
		{
			for(UINT i = 0; i< nRecNum-1; i++)
			{
				TgTRAIN_RECORD *rec = train.GetRecordByPosition(i);
				TgTRAIN_RECORD *next = train.GetRecordByPosition(i+1);
				if(rec == NULL || next == NULL)
					continue;

				int k=0;
				while(k<define.direct_num)
				{
					if( rec->station != define.direct[k].begin_station || 
						rec->out_station_index != define.direct[k].begin_index ||
						next->station != define.direct[k].end_station ||
						next->in_station_index != define.direct[k].end_index)
					{
						k++;
						continue;
					}
					break;
				}

				if(k>=define.direct_num)
					continue;

				if(rec->out_station_index < next->in_station_index)
					train.m_nDirectFlag = DOWN_DIRECT;
				else
					train.m_nDirectFlag = UP_DIRECT;

				mChangeDirectFlag=0;
				break;
			}
		}
	}

	if(mChangeDirectFlag==0)
		train.mbDoubleDirect=false;
	else
		train.mbDoubleDirect=true;
}

void CTGDoc::SetTrainSideAndSchdReal(CTG_TRAIN& train)
{
	UINT nRecNum = train.GetRecordNum();  
	for(UINT i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = train.GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		rec->str_arrive_side = GetSideName(rec->station,rec->arrive_side).Trim();
		if(rec->arrive_side==rec->depart_side)
			rec->str_depart_side=rec->str_arrive_side;
		else
			rec->str_depart_side = GetSideName(rec->station,rec->depart_side).Trim();

		rec->nEngineCnt = GetEngineCount(&train, i);
	}
}

void CTGDoc::SetTrainWidthColor(CTG_TRAIN* ptrain)
{
	if(ptrain==NULL)
		return;
	// ������ɫ���߿�
	BYTE factor=ptrain->myTrainInfo.type;
	int nSpecialFlag = 0; 
	//���޵ȼ� 
	if(ptrain->myTrainInfo.military_rank >=1)
	{
		nSpecialFlag = 4;
	}
	else if(ptrain->myTrainInfo.super_rank >=1)
	{
		nSpecialFlag = 3;
	}
	else if(ptrain->myTrainInfo.length_rank >=1)
	{
		nSpecialFlag = 2;
	}
	else
	{
		nSpecialFlag = 0; 
	}
	
	COLORREF schd_color, real_color, vert_color;
	
	UINT nRecNum = ptrain->GetRecordNum(); 
	for(UINT i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = ptrain->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		int nFlag = nSpecialFlag;
		BYTE nDepartSpace=rec->GetDepartSpace();
		BYTE nArriveSpace=rec->GetArriveSpace();
		if(nDepartSpace>0||nArriveSpace>0)
		{
			nFlag = 1;
			rec->line_type=GetLineColor(factor, nFlag, schd_color, real_color, vert_color);
		}
		else
		{
			rec->line_type=GetLineColor(factor, nFlag, schd_color, real_color, vert_color);
		}

		// ϸ��
		if(GetContextLineWidth() == LINE::THIN)
		{
			if(1==nFlag)
			{
				rec->schd_width = line_style.space_rank.schd_width;
				rec->real_width = line_style.space_rank.real_width;
			}
			else if(2==nFlag)
			{
				rec->schd_width = line_style.length_rank.schd_width;
				rec->real_width = line_style.length_rank.real_width;
			}
			else if(3==nFlag)
			{
				rec->schd_width = line_style.super_rank.schd_width;
				rec->real_width = line_style.super_rank.real_width;
			}
			else if(4==nFlag)
			{
				rec->schd_width = line_style.military_rank.schd_width;
				rec->real_width = line_style.military_rank.real_width;
			}
			else
			{
				rec->schd_width = line_style.normal_rank.schd_width;
				rec->real_width = line_style.normal_rank.real_width;
			}
		}
		else
		{
			if(1==nFlag)
			{
				rec->schd_width = line_style.space_rank.thick_schd_width;
				rec->real_width = line_style.space_rank.thick_real_width;
			}
			else if(2==nFlag)
			{
				rec->schd_width = line_style.length_rank.thick_schd_width;
				rec->real_width = line_style.length_rank.thick_real_width;
			}
			else if(3==nFlag)
			{
				rec->schd_width = line_style.super_rank.thick_schd_width;
				rec->real_width = line_style.super_rank.thick_real_width;
			}
			else if(4==nFlag)
			{
				rec->schd_width = line_style.military_rank.thick_schd_width;
				rec->real_width = line_style.military_rank.thick_real_width;
			}
			else
			{
				rec->schd_width = line_style.normal_rank.thick_schd_width;
				rec->real_width = line_style.normal_rank.thick_real_width;
			}
		}

		rec->real_vert_color=vert_color;
		rec->schd_vert_color=vert_color;
		rec->real_color=real_color;
		rec->schd_color=schd_color;
		rec->factor=factor;
	}   
}

void CTGDoc::ResetLineWidth()
{
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); ++it)
	{
		SetTrainWidthColor(&(it->second));
	}
}

bool CTGDoc::FindTrainByTrainIndex(long& position, DWORD& train_index, const CString& trainId)
{
	//train_index = 0;  //2013.2.22hjz �ָ��г��������ҳ��ι���
	int nOffset = -1;

	map<TRAIN_KEY, CTG_TRAIN>::iterator it;

	if(train_index > 0)
	{
		it =  this->myPDataManage->myTrain.find(train_index);
		if(it == this->myPDataManage->myTrain.end())
			return false;
		
		TgTRAIN_RECORD	*first = (*it).second.GetFirstRecord();
		if(first == NULL)
			return false;

		train_index = (*it).first;

		if(IsFlagStart(first->flag))
			nOffset = GetOffsetMinuteFromGraphBegin(first->depart);
		else
			nOffset = GetOffsetMinuteFromGraphBegin(first->arrive);

		// �����г���ƫ��60����
		position = nOffset-FindTrainMoveDlt;
		return true;
	}
	else
		return false;
}

int CTGDoc::FindTrainByTrainId(const CString& trainId, long postion[], long indexlist[])
{
	int nOffset = -1;
	long train_index;
	map<TRAIN_KEY, CTG_TRAIN>::iterator it;

	std::vector<TRAIN_KEY> findVect;
	it =  this->myPDataManage->myTrain.begin();
	for(; it != this->myPDataManage->myTrain.end(); it++)
	{
		for(int idx=0; idx<(*it).second.GetRecordNum(); idx++)
		{
			if((*it).second.myRecord[idx].arrive_train_id == trainId ||
				(*it).second.myRecord[idx].depart_train_id == trainId ||
				(*it).second.myRecord[idx].original_arrive_train_id == trainId ||
				(*it).second.myRecord[idx].original_depart_train_id == trainId)
			{
				findVect.push_back((*it).first);
				break;
			}
		}
	}
		
	nOffset=-1;
	if(findVect.size()>0)
	{
		if(findVect.size()==1)
		{
			train_index = findVect[0];
			TgTRAIN_RECORD	*first = myPDataManage->GetTrain(train_index)->GetFirstRecord();
			if(first != NULL)
			{
				if(IsFlagStart(first->flag))
					nOffset = GetOffsetMinuteFromGraphBegin(first->depart);
				else
					nOffset = GetOffsetMinuteFromGraphBegin(first->arrive);

				// �����г���ƫ��60����
				postion[0] = nOffset-FindTrainMoveDlt;
				indexlist[0] = train_index;
			}
		}
		else
		{
			for (int i=0;i<findVect.size();i++)
			{
				train_index = findVect[i];
				TgTRAIN_RECORD	*first = myPDataManage->GetTrain(train_index)->GetFirstRecord();
				if(first != NULL)
				{
					if(IsFlagStart(first->flag))
						nOffset = GetOffsetMinuteFromGraphBegin(first->depart);
					else
						nOffset = GetOffsetMinuteFromGraphBegin(first->arrive);
				}

				// �����г���ƫ��60����
				postion[i] = nOffset-FindTrainMoveDlt;
				indexlist[i] = train_index;
			}
		}
	}

	return findVect.size();
}


int CTGDoc::FindTrainByTime(DWORD btm, DWORD etm, ULONG train_index[])
{
	long index;
	int train_count=0;
	map<TRAIN_KEY, CTG_TRAIN>::iterator it;

	std::vector<TRAIN_KEY> findVect;
	it =  this->myPDataManage->myTrain.begin();
	for(; it != this->myPDataManage->myTrain.end(); it++)
	{
		index = (*it).second.GetTrainIndex();
		CTG_TRAIN *train = &(it->second);
		if(train != NULL)
		{
			BOOL IsStart;
			TgTRAIN_RECORD *rec = train->GetFirstRecord();
			if(IsFlagStart(rec->flag))
				IsStart = TRUE;
			else
				IsStart = FALSE;

			if(IfTrainInTimeScope(train,btm,etm,IsStart))
			{
				train_index[train_count] = index;
				train_count++;
			}
		}
	}

	return train_count;
}

BOOL CTGDoc::IfTrainInTimeScope(CTG_TRAIN *ptrain, DWORD begin_time, DWORD end_time, BOOL IsStart)
{
	int rec_count=ptrain->GetRecordNum();
	if(rec_count<=0)
		return FALSE;
	int i;
	for( i=0;i<rec_count;i++)
	{
		if(IsStart && i > 0)
			return FALSE;
		TgTRAIN_RECORD *rec = ptrain->GetRecordByRecIndex(i);
		if (rec==NULL)
			return FALSE;
		if(IfRecordInTimeScope(rec,begin_time,end_time))
			break;			
	}
	if(i<rec_count)
		return TRUE;
	else
		return FALSE;
}

BOOL CTGDoc::IfRecordInTimeScope(TgTRAIN_RECORD *record, DWORD begin_time, DWORD end_time)
{
	if(record->arrive>=begin_time && record->arrive<=end_time)
		return TRUE;
	if(record->depart>=begin_time && record->depart<=end_time)
		return TRUE;
	if(record->arrive<=begin_time && record->depart>=end_time)
		return TRUE;
	return FALSE;
}

BOOL CTGDoc::IfTrainInStationScope(CTG_TRAIN *ptrain, int begin_sta_index, int end_sta_index,DWORD begin_time, DWORD end_time)
{
	BOOL bMatchResult=FALSE;
	int rec_count=ptrain->GetRecordNum();
	if(rec_count<=0)
		return FALSE;
	
	int sta_index_list[64];
	int sta_index_cnt=0;
	if (begin_sta_index<end_sta_index)
	{
		sta_index_cnt=end_sta_index-begin_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index+n;
		}
	}
	else if (begin_sta_index > end_sta_index)
	{
		sta_index_cnt=begin_sta_index-end_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index-n;
		}
	}
	
	int i;
	for( i=0;i<rec_count;i++)
	{
		TgTRAIN_RECORD *rec = ptrain->GetRecordByRecIndex(i);
		TgTRAIN_RECORD *next_rec = ptrain->GetRecordByRecIndex(i+1);
		if (rec==NULL)
			return FALSE;

		if (next_rec==NULL)
			return bMatchResult;

		if(IsActualArrivalTime(next_rec->adjust_status) || IsActualDepartTime(next_rec->adjust_status))
			continue;
		
		for (int x=0; x<sta_index_cnt; x++)
		{
			if (rec->out_station_index == sta_index_list[x] && x!=sta_index_cnt-1)
			{
				if(next_rec->in_station_index == sta_index_list[x+1])
				{
					if (IfRecordInTimeScope(rec, begin_time, end_time) && IfRecordInTimeScope(next_rec, begin_time, end_time))
					{
						bMatchResult=TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

// ���ƻ���ͣ����� ����
void CTGDoc::DrawStopOutofSignalMark(CDC *pCDC, CPoint ptlStart, CPoint ptlEnd, int direct)
{
	int delta = 20;
	CPoint ptl[4];

	ptl[0].x = (ptlStart.x + ptlEnd.x) /2;
	ptl[0].y = ptlStart.y;
	ptl[1].x = (ptlStart.x + ptlEnd.x) /2 + delta/2;
	ptl[1].y = ptlStart.y + delta;
	ptl[2].x = (ptlStart.x + ptlEnd.x) /2 - delta/2;
	ptl[2].y = ptlStart.y + delta;
	ptl[3].x = (ptlStart.x + ptlEnd.x) /2;
	ptl[3].y = ptlStart.y;
	
	if(direct == DOWN_DIRECT)
	{
		for(int i=0; i <4;i++)
		{
			//ptl[i].x = (ptlStart.x + ptlEnd.x) /2;
			ptl[i].y -= delta+3;
		}
	}
	else
	{
		for(int i=0; i <4;i++)
		{
			//ptl[i].x = (ptlStart.x + ptlEnd.x) /2;
			ptl[i].y += 3;
		}
	}


	CString stoptime;
	stoptime.Format("%d", (ptlEnd.x - ptlStart.x)/m_fPixelPerMinute);

	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	
	CFont font;
	font.CreateFontIndirect(&stFont);
	
	CPen mMyPen;
	mMyPen.CreatePen(PS_SOLID, 1, RGB(255,0,0));

	CFont* oldFont = pCDC->SelectObject(&font);
	CPen*  oldPen = pCDC->SelectObject(&mMyPen);
	int    oldAlign = pCDC->SetTextAlign(TA_CENTER|TA_TOP);

	pCDC->TextOut(ptl[0].x, ptl[0].y+delta/3, stoptime);
	pCDC->Polyline(ptl,4);

	pCDC->SetTextAlign(oldAlign);
	pCDC->SelectObject(oldPen);
	pCDC->SelectObject(oldFont);

	mMyPen.DeleteObject();
	font.DeleteObject();
}

bool CTGDoc::GetNodeIndexOfDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index)
{
	do
	{
		index = -1;

		if(single == NULL || rec == NULL)
			break;
		
		int station_no = rec->station;
		if(!this->m_ptgctrl->mConfigTg.m_tgdef.IsDefinePathStation(section, station_no))
			break;

		section_path_define route_define;
		if(!this->m_ptgctrl->mConfigTg.m_tgdef.GetRouteDefine(section, route_define))
			break;

		CString trainid = this->GetTrainShowId(single);
		int nTrainType = m_ptgctrl->mConfigTg.GetTrainTypeByTrainNO(trainid);

		// ����ƥ��ͳ����߻�����·
		if(nTrainType >= 128)
		{
			return GetNodeIndexOfGoodDefine(single, rec, exitEntryFlag, section, index, route_define);
		}
		else
		{
			return GetNodeIndexOfPessagerDefine(single, rec, exitEntryFlag, section, index, route_define);
		}

		
	}while(0);

	index = -1;
	return false;
}
// �������о�·
bool CTGDoc::GetNodeIndexOfAllDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, 
									 int exitEntryFlag, int section, 
									 int &index, section_path_define& route_define)
{
	int station_no = rec->station;
	// ����ÿһ����·����
	TgTRAIN_RECORD* itRd = NULL;
	for(int i=0; i<route_define.m_route.size(); i++)
	{
		bool bFind = false;
		RouteDefine& route = route_define.m_route[i];

		if(route.allSame)
		{
			itRd = single->GetFirstRecord();
			if(route.station_no[0] != itRd->station)
				continue;

			int p=0,k=0;
			for(p=0; (p < route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				if(route.station_no[p] == station_no)
					k=p;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p == route.station_num && itRd==NULL)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
		else
		{
			int k=0;
			for(k=0; k<route.station_num; k++)
			{
				if(route.station_no[k] == station_no)
				{
					bFind = true;
					break;
				}
			}


			if(!bFind)  continue;

			// ��ǰ��
			itRd = rec;
			int nSamePath = 0;
			for(int p=k; (p>=0 && itRd!=NULL); p--)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetPrevStationRecord(itRd);
			}

			if(p<0)
				nSamePath += 1;

			// ������
			itRd = rec;
			for(int p=k; (p<route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p>=route.station_num)
				nSamePath += 1;


			if(nSamePath==2)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
	}

	index = -1;

	return false;
}

// ���տͳ���·����
bool CTGDoc::GetNodeIndexOfGoodDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, 
									  int exitEntryFlag, int section, 
									  int &index, section_path_define& route_define)
{
	int station_no = rec->station;
	// ����ÿһ����·����
	TgTRAIN_RECORD* itRd = NULL;
	for(int i=0; i<route_define.m_route.size(); i++)
	{
		bool bFind = false;
		RouteDefine& route = route_define.m_route[i];
		if(!IsUsedByGoodTrain(route.routeFlag))
			continue;

		if(route.allSame)
		{
			itRd = single->GetFirstRecord();
			if(route.station_no[0] != itRd->station)
				continue;

			int p=0,k=0;
			for(p=0; (p < route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				if(route.station_no[p] == station_no)
					k=p;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p == route.station_num && itRd==NULL)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
		else
		{
			int k=0;
			for(k=0; k<route.station_num; k++)
			{
				if(route.station_no[k] == station_no)
				{
					bFind = true;
					break;
				}
			}


			if(!bFind)  continue;

			// ��ǰ��
			itRd = rec;
			int nSamePath = 0;
			for(int p=k; (p>=0 && itRd!=NULL); p--)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetPrevStationRecord(itRd);
			}

			if(p<0)
				nSamePath += 1;

			// ������
			itRd = rec;
			for(int p=k; (p<route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p>=route.station_num)
				nSamePath += 1;


			if(nSamePath==2)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
	}

	return GetNodeIndexOfAllDefine(single, rec, exitEntryFlag, section, index, route_define);
}

// ���ջ�����·����
bool CTGDoc::GetNodeIndexOfPessagerDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, 
										  int exitEntryFlag, int section, 
										  int &index, section_path_define& route_define)
{
	int station_no = rec->station;
	// ����ÿһ����·����
	TgTRAIN_RECORD* itRd = NULL;
	for(int i=0; i<route_define.m_route.size(); i++)
	{
		bool bFind = false;
		RouteDefine& route = route_define.m_route[i];
		if(!IsUsedByPessageTrain(route.routeFlag))
			continue;

		if(route.allSame)
		{
			itRd = single->GetFirstRecord();
			if(route.station_no[0] != itRd->station)
				continue;

			int p=0,k=0;
			for(p=0; (p < route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				if(route.station_no[p] == station_no)
					k=p;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p == route.station_num && itRd==NULL)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
		else
		{
			int k=0;
			for(k=0; k<route.station_num; k++)
			{
				if(route.station_no[k] == station_no)
				{
					bFind = true;
					break;
				}
			}


			if(!bFind)  continue;

			// ��ǰ��
			itRd = rec;
			int nSamePath = 0;
			for(int p=k; (p>=0 && itRd!=NULL); p--)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetPrevStationRecord(itRd);
			}

			if(p<0)
				nSamePath += 1;

			// ������
			itRd = rec;
			for(int p=k; (p<route.station_num && itRd!=NULL); p++)
			{
				if(itRd->station != route.station_no[p])
					break;
				itRd = single->GetNextStationRecord(itRd);
			}

			if(p>=route.station_num)
				nSamePath += 1;


			if(nSamePath==2)
			{
				if(exitEntryFlag == EXIT_POSITION)
					index = route.out_station_index[k];
				else
					index = route.in_station_index[k];
				return true;
			}
		}
	}

	return GetNodeIndexOfAllDefine(single, rec, exitEntryFlag, section, index, route_define);
}

void CTGDoc::ParseMarkReasonText(const CString& text, CString& dcmdno, CString& reason)
{
	dcmdno = "";
	reason = text;

	int pos = text.Find("^",0);
	if(pos <= 0)
		return;

	if(text.GetLength() > pos+1)
	{
		if(text.GetAt(pos+1) == '^')
		{
			dcmdno = text.Left(pos);
			reason = text.Right(text.GetLength() - pos - 2);
		}
	}
}

CString CTGDoc::CombineReasonText(const CString& dcmdno, const CString& reason)
{
	if(dcmdno.IsEmpty())
		return reason;
	return dcmdno + "^^" + reason;
}

void CTGDoc::DrawTrainRouteOKMark(CDC * pCDC,CTG_TRAIN * single)
{
	// ��·׼���ñ�־----0: ������ 1: ��ɫ�� 2: ԲȦ 3:J���� F�º�
	int flag = this->m_ptgctrl->mConfigTg.m_context.GetDrawRouteOkFlag();
	if(flag == 0 || m_bShowRouteTrigerOkFlag==FALSE)
		return;

	CPoint ptFirst(0,0), ptSecond(0,0);

	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		if(CTC_STATION != GetStationProperty(rec->station))
			continue;
		
		if(!IsArriveRouteOk(*rec) && !IsDepartRouteOk(*rec))
			continue;

		// ��ɫ������ʾ
		if(flag == 1)
		{
			if(rec->adjust_status & 0x0001)
				continue; 
			if(rec->adjust_status & 0x0002)
				continue;

			TgTRAIN_RECORD *pre = NULL;
			if(i>=1)
				pre = single->GetRecordByPosition(i-1);
			else
				pre = NULL;

			if(pre != NULL)
			{
				if(IsFlagStopAbs(pre->flag) || IsFlagStopSignal(pre->flag))
					continue;
				int Ypre = GetStationVPos(pre->out_station_index, PRIMARY_TRACK);
				int Yrec = GetStationVPos(rec->in_station_index, PRIMARY_TRACK);
				int nDirect;
				if(Ypre > Yrec)
					nDirect = UP_MARK;
				else
					nDirect = DOWN_MARK;

				ptFirst.x  = GetXPositionConsiderSecond(pre->depart);
				if(pre->in_station_index != pre->out_station_index)
				{
					if(nDirect == UP_MARK)
						ptFirst.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;
					else
						ptFirst.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
				}
				else
				{
					if(nDirect == UP_MARK)
						ptFirst.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;
					else
						ptFirst.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
				}

				ptSecond.x = GetXPositionConsiderSecond(rec->arrive);
				if(nDirect == UP_MARK)
					ptSecond.y = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;
				else
					ptSecond.y = this->m_sDispDistance[rec->in_station_index].nStationYPos;

				if( IsDepartRouteOk(*rec) || IsArriveRouteOk(*rec))   
				{
					CPen myPen;
					myPen.CreatePen(PS_SOLID, 2, CLR_PURPLE);
					CPen* pmyOldPen = pCDC->SelectObject(&myPen);
					UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

					pCDC->MoveTo(ptFirst.x, ptFirst.y);
					pCDC->LineTo(ptSecond.x, ptSecond.y);

					pCDC->SelectObject(pmyOldPen);
					pCDC->SetBkMode(nOldBackMode);
					myPen.DeleteObject();
				}
			}
		}
		else if(flag == 2)// ԲȦ
		{
			if(rec->adjust_status & 0x0001)
				continue; 
			if(rec->adjust_status & 0x0002)
				continue;

			CPen mPen;
			//�ӳ���·
			if(IsArriveRouteOk(*rec))
			{ 	
				if(IsAutoMakeArriveRoute(rec->stop_condition))
					mPen.CreatePen(PS_SOLID,2,CLR_BLACK);
				else
					mPen.CreatePen(PS_SOLID,2,CLR_RED); //���Զ������ú�ɫ

			}
			//������·
			else if(IsDepartRouteOk(*rec))
			{ 	
				if(IsAutoMakeDepartRoute(rec->stop_condition))
					mPen.CreatePen(PS_SOLID,2,CLR_BLACK);
				else
					mPen.CreatePen(PS_SOLID,2,CLR_RED);//���Զ������ú�ɫ
			}
			else 
			{
				mPen.CreatePen(PS_SOLID,2,CLR_BLACK);
			}

			SIZE size;
			size.cx =8;
			size.cy =8;

			CPen* pOldPen = pCDC->SelectObject(&mPen);
			UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

			int arriveOffset = GetXPositionConsiderSecond(rec->arrive);
			int nTmpInStaOffset = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK);
			if(nTmpInStaOffset > 0)
			{
				CPoint pt(arriveOffset+5, nTmpInStaOffset-5);

				CRect m_rect = CRect(pt,size);
				pCDC->Ellipse(m_rect); 
			}

			if(rec->in_station_index != rec->out_station_index && rec->out_station_index >= 0)
			{
				int nTmpOutStaOffset = GetStationVPos(rec->out_station_index, PRIMARY_TRACK);
				if(nTmpOutStaOffset > 0)
				{
					CPoint pt(arriveOffset+5, nTmpOutStaOffset-5);

					CRect m_rect = CRect(pt,size);
					pCDC->Ellipse(m_rect);
				}
			}

			pCDC->SelectObject (pOldPen);
			pCDC->SetBkMode(TRANSPARENT);
			mPen.DeleteObject();
		}
		else if(flag == 3)// J���� F�Ϻ�
		{
			CPoint pt1;
			CPoint pt2;
			
			bool bArriveReal=(rec->adjust_status & 0x0001)>0?true:false;
			bool bDepartReal=(rec->adjust_status & 0x0002)>0?true:false;
			
			TgTRAIN_RECORD *pre = NULL;
			if(i>=1)
				pre = single->GetRecordByPosition(i-1);
			else
				pre = NULL;

			if(pre!=NULL && IsArriveRouteOk(*rec) && bArriveReal==false) 
			{
				if(!IsFlagStopAbs(pre->flag) && !IsFlagStopSignal(pre->flag))
				{
					int Ypre = GetStationVPos(pre->out_station_index, PRIMARY_TRACK);
					int Yrec = GetStationVPos(rec->in_station_index, PRIMARY_TRACK);
					int nDirect;
					if(Ypre > Yrec)
						nDirect = UP_MARK;
					else
						nDirect = DOWN_MARK;

					pt1.x  = GetXPositionConsiderSecond(pre->depart);
					if(pre->in_station_index != pre->out_station_index)
					{
						if(nDirect == UP_MARK)
							pt1.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;
						else
							pt1.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
					}
					else
					{
						if(nDirect == UP_MARK)
							pt1.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;
						else
							pt1.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
					}

					pt2.x = GetXPositionConsiderSecond(rec->arrive);
					if(nDirect == UP_MARK)
						pt2.y = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;
					else
						pt2.y = this->m_sDispDistance[rec->in_station_index].nStationYPos;

					if(nDirect == DOWN_MARK)
					{
						pt1.x -= 3;
						pt2.x -= 3;
					}
					else
					{
						pt1.x += 3;
						pt2.x += 3;
					}
					CPen* pOldPen, pen;
					pen.CreatePen(PS_SOLID,2, mjcolor);
					pOldPen = pCDC->SelectObject(&pen);
					pCDC->MoveTo(pt1);
					pCDC->LineTo(pt2);
					pCDC->SelectObject(pOldPen);
					pen.DeleteObject();
				}
			}
			
			TgTRAIN_RECORD *next = NULL;
			if(i<nRecNum-1)
				next = single->GetRecordByPosition(i+1);
			else
				next = NULL;
			if(next!=NULL && IsDepartRouteOk(*rec) && bDepartReal==false)
			{
				if(!IsFlagStopAbs(rec->flag) && !IsFlagStopSignal(rec->flag))
				{
					int Ypre = GetStationVPos(rec->out_station_index, PRIMARY_TRACK);
					int Yrec = GetStationVPos(next->in_station_index, PRIMARY_TRACK);
					int nDirect;
					if(Ypre > Yrec)
						nDirect = UP_MARK;
					else
						nDirect = DOWN_MARK;

					pt1.x  = GetXPositionConsiderSecond(rec->depart);
					if(rec->in_station_index != rec->out_station_index)
					{
						if(nDirect == UP_MARK)
							pt1.y = this->m_sDispDistance[rec->out_station_index].nStationYPos;
						else
							pt1.y = this->m_sDispDistance[rec->out_station_index].nSecondStationYPos;
					}
					else
					{
						if(nDirect == UP_MARK)
							pt1.y = this->m_sDispDistance[rec->out_station_index].nStationYPos;
						else
							pt1.y = this->m_sDispDistance[rec->out_station_index].nSecondStationYPos;
					}

					pt2.x = GetXPositionConsiderSecond(next->arrive);
					if(nDirect == UP_MARK)
						pt2.y = this->m_sDispDistance[next->in_station_index].nSecondStationYPos;
					else
						pt2.y = this->m_sDispDistance[next->in_station_index].nStationYPos;

					if(nDirect == DOWN_MARK)
					{
						pt1.x += 3;
						pt2.x += 3;
					}
					else
					{
						pt1.x -= 3;
						pt2.x -= 3;
					}

					CPen* pOldPen, pen;
					pen.CreatePen(PS_SOLID,2, mfcolor);
					pOldPen = pCDC->SelectObject(&pen);
					pCDC->MoveTo(pt1);
					pCDC->LineTo(pt2);
					pCDC->SelectObject(pOldPen);
					pen.DeleteObject();
				}
			}
		}
	}
}

void CTGDoc::DrawTrainTrigerMark(CDC * pCDC,CTG_TRAIN * single)
{
	// ���ڻ��Ƶ�ʱ����ʾ���
	if(m_nOperatorStatus == INSERT_TRAIN)
		return;

	if(this->m_ptgctrl->mConfigTg.m_context.m_nRouteTrigerMode<=0 || m_bShowRouteNotAutoTrigerFlag==FALSE)
		return;

	int dlty = 7;
	BYTE nArlTrigeFlag=0;
	BYTE nDepTrigeFlag=0;

	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		if(CTC_STATION != GetStationProperty(rec->station))
			continue;
		
		if(IsAutoMakeArriveRoute(rec->stop_condition))
			nArlTrigeFlag=0;
		else
			nArlTrigeFlag=1;

		if(IsAutoMakeDepartRoute(rec->stop_condition))
			nDepTrigeFlag=0;
		else
			nDepTrigeFlag=1;
			
		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive);
		int departOffset = GetXPositionConsiderSecond(rec->depart);	

		if(i == 0 && IsFlagStart(rec->flag))
		{
			nArlTrigeFlag=0;
		}
		else if((nRecNum == i+1) && IsFlagTerminal(rec->flag))
		{
			nDepTrigeFlag=0;
		}
		
		if(nArlTrigeFlag>0 || nDepTrigeFlag>0)
		{
			CString strText="��";
			CFont *pOldFont = pCDC->SelectObject(&fontTech);		 
			UINT nOldColor = pCDC->SetTextColor(CLR_BLUE);
			UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
			UINT nAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			
			if(rec->in_station_index == rec->out_station_index)
			{
				if(rec->in_station_index >=0 && rec->in_station_index < this->m_nDispDistanceNum)
				{
					int nInStaOffset = 0;
					if(rec->in_sta_direct == UP_DIRECT)
					{
						if(nArlTrigeFlag>0)
						{
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;
							pCDC->SetTextColor(CLR_GREEN);
							//if(arrivalOffset == departOffset)
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							//else
							//    pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
							pCDC->TextOut(arrivalOffset, nInStaOffset-dlty, strText);
						}
						if(nDepTrigeFlag>0)
						{
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nStationYPos;
							pCDC->SetTextColor(CLR_BLACK);
							//if(arrivalOffset == departOffset)
								pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
							//else
							//	pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							pCDC->TextOut(departOffset, nInStaOffset+dlty, strText);
						}
					}
					else
					{
						if(nArlTrigeFlag>0)
						{
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nStationYPos;
							pCDC->SetTextColor(CLR_GREEN);
							//if(arrivalOffset == departOffset)
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							//else
							//	pCDC->SetTextAlign(TA_LEFT|TA_TOP);
							pCDC->TextOut(arrivalOffset, nInStaOffset+dlty, strText);
						}
						if(nDepTrigeFlag>0)
						{
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;
							pCDC->SetTextColor(CLR_BLACK);
							//if(arrivalOffset == departOffset)
								pCDC->SetTextAlign(TA_LEFT|TA_TOP);
							//else
							//	pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							pCDC->TextOut(departOffset, nInStaOffset-dlty, strText);
						}
					}
				}
			}
			else
			{
				if(rec->in_station_index >=0 && rec->in_station_index < this->m_nDispDistanceNum)
				{
					int nInStaOffset = 0;
					if(rec->in_sta_direct == UP_DIRECT)
					{
						nInStaOffset = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;
						if(nArlTrigeFlag>0)
						{
							pCDC->SetTextColor(CLR_GREEN);
							pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
							pCDC->TextOut(arrivalOffset, nInStaOffset, strText);
						}
					}
					else
					{
						nInStaOffset = this->m_sDispDistance[rec->in_station_index].nStationYPos;
						if(nArlTrigeFlag>0)
						{
							pCDC->SetTextColor(CLR_GREEN);
							pCDC->SetTextAlign(TA_LEFT|TA_TOP);
							pCDC->TextOut(arrivalOffset, nInStaOffset, strText);
						}
					}
				}
				
				if(rec->out_station_index >=0 && rec->out_station_index < this->m_nDispDistanceNum)
				{
					int nOutStaOffset = 0;
					if(rec->out_sta_direct == UP_DIRECT)
					{
						nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nStationYPos;
						if(nDepTrigeFlag>0)
						{
							pCDC->SetTextColor(CLR_BLACK);
							pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							pCDC->TextOut(departOffset, nOutStaOffset, strText);
						}
					}
					else
					{
						nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nSecondStationYPos;
						if(nDepTrigeFlag>0)
						{
							pCDC->SetTextColor(CLR_BLACK);
							pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							pCDC->TextOut(departOffset, nOutStaOffset, strText);
						}
					}
				}
			}
			pCDC->SelectObject(pOldFont);
			pCDC->SetTextColor(nOldColor);
			pCDC->SetBkMode(nOldBackMode);
			pCDC->SetTextAlign(nAlign);
		}
	}
}

void CTGDoc::DrawTrainTechDepartMark(CDC * pCDC,CTG_TRAIN * single)
{
	//���ڻ��Ƶ�ʱ����ʾ���
	if(m_nOperatorStatus == INSERT_TRAIN)
		return;

	int dltx = 5;
	// ���ڼ���ͣ�㣬 ����CTC����ͣ�����"��" �ϲ�CTC�Ǽ���ͣ�����"��"
	int flag = -1;
	if(m_nShowWorkTechFlag)
	{
		flag=this->m_ptgctrl->mConfigTg.m_context.GetDrawTenicalStopMark();
	}

	CString strText,strTech;

	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		TgTRAIN_RECORD *rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		strTech="";
		if(flag > 0 && IsTechStopTrain(*rec))
		{  
			strTech = "��";
		}
		else if(flag == 0 && !IsTechStopTrain(*rec))
		{  
			strTech = "��";
		}

		if(strTech != "")
		{
			if(this->m_ptgctrl->mConfigTg.m_context.IsTechStopFlagNotSame())
			{
				if(rec->in_sta_direct == DOWN_DIRECT)
					strTech = "��";
				else
					strTech = "��";
			}
		}

		strText="";
		if(m_bShowDepartFlag)
		{
			if(IsMakeDepartRouteByPlan(rec->stop_condition))
				strText ="*";
		}
		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive);
		int departOffset = GetXPositionConsiderSecond(rec->depart);	

		if(i == 0 && IsFlagStart(rec->flag))
		{
			;//strTech.Empty();// ʼ��վ��ʾӪҵ���Ƿ���
		}
		else if((nRecNum == i+1) && IsFlagTerminal(rec->flag))
		{
			strText.Empty();
		}
		else
		{
			if(arrivalOffset == departOffset)
			{
				strText.Empty();
			}
		}

		if(CTC_STATION != GetStationProperty(rec->station))
			strText.Empty();

		if(strText.IsEmpty())
			strText = strTech;
		else if(!strTech.IsEmpty())
			strText += strTech;

		if(!strText.IsEmpty())
		{
			bool bSameSta = true;
			if(rec->in_station_index >=0 && rec->in_station_index < this->m_nDispDistanceNum)
			{
				if(rec->out_station_index >=0 && rec->out_station_index < this->m_nDispDistanceNum)
				{
					if(rec->in_station_index != rec->out_station_index)
						bSameSta = false;
				}
			}
			else
			{
				if(rec->out_station_index >=0 && rec->out_station_index < this->m_nDispDistanceNum)
				{
					;
				}
				else
				{
					continue;
				}
			}

			CFont *pOldFont = pCDC->SelectObject(&fontTech);		 
			UINT nOldColor = pCDC->SetTextColor(CLR_BLUE);
			UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
			UINT nAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			if(this->m_ptgctrl->mConfigTg.m_context.IsTechStopFlagNotSame())
			{
				if(rec->in_sta_direct == DOWN_DIRECT)
					nAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				else
					nAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
			}

			if(bSameSta)
			{
				if(rec->in_station_index >=0 && rec->in_station_index < this->m_nDispDistanceNum)
				{
					int nInStaOffset = 0;
					if(rec->in_sta_direct == UP_DIRECT)
						nInStaOffset = this->m_sDispDistance[rec->in_station_index].nStationYPos;
					else
						nInStaOffset = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;

					pCDC->TextOut(departOffset+dltx, nInStaOffset, strText);
				}
				else if(rec->out_station_index >=0 && rec->out_station_index < this->m_nDispDistanceNum)
				{
					int nOutStaOffset = 0;
					if(rec->out_sta_direct == UP_DIRECT)
						nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nStationYPos;
					else
						nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nSecondStationYPos;

					pCDC->TextOut(departOffset+dltx, nOutStaOffset, strText);
				}
			}
			else
			{
				int nStopFlag = m_ptgctrl->mConfigTg.m_tgdef.GetStationStopLine(m_nCurrentSectionNo, 
													rec->station, rec->in_station_index, 
													rec->out_station_index);
				if(1 == nStopFlag)
				{
					if(rec->in_station_index >=0 && rec->in_station_index < this->m_nDispDistanceNum)
					{
						int nInStaOffset = 0;
						if(rec->in_sta_direct == UP_DIRECT)
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nStationYPos;
						else
							nInStaOffset = this->m_sDispDistance[rec->in_station_index].nSecondStationYPos;

						pCDC->TextOut(departOffset+dltx, nInStaOffset, strText);
					}
				}
				else
				{
					if(rec->out_station_index >=0 && rec->out_station_index < this->m_nDispDistanceNum)
					{
						int nOutStaOffset = 0;
						if(rec->out_sta_direct == UP_DIRECT)
							nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nStationYPos;
						else
							nOutStaOffset = this->m_sDispDistance[rec->out_station_index].nSecondStationYPos;

						pCDC->TextOut(departOffset+dltx, nOutStaOffset, strText);
					}
				}
			}
			pCDC->SelectObject(pOldFont);
			pCDC->SetTextColor(nOldColor);
			pCDC->SetBkMode(nOldBackMode);
			pCDC->SetTextAlign(nAlign);
		}
	}
}

void CTGDoc::GetSepState(int index, bool& bIndexSep)
{
	bIndexSep = false;
	
	const node_info *pNodeInfo = this->m_ptgctrl->mConfigTg.m_tgdef.GetNodeByIndex(m_nCurrentSectionNo, index);
	if(pNodeInfo != NULL)
	{
		if(pNodeInfo->show_sation_sep > 0)
			bIndexSep = true;
	}
}

void CTGDoc::DrawTrainWorkText(CDC * pCDC,CTG_TRAIN * single)
{
	int dltx = 5;
	int dlty = 0;
	int nInStaOffset=0;
	int nOutStaOffset=0;

	int  nOldColor = pCDC->SetTextColor(RGB(255,0,0));
	if(this->m_ptgctrl->mConfigTg.m_context.IsHrbTransportColor())
		pCDC->SetTextColor(RGB(0,0,128));
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pCDC->SelectObject(&fontText);

	int nTrainDirect = single->m_nDirectFlag;

	TgTRAIN_RECORD *rec = NULL, *next=NULL;
	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		CString strAlarmText = rec->szAlarmText;
		if(!this->m_bShowConflict)
			strAlarmText.Empty();

		CString strWorkText;
		CString tmp_opms_text;
		if (this->m_bShowOPMSTrainsInfo && rec->szOpmsStationText!="")
			tmp_opms_text = "("+rec->szOpmsStationText+")";

		if(!rec->szStationText.IsEmpty())
		{
			if(!strAlarmText.IsEmpty())
				strWorkText = rec->szStationText + tmp_opms_text + "/" + strAlarmText;
			else
				strWorkText = rec->szStationText + tmp_opms_text;
		}
		else
		{
			strWorkText = tmp_opms_text +strAlarmText;
		}
		if(strWorkText.IsEmpty())
			continue;

		next = single->GetRecordByPosition(i+1);
		if(next != NULL)
		{
			if(nRecNum == 2 && rec->station == next->station)
			{
				nTrainDirect = single->m_nDirectFlag;
			}
			else
			{
				int Yrec = GetStationVPos(rec->out_station_index,  PRIMARY_TRACK);
				int Ynxt = GetStationVPos(next->in_station_index,  PRIMARY_TRACK);
				if(Yrec > Ynxt)
					nTrainDirect = UP_DIRECT;
				else
					nTrainDirect = DOWN_DIRECT;
			}
		}

		nInStaOffset = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK)+dlty;
		nOutStaOffset = -1;
		// ��ںͳ�����ͬ
		if(rec->in_station_index != rec->out_station_index)
		{
			nOutStaOffset = GetStationVPos(rec->out_station_index, PRIMARY_TRACK)+dlty;
		}

		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive)+dltx;
		int departOffset = GetXPositionConsiderSecond(rec->depart)+dltx;	
		int Xpos = arrivalOffset + 5;//(arrivalOffset+departOffset)/2;
		CSize sz=pCDC->GetTextExtent(strWorkText);
		if(nTrainDirect == UP_DIRECT)
		{
			pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		}
		else
		{
			pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		}

		if(arrivalOffset == departOffset)
		{
			if(nRecNum == (i+1))
			{
				if(rec->in_station_index >=0)
				{
					pCDC->TextOut(Xpos, nInStaOffset, strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect=CRect(Xpos, nInStaOffset-sz.cy, Xpos+sz.cx, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				// ��ںͳ��ڲ���ͬ
				if(rec->in_station_index != rec->out_station_index && rec->out_station_index>=0)
				{
					pCDC->TextOut(Xpos, nOutStaOffset,  strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect=CRect(Xpos, nOutStaOffset-sz.cy, Xpos+sz.cx, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
			else
			{
				if(rec->in_station_index >=0 )
				{
					pCDC->TextOut(Xpos, nInStaOffset, strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect=CRect(Xpos, nInStaOffset-sz.cy, Xpos+sz.cx, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				// ��ںͳ��ڲ���ͬ
				if(rec->in_station_index != rec->out_station_index && rec->out_station_index>=0)
				{
					pCDC->TextOut(Xpos, nOutStaOffset,  strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect=CRect(Xpos, nOutStaOffset-sz.cy, Xpos+sz.cx, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
		}
		else
		{
			if(rec->in_station_index >=0)
			{
				pCDC->TextOut(Xpos, nInStaOffset, strWorkText);

				if(m_bIsVerifyTrainRect)
				{
				CRect rect=CRect(Xpos, nInStaOffset-sz.cy, Xpos+sz.cx, nInStaOffset+sz.cy);
				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			}
			// ��ںͳ��ڲ���ͬ
			if(rec->in_station_index != rec->out_station_index  && rec->out_station_index>=0)
			{
				if(m_nTrainLeapLineFlag == 2)
				{
					pCDC->TextOut(Xpos, nOutStaOffset, strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect=CRect(Xpos, nOutStaOffset-sz.cy, Xpos+sz.cx, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
		}
	}

	// �ָ�
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
}

// �����г��ɵ�
void CTGDoc::DrawTrainWorkSide(CDC * pCDC,CTG_TRAIN * single,int nDrawMode)
{
	CString a_side;
	CString d_side;

	int nInStaOffset=0;
	int nOutStaOffset=0;

	int  nOldColor = pCDC->SetTextColor(RGB(0,0,255));
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = NULL;
	pOldFont = pCDC->SelectObject(&fontSide);
	
	TgTRAIN_RECORD *rec = NULL;
	int nRecNum = single->GetRecordNum();
	int nOldDrawMode = nDrawMode;
	int in_station_index=-1,out_station_index=-1;
	for(int i = 0; i< nRecNum; i++)
	{
		rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		if(CTC_STATION != GetStationProperty(rec->station))
			continue;
		
		if(this->m_ptgctrl->mConfigTg.m_context.IsNotShowSideCtcStation(rec->station))
			continue;

		if(rec->in_station_index>=0)
			in_station_index = rec->in_station_index;
		else
			in_station_index = rec->out_station_index;

		if(rec->out_station_index>=0)
			out_station_index = rec->out_station_index;
		else
			out_station_index = rec->in_station_index;

		if(in_station_index < 0 || out_station_index < 0)
			continue;

		bool bLock=(IsForceSide(*rec)==0) ? false : true;

		pCDC->SetTextColor(RGB(0,0,255));
		nDrawMode = nOldDrawMode;

		COLORREF clr;
		if(this->m_ptgctrl->mConfigTg.m_context.IsShowSideEntryNoPassFlag(clr))
		{
			int flag=0;
			if (!CheckSideEntry(rec->arrive_side, rec->depart_side, rec, flag))
			{
				if(0==i && IsFlagStart(rec->flag))
				{
					if(flag==2)
					{
						pCDC->SetTextColor(clr);
						nDrawMode = DRAW_SCREEN;
					}
				}
				if(i==nRecNum-1 && IsFlagTerminal(rec->flag))
				{
					if(flag==1)
					{
						pCDC->SetTextColor(clr);
						nDrawMode = DRAW_SCREEN;
					}
				}
				else
				{
					pCDC->SetTextColor(clr);
					nDrawMode = DRAW_SCREEN;
				}
			}
		
			if(IsDispalySide(rec->stop_condition) || IsDispalySide2(rec->stop_condition))
			{
				pCDC->SetTextColor(clr);
				nDrawMode = DRAW_SCREEN;
			}
		}
				
		a_side = rec->str_arrive_side;
		d_side = rec->str_depart_side;

		if(a_side =="?G")
		{
			if(!(m_nShowErrWorkSideFlag || m_nShowWorkSideFlag))
			{
				a_side.Empty();
			}
		}
		else 
		{
			if(!((nDrawMode & DRAW_SCREEN) || m_nShowWorkSideFlag))
			{
				a_side.Empty();
			}
		}

		if(d_side =="?G")
		{
			if(!(m_nShowErrWorkSideFlag || m_nShowWorkSideFlag))
			{
				d_side.Empty();
			}
		}
		else 
		{
			if(!((nDrawMode & DRAW_SCREEN) || m_nShowWorkSideFlag))
			{
				d_side.Empty();
			}
		}

		if(a_side =="" && d_side =="") // �Ż�Ч�� --- �ɵ�Ϊ�ղ������»��ɵ�
		{
			continue;
		}
		
		if(bLock)
		{
			if(!a_side.IsEmpty())
				a_side+="(��)";
			if(!d_side.IsEmpty())
				d_side+="(��)";
		}
		
		nInStaOffset  = GetStationSideVPos(rec, in_station_index, rec->arrive_side);
		nOutStaOffset = GetStationSideVPos(rec, out_station_index, rec->depart_side);
		
		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive);
		int departOffset = GetXPositionConsiderSecond(rec->depart);
		
		if(rec->in_sta_direct == UP_DIRECT)
			pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		else
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		
		int vpos=-10;
		if(0!=m_nADTimeSystle) // ����ʱ����ʾ��ʱ,�ɵ��ڵ�����ʱ����ʾ
		{
			vpos=-25;
		}
		if(in_station_index == out_station_index)
		{
			if(a_side == d_side)
			{
				pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, a_side);
				
				if(m_bIsVerifyTrainRect)
				{
				CSize sz=pCDC->GetTextExtent(a_side);
				CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			}
			else
			{
				CString str;
				str.Format("%s��%s��", a_side, d_side);
				pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, str);

				if(m_bIsVerifyTrainRect)
				{
				CSize sz=pCDC->GetTextExtent(str);
				CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			}
		}
		else
		{
			if(m_nTrainLeapLineFlag == 3)
			{
				if(a_side == d_side)
				{
					pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, a_side);
					pCDC->TextOut(departOffset+vpos, nOutStaOffset,  a_side);

					if(m_bIsVerifyTrainRect)
					{
					CSize sz=pCDC->GetTextExtent(a_side);
					CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);

					rect = CRect(departOffset+vpos-sz.cx, nOutStaOffset-sz.cy, departOffset+vpos, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				else
				{
					CString str;
					str.Format("%s��%s��", a_side, d_side);
					pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, str);
					pCDC->TextOut(departOffset+vpos, nOutStaOffset,  str);

					if(m_bIsVerifyTrainRect)
					{
					CSize sz=pCDC->GetTextExtent(str);
					CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);

					rect = CRect(departOffset+vpos-sz.cx, nOutStaOffset-sz.cy, departOffset+vpos, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
			else
			{
				if(a_side == d_side)
				{
					pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, a_side);
					if(rec->in_sta_direct == UP_DIRECT)
						pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					else
						pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					pCDC->TextOut(arrivalOffset+vpos, nOutStaOffset,  a_side);

					if(m_bIsVerifyTrainRect)
					{
					CSize sz=pCDC->GetTextExtent(a_side);
					CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);

					rect = CRect(arrivalOffset+vpos-sz.cx, nOutStaOffset-sz.cy, arrivalOffset+vpos, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				else
				{
					CString str;
					str.Format("%s��%s��", a_side, d_side);
					pCDC->TextOut(arrivalOffset+vpos, nInStaOffset, str);
					if(rec->in_sta_direct == UP_DIRECT)
						pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					else
						pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					pCDC->TextOut(arrivalOffset+vpos, nOutStaOffset,  str);

					if(m_bIsVerifyTrainRect)
					{
					CSize sz=pCDC->GetTextExtent(str);
					CRect rect = CRect(arrivalOffset+vpos-sz.cx, nInStaOffset-sz.cy, arrivalOffset+vpos, nInStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);

					rect = CRect(arrivalOffset+vpos-sz.cx, nOutStaOffset-sz.cy, arrivalOffset+vpos, nOutStaOffset+sz.cy);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
		}
	}

	// �ָ�
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
}

void CTGDoc::SetTrainRect(CTG_TRAIN* ptrain)
{
	if(ptrain==NULL)
		return;
	if(m_bIsVerifyTrainRect)
	{
		if(GetTgType() == TG_TYPE::BASIC_TG)
			SetBasicTrainRect(ptrain);
		else
			SetWorkTrainRect(ptrain);
		return;
	}
	
	UINT RecordNum = ptrain->GetRecordNum();
	if(RecordNum < 1)
		return;

	// -----------------------------------------------------------
	for(int i = 0; i < MAX_DAY_OF_BASIC_TRAIN; i++)
		ptrain->myPosition[i].SetRectEmpty();

	ptrain->days = 0;

	if(GetTgType() == TG_TYPE::BASIC_TG)
	{
		SetBasicTrainDate(ptrain);

		int mix_index=999, max_index=0;
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
			if(curr == NULL)
				continue;

			if(curr->in_station_index >=0)
			{
				if(curr->in_station_index > max_index)
					max_index = curr->in_station_index;

				if(curr->in_station_index < mix_index)
					mix_index = curr->in_station_index;
			}

			if(curr->out_station_index >=0)
			{
				if(curr->out_station_index > max_index)
					max_index = curr->out_station_index;

				if(curr->out_station_index < mix_index)
					mix_index = curr->out_station_index;
			}
		}

		int top,bottom;
		if(mix_index < this->m_nDispDistanceNum && max_index < this->m_nDispDistanceNum)
		{
			top = this->m_sDispDistance[mix_index].nStationYPos - 150;
			bottom = this->m_sDispDistance[max_index].nSecondStationYPos + 150;
		}
		else
		{
			top = this->m_sDispDistance[0].nStationYPos - 150;
			bottom = this->m_sDispDistance[this->m_nDispDistanceNum-1].nSecondStationYPos + 150;
		}
			
		int left = 9999999,right=0;
		int arl=0, dep=0;
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
			if(curr == NULL)
				continue;
			
			if(idx == 0)
			{
				if(IsFlagAbsStart(curr->flag))
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;
					dep = arl;
				}
				else if(IsFlagStart(curr->flag))
				{
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					arl = dep;
				}
				else
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					if(IsFlagGiveout(curr->flag))
						dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					else
						dep = arl;
				}
			}
			if(idx == RecordNum-1)
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);

				if(IsFlagAbsTerminal(curr->flag))
					dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
				else if(IsFlagGiveout(curr->flag))
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				else
					dep = arl;
			}
			if(idx > 0 && idx < RecordNum-1)
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
			}

			if(arl < left)
				left = arl;
			if(dep > right)
				right = dep;
		}

		left -= 120;
		right += 80;

		int nWholePixel=24*this->m_fPixelPerHour;
		if(top<0) top=0;
		if(left<0)	left=0;
		if(right>nWholePixel) right=nWholePixel;
		if(bottom>m_nPageHeight) bottom=m_nPageHeight;

		ptrain->days=1;
	
		TgTRAIN_RECORD* first = ptrain->GetFirstRecord();
		TgTRAIN_RECORD* last  = ptrain->GetLastRecord();
		if(first!=NULL && last!=NULL && first!=last)
		{
			long nleft=0,nright=0,dlt;
			if(IsFlagStart(first->flag))
			{
				dlt=first->depart;
				nleft = this->GetOffsetPixelFromGraphBegin(first->depart);
			}
			else
			{
				dlt=first->arrive;
				nleft = this->GetOffsetPixelFromGraphBegin(first->arrive);
			}

			if(IsFlagTerminal(last->flag))
			{
				dlt=last->arrive-dlt;
				nright = this->GetOffsetPixelFromGraphBegin(last->arrive);
			}
			else
			{
				dlt=last->depart-dlt;
				nright = this->GetOffsetPixelFromGraphBegin(last->depart);
			}

			if(dlt>=86400)
				ptrain->myPosition[0].SetRect(0, top, nWholePixel, bottom);
			else
			{
				if(nright<nleft)
				{
					nleft-=120;
					if(nleft<0)
						nleft=0;

					nright+=100;
					if(nright>nWholePixel)
						nright=nWholePixel;
					ptrain->myPosition[0].SetRect(nleft, top, nWholePixel, bottom);
					ptrain->myPosition[1].SetRect(0, top, nright, bottom);
					ptrain->days=2;
				}
				else
					ptrain->myPosition[0].SetRect(left, top, right, bottom);
			}
		}
		else
		{
			ptrain->myPosition[0].SetRect(left, top, right, bottom);
		}
	}
	else
	{
		int mix_index=999, max_index=0;
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
			if(curr == NULL)
				continue;

			if(curr->in_station_index >=0)
			{
				if(curr->in_station_index > max_index)
					max_index = curr->in_station_index;

				if(curr->in_station_index < mix_index)
					mix_index = curr->in_station_index;
			}

			if(curr->out_station_index >=0)
			{
				if(curr->out_station_index > max_index)
					max_index = curr->out_station_index;

				if(curr->out_station_index < mix_index)
					mix_index = curr->out_station_index;
			}
		}
		int top,bottom;
		if(mix_index < this->m_nDispDistanceNum && max_index < this->m_nDispDistanceNum)
		{
			top = this->m_sDispDistance[mix_index].nStationYPos - 250;
			bottom = this->m_sDispDistance[max_index].nSecondStationYPos + 250;
		}
		else
		{
			top = this->m_sDispDistance[0].nStationYPos - 250;
			bottom = this->m_sDispDistance[this->m_nDispDistanceNum-1].nSecondStationYPos + 250;
		}
		
		int left = 9999999,right=0;
        int arl=0, dep=0;
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
			if(curr == NULL)
				continue;
			
			if(idx == 0)
			{
				if(IsFlagAbsStart(curr->flag))
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					dep = arl;
					arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;	
				}
				else if(IsFlagStart(curr->flag))
				{
				    dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					arl = dep;
				}
				else
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					if(IsFlagGiveout(curr->flag))
						dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					else
					    dep = arl;
				}
			}
			if(idx == RecordNum-1)
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);

				if(IsFlagAbsTerminal(curr->flag))
					dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
				else if(IsFlagGiveout(curr->flag))
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				else
					dep = arl;
			}
			if(idx > 0 && idx < RecordNum-1)
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
			}

			if(arl < left)
				left = arl;
			if(dep > right)
				right = dep;
		}

		if(1 == RecordNum) // 2016-12-14 ��������ʼ��
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(0);
			if(curr != NULL)
			{
				if(IsFlagAbsStart(curr->flag))
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;	
				}
				
				if(IsFlagAbsTerminal(curr->flag))
				{
					arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
					dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
				}

				if(arl < left)
					left = arl;
				if(dep > right)
					right = dep;
			}
		}
		
		TgTRAIN_RECORD* first = ptrain->GetFirstRecord();
		if(NULL != first)
		{
			// recuse train
			if(IsFlagRescue(first->flag) && first->special_train_index > 0)
			{
				CTG_TRAIN* rescuedtrain = this->GetTrainByIndex(first->special_train_index);
				if(NULL != rescuedtrain)
				{
					TgTRAIN_RECORD* last = rescuedtrain->GetLastRecord();
					if(last != NULL && IsFlagRescued(last->flag))
					{
						int pos = this->GetOffsetPixelFromGraphBegin(last->depart)+m_fPixelPerMinute*last->stop_abs_e_time;
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}

			// divide train
			if(IsFlagDivide(first->flag) && first->special_train_index > 0)
			{
				ULONG train_index = 0;
				CTG_TRAIN* dividetrain1 = this->GetTrainByIndex(first->special_train_index);
				if(NULL != dividetrain1)
				{
					TgTRAIN_RECORD* first = dividetrain1->GetFirstRecord();
					TgTRAIN_RECORD* last = dividetrain1->GetLastRecord();
					if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index > 0)
					{
						train_index = first->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
					else if(last != NULL && IsFlagDivided(last->flag) && last->special_train_index > 0)
					{
						train_index = last->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}

				CTG_TRAIN* dividetrain2 = this->GetTrainByIndex(train_index);
				if(NULL != dividetrain2)
				{
					TgTRAIN_RECORD* first = dividetrain2->GetFirstRecord();
					TgTRAIN_RECORD* last = dividetrain2->GetLastRecord();
					if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
					else if(last != NULL && IsFlagDivided(last->flag) && last->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}

			// combine train
			if(IsFlagCombine(first->flag) && first->special_train_index > 0)
			{
				ULONG train_index = 0;
				CTG_TRAIN* combinetrain1 = this->GetTrainByIndex(first->special_train_index);
				if(NULL != combinetrain1)
				{
					TgTRAIN_RECORD* last = combinetrain1->GetLastRecord();
					if(last != NULL && IsFlagCombined(last->flag) && last->special_train_index > 0)
					{
						train_index = last->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}

				CTG_TRAIN* combinetrain2 = this->GetTrainByIndex(train_index);
				if(NULL != combinetrain2)
				{
					TgTRAIN_RECORD* last = combinetrain2->GetLastRecord();
					if(last != NULL && IsFlagCombined(last->flag) && last->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}
		}
		
		TgTRAIN_RECORD* last = ptrain->GetLastRecord();
		if(last != NULL)
		{
			// rescue train
			if(IsFlagRescued(last->flag) && last->special_train_index > 0)
			{
				CTG_TRAIN* rescuetrain = this->GetTrainByIndex(last->special_train_index);
				if(NULL != rescuetrain)
				{
					TgTRAIN_RECORD* first = rescuetrain->GetFirstRecord();
					if(first != NULL && IsFlagRescue(first->flag))
					{
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart)+m_fPixelPerMinute*first->stop_abs_s_time;
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}

			// unit train
			if(IsFlagUnitOtherTrain(last->flag) && last->special_train_index > 0)
			{
				CTG_TRAIN* unittrain = this->GetTrainByIndex(last->special_train_index);
				if(NULL != unittrain)
				{
					TgTRAIN_RECORD* midrec = unittrain->GetStationRecord(last->station);
					if(midrec != NULL && IsFalgBeUnitedByOther(midrec->flag))
					{
						int pos = this->GetOffsetPixelFromGraphBegin(midrec->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}
			// divided train
			if(IsFlagDivided(last->flag) && last->special_train_index > 0)
			{
				CTG_TRAIN* dividetrain1 = this->GetTrainByIndex(last->special_train_index);
				ULONG train_index = 0;
				if(NULL != dividetrain1)
				{
					TgTRAIN_RECORD* first = dividetrain1->GetFirstRecord();
					if(first != NULL && IsFlagDivide(first->flag))
					{
						train_index = first->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
				
				CTG_TRAIN* dividetrain2 = this->GetTrainByIndex(train_index);
				if(NULL != dividetrain2)
				{
					TgTRAIN_RECORD* first = dividetrain2->GetFirstRecord();
					if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}

			// combine train
			if(IsFlagCombined(last->flag) && last->special_train_index > 0)
			{
				ULONG train_index = 0;
				CTG_TRAIN* combinetrain1 = this->GetTrainByIndex(last->special_train_index);
				if(NULL != combinetrain1)
				{
					TgTRAIN_RECORD* first = combinetrain1->GetFirstRecord();
					TgTRAIN_RECORD* last = combinetrain1->GetLastRecord();
					if(first != NULL && IsFlagCombine(first->flag) && first->special_train_index > 0)
					{
						train_index = first->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
					else if(last != NULL && IsFlagCombined(last->flag) && last->special_train_index > 0)
					{
						train_index = last->special_train_index;
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}

				CTG_TRAIN* combinetrain2 = this->GetTrainByIndex(train_index);
				if(NULL != combinetrain2)
				{
					TgTRAIN_RECORD* first = combinetrain2->GetFirstRecord();
					TgTRAIN_RECORD* last  = combinetrain2->GetLastRecord();
					if(first != NULL && IsFlagCombine(first->flag) && first->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
					else if(last != NULL && IsFlagCombined(last->flag) && last->special_train_index == ptrain->m_nTrainIndex)
					{
						int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
				}
			}
		}
		
		// unit train
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* midrec = ptrain->GetRecordByPosition(idx);
			if(midrec == NULL)
				continue;
			if(IsFalgBeUnitedByOther(midrec->flag))
			{
				CTG_TRAIN* unitedtrain = this->GetTrainByIndex(midrec->special_train_index);
				if(NULL != unitedtrain)
				{
					TgTRAIN_RECORD* last = unitedtrain->GetLastRecord();
					if(last != NULL && IsFlagUnitOtherTrain(last->flag))
					{
						int pos = this->GetOffsetPixelFromGraphBegin(last->depart)+m_fPixelPerMinute*last->stop_abs_s_time;
						if(pos < left)
							left = pos;
						if(pos > right)
							right = pos;
					}
					break;
				}
			}
		}

		left -= 120;
		right += 80;
		if(top<0)	top=0;
		if(left<0)	left=0;

		ptrain->myPosition[0].SetRect(left, top, right, bottom);
		ptrain->days=1;

		// �򵥹���������Ϣ�ͳ�վ��Ϣ����
		CRect myStaTextRect, myAbsTextRect;
		TgTRAIN_RECORD* cur = NULL, *next = NULL;
		for(int idx=0; idx<RecordNum; idx++)
		{
			next = ptrain->GetRecordByPosition(idx+1);
			cur = ptrain->GetRecordByPosition(idx);
			if(NULL == cur) 
				continue;

			myStaTextRect.top   = 0;
			myStaTextRect.bottom  = 0;
			myStaTextRect.left  = 0;
			myStaTextRect.right = 0;

			myAbsTextRect.top   = 0;
			myAbsTextRect.bottom  = 0;
			myAbsTextRect.left  = 0;
			myAbsTextRect.right = 0;

			CString strStaText="";
			if(this->m_bShowOPMSTrainsInfo)
				strStaText = cur->szAlarmText + cur->szStationText + cur->szOpmsStationText;
			else
				strStaText = cur->szAlarmText + cur->szStationText;

			if(!strStaText.IsEmpty())
			{
				int arrivalOffset = m_fPixelPerMinute * GetOffsetMinuteFromGraphBegin(cur->arrive);
				int departOffset = m_fPixelPerMinute * GetOffsetMinuteFromGraphBegin(cur->depart);	
				int Xpos = (arrivalOffset+departOffset)/2;
				int nInStaOffset = GetStationVPos(cur->in_station_index,  PRIMARY_TRACK);

				myStaTextRect.left  = Xpos - strStaText.GetLength()*3-20;
				myStaTextRect.right = Xpos + strStaText.GetLength()*3+20;
				myStaTextRect.top   = nInStaOffset - 10;
				myStaTextRect.bottom  = nInStaOffset + 10;

				if(myStaTextRect.left < 0)
					myStaTextRect.left = 0;
				if(myStaTextRect.top < 0)
					myStaTextRect.top = 0;

				ptrain->myPosition[0].UnionRect(&myStaTextRect, &ptrain->myPosition[0]);
			}

			if(NULL == next)
				continue;

			CString strABSText = cur->szAbsText;
			if(!strABSText.IsEmpty())
			{
				int arrivalOffset = m_fPixelPerMinute * GetOffsetMinuteFromGraphBegin(next->arrive);
				int departOffset = m_fPixelPerMinute * GetOffsetMinuteFromGraphBegin(cur->depart);	

				int nX = (arrivalOffset+departOffset)/2;		
				int nY = (GetStationVPos(cur->out_station_index,  PRIMARY_TRACK) + GetStationVPos(next->in_station_index,  PRIMARY_TRACK))/2;

				myAbsTextRect.left  = nX - strABSText.GetLength()*3-20;
				myAbsTextRect.right = nX + strABSText.GetLength()*3+20;
				myAbsTextRect.top   = nY-10;
				myAbsTextRect.bottom  = nY+10;

				if(myAbsTextRect.left < 0)
					myAbsTextRect.left = 0;
				if(myAbsTextRect.top < 0)
					myAbsTextRect.top = 0;

				ptrain->myPosition[0].UnionRect(&myAbsTextRect, &ptrain->myPosition[0]);
			}
		}

		// �����۷��г�
		for(UINT idx = 0; idx < RecordNum; idx++)
		{
			TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
			TgTRAIN_RECORD* nxtr = ptrain->GetRecordByPosition(idx+1);
			if(curr == NULL || nxtr == NULL)
				break;

			if(IsFlagStopAbs(curr->flag) && (curr->station == nxtr->station))
			{
				int tmp = 0;
				if(curr->out_sta_direct == DOWN_DIRECT)
					tmp = UP_DIRECT;
				else
					tmp = DOWN_DIRECT;
				// �����۷�
				if(curr->station == nxtr->station && IsSetTrainFlag(ptrain->myTrainInfo.adjust_flag))
				{
					int ptlStartY;
					if(curr->out_sta_direct == DOWN_DIRECT)//down direct 
						ptlStartY = GetVPosition(curr->out_station_index, POS_FLAG::DOWN);
					else
						ptlStartY = GetVPosition(curr->out_station_index, POS_FLAG::UP);

					int absY = GetStationAbsPosition(ptrain, curr);
					int dlt = absY - ptlStartY;
										
					
					if(ptrain->myPosition[0].top>ptlStartY + dlt/2)
						ptrain->myPosition[0].top=ptlStartY + dlt/2;
					if(ptrain->myPosition[0].bottom<ptlStartY + dlt/2 + dlt)
						ptrain->myPosition[0].bottom=ptlStartY + dlt/2 + dlt;

					// ��������ֲ������г�ˢ�¿�
					if(IsAbsPartRunTrain(ptrain->myTrainInfo.adjust_flag))
					{
						int x = GetOffsetPixelFromGraphBegin(curr->depart+curr->stop_abs_s_time*60-ptrain->myTrainInfo.stop_abs_time*60);
						if(x<ptrain->myPosition[0].left && x>20)
						{
							ptrain->myPosition[0].left = x-20;
						}
					}
				}
			}
		}
	}
}

// �����г��ڳ�վ����
void CTGDoc::SetDirectAtStation(CTG_TRAIN* train)
{
	UINT nRecNum = train->GetRecordNum();  
	for(UINT i = 0; i< nRecNum; i++)
	{
		if(i == 0)
		{
			train->myRecord[i].out_sta_direct = UP_DIRECT;
			if(i < nRecNum-1)
			{
				if(train->myRecord[i].out_station_index > train->myRecord[i+1].in_station_index)
					train->myRecord[i].out_sta_direct = UP_DIRECT;
				else if(train->myRecord[i].out_station_index < train->myRecord[i+1].in_station_index)
					train->myRecord[i].out_sta_direct = DOWN_DIRECT;
				else
				{
					if(IsFlagStopAbs(train->myRecord[i].flag))
					{
						if(IsFlagStopAbsUp(train->myRecord[i].flag))
							train->myRecord[i].out_sta_direct=UP_DIRECT;
						else
							train->myRecord[i].out_sta_direct=DOWN_DIRECT;
					}
				}
			}
			else
			{
				int flag=GetEntryExitDirection(train, &train->myRecord[i], EXIT_POSITION, m_nCurrentSectionNo);
				if(flag != -1 && nRecNum > 1)
				{
					if(flag==UP_MARK)
						train->myRecord[i].out_sta_direct=UP_DIRECT;
					else
						train->myRecord[i].out_sta_direct=DOWN_DIRECT;
				}
				else
				{
					CString strTrainId = train->myRecord[i].depart_train_id;
					if(strTrainId.IsEmpty())
					{
						strTrainId = train->myRecord[i].arrive_train_id;
					}

					if(!strTrainId.IsEmpty())
					{
						train->myRecord[i].out_sta_direct = GetTrainDirectionByTrainId(strTrainId);
						if(IsFlagStopAbs(train->myRecord[i].flag))
						{
							if(IsFlagStopAbsUp(train->myRecord[i].flag))
								train->myRecord[i].out_sta_direct=UP_DIRECT;
							else
								train->myRecord[i].out_sta_direct=DOWN_DIRECT;
						}
					}
				}
			}

			if(IsFlagStart(train->myRecord[i].flag))
			{
				if(IsFlagStopAbs(train->myRecord[i].flag))
				{
					if(IsFlagStopAbsUp(train->myRecord[i].flag))
						train->myRecord[i].in_sta_direct=UP_DIRECT;
					else
						train->myRecord[i].in_sta_direct=DOWN_DIRECT;
				}
				else
				{
					train->myRecord[i].in_sta_direct = train->myRecord[i].out_sta_direct;
				}
			}
			else if(IsFlagAbsStart(train->myRecord[i].flag))
			{	
				if(IsAbsStartStopDown(train->myTrainInfo.adjust_flag))
					train->myRecord[i].in_sta_direct=UP_DIRECT;
				else
					train->myRecord[i].in_sta_direct=DOWN_DIRECT;
			}
			else
			{
				if(IsFlagStopAbs(train->myRecord[i].flag))
				{
					if(IsFlagStopAbsUp(train->myRecord[i].flag))
						train->myRecord[i].in_sta_direct=UP_DIRECT;
					else
						train->myRecord[i].in_sta_direct=DOWN_DIRECT;
				}
				else if(train->myRecord[i].in_station_index == train->myRecord[i].out_station_index)
				{
					train->myRecord[i].in_sta_direct = train->myRecord[i].out_sta_direct;
				}
				else if(train->myRecord[i].in_station_index > train->myRecord[i].out_station_index)
				{
					train->myRecord[i].in_sta_direct=UP_DIRECT;
				}
				else  if(train->myRecord[i].in_station_index < train->myRecord[i].out_station_index)
				{
					train->myRecord[i].in_sta_direct=DOWN_DIRECT;
				}
			}
		}
		else if(i == nRecNum-1)
		{
			train->myRecord[i].in_sta_direct = UP_DIRECT;
			
            if(train->myRecord[i-1].out_station_index > train->myRecord[i].in_station_index)
				train->myRecord[i].in_sta_direct = UP_DIRECT;
			else if(train->myRecord[i-1].out_station_index < train->myRecord[i].in_station_index)
				train->myRecord[i].in_sta_direct = DOWN_DIRECT;
			else
			{
				if(IsFlagStopAbs(train->myRecord[i-1].flag))
				{
					if(IsFlagStopAbsUp(train->myRecord[i-1].flag))
						train->myRecord[i].in_sta_direct=DOWN_DIRECT;
					else
						train->myRecord[i].in_sta_direct=UP_DIRECT;
				}
			}
			
			//int flag=GetEntryExitDirection(train, &train->myRecord[i], EXIT_POSITION, m_nCurrentSectionNo);
			//if(flag != -1)
			//{
			//	if(flag==UP_MARK)
			//		train->myRecord[i].out_sta_direct=UP_DIRECT;
			//	else
			//		train->myRecord[i].out_sta_direct=DOWN_DIRECT;
			//}
			//else
			//{
				train->myRecord[i].out_sta_direct = train->myRecord[i].in_sta_direct;
			//}
		}
		else
		{
			train->myRecord[i].in_sta_direct = UP_DIRECT;
			
            if(train->myRecord[i-1].out_station_index > train->myRecord[i].in_station_index)
				train->myRecord[i].in_sta_direct = UP_DIRECT;
			else if(train->myRecord[i-1].out_station_index < train->myRecord[i].in_station_index)
				train->myRecord[i].in_sta_direct = DOWN_DIRECT;
			else
			{
				if(IsFlagStopAbs(train->myRecord[i-1].flag))
				{
					if(IsFlagStopAbsUp(train->myRecord[i-1].flag))
						train->myRecord[i].in_sta_direct=DOWN_DIRECT;
					else
						train->myRecord[i].in_sta_direct=UP_DIRECT;
				}
			}

			train->myRecord[i].out_sta_direct = UP_DIRECT;
	
			if(train->myRecord[i].out_station_index > train->myRecord[i+1].in_station_index)
				train->myRecord[i].out_sta_direct = UP_DIRECT;
			else if(train->myRecord[i].out_station_index < train->myRecord[i+1].in_station_index)
				train->myRecord[i].out_sta_direct = DOWN_DIRECT;
			else
			{
				if(IsFlagStopAbs(train->myRecord[i].flag))
				{
					if(IsFlagStopAbsUp(train->myRecord[i].flag))
						train->myRecord[i].out_sta_direct=UP_DIRECT;
					else
						train->myRecord[i].out_sta_direct=DOWN_DIRECT;
				}
			}
		}

	}

	
	return;
}

void CTGDoc::SetStaticTimeByCfg()
{
	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	while(it != myPDataManage->myTrain.end())
	{
		CTG_TRAIN& train = it->second;
		
		if((GetEarlyLateTime() == train.m_nTrainType) || GetEarlyLateTime() == TYPE::ALL)
		{
			InitEarlyLastTime(&train);
		}
		it++;
	}
}

bool CTGDoc::SetBlockDirect(CTg_BLOCK* block)
{
	bool bUp = false, bDown = false, bNone = false;
	for(int i=0; i<block->count;i++)
	{
		const ABS_LINE* line = m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(block->ele_number[i]);
		if(NULL != line)
		{
			if(line->nDirect == 0)
				bUp = true;
			else if(line->nDirect == 1)
				bDown = true;
			else if(line->nDirect == 3)
				bNone = true;
			else
			{
				CString str;
				str.Format("TG.def���ߺ� %d �ķ������ò���ȷ %d (��ȷֵ:0:���� 1:���� 3:����)", line->nAbsNo, line->nDirect);
				sysprint(0,0, "%d", str);
				AfxMessageBox(str);
			}

		}
	}

	if(bUp && bDown)
		SetLineDirectionDual(block->line);
	else
	{
		if(bUp)
		{
			if(bNone)
				SetLineDirectionDual(block->line);
			else
				SetLineDirectionUp(block->line);
		}
		else if(bDown)
		{
			if(bNone)
				SetLineDirectionDual(block->line);
			else
				SetLineDirectionDown(block->line);
		}
		else if(bNone)
			SetLineDirectionNone(block->line);
		else
		{
			return false;
		}
	}

	return true;
}

int CTGDoc::GetVPosition(int idx, int flag)
{
	if(idx < 0 || idx >= this->m_nDispDistanceNum)
		return -1;

	if(POS_FLAG::UP == flag)
	{
		return m_sDispDistance[idx].nStationYPos;
	}
	else if(POS_FLAG::DOWN == flag)
	{
		return m_sDispDistance[idx].nSecondStationYPos;
	}
	else
	{
		return -1;
	}
}

int CTGDoc::GetSubSection(int idx)
{
	if(idx < 0 || idx >= this->m_nDispDistanceNum)
		return 0;

	return m_sDispDistance[idx].sub_section;
}

int CTGDoc::GetNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list)
{
	abs_line_list.clear();
	int little = (index1 < index2) ? index1 : index2;
	int big   = (index1 > index2) ? index1 : index2;

	// ���ӺϷ����
	if(little<0 || little>=this->m_nDispDistanceNum)
		return 0;
	if(big<0 || big>=this->m_nDispDistanceNum)
		return 0;

	for(int i=0; i<m_sDispDistance[little].down_line_count; i++)
	{
		if(i>=LINE_MAX_CNT)
			break;

		for(int j=0; j<m_sDispDistance[big].up_line_count; j++)
		{
			if(j>=LINE_MAX_CNT)
				break;

			if(m_sDispDistance[little].down_line_list[i] == m_sDispDistance[big].up_line_list[j])
			{
				abs_line_list.push_back(m_sDispDistance[little].down_line_list[i]);
				break;
			}
		}
	}

	return (int)abs_line_list.size();
}

int  CTGDoc::GetMutiNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list)
{
	abs_line_list.clear();

	// ���ӺϷ����
	if(index1<0 || index1>=this->m_nDispDistanceNum)
		return 0;
	if(index2<0 || index2>=this->m_nDispDistanceNum)
		return 0;

	for(int m=index1;m<index2;m++)
	{
		for(int n=m+1; n<=index2;n++)
		{
			std::vector<int> abs_list;
			GetNeibAbsLines(m, n, abs_list);
		
			for(int i=0; i<abs_list.size(); i++)
			{
				bool bFind=false;
				for(int k=0; k<abs_line_list.size(); k++)
				{										
					if(abs_line_list[k]==abs_list[i])
					{
						bFind=true;
						break;
					}
				}
				if(!bFind)
				{
					abs_line_list.push_back(abs_list[i]);
				}
			}
			abs_list.clear();
		}
	}

	return abs_line_list.size();
}

BOOL  CTGDoc::SetABSStart(CTG_TRAIN* train, CString strTrainId, long usTimeValue, int ypos, std::vector<int>& abs_line_list)
{
	if(NULL == train)
		return FALSE;

	if(train->GetRecordNum() > 0)
		return FALSE;
	int nStationNo1=-1, nStationNo2=-1, uEntry=-1;
	int nDirect = this->GetTrainDirectionByTrainId(strTrainId);
	int minpos = 99999, nIndex1, nIndex2;
	for(int i=1; i<this->m_nDispDistanceNum; i++)
	{
		int mid = (m_sDispDistance[i-1].nSecondStationYPos + m_sDispDistance[i].nStationYPos)/2;
		if(abs(mid-ypos)>8)
			continue;
		
		if(minpos > mid)
		{
			minpos = mid;
			nIndex1 = i-1;
			nIndex2 = i;
			nStationNo1 = m_sDispDistance[i-1].station_number;
			nStationNo2 = m_sDispDistance[i].station_number;
		}
	}
	if(abs(minpos-ypos)>8)
		return FALSE;

	if(GetNeibAbsLines(nIndex1, nIndex2, abs_line_list) <= 0)
		return FALSE;

	
	int nStationNo, node_index;
	if(nDirect == UP_DIRECT)
	{
		nStationNo = nStationNo1;
		node_index = nIndex1;
	}
	else
	{
		nStationNo = nStationNo2;
		node_index = nIndex2;
	}
	
	if(abs_line_list.size() == 0)
	{
		return FALSE;
	}
	else
	{
		uEntry = ConvertAbsnumStationToEntry(nStationNo, abs_line_list[0]);
	}
	TgTRAIN_RECORD rec;
	rec.arrive_train_id = strTrainId;
	rec.depart_train_id = strTrainId;
	rec.rec_index = 0;
	rec.station = nStationNo;
	
	train->myTrainInfo.type = m_ptgctrl->mConfigTg.GetTrainTypeByTrainNO(strTrainId);
	train->myTrainInfo.stop_abs_time = 10;
	rec.stop_abs_s_time = 0;
	rec.stop_abs_e_time = 0;

	rec.arrive = usTimeValue+rec.stop_abs_s_time*60;
	rec.depart = usTimeValue+rec.stop_abs_s_time*60;
	
	rec.plan_arrive =  rec.arrive;
	rec.plan_depart = rec.depart;

	rec.entry = uEntry;
	rec.exit = -1;
	rec.arrive_side = -1; 	
	rec.depart_side = -1;

	rec.adjust_status = 0;
	rec.early_depart_time=0x8000;  //0x8000��ʾ���޶���ǰ����(Ĭ��)

	rec.flag = 0;
	
	SetFlagAbsStart(rec.flag);
	if(nDirect == UP_DIRECT)
		SetAbsStartStopDown(train->myTrainInfo.adjust_flag);
	else
		SetAbsStartStopUp(train->myTrainInfo.adjust_flag);
	
	rec.static_arrive=0;
	rec.static_depart=0;
	
	rec.in_station_index = node_index;
	rec.out_station_index = node_index;

	if(train->AddRecord(rec, this->GetTgType()) == -1)
		return FALSE;

	return TRUE;
}

int  CTGDoc::GenExitEntryByInitABS(int conid, int station1, int station2, int index1, int index2, int& exit, int& entry)
{
	if(!this->JudeNodesConnected(index1, index2))
	{
		return 1;
	}
	int nAbsLineNo = m_ptgctrl->mConfigTg.m_tgdef.GetInitAbsLineNo(conid, station1, station2, index1, index2);
	if(nAbsLineNo > 0)
	{
		exit  = ConvertAbsnumStationToEntry(station1, nAbsLineNo);
		entry = ConvertAbsnumStationToEntry(station2, nAbsLineNo);
		
		if(exit > 0 && entry > 0)
		{
			return 2; // �ɹ�
		}
		else
		{
			CString alarm;
			alarm.Format("[����վ�����ó����] ��tg.def��[REVERSE_ABS]�����˳�վ:%d/%d, վ��:%d/%d, �ߺ�: %d", 
				station1, station2, index1, index2, nAbsLineNo);

			this->sysprint(10,0, "%s", alarm);

			if(exit <= 0)
			{
				alarm.Format("[����վ�����ó����] ��entry.cfg�г�վ %d û������line %d, ���ɳ����ʧ��", station1, nAbsLineNo);
				this->sysprint(10,0, "%s", alarm);
			}
			if(entry <= 0)
			{
				alarm.Format("[����վ�����ó����] ��entry.cfg�г�վ %d û������line %d, ���ɳ����ʧ��", station2, nAbsLineNo);
				this->sysprint(10,0, "%s", alarm);
			}

			return 3;
		}
	}

	return 0;
}

int CTGDoc::GetTrainIds(CStringArray& trainidary, CTG_TRAIN *train)
{
	trainidary.RemoveAll();

	if(NULL == train) 
		return 0;

	for(int i=0; i<train->GetRecordNum(); i++)
	{
		CString trainid = train->myRecord[i].arrive_train_id;
		if(trainid != "")
		{
			int k=0;
			for(k=0; k<trainidary.GetCount(); k++)
			{
				if(trainid == trainidary.GetAt(k))
					break;
			}

			if(k >= trainidary.GetCount())
			{
				trainidary.Add(trainid);
			}
		}

		trainid = train->myRecord[i].depart_train_id;
		if(trainid != "")
		{
			int k=0;
			for(k=0; k<trainidary.GetCount(); k++)
			{
				if(trainid == trainidary.GetAt(k))
					break;
			}

			if(k >= trainidary.GetCount())
			{
				trainidary.Add(trainid);
			}
		}
	}

	return trainidary.GetCount();
}

int  CTGDoc::GetNoteChineseChar(CString strNotes, int len)
{
	BYTE* logbuf = new BYTE[strNotes.GetLength()+1];
	if(NULL == logbuf)
		return len;

	memset(logbuf, 0, strNotes.GetLength()+1);
	strncpy((char*)logbuf,  strNotes,  strNotes.GetLength());
	logbuf[strNotes.GetLength()]=0;

	int flag=0;
	for(int i=0; i<len&&i<strNotes.GetLength(); i++)
	{
		BYTE ch = logbuf[i];
		if(ch>=128) // ����
		{
			flag++;
		}
	}

	delete[] logbuf;

	// ��������
	if(flag%2 == 0)
		return len;
	else
		return len-1;
}

void CTGDoc::ResetContinueDraw()
{
	m_ContinueDrawInfo.m_bContinueDraw=false;
	m_ContinueDrawInfo.m_nRecIndex=-1;
	m_ContinueDrawInfo.m_nTrainIndex=-1;
}

void CTGDoc::SetContinueDraw(int rec_index, long trainIndex)
{
	m_ContinueDrawInfo.m_bContinueDraw=true;
	m_ContinueDrawInfo.m_nRecIndex=rec_index;
	m_ContinueDrawInfo.m_nTrainIndex=trainIndex;
}

bool CTGDoc::IsContinueDrawTrain(int& rec_index, long& trainIndex)
{
	rec_index=-1;
	trainIndex=-1;

	if(m_ContinueDrawInfo.m_bContinueDraw)
	{
		rec_index=m_ContinueDrawInfo.m_nRecIndex;
		trainIndex=m_ContinueDrawInfo.m_nTrainIndex;
		return true;
	}

	return false;
}

RESULT CTGDoc::StationInTwoSubSection(int index1, int index2)
{
	if(index1<0 || index1 >= this->m_nDispDistanceNum)
		return UNKNOW;
	if(index2<0 || index2 >= this->m_nDispDistanceNum)
		return UNKNOW;
	
	if(m_sDispDistance[index1].sub_section == m_sDispDistance[index2].sub_section)
		return SAME;
	else
		return NOT_SAME;
}
void CTGDoc::ReGeneratetStationInOutIndex(CTG_TRAIN& train)
{
	if(!this->m_bCalcStationIndex)
		return;

	int index = -1;
	TgTRAIN_RECORD *prec = NULL;
	UINT nCountNum = train.GetRecordNum();
	for(UINT nRecIndex=0; nRecIndex<nCountNum; nRecIndex++)
	{
		TgTRAIN_RECORD *prec = train.GetRecordByRecIndex(nRecIndex);
		if(prec == NULL)
			continue;

		index = GetNodeEntryIndex(&train,prec,m_nCurrentSectionNo);
		if(index >=0)
			prec->in_station_index = index;
		
		index = GetNodeExitIndex(&train,prec,m_nCurrentSectionNo);
		if(index >=0)
			prec->out_station_index = index;
	}	

	// A-A'+B
	prec = train.GetFirstRecord();
	if(prec != NULL)
	{
		if(NOT_SAME == StationInTwoSubSection(prec->in_station_index, prec->out_station_index))
		{
			prec->in_station_index = prec->out_station_index;
		}
	}

	// A+B-B'
	prec = train.GetLastRecord();
	if(prec != NULL)
	{
		if(NOT_SAME == StationInTwoSubSection(prec->in_station_index, prec->out_station_index))
		{
			prec->out_station_index = prec->in_station_index;
		}
	}
	
	// A-B+B'-C
	for(UINT nRecIndex=0; nRecIndex<nCountNum; nRecIndex++)
	{
		TgTRAIN_RECORD *prec = train.GetRecordByPosition(nRecIndex-1);
		TgTRAIN_RECORD *curr = train.GetRecordByPosition(nRecIndex);
		TgTRAIN_RECORD *next = train.GetRecordByPosition(nRecIndex+1);
		if(curr == NULL || prec == NULL || next == NULL)
			continue;

		int pre_out = prec->out_station_index;
		int next_in = next->in_station_index;

		if(pre_out<0 || pre_out >= this->m_nDispDistanceNum)
			continue;
		if(next_in<0 || next_in >= this->m_nDispDistanceNum)
			continue;
	
		int curr_in  = curr->in_station_index;
		int curr_out = curr->out_station_index;
		if(curr_in<0 || curr_in >= this->m_nDispDistanceNum)
			continue;
		if(curr_out<0 || curr_out >= this->m_nDispDistanceNum)
			continue;

		if(SAME == StationInTwoSubSection(pre_out, next_in))
		{
			if(NOT_SAME == StationInTwoSubSection(pre_out, curr_in))
			{
				curr->out_station_index = curr->in_station_index;
			}
			if(NOT_SAME == StationInTwoSubSection(curr_out, next_in))
			{
				curr->in_station_index = curr->out_station_index;
			}
		}
	}

	return;
}

bool CTGDoc::EntrySideConnect(int station, int side, int exit)
{
	for(int i=0;i < m_nStationNum;i++)
	{	
		if(NULL == m_pStationDef[i])
			continue;

		if(m_pStationDef[i]->GetStationNo() == station)
		{
			if(STA_SUCCESS == m_pStationDef[i]->isEntrySideConnect(side, exit))
				return true;
			break;
		}
	}

	return false;
}


// �����г���Ǯ��
void CTGDoc::DrawTrainBenqianTime(CDC *pCDC, CTG_TRAIN * single, TgTRAIN_RECORD* first, const CPoint& ptlEnd, CRect& tempRect)
{
	bool bShowAcceptTrainLate = false;
	bool bShowAcceptTruckLate = false;

	if(GetTgType() != TG_TYPE::BASIC_TG)
	{
		bShowAcceptTrainLate = m_bShowAcceptTrainLate;
		bShowAcceptTruckLate = m_bShowAcceptTruckLate;
	}
	else 
	{
		bShowAcceptTrainLate = false;
		bShowAcceptTruckLate = false;
	}

	if(!bShowAcceptTrainLate && !bShowAcceptTruckLate)
		return;

	bool bIsFenJieStation = this->m_ptgctrl->mConfigTg.m_tgdef.IsInterfaceStation(first->station);
	bool bIsBianZhuStation = this->m_ptgctrl->mConfigTg.m_tgdef.IsBianzuStation(first->station);
	if(!bIsBianZhuStation)
	{
		TgTRAIN_RECORD *second = single->GetRecordByPosition(1);
		if(second != NULL)
			bIsBianZhuStation = this->m_ptgctrl->mConfigTg.m_tgdef.IsBianzuStation(second->station);
	}
	
	if( (IsTrain(single->myTrainInfo) && bShowAcceptTrainLate && bIsFenJieStation)
	  ||(IsTruck(single->myTrainInfo) && bShowAcceptTruckLate && bIsBianZhuStation))
	{
		int timeValue = single->myTrainInfo.sborder_actual_time - single->myTrainInfo.sborder_plan_time;
		timeValue = timeValue/60;

		CPen nPen;
		if(timeValue>0)
			nPen.CreatePen(PS_INSIDEFRAME,1,CLR_BLUE);
		else
			nPen.CreatePen(PS_INSIDEFRAME,1,CLR_RED);

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
		
		CFont* oldFont = pCDC->SelectObject(&fontEarlyLastTime);
		CSize tx_size = pCDC->GetTextExtent(str_text);
		if(tempRect.Width()<tx_size.cx)
		{
			int dlt=(tx_size.cx-tempRect.Width())/2+1;
			tempRect.left -= dlt;
			tempRect.right += dlt;
			tempRect.top -= dlt;
			tempRect.bottom += dlt;
		}
		
		if(m_bIsVerifyTrainRect)
		{
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, tempRect);
		#endif
		single->UnionRect(tempRect);
		}

		CPen* poldPen = pCDC->SelectObject(&nPen);
		pCDC->Arc(tempRect,CPoint(tempRect.left, tempRect.CenterPoint().y), CPoint(tempRect.left, tempRect.CenterPoint().y));
		pCDC->SelectObject(poldPen);
		nPen.DeleteObject();

		UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER);
		UINT nOldColor = pCDC->SetTextColor(CLR_BLACK);
		pCDC->TextOut (tempRect.CenterPoint().x, tempRect.CenterPoint().y - tx_size.cy/2, str_text);
		
		pCDC->SetTextAlign(nOldAlign);
		pCDC->SetTextColor(nOldColor);
		pCDC->SelectObject(oldFont);
	}
	return;
}

void CTGDoc::InitTGState()
{
	m_nOperatorStatus = NORMAL_STATUS;
	m_bDeleteMutiTrain=false;
	m_bMoveMutiTrain=false;
}

bool  CTGDoc::IsEntrySideConnect(int station, int side, int entry)
{
	if(CTC_STATION == GetStationProperty(station))
	{
		if (!EntrySideConnect(station, side, entry))
		{
			return false;
		}
	}

	return true;
}

bool  CTGDoc::IsSideExitConnect(int station, int side, int exit)
{
	if(CTC_STATION == GetStationProperty(station))
	{
		if (!EntrySideConnect(station, side, exit))
		{
			return false;
		}
	}

	return true;
}

// ���ɵ��ͳ�����Ƿ���ͨ
bool CTGDoc::CheckSideEntry(int arl_side_no, int dep_side_no, const TgTRAIN_RECORD* rec, int& flag)
{
	flag=0;
	if(CTC_STATION == GetStationProperty(rec->station))
	{
		if (IS_VALID_SIDE(arl_side_no) && IS_VALID_ENTRY(rec->entry) && !EntrySideConnect(rec->station, arl_side_no, rec->entry))
		{
			flag=1;
			return false;
		}
		if (IS_VALID_SIDE(dep_side_no) && IS_VALID_ENTRY(rec->exit)  && !EntrySideConnect(rec->station, dep_side_no, rec->exit))
		{
			flag=2;
			return false;
		}
	}

	return true;
}

const CTG_TRAIN* CTGDoc::GetTrainByPoint(CPoint& pt)
{
	return myPDataManage->GetTrainByPoint(pt);
}

TRAIN_CHEZUHAO* CTGDoc::GetCurrentSelTrainCZH()
{
	return myChezuhao.GetCzhByCheZuHao(m_CurSelCZH);
}

void CTGDoc::SetCurrentSelTrainCZH(CString czh)
{
	m_CurSelCZH=czh;
}

void CTGDoc::SetFormView(CTGForm* pView)
{
	m_pView=pView;
}

void CTGDoc::ModifyTrainCZH(CString czh, CString newczh)
{
	TRAIN_CHEZUHAO* pTrainsCZH=myChezuhao.GetCzhByCheZuHao(czh);
	if(pTrainsCZH==NULL)
		return;

	TRAIN_CHEZUHAO TrainsCZH=*pTrainsCZH;
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it=TrainsCZH.trains.begin();
	for(;it!=TrainsCZH.trains.end(); it++)
	{
		CTG_TRAIN* pTrain=GetTrainByIndex(it->second.train_index);
		if(pTrain==NULL)
			continue;
		
		CTG_TRAIN train = *pTrain;   // ����ֱ�Ӹ��ڴ�����
		train.myTrainInfo.shift_index_string=newczh;

		SendUpdateTrain(train);
	}
	//pTrainsCZH->czh=newczh;

	return;
}

void CTGDoc::ReGenTrainReturnTrainID(CTG_TRAIN& train)
{
	CString czh=train.myTrainInfo.shift_index_string;
	if(czh.IsEmpty())
	{
		sysprint(0,0, "���α��, ����û�г����ϵ, �������۷�����:%s", GetTrainShowId(train));
	}
	else
	{
		sysprint(0,0, "���α��, �����ϵΪ:%s, �����۷�����:%s", czh, GetTrainShowId(train));
		
		CStringArray text;
		GenTrainReturnTrainID(czh, text);
		if(text.GetCount()==0)
		{
			sysprint(0,0, "�����۷�����:�ɹ�");
		}
	}
}

bool CTGDoc::GenTrainReturnTrainID(CString czh, CStringArray& text)
{
	sysprint(10, 0, "-----���ݳ���� %s �����۷�����-----", czh);
	text.RemoveAll();
	TRAIN_CHEZUHAO* pTrainsCZH=myChezuhao.GetCzhByCheZuHao(czh);
	if(pTrainsCZH==NULL)
	{
		CString warning;
		warning.Format("�Ҳ��������Ϊ %s �ĳ����, �����۷�����ʧ��", czh);
		text.Add(warning);
		sysprint(10, 0, "%s", warning);
		return false;
	}
	return myChezuhao.SetReturnTrainId(pTrainsCZH, text);
}

void CTGDoc::ReGenTrainCZHRelation(CString czh)
{
	myChezuhao.DeleteCzhByCheZuHao(czh);
	myPDataManage->GenTrainCZHRelation(czh);
	return;
}

bool CTGDoc::IsCanSetInterCityReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex)
{
	list_trainno.RemoveAll();
	list_trainindex.RemoveAll();
	CString strczh=train.myTrainInfo.shift_index_string;
	TRAIN_CHEZUHAO* pTrainsCZH=myChezuhao.GetCzhByCheZuHao(strczh);
	if(pTrainsCZH==NULL)
	{
		text.Format("�г�%sû�г����ϵ,���������۷�!", GetTrainShowId(train));
		return false;
	}
	TgTRAIN_RECORD* pLast=train.GetLastRecord();
	if(IsFlagTerminal(pLast->flag))
	{
		std::map<ULONG, TRAIN_CZH_INFO>::iterator it = pTrainsCZH->trains.begin();
		for(;it!=pTrainsCZH->trains.end(); it++)
		{
			if(it->second.train_index==train.m_nTrainIndex)
			{
				it++;
				if(it==pTrainsCZH->trains.end())
				{
					text.Format("�г�%s û�к�����Ӧʼ���г�,���������۷�!", GetTrainShowId(train));
					return false;
				}
				else
				{
					if(it->second.begin_station==pLast->station)
					{
						CTG_TRAIN* tmp=this->GetTrainByIndex(it->second.train_index);
						if(tmp)
						{
							list_trainno.Add(it->second.trainid);
							list_trainindex.Add(it->second.train_index);
							it++;
							for(;it!=pTrainsCZH->trains.end(); it++)
							{
								if(it->second.begin_station==pLast->station)
								{
									CTG_TRAIN* tmp=this->GetTrainByIndex(it->second.train_index);
									if(tmp)
									{
										list_trainno.Add(it->second.trainid);
										list_trainindex.Add(it->second.train_index);
									}
								}
							}
							return true;
						}
						else
						{
							text.Format("�г�%s ���ڳ��鲻������,�������Ȼ���������۷�!", GetTrainShowId(train));
							return false;
						}
					}
					else
					{
						text.Format("�г�%s ���ڳ��鲻�����۷�����,�������Ȼ���������۷�!", GetTrainShowId(train));
						return false;
					}
				}
			}
		}
	}
	else
	{
		text.Format("�г�%s�����յ���,���������۷�!", GetTrainShowId(train));
		return false;
	}
	return false;
}

bool CTGDoc::IsCanSetInterCityStartReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex)
{
	list_trainno.RemoveAll();
	list_trainindex.RemoveAll();
	CString strczh=train.myTrainInfo.shift_index_string;
	TRAIN_CHEZUHAO* pTrainsCZH=myChezuhao.GetCzhByCheZuHao(strczh);
	if(pTrainsCZH != NULL)
	{
		TgTRAIN_RECORD* pFirst=train.GetFirstRecord();
		if(pFirst!=NULL)
		{
			CStringArray trainno_list;
			CDWordArray trainindex_list;
			std::map<ULONG, TRAIN_CZH_INFO>::iterator it = pTrainsCZH->trains.begin();
			for(;it!=pTrainsCZH->trains.end(); it++)
			{
				if(it->second.train_index==train.m_nTrainIndex)
				{
					break;
				}
				if(it->second.end_station==pFirst->station)
				{
					CTG_TRAIN* tmp=this->GetTrainByIndex(it->second.train_index);
					if(tmp)
					{
						trainno_list.Add(it->second.trainid);
						trainindex_list.Add(it->second.train_index);
					}
				}
			}
			int nCount=trainno_list.GetCount();
			if(nCount>0)
			{
				for(int n=nCount-1; n>=0; n--)
				{
					list_trainno.Add(trainno_list.GetAt(n));
					list_trainindex.Add(trainindex_list.GetAt(n));
				}
			}
		}
	}
	return true;
}

void CTGDoc::DeleteTrainCZH(CString czh)
{
	TRAIN_CHEZUHAO* pTrainsCZH=myChezuhao.GetCzhByCheZuHao(czh);
	if(pTrainsCZH==NULL)
		return;
	
	TRAIN_CHEZUHAO TrainsCZH=*pTrainsCZH;
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it=TrainsCZH.trains.begin();
	for(;it!=TrainsCZH.trains.end(); it++)
	{
		CTG_TRAIN* pTrain=GetTrainByIndex(it->second.train_index);
		if(pTrain==NULL)
			continue;
		
		CTG_TRAIN train=*pTrain;   // ����ֱ�Ӹ��ڴ�����
		train.myTrainInfo.shift_index_string="";
		TgTRAIN_RECORD* last=train.GetLastRecord();
		if(last!=NULL)
		{
			if(IsFlagTerminal(last->flag))
			{
				last->depart_train_id=last->arrive_train_id;
			}
		}

		this->SendUpdateTrain(train);
	}

	m_pView->ClearTrainCZHLine(pTrainsCZH);

	//pTrainsCZH->czh="";
	//pTrainsCZH->trains.clear();
	//pTrainsCZH->SetRect(0,0,0,0);

	return;
}

void CTGDoc::AddNewTrainCZH(CString czh)
{
	TRAIN_CHEZUHAO* pTrainCZH=myChezuhao.GetNewCzh();
	if(pTrainCZH)
	{
		pTrainCZH->czh=czh;
	}

	return;
}

void CTGDoc::AddTrainToCZHGroup(CString czh, CString trainid)
{
	UINT nFindNum=0;
	CTG_TRAIN* pTrain=GetTrainByTrainId(trainid, nFindNum);
	if(nFindNum>1)
	{
		AfxMessageBox("�ҵ�����г�����ѡ���г����복��");
		return;
	}
	if(pTrain==NULL)
	{
		AfxMessageBox("�ڵ�ǰͼ���Ҳ�����������г�");
		return;
	}
	
	CTG_TRAIN train = *pTrain;   // ����ֱ�Ӹ��ڴ�����
	train.myTrainInfo.shift_index_string=czh;

	this->SendUpdateTrain(train);
}
	
bool CTGDoc::DelTrainFromCZHGroup(CString czh, CString trainindex)
{
	UINT nFindNum=0;
	ULONG nTrainIndex=atol(trainindex);

	CTG_TRAIN* pTrain=GetTrainByIndex(nTrainIndex);
	if(pTrain==NULL)
	{
		AfxMessageBox("�ڵ�ǰͼ���Ҳ�����������г�");
		return false;
	}
	
	CTG_TRAIN train = *pTrain;   // ����ֱ�Ӹ��ڴ�����
	train.myTrainInfo.shift_index_string="";
	this->SendUpdateTrain(train);
	return true;
}

void CTGDoc::ReGenTrainCZHRelation()
{
	sysprint(10, 0, "|=======���������г������ϵ=======|");
	if(!m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
		return;

	myChezuhao.ClearAllTrainCheHao();
		
	std::map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	while(it != myPDataManage->myTrain.end())
	{
		myChezuhao.AppendCheZuHao(it->second);
		it++;
	}
}

void CTGDoc::CheckTrainSideInfo(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary)
{
	CString strText="";
	TgTRAIN_RECORD *precord=ptrain->GetRecordByRecIndex(rec_index);
	if(!precord)
		return;
	
	BYTE arrive_side=precord->arrive_side;
	BYTE depart_side=precord->depart_side;
	int  station=precord->station;

	if(GetStationProperty(station)!=CTC_STATION)
		return;

	CStringArray Textary;
	CheckSideRelation(ptrain, rec_index, Textary);
	for(int i=0; i<Textary.GetCount(); i++)
	{
		ary.Add(Textary.GetAt(i));
	}

	CString strStaName = GetStationName(station);
	CString strTrainId=this->GetTrainShowId(ptrain);
	
	CStationDef *pStationDef=GetStationSideDef(station);
	if(!pStationDef)
	{
		strText.Format("��ȡ %s վϸʧ��,��˶� %s �Ƿ�����վϸ", strStaName, strTrainId);
		ary.Add(strText);
		return;
	}

	bool bStart=false, bTerminal=false;
	bool bArriveSideOK=true, bDepartSideOK=true;

	if(IsFlagStart(precord->flag) && (0==rec_index)) // ʼ����,��鷢���ɵ�
	{
		bStart=true;
	}
	if(IsFlagTerminal(precord->flag) && (ptrain->GetRecordNum()==rec_index+1)) // �յ���,���ӳ��ɵ�
	{
		bTerminal=true;
	}
	
	CString arrive_side_name=this->GetSideName(station, arrive_side);
	CString depart_side_name=this->GetSideName(station, depart_side);

	if(arrive_side_name=="?G")
	{
		bArriveSideOK=false;
		if(!bStart)
		{
			strText.Format("���� %s %s û�нӳ��ɵ�", strTrainId, strStaName);
			ary.Add(strText);
		}
	}
	
	if(depart_side_name=="?G")
	{
		bDepartSideOK=false;
		if(!bTerminal)
		{
			strText.Format("���� %s %s û�з����ɵ�", strTrainId, strStaName);
			ary.Add(strText);
		}
	}
	
	if(bArriveSideOK && bDepartSideOK && !bStart && !bTerminal)
	{
		if(arrive_side != depart_side)
		{
			strText.Format("�г� %s %s �����ɵ���һ��", strTrainId, strStaName);
			ary.Add(strText);
		}
	}

	if(bStart) // ʼ����鷢���ɵ�
	{
		if(bDepartSideOK)
		{
			// ��·�ɵ����
			CTG_TRAIN *pre_train = myChezuhao.GetPreTrain(ptrain);
			if(pre_train!=NULL)
			{
				const TgTRAIN_RECORD *plast=pre_train->GetLastRecord();
				if(plast)
				{
					if(plast->station==precord->station)
					{
						if(plast->arrive_side!=precord->depart_side)
						{
							CString trainName=this->GetTrainShowId(pre_train);
							CString side_name=this->GetSideName(plast->station, plast->arrive_side);
							strText.Format("%s�κ�%s��ͬ��·,�ɵ���ͬ�޷��۷�,%s����%s��%sʼ��,%s��%s�յ�", 
								strTrainId, trainName,
								strStaName, strTrainId, depart_side_name, trainName, side_name);
							ary.Add(strText);
						}
					}
				}
			}

			// ��鳬�޳��̶��ɵ�
			if (ptrain->myTrainInfo.space_rank>0) // 20090710 
			{
				if (!pStationDef->isReceiveExceedTrain(depart_side, ptrain->myTrainInfo.space_rank))
				{
					strText.Format("����%s, %s, �����ɵ� %s ���ܽӳ��޳�", strTrainId, strStaName, depart_side_name);
					ary.Add(strText);
				}
			}

			// ���ɵ�������
			if (ptrain->myTrainInfo.b_elec_rank)
			{
				if (!pStationDef->isCatenary(depart_side))
				{
					strText.Format("����%s, %s, �����ɵ� %s �޵�����", strTrainId, strStaName, depart_side_name);
					ary.Add(strText);
				}
			}

			// ���ɵ��ӷ�������
			BYTE nTrainType = (ptrain->GetTrainType()==TYPE::PASSENGER) ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if( !pStationDef->isReceiveTrain(depart_side, nTrainType) )
			{
				strText.Format("%s %s %s ���ܷ������ͳ�", strTrainId,strStaName, depart_side_name);
				ary.Add(strText);
			}

			// ���ͳ�
			if (ptrain->GetTrainType() == TYPE::PASSENGER)
			{
				// ������ҵ
				if(!IsTechStopTrain(*precord))
				{
					if(!this->m_ptgctrl->mConfigTg.isPlatform(station, depart_side))
					{
						strText.Format("����%s, %s, %s ��վ̨,���ܰ��������ҵ!", strTrainId, strStaName, depart_side_name);
						ary.Add(strText);
					}
					else if(IsCrhTrain(ptrain->myTrainInfo) && !this->m_ptgctrl->mConfigTg.isHighPlatform(station,depart_side))
					{
						strText.Format("����%s, %s, ������Ǹ�վ̨�ɵ� %s ����!", strTrainId, strStaName, depart_side_name);
						ary.Add(strText);
					}
				}
			}
		}
	}
	else if(bTerminal)
	{
		if(bArriveSideOK)
		{
			// ��·�ɵ����
			CTG_TRAIN *next_train = myChezuhao.GetNextTrain(ptrain);
			if(next_train!=NULL)
			{
				const TgTRAIN_RECORD *pFirst=next_train->GetFirstRecord();
				if(pFirst)
				{
					if(pFirst->station==precord->station)
					{
						if(pFirst->depart_side!=precord->arrive_side)
						{
							CString trainName=this->GetTrainShowId(next_train);
							CString side_name=this->GetSideName(pFirst->station, pFirst->depart_side);
							strText.Format("%s�κ�%s��ͬ��·,�ɵ���ͬ�޷��۷�,%s����%s��%s�յ�,%s��%sʼ��", 
								strTrainId, trainName,
								strStaName, strTrainId, arrive_side_name, trainName, side_name);
							ary.Add(strText);
						}
					}
				}
			}

			// ��鳬�޳��̶��ɵ�
			if (ptrain->myTrainInfo.space_rank>0) // 20090710 
			{
				if (!pStationDef->isReceiveExceedTrain(arrive_side, ptrain->myTrainInfo.space_rank))
				{
					strText.Format("����%s, %s, �ӳ��ɵ� %s ���ܽӳ��޳�", strTrainId,strStaName, arrive_side_name);
					ary.Add(strText);
				}
			}

			// ���ɵ�������
			if (ptrain->myTrainInfo.b_elec_rank)
			{
				if (!pStationDef->isCatenary(arrive_side) )
				{
					strText.Format("����%s, %s, �ӳ��ɵ� %s �޵�����", strTrainId, strStaName, arrive_side_name);
					ary.Add(strText);
				}
			}

			// ���ɵ��ӷ�������
			BYTE nTrainType = (ptrain->GetTrainType()==TYPE::PASSENGER) ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if(!pStationDef->isReceiveTrain(arrive_side, nTrainType))
			{
				strText.Format("����%s, %s, �ɵ� %s ���ܽӴ����ͳ�", strTrainId,strStaName, arrive_side_name);
				ary.Add(strText);
			}
		
			// ���ͳ�
			if (ptrain->GetTrainType() == TYPE::PASSENGER)
			{
				// ������ҵ
				if(!IsTechStopTrain(*precord))
				{
					if(!this->m_ptgctrl->mConfigTg.isPlatform(station, arrive_side))
					{
						strText.Format("����%s, %s, %s ��վ̨,���ܰ��������ҵ", strTrainId,	 strStaName, arrive_side_name);
						ary.Add(strText);
					}
					else if (IsCrhTrain(ptrain->myTrainInfo) && !this->m_ptgctrl->mConfigTg.isHighPlatform(station,arrive_side))
					{
						strText.Format("����%s, %s, ���������Ǹ�վ̨�ɵ� %s", strTrainId, strStaName, arrive_side_name);
						ary.Add(strText);
					}
				}
			}
		}
	}
	else
	{
		// ��鳬�޳��̶��ɵ�
		if (ptrain->myTrainInfo.space_rank>0) // 20090710 
		{
			if (bArriveSideOK && !pStationDef->isReceiveExceedTrain(arrive_side, ptrain->myTrainInfo.space_rank))
			{
				strText.Format("����%s, %s, �ӳ��ɵ� %s ���ܽӳ��޳�", strTrainId,strStaName, arrive_side_name);
				ary.Add(strText);
			}
			if (bDepartSideOK && arrive_side != depart_side && !pStationDef->isReceiveExceedTrain(depart_side, ptrain->myTrainInfo.space_rank))
			{
				strText.Format("����%s, %s, �����ɵ� %s ���ܽӳ��޳�", strTrainId,strStaName, depart_side_name);
				ary.Add(strText);
			}
		}

		// ���ɵ�������
		if (ptrain->myTrainInfo.b_elec_rank)
		{
			if (bArriveSideOK && !pStationDef->isCatenary(arrive_side) )
			{
				strText.Format("����%s, %s, �ӳ��ɵ� %s �޵�����", strTrainId, strStaName, arrive_side_name);
				ary.Add(strText);
			}
			if (bDepartSideOK && arrive_side != depart_side && !pStationDef->isCatenary(depart_side) )
			{
				strText.Format("����%s, %s, �����ɵ� %s �޵�����", strTrainId, strStaName, depart_side_name);
				ary.Add(strText);
			}
		}

		// ���ɵ��ӷ�������
		BYTE nTrainType = (ptrain->GetTrainType()==TYPE::PASSENGER) ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
		if(bArriveSideOK && !pStationDef->isReceiveTrain(arrive_side, nTrainType))
		{
			strText.Format("����%s, %s, �ɵ� %s ���ܽӴ����ͳ�", strTrainId,strStaName, arrive_side_name);
			ary.Add(strText);
		}
		if(bDepartSideOK && arrive_side != depart_side && !pStationDef->isReceiveTrain(depart_side, nTrainType) )
		{
			strText.Format("����%s,%s, �����ɵ� %s ���ܽӴ����ͳ�", strTrainId,strStaName, depart_side_name);
			ary.Add(strText);
		}

		// ���ͳ�
		if (ptrain->GetTrainType() == TYPE::PASSENGER)
		{
			// ����ͨ��
			if(bArriveSideOK && bDepartSideOK && !bStart && !bTerminal)
			{
				if(precord->arrive == precord->depart)
				{
					if(!pStationDef->isMainSide(precord->arrive_side, precord->entry) || !pStationDef->isMainSide(precord->depart_side, precord->exit))
					{
						strText.Format("%s %s %s �ͳ�����ͨ��", strTrainId,strStaName, arrive_side_name);
						ary.Add(strText);
					}
				}
			}
			
			// ������ҵ
			if(!IsTechStopTrain(*precord) && (precord->arrive != precord->depart))
			{
				if(bArriveSideOK)
				{
					if(!this->m_ptgctrl->mConfigTg.isPlatform(station, arrive_side))
					{
						strText.Format("����%s, %s, %s ��վ̨,���ܰ��������ҵ", strTrainId,	 strStaName, arrive_side_name);
						ary.Add(strText);
					}
					else if (IsCrhTrain(ptrain->myTrainInfo) && !this->m_ptgctrl->mConfigTg.isHighPlatform(station,arrive_side))
					{
						strText.Format("����%s, %s, ���������Ǹ�վ̨�ɵ� %s", strTrainId, strStaName, arrive_side_name);
						ary.Add(strText);
					}
				}
				if(bDepartSideOK && arrive_side != depart_side)
				{
					if( !this->m_ptgctrl->mConfigTg.isPlatform(station, depart_side))
					{
						strText.Format("����%s, %s, %s ��վ̨,���ܰ��������ҵ!", strTrainId, strStaName, depart_side_name);
						ary.Add(strText);
					}
					else if(IsCrhTrain(ptrain->myTrainInfo) && !this->m_ptgctrl->mConfigTg.isHighPlatform(station,depart_side))
					{
						strText.Format("����%s, %s, ������Ǹ�վ̨�ɵ� %s ����!", strTrainId, strStaName, depart_side_name);
						ary.Add(strText);
					}
				}
			}
		}
	}
	
	// �����ں͹ɵ��Ƿ���ͨ
	if(!bStart)
	{
		if (!pStationDef->IsValidEntry(precord->entry))
		{
			strText.Format("���� %s %s ���%d�Ƿ�",strTrainId,strStaName, precord->entry);
			ary.Add(strText);
		}
		else
		{
			if (bArriveSideOK && STA_FAILED==pStationDef->isEntrySideConnect(precord->arrive_side, precord->entry))
			{
				strText.Format("���� %s %s ���%d��%s ����ͨ",strTrainId, strStaName, precord->entry, arrive_side_name);
				ary.Add(strText);
			}
		}
	}

	// ���ɵ��ͳ����Ƿ���ͨ
	if(!bTerminal)
	{
		if (!pStationDef->IsValidEntry(precord->exit))
		{
			strText.Format("����%s %s ����%d�Ƿ�",strTrainId,strStaName,precord->exit);
			ary.Add(strText);
		}
		else
		{
			if (bDepartSideOK && STA_FAILED == pStationDef->isEntrySideConnect(precord->depart_side, precord->exit))
			{
				strText.Format("���� %s %s %s�ͳ���%d ����ͨ",strTrainId, strStaName, depart_side_name, precord->exit);
				ary.Add(strText);
			}
		}
	}

	WORD absnum1=1,absnum2=2;
	WORD dir1=0xffff,dir2=0xffff;
	TgTRAIN_RECORD *pprerec=ptrain->GetRecordByRecIndex(rec_index-1);
	if(NULL != pprerec)
	{
		if(ConvertStationEntryToAbsnum(precord->station,precord->entry,&absnum1,&dir1) 
			&& ConvertStationEntryToAbsnum(pprerec->station,pprerec->exit,&absnum2,&dir2))
		{
			if(absnum1!=absnum2)
			{
				strText.Format("����%s�޷��� %s ���� %s",strTrainId, 
					GetStationName(pprerec->station), GetStationName(precord->station));
				ary.Add(strText);
			}
		}
	}
	TgTRAIN_RECORD *pnextrec=ptrain->GetRecordByRecIndex(rec_index+1);
	if(NULL != pnextrec)
	{
		absnum1=1,absnum2=2;
		if(ConvertStationEntryToAbsnum(pnextrec->station,pnextrec->entry,&absnum1,&dir1) 
			&& ConvertStationEntryToAbsnum(precord->station,precord->exit,&absnum2,&dir2))
		{
			if(absnum1!=absnum2)
			{
				strText.Format("����%s�޷��� %s ���� %s",strTrainId, 
					GetStationName(precord->station), GetStationName(pnextrec->station));
				ary.Add(strText);
			}
		}
	}
	return;
}

// ------------------------------------------------------------------------
int CTGDoc::SetTrainExitByCfg(CTG_TRAIN* train, int rec_index)
{
	if(train==NULL)
		return -1;

	TgTRAIN_RECORD *pre = train->GetRecordByPosition(rec_index-1);
	TgTRAIN_RECORD *rec = train->GetRecordByPosition(rec_index);
	if(pre==NULL || rec==NULL)
		return -1;

	if(!this->JudeNodesConnected(pre->out_station_index, rec->in_station_index))
	{
		return -1;
	}

	int abslineno[10];
	int nAbsLineNum = m_ptgctrl->mConfigTg.m_tgdef.GetSpecialExitEntryLineNo(m_nCurrentSectionNo, 
											pre->station,rec->station, 
											pre->out_station_index,rec->in_station_index, abslineno);
	if(nAbsLineNum > 0)
	{
		int exit  = ConvertAbsnumStationToEntry(rec->station, abslineno[0]);
		if(exit > 0)
		{
			rec->exit  = exit;
			
			CString alarm;
			alarm.Format("����tg.def��[SPECIAL_EXIT_ENTRY]=%d ���� ��վ%d-%d վ��%d-%d lineno %d������ %s exit %d", 
											m_nCurrentSectionNo, 
											pre->station,rec->station, 
											pre->out_station_index,rec->in_station_index, 
											abslineno[0],this->GetTrainShowId(train), rec->exit);

			this->sysprint(10,0, "%s", alarm);
			
			return rec->exit; // �ɹ�
		}
		else
		{
			CString alarm;
			alarm.Format("���� %s ���, ����tg.def��[SPECIAL_EXIT_ENTRY]���õ� lineno %d ��entry.cfg�г�վ %d û�����ó����", 
							this->GetTrainShowId(train), abslineno[0], rec->station);
				
			this->sysprint(10,0, "%s", alarm);
		}
	}
	
	return -1;
}

int CTGDoc::SetTrainEntryByCfg(CTG_TRAIN* train, int rec_index)
{
	if(train==NULL)
		return -1;

	TgTRAIN_RECORD *rec = train->GetRecordByPosition(rec_index);
	TgTRAIN_RECORD *nxt = train->GetRecordByPosition(rec_index+1);
	if(rec==NULL || nxt==NULL)
		return -1;

	if(!this->JudeNodesConnected(rec->out_station_index, nxt->in_station_index))
	{
		return -1;
	}

	int abslineno[10];
	int nAbsLineNum = m_ptgctrl->mConfigTg.m_tgdef.GetSpecialExitEntryLineNo(m_nCurrentSectionNo, 
											rec->station,nxt->station, 
											rec->out_station_index,nxt->in_station_index,abslineno);
	if(nAbsLineNum > 0)
	{
		int entry  = ConvertAbsnumStationToEntry(rec->station, abslineno[0]);
		if(entry > 0)
		{
			rec->entry = entry;
						
			CString alarm;
			alarm.Format("����tg.def��[SPECIAL_EXIT_ENTRY]=%d ���� ��վ%d-%d վ��%d-%d lineno %d������ %s entry %d", 
											m_nCurrentSectionNo, 
											rec->station, nxt->station, 
											rec->out_station_index,nxt->in_station_index, 
											abslineno[0], this->GetTrainShowId(train), rec->entry);

			this->sysprint(10,0, "%s", alarm);
			
			return rec->entry; // �ɹ�
		}
		else
		{
			CString alarm;
			alarm.Format("���� %s ���, ����tg.def��[SPECIAL_EXIT_ENTRY]���õ� lineno %d ��entry.cfg�г�վ %d û�����ó����", 
						this->GetTrainShowId(train), abslineno[0], rec->station);
				
			this->sysprint(10,0, "%s", alarm);
		}
	}
		
	return -1;
}

int CTGDoc::SetTrainExitByCfgForUI(CTG_TRAIN* train, int rec_index, int abslineno[])
{
	if(train==NULL)
		return -1;

	TgTRAIN_RECORD *pre = train->GetRecordByPosition(rec_index-1);
	TgTRAIN_RECORD *rec = train->GetRecordByPosition(rec_index);
	if(pre==NULL || rec==NULL)
		return -1;

	if(!this->JudeNodesConnected(pre->out_station_index, rec->in_station_index))
	{
		return -1;
	}

	int nAbsLineNum = m_ptgctrl->mConfigTg.m_tgdef.GetSpecialExitEntryLineNo(m_nCurrentSectionNo, 
											pre->station,rec->station, 
											pre->out_station_index,rec->in_station_index, abslineno);
	return nAbsLineNum;
}

int CTGDoc::SetTrainEntryByCfgForUI(CTG_TRAIN* train, int rec_index, int abslineno[])
{
	if(train==NULL)
		return -1;

	TgTRAIN_RECORD *rec = train->GetRecordByPosition(rec_index);
	TgTRAIN_RECORD *nxt = train->GetRecordByPosition(rec_index+1);
	if(rec==NULL || nxt==NULL)
		return -1;

	if(!this->JudeNodesConnected(rec->out_station_index, nxt->in_station_index))
	{
		return -1;
	}

	int nAbsLineNum = m_ptgctrl->mConfigTg.m_tgdef.GetSpecialExitEntryLineNo(m_nCurrentSectionNo, 
											rec->station,nxt->station, 
											rec->out_station_index,nxt->in_station_index,abslineno);
	return nAbsLineNum;
}

// �������Զ�ƽ������ʩ��
void CTGDoc::AddMarkMoveIndex(ULONG index)
{
	mark_move_index[index]=index;
	WriteToFile();
	this->sysprint(10,0, "AddMarkMoveIndex %d size=%d", index, mark_move_index.size());
}

void CTGDoc::DelMarkMoveIndex(ULONG index)
{
	std::map<ULONG,ULONG>::iterator it=mark_move_index.find(index);
	if(it!=mark_move_index.end())
	{
		it->second=0;
		WriteToFile();
		this->sysprint(10,0, "DelMarkMoveIndex %d size=%d", index,  mark_move_index.size());
	}
}

bool CTGDoc::IsMarkMove(ULONG index)
{
	std::map<ULONG,ULONG>::iterator it=mark_move_index.find(index);
	if(it!=mark_move_index.end())
	{
		if(it->second>0)
			return true;
	}

	return false;
}

void CTGDoc::ReadFromFile()
{
	CString pathname="";
	pathname.Format("%s\\automovemark%d.xml", this->GetDataPath(), m_nCurrentSectionNo);
	mark_move_index.clear();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		return; 
	}
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode;
	

	pRootNode = pXMLDoc->selectSingleNode( "ROOT");
	if(pRootNode == NULL)
	{
		return;
	}

	// THIS_CONSOLE
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "INDEX");
	for(int m = 0; m < list->Getlength(); m++)
	{
		MSXML::IXMLDOMNodePtr pNode = list->Getitem(m);
		if(pNode==NULL)
			continue;
			
		_variant_t v;
		MSXML::IXMLDOMNodePtr pChildNode=pNode->selectSingleNode("@del");
		if(pChildNode)
		{
			v = pChildNode->GetnodeTypedValue();
			int del = atol(CString(v.bstrVal));	
			if(del<=0)
				continue;
		}

		ULONG index=0;
		v = pNode->GetnodeTypedValue();
		index = atol(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
		
		if(index>0)
		{
			mark_move_index[index]=index;
			this->sysprint(10,0, "ReadMarkMoveIndex %d size=%d", index,  mark_move_index.size());
		}
	}

	WriteToFile();
}
	
void CTGDoc::WriteToFile()
{
	CString fileName;
	fileName.Format("%s\\automovemark%d.xml", this->GetDataPath(), m_nCurrentSectionNo);
	CStdioFile file;
	if(file.Open(fileName, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString tmp;

		file.WriteString("<ROOT>");
		std::map<ULONG,ULONG>::iterator it=mark_move_index.begin();
		for(; it!=mark_move_index.end(); it++)
		{
			tmp.Format("\t<INDEX del=\"%d\">%d</INDEX>", it->second, it->first);
			file.WriteString(tmp);
		}
		file.WriteString("</ROOT>");
		file.Flush();
		file.Close();
	}
}

int  CTGDoc::GetTrainDirectByTrainId(CString trainId)
{
	int nFocusTrainDirect=-1;

	int len = trainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = trainId.GetAt(i);
		if(0x30<=lastchar && lastchar<=0x39)
		{
			if(lastchar%2 == 0)
				nFocusTrainDirect = UP_DIRECT;
			else
				nFocusTrainDirect = DOWN_DIRECT;
			break;
		}
	}
	
	return nFocusTrainDirect;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CTGDoc::GetEarlyLateTimeRd(long train_index, int station, int rec_index,CString& trainid)
{
	CString str;
	str.Format("%d_%d_%d_%s", train_index,  station,  rec_index,trainid);
	std::map<CString, LateInfo>::iterator it = this->m_ptgctrl->mConfigTg.m_earlylatetimelog.find(str);
	if(it != this->m_ptgctrl->mConfigTg.m_earlylatetimelog.end())
		return it->second.flag;
	return 0;
}

// ��¼�����
void CTGDoc::SetLateEarlyTimeRd(long train_index, int station, int rec_index, BYTE flag,CString& trainid)
{
	CString str;
	str.Format("%d_%d_%d_%s", train_index,  station,  rec_index, trainid);
	
	this->m_ptgctrl->AddEarlyLateInfo(str, flag, GetShiftId());
}

/*
*	nRecordNo: �ƶ���� ���
*	nTimeOffset: ʱ��仯
*   nFlag: 1 ����� 2 ����� 3 �������
**/
int CTGDoc::OffsetWorkScheduleTimeByRouteOk(CTG_TRAIN *train, int nRecordNo, int nTimeOffset, int nFlag)
{
	if(NULL == train)
		return -1;

	UINT nNum=train->GetRecordNum();
	if(nRecordNo >= nNum || nRecordNo < 0)
		return -1;

	bool bMoveAfterTime = m_ptgctrl->mConfigTg.m_context.IsMoveAfterTime(); // �Ƿ�ƽ�ƺ��������� true:ƽ�ƺ��� false:��ƽ�ƺ���

	TgTRAIN_RECORD* rec = train->GetRecordByRecIndex(nRecordNo);
	if(rec==NULL)
		return 0;

	if(nFlag == 3)
	{
		if(IsArriveRouteOk(*rec) || IsDepartRouteOk(*rec))
			return 0;
		rec->arrive += nTimeOffset;
		rec->depart += nTimeOffset;
	}
	else if(nFlag == 2)
	{
		if(IsArriveRouteOk(*rec))
			return 0;

		rec->arrive += nTimeOffset;
		if(!bMoveAfterTime && IsDepartRouteOk(*rec))
		{
			if(rec->arrive>=rec->depart)
				rec->arrive -= nTimeOffset;
			return 0;
		}
		rec->depart += nTimeOffset;
	}
	else if(nFlag == 1)
	{
		if(IsDepartRouteOk(*rec))
			return 0;

		rec->depart += nTimeOffset;
	}
	else 
	{
		return 0;
	}

	if(!bMoveAfterTime && IsDepartRouteOk(*rec))
		return 0;
	
	// ѡ�г�վ������ʵ�ʵ�, ������վ��ƫ��
	if(IsActualDepartTime(rec->adjust_status))
		return 0;

	for(UINT idx = nRecordNo+1; idx < nNum; idx++)
	{
		TgTRAIN_RECORD* prec = train->GetRecordByRecIndex(idx);
		if(prec==NULL)
			break;
		if(IsActualArrivalTime(prec->adjust_status) || IsActualDepartTime(prec->adjust_status))
			break;
		
		if(!bMoveAfterTime && (IsArriveRouteOk(*prec) || IsDepartRouteOk(*prec)))
			break;

		prec->arrive += nTimeOffset;
		prec->depart += nTimeOffset;
	}
	
	return 0;
}

int  CTGDoc::GetEntry(int node_index)
{
	if(node_index < 0 || node_index >= this->m_nDispDistanceNum)
		return 0;

	return m_sDispDistance[node_index].entry;
}

int  CTGDoc::GetExit(int node_index)
{
	if(node_index < 0 || node_index >= this->m_nDispDistanceNum)
		return 0;

	return m_sDispDistance[node_index].exit;
}

bool CTGDoc::IsAbsBySide(HWND hWnd, BYTE side_no, int rec_index, CTG_TRAIN *pTrain, int param)
{
	if(NULL==pTrain)
		return false;

	TgTRAIN_RECORD *rec = pTrain->GetRecordByPosition(rec_index);
	if(NULL == rec)
		return false;
	
	CString strTrainId=GetTrainShowId(pTrain);
	// �ɵ����������ж�
	if(param==1)
	{
		TgTRAIN_RECORD *pre_rec = pTrain->GetRecordByPosition(rec_index-1);
		if(pre_rec==NULL)
			return false;

		CString str;
		int exit=0, entry=0;
		if(m_ptgctrl->mConfigTg.IsThisStationEntryBySide(pre_rec->station, rec->station, side_no, exit, entry, str))
		{
			CString strText;
			strText.Format("%s ������'%s' ���� '%s' ����'%s':%s, ��ȷ���޸Ĺɵ���ʹ�ø�������?", 
				strTrainId,
				GetStationName(pre_rec->station), 
				str,
				GetStationName(rec->station), 
				GetSideName(rec->station, side_no));
			if(MessageBox(hWnd, strText, "��ʾ", MB_OK|MB_ICONQUESTION|MB_YESNO)==IDYES)
			{
				sysprint(4,0,"[TG]:%s, ����Աѡ��'��'��", strText);
				rec->entry=entry;
				pre_rec->exit=exit;
				return true;
			}
			else
			{
				sysprint(4,0,"[TG]:%s, ����Աѡ��'��'��", strText);
			}
		}
	}
	else if(param==2)
	{
		TgTRAIN_RECORD *next_rec = pTrain->GetRecordByPosition(rec_index+1);
		if(next_rec==NULL)
			return false;
		
		CString str;
		int exit=0, entry=0;
		if(m_ptgctrl->mConfigTg.IsThisStationExitBySide(rec->station, next_rec->station, side_no, exit, entry, str))
		{
			CString strText;
			strText.Format("%s ������'%s':%s ���� '%s' ����'%s', ��ȷ���޸Ĺɵ���ʹ�ø�������?", 
				strTrainId,
				GetStationName(rec->station), 
				GetSideName(rec->station, side_no),
				str, 
				GetStationName(next_rec->station));
			if(MessageBox(hWnd, strText, "��ʾ", MB_OK|MB_ICONQUESTION|MB_YESNO)==IDYES)
			{
				sysprint(4,0,"[TG]:%s, ����Աѡ��'��'��", strText);
				rec->exit=exit;
				next_rec->entry=entry;
				return true;
			}
			else
			{
				sysprint(4,0,"[TG]:%s, ����Աѡ��'��'��", strText);
			}
		}
	}
	
	return false;
}

void CTGDoc::SetAllTrainsErrorSide(int dis_index, int station_no)
{
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for( ; it != myPDataManage->myTrain.end(); ++it)
	{
		CTG_TRAIN train = it->second;    // ����ֱ�Ӹ��ڴ�����

		if (train.myRecord.size() <= 0 ) 
			continue;

		int pos=-1;
		for (int i=0; i<train.GetRecordNum(); i++)
		{
			TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(i);
			if (!rec)
				continue;

			if (rec->station != station_no)
				continue;

			if (rec->out_station_index == dis_index || rec->in_station_index == dis_index)
			{
				rec->arrive_side=255;
				rec->depart_side=255;

				sysprint(4,0,"(SetAllTrainsErrorSide) ����Train %s(%d) ��վ %d �ɵ�Ϊ?��", train.GetTrainId(), train.GetTrainIndex(), rec->station);

				pos=i;
			}
		}

		if (pos>=0)
			SendUpdateTrain(train, SCHD_UPDATE, MODIFY_SIDE, 0, pos);
	}
}

CStationDef* CTGDoc::GetStationSideDef(int station)
{
	CStationDef *pStationDef=NULL;
	for(int i=0; i<m_nStationNum; i++)
	{
		if(m_pStationDef[i]==NULL)
			continue;
		
		if(m_pStationDef[i]->GetStationNo()==station)
		{
			pStationDef=m_pStationDef[i];
			break;
		}
	}
	
	return pStationDef;
}

bool  CTGDoc::ArriveSideSetEntry(HWND hWnd, int arrive_side, int rec_index, CTG_TRAIN *pTrain)
{
	if(NULL==pTrain)
		return false;

	const TgTRAIN_RECORD* rec=pTrain->GetRecordByRecIndex(rec_index);
	if(rec==NULL)
		return false;

	if(CTC_STATION != GetStationProperty(rec->station))
	{
		return true; // TDCS ��վ������
	}

	CString trainName = GetTrainShowId(pTrain);
	if(IsFlagStart(rec->flag) && (0==rec_index)) 
	{
		;// ʼ����,�������ڹɵ�
	}
	else
	{
		if (!IsEntrySideConnect(rec->station, arrive_side, rec->entry))
		{
			CString strText;
			strText.Format("��ע��:�� %s �г� %s ���ܴ���һվ���뵽 %s!", 
					GetStationName(rec->station), trainName, GetSideName(rec->station, arrive_side));
			
			MessageBox(hWnd, strText, "����", MB_OK|MB_ICONHAND);
			if(!IsAbsBySide(hWnd, arrive_side, rec_index, pTrain, 1))
			{
				return false;
			}
		}
	}

	return true;
}

bool  CTGDoc::DepartSideSetEntry(HWND hWnd, int depart_side, int rec_index, CTG_TRAIN *pTrain)
{
	if(NULL==pTrain)
		return false;

	const TgTRAIN_RECORD* rec=pTrain->GetRecordByRecIndex(rec_index);
	if(rec==NULL)
		return false;

	if(CTC_STATION != GetStationProperty(rec->station))
	{
		return true; // TDCS ��վ������
	}

	CString trainName = GetTrainShowId(pTrain);
	if(IsFlagTerminal(rec->flag) && (pTrain->GetRecordNum()==rec_index+1)) 
	{
		;// �յ���,�������ڹɵ�
	}
	else
	{
		if (!IsSideExitConnect(rec->station, depart_side, rec->exit))
		{
			CString strText;
			strText.Format("��ע��:�� %s �г� %s ���ܴ� %s ��������һվ", 
					GetStationName(rec->station), trainName, GetSideName(rec->station, depart_side));
			
			MessageBox(hWnd, strText, "����", MB_OK|MB_ICONHAND);
			if(!IsAbsBySide(hWnd, depart_side, rec_index, pTrain, 2))
			{
				return false;
			}
		}
	}
	return true;
}

// ���ɵ��ͳ�����Ƿ���ͨ,�������ͨ,��ͼ���չɵ�
bool  CTGDoc::SideSetEntry(HWND hWnd, int arrive_side, int depart_side, int rec_index, CTG_TRAIN *pTrain)
{
	if(NULL==pTrain)
		return false;

	const TgTRAIN_RECORD* rec=pTrain->GetRecordByRecIndex(rec_index);
	if(rec==NULL)
		return false;

	if(CTC_STATION != GetStationProperty(rec->station))
	{
		return true; // TDCS ��վ������
	}

	CString trainName = GetTrainShowId(pTrain);
	if(IsFlagStart(rec->flag) && (0==rec_index)) 
	{
		;// ʼ����,�������ڹɵ�
	}
	else
	{
		if (!IsEntrySideConnect(rec->station, arrive_side, rec->entry))
		{
			CString strText;
			strText.Format("��ע��:�� %s �г� %s ���ܴ���һվ���뵽 %s!", 
					GetStationName(rec->station), trainName, GetSideName(rec->station, arrive_side));
			
			MessageBox(hWnd, strText, "����", MB_OK|MB_ICONHAND);
			if(!IsAbsBySide(hWnd, arrive_side, rec_index, pTrain, 1))
			{
				return false;
			}
		}
	}

	if(IsFlagTerminal(rec->flag) && (pTrain->GetRecordNum()==rec_index+1)) 
	{
		;// �յ���,�������ڹɵ�
	}
	else
	{
		if (!IsSideExitConnect(rec->station, depart_side, rec->exit))
		{
			CString strText;
			strText.Format("��ע��:�� %s �г� %s ���ܴ� %s ��������һվ", 
					GetStationName(rec->station), trainName, GetSideName(rec->station, depart_side));
			
			MessageBox(hWnd, strText, "����", MB_OK|MB_ICONHAND);
			if(!IsAbsBySide(hWnd, depart_side, rec_index, pTrain, 2))
			{
				return false;
			}
		}
	}
	return true;
}

bool CTGDoc::IsEnableChangeSide(HWND hWnd, TgTRAIN_RECORD *rec, CTG_TRAIN *pTrain)
{
	if(NULL==pTrain || NULL==rec)
		return false;
	// ��·���޸Ĺɵ� �޸Ĺɵ���ʾ�û�
	if(m_ptgctrl->mConfigTg.m_context.IsSwtichStation(rec->station))
	{
		CSuperPassDlg pdlg;
		if (pdlg.DoModal() == IDOK)
		{
			if (pdlg.m_super_pass.Compare("1234") != 0)
			{
				CString str;
				str.Format("%s","�������!�޸Ĺɵ�ʧ��!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	//��·���ų�,�ɵ������Ҫ �˹�ȷ��
	if(IsArriveRouteOk(*rec) && (CTC_STATION == GetStationProperty(rec->station)) )
	{
		CString strText="";
		strText = "�ӳ���·�Ѱ���,ȷʵҪ�޸ĸó��Ĺɵ���?";
		sysprint(4,0,"%s",strText);
		if(MessageBox(hWnd, strText, "����", MB_YESNO|MB_ICONHAND)==IDYES)
		{
			sysprint(4,0,"����޸Ĺɵ�");;
		}
		else
		{
			sysprint(4,0,"ȡ���޸Ĺɵ�");
			return false;
		}
	}
	
	if(TG_IsTrainIntoSide(*rec) && (CTC_STATION == GetStationProperty(rec->station)) )
	{
		CString strText="";
		strText = "�г��ѽ�վ,ȷʵҪ�޸ĸó��Ĺɵ���?";
		sysprint(4,0,"%s",strText);
		if(MessageBox(hWnd, strText, "����", MB_YESNO|MB_ICONHAND)==IDYES)
		{
			sysprint(4,0,"����޸Ĺɵ�");;
		}
		else
		{
			sysprint(4,0,"ȡ���޸Ĺɵ�");
			return false;
		}
	}

	// 20101218 ̫ԭ ֻ���ض������ص㳵�Ĺɵ���� 
    if(m_ptgctrl->mConfigTg.m_context.IsCheckCrhAndImpTrackChange())
	{
		CString strText="";
		if(pTrain->IsCHRTrain() || pTrain->myTrainInfo.super_rank>0 )
		{
			strText ="ȷ���޸ĸö���/�ص㳵�Ĺɵ���?";
			sysprint(4,0,"%s",strText);
			if(MessageBox(hWnd, strText, "����", MB_YESNO|MB_ICONHAND)==IDYES)
			{
				sysprint(4,0,"����޸Ĺɵ�");
			}
			else
			{
				sysprint(4,0,"ȡ���޸Ĺɵ�");
				return false;
			}
		}
	}

	return true;
}

bool CTGDoc::IsGoodTrainNotTechStopNeedPwd()
{
	if(m_bGoodTrainNotTechStopNeedPwd==TRUE)
		return true;
	else
		return false;
}

int  CTGDoc::GetLineNoByStations(int from_st, int to_st)
{
	for(int i=0; i<mSetLineNoAry.size(); i++)
	{
		if(mSetLineNoAry[i].station==from_st && mSetLineNoAry[i].nextstation==to_st)
		{
			return mSetLineNoAry[i].line_num;
		}
	}
	return -1;
}

void CTGDoc::DrawTrainNoteText(CDC * pCDC,CTG_TRAIN * single)
{
	int dltx = 5;
	int dlty = 0;
	int nInStaOffset=0;
	int nOutStaOffset=0;

	int  nOldColor = pCDC->SetTextColor(RGB(255,0,0));
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pCDC->SelectObject(&fontTech);

	int nTrainDirect = single->m_nDirectFlag;

	TgTRAIN_RECORD *rec = NULL, *next=NULL;
	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			continue;

		CString strWorkText;
		strWorkText=rec->szPlanNote;
		if(strWorkText.IsEmpty())
			continue;

		next = single->GetRecordByPosition(i+1);
		if(next != NULL)
		{
			if(nRecNum == 2 && rec->station == next->station)
			{
				nTrainDirect = single->m_nDirectFlag;
			}
			else
			{
				int Yrec = GetStationVPos(rec->out_station_index,  PRIMARY_TRACK);
				int Ynxt = GetStationVPos(next->in_station_index,  PRIMARY_TRACK);
				if(Yrec > Ynxt)
					nTrainDirect = UP_DIRECT;
				else
					nTrainDirect = DOWN_DIRECT;
			}
		}

		nInStaOffset = GetStationVPos(rec->in_station_index,  PRIMARY_TRACK)+dlty;
		nOutStaOffset = -1;
		// ��ںͳ�����ͬ
		if(rec->in_station_index != rec->out_station_index)
		{
			nOutStaOffset = GetStationVPos(rec->out_station_index, PRIMARY_TRACK)+dlty;
		}

		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive)+dltx;
		int departOffset = GetXPositionConsiderSecond(rec->depart)+dltx;	
		int Xpos = arrivalOffset - 5;//(arrivalOffset+departOffset)/2;
		if(nTrainDirect == UP_DIRECT)
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		}
		else
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		}

		CSize sz=pCDC->GetTextExtent(strWorkText);
		if(arrivalOffset == departOffset)
		{
			if(nRecNum == (i+1))
			{
				if(rec->in_station_index >=0 && rec->in_station_index < m_nDispDistanceNum)
				{
					pCDC->TextOut(Xpos, nInStaOffset, strWorkText);

					if(m_bIsVerifyTrainRect)
					{
					CRect rect;
					if(nTrainDirect == UP_DIRECT)
						rect=CRect(Xpos-sz.cx, nInStaOffset, Xpos, nInStaOffset+sz.cy);
					else
						rect=CRect(Xpos-sz.cx, nInStaOffset-sz.cy, Xpos, nInStaOffset);
					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
				// ��ںͳ��ڲ���ͬ
				if(rec->in_station_index != rec->out_station_index && rec->out_station_index>=0 && rec->out_station_index < m_nDispDistanceNum)
				{
					pCDC->TextOut(Xpos, nOutStaOffset,  strWorkText);
					
					if(m_bIsVerifyTrainRect)
					{
					CRect rect;
					if(nTrainDirect == UP_DIRECT)
						rect=CRect(Xpos-sz.cx, nOutStaOffset, Xpos, nOutStaOffset+sz.cy);
					else
						rect=CRect(Xpos-sz.cx, nOutStaOffset-sz.cy, Xpos, nOutStaOffset);

					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
			else
			{
				if(rec->in_station_index >=0 && rec->in_station_index < m_nDispDistanceNum)
				{
					pCDC->TextOut(Xpos, nInStaOffset, strWorkText);
					
					if(m_bIsVerifyTrainRect)
					{
					CRect rect;
					if(nTrainDirect == UP_DIRECT)
						rect=CRect(Xpos-sz.cx, nInStaOffset, Xpos, nInStaOffset+sz.cy);
					else
						rect=CRect(Xpos-sz.cx, nInStaOffset-sz.cy, Xpos, nInStaOffset);

					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}

				// ��ںͳ��ڲ���ͬ
				if(rec->in_station_index != rec->out_station_index && rec->out_station_index>=0 && rec->out_station_index < m_nDispDistanceNum)
				{
					pCDC->TextOut(Xpos, nOutStaOffset,  strWorkText);
					
					if(m_bIsVerifyTrainRect)
					{
					CRect rect;
					if(nTrainDirect == UP_DIRECT)
						rect=CRect(Xpos-sz.cx, nOutStaOffset, Xpos, nOutStaOffset+sz.cy);
					else
						rect=CRect(Xpos-sz.cx, nOutStaOffset-sz.cy, Xpos, nOutStaOffset);

					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
		}
		else
		{
			if(rec->in_station_index >=0 && rec->in_station_index < m_nDispDistanceNum)
			{
				pCDC->TextOut(Xpos, nInStaOffset, strWorkText);
				
				if(m_bIsVerifyTrainRect)
				{
				CRect rect;
				if(nTrainDirect == UP_DIRECT)
					rect=CRect(Xpos-sz.cx, nInStaOffset, Xpos, nInStaOffset+sz.cy);
				else
					rect=CRect(Xpos-sz.cx, nInStaOffset-sz.cy, Xpos, nInStaOffset);

				#ifdef DRAW_TRAIN_RECT	
					ShowRect(pCDC, rect);
				#endif
				single->UnionRect(rect);
				}
			}

			// ��ںͳ��ڲ���ͬ
			if(rec->in_station_index != rec->out_station_index  && rec->out_station_index>=0 && rec->out_station_index < m_nDispDistanceNum)
			{
				if(m_nTrainLeapLineFlag == 2)
				{
					pCDC->TextOut(Xpos, nOutStaOffset, strWorkText);
					
					if(m_bIsVerifyTrainRect)
					{
					CRect rect;
					if(nTrainDirect == UP_DIRECT)
						rect=CRect(Xpos-sz.cx, nOutStaOffset, Xpos, nOutStaOffset+sz.cy);
					else
						rect=CRect(Xpos-sz.cx, nOutStaOffset-sz.cy, Xpos, nOutStaOffset);

					#ifdef DRAW_TRAIN_RECT	
						ShowRect(pCDC, rect);
					#endif
					single->UnionRect(rect);
					}
				}
			}
		}
	}

	// �ָ�
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
}

void CTGDoc::DrawTrainGiveoutDirection(CDC * pCDC, TgTRAIN_RECORD * prec, int direct_flag, const CPoint& ptlStart, CTG_TRAIN * single)
{
	if(prec==NULL)
		return;
	
	const char* pstr=NULL;
	if(mDirectionTipAry.size()>0)
	{
		WORD NextLine,direction;
		if(!ConvertStationEntryToAbsnum(prec->station,prec->exit,&NextLine,&direction))
			return;
		
		const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(NextLine);
		if(line == NULL)
			return;

		int station=prec->station;
		int next_station=0;
		if(prec->station == line->nStartStation)
			next_station=line->nEndStation;
		else if(prec->station == line->nEndStation)
			next_station=line->nStartStation;
		
		for(int i=0; i<mDirectionTipAry.size(); i++)
		{
			if(mDirectionTipAry[i].station==station && mDirectionTipAry[i].next_station==next_station)
			{
				pstr=mDirectionTipAry[i].tip;
				break;
			}
		}
	}

	if(mGiveoutTipAry.size()>0)
	{
		for(int i=0; i<mGiveoutTipAry.size(); i++)
		{
			if(mGiveoutTipAry[i].station==prec->station && mGiveoutTipAry[i].exit==prec->exit)
			{
				pstr=mGiveoutTipAry[i].tip;
				break;
			}
		}
	}

	if(pstr==NULL)
		return;
	
	CString str=pstr;

	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT oldAlign=pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	UINT oldColor=pCDC->SetTextColor(RGB(255,0,0));
	CFont *pOldFont = pCDC->SelectObject(&fontTech);		 
	CSize sz=pCDC->GetTextExtent(str);
	CRect rect;
	if(direct_flag==UP_MARK)
	{
		pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pCDC->TextOut(ptlStart.x, ptlStart.y-15, str);
		
		rect=CRect(ptlStart.x, ptlStart.y-20-sz.cy, ptlStart.x+sz.cx, ptlStart.y-10);
	}
	else
	{
		pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlStart.x, ptlStart.y+15, str);

		rect=CRect(ptlStart.x, ptlStart.y+10, ptlStart.x+sz.cx, ptlStart.y+20+sz.cy);
	}
	
	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, rect);
	#endif
	single->UnionRect(rect);
	}
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextAlign(oldAlign);
	pCDC->SetTextColor(oldColor);
	pCDC->SelectObject(pOldFont);
}

void CTGDoc::DrawTrainAcceptDirection(CDC * pCDC,TgTRAIN_RECORD * prec,int direct_flag, const CPoint& ptlStart,CTG_TRAIN * single)
{
	if(prec==NULL)
		return;
	
	const char* pstr=NULL;
	if(mAcceptTipAry.size()>0)
	{
		for(int i=0; i<mAcceptTipAry.size(); i++)
		{
			if(mAcceptTipAry[i].station==prec->station && mAcceptTipAry[i].entry==prec->entry)
			{
				pstr=mAcceptTipAry[i].tip;
				break;
			}
		}
	}

	if(pstr==NULL)
		return;
	
	CString str=pstr;

	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT oldAlign=pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	UINT oldColor=pCDC->SetTextColor(RGB(255,0,0));
	CFont *pOldFont = pCDC->SelectObject(&fontTech);		 
	
	CRect rect;
	CSize sz = pCDC->GetTextExtent(str);
	if(direct_flag==UP_MARK)
	{
		pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pCDC->TextOut(ptlStart.x, ptlStart.y, str);
		rect=CRect(ptlStart.x, ptlStart.y-sz.cy, ptlStart.x+sz.cx, ptlStart.y);
	}
	else
	{
		pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlStart.x, ptlStart.y, str);
		rect=CRect(ptlStart.x, ptlStart.y, ptlStart.x+sz.cx, ptlStart.y+sz.cy);
	}

	if(m_bIsVerifyTrainRect)
	{
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, rect);
	#endif
	single->UnionRect(rect);
	}

	pCDC->SetROP2(nPreviousMode); 
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextAlign(oldAlign);
	pCDC->SetTextColor(oldColor);
	pCDC->SelectObject(pOldFont);
}

CTG_TRAIN* CTGDoc::GetPrevTrainByIndex(long index)
{
	std::map<ULONG, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for(; it != myPDataManage->myTrain.end(); it++)
	{
		if(it->second.prev_train_index == index)
			return &it->second;
	}
	return NULL;
}

void CTGDoc::ReGenTrainJiaoluRelation()
{
	sysprint(10, 0, "|=======���������г���·��ϵ=======|");
	if(!m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
		return;
	myChezuhao.ClearAllTrainCheHao();
	
	vector<TrainGroupInfo> list;
	myPDataManage->GetTrainRelation(list);
	for(int i=0; i<list.size(); i++)
	{
		if(list[i].train_count<=1)
			continue;
		for(int n=0; n<list[i].train_count; n++)
		{
			CTG_TRAIN* ptrain=myPDataManage->GetTrain(list[i].trains[n].train_index);
			if(ptrain)
				myChezuhao.AppendCheZuHaoByRelation(*ptrain, n+1, list[i].strName);
		}
	}
} 

void CTGDoc::GetJiaoluDlgName(CString& title1, CString& title2)
{
	CString mSectionName;
	mSectionName=m_ptgctrl->mConfigTg.GetSectionName(m_nCurrentSectionNo);
	title1.Format("%s--%s--������·�༭(%X)", mSectionName, m_ptgctrl->GetTermTGTypeName(), this);
	title2.Format("%s--%s--�г���·Ԥ��(%X)", mSectionName, m_ptgctrl->GetTermTGTypeName(), this);
}

void CTGDoc::GetChezuhaoDlgName(CString& title1, CString& title2)
{
	CString mSectionName;
	mSectionName=m_ptgctrl->mConfigTg.GetSectionName(m_nCurrentSectionNo);
	title1.Format("%s--%s--��������༭(%X)", mSectionName, m_ptgctrl->GetTermTGTypeName(), this);
	title2.Format("%s--%s--�г���·Ԥ��(%X)", mSectionName, m_ptgctrl->GetTermTGTypeName(), this);
}

// ����δ֪�г�
void CTGDoc::DrawUnknowTrainFlag(CDC *pCDC, CTG_TRAIN * single)
{
	int nOldMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldTextColor = pCDC->SetTextColor(CLR_BLUE);
	int	nOldAlign = pCDC->SetTextAlign(TA_CENTER);
	CFont* pOldFont = pCDC->SelectObject(&fontText);
	CSize sz = pCDC->GetTextExtent("δ֪�г�");
	int in_station_index=-1, out_station_index=-1;
	TgTRAIN_RECORD *rec = NULL;
	int nRecNum = single->GetRecordNum();  
	for(int i = 0; i< nRecNum; i++)
	{
		rec = single->GetRecordByPosition(i);
		if(rec == NULL)
			break;
		
		if(rec->in_station_index>=0)
			in_station_index = rec->in_station_index;
		else
			in_station_index = rec->out_station_index;

		if(rec->out_station_index>=0)
			out_station_index = rec->out_station_index;
		else
			out_station_index = rec->in_station_index;

		if(in_station_index < 0 || out_station_index < 0)
			continue;

		int nInStaOffset  = GetStationSideVPos(rec, in_station_index, rec->arrive_side);
		int nOutStaOffset = GetStationSideVPos(rec, out_station_index, rec->depart_side);
		
		int arrivalOffset = GetXPositionConsiderSecond(rec->arrive);
		int departOffset = GetXPositionConsiderSecond(rec->depart);
		
		pCDC->TextOut(departOffset, nInStaOffset, "δ֪�г�");
		
		if(m_bIsVerifyTrainRect)
		{
		CRect rect=CRect(departOffset-sz.cx/2-1, nInStaOffset-sz.cy, departOffset+sz.cx/2+1, nInStaOffset+sz.cy);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}

		if(nInStaOffset!=nOutStaOffset)
		{
			pCDC->TextOut((departOffset+arrivalOffset)/2, nOutStaOffset, "δ֪�г�");

			if(m_bIsVerifyTrainRect)
			{
			CRect rect=CRect((departOffset+arrivalOffset)/2-sz.cx/2, nOutStaOffset-sz.cy, (departOffset+arrivalOffset)/2+sz.cx/2+1, nOutStaOffset+sz.cy);
			#ifdef DRAW_TRAIN_RECT	
				ShowRect(pCDC, rect);
			#endif
			single->UnionRect(rect);
			}
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(nOldTextColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
			
	return;
}

void CTGDoc::ShowRect(CDC*pCDC, const CRect& rect)
{
	if(rect.IsRectEmpty())
		return;
	pCDC->MoveTo(rect.left,  rect.top);
	pCDC->LineTo(rect.right, rect.top);
	pCDC->LineTo(rect.right, rect.bottom);
	pCDC->LineTo(rect.left,  rect.bottom);
	pCDC->LineTo(rect.left,  rect.top);
}

void CTGDoc::CreateGDIInfo(CDC* pCDC)
{
	LOGFONT stFont;	// ��ʼ������
	memset(&stFont,0,sizeof(stFont));
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Arial Narrow");

	stFont.lfWeight = trainid_style.nomorl_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.nomorl_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontNomorlTrain.CreateFontIndirect(&stFont);

	stFont.lfWeight = trainid_style.elec_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.elec_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontElecTrain.CreateFontIndirect(&stFont);
	
	stFont.lfWeight = trainid_style.space_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.space_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontSpaceTrain.CreateFontIndirect(&stFont);

	stFont.lfWeight = trainid_style.length_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.length_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontLengthTrain.CreateFontIndirect(&stFont);

	stFont.lfWeight = trainid_style.military_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.military_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontMilitaryTrain.CreateFontIndirect(&stFont);

	stFont.lfWeight = trainid_style.super_rank.weight;
	stFont.lfHeight = MulDiv(trainid_style.super_rank.height, -pCDC->GetDeviceCaps(LOGPIXELSY), 72);
	fontSuperTrain.CreateFontIndirect(&stFont);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(10,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_BOLD; 
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	fontEngine.CreateFontIndirect(&stFont);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Arial Narrow");
	fontReturnTrain.CreateFontIndirect(&stFont);

	mLeftPen.CreatePen(PS_SOLID, decorateStyle.width, decorateStyle.lcolor);
	mRightPen.CreatePen(PS_SOLID, decorateStyle.width, decorateStyle.rcolor);
}

void CTGDoc::GetViewScope(TIME& begin, TIME& end)
{
	begin=this->GetBeginTime();
	end=begin+m_ptgctrl->GetTimeLength()*3600;
}

void CTGDoc::GetAcceptDataScope(TIME& begin, TIME& end)
{
	GetViewScope(begin, end);
	end=end+600; // �ɽ��������ӳ�10����
}

bool CTGDoc::IsOnlyShowMark(void) const 
{ 
	if(m_nShowFlag==1) 
		return true;
	return false;
}

bool CTGDoc::IsOnlyShowTrain(void) const 
{ 
	if(m_nShowFlag==2) 
		return true;
	return false;
}

bool CTGDoc::IsShowTrainAndMark(void) const 
{ 
	if(m_nShowFlag==3) 
		return true;
	return false;
}

// ��ʾ���˳�ģʽ�����г�
bool CTGDoc::FindTrainByTrainIndexForSingleTrain(long& position, DWORD& train_index, const CString& trainId)
{
	int nOffset = -1;

	map<TRAIN_KEY, CTG_TRAIN>::iterator it;
	
	if(train_index > 0)
	{
		it =  this->myPDataManage->myTrainTmp.find(train_index);
		if(it == this->myPDataManage->myTrainTmp.end())
			return false;
		
		TgTRAIN_RECORD	*first = (*it).second.GetFirstRecord();
		if(first == NULL)
			return false;

		train_index = (*it).first;

		if(IsFlagStart(first->flag))
			nOffset = GetOffsetMinuteFromGraphBegin(first->depart);
		else
			nOffset = GetOffsetMinuteFromGraphBegin(first->arrive);

		position = nOffset-FindTrainMoveDlt;

		myPDataManage->AppendSingleTrain(train_index, (*it).second);
		return true;
	}
	else
		return false;
}

int CTGDoc::FindTrainByTrainIdForSingleTrain(const CString& trainId, long postion[], long indexlist[])
{
	int nOffset = -1, count=0;
	map<TRAIN_KEY, CTG_TRAIN>::iterator it;

	it =  this->myPDataManage->myTrainTmp.begin();
	for(; it != this->myPDataManage->myTrainTmp.end(); it++)
	{
		for(int idx=0; idx<(*it).second.GetRecordNum(); idx++)
		{
			if((*it).second.myRecord[idx].arrive_train_id == trainId ||
				(*it).second.myRecord[idx].depart_train_id == trainId ||
				(*it).second.myRecord[idx].original_arrive_train_id == trainId ||
				(*it).second.myRecord[idx].original_depart_train_id == trainId)
			{
				TgTRAIN_RECORD	*first = ((*it).second).GetFirstRecord();
				if(first != NULL)
				{
					if(IsFlagStart(first->flag))
						nOffset = GetOffsetMinuteFromGraphBegin(first->depart);
					else
						nOffset = GetOffsetMinuteFromGraphBegin(first->arrive);

					postion[count] = nOffset-FindTrainMoveDlt;
					indexlist[count] = (*it).first;
					count++;
				
					myPDataManage->AppendSingleTrain((*it).first, (*it).second);
				}
				break;
			}
		}
	}
	
	return count;
}

void CTGDoc::TrytoFindAbsEngineReturnTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list)
{
	train_list.clear();

	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for(; it!=myPDataManage->myTrain.end(); it++)
	{
		TgTRAIN_RECORD* prec=it->second.GetFirstRecord();
		if(IsAbsAttainEngineReturn(it->second.myTrainInfo.adjust_flag) && (prec!=NULL))
		{
			if(prec->special_train_index == ptrain->m_nTrainIndex && prec->special_train_index!=0)
			{
				train_list.push_back(it->second);
			}
		}
	}
}
	
void CTGDoc::TrytoFindAbsPartRunTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list)
{
	train_list.clear();

	map<TRAIN_KEY, CTG_TRAIN>::iterator it = myPDataManage->myTrain.begin();
	for(; it!=myPDataManage->myTrain.end(); it++)
	{
		TgTRAIN_RECORD* prec=it->second.GetFirstRecord();
		if(IsAbsPartRunTrain(it->second.myTrainInfo.adjust_flag) && (prec!=NULL))
		{
			if(prec->special_train_index == ptrain->m_nTrainIndex && prec->special_train_index!=0)
			{
				train_list.push_back(it->second);
			}
		}
	}
}

bool CTGDoc::TrytoMoveOtherTrain(CTG_TRAIN* ptrain, CTG_TRAIN& othertrain)
{
	bool bMoveTrain=false;
	TgTRAIN_RECORD* pFirst=othertrain.GetFirstRecord();
	if(ptrain==NULL || pFirst==NULL)
	{
		return false;
	}

	// 1. ����;���۷�����
	if(IsAbsAttainEngineReturn(othertrain.myTrainInfo.adjust_flag))
	{
		if(pFirst->special_train_index==ptrain->m_nTrainIndex)
		{
			int nOldTime = pFirst->arrive - othertrain.myTrainInfo.stop_abs_time*60;
			for(int n=0; n<ptrain->GetRecordNum(); n++)
			{
				TgTRAIN_RECORD* prec=ptrain->GetRecordByRecIndex(n);
				TgTRAIN_RECORD* prec_next=ptrain->GetRecordByRecIndex(n+1);
				if((prec!=NULL) && (prec_next!=NULL))
				{
					if(prec->station == pFirst->station && IsFlagAttainEngine(prec->flag))
					{
						int mid_time=prec_next->arrive/2+prec->depart/2;
						int offset=mid_time-nOldTime;
						offset = offset-offset%60;
						if(offset!=0)
						{
							othertrain.OffsetWorkScheduleTime(offset);	
							bMoveTrain=true;
						}
					}
				}
			}
		}
	}

	// 2. �����ڷֲ�����
	else if(IsAbsPartRunTrain(othertrain.myTrainInfo.adjust_flag))
	{
		if(pFirst->special_train_index==ptrain->m_nTrainIndex)
		{
			int nOldTime = pFirst->depart + pFirst->stop_abs_s_time*60 - othertrain.myTrainInfo.stop_abs_time*60;
			for(int n=0; n<ptrain->GetRecordNum(); n++)
			{
				TgTRAIN_RECORD* prec=ptrain->GetRecordByRecIndex(n);
				TgTRAIN_RECORD* prec_next=ptrain->GetRecordByRecIndex(n+1);
				if(prec!=NULL && prec_next!=NULL)
				{
					if(prec_next->station == pFirst->station && IsFlagTrainPartRun(prec_next->flag))
					{
						long mid_time = prec->depart/2 + prec_next->arrive/2;
						if(IsFlagStopAbs(prec->flag))
							mid_time = prec->depart + prec->stop_abs_e_time*60;
						
						int offset=mid_time-nOldTime;
						offset = offset-offset%60;
						if(offset!=0)
						{
							othertrain.OffsetWorkScheduleTime(offset);
							bMoveTrain=true;
						}
					}
				}
			}
		}
	}

	return bMoveTrain;
}

void CTGDoc::CheckSideRelation(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary)
{
	ary.RemoveAll();
	if(!this->m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
		return;

	TgTRAIN_RECORD *precord=ptrain->GetRecordByRecIndex(rec_index);
	if(!precord)
		return;
		
	int  station=precord->station;
	if(GetStationProperty(station)!=CTC_STATION)
		return;
	
	BYTE arrive_side=precord->arrive_side;
	BYTE depart_side=precord->depart_side;
	DWORD arrive = precord->arrive;
	DWORD depart = precord->depart;
	
	CString strStaName = GetStationName(station);
	CString strTrainId=this->GetTrainShowId(ptrain);
	
	CStationDef *pStationDef=GetStationSideDef(station);
	if(!pStationDef)
	{
		return;
	}

	bool bStart=false, bTerminal=false;
	if(IsFlagStart(precord->flag) && (0==rec_index)) // ʼ����,��鷢���ɵ�
	{
		bStart=true;
	}
	if(IsFlagTerminal(precord->flag) && (ptrain->GetRecordNum()==rec_index+1)) // �յ���,���ӳ��ɵ�
	{
		bTerminal=true;
	}
	TRAIN_INDEX train_index=ptrain->GetTrainIndex();
	CString arrive_side_name=this->GetSideName(station, arrive_side);
	CString depart_side_name=this->GetSideName(station, depart_side);

	bool bArriveSideOK=true, bDepartSideOK=true;
	if(arrive_side_name=="?G")
	{
		bArriveSideOK=false;
	}
	
	if(depart_side_name=="?G")
	{
		bDepartSideOK=false;
	}
	
	if(bStart)
	{
		if(bDepartSideOK)
		{
			CString str=myPDataManage->CheckTrainSide(train_index, station, depart_side, arrive, depart);
			if(str!="")
			{
				ary.Add(str);
			}
		}
	}
	else
	{
		if(bTerminal)
		{
			if(bArriveSideOK)
			{
				CString str=myPDataManage->CheckTrainSide(train_index, station, arrive_side, arrive, depart);
				if(str!="")
				{
					ary.Add(str);
				}
			}
		}
		else
		{
			if(bArriveSideOK && bDepartSideOK)
			{
				if(arrive_side != depart_side)
				{
					CString str=myPDataManage->CheckTrainSide(train_index, station, arrive_side, arrive, depart);
					if(str!="")
					{
						ary.Add(str);
					}
					str=myPDataManage->CheckTrainSide(train_index, station, depart_side, arrive, depart);
					if(str!="")
					{
						ary.Add(str);
					}
				}
				else
				{
					CString str=myPDataManage->CheckTrainSide(train_index, station, arrive_side, arrive, depart);
					if(str!="")
					{
						ary.Add(str);
					}
				}
			}
			else if(bArriveSideOK)
			{
				CString str=myPDataManage->CheckTrainSide(train_index, station, arrive_side, arrive, depart);
				if(str!="")
				{
					ary.Add(str);
				}
			}
			else if(bDepartSideOK)
			{
				CString str=myPDataManage->CheckTrainSide(train_index, station, depart_side, arrive, depart);
				if(str!="")
				{
					ary.Add(str);
				}
			}
		}
	}	
}

void CTGDoc::ReLayoutBmpPageSize(int nSelDistanceNo)
{
	const SectionDisplay* disp = this->m_ptgctrl->mConfigTg.m_tgdef.GetSectionDisplay(m_nCurrentSectionNo);
	if(disp == NULL)
	{
		CString msg;
		msg.Format("�����ļ� TG.def ����%d����ʾ�����Ҳ���",m_nCurrentSectionNo);
		MessageBox(NULL, msg, "����", MB_ICONHAND|MB_OK);
		return;
	}
	
	DispDistance* pDispDistance = GetDispDistanceByIdx(nSelDistanceNo);
	if(pDispDistance->show_sation_sep)
	{
		pDispDistance->show_sation_sep = FALSE;
	}
	else
	{
		pDispDistance->show_sation_sep = TRUE;  
		if(pDispDistance->show_sides_mode == 0)
			pDispDistance->show_sides_mode = 0;    
		else
			pDispDistance->show_sides_mode = 0;   
	}

	int nTotalHightOfSides = 0;
	for (int idx = 0; idx < m_nDispDistanceNum; idx++)
	{
		// �ɵ�չ��������
		if(m_sDispDistance[idx].show_sation_sep != 0)
		{
			nTotalHightOfSides += 
				m_sDispDistance[idx].number_of_side * disp->nSideHeight;
		}
	}

	m_nPageHeight = m_ptgctrl->mConfigTg.m_tgdef.GetSectionHeight(m_nCurrentSectionNo)+nTotalHightOfSides;
	
	this->UpdateAllViews(NULL, MSG_RE_LAYOUT_BMP_SIZE, 0);
}

void CTGDoc::SetTrainRelationSideModify(CTG_TRAIN* ptrain)
{
	if(ptrain==NULL)
		return;
	TRAIN_INDEX this_train_index = ptrain->m_nTrainIndex;
	CTG_TRAIN* poldtrain = GetTrainByIndex(this_train_index);
	if(poldtrain==NULL)
		return;
	TRAIN_INDEX prev_train_index=0, next_train_index=0;
	myPDataManage->GetPrevAndAfterTrainIndex(this_train_index, prev_train_index, next_train_index);
	
	TgTRAIN_RECORD* first=ptrain->GetFirstRecord();
	TgTRAIN_RECORD* oldfirst=poldtrain->GetFirstRecord();
	if(first != NULL && oldfirst != NULL && prev_train_index!=0)
	{
		if((oldfirst->depart_side!=first->depart_side) && (oldfirst->station==first->station))
		{
			if(IsDispalySide2(first->stop_condition))
			{
				;// �Ѿ����ó���ʾ
			}
			else 
			{
				SetFlagDispalySide2(first->stop_condition);
			}
		}
	}
	TgTRAIN_RECORD* last=ptrain->GetLastRecord();
	TgTRAIN_RECORD* oldlast=poldtrain->GetLastRecord();
	if(last !=NULL && oldlast != NULL && next_train_index!=0)
	{
		if((oldlast->arrive_side != last->arrive_side) && (oldlast->station==last->station))
		{
			if(IsDispalySide2(last->stop_condition))
			{
				;// �Ѿ����ó���ʾ
			}
			else
			{
				SetFlagDispalySide2(last->stop_condition);
			}
		}
	}
}

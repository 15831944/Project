// Plot24View.cpp: implementation of the CPlot24View class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <Afxinet.h>
#include <direct.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include "Plot24View.h"
#include "JpegFile.h"
#include "bmpFile.h"
#include "bitmapfile.h"
#include "QueryPhaseStoreTrain.h"
#include "bureau.h"
#include ".\plot24view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "basic_schd_config.h"
extern basic_schd_config* GetBasicConfig();

extern CView *clsPlotView;

#define CLR_BLOCK RGB(0,0,0)

/////////////////////////////////////////////////////////////////////////////
// CPlot24View

IMPLEMENT_DYNCREATE(CPlot24View, CPlotView)

CPlot24View::CPlot24View()
{
	m_picBuf = NULL;
	m_picWidth = 0;
	m_picHeight = 0;	
}

CPlot24View::~CPlot24View()
{
	if(m_picBuf != NULL)
	{
		delete [] m_picBuf;
		m_picBuf = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPlot24View, CPlotView)
	//{{AFX_MSG_MAP(CPlot24View)
	ON_COMMAND(ID_SAVEPICTURE, OnSavepicture)
	ON_COMMAND(ID_PLOT_OUTPUT, OnPlotOutput)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlot24View drawing
LRESULT CPlot24View::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;

	// �յ����������صĵ���Ա����
	if(MSGID_MsgGiveDDyName==pMsgStruct->msg_id)
	{
		CLBTH::MsgGiveDDyName  *pMsg = (CLBTH::MsgGiveDDyName  *)pBase;
		sysprint(5,PMTG.ThreadId,"[PMTG]: ���ܵ���Ա���� shiftid %d %s %s��", pMsg->shift_id, pMsg->user_name1, pMsg->user_name2);

		CPlot24Doc *pDoc = (CPlot24Doc*)GetDocument();
		if(NULL != pDoc)
		{
			if(pDoc->m_ddyName.nConId == pMsg->conid && pDoc->m_ddyName.nShiftId == pMsg->shift_id)
			{
				pDoc->m_ddyName.name1 = pMsg->user_name1;
				pDoc->m_ddyName.name2 = pMsg->user_name2;
				sysprint(5,PMTG.ThreadId,"[PMTG]: ���ܵ���Ա���� shiftid %d %s %s��", pDoc->m_ddyName.nShiftId, pDoc->m_ddyName.name1, pDoc->m_ddyName.name2);
				pDoc->DrawDDYName24(&m_dcMemory,CLR_PLOT);
			}
			else if(pDoc->m_ddyName_Pre.nConId == pMsg->conid && pDoc->m_ddyName_Pre.nShiftId == pMsg->shift_id)
			{
				pDoc->m_ddyName_Pre.name1 = pMsg->user_name1;
				pDoc->m_ddyName_Pre.name2 = pMsg->user_name2;
				sysprint(5,PMTG.ThreadId,"[PMTG]: ���ܵ���Ա���� shiftid %d %s %s��", pDoc->m_ddyName_Pre.nShiftId, pDoc->m_ddyName_Pre.name1, pDoc->m_ddyName_Pre.name2);
				pDoc->DrawDDYName24(&m_dcMemory,CLR_PLOT);
			}
		}
	}
	//����
	else if(pMsgStruct->msg_id==MSGID_MsgNotePadFormat)
	{
		CLBTH::MsgNotePadFormat  *pMsg = (CLBTH::MsgNotePadFormat  *)pBase;
		OnNotepadSheetMsg(pMsg);
	}
	//�׶μ���
	else if(pMsgStruct->msg_id==MSGID_MsgWordPhasePlanSend)
	{
		CLBTH::MsgWordPhasePlanSend  *pMsg = (CLBTH::MsgWordPhasePlanSend  *)pBase;
		OnRealPhaseMsg(pMsg);
	}
	//�ٱ���Ϣ
	else if(pMsgStruct->msg_id==MSGID_MsgSubaoFormat)
	{
		CLBTH::MsgSubaoFormat  *pMsg = (CLBTH::MsgSubaoFormat  *)pBase;
		OnSubaoData(pMsg);
	}

	//վ�泵������Ϣ
	else if(pMsgStruct->msg_id==MSGID_MsgSimpleStationCarCtrl)
	{
		CLBTH::MsgSimpleStationCarCtrl* pMsg = (CLBTH::MsgSimpleStationCarCtrl  *)pBase;
		OnStationCurrentStoredTrainFlag(pMsg);
	}
	//վ�泵������Ϣ
	else if(pMsgStruct->msg_id==MSGID_MsgSimpleStationCarFormat)
	{
		CLBTH::MsgSimpleStationCarFormat* pMsg = (CLBTH::MsgSimpleStationCarFormat  *)pBase;
		OnStationCurrentStoredTrain(pMsg);
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CPlot24View::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgGiveDDyName);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgNotePadFormat);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanSend);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSimpleStationCarCtrl);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSimpleStationCarFormat);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSubaoFormat);

	CPlotView::OnClose();
}
void CPlot24View::OnInitialUpdate()
{
	CPlotView::OnInitialUpdate();

	// TODO: calculate the total size of this view
	CSize mSize;
	CRect rectClient;
	CPlotDoc* p = GetDocument( );
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,p);

	while (pDoc->m_EnvironInitFlag != 3)
		Sleep(10);
	//��ӡҳ��Ŀ�ȣ��߶�
	m_nPageWidth = pDoc->m_nPageWidth;
	m_nPageHeight = pDoc->m_nPageHeight;    

	mSize.cx = m_nPageWidth;
	mSize.cy = m_nPageHeight;
	SetScrollSizes(MM_TEXT,mSize);

	GetClientRect(rectClient);


	CDC *pDC;
	pDC = GetDC( );
	do 
	{
		m_rectViewPort.SetRect(0,0,mSize.cx,mSize.cy);

		if(pDC == NULL)
			return;

		if(m_MemBitmap.CreateCompatibleBitmap(pDC,m_nPageWidth,m_nPageHeight) == 0)
		{
			DWORD ret = GetLastError();
			sysprint(0,PMTG.ThreadId,"Create bitmap error error code = %d",ret);

			SendMessage(WM_CLOSE);
			AfxMessageBox("���ڴ򿪹��࣬���ȹرղ�ʹ�õĴ��ڻ��˳���������µ�½!");			
			return;
		}

		if(m_dcMemory.CreateCompatibleDC(pDC) == 0)
		{
			sysprint(0,PMTG.ThreadId,"Create compatible dc error");
			return;
		}
		m_pOldBitmap = m_dcMemory.SelectObject(&m_MemBitmap);

		m_dcMemory.FillRect(CRect(0,0,m_nPageWidth,m_nPageHeight), &m_brushBackground);
		m_dcMemory.SetBkColor(CLR_WHITE);                    //WHITE

		//��վ��
		pDoc->DrawStationAixis(&m_dcMemory, CLR_PLOT);
		//ʱ����
		pDoc->DrawTimeAixis(&m_dcMemory,CLR_PLOT); //CLR_DARKGREEN
		//���µ�ʱ����
		pDoc->ShowTimeString(&m_dcMemory,CLR_PLOT);
		//���ҵĳ�վ����
		pDoc->ShowStationString(&m_dcMemory,CLR_PLOT,TRUE);	
		//���⳵վ
		pDoc->ShowVirtualStationString(&m_dcMemory,CLR_PLOT);
		//����
		pDoc->DrawHeaderTitle(&m_dcMemory,CLR_PLOT);

		if(pDoc->m_nSchdStatus != BASIC_SCHEDULE)
		{
			//��ע
			pDoc->DrawFooterTitle(&m_dcMemory,CLR_PLOT);			

			//������Ϣ
			pDoc->DrawTrainInform24(&m_dcMemory,CLR_PLOT);  //�г�����
			
			//����Ա����
			pDoc->DrawDDYName24(&m_dcMemory,CLR_PLOT);			

			//����վ�泵���
			if(pDoc->IsPrintPhaseCar(pDoc->m_sPrintInform->usSectionNo))  //��ר̨�����վ�泵
				pDoc->DrawPhaseStoreTrainFrame24(&m_dcMemory,CLR_PLOT);

			//���Ƶ�ͼ�Σ������䣬�ɵ�ͣ����(��ʱ����)
			pDoc->DrawTrainMark(&m_dcMemory);

			//����
			pDoc->DrawJiShi(&m_dcMemory,CLR_PLOT);

			//�׶μƻ����
			if(pDoc->mPlotStyle.id == 1)
			{
				pDoc->DrawPhasePlanFrameXian(&m_dcMemory,CLR_PLOT,NULL);
				pDoc->FillPhaseContentXian(&m_dcMemory,CLR_PLOT);
			}
			else
			{
				pDoc->DrawPhasePlanFrame(&m_dcMemory,CLR_PLOT,NULL);
				pDoc->FillPhaseContent(&m_dcMemory,CLR_PLOT);
			}
		}		

		//����		
		pDoc->DrawSchdTrain(&m_dcMemory);
		pDoc->PlotReDraw(this);
		clsPlotView=this;

	}while(0);

	ReleaseDC(pDC);

	if(pDoc->m_nSchdStatus != BASIC_SCHEDULE)
	{
		//����Ա����
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGiveDDyName,0);
		Query_GetDdyName();
	
		//����
		RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgNotePadFormat,0);
		Query_Rem();
		

		//�׶μ���,�����һЩ��������������ܼ�ʱ����
		RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanSend,0);
		Query_Phase();
		

		//�ٱ���Ϣ�����࣬����֮���������������������ܼ�ʱ����
		pDoc->m_VectTrainsSubao.clear();
		RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSubaoFormat,0);
		Query_Subao();

		//վ�泵��Ϣ
		RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSimpleStationCarCtrl,0);
		RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgSimpleStationCarFormat,0);
		Query_StationCar();
	}
}

void CPlot24View::OnDraw(CDC* pDC)
{
	// TODO: add draw code here
	pDC->BitBlt(0,0,m_nPageWidth,m_nPageHeight, &m_dcMemory, 0, 0, SRCCOPY);
	CPlotView::OnDraw(pDC); 
}

CPlotDoc *CPlot24View::GetDocument( )
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlot24Doc)));
	return (CPlot24Doc*)m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CPlot24View diagnostics

#ifdef _DEBUG
void CPlot24View::AssertValid() const
{
	CPlotView::AssertValid();
}

void CPlot24View::Dump(CDumpContext& dc) const
{
	CPlotView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlot24View message handlers
void CPlot24View::Query_GetDdyName()
{
	Archive ar;
	MsgGetDDyName GetDdyName;

	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));
	// �������Ա����
	CWorkScheduleHolder *pholder=pDoc->m_pTgDoc->GetTGDataHolder();

	CTG_TrainScheduleBase *pdata;
	if(pDoc->m_nSchdStatus == WORK_SCHEDULE)
		pdata = pholder->GetWorkScheduleData();
	else if(pDoc->m_nSchdStatus == SHIFT_SCHEDULE)
		pdata = pholder->GetShiftScheduleData();
	else
		pdata = pholder->GetWorkScheduleData();

	CTG_ScheduleInfo schd;

	pdata->GetScheduleInfo(schd); 
	GetDdyName.conid = schd.m_nSectionID;
	GetDdyName.shift_id = schd.m_nShiftID;

	ar << GetDdyName;

	int error=SendTGCommMessage(pDoc->m_nCurrentSectionNo, ar.getCurrentPos(), (char*)ar.getBuffer());	
	if(error == RET_SUCCESS) 
		sysprint(5,PMTG.ThreadId,"[PMTG]: ����%d̨ shiftid %d ����Ա���� ��",  GetDdyName.conid, GetDdyName.shift_id/*pDoc->m_shiftidFirst*/);
	else
		sysprint(5,PMTG.ThreadId,"[PMTG]: ����%d̨ shiftid %d ����Ա����ʧ�� shiftid %d��", GetDdyName.conid, GetDdyName.shift_id/*pDoc->m_shiftidFirst*/);

	pDoc->m_ddyName.nConId = GetDdyName.conid;
	pDoc->m_ddyName.nShiftId = GetDdyName.shift_id;
	pDoc->m_ddyName.name1 = "";
	pDoc->m_ddyName.name2 = "";

	// ����ǰһ�����Ա����
	Archive ar1;
	MsgGetDDyName GetDdyNamePre;

	GetDdyNamePre.conid = schd.m_nSectionID;
	GetDdyNamePre.shift_id = CTG_ScheduleInfo::Get_PreShiftid(schd.m_nShiftID); 

	ar1 << GetDdyNamePre;

	error=SendTGCommMessage(pDoc->m_nCurrentSectionNo, ar1.getCurrentPos(), (char*)ar1.getBuffer());	
	if(error == RET_SUCCESS) 
		sysprint(5,PMTG.ThreadId,"[PMTG]: ����%d̨ shiftid %d ����Ա���� ��",  GetDdyNamePre.conid, GetDdyNamePre.shift_id);
	else
		sysprint(5,PMTG.ThreadId,"[PMTG]: ����%d̨ shiftid %d ����Ա����ʧ�� shiftid %d��", GetDdyNamePre.conid, GetDdyNamePre.shift_id);

	pDoc->m_ddyName_Pre.nConId = GetDdyNamePre.conid;
	pDoc->m_ddyName_Pre.nShiftId = GetDdyNamePre.shift_id;
	pDoc->m_ddyName_Pre.name1 = "";
	pDoc->m_ddyName_Pre.name2 = "";
	
}

void CPlot24View::Query_Subao()
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));
	CLBTH::MsgSubaoQuery mMsgSubaoQuery;

	unsigned long shift_id;

	if(pDoc->m_printFormat == 1)//��ӡʮ��Сʱ��ֻ��ѯ��ǰ��
	{
		shift_id  = pDoc->m_sPrintInform->usShiftid;
	}
	else
	{
		shift_id =  pDoc->m_shiftidFirst;
	}

	mMsgSubaoQuery.shift_id = shift_id;
	mMsgSubaoQuery.con_id =  pDoc->m_sPrintInform->usSectionNo; 
	mMsgSubaoQuery.schd_type = CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;//pDoc->m_sPrintInform->usSchdlType;
	mMsgSubaoQuery.train_index = -1;   // train_index ,-1 for all train
	mMsgSubaoQuery.query_filter = CLBTH::MsgSubaoQuery::SUBAO_QUERY_BY_SHIFTID;

	long nEntity=GetSpecialEntityID(SERVER_TYPE_TG,pDoc->m_nCurrentSectionNo);
	sysprint(5, 0, "[��ӡ] ��entity %d ���� con_id %d shift_id %d schd_type %d  query_filter %d �ٱ�", nEntity, mMsgSubaoQuery.con_id, mMsgSubaoQuery.shift_id, 
		mMsgSubaoQuery.schd_type, mMsgSubaoQuery.query_filter);

	SendBMsg(&mMsgSubaoQuery, nEntity);
}

void CPlot24View::OnSubaoData(CLBTH::MsgSubaoFormat*  pMsg)
{
	if(NULL == pMsg)
		return;

	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc, GetDocument());
	CLBTH::MsgSubaoFormat *pBuf = pMsg;
	if(pBuf->oper != CLBTH::MsgSubaoFormat::SUBAO_SERVER_RELAY)
		return;

	TRAIN_SUBAO_NEW subao;

	memset(&subao, 0x00, sizeof(TRAIN_SUBAO_NEW));

	subao.shift_id	=  pBuf->shift_id;
	subao.train_idx = pBuf->train_index;
	subao.input_time = pBuf->input_time;
	memset(subao.Train_id,0,sizeof(subao.Train_id));
	strncpy(subao.Train_id, pBuf->train_id, sizeof(pBuf->train_id));	 
	memset(subao.Train_describe,0,sizeof(subao.Train_describe));
	strncpy(subao.Train_describe, pBuf->car_des, sizeof(pBuf->car_des));   //describe  	
	memset(subao.Orig,0,sizeof(subao.Orig));
	strncpy(subao.Orig, pBuf->orig, sizeof(pBuf->orig));   //start station    
	memset(subao.Dest,0,sizeof(subao.Dest));
	strncpy(subao.Dest, pBuf->dest, sizeof(pBuf->dest));   //end station

	subao.engine_count = ((pBuf->engine_count)&0xF0>>4) + (pBuf->engine_count)&0x0F ;
	for(int j=0;j<subao.engine_count; j++)
	{
		strcpy(subao.Engine_id[j],pBuf->subao_engine[j].engine_id);     //engine id  
	}

	memset(subao.Driver_name,0,sizeof(subao.Driver_name));
	strncpy(subao.Driver_name,pBuf->driver_name,sizeof(pBuf->driver_name));  //drive name
	memset(subao.Train_leader_name,0,sizeof(subao.Train_leader_name));
	strncpy(subao.Train_leader_name,pBuf->train_leader_name,sizeof(pBuf->train_leader_name)); 
	memset(subao.Cover,0,sizeof(subao.Cover));
	strncpy(subao.Cover,pBuf->cover,sizeof(pBuf->cover));  //cover
	memset(subao.Tail_no,0,sizeof(subao.Tail_no));
	strncpy(subao.Tail_no,pBuf->tail_no,sizeof(pBuf->tail_no));  //tail_no

	subao.Space_rank= pBuf->space_rank;  //space rank   ******
	subao.Length_rank= pBuf->length_rank;  //length rank ****** 
	subao.Super_rank= pBuf->super_rank;  //super rank  ******
	subao.Military_rank= pBuf->military_rank;  //military rank ******
	subao.Speed_limit= pBuf->speed_limit;  //speed limit ******
	subao.Con_id= pBuf->con_id;    //con id  ******
	subao.E_engine= pBuf->e_engine;  //e_engine ******
	subao.Print_flag= pBuf->print_flag;  //print flag  ******

	subao.K_count=pBuf->pass_cnt;    //k count


	subao.Load_count= pBuf->load_cnt;  //load count  	
	subao.PK_count=  pBuf->pk_count;  //pk count 	
	subao.PZ_count= pBuf->pz_count;    //pz count  	
	subao.CK_count= pBuf->ck_count;  //ck count  	
	subao.CZ_count=  pBuf->cz_count;   // cz count   	
	subao.PIK_count=  pBuf->pik_count;  // pik count   	
	subao.PIZ_count=  pBuf->piz_count;   // piz count  	
	subao.GK_count=   pBuf->gk_count; // gk count 	
	subao.GZ_count=  pBuf->gz_count;   //gz count  	
	subao.O_count=   pBuf->o_count; // o count  	
	subao.S_count=    pBuf->s_count;  // s count	
	subao.Total_count= pBuf->total_count;    //total count  	
	subao.Van_count= pBuf->van_count;  //Van count  ******

	ushort cal_len= pBuf->cal_len; //cal len
	subao.Cal_len=cal_len;

	ushort total_weight= pBuf->total_weight;   //total weight 
	subao.Total_weight=total_weight;	 

	subao.Subao_type= pBuf->subao_type;  //subao type  	 
	subao.Speed=  pBuf->speed ;//speed     ******	 
	subao.Noutil_cnt= pBuf->noutil_cnt; //tail  ****** 
	subao.report_station = pBuf->report_sta;

	BYTE nTractEngineCount =   (pBuf->engine_count)&0x0F ;
	BYTE nAttachEngineCount =  ((pBuf->engine_count)&0xF0 )>>4 ;

	subao.tract_engine_count = 0;
	subao.attach_engine_count = 0;

	int m,n;
	for(m=0;m<nTractEngineCount && m < 8; m++)
	{
		subao.tract_engine_type[m] = pBuf->subao_engine[j].engine_type;  
		strncpy((char*)subao.tract_engine_id[m], pBuf->subao_engine[m].engine_id, 19);  //ǣ��������
		subao.tract_engine_count++;
	}
	for(n=0;n<nAttachEngineCount && n < 8; n++)
	{
		subao.attach_engine_type[n] = pBuf->subao_engine[m+n].engine_type ;  //���һ�������			
		strncpy((char*)subao.attach_engine_id[n], pBuf->subao_engine[m+n].engine_id, 19);  //���һ�����
		subao.attach_engine_count++;
	}

	pDoc->m_VectTrainsSubao.push_back(subao);	
	pDoc->DrawTrainInform24(&m_dcMemory,CLR_PLOT);  //�����г��������ͼ
	Invalidate();

	return;
}

// ����׶μ�������
void CPlot24View::OnRealPhaseMsg(CLBTH::MsgWordPhasePlanSend *pBuf)
{
	if(NULL == pBuf)
		return;

	CPlot24Doc *pDoc = (CPlot24Doc*)GetDocument();
	if(pDoc->m_sPrintInform->usSchdlType == BASIC_SCHEDULE)
		return;


	int phase_id = pBuf->phase_id;
	int position = 0;
	CString strTemp,strContent,strCommander,strCommandTime,strSendTime;
	strTemp = pBuf->phase_content;
	do
	{
		if(strTemp.IsEmpty())
		{
			strContent = strSendTime = strCommandTime = strCommander = " ";
			break;
		}
		position = strTemp.Find('&');
		if(position <0)
			break;
		strCommander = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);
		position = strTemp.Find('&');

		if(position < 0)
			break;
		strCommandTime = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);
		position = strTemp.Find('&');

		if(position < 0)
			break;
		strSendTime = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);

	}while(0);
	strContent = strTemp;
	
	GpcLogTrainMessage(0,0,"����,�׶μ�������:phase_id=%d start_sta_weather=%d end_sta_weather=%d",phase_id,pBuf->start_sta_weather,pBuf->end_sta_weather);
	switch(phase_id)
	{
	case 0:
		pDoc->NOTES.start_wea1 =pBuf->start_sta_weather;  		
		pDoc->NOTES.end_wea1 = pBuf->end_sta_weather; 	 

		strcpy(pDoc->NOTES.note1,(char*)(LPCTSTR)strContent);
		pDoc->NOTES.note_len1 = strContent.GetLength(); 
		strCommander = strCommander +'/'+ strCommandTime;
		strcpy(pDoc->NOTES.command_info1,(char*)(LPCTSTR)strCommander);
		strcpy(pDoc->NOTES.send_time1,(char*)(LPCTSTR)strSendTime);
		break;
	case 1:
		pDoc->NOTES.start_wea2 =pBuf->start_sta_weather; 		
		pDoc->NOTES.end_wea2 = pBuf->end_sta_weather; 	 

		strcpy(pDoc->NOTES.note2,(char*)(LPCTSTR)strContent);
		pDoc->NOTES.note_len2 = strContent.GetLength(); 
		strCommander = strCommander +'/'+ strCommandTime;
		strcpy(pDoc->NOTES.command_info2,(char*)(LPCTSTR)strCommander);
		strcpy(pDoc->NOTES.send_time2,(char*)(LPCTSTR)strSendTime);
		break;
	case 2:
		pDoc->NOTES.start_wea3 =pBuf->start_sta_weather;		
		pDoc->NOTES.end_wea3 = pBuf->end_sta_weather; 	 

		strcpy(pDoc->NOTES.note3,(char*)(LPCTSTR)strContent);
		pDoc->NOTES.note_len3 = strContent.GetLength(); 
		strCommander = strCommander +'/'+ strCommandTime;
		strcpy(pDoc->NOTES.command_info3,(char*)(LPCTSTR)strCommander);
		strcpy(pDoc->NOTES.send_time3,(char*)(LPCTSTR)strSendTime);
		break;
	}

	if(pDoc->mPlotStyle.id == 1)
	{
		pDoc->DrawPhasePlanFrameXian(&m_dcMemory,CLR_PLOT,NULL);
		pDoc->FillPhaseContentXian(&m_dcMemory,CLR_PLOT);
	}
	else
	{
		pDoc->DrawPhasePlanFrame(&m_dcMemory,CLR_PLOT,NULL);
		pDoc->FillPhaseContent(&m_dcMemory,CLR_PLOT);
	}
	

	return;
}

void CPlot24View::OnNotepadSheetMsg(CLBTH::MsgNotePadFormat *pBuf)
{
	if(NULL == pBuf)
		return;

	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument());
	pDoc->NOTES.ddt_id = pBuf->con_id;
	pDoc->NOTES.shift_id = pBuf->shift_id;
	pBuf->schd_type;

	pDoc->NOTES.note_len = pBuf->data_len;
	strncpy(pDoc->NOTES.note, pBuf->data, pBuf->data_len); 
	pDoc->NOTES.note[pBuf->data_len] = 0;

	pDoc->parseNotes();

	if(pDoc->m_sPrintInform->usSchdlType != BASIC_SCHEDULE)
	{
		pDoc->DrawJiShi(&m_dcMemory,CLR_PLOT);
	}
	return;
}
//���ݿ�ʼ��������־
void CPlot24View::OnStationCurrentStoredTrainFlag(CLBTH::MsgSimpleStationCarCtrl*  pMsg)
{
	CLBTH::MsgSimpleStationCarCtrl *pBuf = pMsg;

	if(NULL == pBuf)
		return;
		
	if(CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT == pBuf->phase)
		return;

	if(pBuf->status == CLBTH::MsgSimpleStationCarCtrl::MSG_BEGIN)
	{
		nReceiveStatus = 0;
		GpcLogTrainMessage(0,0, "��ʼ �׶�%d,վ�泵",pBuf->phase);
	}
	else if(pBuf->status == CLBTH::MsgSimpleStationCarCtrl::MSG_END)
	{
		nReceiveStatus = 1;
		GpcLogTrainMessage(0,0, "���� �׶�%d,վ�泵",pBuf->phase);
	}
	
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));
	switch(pBuf->phase)
	{
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0:
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6:
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12:
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18:
		{
			if(nReceiveStatus == 1 && pDoc)
				pDoc->ShowPhaseStoreTrain24(&m_dcMemory, CLR_BLACK, pBuf->station, pBuf->phase);
		}
		break;
	}
	
	return;
}
void CPlot24View::OnStationCurrentStoredTrain(CLBTH::MsgSimpleStationCarFormat *pBuf)
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));
	if(NULL == pBuf)
		return;

	if(!pDoc->IsPrintPhaseCar(pDoc->m_sPrintInform->usSectionNo))
		return;

	if(pBuf->car_count ==0)
		return;
	//1 phase
	int i = pBuf->phase;
	/*
	0   STACAR_PHASE_0,
	1   STACAR_PHASE_6,
	2   STACAR_PHASE_12,
	3   STACAR_PHASE_18,
	4   STACAR_PHASE_CURRENT
	*/

	if(pBuf->phase> 3)  // 0,1,2,3 
		return;

	CTGBasicParam* pBasicParam = GetTGBasicParam();
	if(NULL == pBasicParam)
		return;

	//2 station
	ushort usStationNo = pBuf->station;
	int id = GetStationidInList(usStationNo);	
	if(id == -1) return;

	Info[id][i].usStationNo = usStationNo;     
	//3 input time
	Info[id][i].EditTime = pBuf->input_time;
	//find if this side data exists 
	int nValue = IsSideInfoExist(usStationNo,i,pBuf->side_name);
	int nSideIndex;
	//�����ڸùɵ�����Ϣ
	if(nValue < 0)
	{
		nSideIndex = Info[id][i].usSideNum;
		Info[id][i].usSideNum++;
	}
	else
		nSideIndex = nValue;

	//4 �������
	pBuf->index;
	_tagSideStoredTrain *sideinfo;
	sideinfo = &Info[id][i].sideinfo[nSideIndex];
	//5 side number
	sideinfo->usSideNo = pBuf->side;
	//6 side name
	memset(sideinfo->pszSideName,0,sizeof(sideinfo->pszSideName));
	strncpy(sideinfo->pszSideName,pBuf->side_name, sizeof(pBuf->side_name)-1);
	sideinfo->usRecNum ++ ;            //�ùɵ�����ļ�¼����һ����¼�����ж������

	int j = sideinfo->usRecNum -1;
	//7,8,9,10
	sideinfo->rec[j].usCarriageExplain = pBuf->car_flag;          //˵����Ϣ
	sideinfo->rec[j].usCarriageType = pBuf->car_type;       //��������
	sideinfo->rec[j].usCarriageNum = pBuf->car_count;        //������Ŀ��
	CString remark ;
	remark.Format("%s",pBuf->destine);
	//����^���
	int pos = remark.Find("^");
	if(pos < 0)
	{
		strncpy(sideinfo->rec[j].lpszCarriageRemark, pBuf->destine,sizeof(sideinfo->rec[j].lpszCarriageRemark));  //����ȥ�����Ϻ�	
		sideinfo->rec[j].lpszCarriageApply[0] = '\0';
	}
	else
	{			
		CString apply;
		apply = remark.Left(pos);
		strcpy(sideinfo->rec[j].lpszCarriageApply,(char*)(LPCTSTR)apply);

		apply = remark.Mid(pos+1);
		strcpy(sideinfo->rec[j].lpszCarriageRemark,(char*)(LPCTSTR)apply);     //����ȥ�����Ϻ�	 					
	}

	struct _tagSideStoreCarriage *rec;	
	CString text;

	text  = "";
	//������ʾ��վ����ʾ�ɵ���Ϣ

	CString strStyle;
	for(j=0;j < Info[id][i].usSideNum;j++)    
	{  
		sideinfo = &Info[id][i].sideinfo[j];
		for(int k=0;k < sideinfo->usRecNum;k++)
		{ 
			rec = &sideinfo->rec[k];         
			if(rec->usCarriageNum ==0)     /////new added 
				continue;

			BYTE nSel, nSubSel;
			pBasicParam->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);
			strStyle = pBasicParam->GetCarTypeShortName(nSel);
		    text += sideinfo->pszSideName;//added by zhaoliang
			text += ":";
			text += strStyle;

			if(rec->usCarriageExplain>4 )   ////new added
				rec->usCarriageExplain = 0;			
			//nSel=rec->usCarriageType;

			CString remark = rec->lpszCarriageRemark;

			CString strExplain = pBasicParam->GetCarriageExplain(rec->usCarriageExplain);
			CString strCarShortName = pBasicParam->GetCarShortName(nSel, nSubSel);
			CString desp;
			desp.Format("%s%s%d", strExplain, strCarShortName, rec->usCarriageNum);
			
			//�س���ȥ�� add for xa 11.12
			remark.Trim();
			CString tmp1;
			if(nSel==1 && !remark.IsEmpty())
				tmp1.Format("(%s),", remark); 
			else
				tmp1 = ",";
			text += desp;
			text += tmp1;
			////////////////////////
		}//end for		 
	}//end for


	byte type ;

	if(pBuf->phase ==  CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE_18)
	{
		type = 18;
		pDoc->m_car_phase_18[usStationNo] = text;
	}
	else if(pBuf->phase == CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE_0)
	{
		type = 0;
		pDoc->m_car_phase_0[usStationNo] = text;
	}
	else if(pBuf->phase == CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE_6)
	{
		type = 6;
		pDoc->m_car_phase_6[usStationNo] = text;
	}
	else if(pBuf->phase == CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE_12)
	{
		type = 12;
		pDoc->m_car_phase_12[usStationNo] = text;
	}

	return;
}

void CPlot24View::Query_Rem()
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));
	CLBTH::MsgNotePadQuery mMsgNotePadQuery;
	unsigned long shift_id;
	if(pDoc->m_printFormat == 1)//��ӡʮ��Сʱ��ֻ��ѯ��ǰ��
		shift_id=  pDoc->m_sPrintInform->usShiftid;
	else
		shift_id =  pDoc->m_shiftidFirst;

	mMsgNotePadQuery.shift_id = shift_id;
	mMsgNotePadQuery.con_id = pDoc->m_sPrintInform->usSectionNo;
	mMsgNotePadQuery.schd_type = pDoc->m_sPrintInform->usSchdlType;

	long nEntity=GetSpecialEntityID(SERVER_TYPE_DB,pDoc->m_nCurrentSectionNo);
	sysprint(5, 0, "[��ӡ] ��entity %d ���� con_id %d shift_id %d schd_type %d ����", nEntity, mMsgNotePadQuery.con_id, 
		mMsgNotePadQuery.shift_id, mMsgNotePadQuery.schd_type);

	SendTGBMsg(pDoc->m_nCurrentSectionNo, &mMsgNotePadQuery, nEntity);
	//SendBMsg(&mMsgNotePadQuery, nEntity);
}

void CPlot24View::Query_Phase()
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));

	CLBTH::MsgWordPhasePlanQuery mMsgWordPhasePlanQuery;
	unsigned long shift_id;
	if(pDoc->m_printFormat == 1)//��ӡʮ��Сʱ��ֻ��ѯ��ǰ��
	{
		shift_id = pDoc->m_sPrintInform->usShiftid;	
	}
	else
	{
		shift_id =  pDoc->m_shiftidFirst;
	}

	mMsgWordPhasePlanQuery.shift_id = shift_id;
	mMsgWordPhasePlanQuery.con_id = pDoc->m_sPrintInform->usSectionNo;
	mMsgWordPhasePlanQuery.schd_type = pDoc->m_sPrintInform->usSchdlType;
	mMsgWordPhasePlanQuery.phase_count = 4;

	mMsgWordPhasePlanQuery.phase_id[0] = 3;
	mMsgWordPhasePlanQuery.phase_id[1] = 0;
	mMsgWordPhasePlanQuery.phase_id[2] = 1;
	mMsgWordPhasePlanQuery.phase_id[3] = 2;

	long nEntity=GetSpecialEntityID(SERVER_TYPE_DB,pDoc->m_nCurrentSectionNo);
	sysprint(5, 0, "[��ӡ] ��entity %d ���� con_id %d shift_id %d schd_type %d �׶μ���", nEntity, 
		mMsgWordPhasePlanQuery.con_id, mMsgWordPhasePlanQuery.shift_id, mMsgWordPhasePlanQuery.schd_type);

	SendTGBMsg(pDoc->m_nCurrentSectionNo,&mMsgWordPhasePlanQuery,nEntity);
	//SendBMsg(&mMsgWordPhasePlanQuery,nEntity);
}

extern void SetStartEndtime(BYTE flag, int m_nShiftID, int& nEnd_time, int&nStart_time);
void CPlot24View::Query_StationCar()
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument( ));

	pDoc->m_car_phase_0.clear();
	pDoc->m_car_phase_6.clear();
	pDoc->m_car_phase_12.clear();
	pDoc->m_car_phase_18.clear();

	memset(&Info ,0x00, sizeof(_tagStationStoreTrain)*64*4);

	
	int nStationNum;
	int nStationList[128];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(pDoc->m_nCurrentSectionNo,nStationNum,nStationList);

	int id = pDoc->m_sPrintInform->usShiftid;

	int CurBanbie=id-(id/10)*10;


	CLBTH::MsgSimpleStationCarQuery mMsgSimpleStationCarQuery;
	byte type[3];

	if(CurBanbie ==2)
	{   
		type[0] = 6; 
		type[1] = 12;
		type[2] = 18;  
	}
	else
	{  
		type[0] = 18; 
		type[1] = 0;
		type[2] = 6;  
	}

	int nPhase=0;
	for(int j=0;j<sizeof(type)/sizeof(type[0]);j++)	
	{
		if(type[j] == 18)
			nPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18;
		else if(type[j] == 0)
			nPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0;
		else if(type[j] == 6)
			nPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6;
		else if(type[j] == 12)
			nPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12;

		SetStartEndtime(nPhase, id, nEnd_time, nStart_time);
		mMsgSimpleStationCarQuery.begin_time = nStart_time;
		mMsgSimpleStationCarQuery.end_time = nEnd_time;
		mMsgSimpleStationCarQuery.phase = (CLBTH::MsgSimpleStationCarQuery::STACAR_PHASE)nPhase;
		int StationIndex = 0;
		//only one station
		if(StationIndex != 0)
		{
			mMsgSimpleStationCarQuery.sta_count = 1;
			mMsgSimpleStationCarQuery.sta_list[0] = StationIndex;
		}
		//
		else
		{
			mMsgSimpleStationCarQuery.sta_count = nStationNum;
			for(int i=0;i<nStationNum;i++)
				mMsgSimpleStationCarQuery.sta_list[i] = nStationList[i];
		}

		long nEntity=GetSpecialEntityID(SERVER_TYPE_DB, pDoc->m_nCurrentSectionNo);
		sysprint(5, 0, "[��ӡ] ��entity %d ����׶ε� %d ��վ�泵", nEntity, type[j]);
		SendBMsg(&mMsgSimpleStationCarQuery,nEntity);
	}

	return;
}

void CPlot24View::OnSavepicture() 
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument());

	if(pDoc == NULL)
		return;

	//ɾ��һ����ǰ��ͼƬ�ļ�
	DelPictures(365*24*3600);

	CString fileName,filePath;

	CString picName;	
	filePath = gpcdef.GetDataDir();
	if(filePath.Right(1) == "\\")
		filePath += "Pictures\\";
	else
		filePath += "\\Pictures\\";

	CreateRecursiveDir((char*)(LPCTSTR)filePath);

	if(pDoc->m_nSchdStatus==BASIC_SCHEDULE)
	{
		basic_schd_config* pbasic=GetBasicConfig();
		if(pDoc->m_sPrintInform->usSchdlType == 1)
		{
			CString name=GetBasicConfig()->GetBasicNameByType(pDoc->m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
			if(name.IsEmpty())
				picName.Format("%02d%s_���û���ͼ.Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
			else
				picName.Format("%02d%s_���û���ͼ(%s).Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo),name);
			
		}
		else if(pDoc->m_sPrintInform->usSchdlType == 2)
		{
			CString name=GetBasicConfig()->GetBasicNameByType(pDoc->m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
			if(name.IsEmpty())
				picName.Format("%02d%s_Ԥ������ͼ.Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
			else
				picName.Format("%02d%s_Ԥ������ͼ(%s).Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo), name);
			
		}
		else if(pDoc->m_sPrintInform->usSchdlType == 10)
		{
			CString name=GetBasicConfig()->GetBasicNameByType(pDoc->m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
			if(name.IsEmpty())
				picName.Format("%02d%s_�鵵����ͼ.Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
			else
				picName.Format("%02d%s_�鵵����ͼ(%s).Jpg", pDoc->m_sPrintInform->usSectionNo,
					gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo), name);
		}
		else
		{
			picName.Format("%d%02d%s.Jpg", pDoc->m_sPrintInform->usShiftid, 
					pDoc->m_sPrintInform->usSectionNo,
				gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
		}
	}
	else if(pDoc->m_nSchdStatus==SHIFT_SCHEDULE)
	{
		picName.Format("%d%02d%s_�հ�ƻ�.Jpg", pDoc->m_sPrintInform->usShiftid, 
			pDoc->m_sPrintInform->usSectionNo,
		gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
	}
	else
	{
		picName.Format("%d%02d%s.Jpg", pDoc->m_sPrintInform->usShiftid, 
			pDoc->m_sPrintInform->usSectionNo,
		gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
	}

	filePath += picName;
	
	fileName = filePath;

	CString ext=fileName.Right(4);

	if (!ext.CompareNoCase(".JPG"))
	{
		CBitmapFile bmpFile;
		bmpFile.SaveBitmapAsJpgFile(&m_dcMemory,&m_MemBitmap,(char*)(LPCTSTR)fileName);
	}
	CString tip = "��������ͼ" + fileName + "�ɹ���";


	if(gpcdef.GetUserIdentify() == GPC_DISPATCHER)
	{
		PutFileToServer(fileName, picName);
	}

	AfxMessageBox(tip);
		
	return;
}


BOOL CPlot24View::SaveBmp(HBITMAP hBitmap, CString FileName)
{
	HDC hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���
	int iBits;
	//λͼ��ÿ������ռ�ֽ���
	WORD wBitCount;
	//�����ɫ���С�� λͼ�������ֽڴ�С ��λͼ�ļ���С �� д���ļ��ֽ��� 
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0; 
	//λͼ���Խṹ 
	BITMAP Bitmap;  
	//λͼ�ļ�ͷ�ṹ
	BITMAPFILEHEADER bmfHdr;  
	//λͼ��Ϣͷ�ṹ 
	BITMAPINFOHEADER bi;  
	//ָ��λͼ��Ϣͷ�ṹ  
	LPBITMAPINFOHEADER lpbi;  
	//�����ļ��������ڴ�������ɫ���� 
	HANDLE fh, hDib, hPal,hOldPal=NULL; 

	//����λͼ�ļ�ÿ��������ռ�ֽ��� 
	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
	DeleteDC(hDC); 
	if (iBits <= 1)		wBitCount = 1; 
	else if (iBits <= 4) 	wBitCount = 4; 
	else if (iBits <= 8) 	wBitCount = 8; 
	else				 	wBitCount = 24; 

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize			= sizeof(BITMAPINFOHEADER);
	bi.biWidth			= Bitmap.bmWidth;
	bi.biHeight			= Bitmap.bmHeight;
	bi.biPlanes			= 1;
	bi.biBitCount		= wBitCount;
	bi.biCompression	= BI_RGB;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrImportant	= 0;
	bi.biClrUsed		= 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//Ϊλͼ���ݷ����ڴ� 
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER)); 
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
	*lpbi = bi; 

	// �����ɫ��  
	hPal = GetStockObject(DEFAULT_PALETTE); 
	if (hPal) 
	{ 
		hDC = ::GetDC(NULL); 
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
		RealizePalette(hDC); 
	}

	// ��ȡ�õ�ɫ�����µ�����ֵ 
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
		+dwPaletteSize,	(BITMAPINFO *)lpbi, DIB_RGB_COLORS); 

	//�ָ���ɫ��  
	if (hOldPal) 
	{ 
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
		RealizePalette(hDC); 
		::ReleaseDC(NULL, hDC); 
	} 

	//����λͼ�ļ�  
	fh = CreateFile(FileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 

	if (fh == INVALID_HANDLE_VALUE)  return FALSE; 

	// ����λͼ�ļ�ͷ 
	bmfHdr.bfType = 0x4D42; // "BM" 
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 
	// д��λͼ�ļ�ͷ 
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 
	// д��λͼ�ļ��������� 
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 
	//���  
	GlobalUnlock(hDib); 
	GlobalFree(hDib); 
	CloseHandle(fh); 

	return TRUE;
}

BOOL CPlot24View::SaveJPG(CString fileName, BOOL color)
{	

	// note, because i'm lazy, most image data in this app
	// is handled as 24-bit images. this makes the DIB
	// conversion easier. 1,4,8, 15/16 and 32 bit DIBs are
	// significantly more difficult to handle.

	if (m_picBuf==NULL) {
		AfxMessageBox("No Image!");
		return FALSE;
	}

	// we vertical flip for display. undo that.
	JpegFile::VertFlipBuf(m_picBuf, m_picWidth * 3, m_picHeight);

	// we swap red and blue for display, undo that.
	JpegFile::BGRFromRGB(m_picBuf, m_picWidth, m_picHeight);


	// save RGB packed buffer to JPG
	BOOL ok=JpegFile::RGBToJpegFile(fileName, 
		m_picBuf,
		m_picWidth,
		m_picHeight,
		color, 
		75);			// quality value 1-100.
	if (!ok) {
		AfxMessageBox("Write Error");
	} else {
		// load what we just saved
		//	LoadJPG(fileName);
		//	Invalidate(TRUE);
	}

	return TRUE;

}

BOOL CPlot24View::LoadBMP(CString fileName)
{
	if (m_picBuf!=NULL) {
		delete [] m_picBuf;
	}

	BMPFile theBmpFile;

	m_picBuf=theBmpFile.LoadBMP(fileName, &m_picWidth, &m_picHeight);

	if ((m_picBuf==NULL) || (theBmpFile.m_errorText!="OK")) 
	{
		AfxMessageBox(theBmpFile.m_errorText);
		m_picBuf=NULL;
		return FALSE;
	}

	//////////////////////
	// set up for display

	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	JpegFile::BGRFromRGB(m_picBuf, m_picWidth, m_picHeight);

	// vertical flip for display
	JpegFile::VertFlipBuf(m_picBuf, m_picWidth * 3, m_picHeight);

	return TRUE;
}

//�����ļ�ȫ·��������Ҫ��Ŀ¼
/*********************************************
char* filename //��ʽ�� c:\temp\test\1.txt
//c:\temp\test\
*******************************************/


//ɾ��һ����ǰ��ͼƬ
int CPlot24View::DelPictures(time_t elapsedtime)
{

	CString picPath;
	picPath = CString(gpcdef.GetDataDir());
	if(picPath.Right(1) == "\\")
		picPath += "Pictures\\";
	else
		picPath += "\\Pictures\\";


	picPath += "*.Jpg";

	CFileFind cf;
	BOOL findFlag = TRUE;
	findFlag = cf.FindFile(picPath);

	if(!findFlag)
		return 0;

	time_t curTime,fileTime;
	time(&curTime);
	curTime -= elapsedtime;

	CString fileName;	
	struct _stat st;

	while(findFlag)
	{
		findFlag = cf.FindNextFile();

		if(cf.IsDots() || cf.IsDirectory())
			continue;
		fileName = cf.GetFilePath();
		if(_stat((char*)(LPCTSTR)fileName,&st)<0)
			continue;;

		fileTime = st.st_mtime;

		if(fileTime < curTime)
		{		
			DeleteFile(fileName);
		}			
	}
	cf.Close();
	return 1;
}

void CPlot24View::OnPlotOutput()
{ 
	if(gpcdef.bGpcTrain == TRUE) return;
	int theStatus = 1,rc;

	CPlotDoc* p = GetDocument( );
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,p);

	
	do {
		if(pDoc == NULL)
			return;

		char title[256];
		strcpy(title,gpcdef.GetSectionName(pDoc->m_sPrintInform->usSectionNo));
		if(pDoc->m_nSchdStatus == BASIC_SCHEDULE)
		{
			strcat(title,"����ͼ");
		}
		else
		{
			strcat(title,"����ͼ");
		}


		CDC printDC;
		HDC prnDc;

		CPrintDialog printDlg(TRUE);

		CSize mSize;

		if(printDlg.GetDefaults( ) == 0)
		{  
			sysprint(0,PLOT.ThreadId,"[PLOT] get default plot failed");
			break;
		}

		if((prnDc = printDlg.CreatePrinterDC()) == NULL)
		{
			rc = GetLastError( );
			sysprint(0,PLOT.ThreadId,"[PLOT] create cdc failed %d",rc);
			break;
		}

		if( !printDC.Attach(prnDc)){
			sysprint(0,PLOT.ThreadId,"[PLOT]: attach print dc failed!");
			break;
		}

		printDC.m_bPrinting = TRUE;        

		DOCINFO docinfo;
		memset(&docinfo,0x00,sizeof(DOCINFO));
		docinfo.cbSize = sizeof(DOCINFO);
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = title;

		if(printDC.StartDoc(&docinfo) == -1)
		{
			AfxMessageBox("��ӡ����æ�����Ժ����� !");
			break;
		}
		printDC.StartPage( );

		/////ʵ���豸ÿӢ�����������6779,4157
		mSize.cx = printDC.GetDeviceCaps(HORZRES);
		mSize.cy = printDC.GetDeviceCaps(VERTRES);
		pDoc->ChangeDocumentArea(mSize.cx,mSize.cy);

		this->m_nPageWidth = pDoc->m_nPageWidth;
		this->m_nPageHeight = pDoc->m_nPageHeight;	

		int x_offset_print, y_offset_print, height, width;
		if(!pDoc->GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
			return;
		pDoc->x_offset_print = x_offset_print;
		pDoc->y_offset_print = y_offset_print;


		if(pDoc->m_nSchdStatus != BASIC_SCHEDULE)
		{
			//����վ�泵���	
			if(pDoc->IsPrintPhaseCar(pDoc->m_sPrintInform->usSectionNo)) 
			{
				pDoc->DrawPhaseStoreTrainFrame24(&printDC,CLR_PLOT);
				pDoc->DrawStationCar(&printDC,CLR_BLOCK);
			}

			//������Ϣ
			pDoc->DrawTrainInform24(&printDC,CLR_PLOT);
			//����Ա����
			pDoc->DrawDDYName24(&printDC,CLR_PLOT);

			//����
			pDoc->DrawJiShi(&printDC,CLR_PLOT);

			//�׶μƻ����
			if(pDoc->mPlotStyle.id == 1)
			{
				pDoc->DrawPhasePlanFrameXian(&printDC,CLR_PLOT,NULL);
				pDoc->FillPhaseContentXian(&printDC,CLR_PLOT);
			}
			else
			{
				pDoc->DrawPhasePlanFrame(&printDC,CLR_PLOT,NULL);
				pDoc->FillPhaseContent(&printDC,CLR_PLOT);
			}
	
			//��ע
			pDoc->DrawFooterTitle(&printDC,CLR_PLOT);
		}

		//��վ��
		pDoc->DrawStationAixis(&printDC, CLR_PLOT);
		//ʱ����
		pDoc->DrawTimeAixis(&printDC,CLR_PLOT); //CLR_DARKGREEN
		//���µ�ʱ����
		pDoc->ShowTimeString(&printDC,CLR_PLOT);
		//���ҵĳ�վ����
		pDoc->ShowStationString(&printDC,CLR_PLOT,TRUE);	
		//���⳵վ
		pDoc->ShowVirtualStationString(&printDC,CLR_PLOT);
		//����
		pDoc->DrawHeaderTitle(&printDC,CLR_PLOT);		
		//���Ƴ�
		pDoc->DrawSchdTrain(&printDC);

		//���Ʊ�ǣ�������󣬱�������������
		if(pDoc->m_nSchdStatus != BASIC_SCHEDULE)
		{
			pDoc->DrawTrainMark(&printDC);
		}

		printDC.EndPage( );
		if(printDC.EndDoc( ) < 0)
			break;
		printDC.Detach( );
		DeleteDC(prnDc);

		theStatus = 0;

	}while(0);
	//2004-11-06
	if(pDoc->m_nSchdStatus == BASIC_SCHEDULE)
	{
		pDoc->ChangeDocumentArea(g_basic_total_page_width,g_basic_total_page_height);
	}
	else
	{
		if(gpcdef.GetPrintFormat() == 1)
		{
			pDoc->ChangeDocumentArea(g_zz_total_page_width,g_zz_total_page_height);
		}
		else
		{
			pDoc->ChangeDocumentArea(g_hh_total_page_width,g_hh_total_page_height); 
		}
	}

	this->m_nPageHeight = pDoc->m_nPageHeight;
	this->m_nPageWidth = pDoc->m_nPageWidth;
}

#include "afxinet.h"
void CPlot24View::PutFileToServer(CString filePathName, CString fileName)
{
	CPlot24Doc *pDoc = DYNAMIC_DOWNCAST(CPlot24Doc,GetDocument());
	if(NULL == pDoc)
		return;

	FtpInfo ftpInfo;

	if(!gpcdef.GetFtpLoginInfo(ftpInfo))
	{
		GpcLogTrainMessage(0,0, "��ȡFTP��������Ϣʧ��, ͼƬ��FTP������ʧ��");
		return;
	}
	GpcLogTrainMessage(0,0, "FTP������:ip:%s user:%s pwd:%s port:%d", ftpInfo.ip, ftpInfo.user, ftpInfo.pwd, ftpInfo.port);

	CFtpConnection* pFtpConnection = NULL;
	CInternetSession* pInterSession = new CInternetSession(AfxGetAppName());
	try
	{
		pFtpConnection = pInterSession->GetFtpConnection(ftpInfo.ip, ftpInfo.user, ftpInfo.pwd, ftpInfo.port);
	}
	catch(CInternetException *pEx)
	{
		pEx->Delete();
		pFtpConnection = NULL;
		pInterSession->Close();
		delete pInterSession;
		pInterSession = NULL;
		return;
	}

	CFileFind finder;
	if(finder.FindFile(filePathName))
	{
		DWORD usShiftid = pDoc->m_sPrintInform->usShiftid;
		CString str,month;
		str.Format("%d", usShiftid);
		month = str.Right(6).Left(2);
		
		CString dirName = ftpInfo.dirName;
		dirName.Format("%s/%s/%s", dirName, month, fileName);
		
		if(!pFtpConnection->PutFile(filePathName, dirName))
			GpcLogTrainMessage(0,0, "FTP������:�ϴ��ļ� %s ʧ��", filePathName);
		else
			GpcLogTrainMessage(0,0, "FTP������:�ϴ��ļ� %s �ɹ�", filePathName);
	}
	else
	{
		GpcLogTrainMessage(0,0, "FTP������:�Ҳ��������ϴ��ļ� %s.�ϴ��ļ�ʧ��", filePathName);
	}

	pFtpConnection->Close();
	pInterSession->Close();
	delete pInterSession;
	pInterSession = NULL;
	return;
}


// TestTgView.cpp : CMMPlanView ���ʵ��
//

#include "stdafx.h"
#include "resource.h"
#include "sysshare.h"
#include "MMPlanView.h"
#include "comutil.h"
#include "MMIPlanFrame.h"
#include "utility/xml_utility.h"
#include "WSMaintanPlan.h"
#include "DlgSimpleInput.h"
#include "tgview/tgdocument.h"
#include "DlgGridInput.h"
#include "WebReference.h"
#include "tgpspdata.h"
#include "td/td_frame.h"
#include "tgdocument.h"
#include ".\mmplanview.h"
#include ".\workcancel.h"
#include "tg_function_config.h"
#include "DlgSelectShift.h"
#include "gridctrl_src/GridCellCombo.h"
#include "DlgMarkResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMMPlanView
#define IDC_PLAN_TREE 6100
#define IDC_PLAN_GRID 6000
#define IDC_PLAN_BANBIE 6001
#define IDC_PLAN_DATE 6002
#define IDC_PLAN_SUB_PLANID 6003
#define IDC_PLAN_STARTM 6004
#define IDC_PLAN_ENDTM 6005
#define IDC_PLAN_USETIME 6006
#define IDC_PLAN_REFRESH 6007
#define IDC_PLAN_RJHH   6008

#define IDC_PLAN_SHOW_MARK 6010
#define IDC_PLAN_SHOW_SKYLIGHT 6011

#define TREE_NODE_TYPE_DATE   1
#define TREE_NODE_TYPE_MAINID 2
#define TREE_NODE_TYPE_SUBID  3
#define TREE_NODE_TYPE_MARK   4

static char g_xml_header[]="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
extern char g_info_node_from_ws[];
extern BOOL UR_CanOperateSchedule();
extern int DB_GetEntityStatus(long eid);
IMPLEMENT_DYNCREATE(CMMPlanView, CView)

BEGIN_MESSAGE_MAP(CMMPlanView, CView)
	// ��׼��ӡ����
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK,IDC_PLAN_TREE,OnTreeRClick)
	ON_NOTIFY(TVN_SELCHANGED,IDC_PLAN_TREE,OnTreeSelChanged)
	ON_BN_CLICKED(IDC_PLAN_DATE,OnDate1Click)
	ON_BN_CLICKED(IDC_PLAN_BANBIE,OnDate2Click)
	ON_BN_CLICKED(IDC_PLAN_USETIME,OnUseTimeClick)
	ON_BN_CLICKED(IDC_PLAN_REFRESH,OnMPlanRefresh)
	ON_BN_CLICKED(IDC_PLAN_SHOW_MARK, OnOnlyShowMarkList)
	ON_BN_CLICKED(IDC_PLAN_SHOW_SKYLIGHT, OnOnlyShowSkylightList)

	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_COMMAND(ID_MPLAN_SAVE_TO_DB,OnSaveToDB)
	
	ON_COMMAND(ID_MPLAN_RECV_FROM_OPMS,OnRecvFromOPMS)
	ON_COMMAND(ID_MPLAN_GETALL_INFO, OnMplanGetallInfo)
	ON_COMMAND(ID_MPLAN_GENMARK_ALL,OnGenMarkAll)
	ON_COMMAND(ID_MPLAN_TRANS_DCMD_ALL,OnTransDCMDAll)

	ON_UPDATE_COMMAND_UI(ID_MPLAN_TRANS_DCMD_ALL, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_GENMARK_ALL, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_GETALL_INFO, OnUpdateEnable)

	ON_COMMAND(ID_MMPLAN_CANCEL_MARK,OnCancelGenMark)
	ON_COMMAND(ID_MMPLAN_RECOVER_MARK,OnRecoverGenMark)
	ON_COMMAND(ID_MMPLAN_SELECT_DCMD, OnMmplanSelectDcmd)

	ON_COMMAND(ID_MMPLAN_GEN_MARK,OnGenMark)
	ON_COMMAND(ID_MPLAN_GET_INFO,OnGetInfo)
	ON_COMMAND(ID_MPLAN_GET_DCMD,OnGetDCMD)
	ON_COMMAND(ID_MPLAN_GET_JIESHI,OnGetJieShi)
	ON_COMMAND(ID_MPLAN_TRANS_DCMD,OnTransDCMD)

	ON_UPDATE_COMMAND_UI(ID_MPLAN_GET_INFO, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_GET_DCMD, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_GET_JIESHI, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_TRANS_DCMD, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_MMPLAN_GEN_MARK, OnUpdateEnable)	
END_MESSAGE_MAP()

void CMMPlanView::OnUpdateEnable(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(UR_CanOperateSchedule());
}

CMMPlanView::CMMPlanView()
{
}

CMMPlanView::~CMMPlanView()
{

}

// CMMPlanView ����
void CMMPlanView::OnDraw(CDC* /*pDC*/)
{
}
void CMMPlanView::OnPaint()
{
	// TODO: Add your message handler code here and/or call default
	CPaintDC dc(this);
	CRect ClipRect;
	CBrush brush(RGB(192,192,192));
	ClipRect=dc.m_ps.rcPaint;
	dc.FillRect(&ClipRect,&brush);
	//	return CWnd::OnEraseBkgnd(pDC);
}

// CMMPlanView ���
#ifdef _DEBUG
void CMMPlanView::AssertValid() const
{
	CView::AssertValid();
}

void CMMPlanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTgDocument* CMMPlanView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTgDocument)));
	return (CTgDocument*)m_pDocument;
}
#endif //_DEBUG

LRESULT CMMPlanView::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;


	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void  CMMPlanView::ShowMPlanList(int type)//type ��ʾ��������:0:ȫ�� 1:����/���� 2:�촰 3:ʩ�� 4:ά��
{
	int i;
	((CMMIPlanFrame *)GetParentFrame())->ShowJieShiInBar(NULL,NULL);
	((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(NULL,NULL);
	((CMMIPlanFrame *)GetParentFrame())->ShowDCMDInfoInBar(NULL,NULL);  // clear all

	CTG_TrainScheduleBase *pWorkData;
	CTG_TrainScheduleBase *pShiftData;
	pShiftData=GetDocument()->GetTGDataHolder()->GetShiftScheduleData();
	pWorkData=GetDocument()->GetTGDataHolder()->GetShiftScheduleData();
	CTG_ObjectPtrGroup group;
	BOOL need_selected=FALSE;
	m_mplan_tree.Reset(); // 2010.3.20 ɾ��������Ŀ
	for(i=0;i<GetDocument()->GetMPlanCount();i++)
	{
		CString tmpstr;
		CMMPlanListInfo *pMMPlan=GetDocument()->GetMPlanPtr(i);
		if(!pMMPlan)
			continue;

		if (type==4 && pMMPlan->m_nFlag!=1)
			continue;
		else if (type==3 && pMMPlan->m_nFlag!=0)
			continue;

		need_selected=TRUE; // Ĭ��ȫѡ��!
		CTime sttm,endtm;
		TIME st,et;
		GetDocument()->GetMMPlanRange(st,et); 
		CTime cst=st;
		if(m_bStartDate.GetCheck()!=1)
			cst+=CTimeSpan(1,0,0,0);

		if(m_bUseTime.GetCheck()==1)
		{
			m_startTimeCtrl.GetTime(sttm); 
			m_endTimeCtrl.GetTime(endtm);
			CTime x=CTime(cst.GetYear(),cst.GetMonth(),cst.GetDay(),sttm.GetHour(),sttm.GetMinute(),0);     
			CTime y=CTime(cst.GetYear(),cst.GetMonth(),cst.GetDay(),endtm.GetHour(),endtm.GetMinute(),0);     
			if(y<x)
				y+=CTimeSpan(1,0,0,0);
			if(GetDocument()->GetMMPlanEndTime(pMMPlan)<x.GetTime())
				continue;
			if(GetDocument()->GetMMPlanStartTime(pMMPlan)>y.GetTime())
				continue;
			need_selected=TRUE;
		}
		CString rjhh;
		m_edRiJHH.GetWindowText(rjhh);
		if(!rjhh.IsEmpty())
		{
			if(pMMPlan->GetSubIDByJHH(atoi(rjhh)))
				need_selected=TRUE;
		}
		group.Clear();

		if (type>=3)
			InsertMMPlanToTree(pMMPlan, 0);
		else
			InsertMMPlanToTree(pMMPlan, type);

	}

	m_edRiJHH.SetWindowText("");
}

void  CMMPlanView::ShowMPlanInfo(const char *xml,const char *node)
{
	if(!xml || !node)
	{
		((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(NULL,NULL);
		return;
	}
	CString  listinfo;
	CTD_MPLAN_SG_WX info;
	info.LoadSG_WXFromTDXML(xml,node);
	info.GenViewInfoXML(listinfo,"MMPLAN_INFO");
	((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(listinfo,"MMPLAN_INFO");
}

void CMMPlanView::OnTimer(UINT event)
{

	CView::OnTimer(event);
}


void CMMPlanView::ShowMPlanDate()
{
	TIME st,et;
	GetDocument()->GetMMPlanRange(st,et);
	CTime cst=st;
	m_bStartDate.SetWindowText(cst.Format("%y��%m��%d��"));  
	cst+=CTimeSpan(1,0,0,0);
	m_bSecondDate.SetWindowText(cst.Format("%y��%m��%d��"));
	m_bStartDate.SetCheck(1); 
	m_bSecondDate.SetCheck(1);
}


int CMMPlanView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect(0,0,0,0);

	m_mplan_tree.Create(WS_BORDER|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,rect,this,IDC_PLAN_TREE);
	m_mplan_tree.Startup();

	m_bStartDate.Create("",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rect,this,IDC_PLAN_DATE);
	m_bSecondDate.Create("",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rect,this,IDC_PLAN_BANBIE);

	m_startTimeCtrl.Create(WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT,rect,this,IDC_PLAN_STARTM);
	m_endTimeCtrl.Create(WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT,rect,this,IDC_PLAN_ENDTM);

	m_bUseTime.Create("ָ��ʱ���",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rect,this,IDC_PLAN_USETIME);
	m_cmdRefresh.Create("ˢ��",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,rect,this,IDC_PLAN_REFRESH);

	m_bOnlyShowMark.Create("ʩ��",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rect,this,IDC_PLAN_SHOW_MARK);
	m_bOnlyShowSkyLight.Create("�촰",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rect,this,IDC_PLAN_SHOW_SKYLIGHT);

	m_edRiJHH.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this,IDC_PLAN_RJHH);
	m_capRiJHH.Create("�ռƻ���",WS_CHILD|WS_VISIBLE|SS_CENTER,rect,this);//SS_GRAYFRAME
	
	if(GetTGFunctionConfig()->IsShowMMPlanSkyLightButton())
	{
		m_bOnlyShowMark.ShowWindow(SW_SHOW);
		m_bOnlyShowSkyLight.ShowWindow(SW_SHOW);

		m_bOnlyShowMark.SetCheck(1);
		m_bOnlyShowSkyLight.SetCheck(1);
	}
	else if(GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
	{
		m_bOnlyShowSkyLight.SetWindowText("ά��");

		m_bOnlyShowMark.ShowWindow(SW_SHOW);
		m_bOnlyShowSkyLight.ShowWindow(SW_SHOW);

		m_bOnlyShowMark.SetCheck(1);
		m_bOnlyShowSkyLight.SetCheck(1);
	}
	else 
	{
		m_bOnlyShowMark.ShowWindow(0);
		m_bOnlyShowSkyLight.ShowWindow(0);
	}
	return 0;
}


void CMMPlanView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_bStartDate.GetSafeHwnd())
		m_bStartDate.MoveWindow(2,2,140,20);
	if(m_bSecondDate.GetSafeHwnd())
		m_bSecondDate.MoveWindow(145,2,140,20);
	///////////////////////////////////
	if(m_bUseTime.GetSafeHwnd())
		m_bUseTime.MoveWindow(288,2,100,20);
	if(m_startTimeCtrl.GetSafeHwnd())
		m_startTimeCtrl.MoveWindow(388,2,100,20);
	if(m_endTimeCtrl.GetSafeHwnd())
		m_endTimeCtrl.MoveWindow(492,2,100,20);
	////////////////////////////
	if(m_capRiJHH.GetSafeHwnd())
		m_capRiJHH.MoveWindow(2,27,60,20);
	if(m_edRiJHH.GetSafeHwnd())
		m_edRiJHH.MoveWindow(62,27,60,20);

	if(m_cmdRefresh.GetSafeHwnd())
		m_cmdRefresh.MoveWindow(142,27,50,20);

	if (m_bOnlyShowMark.GetSafeHwnd())
		m_bOnlyShowMark.MoveWindow(197,27,50,20);
	if (m_bOnlyShowSkyLight.GetSafeHwnd())
		m_bOnlyShowSkyLight.MoveWindow(252,27,50,20);

	if(m_mplan_tree.GetSafeHwnd())
	{
		m_mplan_tree.MoveWindow(2,50,cx-5,cy-50); 
	}
}

void CMMPlanView::OnDestroy() 
{
	CView::OnDestroy();
}

//----------------------------------------------------------------------------
void CMMPlanView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CString cs;
	GetTitle(cs);
	GetParentFrame()->SetWindowText(cs);
	m_nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
	ShowMPlanDate();
	ShowMPlanList();
}

void  CMMPlanView::GetTitle(CString &title)
{
	title="ʩ���ƻ�";
}

void CMMPlanView::ShowMPlanInfo(const char *subid)
{
	if(!subid)
	{
		((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(NULL,NULL);
		((CMMIPlanFrame *)GetParentFrame())->ShowJieShiInBar(NULL,NULL);
		((CMMIPlanFrame *)GetParentFrame())->ShowTrnInfoInBar(NULL,NULL);
		return;
	}

	CString rjhh=subid;
	m_edRiJHH.SetWindowText(GET_JHH_FROM_PLANID(rjhh));  
	CString  info,dcmd,jieshi,trninfo;
	CString  infonodestr,dcmdnodestr,jieshinodestr,trninfonodestr;

	CMMPlanListInfo *pMMplan=NULL;
	int mplan_index=GetDocument()->FindMMPlanBySubID(subid); 
	if(mplan_index>=0)
		pMMplan=GetDocument()->GetMPlanPtr(mplan_index);
	if(!pMMplan)
	{
		MessageBox("��Ч��ʩ���ƻ�ID");
		return;
	}
	if(pMMplan->m_bUpdatedFromMsg)
	{
		if(!GetDocument()->ReadMPLANFile(info,rjhh,XMLTYPE_MPLAN_INFO))
		{
			//AfxMessageBox("�޷���ȡxml�ļ�");
			// RequestMPLAN_Info(pMMplan->m_id);
			//return;
		}
		if(!GetDocument()->ReadMPLANFile(jieshi,rjhh,XMLTYPE_MPLAN_JIESHI))
		{
			//AfxMessageBox("�޷���ȡxml�ļ�");
			// RequestMPLAN_Info(pMMplan->m_id);
			//return;
		}
		if(!GetDocument()->ReadMPLANFile(trninfo,rjhh,XMLTYPE_MPLAN_TRNINFO))
		{
			//AfxMessageBox("�޷���ȡxml�ļ�");
			// RequestMPLAN_Info(pMMplan->m_id);
			//return;
		}
		infonodestr=XMLTYPE_MPLAN_INFO;
		jieshinodestr=XMLTYPE_MPLAN_JIESHI;
		trninfonodestr=XMLTYPE_MPLAN_TRNINFO;
	}	
	else
	{
		if(!GetDocument()->LoadMMInfoFromWS(info,pMMplan->m_guid))
		{
			AfxMessageBox("�޷��ӷ�������ȡ,�����´�TDMS��ȡ");
			return;
		}
		infonodestr=g_info_node_from_ws;
		infonodestr+="/";
		infonodestr+=XMLTYPE_MPLAN_INFO;

		jieshi=info;

		jieshinodestr=g_info_node_from_ws;
		jieshinodestr+="/";
		jieshinodestr+=XMLTYPE_MPLAN_JIESHI;
	}
	if(!info.IsEmpty())
	{
		CTD_MPLAN_SG_WX mmobj;
		CString infoxml;
		mmobj.LoadSG_WXFromTDXML(info,infonodestr);
		mmobj.GenViewInfoXML(infoxml,"MMPLAN_INFO");
		((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(infoxml,"MMPLAN_INFO");
	}
	else
		((CMMIPlanFrame *)GetParentFrame())->ShowInfoInBar(NULL,NULL);
	if(!jieshi.IsEmpty())
	{
		((CMMIPlanFrame *)GetParentFrame())->ShowJieShiInBar(jieshi,jieshinodestr);
	}
	else
		((CMMIPlanFrame *)GetParentFrame())->ShowJieShiInBar(NULL,NULL);
	if(!trninfo.IsEmpty())
	{
		((CMMIPlanFrame *)GetParentFrame())->ShowTrnInfoInBar(trninfo,trninfonodestr);
	}
	else
		((CMMIPlanFrame *)GetParentFrame())->ShowTrnInfoInBar(NULL,NULL);

}

void CMMPlanView::ShowMPlanDCMDInfo(const char *planid)
{
	if(!planid)
	{
		((CMMIPlanFrame *)GetParentFrame())->ShowDCMDInfoInBar(NULL,NULL);  // clear all
		return;
	}
	int index=GetDocument()->FindMMPlanBySubID(planid);
	if(index>=0)
		ShowMPlanDCMDInfo(index);
	else
		((CMMIPlanFrame *)GetParentFrame())->ShowDCMDInfoInBar(NULL,NULL);  // clear all
}

void CMMPlanView::ShowMPlanDCMDInfo(int index)
{
	((CMMIPlanFrame *)GetParentFrame())->ShowDCMDInfoInBar(NULL,NULL);  // clear all

	CMMPlanListInfo *pMMplan=GetDocument()->GetMPlanPtr(index); 
	if(!pMMplan)
		return;
	if(pMMplan->m_bUpdatedFromMsg)
	{
		CString dcmd;
		if(GetDocument()->ReadMPLANFile(dcmd,pMMplan->GetMainPlanID(),XMLTYPE_MPLAN_DCMD))
		{
			CString nodestr;
			nodestr.Format("%s/RECORD",XMLTYPE_MPLAN_DCMD);
			((CMMIPlanFrame *)GetParentFrame())->ShowDCMDInfoInBar(dcmd,nodestr);
		}
	}
	else
	{
		// get dcmd from WS
	}
}

void CMMPlanView::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int select_node_type=m_mplan_tree.GetSelectedRelaValue();
	if(select_node_type==TREE_NODE_TYPE_DATE)
		return;
	
	CPoint point;
	GetCursorPos(&point);
	CMenu popmenu;
	popmenu.CreatePopupMenu();

	if(UR_CanOperateSchedule())
	{
		if(select_node_type==TREE_NODE_TYPE_MAINID || select_node_type==TREE_NODE_TYPE_MARK)
		{
			popmenu.AppendMenu(MF_STRING,ID_MMPLAN_GEN_MARK,"������ͼ");
			popmenu.AppendMenu(MF_SEPARATOR);
			popmenu.AppendMenu(MF_STRING,ID_MMPLAN_RECOVER_MARK,"�ָ�ʩ�� ��ͼ");
			popmenu.AppendMenu(MF_STRING,ID_MMPLAN_CANCEL_MARK,"ȡ��ʩ��");
			popmenu.AppendMenu(MF_SEPARATOR);
		}
		if(select_node_type==TREE_NODE_TYPE_MAINID || select_node_type==TREE_NODE_TYPE_SUBID)
			popmenu.AppendMenu(MF_STRING,ID_MPLAN_GET_INFO,"��ȡʩ����Ϣ");
		if(select_node_type==TREE_NODE_TYPE_MAINID)
		{
			popmenu.AppendMenu(MF_STRING,ID_MPLAN_GET_DCMD,"��ȡʩ������");
			popmenu.AppendMenu(MF_STRING,ID_MPLAN_GET_JIESHI,"��ȡʩ����ʾ");
			popmenu.AppendMenu(MF_STRING,ID_MPLAN_TRANS_DCMD,"�����������");
		}
	}
	if(select_node_type==TREE_NODE_TYPE_MAINID)
	{
		popmenu.AppendMenu(MF_STRING,ID_MMPLAN_SELECT_DCMD,"ѡ�й�������");
	}
	popmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		this);	
	popmenu.DestroyMenu( );
}

void CMMPlanView::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MAINID
		|| m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MARK
		|| m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_SUBID)
	{
		CString planid=GetPlanID();
		if(!planid.IsEmpty())
		{
			ShowMPlanInfo(planid);
			ShowMPlanDCMDInfo(planid);
		}
		else
		{
			ShowMPlanInfo(NULL);
			ShowMPlanDCMDInfo((const char *)NULL);
		}
	}
	else
	{
		ShowMPlanInfo(NULL);
		ShowMPlanDCMDInfo((const char *)NULL);
	}
}

static CTime lastTm = CTime::GetCurrentTime();
// ����ʩ��
void CMMPlanView::OnRecvFromOPMS()
{
	if(UR_CanOperateSchedule())
	{
		CTime tm = CTime::GetCurrentTime();
		if(abs(long(tm.GetTime() - lastTm.GetTime())) < 8) // ������ٵ����ȡ
			return;
		lastTm=tm;

		TIME st,et;
		GetDocument()->GetMMPlanRange(st,et); 
		CTime cst=st;
		CTime cet=cst+CTimeSpan(1,0,0,0);
		et=cet.GetTime(); 

		if(m_bStartDate.GetCheck()!=1 &&
			m_bSecondDate.GetCheck()!=1)
		{
			MessageBox("����ѡ��ʱ��","��ʾ");
			return;
		}
		if(m_bStartDate.GetCheck()!=1)
			st=et;
		else if(m_bSecondDate.GetCheck()!=1)
			et=st;

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
		GetDocument()->RequestMMPlanFromOPMS(st,et,NULL,eid);
	}
}

char tmpxml[]=" <?xml version=\"1.0\" standalone=\"yes\" ?>  <ds_MaintainPlan xmlns=\"http://tempuri.org/ds_MaintainPlan.xsd\"> <MAINTAIN_PLAN>  <JHH>dddd</JHH>   </MAINTAIN_PLAN>  </ds_MaintainPlan>";

void CMMPlanView::OnSaveToDB()
{
	GetDocument()->SaveMMPlanToWS(); 
}

void CMMPlanView::OnGenMark()
{      
	if(m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MAINID || m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MARK)
	{
		if(MessageBox("��ͼ������ɾ��ԭ���ı�ǣ�ȷ��������ͼ��?","��ʾ",MB_YESNO)!=IDYES)
			return;

		CString planid=GetPlanID();
		if(planid.IsEmpty())
		{
			MessageBox("������ͼ����ʧ��","��ʾ");
			return;
		}
		
		GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,planid);
		GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,planid);
		int s=0, f=0;
		CString str;
		if(GetDocument()->GenMarkFromMMPlanByPlanId(planid,0x83,s,f))
			str.Format("�ƻ��� %s ��Ӧ��ʩ������, �ɹ���ͼ %d ��, ʧ�� %d ��", planid, s, f);
		else
			str.Format("�ƻ��� %s û�ж�Ӧʩ������, ��ʩ��������ͼ!", planid);

		GpcLogTrainMessage(0,0,"%s", str);
		MessageBox(str,"��ʾ");
	}
}

void CMMPlanView::OnGenMarkAll()
{
	CString strText="", strSuccess="", strFail="";
	int nSuccess=0,nFail=0;

	if (GetTGFunctionConfig()->IsGenAllMarkByDeleteExist())
	{
		if(MessageBox("��ͼ������ɾ��ԭ���ı�ǣ�ȷ��������ͼ��?","��ʾ",MB_YESNO)!=IDYES)
			return;
	}

	int count=m_mplan_tree.GetItemTotalCount();
	for(int i=0;i<count;i++)
	{
		if(m_mplan_tree.GetItemRelaValueByIndex(i)==TREE_NODE_TYPE_MAINID)
		{
			int s=0, f=0;
			CString tmp;
			CString strPlanId=m_mplan_tree.GetItemValueByIndex(i);
			if(!strPlanId.IsEmpty() && GetTGFunctionConfig()->IsGenAllMarkByDeleteExist())	
			{
				GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,strPlanId);
				GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,strPlanId);
			}

			if(GetDocument()->GenMarkFromMMPlanByPlanId(strPlanId,3,s,f))
			{
				tmp.Format("�ƻ� %s �ɹ���ͼʩ������ %d ��, ʧ�� %d ��\r\n", strPlanId, s, f);
				strSuccess+=tmp;
			}
			else
			{
				tmp.Format("�ƻ� %s û��ʩ������, ��ʩ��������ͼ\r\n", strPlanId);
				strFail+=tmp;
			}

			nSuccess+=s;
			nFail+=f;
		}
	}

	strText+=strSuccess;
	strText+="\n";
	strText+=strFail;

	CString strResult;
	strResult.Format("��ע��: ʩ�����ųɹ���ͼ %d ��, ʧ�� %d ��", nSuccess, nFail);

	GpcLogTrainMessage(0,0,strText);
	CDlgMarkResult* dlg = new CDlgMarkResult(strResult, strText, this);
	if (dlg->DoModal() == IDYES)
	{
		delete dlg;
	}

	//MessageBox(strText,"��ʾ");
}

void CMMPlanView::OnTransDCMDAll()
{
	BOOL rc=TRUE;
	int successcnt=0;
	int nodcmdcnt=0;
	int failedcnt=0;
	int MarkMainIdcnt =0;
	bool WsIsOk = true;
	int count=m_mplan_tree.GetItemTotalCount();
	CString failedtext,nodcmdtext,temp;

	for(int i=0;i<count;i++)
	{
		if(m_mplan_tree.GetItemRelaValueByIndex(i)==TREE_NODE_TYPE_MAINID)
		{
			MarkMainIdcnt ++;

			// -2:����webserverʧ�� -1:û�е������� 0:����ʧ�� 1:ת���ɹ�
			int result = GetDocument()->SaveDCMDToWS(m_mplan_tree.GetItemValueByIndex(i));
			
			if (result == -2)
			{
				WsIsOk = false;
				break;
			}
			else if (result == -1)
			{
				temp.Format("\n\t%s",m_mplan_tree.GetItemValueByIndex(i));
				nodcmdtext+= temp;
				nodcmdcnt++;
			}
			else if (result == 0)
			{
				temp.Format("\n\t%s",m_mplan_tree.GetItemValueByIndex(i));
				failedtext+= temp;
				failedcnt++;
			}
			else
				successcnt++;
		}
	}
	if(m_nEntityOfDCMD>0 && successcnt>0)
	{
		CString token,content;
		TIME st,et;
		GetDocument()->GetMMPlanRange(st,et);
		GenXMLMessage_InformDCMDUpdated(token,content,st,et);
		TGSendStringMessage(token,content,m_nEntityOfDCMD);
	}
	if (!WsIsOk)
	{
		MessageBox("WebService����ʧ��, ʩ��ת���������ʧ��","����", MB_OK|MB_ICONHAND);
		GpcLogTrainMessage(0,0,"WebService����ʧ��,ʩ��ת���������ʧ��");
	}
	else
	{
		CString cap;
		cap.Format("ʩ���ƻ��� %d ��, ת��������������,\nʩ��ת��ʧ�� %d ��:%s \nʩ��û�е������� %d ��:%s\n����ת��ɹ�!",MarkMainIdcnt,failedcnt,failedtext,nodcmdcnt,nodcmdtext);
		GpcLogTrainMessage(0,0,cap);
		MessageBox(cap,"��ʾ");
	}
}

void CMMPlanView::OnMplanGetallInfo()
{
	GpcLogTrainMessage(0, 0, "ʩ���ƻ�:��ȡȫ����Ϣ");
	GetDocument()->RequestMMPlanInfoFromOPMS(-1,0x07);  
}

void CMMPlanView::OnGetJieShi()
{
	int selected=m_mplan_tree.GetSelectedRelaValue();
	if(selected==TREE_NODE_TYPE_MAINID
		|| selected==TREE_NODE_TYPE_SUBID)
	{
		CString planid=GetPlanID();
		if(!planid.IsEmpty())
		{
			int index=GetDocument()->FindMMPlanBySubID(planid);
			if(index>=0)
			{
				GetDocument()->RequestMMPlanInfoFromOPMS(index,0x04);  
				return;
			}
		}
	}

	AfxMessageBox("��ѡ��һ��ʩ������");
}
void CMMPlanView::OnGetInfo()
{
	int selected=m_mplan_tree.GetSelectedRelaValue();
	if(selected==TREE_NODE_TYPE_MAINID
		|| selected==TREE_NODE_TYPE_SUBID)
	{
		CString planid=GetPlanID();
		if(!planid.IsEmpty())
		{
			int index=GetDocument()->FindMMPlanBySubID(planid);
			if(index>=0)
			{
				GetDocument()->RequestMMPlanInfoFromOPMS(index,0x01);  
				GpcLogTrainMessage(0, 0, "ʩ���ƻ�:��ȡ������Ϣ");
				return;
			}
		}
	}

	AfxMessageBox("��ѡ��һ��ʩ������");
}

CString CMMPlanView::GetPlanID()
{
	CString planid=m_mplan_tree.GetSelectedValue();
	if(planid.IsEmpty())
	{
		MessageBox("��ѡ�мƻ���","��ʾ");
	}

	return planid;
}

void CMMPlanView::OnGetDCMD()
{
	int selected=m_mplan_tree.GetSelectedRelaValue();
	if(selected==TREE_NODE_TYPE_MAINID
		|| selected==TREE_NODE_TYPE_SUBID)
	{
		CString planid=GetPlanID();
		if(!planid.IsEmpty())
		{
			int index=GetDocument()->FindMMPlanBySubID(planid);
			if(index>=0)
			{
				GetDocument()->RequestMMPlanInfoFromOPMS(index,0x02);  
				GpcLogTrainMessage(0, 0, "ʩ���ƻ�:��ȡ��������");
				return;
			}
		}
	}

	AfxMessageBox("��ѡ��һ��ʩ������");
}

void CMMPlanView::OnTransDCMD()
{
	int selected=m_mplan_tree.GetSelectedRelaValue();
	if(selected==TREE_NODE_TYPE_MAINID || selected==TREE_NODE_TYPE_SUBID)
	{
		CString planid=GetPlanID();
		if(planid.IsEmpty())
		{
			MessageBox("��ѡ��һ��ʩ������!","��ʾ");
			return;
		}
		
		CString str;
		// -2:����webserverʧ�� -1:û�е������� 0:����ʧ�� 1:ת���ɹ�
		int result =GetDocument()->SaveDCMDToWS(planid);
		if(result==-2)
		{
			str="WebService����ʧ��, ʩ��ת���������ʧ��!";
			MessageBox(str,"����", MB_OK|MB_ICONHAND);
		}
		else if(result==-1)
		{
			str.Format("ʩ���ƻ� %s û�ж�Ӧ��������, ת��ʧ��", planid);
			MessageBox(str,"��ʾ", MB_OK|MB_ICONHAND);
		}
		else if(result==1)
		{
			str.Format("ʩ���ƻ� %s ��Ӧ��������ת��ɹ�!", planid);
			if(m_nEntityOfDCMD>0)
			{
				CString token,content;
				TIME st,et;
				GetDocument()->GetMMPlanRange(st,et);
				GenXMLMessage_InformDCMDUpdated(token,content,st,et);
				TGSendStringMessage(token,content,m_nEntityOfDCMD);
				GpcLogTrainMessage(0, 0, "ʩ���ƻ�: %s ��������ת��",planid);
			}
			MessageBox(str,"��ʾ", MB_OK|MB_ICONHAND);
		}
		else if(result==0)
		{
			str.Format("ʩ���ƻ� %s ��Ӧ��������ת��ʧ��!", planid);
			MessageBox(str,"��ʾ", MB_OK|MB_ICONHAND);
		}
	}
	else
	{
		AfxMessageBox("��ѡ��һ��ʩ������");
	}
}


void CMMPlanView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(lHint==TGDATA_MPLAN_UPDATED)
	{
		MMplanInform *pobj=(MMplanInform *)pHint;
		if(pobj->change_flag== MMplanInform::MMPLAN_WORK_STATE_CHANGE)
		{
			if(pobj->token == "MPLAN_DONE_INFORM")
			{
				// ȡ��
				if(pobj->index)
				    TDRetCancelMPlanWorkResult(pobj->id, pobj->nResult);
				else // �ָ�
					TDRetRecoverMPlanWorkResult(pobj->id, pobj->nResult);
			}
		}
		else if(pobj->change_flag== MMplanInform::MMPLAN_LIST_CHANGED)
		{
			OnMPlanRefresh();
		}
		else
		{
			if(pobj->change_flag== MMplanInform::MMPLAN_SELECT_CHANGE)
			{
				MMplanInform *pobj=(MMplanInform *)pHint;
				m_mplan_tree.SelectAndExpandObjectByValue(pobj->id);
				ShowMPlanInfo(pobj->id);
				OnGetInfo(); //�Զ���������һ��ʩ����Ϣ
				GetParentFrame()->ActivateFrame();
			}
			if(pobj->change_flag== MMplanInform::MMPLAN_INFO_CHANGED)
				ShowMPlanInfo(pobj->id);
			else if(pobj->change_flag== MMplanInform::MMPLAN_JIESHI_CHANGED)
				ShowMPlanInfo(pobj->id);
			else if(pobj->change_flag== MMplanInform::MMPLAN_DCMD_CHANGED)
			{
				ShowMPlanDCMDInfo(pobj->index);
			}
			else if(pobj->change_flag== MMplanInform::MMPLAN_CANCEL_WORK)
			{
				CancelMPlanWork(pobj->index);
			}
		}
	}
}

void CMMPlanView::OnDate1Click()
{
	if(m_bStartDate.GetCheck()==0)
		m_bStartDate.SetCheck(1);
	else
		if(m_bStartDate.GetCheck()==1 && m_bSecondDate.GetCheck()==1)
			m_bStartDate.SetCheck(0);
}
void CMMPlanView::OnDate2Click()
{
	if(m_bSecondDate.GetCheck()==0)
		m_bSecondDate.SetCheck(1);
	else
		if(m_bSecondDate.GetCheck()==1 && m_bStartDate.GetCheck()==1)
			m_bSecondDate.SetCheck(0);
}

void CMMPlanView::OnMmplanSelectDcmd()
{
	int selected=m_mplan_tree.GetSelectedRelaValue();
	if(selected==TREE_NODE_TYPE_MAINID || selected==TREE_NODE_TYPE_SUBID)
	{
		if(m_nEntityOfDCMD>0)
		{
			CString token,content;
			TIME st,et;
			GetDocument()->GetMMPlanRange(st,et);

			CString planid = GetPlanID();
			if(!planid.IsEmpty())
			{
				GenXMLMessage_SelectDCMD(token,content,st,et,planid);
				TGSendStringMessage(token,content,m_nEntityOfDCMD);
				GpcLogTrainMessage(0, 0, "������������: %d %s %s", m_nEntityOfDCMD, token, content);
			}
		}
	}
	else
		AfxMessageBox("��ѡ��һ��ʩ���ƻ�");
}

void CMMPlanView::OnUseTimeClick()
{
	if(m_bUseTime.GetCheck()==0)
		m_bUseTime.SetCheck(1);
	else
		m_bUseTime.SetCheck(0);

}

void CMMPlanView::OnOnlyShowMarkList()
{
	if (m_bOnlyShowMark.GetCheck()==0)
	{
		m_bOnlyShowMark.SetCheck(1);
		if (m_bOnlyShowSkyLight.GetCheck()==0)		
		{
			if (GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
				ShowMPlanList(3);
			else
				ShowMPlanList(1);
		}
		else
			ShowMPlanList();
	}
	else
	{
		m_bOnlyShowMark.SetCheck(0);
		if (m_bOnlyShowSkyLight.GetCheck()==0)
			m_bOnlyShowSkyLight.SetCheck(1);
		
		if (GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
			ShowMPlanList(4);
		else
			ShowMPlanList(2);
	}
}

void CMMPlanView::OnOnlyShowSkylightList()
{
	if (m_bOnlyShowSkyLight.GetCheck()==0)
	{
		m_bOnlyShowSkyLight.SetCheck(1);
		if (m_bOnlyShowMark.GetCheck()==0)		
		{
			if (GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
				ShowMPlanList(4);
			else
				ShowMPlanList(2);
		}
		else
			ShowMPlanList();
	}
	else
	{
		m_bOnlyShowSkyLight.SetCheck(0);
		if (m_bOnlyShowMark.GetCheck()==0)
			m_bOnlyShowMark.SetCheck(1);

		if (GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
			ShowMPlanList(3);
		else
			ShowMPlanList(1);
	}
}

void CMMPlanView::OnMPlanRefresh()
{
	if(GetTGFunctionConfig()->IsShowMMPlanSkyLightButton())
	{
		if (m_bOnlyShowMark.GetCheck()==1 && m_bOnlyShowSkyLight.GetCheck()==1)
			ShowMPlanList();
		else if (m_bOnlyShowSkyLight.GetCheck()==1)
			ShowMPlanList(2);
		else if (m_bOnlyShowMark.GetCheck()==1)
			ShowMPlanList(1);
	}
	else if(GetTGFunctionConfig()->IsShowMMPlanMaintainButton())
	{
		if (m_bOnlyShowMark.GetCheck()==1 && m_bOnlyShowSkyLight.GetCheck()==1)
			ShowMPlanList();
		else if (m_bOnlyShowSkyLight.GetCheck()==1)
			ShowMPlanList(4);
		else if (m_bOnlyShowMark.GetCheck()==1)
			ShowMPlanList(3);
	}
	else
	{
		ShowMPlanList();
	}
}
void CMMPlanView::InsertMMPlanToTree(CMMPlanListInfo *mPlan, int type)
{
	CString mainid=mPlan->GetMainPlanID();
	CString rqid=GET_RQSTR_FROM_PLANID(mainid);
	int parentnode=m_mplan_tree.GetIndexByValue(rqid);
	if(parentnode<0)
		parentnode=m_mplan_tree.Insert_1_Item(-1,rqid,rqid,TREE_NODE_TYPE_DATE);   
	CString subid=GET_JHH_FROM_PLANID(mainid);
	int planState=GetDocument()->GetMMPlanState(mainid);
	CString caption="";
	if(planState==1)
		caption=subid+"[ʩ��ȡ��] - "+mPlan->m_topic;
	else
		caption=subid+" - "+mPlan->m_topic;

	parentnode=m_mplan_tree.Insert_1_Item(parentnode,caption,mainid,TREE_NODE_TYPE_MAINID);  // 1: mainid

	CStringArray subids;
	mPlan->GetSubPlanIDs(subids);
	for(int i=1;i<subids.GetCount();i++)  // skio 0, mainid self!
	{
		m_mplan_tree.Insert_1_Item(parentnode,GET_JHH_FROM_PLANID(subids.ElementAt(i)),subids.ElementAt(i),TREE_NODE_TYPE_SUBID); // 0: other ids
	}
	CStringArray marksdes;
	GetDocument()->GetMarkDesc(mainid,marksdes, type);
	for(int i=0;i<marksdes.GetCount();i++)  //  
	{
		m_mplan_tree.Insert_1_Item(parentnode,marksdes.ElementAt(i),mainid,TREE_NODE_TYPE_MARK); // 0: other ids
	}
}

// "ʩ"�˵����� ʩ���ָ�
void CMMPlanView::OnRecoverGenMark()
{
	if(m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MAINID || m_mplan_tree.GetSelectedRelaValue()==TREE_NODE_TYPE_MARK)
	{
		CString planid=GetPlanID();
		CString str;
		str.Format("ȷ�ϻָ� ʩ���ƻ�%s, ������ͼ��?",  planid);
		if(MessageBox(str,"��ʾ",MB_YESNO)!=IDYES)
			return;


		int nRet = GetDocument()->FindMMPlanByMainID(planid);
		if(nRet == -1)
			return;

		CMMPlanListInfo *pMMplan=GetDocument()->GetMPlanPtr(nRet); 
		if(!pMMplan)
			return;

		CString xml, tmp;
		xml = "<MPLAN_DONE_INFORM>";
		{
			tmp.Format("<PLANID>%s</PLANID>", pMMplan->GetMainPlanID());
			xml += tmp;
			char dispid[10];
			if(GetTGFunctionConfig()->GetTmisSectionID(GetDocument()->GetCurrentSectionNo(),dispid)<0)
			{
				AfxMessageBox("�޷���ȡTMIS�ĵ���̨����");
				return;
			}
			tmp.Format("<DISID>%s</DISID>", dispid);
			xml += tmp;

			ULONG shifttm=0;
			time_t st = XMLStringToTime(pMMplan->m_stTm);
			CTime tm(st);
			int nHour = tm.GetHour();
			if(nHour >= 6 && nHour < 18)
			{
				CTime tmp(tm.GetYear(), tm.GetMonth(), tm.GetDay(), 6,0,0);
				shifttm=tmp.GetTime();
			}
			else
			{
				CTime tmp(tm.GetYear(), tm.GetMonth(), tm.GetDay(), 18,0,0);
				shifttm=tmp.GetTime();
			}

			tmp.Format("<SHIFTID>%d</SHIFTID>", shifttm);
			xml += tmp;

			tmp.Format("<STTM>%d</STTM>", st);
			xml += tmp;

			st = XMLStringToTime(pMMplan->m_edTm);
			tmp.Format("<EDTM>%d</EDTM>", st);
			xml += tmp;

			tmp = "<ISCANCLE>0</ISCANCLE>";
			xml += tmp;

			tmp = "<REASON_ID>0</REASON_ID>";
			xml += tmp;

			tmp = "<REASON></REASON>";
			xml += tmp;

			tmp = "<REPORT></REPORT>";
			xml += tmp;
		}
		xml += "</MPLAN_DONE_INFORM>";

		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM);
		TGSendStringMessage("MPLAN_DONE_INFORM", xml, eid);
		GpcLogTrainMessage(0, 0, "��TD(%d)���ͻָ�ʩ�� PLANID %s", eid, pMMplan->GetMainPlanID());
	}
}

// "ʩ"�˵����� ʩ��ȡ��
void CMMPlanView::OnCancelGenMark()
{
	if(!UR_CanOperateSchedule())
		return;
	if((m_mplan_tree.GetSelectedRelaValue()!=TREE_NODE_TYPE_MAINID) && (m_mplan_tree.GetSelectedRelaValue()!=TREE_NODE_TYPE_MARK))
		return;

	CString planid=GetPlanID();
	CString str;
	str.Format("ȷ��ȡ�� ʩ���ƻ�%s��?",  planid);
	if(MessageBox(str,"��ʾ",MB_YESNO)!=IDYES)
		return;

	int index = GetDocument()->FindMMPlanByMainID(planid);
	if(index == -1)
		return;

	CancelMPlanWork(index);
}

// GPC����ʩ��ȡ��
void CMMPlanView::CancelMPlanWork(int index)
{
	if(!UR_CanOperateSchedule())
		return;
	CMMPlanListInfo *pMMplan=GetDocument()->GetMPlanPtr(index); 
	if(!pMMplan)
		return;

	CCancelWorkDlg dlg(this);
	dlg.m_strPlanID = pMMplan->GetMainPlanID();
	if(dlg.DoModal()==IDOK)
	{
		CString xml, tmp;
		xml = "<MPLAN_DONE_INFORM>";
		{
			tmp.Format("<PLANID>%s</PLANID>", pMMplan->GetMainPlanID());
			xml += tmp;
			char dispid[10];
			if(GetTGFunctionConfig()->GetTmisSectionID(GetDocument()->GetCurrentSectionNo(),dispid)<0)
			{
				AfxMessageBox("�޷���ȡTMIS�ĵ���̨����");
				return;
			}
			tmp.Format("<DISID>%s</DISID>", dispid);
			xml += tmp;

			ULONG shifttm=0;
			time_t st = XMLStringToTime(pMMplan->m_stTm);
			CTime tm(st);
			int nHour = tm.GetHour();
			if(nHour >= 6 && nHour < 18)
			{
				CTime tmp(tm.GetYear(), tm.GetMonth(), tm.GetDay(), 6,0,0);
				shifttm=tmp.GetTime();
			}
			else
			{
				CTime tmp(tm.GetYear(), tm.GetMonth(), tm.GetDay(), 18,0,0);
				shifttm=tmp.GetTime();
			}
			tmp.Format("<SHIFTID>%d</SHIFTID>", shifttm);
			xml += tmp;

			tmp.Format("<STTM>%d</STTM>", st);
			xml += tmp;

			st = XMLStringToTime(pMMplan->m_edTm);
			tmp.Format("<EDTM>%d</EDTM>", st);
			xml += tmp;

			tmp = "<ISCANCLE>1</ISCANCLE>";
			xml += tmp;
			tmp.Format("<REASON_ID>%d</REASON_ID>", dlg.m_nCancelCode);
			xml += tmp;

			CString strtmp;
			XMLCopyValueString(strtmp, dlg.m_strCancelReason);
			tmp.Format("<REASON>%s</REASON>", strtmp);
			xml += tmp;

			CString strtmp1;
			XMLCopyValueString(strtmp1, dlg.m_strText);
			tmp.Format("<REPORT>%s</REPORT>", strtmp1);
			xml += tmp;
		}
		xml += "</MPLAN_DONE_INFORM>";

		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM);
		TGSendStringMessage("MPLAN_DONE_INFORM", xml, eid);
		GpcLogTrainMessage(0, 0, "��TD(%d)����ȡ��ʩ�� PLANID %s", eid, pMMplan->GetMainPlanID());
	}
}

// TD����ʩ��ȡ������
void CMMPlanView::TDRetCancelMPlanWorkResult(const CString& planid, int nResult)
{
	if(planid.IsEmpty())
	{
		AfxMessageBox("ȡ��ʩ��ʧ��, ��Ϊ���ؼƻ���Ϊ��");
		return;
	}

	int nRet = GetDocument()->FindMMPlanByMainID(planid);
	if(nRet == -1)
		return;

	CMMPlanListInfo *pMMplan=GetDocument()->GetMPlanPtr(nRet); 
	if(!pMMplan)
		return;

	CString str;

	// ȡ��ʩ��ʧ��
	if(nResult == 0)
	{
		str.Format("��ע��, ��ղ�ȡ��ʩ��ִ��ʧ��, �ƻ��� %s, ������ȡ��", planid);
		GpcLogTrainMessage(0,0,"[ȡ��ʩ��] %s", str);
		AfxMessageBox(str);
		return;
	}
	else
	{
		str.Format("��ע��, ��ղ�ȡ��ʩ��ִ�гɹ�, ��ɾ���ƻ���%s��Ӧ����ͼ�ķ���", planid);
		GpcLogTrainMessage(0,0,"[ȡ��ʩ��] %s", str);

		pMMplan->m_nStatus=1; //����ʩ��ȡ�� ״̬
		GetDocument()->SaveMemMMPlanListToFile(); // ����״̬
		
		AfxMessageBox(str);
		OnMPlanRefresh();
	}

	GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,planid);
	GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,planid);

	return;
}

// TD����ʩ���ָ�����
void CMMPlanView::TDRetRecoverMPlanWorkResult(const CString& planid, int nResult)
{
	if(planid.IsEmpty())
	{
		AfxMessageBox("�ָ�ʩ��ʧ��, ��Ϊ���ؼƻ���Ϊ��");
		return;
	}

	int nRet = GetDocument()->FindMMPlanByMainID(planid);
	if(nRet == -1)
		return;

	CMMPlanListInfo *pMMplan=GetDocument()->GetMPlanPtr(nRet); 
	if(!pMMplan)
		return;

	CString str;

	// �ָ�ʩ��ʧ��
	if(nResult == 0)
	{
		str.Format("��ע��, ��ղ� �ָ�ʩ�� ִ��ʧ��, �ƻ��� %s, �����»ָ�", planid);
		GpcLogTrainMessage(0,0,"[�ָ�ʩ��] %s", str);
		AfxMessageBox(str);
		return;
	}
	else
	{
		str.Format("��ע��, ��ղ� �ָ�ʩ�� ִ�гɹ�, �ƻ���%s��Ӧ���Ž���ͼ", planid);
		GpcLogTrainMessage(0,0,"[�ָ�ʩ��] %s", str);

		pMMplan->m_nStatus=0; //������ͼ״̬
		GetDocument()->SaveMemMMPlanListToFile();// ����״̬
		
		AfxMessageBox(str);
		OnMPlanRefresh();
	}

	// 1. ɾ��
	GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,planid);
	GetDocument()->DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,planid);

	// 2. ����
	int s=0, f=0;
	if(GetDocument()->GenMarkFromMMPlanByPlanId(planid,0x83,s,f))
		str.Format("�ָ�ʩ��, �ƻ��� %s ��Ӧ��ʩ������, �ɹ���ͼ %d ��, ʧ�� %d ��", planid, s, f);
	else
		str.Format("�ָ�ʩ��, �ƻ��� %s û��ʩ������, ������ͼʧ��", planid);

	GpcLogTrainMessage(0,0,"%s", str);
	MessageBox(str,"��ʾ");
	return;
}

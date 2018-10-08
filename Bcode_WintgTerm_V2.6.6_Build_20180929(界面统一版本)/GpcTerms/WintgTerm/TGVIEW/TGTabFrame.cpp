// TGFrame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <afxpriv.h>
#include "resource.h"
#include "TGTabFrame.h"
#include "sysshare.h"
#include "tgview.h"
#include "TgEditView.h"
#include "function.h"
// CTgTabFrame

IMPLEMENT_DYNCREATE(CTgTabFrame, CMDIChildWnd)

CTgTabFrame::CTgTabFrame()
{
}

CTgTabFrame::~CTgTabFrame()
{
}


BEGIN_MESSAGE_MAP(CTgTabFrame, CMDIChildWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDC_SEND_STATION_PLAN, OnSendPlan)
	ON_COMMAND(IDC_RICEIVE_PLAN_1, OnRecieveNeighber)
	ON_COMMAND(ID_MANUAL_SELECT_BASIC_TRAIN, OnRecieveBasicPlan)
	ON_COMMAND(IDC_NEW_DC_COMMAND, OnNewDDML)
	ON_COMMAND(ID_DDML_MANAGE, OnDDMLQuery)
END_MESSAGE_MAP()

CView  *CTgTabFrame::GetBasicTGView()
{
	return  m_tabWnd.GetView(2);
}

CView  *CTgTabFrame::GetShiftTGView()
{
	return m_tabWnd.GetView(1);
}

CView  *CTgTabFrame::GetWorkTGView()
{
	return m_tabWnd.GetView(0);
}

BOOL  CTgTabFrame::InitScheduleView(int shift_id,const char *datapath)
{
	CTgView *pv;
	pv=(CTgView *)GetWorkTGView();
	if(pv)
       pv->InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,datapath);
	pv=(CTgView *)GetWorkTGView();
	if(pv)
		pv->InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,datapath);
	pv=(CTgView *)GetWorkTGView();
	if(pv)
		pv->InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,datapath);
	return TRUE;
}

// CTgTabFrame ��Ϣ�������
void CTgTabFrame::OnClose() 
{
    int m_nCurrentSectionNo=-1;
	CTgView *pv=(CTgView *)GetWorkTGView();
	if(pv)
		m_nCurrentSectionNo=pv->GetViewSectionNo();
	if(gpcdef.GetPicturesTip() > 0
		&& gpcdef.GetUserIdentify() == GPC_DISPATCHER
		&& gpcdef.GetLoginShiftid()%2 == 0 
		&& !gpcdef.CheckPicExist(m_nCurrentSectionNo))
	{
		//�ж�ʱ�䷶Χ�Ƿ��ڵ�½��ε�19:00-20:00
		time_t curt,startt,endt;
		time(&curt);
		startt = long_time(gpcdef.GetLoginDate(),19*3600);
		endt = long_time(gpcdef.GetLoginDate(),20*3600);
		if(curt>=startt && curt<=endt)
		{
			if(AfxMessageBox("����ͼ��û�б���ΪͼƬ����ȷ��Ҫ�˳���",MB_YESNO)==IDNO)
				return;
		}		
	}
	pv->CloseView();
	DestroyWindow();
	//CMDIChildWnd::OnClose();
}
BOOL CTgTabFrame::AddButtonToBar(CButton& btn,CString& str,UINT id,CRect &rect)
{
	if (!btn.Create(str,WS_CHILD | WS_VISIBLE ,rect, &m_wndToolBar, id))	 
		return FALSE;
	btn.SetFont(m_wndToolBar.GetFont());
	btn.SetWindowPos(&btn, rect.left,rect.top , 0, 0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	return TRUE;
}

int CTgTabFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
		if (!m_wndToolBar.Create(this,   WS_BORDER |WS_VISIBLE | CBRS_BOTTOM  )
			|| !m_wndToolBar.LoadToolBar(IDR_WINTG_BOTTOM))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}
		CRect temp;
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screenNum = gpcdef.GetScreenNum();
       screenx = screenx/screenNum;

		m_wndToolBar.GetItemRect(0,&temp);
		int width=(screenx-20)/20;
		if((width-8)>0&&(temp.Height()-7)>0)
			m_wndToolBar.SetSizes(CSize(width,temp.Height()),CSize(width-8,temp.Height()-7));
		
		for(int i=0;i<20;i++)
		{   
			CRect	rect;
			m_wndToolBar.GetItemRect(i, &rect);
			switch(i){
			case 0:
					AddButtonToBar(m_toolBarButtonWidth,CString("�Ŵ�"),IDC_WIDTH,rect);
					break;
			case 1:	
					AddButtonToBar(m_toolBarButtonSmall,CString("��С"),IDC_SMALL,rect);
					break;
			case 2:
					AddButtonToBar(m_toolBarButtonTurnReality,CString("תʵ��"),ID_TURN_REALITY,rect);
					break;
			case 3:
					AddButtonToBar(m_toolBarButtonSendStaPlan,CString("�¼ƻ�"),IDC_SEND_STATION_PLAN,rect);
					break;
			case 4:
					AddButtonToBar(m_toolBarButtonSendNeighber,CString("����̨"),IDC_SEND_NEIGHBER,rect);
					break;
			case 5:
					AddButtonToBar(m_toolBarButtonRecvPlan,CString("�ռƻ�"),IDC_RICEIVE_PLAN_1,rect);
					break;
			case 14:
					AddButtonToBar(m_toolBarButtonInsertNewTrn,CString("��ʼ����"),ID_INSERT_NEW_TRAIN,rect);
					break;
			case 15:
					AddButtonToBar(m_toolBarButtonDrawNewTrnEnd,CString("��������"),ID_DRAW_NEW_TRAIN_END,rect);
					break;
			case 16:
					AddButtonToBar(m_toolBarButtonNewDC,CString("��������"),IDC_NEW_DC_COMMAND,rect);
					break;
			case 17:
					AddButtonToBar(m_toolBarButtonDCManage,CString("�������"),ID_DDML_MANAGE,rect);
					break;
			case 18:
					AddButtonToBar(m_toolBarButtonBasicTrn,CString("ͼ���ƻ�"),ID_MANUAL_SELECT_BASIC_TRAIN,rect);
					break;
			case 19:
					AddButtonToBar(m_toolBarButtonNote,CString("ͼ��ע��"),ID_TEXT_NOTE,rect);
					break;
			default:
				break;
			}
			
			if(i==6)
			{
				//m_wndToolBar.SetButtonInfo(i, ID_DIRECT_UP, TBBS_SEPARATOR,72);
				
				if (!m_toolBarButtonUp.Create("����",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON  ,
					rect, &m_wndToolBar, ID_DIRECT_UP))	 
					return FALSE;
				m_toolBarButtonUp.SetFont(m_wndToolBar.GetFont());
				m_toolBarButtonUp.SetWindowPos(&m_toolBarButtonUp, rect.left,
					rect.top , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
				
			}
			else if(i==7)	
			{
				//m_wndToolBar.SetButtonInfo(i, ID_DIRECT_DOWN,TBBS_BUTTON, 72);
				if (!m_toolBarButtonDown.Create("����",WS_CHILD|WS_VISIBLE | BS_RADIOBUTTON  ,
					rect, &m_wndToolBar, ID_DIRECT_DOWN))	 
					return FALSE;
				m_toolBarButtonDown.SetFont(m_wndToolBar.GetFont());
				m_toolBarButtonDown.SetWindowPos(&m_toolBarButtonDown, rect.left,
					rect.top , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);			
			}
			else if(i==8)
			{
				//m_wndToolBar.SetButtonInfo(i, ID_DIRECT_MIXED,TBBS_BUTTON, 72);
				if (!m_toolBarButtonMixed.Create("������",WS_CHILD|WS_VISIBLE | BS_RADIOBUTTON  ,
					rect, &m_wndToolBar, ID_DIRECT_MIXED))	 
					return FALSE;
				m_toolBarButtonMixed.SetFont(m_wndToolBar.GetFont());
				m_toolBarButtonMixed.SetWindowPos(&m_toolBarButtonMixed, rect.left,
					rect.top  , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			else if(i==9)
			{
				if (!m_toolBarButtonPassenger.Create("����",WS_CHILD|WS_VISIBLE | BS_RADIOBUTTON   ,
					rect, &m_wndToolBar, ID_SHOW_PASSENGER))	 
					return FALSE;
				m_toolBarButtonPassenger.SetFont(m_wndToolBar.GetFont());	
				m_toolBarButtonPassenger.SetWindowPos(&m_toolBarButtonPassenger, rect.left,
					rect.top  , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			else if(i==10)
			{
				if (!m_toolBarButtonFreight.Create("�ͳ�",WS_CHILD|WS_VISIBLE | BS_RADIOBUTTON  ,
					rect, &m_wndToolBar, ID_SHOW_FREIGHT))	 
					return FALSE;
				m_toolBarButtonFreight.SetFont(m_wndToolBar.GetFont());		
				m_toolBarButtonFreight.SetWindowPos(&m_toolBarButtonFreight, rect.left,
					rect.top  , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			
			else if(i==11)
			{
				if (!m_toolBarButtonAllTrain.Create("�ͻ���",WS_CHILD|WS_VISIBLE | BS_RADIOBUTTON  ,
					rect, &m_wndToolBar, ID_SHOW_ALL_TRAIN))	 
					return FALSE;
				m_toolBarButtonAllTrain.SetFont(m_wndToolBar.GetFont());	
				m_toolBarButtonAllTrain.SetWindowPos(&m_toolBarButtonAllTrain, rect.left,
					rect.top  , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			else if(i==12)
			{
				if (!m_toolBarButtonReceivePoint.Create("����յ�",WS_CHILD|WS_VISIBLE | BS_CHECKBOX  ,
					rect, &m_wndToolBar, IDC_RECEIVE_POINT))	 
					return FALSE;
				m_toolBarButtonReceivePoint.SetFont(m_wndToolBar.GetFont());	
				m_toolBarButtonReceivePoint.SetWindowPos(&m_toolBarButtonReceivePoint, rect.left,
					rect.top , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			else if(i==13)
			{
				if (!m_toolBarButtonPointAdjust.Create("������",WS_CHILD|WS_VISIBLE | BS_CHECKBOX  ,
					rect, &m_wndToolBar, ID_SELFNEXTSTD))	 
					return FALSE;
				m_toolBarButtonPointAdjust.SetFont(m_wndToolBar.GetFont());	
				m_toolBarButtonPointAdjust.SetWindowPos(&m_toolBarButtonPointAdjust, rect.left,
					rect.top , 0, 0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);		
			}
			
		}
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() 
			| CBRS_TOOLTIPS |CBRS_BORDER_3D |CBRS_BORDER_LEFT);
		m_wndToolBar.ShowWindow(SW_SHOW);
		m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM ); //CBRS_ALIGN_ANY

		return CMDIChildWnd::OnCreate(lpCreateStruct);		
}

BOOL CTgTabFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	BOOL ret = m_tabWnd.Create(WS_VISIBLE|WS_CHILD, this, AFX_IDW_PANE_FIRST);
	if (!ret)
		return FALSE;
	ret = m_tabWnd.CreateView("ʵ������ͼ", RUNTIME_CLASS(CTgEditView), pContext);
	if (!ret)
		return FALSE;
	ret = m_tabWnd.CreateView("��ƻ�ͼ", RUNTIME_CLASS(CTgView), pContext);
	if (!ret)
		return FALSE;
	ret = m_tabWnd.CreateView("����ͼ", RUNTIME_CLASS(CTgView), pContext);
	if (!ret)
		return FALSE;

	return TRUE;	
}

void CTgTabFrame::OnSize(UINT nType,int cx,int cy)
{
m_tabWnd.PostMessage(WM_SIZEPARENT,0,0);
}

void CTgTabFrame::OnShowToolbar() 
{
	if(!m_wndToolBar.IsWindowVisible())
	{
		m_wndToolBar.ShowWindow(SW_SHOW);
		m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM); 
		EnableDocking(CBRS_ALIGN_BOTTOM); 
		DockControlBar(&m_wndToolBar);	
		
	}
	MDIMaximize();
}


void CTgTabFrame::ActivateFrame(int nCmdShow) 
{
	
	CFrameWnd::ActivateFrame(nCmdShow);	
}

void CTgTabFrame::OnNewDDML()
{
	AfxMessageBox("�½���������û��ʵ��");
}

void CTgTabFrame::OnDDMLQuery()
{
	AfxMessageBox("�����������û��ʵ��");
}

void CTgTabFrame::OnSendPlan()
{
	AfxMessageBox("���ƻ�û��ʵ��");
}


void CTgTabFrame::OnRecieveNeighber()
{
	AfxMessageBox("����̨�ƻ�û��ʵ��");
}

void CTgTabFrame::OnRecieveBasicPlan()
{
	AfxMessageBox("������ͼ�ƻ�û��ʵ��");
}

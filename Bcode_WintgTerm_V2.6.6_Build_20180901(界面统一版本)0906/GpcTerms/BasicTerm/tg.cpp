// tg.cpp : implementation file
//

#include "stdafx.h"

#include <stdlib.h>

#include <io.h>
#include "oscalls.h"
#include "sysshare.h"
#include "resource.h"
#include "tg.h"
#include "winspool.h"

#include "plotdll/plot24doc.h"
#include "plotdll/plot24view.h"
#include "plotdll/plotframe.h"

#include "mainframe.h"
#include "icommtask.h"
#include "logindlg.h"
#include "dlgwait.h"
#include "db_msgproc.h"
#include "gpcdef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////NEWTG ///////////////////////
#include "tgview.h"
#include "tgframe.h"
#include "tgeditview.h"
#include "basictgframe.h"
#include "basic_schd_config.h"
///////////////////////////////////////////////
#include "utility/mmi_function_config.h"

extern BOOL UR_CanOperateSchedule();
extern bool TGMessageProcessNew(long entity_id,unsigned char *ptr,int length);
extern bool NetworkEntityStatusProcessNew(int evt_type,long entity_id, int status);
extern void SetAuthFilePath(const char *path);

UINT g_nManagerInformMSG;

BOOL ReqBasicName_ToServer(int con_id, bool bload);

static basic_schd_config glSectionBasic;
basic_schd_config* GetBasicConfig()
{
	return &glSectionBasic;
}
static CMMIFunctionConfig g_mmi_config;
CMMIFunctionConfig *GetMMIFunctionConfig()
{
	return &g_mmi_config;
}

IMPLEMENT_DYNCREATE(CWintg, CWinApp)
CWintg::CWintg( )
{
	m_pDocManager = NULL;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}
////////////////////////////////////////////
CWintg  theWintgApp;
CGPCContext m_gpcContext;
////////////////////////////////////////////
CWintg::~CWintg( )
{
	bWinTGExit=false;
}

/////////////////////////////////////////////
BOOL CWintg::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	CoInitialize(NULL);

	InitCommonControls();

	CWinApp::InitInstance();
	AfxInitRichEdit();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())  
	{
		AfxMessageBox("AfxOleInit failed!");
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("WinTG V1.0"));

	ParseCommandLine(m_cmdline);

	///////��ʼ�� GPC.DEF �͵�ǰ�û�����
	CString gpcPath = "D:\\CTC_GPC_TD";
	if(!m_cmdline.m_datapath.IsEmpty())   
		gpcPath = m_cmdline.m_datapath;

	gpcdef.SetGPCWorkDir(gpcPath.GetBuffer());
	// ������ȡ��ͬentity.cfg����. 20161012
	if(m_cmdline.m_entityfile=="")
		m_cmdline.m_entityfile="entity.cfg";
	if(!gpcdef.WintgTermLoadProfile(m_cmdline.m_entityfile))
	{
		return FALSE;
	}

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PLOT,
		RUNTIME_CLASS(CPlot24Doc),
		RUNTIME_CLASS(CPlotFrame), // custom MDI child frame
		RUNTIME_CLASS(CPlot24View));

	AddDocTemplate(pDocTemplate);
	m_pWintgPlotFormDoc=pDocTemplate;
	if(gpcdef.IsFormatView())
	{
		pDocTemplate = new CMultiDocTemplate(
			IDR_FORMAT_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgView));

		AddDocTemplate(pDocTemplate);
		m_pTemplateBasic=pDocTemplate;

		pDocTemplate = new CMultiDocTemplate(
			IDR_FORMAT_PRE_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CBasicTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgEditView));

		AddDocTemplate(pDocTemplate);
		m_pTemplatePreBasic=pDocTemplate;

		pDocTemplate = new CMultiDocTemplate(
			IDR_FORMAT_BACK_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgView));

		AddDocTemplate(pDocTemplate);
		m_pTemplateBackBasic=pDocTemplate;
	}
	else
	{
		pDocTemplate = new CMultiDocTemplate(
			IDR_TG_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgView));

		AddDocTemplate(pDocTemplate);
		m_pTemplateBasic=pDocTemplate;

		pDocTemplate = new CMultiDocTemplate(
			IDR_TG_PRE_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CBasicTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgEditView));

		AddDocTemplate(pDocTemplate);
		m_pTemplatePreBasic=pDocTemplate;

		pDocTemplate = new CMultiDocTemplate(
			IDR_TG_BACK_BASIC_SCHEDULE,
			RUNTIME_CLASS(CTgDocument),
			RUNTIME_CLASS(CTgFrame), // custom MDI child frame
			RUNTIME_CLASS(CTgView));

		AddDocTemplate(pDocTemplate);
		m_pTemplateBackBasic=pDocTemplate;
	}
	int theStatus = 1;
	CTime tm = CTime::GetCurrentTime();
	m_gpcContext.mTmRun = tm.GetTime();
	do 
	{
		SetCommServerMode(TRUE);
		if(!InitTask())
			break;

		// The main window has been initialized, so show and update it.
		CMainFrame *pWintgMainFrame = new CMainFrame;
		if (!pWintgMainFrame->LoadFrame(IDR_TGMAIN))
			break;
		m_pMainWnd = pWintgMainFrame;
		m_pMainWnd->ShowWindow(SW_MAXIMIZE);
		m_pMainWnd->UpdateWindow();

		CreateScheduleWindow(m_gpcContext.mSectionId,CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,TRUE);
		
		g_nManagerInformMSG=RegisterWindowMessage("CTC-GPC-BASIC-INFORM-MSG");
		theStatus = 0;
	}while(0);
	if(theStatus)
	{
		EndComTask();
		return FALSE;
	}
	return TRUE;
}

int CWintg::ExitInstance( )
{
	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWintg, CWinApp)
	//{{AFX_MSG_MAP(CWintg)
		// NOTE - the ClassWizard will add and remove mapping macros here.

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWintg message handlers
void CWintg::AddDocTemplate(CDocTemplate * pTemplate)
{
	CWinApp::AddDocTemplate(pTemplate);
}

int CWintg::GetOpenDocumentCount()
{
	ASSERT(m_pDocManager != NULL);
	return m_pDocManager->GetOpenDocumentCount();
}

POSITION CWintg::GetFirstDocTemplatePosition() const
{
	if (m_pDocManager == NULL)
		return NULL;
	return m_pDocManager->GetFirstDocTemplatePosition();
}

CDocTemplate * CWintg::GetNextDocTemplate(POSITION & rPosition) const
{
	ASSERT(m_pDocManager != NULL);
	return m_pDocManager->GetNextDocTemplate(rPosition);
}

int CWintg::OnFileNew()
{
	return 0;
}

static int g_delay_couter=0;
BOOL CWintg::PumpMessage( )
{
	if(g_delay_couter++>10) // 2010.2.26 ��Ҫ: ����ÿ�ζ����ã��������ܿ���
	{
		DB_PollingData();
		g_delay_couter=0;
	}
	return CWinApp::PumpMessage();
}

BOOL CWintg::OnIdle(LONG lCount) 
{
	DB_PollingData();  // �������ݸ���
	return CWinApp::OnIdle(lCount);
}

BOOL CWintg::IsIdleMessage(MSG* pMsg) 
{
	if (!CWinApp::IsIdleMessage(pMsg) || 
		pMsg->message == WM_TIMER ||
		pMsg->message == WM_MOUSEMOVE)
		return FALSE;
	else
		return TRUE;
}

int CWintg::Run() 
{
	return CWinApp::Run();
}

bool CWintg::InitTask()
{
	do
	{
		char sdatapath[256];
		
		CreateDirectory(gpcdef.GetLogDir(),NULL); // 2008.11.24 ����logĿ¼
		CreateDirectory(gpcdef.GetXMLDir(),NULL); // 2008.11.24 ����logĿ¼
		CString str_s=gpcdef.GetDataDir();
		str_s=str_s+"\\user_cfg";
		CreateDirectory(str_s,NULL); // 2018.05.14 ����user_cfgĿ¼
		SetSysPrintLogMask("wintg",m_cmdline.m_nLogLevel,m_cmdline.m_nLogTypeMask);

		CString gpcPath = gpcdef.GetWorkPathName();

		LoadTGNOXML(gpcPath+"\\data\\TGNO.xml");
		if(m_cmdline.m_gpcname.IsEmpty())   
 			gpcdef.SetDefaultUser();
		else
			gpcdef.SetDefaultUser(m_cmdline.m_gpcname);
			
		// gpcdef.CompleteWintgTermUserInfo, ������gpcdef.SetDefaultUser֮�����
		gpcdef.CompleteWintgTermUserInfo();

		m_gpcContext.mSectionId=gpcdef.GetDefaultSection();
		//////////// ��ʼ��ͨ������ ////////////////
		
		GPCENTITY entityInfo;
		if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
		{
			AfxMessageBox("�����ļ�����:�Ҳ������û���ENTITY!");
			return false;
		}

		if(m_cmdline.m_commcfgfile.IsEmpty())   
		{
			sprintf(sdatapath,"%s\\commcfg\\commlc.cfg",gpcdef.GetDataDir()); // 2008.10.22
		}
		else
		{
			sprintf(sdatapath,"%s\\commcfg\\%s",gpcdef.GetDataDir(),m_cmdline.m_commcfgfile); // 2008.10.22
		}
		if(m_cmdline.m_server_entity_id>0)
			SetDefaultServerEntityID(m_cmdline.m_server_entity_id);
				
	    if(!RegisterDirectMsgHandler(TGMessageProcessNew))
		{
			AfxMessageBox("RegisterDirectMsgHandler ʧ��, ϵͳ�˳�",MB_OK);
			break;
		}
		
		CString szAppID;
		szAppID.Format("CASCO_BASICTERM_CLIENT_%08x_%08x", entityInfo.nEntityId, entityInfo.nEntityId);
		if(!ProgramOnce(szAppID, FALSE))
		{
			return false;
		}

		if(!RegisterCommEventHandler(NetworkEntityStatusProcessNew))
		{
			AfxMessageBox("RegisterCommEventHandler ʧ��, ϵͳ�˳�",MB_OK);
			break;
		}
		gpcdef.SetThisEntity(entityInfo.nEntityId);
		if(!InitComTask(entityInfo.nEntityId, sdatapath, 0))
		{
			AfxMessageBox("ͨ�������ʼ��ʧ��, ϵͳ�˳�",MB_OK);
			GpcLogSysMessage(0,"ͨ�������ʼ��ʧ��, ϵͳ�˳�");
			break;
		}
		else
		{
			GpcLogSysMessage(0,"ͨ�������ʼ���ɹ�");
		}

		if (gpcdef.IsTgMsgToGateway() || gpcdef.IsOtherMsgToGateway())
		{
			if (-1==GetSpecialEntityID(SERVER_TYPE_GATEWAY))
			{
				AfxMessageBox("entity.cfg��δ����Gateway��ʵ���, ϵͳ�˳�",MB_OK);
				break;;
			}
		}

		//// ������֤�����ļ�·��
		sprintf(sdatapath,"%s\\auth.cfg",gpcdef.GetDataDir()); // 2008.10.22
		SetAuthFilePath(sdatapath);

		GpcLogSysMessage(0,"WintgTerm ��������, ��ǰʵ���: %d.", entityInfo.nEntityId);
		/////// ��ʼ���߼�����
		CString tg_cfgfile;
		CDlgWait *pw=new CDlgWait("����ϵͳ��ʼ��,������Ҫ1-2����ʱ��,���Ժ�...");

		if(!DB_Init(gpcPath))
		{
			delete pw;
			AfxMessageBox("DBInit ʧ��, ϵͳ�˳�",MB_OK);
			break;
		}

		tg_cfgfile.Format("%s/default.cfg",gpcdef.GetDataDir());
		if(!g_mmi_config.InitData(tg_cfgfile))
		{
			tg_cfgfile.Format("%s/default_public.cfg",gpcdef.GetDataDir());
			g_mmi_config.InitData(tg_cfgfile);
		}
		
		GpcLogSysMessage(0,"��¼�ɹ�.");
		int section_id=m_gpcContext.mSectionId;
		CTgDocument	*pdoc=CTgDocument::CreateTgDocument(section_id);
		delete pw;
		
		return true;
	}while(0);
	return false;
}

// ��ӡ����ͼ
void CWintg::PlotSchedule(CTgDocument *pdoc,int sche_type,const CString& plot_param, const int plotflag)
{
	CTG_ScheduleInfo info;
	CTG_TrainScheduleBase *pdata = pdoc->GetScheduleData((CTG_ScheduleInfo::SCHEDULE_TYPE)sche_type); 

	pdata->GetScheduleInfo(info);
	int id = pdoc->GetCurrentShiftID();
	do 
	{
		int nValue,nValue2;
		PRINTINFORM sPrintInform;
		if(((CWintg *)AfxGetApp())->IsPlotWindowOpened() == TRUE)
		{
			AfxMessageBox("���ȹرմ�ӡ�����ٽ����µĴ�ӡ����!");
			break;
		}
		int printForm = gpcdef.GetPrintFormat();; //12Сʱ��ʽ,0Ϊ24Сʱ��ʽ
		long m_nShiftDate = (id/1000000 - 1990) * 380 + (id/10000-(id/1000000)*100 - 1) * 31 + id/100 - (id/10000)*100  - 1 ;

		sPrintInform.sBasicFlag=0;
		if(plotflag==1) // ����ͼ 18-06
		{
			sPrintInform.sBasicFlag=1;
			nValue = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),18*3600);
			nValue2 = long_time(m_nShiftDate,6*3600);
		}
		else if(plotflag==2) // ����ͼ 06-18
		{
			sPrintInform.sBasicFlag=2;
			nValue = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),6*3600);
			nValue2 = long_time(m_nShiftDate,6*3600);
		}
		else
		{
			if(printForm == 1)
			{	
				if(info.GetShiftNo() == 2)//6:00-18:00
				{
					nValue = long_time(m_nShiftDate,6*3600);  
					nValue2 = long_time(m_nShiftDate,18*3600);				
				}
				else//18:00-6:00
				{
					nValue = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),18*3600);
					nValue2 = long_time(m_nShiftDate,6*3600);					
				}
			}
			else
			{		
				nValue = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),18*3600);
				nValue2 = long_time(m_nShiftDate,18*3600);			
			}
		}
		sPrintInform.usSectionNo = info.m_nSectionID;  
		sPrintInform.usSchdlType = info.m_nType;             
		sPrintInform.usShiftid = info.m_nShiftID;     

		long usStartDate,usStartTime,usEndDate,usEndTime;
		usStartTime = minute_time(nValue,&usStartDate);
		
		usEndTime = minute_time(nValue2,&usEndDate);
		
		sPrintInform.usStartDate = usStartDate;
		sPrintInform.usStartTime = usStartTime;;
		sPrintInform.usEndDate = usEndDate;
		sPrintInform.usEndTime = usEndTime;
		OpenPlotWindow(&sPrintInform,pdoc,plot_param);
	}while(0);
}
//
bool CWintg::OpenPlotWindow(PRINTINFORM *pPrintInform, CTgDocument *pdoc,const CString& plot_param)
{
	CFrameWnd  *fw;
    CPlot24Doc *plotdoc=(CPlot24Doc *)m_pWintgPlotFormDoc->CreateNewDocument();
	plotdoc->ParsePlotParam(plot_param);
    if(plotdoc->InitData(pPrintInform,pdoc))
	{
		fw=m_pWintgPlotFormDoc->CreateNewFrame(plotdoc,NULL);
		if(!fw) return false;
		fw->InitialUpdateFrame(plotdoc,TRUE);
		
		fw->ShowWindow(SW_SHOWMAXIMIZED);
	}
	return true;
}
extern CView *clsPlotView;

CView *CWintg::GetOpenedPlotWindow()
{
   return  clsPlotView;
}

bool CWintg::IsPlotWindowOpened()
{
	if(clsPlotView && ::IsWindow(clsPlotView->m_hWnd))
		return true;
	else
		return false;
}

bool CWintg::SendMessageToPlotWindow(UINT msg,WPARAM wp,LPARAM lp)
{
  if(clsPlotView && ::IsWindow(clsPlotView->m_hWnd))
  {
    	clsPlotView->SendMessage(msg,wp,lp);
		return true;
  }
  else
	  return false;
}
BOOL CWintg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==g_nManagerInformMSG  && pMsg->wParam==1)
	{
	    if(m_pMainWnd)
			m_pMainWnd->DestroyWindow(); 
		return FALSE;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

void  CWintg::CloseBackupBasicFrame(int nSectionId)
{
	CMDIFrameWnd* pFrameWnd=(CMDIFrameWnd*)GetMainWnd();
    CWnd* pActiveChildWnd=pFrameWnd->MDIGetActive(NULL);
    CMDIChildWnd* pChildWnd=NULL;
    if(pActiveChildWnd!=NULL)
		pChildWnd=(CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);
	else
		return;
	
	std::vector<CFrameWnd*> frame;
	while(pChildWnd!=NULL)
	{
		CView *pv=pChildWnd->GetActiveView();
		if(pv && pv->IsKindOf(RUNTIME_CLASS(CTgView)))
		{
			CTG_ScheduleInfo schdinfo;
			((CTgView *)pv)->GetViewScheduleInfo(schdinfo);
			if((schdinfo.m_nSectionID==nSectionId)&&(schdinfo.m_nType==schdinfo.SCHEDULE_TYPE_BASIC_BACKUP))
			{
				frame.push_back(pv->GetParentFrame());
			}
		}
			
		pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
	}

	for(int i=0; i<frame.size(); i++)
	{
		frame[i]->SendMessage(WM_CLOSE); 
	}
}

void  CWintg::CloseSectionIdFrame(int nSectionId)
{
	CMDIFrameWnd* pFrameWnd=(CMDIFrameWnd*)GetMainWnd();
    CWnd* pActiveChildWnd=pFrameWnd->MDIGetActive(NULL);
    CMDIChildWnd* pChildWnd=NULL;
    if(pActiveChildWnd!=NULL)
		pChildWnd=(CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);
	else
		return;
	
	CFrameWnd* pMMPlanFrame=NULL;
	std::vector<CFrameWnd*> frame;
	while(pChildWnd!=NULL)
	{
		CView *pv=pChildWnd->GetActiveView();
		if(pv && pv->IsKindOf(RUNTIME_CLASS(CTgView)))
		{
			CTG_ScheduleInfo schdinfo;
			((CTgView *)pv)->GetViewScheduleInfo(schdinfo);
			if(schdinfo.m_nSectionID== nSectionId)
			{
				frame.push_back(pv->GetParentFrame());
			}
		}
			
		pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
	}

	if(pMMPlanFrame)
	{
		pMMPlanFrame->SendMessage(WM_CLOSE); 
	}

	for(int i=0; i<frame.size(); i++)
	{
		frame[i]->SendMessage(WM_CLOSE); 
	}

	return;
}

CFrameWnd * CWintg::FindScheduleWindow(int section_id,int  schd_type, int& nFlag)
{
	nFlag = 0;
	CMDIFrameWnd* pFrameWnd=(CMDIFrameWnd*)GetMainWnd();
    CWnd* pActiveChildWnd=pFrameWnd->MDIGetActive(NULL);
    CMDIChildWnd* pChildWnd=NULL;
    if(pActiveChildWnd!=NULL)
		pChildWnd=(CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);
	else
		return NULL;
	while(pChildWnd!=NULL)
	{
		CView *pv=pChildWnd->GetActiveView();
		if(pv && pv->IsKindOf(RUNTIME_CLASS(CTgView)))
		{
			CTG_ScheduleInfo schdinfo;
			((CTgView *)pv)->GetViewScheduleInfo(schdinfo);
			if(schdinfo.m_nType==schd_type &&  schdinfo.m_nSectionID== section_id)
			{
				nFlag = 1;
				return pv->GetParentFrame(); 
			}
		}
		
		pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
	}
	return NULL;
}

BOOL CWintg::CreateScheduleWindow(int section_id,int schd_type, BOOL can_close)
{
	int nFlag = 0;
	CFrameWnd *pf=FindScheduleWindow(section_id, schd_type, nFlag);
	if(NULL != pf)
	{
		if(nFlag == 1)
		{
			pf->ActivateFrame();
			return TRUE;
		}
		else
		{
			pf->SendMessage(WM_CLOSE);
			pf=NULL;
		}
	}

	BOOL query_mode=TRUE;
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		query_mode=FALSE;
	
	CString datapath;
	datapath=gpcdef.GetDataDir();
	CTgDocument	*pdoc=CTgDocument::CreateTgDocument(section_id);
	if(!pdoc)
	{
		AfxMessageBox("�޷����������ĵ�!");
		return FALSE;
	}
	if(!query_mode)	
		pdoc->SetQueryMode(FALSE); 

	CMultiDocTemplate *pDocTemp;
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		pDocTemp=m_pTemplatePreBasic;
	else if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		pDocTemp=m_pTemplateBackBasic;
	else
		pDocTemp=m_pTemplateBasic;
	
	pf=pDocTemp->CreateNewFrame(pdoc,NULL); 
	if(!pf)
	{
		AfxMessageBox("�޷�������ͼ���!");
		return FALSE;
	}
	pf->InitialUpdateFrame(pdoc,TRUE);
	pf->ActivateFrame(SW_MAXIMIZE);

	CTgView *pv=(CTgView *)pf->GetActiveView();
	if(!pv)
	{
		AfxMessageBox("�޷�������ͼ!");
		return FALSE;
	}
	pv->InitScheduleView((CTG_ScheduleInfo::SCHEDULE_TYPE)schd_type,datapath,query_mode);
	pv->SetCanClose(TRUE); 
	((CTgFrame *)pf)->SetFrameType(0);

	return TRUE;
}

BOOL  ReqBasicName_ToServer(int con_id, bool bload)
{
	GetBasicConfig()->InitBasicSchdName(con_id);
	MsgDataStyleReq2 msg;
	msg.nStatCount= 1;
	msg.nStno[0]=con_id; 
	msg.nStyle[0]=MsgDataStyleReq2::SCHEDULE_NAME;
	if(bload)
		msg.nStatus[0]= MsgDataStyleReq2::LOAD; 
	else
		msg.nStatus[0]= MsgDataStyleReq2::CLOSE; 
			
	msg.nStartTime =0;
	msg.nEndTime = 0;

	long nServerEntityID = GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0);

	GpcLogTrainMessage(0, 0, "�����ͼ������:%d, �������ͼ����(MsgDataStyleReq2) con_id %d, style %0.4X load(1:Close 0:Open) %d nStartTime %d nEndTime %d",
					nServerEntityID, msg.nStno[0], msg.nStyle[0], msg.nStatus[0], msg.nStartTime, msg.nEndTime);

	if(SendBMsg(&msg, nServerEntityID,0))
		return TRUE;
	else
		return FALSE;
}

bool CWintg::ClearTGOCXSchedule(int section_id,int  schd_type)
{
	CMDIFrameWnd* pFrameWnd=(CMDIFrameWnd*)GetMainWnd();
    CWnd* pActiveChildWnd=pFrameWnd->MDIGetActive(NULL);
    CMDIChildWnd* pChildWnd=NULL;
    if(pActiveChildWnd!=NULL)
		pChildWnd=(CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);
	else
		return false;
	while(pChildWnd!=NULL)
	{
		CView *pv=pChildWnd->GetActiveView();
		if(pv && pv->IsKindOf(RUNTIME_CLASS(CTgView)))
		{
			CTgView* pTgView = (CTgView *)pv;
			CTG_ScheduleInfo schdinfo;
			pTgView->GetViewScheduleInfo(schdinfo);
			if(schdinfo.m_nType==schd_type &&  schdinfo.m_nSectionID== section_id)
			{
				GpcLogTrainMessage(0, 0, "���OCX�� con_id %d schd_type %d����",section_id, schd_type);
					
				pTgView->BeginLoad();
				pTgView->EndLoad();
				pTgView->SetTitleUpdateTimer();
				return true; 
			}
		}
	
		pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
	}
	return false;
}

#import <msxml.dll> named_guids
using namespace std;
using namespace MSXML;

BOOL CWintg::LoadTGNOXML(CString pathname)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("����%s�����ڻ��߲�����xml�﷨(���������������Ƿ��ܴ�)", pathname);
		AfxMessageBox(prompt);
		
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "GPC_TG_DEFINE");
	if(pRootNode == NULL)
		return false;
  
    // TABLE
	MSXML::IXMLDOMNodeListPtr list;
	
	// RouteTrigFlag 
	pNode = pRootNode->selectSingleNode("RouteTrigFlag");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int m_set = atoi(CString(v.bstrVal));
			gpcdef.SetShowRouteTrigFlag(m_set);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// RouteOkMark 
	pNode = pRootNode->selectSingleNode("RouteOkMark");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			int m_set = atoi(CString(v.bstrVal));
			gpcdef.SetShowRouteOkMark(m_set);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	return true;
}

// ������������ͼʱ����Ψһʵ���
// szAppID Ψһ��ʶ
// szAppID.Format("CASCO_LAYOUT_TERM_CLIENT_%08x_%08x", LocalEntityID, ServerEntityID);
//
// bForceStartup ǿ��������ԭ�����˳�

UINT REGIST_MSG_PROGRAMONCE = 0;
BOOL CWintg::ProgramOnce(LPCSTR szAppID, BOOL bForceStartup)
{
	HWND hDesktopWindow = ::GetDesktopWindow();

	//�жϸ�Client�Ƿ�������
	startup.szName = szAppID;
	REGIST_MSG_PROGRAMONCE = RegisterWindowMessage(startup.szName);

	//ͨ��Ψһ�ź����б��ն��Ƿ��Ѿ�����
	startup.hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, startup.szName);
	if(NULL == startup.hSemaphore)
	{
		startup.hSemaphore = CreateSemaphore(NULL, 1, 1, startup.szName);
		if(NULL == startup.hSemaphore)
		{
			sysprint(100, 0, "[OS_ERROR] ����ϵͳ�޷������ź��� %s", startup.szName);
			return FALSE;
		}
		sysprint(0, 0, "[INFO] WingtgTerm(%s) �ն�׼������", startup.szName);
	}
	else
	{
		sysprint(0, 0, "[INFO] WingtgTerm(%s) �ն�������, ���ܶ������", startup.szName);

		//���洰���еǼ� �ն˴��ڵľ��
		if(hDesktopWindow)
		{
			HWND hClient = (HWND)::GetProp(hDesktopWindow, startup.szName);
			if(::IsWindow(hClient))
			{
				if(bForceStartup)
				{
					sysprint(0, 0, "[INFO] �ն�����ǿ������");

					BOOL bSuccess = ::SendMessage(hClient, REGIST_MSG_PROGRAMONCE, 1, 0);	//1�˳�
					if(bSuccess)
						return TRUE;
					else
						return FALSE;
				}
				else
				{
					sysprint(0, 0, "[INFO] �ն��˳�");
					::SendMessage(hClient, REGIST_MSG_PROGRAMONCE, 0, 0);

					//::BringWindowToTop(hClient);
					return FALSE;
				}
			}
			else
			{	//ԭ�ն��˳�ʱδע��
				::RemoveProp(hDesktopWindow, startup.szName);
			}
		}

		//û�����洰�ڣ�����TopWindow�����ڷ�����Ϣ
		sysprint(0, 0, "[INFO] ������TopWindow���ڷ���REGIST_MSG_PROGRAMONCE��Ϣ");
		if(bForceStartup)
		{	//ԭ�����˳�
			BOOL bSuccess = ::SendMessage(HWND_BROADCAST, REGIST_MSG_PROGRAMONCE, 1, 0);	//1�˳�
			if(bSuccess)
				return TRUE;
			else
				return FALSE;
		}
		else
		{	//ԭ����ԭ
			::SendMessage(HWND_BROADCAST, REGIST_MSG_PROGRAMONCE, 0, 0);
			return FALSE;
		}
	}

	return TRUE;
}
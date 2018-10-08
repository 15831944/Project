// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
#include "LoginDlg.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "tgmsg.h"      /***�Զ�����Ϣ ��UM_LOGIN_MSG  **/ 
#include <BaseMsg/clbth.h>
#include <BaseMsg/gpc_msgid.h>
#include "dynCertificate.h"
#include "icommtask.h"
#include "td/td_frame.h"
#include ".\logindlg.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const COLORREF g_crGray=RGB(192,192,192);
const COLORREF g_crRed=RGB(255,0,255);
extern void SetCurrentLoginInfo(const char *userid,const char *password,int shiftid); // defined in pubfun.cpp

typedef int ( *pFPIFea_IP)(const char* ipaddrpt, BYTE *psTZ,  int* IpLength,int nTF);
typedef int ( *pFPIFeature_FDR)(int nPortNo, unsigned char *psTZ, int* lpLength);
typedef int ( *pRemoteVerifyFp)(char* addr, const int port, char* userName,unsigned char* fingerInfo);
typedef int (__stdcall *pGiOtp_Auth)(const char* sHost,const int iPort, const char* sAppID, const char* sUserID, const char* sAuthCode,char** sMsg); 

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog
const int timeid = 86418;
CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_consoleid = _T("");
	m_userid = _T("");
	m_username = _T("");
	m_password = _T("");
	m_date = _T("");
	m_banbie = _T("");
	m_username3 = _T("");
	m_checkgenban = FALSE;
	//}}AFX_DATA_INIT
	m_nAuthType=0;
	m_fixToken="";
	m_nPreSchdReqNum=0;
	m_bCtrlLoginBanci=false;
	m_nMaxPreNum = 2;
	m_hGiOtpAPIDLL=NULL;
	m_hFpClientDLL=NULL;
	m_hJHDLLCUDLL=NULL;
	m_isLocalGetData = 0 ;
	m_UsbPort = -1 ;
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Control(pDX, IDC_USERNAME32, m_username3Ctrl);
	DDX_Control(pDX, IDC_BANBIE, m_BanbieCtrl);
	DDX_Control(pDX, IDC_DATE, m_DateCtrl);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordCtrl);
	DDX_Control(pDX, IDC_USERNAME, m_UsernameCtrl);
	DDX_Control(pDX, IDC_USERID, m_UseridCtrl);
	DDX_Text(pDX, IDC_CONSOLEID, m_consoleid);
	DDX_Text(pDX, IDC_USERID, m_userid);
	DDV_MaxChars(pDX, m_userid, 10);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 10);
	DDX_Text(pDX, IDC_DATE, m_date);
	DDX_Text(pDX, IDC_BANBIE, m_banbie);	
	DDX_Text(pDX, IDC_USERNAME32, m_username3);	
	DDV_MaxChars(pDX, m_username3, 10);
	DDX_Check(pDX, IDC_CHECK_GENBAN2, m_checkgenban);
	DDX_Control(pDX, IDC_CHECK_AUTOREFRESH, m_checkbtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_CHECK_GENBAN2, OnCheckgenban)
	ON_BN_CLICKED(IDOK, OnButtonEnter)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_CHECK_AUTOREFRESH, OnBnClickedCheckLogin)
	ON_MESSAGE(UM_UNI_MESSAGE, OnMsgGpcAccessReply)
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers
void LoginWindowSetAutoReloginEnabled(bool flag);
void CLoginDlg::OnButtonEnter() 
{   	
	int theStatus = 1;
	UpdateData(TRUE);

	if(m_password.GetLength()>0)
	{
		// ��������
		if((m_userid=="casco") && (m_password=="casco" || m_password=="CASCO"))
		{
			m_username="CASCO";
			GpcLogTrainMessage(0,0,"[LOGIN]:ʹ�� CASCO");
			LoginOK();
			return;
		}


		if(m_nAuthType == 0) // CTC������ȷ������
		{
			SendLoginInfo(m_userid,m_strCtcPwd);
			GpcLogTrainMessage(0,0,"[LOGIN]:ȷ��CTC��������֤ %s %s", m_userid, m_strCtcPwd);
		}
		else if(m_nAuthType == 1) // �����ڲ���֤���ɹ�,�Ŵﻷ����֤
		{
			if(m_strCtcPwd == m_password) // ��������������==�˹���������
			{
				GpcLogTrainMessage(0,0,"[LOGIN]:ʹ�� CTC���� ��֤ͨ��,����ϵͳ");
				LoginOK();	//��֤ͨ��������ϵͳ
			}
			else
			{
				bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(),m_password.GetBuffer());  // ���͸������֤������
				if(m_bcfvsr_status)
				{
					GpcLogTrainMessage(0,0,"[LOGIN]:ʹ�������֤��������֤ͨ��,����ϵͳ");
					LoginOK();	//��֤ͨ��������ϵͳ
				}
			}
		}
		else if(m_nAuthType == 2) // ��ȫ�����ڲ���֤
		{
			SendLoginInfo(m_userid,m_password);
			GpcLogTrainMessage(0,0,"[LOGIN]:��ȫ��CTC��������֤ %s %s", m_userid, m_password);
		}
		else if(m_nAuthType == 3) // ��ȫ�Ŵﻷ����֤
		{
			bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(),m_password.GetBuffer());  // ���͸������֤������
			if(m_bcfvsr_status)
			{
				SendLoginInfo(m_userid,m_strCtcPwd);
				GpcLogTrainMessage(0,0,"[LOGIN]:��ȫ���Ŵﻷ���������֤ %s %s %s", m_userid, m_strCtcPwd, m_password);
			}
		}
		else
		{
			CString text;
			text.Format("��ע��, ���ն��û���gpc.def��[USER]�е�½��֤�����ô���, �Ϸ�ֵ0,1,2,3");
			AfxMessageBox(text);
			GpcLogTrainMessage(0,0,"%s", text);
		}
	}
	else
	{
		if(m_bIgnoreLoginTm)
		{
			CString text="��ע��,����԰���ص���½,��ȷʵҪ��ô����?\n��������벻Ҫѡ��\"���԰��(6��-8���18��-20��)�ص�\"";
			if(MessageBox(text,"��ʾ",MB_YESNO|MB_ICONQUESTION)==IDNO)
			{
				return;
			}
		}

		m_userid.TrimLeft();
		if(m_userid.GetLength() == 0)
		{    
			AfxMessageBox("��������룡",MB_OK);
			m_UseridCtrl.SetFocus( );                     
		}
		else
		{
			SendGpcAccessInfo(CLBTH::MsgGpcAccessInfo::WANT_PWD,m_userid);

			if(!strcmp(m_userid,"casco"))
			{
				m_PasswordCtrl.EnableWindow(TRUE); 
                m_PasswordCtrl.SetFocus();
			}
			// ���´������ʹ��
			/*CLBTH::MsgGpcAccessReply reply;
			reply.status=CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_OK;
			strcpy(reply.pwd, "12345");
			strcpy(reply.user_name, "������");

			UNI_MESSAGE_STRUCT  lParam;
			lParam.source_id=10001;
			lParam.msg_id=0;
			lParam.pmsg=&reply;
			OnMsgGpcAccessReply(0, (LPARAM)&lParam);*/
		}
	}
}

BOOL CLoginDlg::OnInitDialog() 
{   
	CDialog::OnInitDialog();
	CoInitialize(NULL);
	CTime t=CTime::GetCurrentTime(); 
	m_nShiftID=Get_CurrentShiftid();

	CString DispatchStationName;          
	DispatchStationName.Format ("%s",gpcdef.GetCurUserConsoleName());
	m_consoleid=DispatchStationName; 
	UpdateData(FALSE);   

	m_UseridCtrl.EnableWindow(TRUE);
	m_UsernameCtrl.EnableWindow(FALSE);
	m_PasswordCtrl.EnableWindow (FALSE);

	m_UseridCtrl.SetFocus();

	ConvertShiftidToString(Get_CurrentShiftid());
	UpdateData(FALSE);

	//�жϵ�ǰ�Ƿ���Ȩ�޵�½������ԱȨ��bֻ���ڽ�����Сʱ���ڵ�½
	//�����������Сʱ��ֻ�ܵ�½��ǰ�࣬���������ƶ���ť���
	//����Сʱ���ڣ���������ֻ��һ����ť������
	CTime curt = CTime::GetCurrentTime();

	time_t tbegin,tend;

	if(curt.GetHour()>=6 && curt.GetHour()<18)//�ڶ���
	{
		CTime shiftt = CTime(curt.GetYear(),curt.GetMonth(),curt.GetDay(),6,0,0);
		tbegin = shiftt.GetTime();
		tend = tbegin + 3*3600;

	}
	else if(curt.GetHour()>=18)//��һ��͵�ǰ���ڵ�18���
	{
		CTime shiftt = CTime(curt.GetYear(),curt.GetMonth(),curt.GetDay(),18,0,0);
		tbegin = shiftt.GetTime();
		tend = tbegin + 3*3600;
	}
	else//��һ�࣬��ǰһ���18��ȣ��϶��Ѿ���ʱ��
	{
		tbegin = 0;
		tend = 0;
	}

	m_bIgnoreLoginTm=false;
	m_checkbtn.SetCheck(FALSE);
	m_checkbtn.EnableWindow(FALSE);
	m_checkbtn.ShowWindow(SW_HIDE);
		
	//	InitDlgPosition(this);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGpcAccessReply,0);
	
	SetLoginWindowOpenFlag(true); // ��ֹ�Զ��ط��ڶ��׶ε�¼

	// ���ظ�����֤dll
	CString szPrompt;
	m_hGiOtpAPIDLL = LoadLibrary( "GiAgentX.dll" );
	if (m_hGiOtpAPIDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ���������֤ģ��GiAgentX.dll ������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}

	m_hFpClientDLL = LoadLibrary( "FpClient.dll" );
	if (m_hFpClientDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��ָ����֤ģ��FpClient.dll ������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}

	m_hJHDLLCUDLL = LoadLibrary( "jhdllcu.dll" );
	if (m_hJHDLLCUDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��ָ�Ʋɼ�ģ��jhdllcu.dll ������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}
	m_nAuthType = gpcdef.GetLoginType();
	nFlagEnableVer=0;
	LoadCertifyFile();
	LoadCertifyFile3();

	GpcLogTrainMessage(0,0,"��֤�汾:%d(0:��̬������ 1:��֤������ 2:ָ����֤)" , nFlagEnableVer);
	
	return TRUE;   
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(m_DateCtrl.m_hWnd==pWnd->m_hWnd || m_BanbieCtrl.m_hWnd==pWnd->m_hWnd )
	{     
		pDC->SetBkColor(bk_color);
		return (HBRUSH) m_Brush;
	}  

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CLoginDlg::OnPrevious() 
{   
	UpdateData(TRUE);
	m_nShiftID=Get_PreShiftid(m_nShiftID);
	ConvertShiftidToString(m_nShiftID);

	m_nPreSchdReqNum++;
	if(m_nPreSchdReqNum>=m_nMaxPreNum && m_bCtrlLoginBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(FALSE);
	}
}

void CLoginDlg::OnNext() 
{   
	UpdateData(TRUE);
	m_nShiftID=Get_NextShiftid(m_nShiftID);
	ConvertShiftidToString(m_nShiftID);

	m_nPreSchdReqNum--;
	if(m_nPreSchdReqNum<m_nMaxPreNum && m_bCtrlLoginBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(TRUE);
	}
}

CString CLoginDlg::GetNextDay()
{ 
	int  CurYear,CurMonth,CurDay;
	CString NextDay;

	CTime t=CTime::GetCurrentTime();
	CurYear=t.GetYear();
	CurMonth=t.GetMonth();
	CurDay=t.GetDay();
	ASSERT(CurDay>=1 && CurDay<=31);
	switch(CurDay)
	{
	case 28:
		if( (CurYear %4==0 && CurYear %100!=0)||(CurYear %400==0) )
			NextDay.Format ("%4d��%d��%d��",CurYear,CurMonth,29);
		else
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth+1,  1);
		break;

	case 29:
		if(CurMonth==2)
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth+1,1);
		else
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth,30);
		break;

	case 30:
		if(CurMonth==1 || CurMonth==3 || CurMonth==5 || CurMonth==7 || CurMonth==8
			|| CurMonth==10 ||CurMonth==12)
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth,31);
		else
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth+1,1);
		break;

	case 31:
		if(CurMonth==12)
			NextDay.Format ("%4d�� %d��%d��",CurYear+1,1,1);
		else
			NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth+1,1);
		break;

	default:
		NextDay.Format ("%4d�� %d��%d��",CurYear,CurMonth,CurDay+1);
		break;

	}

	return NextDay;
}

int CLoginDlg::GetConsoleid()
{
	CString DispatchStationName;  // ����̨����
	int consoleid;

	DispatchStationName.Format ("%s",gpcdef.GetCurUserName ());
	ASSERT( DispatchStationName.Remove('g')==1 );
	ASSERT( DispatchStationName.Remove('p')==1 );
	ASSERT( DispatchStationName.Remove('c')==1) ;

	consoleid=atoi(DispatchStationName);
	ASSERT(consoleid>=0);
	return consoleid;
}

void CLoginDlg::Set_Ddy_Name(unsigned long shift_id, short ddt_id,const char *ddyname)
{
	CLBTH::MsgSetDDyName  msg;
	msg.conid=ddt_id;
	msg.shift_id=shift_id;
	strncpy(msg.user_name,ddyname,sizeof(msg.user_name)-1);

	long  server_id=GetSpecialEntityID(SERVER_TYPE_DB,ddt_id);
	GpcLogTrainMessage(0,0,"[LOGIN]:send entity:  %d  conid:  %d shift_id:  %d ��½����Ա����: %s",  
		server_id,  msg.conid,  msg.shift_id, msg.user_name);
	if(!SendTGBMsg(ddt_id,&msg,server_id))
		SendBMsg(&msg,0);
}


void CLoginDlg::ConvertShiftidToString(unsigned long id)
{
	CString date,banbie;

	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d��%2d��%2d��",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "��һ��" : "�ڶ���";

	m_date=date;
	m_banbie=banbie;

	if(CurBanbie==1)
		bk_color=g_crGray;
	else
		bk_color=g_crRed;
	m_Brush.DeleteObject();
	m_Brush.CreateSolidBrush(bk_color); //color initiate;

	UpdateData(FALSE);
	Invalidate(FALSE);
}

void CLoginDlg::OnCheckgenban() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_checkgenban)
	{
		m_username3Ctrl.EnableWindow(TRUE);
		m_username3Ctrl.SetFocus();
	}
	else
	{
		m_username3Ctrl.EnableWindow(FALSE);
	}
}


LRESULT CLoginDlg::OnMsgGpcAccessReply(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if (MSGID_MsgGpcAccessReply == pMsgStruct->msg_id)
	{
		CLBTH::MsgGpcAccessReply *pServerReply= ( CLBTH::MsgGpcAccessReply *)pMsgStruct->pmsg;

		switch(pServerReply->status)
		{
		case CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK_OK:
			{
				GpcLogTrainMessage(0,0,"[LOGIN]:CTC��������֤ͨ��, ����ϵͳ");
				LoginOK();
			}
			break;
		case CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK_FAIL:
			AfxMessageBox("�û�������У�����,����������!");
			break;  
		case CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_OK:
			{
				if(nFlagEnableVer == 2) // �Ŵﻷ��ָ�Ʒ�����֤
				{
					m_username=pServerReply->user_name;
					m_strCtcPwd = pServerReply->pwd;
					UpdateData(FALSE);
					m_UseridCtrl.EnableWindow(FALSE);
					GpcLogTrainMessage(0,0,"[LOGIN]:CTC���������� %s %s", m_username, m_strCtcPwd);

					SetTimer(timeid,100,NULL);
				}
				else
				{
					m_strCtcPwd = pServerReply->pwd;
					m_username=pServerReply->user_name;
					UpdateData(FALSE);
					m_PasswordCtrl.EnableWindow(TRUE); 
					m_UseridCtrl.EnableWindow(FALSE);
					m_PasswordCtrl.SetFocus();
					GpcLogTrainMessage(0,0,"[LOGIN]:CTC���������� %s %s", m_username, m_strCtcPwd);
				}
			}
			break;
		case CLBTH::MsgGpcAccessReply::GPC_ACCESS_SERVER_READY:
			m_username=pServerReply->user_name;
			UpdateData(FALSE);
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_NOT_EXIST:
			AfxMessageBox("���û�������,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_INVALID_GPC_ID:
			AfxMessageBox("�û����벻��ȷ,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_JOBID_NOT_EXIST:
			AfxMessageBox("�û�����Ȩ�޲�����,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_CONID_NOT_EXIST:
			AfxMessageBox("�û���Ӧ�ĵ���̨������,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_GPCID_ALREADY_ONLINE:
			AfxMessageBox("�û��ѵ�½������ϵ����Ա!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_CONID_DISMATCH:
			AfxMessageBox("�û����ܲ�����̨,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_JOBID_DISMATCH:
			AfxMessageBox("�û������Ȩ�޲�ƥ��,����������!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_UNKNOWN_MESSAGE:
			AfxMessageBox("δ֪��Ϣ,����������!");
			break;		 
		default:
			break;
		}
	}
	return TRUE;
}

void CLoginDlg::OnDestroy() 
{
	KillTimer(timeid);
	if (m_hGiOtpAPIDLL)
		FreeLibrary(m_hGiOtpAPIDLL);
	if (m_hFpClientDLL)
		FreeLibrary(m_hFpClientDLL);
	if (m_hJHDLLCUDLL)
		FreeLibrary(m_hJHDLLCUDLL);
	m_vectsvr.clear();
	m_vectsvr2.clear();
	m_vectsvr3.clear();
	::CoUninitialize();
	CDialog::OnDestroy();
	SetLoginWindowOpenFlag(false); // �����Զ��ط��ڶ��׶ε�¼
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGpcAccessReply);
}

void CLoginDlg::SendLoginInfo(const char *username,const char *passwd) //  get userid from gpcdef
{
	CLBTH::MsgGpcAccessReply mMsgGpcAccessInfo;
	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	int m_nCurrentSectionNo = m_nCurrentSectionList[0];

	strcpy(mMsgGpcAccessInfo.user_code,username);
	strcpy(mMsgGpcAccessInfo.user_name,""); // gpcdef.GetCurrentUserid());
	strcpy(mMsgGpcAccessInfo.pwd,passwd);
	mMsgGpcAccessInfo.job_id = gpcdef.GetUserIdentify(); ///�������ͣ��ƻ�Ա������
	mMsgGpcAccessInfo.gpc_id = gpcdef.GetConsoleid(); //Ψһ��,��usernameת������   
	mMsgGpcAccessInfo.con_id = m_nCurrentSectionNo; 
	strcpy(mMsgGpcAccessInfo.os_identify,"W");
	strcpy(mMsgGpcAccessInfo.sys_version,"2.0");
	mMsgGpcAccessInfo.status=CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK;
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgGpcAccessInfo,server_id);
	else
		SendBMsg(&mMsgGpcAccessInfo,0);
}

void CLoginDlg::SendGpcAccessInfo(ushort uAccessStep,const char *userid) //  get userid from gpcdef
{
	CLBTH::MsgGpcAccessInfo mMsgGpcAccessInfo;
	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	int m_nCurrentSectionNo = m_nCurrentSectionList[0];
	strcpy(mMsgGpcAccessInfo.user_code,userid); // gpcdef.GetCurrentUserid());
	mMsgGpcAccessInfo.job_id = gpcdef.GetUserIdentify(); ///�������ͣ��ƻ�Ա������
	mMsgGpcAccessInfo.gpc_id = gpcdef.GetConsoleid(); //Ψһ��,��usernameת������   
	mMsgGpcAccessInfo.con_id = m_nCurrentSectionNo; 

	mMsgGpcAccessInfo.step  = ( CLBTH::MsgGpcAccessInfo::ACCESS_STEP)uAccessStep;
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgGpcAccessInfo,server_id);
	else
		SendBMsg(&mMsgGpcAccessInfo,0); 

}

void CLoginDlg::LoginOK()
{
	//save current username and password
	m_userid.TrimLeft();
	gpcdef.SetCurrentUserid(m_userid);        //userid	
	//���ϸ����������
	if(!m_username3.IsEmpty())
	{
		m_username += "/";
		m_username += m_username3;
	}
	gpcdef.SetCurrentUsername(m_username);
	gpcdef.SetCurrentUserpwd(m_strCtcPwd);

	int shiftid=m_nShiftID;
	gpcdef.SetLoginShiftid(m_nShiftID);        //login shiftid,e.g. 2002060701 
	unsigned long m_usCurrentDate = (shiftid/1000000 - 1990) * 380 +
		( shiftid/10000-(shiftid/1000000)*100 - 1) * 31 + 
		shiftid/100 - (shiftid/10000)*100  - 1 ;			 
	gpcdef.SetLoginDate(m_usCurrentDate);     //login date    

	// ������BȨ�޵���Ա��½����
	if((gpcdef.GetUserIdentify() == GPC_DISPATCHER) &&   (shiftid ==Get_CurrentShiftid()))
	{
		int m_nCurrentSectionList[64];
		int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
		gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
		int m_nCurrentSectionNo = m_nCurrentSectionList[0];
		Set_Ddy_Name(shiftid,m_nCurrentSectionNo,m_username);
	}

	sysprint(0,PMTG.ThreadId,"[LOGIN]:�û��ɹ���½����! ");               				
	LoginWindowSetAutoReloginEnabled(true);

	////// ֪ͨ��������������
	CString token,content;
	if(GenXMLMessage_InformDCMDLogin(token,content,m_username))
	{
		long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
		if(nEntityOfDCMD>0)
		{

			TGSendStringMessage(token,content,nEntityOfDCMD);
		}
	}
	CDialog::OnOK();
}

void CLoginDlg::LoadCertifyFile()
{
	CString szPrompt;

	m_vectsvr.clear();
	m_fixToken="";

	CString strCertifyFile = gpcdef.GetDataPathName();
	strCertifyFile += "\\UserCertificate.xml";
	//�ļ����ɶ�ʱ����ԭ����֤������
	if( _access(strCertifyFile, 4) == -1)
		return;

	MSXML::IXMLDOMNodePtr pNode, pChild;
	MSXML::IXMLDOMDocumentPtr pDOMDoc;
	HRESULT hr = pDOMDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if( FAILED(hr) )
	{
		szPrompt.Format( "[LOGIN]:MSXML 6.0û�а�װ" );
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox(szPrompt);
		return;
	}

	if( VARIANT_TRUE != pDOMDoc->load( (LPCSTR)strCertifyFile) )
	{	//�ļ���ʽ����ȷʱ�ı���
		MSXML::IXMLDOMParseErrorPtr pError = pDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format(
				"Validation failed on %s"
				"\nLine: %d"
				"\n=====================", 
				strCertifyFile, pError->Getline());

			_bstr_t strRes =  _bstr_t(str)
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText();

			szPrompt = (const char*)strRes;
			GpcLogTrainMessage(0,0,"[LOGIN]:%s", szPrompt);
			AfxMessageBox( szPrompt );
			return;
		}
	}

	// �Ŵﻷ������֤��ʶ
	pNode = pDOMDoc->selectSingleNode( "USER/TOKEN" );
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_fixToken = v.bstrVal;
		m_fixToken.Trim();
		m_fixToken = m_fixToken.MakeLower();
		GpcLogTrainMessage(0,0,"[LOGIN]:�Ŵﻷ������֤��ʶ: %s", CString(v.bstrVal));
	}
		
	// ��ģʽ��֤ ��֤������
	MSXML::IXMLDOMNodeListPtr pNodeServerList = pDOMDoc->selectNodes("USER/SERVERLIST/SERVER");
	for(int i=0; i<pNodeServerList->Getlength(); i++)
	{
		MSXML::IXMLDOMNodePtr pServerNode=pNodeServerList->Getitem(i);
		if(pServerNode==NULL)
			continue;

		CertiSvrInfo svr;
		_variant_t v;

		pNode = pServerNode->selectSingleNode("IP");
		if(NULL != pNode)
		{
			v = pNode->GetnodeTypedValue();
			CString ip = v.bstrVal;
			if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
			{
				strncpy(svr.strIp, ip, sizeof(svr.strIp));
				svr.strIp[15] = 0;
			}
		}

		pNode = pServerNode->selectSingleNode("SVRPORT");
		if(NULL != pNode)
		{
			v = pNode->GetnodeTypedValue();
			svr.port = strtoul(CString(v.bstrVal), NULL, 0);
		}
		if(CString(svr.strIp) != "" && svr.port >0)	
		{
			m_vectsvr.push_back(svr);
			GpcLogTrainMessage(0,0,"[LOGIN]:��֤������ IP: %s, port: %d", svr.strIp, svr.port);
		}
		else
		{
			AfxMessageBox("��֤���������ô���");
			GpcLogTrainMessage(0,0,"[LOGIN]:��֤���������ô��� IP : %s, port:  %d", svr.strIp, svr.port);
		}
	}

	if(m_vectsvr.size()>0)
		nFlagEnableVer=1;
	return;
}

bool CLoginDlg::CertificateUser(char *username, char *pwd) 
{
	SetWindowText("��������֤������ͨ��У���û���Ϣ...");
	
	int nFlagCertifyModle = 1;

	// ��֤�汾 1:��̬������ 2:��֤������ 3:ָ����֤
	if(nFlagEnableVer==1)
	{
		//  �����ȫ������֤������,������m_fixToken, ��������m_fixToken����
		//  ��:��̬�����ƺ���֤���������ֻ�ϼ���,���������ע��
		if(m_fixToken.IsEmpty())
			nFlagCertifyModle = 2;
		else
		{
			CString strUserCode = username; // �û�����
			if(strUserCode.GetLength() >= m_fixToken.GetLength())
			{
				// ��̬�����ƺ���֤���������ֻ�ϼ���ʱ,�����û���
				// ��,����m_fixToken=nc�������nc1213,������֤��������֤;�������1213,
				// ���ն�̬��������֤�������ж�strUserName�Ƿ�Ϊm_fixToken
				// ǰ׺
				CString token = strUserCode.Left(m_fixToken.GetLength());
				if(token == m_fixToken)
					nFlagCertifyModle = 2;
			}
		}
	}
	else if(nFlagEnableVer==2)
	{
			nFlagCertifyModle = 3;
	}
	
	CString szSysID = gpcdef.GetCurUserName();
	GpcLogTrainMessage(0,0,"[LOGIN]:SysID %s UserName %s", szSysID, username);
	
	if(nFlagCertifyModle == 1)		// �����û������֤
	{
		return CertificateUserVersion1(username, pwd);
	}
	if(nFlagCertifyModle == 2)
	{
		return CertificateUserVersion2(username, pwd, szSysID.GetBuffer());
	}
	if (nFlagCertifyModle == 3)
	{
		return CertificateUserVersion3(username);
	}

	GpcLogTrainMessage(0,0,"��֤ģʽ:%d(1:��̬������ 2:��֤������ 3:ָ����֤)" , nFlagCertifyModle);
	
	return false;
}

bool CLoginDlg::CertificateUserVersion1(char *username, char *pwd)
{
	nFlag = 0;
	CDynCertificate dynCert;
	CString cap;
	GetWindowText(cap);
	SetWindowText("��������֤������ͨ��У���û���Ϣ...");
	if(!dynCert.DynInit())
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:��̬������֤�����ʼ��ʧ��");
		AfxMessageBox("��̬������֤�����ʼ��ʧ��");
		SetWindowText(cap);
		return false;
	}

	int rc = dynCert.DynVerify(username, pwd);

	dynCert.DynUninit();
	SetWindowText(cap);

	if(rc == AUTH_USER_NOTEXIST)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:�û�����֤�����������ڣ�����ϵ����Ա");
		AfxMessageBox("�û�����֤�����������ڣ�����ϵ����Ա");				
	}    
	else if(rc == AUTH_PWD_MISMATCH)
	{
		AfxMessageBox("�������,���������� ");
		sysprint(0,PMTG.ThreadId,"[LOGIN]:�û��������! ");
		m_password="";
		UpdateData(FALSE);
		m_PasswordCtrl.SetFocus();
	}
	else if(rc == AUTH_ERR_NET)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:���ӿ�����֤����������! ");
		AfxMessageBox("���ӿ�����֤���������� ");
	}
	else if(rc == AUTH_ERR_OTHER)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:������֤����! ");
		AfxMessageBox("������֤��������ϵ����Ա");
	}

	//�鿴���ش����Ƿ�����������
	if(rc == AUTH_ERR_NET || rc == AUTH_ERR_OTHER)
	{
		rc=AUTH_ERR_NET;
	}

	return (rc==AUTH_SUCCESS);
}

bool CLoginDlg::CertificateUserVersion2(char *username, char *pwd, char* szSysID)
{
	CString szPrompt;

	if (m_hGiOtpAPIDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ���������֤ģ��GiAgentX.dll ������");
		return false;
	}

	//DLL API�ӿں���
	pGiOtp_Auth GiOtp_Auth=(pGiOtp_Auth)GetProcAddress(m_hGiOtpAPIDLL,"GiOtp_Auth");
	if (GiOtp_Auth == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��GiAgentX.dll�к���GiOtp_Auth������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//���������������֤������һ����������֤ͨ��ʱ����ѭ��
	CString szSvrIPAddr("127.0.0.1");	//�����Ʒ�����IP��ַ
	int nSvrPort = 12751;				//�����Ʒ���˿ں�
	bool bPass = false;
	for(int idx=0; idx<m_vectsvr.size(); idx++)
	{
		nSvrPort = m_vectsvr[idx].port;
		szSvrIPAddr = m_vectsvr[idx].strIp;
	
		//��ʼУ��
		char* pTemp = NULL;
		int nVal = GiOtp_Auth(szSvrIPAddr, nSvrPort, szSysID, username, pwd,&pTemp);
		if (nVal == 0)
		{ 
			szPrompt.Format(
				"[LOGIN]- ����֤������, ��֤������%s(port%d) ��֤ ��Դ:%s �û�:%s ͨ��",
				szSvrIPAddr, nSvrPort, szSysID, username);
			GpcLogTrainMessage(0,0,"%s", szPrompt);
			
			bPass = true;// ��֤ͨ��
			break; 
		}
		else
		{	
			//��֤��ͨ������¼ʧ��ԭ��
			struct FailReason {int nCode; LPCSTR szReason; };
			static FailReason reason[] = {
				{-1100, "������֤������ʧ��"},
				{-1101, "���ݴ���ʧ��"},
				{-1102, "DACУ��ʧ�ܣ����ݿ��ܱ��۸�"},
				{-9999, "ϵͳ�쳣"},
				{-1001, "Ӧ�ñ�ʶ XXX������"},
				{-1002, "����Ա XXX ������"},
				{-1003, "����ԱXXX������ʹ��״̬"},
				{-1004, "����ԱXXXδ��������"},
				{-1008, "������֤�������ϴ���ͬ"},
				{-1099, "Ӧ�������"},
				{-1021, "����ͬ������ʧ��"},
				{-1020, "�޴�����"},
				{-1009, "�������"},
			};
			for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
			{
				if(reason[iReason].nCode == nVal)
				{
					szPrompt.Format(
						"[LOGIN]- ����֤������%s(port%d) ��֤ ��Դ:%s �û�:%s ��ͨ��,\n"
						"������:%d, ԭ��:%s",
						szSvrIPAddr, nSvrPort, szSysID, username, 
						nVal, reason[iReason].szReason);
					GpcLogTrainMessage(0,0,"%s", szPrompt);
					AfxMessageBox(szPrompt);
					break;
				}
			}
		}
	}

	return bPass;
}

void CLoginDlg::LoadCertifyFile3()
{
	CString szPrompt;
	m_vectsvr2.clear();
	m_vectsvr3.clear();
	
	CString strCertifyFile = gpcdef.GetDataPathName();
	strCertifyFile += "\\UserFingerCertificateNo.xml";
	//�ļ����ɶ�ʱ����ԭ����֤������
	if( _access(strCertifyFile, 4) == -1)
		return;

	MSXML::IXMLDOMNodePtr pNode, pChild, pServerNode;
	MSXML::IXMLDOMDocumentPtr pDOMDoc;
	HRESULT hr = pDOMDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if( FAILED(hr) )
	{
		szPrompt.Format( "[LOGIN]:MSXML 6.0û�а�װ" );
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox(szPrompt);
		return;
	}

	if( VARIANT_TRUE != pDOMDoc->load( (LPCSTR)strCertifyFile) )
	{	//�ļ���ʽ����ȷʱ�ı���
		MSXML::IXMLDOMParseErrorPtr pError = pDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format(
				"Validation failed on %s"
				"\nLine: %d"
				"\n=====================", 
				strCertifyFile, pError->Getline());

			_bstr_t strRes =  _bstr_t(str)
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText();

			szPrompt = (const char*)strRes;
			GpcLogTrainMessage(0,0,"[LOGIN]:%s", szPrompt);
			AfxMessageBox( szPrompt );
			return;
		}
	}

	MSXML::IXMLDOMNodePtr UsbNode = pDOMDoc->selectSingleNode("USER/USBPORT");
	if(UsbNode)
	{
		m_isLocalGetData = 1 ;
		_variant_t Val = UsbNode ->GetnodeTypedValue();
		m_UsbPort =atoi(CString(Val.bstrVal));

		if(m_UsbPort < -1 || m_UsbPort >9)
		{
			AfxMessageBox("ָ�Ʋɼ��豸���ô���");
			GpcLogTrainMessage(0,0,"[LOGIN]:ָ�Ʋɼ��豸���ô��� USBID: %d", m_UsbPort);
		}
	}
	else
	{
	//ָ�Ʋɼ��豸
		MSXML::IXMLDOMNodeListPtr pNodeFPEquipmentList = pDOMDoc->selectNodes("USER/EQUIPMENT_LIST/EQUIPMENT");
		for(int i=0; i<pNodeFPEquipmentList->Getlength(); i++)
		{
			int nGpcNo = 0;
			pServerNode=pNodeFPEquipmentList->Getitem(i);
			if(pServerNode==NULL)
				continue;

			CertiSvrInfo svr2;
			_variant_t v2;

			svr2.nTf=1;
			pNode = pServerNode->selectSingleNode("@GPCNO");
			if(NULL != pNode)
			{
				v2 = pNode->GetnodeTypedValue();
				nGpcNo=atoi(CString(v2.bstrVal));

				if(nGpcNo != gpcdef.GetUserGpcId())
					continue;
			}

			if (gpcdef.GetUserGpcId() == nGpcNo)
			{
				pNode = pServerNode->selectSingleNode("TF");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					svr2.nTf=atoi(CString(v2.bstrVal));
				}

				pNode = pServerNode->selectSingleNode("IP");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					CString ip = v2.bstrVal;
					if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
					{
						strncpy(svr2.strIp, ip, sizeof(svr2.strIp));
						svr2.strIp[15] = 0;
					}
				}

				pNode = pServerNode->selectSingleNode("SVRPORT");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					svr2.port = strtoul(CString(v2.bstrVal), NULL, 0);
				}
			}

			if(CString(svr2.strIp) != "" && svr2.port >0 && svr2.nTf == 1 && nGpcNo > 0)
			{
				m_vectsvr2.push_back(svr2);
				GpcLogTrainMessage(0,0,"[LOGIN]:ָ�Ʋɼ��豸 IP: %s , port: %d, ntf: %d, GpcNo: %d", svr2.strIp, svr2.port, svr2.nTf, nGpcNo);
			}
			else
			{
				AfxMessageBox("ָ�Ʋɼ��豸���ô���");
				GpcLogTrainMessage(0,0,"[LOGIN]:ָ�Ʋɼ��豸���ô��� IP: %s ,port: %d, ntf: %d, GpcNo: %d ", svr2.strIp, svr2.port, svr2.nTf, nGpcNo);
				return;
			}
		//ָ�ƱȶԷ�����
		}
	}
	if (m_vectsvr2.size() == 0)
	{
		GpcLogTrainMessage(0,0,"[LOGIN]:��ǰ�û�û������ָ�Ʋɼ��豸");
		return;
	}
	MSXML::IXMLDOMNodeListPtr pNodeFPServerList = pDOMDoc->selectNodes("USER/SERVER_LIST/SERVER");
	for(int i=0; i<pNodeFPServerList->Getlength(); i++)                      
	{
		pServerNode=pNodeFPServerList->Getitem(i);
		if(pServerNode==NULL)
			continue;

		CertiSvrInfo svr3;
		_variant_t v3;

		pNode = pServerNode->selectSingleNode("IP");
		if(NULL != pNode)
		{
			v3 = pNode->GetnodeTypedValue();
			CString ip = v3.bstrVal;
			if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
			{
				strncpy(svr3.strIp, ip, sizeof(svr3.strIp));
				svr3.strIp[15] = 0;
			}
		}

		pNode = pServerNode->selectSingleNode("SVRPORT");
		if(NULL != pNode)
		{
			v3 = pNode->GetnodeTypedValue();
			svr3.port = strtoul(CString(v3.bstrVal), NULL, 0);
		}
		if(CString(svr3.strIp) != "" && svr3.port > 0)
		{
			m_vectsvr3.push_back(svr3);
			GpcLogTrainMessage(0,0,"[LOGIN]:ָ�ƱȶԷ����� IP: %s , port: %d", svr3.strIp, svr3.port);
		}
		else
		{
			AfxMessageBox("ָ�ƱȶԷ��������ô���");
			GpcLogTrainMessage(0,0,"[LOGIN]:ָ�ƱȶԷ��������ô��� IP: %s , port: %d ", svr3.strIp, svr3.port);
			return;
		}
	}
	
	if (m_vectsvr3.size()>0 )
	{
		if(m_vectsvr2.size()>0 || m_UsbPort >= 0)
			nFlagEnableVer=2;
	}
	return;
}
bool CLoginDlg::CertificateUserVersion3(char *username)//�Ŵﻷ��ָ����֤ hjz
{
	CString szPrompt;
	if (m_hFpClientDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��ָ����֤ģ��FpClient.dll ������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//DLL API�ӿں���
	pRemoteVerifyFp RemoteVerifyFp=(pRemoteVerifyFp)GetProcAddress(m_hFpClientDLL,"remoteVerifyFp");
	if (RemoteVerifyFp == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��FpClient.dll�к���RemoteVerifyFp������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//1.ָ�Ʋɼ�
	BYTE szFingerInfo[512];
	int  nFingerInfoLen=sizeof(szFingerInfo)/sizeof(szFingerInfo[0]);
	memset(szFingerInfo, 0, nFingerInfoLen);
	bool bCollectionOK = FingerCollection(szFingerInfo, &nFingerInfoLen);

	//2.���������������бȶ�
	if(bCollectionOK)
	{
		SetWindowText("������֤�û�ָ��......");
		int szPort = 0;
		for(int idx=0; idx<m_vectsvr3.size(); idx++)
		{
			CString szSvrIPAddr = m_vectsvr3[idx].strIp;
			szPort = m_vectsvr3[idx].port;

			//��ʼУ��
			int nVal = RemoteVerifyFp(szSvrIPAddr.GetBuffer(), szPort, username, szFingerInfo);
			if (nVal == 0)
			{ 
				szPrompt.Format("[LOGIN]- ָ����֤������%s:%d ��֤�û�:%s ͨ��",szSvrIPAddr,szPort, username);
				return true;// ��֤ͨ��
			}
			else
			{	
				//��֤��ͨ������¼ʧ��ԭ��
				struct FailReason {int nCode; LPCSTR szReason; };
				static FailReason reason[] = {
					{-1, "���ӳ�ʱ"},
					{1, "�ȶ�ʧ��,��öָ�Ʋ���ͬ"},
					{2, "���ݸ�ʽ����ȷ"},
					{3, "��������ʧ��"},
				};
				for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
				{
					if(reason[iReason].nCode == nVal)
					{
						szPrompt.Format(
							"[LOGIN]- ��֤������%s:%d ��֤�û�:%s ��ͨ��,������:%d, ԭ��:%s",
							szSvrIPAddr, szPort, username, nVal, reason[iReason].szReason);
						GpcLogTrainMessage(0,0,"%s", szPrompt);
						break;
					}
				}
			}
		}
	}
	
	szPrompt.Format("���ָ����֤ʧ��, ��Ȩ�޵�½���ն�, ��ȷ�ϲ����������û�����");
	GpcLogTrainMessage(0,0,"%s username %", szPrompt, username);

	MessageBox(szPrompt, "����", MB_ICONHAND);

	return false;
}

bool CLoginDlg::FingerCollection(BYTE* szFingerInfo, int* nFingerInfoLen)
{
	CString szPrompt;
	if (m_hJHDLLCUDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��ָ�Ʋɼ�ģ��jhdllcu.dll ������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		MessageBox("�޷�����ָ�Ʋɼ�, ��ά����Աȷ��dllĿ¼���Ƿ����jhdllcu.dll", "����", MB_ICONWARNING);
		return false;
	}
	pFPIFea_IP FPIFea_IP=(pFPIFea_IP)GetProcAddress(m_hJHDLLCUDLL,"FPIFea_IP");
	if (FPIFea_IP == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��jhdllcu.dll�к���FPIFea_IP������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);

		MessageBox("�޷�����ָ�Ʋɼ�, ��ά����Աȷ��dllĿ¼��jhdllcu.dll�Ƿ�Ϊ��ȷ�汾", "����", MB_ICONWARNING);
		return false;
	}
	pFPIFeature_FDR FPIFeature_FDR=(pFPIFeature_FDR)GetProcAddress(m_hJHDLLCUDLL,"FPIFeature_FDR");

	if(FPIFeature_FDR == NULL)
	{
		szPrompt.Format("[LOGIN]:�Ŵﻷ��jhdllcu.dll�к���FPIFeature_FDR������");
		GpcLogTrainMessage(0,0,"%s", szPrompt);

		MessageBox("�޷�����ָ�Ʋɼ�, ��ά����Աȷ��dllĿ¼��jhdllcu.dll�Ƿ�Ϊ��ȷ�汾", "����", MB_ICONWARNING);
		return false;
	
	}

	if(m_isLocalGetData == 1)
	{
		while(1)
		{
			int nVal = FPIFeature_FDR(m_UsbPort, szFingerInfo, nFingerInfoLen); 
			if (nVal != 0)
			{	
				struct FailReason {int nCode; LPCSTR szReason; };
				static FailReason reason[] = {
					{-1, "��������ʧ��"},
					{-2, "������������������"},
					{-3, "��������ʧ��"},
					{-4, "��������ʧ��"},
					{-5, "ָ�Ʋɼ�ʧ��"},
				};
				for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
				{
					if(reason[iReason].nCode == nVal)
					{
						szPrompt.Format(
							"[LOGIN]- ָ�Ʋɼ�usb�豸�ɼ�ָ��ʧ��, ������:%d, ԭ��:%s",
							nVal, reason[iReason].szReason);

						GpcLogTrainMessage(0, 0,"%s", szPrompt);
						break;
					}
				}		
			}
			else
			{
				return true; // usb�ɼ��ɹ�,����
			}
			if(MessageBox("ָ�Ʋɼ�ʧ��, ���Ƿ����²ɼ�", "����", MB_ICONWARNING|MB_OKCANCEL) == IDCANCEL)
				break;
		}
		return false;
	}
	else
	{
		while(1)
		{
			CString szEquipIPAddr;
			for(int idx=0; idx<m_vectsvr2.size(); idx++)
			{
				SetWindowText("�����ָ�Ʋɼ�");

				szEquipIPAddr.Format("%s:%d", m_vectsvr2[idx].strIp, m_vectsvr2[idx].port);
				int nVal = FPIFea_IP(szEquipIPAddr, szFingerInfo, nFingerInfoLen, m_vectsvr2[idx].nTf);
				if (nVal != 0)
				{	
					struct FailReason {int nCode; LPCSTR szReason; };
					static FailReason reason[] = {
						{-1, "��������ʧ��"},
						{-2, "������������������"},
						{-3, "��������ʧ��"},
						{-4, "��������ʧ��"},
						{-5, "ָ�Ʋɼ�ʧ��"},
					};
					for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
					{
						if(reason[iReason].nCode == nVal)
						{
							szPrompt.Format(
								"[LOGIN]- ָ�Ʋɼ��豸%s�ɼ�ָ��ʧ��, ������:%d, ԭ��:%s",
								szEquipIPAddr, nVal, reason[iReason].szReason);

								GpcLogTrainMessage(0, 0,"%s", szPrompt);
							break;
						}
					}
				}
				else
				{
					return true; // �ɼ��ɹ�,����
				}
			}

			if(MessageBox("ָ�Ʋɼ�ʧ��, ���Ƿ����²ɼ�", "����", MB_ICONWARNING|MB_OKCANCEL) == IDCANCEL)
				break;
		}
		return false;
	}
}

void CLoginDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent==timeid)
	{
		KillTimer(timeid);
		bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(), "");  // ���͸������֤������
		if(m_bcfvsr_status)
		{			
			SendLoginInfo(m_userid,m_strCtcPwd);
			GpcLogTrainMessage(0,0,"[LOGIN]:��ȫ���Ŵﻷ��ָ�Ʒ�������֤ %s %s", m_userid, m_strCtcPwd);
		}
		else
		{
			m_username="";
			m_strCtcPwd="";
			UpdateData(FALSE);
			m_UseridCtrl.EnableWindow(TRUE);
			m_PasswordCtrl.EnableWindow(FALSE);
			m_UseridCtrl.SetWindowText("");
			m_UseridCtrl.SetFocus( );
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CLoginDlg::OnBnClickedCheckLogin()
{
	if(m_checkbtn.GetCheck()==TRUE)
	{
		m_bIgnoreLoginTm=true;
	}
	else
	{
		m_bIgnoreLoginTm=false;
	}
}

#include "StdAfx.h"
#include "tgdocument.h"
#include ".\dlgbasicdeletename.h"
#include "basic_schd_config.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>
#include <baseMsg\msg_schd.h>
#include "sysshare.h"
#include "tgmsg.h"

extern basic_schd_config* GetBasicConfig();
extern BOOL ReqBasicName_ToServer(int con_id, bool bload);

CDlgBasicDeleteName::CDlgBasicDeleteName(int conid, CTgDocument *pDoc, CWnd* pParent)
: CDialog(CDlgBasicDeleteName::IDD, pParent)
{
	m_nConid=conid;
	m_pDoc=pDoc;
	m_pParant=pParent;
}

CDlgBasicDeleteName::~CDlgBasicDeleteName(void)
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgBasicScheduleSet);
}

BOOL CDlgBasicDeleteName::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgBasicScheduleSet,0);
	UpdateBakcupBasicName();
	InitDlgPosition(this);
	return TRUE;  
}

void CDlgBasicDeleteName::DoDataExchange(CDataExchange* pDX)
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDlgBasicDeleteName)
	DDX_Control(pDX, IDC_LIST_BASICNAME, m_combo_basicname);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CDlgBasicDeleteName, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

void CDlgBasicDeleteName::OnBnClickedBtnDelete()
{
	int nSel = m_combo_basicname.GetCurSel();
	if(nSel>=0)
	{
		CLBTH::MsgBasicScheduleSet schd_name;
		schd_name.nConId =m_nConid;
		schd_name.nScheduleId=m_combo_basicname.GetItemData(nSel);

		// ����ͼ���� = 0 ��ʾ ɾ����
		memset(schd_name.nScheduleName, 0, MAX_SCHEDULE_NAME_LEN);

		schd_name.nStartTime=0;
		schd_name.nEndTime=0;

		// Ԥ��Ͷ��ʱ��
		schd_name.nEstimateStartTime=0;

		// ����ʱ��
		CTime tm=CTime::GetCurrentTime();
		schd_name.nSetNameTime=tm.GetTime();

		// ����ʵ��
		schd_name.nSetNameEntity=gpcdef.GetThisEntity();

		long eid=GetSpecialEntityID(SERVER_TYPE_BASIC_TG,schd_name.nConId);
		SendTGBMsg(schd_name.nConId,&schd_name,eid);

		sysprint(10,PMTG.ThreadId,"[ɾ���鵵����ͼ]:%d��%d����ɾ���鵵����ͼ nConId %d nScheduleId %d nScheduleName:%s.", 
			schd_name.nSetNameEntity,
			eid, schd_name.nConId, schd_name.nScheduleId, 
			schd_name.nScheduleName);
		
		m_combo_basicname.ResetContent();
		sysprint(10,PMTG.ThreadId,"[ɾ���鵵����ͼ]:�������ѡ����й鵵����ͼ����,�ȴ����������ظ��¹鵵����ͼ����.");

		SetTimer(22222,200, NULL);
	}
}

//void CDlgBasicDeleteName::OnOK()
//{
//	// TODO: �ڴ����ר�ô����/����û���
//
//	CDialog::OnOK();
//}

void CDlgBasicDeleteName::OnBnClickedBtnClose()
{
	OnCancel();
}

void CDlgBasicDeleteName::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 22222)
	{
		KillTimer(22222);
		ReqBasicName_ToServer(m_nConid, true);
	}
		
	CDialog::OnTimer(nIDEvent);
}

LRESULT CDlgBasicDeleteName::OnCommMsg(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id == MSGID_MsgBasicScheduleSet)
	{
		CLBTH::MsgBasicScheduleSet *pMsg= ( CLBTH::MsgBasicScheduleSet *)pMsgStruct->pmsg;
		if(m_nConid == pMsg->nConId)
		{
			// 2017-09-11 ��δ���� tg_logic_main���ظ������ǲ���ɾ������Ϊtg_logic_main�к󱣴棬������ˢ�£���������һ������ͼ����
			pMsg->nScheduleName[MAX_SCHEDULE_NAME_LEN-1]=0;
	        GetBasicConfig()->SetBasicSchdName(pMsg->nConId, pMsg->nScheduleId, pMsg->nScheduleName, pMsg->nStartTime, pMsg->nEndTime, pMsg->nEstimateStartTime);
			
			sysprint(10,PMTG.ThreadId,"[ɾ���鵵����ͼ]:�յ����������ػ���ͼ����,���¶Ի���鵵����ͼ����.");
			UpdateBakcupBasicName();
		}
	}
	
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CDlgBasicDeleteName::UpdateBakcupBasicName()
{
	m_combo_basicname.ResetContent();
	SECTION_BASIC_NAME* pBasic = GetBasicConfig()->GetBasicSchdNameStruct(m_nConid);
	if(pBasic)
	{
		for(int i=0; i<pBasic->num; i++)
		{
			CString text = pBasic->basic_backup_schd[i].name;
			int     type = pBasic->basic_backup_schd[i].type;
			int index=m_combo_basicname.AddString(text);
			m_combo_basicname.SetItemData(index, type);
		}
	}
}

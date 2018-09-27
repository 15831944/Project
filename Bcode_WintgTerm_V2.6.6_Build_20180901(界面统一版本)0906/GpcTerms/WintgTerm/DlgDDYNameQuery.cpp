// DlgBasicParam.cpp : implementation file
//

#include "stdafx.h"
#include "tg.h"
#include "tgdocument.h"
#include "DlgDDYNameQuery.h"
#include "tgmsg.h"
#include "sysshare.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDDYNameQuery dialog
CDlgDDYNameQuery::CDlgDDYNameQuery(CTgDocument *pDoc, int conid, long shiftid, CWnd* pParent)
: CDialog(CDlgDDYNameQuery::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDDYNameQuery)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_conid=conid;
	m_shiftid=shiftid;
}
CDlgDDYNameQuery::~CDlgDDYNameQuery()
{
}
void CDlgDDYNameQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDDYNameQuery)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDDYNameQuery, CDialog)
	//{{AFX_MSG_MAP(CDlgDDYNameQuery)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnOK)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
//�յ���ͼ�ķ�����Ϣ

LRESULT CDlgDDYNameQuery::OnCommMsg(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;
	if(pMsgStruct->msg_id == MSGID_MsgGiveDDyName)
	{
		CString title1, title2;

		CLBTH::MsgGiveDDyName  *pMsg = (CLBTH::MsgGiveDDyName  *)pBase;

		if(m_conid == pMsg->conid && m_shiftid == pMsg->shift_id)
		{
			m_ddyName1 = pMsg->user_name1;
			m_ddyName2 = pMsg->user_name2;
			
			sysprint(5,PMTG.ThreadId,"�յ�ֵ�����Ա���� conid %d shiftid %d name1:%s name2:%s", pMsg->conid, pMsg->shift_id, m_ddyName1, m_ddyName2);
			
			
			if(m_shiftid%2==0)
			{
				title1 = "ֵ�����Ա(6��-8��):"+m_ddyName1;
				title2 = "ֵ�����Ա(8��-18��):"+m_ddyName2;
			}
			else
			{
				title1 = "ֵ�����Ա(18��-20��):"+m_ddyName1;
				title2 = "ֵ�����Ա(20��-6��):"+m_ddyName2;
			}
		}
		else
		{
			title1.Format("�յ�ֵ�����Ա���� conid %d shiftid %d name1:%s name2:%s�����Ǻ͵�ǰ̨�Ż��β�ͬ!", pMsg->conid, pMsg->shift_id, m_ddyName1, m_ddyName2);

			sysprint(5,PMTG.ThreadId, "%s", title1);
		}

		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
		if(pEdit)
		{
			pEdit->SetWindowText(title1);
		}
		pEdit = (CEdit*)GetDlgItem(IDC_EDIT4);
		if(pEdit)
		{
			pEdit->SetWindowText(title2);
		}
	}
	
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CDlgDDYNameQuery::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGiveDDyName,0);

	// TODO: Add extra initialization here
	InitDlgPosition(this);

	Query_GetDdyName();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDDYNameQuery::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgGiveDDyName);
	OnCancel();

	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDDYNameQuery message handlers
void CDlgDDYNameQuery::OnOK() 
{
	OnClose();
}

//----------------------------------------------------------------------------
void CDlgDDYNameQuery::Query_GetDdyName()
{
	MsgGetDDyName GetDdyName;

	GetDdyName.conid = m_conid;
	GetDdyName.shift_id = m_shiftid;

	Archive ar;
	ar << GetDdyName;

	long entity_id=GetSpecialEntityID(SERVER_TYPE_DB, GetDdyName.conid);
	int error=SendTGCommMessage(GetDdyName.conid, ar.getCurrentPos(), (char*)ar.getBuffer(), entity_id);	
	if(error == RET_SUCCESS) 
		sysprint(5,PMTG.ThreadId,"��svr %d ����%d̨ shiftid %d ����Ա����",  entity_id, GetDdyName.conid, GetDdyName.shift_id);
	else
		sysprint(5,PMTG.ThreadId,"��svr %d ����%d̨ shiftid %d ����Ա����ʧ��", entity_id, GetDdyName.conid, GetDdyName.shift_id);

	m_ddyName1 = "";
	m_ddyName2 = "";
}

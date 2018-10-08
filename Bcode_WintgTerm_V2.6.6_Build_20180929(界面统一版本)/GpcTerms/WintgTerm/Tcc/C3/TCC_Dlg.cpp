// TCC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tg.h"
#include "resource.h"
#include "memshare.h"
#include "TCC_Dlg.h"
#include <algorithm>
#include <SuperPassDlg.h>
#include "DirectLimit.h"
#include <oscalls.h>

namespace TCC_C3 {

static HWND hWndTccUI = NULL;
HWND GetTccDialogWndHandle()
{
	return hWndTccUI;
}

void RegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList2,0);//353
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport2,0);// 352
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult2,0); //356
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand2,0);// 355
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2,0);//361
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

    RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest,0);//413
	GpcLogTrainMessage(0,0, "[TCC_C3]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
}

void UnRegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList2);//353
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport2);// 352
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult2); //356
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand2);// 355
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2);//361
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

    UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest);//413
	GpcLogTrainMessage(0,0, "[TCC_C3]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
}

IMPLEMENT_DYNAMIC(CTccDlg, CDialog);

CTccDlg::CTccDlg() : m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::CTccDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::CTccDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::~CTccDlg()
{
	m_pTccClientInfoProvider->Release();
	m_pTccAdviseSink->Release();
}

void CTccDlg::SetInterface(ITccAdviseSink *pTccAdviseSink, ITccClientInfoProvider *pInfoProvider)
{
	pTccAdviseSink->AddRef();
	this->m_pTccAdviseSink = pTccAdviseSink;

	pInfoProvider->AddRef();
	m_pTccClientInfoProvider = pInfoProvider;
}

STOCK_COMMAND* CTccDlg::GetConfirmCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	return m_ConfirmCommandPage.GetCommand(nConsoleID, sysid, nCmdID);
}

STOCK_COMMAND* CTccDlg::GetLocalCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	return m_ConfirmCommandPage.GetCommand(nConsoleID, sysid, nCmdID);
}

BOOL CTccDlg::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
	if(hWndTccUI)
	{
		delete this;
		return FALSE;
	}

	m_bAutoDelete = TRUE;

	BOOL b = CDialog::Create(IDD, pParentWnd);
	if(!b)
		delete this;

	return b;
}

void ResetTccList(CComboBox& TccList, ITccClientInfoProvider *pInfoProvider)
{
	TccList.ResetContent();

	// �п������б�
	std::vector<stano_t> staList;
	pInfoProvider->GetStationList(staList);
	for(std::vector<stano_t>::iterator iter = staList.begin(); iter != staList.end(); iter++)
	{
		INT nIndex = TccList.AddString(
			pInfoProvider->GetStationName(*iter));
		ASSERT(CB_ERR != nIndex);
		TccList.SetItemData(nIndex, *iter);
	}
	
	TccList.SetCurSel(-1);
}

BOOL CTccDlg::OnInitDialog()
{
	BOOL bResult = CDialog::OnInitDialog();

	if(!TccInitialize(m_pTccClientInfoProvider))
		return FALSE;

	//ModifyStyle(WS_SYSMENU, 0, NULL);

	// TODO:  �ڴ��������ר�ô���
	if(NULL == m_pTccClientInfoProvider)
	{
		MessageBox("û�а�װTccClientInfoProvider�ӿ�!");
		return FALSE;
	}
	if(NULL == m_pTccAdviseSink)
	{
		MessageBox("û�а�װTccAdviseSink�ӿ�!");
		return FALSE;
	}

	ITccClientInfoProvider *pInfoProvider = m_pTccClientInfoProvider;

	// �п������б�
	ResetTccList(m_TccList, pInfoProvider);
	
	m_sheet.AddPage("�п�ʵʱ����", &m_LircCommandPage, CLircCommandPage::IDD);
	m_sheet.AddPage("��������", &m_ConfirmCommandPage, CConfirmCommandPage::IDD);
	m_sheet.AddPage("��ʷ����", &m_HistoryCommandPage, CHistoryCommandPage::IDD);
	//m_sheet.AddPage("ԭʼ����", &m_LocalCommandPage, CLocalCommandPage::IDD);
	m_sheet.Show();

	CenterWindow();

	RegisterTCCMsgProcHandleWnd();
	// C2Ȩ�޿���
	bool bExecute = false;
	if(m_pTccClientInfoProvider->IsCTC2GPC())
	{
		if(m_pTccClientInfoProvider->IsTCC2Execute())
		{
			bExecute = true;
		}
	}

	if(bExecute)
	{
		bExecute=gpcdef.IsDirectExecuteTCC();
	}

	if(!bExecute)
	{		
		CWnd* pWnd = GetDlgItem(IDC_TCC_REFRESH);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	return bResult;
}

BEGIN_MESSAGE_MAP(CTccDlg, CDialog)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND, OnReceiveTccCommand)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, OnReceiveTccLineInitialCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCC_REPORT, OnReceiveTccReport)
	ON_MESSAGE(UM_RECEIVE_CONFIRM_COMMAND, OnReceiveConfirmCommand)
	ON_MESSAGE(UM_CLIENTENV_CHANGED, OnClientEnvChanged)
	ON_MESSAGE(UM_TCC_INITCOMPLETE, OnTccInitComplete)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_CBN_SELCHANGE(IDC_TCC_LIST, OnSelectTcc)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnDirectLimit)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

struct _TCCLINE_INITIAL_COMMAND_RESULT {
	stano_t nTccID;
	lineid_t nTccLineID;
	dir_t dir;
	DWORD nStatus;
	CString strComment;
};

LRESULT CTccDlg::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;

	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
	{
		// �յ����ɻ��������п�ʵʱ��������
		if(pMsgStruct->msg_id==MSGID_MsgStccCommandList2)
		{
			CLBTH::MsgStccCommandList2  *pMsg = (CLBTH::MsgStccCommandList2  *)pBase;

			TCC_C3::TCC_COMMAND cmd;
			cmd.nConsoleID = pMsg->console_no;		// ����̨ ̨��
			TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);		// ϵͳ��ʶ
			cmd.nCmdID = pMsg->cmd_no;			// ����ţ�0~9999��
			cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// ��·��
			cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
			cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// ��ʼ����������꣨��mΪ��λ��
			cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
			cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
			cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
			cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
			cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
			cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
			cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
			cmd.bStartNow = pMsg->bStartNow;		// �Ƿ�������ʼ
			cmd.bEndless = pMsg->bEndless;		// �Ƿ�������Ч
			cmd.nTime[0] = pMsg->time[0];		// ��ʼ������ʱ��
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// ����ֵ

			cmd.nTccID = pMsg->tcc_no;			// ����վ���пأ�
			cmd.nTccLineID = pMsg->tcc_line_no;	// ����վ��·
			cmd.nTccSenderID = pMsg->tcc_sender;	// ����վ������
			cmd.nTccKmMarkFlag[0] = ((pMsg->tcc_km_marker[0].nKmsysCode >> 6) & 0x03);	// ����վ����꣨��mΪ��λ��
			cmd.nTccKmMarkFlag[1] = ((pMsg->tcc_km_marker[1].nKmsysCode >> 6) & 0x03);

			//TCC_C3::Receive_TccCommand(cmd, 0);
			::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND, 0, (LPARAM)&cmd);
		}
		// �п�״̬
		else if (pMsgStruct->msg_id==MSGID_MsgStccReport2)
		{
			CLBTH::MsgStccReport2  *pMsg = (CLBTH::MsgStccReport2  *)pBase;

			TCC_C3::TCC_STATUS status;
			status.comm_active = pMsg->comm_active;	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
			status.init_status = pMsg->init_status;
			status.nStaID = pMsg->sta_no;	// վ��
			for(int iSta=0; iSta<pMsg->nStaNum && iSta<sizeof(status.sta)/sizeof(status.sta[0]); iSta++)
			{
				status.sta[iSta].nTccID = pMsg->sta[iSta].nStaID;
				status.sta[iSta].standby_status = pMsg->sta[iSta].stby_status;
				status.sta[iSta].master_status = pMsg->sta[iSta].master_status;
				for(int i=0; i<2; i++)
					status.sta[iSta].ilock_channel[i] = pMsg->sta[iSta].ilock_channel[i];	// ����ͨ��״̬
				for(i=0; i<2; i++)
					status.sta[iSta].ctc_channel[i] = pMsg->sta[iSta].ctc_channel[i];	// CTCͨ��״̬
				for(i=0; i<2; i++)
					status.sta[iSta].switch_channel[i] = pMsg->sta[iSta].switch_channel[i];	// ������ͨ��״̬

				for(i=0; i<16; i++)
				{
					status.sta[iSta].leu[i].bValid = pMsg->sta[iSta].leu[i].bValid;
					for(int j=0; j<2; j++)
						status.sta[iSta].leu[i].leu_channel[j] =pMsg->sta[iSta].leu[i].leu_channel[j];	// LEUͨ��״̬

					for(j=0; j<4; j++)
						status.sta[iSta].leu[i].leu_ack[j]  = pMsg->sta[iSta].leu[i].leu_ans[j];	// LEU1Ӧ����״̬
				}
				status.sta[iSta].nZCNum = pMsg->sta[iSta].nZCNum;
				for(i=0; i<pMsg->sta[iSta].nZCNum && i<8; i++)
				{
					status.sta[iSta].ZC[i].nStaID = pMsg->sta[iSta].ZC[i].nStaID;
					status.sta[iSta].ZC[i].channel[0] = pMsg->sta[iSta].ZC[i].channel[0];
					status.sta[iSta].ZC[i].channel[1] = pMsg->sta[iSta].ZC[i].channel[1];
				}
			}
			status.nStaNum = iSta;

			//TCC_C3::Receive_TccReport(pMsg->sta_no, status);
			::SendMessage(hWnd, UM_RECEIVE_TCC_REPORT, pMsg->sta_no, (LPARAM)&status);
		}
		else if (pMsgStruct->msg_id==MSGID_MsgStccCommandSendResult2)
		{
			CLBTH::MsgStccCommandSendResult2  *pMsg = (CLBTH::MsgStccCommandSendResult2  *)pBase;
			USHORT nTaskId;
			if(pMsg->bInitOpration)
			{
				TCC_C3::dir_t dir = TCC_C3::DIR_UNKNOWN;
				switch(pMsg->section_dir)
				{
				case 0:
					dir = TCC_C3::DIR_UP;
					break;
				case 1:
					dir = TCC_C3::DIR_DOWN;
					break;
				case 2:
					dir = TCC_C3::DIR_DUAL;
					break;
				default:
					break;
				}

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, 1024);
				//AfxMessageBox(buf);

				//TCC_C3::Receive_TccLineInitialCommandResult(pMsg->tcc_no, pMsg->tcc_line_no, dir, pMsg->result, buf);
				_TCCLINE_INITIAL_COMMAND_RESULT data;
				data.nTccID = pMsg->tcc_no;
				data.nTccLineID = pMsg->tcc_line_no;
				data.dir = dir;
				data.nStatus = pMsg->result;
				data.strComment = buf;
				::SendMessage(hWnd, UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, 0, (LPARAM)&data);
			}
			else
			{
				TCC_C3::TCC_COMMAND_RESULT cmd_result;
				cmd_result.cmd.nConsoleID = pMsg->console_no;
				TCC_C3::SysidFromByteArray(cmd_result.cmd.sys_id, pMsg->sys_id, sizeof(pMsg->sys_id));
				cmd_result.cmd.nCmdID = pMsg->cmd_no;		// �����(0~9999)
				cmd_result.cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼվ
				cmd_result.cmd.nStaID[1] = pMsg->sta_no[1];		// ����վ
				cmd_result.cmd.nLineID = pMsg->line_no;		// ��·��
				cmd_result.cmd.bFullLimit = pMsg->bFullLimit;	// �Ƿ�ȫ������
				cmd_result.cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// ��ʼ����������꣨��mΪ��λ��
				cmd_result.cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
				cmd_result.cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
				cmd_result.cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
				cmd_result.cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
				cmd_result.cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
				cmd_result.cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
				cmd_result.cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
				cmd_result.cmd.bStartNow = pMsg->bStartNow;		// �Ƿ�������ʼ
				cmd_result.cmd.bEndless = pMsg->bEndless;		// �Ƿ�������Ч
				cmd_result.cmd.nTime[0] = pMsg->time[0];			// ��ʼʱ��(��)
				cmd_result.cmd.nTime[1] = pMsg->time[1];			// ����ʱ��(��)
				cmd_result.cmd.nSpeedMaxVal = pMsg->speed;		// ����ֵ(5KM/H�ı���)

				cmd_result.cmd.nTccID = pMsg->tcc_no;
				cmd_result.cmd.nTccLineID = pMsg->tcc_line_no;
				cmd_result.cmd.nTccSenderID = pMsg->tcc_sender;
				cmd_result.cmd.bCancelLimit = pMsg->bCancelLimit;
				cmd_result.cmd.nTccKmMarkFlag[0] = ((pMsg->tcc_km_marker[0].nKmsysCode >> 6) & 0x03);
				cmd_result.cmd.nTccKmMarkFlag[1] = ((pMsg->tcc_km_marker[1].nKmsysCode >> 6) & 0x03);

				cmd_result.result = pMsg->result;

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, sizeof(buf));
				cmd_result.strComment = CString(buf, sizeof(buf));

				bool b = false;
				if(HWND hWnd = TCC_C3::GetTccCommandSendDlgHandle())
				{
					if(::IsWindow(hWnd))
					{
						//b = TCC_C3::Receive_TccCommandResult(cmd_result, hWnd);
						b = ::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&cmd_result);
					}
				}
				if(!b)
				{
					CString str, szLog;
					szLog.Format("[TCC]: ���������%d, վ��%d���пز������������%d(%s)������ڹ���رգ�������״̬���ܲ���ȷ!", 
						cmd_result.cmd.nCmdID, cmd_result.cmd.nTccID,
						cmd_result.result, cmd_result.strComment);
					GpcLogDebugMessage(0, szLog);

					//ִ�гɹ���,�޸ķ�������Ӧ�����״̬, ������Ϣ����ΪCheckorID��ֵ
					if(4 == cmd_result.result)
					{
						CLBTH::MsgTccCommandStatusChanged msg;

						msg.console_no = cmd_result.cmd.nConsoleID;
						TCC_C3::SysidToByteArray(cmd_result.cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));		// ϵͳ��ʶ
						msg.cmd_no = cmd_result.cmd.nCmdID;
						msg.tcc_no = cmd_result.cmd.nTccID;

						msg.b_status_changed = TRUE;
						msg.tcc_status = TCC_C3::EXECUTING;

						msg.b_sender_set = TRUE;
						msg.tcc_sender = cmd_result.cmd.nTccSenderID;	// ����վ������

						msg.b_time_set = TRUE;
						time_t tm;
						time(&tm);
						msg.tcc_time = tm;

						SendBMsg(&msg, 0);

						TCC_C3::ITccClientInfoProvider *pInfoProvider = NULL;

						pInfoProvider = ((CWintg *)AfxGetApp())->GetMainFrame()->m_pTcc3InfoProvider;

						CString szStaName;
						if(pInfoProvider)
							szStaName = pInfoProvider->GetStationName(msg.tcc_no);

						GpcLogTrainMessage(0, msg.tcc_no, 
							"[TCC]: ���÷�����ȷ������״̬ �����=%d sysid=%s վ=%s[%d] ״̬:��ִ��[%d]",
							msg.cmd_no, TCC_C3::SysidToString(cmd_result.cmd.sys_id),
							szStaName, msg.tcc_no,
							msg.tcc_status);
					}
				}
			}
		}
		else if (pMsgStruct->msg_id==MSGID_MsgTccCommand2)
		{
			CLBTH::MsgTccCommand2  *pMsg = (CLBTH::MsgTccCommand2  *)pBase;
			// ����Ϣ����ȡȷ������
			TCC_C3::STOCK_COMMAND cmd;
			cmd.dcmd_id = (char*)pMsg->dcmd_no;
			cmd.dcmd_send_entity = pMsg->dcmd_entity;	//������������
			cmd.dcmd_create_time = pMsg->create_time;

			cmd.nConsoleID = pMsg->console_no;
			TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
			TCC_C3::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
			cmd.nCmdID = pMsg->cmd_no;
			cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// ��·��
			cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
			cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// ��ʼ����������꣨��mΪ��λ��
			cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
			cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
			cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
			cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
			cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
			cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
			cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
			cmd.bStartNow  = pMsg->bStartNow;		// �Ƿ�������ʼ
			cmd.bEndless   = pMsg->bEndless;		// �Ƿ�������Ч
			cmd.nTime[0] = pMsg->time[0];		// ��ʼ������ʱ��
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// ����ֵ
			cmd.bCancelLimit = pMsg->bCancelLimit;		// �Ƿ� ȡ����������

			cmd.nCreatorID = pMsg->creator;		// ԭʼ���������
			cmd.nCheckorID = pMsg->checkor;		// ����У����
			cmd.nConfirmorID = pMsg->confirmor;	// ȷ�Ϸֽ���
			cmd.nCreateTime = pMsg->create_time;

			cmd.tcc.resize(pMsg->tcc_num);		// ����վ����
			for(int i=0; i<cmd.tcc.size(); i++)
			{
				cmd.tcc[i].nID = pMsg->tcc_no[i];		// ����վ���пأ�
				cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // ����վ��·
				cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
				cmd.tcc[i].nTime = pMsg->tcc_time[i];
				cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// ����վ����״̬
				cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// ����վ�����ˣ���վ���������е���
				cmd.tcc[i].nKmMarkFlag[0] = ((pMsg->tcc_km_marker[i][0].nKmsysCode >> 6) & 0x03);
				cmd.tcc[i].nKmMarkFlag[1] = ((pMsg->tcc_km_marker[i][1].nKmsysCode >> 6) & 0x03);
			}

			// ȷ������UI��ʾ
			//TCC_C3::Receive_StockCommand(cmd);
			::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
		}
		else if (pMsgStruct->msg_id==MSGID_MsgTccCommandData2)
		{
			CLBTH::MsgTccCommandData2  *pMsg = (CLBTH::MsgTccCommandData2  *)pBase;
			if (1)
			{
				// ����Ϣ����ȡȷ������
				TCC_C3::STOCK_COMMAND cmd;
				cmd.dcmd_id = (char*)pMsg->dcmd_no;
				cmd.dcmd_send_entity = pMsg->dcmd_entity;	//������������
				cmd.dcmd_create_time = pMsg->create_time;

				cmd.nConsoleID = pMsg->console_no;
				TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
				TCC_C3::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
				cmd.nCmdID = pMsg->cmd_no;
				cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
				cmd.nStaID[1] = pMsg->sta_no[1];
				cmd.nLineID = pMsg->line_no;		// ��·��
				cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
				cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// ��ʼ����������꣨��mΪ��λ��
				cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
				cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
				cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
				cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
				cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
				cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
				cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
				cmd.bStartNow  = pMsg->bStartNow;		// �Ƿ�������ʼ
				cmd.bEndless   = pMsg->bEndless;		// �Ƿ�������Ч
				cmd.nTime[0] = pMsg->time[0];		// ��ʼ������ʱ��
				cmd.nTime[1] = pMsg->time[1];
				cmd.nSpeedMaxVal = pMsg->speed;			// ����ֵ
				cmd.bCancelLimit = pMsg->bCancelLimit;		// �Ƿ� ȡ����������

				cmd.nCreatorID = pMsg->creator;		// ԭʼ���������
				cmd.nCheckorID = pMsg->checkor;		// ����У����
				cmd.nConfirmorID = pMsg->confirmor;	// ȷ�Ϸֽ���
				cmd.nCreateTime = pMsg->create_time;

				cmd.tcc.resize(pMsg->tcc_num);		// ����վ����
				for(int i=0; i<cmd.tcc.size(); i++)
				{
					cmd.tcc[i].nID = pMsg->tcc_no[i];		// ����վ���пأ�
					cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // ����վ��·
					cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
					cmd.tcc[i].nTime = pMsg->tcc_time[i];
					cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// ����վ����״̬
					cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// ����վ�����ˣ���վ���������е���
					cmd.tcc[i].nKmMarkFlag[0] = ((pMsg->tcc_km_marker[i][0].nKmsysCode >> 6) & 0x03);
					cmd.tcc[i].nKmMarkFlag[1] = ((pMsg->tcc_km_marker[i][1].nKmsysCode >> 6) & 0x03);
				}

				// ȷ������UI��ʾ
				switch(pMsg->type)
				{
				case CLBTH::MsgTccCommandData2::QUERY_CURRENT:
					//TCC_C3::Receive_StockCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
					break;
				case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
					//TCC_C3::Receive_HistoryCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 1, (LPARAM)&cmd);
					break;
				default:
					break;
				}
			}
		}
		else if(pMsgStruct->msg_id==MSGID_MsgStccCommandIDRequest)
		{
			CLBTH::MsgStccCommandIDRequest *pMsg = (CLBTH::MsgStccCommandIDRequest *)pBase;
			bool bValid;
			switch(pMsg->nStatus)
			{
			case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_SUCCESS:
				bValid = true;
				break;
			case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_FAIL:
				bValid = false;
				break;
			default:
				return CLBTH::HRCODE(CLBTH::HRCODE::HC_FAIL);
				break;
			}

			TCC_C3::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void TccInitComplete(DWORD nTccID)
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
		::SendMessage(hWnd, UM_TCC_INITCOMPLETE, nTccID, 0);
}

LRESULT CTccDlg::OnTccInitComplete(WPARAM wParam, LPARAM lParam)
{
	//if(HWND hWnd = GetTccCommandSendDlgHandle())
	//{
	//	::MessageBox(hWnd, "��ǰ��׼����ʼ���п�, ����ɺ��ٲ���!", "�пس�ʼ��", MB_OK);
	//	return FALSE;
	//}

	stano_t nTccID = (stano_t)wParam;
	m_sheet.SelectPage(0);
	for(int i=0; i<m_TccList.GetCount(); i++)
	{
		if(m_TccList.GetItemData(i) == nTccID)
		{
			m_TccList.SetCurSel(i);
			OnSelectTcc();
			//m_LircCommandPage.OnTccInitComplete();

			return TRUE;
		}
	}
	return FALSE;
}

void TccDialogUpdate(DWORD dwReason)
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
		::SendMessage(hWnd, UM_CLIENTENV_CHANGED, dwReason, 0);
}

LRESULT CTccDlg::OnClientEnvChanged(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case STATIONLIST_CHANGED:
		break;
	case LOC_COMMAND_STORE_CHANGED:
		break;
	}
	return TRUE;
}


// �յ����ɻ��������п�ʵʱ��������
//BOOL Receive_TccCommand(TCC_COMMAND& cmd, DWORD nStatus)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND, nStatus, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveTccCommand(WPARAM wParam, LPARAM lParam)
{
	TCC_COMMAND& cmd = *(TCC_COMMAND*)lParam;
	if(cmd.nTccID != m_nCurrentTccID)
		return FALSE;

	DWORD nStatus = wParam;
	m_LircCommandPage.AppendCommand(cmd, nStatus);

	return TRUE;
}

//BOOL Receive_TccCommandResult(TCC_COMMAND_RESULT& cmd_result, HWND hWnd)
//{
	/*HWND hTccDlgWnd = GetTccDialogWndHandle();
	if(::IsWindow(hTccDlgWnd))
	{
		CTccDlg *pDlg = (CTccDlg*)CWnd::FromHandle(hTccDlgWnd);
		ITccClientInfoProvider *pInfoProvider = pDlg->GetTccClientInfoProvider();

		USHORT nTaskId;
		OsGetTaskId(&nTaskId);

		CString szLog, sz;
		szLog.Format("���ٲ����ķ���: �����=%d վ��=%d ��·��=%d �����=%s[%d]----%s[%d] ���=%d Comment=%s",
			cmd_result.cmd.nCmdID, cmd_result.cmd.nTccID, cmd_result.cmd.nTccLineID,
			cmd_result.cmd.nKmMark[0].toString(pInfoProvider, pInfoProvider->GetLineID(cmd_result.cmd.nTccID, cmd_result.cmd.nTccLineID)), cmd_result.cmd.nTccKmMarkFlag[0],
			cmd_result.cmd.nKmMark[1].toString(pInfoProvider), cmd_result.cmd.nTccKmMarkFlag[1],
			cmd_result.result, cmd_result.strComment
			);

		GpcLogTrainMessage(nTaskId, cmd_result.cmd.nTccID, "%s", szLog);
	}*/

	//if(::IsWindow(hWnd))
	//{
	//	return ::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&cmd_result);
	//}
	//return FALSE;
//}

LRESULT CTccDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	TCC_COMMAND_RESULT *pdata = (TCC_COMMAND_RESULT*)lParam;
	if(pdata->cmd.nTccID == m_nCurrentTccID)
	{
		m_ConfirmCommandPage.SetCommandOperResult(pdata->cmd, pdata->result, pdata->strComment);
	}

	return TRUE;
}

//struct _TCCLINE_INITIAL_COMMAND_RESULT {
//	stano_t nTccID;
//	lineid_t nTccLineID;
//	dir_t dir;
//	DWORD nStatus;
//	CString strComment;
//};
//void Receive_TccLineInitialCommandResult(stano_t nTccID, lineid_t nTccLineID, dir_t dir, DWORD nStatus, LPCSTR strComment)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		_TCCLINE_INITIAL_COMMAND_RESULT data;
//		data.nTccID = nTccID;
//		data.nTccLineID = nTccLineID;
//		data.dir = dir;
//		data.nStatus = nStatus;
//		data.strComment = strComment;
//		::SendMessage(hWnd, UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, 0, (LPARAM)&data);
//	}
//}

LRESULT CTccDlg::OnReceiveTccLineInitialCommandResult(WPARAM wParam, LPARAM lParam)
{
	_TCCLINE_INITIAL_COMMAND_RESULT *pdata = (_TCCLINE_INITIAL_COMMAND_RESULT*)lParam;
	if(pdata->nTccID == m_nCurrentTccID)
	{
	}

	return TRUE;
}

// �յ����ɻ��������п�״̬
//BOOL Receive_TccReport(DWORD nTccID, TCC_STATUS& status)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_TCC_REPORT, nTccID, (LPARAM)&status);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveTccReport(WPARAM wParam, LPARAM lParam)
{
	m_LircCommandPage.Tcc_Report((DWORD)wParam, *(TCC_STATUS*)lParam);
	return TRUE;
}

//BOOL Receive_StockCommand(STOCK_COMMAND& cmd)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

//BOOL Receive_HistoryCommand(STOCK_COMMAND& cmd)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 1, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveConfirmCommand(WPARAM wParam, LPARAM lParam)
{
	STOCK_COMMAND& cmd = *(STOCK_COMMAND*)lParam;

	TCC_COMMAND tcccmd;
	if(!cmd.GetTccCommand(&tcccmd, m_nCurrentTccID))
		return FALSE;

	if(RAW_STOCK_COMMAND *pRawCmd = m_LocalCommandPage.GetRawCommand(cmd.raw_sysid))
	{
		BOOL bChanged = FALSE;
		BOOL bCancelAll = TRUE;
		for(int i=0; i<cmd.tcc.size(); i++)
		{
			if(pRawCmd->SetStatus(cmd.sys_id, cmd.tcc[i].nID, cmd.tcc[i].nStatus))
				bChanged = TRUE;

			if(pRawCmd->SetSender(cmd.sys_id, cmd.tcc[i].nID, cmd.tcc[i].nSenderID))
				bChanged = TRUE;
				
			if(CANCELED == cmd.tcc[i].nStatus && bCancelAll)
				bCancelAll = FALSE;
		}
		if(bChanged)
		{
			m_LocalCommandPage.AppendCommand(*pRawCmd);
			// ��������״̬��ԭʼ�����ļ�
			CString szFilename;
			szFilename.Format("%s\\%s.%s", 
				m_pTccClientInfoProvider->GetTccDataDirectory(), 
				GetCommandExtraFileName(cmd.raw_sysid, cmd.nCmdID),
				GetCommandExtFileName());
			SaveCommand(*pRawCmd, szFilename);
		}
	}


	// ɾ����������ȫ��ȡ��������

	switch(wParam)
	{
	case 0:
		m_ConfirmCommandPage.AppendCommand(cmd);
		break;
	case 1:
		m_HistoryCommandPage.AppendCommand(cmd);
		break;
	default:
		break;
	}

	return TRUE;
}

int CTccDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	CoInitialize(NULL);
	hWndTccUI = GetSafeHwnd();

	return 0;
}

void CTccDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB1, m_sheet);
	DDX_Control(pDX, IDC_TCC_LIST, m_TccList);
}

void CTccDlg::OnSelectTcc()
{
	INT nIndex = m_TccList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	m_nCurrentTccID = (stano_t)m_TccList.GetItemData(nIndex);
	OnRefreshTcc();
}

void CTccDlg::OnRefreshTcc()
{
	if(CB_ERR == m_TccList.GetCurSel())
	{
		MessageBox("��ѡ��վ!");
		return ;
	}

	m_LircCommandPage.Refresh();
	m_ConfirmCommandPage.Refresh();
	m_HistoryCommandPage.Refresh();
}

void CTccDlg::OnDirectLimit()
{
	CSuperPassDlg  passdlg;
	passdlg.m_strPrompt = "����������";
	
	if (passdlg.DoModal() == IDOK)
	{
		if(passdlg.m_super_pass.Compare("1234") != 0)
		{
			MessageBox("�������!","����", MB_OK);
			return;
		}
	}
	else
	{
		return;
	}


	CDirectLimit dlg(GetTccClientInfoProvider(), GetTccAdviseSink(), this);
	dlg.DoModal();
}

void CTccDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	CoUninitialize();
	hWndTccUI = NULL;
}

void CTccDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(m_bAutoDelete)
		delete this;

	CDialog::PostNcDestroy();
}

void CTccDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������
	if(m_pTccClientInfoProvider->IsStationListChanged() )
	{
		// �п������б�
		ResetTccList(m_TccList, m_pTccClientInfoProvider);
	}
}

};

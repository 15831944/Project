// TCC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "TCC_Dlg.h"
#include <algorithm>

namespace TCC {

static HWND hWndTccUI = NULL;
HWND GetTccDialogWndHandle()
{
	return hWndTccUI;
}

void RegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList,0);//316
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport,0);// 306
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult,0); //314
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand,0);// 307
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2,0);//361
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest,0);//413
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
}
	
void UnRegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList);//316
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport);// 306
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult); //314
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand);// 307
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2);//361
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest);//413
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
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
	//m_sheet.AddPage("ԭʼ����", &m_LocalCommandPage, CLocalCommandPage::IDD);
	m_sheet.AddPage("��ʷ����", &m_HistoryCommandPage, CHistoryCommandPage::IDD);
	m_sheet.Show();

	CenterWindow();
    
	RegisterTCCMsgProcHandleWnd();
	return bResult;
}

BEGIN_MESSAGE_MAP(CTccDlg, CDialog)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND, OnReceiveTccCommand)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, OnReceiveTccLineInitialCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCC_REPORT, OnReceiveTccReport)
	ON_MESSAGE(UM_RECEIVE_CONFIRM_COMMAND, OnReceiveConfirmCommand)
	ON_MESSAGE(UM_CLIENTENV_CHANGED, OnClientEnvChanged)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_CBN_SELCHANGE(IDC_TCC_LIST, OnSelectTcc)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnRefreshTcc)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

struct _TCC_COMMAND_RESULT {
	TCC_COMMAND cmd;
	DWORD nStatus;
	CString strComment;
};

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

	int id = pBase->getTypeIDStatic();
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
	{
		// �յ����ɻ��������п�ʵʱ��������
		if(MSGID_MsgStccCommandList==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccCommandList  *pMsg = (CLBTH::MsgStccCommandList  *)pBase;

			TCC::TCC_COMMAND cmd;
			cmd.nConsoleID = pMsg->console_no;		// ����̨ ̨��
			TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);		// ϵͳ��ʶ
			cmd.nCmdID = pMsg->cmd_no;			// ����ţ�0~9999��
			cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// ��·��
			cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
			cmd.nKmMark[0] = pMsg->km_marker[0];	// ��ʼ����������꣨��mΪ��λ��
			cmd.nKmMark[1] = pMsg->km_marker[1];
			cmd.bStartNow = pMsg->bStartNow;		// �Ƿ�������ʼ
			cmd.bEndless = pMsg->bEndless;		// �Ƿ�������Ч
			cmd.nTime[0] = pMsg->time[0];		// ��ʼ������ʱ��
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// ����ֵ

			cmd.nTccID = pMsg->tcc_no;			// ����վ���пأ�
			cmd.nTccLineID = pMsg->tcc_line_no;	// ����վ��·
			cmd.nTccSenderID = pMsg->tcc_sender;	// ����վ������

			::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND, 0, (LPARAM)&cmd);
		}
		// �п�״̬
		else if (MSGID_MsgStccReport==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccReport  *pMsg = (CLBTH::MsgStccReport  *)pBase;

			TCC::TCC_STATUS status;
			int nTccID = pMsg->sta_no;	// վ��
			status.comm_active = pMsg->comm_active;	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
			for(int i=0; i<2; i++)
				status.stcc_master[i] = pMsg->stcc_master[i];	// �п�����״̬
			for(i=0; i<2; i++)
				status.ilock_channel[i] = pMsg->ilock_channel[i];	// ����ͨ��״̬
			for(i=0; i<2; i++)
				status.ctc_channel[i] = pMsg->ctc_channel[i];	// CTCͨ��״̬
			for(i=0; i<4; i++)
				status.leu_channel[i]= pMsg->leu_channel[i];	// LEUͨ��״̬


			for(i=0; i<4; i++)
				status.leu_ack[i]  = pMsg->leu1_ans[i];	// LEU1Ӧ����״̬
			for(i=0; i<4; i++)
				status.leu_ack[4+i]= pMsg->leu2_ans[i];	// LEU2Ӧ����״̬
			for(i=0; i<4; i++)
				status.leu_ack[8+i]= pMsg->leu3_ans[i];	// LEU3Ӧ����״̬
			for(i=0; i<4; i++)
				status.leu_ack[12+i]= pMsg->leu4_ans[i];	// LEU4Ӧ����״̬

			status.init_section_num = pMsg->init_section_num;
			for(i=0; i<status.init_section_num; i++)
			{
				status.init_line_no[i] = pMsg->init_line_no[i];

				status.init_direction[i] = TCC::DIR_UNKNOWN;
				switch(pMsg->init_direction[i])
				{
				case 0:
					status.init_direction[i] = TCC::DIR_UP;
					break;
				case 1:
					status.init_direction[i] = TCC::DIR_DOWN;
					break;
				default:
					break;
				}
			}

			::SendMessage(hWnd, UM_RECEIVE_TCC_REPORT, nTccID, (LPARAM)&status);
		}
		else if (MSGID_MsgStccCommandSendResult==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccCommandSendResult  *pMsg = (CLBTH::MsgStccCommandSendResult  *)pBase;
			if(pMsg->bInitOpration)
			{
				TCC::dir_t dir = TCC::DIR_UNKNOWN;
				switch(pMsg->section_dir)
				{
				case 0:
					dir = TCC::DIR_UP;
					break;
				case 1:
					dir = TCC::DIR_DOWN;
					break;
				case 2:
					dir = TCC::DIR_DUAL;
					break;
				default:
					break;
				}

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, 1024);
				//AfxMessageBox(buf);

				//TCC::Receive_TccLineInitialCommandResult(pMsg->tcc_no, pMsg->tcc_line_no, dir, pMsg->result, buf);
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
				TCC::TCC_COMMAND cmd;
				cmd.nConsoleID = pMsg->console_no;
				TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, sizeof(pMsg->sys_id));
				cmd.nCmdID = pMsg->cmd_no;		// �����(0~9999)
				cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼվ
				cmd.nStaID[1] = pMsg->sta_no[1];		// ����վ
				cmd.nLineID = pMsg->line_no;		// ��·��
				cmd.bFullLimit = pMsg->bFullLimit;	// �Ƿ�ȫ������
				cmd.nKmMark[0] = pMsg->km_marker[0];	// ��ʼ�����
				cmd.nKmMark[1] = pMsg->km_marker[1];	// ���������
				cmd.bStartNow = pMsg->bStartNow;		// �Ƿ�������ʼ
				cmd.bEndless = pMsg->bEndless;		// �Ƿ�������Ч
				cmd.nTime[0] = pMsg->time[0];			// ��ʼʱ��(��)
				cmd.nTime[1] = pMsg->time[1];			// ����ʱ��(��)
				cmd.nSpeedMaxVal = pMsg->speed;		

				cmd.nTccID = pMsg->tcc_no;
				cmd.nTccLineID = pMsg->tcc_line_no;
				cmd.nTccSenderID = pMsg->tcc_sender;
				cmd.bCancelLimit = pMsg->bCancelLimit;

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, 1024);
				//AfxMessageBox(buf);

				//TCC::Receive_TccCommandResult(cmd, pMsg->result, buf);
				_TCC_COMMAND_RESULT data;
				data.cmd = cmd;
				data.nStatus = pMsg->result;
				data.strComment = buf;
				::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&data);
			}
		}
		else if (MSGID_MsgTccCommand==pMsgStruct->msg_id)
		{
			CLBTH::MsgTccCommand  *pMsg = (CLBTH::MsgTccCommand  *)pBase;
			// ����Ϣ����ȡȷ������
			TCC::STOCK_COMMAND cmd;
			cmd.nConsoleID = pMsg->console_no;
			TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
			TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
			cmd.nCmdID = pMsg->cmd_no;
			cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// ��·��
			cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
			cmd.nKmMark[0] = pMsg->km_marker[0];	// ��ʼ����������꣨��mΪ��λ��
			cmd.nKmMark[1] = pMsg->km_marker[1];
			cmd.bStartNow  = pMsg->bStartNow;		// �Ƿ�������ʼ
			cmd.bEndless   = pMsg->bEndless;		// �Ƿ�������Ч
			cmd.nTime[0] = pMsg->time[0];		// ��ʼ������ʱ��
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// ����ֵ
			cmd.bCancelLimit = pMsg->bCancelLimit;		// �Ƿ� ȡ����������

			cmd.nCreatorID = pMsg->creator;		// ԭʼ���������
			cmd.nCheckorID = pMsg->checkor;		// ����У����
			cmd.nConfirmorID = pMsg->confirmor;	// ȷ�Ϸֽ���

			cmd.tcc.resize(pMsg->tcc_num);		// ����վ����
			for(int i=0; i<cmd.tcc.size(); i++)
			{
				cmd.tcc[i].nID = pMsg->tcc_no[i];		// ����վ���пأ�
				cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // ����վ��·
				cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
				cmd.tcc[i].nTime = pMsg->tcc_time[i];
				cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// ����վ����״̬
				cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// ����վ�����ˣ���վ���������е���
			}

			// ȷ������UI��ʾ
			//TCC::Receive_StockCommand(cmd);
			::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
		}
		else if (MSGID_MsgTccCommandData2==pMsgStruct->msg_id)
		{
			CLBTH::MsgTccCommandData2  *pMsg = (CLBTH::MsgTccCommandData2  *)pBase;
			if (1)
			{
				//����C2�Ľ������
				TCC::STOCK_COMMAND cmd;
				cmd.dcmd_id = (char*)pMsg->dcmd_no;
				cmd.dcmd_send_entity = pMsg->dcmd_entity;	//������������
				cmd.dcmd_create_time = pMsg->create_time;

				cmd.nConsoleID = pMsg->console_no;
				TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
				TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
				cmd.nCmdID = pMsg->cmd_no;
				cmd.nStaID[0] = pMsg->sta_no[0];		// ��ʼ������վ��
				cmd.nStaID[1] = pMsg->sta_no[1];
				cmd.nLineID = pMsg->line_no;		// ��·��
				cmd.bFullLimit = pMsg->bFullLimit;		// �Ƿ�ȫ������
				cmd.nKmMark[0] = (pMsg->km_marker[0].val & 0x0FFFFFF) + ((pMsg->km_marker[0].k_cl & 0x3F) << 24) + ((pMsg->km_marker[0].m_cl & 0x01) << 30);	// ��ʼ����������꣨��mΪ��λ��
				cmd.nKmMark[1] = (pMsg->km_marker[1].val & 0x0FFFFFF) + ((pMsg->km_marker[1].k_cl & 0x3F) << 24) + ((pMsg->km_marker[1].m_cl & 0x01) << 30);
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
				}

				// ȷ������UI��ʾ
				switch(pMsg->type)
				{
				case CLBTH::MsgTccCommandData2::QUERY_CURRENT:
					//TCC::Receive_StockCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
					break;
				case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
					//TCC::Receive_HistoryCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 1, (LPARAM)&cmd);
					break;
				default:
					break;
				}
			}
		}
		else if(MSGID_MsgStccCommandIDRequest==pMsgStruct->msg_id)
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

			TCC::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
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


//void Receive_TccCommandResult(TCC_COMMAND& cmd, DWORD nStatus, LPCSTR strComment)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		_TCC_COMMAND_RESULT data;
//		data.cmd = cmd;
//		data.nStatus = nStatus;
//		data.strComment = strComment;
//		::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&data);
//	}
//}

LRESULT CTccDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	_TCC_COMMAND_RESULT *pdata = (_TCC_COMMAND_RESULT*)lParam;
	if(pdata->cmd.nTccID == m_nCurrentTccID)
	{
		m_ConfirmCommandPage.SetCommandOperResult(pdata->cmd, pdata->nStatus, pdata->strComment);
	}

	return TRUE;
}


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
		m_LircCommandPage.SetTccLineInitialCommandOperResult(
			pdata->nTccID, 
			pdata->nTccLineID, 
			pdata->dir,
			pdata->nStatus,
			pdata->strComment);
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

	/*if(RAW_STOCK_COMMAND *pRawCmd = m_LocalCommandPage.GetRawCommand(cmd.raw_sysid))
	{
		BOOL bChanged = FALSE;
		BOOL bCancelAll = TRUE;
		for(int i=0; i<cmd.tcc.size() && i<8; i++)
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
	}*/


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

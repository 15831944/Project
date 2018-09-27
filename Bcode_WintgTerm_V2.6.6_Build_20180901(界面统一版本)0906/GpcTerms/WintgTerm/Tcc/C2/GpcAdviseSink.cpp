// TCC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GpcAdviseSink.h"
#include <Tcc\BMsgSend.h>

#include <sysshare.h>
#include <oscalls.h>
#include <widgets.h>
#include <MsgDispatch.h>
#include "..\TCC_COMMON_FILE.h"
	
namespace TCC
{


INT CTccGpcAdviseSink::OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir,HWND hTccDlg)
{
	CLBTH::MsgStccCommandSend msg;

	//msg.console_no = cmd.nConsoleID;
	//TCC::SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	//msg.cmd_no = cmd.nCmdID;		// �����(0~9999)
	//msg.sta_no[0] = cmd.nStaID[0];		// ��ʼվ
	//msg.sta_no[1] = cmd.nStaID[1];		// ����վ
	//msg.line_no = cmd.nLineID;		// ��·��
	//msg.bFullLimit = cmd.bFullLimit;	// �Ƿ�ȫ������
	//msg.km_marker[0] = cmd.nKmMark[0];	// ��ʼ�����
	//msg.km_marker[1] = cmd.nKmMark[1];	// ���������
	//msg.bStartNow = cmd.bStartNow;		// �Ƿ�������ʼ
	//msg.bEndless = cmd.bEndless;		// �Ƿ�������Ч
	//msg.time[0] = cmd.nTime[0];			// ��ʼʱ��(��)
	//msg.time[1] = cmd.nTime[1];			// ����ʱ��(��)
	//msg.speed = cmd.nSpeedMaxVal;		// ����ֵ(5KM/H�ı���)

	msg.tcc_no = nTccID;
	msg.tcc_line_no = nTccLineID;
	msg.tcc_sender = 0;
	//msg.bCancelLimit = cmd.bCancelLimit;

	msg.bInitOpration = TRUE;
	switch(dir)
	{
	case DIR_DOWN:
		msg.section_dir = 1;
		break;
	case DIR_UP:
		msg.section_dir = 0;
		break;
	case DIR_DUAL:
		msg.section_dir = 2;
		break;
	default:
		return -1;
		break;
	}

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(), entity))
	{
		msg.tcc_sender = entity.nEntityId;
	}

	CString prompt;
	INT nRet = 1;

	do {
		if(0 == nTccID || !gpcdef.GetEntityInfo(102, nTccID, entity))
		{
			prompt.Format("վ%d �Ҳ������ɻ�, ����ʧ��!", nTccID);
			break;
		}

		//��վ���ɻ�û���߼����ܣ���������վ�����ɻ�
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("վ%d �Ҳ�������վ�����ɻ�, ����ʧ��!", nTccID);
			break;
		}

		SendBMsg(&msg,entity.nEntityId);


		GpcLogTrainMessage(0, nTccID, 
			"[TCC]: send ���ٳ�ʼ������ to �п�=%s[%d] ��·=%s[%d] ����=%d ", 
			m_pInfoProvider->GetStationName(nTccID), nTccID, 
			m_pInfoProvider->GetTccLineName(nTccID, nTccLineID), nTccLineID, 
			dir);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}

// ����TCC_COMMAND���п�
INT CTccGpcAdviseSink::OnSendTccCommand(TCC_COMMAND& cmd, HWND hTccDlg)
{
	ASSERT(cmd.nTccID != 0);

	CLBTH::MsgStccCommandSend msg;
	msg.bInitOpration = FALSE;

	msg.console_no = cmd.nConsoleID;
	SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	msg.cmd_no = cmd.nCmdID;		// �����(0~9999)
	msg.sta_no[0] = cmd.nStaID[0];		// ��ʼվ
	msg.sta_no[1] = cmd.nStaID[1];		// ����վ
	msg.line_no = cmd.nLineID;		// ��·��
	msg.bFullLimit = cmd.bFullLimit;	// �Ƿ�ȫ������
	msg.km_marker[0] = cmd.nKmMark[0].GetDWORD();	// ��ʼ�����
	msg.km_marker[1] = cmd.nKmMark[1].GetDWORD();	// ���������
	msg.bStartNow = cmd.bStartNow;		// �Ƿ�������ʼ
	msg.bEndless = cmd.bEndless;		// �Ƿ�������Ч
	msg.time[0] = cmd.nTime[0];			// ��ʼʱ��(��)
	msg.time[1] = cmd.nTime[1];			// ����ʱ��(��)
	msg.speed = cmd.nSpeedMaxVal;		// ����ֵ

	msg.tcc_no = cmd.nTccID;
	msg.tcc_line_no = cmd.nTccLineID;
	msg.tcc_sender = 0;
	msg.bCancelLimit = cmd.bCancelLimit;

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
	{
		msg.tcc_sender = entity.nEntityId;
	}

	CString prompt;
	INT nRet = 1;
	do {
		if(0 == cmd.nTccID || !gpcdef.GetEntityInfo(102, cmd.nTccID, entity))
		{
			prompt.Format("վ%d �Ҳ������ɻ�, ����ʧ��!", cmd.nTccID);
			break;
		}

		//��վ���ɻ�û���߼����ܣ���������վ�����ɻ�
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("վ%d �Ҳ�������վ�����ɻ�, ����ʧ��!", cmd.nTccID);
			break;
		}

		SendBMsg(&msg, entity.nEntityId);

		CString szLog, sz;
		szLog.Format("[TCC]: send TCC_COMMAND to �п�(%s[%d]) �����(%d) ��·(%s[%d]) �����%s--%s ",
			m_pInfoProvider->GetStationName(cmd.nTccID), cmd.nTccID,
			cmd.nCmdID, 
			m_pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID,
			cmd.nKmMark[0].toString(), cmd.nKmMark[1].toString()
			);

		if(cmd.bCancelLimit)
			sz.Format("ȡ������");
		else
			sz.Format("����ֵ(%d)", cmd.nSpeedMaxVal);
		szLog += sz;

		GpcLogTrainMessage(0, cmd.nTccID, szLog);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;

	return 0;
}

// ���п����� ���������б�
INT CTccGpcAdviseSink::OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg)
{
	CLBTH::MsgStccCommandListQuery msg;

	msg.inquirer = 0;	// �����豸
	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
		msg.inquirer = entity.nEntityId;

	msg.tcc_no = nTccID;

	CString prompt;
	INT nRet = 1;
	do {
		if(0 == nTccID || !gpcdef.GetEntityInfo(102, nTccID, entity))
		{
			prompt.Format("վ%d �Ҳ������ɻ�, ����ʧ��!", nTccID);
			break;
		}

		//��վ���ɻ�û���߼����ܣ���������վ�����ɻ�
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("վ%d �Ҳ�������վ�����ɻ�, ����ʧ��!", nTccID);
			break;
		}

		SendBMsg(&msg, entity.nEntityId);
		

		GpcLogDebugMessage(0, "[TCC]: send MsgStccCommandListQuery to �п�=%s[%d]",
			m_pInfoProvider->GetStationName(nTccID), nTccID);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}



void OnCancelStockCommand(WPARAM wParam, LPARAM lParam)
{
	
	
	return;
}



// ��ʵʱ�б�ȡ�����Ҫ������У��
INT CTccGpcAdviseSink::OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg)
{
	INT nRet = -1;
	CString prompt;

	do {
		/*GPCENTITY entity;
		if(!gpcdef.GetEntityInfo(cmd.nTccSenderID, entity))
		{
		prompt.Format("������� ����ȡ��");
		break;
		}

		// �������е�
		if(1 != entity.nGroupId && 2 != entity.nGroupId)
		{
		prompt.Format("�������������е�̨");
		break;
		}
		else if(entity.nSpecial1 != cmd.nConsoleID && 0 != cmd.nConsoleID)
		{
		prompt.Format("���Ǳ�̨�·����������ȡ��!");
		break;
		}*/
		if(0 != cmd.nConsoleID)
		{
			if(m_pInfoProvider->GetConsoleID() != cmd.nConsoleID ||
				pStockCmd && pStockCmd->nConsoleID != m_pInfoProvider->GetConsoleID() )
			{
				prompt.Format("����������Ǳ�����̨�·��ģ� ��Ȩȡ��!");
				break;
			}
		}

		//if(m_pWintgForm)
		//	m_pWintgForm->SendMessage(UM_TCC_LIMITSPEED_CMD_CANCEL, (WPARAM)&cmd, (LPARAM)pStockCmd);
		// �����������ն˷���ȡ����Ϣ
		
		TCC::RAW_STOCK_COMMAND c2rawcmd;

		if(pStockCmd != NULL)
		{
			pStockCmd->ComposeRawStcokCommand(c2rawcmd);
		}
		else
		{
			cmd.ComposeRawStcokCommand(c2rawcmd);
		}
		
		// ���⴦��--���ɻ����������ȫ���ٹ���귵��Ϊ9999999,���ǿ���Ϊ0000000
		if(c2rawcmd.bFullLimit)
		{
			c2rawcmd.nKmMark[0].val = 0;
			c2rawcmd.nKmMark[1].val = 0;
		}

		SendCancelDCmdC2(c2rawcmd, cmd.nConsoleID);			
		
		nRet = 0;
	} while(0);

	if(0 != nRet)
	{
		if(hTccDlg)
			MessageBox(hTccDlg, prompt, "ע��", MB_ICONWARNING|MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}


INT CTccGpcAdviseSink::OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg)
{
	return 0;
}


// �����������ȷ������
INT CTccGpcAdviseSink::OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg)
{
	CLBTH::MsgTccCommandAdd msg;
	msg.console_no = cmd.nConsoleID;	// ����̨ ̨��
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);		// ϵͳ��ʶ
	SysidToByteArray(cmd.raw_sysid, msg.raw_sysid, 16);// ԭʼ����ϵͳ��ʶ

	msg.cmd_no = cmd.nCmdID;
	msg.sta_no[0] = cmd.nStaID[0];		// ��ʼ������վ��
	msg.sta_no[1] = cmd.nStaID[1];

	msg.line_no   = cmd.nLineID;		// ��·��
	msg.bFullLimit= cmd.bFullLimit;		// �Ƿ�ȫ������
	msg.km_marker[0] = cmd.nKmMark[0].GetDWORD();	// ��ʼ����������꣨��mΪ��λ��
	msg.km_marker[1] = cmd.nKmMark[1].GetDWORD();
	msg.bStartNow = cmd.bStartNow;		// �Ƿ�������ʼ
	msg.bEndless  = cmd.bEndless;		// �Ƿ�������Ч
	msg.time[0]   = cmd.nTime[0];		// ��ʼ������ʱ��
	msg.time[1]   = cmd.nTime[1];
	msg.speed     = cmd.nSpeedMaxVal;	// ����ֵ
	msg.bCancelLimit = cmd.bCancelLimit;// �Ƿ� ȡ����������

	msg.creator = cmd.nCreatorID;		// ԭʼ���������
	msg.checkor = cmd.nCheckorID;		// ����У����
	msg.confirmor = cmd.nConfirmorID;	// ȷ�Ϸֽ���

	msg.tcc_num = cmd.tcc.size();		// ����վ����
	for(int i=0; i<cmd.tcc.size(); i++)
	{
		msg.tcc_no[i] = cmd.tcc[i].nID;		// ����վ���пأ�
		msg.tcc_line_no[i] = cmd.tcc[i].nTccLineID; // ����վ��·
		msg.tcc_status[i] = cmd.tcc[i].nStatus;	// ����վ����״̬
		msg.tcc_sender[i] = cmd.tcc[i].nSenderID;	// ����վ�����ˣ���վ���������е���
	}


	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg,eid);
    GpcLogTrainMessage(0,0, "[TCC]: ������� %d ����ȷ������ �����=%d sysid=%s",
		eid, cmd.nCmdID,SysidToString(cmd.sys_id));
	return 0;
}

// ɾ��������ȷ������ STOCK_COMMAND �� ����վ
// ȷ�����������վ��ȫɾ���� ��ɾ��ȷ������
INT CTccGpcAdviseSink::OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg)
{
	CLBTH::MsgTccCommandDel msg;
	msg.console_no = cmd.nConsoleID;		// ����̨ ̨��
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);
	msg.cmd_no = cmd.nCmdID;
	msg.tcc_no = nTccID;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);
	

	GpcLogTrainMessage(0,0, "[TCC]: ɾ�������� %d ȷ������ �����=%d վ��=%s[%d] sysid=%s",
		eid, cmd.nCmdID, m_pInfoProvider->GetStationName(nTccID), nTccID, SysidToString(cmd.sys_id));

	return 0;
}

// �����������ȷ�������б� STOCK_COMMAND
INT CTccGpcAdviseSink::OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag)
{
	CLBTH::MsgTccCommandQuery2 msg;
	msg.console_no = 0;
	msg.tcc_no = nTccID;
	if(0 == nTccID)
		msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_ALL;
	else
		msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_BY_TCCNO;
	if(flag == 0)
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_CURRENT;
	else
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_HISTORY;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg, eid);
	
	CString szLog, sz;
	szLog.Format("[TCC]: ��������� %d ȷ�������б� վ=%s[%d]", 
		eid, m_pInfoProvider->GetStationName(nTccID), nTccID);

	if(flag == 0)
		sz.Format(" ��ѯȷ���б�");
	else
		sz.Format(" ��ѯ��ʷ�б�");
	szLog += sz;

	GpcLogDebugMessage(0, sz);

	return 0;
}

INT CTccGpcAdviseSink::OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag)
{
	CLBTH::MsgTccCommandQuery2 msg;

	if(flag == 0)
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_CURRENT;
	else
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_HISTORY;

	msg.console_no = nConsoleID;
	msg.tcc_no = nTccID;
	memcpy(msg.dcmd_no, dcmd_id, sizeof(msg.dcmd_no));
	msg.time_scope[0] = tmScopeBegin;
	msg.time_scope[1] = tmScopeEnd;
	msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_BY_TCCNO;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg,eid);
	
	CString szLog, sz;
	szLog.Format("[TCC]: ���������ȷ�������б� վ=%s[%d]", 
		m_pInfoProvider->GetStationName(nTccID), nTccID);

	if(flag == 0)
		sz.Format(" ��ѯȷ���б�");
	else
		sz.Format(" ��ѯ��ʷ�б�");
	szLog += sz;

	GpcLogDebugMessage(0, sz);

	return 0;
}


INT CTccGpcAdviseSink::OnTccCommandFileChanged(
		LPCSTR szFilename, 
		DWORD dwOper)
{
	return 0;
}


INT CTccGpcAdviseSink::OnTccCommandStatusChanged(
		TCC_COMMAND &cmd,
		DWORD status,
		DWORD dwParam)
{
#if 0
	if(m_pWintgForm)
		((CWintgForm *)m_pWintgForm)->SetFigureStatusColor(dwParam, status);
#endif
	return 0;
}

BOOL CTccGpcAdviseSink::ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg)
{
	if(!gpcdef.IsTccCmdIDRequireServerConfirm(2))
	{
		return ITccAdviseSink::ConfirmCommandID(nConsoleID, nCommandID, nMinCommandID, nMaxCommandID, who_confirm_cmdid, dwCookie, hTccDlg);
	}

	who_confirm_cmdid = OTHER_CONFIRM_COMMAND_ID;

	CLBTH::MsgStccCommandIDRequest msg;
	msg.oper_mode = CLBTH::MsgStccCommandIDRequest::GET_CMDID;
	msg.console_no = (WORD)nConsoleID;
	msg.cmd_no = nCommandID;
	msg.min_cmd_no = nMinCommandID;
	msg.max_cmd_no = nMaxCommandID;
	msg.dwCookie = dwCookie;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg,eid);
	
	GpcLogTrainMessage(0,0, "[TCC]: ��������������������ȷ��(̨��=%d, ��ǰ�����=%d)", nConsoleID, nCommandID);

	return TRUE;
}

}

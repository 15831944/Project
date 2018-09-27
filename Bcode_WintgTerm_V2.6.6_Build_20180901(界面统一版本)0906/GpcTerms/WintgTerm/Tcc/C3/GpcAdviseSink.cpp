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
		
namespace TCC_C3
{


INT CTccGpcAdviseSink::OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir,HWND hTccDlg)
{
	CLBTH::MsgStccInitComplete msg;//		MsgStccCommandSend msgSCS;

//		msg.bCancelLimit = stTccInfo.bCancelLimit;
	msg.console_no = 0;
//		SysidToByteArray(stTccInfo.sys_id,msgSCS.sys_id,sizeof(msgSCS.sys_id));
//		msgSCS.cmd_no = stTccInfo.nCmdID;
//		msgSCS.sta_no[0] = stTccInfo.nStaID[0];
//		msgSCS.sta_no[1] = stTccInfo.nStaID[1];
//		msgSCS.line_no = stTccInfo.nLineID;
//		msgSCS.bFullLimit = stTccInfo.bFullLimit;
//		msgSCS.km_marker[0] = stTccInfo.nKmMark[0];
//		msgSCS.km_marker[1] = stTccInfo.nKmMark[1];
//		msgSCS.bStartNow = stTccInfo.bStartNow;
//		msgSCS.bEndless = stTccInfo.bEndless;
//		msgSCS.time[0] = stTccInfo.nTime[0];
//		msgSCS.time[1] = stTccInfo.nTime[1];
//		msgSCS.speed = stTccInfo.nSpeedMaxVal;
	msg.tcc_no = nTccID;
	//msg.tcc_sender = g_csStpcConfig.GetLocalEntityID();
	//msgSCS.tcc_line_no = nLineID;
	//msgSCS.bInitOpration = TRUE;
	//switch(nDir)
	//{
	//case DIR_DOWN:
	//	msgSCS.section_dir = 1;
	//	break;
	//case DIR_UP:
	//	msgSCS.section_dir = 0;
	//	break;
	//case DIR_DUAL:
	//	msgSCS.section_dir = 2;
	//	break;
	//default:
	//	break;
	//}

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

		SendBMsg(&msg, entity.nEntityId);
		

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
INT CTccGpcAdviseSink::OnSendTccCommand(TCC_COMMAND& cmd, DWORD flag, HWND hTccDlg)
{
	ASSERT(cmd.nTccID != 0);

	CLBTH::MsgStccCommandSend2 msg;
	msg.bInitOpration = FALSE;

	msg.console_no = cmd.nConsoleID;
	SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	msg.cmd_no = cmd.nCmdID;		// �����(0~9999)
	msg.sta_no[0] = cmd.nStaID[0];		// ��ʼվ
	msg.sta_no[1] = cmd.nStaID[1];		// ����վ
	msg.line_no = cmd.nLineID;		// ��·��
	msg.bFullLimit = cmd.bFullLimit;	// �Ƿ�ȫ������
	msg.km_marker[0].nKmsysCode = cmd.nKmMark[0].flag;	// ��ʼ����������꣨��mΪ��λ��
	msg.km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.km_marker[1].nKmsysCode = cmd.nKmMark[1].flag;
	msg.km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.km_marker[1].val        = cmd.nKmMark[1].info.val;
	msg.bStartNow = cmd.bStartNow;		// �Ƿ�������ʼ
	msg.bEndless = cmd.bEndless;		// �Ƿ�������Ч
	msg.time[0] = cmd.nTime[0];			// ��ʼʱ��(��)
	msg.time[1] = cmd.nTime[1];			// ����ʱ��(��)
	msg.speed = cmd.nSpeedMaxVal;		// ����ֵ(5KM/H�ı���)

	msg.tcc_no = cmd.nTccID;
	msg.tcc_line_no = cmd.nTccLineID;
	msg.tcc_sender = 0;
	msg.tcc_km_marker[0].nKmsysCode = ((cmd.nTccKmMarkFlag[0] & 0x03) << 6) + (cmd.nKmMark[0].flag & 0x3F);
	msg.tcc_km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.tcc_km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.tcc_km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.tcc_km_marker[1].nKmsysCode = ((cmd.nTccKmMarkFlag[1] & 0x03) << 6) + (cmd.nKmMark[1].flag & 0x3F);
	msg.tcc_km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.tcc_km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.tcc_km_marker[1].val        = cmd.nKmMark[1].info.val;

	msg.bCancelLimit = cmd.bCancelLimit;

	if(flag == 1)
		msg.op = 1;
	else if(flag == 2)
		msg.op = 2;
	else if(flag == 3)
		msg.op = 3;
	else
	{
		MessageBox(hTccDlg, "����ʶ�Ĳ�����־", NULL, MB_OK);
		return 1;
	}

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
	{
		msg.tcc_sender = entity.nEntityId;
		
		cmd.nTccSenderID = entity.nEntityId;//��������ɻ�����
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

		SendBMsg(&msg,entity.nEntityId);
		

		CString szLog, sz;
		szLog.Format("[TCC]: send TCC_COMMAND to �п�(%s[%d]) �����(%d) ��·(%s[%d]) �����%s--%s ",
			m_pInfoProvider->GetStationName(cmd.nTccID), cmd.nTccID,
			cmd.nCmdID, 
			m_pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID,
			cmd.nKmMark[0].toString(m_pInfoProvider, cmd.nLineID),
			cmd.nKmMark[1].toString(m_pInfoProvider, cmd.nLineID)
			);

		if(cmd.bCancelLimit)
			sz.Format("ȡ������");
		else
			sz.Format("����ֵ(%d)", cmd.nSpeedMaxVal);
		szLog += sz;

		switch(flag)
		{
		case 1:
			sz = " У��";
			break;
		case 2:
			sz = " ִ��";
			break;
		case 3:
			sz = " ��ʼ�����";
			break;
		}
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
	   SendBMsg(&msg,entity.nEntityId);
		
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
//#if 0
//		if(m_pWintgForm)
//			m_pWintgForm->SendMessage(UM_TCC_LIMITSPEED_CMD_CANCEL, (WPARAM)&cmd, (LPARAM)pStockCmd);
//#endif 
		TCC_C3::RAW_STOCK_COMMAND raw_cmd;		
		if(pStockCmd != NULL)
		{
			pStockCmd->ComposeRawStcokCommand(raw_cmd);
		}
		else
		{
			cmd.ComposeRawStcokCommand(raw_cmd);
		}
		
		raw_cmd.bCancelLimit = TRUE;
		raw_cmd.bGenCancelCommand = FALSE;
		
		if(raw_cmd.nCmdType == 0)
		{
			if (raw_cmd.nLineID <= 0)
				raw_cmd.nCmdType = 1;
			else
				raw_cmd.nCmdType = 4;
		}
		
		SendCancelDCmdC3(raw_cmd,cmd.nConsoleID,2);
		
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
	CLBTH::MsgTccCommandAdd2 msg;
	memcpy(msg.dcmd_no, cmd.dcmd_id, sizeof(msg.dcmd_no));	//���������
	msg.console_no = cmd.nConsoleID;	// ����̨ ̨��
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);		// ϵͳ��ʶ
	SysidToByteArray(cmd.raw_sysid, msg.raw_sysid, 16);// ԭʼ����ϵͳ��ʶ

	msg.cmd_no = cmd.nCmdID;			// ����ţ�0~9999��
	msg.sta_no[0] = cmd.nStaID[0];		// ��ʼ������վ��
	msg.sta_no[1] = cmd.nStaID[1];

	msg.line_no   = cmd.nLineID;		// ��·��
	msg.bFullLimit= cmd.bFullLimit;		// �Ƿ�ȫ������
	msg.km_marker[0].nKmsysCode = cmd.nKmMark[0].flag;	// ��ʼ����������꣨��mΪ��λ��
	msg.km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.km_marker[1].nKmsysCode = cmd.nKmMark[1].flag;
	msg.km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.km_marker[1].val        = cmd.nKmMark[1].info.val;
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
		msg.tcc_no[i] = cmd.tcc[i].nID;				// ����վ���пأ�
		msg.tcc_line_no[i] = cmd.tcc[i].nTccLineID; // ����վ��·
		msg.tcc_status[i] = cmd.tcc[i].nStatus;		// ����վ����״̬
		msg.tcc_sender[i] = cmd.tcc[i].nSenderID;	// ����վ�����ˣ���վ���������е���
		msg.tcc_km_marker[i][0].nKmsysCode = ((cmd.tcc[i].nKmMarkFlag[0] & 0x03) << 6) + (cmd.nKmMark[0].flag & 0x3F);
		msg.tcc_km_marker[i][0].k_cl       = cmd.nKmMark[0].info.km_cl;
		msg.tcc_km_marker[i][0].m_cl       = cmd.nKmMark[0].info.m_cl;
		msg.tcc_km_marker[i][0].val        = cmd.nKmMark[0].info.val;
		msg.tcc_km_marker[i][1].nKmsysCode = ((cmd.tcc[i].nKmMarkFlag[1] & 0x03) << 6) + (cmd.nKmMark[1].flag & 0x3F);
		msg.tcc_km_marker[i][1].k_cl       = cmd.nKmMark[1].info.km_cl;
		msg.tcc_km_marker[i][1].m_cl       = cmd.nKmMark[1].info.m_cl;
		msg.tcc_km_marker[i][1].val        = cmd.nKmMark[1].info.val;
		msg.tcc_time[i] = 0;
	}

	msg.nMarkIndex = 0;		// ����ͼ�������б�־

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);
	
	GpcLogTrainMessage(0,0, "[TCC]: ���ӷ�����ȷ������ �����=%d sysid=%s",
		cmd.nCmdID, SysidToString(cmd.sys_id));

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
	
	GpcLogTrainMessage(0,0, "[TCC]: ɾ��������ȷ������ �����=%d վ��=%s[%d] sysid=%s",
		cmd.nCmdID, m_pInfoProvider->GetStationName(nTccID), nTccID, SysidToString(cmd.sys_id));

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
	//if(m_pWintgFormWnd)
	//	((CWintgForm*)m_pWintgFormWnd)->SetFigureStatusColor(dwParam, status);

	return 0;
}


void CTccGpcAdviseSink::OnLogPrint(LPCSTR szLog)
{

	USHORT usCommQueueHandle = -1;
	USHORT nTaskId;
	OsGetTaskId(&nTaskId);

	sysprint(0, nTaskId, "[TCC]: %s", szLog);
}

void CTccGpcAdviseSink::SetStockCommandStatus(STOCKCOMMAND_STATUS_SETTING_MODE mode, STOCK_COMMAND_STATUS_CHANGE_INFO& info, HWND hTccDlg)
{
	CLBTH::MsgTccCommandSet msg;
	msg.console_no = info.nConsoleID;		// ����̨ ̨��
	SysidToByteArray(info.sys_id, msg.sys_id, sizeof(msg.sys_id));		// ϵͳ��ʶ
	msg.cmd_no = info.nCmdID;			// ����ţ�0~9999��
	msg.tcc_no = info.nTccID;			// ����վ��0Ϊ��ͬ����ŵ���������վ���

	msg.tcc_status = info.nStatus;         // �����MsgTccCommand��tcc_status��ͬ
	msg.tcc_time = info.nTime;	    // ����վ������Чʱ��

	msg.tcc_checkor = info.nCheckorID; // ����վУ����
	msg.tcc_sender = info.nConsoleID;	// ����վ�����ˣ���վ���������е���

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);

	CString szLog, sz;
	switch(mode)
	{
	case ITccAdviseSink::MAN_SET_STOCKCOMMAND_STATUS:
		sz = "[TCC]: �˹����÷�����ȷ������״̬ ";
		break;
	case ITccAdviseSink::LIRC_SET_STOCKCOMMAND_STATUS:
	default:
		sz = "[TCC]: ���÷�����ȷ������״̬ ";
		break;
	}

	GpcLogTrainMessage(0, info.nTccID, 
		"%s �����=%d sysid=%s վ=%s[%d] ״̬=%s[%d]",
		sz, info.nCmdID, SysidToString(info.sys_id),
		m_pInfoProvider->GetStationName(info.nTccID), info.nTccID,
		GetStockCommandStatusString(info.nStatus), info.nStatus);
}

BOOL CTccGpcAdviseSink::ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg)
{
	if(!gpcdef.IsTccCmdIDRequireServerConfirm(3))
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
	SendBMsg(&msg, eid);

	GpcLogTrainMessage(0,0, "[TCC]: ��������������������ȷ��(̨��=%d, ��ǰ�����=%d)", nConsoleID, nCommandID);

	return TRUE;
}

}
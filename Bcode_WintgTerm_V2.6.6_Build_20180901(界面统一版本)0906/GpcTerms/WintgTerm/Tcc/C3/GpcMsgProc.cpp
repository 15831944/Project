// TCC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tg.h"
#include <Tcc/C3/TCC.h>
#include <Tcc/C2/Tcc.h>
#include <BaseMsg/clbth.h>
#include <sysshare.h>


IMPORT_C_VISITOR(visitorofWintg);
                    				
DEFINE_BASEMSG_PROC(MsgStccCommandList2, pMsg, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

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

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccReport2, pMsg, vpParam, visitorofWintg)
{

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
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// LiRC������������ķ���
                 
DEFINE_BASEMSG_PROC(MsgStccCommandSendResult2, pMsg, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
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

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// ����������ȷ�����������б�
// ����ṹ: STOCK_COMMAND
// ����״̬�뷢���˵�Ҫ���õ�ԭʼ����������
                    
DEFINE_BASEMSG_PROC(MsgTccCommand2, pMsg, vpParam, visitorofWintg)
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
	//TCC_C3::Receive_StockCommand(cmd);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// �ӷ���������ȷ������  
// ȷ��������������͵�Э����C3��ʽ�ģ�����C2��ֲ��C3��������ʱ������ʹ��C3Э���·���������C2ʱ����ת��������Ҫ�ĸ�ʽ
DEFINE_BASEMSG_PROC(MsgTccCommandData2, pMsg, vpParam, visitorofWintg)
{
	extern BOOL IsTCC_C3();
	if (IsTCC_C3())
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
			break;
		case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
			//TCC_C3::Receive_HistoryCommand(cmd);
			break;
		default:
			break;
		}
	}
	else
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
		//	TCC::Receive_StockCommand(cmd);
			break;
		case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
		//	TCC::Receive_HistoryCommand(cmd);
			break;
		default:
			break;
		}
	}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


/*DEFINE_BASEMSG_PROC(MsgStccCommandIDRequest, pMsg, vpParam, visitorofWintg)
{
	if(CLBTH::MsgStccCommandIDRequest::SET_CMDID != pMsg->oper_mode)
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_FAIL);

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

	extern BOOL IsTCC_C3();
	if (IsTCC_C3())
		TCC_C3::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);
	else
		TCC::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
*/
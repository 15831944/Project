// TCC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCC.h"
#include <BaseMsg/clbth.h>
#include <sysshare.h>

IMPORT_C_VISITOR(visitorofWintg);
DEFINE_BASEMSG_PROC(MsgStccCmdStatus, pMsg, vpParam, visitorofWintg)
{
	//DWORD status = TCC::TCC_UNKNOWN;// δ֪
	//switch(pMsg->status_type)
	//{
	//case CLBTH::MsgStccCmdStatus::LIRC_RECEIVED:
	//	status = TCC::LIRC_RECEIVED;		// ���ɻ��յ�
	//	break;
	//case CLBTH::MsgStccCmdStatus::LIRC_SEND_FAIL:
	//	status = TCC::LIRC_SEND_FAIL;	// ���ɻ�����ʧ��(Э��֡����)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_RECEIVED:
	//	status = TCC::TCC_RECEIVED;		// �п��յ�(Э��֡����)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_ACCEPT:
	//	status = TCC::TCC_ACCEPT;	// �޴���
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_WAITFOR_EXEC:
	//	status = TCC::TCC_WAITFOR_EXEC;	// �Ŷӣ��ȴ�ִ��
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_EXECUTING:
	//	status = TCC::TCC_EXECUTING;		// ����ִ��
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_ZOMBIE:
	//	status = TCC::TCC_ZOMBIE;		// ִ�н���(����ʱ�䵽)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CANCELED:
	//	status = TCC::TCC_CANCELED;		// ��ȡ��(�˹�, ��������)
	//	break;

	//case CLBTH::MsgStccCmdStatus::ERR_START_KM:
	//case CLBTH::MsgStccCmdStatus::ERR_END_KM:
	//case CLBTH::MsgStccCmdStatus::ERR_ALL_KM:
	//	status = TCC::TCC_ERR_KM;	// �����Ƿ�
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_SPEED:
	//	status = TCC::TCC_ERR_SPEED;	// ���ٷǷ�
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_LONG:
	//	status = TCC::TCC_ERR_REGION_LONG;	// ����̫��
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_EXIST:
	//	status = TCC::TCC_ERR_REGION_EXIST;	// �����Ѵ���
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_INVALID:
	//	status = TCC::TCC_ERR_REGION_INVALID;// ����Ƿ�
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_DUP_CMDNO:
	//	status = TCC::TCC_ERR_DUP_CMDNO;		// ������ظ�
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_TIME_FAIL:
	//	status = TCC::TCC_ERR_TIME_FAIL;		// ʱ��У��ʧ��
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_OK: // ��ȷ��
	//	status = TCC::TCC_CONFIRM_OK;
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_CANCEL:	// ��ȡ��
	//	status = TCC::TCC_CONFIRM_CANCEL;
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_TIMEOUT:	// ��ʱ��δȷ��
	//	status = TCC::TCC_CONFIRM_TIMEOUT;
	//	break;

	//case CLBTH::MsgStccCmdStatus::ERR_UNKNOWN:// δ֪����
	//	break;
	//};

	//switch(pMsg->action_type)
	//{
	//case CLBTH::MsgStccCmdStatus::COMMAND_REPLY:
	//	//TCC::Tcc_SetCmdStatus(pMsg->sys_id, pMsg->cmd_no, pMsg->sta_no, status);
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_REPORT:{
	//	//TCC::TCC_COMMAND cmd;
	//	//cmd.nSysID = pMsg->sys_id;
	//	//cmd.nCmdID = pMsg->cmd_no;
	//	//cmd.nTccLineID = pMsg->line_no;

	//	//cmd.nKmMark[0] = pMsg->km_marker[0];
	//	//cmd.nKmMark[1] = pMsg->km_marker[1];
	//	//cmd.nSpeedMaxVal = pMsg->speed;
	//	//cmd.nTime[0] = pMsg->time[0];
	//	//cmd.nTime[1] = pMsg->time[1];

	//	//TCC::Tcc_SetHistoryCmd(cmd.nCmdID, pMsg->sta_no, cmd, status);
	//	}break;
	//}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccCommandList, pMsg, vpParam, visitorofWintg)
{
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

	//TCC::Receive_TccCommand(cmd, 0);   qiaoyt del
    TRACE("\n MsgStccCommandList");
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccReport, pMsg, vpParam, visitorofWintg)
{
	TCC::TCC_STATUS status;
	pMsg->sta_no;	// վ��
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
	TRACE("\n MsgStccReport");
//	TCC::Receive_TccReport(pMsg->sta_no, status);  qiaoyt del
	
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// LiRC������������ķ���
DEFINE_BASEMSG_PROC(MsgStccCommandSendResult, pMsg, vpParam, visitorofWintg)
{
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
	}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// ����������ȷ�����������б�
// ����ṹ: STOCK_COMMAND
// ����״̬�뷢���˵�Ҫ���õ�ԭʼ����������
DEFINE_BASEMSG_PROC(MsgTccCommand, pMsg, vpParam, visitorofWintg)
{
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

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

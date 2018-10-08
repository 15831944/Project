// TccCommandSendDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
//#include "tcc.h"
#include "tcc_dlg.h"
//#include "sysruninfo.h"
#include "TccCommandSendDlg.h"

namespace TCC_C3
{

	HWND hWndTccCommandSendDlg = NULL;
	HWND GetTccCommandSendDlgHandle()
	{
		return hWndTccCommandSendDlg;
	}

// CTccCommandSendDlg �Ի���

IMPLEMENT_DYNAMIC(CTccCommandSendDlg, CDialog)
CTccCommandSendDlg::CTccCommandSendDlg(
		TCC_COMMAND& cmd, int op_flag,
		STOCK_COMMAND *pStockCmd,
		ITccClientInfoProvider *pInfoProvider,
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, m_pStockCmd(pStockCmd)
	, m_sContent(_T(""))
	, m_sStatus(_T(""))
	, m_bCanExecuteTccCommand(FALSE)
	, m_TccCommand(cmd)
	, m_opflag(op_flag)
{
	ASSERT(pInfoProvider && pAdviseSink);
	m_pInfoProvider = pInfoProvider;
	m_pInfoProvider->AddRef();

	m_pAdviseSink = pAdviseSink;
	m_pAdviseSink->AddRef();

	m_bCanClose = TRUE;
}

CTccCommandSendDlg::~CTccCommandSendDlg()
{
	if(m_pInfoProvider)
	{
		m_pInfoProvider->Release();
		m_pInfoProvider = NULL;
	}

	if(m_pAdviseSink)
	{
		m_pAdviseSink->Release();
		m_pAdviseSink = NULL;
	}
}

void CTccCommandSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TCCCMDCONTENT, m_sContent);
	DDX_Text(pDX, IDC_EDIT_TCCCMDSTATUS, m_sStatus);
}

BEGIN_MESSAGE_MAP(CTccCommandSendDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_TCCCMD_TRY_EXECUTE, OnBnClickedBtnTryExecute)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CTccCommandSendDlg ��Ϣ�������
BOOL CTccCommandSendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	hWndTccCommandSendDlg = GetSafeHwnd();

	switch(m_opflag)
	{
	case TCC_COMMAND_DIRECT:	//ֱ������
	case TCC_COMMAND_CONFIRM:	//ȷ���б�������
		SetWindowText("��ʱ���������´�");
		m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_CANCEL:	//ȡ������
		ASSERT(m_TccCommand.bCancelLimit);
		SetWindowText("��ʱ����ȡ�������´�");
		m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_FULLCANCEL:		//ǿ��ȫ��������
		ASSERT(m_TccCommand.nCmdID == 0xFFFF);
		SetWindowText("ǿ��ȫ�������������´�");
		m_sContent.Format(
			"�����: %d"
			"\r\nĿ��վ: %s(%d)"
			"\r\n��·:%s(%d)",
			m_TccCommand.nCmdID,
			m_pInfoProvider->GetStationName(m_TccCommand.nTccID), m_TccCommand.nTccID,
			m_pInfoProvider->GetTccLineName(m_TccCommand.nTccID, m_TccCommand.nTccLineID), m_TccCommand.nTccLineID
			);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_INITCOMPLETE:	//��ʼ�����
		ASSERT(m_TccCommand.nCmdID == 10000);
		SetWindowText("��ʼ����������´�");
		m_sContent.Format(
			"�����: %d"
			"\r\nĿ��վ: %s(%d)",
			m_TccCommand.nCmdID,
			m_pInfoProvider->GetStationName(m_TccCommand.nTccID), m_TccCommand.nTccID
			);
		//m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTccCommandSendDlg::OnBnClickedCancel()
{
	/*if(!m_bCanClose)
	{
		if(IDOK != MessageBox(
			"�п�ִ�н��δ������ǿ�йرմ��ں󣬷�����״̬���ܻ᲻��ȷ!",
			"�����´�",
			MB_OKCANCEL)
			)
		{
			return ;
		}
	}*/

	OnCancel();
}


void CTccCommandSendDlg::OnBnClickedBtnTryExecute()
{
	if(TCC_COMMAND_INITCOMPLETE == m_opflag)  // TCC_COMMAND_FULLCANCEL == m_opflag //Ҳ����У��
	{
		MessageBox("���������ҪУ��, ��ֱ�ӷ���");
		return ;
	}
	if(m_bCanExecuteTccCommand)
	{
		MessageBox("�Ѿ�У��ɹ�������Ҫ����У�飻�����ʱ����رձ����ں����·������");
		return ;
	}

	TCC_COMMAND tcccmd = m_TccCommand;

	m_sStatus = "׼������ ��ʱ����+У��";

	if(m_pAdviseSink->OnSendTccCommand(tcccmd, 1, m_hWnd) != 0)
	{
		m_sStatus = "���� ��ʱ����+У�� ʧ��";
		MessageBox("���� ��ʱ����У�� ʧ��");
	}
	else
	{
		m_sStatus = "�Ѿ����� ��ʱ����+У��";

		//У������ȴ���ִ
		m_bCanClose = FALSE;
	}
	UpdateData(FALSE);
}

void CTccCommandSendDlg::OnBnClickedOk()
{
	int op_flag = 2;
	switch(m_opflag)
	{
	case TCC_COMMAND_DIRECT:	//ֱ������
	case TCC_COMMAND_CONFIRM:	//ȷ���б�������
	case TCC_COMMAND_CANCEL:	//ȡ������
	default:
		op_flag = 2;
		break;
	case TCC_COMMAND_INITCOMPLETE:	//��ʼ�����
		m_bCanExecuteTccCommand = TRUE;
		op_flag = 3;
		break;
	case TCC_COMMAND_FULLCANCEL:		//ǿ��ȫ��������
		// m_bCanExecuteTccCommand = TRUE; // Ҳ����У��
		op_flag = 2;
		break;
	}
	if(!m_bCanExecuteTccCommand)
	{
		MessageBox("�п�У��ɹ���ſ��Է���!");
		return ;
	}

	m_sStatus = "׼������ ��ʱ����+ִ��";

	if(m_pAdviseSink->OnSendTccCommand(m_TccCommand, op_flag, m_hWnd) != 0)
	{
		m_sStatus = "���� ��ʱ����+ִ�� ʧ��";
		MessageBox("���� ��ʱ����ִ��ʧ��");
	}
	else
	{
		m_sStatus = "�Ѿ����� ��ʱ����+ִ��";

		//ִ�к����ȴ���ִ
		m_bCanClose = FALSE;
	}
	UpdateData(FALSE);
//	OnOK();
}

LRESULT CTccCommandSendDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	TCC_COMMAND_RESULT& cmd_result = *(TCC_COMMAND_RESULT *)lParam;
	
	if(m_TccCommand.nCmdID != cmd_result.cmd.nCmdID)
	{
		m_sStatus.Format("����Ų����(��%d, ��%d)", m_TccCommand.nCmdID, cmd_result.cmd.nCmdID);
	}
	
	else if(m_TccCommand.nTccID != cmd_result.cmd.nTccID)
	{
		m_sStatus.Format("վ�Ų����(��%d, ��%d)", m_TccCommand.nTccID, cmd_result.cmd.nTccID);
	}
	
	else if (m_TccCommand.nKmMark[0].info.val != cmd_result.cmd.nKmMark[0].info.val) //(m_TccCommand.nKmMark[0].Unequal(cmd_result.cmd.nKmMark[0]))
	{
		m_sStatus.Format("��ʼ����겻���(��%s, ��%s)", 
			m_TccCommand.nKmMark[0].toString(m_pInfoProvider, m_TccCommand.nLineID), 
			cmd_result.cmd.nKmMark[0].toString(m_pInfoProvider, m_TccCommand.nLineID));
	}

	else if (m_TccCommand.nKmMark[1].info.val != cmd_result.cmd.nKmMark[1].info.val)//(m_TccCommand.nKmMark[1].Unequal(cmd_result.cmd.nKmMark[1]))
	{
		m_sStatus.Format("��������겻���(��%s, ��%s)", 
			m_TccCommand.nKmMark[1].toString(m_pInfoProvider, m_TccCommand.nLineID), 
			cmd_result.cmd.nKmMark[1].toString(m_pInfoProvider, m_TccCommand.nLineID));
	}

	else if (m_TccCommand.nSpeedMaxVal != cmd_result.cmd.nSpeedMaxVal)
	{
		m_sStatus.Format("�ٶȲ����(��%d, ��%d)", m_TccCommand.nSpeedMaxVal, cmd_result.cmd.nSpeedMaxVal);
	}

	else if (m_TccCommand.nTccLineID != cmd_result.cmd.nTccLineID)
	{
		m_sStatus.Format("��·�Ų����(��%d, ��%d)", m_TccCommand.nTccLineID, cmd_result.cmd.nTccLineID);
	}

	else //if(m_TccCommand == cmd_result.cmd)
	{
		m_sStatus.Format("%s", cmd_result.strComment);

		// ����ִ�н��
		//     1: ���ɻ��յ�
		//     2:�п�У��ɹ�
		//     3:�п�У��ʧ��
		//     4:�п�ִ�гɹ�
		//     5:�п�ִ��ʧ��
		//     6:�п�ϵͳ�쳣
		//     7:���ɻ����ʧ�� 

		//У���Ƿ�ɹ�
		switch(cmd_result.result)
		{
		case 2:
			m_bCanExecuteTccCommand = TRUE;
			break;
		default:
			m_bCanExecuteTccCommand = FALSE;
			break;
		}

		//�Ƿ���Թرմ���
		switch(cmd_result.result)
		{
		case 2:
		case 3:
		case 4:
		case 5:
			m_bCanClose = TRUE;
			break;
		default:
			m_bCanClose = FALSE;
			break;
		}

		//���÷�������ȷ������״̬
		if(m_pStockCmd && 4 == cmd_result.result)
		{
			ITccAdviseSink::STOCK_COMMAND_STATUS_CHANGE_INFO info;
			info.nConsoleID = m_TccCommand.nConsoleID;
			info.sys_id = m_TccCommand.sys_id;
			info.nCmdID = m_TccCommand.nCmdID;
			info.nTccID = m_TccCommand.nTccID;
			info.nStatus = EXECUTING;
			time_t tm;
			time(&tm);
			info.nTime = tm;

			info.nCheckorID = 0;
			for(int i=0; i<m_pStockCmd->tcc.size(); i++)
			{
				if(m_pStockCmd->tcc[i].nID == info.nTccID)
				{
					info.nCheckorID = m_pStockCmd->tcc[i].nCheckorID;
					break;
				}
			}
			info.nSenderID  = m_TccCommand.nTccSenderID;

			//ֱ�����ٳɹ������������������һ��ȷ������
			if(m_opflag == TCC_COMMAND_DIRECT)
				m_pAdviseSink->OnSendStockCommand(*m_pStockCmd, GetSafeHwnd());

			//ִ�гɹ���,�޸ķ�������Ӧ�����״̬
			m_pAdviseSink->SetStockCommandStatus(ITccAdviseSink::LIRC_SET_STOCKCOMMAND_STATUS, info, GetSafeHwnd());
		}
	}

	UpdateData(FALSE);
	return TRUE;
}

void CTccCommandSendDlg::OnDestroy()
{
	CDialog::OnDestroy();
	hWndTccCommandSendDlg = NULL;
}

}
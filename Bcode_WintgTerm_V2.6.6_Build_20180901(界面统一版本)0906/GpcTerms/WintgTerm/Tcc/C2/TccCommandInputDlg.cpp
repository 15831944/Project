// Tcc\TccCommandInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "tcc_base.h"
#include "TccCommandInputDlg.h"
#include "PasswordInputDlg.h"
#include "CmdTypeSelectDlg.h"

#include <algorithm>

namespace TCC {

/*
 * վ�����ٺ������������û����빫���
 * ����ȫ���� ϵͳ�Զ����빫��� 0--0
 * ��������   ϵͳ�Զ����빫��� 0--9999999
 */

BOOL RawStockCommandInput(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);
	CTccCommandInputDlg dlg(NULL, FALSE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

BOOL RawStockCommandReEdit(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);
	CTccCommandInputDlg dlg(pcmd, FALSE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

BOOL RawStockCommandValid(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);

	CTccCommandInputDlg dlg(pcmd, TRUE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

// CTccCommandInputDlg �Ի���

IMPLEMENT_DYNAMIC(CTccCommandInputDlg, CDialog)
CTccCommandInputDlg::CTccCommandInputDlg(
	RAW_STOCK_COMMAND* pcmd,
	BOOL bCheckCommand,
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTccCommandInputDlg::IDD, pParent)
	, m_pInfoProvider(pInfoProvider)
	, m_pAdviseSink(pAdviseSink)
	, m_bCheckCommand(bCheckCommand)
	, m_nCmdID(0)
	, m_nKmBegin(0)
	, m_nMBegin(0)
	, m_nKmEnd(0)
	, m_nMEnd(0)
	, m_bTimeBegin(1)
	, m_bTimeEnd(1)
	, m_bCancelLimit(FALSE)
	, m_bGenCancelCommand(FALSE)
	, m_bCmdIDConfirmed(false)
	, m_bManualChangeCommandID(FALSE)
{
	if(pcmd)
	{
		m_cmd = *pcmd;
		if(0 != m_cmd.nCmdID)
			m_bCmdIDConfirmed = true;
	}
	else
	{
		if(m_bCheckCommand)
			ASSERT(FALSE);
	}
}

CTccCommandInputDlg::~CTccCommandInputDlg()
{
}

void CTccCommandInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCC_CMD_TYPE, m_CmdType);
	DDX_Control(pDX, IDC_TCC_CMDID, m_CmdID);
	DDX_Text(pDX, IDC_TCC_CMDID, m_nCmdID);
	DDV_MinMaxUInt(pDX, m_nCmdID, 0, 9999);
	DDX_Control(pDX, IDC_CHANGE_TCCCMDID, m_btnChangeCmdID);
	DDX_Control(pDX, IDC_AUTOCOMPUTE_TCCCMDID, m_btnAutoComputeCmdID);

	DDX_Control(pDX, IDC_TCC_CMD_STA, m_StaBegin);
	DDX_Control(pDX, IDC_TCC_CMD_STA2,m_StaEnd);

	DDX_Control(pDX, IDC_TCC_CMD_ABSLINE, m_AbsLine);


	DDX_Text(pDX, IDC_TCC_CMD_KM, m_nKmBegin);
	DDV_MinMaxInt(pDX, m_nKmBegin, 0, 9999);
	DDX_Text(pDX, IDC_TCC_CMD_M, m_nMBegin);
	DDV_MinMaxInt(pDX, m_nMBegin, 0, 999);
	DDX_Control(pDX, IDC_TCC_CMD_KM, m_KmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_M, m_MBegin);
	DDX_Text(pDX, IDC_TCC_CMD_KM2, m_nKmEnd);
	DDV_MinMaxInt(pDX, m_nKmEnd, 0, 9999);
	DDX_Text(pDX, IDC_TCC_CMD_M2, m_nMEnd);
	DDV_MinMaxInt(pDX, m_nMEnd, 0, 999);
	DDX_Control(pDX, IDC_TCC_CMD_KM2, m_KmEnd);
	DDX_Control(pDX, IDC_TCC_CMD_M2, m_MEnd);

	DDX_Control(pDX, IDC_TCC_CMD_LONGKM, m_StartLongKM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGKM2, m_EndLongKM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM, m_StartLongM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM2, m_EndLongM);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_bTimeBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN, m_tmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_BeginRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO2, m_BeginRadio2);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_bTimeEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND, m_tmEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_EndRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO2, m_EndRadio2);

	DDX_Check(pDX, IDC_TCC_CANCE_LIMIT, m_bCancelLimit);
	DDX_Control(pDX, IDC_TCC_CANCE_LIMIT, m_CancelLimit);
	DDX_Check(pDX, IDC_TCC_CANCELCOMMAND, m_bGenCancelCommand);
	DDX_Control(pDX, IDC_TCC_CANCELCOMMAND, m_GenCancelCommand);

	DDX_Control(pDX, IDC_TCC_LIMIT2, m_Limit[1]);
	DDX_Control(pDX, IDC_TCC_LIMIT3, m_Limit[2]);
	DDX_Control(pDX, IDC_TCC_LIMIT4, m_Limit[3]);
	DDX_Control(pDX, IDC_TCC_LIMIT5, m_Limit[4]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL,  m_LimitVal[0]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL2, m_LimitVal[1]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL3, m_LimitVal[2]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL4, m_LimitVal[3]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL5, m_LimitVal[4]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID,  m_TrainID[0]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID2, m_TrainID[1]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID3, m_TrainID[2]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID4, m_TrainID[3]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID5, m_TrainID[4]);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);
}


BEGIN_MESSAGE_MAP(CTccCommandInputDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO2, OnBtnClickedTccCmdTimebeginRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO,  OnBtnClickedTccCmdTimebeginRadio)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO2,   OnBtnClickedTccCmdTimeendRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO,    OnBtnClickedTccCmdTimeendRadio)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_TYPE, OnCbnSelchangeTccCmdType)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_STA, OnCbnSelchangeTccCmdSta)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_STA2, OnCbnSelchangeTccCmdSta2)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_ABSLINE, OnCbnSelchangeTccLine)
	ON_BN_CLICKED(IDC_TCC_CANCE_LIMIT, OnSelCancelLimit)
	ON_BN_CLICKED(IDC_TCC_LIMIT2, OnBtnClickedLimit2)
	ON_BN_CLICKED(IDC_TCC_LIMIT3, OnBtnClickedLimit3)
	ON_BN_CLICKED(IDC_TCC_LIMIT4, OnBtnClickedLimit4)
	ON_BN_CLICKED(IDC_TCC_LIMIT5, OnBtnClickedLimit5)
	ON_BN_CLICKED(IDC_CHANGE_TCCCMDID, OnChangeTccCmdID)
	ON_BN_CLICKED(IDC_AUTOCOMPUTE_TCCCMDID, OnBtnClickedAutoComputeCmdID)
	
	ON_MESSAGE(UM_SERVER_CONFIRM_COMMAND_ID, OnServerConfirmCommandID)
END_MESSAGE_MAP()

void ResetEndStationList(
	CComboBox &StaBegin,
	CComboBox &StaEnd,
	ITccClientInfoProvider *pInfoProvider)
{
	INT nIndex = StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	// ��ʼվ, ����վ
	stano_t nStaID, nStaID2;
	nStaID = (stano_t)StaBegin.GetItemData(nIndex);

	// ɾ������վ�б�����
	StaEnd.ResetContent();

	// ����վ�б������վ
	std::vector<stano_t> staList, staLinkedList;
	pInfoProvider->GetStationList(staList);
	pInfoProvider->GetLinkedStationList(nStaID, staLinkedList);

	for(int i=0; i<staLinkedList.size(); i++)
	{
		nStaID2 = staLinkedList[i];
		if(staList.end() == std::find(staList.begin(), staList.end(), nStaID2))
			continue;

		nIndex = StaEnd.AddString(pInfoProvider->GetStationName(nStaID2));
		ASSERT(CB_ERR != nIndex);
		StaEnd.SetItemData(nIndex, nStaID2);
	}
	StaEnd.SetCurSel(-1);
}

// CTccCommandInputDlg ��Ϣ�������
BOOL TCC::CTccCommandInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if(NULL == m_pInfoProvider)
	{
		MessageBox("û�а�װTccClientInfoProvider�ӿ�!");
		return FALSE;
	}

	if(!TccInitialize(m_pInfoProvider))
		return FALSE;

	if(m_bManualChangeCommandID)
		m_btnChangeCmdID.SetWindowText("�˹�ȷ��");
	else
		m_btnChangeCmdID.SetWindowText("�޸������");

	RegisterConfirmCommandIDReceiver( GetSafeHwnd() );

	m_tmBegin.SetFormat("yyy'/'MM'/'dd HH':'mm");
	m_tmEnd.SetFormat("yyy'/'MM'/'dd HH':'mm");

	CString str;
	// ��ʼ�����������б�
	INT nIndex = m_CmdType.AddString("վ������");
	m_CmdType.SetItemData(nIndex, STATION_LIMIT);
	nIndex = m_CmdType.AddString("��������");
	m_CmdType.SetItemData(nIndex, ABSAPP_LIMIT);
	nIndex = m_CmdType.AddString("����ȫ����");
	m_CmdType.SetItemData(nIndex, ABSAPP_FULL_LIMIT);
	nIndex = m_CmdType.AddString("��������");
	m_CmdType.SetItemData(nIndex, BLOCK_LIMIT);

	// ��ʼ����ʼվ������վ����·�б�
	std::vector<stano_t> staList;
	m_pInfoProvider->GetStationList(staList);
	for(int i=0; i<staList.size(); i++)
	{
		str = m_pInfoProvider->GetStationName(staList[i]);
		INT nIndex = m_StaBegin.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StaBegin.SetItemData(nIndex, staList[i]);
	}
	m_StaBegin.SetCurSel(-1);
	//OnCbnSelchangeTccCmdSta();

	// ����ֵ
	//CString limitspeed[5] = {"45", "60", "80", "120", "180" };
	extern std::vector<int> AvailableLimitSpeedList;
	for(i=0; i<AvailableLimitSpeedList.size(); i++)
	{
		str.Format("%d", AvailableLimitSpeedList[i]);
		
		//С��45km/h�Ĵ���
		if(AvailableLimitSpeedList[i] < 45 )
			str+="(45)";

		INT nIndex;
		for(int j=0; j<5; j++)
		{
			nIndex = m_LimitVal[j].AddString(str);
			ASSERT(CB_ERR != nIndex);
			m_LimitVal[j].SetItemData(nIndex, AvailableLimitSpeedList[i]);
		}
	}
	//����
	extern std::map<int,CString> LongKmMap;
	 std::map<int,CString>::iterator  iter;

	 for(iter=LongKmMap.begin(); iter!=LongKmMap.end(); iter++)
	{
		str.Format("%s", iter->second);
		INT nIndex = m_StartLongKM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StartLongKM.SetItemData(nIndex, iter->first);

		str.Format("%s", iter->second);
		nIndex = m_EndLongKM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_EndLongKM.SetItemData(nIndex, iter->first);
	}

	m_StartLongKM.SetCurSel(-1);
	m_EndLongKM.SetCurSel(-1);

	extern std::map<int,CString> LongMMap;

	for(iter=LongMMap.begin(); iter!=LongMMap.end(); iter++)
	{
		str.Format("%s", iter->second);
		INT nIndex = m_StartLongM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StartLongM.SetItemData(nIndex, iter->first);

		str.Format("%s", iter->second);
		nIndex = m_EndLongM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_EndLongM.SetItemData(nIndex, iter->first);
	}

	m_StartLongM.SetCurSel(-1);
	m_EndLongM.SetCurSel(-1);

	//end ����

	m_LimitVal[0].SetCurSel(-1);
	for(i=1; i<5; i++)
	{
		m_Limit[i].EnableWindow(FALSE);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
	}
	m_Limit[1].EnableWindow(TRUE);
	m_LimitVal[0].EnableWindow(TRUE);
	m_TrainID[0].EnableWindow(TRUE);

	CRect rcl;
	GetDlgItem(IDC_TCC_LIST)->GetWindowRect(&rcl);
	ScreenToClient(rcl);
	m_TccList.Create(rcl, this, 8341);
	InitListCtrl(m_TccList);

	m_bCancelLimit = m_cmd.bCancelLimit;
	m_CancelLimit.EnableWindow(FALSE);
	m_bGenCancelCommand = m_cmd.bGenCancelCommand;
	m_GenCancelCommand.EnableWindow(FALSE);

	//�������֮�󣬲����Ը������
	m_nCmdID = m_cmd.nCmdID;
	if(0 != m_cmd.nCmdID)
	{
		m_CmdID.EnableWindow(FALSE);
		m_btnChangeCmdID.ShowWindow(SW_HIDE);
		m_btnAutoComputeCmdID.ShowWindow(SW_HIDE);
	}

	if(m_bCheckCommand)//У��
	{
		str.Format("��������У��(%s)", SysidToString(m_cmd.raw_sysid));
		SetWindowText(str);

		m_tmBegin.EnableWindow(FALSE);
		m_tmEnd.EnableWindow(FALSE);

		OnCbnSelchangeTccCmdType();
		m_CmdString = m_cmd.toString(m_pInfoProvider);

		if(m_bCancelLimit)
		{
			m_Limit[1].EnableWindow(FALSE);
			m_LimitVal[0].EnableWindow(FALSE);
			m_TrainID[0].EnableWindow(FALSE);
		}

		UpdateData(FALSE);
	}
	else
	{
		//����У��ʱ������Ϣ��ʾ��
		CWnd *pWnd = GetDlgItem(IDC_TCC_CMD_NOTE);
		ASSERT(NULL != pWnd);

		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CRect rc;
		pWnd->GetWindowRect(&rc);
		rcWnd.bottom = rc.top;
		SetWindowPos(NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
		CenterWindow();
		pWnd->ShowWindow(SW_HIDE);

		do {
			// �༭����״̬����ԭ����ȡ��
			sysid_t sysid;
			SysidCreateNil(&sysid);
			if(m_cmd.raw_sysid == sysid)
			{
				SysidCreate(&m_cmd.raw_sysid);
			}
			if(0 == m_cmd.nCmdID)
			{
				CCmdTypeSelectDlg dlg(this);
				if(IDOK == dlg.DoModal())
				{
					m_bCancelLimit = m_cmd.bCancelLimit = FALSE;
				}
				else
				{
					m_bCancelLimit = m_cmd.bCancelLimit = TRUE;
				}

				if(!m_bCancelLimit)
				{
					m_nCmdID = m_pInfoProvider->GetCommandID();

					RequestCommandID(m_nCmdID);
				}
			}
			UpdateData(FALSE);

			str.Format("������������(%s)", SysidToString(m_cmd.raw_sysid));
			SetWindowText(str);
			
			if(m_bCancelLimit)
			{
				m_bGenCancelCommand = FALSE;
	
				m_CmdID.EnableWindow(TRUE);
				m_btnChangeCmdID.ShowWindow(SW_HIDE);
				m_btnAutoComputeCmdID.ShowWindow(SW_HIDE);

				m_LimitVal[0].EnableWindow(FALSE);
				m_TrainID[0].EnableWindow(FALSE);
				m_Limit[1].EnableWindow(FALSE);
			}
			else
				m_GenCancelCommand.EnableWindow(TRUE);

			if(m_cmd.nStaID[1] == 0)
			{
				if(m_pInfoProvider->IsTccBlockLine(m_cmd.nStaID[0], m_cmd.nLineID))
					m_CmdType.SetCurSel(3);
				else
					m_CmdType.SetCurSel(0);
			}
			else
			{
				if(m_cmd.bFullLimit)
					m_CmdType.SetCurSel(2);
				else
					m_CmdType.SetCurSel(1);
			}
			OnCbnSelchangeTccCmdType();


			// ��ʼվ
			for(int i=0; i<m_StaBegin.GetCount(); i++)
			{
				if(m_StaBegin.GetItemData(i) == m_cmd.nStaID[0])
				{
					m_StaBegin.SetCurSel(i);
					OnCbnSelchangeTccCmdSta();
					break;
				}
			}

			// ����վ
			for(i=0; i<m_StaEnd.GetCount(); i++)
			{
				if(m_StaEnd.GetItemData(i) == m_cmd.nStaID[1])
				{
					m_StaEnd.SetCurSel(i);
					OnCbnSelchangeTccCmdSta2();
					break;
				}
			}

			// ��·
			for(i=0; i<m_AbsLine.GetCount(); i++)
			{
				if(m_AbsLine.GetItemData(i) == m_cmd.nLineID)
				{
					m_AbsLine.SetCurSel(i);
					OnCbnSelchangeTccLine();
					break;
				}
			}
			//����ֵ
			if(!m_cmd.bCancelLimit)
			{
				for(i=0; i<5; i++)
				{
					if(m_cmd.train_cmd[i].nSpeedMaxVal <= 0)
						continue;

					if(i>0)	m_Limit[i].SetCheck(TRUE);
					for(int j=0; j<m_LimitVal[i].GetCount(); j++)
					{
						if(m_LimitVal[i].GetItemData(j) == m_cmd.train_cmd[i].nSpeed)
						{
							m_LimitVal[i].SetCurSel(j);
							break;
						}
					}
					m_TrainID[i].SetWindowText(m_cmd.train_cmd[i].szTrainID);
					m_LimitVal[i].EnableWindow(TRUE);
					m_TrainID[i].EnableWindow(TRUE);
					if(i<4)
						m_Limit[i+1].EnableWindow(TRUE);
				}
			}
			//��ʼʱ��
			if(m_cmd.bStartNow || m_cmd.nTime[0] == 0)
			{
				// ������ʼ��ȡ����ʱ��
				m_bTimeBegin = 1;
				m_tmBegin.EnableWindow(FALSE);
			}
			else
			{
				m_bTimeBegin = 0;
				m_tmBegin.SetTime(&CTime(m_cmd.nTime[0]));
				m_tmBegin.EnableWindow(TRUE);
			}
			
			if(m_cmd.bEndless || m_cmd.nTime[1] == 0)
			{
				// ������Ч��ȡ����ʱ��
				m_bTimeEnd = 1;
				m_tmEnd.EnableWindow(FALSE);
			}
			else
			{
				m_bTimeEnd = 0;
				m_tmEnd.SetTime(&CTime(m_cmd.nTime[1]));
				m_tmEnd.EnableWindow(TRUE);
			}

			// �����, ������
			m_nKmBegin = m_cmd.nKmMark[0].val / 1000;
			m_nMBegin  = m_cmd.nKmMark[0].val % 1000;
			m_nKmEnd   = m_cmd.nKmMark[1].val / 1000;
			m_nMEnd    = m_cmd.nKmMark[1].val % 1000;
			{
				BYTE nFlag = m_cmd.nKmMark[0].km_cl;//GetLongKmFlag(m_cmd.nKmMark[0]);
				for(i=0; i<m_StartLongKM.GetCount(); i++)
				{
					if(m_StartLongKM.GetItemData(i) == nFlag)
					{
						m_StartLongKM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[0].m_cl;//GetLongMFlag(m_cmd.nKmMark[0]);
				for(i=0; i<m_StartLongM.GetCount(); i++)
				{
					if(m_StartLongM.GetItemData(i) == nFlag)
					{
						m_StartLongM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[1].km_cl;//GetLongKmFlag(m_cmd.nKmMark[1]);
				for(i=0; i<m_EndLongKM.GetCount(); i++)
				{
					if(m_EndLongKM.GetItemData(i) == nFlag)
					{
						m_EndLongKM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[1].m_cl;//GetLongMFlag(m_cmd.nKmMark[1]);
				for(i=0; i<m_EndLongM.GetCount(); i++)
				{
					if(m_EndLongM.GetItemData(i) == nFlag)
					{
						m_EndLongM.SetCurSel(i);
						break;
					}
				}
			
			}
		}while(0);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTccCommandInputDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnRegisterConfirmCommandIDReceiver( GetSafeHwnd() );
}


// ѡ��ʼվʱ������վ��ֻ����վ�ɹ�ѡ��ͬʱ��·��Ӧ�仯
void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta()
{
	INT nIndex = m_StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	CString szStaName;
	m_StaBegin.GetLBText(nIndex, szStaName);
	stano_t nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);

	m_StaEnd.ResetContent();
	m_AbsLine.ResetContent();

	nIndex = m_CmdType.GetCurSel();
	ASSERT(CB_ERR != nIndex);
	int nCmdType = m_CmdType.GetItemData(nIndex);
	if(STATION_LIMIT == nCmdType)
	{
		// �����·
		std::vector<lineid_t> lines;
		m_pInfoProvider->GetTccLineList(nStaID, lines);

		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(nStaID, lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);

		// ��������վ
		m_TccList.DeleteNonFixedRows();
	}
	else if(BLOCK_LIMIT == nCmdType)
	{
		// �����·
		std::vector<lineid_t> lines;
		m_pInfoProvider->GetTccBlockLineList(nStaID, lines);

		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccBlockLineName(nStaID, lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);

		// ��������վ
		m_TccList.DeleteNonFixedRows();
	}
	else
	{
		ResetEndStationList(m_StaBegin, m_StaEnd, m_pInfoProvider);

		// ��������վ
		m_TccList.DeleteNonFixedRows();
	}
	m_TccList.Refresh();
}

// ѡ�����վʱ����ʼվ���仯������·��Ӧ�仯
void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta2()
{
	// ��ʼվ������վ
	stano_t nStaID, nStaID2;
	CString szStaName, szStaName2;

	INT nIndex = m_StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);
	m_StaBegin.GetLBText(nIndex, szStaName);

	nIndex = m_StaEnd.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	nStaID2 = (stano_t)m_StaEnd.GetItemData(nIndex);
	m_StaEnd.GetLBText(nIndex, szStaName2);

	switch(m_pInfoProvider->GetStationDirection(nStaID, nStaID2))
	{
	case DIR_UP:	// ��������ȫ����
		m_KmBegin.SetWindowText("0");
		m_MBegin.SetWindowText("0");
		m_KmEnd.SetWindowText("0");
		m_MEnd.SetWindowText("0");
		break;
	case DIR_DOWN:	// ��������ȫ����
		m_KmBegin.SetWindowText("0");
		m_MBegin.SetWindowText("0");
		m_KmEnd.SetWindowText("0");
		m_MEnd.SetWindowText("0");
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	//ɾ����·�б�����
	m_AbsLine.ResetContent();

	// �����·
	std::vector<lineid_t> lines;
	m_pInfoProvider->GetTccLineList(nStaID, nStaID2, lines);

	for(int i=0; i<lines.size(); i++)
	{
		nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(nStaID, lines[i]) );
		ASSERT(CB_ERR != nIndex);
		m_AbsLine.SetItemData(nIndex, lines[i]);
	}
	m_AbsLine.SetCurSel(-1);

	// ��������վ
	m_TccList.SetRowCount(m_TccList.GetFixedRowCount());

	int row = m_TccList.InsertRow(szStaName, -1);
	m_TccList.SetItemData(row, 0, nStaID);
	m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
	m_TccList.SetItemData(row, 1, 0);
	m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
	m_TccList.SetItemData(row,  2, 0);
	m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

	row = m_TccList.InsertRow(szStaName2, -1);
	m_TccList.SetItemData(row, 0, nStaID2);
	m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
	m_TccList.SetItemData(row, 1, 0);
	m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
	m_TccList.SetItemData(row,  2, 0);
	m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

	m_TccList.Refresh();
}

void CTccCommandInputDlg::OnCbnSelchangeTccLine()
{
	stano_t nStaID = 0,	nStaID2 = 0;

	// ��ʼվ
	INT nIndex = m_StaBegin.GetCurSel();
	ASSERT(CB_ERR != nIndex);
	nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);

	// ����վ
	nIndex = m_StaEnd.GetCurSel();
	if(CB_ERR != nIndex)
		nStaID2 = (stano_t)m_StaEnd.GetItemData(nIndex);
	
	// ��·��
	nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	lineid_t nTccLineID = (lineid_t)m_AbsLine.GetItemData(nIndex);
	CString szTccLineName = m_pInfoProvider->GetTccLineName(nStaID, nTccLineID);

	nIndex = m_CmdType.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	int nCmdType = m_CmdType.GetItemData(nIndex);

	if(0 != nStaID2)
	{
		//m_TccList.SetItemText(1, 1, szTccLineName);
		//m_TccList.SetItemData(1, 1, nTccLineID);

		//m_TccList.SetItemText(2, 1, szTccLineName);
		//m_TccList.SetItemData(2, 1, m_pInfoProvider->GetTccLine(nStaID2, nStaID, nTccLineID));
	}
	else if(BLOCK_LIMIT == nCmdType)
	{
		// ��������
		CString szTccLineName = m_pInfoProvider->GetTccBlockLineName(nStaID, nTccLineID);

		// ��������վ
		m_TccList.DeleteNonFixedRows();

		int row = -1;
		CString szStaName;
		CGridCellCheck *pCell = NULL;
	
		szStaName = m_pInfoProvider->GetStationName(nStaID);
		row = m_TccList.InsertRow(szStaName, -1);
		m_TccList.SetItemData(row, 0, nStaID);
		m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));	
		
		m_TccList.SetItemText(1, 1, szTccLineName);
		m_TccList.SetItemData(1, 1, nTccLineID);
		m_TccList.SetItemData(row, 1, 0);
		m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	
		m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
		m_TccList.SetItemData(row,  2, 0);
		m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
	}
	else
	{
		// վ������

		// ��������վ
		m_TccList.DeleteNonFixedRows();

		int row = -1;
		CString szStaName;
		
		szStaName = m_pInfoProvider->GetStationName(nStaID);
		row = m_TccList.InsertRow(szStaName, -1);
		m_TccList.SetItemData(row, 0, nStaID);
		m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
		m_TccList.SetItemData(row, 1, 0);
		m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	
		m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
		m_TccList.SetItemData(row,  2, 0);
		m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

		stano_t nLinkStaID = 0;
		nLinkStaID = m_pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_UP);
		if(0 != nLinkStaID)
		{
			szStaName = m_pInfoProvider->GetStationName(nLinkStaID);
			row = m_TccList.InsertRow(szStaName, -1);
			m_TccList.SetItemData(row, 0, nLinkStaID);
			m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
			m_TccList.SetItemData(row, 1, 0);
			m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
			m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
			m_TccList.SetItemData(row,  2, 0);
			m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
		}

		nLinkStaID = m_pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_DOWN);
		if(0 != nLinkStaID)
		{
			szStaName = m_pInfoProvider->GetStationName(nLinkStaID);
			row = m_TccList.InsertRow(szStaName, -1);
			m_TccList.SetItemData(row, 0, nLinkStaID);
			m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
			m_TccList.SetItemData(row, 1, 0);
			m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
			m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
			m_TccList.SetItemData(row,  2, 0);
			m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
		}
	}

		// ��������վ��·
		for(int row=m_TccList.GetFixedRowCount(); row<m_TccList.GetRowCount(); row++)
		{
			stano_t nLinkStaID = (stano_t)m_TccList.GetItemData(row, 0);
			lineid_t nLinkTccLineID = 0;

			if(nStaID == nLinkStaID)
			{
				m_TccList.SetItemText(row, 1, szTccLineName);
				m_TccList.SetItemData(row, 1, nTccLineID);

				nLinkTccLineID = nTccLineID;
			}
			else
			{
				// ÿ����·���ʱ��������ϴε���·ѡ��
				m_TccList.SetItemText(row, 1, NULL);

				// ��������վ����·�����б�
				CStringArray options;
				std::vector<lineid_t> lines;

				m_TccList.SetItemState(row, 1, ~GVIS_READONLY & m_TccList.GetItemState(row, 1));
				m_pInfoProvider->GetTccLineList(nLinkStaID, nStaID, lines);
				for(int j=0; j<lines.size(); j++)
					options.Add( m_pInfoProvider->GetTccLineName(nLinkStaID, lines[j]) );
				
				m_TccList.SetCellType(row, 1, RUNTIME_CLASS(CGridCellCombo));
				CGridCellCombo *pCell= (CGridCellCombo*) m_TccList.GetCell(row, 1);
				pCell->SetOptions(options);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST ); //CBS_DROPDOWNLIST

				// ReEdit��ĳվУ��ʱ, �����뱾�μ������·
				if(!m_bCheckCommand)
				{
					nLinkTccLineID = m_pInfoProvider->GetTccLine(nLinkStaID, nStaID, nTccLineID);
					if(0 != nLinkTccLineID)
						pCell->SetText(m_pInfoProvider->GetTccLineName(nLinkStaID, nLinkTccLineID));
				}
			}

			// ReEdit��ĳվУ��ʱ
			if(!m_bCheckCommand)
			{
				bool bTccValidate = false;

				for(int i=0; i<m_cmd.tcc.size(); i++)
				{
					if(m_cmd.tcc[i].nID == nLinkStaID)
					{
						if(nLinkTccLineID == m_cmd.tcc[i].nTccLineID)
						{
							bTccValidate = m_cmd.tcc[i].bTccValidate;
						}
						else
						{
							//�������·���������ֵ��ͬ�����������û�ѡ��
							m_TccList.SetItemText(row, 1, NULL);
						}

						break;
					}
				}
				
				CGridCellCheck *pCheckCell = (CGridCellCheck *)m_TccList.GetCell(row, 2);
				if(pCheckCell)
					pCheckCell->SetCheck(bTccValidate);
			}
		}
	
	m_TccList.Refresh();
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimeendRadio2()
{
	m_tmEnd.EnableWindow(FALSE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimeendRadio()
{
	m_tmEnd.EnableWindow(TRUE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimebeginRadio2()
{
	m_tmBegin.EnableWindow(FALSE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimebeginRadio()
{
	m_tmBegin.EnableWindow(TRUE);
}

void CTccCommandInputDlg::OnBtnClickedLimit2()
{
	int i=1;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("��һ������û��ѡ��!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit3()
{
	int i=2;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("��һ������û��ѡ��!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit4()
{
	int i=3;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("��һ������û��ѡ��!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit5()
{
	int i=4;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("��һ������û��ѡ��!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		//m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	if( !UpdateData(TRUE) )
		return ;

	RAW_STOCK_COMMAND cmd = m_cmd;
	//cmd.nConsoleID = m_cmd.nConsoleID;
	//cmd.raw_sysid = m_cmd.raw_sysid;

	CString prompt("���ݲ���ȷ!");
	BOOL bSuccess = FALSE;
	do {
		cmd.bGenCancelCommand = m_bGenCancelCommand;

		int nIndex = m_CmdType.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("��ѡ����������!");
			break;
		}
		int nCmdType = m_CmdType.GetItemData(nIndex);
		cmd.bFullLimit = (ABSAPP_FULL_LIMIT == nCmdType);

		// �����
		cmd.nCmdID = (cmdid_t)m_nCmdID;
		if(0 == m_cmd.nCmdID && !m_pInfoProvider->IsValidCommandID(cmd.nCmdID))
		{	//�����������ű����ںϷ���Χ�ڣ��Ѿ�������ŵĲ����Ϸ��ԣ��Ա㲻ͬ��GPC�ն˷ֱ��������ͼ���
			prompt.Format("��������ű����ڷ�Χ%d--%d��", 
				m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID());
			break;
		}

		// ��ʼվ
		nIndex = m_StaBegin.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("��ѡ��ʼվ!");
			break;
		}
		cmd.nStaID[0] = (stano_t)m_StaBegin.GetItemData(nIndex);

		// ����վ
		nIndex = m_StaEnd.GetCurSel();
		DWORD dw = 0;
		if(CB_ERR != nIndex)
		{
			dw = m_StaEnd.GetItemData(nIndex);
		}
		else if(cmd.bFullLimit)
		{
			prompt.Format("����ȫ������,����ѡ�����վ!");
			break;
		}
		cmd.nStaID[1] = (stano_t)dw;

		// ��·��
		nIndex = m_AbsLine.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("��ѡ����·!");
			break;
		}
		cmd.nLineID = (lineid_t)m_AbsLine.GetItemData(nIndex);

		// ����곤��,��3�ֽڻ��ǹ���꣬��1�ֽڵ�0-5��ΪKM�ĳ���,6��ΪM�ĳ���
		LONG nLongKmSel = m_StartLongKM.GetCurSel();
		LONG nLongKm = 0;
		if(nLongKmSel != CB_ERR)
			nLongKm = m_StartLongKM.GetItemData(nLongKmSel);
		LONG nLongMSel = m_StartLongM.GetCurSel();
		LONG nLongM = 0;
		if(nLongMSel != CB_ERR)
			nLongM = m_StartLongM.GetItemData(nLongMSel);
		
		cmd.nKmMark[0].val = m_nKmBegin * 1000 + m_nMBegin;// + (nLongKm<<24) + (nLongM<<30);
		cmd.nKmMark[0].km_cl = nLongKm;
		cmd.nKmMark[0].m_cl  = nLongM;

		nLongKmSel = m_EndLongKM.GetCurSel();
		nLongKm = 0;
		if(nLongKmSel != CB_ERR)
			nLongKm = m_EndLongKM.GetItemData(nLongKmSel);
		nLongMSel = m_EndLongM.GetCurSel();
		nLongM = 0;
		if(nLongMSel != CB_ERR)
			nLongM = m_EndLongM.GetItemData(nLongMSel);
		
		cmd.nKmMark[1].val = m_nKmEnd * 1000 + m_nMEnd;// + (nLongKm<<24) + (nLongM<<30);
		cmd.nKmMark[1].km_cl = nLongKm;
		cmd.nKmMark[1].m_cl  = nLongM;
		//����end
		if(cmd.nKmMark[0] == cmd.nKmMark[1]
			&& (cmd.nKmMark[0].val == 0 || cmd.nKmMark[0].val == 9999999)
			&& !cmd.bFullLimit && BLOCK_LIMIT != nCmdType)
		{
			prompt.Format("����ȫ���ٲ���ֱ�����빫��꣬������ѡ����������!");
			break;
		}

		// ��ʼʱ��
		if(0==m_bTimeBegin)
		{
			CTime ct;
			m_tmBegin.GetTime(ct);
			cmd.nTime[0] = ct.GetTime() - ct.GetSecond();
			cmd.bStartNow = FALSE;
		}
		else
		{
			cmd.nTime[0] = 0;
			cmd.bStartNow = TRUE;
		}

		// ����ʱ��
		if(0==m_bTimeEnd)
		{
			CTime ct;
			m_tmEnd.GetTime(ct);
			cmd.nTime[1] = ct.GetTime() - ct.GetSecond();
			cmd.bEndless = FALSE;
		}
		else
		{
			cmd.nTime[1] = 0;
			cmd.bEndless = TRUE;
		}

		// ����ֵ
		cmd.bCancelLimit = m_bCancelLimit;
		if(cmd.bCancelLimit)
		{
			cmd.train_cmd[0].nSpeed = cmd.train_cmd[0].nSpeedMaxVal = 45;
		}
		else
		{
			nIndex = m_LimitVal[0].GetCurSel();
			if(CB_ERR == nIndex)
			{
				prompt.Format("��ѡ������!");
				break;
			}
			cmd.train_cmd[0].nSpeed = m_LimitVal[0].GetItemData(nIndex);
			if(cmd.train_cmd[0].nSpeed > 45)
				cmd.train_cmd[0].nSpeedMaxVal = cmd.train_cmd[0].nSpeed;
			else if(cmd.train_cmd[0].nSpeed > 0)
				cmd.train_cmd[0].nSpeedMaxVal = 45;
			m_TrainID[0].GetWindowText(cmd.train_cmd[0].szTrainID);

			int dw = 0;
			for(int i=1; i<5; i++)
			{
				if( m_Limit[i].GetCheck() )
				{
					nIndex = m_LimitVal[i].GetCurSel();
					dw = 0;
					if(CB_ERR != nIndex)
						dw = m_LimitVal[i].GetItemData(nIndex);
					cmd.train_cmd[i].nSpeed =  dw;
					if(cmd.train_cmd[i].nSpeed > 45)
						cmd.train_cmd[i].nSpeedMaxVal = cmd.train_cmd[i].nSpeed;
					else if(cmd.train_cmd[i].nSpeed > 0)
						cmd.train_cmd[i].nSpeedMaxVal = 45;
					m_TrainID[i].GetWindowText(cmd.train_cmd[i].szTrainID);
				}
				else
				{
					cmd.train_cmd[i] = RAW_STOCK_COMMAND::_TRAINCMD();
				}
			}

			for(i=0; i<4; i++)
			{
				if(0 != cmd.train_cmd[i].nSpeed
					&& cmd.train_cmd[i].nSpeed == cmd.train_cmd[i+1].nSpeed)
					break;
			}
			if(i != 4)
			{
				prompt.Format("������������������ֵ��ͬ!");
				break;
			}
		}

		// ����վ�б�
		cmd.tcc.clear();
		for(int i=m_TccList.GetFixedRowCount(); i<m_TccList.GetRowCount(); i++)
		{
			RAW_STOCK_COMMAND::_TCC_STRUCT tcc;
			tcc.nID = (stano_t)m_TccList.GetItemData(i, 0);
			tcc.nTccLineID = (lineid_t)m_TccList.GetItemData(i, 1);
			if(0 == tcc.nTccLineID)
			{
				CString str = m_TccList.GetItemText(i, 1);
				std::vector<lineid_t> lines;
				m_pInfoProvider->GetTccLineList(tcc.nID, cmd.nStaID[0], lines);
				for(int j=0; j<lines.size(); j++)
				{
					if(str == m_pInfoProvider->GetTccLineName(tcc.nID, lines[j]))
					{
						tcc.nTccLineID = lines[j];
						break;
					}
				}
			}
			if(0 == tcc.nTccLineID)
				break;
			
			//��վ�Ƿ���ҪУ��
			CGridCellCheck *pCell = NULL;
			pCell = (CGridCellCheck *)m_TccList.GetCell(i, 2);
			ASSERT(pCell != NULL);
			BOOL bChecked = pCell->GetCheck();
			tcc.bTccValidate = bChecked;

			cmd.tcc.push_back(tcc);
		}
		if(m_TccList.GetRowCount()!=i)
		{
			prompt.Format("��ѡ������վ����·!");
			break;
		}

		// ����ÿ�����ٵ�״̬�б�
		for(i=0; i<5; i++)
		{
			cmd.train_cmd[i].tcc.resize(cmd.tcc.size());
		}

		//�������վ�б��Ƿ���ȷ
		if(!cmd.bFullLimit)
		{
			std::vector<stano_t> tcclist;
			int builtin_id = m_pInfoProvider->GetLineID(cmd.tcc[0].nID, cmd.tcc[0].nTccLineID);
			if(0 < builtin_id)
			{
				tcclist = m_pInfoProvider->GetTccList(builtin_id, cmd.nKmMark[0], cmd.nKmMark[1]);
			
				for(i=0; i<tcclist.size(); i++)
				{
					for(int j=0; j<cmd.tcc.size() && tcclist[i] != cmd.tcc[j].nID; j++);
					if(j >= cmd.tcc.size())
						break;
				}
				if(i < tcclist.size() || tcclist.size() != cmd.tcc.size())
				{
					prompt = "���ݹ��������������վ�б�: \r\n";
					CString sz;
					for(i=0; i<tcclist.size(); i++)
					{
						sz.Format("        վ %s(%d)\r\n", m_pInfoProvider->GetStationName(tcclist[i]), tcclist[i]);
						prompt += sz;
					}
					prompt += "\r\n�뵱ǰ����վ�б�һ��!\r\n";
					prompt += "���鹫�������������Ƿ���ȷ!";

					break;
				}
			}
		}

		bSuccess = TRUE;
	} while(0);
	if(!bSuccess)
	{
		MessageBox(prompt);
		return ;
	}


	if(m_bCheckCommand)
	{
		// ���һ����
		if(cmd != m_cmd)
		{
			prompt.Format("���һ�£�ԭ��\n%s", GetDifferenceReason(cmd, m_cmd, m_pInfoProvider));
			MessageBox(prompt);
			return ;
		}
	}
	else
	{
		if(m_bManualChangeCommandID)
		{
			MessageBox("����������޸ĺ�δȷ�ϣ�����\"�˹�ȷ��\"��\"�Զ�ѡ��\"!");
			return;
		}

		if(!cmd.bCancelLimit && !m_bCmdIDConfirmed)
		{
			int nRet = MessageBox("������û��ȷ��������Ƿ���Ч, �Ƿ�Ҫ�������ȷ��?", "�����ȷ��", MB_YESNO);
			//MessageBox�ڼ�, ȷ����Ϣ�Ƿ��Ѵ���
			if(!m_bCmdIDConfirmed)
			{
				if(IDYES == nRet)
					RequestCommandID(m_nCmdID);
			}	
			return;
		}

		// ��ȡ������, ����������Ա��Զ�����
		if(!cmd.bCancelLimit && m_cmd.nCmdID != cmd.nCmdID)
		{
			m_pInfoProvider->SaveCommandID(cmd.nCmdID);
		}

		m_cmd = cmd;
	}

	CDialog::OnOK();
}

void CTccCommandInputDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

void  CTccCommandInputDlg::OnChangeTccCmdID()
{
	if(!UpdateData())
		return;

	if(!m_pInfoProvider->IsValidCommandID(m_nCmdID))
	{
		CString prompt;
		prompt.Format("��������ű����ڷ�Χ%d--%d��", 
			m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID());
		MessageBox(prompt);

		m_CmdID.SetFocus();
		return;
	}

	if(m_bManualChangeCommandID)
	{
		m_CmdID.EnableWindow(FALSE);

		m_btnChangeCmdID.EnableWindow(FALSE);
		m_btnAutoComputeCmdID.EnableWindow(FALSE);

		RequestCommandID(m_nCmdID);
	}
	else
	{
		CPasswordInputDlg passwdinput(this);
		if(IDOK == passwdinput.DoModal())
		{
			if(passwdinput.GetPassword() != "1234")
			{
				MessageBox("���벻��ȷ!");
				return;
			}

			m_bManualChangeCommandID = TRUE;
			m_btnChangeCmdID.SetWindowText("�˹�ȷ��");

			m_CmdID.EnableWindow(TRUE);
			m_CmdID.SetFocus();
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedAutoComputeCmdID()
{
   m_nCmdID = m_pInfoProvider->GetCommandID();
	CString str;
	str.Format("%d", m_nCmdID);
	m_CmdID.SetWindowText(str);

	m_CmdID.EnableWindow(FALSE);

	m_bManualChangeCommandID = FALSE;
	m_btnChangeCmdID.SetWindowText("�޸������");

	m_btnAutoComputeCmdID.EnableWindow(FALSE);
	m_btnChangeCmdID.EnableWindow(FALSE);

	RequestCommandID(m_nCmdID);
}

void CTccCommandInputDlg::RequestCommandID(cmdid_t nCmdID)
{
	//��������ʱ, ����ű���ɷ�������ȷ��������Ƿ���Ч
	int who_confirm_command_id = ITccAdviseSink::SELF_CONFIRM_COMMAND_ID;

	dwServerConfirmCookie ++;
	m_bCmdIDConfirmed = false;

	m_pAdviseSink->ConfirmCommandID(m_pInfoProvider->GetConsoleID(), m_nCmdID, 
		m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID(),
		who_confirm_command_id, dwServerConfirmCookie, GetSafeHwnd());
	
	if(ITccAdviseSink::SELF_CONFIRM_COMMAND_ID == who_confirm_command_id)
		m_bCmdIDConfirmed = true;
}

LRESULT CTccCommandInputDlg::OnServerConfirmCommandID(WPARAM wParam, LPARAM lParam)
{
	// �������Ѿ�ȷ�Ϲ�
	if(m_bCmdIDConfirmed)
		return FALSE;

	bool bValid = (bool)wParam;
	SERVER_CONFIRM_COMMAND_ID_STRUCT& ds = *(SERVER_CONFIRM_COMMAND_ID_STRUCT*)lParam;

	if(ds.dwCookie != dwServerConfirmCookie)
		return FALSE;

	if(ds.bValid)
	{
		m_bCmdIDConfirmed = true;

		m_btnChangeCmdID.EnableWindow(TRUE);
		m_bManualChangeCommandID = FALSE;
		m_btnChangeCmdID.SetWindowText("�޸������");
		m_btnAutoComputeCmdID.EnableWindow(TRUE);
	}
	else
	{
		if(m_bManualChangeCommandID)
		{
			MessageBox("�˹��޸ĵ�������Ѿ�����!");
			
			m_btnChangeCmdID.EnableWindow(TRUE);
			m_btnAutoComputeCmdID.EnableWindow(TRUE);

			m_CmdID.EnableWindow(TRUE);
			m_CmdID.SetFocus();

			return FALSE;
		}

		// ������ȷ�ϲ��ɹ�, ����һ���µ������
		if(m_nCmdID == m_pInfoProvider->GetCommandID())
			m_pInfoProvider->NextCommandID();

       m_nCmdID = m_pInfoProvider->GetCommandID();
	   CString str;
	   str.Format("%d", m_nCmdID);
	   m_CmdID.SetWindowText(str);

	   RequestCommandID(m_nCmdID);
	}

	return TRUE;
}

void CTccCommandInputDlg::OnCbnSelchangeTccCmdType()
{
	INT nIndex = m_CmdType.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_StaBegin.SetCurSel(-1);
		m_StaBegin.EnableWindow(FALSE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		m_AbsLine.SetCurSel(-1);
		m_AbsLine.EnableWindow(FALSE);
		return ;
	}

	m_nKmBegin = m_nMBegin = m_nKmEnd = m_nMEnd = 0;

	switch(m_CmdType.GetItemData(nIndex))
	{
	default:
		ASSERT(FALSE);
		break;
	case STATION_LIMIT:	// վ������
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);

		m_StartLongKM.EnableWindow(TRUE);
		m_StartLongM.EnableWindow(TRUE);
		m_EndLongKM.EnableWindow(TRUE);
		m_EndLongM.EnableWindow(TRUE);
		break;
	case ABSAPP_LIMIT:	// ��������
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.EnableWindow(TRUE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);

		m_StartLongKM.EnableWindow(TRUE);
		m_StartLongM.EnableWindow(TRUE);
		m_EndLongKM.EnableWindow(TRUE);
		m_EndLongM.EnableWindow(TRUE);
		break;
	case ABSAPP_FULL_LIMIT:	// ����ȫ������
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.EnableWindow(TRUE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);

		m_StartLongKM.EnableWindow(FALSE);
		m_StartLongKM.SetCurSel(-1);
		m_StartLongM.EnableWindow(FALSE);
		m_StartLongM.SetCurSel(-1);
		m_EndLongKM.EnableWindow(FALSE);
		m_EndLongKM.SetCurSel(-1);
		m_EndLongM.EnableWindow(FALSE);
		m_EndLongM.SetCurSel(-1);
		break;
	case BLOCK_LIMIT:
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);

		m_nKmBegin = 0;
		m_nMBegin = 0;
		m_nKmEnd = 9999;
		m_nMEnd = 999;

		m_StartLongKM.EnableWindow(FALSE);
		m_StartLongKM.SetCurSel(-1);
		m_StartLongM.EnableWindow(FALSE);
		m_StartLongM.SetCurSel(-1);
		m_EndLongKM.EnableWindow(FALSE);
		m_EndLongKM.SetCurSel(-1);
		m_EndLongM.EnableWindow(FALSE);
		m_EndLongM.SetCurSel(-1);
		break;
	}
	UpdateData(FALSE);
}

void CTccCommandInputDlg::InitListCtrl(CGridCtrl& grid)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"����վ", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"��·",   140, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"��У��", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
	};

	CRect rcClient;
	grid.GetClientRect(&rcClient);

	try{
		grid.SetRowCount(1);                //��
		grid.SetColumnCount(sizeof(colinfo)/sizeof(colinfo[0]));             //��
		grid.SetFixedRowCount(1);        //�̶���
		grid.SetFixedColumnCount(0);     //�̶���		    
		grid.SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		grid.SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
	    grid.SetFixedBkColor(RGB(255,242,240));

		for(int i=0; i<grid.GetColumnCount(); i++)
		{
			grid.SetColumnWidth(i, colinfo[i].width);
		}
	}
   	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.row = 0;  //��
	for(item.col = 0; item.col < grid.GetColumnCount(); item.col++)
	{
		item.strText.Format(colinfo[item.col].label);
		item.nFormat = colinfo[item.col].fmt;
		grid.SetItem(&item);
	}
	
	grid.SetSingleRowSelection();
}

void CTccCommandInputDlg::OnSelCancelLimit() 
{
 
  if(m_CancelLimit.GetCheck())
	  MessageBox("��ע�⣺,ȡ����������ġ���������š������ǰ����Ӧ��������������ĺ���һ��!");;
}

};

// Tcc\TccCommandInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "tcc_base.h"
#include "TccCommandInputDlg.h"
#include "TccCommandSketchMapDlg.h"
#include "PasswordInputDlg.h"
#include "CmdTypeSelectDlg.h"

#include <algorithm>

namespace TCC_C3 {

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
	//, m_nKmBegin(0)//, m_nKmBegin2(0)
	//, m_nKmEnd(0)//, m_nKmEnd2(0)
	, m_bTimeBegin(1)
	, m_bTimeEnd(1)
	, m_bCancelLimit(FALSE)
	, m_bGenCancelCommand(FALSE)
	, m_nMinKm(0)
	, m_nMaxKm(0)
	, m_nStaBegin(0)
	, m_nStaEnd(0)
	, m_nDirection(2)//, m_nDirection2
	, m_nCmdType(-1)
	, m_bCmdIDConfirmed(false)
	, m_bManualChangeCommandID(FALSE)
{
	if(pcmd)
	{
		m_cmd = *pcmd;
		if(0 != m_cmd.nCmdID)
			m_bCmdIDConfirmed = true;
		if(!m_bCheckCommand)
			m_bGenCancelCommand = m_cmd.bGenCancelCommand;
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

	DDX_Control(pDX, IDC_TCC_CMD_LONGKM,  m_Kcl[0]);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM,   m_Mcl[0]);
	DDX_Control(pDX, IDC_TCC_CMD_LONGKM2, m_Kcl[1]);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM2,  m_Mcl[1]);

	if(pDX->m_bSaveAndValidate)
	{
		CString szKm, szM, szKcl, szMcl;

		DDX_Text(pDX, IDC_TCC_CMD_M, szM);
		DDX_Text(pDX, IDC_TCC_CMD_KM, szKm);

		DDX_CBString(pDX, IDC_TCC_CMD_LONGKM, szKcl);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGM,  szMcl);

		if(szKm.IsEmpty())
			szKm = "0";
		if(szM.IsEmpty())
			szM = "0";
		m_szKmBegin = szKm + szKcl + "+" + szM + szMcl;
	}
	else
	{
		CString szPrex, szKm, szM, szKcl, szMcl;

		parseKmMark(m_szKmBegin, szPrex, szKm, szKcl, szM, szMcl);

		m_Kcl[0].SetCurSel(0);
		m_Mcl[0].SetCurSel(0);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGKM, szKcl);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGM,  szMcl);
		DDX_Text(pDX, IDC_TCC_CMD_KM, szPrex + szKm);
		DDX_Text(pDX, IDC_TCC_CMD_M, szM);
	}

	if(pDX->m_bSaveAndValidate)
	{
		CString szKm, szM, szKcl, szMcl;

		DDX_Text(pDX, IDC_TCC_CMD_M2, szM);
		DDX_Text(pDX, IDC_TCC_CMD_KM2, szKm);

		DDX_CBString(pDX, IDC_TCC_CMD_LONGKM2, szKcl);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGM2,  szMcl);

		if(szKm.IsEmpty())
			szKm = "0";
		if(szM.IsEmpty())
			szM = "0";
		m_szKmEnd = szKm + szKcl + "+" + szM + szMcl;
	}
	else
	{
		CString szPrex, szKm, szM, szKcl, szMcl;

		parseKmMark(m_szKmEnd, szPrex, szKm, szKcl, szM, szMcl);

		m_Kcl[1].SetCurSel(0);
		m_Mcl[1].SetCurSel(0);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGKM2, szKcl);
		DDX_CBString(pDX, IDC_TCC_CMD_LONGM2,  szMcl);
		DDX_Text(pDX, IDC_TCC_CMD_KM2, szPrex + szKm);
		DDX_Text(pDX, IDC_TCC_CMD_M2, szM);
	}

	DDX_Control(pDX, IDC_TCC_CMD_KM, m_KmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_M, m_MBegin);
	DDX_Control(pDX, IDC_TCC_CMD_KM2, m_KmEnd);
	DDX_Control(pDX, IDC_TCC_CMD_M2, m_MEnd);

	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN, m_tmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND, m_tmEnd);

	DDX_Radio(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_bTimeBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_BeginRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO2, m_BeginRadio2);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_bTimeEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_EndRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO2, m_EndRadio2);

	DDX_Check(pDX, IDC_TCC_CANCE_LIMIT, m_bCancelLimit);
	DDX_Control(pDX, IDC_TCC_CANCE_LIMIT, m_CancelLimit);

	DDX_Check(pDX, IDC_TCC_CANCELCOMMAND, m_bGenCancelCommand);
	DDX_Control(pDX, IDC_TCC_CANCELCOMMAND, m_GenCancelCommand);

	DDX_Control(pDX, IDC_TCC_LIMIT,  m_Limit[0]);
	DDX_Control(pDX, IDC_TCC_LIMIT2, m_Limit[1]);
	DDX_Control(pDX, IDC_TCC_LIMIT3, m_Limit[2]);
	DDX_Control(pDX, IDC_TCC_LIMIT4, m_Limit[3]);
	DDX_Control(pDX, IDC_TCC_LIMIT5, m_Limit[4]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL, m_LimitVal[0]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL2, m_LimitVal[1]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL3, m_LimitVal[2]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL4, m_LimitVal[3]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL5, m_LimitVal[4]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID,  m_TrainID[0]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID2, m_TrainID[1]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID3, m_TrainID[2]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID4, m_TrainID[3]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID5, m_TrainID[4]);

	DDX_Control(pDX, IDC_TCC_CMD_VALID_KM, m_ValidKmMarker);
	DDX_Control(pDX, IDC_TCC_CMD_ADJUST_KM, m_AdjustKmMarker);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);

	DDX_Radio(pDX, IDC_RADIO1, m_nDirection);
	DDX_Control(pDX, IDC_RADIO1, m_btnDirDown);
	DDX_Control(pDX, IDC_RADIO2, m_btnDirUp);
	DDX_Control(pDX, IDC_RADIO3, m_btnDirDual);
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
	ON_BN_CLICKED(IDC_TCC_LIMIT, OnBtnClickedLimit)
	ON_BN_CLICKED(IDC_TCC_LIMIT2, OnBtnClickedLimit2)
	ON_BN_CLICKED(IDC_TCC_LIMIT3, OnBtnClickedLimit3)
	ON_BN_CLICKED(IDC_TCC_LIMIT4, OnBtnClickedLimit4)
	ON_BN_CLICKED(IDC_TCC_LIMIT5, OnBtnClickedLimit5)
	ON_EN_KILLFOCUS(IDC_TCC_CMD_KM, OnKmMarkerChanged)
	ON_EN_KILLFOCUS(IDC_TCC_CMD_M, OnKmMarkerChanged)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_LONGKM, OnKmMarkerChanged)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_LONGM, OnKmMarkerChanged)
	ON_EN_KILLFOCUS(IDC_TCC_CMD_KM2, OnKmMarkerChanged)
	ON_EN_KILLFOCUS(IDC_TCC_CMD_M2, OnKmMarkerChanged)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_LONGKM2, OnKmMarkerChanged)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_LONGM2, OnKmMarkerChanged)
	ON_BN_CLICKED(IDC_BTN_ADJUST_ACK,OnBnClickedBtnAdjustAck)
	ON_BN_CLICKED(IDC_RADIO1, OnSelDirection)
	ON_BN_CLICKED(IDC_RADIO2, OnSelDirection)
	ON_BN_CLICKED(IDC_RADIO3, OnSelDirection)
	ON_BN_CLICKED(IDC_LS_SKETCHMAP, OnSketchMap)
	ON_BN_CLICKED(IDC_CHANGE_TCCCMDID, OnChangeTccCmdID)
	ON_BN_CLICKED(IDC_AUTOCOMPUTE_TCCCMDID, OnBtnClickedAutoComputeCmdID)
	
	ON_MESSAGE(UM_SERVER_CONFIRM_COMMAND_ID, OnServerConfirmCommandID)
END_MESSAGE_MAP()

// CTccCommandInputDlg ��Ϣ�������
BOOL CTccCommandInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_BtnEn=false;     //��ʼ��������ʾ����
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

	//��ʼ������վ�б�
	CRect rcl;
	GetDlgItem(IDC_TCC_LIST)->GetWindowRect(&rcl);
	ScreenToClient(rcl);
	m_TccList.Create(rcl, this, 8341);

	CString str;
	int nIndex;

	struct {
		int nCmdType;
		LPCSTR szTitle;
	} cmdtype_title[] = {
		{STATION_LIMIT,		"��������"},
		//{ABSAPP_LIMIT,		"��������"},
		//{ABSAPP_FULL_LIMIT, "ȫ������"},
		{KM_LIMIT, "��������"},
	};
	// ��ʼ�����������б�
	for(int i=0; i<sizeof(cmdtype_title)/sizeof(cmdtype_title[0]); i++)
	{
		nIndex = m_CmdType.AddString(cmdtype_title[i].szTitle);
		m_CmdType.SetItemData(nIndex, cmdtype_title[i].nCmdType);
	}

	for(KCL_MAP *p = GetKclMap(); p && p->szKcl != NULL; p++)
	{
		nIndex = m_Kcl[0].AddString(p->szKcl);
		m_Kcl[0].SetItemData(nIndex, p->n);

		nIndex = m_Kcl[1].AddString(p->szKcl);
		m_Kcl[1].SetItemData(nIndex, p->n);
	}

	for(i=0; i<2; i++)
	{
		nIndex = m_Mcl[i].AddString("");
		m_Mcl[i].SetItemData(nIndex, 0);
		nIndex = m_Mcl[i].AddString("a");
		m_Mcl[i].SetItemData(nIndex, 1);
	}

	// ��ʼ����ʼվ������վ
	std::vector<stano_t> staList;
	m_pInfoProvider->GetStationList(staList);
	for(int i=0; i<staList.size(); i++)
	{
		str.Format("%s (%d)", m_pInfoProvider->GetStationName(staList[i]), staList[i]);
		nIndex = m_StaBegin.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StaBegin.SetItemData(nIndex, staList[i]);
	}
	m_StaBegin.SetCurSel(-1);
	//OnCbnSelchangeTccCmdSta();

	OnCbnSelchangeTccCmdType();

	m_bCancelLimit = m_cmd.bCancelLimit;
	m_CancelLimit.EnableWindow(FALSE);
	m_bGenCancelCommand = m_cmd.bGenCancelCommand;
	m_GenCancelCommand.EnableWindow(FALSE);

	m_Limit[0].SetCheck(TRUE);

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
		m_CmdString = m_cmd.toString(m_pInfoProvider);

		if(m_bCancelLimit)
		{
			m_Limit[1].EnableWindow(FALSE);
			m_LimitVal[0].EnableWindow(FALSE);
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
				m_Limit[1].EnableWindow(FALSE);
			}

			//��������
			/*if(0 == m_cmd.nStaID[0] && 0 == m_cmd.nStaID[1])
			{
				if(m_cmd.nLineID)
					m_nCmdType = KM_LIMIT;
			}
			else if(0 == m_cmd.nStaID[1])
				m_nCmdType = STATION_LIMIT;
			else if(m_cmd.bFullLimit)
				m_nCmdType = ABSAPP_FULL_LIMIT;
			else
				m_nCmdType = ABSAPP_LIMIT;*/
			m_nCmdType = m_cmd.nCmdType;
			for(i=0; i<m_CmdType.GetCount(); i++)
			{
				if(m_CmdType.GetItemData(i) == m_nCmdType)
				{
					m_CmdType.SetCurSel(i);
					OnCbnSelchangeTccCmdType();
					break;
				}
			}
			if(i == m_CmdType.GetCount())
				break;

			// ��ʼվ
			for(i=0; i<m_StaBegin.GetCount(); i++)
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
				int builtin_id = 0;
				bool bFind = false;
				switch(m_nCmdType)
				{
				case STATION_LIMIT:
					if(m_cmd.tcc[0].nTccLineID == m_AbsLine.GetItemData(i))
						bFind = true;
					break;

				case ABSAPP_LIMIT:
				case ABSAPP_FULL_LIMIT:
					builtin_id = m_pInfoProvider->GetLineID(m_cmd.nStaID[0], m_AbsLine.GetItemData(i));
					if(builtin_id == m_cmd.nLineID)
						bFind = true;
					break;
				case KM_LIMIT:
					builtin_id = m_AbsLine.GetItemData(i);
					if(builtin_id == m_cmd.nLineID)
						bFind = true;
					break;
				}
				if(bFind)
				{
					m_AbsLine.SetCurSel(i);
					OnCbnSelchangeTccLine();
					break;
				}
			}

			switch(m_cmd.nDirection)
			{
			case 1:
				m_nDirection = 0;
				break;
			case 2:
				m_nDirection = 1;
				break;
			case 3:
				m_nDirection = 2;
				break;
			}
			m_szKmBegin = m_cmd.nKmMark[0].toString(m_pInfoProvider, m_cmd.nLineID);
			m_szKmEnd   = m_cmd.nKmMark[1].toString(m_pInfoProvider, m_cmd.nLineID);
			UpdateData(FALSE);
			OnSelDirection();			

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
		}while(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTccCommandInputDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnRegisterConfirmCommandIDReceiver( GetSafeHwnd() );
}

//ѡ����������
void CTccCommandInputDlg::OnCbnSelchangeTccCmdType()
{
	m_nStaBegin = 0;
	m_nStaEnd   = 0;
	m_nLineID   = 0;
	m_nMinKm    = 0;
	m_nMaxKm    = 0;
	m_szKmBegin  = "";
	m_szKmEnd    = "";
    m_BtnEn=false;
	INT nIndex = m_CmdType.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_StaBegin.SetCurSel(-1);
		OnCbnSelchangeTccCmdSta();
		m_StaBegin.EnableWindow(FALSE);

		return ;
	}
	m_nCmdType = m_CmdType.GetItemData(nIndex);
	m_StaBegin.SetCurSel(-1);
	OnCbnSelchangeTccCmdSta();

	if(KM_LIMIT == m_nCmdType)
	{
		m_StaBegin.EnableWindow(FALSE);

		m_AbsLine.EnableWindow(TRUE);
		m_AbsLine.ResetContent();

		std::vector<int> lines;
		m_pInfoProvider->GetLineList(lines);
		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetLineName(lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);
		OnCbnSelchangeTccLine();
	}
	else
	{
		m_StaBegin.EnableWindow(TRUE);
	}
}

// ѡ��ʼվʱ
// վ�����ٺͲ������٣���·�б�仯������վ�б�������
// �������ٺ�����ȫ���٣�����վ�б�仯
void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta()
{
	m_nStaEnd   = 0;
	m_nLineID   = 0;
	m_nMinKm    = 0;
	m_nMaxKm    = 0;
	m_szKmBegin  = "";
	m_szKmEnd    = "";

	INT nIndex = m_StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		switch(m_nCmdType)
		{
		case ABSAPP_LIMIT:
		case ABSAPP_FULL_LIMIT:
			OnCbnSelchangeTccCmdSta2();
			break;
		case STATION_LIMIT:
		default:
			m_AbsLine.SetCurSel(-1);
			OnCbnSelchangeTccLine();
			m_AbsLine.EnableWindow(FALSE);
			break;
		}
		return;
	}
	m_nStaBegin = (stano_t)m_StaBegin.GetItemData(nIndex);

	CString str;
	switch(m_nCmdType)
	{
	default:
		ASSERT(FALSE);
		break;
	case ABSAPP_LIMIT:
	case ABSAPP_FULL_LIMIT: {
		m_StaEnd.EnableWindow(TRUE);
		m_StaEnd.ResetContent();

		std::vector<stano_t> staList;
		m_pInfoProvider->GetLinkedStationList(m_nStaBegin, staList);

		m_StaEnd.ResetContent();
		for(int i=0; i<staList.size(); i++)
		{
			str.Format("%s(%d)", m_pInfoProvider->GetStationName(staList[i]), staList[i]);
			nIndex = m_StaEnd.AddString(str);
			ASSERT(CB_ERR != nIndex);
			m_StaEnd.SetItemData(nIndex, staList[i]);
		}
		m_StaEnd.SetCurSel(-1);
		OnCbnSelchangeTccCmdSta2();
							}break;
	case STATION_LIMIT: {
		m_AbsLine.EnableWindow(TRUE);
		m_AbsLine.ResetContent();

		std::vector<lineid_t> lines;
		m_pInfoProvider->GetBlockList(m_nStaBegin, lines);//GetTccLineList(m_nStaBegin, lines);
		if(0 == lines.size())
		{
			str.Format("%s(%d) û����·", m_pInfoProvider->GetStationName(m_nStaBegin), m_nStaBegin);
			MessageBox(str);
			return ;
		}
		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(m_nStaBegin, lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);
		OnCbnSelchangeTccLine();
						}break;
	}
}

void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta2()
{
	m_nLineID   = 0;
	m_nMinKm    = 0;
	m_nMaxKm    = 0;
	m_szKmBegin  = "";
	m_szKmEnd    = "";

	INT nIndex = m_StaEnd.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_AbsLine.SetCurSel(-1);
		OnCbnSelchangeTccLine();
		m_AbsLine.EnableWindow(FALSE);
		return;
	}
	ASSERT(0 < m_nStaBegin);
	m_nStaEnd = (stano_t)m_StaEnd.GetItemData(nIndex);

	m_AbsLine.EnableWindow(TRUE);
	m_AbsLine.ResetContent();

	std::vector<lineid_t> lines;
	m_pInfoProvider->GetTccLineList(m_nStaBegin, m_nStaEnd, lines);
	if(0 == lines.size())
	{
		CString str;
		str.Format("%s(%d)��%s(%d) û����·", 
			m_pInfoProvider->GetStationName(m_nStaBegin), m_nStaBegin,
			m_pInfoProvider->GetStationName(m_nStaEnd), m_nStaEnd
			);
		MessageBox(str);
		return ;
	}

	for(int i=0; i<lines.size(); i++)
	{
		nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(m_nStaBegin, lines[i]) );
		ASSERT(CB_ERR != nIndex);
		m_AbsLine.SetItemData(nIndex, lines[i]);
	}
	m_AbsLine.SetCurSel(-1);
	OnCbnSelchangeTccLine();
}

// ��·ѡ���, ���ٷ�������ֵ�б����ű仯, �������÷�ΧӦ���ű仯
void CTccCommandInputDlg::OnCbnSelchangeTccLine()
{
	UpdateData(TRUE);
	m_nMinKm    = 0;
	m_nMaxKm    = 0;
	m_szKmBegin  = "";
	m_szKmEnd    = "";
	UpdateData(FALSE);
	//OnAdjustKmMarker();
	m_ValidKmMarker.SetWindowText("");
	m_AdjustKmMarker.SetWindowText("");
	m_nKmBeginAJUST = m_nKmEndAJUST = -1;
	m_bKmMarkAdjustAck = FALSE;
	m_TccList.Clear();

	INT nIndex;
	nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);
		m_Kcl[0].EnableWindow(FALSE);
		m_Kcl[1].EnableWindow(FALSE);
		m_Mcl[0].EnableWindow(FALSE);
		m_Mcl[1].EnableWindow(FALSE);

		m_btnDirDown.EnableWindow(FALSE);
		m_btnDirUp.EnableWindow(FALSE);
		m_btnDirDual.EnableWindow(FALSE);
		m_BtnEn=false;
		//m_LimitVal[0].SetCurSel(-1);
		//m_LimitVal[0].EnableWindow(FALSE);
		//m_TrainID[0].SetWindowText("");
		//m_TrainID[0].EnableWindow(FALSE);
		//m_Limit[1].EnableWindow(FALSE);
		m_Limit[0].SetCheck(BST_UNCHECKED);
		
		OnSelDirection();
		return;
	}
	//m_LimitVal[0].EnableWindow(TRUE);
	//m_TrainID[0].EnableWindow(TRUE);
	//m_Limit[1].EnableWindow(TRUE);
	m_Limit[0].SetCheck(BST_UNCHECKED);
	m_BtnEn=true;       //������ʾ����
	OnBtnClickedLimit();
	m_Limit[0].SetCheck(BST_CHECKED);

	dir_t dir = DIR_UNKNOWN;
	switch(m_nCmdType)
	{
	default:
		ASSERT(FALSE);
		break;
	case ABSAPP_LIMIT:
	case ABSAPP_FULL_LIMIT: 
	case STATION_LIMIT: {
		lineid_t tccline_id = (lineid_t)m_AbsLine.GetItemData(nIndex);
		dir = m_pInfoProvider->GetTccLineDirection(m_nStaBegin, tccline_id);
		m_nLineID = m_pInfoProvider->GetLineID(m_nStaBegin, tccline_id);
					  } break;
	case KM_LIMIT: {
		m_nLineID = m_AbsLine.GetItemData(nIndex);
		dir = m_pInfoProvider->GetLineDirection(m_nLineID);
				   } break;
	}

	switch(dir)
	{
	case DIR_DOWN:
		m_nDirection = 0;
		break;
	case DIR_UP:
		m_nDirection = 1;
		break;
	case DIR_DUAL:
		m_nDirection = 2;
		break;
	case DIR_UNKNOWN:
	default:
		m_nDirection = 2;
		break;
	}
	//m_nDirection2 = m_nDirection = 2;	//Ŀǰֻ����˫������
	UpdateData(FALSE);
	OnSelDirection();
}

void CTccCommandInputDlg::OnSelDirection()
{
	for(int i=0; i<5; i++)
	{
		m_LimitVal[i].ResetContent();
		m_TrainID[i].SetWindowText("");
	}
	OnBtnClickedLimit();

	INT nIndex;
	nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	if( !UpdateData(TRUE) )
		return ;

	dir_t dir = DIR_DUAL;
	switch(m_nDirection)
	{
	case 0:
		dir = DIR_DOWN;
		break;
	case 1:
		dir = DIR_UP;
		break;
	case 2:
		dir = DIR_DUAL;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	CString str;
	std::vector<int> availabespeeds;

	switch(m_nCmdType)
	{
	//ȫ������
	case ABSAPP_FULL_LIMIT: {
		lineid_t tccline_id = (lineid_t)m_AbsLine.GetItemData(nIndex);
		m_pInfoProvider->GetTccLineSpeedList(m_nStaBegin, tccline_id, dir, availabespeeds);

		kmmark_t nKmBegin = 0,	nKmEnd = 0;
		kmmark_t nKmBegin2 = 0,	nKmEnd2 = 0;
		BOOL bFullSecLimit = FALSE;
		dir_t dir = m_pInfoProvider->GetStationDirection(m_nStaBegin, m_nStaEnd);
		switch(dir)
		{
		case DIR_DOWN:
			m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaBegin, tccline_id, DIR_DOWN, &nKmBegin, &nKmEnd);
			m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaEnd, tccline_id, DIR_UP, &nKmBegin2, &nKmEnd2);
			break;
		case DIR_UP:
			m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaEnd, tccline_id, DIR_DOWN, &nKmBegin, &nKmEnd);
			m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaBegin, tccline_id, DIR_UP, &nKmBegin2, &nKmEnd2);
			break;
		default:
			ASSERT(FALSE);
			m_AdjustKmMarker.SetWindowText("�������:����վ�����Բ���ȷ");
			return;
			break;
		}

		int nHitFullSecNum = 0;
		int nHitFullSecNum2= 0;
		m_pInfoProvider->GetFullLimitKmMarker(m_nLineID, &nKmBegin, &nKmEnd, &nHitFullSecNum);
		m_pInfoProvider->GetFullLimitKmMarker(m_nLineID, &nKmBegin2, &nKmEnd2, &nHitFullSecNum2);
		if(nHitFullSecNum == 1 && nHitFullSecNum2 == 1
			&& nKmBegin.Equal(nKmBegin2) && nKmEnd.Equal(nKmEnd2)
			)
		{
			m_szKmBegin = nKmBegin.toString(m_pInfoProvider, m_nLineID);
			m_szKmEnd   = nKmEnd.toString(m_pInfoProvider, m_nLineID);
			m_nMinKm = nKmBegin;
			m_nMaxKm = nKmEnd;
			bFullSecLimit = TRUE;
		}
		else if(nHitFullSecNum >= 1 || nHitFullSecNum2 >= 1)
		{
			m_AdjustKmMarker.SetWindowText("�޷�ȷ��ȫ�����ٷ�Χ�����ܴ����м�վ������ͨ�����빫���������");
			return;
		}
		else
		{
			m_AdjustKmMarker.SetWindowText("�޷�ȷ��ȫ�����ٷ�Χ�������ļ��д�");
			return;
		}

		if(!bFullSecLimit)
		{
			m_nMinKm = 0,	m_nMaxKm = 9999999;
			switch(m_pInfoProvider->GetStationDirection(m_nStaBegin, m_nStaEnd))
			{
			case DIR_UP:	// ��������ȫ����
				m_szKmBegin = "";
				m_szKmEnd = "9999999";
				break;
			case DIR_DOWN:	// ��������ȫ����
				m_szKmBegin = "9999999";
				m_szKmEnd = "";
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		m_szKmBegin2 = m_szKmBegin,	m_szKmEnd2 = m_szKmEnd;
		UpdateData(FALSE);

		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);
		m_Kcl[0].EnableWindow(FALSE);
		m_Kcl[1].EnableWindow(FALSE);
		m_Mcl[0].EnableWindow(FALSE);
		m_Mcl[1].EnableWindow(FALSE);
		m_btnDirDown.EnableWindow(FALSE);
		m_btnDirUp.EnableWindow(FALSE);
		m_btnDirDual.EnableWindow(FALSE);
		m_BtnEn=false;
		str.Format("%s �� %s", m_nMinKm.toString(m_pInfoProvider, m_nLineID), m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
		m_ValidKmMarker.SetWindowText(str);

		str.Format("ȫ�����٣�%s �� %s", m_nMinKm.toString(m_pInfoProvider, m_nLineID), m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
		m_AdjustKmMarker.SetWindowText(str);
		m_nKmBeginAJUST = m_nMinKm;//m_nKmBegin
		m_nKmEndAJUST = m_nMaxKm;//m_nKmEnd
		m_bKmMarkAdjustAck = TRUE;

		AdjustKmMarker();

		// ��������վ
		/*std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT> tccs;
		RAW_STOCK_COMMAND::_TCC_STRUCT tcc_km;
		tcc_km.nID = m_nStaBegin;
		tcc_km.nTccLineID = tccline_id;
		tcc_km.nKmMark[0] = m_nKmBegin;
		tcc_km.nKmMark[1]   = bFullSecLimit ? m_nKmEnd : m_nKmBegin;
		tccs.push_back(tcc_km);

		tcc_km.nID = m_nStaEnd;
		tcc_km.nTccLineID = m_pInfoProvider->GetTccLine(m_nStaEnd, m_nStaBegin, tccline_id);
		tcc_km.nKmMark[0] = bFullSecLimit ? m_nKmBegin : m_nKmEnd;
		tcc_km.nKmMark[1]   = m_nKmEnd;
		tccs.push_back(tcc_km);

		m_TccList.SetTccList(tccs, m_pInfoProvider);*/
							}
		break;

	//�������٣���Ч����귶ΧΪ������վ��Ͻ�Ĺ�������
	case ABSAPP_LIMIT: {
		lineid_t tccline_id = (lineid_t)m_AbsLine.GetItemData(nIndex);
		m_pInfoProvider->GetTccLineSpeedList(m_nStaBegin, tccline_id, dir, availabespeeds);

		kmmark_t nKmBegin = 0,	nKmEnd = 0,	nKmBegin2 = 0,	nKmEnd2 = 0;
		if(m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaBegin, tccline_id, DIR_DUAL, &nKmBegin, &nKmEnd)
			&& m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaEnd, tccline_id, DIR_DUAL, &nKmBegin2, &nKmEnd2))
		{
			if(nKmBegin.Greater(nKmBegin2, m_nLineID))
			{
				m_szKmBegin = nKmBegin.toString(m_pInfoProvider, m_nLineID);
				m_nMinKm = nKmBegin;
			}
			else
			{
				m_szKmBegin = nKmBegin2.toString(m_pInfoProvider, m_nLineID);
				m_nMinKm = nKmBegin2;
			}
			if(nKmEnd.Less(nKmEnd2, m_nLineID))
			{
				m_szKmEnd = nKmEnd.toString(m_pInfoProvider, m_nLineID);
				m_nMaxKm = nKmEnd;
			}
			else
			{
				m_szKmEnd = nKmEnd2.toString(m_pInfoProvider, m_nLineID);
				m_nMaxKm = nKmEnd2;
			}
		}
		str.Format("%s �� %s", m_nMinKm.toString(m_pInfoProvider, m_nLineID), m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
		m_ValidKmMarker.SetWindowText(str);

		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);
		m_Kcl[0].EnableWindow(TRUE);
		m_Kcl[1].EnableWindow(TRUE);
		m_Mcl[0].EnableWindow(TRUE);
		m_Mcl[1].EnableWindow(TRUE);
		m_btnDirDown.EnableWindow(TRUE);
		m_btnDirUp.EnableWindow(TRUE);
		m_btnDirDual.EnableWindow(TRUE);
		m_BtnEn=TRUE;
		OnKmMarkerChanged();
					   }
		break;

	//վ�ڻ��������
	case STATION_LIMIT: {
		lineid_t tccline_id = (lineid_t)m_AbsLine.GetItemData(nIndex);
		m_pInfoProvider->GetTccLineSpeedList(m_nStaBegin, tccline_id, dir, availabespeeds);

		if(0 >= m_nLineID)
		{	//��������
			m_nMinKm = 0;
			m_szKmBegin = m_szKmBegin2 = "";
			m_nMaxKm = 9999999;
			m_szKmEnd = m_szKmEnd2 = "9999999";
			m_nDirection2 = m_nDirection;
			UpdateData(FALSE);

			m_KmBegin.EnableWindow(FALSE);
			m_MBegin.EnableWindow(FALSE);
			m_KmEnd.EnableWindow(FALSE);
			m_MEnd.EnableWindow(FALSE);
			m_Kcl[0].EnableWindow(FALSE);
			m_Kcl[1].EnableWindow(FALSE);
			m_Mcl[0].EnableWindow(FALSE);
			m_Mcl[1].EnableWindow(FALSE);
			m_btnDirDown.EnableWindow(FALSE);
			m_btnDirUp.EnableWindow(FALSE);
			m_btnDirDual.EnableWindow(FALSE);
			m_BtnEn=false;
			//OnAdjustKmMarker();
			str.Format("%s �� %s", 
				m_nMinKm.toString(m_pInfoProvider, m_nLineID), 
				m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
			m_ValidKmMarker.SetWindowText(str);
			m_AdjustKmMarker.SetWindowText("�������٣�����Ҫ�鵵");
			m_nKmBeginAJUST = m_nMinKm;//m_nKmBegin;
			m_nKmEndAJUST = m_nMaxKm;//m_nKmEnd;
			m_nKmBeginAJUST.flag = m_nKmEndAJUST.flag = m_pInfoProvider->GetBlockKmsysCode();
			m_bKmMarkAdjustAck = TRUE;

			// ��������վ
			std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT> tccs;
			RAW_STOCK_COMMAND::_TCC_STRUCT tcc_km;
			tcc_km.nID = m_nStaBegin;
			tcc_km.nTccLineID = tccline_id;
			tcc_km.nKmMarkFlag[0] = 0;
			tcc_km.nKmMarkFlag[1] = 0;
			if(!m_bCheckCommand)
			{
				for(i=0; i<m_cmd.tcc.size(); i++)
				{
					if(m_cmd.tcc[i].nID != tcc_km.nID
						|| m_cmd.tcc[i].nTccLineID != tcc_km.nTccLineID)
						continue;
					
					tcc_km.bTccValidate = m_cmd.tcc[i].bTccValidate;
					break;
				}
			}
			tccs.push_back(tcc_km);

			m_TccList.SetTccList(tccs, m_pInfoProvider);
		}
		else
		{
			m_pInfoProvider->GetTccLineKmMarkerScope(m_nStaBegin, tccline_id, dir, &m_nMinKm, &m_nMaxKm);
			str.Format("%s �� %s", m_nMinKm.toString(m_pInfoProvider, m_nLineID), m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
			m_ValidKmMarker.SetWindowText(str);

			m_KmBegin.EnableWindow(TRUE);
			m_MBegin.EnableWindow(TRUE);
			m_KmEnd.EnableWindow(TRUE);
			m_MEnd.EnableWindow(TRUE);
			m_Kcl[0].EnableWindow(TRUE);
			m_Kcl[1].EnableWindow(TRUE);
			m_Mcl[0].EnableWindow(TRUE);
			m_Mcl[1].EnableWindow(TRUE);
			m_btnDirDown.EnableWindow(TRUE);
			m_btnDirUp.EnableWindow(TRUE);
			m_btnDirDual.EnableWindow(TRUE);
			m_BtnEn=TRUE;
			OnKmMarkerChanged();
		}
				   }
		break;

	case KM_LIMIT: {
		m_pInfoProvider->GetLineSpeedList(m_nLineID, dir, availabespeeds);

		m_pInfoProvider->GetLineKmMarkerScope(m_nLineID, dir, &m_nMinKm, &m_nMaxKm);
		str.Format("%s �� %s", m_nMinKm.toString(m_pInfoProvider, m_nLineID), m_nMaxKm.toString(m_pInfoProvider, m_nLineID));
		m_ValidKmMarker.SetWindowText(str);

		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);
		m_Kcl[0].EnableWindow(TRUE);
		m_Kcl[1].EnableWindow(TRUE);
		m_Mcl[0].EnableWindow(TRUE);
		m_Mcl[1].EnableWindow(TRUE);
		m_btnDirDown.EnableWindow(TRUE);
		m_btnDirUp.EnableWindow(TRUE);
		m_btnDirDual.EnableWindow(TRUE);
		m_BtnEn=TRUE;
		OnKmMarkerChanged();
				   }
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	// ��ʼ������ֵ�б�
	for(int i=0; i<availabespeeds.size(); i++)
	{
		if(availabespeeds[i] < 45)
			str.Format("%d (45)", availabespeeds[i]);
		else
			str.Format("%d", availabespeeds[i]);
		for(int j=0; j<5; j++)
		{
			nIndex = m_LimitVal[j].AddString(str);
			ASSERT(CB_ERR != nIndex);
			m_LimitVal[j].SetItemData(nIndex, availabespeeds[i]);
		}
	}
}

void CTccCommandInputDlg::OnKmMarkerChanged()
{
	if( !UpdateData(TRUE) )
		return ;

	if(m_szKmBegin2 != m_szKmBegin
		|| m_szKmEnd2 != m_szKmEnd
		|| m_nDirection2 != m_nDirection)
	{
		AdjustKmMarker();

		m_szKmBegin2 = m_szKmBegin;
		m_szKmEnd2   = m_szKmEnd;
		m_nDirection2 = m_nDirection;
	}
}

void CTccCommandInputDlg::AdjustKmMarker()
{
	CString str;
	m_nKmBeginAJUST=-1;
	m_nKmEndAJUST=-1;

	int nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_AdjustKmMarker.SetWindowText("");
		m_TccList.Clear();
		return;
	}
	ASSERT(m_nLineID > 0);

	kmmark_t nKmBegin,	nKmEnd;
	const LINE_STRUCT *pLine = GetLineStruct(m_nLineID);
	if(NULL == pLine)
		return;
	
	if(!parseKmMark(*pLine, m_szKmBegin, &nKmBegin))
	{
		str.Format("��ʼ����� %s ���Ϸ�", m_szKmBegin);
		m_AdjustKmMarker.SetWindowText(str);
		return;
	}
	if(!parseKmMark(*pLine, m_szKmEnd, &nKmEnd))
	{
		str.Format("��������� %s ���Ϸ�", m_szKmEnd);
		m_AdjustKmMarker.SetWindowText(str);
		return;
	}

	if(nKmBegin.Equal(0) && 9999999 != nKmEnd.info.val || 9999999 != nKmBegin.info.val && nKmEnd.Equal(0))
		 return;

	//switch(m_nCmdType)
	//{
	//case STATION_LIMIT: {
	//	lineid_t tccline_id = m_AbsLine.GetItemData(nIndex);
	//					} break;
	//case ABSAPP_LIMIT:
	//case ABSAPP_FULL_LIMIT: {
	//	lineid_t tccline_id = m_AbsLine.GetItemData(nIndex);
	//						} break;
	//case KM_LIMIT:
	//	break;
	//default:
	//	ASSERT(FALSE);
	//	break;
	//}

	m_TccList.Clear();

	if(nKmBegin.Greater(nKmEnd, m_nLineID))
	{
		kmmark_t temp = nKmBegin;
		nKmBegin = nKmEnd;
		nKmEnd   = temp;
	}
	//if(nKmEnd.info.val - nKmBegin.info.val > 6000)
	//{
	//	m_AdjustKmMarker.SetWindowText("����6Km");
	//	return;
	//}

	if(nKmBegin.Equal(nKmEnd))
	{
		m_AdjustKmMarker.SetWindowText("���ٿ�ʼ�����������ȣ��޷��鵵");
		return ;
	}
	if(nKmBegin.Less(m_nMinKm, m_nLineID))
	{
		str.Format("���ٹ���� %s ������Ч������ڣ��޷��鵵",
			nKmBegin.toString(m_pInfoProvider, m_nLineID));

		m_AdjustKmMarker.SetWindowText(str);
		return;
	}
	if(nKmEnd.Greater(m_nMaxKm, m_nLineID))
	{
		str.Format("���ٹ���� %s ������Ч������ڣ��޷��鵵",
			nKmEnd.toString(m_pInfoProvider, m_nLineID));

		m_AdjustKmMarker.SetWindowText(str);
		return;
	}
    
	dir_t dir = DIR_DUAL;
	switch(m_nDirection)
	{
	case 0:
		dir = DIR_DOWN;
		break;
	case 1:
		dir = DIR_UP;
		break;
	case 2:
		dir = DIR_DUAL;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	int nHitTrackNum = 0;
	if(m_pInfoProvider->AdjustKmMarker(m_nLineID, dir, &nKmBegin, &nKmEnd, &nHitTrackNum))
	{
		if(TRUE || nHitTrackNum <= 5)
		{
		//long nStaMinKm = 0,		nStaMaxKm = 0;
		
		//nIndex = m_TccStation.GetCurSel();
		//ASSERT(CB_ERR != nIndex);
		//stano_t nTccID = (stano_t)m_TccStation.GetItemData(nIndex);

		//m_pInfoProvider->GetTccLineKmMarkerScope(nTccID, builtin_id, dir, &nStaMinKm, &nStaMaxKm);
		//if(min(nKmEnd, nStaMaxKm) - max(nKmBegin, nStaMinKm) > 0)
		//{
			str.Format("%s �� %s", nKmBegin.toString(m_pInfoProvider, m_nLineID), nKmEnd.toString(m_pInfoProvider, m_nLineID));
			m_AdjustKmMarker.SetWindowText(str);
			m_nKmBeginAJUST=nKmBegin;
			m_nKmEndAJUST=nKmEnd;
		//}
		//else
		//{
		//	m_nKmBeginAJUST=-1;
		//	m_nKmEndAJUST=-1;
		//	m_AdjustKmMarker.SetWindowText("�޷��鵵, Ŀ������վ���ڹ鵵����귶Χ��");
		//	return;
		//}
		}
		else
		{
			int nHitFullSecNum = 0;
			m_pInfoProvider->GetFullLimitKmMarker(m_nLineID, &nKmBegin, &nKmEnd, &nHitFullSecNum);
			if(nHitFullSecNum == 1)
			{
				str.Format("ȫ�����٣�%s �� %s", nKmBegin.toString(m_pInfoProvider, m_nLineID), nKmEnd.toString(m_pInfoProvider, m_nLineID));
				m_AdjustKmMarker.SetWindowText(str);
				m_nKmBeginAJUST=nKmBegin;
				m_nKmEndAJUST=nKmEnd;
			}
			else
			{
				m_nKmBeginAJUST=-1;
				m_nKmEndAJUST=-1;

				if(nHitFullSecNum > 1)
					m_AdjustKmMarker.SetWindowText("�޷�ȷ��ȫ�����ٷ�Χ�����ܴ����м�վ������С����귶Χ");
				else
					m_AdjustKmMarker.SetWindowText("����겻���κ�վ��ȫ�����ٷ�Χ�ڣ�����������");
				return ;
			}
		}
	}
	else
	{
		m_nKmBeginAJUST=-1;
	    m_nKmEndAJUST=-1;
		m_AdjustKmMarker.SetWindowText("�޷��鵵");
		return ;
	}

	std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT> tccs;
	m_pInfoProvider->GetHitTccList(m_nLineID, dir, nKmBegin, nKmEnd, tccs);
	if(!m_bCheckCommand)
	{
		for(int i=0; i<tccs.size(); i++)
		{
			for(int j=0; j<m_cmd.tcc.size(); j++)
			{
				if(m_cmd.tcc[j].nID != tccs[i].nID
					|| m_cmd.tcc[j].nTccLineID != tccs[i].nTccLineID)
					continue;
				if(m_cmd.tcc[j].nKmMarkFlag[0] != tccs[i].nKmMarkFlag[0]
					|| m_cmd.tcc[j].nKmMarkFlag[1] != tccs[i].nKmMarkFlag[1])
					continue;

				tccs[i].bTccValidate = m_cmd.tcc[j].bTccValidate;
				break;
			}
		}
	}

	// ��������վ
	m_TccList.SetTccList(tccs, m_pInfoProvider);
}

void CTccCommandInputDlg::OnBnClickedBtnAdjustAck() //IDC_BTN_ADJUST_ACK
{
	if(m_nKmBeginAJUST.GetDWORD() == -1 || m_nKmEndAJUST.GetDWORD() == -1)
		MessageBox("�鵵���ɹ�!");
	else
	{
		m_szKmBegin=m_nKmBeginAJUST.toString(m_pInfoProvider, m_nLineID);
		m_szKmEnd=m_nKmEndAJUST.toString(m_pInfoProvider, m_nLineID);
		UpdateData(FALSE);

		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);
		m_Kcl[0].EnableWindow(FALSE);
		m_Kcl[1].EnableWindow(FALSE);
		m_Mcl[0].EnableWindow(FALSE);
		m_Mcl[1].EnableWindow(FALSE);

		m_bKmMarkAdjustAck = TRUE;

		m_BtnEn=false;
	}
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

void CTccCommandInputDlg::OnBtnClickedLimit()
{
	int i=0;
	if(m_Limit[i].GetCheck() &&  !m_bCancelLimit) // cuihu2008  add &&  !m_bCancelLimit
	{
		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		m_LimitVal[i].SetCurSel(-1);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
		for(int j=i+1; j<5; j++)
		{
			m_Limit[j].SetCheck(FALSE);
			m_Limit[j].EnableWindow(FALSE);

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
		}
	}
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
		m_LimitVal[i].SetCurSel(-1);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
		for(int j=i+1; j<5; j++)
		{
			m_Limit[j].SetCheck(FALSE);
			m_Limit[j].EnableWindow(FALSE);

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
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
		m_LimitVal[i].SetCurSel(-1);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
		for(int j=i+1; j<5; j++)
		{
			m_Limit[j].SetCheck(FALSE);
			m_Limit[j].EnableWindow(FALSE);

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
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
		m_LimitVal[i].SetCurSel(-1);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
		for(int j=i+1; j<5; j++)
		{
			m_Limit[j].SetCheck(FALSE);
			m_Limit[j].EnableWindow(FALSE);

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
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
		m_LimitVal[i].SetCurSel(-1);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
		for(int j=i+1; j<5; j++)
		{
			m_Limit[j].SetCheck(FALSE);
			m_Limit[j].EnableWindow(FALSE);

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
		}
	}
}

void CTccCommandInputDlg::OnSketchMap()
{
	UpdateData(TRUE);
	dir_t dir=DIR_UNKNOWN;
	switch (m_nDirection)
	{
	case 0:
		dir=DIR_DOWN;
		break;
	case 1:
		dir = DIR_UP;
		break;
	case 2:
		dir = DIR_DUAL;
		break;
	default :
		dir=DIR_UNKNOWN;
		break;
	}

	if((m_nKmBeginAJUST.GetDWORD() != -1 && m_nKmEndAJUST.GetDWORD() != -1)||m_BtnEn)
	{
		if(GetLineStruct(m_nLineID)==NULL)
		{
			MessageBox("��������û��ʾ��ͼ","��ʾ",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
			return;
		}
		kmmark_t kmBegin,kmEnd;
		m_pInfoProvider->GetLineKmMarkerScope(m_nLineID,dir,&kmBegin,&kmEnd);
		if (kmBegin.Equal(kmEnd))
		{
			MessageBox("����Ч����꣬����������","��ʾ",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
			return;
		}
		CTccCommandSketchMapDlg dlg(m_pInfoProvider, this);
		dlg.SetKmMark(m_nLineID, m_nKmBeginAJUST, m_nKmEndAJUST);
		dlg.m_BtnEn=m_BtnEn;
		dlg.m_SketchMap.dir_limit=dir;
		if(dlg.DoModal()==IDOK)
		{
			m_szKmBegin=dlg.m_SketchMap.m_nKmMark[0].toString(m_pInfoProvider, m_nLineID);
			m_szKmEnd=dlg.m_SketchMap.m_nKmMark[1].toString(m_pInfoProvider, m_nLineID);
			UpdateData(false);
			AdjustKmMarker();
		}
	}
}

void CTccCommandInputDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	if( !UpdateData(TRUE) )
		return ;

	if(!m_bKmMarkAdjustAck)
	{
		MessageBox("���ȹ鵵!");
		return;
	}

	RAW_STOCK_COMMAND cmd = m_cmd;
	//BOOL bNewCmd = m_cmd.nCmdID == 0 ? TRUE : FALSE;
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
		//cmd.bFullLimit = (ABSAPP_FULL_LIMIT == m_nCmdType);
		cmd.nCmdType = m_nCmdType;
		switch(m_nDirection)
		{
		case 0:	//����
			cmd.nDirection = 1;
			break;
		case 1:	//����
			cmd.nDirection = 2;
			break;
		case 2:	//˫��
			cmd.nDirection = 3;
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		// �����
		cmd.nCmdID = (cmdid_t)m_nCmdID;
		if(0 == cmd.nCmdID)
		{
			prompt.Format("����Ų���Ϊ0");
			break;
		}
		if(!m_bCancelLimit && 0 == m_cmd.nCmdID
			&& !m_pInfoProvider->IsValidCommandID(cmd.nCmdID))
		{	//�����������ű����ںϷ���Χ�ڣ��Ѿ�������ŵĲ����Ϸ��ԣ��Ա㲻ͬ��GPC�ն˷ֱ��������ͼ���
			prompt.Format("��������ű����ڷ�Χ%d--%d��", 
				m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID());
			break;
		}

		// ��ʼվ
		nIndex = m_StaBegin.GetCurSel();
		if(KM_LIMIT == m_nCmdType)
			cmd.nStaID[0] = 0;
		else
		{
			if(CB_ERR == nIndex)
			{
				prompt.Format("��ѡ��ʼվ!");
				break;
			}
			cmd.nStaID[0] = m_nStaBegin;
		}
		// ����վ
		nIndex = m_StaEnd.GetCurSel();
		if(ABSAPP_LIMIT == m_nCmdType || ABSAPP_FULL_LIMIT == m_nCmdType)
		{
			if(CB_ERR == nIndex)
			{
				prompt.Format("��ѡ�����վ!");
				break;
			}
			cmd.nStaID[1] = m_nStaEnd;
		}
		else
			cmd.nStaID[1] = 0;

		// ��·��
		nIndex = m_AbsLine.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("��ѡ����·!");
			break;
		}
		switch(m_nCmdType)
		{
		case STATION_LIMIT:
		case ABSAPP_LIMIT:
		case ABSAPP_FULL_LIMIT:
			cmd.nLineID = m_pInfoProvider->GetLineID(cmd.nStaID[0], m_AbsLine.GetItemData(nIndex));
			break;
		case KM_LIMIT:
			cmd.nLineID = m_AbsLine.GetItemData(nIndex);
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		// �����
		cmd.nKmMark[0] = m_nKmBeginAJUST;//m_nKmBegin;
		cmd.nKmMark[1] = m_nKmEndAJUST;//m_nKmEnd;
		if(cmd.nKmMark[0].Equal(cmd.nKmMark[1])
			&& (cmd.nKmMark[0].info.val == 0 || cmd.nKmMark[0].info.val == 9999999)
			&& !cmd.bFullLimit)
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
		m_TccList.GetTccList(cmd.tcc);
		if(cmd.tcc.size() == 0)
		{
			prompt.Format("û������վ�����ǺϷ�����!");
			break;
		}

		// ����ÿ�����ٵ�״̬�б�
		for(int i=0; i<sizeof(cmd.train_cmd)/sizeof(cmd.train_cmd[0]); i++)
		{
			cmd.train_cmd[i].tcc.resize(cmd.tcc.size());
		}

		/*cmd.tcc.clear();
		RAW_STOCK_COMMAND::_TCC_STRUCT tcc;
		for(i=m_TccList.GetFixedRowCount(); i<m_TccList.GetRowCount(); i++)
		{
			tcc.nID = (stano_t)m_TccList.GetItemData(i, 0);
			tcc.nTccLineID = (lineid_t)m_TccList.GetItemData(i, 1);
			if(0 == tcc.nTccLineID)
				break;

			CGridCellCheck *pCell = NULL;
			pCell = (CGridCellCheck *)m_TccList.GetCell(i, 2);
			ASSERT(pCell != NULL);
            tcc.bTccValidate = pCell->GetCheck() ? 1 : 0;
			tcc.nKmMark[0] = m_TccList.GetItemData(i, 3);
			tcc.nKmMark[1] = m_TccList.GetItemData(i, 4);

			cmd.tcc.push_back(tcc);
		}
		if(m_TccList.GetRowCount()!=i)
		{
			prompt.Format("����վ%d����·��Ϊ0, ���������ļ�!", tcc.nID);
			break;
		}
		if(cmd.tcc.size() == 0)
		{
			prompt.Format("û������վ�����ǺϷ�����!");
			break;
		}
		for(i=0; i<sizeof(cmd.train_cmd)/sizeof(cmd.train_cmd[0]); i++)
		{
			if(cmd.train_cmd[i].nSpeedMaxVal > 0)
				cmd.train_cmd[i].tcc.resize(cmd.tcc.size());
		}*/

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

/*void CTccCommandInputDlg::OnBtnClickedCancelLimit()
{
	if(m_CancelLimit.GetCheck() == BST_CHECKED)
	{
		m_bCancelLimit = TRUE;
		m_bGenCancelCommand = FALSE;
		//m_GenCancelCommand.EnableWindow(FALSE);
		//m_Limit[1].SetCheck(BST_UNCHECKED);
		//OnBtnClickedLimit2();
		//m_Limit[1].EnableWindow(FALSE);
		m_Limit[0].SetCheck(BST_UNCHECKED);  // cuihu 2008
	}
	else
	{
		m_bCancelLimit = FALSE;
		//m_GenCancelCommand.EnableWindow(TRUE);
		//if(m_Limit[0].GetCheck() == BST_CHECKED)
		//	m_Limit[1].EnableWindow(TRUE);
		m_Limit[0].SetCheck(BST_CHECKED);  // cuihu 2008
	}
	OnBtnClickedLimit();    // cuihu 2008
}*/

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


CTccCommandInputDlg::CTccList::CTccList()
{
}
CTccCommandInputDlg::CTccList::~CTccList()
{
}

BOOL CTccCommandInputDlg::CTccList::Create(const RECT& rect, CWnd *pParent, UINT nID)
{
	if(!grid.Create(rect, pParent, nID))
		return FALSE;

	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"����վ(У��)", 200, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"��·", 60, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		//{"У��", 60, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"��ʼ�����",   0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"���������", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"Ŀ���п�", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
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
		return FALSE;
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
	grid.SetEditable(FALSE);

	return TRUE;
}
void CTccCommandInputDlg::CTccList::Clear()
{
	m_tccs.clear();
	grid.SetRowCount(grid.GetFixedRowCount());
}
void CTccCommandInputDlg::CTccList::SetTccList(vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tccs, ITccClientInfoProvider *pInfoProvider)
{
	Clear();

	ASSERT(pInfoProvider);

	CString str, str2;

	// ��������վ
	for(int i=0; i<tccs.size(); i++)
	{
		int builtin_id = pInfoProvider->GetLineID(tccs[i].nID, tccs[i].nTccLineID);

		stano_t nParentStaID = tccs[i].nID;
		if(pInfoProvider->IsZC_TccID(tccs[i].nID))
			nParentStaID = pInfoProvider->GetParentStaID(tccs[i].nID);

		int row = -1;
		/*for(int j=0; j<m_tccs.size(); j++)
		{
			if(m_tccs[j].nID == nParentStaID)
			{
				row = j + grid.GetFixedRowCount();
				break;
			}
		}*/
		if(-1 == row)
		{
			m_tccs.push_back(tccs[i]);
			//m_tccs[j].nID = nParentStaID;

			str.Format("%s (%d)",  pInfoProvider->GetStationName(nParentStaID), nParentStaID);
			row = grid.InsertRow(str);
			grid.SetItemData(row, 0, nParentStaID);
			grid.SetCellType(row, 0, RUNTIME_CLASS(CGridCellCheck));

			CGridCellBase *pCell = NULL;
			pCell = grid.GetCell(row, 0);
			ASSERT(pCell != NULL && pCell->IsKindOf(RUNTIME_CLASS(CGridCellCheck)));
			CGridCellCheck *pCheck = (CGridCellCheck *)pCell;
			pCheck->SetCheck(tccs[i].bTccValidate);

			str.Format("%d", tccs[i].nTccLineID);
			grid.SetItemText(row, 1, str);

			//str.Format("%s", tccs[i].nKmMark[0].toString(pInfoProvider, builtin_id));
			str.Format("%d", tccs[i].nKmMarkFlag[0]);
			grid.SetItemText(row, 2, str);

			//str.Format("%s", tccs[i].nKmMark[1].toString(pInfoProvider, builtin_id));
			str.Format("%d", tccs[i].nKmMarkFlag[1]);
			grid.SetItemText(row, 3, str);

			str.Format("%s(%d)",  pInfoProvider->GetStationName(tccs[i].nID), tccs[i].nID);
			grid.SetItemText(row, 4, str);
			//grid.SetItemData(row, 4, tccs[i].nTccID);
		}
		/*else
		{
			kmmark_t nKmBegin = tccs[i].nKmMark[0];
			if(nKmBegin.Greater(m_tccs[j].nKmMark[0], builtin_id))
				nKmBegin = m_tccs[j].nKmMark[0];

			str.Format("%s", nKmBegin.toString(pInfoProvider, builtin_id));
			grid.SetItemText(row, 2, str);

			kmmark_t nKmEnd = tccs[i].nKmMark[1];
			if(nKmEnd.Less(m_tccs[j].nKmMark[1], builtin_id))
				nKmEnd = m_tccs[j].nKmMark[1];

			str.Format("%s", nKmEnd.toString(pInfoProvider, builtin_id));
			grid.SetItemText(row, 3, str);

			str.Format("%s, %s(%d)",  
				grid.GetItemText(row, 5),
				pInfoProvider->GetStationName(tccs[i].nID), tccs[i].nID);
			grid.SetItemText(row, 4, str);
		}*/

	}
	grid.Refresh();
}

void  CTccCommandInputDlg::CTccList::GetTccList(vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tccs)
{
	for(int j=grid.GetFixedRowCount(); j<grid.GetRowCount(); j++)
	{
		CGridCellBase *pCell = NULL;
		pCell = grid.GetCell(j, 0);
		ASSERT(pCell != NULL);
		if(pCell->IsKindOf(RUNTIME_CLASS(CGridCellCheck)))
		{
			CGridCellCheck *pCheck = (CGridCellCheck *)pCell;
			m_tccs[j - grid.GetFixedRowCount()].bTccValidate = pCheck->GetCheck() ? 1 : 0;
		}
	}
	tccs = m_tccs;
}

};

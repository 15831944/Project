/*
 * �п� ��ǰ���������б�
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"
#include "TccCommandSendDlg.h"
#include "TccCommand_FullCancel.h"
#include "PasswordInputDlg.h"

namespace TCC_C3 {


// CTCCmdStatusPage �Ի���

IMPLEMENT_DYNAMIC(CLircCommandPage, CPropertyPage)
CLircCommandPage::CLircCommandPage()
	: CPropertyPage(CLircCommandPage::IDD)
{
}

CLircCommandPage::~CLircCommandPage()
{
}

CTccDlg* CLircCommandPage::GetTccDlg()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd);
	pWnd = pWnd->GetParent();
	ASSERT(pWnd);

	return (CTccDlg*)pWnd;
}

ITccAdviseSink* CLircCommandPage::GetTccAdviseSink()
{
	return GetTccDlg()->GetTccAdviseSink();
}

ITccClientInfoProvider* CLircCommandPage::GetTccClientInfoProvider()
{
	return GetTccDlg()->GetTccClientInfoProvider();
}

stano_t CLircCommandPage::GetCurrentTccID()
{
	return GetTccDlg()->GetCurrentTccID();
}

void CLircCommandPage::Refresh()
{
	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("��ѡ��վ!");
		return ;
	}

	// ���ʵʱ�����б�
	OnRequestTccCmdList();
}

void CLircCommandPage::AppendCommand(TCC_COMMAND& cmd, DWORD nStatus)
{
	if(cmd.nTccID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	CListCtrl *pListCtrl = &m_CmdList;

	CString str;

	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));

	for(int i=0; i<pListCtrl->GetItemCount(); i++)
	{
		TCC_COMMAND *pCmd = (TCC_COMMAND *)pListCtrl->GetItemData(i);
		if(pCmd->nConsoleID == cmd.nConsoleID
			&& pCmd->nCmdID == cmd.nCmdID
			&& pCmd->sys_id == cmd.sys_id)
		{
			*pCmd = cmd;
			break;
		}
	}
	if(i == pListCtrl->GetItemCount())
	{
		// ��ƨ�ɺ�����
		TCC_COMMAND *pCmd = new TCC_COMMAND;
		ASSERT(pCmd);
		*pCmd = cmd;

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		ASSERT(0 == COL_CMDID);

		lvi.iItem  = pListCtrl->GetItemCount();
		lvi.iSubItem = 0;
		str.Format("%4d", cmd.nCmdID);
		lvi.pszText = (LPSTR)(LPCSTR)str;
		lvi.lParam = (DWORD)pCmd;
		lvi.iItem = pListCtrl->InsertItem(&lvi);
	}
	else
	{
		lvi.iItem = i;
	}

	lvi.mask = LVIF_TEXT;

	// ��ʼվ
	lvi.iSubItem = COL_BEGINSTA;
	if(cmd.nStaID[0])
		str = pInfoProvider->GetStationName(cmd.nStaID[0]);
	else
		str.Empty();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ����վ
	lvi.iSubItem = COL_ENDSTA;
	if(cmd.nStaID[1])
        str = pInfoProvider->GetStationName(cmd.nStaID[1]);
	else
		str.Empty();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	int builtin_id = pInfoProvider->GetLineID(cmd.nTccID, cmd.nTccLineID);
	// ������
	lvi.iSubItem = COL_ABSLINEID;
	str.Format("%s(builtin_id=%d)", pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nLineID);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ��ʼ�����
	lvi.iSubItem = COL_START_KM;
	if(cmd.bFullLimit)
		str.Format("ȫ����");
	else
		str = cmd.nKmMark[0].toString(pInfoProvider, builtin_id);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ���������
	lvi.iSubItem = COL_END_KM;
	if(cmd.bFullLimit)
		str.Format("ȫ����");
	else
		str = cmd.nKmMark[1].toString(pInfoProvider, builtin_id);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ����ֵ
	lvi.iSubItem = COL_SPEEDVAL;
	if(cmd.bCancelLimit)
		str.Format("ȡ������");
	else
		str.Format("%3dKm/h", cmd.nSpeedMaxVal);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ���ٿ�ʼʱ��
	lvi.iSubItem = COL_START_TIME;
	CTime ct;
	if(cmd.bStartNow || cmd.nTime[0] <= 0)
		str.Format("������ʼ");
	else
	{
		ct = cmd.nTime[0];
		str = ct.Format("%m/%d %H:%M");
	}
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ���ٽ���ʱ��
	lvi.iSubItem = COL_END_TIME;
	if(cmd.bEndless || cmd.nTime[1] <= 0)
		str.Format("������Ч");
	else
	{
		ct = cmd.nTime[1];
		str = ct.Format("%m/%d %H:%M");
	}
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ״̬
}

void CLircCommandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TCC_CMD_LIST, m_CmdList);
}

void CLircCommandPage::InitListCtrl(CListCtrl *pListCtrl)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"�����",		60, LVCFMT_RIGHT },
		{"��ʼվ",      0, LVCFMT_LEFT },
		{"����վ",      0, LVCFMT_LEFT },
		{"������",		100, LVCFMT_LEFT },
		{"��ʼ�����",	80, LVCFMT_LEFT },
		{"���������",	80, LVCFMT_LEFT },
		{"����ֵ",		60, LVCFMT_LEFT },
		{"��ʼʱ��",	80, LVCFMT_LEFT },
		{"����ʱ��",	80, LVCFMT_LEFT },
	};

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int j = 0; j < sizeof(colinfo)/sizeof(colinfo[0]); j++)
	{
		lvc.iSubItem = j;
		lvc.pszText = colinfo[j].label;
		lvc.cx = colinfo[j].width;
		lvc.fmt = colinfo[j].fmt;
		pListCtrl->InsertColumn(j, &lvc);
	}
}

BOOL CLircCommandPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(NULL == pInfoProvider)
	{
		MessageBox("û�а�װTccClientInfoProvider�ӿ�!");
		return FALSE;
	}
	ITccAdviseSink *pAdviseSink = GetTccAdviseSink();
	if(NULL == pAdviseSink)
	{
		MessageBox("û�а�װTccAdviseSink�ӿ�!");
		return FALSE;
	}

	InitListCtrl(&m_CmdList);

	CRect rcl;
	GetDlgItem(IDC_TCC_STATUS_LIST)->GetWindowRect(&rcl);
	ScreenToClient(&rcl);
	m_StccStatusGrid.Create(rcl, this, 102);

	//OnRequestTccCmdList();	// ����״̬�б�
	// Ȩ������
	if(!GetCancelRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_CANCELCMD_BTN);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	// Ȩ������
	if(!GetExecuteRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
		pWnd = GetDlgItem(IDC_TCC_COMMAND_FULLCANCEL);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BEGIN_MESSAGE_MAP(CLircCommandPage, CPropertyPage)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnRequestTccCmdList)
	ON_CBN_SELCHANGE(IDC_TCC_LIST, OnRequestTccCmdList)
	ON_BN_CLICKED(IDC_TCC_CANCELCMD_BTN, OnCancelLimitSpeed)
	ON_BN_CLICKED(IDC_TCC_INIT_COMPLETE, OnTccInitComplete)
	ON_BN_CLICKED(IDC_TCC_COMMAND_FULLCANCEL, OnTccCommand_FullCancel)
	ON_COMMAND(ID_CANCEL_LIMITSPEED, OnCancelLimitSpeed)
	ON_COMMAND(ID_CANCEL_LIMITSPEED_DIRECT, OnCancelLimitSpeedDirect)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_DBLCLK, IDC_TCC_CMD_LIST, OnNMDBClickCmdList)
END_MESSAGE_MAP()


// CLircCommandPage ��Ϣ�������

void CLircCommandPage::OnNMDBClickCmdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMLISTVIEW pNMLISTVIEW = (NMLISTVIEW*)pNMHDR;
	int nItem = pNMLISTVIEW->iItem;
	if(nItem < 0)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nItem);
	if(pcmd)
	{
		CString prompt = pcmd->toString( GetTccClientInfoProvider() );
		MessageBox(prompt);
	}

	*pResult = 0;
}

// ˢ�����������б�
void CLircCommandPage::OnRequestTccCmdList()
{
	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("��ѡ��վ!");
		return ;
	}
	m_bInitComplete = FALSE;

	// ��������б�
	for(int i=0; i<m_CmdList.GetItemCount(); i++)
	{
		TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(i);
		if(pcmd)
			delete pcmd;
	}
	m_CmdList.DeleteAllItems();

	// ����п�״̬
	m_StccStatusGrid.ClearStatus();

	// ���������б�
	GetTccAdviseSink()->OnRequestTccCmdList(nTccID, GetSafeHwnd());
}

// ȡ�����٣���ͨ������������顢ȷ�Ϻ�ֽ⵽������
void CLircCommandPage::OnCancelLimitSpeed()
{
	// Ȩ������
	if(!GetCancelRight())
		return;

	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		STOCK_COMMAND *pStockCmd = GetTccDlg()->GetConfirmCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		if(NULL == pStockCmd)
			pStockCmd = GetTccDlg()->GetLocalCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		GetTccAdviseSink()->OnCancelTccCommand(*pcmd, pStockCmd, GetSafeHwnd());
	}
}

bool CLircCommandPage::GetExecuteRight()
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(pInfoProvider==NULL)
		return false;

	if(pInfoProvider->IsCTC2GPC())
	{
		if(pInfoProvider->IsTCC2Execute())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CLircCommandPage::GetCancelRight()
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(pInfoProvider==NULL)
		return false;

	if(pInfoProvider->IsTCC2ReadOnly())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CLircCommandPage::OnCancelLimitSpeedDirect()
{
	// Ȩ������
	if(!GetExecuteRight())
		return;

	if(!gpcdef.IsCanCancelLimitDcmdDirect(3))
		return;
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

		TCC_COMMAND cmd = *pcmd;
		int builtin_id = pInfoProvider->GetLineID(cmd.nTccID, cmd.nTccLineID);
		if(cmd.nLineID != builtin_id)
		{
			MessageBox("���ɻ����ص���·������п���·�Ų�һ�£����п���·��Ϊ��!");
			cmd.nLineID = builtin_id;
		}
		cmd.bCancelLimit = TRUE;

		if(IDOK != MessageBox(cmd.toString(pInfoProvider), "ǿ����������ȡ��", MB_OKCANCEL))
			return;

		//std::vector<CString> pwds = m_pInfoProvider->GetPassword(ITccClientInfoProvider::DIRECT_CANCEL_LIRC_LIMITSPEED);
		CPasswordInputDlg passwdinput(this);
		if(IDOK == passwdinput.DoModal())
		{
			if(passwdinput.GetPassword() != "1234")
			{
				MessageBox("���벻��ȷ!");
				return;
			}

			CTccCommandSendDlg dlg(cmd, CTccCommandSendDlg::TCC_COMMAND_CANCEL, NULL,
				pInfoProvider, GetTccAdviseSink(), this);
			dlg.DoModal();

			//GetTccAdviseSink()->OnSendTccCommand(cmd, GetSafeHwnd());
		}
	}
}

void CLircCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return;

	CMenu menu;
	menu.CreatePopupMenu();
    
	bool bMenu = false;
	// Ȩ������
	if(GetCancelRight())
	{
		menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_CANCEL_LIMITSPEED, 
				"����[ȡ������]��������");
		bMenu = true;
	}

	// Ȩ������
	if(GetExecuteRight())
	{
		if(gpcdef.IsCanCancelLimitDcmdDirect(3))
		{
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_CANCEL_LIMITSPEED_DIRECT, 
				"ǿ��ȡ������");
			bMenu = true;
		}
	}

	if(!bMenu) return;

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

void CLircCommandPage::OnTccInitComplete()
{
	// Ȩ������
	if(!GetExecuteRight())
		return;

	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("��ѡ��վ!");
		return ;
	}

	if(m_bInitComplete)
	{
		if(IDOK != MessageBox("ע�⣺�п������Գ�ʼ��������У���ȷ���ٴγ�ʼ����?", "��ʼ�����", MB_OKCANCEL|MB_ICONSTOP))
			return;
	}

	if(IDOK != MessageBox("ע�⣺���п����Ĺ��ϻָ������������������ȷ��͸�վ�����������Ȼ���ٽ��б�\"��ʼ�����\"����", "��ʼ�����", MB_OKCANCEL|MB_ICONSTOP))
		return;

	CPasswordInputDlg passwdinput(this);
	if(IDOK == passwdinput.DoModal())
	{
		if(passwdinput.GetPassword() != "1234")
		{
			MessageBox("���벻��ȷ!");
			return;
		}
	}
	else
	{
		return;
	}

	TCC_COMMAND tcccmd;
	tcccmd.nConsoleID = GetTccClientInfoProvider()->GetConsoleID();
	tcccmd.nCmdID = 10000;
	SysidCreate(&tcccmd.sys_id);
	tcccmd.nTccID = nTccID;
	CTccCommandSendDlg dlg(tcccmd, CTccCommandSendDlg::TCC_COMMAND_INITCOMPLETE, NULL, GetTccClientInfoProvider(), GetTccAdviseSink());
	dlg.DoModal();
}

void CLircCommandPage::OnTccCommand_FullCancel()
{
	// Ȩ������
	if(!GetExecuteRight())
		return;

	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("��ѡ��վ!");
		return ;
	}

	if(IDOK != MessageBox("ע�⣺\"������\"������ȡ��ָ����վ��ָ����·�ϵ��������٣�������Ϊ������״̬��ȷ��������?", "������", MB_OKCANCEL|MB_ICONSTOP))
		return;

	CTccCommand_FullCancel dlg(nTccID, GetTccClientInfoProvider(), GetTccAdviseSink());
	dlg.DoModal();
}




CStccStatusGrid::CStccStatusGrid()
{
}

CStccStatusGrid::~CStccStatusGrid()
{
}

IMPLEMENT_DYNAMIC(CStccStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccStatusGrid, CGridCtrl)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CStccStatusGrid::Create(const RECT& rect, CWnd* parent, UINT nID, DWORD dwStyle)
{
	if(!CGridCtrl::Create(rect, parent, nID, dwStyle))
		return FALSE;


	CGridCtrl& grid = *this;

	CRect rcClient;
	grid.GetClientRect(&rcClient);

	try{
		//grid.SetRowCount(1);                //��
		grid.SetColumnCount(5);             //��
		//grid.SetFixedRowCount(1);        //�̶���
		grid.SetFixedColumnCount(1);     //�̶���		    
		grid.SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		grid.SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
	//    grid.SetFixedBkColor(RGB(255,242,240));
		grid.SetEditable(FALSE);
		grid.SetColumnWidth(0, 80);
		grid.SetColumnWidth(1, 120);
		grid.SetColumnWidth(2, 100);
		grid.SetColumnWidth(3, 120);
		grid.SetColumnWidth(4, 150);
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

	grid.SetSingleRowSelection();
    //ExpandColumnsToFit();

	return TRUE;
}
void CStccStatusGrid::ClearStatus()
{
	SetRowCount(0);
	KillTimer(8341);
}

void CLircCommandPage::Tcc_Report(DWORD nTccID, TCC_STATUS& status)
{
	if(nTccID != GetCurrentTccID())
		return ;

	m_StccStatusGrid.SetStatus(status);
	KillTimer(8341);
	if(status.init_status == 1 || status.init_status == 2)
	{
		m_bInitComplete = TRUE;

		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(!GetExecuteRight())
		{
			if(pWnd)
			{
				pWnd->EnableWindow(FALSE);
			}
		}
		else
		{
			// ���ܶ�γ�ʼ���п�����
			if(!gpcdef.IsReInitTCC(3))
			{
				pWnd->EnableWindow(FALSE);
			}
			else
			{
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		SetTimer(8341, 500, NULL);

		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(!GetExecuteRight())
		{
			if(pWnd)
			{
				pWnd->EnableWindow(FALSE);
			}
		}
		else
		{
			pWnd->EnableWindow(TRUE);
		}
	}
}

	COLORREF GREEN = RGB(0, 255, 0);
	COLORREF RED = RGB(255, 0, 0);
void GetConnectingUI(BYTE v, CString& str)
{
		switch(v)
		{
		case 0:
			str = "��";
			break;
		case 1:
			str = "ͨ";
			break;
		case 3:
			str = "��Ч";
			break;
		default:
			str.Format("(?%d)", v);
			break;
		}
}

void CStccStatusGrid::SetStatus(TCC_STATUS& status)
{
	//if(status.nStaNum)
	//	SetRowCount(18*status.nStaNum+1);
	//else
	//	SetRowCount(19);
	//for(int row=0; row<GetRowCount(); row++)
	//{
	//	for(int col=1; col<GetColumnCount(); col+=2)
	//		SetItemBkColour(row ,col , RGB(255,242,240));
	//}

	SetRowCount(0);
	SetBkColor(RGB(255,242,240));
	CString str, str2, str3;
	int row = 0;

	COLORREF clr = RED;

	str.Format("����վ%d", status.nStaID);
	//SetItemText(row, 0, str);
	row = InsertRow(str);

	SetItemText(row, 1, "�п�ͨ��");
	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
	switch(status.comm_active)
	{
	case 0:
	default:
		str = "��";	
		clr = RED;
		break;
	case 1:
		str = "����";
		clr = GREEN;
		break;
	case 2:
		str = "˫��";
		clr = GREEN;
		break;
	}
	SetItemFgColour(row, 2, clr);
	SetItemText(row, 2, str);

	SetItemText(row, 3, "��ʼ��״̬");
	// ��ʼ��״̬	(0=��;1=����;2=˫��)
	switch(status.init_status)
	{
	case 0:
	default:
		str = "δ��ʼ��";	
		clr = RED;
		break;
	case 1:
		str = "����ʼ��...";
		clr = GREEN;
		break;
	case 2:
		str = "�����";
		clr = GREEN;
		break;
	}
	SetItemFgColour(row, 4, clr);
	SetItemText(row, 4, str);
	
	for(int iSta=0; iSta<status.nStaNum||iSta<1; iSta++)
	{
		str.Format("վ%d", status.sta[iSta].nTccID);
		//SetItemText(row, 0, str);
		row = InsertRow(str);
		SetItemBkColour(row, 0, RGB(255, 0, 0));

		// �п�����״̬
		SetItemText(row, 1, "�п�״̬");
		switch(status.sta[iSta].standby_status)
		{
		case 2:
			switch(status.sta[iSta].master_status)
			{
			case 2:
				str = "A��,B��";
				clr = 0;
				break;
			case 1:
				str = "A��,B��";
				clr = GREEN;
				break;
			default:
				str.Format("˫��, δ֪%d", status.sta[iSta].master_status);
				clr = 0;
				break;
			}
			clr = GREEN;
			break;
		case 1:
			switch(status.sta[iSta].master_status)
			{
			case 2:
				str = "A��,B��";
				clr = 0;
				break;
			case 1:
				str = "A��,B��";
				clr = GREEN;
				break;
			default:
				str.Format("����, δ֪%d", status.sta[iSta].master_status);
				clr = 0;
				break;
			}
			clr = GREEN;
			break;
		default:
			str.Format("A��,B��(δ����%d)", status.sta[iSta].standby_status);
			clr = RED;
			break;
		}
		SetItemFgColour(row, 2, clr);
		SetItemText(row, 2, str);

		SetItemText(row, 3, "��CTC");
		// CTCͨ��״̬
		GetConnectingUI(status.sta[iSta].ctc_channel[0], str2);
		GetConnectingUI(status.sta[iSta].ctc_channel[1], str3);
		str.Format("A%s,B%s", str2, str3);
		SetItemText(row, 4, str);
		if(1 == status.sta[iSta].ctc_channel[0] || 1 == status.sta[iSta].ctc_channel[1])
			clr = GREEN;
		else
			clr = RED;
		SetItemFgColour(row, 4, clr);

		//row++;

		row = InsertRow("");
		// ����ͨ��״̬
		SetItemText(row, 1, "������");
		GetConnectingUI(status.sta[iSta].ilock_channel[0], str2);
		GetConnectingUI(status.sta[iSta].ilock_channel[1], str3);
		str.Format("A%s,B%s", str2, str3);
		SetItemText(row, 2, str);
		if(1 == status.sta[iSta].ilock_channel[0] || 1 == status.sta[iSta].ilock_channel[1])
			clr = GREEN;
		else
			clr = RED;
		SetItemFgColour(row, 2, clr);

		//SetItemText(row, 3, "�뽻����");
		//GetConnectingUI(status.sta[iSta].switch_channel[0], str2);
		//GetConnectingUI(status.sta[iSta].switch_channel[1], str3);
		//str.Format("A%s,B%s", str2, str3);
		//SetItemText(row, 4, str);
		//if(1 == status.sta[iSta].switch_channel[0] || 1 == status.sta[iSta].switch_channel[1])
		//	clr = GREEN;
		//else
		//	clr = RED;
		//SetItemFgColour(row, 4, clr);

		//row ++;

		for(int iLeu=0; iLeu<16; iLeu++)
		{
			//if(!status.sta[iSta].leu[iLeu].bValid)
			//	continue;
			str.Format("LEU%d", iLeu);
			//SetItemText(row, 0, str);
			row = InsertRow(str);
			
			//LEUͨ��״̬
			SetItemText(row, 1, "ͨ��״̬");
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_channel[0], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_channel[1], str3);
			str.Format("A%s,B%s", str2, str3);
			SetItemText(row, 2, str);
			if(1 == status.sta[iSta].leu[iLeu].leu_channel[0]
				|| 1 == status.sta[iSta].leu[iLeu].leu_channel[1])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 2, clr);

			// LEUӦ����״̬
			SetItemText(row, 3, "Ӧ����״̬");
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[0], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[1], str3);
			str.Format("0%s,1%s", str2, str3);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[2], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[3], str3);
			str += ",2" + str2 +",3" + str3;

			if(1 == status.sta[iSta].leu[iLeu].leu_ack[0]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[1]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[2]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[3])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 4, clr);
			SetItemText(row, 4, str);
			//row++;
		}

		for(int iZC=0; iZC < 8 && iZC < status.sta[iSta].nZCNum; iZC++)
		{
			str.Format("���м�%d", status.sta[iSta].ZC[iZC].nStaID);
			//SetItemText(row, 0, str);
			row = InsertRow(str);
			
			//ͨ��״̬
			SetItemText(row, 1, "ͨ��A");
			GetConnectingUI(status.sta[iSta].ZC[iZC].channel[0], str);
			SetItemText(row, 2, str);
			if(1 == status.sta[iSta].ZC[iZC].channel[0])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 2, clr);
			SetItemText(row, 3, "ͨ��B");
			GetConnectingUI(status.sta[iSta].ZC[iZC].channel[1], str);
			SetItemText(row, 4, str);
			if(1 == status.sta[iSta].ZC[iZC].channel[1])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 4, clr);
			//row++;
		}
	}	
	Refresh();
}


void CStccStatusGrid::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 8341)
	{
		static int nTimerCount = 0;
		nTimerCount ++;

		int row = 1, col = 3;

		if(nTimerCount % 2 == 0)
			SetItemBkColour(row, col, RGB(0,0,0));
		else
			SetItemBkColour(row, col, RGB(255,255,255));

		RedrawCell(row, col);
	}

	CGridCtrl::OnTimer(nIDEvent);
}


};

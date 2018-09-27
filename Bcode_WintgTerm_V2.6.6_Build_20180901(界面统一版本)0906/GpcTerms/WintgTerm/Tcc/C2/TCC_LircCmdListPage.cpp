/*
 * �п� ��ǰ���������б�
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"
#include <SuperPassDlg.h>
#include <DlgMessageBox.h>

namespace TCC {


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

	// ˢ����·�б�
	m_StccLineStatusGrid.SelectStation(nTccID);

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

	// ������
	lvi.iSubItem = COL_ABSLINEID;
	str = pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nLineID);
	if(str.IsEmpty())
		str = gpcdef.GetC3TccLineName(cmd.nLineID);
		
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ��ʼ�����
	lvi.iSubItem = COL_START_KM;
	if(cmd.bFullLimit)
		str.Format("ȫ����");
	else
		str = cmd.nKmMark[0].toString();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// ���������
	lvi.iSubItem = COL_END_KM;
	if(cmd.bFullLimit)
		str.Format("ȫ����");
	else
		str = cmd.nKmMark[1].toString();
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
		{"��ʼվ",      80, LVCFMT_LEFT },
		{"����վ",      80, LVCFMT_LEFT },
		{"������",		100, LVCFMT_LEFT },
		{"��ʼ�����",	120, LVCFMT_LEFT },
		{"���������",	120, LVCFMT_LEFT },
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

	GetDlgItem(IDC_TCC_LINE_STATUS_GRID)->GetWindowRect(&rcl);
	ScreenToClient(&rcl);
	m_StccLineStatusGrid.Create(rcl, this, 103);

	//OnRequestTccCmdList();	// ����״̬�б�
	// Ȩ�޿���
	if(!GetCancelRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_CANCELCMD_BTN);
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
	ON_COMMAND(ID_CANCEL_LIMITSPEED, OnCancelLimitSpeed)
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

	// �����·״̬
	m_StccLineStatusGrid.ClearStatus();

	// ���������б�
	GetTccAdviseSink()->OnRequestTccCmdList(nTccID, GetSafeHwnd());
}

bool CLircCommandPage::GetExecuteRight()
{
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	
	if(pIProvider->IsCTCGPC())
	{
		if(pIProvider->IsTCCExecute())
			return true;
	}
	return false;
}

bool CLircCommandPage::GetCancelRight()
{
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	
	if(!pIProvider->IsTCCReadOnly())
		return true;

	return false;
}

// ȡ�����٣���ͨ������������顢ȷ�Ϻ�ֽ⵽������
void CLircCommandPage::OnCancelLimitSpeed()
{
	// Ȩ�޿���
	if(!GetCancelRight())
		return;
	
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		STOCK_COMMAND *pStockCmd = GetTccDlg()->GetConfirmCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		//if(NULL == pStockCmd)
		//	pStockCmd = GetTccDlg()->GetLocalCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		GetTccAdviseSink()->OnCancelTccCommand(*pcmd, pStockCmd, GetSafeHwnd());
	}
}

void CLircCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Ȩ�޿���
	if(!GetCancelRight())
		return;

	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return;
    
	CMenu menu;
	menu.CreatePopupMenu();
	menu.InsertMenu(
		0, 
		MF_BYPOSITION|MF_STRING, 
		ID_CANCEL_LIMITSPEED, 
		"����ȡ�����ٵ�������");

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

CStccStatusGrid::CStccStatusGrid()
{
}

CStccStatusGrid::~CStccStatusGrid()
{
}

IMPLEMENT_DYNAMIC(CStccStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccStatusGrid, CGridCtrl)
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
		grid.SetColumnCount(10);             //��
		//grid.SetFixedRowCount(1);        //�̶���
	//	grid.SetFixedColumnCount(1);     //�̶���		    
		grid.SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		grid.SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
	//    grid.SetFixedBkColor(RGB(255,242,240));
		grid.SetEditable(FALSE);
		grid.SetColumnWidth(0, 100);
		grid.SetColumnWidth(1, 50);
		grid.SetColumnWidth(2, 100);
		grid.SetColumnWidth(3, 50);
		grid.SetColumnWidth(4, 100);
		grid.SetColumnWidth(5, 50);
		grid.SetColumnWidth(6, 100);
		grid.SetColumnWidth(7, 50);
		grid.SetColumnWidth(8, 100);
		grid.SetColumnWidth(9, 50);
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

	item.row = grid.InsertRow("�п�ͨ��״̬");
	item.row = grid.InsertRow("�п�A");
	item.row = grid.InsertRow("�п�B");;
	item.row = grid.InsertRow("����ͨ��A");
	item.row = grid.InsertRow("����ͨ��B");
	item.row = grid.InsertRow("CTCͨ��A");
	item.row = grid.InsertRow("CTCͨ��B");

	item.col = 2;
	
	for(; item.col < 10; )
	{
		item.row = 0;
		item.strText.Format("LEU%d״̬", item.col/2);
		grid.SetItem(&item);
		
		item.row++;
		for(; item.row < 5; item.row++)
		{
			item.strText.Format("LEU%dӦ����%d", item.col/2, item.row);
			grid.SetItem(&item);
		}

		item.col += 2;
	}

	for(int row=0; row<grid.GetRowCount(); row++)
	{
		for(int col=0; col<grid.GetColumnCount(); )
		{
			SetItemBkColour(row ,col , RGB(255,242,240));
			col+=2;
		}
	}
	
	grid.SetSingleRowSelection();
    ExpandColumnsToFit();

	return TRUE;
}
void CStccStatusGrid::ClearStatus()
{
	for(int row=0; row<GetRowCount(); row++)
	{
		for(int col=1; col<GetColumnCount();)
		{
			SetItemText(row, col, NULL);
			col += 2;
		}
	}

	for(int col=1; col<GetColumnCount();)
	{
		RedrawColumn(col);
		col += 2;
	}

}

void CLircCommandPage::Tcc_Report(DWORD nTccID, TCC_STATUS& status)
{
	if(nTccID != GetCurrentTccID())
		return ;
	m_StccStatusGrid.SetStatus(status);

	// ������·״̬
	lineid_t nTccLineID[8];
	dir_t    dir[8];
	for(int i=0; i<status.init_section_num; i++)
	{
		nTccLineID[i] = status.init_line_no[i];
		dir[i] = status.init_direction[i];
	}
	m_StccLineStatusGrid.SetStatus(GetCurrentTccID(), nTccLineID, dir, status.init_section_num);
}

void CStccStatusGrid::SetStatus(TCC_STATUS& status)
{
	CString str;
	int row = 0;

	COLORREF GREEN = RGB(0, 255, 0);
	COLORREF RED = RGB(255, 0, 0);

	COLORREF clr = RED;
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
	SetItemFgColour(row, 1, clr);
	SetItemText(row++, 1, str);

	// �п�����״̬
	for(int i=0; i<2; i++, row++)
	{
		switch(status.stcc_master[i])
		{
		case 0:
			str = "����";
			clr = 0;
			break;
		case 1:
			str = "����";
			clr = GREEN;
			break;
		default:
			str.Format("δ֪ %d", status.stcc_master[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);

	}

	// ����ͨ��״̬
	for(i=0; i<2; i++,row++)
	{
		switch(status.ilock_channel[i])
		{
		case 0:
			str = "��";
			clr = RED;
			break;
		case 1:
			str = "ͨ";
			clr = GREEN;
			break;
		default:
			str.Format("δ֪ %d", status.ilock_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);
	}

	// CTCͨ��״̬
	for(i=0; i<2; i++,row++)
	{
		switch(status.ctc_channel[i])
		{
		case 0:
			str = "��";
			clr = RED;
			break;
		case 1:
			str = "ͨ";
			clr = GREEN;
			break;
		default:
			str.Format("δ֪ %d", status.ctc_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);
	}

	row = 0;
	// LEUͨ��״̬��Ӧ����
	for(i=0; i<4; i++)
	{
//LEUͨ��״̬		
		switch(status.leu_channel[i])
		{
		case 0:
			str = "��";
			clr = RED;
			break;
		case 1:
			str = "ͨ";
			clr = GREEN;
			break;
		default:
			str.Format("δ֪ %d", status.leu_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, i*2+3, clr);
		SetItemText(row++, i*2+3, str);

// Ӧ����״̬
		for(int j=0; j<4; j++)
		{
			switch(status.leu_ack[i*4+j])
			{
			case 0:
				str = "��";
				clr = RED;
				break;
			case 1:
				str = "ͨ";
				clr = GREEN;
				break;
			default:
				str.Format("δ֪ %d", status.leu_ack[i*4+j]);
				clr = 0;
				break;
			}
			SetItemFgColour(row,  i*2+3, clr);
			SetItemText(row++,  i*2+3, str);
		}

		row = 0;
	}	


	Refresh();
}


CStccLineStatusGrid::CStccLineStatusGrid()
	: m_nCurrentTccID(0)
{
}

CStccLineStatusGrid::~CStccLineStatusGrid()
{
}

ITccClientInfoProvider* CStccLineStatusGrid::GetTccClientInfoProvider()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetTccClientInfoProvider();
}

ITccAdviseSink* CStccLineStatusGrid::GetTccAdviseSink()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetTccAdviseSink();
}
stano_t CStccLineStatusGrid::GetCurrentTccID()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetCurrentTccID();
}


IMPLEMENT_DYNAMIC(CStccLineStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccLineStatusGrid, CGridCtrl)
	//{{AFX_MSG_MAP(CShuntNoteGou)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_INIT_STCC_LINE, OnInitStccLine)
	ON_COMMAND(ID_INIT_STCC_LINE2, OnInitStccLine2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CStccLineStatusGrid::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	if(!CGridCtrl::Create(rect, pParentWnd, nID, dwStyle))
		return FALSE;

	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"��·", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"����", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"����վ", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"״̬", 120,   DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"����", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
	};

	CRect rcClient;
	GetClientRect(&rcClient);

	int nColNum = sizeof(colinfo)/sizeof(colinfo[0]);
	try{
		SetRowCount(1);                //��
		SetColumnCount(nColNum);             //��
		SetFixedRowCount(1);        //�̶���
		SetFixedColumnCount(3);     //�̶���		    
		SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
	    SetFixedBkColor(RGB(255,242,240));

		for(int i=0; i<GetColumnCount(); i++)
		{
			SetColumnWidth(i, colinfo[i].width);
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
	for(item.col = 0; item.col < GetColumnCount(); item.col++)
	{
		item.strText.Format(colinfo[item.col].label);
		item.nFormat = colinfo[item.col].fmt;
		SetItem(&item);
	}
	
	SetSingleRowSelection();
//    ExpandColumnsToFit();

	return TRUE;
}

void CStccLineStatusGrid::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Ȩ�޿���
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return;
	
	if(pIProvider->IsCTCGPC())
	{
		if(!pIProvider->IsTCCExecute())
			return;
	}
	else
	{
		return;
	}
			
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;

	int nInitComplete = (int)GetItemData(cellFocus.row, 3);
	
	CString str = GetItemText(cellFocus.row, 1);

	if(str == "��������")
	{
		CMenu menu;
		menu.CreatePopupMenu();

		if(nInitComplete == 2)
		{
			// ���ܶ�γ�ʼ���п�����
			if(!gpcdef.IsReInitTCC(2))
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE2, 
					"���߳�ʼ��");
			}
			else
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"���߳�ʼ��");
			}
		}
		else
		{
			menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"���߳�ʼ��");
		}

		menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			point.x, point.y, 
			this);
	}
	else
	{
		CMenu menu;
		menu.CreatePopupMenu();
		if(nInitComplete == 2)
		{
			// ���ܶ�γ�ʼ���п�����
			if(!gpcdef.IsReInitTCC(2))
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE, 
					"���ٳ�ʼ��");
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE2, 
					"ȫ�����ٳ�ʼ��");
			}
			else
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE, 
					"���ٳ�ʼ��");
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"ȫ�����ٳ�ʼ��");
			}
		}
		else
		{
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_INIT_STCC_LINE, 
				"���ٳ�ʼ��");
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_INIT_STCC_LINE2, 
				"ȫ�����ٳ�ʼ��");
		}
		

		menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			point.x, point.y, 
			this);
	}
}


void CStccLineStatusGrid::OnInitStccLine()
{
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;
	INT row = cellFocus.row;

	if(0 == GetItemData(row, 3))
	{
		MessageBox("���ܳ�ʼ��!");
		return ;
	}

	extern std::vector<CString> AvailablePWDList;
	CSuperPassDlg  dlg, dlg1;
	dlg.m_strPrompt = "�������һ������";
	dlg1.m_strPrompt = "������ڶ�������";

	if(AvailablePWDList.size()==0)
	{
		stano_t nTccID = GetCurrentTccID();
		lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);
		dir_t dir = (dir_t)GetItemData(row, 1);

		GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, dir, GetSafeHwnd());
	}
	else
	{
		if (dlg.DoModal() == IDOK)
		{

			CString str = AvailablePWDList[0];
							
			if(dlg.m_super_pass.Compare(str) != 0)
			{
				CString str;
				str.Format("%s","�������!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return;
			}
			else
			{
				if (dlg1.DoModal() == IDOK)
				{
					CString str = AvailablePWDList[1];
							
					if(dlg1.m_super_pass.Compare(str) != 0)
					{
						CString str;
						str.Format("%s","�������!");
						CDlgMessageBox dlg(str);
						dlg.DoModal();
						return;
					}
				}
				else
					return;
			}
			
			stano_t nTccID = GetCurrentTccID();
			lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);
			dir_t dir = (dir_t)GetItemData(row, 1);

			GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, dir, GetSafeHwnd());

		}
	}

}

void CStccLineStatusGrid::OnInitStccLine2()
{
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;
	INT row = cellFocus.row;
	
	if(0 == GetItemData(row, 3))
	{
		MessageBox("����ȫ�߳�ʼ��!");
		return ;
	}

	extern std::vector<CString> AvailablePWDList;
	CSuperPassDlg  dlg, dlg1;
	dlg.m_strPrompt = "�������һ������";
	dlg1.m_strPrompt = "������ڶ�������";

	if(AvailablePWDList.size()==0)
	{
		stano_t nTccID = GetCurrentTccID();
		lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);

		GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, DIR_DUAL, GetSafeHwnd());
	}

	else
	{
		if (dlg.DoModal() == IDOK)
		{

			CString str = AvailablePWDList[0];
							
			if(dlg.m_super_pass.Compare(str) != 0)
			{
				CString str;
				str.Format("%s","�������!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return;
			}
			else
			{
				if (dlg1.DoModal() == IDOK)
				{
					CString str = AvailablePWDList[1];
							
					if(dlg1.m_super_pass.Compare(str) != 0)
					{
						CString str;
						str.Format("%s","�������!");
						CDlgMessageBox dlg(str);
						dlg.DoModal();
						return;
					}
				}
				else
					return;
			}
			stano_t nTccID = GetCurrentTccID();
			lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);

			GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, DIR_DUAL, GetSafeHwnd());
		}
	}
}

void CStccLineStatusGrid::SelectStation(stano_t nStaID)
{
	DeleteNonFixedRows();

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	std::vector<lineid_t> lines;
	pInfoProvider->GetTccLineList(nStaID, lines);

	stano_t nLinkStaID = 0;
	for(int i=0; i<lines.size(); i++)
	{
		lineid_t nTccLineID = lines[i];

		TCC::dir_t dir = pInfoProvider->GetTccLineAbsNum(nStaID, nTccLineID);
		
		if(TCC::DIR_UP == dir || TCC::DIR_DUAL == dir)
		{
			item.col = 0;
			item.row = InsertRow(pInfoProvider->GetTccLineName(nStaID, nTccLineID));
			SetItemData(item.row, item.col, nTccLineID);

			item.col ++;
			item.strText = "���з���";
			item.lParam = DIR_UP;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_UP);
			if(nLinkStaID)
				item.strText = pInfoProvider->GetStationName(nLinkStaID);
			else
				item.strText.Empty();
			item.col ++;
			item.lParam = nLinkStaID;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			item.col ++;
			item.strText = "δ֪";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 0;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);
			SetItemState(item.row, item.col, GVIS_READONLY);

			item.col ++;
			SetItemState(item.row, item.col, GVIS_READONLY);
		}

		if(TCC::DIR_DOWN == dir || TCC::DIR_DUAL == dir)
		{
			item.col = 0;
			item.row = InsertRow(pInfoProvider->GetTccLineName(nStaID, nTccLineID));
			SetItemData(item.row, item.col, nTccLineID);

			item.col ++;
			item.strText = "���з���";
			item.lParam = DIR_DOWN;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_DOWN);
			if(nLinkStaID)
				item.strText = pInfoProvider->GetStationName(nLinkStaID);
			else
				item.strText.Empty();
			item.col ++;
			item.lParam = nLinkStaID;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			item.col ++;
			item.strText = "δ֪";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 0;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);
			SetItemState(item.row, item.col, GVIS_READONLY);

			item.col ++;
			SetItemState(item.row, item.col, GVIS_READONLY);
		}
	}

	lines.clear();
	pInfoProvider->GetTccBlockLineList(nStaID, lines);

	for(int i=0; i<lines.size(); i++)
	{
		lineid_t nTccLineID = lines[i];
		
		item.col = 0;
		item.row = InsertRow(pInfoProvider->GetTccBlockLineName(nStaID, nTccLineID));
		SetItemData(item.row, item.col, nTccLineID);

		item.col ++;
		item.strText = "��������";
		item.lParam = DIR_UP;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);

		item.col ++;
		item.strText.Empty();
		item.lParam = 0;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);

		item.col ++;
		item.strText = "δ֪";
		SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
		item.lParam = 0;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);
		SetItemState(item.row, item.col, GVIS_READONLY);

		item.col ++;
		SetItemState(item.row, item.col, GVIS_READONLY);

	}
	Refresh();
}

void CStccLineStatusGrid::ClearStatus()
{
	for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
	{
		SetItemText(row, 3, "δ֪");
		SetItemData(row, 3, 0);
		SetItemBkColour(row, 3, RGB(255,255,0));
		SetItemText(row, 4, NULL);
	}
	RedrawColumn(3);
	RedrawColumn(4);
}

void CStccLineStatusGrid::SetStatus(stano_t nStaID, lineid_t nTccLineID[], dir_t dir[], INT num)
{
	if(nStaID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.col = 3;
	for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
	{
		item.row = row;
		item.strText = "����";
		SetItemBkColour(item.row, item.col, RGB(0, 255, 0));
		item.lParam = 2;
		SetItem(&item);
	}

	for(int i=0; i<num; i++)
	{
		stano_t nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID[i], dir[i]);
		for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
		{
			item.row = row;
			if(GetItemData(row, 0) != nTccLineID[i]
				|| GetItemData(row, 1) != dir[i]
				|| GetItemData(row, 2) != nLinkStaID)
			{
				continue;
			}
			item.strText = "����δ��ʼ��";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 1;
			SetItem(&item);
		}
	}
	RedrawColumn(item.col);
}

void CLircCommandPage::SetTccLineInitialCommandOperResult(
		stano_t nTccID, 
		lineid_t nTccLineID, 
		dir_t dir,
		DWORD nStatus,
		CString strComment)
{
	if(nTccID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	//��������
	BOOL bSidings = pInfoProvider->IsTccBlockLine(nTccID, nTccLineID);
	if(bSidings)
	{
		GV_ITEM item;
		item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

		for(int row=m_StccLineStatusGrid.GetFixedRowCount(); row<m_StccLineStatusGrid.GetRowCount(); row++)
		{
			if(m_StccLineStatusGrid.GetItemData(row, 0)== nTccLineID)
			{
				item.row = row;
				item.col = 4;
				item.strText = strComment;
				item.lParam = 0;
				m_StccLineStatusGrid.SetItem(&item);
				
				m_StccLineStatusGrid.RedrawCell(row, item.col);
			}
		}
	}
	else
	//
	{
		if(dir == DIR_DUAL)
		{
			SetTccLineInitialCommandOperResult(nTccID, nTccLineID, DIR_UP, nStatus, strComment);
			SetTccLineInitialCommandOperResult(nTccID, nTccLineID, DIR_DOWN, nStatus, strComment);
		}
		else
		{
			stano_t nLinkStaID = pInfoProvider->GetLinkedStation(nTccID, nTccLineID, dir);

			GV_ITEM item;
			item.mask = GVIF_TEXT|GVIF_FORMAT;
			item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

			for(int row=m_StccLineStatusGrid.GetFixedRowCount(); row<m_StccLineStatusGrid.GetRowCount(); row++)
			{
				if(m_StccLineStatusGrid.GetItemData(row, 0) != nTccLineID
					|| (m_StccLineStatusGrid.GetItemData(row, 1) != dir)
					|| m_StccLineStatusGrid.GetItemData(row, 2) != nLinkStaID)
					continue;

				item.row = row;
				item.col = 4;
				item.strText = strComment;
				item.lParam = 0;
				m_StccLineStatusGrid.SetItem(&item);
				
				m_StccLineStatusGrid.RedrawCell(row, item.col);
			}
		}
	}

}


};

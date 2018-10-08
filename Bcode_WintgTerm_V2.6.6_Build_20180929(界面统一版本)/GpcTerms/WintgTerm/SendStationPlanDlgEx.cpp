// SendStationPlanDlgEx.cpp : implementation file
//

#include "stdafx.h"
#include "SendStationPlanDlgEx.h"
#include "GridCtrl_src/GridCellCheck.h"

#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include ".\sendstationplandlgex.h"
#include "tg_function_config.h"
#include "DlgMarkResult.h"
#include "station_side_full_check.h"

#define WHOLE_DAY_SECOND	86400
#define WHOLE_DAY_MINUTE	1440

extern int WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
					ushort count,char *buffer);
extern bool IsTDCommSvr(long nEntityId);
// ��ȡ��վ����ģʽ,�·��׶μƻ����ʹ��
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlgExEx dialog
#define ITEMNUM 10

// ��Ż���ͼ��Ϣ�ṹ
struct station_info
{
	TRAIN_NO ar_trainid;
	TRAIN_NO de_trainid;
	int  flag;
	int  ar_side;
	int  de_side;
	int  prev_station;
	int  next_station;
};

struct node_basic_info
{
	int station;
	int count;
	station_info info[2048];
};

int m_st_basic_info_count=0;
node_basic_info st_basic_info[MAX_STATION_PER_SECTION];

CSendStationPlanDlgEx::CSendStationPlanDlgEx(CTgDocument *pDoc, CWnd* pParent/*=NULL*/):
CPropertyPage(CSendStationPlanDlgEx::IDD)
{
	//{{AFX_DATA_INIT(CSendStationPlanDlgEx)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nRecNum=0;
	m_nCols = ITEMNUM;
	nUnitNum =0;
	m_pGridCtrl = NULL;
	bAllStationFlag = TRUE;
	m_pStplanGrid=NULL;
	m_pPlanReceiveInfo=m_pDoc->GetSTPLAN()->GetPlanReceiveInfo();  
	m_pPredictServer = m_pDoc->GetTrainDepartPredictHold();
}

CSendStationPlanDlgEx::~CSendStationPlanDlgEx()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
}
void CSendStationPlanDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendStationPlanDlgEx)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK2, m_disableAutoChangeTrainNO);
}


BEGIN_MESSAGE_MAP(CSendStationPlanDlgEx, CPropertyPage)
	//{{AFX_MSG_MAP(CSendStationPlanDlgEx)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_GRID, OnGrid)
	ON_MESSAGE(UM_STATION_RECV_STATUS_MSG,ChangeStationReceiveStatus)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK, OnCheckAll)
	ON_BN_CLICKED(IDC_CHECK_STPC, OnCheckStpc)
	ON_BN_CLICKED(IDC_CHECK_LIRC, OnCheckLirc)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_SHOW_DETAIL, OnBnClickedShowDetail)
	ON_CBN_SELCHANGE(IDC_CBO_TIME_RANGE, OnCbnSelchangeCboTimeRange)
	ON_CBN_EDITCHANGE(IDC_CBO_TIME_RANGE, OnCbnEditchangeCboTimeRange)
	ON_BN_CLICKED(IDC_CLEAR_ALARM, OnBnClickedClearAlarm)
	ON_BN_CLICKED(IDC_CHECK_CTC_ONLY, OnBnClickedCheckCtcOnly)
	ON_BN_CLICKED(IDC_CHECK_AUTOCLOSE, OnBnClickedCheckAutoclose)
	ON_CBN_SELCHANGE(IDC_CBO_GROUP, OnCbnSelchangeCboGroup)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlgEx message handlers
///// 20080506 CUIHU //////////////
void CSendStationPlanDlgEx::SetSendPlanTimeRange()
{
	CString strTime;
	CEdit *pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_START_TIME);
	strTime = OsConvertTimetoString(m_pDoc->GetCurrentTGTime()/60*60 );   
	pEdit->SetWindowText(strTime);

	if(m_pDoc->GetCurrentTGTime() + m_pDoc->m_tmPlanSendRange*3600 > WHOLE_DAY_SECOND)
		strTime = OsConvertTimetoString(m_pDoc->GetCurrentTGTime()/60*60 + m_pDoc->m_tmPlanSendRange*3600 - WHOLE_DAY_SECOND); 
	else
		strTime = OsConvertTimetoString(m_pDoc->GetCurrentTGTime()/60*60 + m_pDoc->m_tmPlanSendRange*3600); 
	pEdit = (CEdit*)GetDlgItem(IDC_END_TIME);
	pEdit->SetWindowText(strTime);

}
/////////////////////////////////////////////////
BOOL CSendStationPlanDlgEx::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_AUTOCLOSE);
	if (GetTGFunctionConfig()->IsDefaultSendPlanClose())
		pButton->SetCheck(true);
	else
		pButton->SetCheck(false);

	m_pDoc->m_tmPlanSendRange = GetTGFunctionConfig()->GetDefaultSendPlanTRange();
	
	nCurentId = ::GetTickCount();
	//////////// 20080506 CUIHU ///////////////
	/// ���÷��ͼƻ�ʱ�䷶Χ������������ȡ�����δ���
	SetSendPlanTimeRange();
	///////////////////////////////////////////
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return FALSE;

		// Create the Gridctrl window
		CStatic *pWnd;
		pWnd = (CStatic *)GetDlgItem(IDC_GRID);
		CRect rect;
		pWnd->GetWindowRect(rect);
		ScreenToClient(&rect);
		m_pGridCtrl->Create(rect, this, IDC_GRID);

		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);
		m_pGridCtrl->EnableDragAndDrop(FALSE);
		
		m_nRecNum=gpcdef.GetSpecialGPCList(m_pDoc->m_nCurrentSectionNo, m_nRecList);
		if (m_nRecNum==0)
		{
			m_nRecNum = gpcdef.GetSectionPlanReceivers(m_pDoc->m_nCurrentSectionNo,m_nRecList);
			GetDlgItem(IDC_GROUP_TEXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CBO_GROUP)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_GROUP_TEXT)->ShowWindow(SW_SHOW);
			CComboBox *group_box = (CComboBox*)GetDlgItem(IDC_CBO_GROUP);
			group_box->ShowWindow(SW_SHOW);
			
			int nGroupList[32];
			int nGroupNum = gpcdef.GetSpecialGPCGroupList(m_pDoc->m_nCurrentSectionNo,nGroupList);
			
			for (int i=0; i<nGroupNum; i++)
			{
				CString tmp;
				tmp = gpcdef.GetSpecialGroupNameByIndex(nGroupList[i]);
				group_box->InsertString(i,tmp);
				group_box->SetItemData(i,nGroupList[i]);
			}
		}

		try 
		{
			m_pGridCtrl->SetRowCount(m_nRecNum +1);
			m_pGridCtrl->SetColumnCount(m_nCols);
			m_pGridCtrl->SetFixedRowCount(m_nFixRows);
			m_pGridCtrl->SetFixedColumnCount(m_nFixCols);
			m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));   //����ɫ
			m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
			m_pGridCtrl->SetFixedBkColor(RGB(255,242,240));
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return FALSE;
		}
	}	

	TCHAR rgtsz[ITEMNUM][20] = {_T("���"),_T("��վ"),_T("����ʵ��"),_T("�ϴ��´�"),
		_T("��ʼʱ��"),_T("����ʱ��"), _T("�Ƿ����"),_T("�Ƿ�ǩ��"),_T("ǩ����"),
		_T("ǩ��ʱ��")};

	CRect rect;

	DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;

	int row,col;
	GPCPLANRECEIVER rec;

	GPCENTITY entityInfo; DWORD nEntityId;
	CTime t; time_t uTime_t; CString sContent;
	for ( row = 0; row <= m_nRecNum; row++)
	{
		for ( col = 0; col < m_nCols; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;

			if (row == 0)
			{
				Item.nFormat = DT_CENTER|DT_WORDBREAK|DT_NOPREFIX;
				Item.strText.Format(rgtsz[col]);
				m_pGridCtrl->SetItemBkColour(row,col,CLR_GRAY);

			}
			else
			{            
				switch(col)
				{
				case 0: 		 
					Item.strText.Format(_T("%d"),row);		
					Item.nFormat = dwTextStyle;
					break;	
				case 1:
					Item.nFormat = dwTextStyle;				
					nEntityId = m_pPlanReceiveInfo->m_StationPlan[row-1].Entityid;
					gpcdef.GetEntityInfo(nEntityId,entityInfo);
					if(IsTDCommSvr(nEntityId))
					{
						int nStationNo=m_pPlanReceiveInfo->m_StationPlan[row-1].nStation;
						CString str=gpcdef.GetStationName(nStationNo);
						Item.strText.Format(_T("%s"), str);
					}
					else
					{
						Item.strText.Format(_T("%s"), entityInfo.sName);
					}
					break;
				case 2:
					Item.nFormat = dwTextStyle;		
					gpcdef.GetReceiverInfo(m_nRecList[row-1],rec);
					Item.strText.Format(_T("%s"),
						m_pPlanReceiveInfo->m_StationPlan[row-1].EntityName);
					m_pGridCtrl->SetCellType(row,col, RUNTIME_CLASS(CGridCellCheck));		
					m_pGridCtrl->SetItemFgColour (row,col,CLR_BLUE);	
					break;
				case 3:
					uTime_t = m_pPlanReceiveInfo->m_StationPlan[row-1].send_time ;

					if(uTime_t>0) 
					{ 
						t = CTime(uTime_t);
						if(t.GetMinute()>9)
							sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
						else
							sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
					}
					else
						sContent.Empty();
					Item.nFormat = dwTextStyle;
					Item.strText = sContent;
					break;
				case 4:						 
					uTime_t = m_pPlanReceiveInfo->m_StationPlan[row-1].begin_time; 
					if(uTime_t>0) 
					{
						t = CTime(uTime_t);
						if(t.GetMinute()>9)
							sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
						else
							sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
					}
					else
						sContent.Empty();
					Item.nFormat = dwTextStyle;
					Item.strText = sContent;
					break;
				case 5:					
					uTime_t = m_pPlanReceiveInfo->m_StationPlan[row-1].end_time; 
					if(uTime_t>0) 
					{ t = CTime(uTime_t);
					if(t.GetMinute()>9)
						sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
					else
						sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
					}
					else
						sContent.Empty();
					Item.nFormat = dwTextStyle;
					Item.strText = sContent;
					break;
				case 6:	
					Item.nFormat = dwTextStyle;
					if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_SIGN)
					{
						Item.strText = "��";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_ENTITY_DEAD)
					{
						Item.strText = "�Ͽ�"; ////վ��
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
					else  if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_RECV)
					{
						Item.strText = "��";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else  if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_NO_DATA)
					{
						Item.strText = "";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else  if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_SENDING)
					{
						Item.strText = "������...";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else  
					{
						Item.strText = "";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
					break;		 
				case 7:				
					Item.nFormat = dwTextStyle;
					if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag
						== CLBTH::MsgStPlanStatusAnswer::PLAN_SIGN)
					{
						Item.strText = "��";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else if(m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_SENDING 
						|| m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_ENTITY_DEAD
						||m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_NO_CHANGE
						||m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_NO_DATA
						||m_pPlanReceiveInfo->m_StationPlan[row-1].bReceiveFlag 
						== CLBTH::MsgStPlanStatusAnswer::PLAN_RECV)
					{
						Item.strText = "��";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
					else
					{
						Item.strText = "";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}

					break;
				case 8:

					Item.nFormat = dwTextStyle;
					Item.strText = m_pPlanReceiveInfo->m_StationPlan[row-1].strSignName;
				default:
					Item.nFormat = dwTextStyle;
					break;
				}			
			}
			m_pGridCtrl->SetItem(&Item);

		}
	}
	m_pGridCtrl->AutoSizeColumns();
	//set checked
	CGridCellCheck * pCell;
	for ( row = 1; row <= m_nRecNum; row++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		if(pCell)
			pCell->SetCheck(TRUE);
	}

	QueryStStatus();
	if((SetTimer(1976,5000,NULL)) == 0)
		TRACE0("create send station plan timer failed");

	CButton *pWnd;   
	pWnd = (CButton *)GetDlgItem(IDC_CHECK);
	bool bFlag = TRUE;

	CButton *pWnd1,*pWnd2;
	pWnd1 = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_LIRC);

	pWnd->SetCheck(bFlag);
	pWnd1->SetCheck(bFlag);
	pWnd2->SetCheck(bFlag);

	///////////// 20080506 CUIHU ����ʱ�䷶Χ��ѡ��///////////////
	struct struTmRange{
		int  tm;
		char str[8];
	};
	struct struTmRange tm_range[]={
		{1,"1"},
		{2,"2"},
		{3,"3"},
		{4,"4"},
		{5,"5"}
	};

	int i,cur_sel=-1;
	for(i=0;i<sizeof(tm_range)/sizeof(struct struTmRange);i++)
	{
		int item=((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->AddString(tm_range[i].str);
		((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->SetItemData(item,tm_range[i].tm);
		if(m_pDoc->m_tmPlanSendRange == tm_range[i].tm)
			cur_sel=item;
	}
	((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->SetCurSel(cur_sel);

	if(!GetTGFunctionConfig()->IsCheckPlanValid(m_pDoc->m_nCurrentSectionNo))
	{ 
		CWnd* pWnd = GetDlgItem(ID_SHOW_DETAIL);
		pWnd->EnableWindow(0);

		pWnd = GetDlgItem(IDC_CLEAR_ALARM);
		pWnd->EnableWindow(0);
	}

	// �Ƿ����ѡ���յ��г����Զ����Ϊ��һ��ʼ������
	if(GetTGFunctionConfig()->IsTrainNoAutoChgEnable())
		m_disableAutoChangeTrainNO.EnableWindow(TRUE);
	else
		m_disableAutoChangeTrainNO.EnableWindow(FALSE);
	
	////////////////////////////////////////////
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// ѡ�����н׶μƻ�����ʵ��
void CSendStationPlanDlgEx::SelAllStplanRecEntity()
{
	CButton *pWnd1 = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	CButton *pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	CButton *pWnd3 = (CButton *)GetDlgItem(IDC_CHECK);
	if(NULL != pWnd1)
		pWnd1->SetCheck(TRUE);
	if(NULL != pWnd2)
		pWnd2->SetCheck(TRUE);
	if(NULL != pWnd3)
		pWnd3->SetCheck(TRUE);

	CGridCellCheck *pCell;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		pCell->SetCheck(TRUE);
	}
}

// ��ѡ��LIRC
void CSendStationPlanDlgEx::SendStplanOnlySelLIRC()
{
	CButton *pWnd1 = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	CButton *pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	CButton *pWnd3 = (CButton *)GetDlgItem(IDC_CHECK);
	if(NULL != pWnd1)
		pWnd1->SetCheck(FALSE);
	if(NULL != pWnd2)
		pWnd2->SetCheck(TRUE);
	if(NULL != pWnd3)
		pWnd3->SetCheck(FALSE);

	CGridCellCheck *pCell;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		CString strText = m_pGridCtrl->GetItemText(row,2);  
		if(strText =="��վ�ն�")
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
			pCell->SetCheck(FALSE);
		}
	}
}
// G27 �·��׶μƻ� ֪ͨ��������
CString CSendStationPlanDlgEx::GetPlanDCmdId()
{
	CString tmp;
	tmp.Format("%d|%d", nCurentId, GetLocalEntityID());
	return tmp;
}

void CSendStationPlanDlgEx::SendNoticeToDCmd(int nFlag)
{
	if(!GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
		return;


	CTime bt = CTime::GetCurrentTime();
	CTime et = bt + CTimeSpan(0,m_pDoc->m_tmPlanSendRange,0,0);

	CString xml,tmp;
	xml = "<GEN_SECTIONPLAN_DCMD>";
	tmp.Format("<Id>%s</Id>", GetPlanDCmdId());
	xml += tmp;
	tmp.Format("<Flag>%d</Flag>", nFlag);
	xml += tmp;
	GPCENTITY entityInfo;
	gpcdef.GetEntityInfo(GetLocalEntityID(), entityInfo);

	tmp.Format("<Console_Name>%s</Console_Name>", entityInfo.sName);
	xml += tmp;
	tmp = "<Start_Time>";
	tmp += bt.Format("%Y-%m-%d %H:%M:%S");
	tmp +="</Start_Time>";
	xml += tmp;
	tmp = "<End_Time>";
	tmp += et.Format("%Y-%m-%d %H:%M:%S");
	tmp +="</End_Time>";
	xml += tmp;
	xml += "</GEN_SECTIONPLAN_DCMD>";

	long eid=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
	if(eid<0)
	{
		MessageBox("�޷����Ҷ�Ӧ�ĵ�������ʵ��,����֪ͨ���������·���������","��ʾ");
		return;
	}

	CString token,content;
	GenXMLMessage_InformDCMDToFront(token,content);
	TGSendStringMessage(token,content,eid);

	token="GEN_SECTIONPLAN_DCMD";
	TGSendStringMessage(token,xml,eid);
	sysprint(5,0,"����������ն� %d ���ͽ׶μƻ�����֪ͨXML:\n", eid);
	sysprint(5,0,"���ͽ׶μƻ�����֪ͨXML%s\n", xml.Left(50));
	sysprint(5,0,"���ͽ׶μƻ�����֪ͨXML%s\n", xml.Right(xml.GetLength()-50));
}

void CSendStationPlanDlgEx::OnOK() 
{
	SetSendPlanTimeRange();
	UpdateData(TRUE);

	int  nSelectedNum = 0;
	int  nSelectedList[MAX_STATION_PER_SECTION];

	CString strTime;
	CEdit *pEdit;
	long startTime,endTime;
	long startDate,endDate;

	pEdit = (CEdit*)GetDlgItem(IDC_START_TIME);
	pEdit->GetWindowText(strTime);
	startTime =  OsConvertStringtoTime((char *)(LPCTSTR)strTime);
	startDate = m_pDoc->GetCurrentTGDate();//
	pEdit = (CEdit*)GetDlgItem(IDC_END_TIME);
	pEdit->GetWindowText(strTime);
	endTime =  OsConvertStringtoTime((char *)(LPCTSTR)strTime);
	if(startTime > endTime)
		endDate = OsGetNextDate(PMTG.ThreadId,m_pDoc->GetCurrentTGDate());//
	else
		endDate = m_pDoc->GetCurrentTGDate();//


	if(long_time(startDate,startTime) <= 0 || long_time(endDate,endTime) <= 0){
		AfxMessageBox("�´�ʱ�䷶Χ����,����������ʱ��!");
		return;
	}

	if(long_time(endDate,endTime) - long_time(startDate,startTime) >5*60*60){
		AfxMessageBox("�´�ʱ�䷶Χ���ܴ�����Сʱ,����������!");
		return;	
	}
	CString str;
	BOOL m_bChecked;
	CGridCellCheck *pCell;
	int nSectionNo = m_pDoc->m_nCurrentSectionNo;
	int nSelEntity = 0;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		m_bChecked = pCell->GetCheck ();
		if(m_bChecked)
		{
			nSelEntity++;
		}
	}
	if(nSelEntity<=0)
	{
		AfxMessageBox("����ѡ���·�ʵ�����!");
		return;
	}

	//������ͼ��С��10�룬��ô��������
	static unsigned long pre_tickcount = 0;
	unsigned long cur_tickcount = GetTickCount();
	if(cur_tickcount - pre_tickcount<10*1000 )
	{
		AfxMessageBox("���η��ͽ׶μƻ�ʱ�䲻��С��10��");
		return;
	}

	CString lirc_sta="", stpc_sta="", zhandiao_sta="", pessanger_sta="", other_sta="";
	int passernger_sta_count=0;
	for(row = 1; row <= m_pPlanReceiveInfo->nEntityNum; row++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		m_bChecked = pCell->GetCheck();
		if(m_bChecked)
		{
			//ʵ��index
			nSelectedList[nSelectedNum]= m_pPlanReceiveInfo->m_StationPlan[row-1].nEntityIndex;
			nSelectedNum++;

			if (m_pGridCtrl->GetItemText(row,2) == "�ͷ�")
				passernger_sta_count++;
		}
		else
		{
			if (m_pGridCtrl->GetItemText(row,2) == "���ɻ�")
				lirc_sta += " " + m_pGridCtrl->GetItemText(row,1);
			else if (m_pGridCtrl->GetItemText(row,2) == "��վ�ն�")
				stpc_sta += " " + m_pGridCtrl->GetItemText(row,1);
			else if (m_pGridCtrl->GetItemText(row,2) == "վ��")
				zhandiao_sta += " " + m_pGridCtrl->GetItemText(row,1);
			else if (m_pGridCtrl->GetItemText(row,2) == "�ͷ�")
				pessanger_sta += " " + m_pGridCtrl->GetItemText(row,1);
			else 
				other_sta ="δ֪�ն�";
		}
	}
	CString text_Alarm="",tmp;
	if (lirc_sta!="")
	{
		tmp.Format("\n���ɻ�:%s",lirc_sta);
		text_Alarm += tmp;
	}

	if(stpc_sta!="")
	{
		tmp.Format("\n�����ն�:%s",stpc_sta);
		text_Alarm += tmp;
	}

	if (zhandiao_sta!="")
	{
		tmp.Format("\nվ��:%s",zhandiao_sta);
		text_Alarm += tmp;
	}

	if (pessanger_sta!="")
	{
		tmp.Format("\n�ͷ�:%s",pessanger_sta);
		text_Alarm += tmp;
	}

	if (other_sta!="")
	{
		tmp.Format("\n����:%s",other_sta);
		text_Alarm += tmp;
	}

	if (text_Alarm!="")
	{
		tmp="[Error]����δ��ѡ�ƻ����յ�λ:";
		tmp+=text_Alarm;
		GpcLogTrainMessage(0,0,tmp);
	
		if (GetTGFunctionConfig()->IsQuestionStplanSelState())
		{
			tmp = "��ע��,����δ��ѡ�ƻ����յ�λ:" + text_Alarm + "\nȷ�ϲ�����Щ��λ�·��ƻ���?";
			if (AfxMessageBox(tmp,MB_YESNO) == IDYES)
			{
				GpcLogTrainMessage(0,0,"����Աȷ����Щ��λ���·��׶μƻ�");
			}
			else
			{
				GpcLogTrainMessage(0,0,"����Ա�������¹�ѡ�·��׶μƻ���λ");
				return;
			}
		}
	}

	m_pGridCtrl->UpdateData();
	Invalidate(FALSE);


	CLBTH::MsgStPlanSendOrder mMsgStPlanSendOrder;
	mMsgStPlanSendOrder.con_id = m_pDoc->m_nCurrentSectionNo;  //���� 1 
	CTime t = CTime::GetCurrentTime();
	mMsgStPlanSendOrder.send_time = (time_t)t.GetTime(); //2

	start_time = long_time(startDate,startTime);//3
	end_time = long_time(endDate,endTime); //4

	mMsgStPlanSendOrder.begin_time = start_time;
	mMsgStPlanSendOrder.end_time = end_time;

	char* pTmp = NULL;
	if(GetTGFunctionConfig()->IsSendStplanUseName())
		pTmp = gpcdef.GetCurrentUsername();
	else
		pTmp = gpcdef.GetCurrentUserid();
	CString ddyName("����Ա");
	if(pTmp)
	   ddyName = pTmp;

	memset(mMsgStPlanSendOrder.sender_name, 0, sizeof(USERNAME));
	strncpy(mMsgStPlanSendOrder.sender_name, ddyName, sizeof(USERNAME)-1);  //5
	mMsgStPlanSendOrder.sender_name[sizeof(USERNAME)-1]=0;
	GpcLogTrainMessage(0, 0, "���ͽ׶μƻ�����Ա %s", mMsgStPlanSendOrder.sender_name);

	int nChkRs = 0;
	if(GetTGFunctionConfig()->IsCheckPlanValid(nSectionNo))
	{
		nChkRs = CheckAllTrainDataIsValid(nSelectedList, nSelectedNum);
	}

	if ( nChkRs != 0 )
	{
		if ( AfxMessageBox("���ڲ�����ļƻ����Ƿ�ǿ���´", MB_YESNO) != IDYES )
		{
			AfxMessageBox("��ע��: ��ȡ���˱����´�ƻ�, �����ϵͳ������Ϣ�޸ļƻ�");
			return;
		}
		GpcLogTrainMessage(0,0,"��ע��: ���ڲ�����ƻ�,��ǿ���´�.");
	}

	// �����г�����
	bool bSecretTrainSendToSta = GetTGFunctionConfig()->IsSecretTrainToStation();

	m_pDoc->GetSTPLAN()->ClearPlanFlag();

	// 20150623�����ǳǼ�
	// ÿ�ε���·��ƻ�,��Ҫ������³����ϵ��dataĿ¼�³����ϵ�ļ� czh_relation.xml
	m_pDoc->GetSTPLAN()->OutprintCZHRelation();

	// ��dataĿ¼��,��ȡ���³����ļ� czh_relation.xml, ���ݳ����ļ��г����ϵ,��֯�ƻ�
	m_pDoc->GetSTPLAN()->LoadTrainRelation();

	if(GetTGFunctionConfig()->IsDisableAutoTrainNo())
	{
		GpcLogTrainMessage(0, 0, "�·��׶μƻ�,����Ա�رճ����Զ��������");
	}

	// Ԥ��֪ͨ
	std::vector<int> predict_station;
	std::vector<stplannotice> predict_notice;
	predict_station.clear();
	predict_notice.clear();

	bool send_to_passenger_sta=true;
	if (passernger_sta_count>0 && GetTGFunctionConfig()->IsQuestionStplanSelPassengerSta())
	{
		if (AfxMessageBox("��ȷ��,�Ƿ񽫵�ǰ�׶μƻ��·����͵�ϵͳ?",MB_YESNO) == IDYES)
			GpcLogTrainMessage(0,0,"����Աȷ���·��׶μƻ����͵�ϵͳ");
		else
		{
			GpcLogTrainMessage(0,0,"����Ա�ܾ��·��׶μƻ����͵�ϵͳ");
			send_to_passenger_sta=false;
		}
	}

	// �´�ƻ�
	CString report_early; // ���Ͼ֣�������㷢����ϸ��Ϣ

	pre_tickcount = cur_tickcount;
	CLBTH::Archive ar( CLBTH::Archive::store);
	GPCENTITY entityInfo;
	for(int j=0;j<nSelectedNum;j++)
	{		 
		GPCPLANRECEIVER rec;
		gpcdef.GetReceiverInfo(nSelectedList[j],rec); 

		gpcdef.GetEntityInfo(rec.sentity,entityInfo);  

		mMsgStPlanSendOrder.st =  entityInfo.nEntityId;
		//���ڳ�վPC
		if(entityInfo.nGroupId == STPC_WACHER)
		{
			mMsgStPlanSendOrder.prev_sta_count = 2;
			mMsgStPlanSendOrder.next_sta_count = 2;
		}
		//�������ɻ�
		else if(entityInfo.nGroupId == EQUIP_LIRC)
		{
			mMsgStPlanSendOrder.prev_sta_count = 1;
			mMsgStPlanSendOrder.next_sta_count = 1;
		}
		else if(entityInfo.nGroupId == EQUIP_SERVER) // ��TD������ת��
		{
			mMsgStPlanSendOrder.prev_sta_count = 0;
			mMsgStPlanSendOrder.next_sta_count = 0;

			if (!send_to_passenger_sta)
				continue;
		}
		mMsgStPlanSendOrder.st_echo = CLBTH::MsgStPlanSendOrder::PLAN_TOTAL_ECHO;  
		mMsgStPlanSendOrder.station =   rec.nstationNo;	 

		if(entityInfo.nGroupId == EQUIP_SERVER) // ��TD������ת��
			mMsgStPlanSendOrder.station_dmis =   rec.nstationNo;
		else
			mMsgStPlanSendOrder.station_dmis =   entityInfo.nSpecial1;

		if(rec.nEntryExit>0 && rec.nEntryExit<= CLBTH::MAX_STPLAN_DSTN_CNT )
		{			
			mMsgStPlanSendOrder.count = rec.nEntryExit;
			for(int k=0;k<rec.nEntryExit;k++)
			{
				mMsgStPlanSendOrder.entry[k] = rec.entry_exit[k].entry;
				mMsgStPlanSendOrder.exit[k] = rec.entry_exit[k].exit;
				mMsgStPlanSendOrder.yard[k] = 0;
				mMsgStPlanSendOrder.train_type[k] = (byte)rec.ntraintye;
			}			
		}
		else
		{
			mMsgStPlanSendOrder.count = 1;

			mMsgStPlanSendOrder.entry[0] = -1;
			mMsgStPlanSendOrder.exit[0] = -1;
			mMsgStPlanSendOrder.yard[0] = 0;
			mMsgStPlanSendOrder.train_type[0] = -1;			
		}

		CString waitstr;
		waitstr.Format("���ͼƻ� %s", entityInfo.sName); 
		ShowSendPlanText(waitstr);

		int match=rec.match; // �Ƿ���ȫƥ�����ڣ������Ƿ���
		m_pDoc->GetSTPLAN()->SetPlanFlag(entityInfo.nEntityId, mMsgStPlanSendOrder.station_dmis, j);
		bool bEnable3Function=false;
		if(gpcdef.IsBasicPlanIndex(m_pDoc->m_nCurrentSectionNo, nSelectedList[j]))
		{
			m_pDoc->GetSTPLAN()->SendBasicPlan(mMsgStPlanSendOrder, bSecretTrainSendToSta, match);
			bEnable3Function=true;
		}

		predict_notice.clear();
		m_pDoc->GetSTPLAN()->SendPlan(mMsgStPlanSendOrder, bSecretTrainSendToSta, predict_notice, report_early, bEnable3Function, match);	
		if(m_pPredictServer)
		{
			int k=0;
			for(; k<predict_station.size(); k++)
			{
				if(predict_station[k]==mMsgStPlanSendOrder.station)
					break;
			}
			if(k>=predict_station.size())
			{
				predict_station.push_back(mMsgStPlanSendOrder.station);
				m_pPredictServer->SendStplan(predict_notice, mMsgStPlanSendOrder.station);
			}
		}
	}
	predict_notice.clear();
	predict_station.clear();

	ShowSendPlanText("");
	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_AUTOCLOSE);
	//������������رնԻ���
	if(pButton->GetCheck() && nChkRs == 0)
		OnCancel() ;

	if ( nChkRs != 0 )
		AfxMessageBox("�ƻ���ǿ���´�����ϵͳ������Ϣ���ƻ���ȷ��");

	if (!report_early.IsEmpty() && GetTGFunctionConfig()->GetStPlanReportEarlyTimeRange()>0)
	{
		GpcLogTrainMessage(0,0,"��ʾ����Ա��Ϣ:"+report_early);
		CDlgMarkResult* dlg = new CDlgMarkResult("��ע��", report_early, this);
		if (dlg->DoModal() == IDOK)
		{
			GpcLogTrainMessage(0,0,"����Ա���ȷ��");
			delete dlg;
		}

	}
}

void CSendStationPlanDlgEx::ShowSendPlanText(CString text)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PLAN_TEXT);
	if(NULL == pEdit)
		return;

	pEdit->SetWindowText(text);
	pEdit->UpdateWindow();
}

LRESULT CSendStationPlanDlgEx::ChangeStationReceiveStatus(WPARAM wParam,LPARAM lParam) 
{
	int nEntityIndex,nFlag;
	nEntityIndex = (int)wParam;
	nFlag = (int)lParam;

	GpcLogTrainMessage(0,0,"�¼ƻ���ִ: rec_status: %d  index: %d", nFlag, nEntityIndex);

	int nSectionNo = m_pDoc->m_nCurrentSectionNo;
	time_t t_begin,t_end,t_send,t_recv_time;
	CString sContent;
	for(int i = 0; i <= m_pPlanReceiveInfo->nEntityNum; i++)
	{
		if(nEntityIndex != m_pPlanReceiveInfo->m_StationPlan[i].nEntityIndex)
			continue;
		t_begin = m_pPlanReceiveInfo->m_StationPlan[i].begin_time;
		t_end = m_pPlanReceiveInfo->m_StationPlan[i].end_time;
		t_send = m_pPlanReceiveInfo->m_StationPlan[i].send_time;
		t_recv_time = m_pPlanReceiveInfo->m_StationPlan[i].recv_time;

		CTime t;
		if(t_send>0) 
		{ 
			t = CTime(t_send);
			if(t.GetMinute()>9)
				sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
			else
				sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
		}
		else
			sContent.Empty();    
		m_pGridCtrl->SetItemText (i+1,3,sContent); //����ʱ��

		if(t_begin>0) 
		{
			t = CTime(t_begin);
			if(t.GetMinute()>9)
				sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
			else
				sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
		}
		else
			sContent.Empty();   
		m_pGridCtrl->SetItemText (i+1,4,sContent); // ��ʼʱ��

		if(t_end>0) 
		{
			t = CTime(t_end);
			if(t.GetMinute()>9)
				sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
			else
				sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
		}
		else
			sContent.Empty();   
		m_pGridCtrl->SetItemText (i+1,5,sContent); //����ʱ��

		if(t_recv_time>0) 
		{ 
			t = CTime(t_recv_time);
			if(t.GetMinute()>9)
				sContent.Format("%2d:%2d",t.GetHour(),t.GetMinute());
			else
				sContent.Format("%2d:0%d",t.GetHour(),t.GetMinute());
		}
		else
			sContent.Empty();   

		if(nFlag ==CLBTH::MsgStPlanStatusAnswer::PLAN_SIGN )//1
		{
			m_pGridCtrl->SetItemText (i+1,6,"��"); //����
			m_pGridCtrl->SetItemText (i+1,7,"��"); //ǩ��
			m_pGridCtrl->SetItemFgColour (i+1,6,CLR_GREEN);
			m_pGridCtrl->SetItemFgColour (i+1,7,CLR_GREEN);

			m_pGridCtrl->SetItemFgColour (i+1,8,CLR_GREEN);  //ǩ��ʱ��
			m_pGridCtrl->SetItemFgColour (i+1,9,CLR_GREEN);  //ǩ����

			CString str;
			str.Format("%s",m_pPlanReceiveInfo->m_StationPlan[i].strSignName); 
			m_pGridCtrl->SetItemText (i+1,8,str);      //ǩ����    
			m_pGridCtrl->SetItemText (i+1,9,sContent); //ǩ��ʱ��
		}
		else
		{
			switch(nFlag)
			{ 
			case CLBTH::MsgStPlanStatusAnswer::PLAN_RECV: // 0
				m_pGridCtrl->SetItemText(i+1,6,"��");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_GREEN);
				m_pGridCtrl->SetItemFgColour (i+1,2,CLR_BLUE);
				break;
			case CLBTH::MsgStPlanStatusAnswer::PLAN_NO_CHANGE://3
				m_pGridCtrl->SetItemText(i+1,6,"�ƻ�û��");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_INDIANRED);
				break;
			case CLBTH::MsgStPlanStatusAnswer::PLAN_ENTITY_DEAD: //5
				m_pGridCtrl->SetItemText(i+1,6,"�Ͽ�");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_INDIANRED);
				break;
			case CLBTH::MsgStPlanStatusAnswer::PLAN_NO_DATA://4
				m_pGridCtrl->SetItemText(i+1,6,"���·�����");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_INDIANRED);
				break;
			case CLBTH::MsgStPlanStatusAnswer::PLAN_SENDING: //2
				m_pGridCtrl->SetItemText(i+1,6,"�ѷ���");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_INDIANRED);
				m_pGridCtrl->SetItemFgColour (i+1,2,CLR_RED);
				break;
			case CLBTH::MsgStPlanStatusAnswer::PLAN_INIT://7
				m_pGridCtrl->SetItemText(i+1,6,"");
				m_pGridCtrl->SetItemFgColour (i+1,6,CLR_INDIANRED);
				break;

			}
			m_pGridCtrl->SetItemText (i+1,7,"��"); //ǩ��
			m_pGridCtrl->SetItemFgColour (i+1,7,CLR_INDIANRED);
			m_pGridCtrl->SetItemText (i+1,8,"");      //ǩ����    
			m_pGridCtrl->SetItemText (i+1,9,""); //ǩ��ʱ��
			m_pGridCtrl->SetItemFgColour (i+1,8,CLR_INDIANRED);
			m_pGridCtrl->SetItemFgColour (i+1,9,CLR_INDIANRED);

		}

	}

	m_pGridCtrl->UpdateData();
	Invalidate(FALSE);
	return TRUE;
}

void CSendStationPlanDlgEx::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1976)
	{
		SetSendPlanTimeRange();
	}
	CPropertyPage::OnTimer(nIDEvent);
}

void CSendStationPlanDlgEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CSendStationPlanDlgEx::OnGrid() 
{
	BOOL checkAllLirc=TRUE, checkAllStpc=TRUE;
	CGridCellCheck *pCell;
	CString strText;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		strText = m_pGridCtrl->GetItemText(row,2);  
		if(strText =="��վ�ն�")	
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
			if(!pCell->GetCheck())
				checkAllStpc=FALSE;
		}

		if(strText =="���ɻ�")	
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
			if(!pCell->GetCheck())
				checkAllLirc=FALSE;
		}
	}

	CButton *pLirc,*pStpc,*pAll;
	pLirc = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	pStpc = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	pAll = (CButton *)GetDlgItem(IDC_CHECK);
	if (!checkAllStpc)
	{
		pStpc->SetCheck(FALSE);
		pAll->SetCheck(FALSE);
	}
	else
		pStpc->SetCheck(TRUE);

	if (!checkAllLirc)
	{
		pLirc->SetCheck(FALSE);
		pAll->SetCheck(FALSE);
	}
	else
		pLirc->SetCheck(TRUE);

	if (checkAllStpc && checkAllLirc)
		pAll->SetCheck(TRUE);
}

void CSendStationPlanDlgEx::OnDestroy() 
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
	CPropertyPage::OnDestroy();
}

void CSendStationPlanDlgEx::OnCancel() 
{
	GetParent()->ShowWindow(SW_HIDE);
}
void CSendStationPlanDlgEx::SendMsgStPlanStatusRequest(CLBTH::Archive &ar, int nStationNo)
{
	do{		
		CLBTH::MsgStPlanStatusRequest mMsgStPlanStatusRequest;

		mMsgStPlanStatusRequest.con_id = m_pDoc->m_nCurrentSectionNo;  //���� 1 
		mMsgStPlanStatusRequest.station = nStationNo; //station no 3 

		int nEntityNum = m_pPlanReceiveInfo->nEntityNum;
		if(nEntityNum<=0)
			break;
		mMsgStPlanStatusRequest.st_cnt =0; // 4
		for(int i=0;i<nEntityNum;i++) //5 
		{
			if(nStationNo ==
				m_pPlanReceiveInfo->m_StationPlan[i].nStation)
			{
				mMsgStPlanStatusRequest.st[mMsgStPlanStatusRequest.st_cnt]
				= m_pPlanReceiveInfo->m_StationPlan[i].Entityid;
				mMsgStPlanStatusRequest.st_cnt++;
			}
		}
		mMsgStPlanStatusRequest.serialize(ar);

	} while(0);

}
void CSendStationPlanDlgEx::QueryStStatus()
{
	int nEntityNum ;
	nEntityNum= m_pPlanReceiveInfo->nEntityNum;

	int nPreStationNo =-1;
	int nCurStationNo;
	CLBTH::Archive ar( CLBTH::Archive::store);


	for(int i=0;i<nEntityNum;i++) //5 
	{
		nCurStationNo = m_pPlanReceiveInfo->m_StationPlan[i].nStation;
		if(nPreStationNo !=nCurStationNo)
		{
			SendMsgStPlanStatusRequest(ar, nCurStationNo);
			nPreStationNo = nCurStationNo;
		}
	}



	char *ptr = (char *)ar.getBuffer();
	int count =  ar.getCount();


	SendTGCommMessage(0,ar.getCount(),(char *)ar.getBuffer());	
}

int CSendStationPlanDlgEx::GetStationList(int *nStList)
{
	int nEntityNum ;
	nEntityNum= m_pPlanReceiveInfo->nEntityNum;

	int nPreStationNo =-1;
	int nCurStationNo;
	int nStationNum= 0;
	for(int i=0;i<nEntityNum;i++) //5 
	{
		nCurStationNo = m_pPlanReceiveInfo->m_StationPlan[i].nStation;
		if(nCurStationNo !=nPreStationNo)
		{		    
			nStList[nStationNum] = nCurStationNo;
			nStationNum++;
			nPreStationNo = nCurStationNo;
		}
	}
	return nStationNum;

}

int CSendStationPlanDlgEx::GetEntityList(int nStationNo, int *nEntityList)
{
	if(nStationNo<-1)
		return -1;
	int nEntityNum ;
	int nEntity =0;
	nEntityNum=gpcdef.GetSpecialGPCList(m_pDoc->m_nCurrentSectionNo, nEntityList);
	if (nEntityNum==0)
		nEntityNum = gpcdef.GetSectionPlanReceivers(m_pDoc->m_nCurrentSectionNo,nEntityList);
	GPCPLANRECEIVER RECIVER_rec;

	for(int i=0;i<nEntityNum;i++)
	{
		gpcdef.GetReceiverInfo(nEntityList[i],RECIVER_rec);
		if(nStationNo == RECIVER_rec.nstationNo)
		{		  
			nEntityList[nEntity] = i; //gpcdef.GetEntityInfo(RECIVER_rec.sentity,enc);
			nEntity++;
		}

	}
	return nEntity;
}
void CSendStationPlanDlgEx::OnCheckAll() 
{
	CButton *pWnd;    CGridCellCheck *pCell;
	pWnd = (CButton *)GetDlgItem(IDC_CHECK);
	int bFlag = pWnd->GetCheck();

	CString strText;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		strText = m_pGridCtrl->GetItemText(row,2);
		if(strText !="��վ�ն�" && strText !="���ɻ�" && strText !="վ��") // ���ǵ� �ͷ��ƻ� qiaoyt 20100726
		{
			pCell->SetCheck(TRUE);
			continue;
		}
		pCell->SetCheck(bFlag);
	}

	CButton *pWnd1,*pWnd2;
	pWnd1 = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	pWnd1->SetCheck(bFlag);
	pWnd2->SetCheck(bFlag);

	((CComboBox*)GetDlgItem(IDC_CBO_GROUP))->SetCurSel(-1);
}

void CSendStationPlanDlgEx::OnCheckStpc() 
{
	CButton *pWnd;    CGridCellCheck *pCell;
	pWnd = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	int bFlag = pWnd->GetCheck();

	CString strText;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		strText = m_pGridCtrl->GetItemText(row,2);  
		if(strText !="��վ�ն�")  // ���ǵ� �ͷ��ƻ� qiaoyt 20100726
			continue;
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		pCell->SetCheck(bFlag);
	}
	CButton *pWnd2,*pWndAll;
	pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	pWndAll = (CButton *)GetDlgItem(IDC_CHECK);
	int bFlag2 = pWnd2->GetCheck(); 
	if(bFlag && bFlag2)
		pWndAll->SetCheck(TRUE);
	else  
		pWndAll->SetCheck(FALSE);

	((CComboBox*)GetDlgItem(IDC_CBO_GROUP))->SetCurSel(-1);
}

void CSendStationPlanDlgEx::OnCheckLirc() 
{
	CButton *pWnd;    CGridCellCheck *pCell;
	pWnd = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	int bFlag = pWnd->GetCheck();

	CString strText;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		strText = m_pGridCtrl->GetItemText(row,2);  
		if(strText !="���ɻ�")  // ���ǵ� �ͷ��ƻ� qiaoyt 20100726
			continue;
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		pCell->SetCheck(bFlag);
	}

	CButton *pWnd2,*pWndAll;
	pWnd2 = (CButton *)GetDlgItem(IDC_CHECK_STPC);
	pWndAll = (CButton *)GetDlgItem(IDC_CHECK);
	int bFlag2 = pWnd2->GetCheck(); 
	if(bFlag && bFlag2)
		pWndAll->SetCheck(TRUE);
	else  
		pWndAll->SetCheck(FALSE);

	((CComboBox*)GetDlgItem(IDC_CBO_GROUP))->SetCurSel(-1);
}

void CSendStationPlanDlgEx::ShowStatus(int id, CString str)
{
	if(str.IsEmpty())
		return;
	if(m_pStplanGrid)
	{
		m_pStplanGrid->AddWarnInfo(id, str);
	}
}

//����г����ݵ���Ч��
int CSendStationPlanDlgEx::CheckAllTrainDataIsValid(int nSelectedList[], int nSelectedNum)
{
	CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
	if(pholder==NULL)
		return 0;

	CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();
	if(pdata==NULL)
		return 0;

	StationStplanNotWarning stplan_cfg;
	CTG_ScheduleInfo schd;
	pdata->GetScheduleInfo(schd); 
	
	GetTGFunctionConfig()->LoadStplanNoWarningStation(schd.m_nSectionID, stplan_cfg);

	GPCPLANRECEIVER rec;

	// �ȼ��ƻ��Ϸ���
	m_nSelectedStaNum = 0;
	for(int j=0;j<nSelectedNum;j++)
	{		 	 
		gpcdef.GetReceiverInfo(nSelectedList[j],rec);
		if(!GetTGFunctionConfig()->IsCheckAllStationStPlan())
		{
			if(gpcdef.IsCtcStation(rec.nstationNo))
			{
				if(!IsNeedCheckCtcModeStplan(rec.nstationNo))
					continue;
			}
			else
				continue;
		}

		bool bCheckStplan=true;
		for(int n=0; n<stplan_cfg.station_count; n++)
		{
			if(rec.nstationNo==stplan_cfg.station_lists[n])
			{
				bCheckStplan=false;
				break;
			}
		}

		// �¼ƻ��Ƿ񱨾�
		if(bCheckStplan)
		{
			AddSelectedSta(rec.nstationNo);	
		}
		else
		{
			GpcLogTrainMessage(0,0,"�·��׶μƻ�,��������section=%d station=%d���ò����ƻ�������.",
				schd.m_nSectionID, rec.nstationNo);
		}
	}

	if(m_pStplanGrid)
		m_pStplanGrid->ClearView();

	int iResult=CheckStationSideFull(pdata);
	if(m_nSelectedStaNum <=0)
		return iResult;

		
	bool bCheckWithBasic=GetTGFunctionConfig()->IsCheckStPlanWithBasicSchd();
	CString strInfo;
	
	CTG_TrainPtrGroup group;
	pdata->FindTrains(group,NULL_STATION,start_time,end_time);

	CTG_ObjectPtrGroup markgroup;
	pdata->FindMarks(markgroup,start_time,end_time);
	
	for(int j=0;j<group.GetTrainCount();j++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(j);
		if(NULL==ptrain)
			continue;
		
		CString waitstr;
		waitstr.Format("���ƻ�:%s", ptrain->GetTrainID()); 
		ShowSendPlanText(waitstr);

		for ( int i = 0; i < ptrain->GetTrainRecordCount(); i++ )
		{
			CTG_TrainRecord  *rec = ptrain->GetTrainRecord(i);
			if(NULL == rec)
				continue;

			if (IsSelectedSta(rec->GetStationNo()) && rec->IfInTimeScope(start_time,end_time))
			{
				pholder->CheckStplan(ptrain, start_time, end_time, group, markgroup, i);
				if(rec->HaveWarnInfo())
				{
					WORD id=0;
					CString text;
                    for(int index=0; index<1024; index++)
					{
						if(rec->GetStplanWarnInfo(index, id, text))
						{
							ShowStatus(id, text);
						}
						else
						{
							break;
						}
					}
					rec->ClearStplanWarnInfo();
				}

				CString str="";
				if(pholder->CheckTrainDepartStplan(m_pDoc->m_nCurrentSectionNo, *ptrain, markgroup, i, str))
				{
					ShowStatus(29, str);
				}

				if(ptrain->IsCRHTrain())
				{
					if(IS_VALID_SIDE(rec->m_data.arrive_side))
					{
						if(!GetTGBasicParam()->VerifyNotCRHStopSide(rec->m_data.arrive_side, rec->m_data.station, ptrain->GetTrainInfo()->m_data))
						{
							const char* p=GetTGBasicParam()->GetSideNameByID(rec->m_data.station, rec->m_data.arrive_side);
							if(p)
							{
								str.Format("%sվ%s,���ܽӷ�������%s��", gpcdef.GetStationName(rec->m_data.station), p, ptrain->GetTrainID());
								ShowStatus(35, str);
							}
						}
					}
					if(IS_VALID_SIDE(rec->m_data.depart_side) && (rec->m_data.arrive_side!=rec->m_data.depart_side))
					{
						if(!GetTGBasicParam()->VerifyNotCRHStopSide(rec->m_data.depart_side, rec->m_data.station, ptrain->GetTrainInfo()->m_data))
						{
							const char* p=GetTGBasicParam()->GetSideNameByID(rec->m_data.station, rec->m_data.depart_side);
							if(p)
							{
								str.Format("%sվ%s,���ܽӷ�������%s��", gpcdef.GetStationName(rec->m_data.station), p, ptrain->GetTrainID());
								ShowStatus(35, str);
							}
						}
					}
				}
				
				//���ƻ��Ƿ���ʵ�ʵ�
				if(rec->IsActualDepartTimeRecord())
				{
					CString str="";
					if(!rec->IsActualArriveTimeRecord())
					{
						str.Format("%s %s ����Ϊ�ƻ���, ���ǳ���Ϊʵ�ʵ�", ptrain->GetTrainID(), gpcdef.GetStationName(rec->m_data.station));
						//m_lbInfoList->AddString(str);
						GpcLogTrainMessage(0,0,"�¼ƻ�ʱ����������ƻ�:%s",str);
					}
				}
				else if(rec->IsPassRec())
				{
					if(rec->IsActualArriveTimeRecord())
					{
						str.Format("%s %s ͨ���г� ����Ϊʵ�ʵ�, ���ǳ���Ϊ�ƻ���", ptrain->GetTrainID(), gpcdef.GetStationName(rec->m_data.station));
						GpcLogTrainMessage(0,0,"�¼ƻ�ʱ����������ƻ�:%s",str);
					}
				}
 			    // ����Ƚϻ���ͼ��Ϣ
				if(gpcdef.IsCtcStation(rec->m_data.station) && bCheckWithBasic && ptrain->IsPassengerTrain()) // ֻ�жϿͳ�
				{
					int flag=0; // 1:ʼ�� 2:�յ� 3:ͨ�� 4:����
					if(i==0 && rec->IsStartRec())
						flag=1;
					else if(i+1==ptrain->GetTrainRecordCount() && rec->IsTerminalRec())
						flag=2;
					else if(rec->IsPassRec())
						flag=3;
					else
						flag=4;
					if(GetTGFunctionConfig()->IsAlarmOnlyCheckCRHTrainSide())
					{
						if (ptrain->IsCRHTrain())
						{
							const CTG_TrainRecord  *prev_rec = ptrain->GetTrainRecordNoDelete_Const(i-1);
							const CTG_TrainRecord  *next_rec = ptrain->GetTrainRecordNoDelete_Const(i+1);
							int prev_station=0, next_station=0;
							if(prev_rec!=NULL)
								prev_station=prev_rec->m_data.station;
							if(next_rec!=NULL)
								next_station=next_rec->m_data.station;
							CheckPassengerTrainSideAndTm(rec, flag, prev_station, next_station, ptrain->IsCRHTrain());
						}
					}
					else
					{
						const CTG_TrainRecord  *prev_rec = ptrain->GetTrainRecordNoDelete_Const(i-1);
						const CTG_TrainRecord  *next_rec = ptrain->GetTrainRecordNoDelete_Const(i+1);
						int prev_station=0, next_station=0;
						if(prev_rec!=NULL)
							prev_station=prev_rec->m_data.station;
						if(next_rec!=NULL)
							next_station=next_rec->m_data.station;

						CheckPassengerTrainSideAndTm(rec, flag, prev_station, next_station, ptrain->IsCRHTrain());
					}
				}
			}
		}
	}
	
	if(m_pStplanGrid)
	{
		m_pStplanGrid->DisplayWarnInfo();
		if(m_pStplanGrid->HaveStplanWarnInfo())
		{	
			return -1;
		}	
	}
	return 0;
}

int  CSendStationPlanDlgEx::AddSelectedSta( int nStNo )
{
	if (IsSelectedSta(nStNo))
		return 0;
	if (m_nSelectedStaNum >= MAX_STATION_PER_SECTION )
		return -1;
	m_nSelectedStaList[m_nSelectedStaNum++] = nStNo;
	return 0;
}

bool CSendStationPlanDlgEx::IsSelectedSta( int nStNo )
{
	for ( int i = 0; i < m_nSelectedStaNum; i++ )
	{
		if ( nStNo == m_nSelectedStaList[i] )
			return TRUE;
	}
	return FALSE;
}

void CSendStationPlanDlgEx::OnBnClickedShowDetail()
{
	SetSendPlanTimeRange();
	UpdateData(TRUE);

	int  nSelectedNum = 0;
	int  nSelectedList[MAX_STATION_PER_SECTION];

	CString strTime;
	CEdit *pEdit;
	long startTime,endTime;
	long startDate,endDate;

	pEdit = (CEdit*)GetDlgItem(IDC_START_TIME);
	pEdit->GetWindowText(strTime);
	startTime =  OsConvertStringtoTime((char *)(LPCTSTR)strTime);
	startDate = m_pDoc->GetCurrentTGDate();
	pEdit = (CEdit*)GetDlgItem(IDC_END_TIME);
	pEdit->GetWindowText(strTime);
	endTime =  OsConvertStringtoTime((char *)(LPCTSTR)strTime);
	if(startTime > endTime)
		endDate = OsGetNextDate(PMTG.ThreadId,m_pDoc->GetCurrentTGDate());
	else
		endDate = m_pDoc->GetCurrentTGDate();


	if(long_time(startDate,startTime) <= 0 || long_time(endDate,endTime) <= 0){
		AfxMessageBox("�´�ʱ�䷶Χ����,����������ʱ��!");
		return;
	}

	if(long_time(endDate,endTime) - long_time(startDate,startTime) >5*60*60){
		AfxMessageBox("�´�ʱ�䷶Χ���ܴ�����Сʱ,����������!");
		return;	
	}
	CString str;
	BOOL m_bChecked;
	CGridCellCheck *pCell;
	int nSectionNo = m_pDoc->m_nCurrentSectionNo;
	int nSelEntity = 0;
	for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		m_bChecked = pCell->GetCheck ();
		if(m_bChecked)
		{
			nSelEntity++;
		}
	}
	if(nSelEntity<=0)
	{
		AfxMessageBox("����ѡ���·�ʵ�����!");
		return;
	}
	for(row = 1; row <= m_pPlanReceiveInfo->nEntityNum; row++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		m_bChecked = pCell->GetCheck();
		if(m_bChecked)
		{
			//ʵ��index
			nSelectedList[nSelectedNum]= m_pPlanReceiveInfo->m_StationPlan[row-1].nEntityIndex;
			nSelectedNum++;
		}
	}

	m_pGridCtrl->UpdateData ();

	start_time = long_time(startDate,startTime);//3
	end_time = long_time(endDate,endTime); //4

	CheckAllTrainDataIsValid(nSelectedList, nSelectedNum);
	return;
}

HBRUSH CSendStationPlanDlgEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	if( GetDlgItem(IDC_LIST_MSG)->m_hWnd==pWnd->m_hWnd )
	{     
		pDC->SetBkColor(RGB(221,220,219));
	}  

	return hbr;
}

void CSendStationPlanDlgEx::OnCbnSelchangeCboTimeRange()
{
	int cur_sel=((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->GetCurSel();
	if(cur_sel>=0)
	{
		int tmPlanSendRange = ((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->GetItemData(cur_sel);
		if(m_pDoc->m_tmPlanSendRange != tmPlanSendRange)
			SendNoticeToDCmd(1);
		m_pDoc->m_tmPlanSendRange = tmPlanSendRange;
		SetSendPlanTimeRange();
	}
}

void CSendStationPlanDlgEx::OnCbnEditchangeCboTimeRange()
{
	int cur_sel=((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->GetCurSel();
	if(cur_sel>=0)
	{
		int tmPlanSendRange = ((CComboBox *)GetDlgItem(IDC_CBO_TIME_RANGE))->GetItemData(cur_sel);
		if(m_pDoc->m_tmPlanSendRange != tmPlanSendRange)
			SendNoticeToDCmd(1);
		m_pDoc->m_tmPlanSendRange = tmPlanSendRange;
		SetSendPlanTimeRange();
	}
}

void CSendStationPlanDlgEx::OnBnClickedClearAlarm()
{
	if(m_pStplanGrid)
		m_pStplanGrid->ClearView();
}
//ֻ��CTC��վ�����ɻ��¼ƻ�
void CSendStationPlanDlgEx::OnBnClickedCheckCtcOnly()
{
	CButton *pWnd;    CGridCellCheck *pCell;
	pWnd = (CButton *)GetDlgItem(IDC_CHECK_CTC_ONLY);
	int bFlag = pWnd->GetCheck();
	if(!bFlag)	
		return;
	int nSectionNo = m_pDoc->m_nCurrentSectionNo;
	for(int row = 1; row <= m_pPlanReceiveInfo->nEntityNum; row++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
		int stationNo = m_pPlanReceiveInfo->m_StationPlan[row-1].nStation;
		if(gpcdef.IsCtcStation(stationNo))
			pCell->SetCheck(TRUE);
		else
			pCell->SetCheck(FALSE);


	}

	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK_LIRC);
	pBtn->SetCheck(TRUE);
	OnCheckLirc();
}

void CSendStationPlanDlgEx::OnBnClickedCheckAutoclose()
{
}

bool CSendStationPlanDlgEx::IsNeedCheckCtcModeStplan(int station)
{
	bool bret = false;
	TG_STATION_STATUS * pst = GetStationStatusPtr(station);
	if(NULL != pst)
	{
		switch(pst->ctrl_mode)
		{
		case SMODE_DISTRIBUTED: // ��ɢ����
			GpcLogTrainMessage(0,0,"CTC��վ:%s, ��ɢ����, ��Ҫ���ƻ�", gpcdef.GetStationName(station));
			bret = true;
			break;
		case SMODE_CENTER_CTRL: // ������ȫ����
			GpcLogTrainMessage(0,0,"CTC��վ:%s, ������ȫ����, ��Ҫ���ƻ�", gpcdef.GetStationName(station));
			bret = true;
			break;
		case SMODE_STATION_CTRL: // ��վ��ȫ����
			bret = GetTGFunctionConfig()->IsCheckCtcModeStaCtrlStPlan();
			if(bret)
				GpcLogTrainMessage(0,0,"CTC��վ:%s, ��վ��ȫ����, ��Ҫ���ƻ�", gpcdef.GetStationName(station));
			else
				GpcLogTrainMessage(0,0,"CTC��վ:%s, ��վ��ȫ����, ����tg_logic��NoCheckCtcModeStaCtrlStPlan����,����Ҫ���ƻ�", 
																			gpcdef.GetStationName(station));
			break;
		case SMODE_EMERGENCY: // �ǳ�վ��
			GpcLogTrainMessage(0,0,"CTC��վ:%s, �ǳ�վ��, ����Ҫ���ƻ�", gpcdef.GetStationName(station));		
			break;
		default:
			;
		}
	}

	return bret;
}

//����г����ݵ���Ч��
void CSendStationPlanDlgEx::CheckPassengerTrainSideAndTm(CTG_TrainRecord *rec, int flag, int prev_station, int next_station, BOOL bCrhTrain)
{
	if(rec==NULL)
		return;
	
	CString trainid=rec->m_data.depart_train_id;
	if(flag==2)
		trainid=rec->m_data.arrive_train_id;
	int station=rec->m_data.station;

	int n=0;
	for(n=0; n<m_st_basic_info_count&&n<MAX_STATION_PER_SECTION; n++)
	{
		if(st_basic_info[n].station!=rec->m_data.station)
		{
			continue;
		}
		int num=0;
		for(int m=0;m<st_basic_info[n].count;m++)
		{
			int bflag=st_basic_info[n].info[m].flag;

			CString basictrainid=st_basic_info[n].info[m].de_trainid;
			if(bflag==2)
				basictrainid=st_basic_info[n].info[m].ar_trainid;
			
			bool bCondition=false;
			if(GetTGFunctionConfig()->IsConsiderTwicePassSameStation())
			{
				if((trainid == basictrainid) && (st_basic_info[n].info[m].prev_station == prev_station) && (st_basic_info[n].info[m].next_station == next_station))
				{
					bCondition=true;
				}
			}
			else
			{
				if(trainid == basictrainid)
				{
					bCondition=true;
				}
			}
			
			if(bCondition)
			{
				num++;
				if(bflag==flag)
				{
					bool bCheckArriveSide=false, bCheckDepartSide=false;
					if(1==bflag) // ʼ��
					{
						bCheckArriveSide=false;
						bCheckDepartSide=true;
					}
					else if(flag==2) // �յ�
					{
						bCheckArriveSide=true;
						bCheckDepartSide=false;
					}
					else if(flag==3 || flag==4)
					{
						bCheckArriveSide=true;
						bCheckDepartSide=true;
					}
					else
					{
						bCheckArriveSide=false;
						bCheckDepartSide=false;
					}
					
					if(bCheckArriveSide && (rec->m_data.arrive_side != st_basic_info[n].info[m].ar_side))
					{
						if (bCrhTrain)
						{
							CString str;
							str.Format("������%s����%sվ�ӳ��ɵ���ͼ���ɵ���һ��", trainid, gpcdef.GetStationName(station));
							ShowStatus(38, str);
						}
						else
						{
							CString str;
							str.Format("%s����%sվ�ӳ��ɵ���ͼ���ɵ���һ��", trainid, gpcdef.GetStationName(station));
							ShowStatus(31, str);
						}
					}
					if(bCheckDepartSide && (rec->m_data.depart_side != st_basic_info[n].info[m].de_side))
					{
						if (bCrhTrain)
						{
							CString str;
							str.Format("������%s����%sվ�����ɵ���ͼ���ɵ���һ��", trainid, gpcdef.GetStationName(station));
							ShowStatus(37, str);
						}
						else
						{
							CString str;
							str.Format("%s����%sվ�����ɵ���ͼ���ɵ���һ��", trainid, gpcdef.GetStationName(station));
							ShowStatus(30, str);
						}
					}
				}
				else
				{
					CString str1,str2;
					if(flag==1)
						str1.Format("�ƻ�:ʼ��,");
					else if(flag==2)
						str1.Format("�ƻ�:�յ�,");
					else if(flag==3)
						str1.Format("�ƻ�:ͨ��,");
					else if(flag==4)
						str1.Format("�ƻ�:����,");

					if(bflag==1)
						str2.Format("ͼ��:ʼ��");
					else if(bflag==2)
						str2.Format("ͼ��:�յ�");
					else if(bflag==3)
						str2.Format("ͼ��:ͨ��");
					else if(bflag==4)
						str2.Format("ͼ��:����");

					CString str;
					str.Format("%s����%sվ��ͼ��������һ��:%s%s", trainid, gpcdef.GetStationName(station), str1, str2);
					ShowStatus(32, str);
				}
			}
		}
		if(num==0)
		{
			CString str;
			str.Format("����ͼ���޳���%s,���ʵ%sվ������ϵ�͹ɵ�", trainid, gpcdef.GetStationName(station)); // stplan warn 20161019
			ShowStatus(33, str);
		}
		else if(num>=2)
		{
			CString str;
			str.Format("����ͼ����%d������%s,���ʵ%sվ������ϵ�͹ɵ�", num,trainid, gpcdef.GetStationName(station)); // stplan warn 20161019
			ShowStatus(34, str);
		}
		break;
	}
	if(n>=m_st_basic_info_count)
	{
		CString str;
		str.Format("����ͼ���޳���%s,���ʵ%sվ������ϵ�͹ɵ�",trainid, gpcdef.GetStationName(station)); // stplan warn 20161019
		ShowStatus(33, str);
	}
	return;
}

void CSendStationPlanDlgEx::OnCbnSelchangeCboGroup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox *group_box = (CComboBox*)GetDlgItem(IDC_CBO_GROUP);

	CGridCellCheck *pCell;
	int cur_sel = group_box->GetCurSel();
	int group_id = group_box->GetItemData(cur_sel);

	SpecialGPCStplanGroup *stplan_group=NULL;
	stplan_group = gpcdef.GetSpcialGroupByIndex(group_id);

	if (stplan_group==NULL)
		return;

	int no;

	for (int i=1; i<m_pGridCtrl->GetRowCount(); i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
		pCell->SetCheck(FALSE);
	}

	for (int index=0; index<stplan_group->nPlanReceiverCount; index++)
	{
		int recv_id = stplan_group->nPlanReceiverList[index];
		for(int row=1;row<m_pGridCtrl->GetRowCount();row++ )
		{
			no = atoi(m_pGridCtrl->GetItemText(row,0));  
			if(m_nRecList[no-1] == recv_id)
			{
				pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,2);
				pCell->SetCheck(TRUE);
				
				break;
			}
		}
	}

	OnGrid();
}

void CSendStationPlanDlgEx::OnBnClickedCheck2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_disableAutoChangeTrainNO.GetCheck()==1)
	{
		CString str;
		str="��ȷʵҪ�ر�: �յ��г������Զ����Ϊͬ��·ʼ���г����ι�����?";
		GpcLogTrainMessage(0, 0, "%s", str);
		if(MessageBox(str, "��Ҫ����", MB_YESNO)==IDYES)
		{
			GpcLogTrainMessage(0, 0, "����Աѡ�� ��");
			GetTGFunctionConfig()->SetDisableAutoTrainNo(true);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "����Աѡ�� ��");
			GetTGFunctionConfig()->SetDisableAutoTrainNo(false);
		}
	}
	else
	{
		CString str;
		str="��ȷʵҪ����: �յ��г������Զ����Ϊͬ��·ʼ���г����ι�����?";
		GpcLogTrainMessage(0, 0, "%s", str);
		if(MessageBox(str, "��Ҫ����", MB_YESNO)==IDYES)
		{
			GpcLogTrainMessage(0, 0, "����Աѡ�� ��");
			GetTGFunctionConfig()->SetDisableAutoTrainNo(false);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "����Աѡ�� ��");
			GetTGFunctionConfig()->SetDisableAutoTrainNo(true);
		}
	}

	SetChngTrainButtonByConfig();
}

void CSendStationPlanDlgEx::SetChngTrainButtonByConfig()
{
	if(GetTGFunctionConfig()->IsDisableAutoTrainNo())
	{
		m_disableAutoChangeTrainNO.SetCheck(1);
		GpcLogTrainMessage(0, 0, "�ر��յ��г������Զ����Ϊͬ��·ʼ���г����ι���");
	}
	else
	{
		m_disableAutoChangeTrainNO.SetCheck(0);
		GpcLogTrainMessage(0, 0, "�����յ��г������Զ����Ϊͬ��·ʼ���г����ι���");
	}
}

// ���ɻ���ͼ��Ϣ,�����Ƿ��Զ��������
void CSendStationPlanDlgEx::SetBasicSchduleInfo()
{
	SetChngTrainButtonByConfig();
	m_st_basic_info_count=0;
	memset(st_basic_info, 0, sizeof(st_basic_info));

	bool bCheckWithBasic=GetTGFunctionConfig()->IsCheckStPlanWithBasicSchd();
	if(bCheckWithBasic)
	{
		CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
		CTG_TrainScheduleBase *pbasicdata=pholder->GetBasicScheduleData();

		CTG_TrainPtrGroup groupBasic;
		pbasicdata->FindPessangerTrains(groupBasic);
		
		for(int n=0;n<groupBasic.GetTrainCount();n++)
		{
			CTG_SingleTrain *pbasictrain=groupBasic.GetTrainPtr(n);
			if(pbasictrain==NULL)
				continue;
			if(pbasictrain->GetTrainRecordCount()<=0)
				continue;
			
			if(pbasictrain->IsPassengerTrain()==FALSE) // ֻ�жϿͳ�
				continue;
			
			if(GetTGFunctionConfig()->IsAlarmOnlyCheckCRHTrainSide())
			{
				if (pbasictrain->IsCRHTrain())
				{
					;// only record crh train
				}
				else
				{
					continue;
				}
			}
			for(int i=0; i<pbasictrain->GetTrainRecordCount(); i++)
			{
				CTG_TrainRecord *prec=pbasictrain->GetTrainRecordByRecIndex(i);
				if(prec==NULL)
				{
					continue;
				}
				CTG_TrainRecord *prev_rec=pbasictrain->GetTrainRecordByRecIndex(i-1);
				CTG_TrainRecord *next_rec=pbasictrain->GetTrainRecordByRecIndex(i+1);
				
				ULONG flag=0; // 1:ʼ�� 2:�յ� 3:ͨ�� 4:����
				if(i==0 && prec->IsStartRec())
					flag=1;
				else if(i+1==pbasictrain->GetTrainRecordCount() && prec->IsTerminalRec())
					flag=2;
				else if(prec->IsPassRec())
					flag=3;
				else
					flag=4;
				int n=0;
				for(n=0; n<m_st_basic_info_count; n++)
				{
					if(st_basic_info[n].station==prec->m_data.station)
					{
						int cnt=st_basic_info[n].count;
						if(cnt<2047)
						{
							st_basic_info[n].info[cnt].flag=flag;
							strncpy(st_basic_info[n].info[cnt].ar_trainid, prec->m_data.arrive_train_id, 9);
							strncpy(st_basic_info[n].info[cnt].de_trainid, prec->m_data.depart_train_id, 9);
							st_basic_info[n].info[cnt].ar_side= prec->m_data.arrive_side;
							st_basic_info[n].info[cnt].de_side= prec->m_data.depart_side;

							if(NULL != prev_rec)
								st_basic_info[n].info[cnt].prev_station=prev_rec->m_data.station;
							else
								st_basic_info[n].info[cnt].prev_station=0;

							if(NULL != next_rec)
								st_basic_info[n].info[cnt].next_station=next_rec->m_data.station;
							else
								st_basic_info[n].info[cnt].next_station=0;
							
							st_basic_info[n].count++;
						}
						break;
					}
				}
				if(n>=m_st_basic_info_count&&n<MAX_STATION_PER_SECTION)
				{
					st_basic_info[n].station=prec->m_data.station;
					st_basic_info[n].count=1;
					st_basic_info[n].info[0].flag=flag;
					strncpy(st_basic_info[n].info[0].ar_trainid, prec->m_data.arrive_train_id, 9);
					strncpy(st_basic_info[n].info[0].de_trainid, prec->m_data.depart_train_id, 9);
					st_basic_info[n].info[0].ar_side= prec->m_data.arrive_side;
					st_basic_info[n].info[0].de_side= prec->m_data.depart_side;

					if(NULL != prev_rec)
						st_basic_info[n].info[0].prev_station=prev_rec->m_data.station;
					else
						st_basic_info[n].info[0].prev_station=0;

					if(NULL != next_rec)
						st_basic_info[n].info[0].next_station=next_rec->m_data.station;
					else
						st_basic_info[n].info[0].next_station=0;

					m_st_basic_info_count++;
				}
			}
		}
	}
}

int CSendStationPlanDlgEx::CheckStationSideFull(CTG_TrainScheduleBase *pdata)
{
	if(GetTGFunctionConfig()->IsCheckSideFull())
	{
		GetSideFullCheck()->Init();

		CTG_TrainPtrGroup group;
		pdata->FindTrains(group,NULL_STATION,start_time,end_time);
		for(int j=0;j<group.GetTrainCount();j++)
		{
			CTG_SingleTrain *ptrain=group.GetTrainPtr(j);
			if(NULL==ptrain)
				continue;
		
			for ( int i = 0; i < ptrain->GetTrainRecordCount(); i++ )
			{
				CTG_TrainRecord  *rec = ptrain->GetTrainRecord(i);
				if(NULL == rec)
					continue;

				if (rec->IfInTimeScope(start_time,end_time))
				{
					GetSideFullCheck()->AddStationSideOccupy(rec->GetStationNo(), rec->m_data.arrive_side,
						rec->GetArriveTime(), rec->GetDepartTime());
				}
			}
		}
		
		CStringArray list;
		GetSideFullCheck()->GetStationSideOccupyFull(list);
		if(list.GetCount()>0)
		{
			for(int b=0; b<list.GetCount(); b++)
			{
				ShowStatus(40, list.GetAt(b));
			}
		}
			
		if(m_pStplanGrid)
		{
			m_pStplanGrid->DisplayWarnInfo();
			if(m_pStplanGrid->HaveStplanWarnInfo())
			{	
				return -1;
			}
		}
	}

	return 0;
}


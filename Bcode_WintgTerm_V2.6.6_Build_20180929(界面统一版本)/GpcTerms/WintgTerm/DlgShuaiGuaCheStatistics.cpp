// DlgShuaiGuaCheStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "wintgdoc.h"
#include "DlgShuaiGuaCheStatistics.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



_TCHAR* g_sgc_Headtitle[] = {_T("����"),_T("����"),_T("�س�"),_T("������;")};
_TCHAR* g_sgc_Reartitle[] = {_T("����"),_T("����"),_T("�س�"),_T("����"),_T("����"),_T("�س�"),_T("����")};

/*_T("̲ͷ��"),_T("���Ҵ�"),
		_T("��ͷ��"),_T("����"),_T("��Ϫ"),_T("������"),_T("��ͷ"),_T("��ɳ��"),_T("����"),
		_T("����"),_T("����ƺ"),_T("ȫ��"),_T("����"),_T("������"),_T("��ˮ"),_T("��ˮ")
	};
*/

int g_sgc_HeadtitleColWidth[] = {80,40,40,40};
int g_sgc_ReartitleColWidth[] = {80,40,40,80,40,40,100};



/////////////////////////////////////////////////////////////////////////////
// CDlgShuaiGuaCheStatistics dialog


CDlgShuaiGuaCheStatistics::CDlgShuaiGuaCheStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgShuaiGuaCheStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgShuaiGuaCheStatistics)
	//}}AFX_DATA_INIT

	m_pGridCtrl = NULL;
	memset(&m_scInfo,0x00,sizeof(SCROLLINFO));
	
	m_CurMinCol = 0;
	m_xoffset = 1;

	m_HeadColNum = sizeof(g_sgc_Headtitle)/sizeof(_TCHAR*);
	m_RearColNum = sizeof(g_sgc_Reartitle)/sizeof(_TCHAR*);

	m_SingleStationColWidth = 60;
	m_StationNum = 0;
	
}

CDlgShuaiGuaCheStatistics::~CDlgShuaiGuaCheStatistics()
{
	if(m_pGridCtrl != NULL)
		delete m_pGridCtrl;
	

}

void CDlgShuaiGuaCheStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgShuaiGuaCheStatistics)
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgShuaiGuaCheStatistics, CDialog)
	//{{AFX_MSG_MAP(CDlgShuaiGuaCheStatistics)
	ON_WM_PAINT()
	ON_MESSAGE(UM_SHUAIGUACHE_STATISTICS,OnHScrollStatisticsContent)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_WM_CLOSE()
	ON_COMMAND(ID_STATSTICS_REMOTELOAD, OnStatsticsSgcRemoteload)
	ON_MESSAGE(UM_TGMSG_ADDTRAIN,OnSchdAddTrain)
	ON_MESSAGE(UM_TGMSG_SCHDSTATUS,OnSchdStatus)
	ON_BN_CLICKED(IDC_STATISTICS_LOADSCHEDULE, OnStatisticsLoadschedule)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_STATISTICS_SECTION,ID_STATISTICS_SECTION_END,OnStatisticsSection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgShuaiGuaCheStatistics message handlers

BOOL CDlgShuaiGuaCheStatistics::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//����ʱ�䷶Χ
	
	if(g_StatisticsSecInfo == NULL)
	{
		g_StatisticsSecInfo = new CStatisticsSectionInfo;
		g_StatisticsSecInfo->LoadProfile();
	}
	m_datastore.LoadSectionInfo();


	//����������Ϣ
	//LoadStatisticsSectionInfo();
	//m_AllColNum = m_HeadColNum + m_RearColNum + m_StationNum;

	SetDefaultVal();
	SetGridColWidth();
	

	CWnd *pWnd;
	CRect pWndrect;
	//����Grid�ؼ�
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CMyGridCtrl;
		if (!m_pGridCtrl) 
			return FALSE;	
		
	}	
		
	//������������
	try
	{
		m_pGridCtrl->DeleteAllItems();
		pWnd = GetDlgItem(IDC_STATIC_SGCCONTENT1);
		pWnd->GetWindowRect(pWndrect);
		ScreenToClient(pWndrect); 
		m_pGridCtrl->Create(pWndrect, this, IDC_STATIC_SGCCONTENT1);
		
		m_pGridCtrl->SetRowCount(20);
		m_pGridCtrl->SetColumnCount(m_GridColNum);
		m_pGridCtrl->SetEditable(FALSE);

		for(int i=0; i<m_GridColNum; i++)
		{
			m_pGridCtrl->SetColumnWidth(i,m_GridColWidth[i]);
		}
			
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	
	
	pWnd = GetDlgItem(IDC_STATIC_TITLE1);
	CDC* pDC = pWnd->GetDC();

	if(m_MemDC.CreateCompatibleDC(pDC) == 0)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: shuai gua che,create  memory DC error");
		return FALSE;
	}
	
	CRect rect;
	pWnd->GetClientRect(&rect);
	m_MemBitmapWidth = m_pGridCtrl->GetVirtualWidth();
	m_MemBitmapHeight = rect.Height();

	if(m_MemBitmap.CreateCompatibleBitmap(pDC,m_MemBitmapWidth,m_MemBitmapHeight) == 0)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: shuai gua che, create memory	 bitmap error");
		return FALSE;
	}
			
	m_pOldBitmap = m_MemDC.SelectObject(&m_MemBitmap);
	pWnd->ReleaseDC(pDC);
	


	DrawMemTitle(&m_MemDC);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgShuaiGuaCheStatistics::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	if(m_pGridCtrl == NULL || m_pGridCtrl->GetSafeHwnd() == NULL)
		return;

	

	CWnd* pwnd = GetDlgItem(IDC_STATIC_TITLE1);
	CRect rect;
	pwnd->GetClientRect(&rect);
	CDC* dc2 = pwnd->GetDC();		
	pwnd->Invalidate();
	pwnd->UpdateWindow();
	
	int sourcex,sourcey;
	sourcex = ComputeSourceX();
	sourcey = 0;
	
	dc2->BitBlt(0,0,rect.Width(),rect.Height(),
			&m_MemDC,sourcex,sourcey,SRCCOPY);
	/*
	if(m_scInfo.nPos == 0)
	{
		sourcex = 0;
		sourcey = 0;
		dc2->BitBlt(0,0,rect.Width(),rect.Height(),
				&m_MemDC,0,0,SRCCOPY);

	}
	else if((m_scInfo.nPos+m_scInfo.nPage) >= m_scInfo.nMax)
	{
		//����cgridctrl���µĲ��ֿհף�grid�׼���������ʹ��߿��Ӧһ��cell����ʼλ��
		int colNo = 0;
		int colwidth = m_MemBitmapWidth/m_columns;
		while(colNo*colwidth<rect.Width())
		{
			colNo++;
		}

	

		sourcex = (m_columns-colNo-1)*colwidth;
		sourcey = 0;
		dc2->BitBlt(0,0,rect.Width(),rect.Height(),
				&m_MemDC,sourcex,sourcey,SRCCOPY);		
	}
	else
	{
		sourcex = m_MemBitmapWidth*m_scInfo.nPos/m_scInfo.nMax;
		sourcey = 0;
		dc2->BitBlt(0,0,rect.Width(),rect.Height(),
				&m_MemDC,sourcex,sourcey,SRCCOPY);
	}
	*/		
	pwnd->ReleaseDC(dc2);
	//TRACE("OnPaint:%d %d %d \n",m_scInfo.nMax,m_scInfo.nMin,m_scInfo.nPos);
	

	// Do not call CDialog::OnPaint() for painting messages
}

LRESULT CDlgShuaiGuaCheStatistics::OnHScrollStatisticsContent(WPARAM minColNum,LPARAM scrollinfo)
{
	memcpy((void*)&m_scInfo,(void*)scrollinfo,sizeof(SCROLLINFO));
	
	//TRACE("OnHscroll:%d %d %d \n",m_scInfo.nMax,m_scInfo.nMin,m_scInfo.nPos);
	CRect rect;
	GetDlgItem(IDC_STATIC_TITLE1)->GetWindowRect(&rect);

	ScreenToClient(&rect);
	m_CurMinCol = minColNum;
	InvalidateRect(&rect);
	return TRUE;

}

/*************************************************************************
*Purpose:���ڴ�dc�л��Ʊ��������Ա���grid��ˮƽ����������ʱ����Կ��ٿ�����λͼ��ָ��λ��
*
*Input:CDC *pDC
*		CDC* pDC: ����ͼ���CDC
*
*Output:
*
*Return: TRUE/FALSE
*
*************************************************************************/
BOOL CDlgShuaiGuaCheStatistics::DrawMemTitle(CDC *pDC)
{
	//��������ε�ɫ�óɰ�ɫ
	CRect rect(0,0,m_MemBitmapWidth,m_MemBitmapHeight);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->Rectangle(&rect);
	
	//���Ƶ�һ������
	pDC->MoveTo(0,0);
	pDC->LineTo(0,m_MemBitmapHeight);

	
	CRect tempRect;
	tempRect.top = 0;
	tempRect.left = 0;
	tempRect.right = 0;
	tempRect.bottom = m_MemBitmapHeight;

	int i;
	int curWidthSum = 0;
	CString tempStr;

	//���Ʊ�������վǰ��ı���
	//for(i=0; i<m_HeadColNum+m_StationNum+m_RearColNum; i++)
	for(i=0; i<m_TitleColNum; i++)
	{
		curWidthSum += m_TitleColWidth[i];

		pDC->MoveTo(curWidthSum,0);
		pDC->LineTo(curWidthSum,m_MemBitmapHeight);

		tempRect.left = tempRect.right;
		tempRect.right = tempRect.left + m_TitleColWidth[i];

		if(i>=0 && i<m_HeadColNum)
		{
			tempStr = g_sgc_Headtitle[i];
			DrawHeadRearTitle(pDC,tempStr,tempRect);
		}
		else if(i>=(m_HeadColNum+m_StationNum))
		{
			tempStr = g_sgc_Reartitle[i-m_HeadColNum-m_StationNum];
			DrawHeadRearTitle(pDC,tempStr,tempRect);
		}
		else
		{
			tempStr = m_StationColName[i-m_HeadColNum];
			DrawStationNameTitle(pDC,tempStr,tempRect);	
		}

	}

	return TRUE;
}
/*************************************************************************
*Purpose:�ڱ�������ͷβ������
*
*Input:CDC *pDC, CString DrawString, CRect DrawRect
*		CDC* pDC: ����ͼ���CDC
*		CString DrawString:Ҫ���Ƶ��ַ�������վ����
*		CRect DrawRect:���Ƶ�һ����Χ
*
*Output:
*
*Return: TRUE/FALSE
*
*************************************************************************/
BOOL CDlgShuaiGuaCheStatistics::DrawHeadRearTitle(CDC *pDC, CString DrawString, CRect DrawRect)
{
	int ChineseCharNum;
	ChineseCharNum = (DrawString.GetLength()/2 > 4)?4:DrawString.GetLength()/2;

	if(ChineseCharNum<=0)
		return TRUE;

	
	int rectHeight = DrawRect.Height()/4;

	DrawRect.InflateRect(0,-5,0,-5);
	
	CRect tempRect1,tempRect2,tempRect3,tempRect4;
	tempRect1 = tempRect2 = tempRect3 = tempRect4 = DrawRect;
	
	tempRect1.bottom = tempRect1.top+rectHeight;
	
	tempRect2.top = tempRect1.bottom;
	tempRect2.bottom = tempRect2.top+rectHeight;

	tempRect3.top = tempRect2.bottom;
	tempRect3.bottom = tempRect3.top+rectHeight;

	tempRect4.top = tempRect3.bottom;
	tempRect4.bottom = tempRect4.top+rectHeight;
	


	CString Char1,Char2,Char3,Char4;

	

	switch(ChineseCharNum)
	{
	case 1:
		Char1 = DrawString.Mid(0,2);
		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	case 2:
		Char1 = DrawString.Mid(0,2);
		Char4 = DrawString.Mid(2,2);
		
		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		tempRect4.top = tempRect4.bottom - rectHeight;
		pDC->DrawText(Char4,&tempRect4,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	case 3:
		Char1 = DrawString.Mid(0,2);
		Char2 = DrawString.Mid(2,2);
		Char4 = DrawString.Mid(4,2);

		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		tempRect2.bottom = tempRect3.bottom;
		pDC->DrawText(Char2,&tempRect2,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char4,&tempRect4,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;

	case 4:	
		Char1 = DrawString.Mid(0,2);
		Char2 = DrawString.Mid(2,2);
		Char3 = DrawString.Mid(4,2);
		Char4 = DrawString.Mid(6,2);
		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char2,&tempRect2,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char3,&tempRect3,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char4,&tempRect4,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	return TRUE;

}
/*************************************************************************
*Purpose:�ڱ������л滭��վ�����ǲ���
*
*Input:CDC *pDC, CString DrawString, CRect DrawRect
*		CDC* pDC: ����ͼ���CDC
*		CString DrawString:Ҫ���Ƶ��ַ�������վ����
*		CRect DrawRect:���Ƶ�һ����Χ
*
*Output:
*
*Return: TRUE/FALSE
*
*************************************************************************/
BOOL CDlgShuaiGuaCheStatistics::DrawStationNameTitle(CDC *pDC, CString DrawString, CRect DrawRect)
{
	int ChineseCharNum;
	ChineseCharNum = (DrawString.GetLength()/2 > 3)?3:DrawString.GetLength()/2;

	if(ChineseCharNum<=0)
		return TRUE;
	
	int rectHeight = DrawRect.Height()/4;

	DrawRect.InflateRect(0,-5,0,-5);
	
	CRect tempRect1,tempRect2,tempRect3,tempRect4Left,tempRect4Right;
	tempRect1 = tempRect2 = tempRect3 = tempRect4Left = tempRect4Right = DrawRect;
	
	tempRect1.bottom = tempRect1.top+rectHeight;
	
	tempRect2.top = tempRect1.bottom;
	tempRect2.bottom = tempRect2.top+rectHeight;

	tempRect3.top = tempRect2.bottom;
	tempRect3.bottom = tempRect3.top+rectHeight;

	tempRect4Left.top = tempRect3.bottom;
	tempRect4Left.right = tempRect4Left.left + DrawRect.Width()/2;

	tempRect4Right.top = tempRect3.bottom;
	tempRect4Right.left = tempRect4Left.right;
	


	CString Char1,Char2,Char3,Char4Left,Char4Right;
	Char4Left = "˦";
	Char4Right = "��";

	switch(ChineseCharNum)
	{
	case 1:
		Char1 = DrawString.Mid(0,2);
		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	case 2:
		Char1 = DrawString.Mid(0,2);
		Char3 = DrawString.Mid(2,2);
		
		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char3,&tempRect3,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	case 3:
		Char1 = DrawString.Mid(0,2);
		Char2 = DrawString.Mid(2,2);
		Char3 = DrawString.Mid(4,2);

		pDC->DrawText(Char1,&tempRect1,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char2,&tempRect2,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		pDC->DrawText(Char3,&tempRect3,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	
	}
	pDC->DrawText(Char4Left,&tempRect4Left,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	pDC->DrawText(Char4Right,&tempRect4Right,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

	CPoint pStart,pEnd;
	pStart.x = tempRect4Left.left;
	pStart.y = tempRect4Left.top;
	pEnd.x = tempRect4Right.right;
	pEnd.y = pStart.y;

	pDC->MoveTo(pStart);
	pDC->LineTo(pEnd);

	CPen dash,*poldPen;
	dash.CreatePen(PS_DASH,1,CLR_BLACK);
	poldPen = pDC->SelectObject(&dash);

	pStart.x = tempRect4Left.right;
	pEnd.x = pStart.x;
	pEnd.y = tempRect4Left.bottom;

	pDC->MoveTo(pStart);
	pDC->LineTo(pEnd);

	pDC->SelectObject(poldPen);


	return TRUE;

}


/*************************************************************************
*Purpose:����grid�ؼ���ˮƽ������������ʱ��Ҫ������λͼ����ʼλ��
*
*Input:
*
*Output:
*
*Return: int Ҫ������λͼ����ʼλ��
*
*************************************************************************/
int CDlgShuaiGuaCheStatistics::ComputeSourceX()
{
	int ret = 0;
	for(int i=0; i<m_CurMinCol; i++)
	{
		ret += m_GridColWidth[i];
	}
	return ret;

}

/*************************************************************************
*Purpose:����Grid �ؼ���ÿ���еĿ�Ⱥ��е����������ñ�������ÿ���еĿ�Ⱥ��е�����
*			�������е������Ϳռ��е������Ĳ���ڳ�վ������
*
*Input:
*
*Output:
*
*Return: TRUE/FALSE 
*
*************************************************************************/
BOOL CDlgShuaiGuaCheStatistics::SetGridColWidth()
{
	int i;
	m_TitleColNum = 0;
	m_GridColNum = 0;
	for(i=0; i<m_HeadColNum; i++)
	{
		m_TitleColWidth[m_TitleColNum++] = g_sgc_HeadtitleColWidth[i];
		m_GridColWidth[m_GridColNum++] = g_sgc_HeadtitleColWidth[i];
	}

	for(i=0; i<m_StationNum; i++)
	{
		m_TitleColWidth[m_TitleColNum++] = m_SingleStationColWidth;
		
		m_GridColWidth[m_GridColNum++] = m_SingleStationColWidth/2;
		m_GridColWidth[m_GridColNum++] = m_SingleStationColWidth/2;
	}

	for(i=0; i<m_RearColNum; i++)
	{
		m_TitleColWidth[m_TitleColNum++] = g_sgc_ReartitleColWidth[i];
		m_GridColWidth[m_GridColNum++] = g_sgc_ReartitleColWidth[i];
	}
	return TRUE;
}

/*************************************************************************
*Purpose:�ڳ�ʼ����ʱ������ȱʡ�����䣬ȱʡʱ��
*
*Input:
*
*Output:
*
*Return: TRUE/FALSE 
*
*************************************************************************/
BOOL CDlgShuaiGuaCheStatistics::SetDefaultVal()
{
	
	m_CurStatisticsSection = g_StatisticsSecInfo->GetSectionByIndex(0);
	if(m_CurStatisticsSection==NULL)
		return FALSE;
	
	m_StationNum = m_CurStatisticsSection->nStationNum;

	m_CurStatisticsSectionNo = m_CurStatisticsSection->nSectionNo;
	m_curShiftDataStore = NULL;

	

	int i,j;
	for(j=0; j<m_StationNum; j++)
	{
		strcpy(m_StationColName[j],gpcdef.GetStationName(m_CurStatisticsSection->nStationList[j]));
	}

	//add dyn menu
	CMenu* pSectionMenu = NULL;
 	CMenu* pTopMenu = this->GetMenu();
	
	//���´������ݺ��Զ���������ѡ����Ч
	if(pTopMenu != NULL)
	{
		pTopMenu->EnableMenuItem(ID_STATISTICS_FLAGSEND, MF_GRAYED);
		pTopMenu->EnableMenuItem(ID_STATISTICS_DATASEND, MF_GRAYED);
		pTopMenu->EnableMenuItem(ID_STATISTICS_LOAD_ALL_SEC, MF_GRAYED);		
	}

 	int nSectionNum,nSectionList[30];
	StatisticsSection* psList[30];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	g_StatisticsSecInfo->GetSectionList(nSectionNum,psList);

	    
	int iPos;
 
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu *pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_STATISTICS_SECTION)
		{
			pSectionMenu = pMenu;
			break;
		}
	}
	ASSERT(pSectionMenu != NULL);

	for (iPos = pSectionMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
		pSectionMenu->DeleteMenu(iPos, MF_BYPOSITION);

	for (i = 0; i < nSectionNum; i++)//int
	{
 		CString strSection = psList[i]->sSectionName; //gpcdef.GetSectionName(nSectionList[i]);
		pSectionMenu->AppendMenu(MF_STRING | MF_ENABLED,
			ID_STATISTICS_SECTION + i, strSection);
	}
	
	pSectionMenu->CheckMenuItem(ID_STATISTICS_SECTION + 0, MF_CHECKED);

	return TRUE;
}



void CDlgShuaiGuaCheStatistics::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	
	SendMessage(WM_CLOSE);
}

void CDlgShuaiGuaCheStatistics::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_MemDC.DeleteDC();
	m_MemBitmap.DeleteObject();
/*
	for(int i=0; i<m_StatisticsSectionNum; i++)
		free(m_StatisticsSection[i]);
	m_StatisticsSectionNum = 0;
*/
	//m_datastore.ClearAll();

	m_pGridCtrl->DestroyWindow();
	DestroyWindow();
	CDialog::OnClose();
}

void CDlgShuaiGuaCheStatistics::OnStatsticsSgcRemoteload() 
{
	// TODO: Add your command handler code here
	
	//��ȡ��ǰѡ�е�����
	StatisticsSection* curSection = m_CurStatisticsSection ;
	long shiftid = gpcdef.GetLoginShiftid();
	long shift_date,shift_no;

	CHistorySchdDlg *dlg = new CHistorySchdDlg();
	if(dlg->DoModal() == IDOK){
			shiftid = gpcdef.GetHistoryShiftid();
			shift_date = (shiftid/1000000 - 1990) * 380 +
			(shiftid/10000-(shiftid/1000000)*100 - 1) * 31 + 
			shiftid/100 - (shiftid/10000)*100  - 1 ;
		shift_no = shiftid - shiftid/100*100;
	}
	else
		return;
/*
	int dispNum;
	int dispList[5];
	long shiftList[5];
	dispNum = curSection->nDispatcherNum;
	for(int i=0; i<dispNum; i++)
	{
		dispList[i] = curSection->dispInfo[i].nDispatcherNo;
		shiftList[i] = shift_date;		
	}

	m_datastore.SetLoadSchedule(dispNum,dispList,shiftList);
	m_datastore.LoadDataBegin();
*/
	m_datastore.SetLoadSectionList(1,&curSection->nSectionNo,shift_date);
	m_datastore.LoadSettedSectionData();
}

LRESULT CDlgShuaiGuaCheStatistics::OnSchdAddTrain(WPARAM wParam,LPARAM lParam)
{
	m_datastore.OnSchdAddTrain(wParam+1,lParam);
     return TRUE;
}

LRESULT CDlgShuaiGuaCheStatistics::OnSchdStatus(WPARAM wParam,LPARAM lParam)
{
	m_datastore.OnSchdStatus(wParam,lParam);
    return TRUE;
}


void CDlgShuaiGuaCheStatistics::OnStatisticsLoadschedule() 
{
	// TODO: Add your control notification handler code here

	
	
	int nstationNum = 32;
	int nstationList[32];
	
	g_StatisticsSecInfo->GetIntersectStation(m_CurStatisticsSection->nSectionNo,nstationNum,nstationList);
	
	//m_datastore.MergeBlock(nstationNum,nstationList);
	m_datastore.MergeAllBlock();
	
	CShiftDataPerDispStation* shiftdata;
	shiftdata = m_datastore.GetShiftDataByIndex(0);
	if(shiftdata == NULL)
		return;
	shiftdata->SetFlagAll();
	//m_datastore.FilterBySubSection(m_CurStatisticsSection);
	ShowRecordInGrid();
/*
	CSingleTrain4Statistics* single;
	Pstruct TRAINRECORD* rec;
	
	int i,j,m;
	for(i=0;  i<m_datastore.GetShiftDataNum();i++)
	{
		m_curShiftDataStore = m_datastore.GetShiftDataByIndex(i);
		m = m_curShiftDataStore->GetTrainNum();
		TRACE("Train num = %d \n",m);
		for(j=0; j<m; j++)
		{
			single = m_curShiftDataStore->GetSingleTrain(j);
			if(!strcmp(single->GetTrainName(),"111"))
			{
				for(int k=0; k<single->m_nTrainInfo.station_text_count; k++)
				{
					int recindex = single->m_nTrainInfo.station_text_id[k];
					TRACE("%s %s,",gpcdef.GetStationName(single->GetIndexRecord(recindex)->station),single->m_nTrainInfo.station_text[k]);
				}
				Sleep(0);
			}
			//TRACE("%d :%s  %d \n ",j,single->GetTrainName(),single->GetRecordNum());
		}
		
	}
	*/
}
void  CDlgShuaiGuaCheStatistics::OnStatisticsSection(UINT message) 
{
	// TODO: Add your command handler code here
	
	if(m_pGridCtrl!= NULL)
	{
		m_pGridCtrl->SetRedraw(FALSE);
		for(int i=0; i<m_pGridCtrl->GetRowCount(); i++)
		{
			for(int j=0; j<m_pGridCtrl->GetColumnCount();j++)
			{
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = i;
				Item.col = j;
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.strText = "";
				m_pGridCtrl->SetItem(&Item);

			}
		}
		m_pGridCtrl->SetRowCount(20);
		m_pGridCtrl->SetRedraw(TRUE);
	}

	int iSel = (int)message;
 
	int iSelIndex = iSel - ID_STATISTICS_SECTION;
		
	long iPos;
	CMenu* pSectionMenu = NULL;
	CMenu* pTopMenu = this->GetMenu();
	
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu *pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_STATISTICS_SECTION)
		{
			pSectionMenu = pMenu;
			break;
		}
	}
	ASSERT(pSectionMenu != NULL);	
	
	for(iPos = pSectionMenu->GetMenuItemCount() - 1; iPos >=0;iPos --)
	{
		pSectionMenu->CheckMenuItem(ID_STATISTICS_SECTION + iPos, MF_UNCHECKED);
	}
	
	pSectionMenu->CheckMenuItem(ID_STATISTICS_SECTION + iSelIndex, MF_CHECKED);
	
	m_CurStatisticsSection = g_StatisticsSecInfo->GetSectionByIndex(iSelIndex);

	CString title;
	title = m_CurStatisticsSection->sSectionName;
	title = title + "�������˦�ұ�";
	SetWindowText(title);

	
	if(m_CurStatisticsSection==NULL)
		return   ;
	
	m_StationNum = m_CurStatisticsSection->nStationNum;
	m_CurStatisticsSectionNo = m_CurStatisticsSection->nSectionNo;

	int j;
	for(j=0; j<m_StationNum; j++)
	{
		strcpy(m_StationColName[j],gpcdef.GetStationName(m_CurStatisticsSection->nStationList[j]));
	}
	SetGridColWidth();
	m_pGridCtrl->SetColumnCount(m_GridColNum);
	for(j=0; j<m_GridColNum; j++)
	{
		m_pGridCtrl->SetColumnWidth(j,m_GridColWidth[j]);
	}

	m_MemDC.SelectObject(m_pOldBitmap);
	m_MemBitmap.DeleteObject();

	m_MemBitmapWidth = m_pGridCtrl->GetVirtualWidth();
	if(m_MemBitmap.CreateCompatibleBitmap(&m_MemDC,m_MemBitmapWidth,m_MemBitmapHeight) == 0)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: shuai gua che, create memory	 bitmap error");
		return  ;
	}			
	m_pOldBitmap = m_MemDC.SelectObject(&m_MemBitmap);
		
	
	DrawMemTitle(&m_MemDC);
	OnPaint();
    return  ;
	
}
/*************************************************************************
*Purpose:��Grid�ؼ�����ʾ��ѯ����������
*
*Input:
*
*Output:
*
*Return:
*
*************************************************************************/
void CDlgShuaiGuaCheStatistics::ShowRecordInGrid()
{

	if(m_pGridCtrl == NULL)
		return;

	

	CShiftDataPerDispStation* shiftdata;
	shiftdata = m_datastore.GetShiftDataByIndex(0);

	if(shiftdata== NULL)
		return;

	int trainNum=500;
	CSingleTrain4Statistics* trainSelected[500];
	shiftdata->GetDirectionSortedArray(trainNum,trainSelected);
	UpdateData(TRUE);

	

	m_pGridCtrl->SetRowCount(trainNum);
	m_pGridCtrl->SetRedraw(FALSE);

	int rows,cols;
	CString temp,temp1;
	CTime timetemp;
//	long nDelta;
//	int nHour,nMinute;

	CSingleTrain4Statistics* single;
//	Pstruct TRAINRECORD *recstart,*recend;
//	char reversedName[20];

	int records_displayed = 0;
	int shuaigua[4];
	for(rows=0; rows<trainNum; rows++)
	{	
		//single = shiftdata->GetSingleTrain(rows);
		single = trainSelected[rows];
		if(!single->GetStatisticsFlag())
			continue;
		if(!single->HasShuaiGuaInfo(m_CurStatisticsSection->nStationNum,m_CurStatisticsSection->nStationList,shiftdata->nShiftidEnd))
			continue;
		
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = records_displayed;
		Item.col = 0;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			
		temp.Format("%s",single->GetTrainName());
			
		Item.strText = temp;
		m_pGridCtrl->SetItem(&Item);

		for(cols=0; cols < m_StationNum; cols++)
		{ 
			if(!single->GetShuaiGuaInfo(m_CurStatisticsSection->nStationList[cols],shuaigua,shiftdata->nShiftidEnd))
				continue;
			GV_ITEM Item1,Item2;
			Item1.mask = GVIF_TEXT|GVIF_FORMAT;
			Item1.row = records_displayed;
			Item1.col = m_HeadColNum+cols*2;
			Item1.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			
			temp.Format("%d",shuaigua[2]+shuaigua[3]);
			
			Item1.strText = temp;
			m_pGridCtrl->SetItem(&Item1);

			Item2.mask = GVIF_TEXT|GVIF_FORMAT;
			Item2.row = records_displayed;
			Item2.col = m_HeadColNum+cols*2+1;
			Item2.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			
			temp.Format("%d",shuaigua[0]+shuaigua[1]);
			
			Item2.strText = temp;
			m_pGridCtrl->SetItem(&Item2);

		}
		
		GV_ITEM ItemRear;
		ItemRear.mask = GVIF_TEXT|GVIF_FORMAT;
		ItemRear.row = records_displayed;
		ItemRear.col = m_HeadColNum+m_StationNum*2;
		ItemRear.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			
		temp.Format("%s",single->GetTrainName());
			
		ItemRear.strText = temp;
		m_pGridCtrl->SetItem(&ItemRear);

		records_displayed++;
		
	}

	m_pGridCtrl->SetRowCount(records_displayed+1);
	m_pGridCtrl->SetRedraw(TRUE);
}

void CDlgShuaiGuaCheStatistics::OnSetFocus(CWnd* pNewWnd) 
{
	CDialog::OnSetFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	AfxMessageBox("focus");
	
}

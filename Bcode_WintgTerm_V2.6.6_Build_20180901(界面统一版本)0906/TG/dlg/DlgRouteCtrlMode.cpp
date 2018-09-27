// DlgRouteCtrlMode.cpp : implementation file
//

#include "stdafx.h"
#include "DlgRouteCtrlMode.h"
#include ".\dlgroutectrlmode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRouteCtrlMode dialog


CDlgRouteCtrlMode::CDlgRouteCtrlMode(CTGDoc *pDoc,int nStNo,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRouteCtrlMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRouteCtrlMode)
		// NOTE: the ClassWizard will add member initialization here
	nStationNo = nStNo;
	m_pDoc = pDoc;
	//}}AFX_DATA_INIT
}


void CDlgRouteCtrlMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRouteCtrlMode)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRouteCtrlMode, CDialog)
	//{{AFX_MSG_MAP(CDlgRouteCtrlMode)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRouteCtrlMode message handlers

BOOL CDlgRouteCtrlMode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	BOOL bFound = FALSE;
	
	for (int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
	{
		int nStation = m_pDoc->m_sDispDistance[i].station_number;
		if(nStationNo == nStation)
		{
			bFound = TRUE;
			break;
		}
	}
	if(bFound == FALSE) return FALSE;
	
	CWnd *pWnd = GetDlgItem(IDC_STATION);
	pWnd->SetWindowText(m_pDoc->GetStationName(nStationNo));
	
	CString strStatus;
	if(m_pDoc->m_sDispDistance[i].usLircStatus != 0)
	{
       //strStatus ="վ��";
       strStatus ="�복վ�Ͽ�";
	}
	else
	{
      if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 0)    //��ͼ��·
		  strStatus ="��ͼ��·";
	   else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 1) //�洢����
		  strStatus ="�洢����";
	   else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 2) //�˹���·
			 strStatus ="�˹���·";
	   else
             strStatus ="δ֪״̬";
	 }
	
	 pWnd = GetDlgItem(IDC_STATUS);
	 pWnd->SetWindowText(strStatus);
	 

	 strStatus.Empty();
	 pWnd = GetDlgItem(IDC_MODE_STATUS);
	 if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 0)       //��ɢ���� 
		 //nOldColor = pDC->SetTextColor(CLR_LAWNGREEN);
		 strStatus = "��ɢ���ɿ���";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 1)  //���Ŀ��� 
		 //nOldColor = pDC->SetTextColor(CLR_YELLOW);
		 strStatus = "�������ȿ���";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 2)  //��վ����
		 //nOldColor = pDC->SetTextColor(CLR_BLUE);
		 strStatus = "��վ���ȿ���";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 3)  //�ǳ�����
		 //nOldColor = pDC->SetTextColor(CLR_DARKGRAY);
		 strStatus = "�ǳ�վ��";
	 else
		 //nOldColor = pDC->SetTextColor(CLR_DARKGRAY);
		 strStatus = "�복վ�Ͽ�";
	 pWnd->SetWindowText(strStatus);
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgRouteCtrlMode::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	// TODO:  �ڴ˸��� DC ���κ�����
    DISPDISTANCE m_pDoc->m_sDispDistance[128];
	BOOL bFound = FALSE;
	m_sDispDistance = m_pDoc->m_sDispDistance;
	for (int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
	{
		int nStation = m_pDoc->m_sDispDistance[i].station_number;
		if(nStationNo == nStation)
		{
			bFound = TRUE;
			break;
		}
	}
	if(bFound == FALSE) return hbr;

	COLORREF col;
    if(GetDlgItem(IDC_MODE_STATUS)->m_hWnd==pWnd->m_hWnd )
	{
       if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 0)       //��ɢ���� 
		  col = CLR_LAWNGREEN;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 1)  //���Ŀ��� 
		  col =  CLR_YELLOW;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 2)  //��վ����
		  col = CLR_BLUE;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 3)  //�ǳ�����
		  col = CLR_GRAY;
	   else
		  col = CLR_RED ;

		pDC->SetBkColor(col);


	}
	*/
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

// DlgStatisticsTrain.cpp : implementation file
//

#include "stdafx.h"
#include "wintgdoc.h"
#include "DlgStatisticsTrain.h"
#include "usbclientdll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStatisticsTrain dialog


CDlgStatisticsTrain::CDlgStatisticsTrain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStatisticsTrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStatisticsTrain)
	//}}AFX_DATA_INIT
}


void CDlgStatisticsTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStatisticsTrain)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStatisticsTrain, CDialog)
	//{{AFX_MSG_MAP(CDlgStatisticsTrain)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_DATASEND, OnButtonDatasend)
	ON_MESSAGE(UM_TGMSG_ADDTRAIN,OnSchdAddTrain)
	ON_MESSAGE(UM_TGMSG_SCHDSTATUS,OnSchdStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStatisticsTrain message handlers

void CDlgStatisticsTrain::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_CLOSE);
	
}

void CDlgStatisticsTrain::OnButtonLoad() 
{
	// TODO: Add your control notification handler code here
	//��ȡ��ǰѡ�е�����
	
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

	m_shiftdate = shift_date;

	m_datastore.LoadAllSections(shift_date);
	
}


LRESULT CDlgStatisticsTrain::OnSchdAddTrain(WPARAM wParam,LPARAM lParam)
{
	m_datastore.OnSchdAddTrain(wParam+1,lParam);
	return TRUE;
}

LRESULT CDlgStatisticsTrain::OnSchdStatus(WPARAM wParam,LPARAM lParam)
{
	m_datastore.OnSchdStatus(wParam,lParam);
	return TRUE;
}

void CDlgStatisticsTrain::OnButtonDatasend() 
{
#ifdef USB_FILE_TRANS_SUPPORT
	// TODO: Add your control notification handler code here
	CString destpath  = m_datastore.m_section.GetFilePath();
	CString filename = "TJ";
	CString strtime = "";
	if(m_shiftdate<=0)
	{
		strtime = "*";
	}
	else
	{
		CTime t(long_time(m_shiftdate,0)) ;
		 strtime = t.Format("%Y%m%d");
	}
		
	filename = filename + strtime +"�ÿ��г�.txt";

	destpath +=  "\\";
	destpath += filename;

	if(!USB_WDU_INIT())
	{
		AfxMessageBox("��ʼ��USB����ʧ�ܣ�����USB�����Ƿ�����������!");
		return;
	}

	CFileFind finder;
	BOOL bWorking = finder.FindFile(destpath);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		if(!USB_SINGLEFILE_TRANSFER((char*)(LPCSTR)finder.GetFilePath()))
		{
			USB_WDU_UNINIT();
			sysprint(0,PMTG.ThreadId,(char*)(LPCSTR)(CString("�����ļ�"+finder.GetFilePath()+"ʧ��\n")));
			TRACE("�����ļ�"+finder.GetFilePath()+"ʧ��\n");
			AfxMessageBox("�����ļ�"+finder.GetFilePath()+"ʧ��!\n��ȷ��USB������һ���Ѿ���ȷ���Ӳ��ҶԷ���������ճ���������!\n����Է����ճ����Ѿ����У��볢�����������ý��ճ���!");		
			return ;
		}		
	}
	
	USB_WDU_UNINIT();
#endif
	
}

BOOL CDlgStatisticsTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_shiftdate = 0;
	m_datastore.LoadSectionInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

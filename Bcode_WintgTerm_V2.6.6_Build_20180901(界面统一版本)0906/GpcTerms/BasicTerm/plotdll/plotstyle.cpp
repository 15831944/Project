// PlotStyle.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PlotStyle.h"
#include "graphmark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotStyle dialog

#define TRAINTYPENAMENUMBER	20
_TCHAR *_gszTrainTypeName[] =
{
	_T("�ÿ��г������������ؿ��г���"), 
	_T("�ÿ��г�����������г���"),
	_T("���˻����г���ֱ���г�"),
	_T("�����г�"),
	_T("ֱͨ�����Ρ�С��ת�г�"),
	_T("�����г�"),
	_T("���޻����г�"),
	_T("ժ���г�"),
	_T("��ʱ�ÿͿ쳵"),
	_T("��ʱ�ÿ��г�"),
	_T("���͵��ÿ��г�"),
	_T("�����г����Ҳ�����˫��ǣ���򸽹��л𳵻����г�"),
	_T("����"),
	_T("�عҵ���"),
	_T("�߼�ר�м������г�"),
	_T("�����г�"),
	_T("���͵ľ����г�"),
	_T("���͹�����������г�"),
	_T("��Ԯ�г�����ѩ�г�"),
	_T("·���г����������̡����ϼ��޳����ó������Ϳ��س�������ת�г��ȣ�"),
};


CPlotStyle::CPlotStyle(CWnd* pParent /*=NULL*/)
	: CDialog(CPlotStyle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotStyle)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ack.LoadBitmaps("acku","ackd","ackf","ackl");
	m_cancel.LoadBitmaps("retrieveu","retrieved","retrievef");
	m_modify.LoadBitmaps("acceptu","acceptd","acceptf","acceptl");
}


void CPlotStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotStyle)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlotStyle, CDialog)
	//{{AFX_MSG_MAP(CPlotStyle)
	ON_BN_CLICKED(IDC_STYLE_ACK, OnStyleAck)
	ON_BN_CLICKED(IDC_STYLE_CANCEL, OnStyleCancel)
	ON_BN_CLICKED(IDC_STYLE_CHANGE, OnStyleChange)
	ON_WM_LBUTTONDOWN()
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotStyle message handlers

void CPlotStyle::OnStyleAck() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK( );
}

void CPlotStyle::OnStyleCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );
}

void CPlotStyle::OnStyleChange() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CPlotStyle::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPlotStyle::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CPlotStyle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	VERIFY(m_ack.SubclassDlgItem(IDC_STYLE_ACK,this));
	VERIFY(m_cancel.SubclassDlgItem(IDC_STYLE_CANCEL,this));
	VERIFY(m_modify.SubclassDlgItem(IDC_STYLE_CHANGE,this));

	m_ack.SizeToContent( );
	m_cancel.SizeToContent( );
	m_modify.SizeToContent( );
	// TODO: Add extra initialization here

	CWnd *pWnd = GetDlgItem(IDC_STYLE_SAMPLE);
	CRect rclRect;
	CGraphConnect *pFigure;

	pWnd->GetWindowRect(&rclRect);
	ScreenToClient(&rclRect);
	m_sample.Create("sample",0,rclRect,this,IDC_STYLE_SAMPLE + 30);

	CPoint ptStart,ptEnd;
	ptStart.x = 3;
	ptEnd.x = rclRect.Width( ) - 3;
	ptStart.y = 3;
// allocate sample 
	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDoubleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_BLUE);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDoubleLine(ptStart,ptEnd,CLR_BLUE,CLR_BLUE);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphCircleLine(ptStart,ptEnd,CLR_RED,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphADLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertDoubleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertSingleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSolidDashLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphTriangleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphTriangleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphArrowLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDashLine(ptStart,ptEnd,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectDashLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertSingleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphXSignalLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);	

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphCircleLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlotStyle::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
/*	if(nIDCtl == IDC_STYLE_CURRENT)
	{
		CWnd *pWnd = GetDlgItem(IDC_STYLE_CURRENT);
//		CDC *pCDC = pWnd->GetDC( );
		
	} else if(nIDCtl == IDC_STYLE_SAMPLE + 30){ */
		CWnd *pWnd = GetDlgItem(IDC_STYLE_SAMPLE + 30);
		CDC *pCDC = pWnd->GetDC( );
		m_sample.OnDraw(pCDC);
		pWnd->ReleaseDC(pCDC);
	
}

void CPlotStyle::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rclClipBox,rclWindow,rclSect;

	dc.GetClipBox(&rclClipBox);
	m_sample.GetWindowRect(&rclWindow);
	ScreenToClient(&rclWindow);
	if(rclSect.IntersectRect(&rclClipBox,&rclWindow))
		m_sample.OnDraw(&dc);
	// Do not call CDialog::OnPaint() for painting messages
}

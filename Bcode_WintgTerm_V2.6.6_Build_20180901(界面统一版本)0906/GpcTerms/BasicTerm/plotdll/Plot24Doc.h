// Plot24Doc.h: interface for the CPlot24Doc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOT24DOC_H__CB17D7D9_1011_4FFF_84D6_86B3613DFF49__INCLUDED_)
#define AFX_PLOT24DOC_H__CB17D7D9_1011_4FFF_84D6_86B3613DFF49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif // !defined(AFX_PLOT24DOC_H__CB17D7D9_1011_4FFF_84D6_86B3613DFF49__INCLUDED_)

#include "bureau.h"
#include "plotDoc.h"

//�߶ȺͿ�ȵ�������
double  const g_zz_total_page_width = 3600;
double  const g_zz_total_page_height = 2250;

//���;�24Сʱ��ӡ���ý���
double const g_hh_total_page_width  = 3840;
double const g_hh_total_page_height = 2400;

//����ͼ��ӡλ������

//�߶ȺͿ�ȵ�������
double  const g_basic_total_page_width = 3600;
double  const g_basic_total_page_height = 2050;

/////////////////////////////////////////////////////////////////////////////
// CPlot24Doc document
struct CDdyName
{
	int  nConId;
	long nShiftId;
	CString name1;
	CString name2;
};

class CPlot24Doc : public CPlotDoc
{
protected:
	CPlot24Doc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPlot24Doc)

// Attributes
public:
	CDdyName m_ddyName; 
	CDdyName m_ddyName_Pre; 

	int m_shiftidFirst;//��һ��İ��

	//��ӡ��ʽΪ12Сʱ����24Сʱ��1Ϊ12Сʱ��0Ϊ24Сʱ
	int m_printFormat;

	int m_phaseStationWidth; //վ�泵��վ���
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlot24Doc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetNoteTextOfTaiYuan( );     //ͳ��վ��ͣ��ʱ�䳬��1Сʱ�ĳ��Σ�add by lxl 20100315
	virtual void DrawHeaderTitle(CDC *pCDC,COLORREF clrColor);
	virtual void DrawLanzhouHeaderTitle(CDC *pCDC,COLORREF clrColor);
	virtual void DrawFooterTitle(CDC *pCDC,COLORREF clrColor);
	
	void DrawJiShi(CDC *pCDC,COLORREF clr);
	void DrawLanzhouJiShi(CDC *pCDC,COLORREF clr);
	
	void DrawJiShi24(CDC *pCDC,COLORREF clrColor);
	void DrawJiShi12(CDC *pCDC, COLORREF clrColor);

	void FillPhaseContent(CDC *pCDC, COLORREF clr);
	void FillPhaseContentXian(CDC *pCDC, COLORREF clr);
	void DrawPhasePlanFrame(CDC *pDC,LONG lColor,CRect *pRect);
	void DrawPhasePlanFrameXian(CDC *pDC,LONG lColor,CRect *pRect);
	
	virtual void ChangeDocumentArea(int width = 4160,int height = 2292);
	void DrawStationCar(CDC *pCDC, COLORREF clr);
	void ShowPhaseStoreTrain24(CDC *pCDC, COLORREF clr, int nStationNo, int phase);
	void ShowPhaseStoreTrain24(CDC *pCDC, COLORREF clr, int nStationNo, int phase,CString& stext);
	void DrawPhaseStoreTrainFrame24(CDC *pCDC, COLORREF clr);
	void DrawDDYName24(CDC *pCDC, COLORREF clrBackground);
	void DrawDDYName12(CDC *pCDC, COLORREF clrBackground);
	void DrawLanzhouDDYName12(CDC *pCDC, COLORREF clrBackground);
	
	int CalcAixixY();
	virtual void DrawTimeAixis(CDC * pCDC, long lColor);
	void DrawSubao24(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag);
	void DrawSubao24Xian(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag);
	void DrawSubao24Lanzhou(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag);

	void DrawTrainInform24(CDC *pCDC,COLORREF clrBackground);
	
	virtual void ShowStationString(CDC * pDC, long lColor,int flag);
	virtual void GetGraphStartTime(ulong nShiftId, int schd_type);
	virtual void ShowTimeString(CDC * pCDC, long lColor);
	virtual int InitStationVerticalPos(int schd_type = WORK_SCHEDULE);
	virtual void DrawStationAixis(CDC *pDC,LONG lColor);
	virtual int InitTimeUnit( );
	
	void DrawVirtulStationAixis(CDC *pDC, ULONG lColor);
	void ShowVirtualStationString(CDC * pDC, long lColor);

	void DrawSchdTrain(CDC *pCDC,int nFlag = 0);
	virtual ~CPlot24Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	virtual BOOL InitData(PRINTINFORM *pPrintInform, CTgDocument *pdoc);
protected:
	//{{AFX_MSG(CPlot24Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void PlotReDraw(CView* pView);
};

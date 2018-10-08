#if !defined(AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_)
#define AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgShuaiGuaCheStatistics.h : header file
//
#include "MyGridCtrl.h"
#include "SingleTrain4Statistics.h"
#include "StatisticsStoreStructureEx.h" //��ŵ���̨���ݵĽṹ
#include "HistorySchdDlg.h"			//����ѡ��ѡ����ʷͼ����ѡ���ʱʹ��
#include "tgmsg.h"

#include "StatisticsSectionInfo.h"

#define SGCMAXSTATIONUM 50

extern CStatisticsSectionInfo* g_StatisticsSecInfo;
/*

typedef struct {
	int nDispatcherNo;	//����̨���
	int nStationNum;	//�õ���̨���泵վ����
	int nStationList[SGCMAXSTATIONUM];	//�õ���̨���泵վ�б�
}StatisticsDispatchInfo;

typedef struct{
	int nStationNum;			//������Ҫͳ�Ƶĳ�վ�б����
	int nStationList[10];		//������Ҫͳ�Ƶĳ�վ�б��ڸó�վ�յ�����ʼ�����г�Ҫ����ͳ��
	int nSubSectionNum;			//����������������θ���
	int nSubSectionStart[10];	//������������������б����ʼ��վ���
	int nSubSectionEnd[10];		//������������������б����ֹ��վ���
}StatisticsSubSectionInfo;

typedef struct {
	int nSectionNo;			//���α��
	char sSectionName[20];	//��������
	int nSectionDistance;	//�����εĹ�����
	int nStationNum;		//�����εĳ�վ��Ŀ
	int nDispatcherNum;		//�������������̨����
	StatisticsDispatchInfo dispInfo[SGCMAXDIAPATCHNUMPERSECTION];	//����������ÿ������̨����ϸ��Ϣ
	StatisticsSubSectionInfo subSectionInfo;

}StatisticsSection;

typedef struct{
	int nNumToLoad;				//Ҫ���ĵ����ݸ���
	int nNumLoaded;				//�Ѿ����ĵ����ݸ���
	int nDispNoList[20];		//Ҫ���ĵĵ���̨��
	long nShiftDateList[20];	//Ҫ���ĵ�ʱ��
} LoadListStructure;

*/




/////////////////////////////////////////////////////////////////////////////
// CDlgShuaiGuaCheStatistics dialog

class CDlgShuaiGuaCheStatistics : public CDialog
{
// Construction
public:
	
	
	CDlgShuaiGuaCheStatistics(CWnd* pParent = NULL);   // standard constructor
	~CDlgShuaiGuaCheStatistics();	// standard desconstructor
	
	CMyGridCtrl *m_pGridCtrl;	//���˦�ҳ����ݵı��

	CDC m_MemDC;				//Ϊ����˦�ҳ�����ˮƽ����������ʱ����ʾ��Ӧ�ı�������������
	CBitmap m_MemBitmap;		//�ڴ�λͼ����������ı�����
	CBitmap* m_pOldBitmap;		//�ϵ��ڴ�λͼ
	int m_MemBitmapWidth;		//λͼ��ȣ���gridctrl�ռ�Ŀ����ͬ
	int m_MemBitmapHeight;		//λͼ�߶ȣ��������ı���ĸ߶���ͬ
	
	int m_CurMinCol;			//ÿ��ˮƽ������ʱ����ʾ�ĵ�һ���е��кţ���0��ʼ��������λ���ǵı�����λ��
	SCROLLINFO m_scInfo;		//������ʱ�򷢹����Ĺ�����Ϣ��ʱû����
	
	
	int m_TitleColNum;			//�������������� = m_HeadColNum+m_RearColNum+m_stationNum
	int m_HeadColNum;			//ǰͷ�ϵ�����
	int m_RearColNum;			//β��������
	
	int m_StationNum;			//��վ��Ŀ

	_TCHAR* m_HeadColName[10];	//��������վ����ǰ��ı���
	_TCHAR* m_RearColName[10];	//��������վ���ֺ���ı���
	_TCHAR m_StationColName[SGCMAXSTATIONUM][10];	//��������վ������

	int m_SingleStationColWidth;					//ÿ�г�վ�����ֵĿ��
	int m_TitleColWidth[SGCMAXSTATIONUM+20];		//���ж�Ӧ�Ŀ�ȣ���Ӧ������

	int m_GridColNum;								//Grid�ؼ��������� = m_HeadColNum+m_RearColNum+m_stationNum*2
	int m_GridColWidth[2*SGCMAXSTATIONUM+20];		//���ж�Ӧ�Ŀ�ȣ���Ӧgrid�ؼ�

	int m_xoffset;									//����������ʱ����Ҫƽ�Ƶ�������ʱû����
	
	StatisticsSection* m_CurStatisticsSection;		//��ǰѡ������ָ�� 
	int m_CurStatisticsSectionNo;					//��ǰѡ�����εı��

		//���弸������г����ݵı���
	CStatisticsStoreStructureEx m_datastore;		//����������ݵ�һ���ṹ
	CShiftDataPerDispStation* m_curShiftDataStore;	//��ŵ�ǰҪ�������ݵĽṹ

	
// Dialog Data
	//{{AFX_DATA(CDlgShuaiGuaCheStatistics)
	enum { IDD = IDD_DLG_SHUAIGUASTATISTICS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShuaiGuaCheStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL SetDefaultVal();
	BOOL SetGridColWidth();
	int ComputeSourceX();
	//BOOL LoadStatisticsSectionInfo();
	BOOL DrawStationNameTitle(CDC* pDC,CString DrawString, CRect DrawRect);
	BOOL DrawHeadRearTitle(CDC* pDC,CString DrawString,CRect DrawRect);
	BOOL DrawMemTitle(CDC* pDC);
	void ShowRecordInGrid();
	//int GetStationListofDisp(int sectionNo,int dispNo,int& stationNum,int* stationList);
	// Generated message map functions
	//{{AFX_MSG(CDlgShuaiGuaCheStatistics)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnHScrollStatisticsContent(WPARAM scrolllen,LPARAM scrollinfo);
	afx_msg void OnButtonClose();
	afx_msg void OnClose();
	afx_msg void OnStatsticsSgcRemoteload();
	afx_msg LRESULT OnSchdAddTrain(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSchdStatus(WPARAM wParam,LPARAM lParam);
	afx_msg void OnStatisticsLoadschedule();
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg void OnStatisticsSection(UINT message);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_)

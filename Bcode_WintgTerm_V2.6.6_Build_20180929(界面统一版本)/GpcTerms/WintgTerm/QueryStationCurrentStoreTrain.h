#if !defined(AFX_QUERYSTATIONCURRENTSTORETRAIN_H__8CE01C25_01EA_4227_8C26_CFA17510AEEE__INCLUDED_)
#define AFX_QUERYSTATIONCURRENTSTORETRAIN_H__8CE01C25_01EA_4227_8C26_CFA17510AEEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryStationCurrentStoreTrain.h : header file
//
#include "tg_basicpara.h"
#include "menubutton.h"
#include "headerlist.h"
#include ".\ext_src\CJFlatComboBox.h"  //office ����COMBOX

#include "Dlgmainfrm.h"	//	��ӡ��
#include "cadstruct.h"
#include <baseMsg/gpc_msgid.h>
#include <BaseMsg/clbth.h>
#include <vector>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CQueryStationCurrentStoreTrain dialog
class CTgDocument;
class CQueryStationCurrentStoreTrain : public CDialog
{
// Construction
public:
	CQueryStationCurrentStoreTrain(
	   CTGBasicParam *pBasicPara,
	   int nConid,
	   int nShiftid,
       int nStationNo, 
       int nPhase,     
	   BOOL readonly,
	   CWnd* pParent = NULL);   // standard constructor

public:
	int GetChineseSplitFlag(CString &chinesestr);
	CString ConvertShiftIdToString(unsigned long id);
	void OnPrint(CDC* pDC,CPrintInfo* pInfo);	//��ӡ����
	int  SideRowNumToIndex(int rownum);	//���ڹɵ��б��ж�Ӧ���к�ת���������ݽṹ�ж�Ӧ��������
	void ShowStationSideTrain();	//����ʾһ����վ�Ĺɵ���Ϣ
	void InitIdentify();	
	void GetStationCarsInfo(byte type =1,ushort StationIndex=0);
	void SendSideinfo(int StationIndex);
	BOOL PrintSingleStation();
	
	BOOL m_bReadOnly;
	int   m_nSectionID;
	int   m_nShiftID;
	CMenu m_menuStation;
	CMenuButton m_ctrlStationMenuButton;
	ushort m_nTotalStationNum,m_nCurrentSelectStation;
	ushort m_nAllStationArray[256];
    ushort nSideNum,nSideList[64];//
	int nStationCarriageCount;     // ��վ�ĳ�����Ŀ
    int nSideCarriageCount;        //	�ɵ��ϵĳ�����Ŀ

	int m_cur_side_rownum;		//��ǰ�ɵ���LIST CTRL�е��к�
	int m_cur_carriage_rownum;	//��ǰ�ɵ��ɵ�������LIST CTRL�е��к�


	//��ӡ�ã����ֳ���������
	int m_totalCount;
	int m_pCount,m_cCount,m_pbCount,m_yCount,m_sCount,m_dCount,m_bCount;
	int m_kCount,m_qCount,m_jzCount,m_jzCountK,m_szCount;
	int m_pCountK,m_cCountK,m_pbCountK,m_yCountK,m_sCountK,m_dCountK,m_bCountK;
	int m_kCountK,m_qCountK,m_szCountK,m_axCount,m_axCountK;
	
	
	CComboBox m_carriageexplainCtrl;
	CComboBox m_carriagetypeCtrl;
	CComboBox m_carriageStytleCtrl;
	CComboBox m_carriageApplyCtrl;

	CHeaderList m_ctrlStationStoreTrainList;
	CHeaderList m_ctrlSideStoreTrainList;
	CHeaderList m_ctrlTrainStatisticList;

	//CToolTipWnd m_ctrlTooltipWnd;
	int nStart_time;
	int nEnd_time;
	int m_nCurrentPhase;
	bool bChangdedFlag;
	CDlgMainFrame* m_PreFrame;	//��ӡ���ڿ��

	
	struct _tagStationStoreTrain info;

	int m_nCurrentStationIndex,m_nCurrentSideIndex,m_nCurrentCarriageIndex;
	void ShowCarriageRecord(struct _tagSideStoreCarriage *rec);
	void ShowSideRecord(int nCurrentSideIndex);
// Dialog Data
	//{{AFX_DATA(CQueryStationCurrentStoreTrain)
	enum { IDD = IDD_DLG_STATION_CURRENT_STORE_TRAIN };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryStationCurrentStoreTrain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ClearAllEdit();
	BOOL InitStationSideInfo();

	// Generated message map functions
	//{{AFX_MSG(CQueryStationCurrentStoreTrain)
	virtual BOOL OnInitDialog();
	afx_msg void OnScstAck();
	afx_msg void OnScstCancel();
	afx_msg void OnScstSideChange();
	afx_msg void OnScstSideDelete();
	afx_msg void OnScstSideInsert();
	afx_msg void OnScstStationname();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeScstCarriageExplain();
	afx_msg void OnSave();
	afx_msg void OnEditSide();

	afx_msg void OnButtonPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CTGBasicParam *m_pBasicPara;
	void ShowTrainStatisticInfo();
	afx_msg void OnCbnSelchangeScstCarriageStyle();
public:
	// ��������
	short m_nCarriage_num;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYSTATIONCURRENTSTORETRAIN_H__8CE01C25_01EA_4227_8C26_CFA17510AEEE__INCLUDED_)

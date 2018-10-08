// QueryStationCurrentStoreTrain.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "sysshare.h"
#include "resource.h"
#include "QueryStationCurrentStoreTrain.h"
#include "messages.h"
#include "function.h" 
#include "dlgeditstationsidename.h" 
#include <baseMsg\msgid.h>
#include "tgmsg.h"


#include "db_msgproc.h"
#include ".\querystationcurrentstoretrain.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//��ӡʹ�õĻ�ͼ����
void Drawf_QuseryStation(CDC* pDC,CPrintInfo* pInfo,void* pVoid=NULL);

//��վ�ɵ���Ϣ��LOADTLE�������վ�����������Զ�����
#define SELECT_STATION_COMMAND_BASE		1700

extern void SetStartEndtime(BYTE flag, int m_nShiftID, int& nEnd_time, int&nStart_time);                                          

int OsGetStationSideInfo(ushort usStationNo,ushort *pusSideNum,ushort *usSideList)
{   
	*pusSideNum = 0;
	STATION_SIDES_INFO stationSideInfo;
	if(gpcdef.GetSidesOfStation(usStationNo, stationSideInfo))
	{
		int nCount = stationSideInfo.sideNum;
		for(int i=0;i<nCount;i++)
			usSideList[i]=stationSideInfo.sideno[i];

		*pusSideNum=nCount;
	}
	return *pusSideNum;
}


#define STATIONSTORETRAINCOLUMNS		3
_TCHAR *_gszStationStoreTrainLabel[STATIONSTORETRAINCOLUMNS] =
{
	_T("�ɵ�"), _T("�ִ泵��Ϣ"),_T("��Ŀ"),
};


int _gnStationStoreTrainFmt[STATIONSTORETRAINCOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnStationStoreTrainWidth[STATIONSTORETRAINCOLUMNS] = 
{
	50,430,40,
};

#define SIDESTORECARRIAGECOLUMNS		4
_TCHAR *_gszSideStoreCarriageLabel[SIDESTORECARRIAGECOLUMNS] =
{
	_T("��������"), _T("����(��)��"), _T("ȥ��"),_T("����")  // /�س�ȥ��
};


int _gnSideStoreCarriageFmt[SIDESTORECARRIAGECOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnSideStoreCarriageWidth[SIDESTORECARRIAGECOLUMNS] = 
{
	90,70,90,80
};


_TCHAR *_gszTrainStatisticLabel[3] =
{
	_T("����"), _T("�ճ�"),_T("�س�"),
};

int _gnTrainStatisticFmt[3] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnTrainStatisticWidth[3] = 
{
	75,30,23,
};


/////////////////////////////////////////////////////////////////////////////
// CQueryStationCurrentStoreTrain dialog
CQueryStationCurrentStoreTrain::CQueryStationCurrentStoreTrain(
	 CTGBasicParam *pBasicPara,
	int nConid,
	int nShiftid,
	int nStationNo,////
	int nPhase,////
	BOOL ReadOnly,
	CWnd* pParent /*=NULL*/)
	: CDialog(CQueryStationCurrentStoreTrain::IDD, pParent)
	, m_nCarriage_num(0)
{
	//{{AFX_DATA_INIT(CQueryStationCurrentStoreTrain)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bReadOnly=ReadOnly;
	m_nSectionID = nConid;
	m_nShiftID=nShiftid;
	nStationCarriageCount=0; 
	nSideCarriageCount=0;     

	m_nCurrentSelectStation = nStationNo;   
	m_nCurrentPhase = nPhase;   
	bChangdedFlag = FALSE;
	m_pBasicPara = pBasicPara;
}


void CQueryStationCurrentStoreTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryStationCurrentStoreTrain)
	DDX_Control(pDX, IDC_SCST_CARRIAGE_EXPLAIN, m_carriageexplainCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_TYPE, m_carriagetypeCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_STYLE, m_carriageStytleCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_APPLY, m_carriageApplyCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_SCST_CARRIAGE_NUMBER, m_nCarriage_num);
	DDV_MinMaxShort(pDX, m_nCarriage_num, 0, 128);
}

BEGIN_MESSAGE_MAP(CQueryStationCurrentStoreTrain, CDialog)
	//{{AFX_MSG_MAP(CQueryStationCurrentStoreTrain)
	ON_BN_CLICKED(IDC_SCST_ACK, OnScstAck)
	ON_BN_CLICKED(IDC_SCST_CANCEL, OnScstCancel)
	ON_BN_CLICKED(IDC_SCST_SIDE_CHANGE, OnScstSideChange)
	ON_BN_CLICKED(IDC_SCST_SIDE_DELETE, OnScstSideDelete)
	ON_BN_CLICKED(IDC_SCST_SIDE_INSERT, OnScstSideInsert)
	ON_BN_CLICKED(IDC_SCST_STATIONNAME, OnScstStationname)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_SCST_CARRIAGE_EXPLAIN, OnSelchangeScstCarriageExplain)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_EDIT_SIDE, OnEditSide)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_SCST_CARRIAGE_STYLE, OnCbnSelchangeScstCarriageStyle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryStationCurrentStoreTrain message handlers

BOOL CQueryStationCurrentStoreTrain::PreTranslateMessage(MSG* lpMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((lpMsg->message == WM_KEYDOWN) &&
		((lpMsg->wParam == VK_RETURN) || (lpMsg->wParam == VK_ESCAPE)))
	{
		CWnd *pWndFocus = NULL;
		TCHAR szClass[10];
		if ((lpMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
			){
				return TRUE;
			} else if (lpMsg->wParam == VK_ESCAPE)
				return TRUE;
	}	



	//commented by Qianguorong 2003-4-12 ends here
	return CDialog::PreTranslateMessage(lpMsg);
}

BOOL CQueryStationCurrentStoreTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTime t = CTime::GetCurrentTime( );
	long m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( )) * 60 + t.GetSecond( );
	long m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1) * 31
		+ t.GetDay( ) - 1;

	//��ȡ�༭ʱ�䣬����������ԸĽ�
	this->InitIdentify();


	m_ctrlStationStoreTrainList.SubclassDlgItem(IDC_SCST_STATION_STORE_LIST,this);
	//head of side list  
	m_ctrlStationStoreTrainList.SetHeaderLabel(STATIONSTORETRAINCOLUMNS,_gszStationStoreTrainLabel,
		_gnStationStoreTrainWidth,_gnStationStoreTrainFmt);

	m_ctrlSideStoreTrainList.SubclassDlgItem(IDC_SCST_SIDE_STORE_LIST,this);
	//head of train list 
	m_ctrlSideStoreTrainList.SetHeaderLabel(SIDESTORECARRIAGECOLUMNS,_gszSideStoreCarriageLabel,
		_gnSideStoreCarriageWidth,_gnSideStoreCarriageFmt);
	//����ͳ��
	m_ctrlTrainStatisticList.SubclassDlgItem(IDC_SCST_STATION_STATISTICS,this);
	for(int j = 0; j < m_pBasicPara->m_carKindType.size()+1; j++)
	{
		if(j == 0)
		{
			m_ctrlTrainStatisticList.InsertColumn(j, 
				_gszTrainStatisticLabel[j], 
				_gnTrainStatisticFmt[j],
				_gnTrainStatisticWidth[j], -1);
		}
		else
		{
			m_ctrlTrainStatisticList.InsertColumn(j, 
				m_pBasicPara->m_carKindType[j-1].shortName, 
				_gnTrainStatisticFmt[1],
				_gnTrainStatisticWidth[1], -1);
		}
	}

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;
	int n=0;
	CString sTrainTypeName;
	for(int i=0;i < m_pBasicPara->m_carKindType.size();i++)    
	{  
		for(int j=0; j<m_pBasicPara->m_carKindType[i].carKind.size(); j++)
		{
			sTrainTypeName = m_pBasicPara->m_carKindType[i].carKind[j].name;

			bool bFind=false;
			for(int k=0; k<m_ctrlTrainStatisticList.GetItemCount();k++)
			{
				CString strTitle = m_ctrlTrainStatisticList.GetItemText(k, 0);
				if(strTitle == sTrainTypeName)
				{
					bFind = true;
					break;
				}
			}

			if(!bFind)
			{
				lvi.iItem = n+1;
				lvi.pszText = (char *)(LPCTSTR)sTrainTypeName;
				m_ctrlTrainStatisticList.InsertItem(&lvi);
				n++;
			}
		}
	}

	// ���
	for(int i=0;i<m_pBasicPara->m_carKindType.size();i++)
	{      
		m_carriageStytleCtrl.AddString(m_pBasicPara->m_carKindType[i].name);
	}
	if(m_pBasicPara->m_carKindType.size() > 0)
		m_carriageStytleCtrl.SetCurSel(0);

	// ����
	if(m_pBasicPara->m_carKindType.size()>0)    
	{  
		for(int j=0; j<m_pBasicPara->m_carKindType[0].carKind.size(); j++)
		{
			m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[0].carKind[j].name);
		}

		if(m_pBasicPara->m_carKindType[0].carKind.size() > 0)
			m_carriagetypeCtrl.SetCurSel(0);
	}

	// ����
	CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
	if(pWnd)
		pWnd->SetWindowText("1");

	CSpinButtonCtrl *spinWnd;

	spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);
	spinWnd->SetBuddy(pWnd);
	spinWnd->SetRange(1,128);    
	spinWnd->SetPos(1);

	// ��ҵ˵��
	for(int i=0;i < m_pBasicPara->m_aryWorkExplain.GetCount();i++)
	{
		m_carriageexplainCtrl.AddString(m_pBasicPara->m_aryWorkExplain.GetAt(i));
	}
	if(m_pBasicPara->m_aryWorkExplain.GetCount()>0)
	{
		m_carriageexplainCtrl.SetCurSel(0);
	}

	// ����
	for(int i=0;i < m_pBasicPara->m_aryCarrApply.GetCount();i++)
	{
		m_carriageApplyCtrl.AddString(m_pBasicPara->m_aryCarrApply.GetAt(i));
	}
	if(m_pBasicPara->m_aryCarrApply.GetCount()>0)
	{
		m_carriageApplyCtrl.SetCurSel(0);
	}

	//select station button 
	int nCurrentStationNo;

	m_ctrlStationMenuButton.SubclassDlgItem(IDC_SCST_STATIONNAME,this);

	//������վ�ĵ����˵�
	m_menuStation.CreatePopupMenu( );

	int nStationIndexFlag[32];
	memset(nStationIndexFlag,0x00,sizeof(nStationIndexFlag));
	int nCount = 0;

	int nStationList[64];
	int nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);

	gpcdef.GetSectionPhaseStoreStationList(m_nSectionID,nStationNum,nStationList);

	for(i=0;i<nStationNum;i++)
	{
		// ��վ���ּ��뵯���˵�
		m_menuStation.AppendMenu(MF_STRING|MF_ENABLED,SELECT_STATION_COMMAND_BASE + nCount,
			gpcdef.GetStationName(nStationList[i]));
		m_nAllStationArray[nCount] = nStationList[i];
		nCount += 1;
	}

	m_menuStation.SetDefaultItem(m_nCurrentSelectStation,true);

	CButton *pButton;
	pButton = (CButton *)GetDlgItem(IDC_SCST_STATIONNAME);
	//��ʾĬ�ϳ�վ����
	pButton->SetWindowText(gpcdef.GetStationName(m_nAllStationArray[m_nCurrentSelectStation]));	
	pButton->EnableWindow(FALSE);


	m_nTotalStationNum = nCount;                  //station num total 
	m_nCurrentCarriageIndex = 0;
	m_nCurrentSideIndex = -1;


	nCurrentStationNo = m_nAllStationArray[m_nCurrentSelectStation];

	m_cur_side_rownum = -1;		//��ǰ�ɵ���LIST CTRL�е��к�
	m_cur_carriage_rownum = -1;	//��ǰ�ɵ��ɵ�������LIST CTRL�е��к�

	//���ֳ�������������ӡ�ã�����ʾ��վ��Ϣʱ�����ͳ��
	m_totalCount = 0;
	m_pCount = m_cCount = m_pbCount = m_yCount = m_sCount = m_bCount = 0;
	m_dCount = m_jzCount = m_szCount = m_kCount =  m_axCount = m_qCount = 0;
	m_pCountK = m_cCountK = m_pbCountK = m_yCountK = m_sCountK = m_bCountK = 0;
	m_dCountK = m_jzCountK = m_szCountK = m_kCountK = m_axCountK = m_qCountK = 0;


	m_PreFrame = NULL;//��ӡ���

	//��ʷͼ���Ĳ��ܸ�������
	if(m_bReadOnly)
	{
		CWnd* pwnd = NULL;
		pwnd = GetDlgItem(IDC_SCST_SIDE_INSERT);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_SIDE_CHANGE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_SIDE_DELETE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SAVE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_EDIT_SIDE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_ACK);
		pwnd->EnableWindow(FALSE);

	}

	ShowStationSideTrain();

	ShowTrainStatisticInfo();

	InitDlgPosition(this);
	return TRUE;    
}

void CQueryStationCurrentStoreTrain::OnCbnSelchangeScstCarriageStyle()
{
	CString strCarTypeName;
	// ���
	m_carriageStytleCtrl.GetWindowText(strCarTypeName);

	// ����
	for(int i=0; i<m_pBasicPara->m_carKindType.size(); i++)
	{
		if(strCarTypeName == m_pBasicPara->m_carKindType[i].name)
		{
			if(m_pBasicPara->m_carKindType[i].carKind.size()>0)
				m_carriagetypeCtrl.ResetContent();

			for(int j=0; j<m_pBasicPara->m_carKindType[i].carKind.size(); j++)
			{
				m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[i].carKind[j].name);
			}

			if(m_pBasicPara->m_carKindType[i].carKind.size()>0)
				m_carriagetypeCtrl.SetCurSel(0);
			break;
		}
	}
}

void CQueryStationCurrentStoreTrain::OnScstAck() 
{ 
	m_nCurrentStationIndex = m_nAllStationArray[m_nCurrentSelectStation];
	SendSideinfo(m_nCurrentStationIndex);
	bChangdedFlag = true;
	CDialog::OnOK( );
}

void CQueryStationCurrentStoreTrain::OnScstCancel() 
{
	CDialog::OnCancel( );
}

void CQueryStationCurrentStoreTrain::OnScstSideChange() 
{
	//change Updated info 
	do 
	{
		if(m_nCurrentSideIndex<0) 
			return;
		struct _tagSideStoreCarriage tmp;

		// ����
		BYTE nSubSel = m_carriagetypeCtrl.GetCurSel( );
		if(nSubSel == CB_ERR)
			break;

		// ���
		BYTE nSel = m_carriageStytleCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		
		tmp.usCarriageType = m_pBasicPara->GetCarriageType(nSel, nSubSel);

		// ����
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
		if(pWnd == NULL)
			break;
		CString text;
		pWnd->GetWindowText(text);
		if(text.GetLength( ) == 0)
			break;
		if(IsAllSpace((char *)(LPCTSTR)text))
			break;
		tmp.usCarriageNum = atoi((char *)(LPCTSTR)text);
		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		if(pWnd == NULL)
			break;

		pWnd->GetWindowText(text);
		strcpy(tmp.lpszCarriageRemark,(char *)(LPCTSTR)text);

		m_carriageApplyCtrl.GetWindowText(text);
		strcpy(tmp.lpszCarriageApply,(char *)(LPCTSTR)text);

		tmp.usCarriageExplain = m_carriageexplainCtrl.GetCurSel();


		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		for(int i=0;i<sideinfo->usRecNum;i++)
		{ 
			if( sideinfo->rec[i].usCarriageType  ==tmp.usCarriageType 
				&& sideinfo->rec[i].usCarriageNum  ==tmp.usCarriageNum 
				&& ( strcmp(tmp.lpszCarriageRemark,sideinfo->rec[i].lpszCarriageRemark)==0 )
				&& sideinfo->rec[i].usCarriageExplain ==tmp.usCarriageExplain
				&& ( strcmp(tmp.lpszCarriageApply,sideinfo->rec[i].lpszCarriageApply)==0 ))
			{
				AfxMessageBox("�˹ɵ������д��ֳ��ᣬ���޸ĸó�����Ŀ!",MB_OK);
				return;
			}
		}

		sideinfo->rec[m_nCurrentCarriageIndex] = tmp;

		m_pBasicPara->GetCarTypeAndKindIndex(tmp.usCarriageType, nSel, nSubSel);

		if(nSel<m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;
		}

		CString temp;

		temp.Format("%s%s ",
			m_pBasicPara->m_aryWorkExplain.GetAt(tmp.usCarriageExplain),
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);

		text += temp;

		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,0,text);
		text.Format("%d",tmp.usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,1,text);
		text.Format("%s",tmp.lpszCarriageRemark);
		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,2,text);

		text = "";
		nSideCarriageCount=0;
		nStationCarriageCount-=sideinfo->CarriageCounts;  
		for( i=0;i < sideinfo->usRecNum;i++)
		{   
			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			
			temp.Format("%s%s%d ",
				m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
				sideinfo->rec[i].usCarriageNum);

			text += temp;

			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum;   
			sideinfo->CarriageCounts=nSideCarriageCount;
		}

		nStationCarriageCount+=sideinfo->CarriageCounts; 

	}while(0);
	ShowStationSideTrain();
}

void CQueryStationCurrentStoreTrain::OnScstSideDelete() 
{ 
	do 
	{ 
		if(m_nCurrentSideIndex<0)
			return;

		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		if(m_nCurrentCarriageIndex < 0 || m_nCurrentCarriageIndex >= sideinfo->usRecNum)
			break;
		//ɾ��������Ӧ��
		m_ctrlSideStoreTrainList.DeleteItem(m_nCurrentCarriageIndex);
		//������Ŀ����
		sideinfo->usRecNum -= 1;  /// 1
		//�����������
		for(int i=m_nCurrentCarriageIndex;i < sideinfo->usRecNum;i++)  /// 2
			sideinfo->rec[i] = sideinfo->rec[i + 1];
		if(m_nCurrentCarriageIndex >= sideinfo->usRecNum)
		{
			m_nCurrentCarriageIndex = 0;
			m_ctrlSideStoreTrainList.EnsureVisible(m_nCurrentCarriageIndex,false);
		}
		//ѡ��  ��һ�鳵��
		//m_ctrlSideStoreTrainList.SetCurSel(m_nCurrentCarriageIndex);   
		//��ʾ  ��һ�鳵��
		ShowCarriageRecord(&sideinfo->rec[m_nCurrentCarriageIndex]);

		CString temp;
		CString text;

		nStationCarriageCount-=sideinfo->CarriageCounts; ///��ȥ���йɵ��ĳ�����Ŀ
		nSideCarriageCount=sideinfo->CarriageCounts=0;
		for(i=0;i < sideinfo->usRecNum;i++)
		{  
			if(sideinfo->rec[i].usCarriageNum<=0)
				continue;
			CString temp;
			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			if(nSel < m_pBasicPara->m_carKindType.size())
			{
				text = m_pBasicPara->m_carKindType[nSel].shortName;
				if(text.IsEmpty())
					text = m_pBasicPara->m_carKindType[nSel].name;

				temp.Format("%s%s%d",
					m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,//����
					sideinfo->rec[i].usCarriageNum);//������Ŀ 	
				text += temp;
			}
			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum;
			sideinfo->CarriageCounts=nSideCarriageCount;  
			text += temp;
		}

		nStationCarriageCount+=sideinfo->CarriageCounts;///�����ֹɵ����еĳ�����Ŀ

	}while(0);
	ShowStationSideTrain();
}

void CQueryStationCurrentStoreTrain::OnScstSideInsert() 
{
	// ����1�������¼
	do 
	{ 
		if(m_nCurrentSideIndex<0) 
			return;
		struct _tagSideStoreCarriage tmp;     //��ǰ�ĳ���ṹ���������˽ṹ

		BYTE nSubSel = m_carriagetypeCtrl.GetCurSel();
		if(nSubSel == CB_ERR)
			break;
		
		BYTE nSel = m_carriageStytleCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		
		tmp.usCarriageType = m_pBasicPara->GetCarriageType(nSel, nSubSel);

		//��ó�����Ŀ
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
		if(pWnd == NULL)
			break;
		CString text;
		pWnd->GetWindowText(text);
		if(text.GetLength( ) == 0)
			break;
		if(IsAllSpace((char *)(LPCTSTR)text))
			break;
		tmp.usCarriageNum = atoi((char *)(LPCTSTR)text);  ///////// 2 ��Ŀ


		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		if(pWnd == NULL)
			break;
		pWnd->GetWindowText(text);
		strcpy(tmp.lpszCarriageRemark,(char *)(LPCTSTR)text);////// 3 ��ҵ˵��

		m_carriageApplyCtrl.GetWindowText(text);
		strcpy(tmp.lpszCarriageApply,(char *)(LPCTSTR)text);////// 3 ��ҵ˵��

		nSel = m_carriageexplainCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		tmp.usCarriageExplain = nSel;

		//��ǰ�ɵ�ָ��
		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		for(int i=0;i<sideinfo->usRecNum;i++)
		{ 
			if( sideinfo->rec[i].usCarriageType  ==tmp.usCarriageType 
				&& sideinfo->rec[i].usCarriageNum  ==tmp.usCarriageNum 
				&& ( strcmp(tmp.lpszCarriageRemark,sideinfo->rec[i].lpszCarriageRemark)==0 )
				&& sideinfo->rec[i].usCarriageExplain ==tmp.usCarriageExplain
				&& ( strcmp(tmp.lpszCarriageApply,sideinfo->rec[i].lpszCarriageApply)==0 ))
			{
				AfxMessageBox("�˹ɵ������д��ֳ��ᣬ���޸ĸó�����Ŀ!",MB_OK);
				return;
			}
		}

		sideinfo->rec[sideinfo->usRecNum] = tmp;    //���˳���ṹ��ֵ�� ������

		//��ʾ�˽ṹ�ڳ����б��У� ���� 
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT ;
		lvi.iSubItem = 0;
		CString temp;

		m_pBasicPara->GetCarTypeAndKindIndex(tmp.usCarriageType, nSel, nSubSel);
		if(nSel<m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;
		}

		temp.Format("%s%s ",
			m_pBasicPara->m_aryWorkExplain.GetAt(tmp.usCarriageExplain),
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);

		text += temp;
		lvi.iItem = sideinfo->usRecNum;            //
		lvi.pszText = (char *)(LPCTSTR)text;
		m_ctrlSideStoreTrainList.InsertItem(&lvi);
		text.Format("%d",tmp.usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(sideinfo->usRecNum,1,text);
		text.Format("%s",tmp.lpszCarriageRemark);
		m_ctrlSideStoreTrainList.SetItemText(sideinfo->usRecNum,2,text);
		sideinfo->usRecNum += 1;   //���ͳ��� ��Ŀ

		//��ʾ�˹ɵ����еĳ�����Ϣ��  ����
		CString text2;
		text = "";
		nSideCarriageCount=0;
		nStationCarriageCount-=sideinfo->CarriageCounts;////��ȥδ����ǰ�ĸùɵ��ĳ�����Ŀ
		//���¼���ùɵ��ĳ�����Ŀ
		for(int i=0;i < sideinfo->usRecNum;i++)
		{ 
			if(sideinfo->rec[i].usCarriageNum<=0)
				continue;

			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			temp.Format("%s%s%d ",
				m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
				sideinfo->rec[i].usCarriageNum); 

			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum ;
			sideinfo->CarriageCounts=nSideCarriageCount;

			text += temp;
		}
		nStationCarriageCount+=sideinfo->CarriageCounts ;  //// �����µĸĹɵ��ĳ�����Ŀ

		ClearAllEdit();
	}while(0);
	ShowStationSideTrain();
}

/**********************ѡ��վ************************/
void CQueryStationCurrentStoreTrain::OnScstStationname() 
{
	// ��ʾ��վ�����б�
	m_nCurrentSideIndex = 0;
	nStationCarriageCount=0;///////////////
	nSideCarriageCount=0;   ////////////////  
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText("");
	GetDlgItem(IDC_TOTAL_SUM)->SetWindowText("");

	POINT point;

	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_SCST_STATIONNAME);
	pWnd->GetWindowRect(&rect);
	point.x = rect.left;
	point.y = rect.bottom;

	// -[Feral]-----------------------------------------------------------
	// show and track the menu
	DWORD dwSelectionMade = m_menuStation.TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD),
		point.x, point.y, this
		);
	do 
	{
		if(dwSelectionMade == 0)
			break;

		dwSelectionMade -= SELECT_STATION_COMMAND_BASE;
		if(dwSelectionMade < 0 || dwSelectionMade >= m_nTotalStationNum)
			break;

		if(dwSelectionMade == m_nCurrentSelectStation)
			break;

		m_nCurrentSelectStation = (ushort)dwSelectionMade;
		CButton *pButton = (CButton *)GetDlgItem(IDC_SCST_STATIONNAME);
		//��ʾĬ�ϳ�վ����
		pButton->SetWindowText(gpcdef.GetStationName(m_nAllStationArray[m_nCurrentSelectStation]));


		//��ʼ����վ�ֳ�����
		nSideNum = sizeof(nSideList) / sizeof(nSideList[0]);
		OsGetStationSideInfo(m_nAllStationArray[m_nCurrentSelectStation],&nSideNum,nSideList);
		// CString as; as.Format("sideNum:%d",nSideNum);AfxMessageBox(as);
		if(nSideNum<1)
		{
			GetDlgItem(IDC_SCST_SIDE_INSERT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SCST_SIDE_CHANGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SCST_SIDE_DELETE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
			//break;
		}
		else
		{
			GetDlgItem(IDC_SCST_SIDE_INSERT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SCST_SIDE_CHANGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SCST_SIDE_DELETE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		}

		this->InitIdentify();

		memset(&info,0x00,sizeof(struct _tagStationStoreTrain)); 
		GetStationCarsInfo(m_nAllStationArray[m_nCurrentSelectStation]); ////// 

	}while(0);

	ShowStationSideTrain();
}



//��ʾ���ͳ���Ĵ泵���
void CQueryStationCurrentStoreTrain::ShowCarriageRecord(struct _tagSideStoreCarriage *rec)
{
	do 
	{
		if(rec == NULL)
			break;

		BYTE nSel = 0;
		BYTE nSubSel = 0;
		m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

		m_carriageStytleCtrl.SetCurSel(nSel);
		m_carriagetypeCtrl.ResetContent();
		if(nSel < m_pBasicPara->m_carKindType.size())
		{
			for(int i=0; i<m_pBasicPara->m_carKindType[nSel].carKind.size(); i++)
			{
				m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[nSel].carKind[i].name);
			}

			if(nSubSel < m_pBasicPara->m_carKindType[nSel].carKind.size())
				m_carriagetypeCtrl.SetCurSel(nSubSel);
		}
		
		CString text;
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);                       //������Ŀ
		text.Format("%d",rec->usCarriageNum);

		pWnd->SetWindowText(text);

		CSpinButtonCtrl *spinWnd;
		spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);  //��Ŀ
		spinWnd->SetPos(rec->usCarriageNum);

		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		pWnd->SetWindowText(rec->lpszCarriageRemark);                           //���᷽���Ϻ�

		for(int i=0;i < m_pBasicPara->m_aryCarrApply.GetCount();i++)
		{
			if(m_pBasicPara->m_aryCarrApply.GetAt(i) == rec->lpszCarriageApply)
			{
				m_carriageApplyCtrl.SetCurSel(i);
				break;
			}
		}

		m_carriageexplainCtrl.SetCurSel(rec->usCarriageExplain);

	}while(0);

}

//��ʾ�ɵ��Ĵ泵��� 
void CQueryStationCurrentStoreTrain::ShowSideRecord(int nCurrentSideIndex)
{
	if(nCurrentSideIndex < 0 || nCurrentSideIndex >= sizeof(info.sideinfo)/sizeof(info.sideinfo[0]))
		return;

	CString text;

	struct _tagSideStoredTrain *sideinfo;
	sideinfo = &info.sideinfo[nCurrentSideIndex];

	CWnd *pWnd = GetDlgItem(IDC_SCST_SIDE_NAME);
	text.Format("%s �ɵ�����վ�泵��Ϣ",sideinfo->pszSideName); 
	pWnd->SetWindowText(text);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;

	m_ctrlSideStoreTrainList.DeleteAllItems( );
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;
	struct _tagSideStoreCarriage *rec;

	for(int j=0;j <sideinfo->usRecNum;j++)
	{    
		rec = &sideinfo->rec[j];
		if(rec->usCarriageNum <=0)
			continue;

		BYTE nSel = 0;
		BYTE nSubSel = 0;
		m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

		if(nSel < m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;

			CString temp;
			temp.Format("%s%s ",
				m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);
			text += temp;
		}
		lvi.iItem = j;
		lvi.pszText = (char *)(LPCTSTR)text;

		m_ctrlSideStoreTrainList.InsertItem(&lvi);

		text.Format("%d",rec->usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(j,1,text);

		text.Format("%s",rec->lpszCarriageRemark);  
		m_ctrlSideStoreTrainList.SetItemText(j,2,text);

		text.Format("%s",rec->lpszCarriageApply);  
		m_ctrlSideStoreTrainList.SetItemText(j,3,text);


	}

	pWnd = GetDlgItem(IDC_EDIT_TIME);  
	CTime t=info.EditTime;
	CString s = t.Format( "%Y-%m-%d %H:%M" );
	pWnd->SetWindowText(s);

	m_ctrlSideStoreTrainList.SetCurSel(0);
	if(sideinfo->usRecNum > 0)
	{
		ShowCarriageRecord(&sideinfo->rec[0]);
	}
}


void CQueryStationCurrentStoreTrain::OnDestroy() 
{
	CDialog::OnDestroy();	
}

//ѡ��������: ��,��ж,��װ 
void CQueryStationCurrentStoreTrain::OnSelchangeScstCarriageExplain() 
{
	GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION)->SetWindowText("");
}

//���ͳ�վ�泵��Ϣ
void CQueryStationCurrentStoreTrain::SendSideinfo(int StationIndex)
{
	time_t t_input;
	CLBTH::Archive ar( CLBTH::Archive::store);

	CTime t = CTime::GetCurrentTime();
	if(m_nCurrentPhase!=CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT)
	{
		SetStartEndtime(m_nCurrentPhase, m_nShiftID, nEnd_time, nStart_time);
		t_input =  nStart_time;
	}
	else	
	{
		t_input =  t.GetTime();   //ʱ��
	}

	//msg begin
	CLBTH::MsgSimpleStationCarCtrl  mMsgSimpleStationCarCtrlStart;
	mMsgSimpleStationCarCtrlStart.input_time = t_input;
	mMsgSimpleStationCarCtrlStart.station = StationIndex;
	mMsgSimpleStationCarCtrlStart.phase = (CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarCtrlStart.status = CLBTH::MsgSimpleStationCarCtrl::MSG_BEGIN;
	mMsgSimpleStationCarCtrlStart.serialize(ar);
	//end of msg begin

	//msg data begin
	CLBTH::MsgSimpleStationCarFormat mMsgSimpleStationCarFormat;
	int nSendIndex =0;

	mMsgSimpleStationCarFormat.phase = (CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarFormat.input_time = t_input;
	mMsgSimpleStationCarFormat.station = StationIndex;

	byte bSideNameLen;

	struct _tagSideStoreCarriage *rec;

	for(int i = 0;i < info.usSideNum;i++)
	{
		mMsgSimpleStationCarFormat.side = info.sideinfo[i].usSideNo;

		bSideNameLen = strlen(info.sideinfo[i].pszSideName);
		memset(mMsgSimpleStationCarFormat.side_name, 0, sizeof(mMsgSimpleStationCarFormat.side_name));
		strncpy(mMsgSimpleStationCarFormat.side_name,info.sideinfo[i].pszSideName,bSideNameLen);

		//�ɵ������д泵
		if(info.sideinfo[i].usRecNum>0)
		{
			for(int k=0;k < info.sideinfo[i].usRecNum;k++)   //�ùɵ��Ķ�������
			{
				rec = &info.sideinfo[i].rec[k];

				mMsgSimpleStationCarFormat.index = nSendIndex++; ///

				mMsgSimpleStationCarFormat.car_flag = rec->usCarriageExplain;   //�����־
				mMsgSimpleStationCarFormat.car_type = rec->usCarriageType;      //��������	 
				mMsgSimpleStationCarFormat.car_count = rec->usCarriageNum;      //������Ŀ


				CString strText;
				int len = strlen(rec->lpszCarriageApply);
				if(len>0)
				{
					strText.Format("%s", rec->lpszCarriageApply);//����
					strText +="^";
				}
				CString sRemark;
				sRemark.Format("%s", rec->lpszCarriageRemark);
				strText = strText + sRemark;

				memset(mMsgSimpleStationCarFormat.destine, 0, sizeof(mMsgSimpleStationCarFormat.destine));
				strncpy(mMsgSimpleStationCarFormat.destine, strText, sizeof(mMsgSimpleStationCarFormat.destine)-1);      //����ȥ��

				mMsgSimpleStationCarFormat.serialize(ar);////
			}
		}
		//ֻ�пյĹɵ�
		else if(info.sideinfo[i].usRecNum == 0)
		{
			mMsgSimpleStationCarFormat.index = nSendIndex++; ///		
			mMsgSimpleStationCarFormat.car_flag =0;   //�����־
			mMsgSimpleStationCarFormat.car_type = 0;      //��������	 
			mMsgSimpleStationCarFormat.car_count = 0;      //������Ŀ	
			strcpy(mMsgSimpleStationCarFormat.destine,"");      //����ȥ��		
			mMsgSimpleStationCarFormat.serialize(ar);////
		}
	}
	//end of msg data  

	//msg end
	CLBTH::MsgSimpleStationCarCtrl  mMsgSimpleStationCarCtrlEnd;
	mMsgSimpleStationCarCtrlEnd.input_time = t_input;
	mMsgSimpleStationCarCtrlEnd.station = StationIndex;
	mMsgSimpleStationCarCtrlEnd.phase = (CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarCtrlEnd.status = CLBTH::MsgSimpleStationCarCtrl::MSG_END;
	mMsgSimpleStationCarCtrlEnd.serialize(ar);
	//end msg 

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGCommMessage(m_nSectionID,ar.getCount(),(char *)ar.getBuffer(),eid);
	GpcLogTrainMessage(0,0,"[����վ�泵]: svr_entity %d Station:%d",
		eid, StationIndex);
}

//�����������������ݣ�����һ����վ�Ĺɵ�����
void CQueryStationCurrentStoreTrain::GetStationCarsInfo(byte type,ushort StationIndex)
{
	CLBTH::MsgSimpleStationCarQuery mMsgSimpleStationCarQuery;
	SetStartEndtime(type, m_nShiftID, nEnd_time, nStart_time);
	mMsgSimpleStationCarQuery.begin_time = nStart_time;
	mMsgSimpleStationCarQuery.end_time = nEnd_time;
	mMsgSimpleStationCarQuery.phase = (CLBTH::MsgSimpleStationCarQuery::STACAR_PHASE)type;
	//only one station
	mMsgSimpleStationCarQuery.sta_count = 1;
	mMsgSimpleStationCarQuery.sta_list[0] = StationIndex;

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGBMsg(m_nSectionID,&mMsgSimpleStationCarQuery,eid);
}

void CQueryStationCurrentStoreTrain::OnSave() 
{ 
	if(m_nCurrentSideIndex<0) 
		return;

	m_nCurrentStationIndex = m_nAllStationArray[m_nCurrentSelectStation];

	SendSideinfo(m_nCurrentStationIndex);


	//��ʾ�����޸�ʱ��
	CWnd *pWnd = GetDlgItem(IDC_EDIT_TIME);  
	if(pWnd==NULL)
		return;
	CTime t=CTime::GetCurrentTime();

	CString s = t.Format( "%Y-%m-%d %H:%M" );
	pWnd->SetWindowText(s);

	bChangdedFlag = true;
}

//�������ļ��� line.cfg �л�ø�����վ�Ĺɵ���Ϣ
BOOL CQueryStationCurrentStoreTrain::InitStationSideInfo()
{ 
	return TRUE;
}

/************����༭�������****************/
void CQueryStationCurrentStoreTrain::ClearAllEdit()
{
	m_carriagetypeCtrl.SetCurSel(0);

	CString text;
	CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);    //������Ŀ
	if(pWnd==NULL) 
		return;
	pWnd->SetWindowText("");

	CSpinButtonCtrl *spinWnd;
	spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);  //��Ŀ
	if(spinWnd==NULL)
		return;
	spinWnd->SetPos(1);

	pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
	if(pWnd==NULL)
		return;
	pWnd->SetWindowText("");                           //���᷽���Ϻ�

	m_carriageexplainCtrl.SetCurSel(0);
}


void CQueryStationCurrentStoreTrain::InitIdentify()
{
	if(!gpcdef.GetOperatorRight(STATION_STORE_TRAIN))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_SCST_SIDE_INSERT);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_SIDE);
		pWnd->EnableWindow(FALSE);	
		pWnd = GetDlgItem(IDC_SCST_ACK);
		pWnd->EnableWindow(FALSE);	

		pWnd = GetDlgItem(IDC_SCST_SIDE_CHANGE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SCST_SIDE_DELETE);
		pWnd->EnableWindow(FALSE);	
	}
}

void CQueryStationCurrentStoreTrain::OnEditSide() 
{
	CDlgEditStationSideName dlg(&info);
	if(dlg.DoModal() == IDOK)
	{
		ShowStationSideTrain();
	}
}

void CQueryStationCurrentStoreTrain::ShowStationSideTrain()
{
	m_ctrlStationStoreTrainList.DeleteAllItems();

	struct _tagSideStoreCarriage *rec;	
	CString text,text2,tmp;

	text =text2 = "";

	if(info.usSideNum == 0)
	{
		return;
	}
	CString strSideName;
	struct _tagSideStoredTrain *sideinfo;

	int jzCount,jzCountK,szCount,szCountK,axCount,axCountK;
	jzCount = jzCountK = szCount = szCountK = axCount = axCountK = 0;


	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;


	int current_side_added_rownum;	//��ǰ�ɵ����������޷���ȫ��ʾ���ӵ�����
	int current_side_rownum;		//��ǰ�ɵ���һ�μ����б�ʱ����к�
	int all_sides_added_rownum;		//���йɵ����������޷���ȫ��ʾ���ӵ�����
	int current_toadded_rownum;		//Ҫ���ӵ��е��к�
	int sides_carriages_col_width;	//��ʾ�ɵ������ĸ��еĳ���

	CString current_side_fitted_str;	//û�г������ݳ��ȱ�׼������

	current_side_added_rownum = 0;
	all_sides_added_rownum = 0;
	current_toadded_rownum = 0;
	sides_carriages_col_width = 0;
	current_side_fitted_str = "";

	sides_carriages_col_width = m_ctrlStationStoreTrainList.GetColumnWidth(1);

	m_totalCount=0;
	for(int i=0; i<m_pBasicPara->m_carKindType.size(); i++)
	{
		for(int k=0; k<m_pBasicPara->m_carKindType[i].carKind.size(); k++)
		{
			m_pBasicPara->m_carKindType[i].carKind[k].count=0;
		}
	}

	for(int j=0;j < info.usSideNum;j++)    
	{  
		current_side_fitted_str = "";

		text = "";
		nSideCarriageCount=0;
		sideinfo = &info.sideinfo[j];
		strSideName = CString(sideinfo->pszSideName);

		current_toadded_rownum = j + all_sides_added_rownum;	//��ǰҪ���ӵ��кűȹɵ���Ŀ���ܴ�
		lvi.iItem = current_toadded_rownum;
		current_side_rownum = current_toadded_rownum;
		current_side_added_rownum = 0;	//��ǰ�ɵ��Ѿ����ӵ�����Ϊ0

		lvi.pszText = (char *)(LPCTSTR)strSideName;
		m_ctrlStationStoreTrainList.InsertItem(&lvi);

		CString strStyle;
		for(int k=0;k < sideinfo->usRecNum;k++)
		{ 
			rec = &sideinfo->rec[k];
			if(rec->usCarriageNum <=0)
				continue;
			strStyle = "";

			BYTE nSubSel = 0;
			BYTE nSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

			if(nSel >= m_pBasicPara->m_carKindType.size())
				continue;

			if(nSubSel >= m_pBasicPara->m_carKindType[nSel].carKind.size())
				continue;

			m_totalCount += rec->usCarriageNum;
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].count += rec->usCarriageNum;

			strStyle = m_pBasicPara->m_carKindType[nSel].shortName;
			if(strStyle.IsEmpty())
				strStyle = m_pBasicPara->m_carKindType[nSel].name;

			text += strStyle;
			
			CString strRemark = rec->lpszCarriageRemark; strRemark.Trim();
			CString strCarriageApply = rec->lpszCarriageApply; strCarriageApply.Trim();
			if(strRemark.GetLength() > 0 || strCarriageApply.GetLength()>0)
			{
				tmp.Format("%s%s%d(%s %s) ",
					m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
					rec->usCarriageNum,		
					strRemark, strCarriageApply);
			}
			else
			{
				tmp.Format("%s%s%d ",
					m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
					rec->usCarriageNum);
			}
			text += tmp;

			nSideCarriageCount+=  rec->usCarriageNum;
			sideinfo->CarriageCounts=nSideCarriageCount;   ///�ùɵ��ĳ�����Ŀ
			text2.Format("%d",nSideCarriageCount);	 

			//Ҫ��ʾ�����ݳ����˸��г���
			if(m_ctrlStationStoreTrainList.GetStringWidth(text) >= sides_carriages_col_width) 
			{
				//���Ǳ���������У������һ��������ʾ�ͳ����˳��ȣ����ǲ����д���
				if(current_side_fitted_str.IsEmpty())
					m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,text);      //�ùɵ�����Ϣ;
				else	//���ǵ�һ�����ݣ���������һ��
				{
					m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,current_side_fitted_str);      //�ùɵ�����Ϣ

					lvi.iItem = current_side_rownum + current_side_added_rownum+1;
					strcpy(lvi.pszText,"");
					m_ctrlStationStoreTrainList.InsertItem(&lvi);

					current_side_added_rownum++;
					all_sides_added_rownum++;

					text = strStyle + tmp;
					current_side_fitted_str = "";
				}
			}

			//û�г��������ø��Ƿ�ʽдԭ��λ��
			//��������������һ������û�г��������������һ�������Ѿ������Ժ󱻷ֶ���
			current_side_fitted_str = text;
			m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,text);      //�ùɵ�����Ϣ;

			//�ɵ��������Ǽ�¼�ڸùɵ�����ĵ�һ���к�����
			m_ctrlStationStoreTrainList.SetItemText(current_side_rownum,2,text2);   //�ùɵ�������
		}

		CString strCount;

		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_TOTAL_SUM);
		strCount.Format("%d", m_totalCount); 
		if(pWnd != NULL)
			pWnd->SetWindowText(strCount);

		ShowSideRecord(m_nCurrentSideIndex);     //��ʾ�����ɵ�����Ϣ
		ShowTrainStatisticInfo();
	}
}
BOOL CQueryStationCurrentStoreTrain::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	int idCtrl = (int) wParam;
	if(idCtrl != IDC_SCST_STATION_STORE_LIST && idCtrl != IDC_SCST_SIDE_STORE_LIST) 
		return CDialog::OnNotify(wParam, lParam, pResult);

	NMHDR *nmgv = (NMHDR*)lParam;
	int nMessageType=nmgv->code;


	POSITION pos = NULL;
	int nItem;

	if(idCtrl == IDC_SCST_STATION_STORE_LIST)
	{
		int newsideindex;
		switch(nMessageType)
		{
		case LVN_ITEMCHANGED:

			pos = m_ctrlStationStoreTrainList.GetFirstSelectedItemPosition();
			if(pos == NULL)
				break;
			nItem = m_ctrlStationStoreTrainList.GetNextSelectedItem(pos);

			newsideindex = SideRowNumToIndex(nItem);
			if( newsideindex == m_nCurrentSideIndex)	//Ҫ�ı�ļ�¼��ԭ��һ��������
				break;
			else
			{
				m_nCurrentSideIndex = newsideindex;
				ShowSideRecord(m_nCurrentSideIndex);
			}

			break;

		default:
			break;
		}

	}

	if(idCtrl == IDC_SCST_SIDE_STORE_LIST)
	{
		switch(nMessageType)
		{
		case LVN_ITEMCHANGED:

			pos = m_ctrlSideStoreTrainList.GetFirstSelectedItemPosition();
			if(pos == NULL)
				break;
			nItem = m_ctrlSideStoreTrainList.GetNextSelectedItem(pos);

			if(nItem == m_nCurrentCarriageIndex)	//Ҫ�ı�ļ�¼��ԭ��һ��������
				break;
			else

			{
				m_nCurrentCarriageIndex = nItem;
				ShowCarriageRecord(&(info.sideinfo[m_nCurrentSideIndex].rec[m_nCurrentCarriageIndex]));

			}

			break;

		default:
			break;
		}

	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}


//����LIST CTRL�еĹɵ��к�ת���ɶ�Ӧ��info�е�������
int CQueryStationCurrentStoreTrain::SideRowNumToIndex(int rownum)
{

	if(rownum <=0 )
		return rownum;

	int ret_index = 0;
	char str[100];
	for(int i=0; i<=rownum;i++)//�жϵ������ܹ��Ĺɵ����ֲ�Ϊ�յ��е�����
		//��ô��Ա���ɵ����ݵ�������������-1
	{
		m_ctrlStationStoreTrainList.GetItemText(i,0,str,100);
		if(strlen(str)>0)
			ret_index++;
	}

	return ret_index - 1;

}



void Drawf_QuseryStation(CDC* pDC,CPrintInfo* pInfo,void* pVoid)
{	
	((CQueryStationCurrentStoreTrain*)pVoid)->OnPrint(pDC,pInfo);	
}

void CQueryStationCurrentStoreTrain::OnPrint(CDC* pDC,CPrintInfo* pInfo)
{
	CString disp_msg,station_name;
	CWnd* cwnd_cur_std;
	cwnd_cur_std = GetDlgItem(IDC_SCST_STATIONNAME);
	cwnd_cur_std->GetWindowText(station_name);

	CPen pen;
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	CPen *pOldPen=pDC->SelectObject(&pen);

	CFont font,*oldfont;
	CPoint point;
	//��ӡ����
	CString ss=_T(station_name + "վ�ִ泵��ͳ��");
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	int ghdf=pDC->GetDeviceCaps(LOGPIXELSY);
	CRect DrawRect(0,0,nWidth,nHeight),rect,tempRect;		
	DrawRect.InflateRect(-100,-50,-100,-20);
	//pDC->Rectangle(&DrawRect);		
	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("����"));
	oldfont=pDC->SelectObject(&font);
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);

	rect=tempRect;rect.InflateRect(10,0);
	point=CPoint(DrawRect.left+(DrawRect.Width()-rect.Width())/2,rect.bottom+2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	point.Offset(0,2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	tempRect.right=DrawRect.right;
	pDC->DrawText(ss,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);	
	pDC->SelectObject(oldfont);
	font.DeleteObject();
	//��ʾͳ�ƵĲ���


	CString strStation="ͳ�Ƴ�վ��";
	strStation +=station_name;

	long loginshiftid =  m_nShiftID;

	CString strDate ="���: " + ConvertShiftIdToString(loginshiftid);
	CString temp;


	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("����"));
	pDC->SelectObject(&font);

	pDC->TextOut(rect.left+100,rect.top+250,strDate);
	pDC->TextOut(rect.left+100,rect.top+350,strStation);



	CFont font_title;
	font_title.CreateFont(-MulDiv(11,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("����"));
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(8,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("����"));

	//rect.InflateRect(-100,-630,-100,-200);
	rect.InflateRect(-100,-500,-100,-200);
	pDC->Rectangle(&rect);	     


	//Ҫ��ӡ������
	int row_count = m_ctrlStationStoreTrainList.GetItemCount()+1;
	CString side_name,side_name_next;
	side_name = "";
	side_name_next = "";

	int yStep =	150;//(rect.bottom-rect.top)/2/row_count;
	int i = 0;

	//�Զ������о�
	while((row_count+8)*yStep > (rect.bottom - rect.top))
		yStep-=5;

	//ʵ��������Ҫ���ϱ�������һ��
	for(i=0; i<=row_count; i++)  //�����Ļ���
	{

		if(i>=2)//��ʽ���ɵ����ߣ������������һ�йɵ���Ϊ�գ���ô������
		{
			side_name = m_ctrlStationStoreTrainList.GetItemText(i-2,0);

			if(i < row_count)
				side_name_next = m_ctrlStationStoreTrainList.GetItemText(i-1,0);
			else
				side_name_next = "temp";

			if( side_name_next == "")  //���йɵ���Ϊ�գ���ʾΪ����ɵ�����Ϣ̫����׷�ӵ��У�������
			{
				//pDC->MoveTo(rect.left,rect.top + yStep*2);
				//pDC->TextOut(rect.left+10,rect.top ,"�г�����");
				//pDC->LineTo(rect.right,rect.top + yStep*2);
			}
			else
			{
				pDC->MoveTo(rect.left,rect.top + yStep*i);
				pDC->LineTo(rect.right,rect.top + yStep*i);
			}
		}
		else //���������ĵ���
		{
			pDC->MoveTo(rect.left,rect.top + yStep*i);
			pDC->LineTo(rect.right,rect.top + yStep*i);
		}


	}


	//����Ϊ���У����ݱ���������
	float first_col,second_col,third_col;
	int total_col_width;
	first_col = m_ctrlStationStoreTrainList.GetColumnWidth(0);
	second_col = m_ctrlStationStoreTrainList.GetColumnWidth(1);
	third_col = m_ctrlStationStoreTrainList.GetColumnWidth(2);
	total_col_width = first_col + second_col + third_col;
	first_col = first_col / total_col_width;
	second_col = second_col / total_col_width;
	third_col = third_col / total_col_width;

	for(int j=1; j<=4; j++)
	{
		if(j==1)	//���Բ������;�����߿��ص�����һ����
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.top + yStep*row_count);
		}
		else if(j==2)
		{
			pDC->MoveTo(rect.left + first_col*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + first_col*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else if(j==3)
		{
			pDC->MoveTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top + yStep*row_count);
		}

	}

	//


	//���Ʊ���������

	CRect cellrect;
	int cell_left,cell_top,cell_right,cell_bottom;

	cell_left = rect.left;
	cell_top = rect.top + 15;
	cell_right = cell_left + first_col*(rect.right-rect.left);
	cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	oldfont = pDC->SelectObject(&font_title);
	pDC->DrawText(_gszStationStoreTrainLabel[0],cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + second_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText(_gszStationStoreTrainLabel[1],cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + third_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText(_gszStationStoreTrainLabel[2],cellrect,DT_CENTER);


	pDC->SelectObject(oldfont);



	int first_col_xpos,second_col_xpos,third_col_xpos;

	//��ʾ����
	row_count = row_count - 1; //���ݵ�����,row_count����ǰ�����˱�������һ��

	first_col_xpos = rect.left + 20;
	second_col_xpos = first_col_xpos + first_col*(rect.right - rect.left);
	third_col_xpos = second_col_xpos + second_col*(rect.right - rect.left);

	CString side_content,side_cars_num;

	for(i=1; i<=row_count; i++)  
	{

		side_name = m_ctrlStationStoreTrainList.GetItemText(i-1,0);
		side_content = m_ctrlStationStoreTrainList.GetItemText(i-1,1);
		side_cars_num = m_ctrlStationStoreTrainList.GetItemText(i-1,2);

		pDC->TextOut(first_col_xpos, rect.top+yStep*i+15,side_name);
		pDC->TextOut(second_col_xpos, rect.top+yStep*i+15,side_content);
		pDC->TextOut(third_col_xpos, rect.top+yStep*i+15,side_cars_num);

	}

	//�ж�����ͳ�����ݵ���ʼλ��

	int static_rect_top; 
	//ԭ������ʼλ���ڹɵ���Ϣ��Щ�������棬���Ǳ������ʼλ��������Ų
	if((row_count+1)*yStep > (rect.bottom-rect.top)/2)	
		static_rect_top = rect.top + (row_count+1)*yStep + 50 ;
	else
		static_rect_top = rect.top + (rect.bottom-rect.top)/2 +100;

	//���½���һ��RECT����ԭ�����ε��м䲿�ֿ�ʼ
	CRect static_rect(rect.left,static_rect_top,rect.right,rect.bottom);

	//���У�yStepͬ�����һ��
	yStep = (static_rect.top + yStep*8 > static_rect.bottom)?((static_rect.bottom-static_rect.top)/8):yStep;

	for(i=0; i<=6; i++)  //�����Ļ��ƣ���6��7������
	{
		pDC->MoveTo(static_rect.left,static_rect.top + yStep*i);
		pDC->LineTo(static_rect.right,static_rect.top + yStep*i);

	}

	int xStep = (static_rect.right - static_rect.left)/7;
	//���У�xStep�Լ����¼���
	for(i=0; i<7; i++)  //�����Ļ��ƣ���6��7�����ߣ����һ�������Ѿ�������������ػ�
	{
		pDC->MoveTo(static_rect.left + xStep*i,static_rect.top);
		pDC->LineTo(static_rect.left + xStep*i,static_rect.top + 6*yStep);
	}



	//���Ʊ���


	/*
	|------|-------|-------|-------|-------|--------|------|	<----������
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|	<----������
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|

	��һ��Ϊ�س��ճ���Ϣ
	^	
	|
	|
	|
	*/



	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15,static_rect.left + xStep*(i+1),static_rect.top+yStep);
		//tmp//pDC->DrawText(_gszCarriageType[i-1].lpszCarriageLongName ,cellrect,DT_CENTER);
	}

	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15+yStep*3,static_rect.left + xStep*(i+1),static_rect.top+yStep*4);
		//tmp//pDC->DrawText(_gszCarriageType[i-1+6].lpszCarriageLongName,cellrect,DT_CENTER);
	}

	//�����س��ճ�

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("�س�",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("�ճ�",cellrect,DT_CENTER);

	cell_top = cell_top + 2*yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("�س�",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("�ճ�",cellrect,DT_CENTER);

	//��ճ�����	

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;

	//��һ���س� 	

	CString tempstr;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://�񳵣�
			tempstr.Format("%d",m_pCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://����
			tempstr.Format("%d",m_cCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://ƽ�峵
			tempstr.Format("%d",m_pbCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://�͹޳�
			tempstr.Format("%d",m_yCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://ˮ�೵
			tempstr.Format("%d",m_sCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://���³�
			tempstr.Format("%d",m_bCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}


	//��һ�ſճ�
	//	m_pCount = m_cCount = m_pbCount = m_yCount = m_sCount = m_bCount = 0;
	cell_left = static_rect.left;
	cell_top = static_rect.top + 2*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://�񳵣�
			tempstr.Format("%d",m_pCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://����
			tempstr.Format("%d",m_cCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://ƽ�峵
			tempstr.Format("%d",m_pbCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://�͹޳�
			tempstr.Format("%d",m_yCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://ˮ�೵
			tempstr.Format("%d",m_sCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://���³�
			tempstr.Format("%d",m_bCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}

	cell_left = static_rect.left;
	cell_top = static_rect.top + 4*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://��Ʒ��
			tempstr.Format("%d",m_dCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://��װ�䳵
			tempstr.Format("%d",m_jzCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://ʯ����
			tempstr.Format("%d",m_szCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://�ͳ���
			tempstr.Format("%d",m_kCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://����ƽ��
			tempstr.Format("%d",m_axCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://��������
			tempstr.Format("%d",m_qCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}


	//�ڶ��ſճ� 	
	cell_left = static_rect.left;
	cell_top = static_rect.top + 5*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://��Ʒ��
			tempstr.Format("%d",m_dCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://��װ�䳵
			tempstr.Format("%d",m_jzCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://ʯ����
			tempstr.Format("%d",m_szCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://�ͳ���
			tempstr.Format("%d",m_kCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://����ƽ��
			tempstr.Format("%d",m_axCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://��������
			tempstr.Format("%d",m_qCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}
	//��ӡ����
	cell_left = static_rect.left + 15;
	cell_top = static_rect.top + yStep*7;
	cell_right = cell_left + 2*xStep;
	cell_bottom = cell_top + 2*yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	tempstr.Format("������%d",m_totalCount);

	pDC->DrawText(tempstr,cellrect,DT_LEFT);

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	font.DeleteObject();
	font_title.DeleteObject();
	font_detail.DeleteObject();
}

void CQueryStationCurrentStoreTrain::OnButtonPrint() 
{
	Beep(0xFF,60);
	CString disp_msg,station_name;
	CWnd* cwnd_cur_std;
	cwnd_cur_std = GetDlgItem(IDC_SCST_STATIONNAME);
	cwnd_cur_std->GetWindowText(station_name);

	disp_msg = "��ӡ" + station_name + "վ�ִ泵��Ϣ?";
	if(AfxMessageBox(disp_msg,MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
		return; 


	PrintSingleStation();
}

CString CQueryStationCurrentStoreTrain::ConvertShiftIdToString(unsigned long id)
{

	CString date,banbie;

	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d��%2d��%2d��",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "��һ��" : "�ڶ���";

	CString shift_id_string;
	shift_id_string = date + banbie;
	return shift_id_string;
}

//��ȡ��ӡ�豸����
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

BOOL CQueryStationCurrentStoreTrain::PrintSingleStation()
{
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("�޷���ȡָ���Ĵ�ӡ���豸\n"));
		return FALSE;
	}

	CPrintInfo pInfo;
	HDC prnDc;

	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();

	CDC printDC;
	printDC.Attach(prnDc);

	DOCINFO printDocumentName;
	CString sPrintName;
	sPrintName.Format("%s",gpcdef.GetStationName(m_nCurrentSelectStation));
	sPrintName = sPrintName + "��վ�ִ泵��ӡ";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	printDC.StartPage( );

	OnPrint(&printDC,&pInfo);

	printDC.EndPage( );

	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
	return TRUE;
}

int CQueryStationCurrentStoreTrain::GetChineseSplitFlag(CString &chinesestr)
{

	int len = chinesestr.GetLength();
	char curChar;
	int chineseCount = 0;

	for(int i=0; i<len; i++)
	{
		curChar = chinesestr.GetAt(i);

		if(curChar<0)
		{
			chineseCount++;
		}

	}

	return chineseCount%2;
}

void CQueryStationCurrentStoreTrain::ShowTrainStatisticInfo()
{
	CString text, sTrainTypeName, strTitle;
	
	for(int i=0;i < m_pBasicPara->m_carKindType.size();i++)    
	{  
		for(int k=0; k<m_pBasicPara->m_carKindType[i].carKind.size(); k++)
		{   
			sTrainTypeName = m_pBasicPara->m_carKindType[i].carKind[k].name;

			for(int j=0; j<m_ctrlTrainStatisticList.GetItemCount();j++)
			{
				strTitle = m_ctrlTrainStatisticList.GetItemText(j, 0);
				if(strTitle == sTrainTypeName)
				{
					if(m_pBasicPara->m_carKindType[i].carKind[k].count>0)
						text.Format("%d",m_pBasicPara->m_carKindType[i].carKind[k].count);
					else 
						text.Empty();
					m_ctrlTrainStatisticList.SetItemText(j,1+i,text);
					
					break;
				}
			}
		}
	}
}

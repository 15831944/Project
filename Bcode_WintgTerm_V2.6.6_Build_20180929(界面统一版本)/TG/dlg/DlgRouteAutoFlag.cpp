// DlgRouteAutoFlag.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgRouteAutoFlag.h"
#include "IF.h"

#include ".\dlgrouteautoflag.h"


#define COLUMN_COUNT  5

_TCHAR *_gszRouteAutoFlag[COLUMN_COUNT] =
{
	_T("���"),_T("��վ"),_T("����"),_T("����ӳ���·�Դ�"),_T("��������·�Դ�")
};


// CDlgRouteAutoFlag �Ի���
IMPLEMENT_DYNAMIC(CDlgRouteAutoFlag, CDialog)
CDlgRouteAutoFlag::CDlgRouteAutoFlag(CTGDoc *pDoc, const CTG_TRAIN *single,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRouteAutoFlag::IDD, pParent)
{
	m_pDoc = pDoc;
	m_nTrain = *single;
	m_nCount = 0;
	m_pGridCtrl = NULL;
}

CDlgRouteAutoFlag::~CDlgRouteAutoFlag()
{
	delete m_pGridCtrl;
	m_pGridCtrl = NULL;
}

void CDlgRouteAutoFlag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRouteAutoFlag, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_ARRIVAL_ALL, OnBnClickedArrivalAll)
	ON_BN_CLICKED(IDC_DEPART_ALL, OnBnClickedDepartAll)
END_MESSAGE_MAP()


BOOL CDlgRouteAutoFlag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	initialGridCtrl();
	fillGrid();
	
	
	// TODO: Add extra initialization here
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT) && !m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd* pwnd = GetDlgItem(IDOK);
		if(pwnd)
			pwnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgRouteAutoFlag::initialGridCtrl()
{
	int col;
	if (m_pGridCtrl==NULL)
		m_pGridCtrl= new CGridCtrl;
	if(!m_pGridCtrl) 
		return ;
	
	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID);
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_GRID);
	try{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(1);
		m_pGridCtrl->SetColumnCount(COLUMN_COUNT);           
		m_pGridCtrl->SetRowCount(1);
		m_pGridCtrl->SetFixedTextColor(CLR_BLUE);      //�ı���ɫ
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
       	m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		m_pGridCtrl->SetFixedColumnSelection(TRUE);
		m_pGridCtrl->SetFixedRowSelection(TRUE);

	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
	
    for (col = 0; col < COLUMN_COUNT; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		if(col == 0)
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW|GVIS_FIXEDCOL;
		}
		else
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW;
		}
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(_gszRouteAutoFlag[col]);
		m_pGridCtrl->SetItem(&Item);
		
	}
	m_pGridCtrl->AutoSizeColumns();
}

void CDlgRouteAutoFlag::fillGrid()
{
	CString trainid = m_pDoc->GetTrainShowId(m_nTrain);
	if(!trainid.IsEmpty())
	{
		CString title;
		GetWindowText(title);
		title="�����г�--"+trainid+"--"+title;
		SetWindowText(title);
	}
	int count = 1;
	
	if(NULL == m_pGridCtrl)
		return;
	m_pGridCtrl->DeleteNonFixedRows();
	
	int nRecCount;
    nRecCount = m_nTrain.GetRecordNum();
	m_pGridCtrl->SetRowCount(nRecCount + 1);
    TgTRAIN_RECORD *rec;
	CGridCellCheck *pCell;
	for(int i = 0; i< nRecCount;i++)
	{		 
		rec = m_nTrain.GetRecordByPosition(i);		 
		if(rec != NULL)
		{
			bool bIsNotThisSectionStation = m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station);
			//0 ���
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nState = GVIS_READONLY;
			Item.row = i+1;  //��
			Item.col = 0;  //��
			Item.crBkClr = CLR_RED;        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			Item.strText.Format("%d",i+1);
			m_pGridCtrl->SetItem(&Item);            

            //1 ��վ
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nState = GVIS_READONLY;
			Item.row = i+1;  //��
			Item.col = 1;  //��
			Item.crBkClr = CLR_RED;        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			Item.strText.Format("%s", m_pDoc->GetStationName(rec->station) );

			m_pGridCtrl->SetItem(&Item);            

			//2 ������ͨ��
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nState = GVIS_READONLY;
			Item.row = i+1;  //��
			Item.col = 2;  //��
			Item.crBkClr = CLR_RED;        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			if(IsFlagPass((*rec)))
			   Item.strText.Format("ͨ��");
			else if(IsFlagStop((*rec)))
			   Item.strText.Format("����");
		 
			m_pGridCtrl->SetItem(&Item);            

			//3 �ӳ�
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nState = GVIS_READONLY;
			Item.row = i+1;  //��
			Item.col = 3;  //��
			Item.crBkClr = CLR_RED;        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			Item.strText.Empty();
	 
			if(bIsNotThisSectionStation)
			{
				Item.strText="��Ȩ�޸�";
				m_pGridCtrl->SetItem(&Item);     
			}
			else
			{
				m_pGridCtrl->SetCellType(i+1,3,RUNTIME_CLASS(CGridCellCheck));
				m_pGridCtrl->SetItem(&Item);            

				pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,3);
				if(pCell)
				{
					if(IsAutoMakeArriveRoute(rec->stop_condition))
					pCell->SetCheck(TRUE);
					else  
					pCell->SetCheck(FALSE);
				}
			}
			 
			//4 ����
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nState = GVIS_READONLY;
			Item.row = i+1;  //��
			Item.col = 4;  //��
			Item.crBkClr = CLR_RED;        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			Item.strText.Empty();
			if(bIsNotThisSectionStation)
			{
				Item.strText="��Ȩ�޸�";
				m_pGridCtrl->SetItem(&Item);     
			}
			else
			{
				m_pGridCtrl->SetCellType(i+1,4,RUNTIME_CLASS(CGridCellCheck));
				m_pGridCtrl->SetItem(&Item);          

           		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,4);
				if(pCell)
				{
					if(IsAutoMakeDepartRoute(rec->stop_condition))
						pCell->SetCheck(TRUE);
					else  
						pCell->SetCheck(FALSE);
				}
			}
		}
	}
    m_pGridCtrl->AutoSizeColumns();
}

void CDlgRouteAutoFlag::OnBnClickedOk()
{
    SetRouteAutoFlag();
	OnOK();
 }

void CDlgRouteAutoFlag::SetRouteAutoFlag(void)
{
	if(NULL == m_pGridCtrl)
		return;

	int nRecCount;
    nRecCount = m_nTrain.GetRecordNum();
    TgTRAIN_RECORD *rec;
	CGridCellCheck *pCell;
	for(int i = 0; i < nRecCount; i++)
	{		 
		rec = m_nTrain.GetRecordByPosition(i);		 
		if(rec != NULL)
		{
			bool bIsNotThisSectionStation = m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station);
			if(bIsNotThisSectionStation)
				continue;

			//3 �ӳ�       
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,3);
			if(pCell)
			{
				if(pCell->GetCheck())
				{
					if(!IsAutoMakeArriveRoute(rec->stop_condition))
					{
						m_pDoc->sysprint(4,0,"����Ա�޸�%s���г� %s �ӳ���·�����Դ�", 
							m_pDoc->GetTrainShowId(m_nTrain), m_pDoc->GetStationName(rec->station));

						SetAutoMakeArriveRoute(rec->stop_condition);
					}
				}
				else
				{
					m_pDoc->sysprint(4,0,"����Ա�޸�%s���г� %s �ӳ���·�������Դ�", 
							m_pDoc->GetTrainShowId(m_nTrain), m_pDoc->GetStationName(rec->station));

					SetNotAutoMakeArriveRoute(rec->stop_condition);
				}
			}
			 			
			//4 ����
           	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,4);
			if(pCell)
			{
				if(pCell->GetCheck())
				{
					if(!IsAutoMakeDepartRoute(rec->stop_condition))
					{
						m_pDoc->sysprint(4,0,"����Ա�޸�%s���г� %s ������·�����Դ�", 
							m_pDoc->GetTrainShowId(m_nTrain), m_pDoc->GetStationName(rec->station));

						SetAutoMakeDepartRoute(rec->stop_condition);
					}
				}
				else
				{
					m_pDoc->sysprint(4,0,"����Ա�޸�%s���г� %s ������·�������Դ�", 
							m_pDoc->GetTrainShowId(m_nTrain), m_pDoc->GetStationName(rec->station));

					SetNotAutoMakeDepartRoute(rec->stop_condition);
				}
			}		
		}
	}
}

void CDlgRouteAutoFlag::OnBnClickedArrivalAll()
{
	CButton *pButton = (CButton*)GetDlgItem(IDC_ARRIVAL_ALL);
    BOOL bArrivalALL;
	bArrivalALL = pButton->GetCheck ();
 
	if(NULL == m_pGridCtrl)
		return; 
	
    int nRecCount = m_nTrain.GetRecordNum();
    TgTRAIN_RECORD *rec;
	CGridCellCheck *pCell;
	for(int i = 0; i< nRecCount;i++)
	{		 
		rec = m_nTrain.GetRecordByPosition(i);		 
		if(rec != NULL)
		{
			bool bIsNotThisSectionStation = m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station);
			if(bIsNotThisSectionStation)
				continue;

			//3 �ӳ�       
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,3);
			if(pCell)
			{
				if(bArrivalALL)
				{
					pCell->SetCheck(1); 
				}
				else
				{	
					pCell->SetCheck(0);
				}
			}			 				       
		}
	}
 }

void CDlgRouteAutoFlag::OnBnClickedDepartAll()
{
	CButton *pButton = (CButton*)GetDlgItem(IDC_DEPART_ALL);
    BOOL bDepartALL;
	bDepartALL = pButton->GetCheck ();
 
	if(NULL == m_pGridCtrl)
		return;

	int nRecCount;
    nRecCount = m_nTrain.GetRecordNum();
    TgTRAIN_RECORD *rec;
	CGridCellCheck *pCell;
	for(int i = 0; i< nRecCount;i++)
	{		 
		rec = m_nTrain.GetRecordByPosition(i);		 
		if(rec != NULL)
		{
			bool bIsNotThisSectionStation = m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station);
			if(bIsNotThisSectionStation)
				continue;

			//4 ����       
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,4);
			if(pCell)
			{
				if(bDepartALL)
				{	
					pCell->SetCheck(1);
				}
				else
				{
					pCell->SetCheck(0);
				}
			}			 				       
		}
	}
 }

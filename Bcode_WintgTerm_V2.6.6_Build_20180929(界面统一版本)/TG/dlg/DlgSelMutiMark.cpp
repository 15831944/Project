// DlgSelMutiMark.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "DlgSelMutiMark.h"
#include ".\dlgselmutimark.h"

#define COLUMN_COUNT  5

_TCHAR *_gszBlockTitle[COLUMN_COUNT] =
{
	_T("����"),_T("��������"),_T("����"),_T("��ʼʱ��"),_T("����ʱ��")
};
// CDlgSelMutiMark �Ի���

IMPLEMENT_DYNAMIC(CDlgSelMutiMark, CDialog)
CDlgSelMutiMark::CDlgSelMutiMark(CTGDoc *pDoc, CWnd* pParent)
	: CDialog(CDlgSelMutiMark::IDD, pParent)
{
	m_pDoc = pDoc;
	memset(mSelMarkIndex, 0, sizeof(mSelMarkIndex));
	mSelMarkCount=0;
	m_pGridCtrl=NULL;
}

CDlgSelMutiMark::~CDlgSelMutiMark()
{
	if(m_pGridCtrl!=NULL) 
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CDlgSelMutiMark::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_btm);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_etm);
}


BEGIN_MESSAGE_MAP(CDlgSelMutiMark, CDialog)
	ON_BN_CLICKED(IDC_BTN_FIND, OnBnClickedBtnFind)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelMutiMark ��Ϣ�������
BOOL CDlgSelMutiMark::OnInitDialog() 
{
	CDialog::OnInitDialog();
	 
	int col;
	m_pGridCtrl= new CGridCtrl;
	if(!m_pGridCtrl) 
		return FALSE;
	
	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID2);
	if(pWnd == NULL) 
		return FALSE;

	m_btm.SetFormat("yyyy-MM-dd HH:mm:ss");
	m_etm.SetFormat("yyyy-MM-dd HH:mm:ss");

	CTime tm = CTime::GetCurrentTime();

	CTime btm = tm;
	btm-=CTimeSpan(0, 0, 0, 0);
	m_btm.SetTime(&btm);
	
	CTime etm = tm;
	etm+=CTimeSpan(0, 6, 0, 0);
	m_etm.SetTime(&etm);

	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_GRID2);
	try{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(0);
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
		return FALSE;
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
		Item.strText.Format(_gszBlockTitle[col]);
		m_pGridCtrl->SetItem(&Item);
		
	}
	m_pGridCtrl->AutoSizeColumns();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelMutiMark::OnBnClickedBtnFind()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pGridCtrl==NULL)
		return;

	m_pGridCtrl->DeleteNonFixedRows();
	m_pGridCtrl->Invalidate();
	
	if(NULL == m_pDoc)
		return;

	if(NULL == m_pDoc->myPDataManage)
		return;

	CTime btm, etm;
	m_btm.GetTime(btm);
	m_etm.GetTime(etm);

	if(btm >= etm)
	{
		AfxMessageBox("���ҽ���ʱ�������ڿ�ʼʱ��");
		return;
	}

	ULONG mark_index[2048], index_cnt=2048;
	memset(mark_index, 0, sizeof(mark_index));
	index_cnt=m_pDoc->myPDataManage->FindInvisibleMark(btm.GetTime(), etm.GetTime(), mark_index, index_cnt,m_pDoc->m_nDispDistanceNum);
	
	m_pGridCtrl->SetRowCount(index_cnt + 1);  

	CGridCellCheck *pCell;
	for(int i = 0; i < index_cnt; i++)
	{		 
		CTg_BLOCK* block = m_pDoc->myPDataManage->GetMark(mark_index[i]);
		if(NULL == block)
			continue;
		//0 ���
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //��
		Item.col = 0;  //��
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d", block->mark_index);
		m_pGridCtrl->SetItem(&Item);

		//1 ʩ������
		CString strType;
		switch(block->type)
		{
		case ABS_INHIBIT_MARK:
			strType="�������";
			break;
		case ABS_SLOW_MARK:
			strType="��������";
			break;
		case ABS_FIX_MARK:
			strType="�����ۺ��촰��";
			break;
		case NEW_ABS_ELE_FIX_MARK:
			strType="�����������";
			break;
		case ABS_ELE_FIX_MARK:
			strType="�����������";
			break;
		case SELF_CLOSE_MARK:
			strType="������������ʽ";
			break;
		case SIDE_INHIBIT_MARK:
			strType="վ�ڷ���";
			break;
		case SIDE_SLOW_MARK:
			strType="վ������";
			break;
		case SIDE_OCCUPY_MARK:
			strType="վ��ռ��";
			break;
		case SIDE_TIANCHUANG_MARK:
			strType="վ���촰��";
			break;
		case GRAPH_NOTE_MARK:
			strType="ͼ��ע��";
			break;
			case NEW_GRAPH_NOTE_MARK:
			strType="����ע��";
			break;
		default:
			strType.Format("���� %d", block->type);
		}
	
		//1 ��������
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //��
		Item.col = 1;  //��
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", strType);
		m_pGridCtrl->SetItem(&Item);  

		//2 ����
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //��
		Item.col = 2;  //��
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", block->text);
		m_pGridCtrl->SetItem(&Item);  

		//3 ��ʼʱ��
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //��
		Item.col = 3;  //��
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		
		CTime tm(block->start_time);
		Item.strText=tm.Format("%Y-%m-%d %H:%M");
		
		m_pGridCtrl->SetItem(&Item);  

		//4 ����ʱ��
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = i+1;  //��
		Item.col = 4;  //��
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		
		CTime tm1(block->end_time);
		Item.strText=tm1.Format("%Y-%m-%d %H:%M");

		m_pGridCtrl->SetItem(&Item);  

		m_pGridCtrl->SetCellType(i+1,0,RUNTIME_CLASS(CGridCellCheck));
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,0);
		if(pCell)
		{
			pCell->SetCheck(FALSE);
		} 
	}

	m_pGridCtrl->AutoSizeColumns();

	if(0 == index_cnt)
	{
		AfxMessageBox("��ע��:�ڸ���ʱ�䷶Χ��, ���Ҳ��� �޷���ʾ��ʩ������");
	}
	return;
}
void CDlgSelMutiMark::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	memset(mSelMarkIndex, 0, sizeof(mSelMarkIndex));
	mSelMarkCount=0;

	for(int i=1; i<m_pGridCtrl->GetRowCount(); i++)
	{
		CGridCellCheck* pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,0);
		if(pCell->GetCheck())
		{
			CString text = m_pGridCtrl->GetItemText(i, 0);
			ULONG tmp = atoi(text);
			if(tmp > 0)
			{
				mSelMarkIndex[mSelMarkCount]=tmp;
				mSelMarkCount++;
			}
		}
	}
	OnOK();
}

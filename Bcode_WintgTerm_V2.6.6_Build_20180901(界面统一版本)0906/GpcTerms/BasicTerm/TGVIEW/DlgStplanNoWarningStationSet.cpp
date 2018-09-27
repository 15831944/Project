
#include "stdafx.h"
#include "resource.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgStplanNowarningStationSet.h"
#include ".\DlgStplanNowarningStationSet.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
BOOL CStationSelectGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"��վ");
	sprintf(m_result_def[m_result_col_count].attr_name,"station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"������");
	sprintf(m_result_def[m_result_col_count].attr_name,"b");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=2;
	m_result_col_count++;
	
	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 120);
	SetColumnWidth(1, 80);
			
	return TRUE;
}

void  CStationSelectGrid::AddRow(int station, int icheck)
{
	int row=InsertRow("");
	if(row<0)
		return;
	CString sta_name=gpcdef.GetStationName(station);
	for (int col = 0; col < m_nFieldNum; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;
		Item.col = col;
		if(m_bSingleLine)
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
		else
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_NOPREFIX;
		Item.lParam=0;

		if(col==0)
		{
			Item.strText = sta_name;
			SetItem(&Item);
			GetCell(row,0)->SetData(station);
		}
		else
		{
			Item.strText = "";
			SetItem(&Item);
			SetCellType(row ,col, RUNTIME_CLASS(CGridCellCheck));
			if(icheck==1)
				((CGridCellCheck*)GetCell(row,col))->SetCheck(TRUE);
			else
				((CGridCellCheck*)GetCell(row,col))->SetCheck(FALSE);
		}
	}
}

CDlgStplanNowarningStationSet::CDlgStplanNowarningStationSet(int section, int station_count, int station_list[256], CWnd* pParent)
: CDialog(CDlgStplanNowarningStationSet::IDD, pParent)
{
	m_section=section;
	m_station_count=station_count;
	memset(m_station_list, 0, sizeof(m_station_list));
	memset(m_check_list, 0, sizeof(m_check_list));
	for(int i=0; i<station_count; i++)
	{
		m_station_list[i]=station_list[i];
	}
}

CDlgStplanNowarningStationSet::~CDlgStplanNowarningStationSet()
{
}

void CDlgStplanNowarningStationSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnSave);
}

BEGIN_MESSAGE_MAP(CDlgStplanNowarningStationSet, CDialog)
	ON_COMMAND(IDC_BUTTON1,OnBnClickedSave)
END_MESSAGE_MAP()

BOOL CDlgStplanNowarningStationSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	StationStplanNotWarning stations;
	GetTGFunctionConfig()->LoadStplanNoWarningStation(m_section, stations);

	CStatic *pWnd;
	pWnd = (CStatic *)GetDlgItem(IDC_GRID4);
	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(&rect);

	mGridCtrl.CreateGrid(this,rect,IDC_GRID4);
	mGridCtrl.SetSingleRowSelection(TRUE);
	mGridCtrl.SetEditable(TRUE);
	for (int row = 0; row < m_station_count; row++)
	{
		int station=m_station_list[row];
		int icheck=0;
		for(int n=0; n<stations.station_count; n++)
		{
			if(station==stations.station_lists[n])
			{
				icheck=1;
				break;
			}
		}
		mGridCtrl.AddRow(station, icheck);
	}

	

	for(int row=1;row<mGridCtrl.GetRowCount();row++ )
	{
		const int station = mGridCtrl.GetCell(row,0)->GetData();
		CGridCellCheck *pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,1);
		if(pCell)
		{
			
		}
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CDlgStplanNowarningStationSet::OnBnClickedSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	str.Format("%s\\user_cfg\\NoWarningStplanStationList-%d.xml", gpcdef.GetDataDir(),m_section);
	CStdioFile file;
	if(!file.Open(str, CFile::modeCreate | CFile::modeReadWrite))
	{
		MessageBox("����ʧ��,��ȷ��dataĿ¼���Ƿ����Ŀ¼user_cfg!", "����");
		return;
	}
	file.WriteString("<root>");
	CString line;
	for(int row=1;row<mGridCtrl.GetRowCount();row++ )
	{
		int station = mGridCtrl.GetCell(row,0)->GetData();
		int c=0;
		CGridCellCheck *pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,1);
		if(pCell && (pCell->GetCheck()==TRUE))
		{
			c=1;
		}
		
		line.Format("\n\t<station no=\"%d\" sel=\"%d\" />", 
			station, c);

		file.WriteString(line);
	}
	file.WriteString("\n</root>");
	file.Close();

	CString str1;
	str1.Format("�׶μƻ���������վ���óɹ�,�����%s��", str);
	MessageBox(str1, "��ʾ");
}


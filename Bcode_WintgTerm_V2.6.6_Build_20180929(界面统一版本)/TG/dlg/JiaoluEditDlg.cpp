// CZHManage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"

#include "JiaoLuEditDlg.h"
#include "JiaoluTrainSelDlg.h"
#include ".\jiaolueditdlg.h"

#define   IDC_CZH_GRID_ID  55323
// CZHMultiEditDlg �Ի���
static void GetCZHGridHeader(CString& xml)
{
	CString tmp;
	xml = "<FIELDS>";
	tmp.Format("<FIELD caption=\"��\" attr_name=\"ID\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"���κ�\" attr_name=\"TRAIN_ID\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��̨��ʼ\" attr_name=\"CON_ST_STATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��̨����\" attr_name=\"CON_ED_STATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��ʼʱ��\" attr_name=\"CON_ST_TIME\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"����ʱ��\" attr_name=\"CON_ED_TIME\" />");
	xml += tmp;
	
	xml += "</FIELDS>";
}

IMPLEMENT_DYNAMIC(CJiaoLuEditDlg, CDialog)
CJiaoLuEditDlg::CJiaoLuEditDlg(CString title, TRAIN_INDEX train_index, CTGDoc *pDoc, CWnd* pParent)
	: CDialog(CJiaoLuEditDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	sel_train_index=train_index;
	m_title=title;
}

CJiaoLuEditDlg::~CJiaoLuEditDlg()
{
}

void CJiaoLuEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CZH, m_group);

	DDX_CBString(pDX, IDC_EDIT2, m_trainid);
	DDV_MaxChars(pDX, m_trainid, 9);
}


BEGIN_MESSAGE_MAP(CJiaoLuEditDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_CZH, OnShowSelTrainRelation)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON5, OnBnAddTrainRelation)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnDelTrainRelation)
	ON_MESSAGE(MSG_UPDATE_RELATION, OnUpdateTrainRelation)
	ON_MESSAGE(10101, OnSelectTrain)	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedRefresh)
END_MESSAGE_MAP()


// CZHMultiEditDlg ��Ϣ�������
int   CJiaoLuEditDlg::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	CString xml;
	GetCZHGridHeader(xml);
	
	m_grid.CreateGrid(xml, this,rect,IDC_CZH_GRID_ID);
	m_grid.SetEditable(FALSE);

	return 0;
}

void  CJiaoLuEditDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
	{
		m_grid.MoveWindow(1,35,cx-1,cy-130);
	}

	CDialog::OnSize(nType,cx,cy);
}

BOOL  CJiaoLuEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CFont font;
	font.CreateFont(18, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();

	CEdit* pEdit= NULL;
	pEdit= (CEdit*)GetDlgItem(IDC_EDIT2);
	if(pEdit)
		pEdit->SetLimitText(9);
	
	UpdateTrainRelationGroup();
	
	m_grid.AutoWidth();
	m_grid.Refresh();

	this->SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CJiaoLuEditDlg::OnUpdateTrainRelation(WPARAM wParam, LPARAM lParam)
{
	UpdateTrainRelationGroup();
	return 0;
}

void CJiaoLuEditDlg::OnShowSelTrainRelation()
{
	CWnd* pWnd=GetDlgItem(IDC_EDIT12);
	if(pWnd)
		pWnd->SetWindowText("");

	pWnd=GetDlgItem(IDC_EDIT11);
	if(pWnd)
		pWnd->SetWindowText("");
	
	ShowCZHTrains();

	SetDeleteTrainInfo("", "");
}

void CJiaoLuEditDlg::ShowCZHTrains()
{
	m_grid.DeleteNonFixedRows();
	m_grid.Refresh();

	int nSel=m_group.GetCurSel();
	if(nSel<0 || nSel>=mListRL.size())
		return;

	TrainGroupInfo info=mListRL[nSel];
	CString xml, tmp;
	xml="<CZH>";
	for(int i=0; i<info.train_count; i++)
	{
		xml+="<RECORD>";
		
		if(i==0)
			sel_train_index=info.trains[i].train_index;
		tmp.Format("<TRAIN_INDEX>%d</TRAIN_INDEX>", info.trains[i].train_index);
		xml += tmp;

		CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(info.trains[i].train_index);
		CString strTrain;
		if(ptrain)
			strTrain=m_pDoc->GetTrainShowId(ptrain);
		tmp.Format("<TRAIN_ID>%s</TRAIN_ID>", strTrain);
		xml += tmp;
		
		CWnd* pWnd=GetDlgItem(IDC_EDIT12);
		if(pWnd)
			pWnd->SetWindowText(strTrain);

		pWnd=GetDlgItem(IDC_EDIT11);
		if(pWnd)
		{
			CString trainindexstr;
			trainindexstr.Format("%d",info.trains[i].train_index);
			pWnd->SetWindowText(trainindexstr);
		}

		tmp.Format("<CON_ST_STATION>%s(%s)</CON_ST_STATION>", 
			m_pDoc->GetStationName(info.trains[i].begin_station),
			m_pDoc->GetSideName(info.trains[i].begin_station, info.trains[i].begin_side));
		xml += tmp;

		tmp.Format("<CON_ED_STATION>%s(%s)</CON_ED_STATION>", 
			m_pDoc->GetStationName(info.trains[i].end_station),
			m_pDoc->GetSideName(info.trains[i].end_station, info.trains[i].end_side));
		xml += tmp;

		if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
		{
			CString day, time;
			if(info.trains[i].stTime>0)
			{
				day.Format("%d", info.trains[i].stTime/86400);
				time = ConvertTimetoString(info.trains[i].stTime%86400);
				tmp.Format("<CON_ST_TIME>%s-%s</CON_ST_TIME>", day, time);
				xml += tmp;
			}
			if(info.trains[i].edTime>0)
			{
				day.Format("%d", info.trains[i].edTime/86400);
				time = ConvertTimetoString(info.trains[i].edTime%86400);
				tmp.Format("<CON_ED_TIME>%s-%s</CON_ED_TIME>", day, time);
				xml += tmp;
			}
		}
		else
		{
			if(info.trains[i].stTime>0)
			{
				CTime tm(info.trains[i].stTime);
				tmp.Format("<CON_ST_TIME>%s</CON_ST_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}

			if(info.trains[i].edTime>0)
			{
				CTime tm(info.trains[i].edTime);
				tmp.Format("<CON_ED_TIME>%s</CON_ED_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}
		}

		xml+="</RECORD>";
	}

	xml+="</CZH>";

	m_grid.AddRowDataFromXML(xml, "CZH/RECORD");
	m_grid.Refresh();
}

void CJiaoLuEditDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DestroyWindow();
}

void CJiaoLuEditDlg::OnBnClickedRefresh()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateTrainRelationGroup();
}

void CJiaoLuEditDlg::UpdateTrainRelationGroup()
{
	m_group.ResetContent();

	int nSel=-1;
	mListRL.clear();
	m_pDoc->myPDataManage->GetTrainRelation(mListRL,true);
	
	for(int i=0; i<mListRL.size(); i++)
	{
		int index=m_group.AddString(mListRL[i].strName);
		if(mListRL[i].IsInThisGroup(sel_train_index))
		{
			nSel=index;
		}
	}

	if(nSel>=0)
	{
		m_group.SetCurSel(nSel);
	}

	OnShowSelTrainRelation();
}

CString CJiaoLuEditDlg::GetSelectJiaolu()
{
	CString czh="";

	int nSel=m_group.GetCurSel();
	if(nSel>=0)
	{
		m_group.GetLBText(nSel, czh);
	}
	else
	{
		MessageBox("��ѡ����Ҫ�༭�Ľ�·", "��ʾ", MB_OK|MB_ICONQUESTION);
	}

	return czh;
}

void CJiaoLuEditDlg::OnBnAddTrainRelation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString czh = GetSelectJiaolu();
	if(czh.IsEmpty())
		return;

	CString strTrainIndex,strBeforeTrain, trainid;
	CWnd* pWnd=GetDlgItem(IDC_EDIT2);
	if(pWnd)
		pWnd->GetWindowText(trainid);

	pWnd=GetDlgItem(IDC_EDIT11);
	if(pWnd)
		pWnd->GetWindowText(strTrainIndex);
	pWnd=GetDlgItem(IDC_EDIT12);
	if(pWnd)
		pWnd->GetWindowText(strBeforeTrain);

	if(strTrainIndex.IsEmpty() || strBeforeTrain.IsEmpty())
	{
		MessageBox("����û��ǰ���г�", "��ʾ", MB_OK|MB_ICONQUESTION);
		return;
	}
	if(trainid.IsEmpty())
	{
		MessageBox("������Ҫ�����ĳ���", "��ʾ", MB_OK|MB_ICONQUESTION);
		return;
	}

	CString text;
	text.Format("��ȷʵҪ�ѳ��� %s ��Ϊ %s �ĺ����۷�������?", trainid, strBeforeTrain);
	TRAIN_INDEX prev_train_index=atol(strTrainIndex);
	if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION)!=IDYES)
	{
		return;
	}
	CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(prev_train_index);
	if(ptrain==NULL)
		return;
	
	TgTRAIN_RECORD* prec=ptrain->GetLastRecord();
	if(prec==NULL)
		return;
	
	if(!IsFlagTerminal(prec->flag))
	{
		MessageBox("ǰ���г������յ�,���ܹ���������·", "��ʾ", MB_OK|MB_ICONHAND);
		return;
	}
	CStringArray list_trainno;
	CDWordArray list_trainindex;
	TRAIN_INDEX next_train_index=0;
	m_pDoc->myPDataManage->GetMaybeNextTrains(prec->station,prec->arrive, trainid,list_trainno,list_trainindex);
	if(list_trainno.GetCount()==0)
	{
		MessageBox("���Ҳ���������ĳ���", "��ʾ", MB_OK|MB_ICONHAND);
		return;
	}
	else if(list_trainno.GetCount()==1)
	{
		next_train_index=list_trainindex.GetAt(0);
	}
	else
	{
		AfxMessageBox("����������ĳ���, �ҵ�����г�, ��ѡ��!!");
		CJiaoluTrainSelDlg dlg(this, list_trainno, list_trainindex);
		if(dlg.DoModal()==IDOK)
		{
			next_train_index=dlg.m_return_trainindex;
		}
	}

	if(next_train_index != 0)
	{
		ptrain=m_pDoc->GetTrainByIndex(next_train_index);
		if(ptrain)
		{
			m_pDoc->sysprint(4,0,"[��������]����Ա�����г� %s(%u) �� %s(%u).", 
				trainid, next_train_index, strBeforeTrain, prev_train_index);

			CTG_TRAIN train=*ptrain;
			train.prev_train_index=prev_train_index;
			m_pDoc->SendUpdateTrain(train);
		}
	}
	
	return;
}

LRESULT CJiaoLuEditDlg::OnSelectTrain(WPARAM wParam,LPARAM lParam)
{
	ULONG trainindex=(ULONG)lParam;
	CString strSelTrainid=(char*)wParam;
		
	CString trainindexstr;
	trainindexstr.Format("%d", trainindex);
	SetDeleteTrainInfo(strSelTrainid, trainindexstr);
	return TRUE;
}

void CJiaoLuEditDlg::OnBnDelTrainRelation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString czh = GetSelectJiaolu();
	if(czh.IsEmpty())
		return;

	CString trainid, strtrainindex;
	CWnd* pWnd=GetDlgItem(IDC_EDIT6);
	if(pWnd)
		pWnd->GetWindowText(strtrainindex);
	pWnd=GetDlgItem(IDC_EDIT8);
	if(pWnd)
		pWnd->GetWindowText(trainid);

	if(strtrainindex.IsEmpty() || trainid.IsEmpty())
	{
		MessageBox("��ѡ��Ҫ���������ϵ�ĳ���", "��ʾ", MB_OK|MB_ICONQUESTION);
		return;
	}

	TRAIN_INDEX train_index=atol(strtrainindex);
	CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(train_index);
	if(ptrain==NULL)
	{
		CString text;
		text.Format("�޷���ȡҪ��������ĳ��� %s", trainid);
		MessageBox(text, "��ʾ", MB_OK|MB_ICONHAND);
		return;
	}

	CString text;
	text.Format("��ȷʵҪ����г� %s ��ǰ������ϵ��?", trainid);
	if(IDYES==MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION))
	{
		CTG_TRAIN train=*ptrain;

		m_pDoc->sysprint(4,0,"[��������]����Ա����г� %s(%u)�Ĺ�����ϵprev_train_index=%u", 
				trainid, train_index, train.prev_train_index);

		train.prev_train_index=0;
		m_pDoc->SendUpdateTrain(train);
	}
	
	return;
}

void CJiaoLuEditDlg::PostNcDestroy() 
{
	delete this;
}

void CJiaoLuEditDlg::SetDeleteTrainInfo(CString trainid, CString trainindexstr)
{
	CWnd* pWnd=GetDlgItem(IDC_EDIT8);
	if(pWnd)
		pWnd->SetWindowText(trainid);

	pWnd=GetDlgItem(IDC_EDIT6);
	if(pWnd)
		pWnd->SetWindowText(trainindexstr);
}



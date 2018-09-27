// TransportInfo.cpp : implementation file
//

#include "stdafx.h"
#include ".\DepartPredictDlg.h"
#include "tgdocument.h"
#include "memshare.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include ".\TrainDepartPredictHolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepartPredictDlg dialog
#define   IDC_PREDICT_GRID_ID  54322
extern bool SendTGBMsg(int section_id,CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag=0);
char list_name[3][20]={"Ԥ����Ϣ", "����", "��վ"};

void GetPredictGridHeader(CString& strxml);
void StplanToXML(const TrainPredict& predict, CString& strxml);
TIME GetTimeNow()
{
	CTime tm = CTime::GetCurrentTime();
	return tm.GetTime();
}

CDepartPredictDlg::CDepartPredictDlg(CWnd* pParent)
: CDialog(CDepartPredictDlg::IDD, pParent)
{
	m_current.ResetInfo();
}

void CDepartPredictDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepartPredictDlg)
	DDX_Control(pDX, IDC_COMBO1, m_comboxStation);	
	DDX_Control(pDX, IDC_BUTTON1, m_btnYugao);	
	DDX_Control(pDX, IDC_BUTTON2, m_btnTongyi);	
	DDX_Control(pDX, IDC_BUTTON3, m_btnQuxiao);	
	DDX_Control(pDX, IDC_BUTTON4, m_btnSelect);
	DDX_Control(pDX, IDC_LIST1, m_list);	
	//}}AFX_DATA_MAP 
}

BEGIN_MESSAGE_MAP(CDepartPredictDlg, CDialog)
	//{{AFX_MSG_MAP(CDepartPredictDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeStation)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedYugao)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedTongyi)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedQuxiao)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedSelect)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CREATE()
	
	ON_MESSAGE(10101, OnSelRow)	
	ON_MESSAGE(WM_USER+1, OnPredictUpdate)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CDepartPredictDlg::SetPredictInfo(CTgDocument *pdoc, long shiftid, int SectionId)
{
	m_pDoc = pdoc;
	m_shiftid=shiftid;
	mSectionId=SectionId;
}

// CDepartPredictDlg message handlers
int   CDepartPredictDlg::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	CString xml;
	GetPredictGridHeader(xml);
	
	m_grid.CreateGrid(xml, this,rect,IDC_PREDICT_GRID_ID);
	m_grid.SetEditable(FALSE);
	
	return 0;
}

void  CDepartPredictDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
		m_grid.MoveWindow(1,52,cx-300,cy-53);

	CDialog::OnSize(nType,cx,cy);
}

BOOL  CDepartPredictDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	if(gpcdef.IsEnablePredict())
	{
		m_btnYugao.EnableWindow(TRUE);
		m_btnQuxiao.EnableWindow(TRUE);
		m_btnTongyi.EnableWindow(TRUE);
	}
	else
	{
		m_btnYugao.EnableWindow(FALSE);
		m_btnQuxiao.EnableWindow(FALSE);
		m_btnTongyi.EnableWindow(FALSE);
	}
	m_btnSelect.EnableWindow(FALSE);
	m_pPredictServer = m_pDoc->GetTrainDepartPredictHold();
	m_pPredictServer->RegisterMMI(this);
	OnUpdateStationList();
	OnCbnSelchangeStation();
	
	CFont font;
	font.CreateFont(18, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();
	
	m_grid.AutoWidth();
	m_grid.Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDepartPredictDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	this->ShowWindow(SW_HIDE);
}

void  CDepartPredictDlg::OnCbnSelchangeStation()
{
	OnUpdateStPlan();
	this->SetFocus();
	return;
}

LRESULT CDepartPredictDlg::OnSelRow(WPARAM wParam,LPARAM lParam)
{
	int nSel = m_comboxStation.GetCurSel();
	if(nSel>=0)
	{
		mStation = (STATION_NO)m_comboxStation.GetItemData(nSel);
		mTrainIndex = (TRAIN_INDEX)wParam;
		mRecIndex = (BYTE)lParam;
	}
	return TRUE;
}

LRESULT CDepartPredictDlg::OnPredictUpdate(WPARAM wParam,LPARAM lParam)
{
	this->SetFocus();
	int type = (int)wParam;
	int flag = (int)lParam;
	if(type == 1)
	{
		OnUpdateStPlan();
	}
	else if(type == 2)
	{
		OnUpdateStPlan();
	}
	else if(type == 3)
	{
		OnUpdateStPlan();
	}
	return TRUE;
}

// Ԥ��
void CDepartPredictDlg::OnBnClickedYugao()
{
	m_pPredictServer->LogPredict(0, "����ԱԤ��");

	CStationPredict* pstation = NULL;
	pstation = m_pPredictServer->GetStPredictPlan(mStation);
	if(NULL == pstation)
	{
		MessageBox("�޷���λ�ƻ�,����Ԥ��", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "����ԱԤ��,û���ұ�վ %d �ƻ�", mStation);
		return;
	}

	TrainPredict predict;
	int idx = pstation->GetStplanByTrainIndex(mTrainIndex, mRecIndex, predict);
	if(idx==-1)
	{
		MessageBox("�޷���λ�ƻ�,����Ԥ��", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "�����г�����%u,վ��%d,��վ%d, �Ҳ�����Ӧ�ƻ�", mTrainIndex, mRecIndex, mStation);
		return;
	}
	
	m_pPredictServer->OutputPredict(predict);
	if(predict.next_station_no<=0)
	{
		MessageBox("��ע��, û�к�վ,����Ԥ��", "����", MB_OK|MB_ICONHAND);
		return;
	}
	CString text;
	CString strStation=gpcdef.GetStationName(predict.station_no);
	CString strNeibStation=gpcdef.GetStationName(predict.next_station_no);
	if(!m_pPredictServer->IsMyNeibStation(predict.station_no, predict.next_station_no))
	{
		text.Format("��ע��, ���� %s ������ %s Ԥ��", strStation, strNeibStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}

	if(!m_pPredictServer->IsNeedDowithDepartPredict(mStation))
	{
		text.Format("��ע��, %s ��վ�ػ��߷ǳ�վ�أ��㲻��Ԥ��", strStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}
	
	text="";
	text.Format("��ע��, ��ȷʵ���� %s �� %s Ԥ���г� %s��?", strStation, strNeibStation, predict.depart_train_id);
	if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION) != IDYES)
		return;
	MsgTrainDepartPredict msg;

	msg.nSourceStno = predict.station_no;		// ������վ
	msg.nDestStno = predict.next_station_no;		// ����Ŀ�공վ
	msg.nDestEntry = predict.next_entry;		// Ŀ�공վ�ӳ����
	strcpy(msg.strTid, predict.depart_train_id);			// ���κ�
	msg.nTrainIndex = predict.train_index;	// Train Index
	msg.nRecIndex = predict.rec_index+1;
	
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("�����ļ������Ҳ������û���ENTITY!");
		return;
	}
	msg.nSrcEntity = entityInfo.nEntityId;		// ����Ԥ���ʵ���
	msg.nSrcType=MsgTrainDepartPredict::GPC;// ����Ԥ���ʵ������	
	msg.nOperType=MsgTrainDepartPredict::YUGAO;	// Ԥ������
	msg.nOperTime = GetTimeNow();				// Ԥ��ʱ��

	long eid=GetSpecialEntityID(SERVER_TYPE_PREDICT);
	if(eid==-1)
	{
		AfxMessageBox("�����ļ������Ҳ���Ԥ�������entity!");
		return;
	}
	SendBMsg(&msg, eid);

	m_pPredictServer->LogPredict(0, "***************************");
	m_pPredictServer->LogPredict(0, "����Ա����Ԥ��svr %d, train %s SourceStno %d -->DestStno %d entry %d trainIndex %d recIndex %d", 
			eid, msg.strTid, msg.nSourceStno, msg.nDestStno, 
			msg.nDestEntry, msg.nTrainIndex, msg.nRecIndex);

	pstation->m_stplan[idx].yugao_neib_time = msg.nOperTime;
	pstation->m_stplan[idx].cancel_neib_time = 0;
	pstation->SaveStplanToFile();
	
	m_pPredictServer->LogPredict(0,"����Ԥ����վ �ɹ�, train_index %d rec_index %d trainid %s Ԥ����վʱ��:%d", 
				msg.nTrainIndex, msg.nRecIndex, msg.strTid, msg.nOperTime);

	mRecIndex=0;
	mTrainIndex=0;
	OnUpdateStPlan();
	return;
}

// ͬ��
void CDepartPredictDlg::OnBnClickedTongyi()
{
	m_pPredictServer->LogPredict(0, "����Աͬ��");
	
	CStationPredict* pstation = NULL;
	pstation = m_pPredictServer->GetStPredictPlan(mStation);
	if(NULL == pstation)
	{
		MessageBox("�޷���λ�ƻ�,����ͬ��", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "����Աͬ��,û���ұ�վ %d �ƻ�", mStation);
		return;
	}

	TrainPredict predict;
	int idx = pstation->GetStplanByTrainIndex(mTrainIndex, mRecIndex, predict);
	if(idx==-1)
	{
		MessageBox("�޷���λ�ƻ�,����ͬ��", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "�����г�����%u,վ��%d,��վ%d, �Ҳ�����Ӧ�ƻ�", mTrainIndex, mRecIndex, mStation);
		return;
	}

	m_pPredictServer->OutputPredict(predict);

	if(predict.pre_station_no<=0)
	{
		MessageBox("��ע��, û��ǰ��վ,����ͬ��", "����", MB_OK|MB_ICONHAND);
		return;
	}
	CString text;
	CString strStation=gpcdef.GetStationName(predict.station_no);
	CString strNeibStation=gpcdef.GetStationName(predict.pre_station_no);
	if(!m_pPredictServer->IsMyNeibStation(predict.station_no, predict.pre_station_no))
	{
		text.Format("��ע��, �������� %s ����ͬ�� %s ����", strStation, strNeibStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}

	if(!m_pPredictServer->IsNeedDowithDepartPredict(mStation))
	{
		text.Format("��ע��, %s վ�ػ��߷ǳ�վ�أ��㲻��ͬ��", strStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}

	text="";
	text.Format("��ע��, ��ȷʵ���� %s ͬ��Ӱ���֮ %s ���г� %s��?", strStation, strNeibStation, predict.arrive_train_id);
	if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION) != IDYES)
		return;

	MsgTrainDepartPredict msg;

	msg.nSourceStno = predict.station_no;		// ������վ
	msg.nDestStno = predict.pre_station_no;		// ����Ŀ�공վ
	msg.nDestEntry = predict.entry;		// Ŀ�공վ�ӳ����
	strcpy(msg.strTid, predict.arrive_train_id);			// ���κ�
	msg.nTrainIndex = predict.train_index;	// Train Index
	msg.nRecIndex = predict.rec_index-1;
	
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("�����ļ������Ҳ������û���ENTITY!");
		return;
	}
	msg.nSrcEntity = entityInfo.nEntityId;		// ����Ԥ���ʵ���
	msg.nSrcType=MsgTrainDepartPredict::GPC;// ����Ԥ���ʵ������	
	msg.nOperType=MsgTrainDepartPredict::TONGYI;	// Ԥ������

	msg.nOperTime = GetTimeNow();				// Ԥ��ʱ��

	if(pstation->m_stplan[idx].neib_yugao_time<=0)
	{
		MessageBox("��ע�⣬��վ��δԤ��ó����㲻��ͬ����վ����", "����", MB_OK|MB_ICONHAND);
		return;
	}

	long eid=GetSpecialEntityID(SERVER_TYPE_PREDICT);
	if(eid==-1)
	{
		AfxMessageBox("�����ļ������Ҳ���Ԥ�������entity!");
		return;
	}
	SendBMsg(&msg, eid);

	m_pPredictServer->LogPredict(0, "***************************");
	m_pPredictServer->LogPredict(0, "����Ա����ͬ�� svr %d, train %s SourceStno %d -->DestStno %d entry %d trainIndex %d recIndex %d", 
			eid, msg.strTid, msg.nSourceStno, msg.nDestStno, 
			msg.nDestEntry, msg.nTrainIndex, msg.nRecIndex);
	
	pstation->m_stplan[idx].tongyi_neib_time = msg.nOperTime;
	pstation->SaveStplanToFile();

	m_pPredictServer->LogPredict(0,"����ͬ����վ �ɹ�, train_index %d rec_index %d trainid %s ͬ����վʱ��:%d", 
				msg.nTrainIndex, msg.nRecIndex, msg.strTid, msg.nOperTime);
	
	mRecIndex=0;
	mTrainIndex=0;
	OnUpdateStPlan();
	return;
}

// ȡ������
void CDepartPredictDlg::OnBnClickedQuxiao()
{
	m_pPredictServer->LogPredict(0, "����Աȡ������");

	CStationPredict* pstation = NULL;
	pstation = m_pPredictServer->GetStPredictPlan(mStation);
	if(NULL == pstation)
	{
		MessageBox("�޷���λ�ƻ�,����ȡ������", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "����Աȡ������,û���ұ�վ %d �ƻ�", mStation);
		return;
	}

	TrainPredict predict;
	int idx = pstation->GetStplanByTrainIndex(mTrainIndex, mRecIndex, predict);
	if(idx==-1)
	{
		MessageBox("�޷���λ�ƻ�,����ȡ������", "����", MB_OK|MB_ICONHAND);
		m_pPredictServer->LogPredict(0, "�����г�����%u,վ��%d,��վ%d, �Ҳ�����Ӧ�ƻ�", mTrainIndex, mRecIndex, mStation);
		return;
	}
	m_pPredictServer->OutputPredict(predict);

	CString text;
	if(predict.next_station_no<=0)
	{
		text.Format("��ע��, �üƻ�û�к�վ,����ȡ������");
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}
	
	CString strStation=gpcdef.GetStationName(predict.station_no);
	CString strNeibStation=gpcdef.GetStationName(predict.next_station_no);
	if(!m_pPredictServer->IsMyNeibStation(predict.station_no, predict.next_station_no))
	{
		text.Format("��ע��, ���� %s ������ %s ȡ������", strStation, strNeibStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}

	if(!m_pPredictServer->IsNeedDowithDepartPredict(mStation))
	{
		text.Format("��ע��, %s վ�ػ��߷ǳ�վ�أ��㲻��ȡ��", strStation);
		MessageBox(text, "����", MB_OK|MB_ICONHAND);
		return;
	}

	text="";
	text.Format("��ע��, ��ȷʵ���� %s ȡ���� %s ���� %s��?", strStation, strNeibStation, predict.depart_train_id);
	if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION) != IDYES)
		return;
	MsgTrainDepartPredict msg;
	
	msg.nSourceStno = predict.station_no;		// ������վ
	msg.nDestStno = predict.next_station_no;		// ����Ŀ�공վ
	msg.nDestEntry = predict.next_entry;		// Ŀ�공վ�ӳ����
	strcpy(msg.strTid, predict.depart_train_id);			// ���κ�
	msg.nTrainIndex = predict.train_index;	// Train Index
	if(predict.train_index!=0)
	{
		msg.nRecIndex = predict.rec_index+1;
	}
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("�����ļ������Ҳ������û���ENTITY!");
		return;
	}
	msg.nSrcEntity = entityInfo.nEntityId;		// ����Ԥ���ʵ���
	msg.nSrcType=MsgTrainDepartPredict::GPC;// ����Ԥ���ʵ������	
	msg.nOperType=MsgTrainDepartPredict::QUXIAO;	// Ԥ������

	msg.nOperTime = GetTimeNow();				// Ԥ��ʱ��

	long eid=GetSpecialEntityID(SERVER_TYPE_PREDICT);
	if(eid==-1)
	{
		AfxMessageBox("�����ļ������Ҳ���Ԥ�������entity!");
		return;
	}
	SendBMsg(&msg, eid);

	m_pPredictServer->LogPredict(0, "***************************");
	m_pPredictServer->LogPredict(0, "����Ա����ȡ������ svr %d, train %s SourceStno %d -->DestStno %d entry %d trainIndex %d recIndex %d", 
			eid, msg.strTid, msg.nSourceStno, msg.nDestStno, 
			msg.nDestEntry, msg.nTrainIndex, msg.nRecIndex);

	pstation->m_stplan[idx].neib_tongyi_time=0;
	pstation->m_stplan[idx].yugao_neib_time = 0;
	pstation->m_stplan[idx].cancel_neib_time = msg.nOperTime;
	pstation->SaveStplanToFile();
	
	m_pPredictServer->LogPredict(0,"����ȡ������ �ɹ�, train_index %d rec_index %d trainid %s cancel ȡ������ʱ��:%d", 
				msg.nTrainIndex, msg.nRecIndex, msg.strTid, msg.nOperTime);
	
	mRecIndex=0;
	mTrainIndex=0;
	OnUpdateStPlan();
	return;
}

void CDepartPredictDlg::OnUpdateStPlan()
{
	m_grid.SetRedraw(FALSE);
	m_grid.DeleteNonFixedRows();
	
	multimap<TIME, TrainPredict> mFinish, mPlaning;
	mPlan.clear();
	mFinish.clear();
	mPlaning.clear();
	int nSel = m_comboxStation.GetCurSel();
	if(nSel >= 0)
	{
		mStation = m_comboxStation.GetItemData(nSel);

		CString strTitle;
		strTitle.Format("%s--%s Ԥ��ƻ�", gpcdef.GetSectionName(mSectionId), gpcdef.GetStationName(mStation));
		SetWindowText(strTitle);
			
		CStationPredict* stplan = m_pPredictServer->GetStPredictPlan(mStation);
		if(stplan)
		{
			stplan->SetFinishPredict();
			for(int i=0; i<MAX_STPLAN_NUM; i++)
			{
				if(stplan->m_stplan[i].train_index==0 && stplan->m_stplan[i].station_no==0)
				{
					break;
				}
				if(stplan->m_stplan[i].train_index==0 && stplan->m_stplan[i].station_no==65535)
				{
					continue;
				}

				if(stplan->m_stplan[i].finish_predict==1)
				{
					mFinish.insert(make_pair(stplan->m_stplan[i].sort, stplan->m_stplan[i]));
				}
				else
				{
					mPlaning.insert(make_pair(stplan->m_stplan[i].sort, stplan->m_stplan[i]));
				}
				mPlan.insert(make_pair(stplan->m_stplan[i].sort, stplan->m_stplan[i]));
			}
		}
	}
	else
	{
		mStation=0;
		
		CString strTitle;
		strTitle.Format("%s Ԥ��ƻ�", gpcdef.GetSectionName(mSectionId));
		SetWindowText(strTitle);
	}
	
	CString xml="<STATION_REC_LIST>";
	
	CString strTrainName=m_current.trainno;
	int nFinish=0, nPlan=0;
	CString tmp;
	multimap<TIME, TrainPredict>::iterator it = mFinish.begin();
	for( ; it!=mFinish.end(); it++)
	{
		CString trainstr;
		StplanToXML(it->second, trainstr);
		if(trainstr.IsEmpty())
			continue;

		nFinish++;
		xml+="<RECORD>";
		
		tmp.Format("<HISTORY>1</HISTORY>");
		xml+=tmp;
		
		xml+=trainstr;
		xml+="</RECORD>";
	}

	it = mPlaning.begin();
	for( ; it!=mPlaning.end(); it++)
	{
		CString trainstr;
		StplanToXML(it->second, trainstr);
		if(trainstr.IsEmpty())
			continue;

		nPlan++;
		xml+="<RECORD>";
		if(m_current.flag==1 && strTrainName!="")
		{
			if(strTrainName==it->second.arrive_train_id)
			{
				tmp.Format("<SELECT>1</SELECT>");
				xml+=tmp;
			}
		}
		if(m_current.flag==2 && strTrainName!="")
		{
			if(strTrainName==it->second.depart_train_id)
			{
				tmp.Format("<SELECT>1</SELECT>");
				xml+=tmp;
			}
		}

		xml+=trainstr;
		xml+="</RECORD>";
	}

	xml+="</STATION_REC_LIST>";

	m_grid.AddRowDataFromXML(xml, "STATION_REC_LIST/RECORD");

	m_grid.SetRedraw(TRUE);
	m_grid.AutoSizeRows();

	int nMin, nMax;
	m_grid.GetScrollRange(SB_VERT, &nMin, &nMax);
	if(nFinish>2 && nMax>0)
	{
		float v = nFinish-2;
		v = v/(nPlan+nFinish); 
		int nPos=nMax*v;
		m_grid.SetScrollPos(SB_VERT, nPos);
	}

	m_grid.Refresh();

	return;
}

void CDepartPredictDlg::OnUpdateStationList()
{
	m_comboxStation.ResetContent();

	int ret = 0, sel=0;
	CString staname = "";
	STATION_NO station_list[128];
	int station_num=m_pPredictServer->GetPredictStationList(station_list, 128);
	for(int i = 0; i < station_num; i++)
	{
		staname = gpcdef.GetStationName(station_list[i]);
		ret = m_comboxStation.AddString(staname);
		m_comboxStation.SetItemData(ret, station_list[i]);
	}

	m_comboxStation.SetCurSel(-1);
	mStation=0;
	
	return;
}

//////////////////////////////////////////////////////////////////////////
void GetPredictGridHeader(CString& xml)
{
	CString tmp;
	xml = "<FIELDS>";
	
	tmp.Format("<FIELD caption=\"���ﳵ��\" attr_name=\"ARRIVE_TRAINID\" />");
	xml += tmp;

	tmp.Format("<FIELD caption=\"ǰ����վ\" attr_name=\"PRESTATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"ͼ������\" attr_name=\"PLAN_ARRIVE\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"����\" attr_name=\"ARRIVE\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��վԤ��\" attr_name=\"NEIB_YUGAO_TIME\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"ͬ����վ\" attr_name=\"TONGYI_NEIB_TIME\" />");
	xml += tmp;
	
	tmp.Format("<FIELD caption=\"����վ\" attr_name=\"NEXTSTATION\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��������\" attr_name=\"DEPART_TRAINID\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"ͼ������\" attr_name=\"PLAN_DEPART\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"����\" attr_name=\"DEPART\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"Ԥ����վ\" attr_name=\"YUGAO_NEIB_TIME\" />");
	xml += tmp;
	tmp.Format("<FIELD caption=\"��վͬ��\" attr_name=\"NEIB_TONGYI_TIME\" />");
	xml += tmp;
	xml += "</FIELDS>";
}

void StplanToXML(const TrainPredict& predict, CString& trainstr)
{
	CString tmpstr;
	trainstr="";
	
	CString preStationName, StationName, nextStationName;
	StationName=gpcdef.GetStationName(predict.station_no);
	if(predict.pre_station_no>0)
		preStationName = gpcdef.GetStationName(predict.pre_station_no);
	if(predict.next_station_no>0)
		nextStationName = gpcdef.GetStationName(predict.next_station_no);
		
	tmpstr.Format("<TRAIN_INDEX>%d</TRAIN_INDEX>", predict.train_index);
	trainstr+=tmpstr;

	tmpstr.Format("<REC_INDEX>%d</REC_INDEX>", predict.rec_index);
	trainstr+=tmpstr;

	if(predict.passenger==1)
		tmpstr.Format("<TRAINID_COLOR>%s</TRAINID_COLOR>", "RED");
	else
		tmpstr.Format("<TRAINID_COLOR>%s</TRAINID_COLOR>", "BLUE");
	trainstr+=tmpstr;

	tmpstr.Format("<ARRIVE_TRAINID>%s</ARRIVE_TRAINID>", predict.arrive_train_id);
	trainstr+=tmpstr;

	tmpstr.Format("<DEPART_TRAINID>%s</DEPART_TRAINID>", predict.depart_train_id);
	trainstr+=tmpstr;

	// 0: ���� 1: ʼ�� 2:�յ�
	if(predict.start!=1)
	{
		if(predict.neib_yugao_time>0 && predict.tongyi_neib_time==0)
		{
			tmpstr.Format("<TONGYI_NEIB_BK_COLOR>%s</TONGYI_NEIB_BK_COLOR>", "RED");
			trainstr+=tmpstr;
		}
						
		tmpstr.Format("<PRESTATION>%s</PRESTATION>", preStationName);
		trainstr+=tmpstr;

		if(predict.neib_yugao_time>0)
		{
			CTime tm(predict.neib_yugao_time);
			tmpstr.Format("<NEIB_YUGAO_TIME>%s</NEIB_YUGAO_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}
		
		if(predict.neib_yugao_time>0)
		{
			CTime tm(predict.neib_yugao_time);
			tmpstr.Format("<NEIB_YUGAO_TIME>%s</NEIB_YUGAO_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}

		if(predict.tongyi_neib_time>0)
		{
			CTime tm(predict.tongyi_neib_time);
			tmpstr.Format("<TONGYI_NEIB_TIME>%s</TONGYI_NEIB_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}
		
		if(predict.plan_arrive_time>0)
		{
			CTime tm2(predict.plan_arrive_time);
			tmpstr.Format("<PLAN_ARRIVE>%s</PLAN_ARRIVE>", tm2.Format("%H:%M"));
			trainstr+=tmpstr;
		}

		if(predict.arrive_time>0)
		{
			CTime tm3(predict.arrive_time);
			tmpstr.Format("<ARRIVE>%s</ARRIVE>", tm3.Format("%H:%M"));
			trainstr+=tmpstr;
		}

		if(predict.arrive_real>0)
			tmpstr.Format("<ARRIVE_COLOR>%s</ARRIVE_COLOR>", "BLACK");
		else
			tmpstr.Format("<ARRIVE_COLOR>%s</ARRIVE_COLOR>", "BLUE");
		trainstr+=tmpstr;
	}
	else
	{
		tmpstr.Format("<ARRIVE>%s</ARRIVE>", "ʼ��");
		trainstr+=tmpstr;
	}

	if(predict.start!=2)
	{
		if(predict.yugao_neib_time>0 && predict.neib_tongyi_time==0)
		{
			tmpstr.Format("<NEIB_TONGYI_BK_COLOR>%s</NEIB_TONGYI_BK_COLOR>", "BLUE");
			trainstr+=tmpstr;
		}
							
		if(predict.yugao_neib_time>0)
		{
			CTime tm(predict.yugao_neib_time);
			tmpstr.Format("<YUGAO_NEIB_TIME>%s</YUGAO_NEIB_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}
	
		if(predict.yugao_neib_time>0)
		{
			CTime tm(predict.yugao_neib_time);
			tmpstr.Format("<YUGAO_NEIB_TIME>%s</YUGAO_NEIB_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}
		if(predict.neib_tongyi_time>0)
		{
			CTime tm(predict.neib_tongyi_time);
			tmpstr.Format("<NEIB_TONGYI_TIME>%s</NEIB_TONGYI_TIME>", tm.Format("%H:%M"));
			trainstr+=tmpstr;
		}
	
		tmpstr.Format("<NEXTSTATION>%s</NEXTSTATION>", nextStationName);
		trainstr+=tmpstr;

		if(predict.depart_real>0)
		{
			if(predict.depart_time>0)
			{
				CTime tm2(predict.depart_time);
				tmpstr.Format("<DEPART>%s</DEPART>", tm2.Format("%H:%M"));
				trainstr+=tmpstr;
			}

			if(predict.plan_depart_time>0)
			{
				CTime tm3(predict.plan_depart_time);
				tmpstr.Format("<PLAN_DEPART>%s</PLAN_DEPART>", tm3.Format("%H:%M"));
				trainstr+=tmpstr;
			}
		}
		else
		{
			if(predict.start==0 && predict.plan_arrive_time == predict.plan_depart_time)
			{
				tmpstr.Format("<PLAN_DEPART>%s</PLAN_DEPART>", "ͨ��");
				trainstr+=tmpstr;
			}
			else
			{
				if(predict.plan_depart_time>0)
				{
					CTime tm2(predict.plan_depart_time);
					tmpstr.Format("<PLAN_DEPART>%s</PLAN_DEPART>", tm2.Format("%H:%M"));
					trainstr+=tmpstr;
				}
			}

			if(predict.start==0 && predict.arrive_time == predict.depart_time)
			{
				tmpstr.Format("<DEPART>%s</DEPART>", "ͨ��");
				trainstr+=tmpstr;
			}
			else
			{
				if(predict.depart_time>0)
				{
					CTime tm2(predict.depart_time);
					tmpstr.Format("<DEPART>%s</DEPART>", tm2.Format("%H:%M"));
					trainstr+=tmpstr;
				}
			}
		}

		if(predict.depart_real>0)
			tmpstr.Format("<DEPART_COLOR>%s</DEPART_COLOR>", "BLACK");
		else
			tmpstr.Format("<DEPART_COLOR>%s</DEPART_COLOR>", "BLUE");
		trainstr+=tmpstr;
	}
	else
	{
		tmpstr.Format("<DEPART>%s</DEPART>", "�յ�");
		trainstr+=tmpstr;
	}
}

// ����Ա�ж���Ҫ�����Ǹ��ƻ�
void CDepartPredictDlg::OnBnClickedSelect()
{
	if(m_current.flag==1)
	{
		m_pPredictServer->LogPredict(0, "�ƻ��ظ��˹�ѡ��");
		CStationPredict* pstation = m_pPredictServer->GetStPredictPlan(m_current.this_station, m_current.neib_station);
		if(NULL == pstation)
		{
			return;
		}
		TrainPredict predict;
		int idx = pstation->GetStplanByTrainIndex(mTrainIndex, mRecIndex, predict);
		if(idx==-1)
		{
			MessageBox("��ѡ��Ԥ��ƻ��ж�Ӧ����", "����", MB_OK|MB_ICONHAND);
		}
		else 
		{
			if(CString(predict.arrive_train_id)!=m_current.trainno)
			{
				MessageBox("��ѡ��Ԥ��ƻ��ж�Ӧ����", "����", MB_OK|MB_ICONHAND);
				return;
			}

			if(m_current.state==TrainPredict::NEIB_YUGAO)
			{
				pstation->m_stplan[idx].neib_yugao_time  = m_current.oper_tm;
				pstation->m_stplan[idx].neib_cancel_time = 0;
				pstation->SaveStplanToFile();
				m_pPredictServer->LogPredict(0,"������վԤ�� �ɹ�, train_index %u rec_index %d trainid %s Ԥ��ʱ��:%d.", 
									mTrainIndex, mRecIndex, m_current.trainno, m_current.oper_tm);
			}
			if(m_current.state==TrainPredict::NEIB_CANCEL)
			{
				pstation->m_stplan[idx].tongyi_neib_time =0;
				pstation->m_stplan[idx].neib_yugao_time = 0;
				pstation->m_stplan[idx].neib_cancel_time = m_current.oper_tm;
				pstation->SaveStplanToFile();
				m_pPredictServer->LogPredict(0,"������վȡ������ �ɹ�, train_index %u rec_index %d trainid %s ��վȡ������ʱ�� %d", 
									mTrainIndex, mRecIndex, m_current.trainno, m_current.oper_tm);
			}
			DeleteSaveInfo(m_current);
			m_current.ResetInfo();
			OnUpdateStPlan();
		}
	}
	else if(m_current.flag==2)
	{
		m_pPredictServer->LogPredict(0, "�ƻ��ظ��˹�ѡ��");
		CStationPredict* pstation = m_pPredictServer->GetStPredictPlan(m_current.this_station, m_current.neib_station);
		if(NULL == pstation)
		{
			return;
		}
		TrainPredict predict;
		int idx = pstation->GetStplanByTrainIndex(mTrainIndex, mRecIndex, predict);
		if(idx==-1)
		{
			MessageBox("��ѡ��Ԥ��ƻ��ж�Ӧ����", "����", MB_OK|MB_ICONHAND);
		}
		else
		{
			if(CString(predict.depart_train_id)!=m_current.trainno)
			{
				MessageBox("��ѡ��Ԥ��ƻ��ж�Ӧ����", "����", MB_OK|MB_ICONHAND);
				return;
			}
			
			if(pstation->m_stplan[idx].yugao_neib_time<=0)
			{
				MessageBox("��ע�⣬����δ����վԤ�棬���ܽ�����վͬ��", "����", MB_OK|MB_ICONHAND);
				return;
			}
			else
			{
				pstation->m_stplan[idx].neib_tongyi_time = m_current.oper_tm;
				pstation->SaveStplanToFile();
				m_pPredictServer->LogPredict(0,"������վͬ�� �ɹ�, train_index %u rec_index %d trainid %s ��վͬ��ʱ�� %d", 
									mTrainIndex, mRecIndex, m_current.trainno, m_current.oper_tm);
				DeleteSaveInfo(m_current);
				m_current.ResetInfo();
				OnUpdateStPlan();
			}
		}
	}
	else
	{
		DeleteSaveInfo(m_current);
		m_current.ResetInfo();
	}
	mRecIndex=0;
	mTrainIndex=0;
}

void CDepartPredictDlg::SaveTrainNo(STATION_NO this_station, STATION_NO neib_station, CString str, BYTE state, long oper_tm, int flag)
{
	if(flag==1)
	{
		if(state!=TrainPredict::NEIB_YUGAO && state!=TrainPredict::NEIB_CANCEL)
		{
			return;
		}
	}
	else if(flag==2)
	{
		if(state!=TrainPredict::NEIB_TONGYI)
		{
			return;
		}
	}
	else
	{
		return;
	}
	CStationPredict* pstation = m_pPredictServer->GetStPredictPlan(this_station, neib_station);
	if(NULL == pstation)
	{
		m_pPredictServer->LogPredict(0, "�����ظ��ƻ��˹�ѡ����Ϣ,û�б�վ %d ����վ %d ��Ԥ������, �޷�����Ԥ�����ʱ��", 
			this_station, neib_station);
		return;
	}
	SaveInfo info;
	info.this_station=this_station;
	info.neib_station=neib_station;
	info.flag=flag;
	info.state=state;
	info.oper_tm=oper_tm;
	strncpy(info.trainno, str, 10);
	info.trainno[9]=0;

	m_save_list.push_back(info);
	
	UpdateList();
}

void CDepartPredictDlg::DeleteSaveInfo(SaveInfo& info)
{
	for(int i=0; i<m_save_list.size(); i++)
	{
		if( m_save_list[i].flag==info.flag && m_save_list[i].neib_station==info.neib_station &&
			m_save_list[i].oper_tm==info.oper_tm && m_save_list[i].state==info.state &&
			m_save_list[i].this_station==info.this_station)
		{
			CString str=m_save_list[i].trainno;
			if(str == info.trainno)
			{
				m_save_list.erase(m_save_list.begin()+i);
			}
		}
	}
	UpdateList();
}

void CDepartPredictDlg::UpdateList()
{
	m_list.ResetContent();
	for(int n=0; n<m_save_list.size(); n++)
	{
		CString str;
		SaveInfo info = m_save_list[n];
		if(info.state==TrainPredict::NEIB_YUGAO && info.flag==1)
		{
			str.Format("%s:%s,������Ԥ��%s", 
				gpcdef.GetStationName(info.neib_station),gpcdef.GetStationName(info.this_station), info.trainno);
		}
		if(m_save_list[n].state==TrainPredict::NEIB_CANCEL && info.flag==1)
		{
			str.Format("%s:%s,��ȡ������%s", 
				gpcdef.GetStationName(info.neib_station),gpcdef.GetStationName(info.this_station), info.trainno);
		}
		if(m_save_list[n].state==TrainPredict::NEIB_TONGYI && info.flag==2)
		{
			str.Format("%s:%s,��ͬ���㷢��%s", 
				gpcdef.GetStationName(info.neib_station),gpcdef.GetStationName(info.this_station), info.trainno);
		}
		
		if(str!="")
		{
			int nPos=m_list.AddString(str);
			if(nPos>=0)
			{
				m_list.SetItemData(nPos, m_save_list[n].this_station);
			}
		}
	}

	if(m_list.GetCount()>0)
		m_btnSelect.EnableWindow(TRUE);
	else
		m_btnSelect.EnableWindow(FALSE);
}

void CDepartPredictDlg::OnLbnSelchangeList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iSel=m_list.GetCurSel();
	if(iSel<0)
		return;
	int nStation = m_list.GetItemData(iSel);
	if(iSel<m_save_list.size())
	{
		m_current=m_save_list[iSel];
	}
	else
		return;

	for(int i=0; i<m_comboxStation.GetCount(); i++)
	{
		if(m_comboxStation.GetItemData(i)==nStation)
		{
			m_comboxStation.SetCurSel(i);
			OnUpdateStPlan();
		}
	}
}


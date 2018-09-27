// TrainPlanParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainPlanParaDlg.h"
#include ".\trainplanparadlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg dialog


CTrainPlanParaDlg::CTrainPlanParaDlg(CWnd* pParent)
: CDialog(CTrainPlanParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainPlanParaDlg)
	m_check_adjust_start = FALSE;
	m_check_stop_train = FALSE;
	
	m_station = _T("");
	m_train_id = _T("");
	
	m_exit = 0;
	m_entry = 0;

	//}}AFX_DATA_INIT

	mTgType= 0;
	m_pDoc = NULL;
}

void CTrainPlanParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainPlanParaDlg)
	DDX_Control(pDX, IDC_COMBO_PREVIOUSABS, m_combo_previous_abs);
	DDX_Control(pDX, IDC_CHECK_ABS_TIME, m_ctrlAbsTimeButton);
	DDX_Control(pDX, IDC_CHECK_TRACK, m_ctrlTrackButton);
	DDX_Control(pDX, IDC_COMBO_TRACK, m_combo_track);
	DDX_Control(pDX, IDC_COMBO_NEXTABS, m_combo_next_abs);
	DDX_Control(pDX, IDC_PASS_SIDE, m_sidePassButton);
	
	DDX_Check(pDX, IDC_NO_STOP_TRAIN, m_check_stop_train);

	DDX_Text(pDX, IDC_STATION, m_station);
	DDX_Text(pDX, IDC_TRAIN_ID, m_train_id);

	DDX_Text(pDX, IDC_EDIT4, m_exit);
	DDX_Text(pDX, IDC_EDIT5, m_entry);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_plan_arl_day);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_plan_arl_time);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_plan_dep_day);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_plan_dep_time);
	DDX_Control(pDX, IDC_DATETIMEPICKER6, m_arrival);
	DDX_Control(pDX, IDC_DATETIMEPICKER5, m_depart);
	DDX_Control(pDX, IDC_DATETIMEPICKER7, m_stop_station_time);
	DDX_Control(pDX, IDC_DATETIMEPICKER8, m_min_stop_station_time);
	DDX_Control(pDX, IDC_DATETIMEPICKER9, m_next_abs_runtime);
}


BEGIN_MESSAGE_MAP(CTrainPlanParaDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainPlanParaDlg)
	ON_BN_CLICKED(IDC_CHECK_TRACK, OnCheckTrack)
	ON_BN_CLICKED(IDC_CHECK_ABS_TIME, OnCheckAbsTime)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER6, OnDtnDatetimechangeDatetimepicker6)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER5, OnDtnDatetimechangeDatetimepicker5)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER7, OnDtnDatetimechangeDatetimepicker7)
	ON_CBN_SELCHANGE(IDC_COMBO_TRACK, OnCbnSelSide)
	ON_CBN_SELCHANGE(IDC_COMBO_NEXTABS, OnCbnSelNextAbs)
	ON_CBN_SELCHANGE(IDC_COMBO_PREVIOUSABS, OnCbnSelPreviousAbs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg message handlers

BOOL CTrainPlanParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int index = 0;
	int node_index_next;
	int side_no;
	CString str;
	
	memset(abs_line, 0, sizeof(abs_line));
	memset(abs_line_previous, 0, sizeof(abs_line_previous));

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		return TRUE;
	}

	TgTRAIN_RECORD *tmp = pTrain->GetRecordByRecIndex(m_rec_index);
	if((tmp == NULL) || (tmp!=NULL && tmp->station != m_foucs_station))
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		return TRUE;
	}

	CTG_TRAIN train = *pTrain;
	TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(m_rec_index);
	if(rec == NULL) 
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		return TRUE;
	}
	m_rec=*rec;
    CString title;
	GetWindowText(title);	
	CString strFromTo;
	strFromTo.Format("--%d���� %d����", train.myTrainInfo.from_conid, train.myTrainInfo.to_conid);
	title += strFromTo;
	SetWindowText(title);

	m_check_limit = FALSE; 

	m_dispNum = m_pDoc->m_nDispDistanceNum;

	m_train_id =  m_pDoc->GetTrainShowId(train);
	
	m_station =  m_pDoc->GetStationName(rec->station);
	m_exit = rec->exit;
	m_entry = rec->entry;
	m_nCurrEntry = rec->entry;
	m_nCurrExit = rec->exit;

	//��õ�ǰ��¼�Ľڵ��
	int node_index_current = rec->out_station_index;
	
	if(node_index_current<0 || node_index_current>=m_dispNum)
		return TRUE;

	CString strSide="?";
	for(int i = 0; i < m_pDoc->m_sDispDistance[node_index_current].number_of_side; i++)
	{
		side_no = m_pDoc->m_sDispDistance[node_index_current].side[i].side_number;
		str.Format("%s",m_pDoc->m_sDispDistance[node_index_current].side[i].sidename);
		m_combo_track.AddString(str);
		m_combo_track.SetItemData(i,side_no);
		if(rec->arrive_side == side_no)
		{
			index = i;
			m_combo_track.SetCurSel(index);
			strSide=str;
		}
	}

	if(IsForceSide(*rec))
	{
		m_combo_track.EnableWindow(FALSE);
		m_ctrlTrackButton.SetCheck(1);
		m_pDoc->sysprint(0,0,"�г�:%s %s �ɵ������� %s",m_train_id, m_station,strSide);
	}
	else
	{
		m_combo_track.EnableWindow(TRUE);
		m_ctrlTrackButton.SetCheck(0);
		m_pDoc->sysprint(0,0,"�г�:%s %s �ɵ�δ������ %s",m_train_id, m_station,strSide);
	}
	
	bool bIsNotThisSectionStation=m_pDoc->m_ptgctrl->mConfigTg.IsNotThisSectionStation(rec->station);
	if(bIsNotThisSectionStation)
	{
		m_combo_track.EnableWindow(FALSE);
		m_ctrlTrackButton.EnableWindow(FALSE);
		m_plan_arl_day.EnableWindow(FALSE);
		m_plan_dep_day.EnableWindow(FALSE);
		m_plan_arl_time.EnableWindow(FALSE);
		m_plan_dep_time.EnableWindow(FALSE);
		m_min_stop_station_time.EnableWindow(FALSE);
		m_sidePassButton.EnableWindow(FALSE);
	}
	TgTRAIN_RECORD *next_rec = train.GetRecordByRecIndex(m_rec_index + 1);

	if(next_rec != NULL)
		m_nNextEntry = next_rec->entry;

	// ��һ��������ʱ��
	if(IsSpecifyAbsRunTime(rec))
	{
		m_ctrlAbsTimeButton.SetCheck(TRUE);
		m_next_abs_runtime.EnableWindow(TRUE);
		SetTimeCtrlTime(IDC_DATETIMEPICKER9, rec->spec_run_time * 60);
	}
	else
	{
		m_ctrlAbsTimeButton.SetCheck(FALSE);
		m_next_abs_runtime.EnableWindow(FALSE);
		SetTimeCtrlTime(IDC_DATETIMEPICKER9, 0);
	}
 
	if(mTgType == TG_TYPE::BASIC_TG)
	{
		m_ctrlAbsTimeButton.EnableWindow(FALSE);
	}

	// for curr abs line    
	TgTRAIN_RECORD *previous_rec;

	int abs_line_count = 0;  
	DispDistance *node_current = &m_pDoc->m_sDispDistance[node_index_current];  
	do
	{
		if(next_rec != NULL)
		{
			node_index_next = next_rec->in_station_index;
			if(node_index_next < 0 || node_index_next >= m_pDoc->m_nDispDistanceNum)
			{
				for(i=0; i<node_current->down_line_count; i++)
					abs_line[abs_line_count ++] = node_current->down_line_list [i];
				for(i=0; i<node_current->up_line_count; i++)
					abs_line[abs_line_count ++] = node_current->up_line_list [i];

				break;
			}
			//���Ϊ������ҵ�۷����Σ����ݳ����жϳ�����Ȼ�����г���ʾȫ���Ϸ���ͨ�ߣ����г���ʾȫ���·���ͨ��
			else if(IsFlagStopAbs(rec->flag) && rec->station == next_rec->station)
			{
				if(IsFlagStopAbsUp(rec->flag))//����
				{
					for(i=0; i<node_current->up_line_count; i++)
						abs_line[abs_line_count ++] = node_current->up_line_list [i];

				}
				else
				{
					for(i=0; i<node_current->down_line_count; i++)
						abs_line[abs_line_count ++] = node_current->down_line_list [i];

				}
				break;
			}
			else
				;

			DispDistance  *node_info_next = &m_pDoc->m_sDispDistance[node_index_next];

			for(i=0;i<node_current->down_line_count;i++)
			{
				for(int j= 0;j<node_info_next->up_line_count ;j++)
				{
					if(node_current->down_line_list [i] == node_info_next->up_line_list [j])
						abs_line[abs_line_count ++] = node_current->down_line_list [i];
				}
			}
	
			for(i=0;i<node_current->up_line_count;i++)
			{
				for(int j= 0;j<node_info_next->down_line_count ;j++)
				{
					if(node_current->up_line_list [i] == node_info_next->down_line_list [j])
						abs_line[abs_line_count ++] = node_current->up_line_list [i];
				}
			}
		}
		else
		{//�·��޳�վ ���ó�վΪ��ֹ��վ����ôʹ�øó�վ��ԭʼ�ߺ�

			for(int m=0;m < m_dispNum;m++)
			{
				DispDistance* node_info = &m_pDoc->m_sDispDistance[m];
				if(node_current->station_number != node_info->station_number)
					continue;
				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsOnlyShowSelLine())
				{
					if((rec->in_station_index != m) && (rec->out_station_index != m))
						continue;
				}
				bool bFlag = FALSE;
				for(i=0; i<node_info->down_line_count; i++)
				{
					for(int k=0;k<abs_line_count;k++)
					{
						if(abs_line[k] == node_info->down_line_list [i]){
							bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line[abs_line_count ++] = node_info->down_line_list [i];
				}
				bFlag = FALSE;
				for(i=0; i<node_info->up_line_count; i++)
				{
					for(int k=0;k<abs_line_count;k++){
						if(abs_line[k] == node_info->up_line_list [i]){
							bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line[abs_line_count ++] = node_info->up_line_list [i];
				}
			}

		}
	}while(0);
	
	if(abs_line_count<=0) // ��ֹ�ƻ��߲�����û����һ����
	{
		for(int m=0;m < m_dispNum;m++)
		{
			DispDistance* node_info = &m_pDoc->m_sDispDistance[m];
			if(node_current->station_number != node_info->station_number)
				continue;
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsOnlyShowSelLine())
			{
				if((rec->in_station_index != m) && (rec->out_station_index != m))
					continue;
			}
			for(i=0; i<node_info->down_line_count; i++)
			{
				abs_line[abs_line_count ++] = node_info->down_line_list [i];
			}
			for(i=0; i<node_info->up_line_count; i++)
			{
				abs_line[abs_line_count ++] = node_info->up_line_list [i];
			}
		}
	}
	RemoveSameLineNo(abs_line_count, abs_line);

	WORD nAbsNum,nDir;
	m_pDoc->ConvertStationEntryToAbsnum(rec->station,rec->exit,&nAbsNum,&nDir);
	
	index = -1;  
	CString sStationStart,sStationEnd;
	if(abs_line_count >0)//�������ߺ��в��ҵ�ǰ�ҵ����ߺ�����
	{
		for(i=0;i<abs_line_count;i++)
		{
			const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line[i]);
			if (line)
			{
				if(line->nStartStation>0)
					sStationStart.Format("%s", m_pDoc->GetStationName(line->nStartStation ));
				else
					sStationStart ="����";
				if(line->nEndStation>0)
					sStationEnd.Format("%s", m_pDoc->GetStationName(line->nEndStation ));
				else
					sStationEnd ="����";
				if(line->nDirect !=3)
					str.Format("%s��>%s(%d)%s", sStationStart, sStationEnd, abs_line[i], line->sLineName);
				else
					str.Format("%s<��>%s(%d)%s",sStationStart, sStationEnd, abs_line[i], line->sLineName);
				if(nAbsNum == abs_line[i])
					index = i;

				m_combo_next_abs.AddString(str);
				m_combo_next_abs.SetItemData (i,(DWORD)abs_line[i]);
			}
		}
	}

	//����Ϊ��,����ʾ��
	if(rec->exit != 0)
		m_combo_next_abs.SetCurSel (index);


	//previous abs line info
	//	abs_line_count = 0;    
	node_index_current = rec->in_station_index;
	if(node_index_current<0 || node_index_current>=m_dispNum)
		return TRUE;

	node_current = &m_pDoc->m_sDispDistance[node_index_current]; 
	int abs_line_count_previous= 0;
	do
	{
		previous_rec = train.GetRecordByRecIndex(m_rec_index - 1);

		int node_index_previous;

		if(previous_rec != NULL)
		{
			m_nPrevExit = previous_rec->exit;
			
			node_index_previous = previous_rec->out_station_index;
			if(node_index_previous < 0 || node_index_previous >= m_pDoc->m_nDispDistanceNum)
			{
				for(i=0; i<node_current->down_line_count; i++)
					abs_line_previous[abs_line_count_previous ++] = node_current->down_line_list [i];
				for(i=0; i<node_current->up_line_count; i++)
					abs_line_previous[abs_line_count_previous ++] = node_current->up_line_list [i];
				break;
			}
			else if(IsFlagStopAbs(previous_rec->flag) && rec->station == previous_rec->station)
			{
				if(IsFlagStopAbsUp(previous_rec->flag))//����
				{
					for(i=0; i<node_current->up_line_count; i++)
						abs_line_previous[abs_line_count_previous ++] = node_current->up_line_list [i];

				}
				else
				{
					for(i=0; i<node_current->down_line_count; i++)
						abs_line_previous[abs_line_count_previous ++] = node_current->down_line_list [i];

				}
				break;
			}

			DispDistance* node_info_previous = &m_pDoc->m_sDispDistance[node_index_previous];
			
			for(i=0;i<node_current->up_line_count;i++)
			{
				for(int j= 0;j<node_info_previous->down_line_count ;j++)
				{
					if(node_current->up_line_list [i] == node_info_previous->down_line_list [j])
						abs_line_previous[abs_line_count_previous ++] = node_current->up_line_list [i];
				}
			}   
		
		
			for(i=0;i<node_current->down_line_count;i++)
			{
				for(int j= 0;j<node_info_previous->up_line_count ;j++)
				{
					if(node_current->down_line_list [i] == node_info_previous->up_line_list [j])
						abs_line_previous[abs_line_count_previous ++] = node_current->down_line_list [i];
				}
			}
		}
		else
		{ //�Ϸ��޳�վ ���ó�վΪ��ʼ��վ����ôʹ�øó�վ��ԭʼ�ߺ�
			for(int m=0;m < m_dispNum;m++)
			{
				DispDistance* node_info = &m_pDoc->m_sDispDistance[m];
				if(node_current->station_number != node_info->station_number)
					continue;

				if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsOnlyShowSelLine())
				{
					if((rec->in_station_index != m) && (rec->out_station_index != m))
						continue;
				}

				bool bFlag = FALSE;
				for(i=0; i<node_info->down_line_count; i++){
					for(int k=0;k<abs_line_count_previous;k++){
						if(abs_line_previous[k] == node_info->down_line_list [i]){
							bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line_previous[abs_line_count_previous ++] = node_info->down_line_list [i];
				}
				bFlag = FALSE;
				for(i=0; i<node_info->up_line_count; i++){
					for(int k=0;k<abs_line_count_previous;k++){
						if(abs_line_previous[k] == node_info->up_line_list [i]){
							bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line_previous[abs_line_count_previous ++] = node_info->up_line_list [i];
				}
			}

		}
	}while(0);
	
	if(abs_line_count_previous<=0)
	{
		for(int m=0;m < m_dispNum;m++)
		{
			DispDistance* node_info = &m_pDoc->m_sDispDistance[m];
			if(node_current->station_number != node_info->station_number)
				continue;
			if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsOnlyShowSelLine())
			{
				if((rec->in_station_index != m) && (rec->out_station_index != m))
					continue;
			}

			for(i=0; i<node_info->down_line_count; i++)
			{
				abs_line_previous[abs_line_count_previous ++] = node_info->down_line_list [i];
			}
			for(i=0; i<node_info->up_line_count; i++)
			{
				abs_line_previous[abs_line_count_previous ++] = node_info->up_line_list [i];
			}
		}
	}
	RemoveSameLineNo(abs_line_count_previous, abs_line_previous);
	
	//	WORD nAbsNum,nDir;
	m_pDoc->ConvertStationEntryToAbsnum(rec->station,rec->entry,&nAbsNum,&nDir);

	index = -1;

	if(abs_line_count_previous >0)
	{
		for(i=0;i<abs_line_count_previous;i++)
		{
			const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_line_previous[i]);
			if(line)
			{

				if(line->nStartStation>0)
					sStationStart.Format("%s",m_pDoc->GetStationName(line->nStartStation ));
				else
					sStationStart ="����";
				if(line->nEndStation>0)
					sStationEnd.Format("%s",m_pDoc->GetStationName(line->nEndStation ));
				else
					sStationEnd ="����";
				if(line->nDirect !=3)
					str.Format("%s��>%s(%d)%s",  sStationStart, sStationEnd, abs_line_previous[i], line->sLineName);
				else
					str.Format("%s<��>%s(%d)%s", sStationStart, sStationEnd, abs_line_previous[i], line->sLineName);
				if(nAbsNum == abs_line_previous[i])
					index = i;
				m_combo_previous_abs.AddString(str);
				m_combo_previous_abs.SetItemData (i,(DWORD)abs_line_previous[i]);
			}
			
		}
	}
	//���Ϊ��,����ʾΪ��
	if(rec->entry != 0)
		m_combo_previous_abs.SetCurSel (index);

	if(IsForcePassByMainSide(*rec))
		m_sidePassButton.SetCheck(FALSE);
	else
		m_sidePassButton.SetCheck(TRUE);
			
	if((USHORT)(train.myTrainInfo.adjust_begin_sta & ~0x0080) == (USHORT)rec->rec_index)
		m_check_adjust_start = TRUE;
	else
		m_check_adjust_start = FALSE;

	if(IsForcePass(*rec))
		m_check_stop_train = TRUE;
	else
		m_check_stop_train = FALSE;

	m_plan_arl_day.SetFormat("yyyy-MM-dd");
	m_plan_dep_day.SetFormat("yyyy-MM-dd");
	m_plan_arl_time.SetFormat("HH:mm:ss");
	m_plan_dep_time.SetFormat("HH:mm:ss");
	m_arrival.SetFormat("HH:mm:ss");
	m_depart.SetFormat("HH:mm:ss");

	m_stop_station_time.SetFormat("HH:mm:ss");
	m_min_stop_station_time.SetFormat("HH:mm:ss");
	m_next_abs_runtime.SetFormat("HH:mm:ss");
	
	if(mTgType == TG_TYPE::BASIC_TG)
	{
		m_plan_arl_day.EnableWindow(FALSE);
		m_plan_arl_time.EnableWindow(FALSE);
		m_plan_dep_time.EnableWindow(FALSE);
		m_plan_dep_day.EnableWindow(FALSE);
	}
	else
	{
		CTime timeArlTime(rec->plan_arrive);
		CTime timeDepTime(rec->plan_depart);

		m_plan_arl_day.SetTime(&timeArlTime);
		m_plan_arl_time.SetTime(&timeArlTime);

		m_plan_dep_day.SetTime(&timeDepTime);
		m_plan_dep_time.SetTime(&timeDepTime);

		bool b_plan_time_readonly=m_pDoc->m_ptgctrl->mConfigTg.IsDisableModifyPlanTm();
		if(b_plan_time_readonly)
		{
			m_plan_arl_day.EnableWindow(FALSE);
			m_plan_arl_time.EnableWindow(FALSE);
			m_plan_dep_time.EnableWindow(FALSE);
			m_plan_dep_day.EnableWindow(FALSE);
		}
	}

	if(mTgType == TG_TYPE::BASIC_TG)
	{
		SetTimeCtrlTime(IDC_DATETIMEPICKER6, rec->arrive);

		SetTimeCtrlTime(IDC_DATETIMEPICKER5, rec->depart);
	}
	else
	{
		CTime timeArlTime(rec->arrive);
		CTime timeDepTime(rec->depart);
		m_arrival.SetTime(&timeArlTime);
		m_depart.SetTime(&timeDepTime);
	}

	// ͣվʱ��
	SetTimeCtrlTime(IDC_DATETIMEPICKER7, rec->depart-rec->arrive);
	
	// ��Сͣվʱ��
	SetTimeCtrlTime(IDC_DATETIMEPICKER8, rec->min_stop_time);
	m_pDoc->sysprint(0,0,"�޸ļƻ��߲���,�޸�ǰ:�г� %s �� %s ��ҵʱ��Ϊ %d",m_train_id, m_station,rec->min_stop_time);
	UpdateData(FALSE);

	this->InitIdentify();
    SetCenterPosition();

	if(mTgType == TG_TYPE::WORK_TG)
	{
		CWnd *pWnd;
		if (IsActualDepartTime(rec->adjust_status))
		{
			pWnd = GetDlgItem(IDC_DATETIMEPICKER5);
			pWnd->EnableWindow(FALSE);
		}
		if (IsActualArrivalTime(rec->adjust_status))
		{
			pWnd = GetDlgItem(IDC_DATETIMEPICKER6);
			pWnd->EnableWindow(FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainPlanParaDlg::SetTimeCtrlTime(int nID, long tm)
{
	tm = tm-8*3600;
	if(tm < 0)
		tm += 86400;
	CTime tmpTime(tm);
	CDateTimeCtrl* pTimeCtrl = (CDateTimeCtrl*)GetDlgItem(nID);
	if(pTimeCtrl)
	{
		pTimeCtrl->SetTime(&tmpTime);
	}
}

long CTrainPlanParaDlg::GetTimeCtrlTime(int nID)
{
	CDateTimeCtrl* pTimeCtrl = (CDateTimeCtrl*)GetDlgItem(nID);
	if(pTimeCtrl)
	{
		long time = 0;
		CString strText, tmp;
		pTimeCtrl->GetWindowText(strText);
		tmp = strText.Left(strText.Find(':'));
		time = atoi(tmp)*3600;
		strText = strText.Right(strText.GetLength()-strText.Find(':')-1);
		tmp = strText.Left(strText.Find(':'));
		time += atoi(tmp)*60;
		strText = strText.Right(strText.GetLength()-strText.Find(':')-1);
		time += atoi(strText);
		
		return time;
	}

	return 0;
}

void CTrainPlanParaDlg::SetCenterPosition()
{
	CRect rect, rect1;
	CWnd* pWnd = this->GetParent();
	if(NULL != pWnd)
	{
		pWnd->GetWindowRect(&rect);
		this->GetWindowRect(&rect1);
		int x = rect.CenterPoint().x-rect1.Width()/2;
		int y = rect.CenterPoint().y-rect1.Height()/2;
		this->SetWindowPos(pWnd, x,  y, rect1.Width(), rect1.Height(), SWP_SHOWWINDOW);
	}
}

void CTrainPlanParaDlg::OnCheckTrack() 
{
	int lock_track = m_ctrlTrackButton.GetCheck();
	if(lock_track)
	{
		m_combo_track.EnableWindow (FALSE);
		m_pDoc->sysprint(0, 0, "%s", "�ɵ�����");
	}
	else
	{
		m_combo_track.EnableWindow (TRUE);
		m_pDoc->sysprint(0, 0, "%s", "�ɵ�δ����");
	}
}

void CTrainPlanParaDlg::OnCheckAbsTime() 
{
	if(m_ctrlAbsTimeButton.GetCheck())
		m_next_abs_runtime.EnableWindow (TRUE);
	else
		m_next_abs_runtime.EnableWindow (FALSE);
}

void CTrainPlanParaDlg::OnOK() 
{
	UpdateData(TRUE);
	
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		return;
	}

	TgTRAIN_RECORD *tmp = pTrain->GetRecordByRecIndex(m_rec_index);
	if((tmp == NULL) || (tmp!=NULL && tmp->station != m_foucs_station))
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		return;
	}

	CTG_TRAIN train = *pTrain;
	TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(m_rec_index);
	if(rec == NULL) 
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		this->DestroyWindow();
		return;
	}

	bool b_plan_time_readonly=m_pDoc->m_ptgctrl->mConfigTg.IsDisableModifyPlanTm();
	if(mTgType != TG_TYPE::BASIC_TG && !b_plan_time_readonly)
	{
		CTime arl_day, arl_time;
		CTime dep_day, dep_time;

		m_plan_arl_day.GetTime(arl_day);
		m_plan_arl_time.GetTime(arl_time);
		m_plan_dep_day.GetTime(dep_day);
		m_plan_dep_time.GetTime(dep_time);

		CTime arl(arl_day.GetYear(), arl_day.GetMonth(), arl_day.GetDay(),
			arl_time.GetHour(), arl_time.GetMinute(), arl_time.GetSecond());
		
		CTime dep(dep_day.GetYear(), dep_day.GetMonth(), dep_day.GetDay(),
			dep_time.GetHour(), dep_time.GetMinute(), dep_time.GetSecond());

		if (rec->plan_arrive!=arl.GetTime() || rec->plan_depart!=dep.GetTime())
		{
			CTime prev_arl,prev_dep;
			prev_arl=CTime(rec->plan_arrive);
			prev_dep=CTime(rec->plan_depart);

			m_pDoc->sysprint(0,0,"�޸ļƻ��߲���,����Ա�޸� �г�:%s �� %s ��ͼ��ʱ��, ԭͼ��ʱ��(%s-%s) ��Ϊ (%s-%s)",m_train_id, m_station, prev_arl.Format("%Y-%m-%d %H:%M:%S"), prev_dep.Format("%Y-%m-%d %H:%M:%S"), arl.Format("%Y-%m-%d %H:%M:%S"), dep.Format("%Y-%m-%d %H:%M:%S"));
		}
		rec->plan_arrive = arl.GetTime();
		rec->plan_depart = dep.GetTime();
	}

	int nCurSel = 0;
	nCurSel = m_combo_track.GetCurSel();//add by cf after 8-24
	if(nCurSel==-1)
		m_nTrackNo=255;
	else
		m_nTrackNo = m_combo_track.GetItemData(nCurSel);//add by cf after 8-24

	
	//next abs
	nCurSel = m_combo_next_abs.GetCurSel();
	int nNextAbsLine;
	if(nCurSel != -1 )
	{
		nNextAbsLine = abs_line[nCurSel];
		m_nCurrExit = m_pDoc->ConvertAbsnumStationToEntry(rec->station,nNextAbsLine);
	}
	else
	{
		nNextAbsLine = -1;
		m_nCurrExit = m_exit;
	}

	TgTRAIN_RECORD *next_rec = train.GetRecordByRecIndex(m_rec_index + 1);
	if(next_rec != NULL)
	{
		if(nNextAbsLine == -1)
		{
			m_nNextEntry = next_rec->entry;
		}
		else
		{
			m_nNextEntry = m_pDoc->ConvertAbsnumStationToEntry(next_rec->station,nNextAbsLine);
		}
	}

	if(nNextAbsLine != -1)
	{
		ABS_LINE lineInfo;
		if(m_pDoc->GetABSLine(nNextAbsLine, lineInfo))
		{
			if(lineInfo.nDirect == 3)
			{
				ClearFlagReverse(rec->flag);
			}
			else
			{
				if(rec->station != lineInfo.nStartStation)
					SetFlagReverse(rec->flag);
				else
					ClearFlagReverse(rec->flag);
			}
		}
	}

	//previous abs
	nCurSel = m_combo_previous_abs.GetCurSel();
	int nPreviousAbsLine;
	if(nCurSel != -1 )
	{
		nPreviousAbsLine = abs_line_previous[nCurSel];
		m_nCurrEntry = m_pDoc->ConvertAbsnumStationToEntry(rec->station,nPreviousAbsLine);
	}
	else
	{
		nPreviousAbsLine = -1;
		m_nCurrEntry = m_entry;
	}

	TgTRAIN_RECORD *previous_rec = train.GetRecordByRecIndex(m_rec_index -1);
	if(previous_rec != NULL)
	{
		if(nPreviousAbsLine == -1)
		{
			m_nPrevExit = previous_rec->exit;
		}
		else
		{
			m_nPrevExit = m_pDoc->ConvertAbsnumStationToEntry(previous_rec->station,nPreviousAbsLine);
		}
	}

	// 20141126������Ҫ����� �ƻ��߲������޸Ĺɵ�ʱ ��ʻ��ǵ������εĵ����������б���
	if (pTrain->myTrainInfo.b_elec_rank)
	{
		const CStationDef *pStationDef=NULL;
		for(int i=0; i<MAX_STATION_PER_SECTION; i++)
		{
			if(m_pDoc->m_pStationDef[i] && m_pDoc->m_pStationDef[i]->GetStationNo()==rec->station)
			{
				pStationDef=m_pDoc->m_pStationDef[i];
				break;
			}
		}
		if(pStationDef==NULL)
			return;

		if ( !pStationDef->isCatenary(m_nTrackNo))
		{
			CString strText;
			strText.Format("����%s, %s, �ɵ� %s �޵�����", pTrain->GetTrainId(), m_pDoc->GetStationName(rec->station), m_pDoc->GetSideName(rec->station, m_nTrackNo));

			if(MessageBox(strText+", ��ȷʵ�޸���?", "����", MB_YESNO|MB_ICONHAND)==IDNO)
			{
				strText = "���ݱ���("+strText+") ����Աȡ���޸Ĺɵ�";
				m_pDoc->sysprint(0,0,strText.GetBuffer());
				return;
			}
			
			strText = "����Ա���Ա���("+strText+") ����޸Ĺɵ�";
			m_pDoc->sysprint(0,0,strText.GetBuffer());
		}
	}
	
	if(SendTrainUpdate(&train))
		this->DestroyWindow();
}

BOOL CTrainPlanParaDlg::SendTrainUpdate(CTG_TRAIN* pTrain)
{
	TgTRAIN_RECORD *rec,*next_rec,*prev_rec;
	rec = pTrain->GetRecordByPosition (m_rec_index);
	next_rec = pTrain->GetRecordByPosition (m_rec_index+1 );
	prev_rec = pTrain->GetRecordByPosition (m_rec_index-1 );

	if(rec == NULL)
		return FALSE;

	// ��Сͣվʱ��
	rec->min_stop_time = GetTimeCtrlTime(IDC_DATETIMEPICKER8);
	if(rec->min_stop_time<=0)
		rec->min_stop_time=0;

	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsEnableMinTimeTechStop())
	{
		CString text;
		text.Format("��ע��, ��վӪҵʱ��Ϊ%0.2d:%0.2d:%0.2d", rec->min_stop_time/3600, rec->min_stop_time/60, rec->min_stop_time%60);
		if(rec->min_stop_time==0)
		{
			if(!IsTechStopTrain(*rec))
			{
				if(MessageBox(text+", ��վ�Ƿ�����Ϊ����ͣ��?", "��ʾ", MB_OK|MB_ICONQUESTION|MB_YESNO)==IDYES)
				{
					SetTechStopTrain(*rec);
					m_pDoc->sysprint(0,0,"�޸ļƻ��߲���,�г�:%s �� %s ��ҵʱ��Ϊ %d ����Ϊ����ͣ��",m_train_id, m_station,rec->min_stop_time);
				}
			}
		}
		else
		{
			if(IsTechStopTrain(*rec))
			{
				if(MessageBox(text+", ��վ�Ƿ�����Ϊ�Ǽ���ͣ��?", "��ʾ", MB_OK|MB_ICONQUESTION|MB_YESNO)==IDYES)
				{
					ClearTechStopTrain(*rec);
					m_pDoc->sysprint(0,0,"�޸ļƻ��߲���,�г�:%s �� %s ��ҵʱ��Ϊ %d ����Ϊ�Ǽ���ͣ��",m_train_id, m_station,rec->min_stop_time);
				}
			}
		}
	}
	
	m_pDoc->sysprint(0,0,"�޸ļƻ��߲���,�޸ĺ�:�г� %s �� %s ��ҵʱ��Ϊ %d",m_train_id, m_station,rec->min_stop_time);
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsLockSideNotTechStop() && pTrain->GetTrainType() == TYPE::PASSENGER)
	{
		if(rec->min_stop_time!=0 && !IsTechStopTrain(*rec))
		{
			m_ctrlTrackButton.SetCheck(TRUE);
		}
	}
	// �ɵ����� �������׶�
	// 1. �޸Ĺɵ�
	bool bChangeSide=false;
	if(rec->arrive_side != m_nTrackNo || rec->depart_side != m_nTrackNo)
	{
		bChangeSide=true;
		m_pDoc->sysprint(0,0,"ͨ���޸ļƻ��߲������޸��г�:%s �� %s �ɵ�:%d->%d %d->%d",m_train_id, m_station,
			rec->arrive_side,m_nTrackNo,rec->depart_side,m_nTrackNo);

		rec->arrive_side = m_nTrackNo;
		rec->depart_side = m_nTrackNo;

	}
	// 2. �����ɵ�
	int lock_track = m_ctrlTrackButton.GetCheck();
	if(lock_track)
	{
		SetForceSide(*rec);
		m_pDoc->sysprint(0,0,"�г�:%s %s �ɵ�%d-%d����",m_train_id, m_station,rec->arrive_side,rec->depart_side);
	}
	else
	{
		ReSetForceSide(*rec);
		m_pDoc->sysprint(0,0, "�г�:%s %s �ɵ�%d-%d������",m_train_id, m_station,rec->arrive_side,rec->depart_side);
	}

	// next abs line 
	rec->exit = this->m_nCurrExit;
	rec->entry = this->m_nCurrEntry;

	if(next_rec != NULL)
	{
		next_rec->entry = this->m_nNextEntry;
	}
	if(prev_rec != NULL)
	{
		prev_rec->exit = this->m_nPrevExit;
	}

	if(m_sidePassButton.GetCheck()==1)
	{
		ReSetForcePassByMainSide(*rec);
		m_pDoc->sysprint(0,0,"�г�:%s %s �������ͨ��",m_train_id, m_station);
	}
	else
	{
		SetForcePassByMainSide(*rec);
		m_pDoc->sysprint(0,0,"�г�:%s %s ��ֹ����ͨ��",m_train_id, m_station);
	}

	if(this->m_check_adjust_start)
	{
		pTrain->myTrainInfo.adjust_begin_sta = rec->rec_index;
	}
	else
	{
		pTrain->myTrainInfo.adjust_begin_sta = -1;
	}

	//permit stop train 
	//if(this->m_check_stop_train)
	//{
	//	SetForcePass(*rec);
	//	m_pDoc->sysprint(0,0,"�г�:%s %s �涨ͨ��",m_train_id, m_station);
	//}
	//else
	//{
	//	ReSetForcePass(*rec);
	//	m_pDoc->sysprint(0,0,"�г�:%s %s ȡ���涨ͨ��",m_train_id, m_station);
	//}

	//if (m_pDoc->m_ptgctrl->mConfigTg.m_context.IsParaDlgCheckEntryConnect())
	if(!m_pDoc->SideSetEntry(this->m_hWnd, rec->arrive_side, rec->depart_side, m_rec_index, pTrain))
	{
		return FALSE;
	}

	long nArriveTime = 0;
	long nDepartTime = 0;
	if(mTgType == TG_TYPE::BASIC_TG)
	{
		nArriveTime = GetTimeCtrlTime(IDC_DATETIMEPICKER6);
		nDepartTime	= GetTimeCtrlTime(IDC_DATETIMEPICKER5);
		if((nArriveTime!=(rec->arrive%86400)) || (nDepartTime!=(rec->depart%86400)))
		{
			if(nArriveTime>nDepartTime)
			{
				if(IDYES!=MessageBox("���㲻Ӧ���ڵ���,��ȷ�������޸���", "��ʾ", MB_YESNO|MB_ICONQUESTION))
				{
					return FALSE;
				}
			}
			m_pDoc->sysprint(0,0,"ά��Աѡ����ʹ�����йɵ� %d-%d", rec->arrive_side, rec->depart_side);
				m_pDoc->sysprint(0,0,"ά��Ա�޸ĳ��� %s �� %s ����ʱ��:%d=>%d ����ʱ��:%d=>%d", 
					m_train_id, m_station, rec->arrive, nArriveTime, rec->depart, nDepartTime);
			rec->arrive = nArriveTime;
			rec->depart = nDepartTime;
		}
	}
	else
	{
		CTime timeArlTime, timeDepTime;
		m_arrival.GetTime(timeArlTime);
		m_depart.GetTime(timeDepTime);
		nArriveTime=timeArlTime.GetTime();
		nDepartTime=timeDepTime.GetTime();
		if(nArriveTime>nDepartTime)
		{
			MessageBox("���㲻�����ڵ���,�������޸�", "����", MB_OK|MB_ICONHAND);
			return FALSE;
		}
		
		if((nArriveTime!=rec->arrive) || (nDepartTime!=rec->depart))
		{
			CString strSideName=m_pDoc->GetSideName(rec->station,rec->arrive_side);
			if((rec->arrive==rec->depart) && (nArriveTime!=nDepartTime))
			{
				CString str;
				str.Format("��ע��:���� %s �� %s ԭΪ'ͨ��',�㽫���޸�Ϊ'����',ȷ��ͣ���ɵ� %s �Ƿ���ȷ?\n���ʹ�����йɵ����'��'\n�����޸Ĺɵ����'��'",
					m_train_id, m_station, strSideName);
				m_pDoc->sysprint(0,0,"%s", str);
				if(MessageBox(str, "��ʾ", MB_YESNO|MB_ICONHAND)!=IDYES)
				{
					m_pDoc->sysprint(0,0,"����Աѡ�񷵻��޸Ĺɵ�");
					return FALSE;
				}
				m_pDoc->sysprint(0,0,"����Աѡ����ʹ�����йɵ� %d-%d", rec->arrive_side, rec->depart_side);
				m_pDoc->sysprint(0,0,"����Ա�޸ĳ��� %s �� %s ����ʱ��:%d=>%d ����ʱ��:%d=>%d", 
					m_train_id, m_station, rec->arrive, nArriveTime, rec->depart, nDepartTime);
				rec->arrive = nArriveTime;
				rec->depart = nDepartTime;
			}
			else if((rec->arrive!=rec->depart) && (nArriveTime==nDepartTime))
			{
				CString str;
				str.Format("��ע��:���� %s �� %s ԭΪ'����',�㽫���޸�Ϊ'ͨ��',ȷ��ͨ���ɵ� %s �Ƿ���ȷ?\n���ʹ�����йɵ����'��'\n�����޸Ĺɵ����'��'",
					m_train_id, m_station, strSideName);
				m_pDoc->sysprint(0,0,"%s", str);
				if(MessageBox(str, "��ʾ", MB_YESNO|MB_ICONHAND)!=IDYES)
				{
					m_pDoc->sysprint(0,0,"����Աѡ�񷵻��޸Ĺɵ�");
					return FALSE;
				}
				m_pDoc->sysprint(0,0,"����Աѡ����ʹ�����йɵ� %d-%d", rec->arrive_side, rec->depart_side);
				m_pDoc->sysprint(0,0,"����Ա�޸ĳ��� %s �� %s ����ʱ��:%d=>%d ����ʱ��:%d=>%d", 
					m_train_id, m_station, rec->arrive, nArriveTime, rec->depart, nDepartTime);
				rec->arrive = nArriveTime;
				rec->depart = nDepartTime;
			}
			else
			{
				m_pDoc->sysprint(0,0,"����Աѡ����ʹ�����йɵ� %d-%d", rec->arrive_side, rec->depart_side);
				m_pDoc->sysprint(0,0,"����Ա�޸ĳ��� %s �� %s ����ʱ��:%d=>%d ����ʱ��:%d=>%d", 
					m_train_id, m_station, rec->arrive, nArriveTime, rec->depart, nDepartTime);
				rec->arrive = nArriveTime;
				rec->depart = nDepartTime;
			}
		}
	}
	
	// ��һ��������ʱ��
	if(mTgType != TG_TYPE::BASIC_TG)
	{
		if(m_ctrlAbsTimeButton.GetCheck())
		{
			long lAbsTime = GetTimeCtrlTime(IDC_DATETIMEPICKER9);
			SetSpecifyAbsRunTime(rec, lAbsTime/60);
		}
	}

	if(bChangeSide)
	{
		if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, rec, pTrain))
		{
			return FALSE;
		}
	}

	CStringArray ary;
	m_pDoc->CheckTrainSideInfo(pTrain, m_rec_index, ary);
	
	if(ary.GetCount()>0)
	{
		m_pDoc->sysprint(4,0,"�޸ļƻ��߲�����������:");

		CString strText, tmp;
		for(int i=0; i<ary.GetCount(); i++)
		{
			tmp=ary.GetAt(i);
			m_pDoc->sysprint(4,0,"%s", tmp);
			strText += tmp;
			strText += "\n";
		}

		if(MessageBox(strText+"\n��ȷʵ�޸���?", "����", MB_YESNO|MB_ICONHAND)==IDYES)
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ����޸ļƻ��߲���");
		}
		else
		{
			m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸ļƻ��߲���");
			return FALSE;
		}			
	}

	m_pDoc->SendUpdateTrain(pTrain, SCHD_UPDATE, TRAIN_PLAN_PARA, rec->station, rec->rec_index);
	return TRUE;
}

void CTrainPlanParaDlg::InitIdentify()
{ 
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_DATETIMEPICKER7);
	pWnd->EnableWindow(FALSE);

	if(!m_pDoc->IsCanEditTrain())
	{
		pWnd = GetDlgItem(IDC_DATETIMEPICKER5);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER6);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER2);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER3);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER4);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER8);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER9);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_TRACK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_TRACK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_ABS_TIME);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_NEXTABS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_PREVIOUSABS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_NO_STOP_TRAIN);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_PASS_SIDE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
	}
	else
	{
		pWnd = GetDlgItem(IDC_DATETIMEPICKER5);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_DATETIMEPICKER6);
		pWnd->EnableWindow(TRUE);
	}
}

HBRUSH CTrainPlanParaDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	if(GetDlgItem(IDC_COMBO_TRACK)->m_hWnd==pWnd->m_hWnd )
	{
		pDC->SetBkColor(CLR_LIGHTGREEN);
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CTrainPlanParaDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
    delete this;
	CDialog::PostNcDestroy();
}

void CTrainPlanParaDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->DestroyWindow();
}

// ����仯
void CTrainPlanParaDlg::OnDtnDatetimechangeDatetimepicker6(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetStopTime();
	*pResult = 0;
}

// ����仯
void CTrainPlanParaDlg::OnDtnDatetimechangeDatetimepicker5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetStopTime();
	*pResult = 0;
}

// ͣ��ʱ�ֱ仯
void CTrainPlanParaDlg::OnDtnDatetimechangeDatetimepicker7(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	long nStopTime = GetTimeCtrlTime(IDC_DATETIMEPICKER7);
	if(mTgType == TG_TYPE::BASIC_TG)
	{
		SetTimeCtrlTime(IDC_DATETIMEPICKER5, GetTimeCtrlTime(IDC_DATETIMEPICKER6)+nStopTime);
	}
	else
	{
		CTime timeArlTime;
		m_arrival.GetTime(timeArlTime);
		CTime timeDepTime(timeArlTime.GetTime() + nStopTime);

		m_depart.SetTime(&timeDepTime);
	}

	*pResult = 0;
}

void CTrainPlanParaDlg::SetStopTime()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		OnBnClickedCancel();
		return;
	}

	TgTRAIN_RECORD *tmp = pTrain->GetRecordByRecIndex(m_rec_index);
	if((tmp == NULL) || (tmp!=NULL && tmp->station != m_foucs_station))
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		OnBnClickedCancel();
		return;
	}
	
	CTG_TRAIN train = *pTrain;
	TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(m_rec_index);
	if(rec == NULL) 
	{
		MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
		OnBnClickedCancel();
		return;
	}

	long nStopTime = 0;
	if(mTgType == TG_TYPE::BASIC_TG)
	{
		nStopTime = GetTimeCtrlTime(IDC_DATETIMEPICKER5) -
				GetTimeCtrlTime(IDC_DATETIMEPICKER6);
		
		if(nStopTime<0)
		{
			if(IDYES!=MessageBox("��ȷ��,�������ڵ�����?", "��ʾ", MB_YESNO|MB_ICONQUESTION))
			{
				SetTimeCtrlTime(IDC_DATETIMEPICKER5, rec->depart);
				SetTimeCtrlTime(IDC_DATETIMEPICKER6, rec->arrive);
				return;
			}
		}
		SetTimeCtrlTime(IDC_DATETIMEPICKER7, nStopTime);
	}
	else
	{
		CTime timeArlTime, timeDepTime;
		m_arrival.GetTime(timeArlTime);
		m_depart.GetTime(timeDepTime);
		if(timeDepTime.GetTime()>=timeArlTime.GetTime())
		{
			nStopTime = timeDepTime.GetTime() - timeArlTime.GetTime();
			SetTimeCtrlTime(IDC_DATETIMEPICKER7, nStopTime);
		}
		else
		{
			MessageBox("���㲻�����ڵ���,�������޸�", "����", MB_OK|MB_ICONHAND);
			CTime timeArlTime(rec->arrive), timeDepTime(rec->depart);
			m_arrival.SetTime(&timeArlTime);
			m_depart.SetTime(&timeDepTime);
		}
	}
}

void CTrainPlanParaDlg::RemoveSameLineNo(int& lineno_count, int lineno[])
{
	int count=0;
	int line[100];
	memset(line, 0, sizeof(line));
	for(int m=0; m<lineno_count; m++)
	{
		for(int n=0; n<count; n++)
		{
			if(lineno[m]==line[n])
			{
				break;
			}
		}
		if(n>=count && lineno[m]!=0)
		{
			line[count]=lineno[m];
			count++;
		}
	}

	memset(lineno, 0, sizeof(lineno));
	lineno_count=count;
	for(int m=0; m<lineno_count; m++)
	{
		lineno[m]=line[m];
	}
}

void CTrainPlanParaDlg::OnCbnSelPreviousAbs()
{
	int result=0;
	do
	{
		int nCurSel = m_combo_previous_abs.GetCurSel();
		if(nCurSel==-1)
			break;
		int previous_abs = abs_line_previous[nCurSel];
		int entry = m_pDoc->ConvertAbsnumStationToEntry(m_rec.station, previous_abs);

		int nSel = m_combo_track.GetCurSel();
		if(nSel>=0)
		{
			int nTrackNo = m_combo_track.GetItemData(nSel);
			if(!m_pDoc->IsEntrySideConnect(m_rec.station, nTrackNo, entry))
			{
				CString strSideName;
				m_combo_track.GetLBText(nSel, strSideName);
				CString str;
				str.Format("��ע��,%s ���%d �� %s ����ͨ,��ȷ���޸���?", m_pDoc->GetStationName(m_rec.station), entry, strSideName);
				if(IDYES!=MessageBox(str, "����", MB_YESNO|MB_ICONHAND))
				{
					break;
				}
			}
		}

		m_rec.entry=entry;
		m_pDoc->sysprint(0,0,"�ƻ������޸���һ����Ϊ:%d.", previous_abs);
		
		m_entry = entry;
		m_nCurrEntry = entry;
		UpdateData(FALSE);

		result=1;
	}while(0);

	if(result==0)
	{
		WORD previous_abs_no,nDir;
		m_pDoc->ConvertStationEntryToAbsnum(m_rec.station, m_rec.entry, &previous_abs_no,&nDir);
		
		int nSel = -1;
		for(int i = 0; i < m_combo_previous_abs.GetCount(); i++)
		{
			if(abs_line_previous[i] == previous_abs_no)
			{
				nSel=i;
				break;
			}
		}
		m_combo_previous_abs.SetCurSel(nSel);
	}
}

void CTrainPlanParaDlg::OnCbnSelNextAbs()
{
	int result=0;
	do
	{
		int nCurSel = m_combo_next_abs.GetCurSel();
		if(nCurSel==-1)
			break;
		int next_abs = abs_line[nCurSel];
		int exit = m_pDoc->ConvertAbsnumStationToEntry(m_rec.station, next_abs);
		
		int nSel = m_combo_track.GetCurSel();
		if(nSel>=0)
		{
			int nTrackNo = m_combo_track.GetItemData(nSel);
			if(!m_pDoc->IsSideExitConnect(m_rec.station, nTrackNo, exit))
			{
				CString strSideName;
				m_combo_track.GetLBText(nSel, strSideName);
				CString str;
				str.Format("��ע��,%s ����%d �� %s ����ͨ,��ȷ���޸���?", m_pDoc->GetStationName(m_rec.station), exit, strSideName);
				if(IDYES!=MessageBox(str, "����", MB_YESNO|MB_ICONHAND))
				{
					break;
				}
			}
		}

		m_rec.exit=exit;
		m_pDoc->sysprint(0,0,"�ƻ������޸���һ����Ϊ:%d.", next_abs);

		m_exit = exit;
		m_nCurrExit = exit;
		UpdateData(FALSE);
		result=1;
	}while(0);
	
	if(result==0)
	{
		WORD next_abs_no,nDir;
		m_pDoc->ConvertStationEntryToAbsnum(m_rec.station, m_rec.exit, &next_abs_no,&nDir);

		int nSel = -1;
		for(int i = 0; i < m_combo_next_abs.GetCount(); i++)
		{
			if(abs_line[i] == next_abs_no)
			{
				nSel=i;
				break;
			}
		}
		m_combo_next_abs.SetCurSel(nSel);
	}
}

void CTrainPlanParaDlg::OnCbnSelSide()
{
	int result=0;
	
	do
	{
		CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
		if(NULL == pTrain)
		{
			MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
			OnBnClickedCancel();
			return;
		}
		CTG_TRAIN train = *pTrain;
		TgTRAIN_RECORD *prec = train.GetRecordByRecIndex(m_rec_index);
		TgTRAIN_RECORD *prec_pre=train.GetRecordByRecIndex(m_rec_index-1);
		TgTRAIN_RECORD *prec_next=train.GetRecordByRecIndex(m_rec_index+1);
		if((prec == NULL) || (prec!=NULL && prec->station != m_foucs_station))
		{
			MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ļƻ��߲���", "����");
			OnBnClickedCancel();
			return;
		}

		if(mTgType == TG_TYPE::BASIC_TG)
		{
			long nArriveTime = GetTimeCtrlTime(IDC_DATETIMEPICKER6);
			long nDepartTime	= GetTimeCtrlTime(IDC_DATETIMEPICKER5);
			if((nArriveTime!=(prec->arrive%86400)) || (nDepartTime!=(prec->depart%86400)))
			{
				prec->arrive = nArriveTime;
				prec->depart = nDepartTime;
			}
		}
		else
		{
			CTime timeArlTime, timeDepTime;
			m_arrival.GetTime(timeArlTime);
			m_depart.GetTime(timeDepTime);
			prec->arrive = timeArlTime.GetTime();
			prec->depart = timeDepTime.GetTime();
		}

		int nCurSel = m_combo_track.GetCurSel();
		if(nCurSel==-1)
			break;
		CString strSideName;
		m_combo_track.GetLBText(nCurSel, strSideName);
		BYTE nTrackNo = m_combo_track.GetItemData(nCurSel);
		
		nCurSel = m_combo_next_abs.GetCurSel();
		if(nCurSel>=0)
		{
			prec->exit = m_pDoc->ConvertAbsnumStationToEntry(m_foucs_station, abs_line[nCurSel]);
			if(NULL != prec_next)
			{
				prec_next->entry=m_pDoc->ConvertAbsnumStationToEntry(prec_next->station, abs_line[nCurSel]);
			}
		}

		nCurSel = m_combo_previous_abs.GetCurSel();
		if(nCurSel>=0)
		{
			prec->entry = m_pDoc->ConvertAbsnumStationToEntry(m_foucs_station, abs_line_previous[nCurSel]);
			if(NULL != prec_pre)
			{
				prec_pre->exit=m_pDoc->ConvertAbsnumStationToEntry(prec_pre->station, abs_line_previous[nCurSel]);
			}
		}

		// ��·���޸Ĺɵ� �޸Ĺɵ���ʾ�û�
		if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, prec, &train))
		{
			break;
		}
		
		if(!m_pDoc->SideSetEntry(this->m_hWnd, nTrackNo, nTrackNo, m_rec_index, &train))
		{
			break;
		}
		SetSideFromDispatcher(*prec);

		prec->arrive_side=nTrackNo;
		prec->depart_side=nTrackNo;

		CStringArray ary;
		m_pDoc->CheckTrainSideInfo(&train, m_rec_index, ary);
		if(ary.GetCount()>0)
		{
			m_pDoc->sysprint(4,0,"�޸ļƻ��߲�����--�ɵ�--��������:");

			CString strText, tmp;
			for(int i=0; i<ary.GetCount(); i++)
			{
				tmp=ary.GetAt(i);
				m_pDoc->sysprint(4,0,"%s", tmp);
				strText += tmp;
				strText += "\n";
			}

			if(MessageBox(strText+"\n��ȷʵ�޸���?", "����", MB_YESNO|MB_ICONHAND)==IDYES)
			{
				m_pDoc->sysprint(4,0,"����Ա������������, ����޸Ĺɵ�");
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸Ĺɵ�");
				break;
			}			
		}

		m_rec.arrive_side=nTrackNo;
		m_rec.depart_side=nTrackNo;
		result=1;
	}while(0);

	// �ָ�ѡ��ǰ�ɵ�
	if(result==0)
	{	
		int nSel = -1;
		for(int i = 0; i < m_combo_track.GetCount(); i++)
		{
			if(m_combo_track.GetItemData(i) == m_rec.arrive_side)
			{
				nSel=i;
				break;
			}
		}
		m_combo_track.SetCurSel(nSel);
		m_pDoc->sysprint(0,0,"�ָ��޸�ǰ�ƻ��߲����ɵ�:%d", m_rec.arrive_side);
	}
}

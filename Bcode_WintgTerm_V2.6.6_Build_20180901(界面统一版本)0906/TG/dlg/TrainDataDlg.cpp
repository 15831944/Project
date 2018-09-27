// TrainDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainDataDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainDataDlg dialog
#define TRAIN_ARRIVE	1
#define TRAIN_DEPART	2
const int MAX_COL=18;

CTrainDataDlg::CTrainDataDlg(CTG_TRAIN* train, CTGDoc *pDoc, int tg_type, CWnd* pParent)
	: CDialog(CTrainDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
 	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = MAX_COL;
    m_pGridCtrl = NULL;
	
	if(train)
		m_Train = *train;
	m_RecordsPerPrintPage = 50;
}

void CTrainDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrainDataDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainDataDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainDataDlg message handlers

BOOL CTrainDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		m_pDoc->SetBasicTrainDate(&m_Train);

	int m_recNum = m_Train.GetRecordNum ();

	CString str;
	str.Format("%s(%u)�г���ϸ���� - ",  m_pDoc->GetTrainShowId(m_Train),m_Train.GetTrainIndex());

	if (m_pDoc->GetTgType() != TG_TYPE::BASIC_TG)
	{
		CString t; 
		if (IsBasicScheduleTrain(m_Train.myTrainInfo))
		{
			t.Format("ͼ���г� %d", m_Train.myTrainInfo.basic_train_index);
		}
		else if (IsAdditionScheduleTrain(m_Train.myTrainInfo))
		{
			t.Format("���������г�");
		}
		else if (IsReportTrueTrain(m_Train.myTrainInfo))
		{
			t.Format("�����泵������");
		}
		else if (IsReportFakeTrain(m_Train.myTrainInfo))
		{
			t.Format("����ٳ�������");
		}
		else if (IsFreeDrawTrain(m_Train.myTrainInfo))
		{
			t.Format("���ɻ���");
		}
		else if (IsPathWayCreateTrain(m_Train.myTrainInfo))
		{
			t.Format("��·����");
		}
		else if (IsProbeCreateTrain(m_Train.myTrainInfo))
		{
			t.Format("�Զ�����");
		}
		else if (IsInformalCreateTrain(m_Train.myTrainInfo))
		{
			t.Format("ȱ�ٳ�������");
		}

		str += t;
	}

	SetWindowText(str);

	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return FALSE;

		// Create the Gridctrl window
		CStatic *pWnd;
		pWnd = (CStatic *)GetDlgItem(IDC_GRID);
		CRect rect;
		pWnd->GetClientRect(rect);

		m_pGridCtrl->Create(rect, this, IDC_GRID);

		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);
		m_pGridCtrl->EnableDragAndDrop(FALSE);
		m_nRows = m_recNum +1;

		try 
		{
			m_pGridCtrl->SetRowCount(m_nRows);
			m_pGridCtrl->SetColumnCount(m_nCols);
			m_pGridCtrl->SetFixedRowCount(m_nFixRows);
			m_pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return FALSE;
		}
	}

	show_data(m_pGridCtrl->GetColumnCount());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainDataDlg::show_data(int col_count) 
{
	int m_recNum = m_Train.GetRecordNum();
	
	TCHAR rgtsz[19][20] = { _T("���"),_T("��վ"),_T("ͼ������"),_T("ʵ�ʵ���"),_T("�����"), _T("ͼ������"),_T("ʵ�ʳ���"), _T("�����"),_T("ʵ�ʹɵ�"),_T("ͼ���ɵ�"),_T("���"),_T("����"),_T("���ﳵ��"),_T("��������"),_T("�ɵ��趨"),_T("����ʱ���趨"),_T("����ʱ���趨"),_T("ͼ�������趨"),_T("ͼ�������趨")};
	if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
	{
		sprintf(rgtsz[2], _T("��������"));
		sprintf(rgtsz[3], _T("����ʱ��"));
		sprintf(rgtsz[5], _T("��������"));
		sprintf(rgtsz[6], _T("����ʱ��"));
	}

	CRect rect;

	DWORD dwTextStyle = DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_EDITCONTROL;

	int row,col;

	BOOL m_bShowDate = TRUE;
	CTime timetemp;
	CString temp;
	long nDelta = 0;
	long nHour = 0;
	long nMinute = 0;

	// fill rows/cols with text
	for (row = 0; row <= m_nRows; row++)
	{
		const TgTRAIN_RECORD *rec;
		
		if(row != 0)
		{
			rec = m_Train.GetRecordByRecIndex(row-1);
			if(rec == NULL) 
				continue;
		}
		for ( col = 0; col < col_count; col++)
		{ 
			
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row == 0)
			{
				Item.nFormat = DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX;
				Item.strText.Format(rgtsz[col]);	    
			}
			else
			{ 
				Item.nFormat = dwTextStyle;
				temp.Empty();
				switch(col)
				{
				case 0:
					temp.Format(_T("%d"),row);
					break;
				case 1:
					temp.Format(_T("%s"),m_pDoc->GetStationName(rec->station));
					break;
				case 2:  
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{   // ��������
						temp.Format("%d", rec->arrive/86400);
					}
					else
					{
						timetemp = CTime(rec->plan_arrive);
						if(m_bShowDate)
							temp = timetemp.Format("%H:%M:%S (%m-%d)");
						else
							temp = timetemp.Format("%H:%M");
					}

					if(IsFlagStart(rec->flag))//ʼ�������ŵ���󣬲���ʾֵ
						temp = "";				
					break;
				case 3:
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{
						// ����ʱ��
						temp = CString(ConvertTimetoString(rec->arrive%86400));
					}
					else
					{
						// ʵ�ʵ���
						timetemp = CTime(rec->arrive);
						if(m_bShowDate)
							temp = timetemp.Format("%H:%M:%S (%m-%d)");
						else
							temp = timetemp.Format("%H:%M");

						if(IsActualArrivalTime(rec->adjust_status))//ʵ�ʵ���
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_RED);
					}

					if(IsFlagStart(rec->flag))//ʼ����������ʾֵ
						temp = "";
					break;
				case 4://_T("�����"),
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{
						temp = "����";
					}
					else
					{
						//����֮��
						BYTE nRetFlag = m_pDoc->GetLateTime(rec->plan_arrive, rec->arrive, temp);
						
						if(STATISTIC_FLAG::EARLY == nRetFlag)//���
						{
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_RED);
						}
						else if(STATISTIC_FLAG::RIGHT == nRetFlag)
						{
							temp="����";
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_DARKGREEN);
						}
						else
						{
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_BLUE);
						}		
					}
					if(IsFlagStart(rec->flag))//ʼ����������ʾֵ
						temp = "";

					break;
				case 5:
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{
						// ��������
						temp.Format("%d", rec->depart/86400);
					}
					else
					{
						timetemp = CTime(rec->plan_depart);
						if(m_bShowDate)
							temp = timetemp.Format("%H:%M:%S (%m-%d)");
						else
							temp = timetemp.Format("%H:%M");

					}
					if(IsFlagTerminal(rec->flag))//�յ���������ʾֵ
						temp = "";
					break;
				case 6:
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{
						// ����ʱ��
						temp = CString(ConvertTimetoString(rec->depart%86400));
					}
					else
					{
						// ʵ�ʳ���
						timetemp = CTime(rec->depart);
						if(m_bShowDate)
							temp = timetemp.Format("%H:%M:%S (%m-%d)");
						else
							temp = timetemp.Format("%H:%M");

						if(IsActualDepartTime(rec->adjust_status))//ʵ�ʷ���
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_RED);
					}

					if(IsFlagTerminal(rec->flag))//�յ���������ʾֵ
						temp = "";

					break;
				case 7://_T("�����"),
					if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
					{
						temp = "����";
					}
					else
					{
						BYTE nRetFlag = m_pDoc->GetLateTime(rec->plan_depart, rec->depart, temp);
						
						if(STATISTIC_FLAG::EARLY == nRetFlag)//���
						{
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_RED);
						}
						else if(STATISTIC_FLAG::RIGHT == nRetFlag)
						{
							temp="����";
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_DARKGREEN);
						}
						else
						{
							m_pGridCtrl->SetItemFgColour (Item.row,Item.col,CLR_BLUE);
						}
					}

					if(IsFlagTerminal(rec->flag))//�յ���������ʾֵ
						temp = "";

					break;
				case 8:
					if (rec->arrive_side == rec->depart_side)
					{
						temp.Format("%s",m_pDoc->GetSideName(rec->station, rec->arrive_side));
					}
					else
					{
						temp.Format("%s-%s", 
							m_pDoc->GetSideName(rec->station, rec->arrive_side),
							m_pDoc->GetSideName(rec->station, rec->depart_side));
					}
					break;
				case 9: // 2013.2.28 hjz ����ͼ���ɵ�����ʾ
					if (rec->plan_arrive_side == rec->plan_depart_side)
					{
						temp.Format("%s",m_pDoc->GetSideName(rec->station, rec->plan_arrive_side));
					}
					else
					{
						temp.Format("%s-%s", 
							m_pDoc->GetSideName(rec->station, rec->plan_arrive_side),
							m_pDoc->GetSideName(rec->station, rec->plan_depart_side));
					}
					break;
				case 10:				
					temp.Format(_T("%d"),(short)rec->entry);
					break;
				case 11:				
					temp.Format(_T("%d"),(short)rec->exit);
					break;
				case 12:  //_T("���ﳵ��")
					temp = rec->arrive_train_id;
					break;
				case 13:  //_T("��������")
					temp = rec->depart_train_id;
					break;
				case 14:  //_T("�ɵ��趨") 
					if (IsSideFromBasicSchedule(*rec))
						temp.Format(_T("ͼ���ɵ�"));
					else if (IsSideFromAdditionSchedule(*rec))
						temp.Format(_T("����������"));
					else if (IsSideFromStaticDefine(*rec))
						temp.Format(_T("�̶��ɵ�"));
					else if (IsSideFromDispatcher(*rec))
						temp.Format(_T("�����趨"));
					else if (IsSideFromAutoAdjust(*rec))
						temp.Format(_T("�Զ�ѡ��"));
					else if (IsSideFromStaWatcherChangePlan(*rec))
						temp.Format(_T("��վֵ��Ա"));
					else if (IsSideFromStaWatcherReportActual(*rec))
						temp.Format(_T("�˹�����"));
					else if (IsSideFromLircChangePlan(*rec))
						temp.Format(_T("���ɻ�"));
					else if (IsSideFromLircReportActual(*rec))
						temp.Format(_T("�Զ�����"));
					else if (IsSideFromPathWay(*rec))
						temp.Format(_T("��·�ɵ�"));
					break;

				case 15:  //_T("����ʱ���趨")
					if (IsRecTimeFromDefault(*rec, TRAIN_ARRIVE))
						temp.Format(_T("ȱʡ"));
					else if(IsRecTimeFromAutoAdjust(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�Զ�����"));
					else if(IsRecTimeFromLircChangePlan(*rec, TRAIN_ARRIVE))
						temp.Format(_T("���ɻ�"));
					else if(IsRecTimeFromDispatcherTurnActual(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�˹�תʵ��"));
					else if(IsRecTimeFromStaWatcherChangePlan(*rec, TRAIN_ARRIVE))
						temp.Format(_T("��վֵ��Ա"));
					else if(IsRecTimeFromStaWatcherReport(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�˹�����"));
					else if(IsRecTimeFromLircReportActual(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�Զ�����"));
					else if(IsRecTimeFromLircReportMatch(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�Զ�����ƥ��"));
					else if(IsRecTimeFromSimpleAdjust(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�������"));
					break;

				case 16:  //_T("����ʱ���趨")
					if (IsRecTimeFromDefault(*rec, TRAIN_DEPART))
						temp.Format(_T("ȱʡ"));
					else if(IsRecTimeFromAutoAdjust(*rec, TRAIN_DEPART))
						temp.Format(_T("�Զ�����"));
					else if(IsRecTimeFromLircChangePlan(*rec, TRAIN_DEPART))
						temp.Format(_T("���ɻ�"));
					else if(IsRecTimeFromDispatcherTurnActual(*rec, TRAIN_DEPART))
						temp.Format(_T("�˹�תʵ��"));
					else if(IsRecTimeFromStaWatcherChangePlan(*rec, TRAIN_DEPART))
						temp.Format(_T("��վֵ��Ա"));
					else if(IsRecTimeFromStaWatcherReport(*rec, TRAIN_DEPART))
						temp.Format(_T("�˹�����"));
					else if(IsRecTimeFromLircReportActual(*rec, TRAIN_DEPART))
						temp.Format(_T("�Զ�����"));
					else if(IsRecTimeFromLircReportMatch(*rec, TRAIN_DEPART))
						temp.Format(_T("�Զ�����ƥ��"));
					else if(IsRecTimeFromSimpleAdjust(*rec, TRAIN_DEPART))
						temp.Format(_T("�������"));
					break;

				case 17:   // _T("ͼ�������趨")
					if (IsPlanTimeFromBasicSchedule(*rec, TRAIN_ARRIVE))
						temp.Format(_T("����ͼ"));
					else if (IsPlanTimeFromAdditionSchedule(*rec, TRAIN_ARRIVE))
						temp.Format(_T("��������"));
					else if (IsPlanTimeFromAutoCreate(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�Զ�����"));
					else if (IsPlanTimeFromManualSet(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�˹��趨"));
					break;

				case 18:   // _T("ͼ�������趨")
					if (IsPlanTimeFromBasicSchedule(*rec, TRAIN_ARRIVE))
						temp.Format(_T("����ͼ"));
					else if (IsPlanTimeFromAdditionSchedule(*rec, TRAIN_ARRIVE))
						temp.Format(_T("��������"));
					else if (IsPlanTimeFromAutoCreate(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�Զ�����"));
					else if (IsPlanTimeFromManualSet(*rec, TRAIN_ARRIVE))
						temp.Format(_T("�˹��趨"));
					break;

				default:
					break;
				}
				Item.strText = temp;
			}

			m_pGridCtrl->SetItem(&Item);

		}
	}
	
	for ( col = 0; col < col_count; col++)
	{
		switch (col)
		{
		case 0:
			m_pGridCtrl->SetColumnWidth(col,35);
			break;
		case 1 :
			m_pGridCtrl->SetColumnWidth(col,70);
			break;
		case 2 :
			m_pGridCtrl->SetColumnWidth(col,110);
			break;
		case 3 :
			m_pGridCtrl->SetColumnWidth(col,110);
			break;
		case 4 :
			m_pGridCtrl->SetColumnWidth(col,40);
			break;
		case 5 :
			m_pGridCtrl->SetColumnWidth(col,110);
			break;
		case 6 :
			m_pGridCtrl->SetColumnWidth(col,110);
			break;
		case 7 :
			m_pGridCtrl->SetColumnWidth(col,40);
			break;
		case 8 :
			m_pGridCtrl->SetColumnWidth(col,60);
			break;
		case 9 :
			m_pGridCtrl->SetColumnWidth(col,60);
			break;
		case 10 :
			m_pGridCtrl->SetColumnWidth(col,30);
			break;
		case 11:
			m_pGridCtrl->SetColumnWidth(col,30);
			break;
		case 12:
			m_pGridCtrl->SetColumnWidth(col,60);
			break;
		case 13:
			m_pGridCtrl->SetColumnWidth(col,60);
			break;
		case 14:
			m_pGridCtrl->SetColumnWidth(col,80);
			break;
		case 15:
			m_pGridCtrl->SetColumnWidth(col,80);
			break;
		case 16:
			m_pGridCtrl->SetColumnWidth(col,80);
			break;
		case 17:
			m_pGridCtrl->SetColumnWidth(col,80);
			break;
		case 18:
			m_pGridCtrl->SetColumnWidth(col,80);
			break;
		default:
			break;
		}		
	}

	if (col_count <= 13)
		m_pGridCtrl->ExpandColumnsToFit(TRUE);	
}

void CTrainDataDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(m_pGridCtrl != NULL)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

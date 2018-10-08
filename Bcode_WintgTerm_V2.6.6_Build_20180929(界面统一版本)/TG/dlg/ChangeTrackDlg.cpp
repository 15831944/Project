// ChangeTrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "ChangeTrackDlg.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeTrackDlg dialog


CChangeTrackDlg::CChangeTrackDlg(CWnd* pParent)
	: CDialog(CChangeTrackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeTrackDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = NULL;
	m_rec_index = -1;
	m_train_index = 0;
	m_foucs_station=0;
}


void CChangeTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeTrackDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_COMBO_TRACK, m_combo_arrive_track);
	DDX_Control(pDX, IDC_COMBO_TRACK2, m_combo_depart_track);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeTrackDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeTrackDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_TRACK, OnCbnSelArriveSide)
	ON_CBN_SELCHANGE(IDC_COMBO_TRACK2, OnCbnSelDepartSide)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTrackDlg message handlers

BOOL CChangeTrackDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd *pWnd;

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL != pTrain)
	{
		pWnd = GetDlgItem(IDC_TRAIN_ID);
		pWnd->SetWindowText(m_pDoc->GetTrainShowId(pTrain));
		pWnd->EnableWindow(FALSE);

		TgTRAIN_RECORD *rec = pTrain->GetRecordByRecIndex(m_rec_index);
		if(rec != NULL)
		{
			m_foucs_station=rec->station;
			m_rec=*rec;
			pWnd = GetDlgItem(IDC_STATION);
			pWnd->SetWindowText (m_pDoc->GetStationName(rec->station));
			pWnd->EnableWindow(FALSE);
	    
			const DispDistance* pDispDistance = m_pDoc->GetDispDistance(rec->station);
			if(pDispDistance != NULL)
			{
				CString str;
				int index = -1;
				int index2=-1;
				for(int ii = 0; ii < pDispDistance->number_of_side; ii++)
				{
					int side_no = pDispDistance->side[ii].side_number;
					str.Format("%s",pDispDistance->side[ii].sidename);
					int nPos=m_combo_arrive_track.AddString(str);
					m_combo_arrive_track.SetItemData(nPos, side_no);
					if(rec->arrive_side == side_no)
						index = nPos; 
					nPos=m_combo_depart_track.AddString(str);
					m_combo_depart_track.SetItemData(nPos, side_no);
					if(rec->depart_side == side_no)
						index2 = nPos;
				}
				m_combo_arrive_track.SetCurSel(index);
				m_combo_depart_track.SetCurSel(index2);
			}

			if(IsFlagPass((*rec)))
			{
				m_combo_arrive_track.EnableWindow(FALSE);
				m_combo_depart_track.EnableWindow(FALSE);
				pWnd = GetDlgItem(IDOK);
				if(pWnd)
				{
					pWnd->EnableWindow(FALSE);
				}
			}
		}
	}

	this->InitIdentify();
	SetCenterPosition();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeTrackDlg::SetCenterPosition()
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

void CChangeTrackDlg::InitIdentify()
{	
	if(!m_pDoc->IsCanEditTrain())
	{
		m_combo_arrive_track.EnableWindow(FALSE);
		m_combo_depart_track.EnableWindow(FALSE);
		
		CWnd* pWnd = GetDlgItem(IDOK);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}
}

HBRUSH CChangeTrackDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void CChangeTrackDlg::OnOK() 
{
	if(-1!=SendTrainUpdate())
		this->DestroyWindow();
	//CDialog::OnOK();
}

void CChangeTrackDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->DestroyWindow();
	//CDialog::OnCancel();
}

void CChangeTrackDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CDialog::PostNcDestroy();
}

int CChangeTrackDlg::SendTrainUpdate()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó������Ѿ�ɾ��, �޸ĵ����ɵ�ʧ��", "����");
		return 0;
	}

	CTG_TRAIN train=*pTrain;   // ����ֱ�Ӹ��ڴ�����
	TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(m_rec_index);
	if(rec == NULL)
	{
		MessageBox("��ע��: �ó�վ�����Ѿ�ɾ��, �޸ĵ����ɵ�ʧ��", "����");
		return 0;
	}
	else
	{
		BYTE uarrive_side_new=-1, udepart_side_new=-1; 
	   	
		int nCurSel = m_combo_arrive_track.GetCurSel();
		if(nCurSel==-1)
		{
			MessageBox("��ע��: ��û��ѡ�񵽴�ɵ�", "����");
			return -1;
		}
		CString strArriveSideName;
		m_combo_arrive_track.GetLBText(nCurSel, strArriveSideName);
		uarrive_side_new = m_combo_arrive_track.GetItemData(nCurSel);

		nCurSel = m_combo_depart_track.GetCurSel();
		if(nCurSel==-1)
		{
			MessageBox("��ע��: ��û��ѡ�񷢳��ɵ�", "����");
			return -1;
		}
		CString strDepartSideName;
		m_combo_depart_track.GetLBText(nCurSel, strDepartSideName);
		udepart_side_new = m_combo_depart_track.GetItemData(nCurSel);

		short flag = 0;
		if (rec->arrive_side != uarrive_side_new || rec->depart_side != udepart_side_new)
		{
			flag = 1;
			//�ͳ��Ľӳ��ɵ������Ҫ �˹�ȷ��
			if(train.m_nTrainType == TYPE::PASSENGER)	
			{
				if(uarrive_side_new != rec->arrive_side)
				{
					CString strText;
					strText  ="ȷʵҪ�޸ĸÿͳ��Ľӳ��ɵ���?";

					CDlgMessageBox dlg(strText);
					if(dlg.DoModal() != IDOK)
					{
						flag = 0;
					}
				}
			}
		}

		if(flag)
		{
			m_pDoc->sysprint(4,0,"[TG]: ����Աͨ���Ի���(�޸ĵ����ɵ�)�޸�%s�ɵ� :%d/%d-->%d/%d ", 
				m_pDoc->GetTrainShowId(train), rec->arrive_side, rec->depart_side,
				uarrive_side_new, udepart_side_new);
			
			rec->arrive_side = uarrive_side_new;
			rec->depart_side = udepart_side_new;
			SetSideFromDispatcher(*rec);

			CStringArray ary;
			m_pDoc->CheckTrainSideInfo(&train, m_rec_index, ary);
			
			if(ary.GetCount()>0)
			{
				m_pDoc->sysprint(4,0,"�޸ĵ����ɵ���������:");

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
					m_pDoc->sysprint(4,0,"����Ա������������, ����޸ĵ����ɵ�");
				}
				else
				{
					m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸ĵ����ɵ�");
					return 1;
				}			
			}

			m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, MODIFY_SIDE, 0, m_rec_index);
		}
		return 1;
	}
}

void CChangeTrackDlg::OnCbnSelArriveSide()
{
	int result=0;
	
	do
	{
		CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
		if(NULL == pTrain)
		{
			MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ĵ����ɵ�", "����");
			OnBnClickedCancel();
			return;
		}
		CTG_TRAIN train = *pTrain;
		TgTRAIN_RECORD *prec = train.GetRecordByRecIndex(m_rec_index);
		if((prec == NULL) || (prec!=NULL && prec->station != m_foucs_station))
		{
			MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ĵ����ɵ�", "����");
			OnBnClickedCancel();
			return;
		}

		int nCurSel = m_combo_arrive_track.GetCurSel();
		if(nCurSel==-1)
			break;
		CString strSideName;
		m_combo_arrive_track.GetLBText(nCurSel, strSideName);
		BYTE nTrackNo = m_combo_arrive_track.GetItemData(nCurSel);
		
		// ��·���޸Ĺɵ� �޸Ĺɵ���ʾ�û�
		if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, prec, &train))
		{
			break;
		}
		
		if(!m_pDoc->ArriveSideSetEntry(this->m_hWnd, nTrackNo, m_rec_index, &train))
		{
			break;
		}
		SetSideFromDispatcher(*prec);
		prec->arrive_side=nTrackNo;
		
		CStringArray ary;
		m_pDoc->CheckTrainSideInfo(&train, m_rec_index, ary);
		if(ary.GetCount()>0)
		{
			m_pDoc->sysprint(4,0,"�޸ĵ����ɵ���--����ɵ�--��������:");

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
		result=1;
	}while(0);

	// �ָ�ѡ��ǰ�ɵ�
	if(result==0)
	{	
		int nSel = -1;
		for(int i = 0; i < m_combo_arrive_track.GetCount(); i++)
		{
			if(m_combo_arrive_track.GetItemData(i) == m_rec.arrive_side)
			{
				nSel=i;
				break;
			}
		}
		m_combo_arrive_track.SetCurSel(nSel);
		m_pDoc->sysprint(0,0,"�ָ������޸�ǰ����ɵ�:%d", m_rec.arrive_side);
	}
}

void CChangeTrackDlg::OnCbnSelDepartSide()
{
	int result=0;
	
	do
	{
		CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
		if(NULL == pTrain)
		{
			MessageBox("��ע��: �ó��Ѿ�ɾ��, �����޸ĵ����ɵ�", "����");
			OnBnClickedCancel();
			return;
		}
		CTG_TRAIN train = *pTrain;
		TgTRAIN_RECORD *prec = train.GetRecordByRecIndex(m_rec_index);
		if((prec == NULL) || (prec!=NULL && prec->station != m_foucs_station))
		{
			MessageBox("��ע��: �ó���ѡ��վ�Ѿ�ɾ��, �����޸ĵ����ɵ�", "����");
			OnBnClickedCancel();
			return;
		}

		int nCurSel = m_combo_depart_track.GetCurSel();
		if(nCurSel==-1)
			break;
		CString strSideName;
		m_combo_depart_track.GetLBText(nCurSel, strSideName);
		BYTE nTrackNo = m_combo_depart_track.GetItemData(nCurSel);
		
		// ��·���޸Ĺɵ� �޸Ĺɵ���ʾ�û�
		if(!m_pDoc->IsEnableChangeSide(this->m_hWnd, prec, &train))
		{
			break;
		}
		
		if(!m_pDoc->DepartSideSetEntry(this->m_hWnd, nTrackNo, m_rec_index, &train))
		{
			break;
		}
		SetSideFromDispatcher(*prec);
		prec->depart_side=nTrackNo;

		CStringArray ary;
		m_pDoc->CheckTrainSideInfo(&train, m_rec_index, ary);
		if(ary.GetCount()>0)
		{
			m_pDoc->sysprint(4,0,"�޸ĵ����ɵ���--�����ɵ�--��������:");

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
				m_pDoc->sysprint(4,0,"����Ա������������, ����޸ķ����ɵ�");
			}
			else
			{
				m_pDoc->sysprint(4,0,"����Ա������������, ȡ���޸ķ����ɵ�");
				break;
			}			
		}

		m_rec.depart_side=nTrackNo;
		result=1;
	}while(0);

	// �ָ�ѡ��ǰ�ɵ�
	if(result==0)
	{	
		int nSel = -1;
		for(int i = 0; i < m_combo_depart_track.GetCount(); i++)
		{
			if(m_combo_depart_track.GetItemData(i) == m_rec.arrive_side)
			{
				nSel=i;
				break;
			}
		}
		m_combo_depart_track.SetCurSel(nSel);
		m_pDoc->sysprint(0,0,"�ָ������޸�ǰ�����ɵ�:%d", m_rec.arrive_side);
	}
}

// GuaJiCheDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "GuaJiCheDlg.h"
#include ".\guajichedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg dialog
//����
#define MAX_COLUMN 6
#define MAX_SIZE_TYPE 3
TCHAR * _gzBiaoTi[MAX_COLUMN] ={_T("��������"),_T("��������"),_T("ǣ��/����"),_T("����/��ȼ"),_T("��ʼ��վ"),_T("������վ")};

CGuaJiCheDlg::CGuaJiCheDlg(CWnd *pParent)
    : CDialog(CGuaJiCheDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGuaJiCheDlg)
	m_jiCheNo = _T("");
	//}}AFX_DATA_INIT
    m_engineCnt = 0;
	m_pDoc = NULL;
}

void CGuaJiCheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuaJiCheDlg)
	DDX_Control(pDX, IDC_LIST_CONTENT, m_List);
	DDX_Text(pDX, IDC_EDIT2, m_jiCheNo);
	DDV_MaxChars(pDX, m_jiCheNo, 6);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGuaJiCheDlg, CDialog)
	//{{AFX_MSG_MAP(CGuaJiCheDlg)
	ON_BN_CLICKED(IDC_SAVE, OnUpdate)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE2, OnAdd)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_CBN_SELCHANGE(IDC_COMBO_LOCOL_TYPE, OnCbnSelchangeComboLocolType)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg message handlers


BOOL CGuaJiCheDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pDoc->m_ptgctrl->mConfigTg.m_locoType.GetLocoAllPref(m_pDoc->m_nCurrentSectionNo, locoName, locoPref, locoId, cascoId);
	
	// ���ݱ�̨�����ڵĻ�����
	CString typeName;
	for(int count = 0;count < m_engineCnt;count ++)
	{
		bool bFind = false;
		for(int idx=0; idx<locoPref.GetCount(); idx++)
		{
			if(locoPref.GetAt(idx) == m_engineAry[count].engine_name)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			CString pref = m_engineAry[count].engine_name;
			if(!pref.IsEmpty())
			{
				int cascoid;
				CString type, name;
				
				if(m_pDoc->m_ptgctrl->mConfigTg.m_locoType.GetLocoInfo(0, pref, name, type, cascoid))
				{
					locoPref.Add(pref);
					locoId.Add(atoi(type));
					cascoId.Add(cascoid);
					locoName.Add(name);
				}
			}
		}
	}
	
	this->m_engineCnt = 0;
	for(int idx = 0; idx < this->m_pTrain.myEngine.size() && idx<MAX_ENGINEER_NUM; idx++)
	{
		this->m_engineAry[idx].engine_id     = this->m_pTrain.myEngine[idx].engine_id;
		this->m_engineAry[idx].engine_name   = this->m_pTrain.myEngine[idx].engine_name;

		this->m_engineAry[idx].engine_status = this->m_pTrain.myEngine[idx].engine_status;
		this->m_engineAry[idx].attach_sta    = this->m_pTrain.myEngine[idx].attach_sta;
		this->m_engineAry[idx].unattach_sta  = this->m_pTrain.myEngine[idx].unattach_sta;
		this->m_engineCnt++;

		m_pDoc->sysprint(4,0,"[TG]�޸�ǰ ǣ��/���һ���engine_id:%s engine_name:%s engine_status:%d attach_sta:%d unattach_sta:%d",
			m_engineAry[idx].engine_id,
			m_engineAry[idx].engine_name, 
			m_engineAry[idx].engine_status, 
			m_engineAry[idx].attach_sta,
			m_engineAry[idx].unattach_sta);
	}

	if(this->m_engineCnt == 0)
	{
		m_pDoc->sysprint(4,0,"[TG]�޸�ǰ ǣ��/���һ���û��");
	}

	this->InitialListCtrl();
	this->initLeftColumn();

	CString tmp;
	tmp.Format("%d", -1);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_TRAIN_ID);
	pEdit->SetWindowText(tmp);
	
	if(!m_pDoc->IsCanEditTrain())
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_SAVE2);
		pWnd->EnableWindow(FALSE);
	}
	
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsElecEngineShowDianzi() || m_pDoc->m_ptgctrl->mConfigTg.IsDisenableSheetTab())
	{
		CButton *pWnd = (CButton*)GetDlgItem(IDC_CHECK1);
		if(pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
			if(m_pTrain.myTrainInfo.b_elec_rank)
			    pWnd->SetCheck(TRUE);
			else
				pWnd->SetCheck(FALSE);
		}
	}

	SetCenterPosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGuaJiCheDlg::SetCenterPosition()
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

BOOL CGuaJiCheDlg::InitialListCtrl()
{
	TgTRAIN_RECORD *train_rec;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	CRect rect;
	int count = 0;
	CString text;
	text.Empty();

	//�ı�listctrl�ķ��
	m_List.SetExtendedStyle(LVS_REPORT|LVS_EX_FULLROWSELECT| LVS_EX_ONECLICKACTIVATE);
	m_List.GetClientRect(&rect);


	//��ӱ���ͷ
	for(count=0;count<MAX_COLUMN;count++)
	{
		lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
		lvc.fmt = LVCFMT_CENTER;
		lvc.cx = rect.Width() / MAX_COLUMN;
		lvc.pszText = _gzBiaoTi[count];
		m_List.InsertColumn(count, &lvc);
	}

	//�������
	for(count = 0;count < m_engineCnt;count ++)
	{
		int rec_index;
		BYTE engine_type;
		
		CString typeName = m_engineAry[count].engine_name;
		
		// ��ĸ��������
		lvi.mask  =  LVIF_TEXT;
		lvi.iItem = count;
		lvi.iSubItem = 0;
		text.Format("%s",typeName);
		lvi.pszText = const_cast<char *>((LPCSTR)text);
		m_List.InsertItem(&lvi);

		
		// ������
		CString engineId = m_engineAry[count].engine_id;
		m_List.SetItemText(count, 1, engineId.Right(engineId.GetLength()-3));
	
		// ģʽ
		engine_type = m_engineAry[count].engine_status;
		if(IsAttachEngineExt(engine_type))
		{
			text.Format("����");
		}
		else
		{
			text.Format("ǣ��");			
		}
		m_List.SetItemText(count, 2, text);

		if(IsDieselEngineExt(engine_type))
		{
			text.Format("��ȼ");
		}
		else
		{
			text.Format("����");			
		}
		m_List.SetItemText(count,3,text);

		rec_index  = (int)m_engineAry[count].attach_sta;
		DWORD nData = 1000*rec_index;
		
		train_rec = m_pTrain.GetRecordByPosition(rec_index);
		if(train_rec != NULL)
		{	
			text.Format("%s",m_pDoc->GetStationName(train_rec->station));
		}
		else
		{
			text.Format("��");
		}
		m_List.SetItemText(count,4,text);


		rec_index = (int)m_engineAry[count].unattach_sta;
		nData += rec_index;
		
		train_rec = m_pTrain.GetRecordByPosition(rec_index);
		if(train_rec != NULL)
		{
			text.Format("%s",m_pDoc->GetStationName(train_rec->station));
		}
		else
		{
			text.Format("��");
		}
		m_List.SetItemText(count,5,text);
		m_List.SetItemData(count,nData);
	}
	return TRUE;
}

BOOL CGuaJiCheDlg::initLeftColumn()
{
	TgTRAIN_RECORD *train_rec = NULL;

	int count = 0;
	CString text;
	text.Empty();
	//������ʼ����ߵı༭����
	
	//����
	text.Format("%s ����/ǣ��",  m_pDoc->GetTrainShowId(m_pTrain));
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TEXT);
	pWnd->SetWindowText(text);
	
	CComboBox *pCombo;
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	pCombo->InsertString(0,"����");
	pCombo->SetItemData(0,0);
	pCombo->InsertString(1,"ǣ��");
	pCombo->SetItemData(1,1);
	pCombo->SetCurSel(1); //Ĭ��ǣ��
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	for(int i=0;i<locoPref.GetCount(); i++)
	{
		pCombo->InsertString(i, locoPref.GetAt(i));
	}
	pCombo->SetCurSel(0);	

	// ��������
	CString strName = GetLocoNameByPref(locoPref.GetAt(0));
	pWnd = GetDlgItem(IDC_EDIT5);
	if(pWnd)
		pWnd->SetWindowText(strName);

	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	int usRecordCount = m_pTrain.GetRecordNum();
	for(count = 0;count < usRecordCount;count ++)
	{
		train_rec = m_pTrain.GetRecordByPosition(count);
		if(train_rec == NULL)
		{
			continue;
		}
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
		pCombo->InsertString(count,text);
		//pCombo->SetItemData(count,count);
	}
	pCombo->SetCurSel(0);

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	for(count = 0;count < usRecordCount;count ++)
	{
		train_rec = m_pTrain.GetRecordByPosition(count);
		if(train_rec == NULL)
		{
			continue;
		}
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
		pCombo->InsertString(count,text);
	}
	//pCombo->InsertString(usRecordCount,"����");
	
	pCombo->SetCurSel(usRecordCount-1);
	return TRUE;
}

void CGuaJiCheDlg::fillLeftContent(int curSel)
{
	CEdit *pEdit;

	if(curSel >= m_List.GetItemCount())
		return;

	// ��������
	CString pref = m_List.GetItemText(curSel,0);
		
	CString name = GetLocoNameByPref(pref);
	CWnd* pWnd = GetDlgItem(IDC_EDIT5);
	if(pWnd)
		pWnd->SetWindowText(name);

	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	for(int i=0;i<locoPref.GetCount(); i++)
	{
		if(locoPref.GetAt(i) == pref)
		{
			pCombo->SetCurSel(i);
			break;
		}
	}
	
	// ��������
	CString text = m_List.GetItemText(curSel,1);
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->SetWindowText(text);

	text = m_List.GetItemText(curSel,2);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	if(text == "����")
	{
		pCombo->SetCurSel(0);
	}
	else
	{
		pCombo->SetCurSel(1);
	}

	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	DWORD nData = m_List.GetItemData(curSel);
	UINT index = nData/1000;
	if(index >= 0 && index < m_pTrain.myRecord.size())
	{
		pCombo->SetCurSel(index);
	}
	else
	{
		pCombo->SetWindowText("��");
	}
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	index = nData%1000;
	if(index >= 0 && index < m_pTrain.myRecord.size())
	{
		pCombo->SetCurSel(index);
	}
	else
	{
		pCombo->SetWindowText("��");
	}
}

void CGuaJiCheDlg::OnAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString text, pref;
	int pos = m_List.GetItemCount();
	if(pos >= MAX_ENGINEER_NUM)
	{
		CString tempStr;
		tempStr.Format("���ֻ�ܹ�%d������",MAX_ENGINEER_NUM);
		AfxMessageBox(tempStr);	
		return;
	}

    //������
	TgTRAIN_RECORD *train_rec;
		
	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	pCombo->GetWindowText(pref);
	
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->GetWindowText(text);
	if(text.IsEmpty())
	{
		AfxMessageBox("�����Ų���Ϊ��");
		return;
	}
		
	m_List.InsertItem(pos,pref);

	while(text.GetLength()<5)
	{
		text = "0"+text;
	}
	m_List.SetItemText(pos,1,text);

	//ǣ����ʽ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	if(pCombo == NULL) return;

	int index = pCombo->GetCurSel();
	if(index == 0)
		text.Format("����");
	else
		text.Format("ǣ��");	
	m_List.SetItemText(pos,2,text);
	
	int cascoId = GetCascoIdByPref(pref);
	if(cascoId>=128)
		text.Format("����");
	else
		text.Format("��ȼ");
	m_List.SetItemText(pos,3,text);

	

	// �ҳ���վ
	DWORD nData = 0;
	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	if(pCombo == NULL) return;

	index = pCombo->GetCurSel();
	if(index == -1)
	{
		AfxMessageBox("��ѡ����ʼ��վ");
		return;
	}	
    train_rec = m_pTrain.GetRecordByPosition(index);
	if(train_rec != NULL)
	{
		nData = 1000*index;
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
	}
	else
	{
		text.Format("��");
	}
	m_List.SetItemText(pos,4,text);
		
	//�⳵��վ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	if(pCombo == NULL) return;

	index = pCombo->GetCurSel();
	if(index == -1)
	{
		AfxMessageBox("��ѡ�������վ");
		return;
	}
	train_rec = m_pTrain.GetRecordByPosition(index);
	if(train_rec != NULL)
	{
		nData += index;
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
	}
	else
	{
		nData += pCombo->GetCount()-1;
		text.Format("����");
	}
	m_List.SetItemText(pos,5,text);

	m_List.SetItemData(pos, nData);
}

void CGuaJiCheDlg::OnUpdate() 
{
	CString text, pref;
	
	CString tmp;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_TRAIN_ID);
	pEdit->GetWindowText(tmp);
	int pos = atoi(tmp);
    if(pos == -1)
	{
		AfxMessageBox("��ѡ��Ҫ�޸ĵĻ���");
		return;
	}

    //������
	TgTRAIN_RECORD *train_rec;
		
	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	pCombo->GetWindowText(pref);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->GetWindowText(text);
	if(text.IsEmpty())
	{
		AfxMessageBox("�����Ų���Ϊ��");
		return;
	}

	m_List.SetItemText(pos,0,pref);

	while(text.GetLength()<5)
	{
		text = "0"+text;
	}
	m_List.SetItemText(pos,1,text);

	//ǣ����ʽ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	if(pCombo == NULL) return;

	int index = pCombo->GetCurSel();
	if(index == 0)
		text.Format("����");
	else
		text.Format("ǣ��");	
	m_List.SetItemText(pos,2,text);
	
	int cascoId = GetCascoIdByPref(pref);
	if(cascoId>=128)
		text.Format("����");
	else
		text.Format("��ȼ");
	m_List.SetItemText(pos,3,text);

	// �ҳ���վ
	DWORD nData = 0;
	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	if(pCombo == NULL) return;

	index = pCombo->GetCurSel();
	if(index == -1)
	{
		AfxMessageBox("��ѡ����ʼ��վ");
		return;
	}	
    train_rec = m_pTrain.GetRecordByPosition(index);
	if(train_rec != NULL)
	{
		nData = 1000*index;
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
	}
	else
	{
		text.Format("��");
	}
	m_List.SetItemText(pos,4,text);
		
	// �ҳ���վ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	if(pCombo == NULL) return;

	index = pCombo->GetCurSel();
	if(index == -1)
	{
		AfxMessageBox("��ѡ�������վ");
		return;
	}
	train_rec = m_pTrain.GetRecordByPosition(index);
	if(train_rec != NULL)
	{
		nData += index;
		text.Format("%s",m_pDoc->GetStationName(train_rec->station));
	}
	else
	{
		nData += 0;
		text.Format("��");
	}
	m_List.SetItemText(pos,5,text);

	m_List.SetItemData(pos, nData);
}

void CGuaJiCheDlg::OnDelete() 
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_List.GetNextSelectedItem(pos);
	m_List.DeleteItem(index);
}

BOOL CGuaJiCheDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pNMHDR = (NMHDR *)lParam;
	if(pNMHDR->idFrom == IDC_LIST_CONTENT && pNMHDR->code == NM_CLICK)
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int index = m_List.GetNextSelectedItem(pos);
			
			CString tmp;
			tmp.Format("%d", index);
			CEdit* pEdit = (CEdit*)GetDlgItem(IDC_TRAIN_ID);
			pEdit->SetWindowText(tmp);
			
			fillLeftContent(index);
			
			return TRUE;
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CGuaJiCheDlg::OnEnChangeEdit2()
{
	CString type;
	CWnd* pWnd = GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	if(pWnd)
	{
		pWnd->GetWindowText(type);
	}
	pWnd = GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		CString str;
		pWnd->GetWindowText(str);
		if(type != "GDC")
		{
			if(str.GetLength() == 6)
			{
				pWnd->SetWindowText(str.Left(5));
			}
		}
	}
}

void CGuaJiCheDlg::OnCbnSelchangeComboLocolType()
{
	CString pref;
	CWnd* pWnd = GetDlgItem(IDC_COMBO_LOCOL_TYPE);
	if(pWnd)
	{
		pWnd->GetWindowText(pref);
	}

	CString name = GetLocoNameByPref(pref);
	pWnd = GetDlgItem(IDC_EDIT5);
	if(pWnd)
		pWnd->SetWindowText(name);
	
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);
	pEdit->SetWindowText("");
	
}

int CGuaJiCheDlg::GetCascoIdByLocoId(int id)
{
	for(int i=0; i<locoId.GetCount();i++)
	{
		if(id == locoId.GetAt(i))
		{
			return cascoId.GetAt(i);
		}
	}

	return 0;
}

int CGuaJiCheDlg::GetCascoIdByPref(CString pref)
{
	for(int i=0; i<locoPref.GetCount();i++)
	{
		if(pref == locoPref.GetAt(i))
		{
			return cascoId.GetAt(i);
		}
	}

	return 0;
}

int CGuaJiCheDlg::GetLocoIdByPref(CString pref)
{
	for(int i=0; i<locoPref.GetCount();i++)
	{
		if(pref == locoPref.GetAt(i))
		{
			return locoId.GetAt(i);
		}
	}

	return 0;
}

CString CGuaJiCheDlg::GetLocoNameByPref(CString pref)
{
	for(int i=0; i<locoPref.GetCount();i++)
	{
		if(pref == locoPref.GetAt(i))
		{
			return locoName.GetAt(i);
		}
	}

	return "";
}

void CGuaJiCheDlg::SendTrainUpdate()
{
	CString itemText;
	m_engineCnt = m_List.GetItemCount();
	if(m_engineCnt > MAX_ENGINEER_NUM)
		m_engineCnt = MAX_ENGINEER_NUM;

	for(int pos = 0 ;pos < m_engineCnt;pos++)
	{
		// ��������
		m_engineAry[pos].engine_name = m_List.GetItemText(pos,0);

		// ��������
		CString id="";
		id.Format("%0.3d", this->GetLocoIdByPref(m_engineAry[pos].engine_name));

		m_engineAry[pos].engine_id = id+m_List.GetItemText(pos,1);

		// ǣ����ʽ
		m_engineAry[pos].engine_status = 0;
		itemText = m_List.GetItemText(pos,2);
		if(itemText == "����")
			SetAttachEngineExt(m_engineAry[pos].engine_status);
		else
			SetTractionEngineExt(m_engineAry[pos].engine_status);
		
		// ��������
		CString strLocoId = m_engineAry[pos].engine_id.Left(3);
		int type = GetCascoIdByLocoId(atoi(strLocoId));
		if(type >= 128)
			SetElecEngineExt(m_engineAry[pos].engine_status);
		else
			SetDieselEngineExt(m_engineAry[pos].engine_status);
			

		DWORD nData = m_List.GetItemData(pos);
		// �ҳ�վ
		m_engineAry[pos].attach_sta = nData/1000;
		// ˦��վ
		m_engineAry[pos].unattach_sta = nData%1000;

	}

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó������Ѿ�ɾ��, �޸�ǣ�����һ���ʧ��", "����");
		return;
	}
	
	m_pTrain = *pTrain;
	m_pTrain.myEngine.clear();

	for(int idx = 0; idx < m_engineCnt; idx++)
	{
		TgTRAIN_ENGINE_INFO engine;

		engine.engine_id = this->m_engineAry[idx].engine_id;
		engine.engine_name = this->m_engineAry[idx].engine_name;
		engine.engine_status = this->m_engineAry[idx].engine_status;
		engine.attach_sta = this->m_engineAry[idx].attach_sta;
		engine.unattach_sta = this->m_engineAry[idx].unattach_sta;

		m_pDoc->sysprint(4,0,"[TG]�޸ĺ� ǣ��/���һ���engine_id:%s engine_name:%s engine_status:%d attach_sta:%d unattach_sta:%d",
			this->m_engineAry[idx].engine_id,
			this->m_engineAry[idx].engine_name,
			this->m_engineAry[idx].engine_status, 
			this->m_engineAry[idx].attach_sta,
			this->m_engineAry[idx].unattach_sta);
		
		m_pTrain.myEngine.push_back(engine);
	}

	// �ϲ��������⴦��
	if(m_pDoc->m_ptgctrl->mConfigTg.m_context.IsElecEngineShowDianzi() || m_pDoc->m_ptgctrl->mConfigTg.IsDisenableSheetTab())
	{
		int mRet = 0;
		CButton *pWnd = (CButton*)GetDlgItem(IDC_CHECK1);
		if(pWnd)
			mRet = pWnd->GetCheck();

		if(1 == mRet)
		{
			m_pDoc->sysprint(4,0,"[TG]����Ա�޸��г� %sΪ ��������", m_pDoc->GetTrainShowId(m_pTrain));

			for(int i=0; i<m_pTrain.myRecord.size(); i++)
			{
				SetElectricEngineTrain(m_pTrain.myRecord[i]);
			}
		}
		else
		{
			m_pDoc->sysprint(4,0,"[TG]����Ա�޸��г� %sΪ ��ȼ����", m_pDoc->GetTrainShowId(m_pTrain));

			for(int i=0; i<m_pTrain.myRecord.size(); i++)
			{
				ResetElectricEngineTrain(m_pTrain.myRecord[i]);
			}
		}
	}

	m_pDoc->SendUpdateTrain(m_pTrain, SCHD_UPDATE, FUGUA_ENGINE, 0, 0);
}

void CGuaJiCheDlg::OnBnClickedCancel()
{
	this->DestroyWindow();
}

void CGuaJiCheDlg::OnBnClickedOk()
{
	SendTrainUpdate();
	this->DestroyWindow();
}

void CGuaJiCheDlg::PostNcDestroy()
{
	delete this;
	CDialog::PostNcDestroy();
}

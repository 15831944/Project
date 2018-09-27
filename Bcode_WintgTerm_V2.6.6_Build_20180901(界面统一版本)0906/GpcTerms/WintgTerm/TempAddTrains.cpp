// TempAddTrains.cpp : implementation file
//

#include "stdafx.h"
#include "TempAddTrains.h"
#include "resource.h"
#include "oscalls.h"
#include "sysshare.H"
#include "function.h"
#include "tgdocument.h"
#include "tg_structdef.h"
#include ".\tempaddtrains.h"
#include "TgEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTempAddTrains dialog


CTempAddTrains::CTempAddTrains(CTgDocument *pDoc, int conid, CTgEditView* pParent)
	: CDialog(CTempAddTrains::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTempAddTrains)
	//}}AFX_DATA_INIT
	m_pDoc=pDoc;
	m_pEditView=pParent;
	m_nSectionID = conid;
	m_strTrainId="";
	m_strEngineId="";
}


void CTempAddTrains::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTempAddTrains)
	DDX_Text(pDX, IDC_PLAN_ENGINEID, m_strEngineId);
    DDV_MaxChars(pDX, m_strEngineId, 5);
	DDX_Text(pDX, IDC_PLAN_TRAINID, m_strTrainId);
	DDV_MaxChars(pDX, m_strTrainId, 9);
	DDX_Control(pDX, IDC_LIST1, m_engine_list);
	DDX_Control(pDX, IDC_COMBO1, m_comboxEngineType);
	DDX_Control(pDX, IDC_COMBO2, m_comboxStartStation);
	DDX_Control(pDX, IDC_COMBO3, m_comboxQianYin);
	DDX_Control(pDX, IDC_COMBO4, m_comboxEndStation);
	DDX_Control(pDX, IDC_RUN_TYPE_LZ, m_strAbsRunType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTempAddTrains, CDialog)
	//{{AFX_MSG_MAP(CTempAddTrains)
	ON_EN_CHANGE(IDC_PLAN_TRAINID, OnChangePlanTrainid)
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTE, OnSelchangeComboRoute)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_RADIO_ACCEPT, OnBnClickedRadioAccept)
	ON_BN_CLICKED(IDC_RADIO_START, OnBnClickedRadioStart)
	ON_BN_CLICKED(IDC_RADIO_GIVEOUT, OnBnClickedRadioGiveout)
	ON_BN_CLICKED(IDC_RADIO_TERMINAL, OnBnClickedRadioTerminal)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CTempAddTrains::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWndFocus = GetFocus();

	if ((pMsg->message == WM_KEYDOWN) &&
		((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.
		if ((pMsg->wParam == VK_RETURN)	&&	((pWndFocus = GetFocus()) != NULL) 
			&&	IsChild(pWndFocus))
		{
			CWnd *pWnd;
			if(pWndFocus == GetDlgItem(IDC_PLAN_TRAINID))
			{
				pWnd = GetDlgItem(IDC_PLAN_ENGINEID); 
				GotoDlgCtrl(pWnd); 
			} 
			else if(pWndFocus == GetDlgItem(IDC_PLAN_ENGINEID))
			{
  				pWnd = GetDlgItem(IDC_BUTTON_ADD); 
				GotoDlgCtrl(pWnd); 
			}
			else if(pWndFocus == GetDlgItem(IDC_BUTTON_ADD)){
				//OnButtonAdd();
			}			
		}
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTempAddTrains::OnSelchangeComboRoute() 
{
	CComboBox* pRoute = (CComboBox*)GetDlgItem(IDC_COMBO_ROUTE);
	if(pRoute == NULL)
		return;
	CComboBox* pBeginSta = (CComboBox*)GetDlgItem(IDC_COMBO_START_STATION);
	if(pBeginSta == NULL)
		return;
	CComboBox* pEndSta = (CComboBox*)GetDlgItem(IDC_COMBO_END_STATION);
	if(pEndSta == NULL)
		return;

	m_comboxStartStation.ResetContent();
	m_comboxEndStation.ResetContent();
	pBeginSta->ResetContent();
	pEndSta->ResetContent();
	int nSel = pRoute->GetCurSel();
	if(nSel == -1)
		return;

	int passwayid = pRoute->GetItemData(nSel);
	int stanum=100;
	TG_PassWaySta sta[100];
	if(GetTGBasicParam()->GetPasswaySta(m_nSectionID, passwayid,  sta, &stanum))
	{
		for(int i=0; i<stanum; i++)
		{
			CString staName = gpcdef.GetStationName(sta[i].Station);
			int itemindex=pBeginSta->AddString(staName);
			pBeginSta->SetItemData(itemindex,sta[i].Station);

			m_comboxStartStation.AddString(staName);
			m_comboxEndStation.AddString(staName);

			itemindex=pEndSta->AddString(staName);
			pEndSta->SetItemData(itemindex, sta[i].Station);
		}

		if(stanum>0)
		{
			pBeginSta->SetCurSel(0);
			pEndSta->SetCurSel(stanum-1);
			m_comboxStartStation.SetCurSel(0);
			m_comboxEndStation.SetCurSel(stanum-1);
		}
	}
}

BOOL CTempAddTrains::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_nSelList=-1;
	// �����б�
	DWORD dwStyle = m_engine_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_engine_list.SetExtendedStyle(dwStyle);

	m_engine_list.InsertColumn(0, "����",LVCFMT_CENTER,40);
	m_engine_list.InsertColumn(1, "������",LVCFMT_CENTER,60);
	m_engine_list.InsertColumn(2, "ǣ��",LVCFMT_CENTER,50);
	m_engine_list.InsertColumn(3, "��ʼվ",LVCFMT_CENTER,55);
	m_engine_list.InsertColumn(4, "����վ",LVCFMT_CENTER,55);

	// ������������
	CStringArray arypref;
	int nResult = GetTGBasicParam()->GetLocoAllPref(m_nSectionID, arypref);
	if(nResult <= 0)
	{
		AfxMessageBox("��ע��, dataĿ¼��ȱ�ٻ������������ļ�");
	}
	else
	{
		for(int i=0; i<arypref.GetCount(); i++)
		{
			m_comboxEngineType.AddString(arypref.GetAt(i));
		}
		m_comboxEngineType.SetCurSel(0);
	}

	// ǣ����ʽ
	m_comboxQianYin.AddString("ǣ��");
	m_comboxQianYin.AddString("����");
	m_comboxQianYin.SetCurSel(0);

	m_start_mode=SCHD_ACCEPT;
	m_end_mode=SCHD_GIVEOUT;
	SetCheck();
	
	LoadPasswayInfo();
	
	CStringArray name_list;
	CWordArray run_type_list;
	int count=GetTGBasicParam()->GetAllAbsTimeTypeName(name_list, run_type_list);
	
	int nItem=m_strAbsRunType.AddString("Ĭ������ʱ��");
	m_strAbsRunType.SetItemData(nItem, 255);
	for(int i=0; i<count; i++)
	{
		nItem=m_strAbsRunType.AddString(name_list.GetAt(i));
		m_strAbsRunType.SetItemData(nItem, run_type_list.GetAt(i));
	}
	m_strAbsRunType.SetCurSel(0);
	if(count<=0)
	{
		CWnd* pWnd=GetDlgItem(IDC_INSERT_STATIC233);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);
		m_strAbsRunType.ShowWindow(SW_HIDE);
		m_strAbsRunType.EnableWindow(FALSE);
	}

	InitDlgPosition(this);
	return TRUE;  
}

void CTempAddTrains::SetCheck()
 {
	 CButton* pButtonA = (CButton*)GetDlgItem(IDC_RADIO_ACCEPT);
	CButton* pButtonS = (CButton*)GetDlgItem(IDC_RADIO_START);
	if(pButtonA != NULL && pButtonS != NULL)
	{
		if(m_start_mode==SCHD_ACCEPT)
		{
			pButtonA->SetCheck(1);
			pButtonS->SetCheck(0);
		}
		else
		{
			pButtonA->SetCheck(0);
			pButtonS->SetCheck(1);
		}
	}

	CButton* pButtonT = (CButton*)GetDlgItem(IDC_RADIO_TERMINAL);
	CButton* pButtonG = (CButton*)GetDlgItem(IDC_RADIO_GIVEOUT);
	if(pButtonT != NULL && pButtonG != NULL)
	{
		if(m_end_mode==SCHD_TERMINAL)
		{
			pButtonT->SetCheck(1);
			pButtonG->SetCheck(0);
		}
		else
		{
			pButtonT->SetCheck(0);
			pButtonG->SetCheck(1);
		}
	}
 }

void CTempAddTrains::LoadPasswayInfo()
{
	CComboBox* pRoute = (CComboBox*)GetDlgItem(IDC_COMBO_ROUTE);
	if(pRoute == NULL)
		return;

	passway_count=200; 
	if(GetTGBasicParam()->GetPasswayInfo(m_nSectionID,passway,&passway_count)) // ������ؿɶ��������÷���������
	{
		for(int i=0;i<passway_count;i++)
		{
			int itemindex=pRoute->AddString(passway[i].Pass_way_name);
			pRoute->SetItemData(itemindex,passway[i].Pass_way_id);
		}
	}

	if(passway_count>0)
	{
		pRoute->SetCurSel(0);
		OnSelchangeComboRoute();
	}
}

void CTempAddTrains::OnChangePlanTrainid() 
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_PLAN_TRAINID);
	if(pEdit == NULL)
		return;
	CString trainid;
	pEdit->GetWindowText(trainid);

	trainid = ChangeAlpha(trainid);
	int nTrainDir = m_pDoc->GetTrainIDDirection(trainid);

	CComboBox* pRoute = (CComboBox*)GetDlgItem(IDC_COMBO_ROUTE);
	if(pRoute == NULL)
		return;
	pRoute->ResetContent();

	int itemcount = 0;
	for(int i=0;i<passway_count;i++)
	{
		CString text = passway[i].Pass_way_name;
		if(UP_DIRECT == nTrainDir && text.Find("����")>=0)
		{
			pRoute->AddString(text);
			pRoute->SetItemData(itemcount, passway[i].Pass_way_id);
			itemcount++;
		}
		else if(DOWN_DIRECT == nTrainDir && text.Find("����")>=0)
		{
			pRoute->AddString(text);
			pRoute->SetItemData(itemcount, passway[i].Pass_way_id);
			itemcount++;
		}
	}
	pRoute->SetCurSel(0);
	OnSelchangeComboRoute();
}

void CTempAddTrains::OnBnClickedRadioAccept()
{
	m_start_mode=SCHD_ACCEPT;
	SetCheck();
}

void CTempAddTrains::OnBnClickedRadioStart()
{
	m_start_mode=SCHD_START;
	SetCheck();
}

void CTempAddTrains::OnBnClickedRadioGiveout()
{
	m_end_mode=SCHD_GIVEOUT;
	SetCheck();
}

void CTempAddTrains::OnBnClickedRadioTerminal()
{
	m_end_mode=SCHD_TERMINAL;
	SetCheck();
}

void CTempAddTrains::OnBnClickedButtonClear()
{
	m_start_mode=SCHD_ACCEPT;
	m_end_mode=SCHD_GIVEOUT;
	SetCheck();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_PLAN_TRAINID);
	if(pEdit != NULL)
		pEdit->SetWindowText("");
	pEdit = (CEdit*)GetDlgItem(IDC_PLAN_ENGINEID);
	if(pEdit != NULL)
		pEdit->SetWindowText("");
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TMP_TIME);
	if(pEdit != NULL)
		pEdit->SetWindowText("");
	m_strAbsRunType.SetCurSel(0);
	return;
}

void CTempAddTrains::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CDialog::PostNcDestroy();
}

void CTempAddTrains::OnBnClickedButtonAdd()
{
	do
	{
		TmpTrainInfo info;
		
		info.start_mode=m_start_mode;
		info.end_mode=m_end_mode; 
		
		
		//���κ�
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_PLAN_TRAINID);
		if(pEdit == NULL)
			break;
		CString trainid;
		pEdit->GetWindowText(trainid);
		if (trainid.IsEmpty() &&trainid.GetLength()>9)
		{
			AfxMessageBox("�����복�κţ����߳��δ���9λ");
			break;
		}
		strncpy(info.trainid, trainid, sizeof(info.trainid)-1);
		
		info.bSecret=false;
		CButton* pBtn=(CButton *)GetDlgItem(IDC_SECRET_TRAIN_LZ_PS);
		if(pBtn && pBtn->GetCheck())
		{
			CString str;
			str.Format("��ע��,�г� %s ������Ϊ�����г��� \n\nȷ�����ñ����г�ѡ��\"��\"\n����ѡ��\"��\"", trainid);
			if(IDYES==MessageBox(str, "��ʾ", MB_YESNO|MB_ICONWARNING))
				info.bSecret=true;
		}

		//��ʼ��վ
		CComboBox* pBeginSta = (CComboBox*)GetDlgItem(IDC_COMBO_START_STATION);
		if(pBeginSta == NULL)
			break;
		info.nBeginStaIndex = pBeginSta->GetCurSel();
		
		//��ֹ��վ
		CComboBox* pEndSta = (CComboBox*)GetDlgItem(IDC_COMBO_END_STATION);
		if(pEndSta == NULL)
			break;
		info.nEndStaIndex = pEndSta->GetCurSel();
		
		//ʱ��
		CString strTime;
		pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TMP_TIME);
		if(pEdit == NULL)
			break;
		pEdit->GetWindowText(strTime);
		if(strTime.IsEmpty())
		{
			AfxMessageBox("������ʱ��");
			break;
		}
		strTime=ChangeAlpha(strTime);
		if (strTime.IsEmpty() && strTime.GetLength()>5)
		{
			AfxMessageBox("����ʱ�䲻��ȷ");
			break;
		}
		for(int i=0; i<strTime.GetLength(); i++)
		{
			char ch = strTime.GetAt(i);
			if( '.' !=ch && ('0' > ch || ch > '9'))
			{
				AfxMessageBox("����ʱ�䲻��ȷ, ���ڷǷ��ַ�");
				break;
			}
		}
		if(i<strTime.GetLength())
			break;

		CString strHour, strMinute;
		int pos = strTime.Find('.');
		if(pos == -1)
		{
			strMinute="0";
			strHour = strTime;
			if (strHour.GetLength()>2)
			{
				AfxMessageBox("����ʱ�䲻��ȷ,Сʱλ������");
				break;
			}
		}
		else
		{
			strHour    = strTime.Left(pos);
			strMinute = strTime.Right(strTime.GetLength()-pos-1);
			if (strHour.GetLength()>2)
			{
				AfxMessageBox("����ʱ�䲻��ȷ,Сʱλ������");
				break;
			}
			if (strMinute.GetLength()>2)
			{
				AfxMessageBox("����ʱ�䲻��ȷ,����λ������");
				break;
			}
		}

		int hour = atoi(strHour);
		int minite = atoi(strMinute);
		if(hour<0 && hour >23)
		{
			AfxMessageBox("����ʱ�䲻��ȷ,Сʱ��ֵ�Ƿ�");
		}
		if(minite<0 && minite >59)
		{
			AfxMessageBox("����ʱ�䲻��ȷ,������ֵ�Ƿ�");
		}

		CTime tm = CTime::GetCurrentTime();
		if(tm.GetHour() > hour)
		{
			tm += CTimeSpan(1,0,0,0); // ��һ��
		}
		
		CTime bm(tm.GetYear(), tm.GetMonth(), tm.GetDay(), hour, minite, 0);
		info.beginTime = bm.GetTime();

		//��·
		CComboBox* pRoute = (CComboBox*)GetDlgItem(IDC_COMBO_ROUTE);
		if(pRoute == NULL)
			break;
		int nSel = pRoute->GetCurSel();
		if(nSel == -1)
			break;

		info.nSelPasswayId= pRoute->GetItemData(nSel);
		
		//������
		info.nEngineCount=0;
		for(int colno =0;info.nEngineCount<5&&colno<m_engine_list.GetItemCount(); colno++)
		{
			CString prefName = m_engine_list.GetItemText(colno, 0);
			CString engineNo = m_engine_list.GetItemText(colno, 1);
			CString id = GetTGBasicParam()->GetLocoIdByPref(m_nSectionID, prefName);
			
			engineNo = id+engineNo;
			strncpy(info.engineId[info.nEngineCount], engineNo, 8);
			info.status[info.nEngineCount]=m_engine_list.GetItemData(colno);
			info.nEngineCount++;
		}

		nSel=m_strAbsRunType.GetCurSel();
		if(nSel==-1)
		{
			AfxMessageBox("��ѡ����������ʱ������");
		}
		int nRunType=m_strAbsRunType.GetItemData(nSel);
		CString strSelText;
		m_strAbsRunType.GetLBText(nSel, strSelText);
		if(nRunType!=255)
		{
			CString str;
			str.Format("��ע��,�г� %s ������ %s ��������ʱ�����ͣ� \n\nȷ��ѡ��\"��\"\n����ѡ��\"��\"", info.trainid, strSelText);
			if(IDYES!=MessageBox(str, "��ʾ", MB_YESNO|MB_ICONWARNING))
				break;
		}
		info.nRunType=nRunType;

		m_pEditView->OnAddTmpTrainPassway(&info);
		InitEngineInfo();
		m_strAbsRunType.SetCurSel(0); // �г���ͼ֮��,�ָ�����ʱ������Ϊ:Ĭ������ʱ��
	} while (0);
}

void CTempAddTrains::InitEngineInfo()
{
	m_engine_list.DeleteAllItems();
	CWnd* pWnd = GetDlgItem(IDC_PLAN_ENGINEID);
	if(NULL != pWnd)
	{
		pWnd->SetWindowText("");
	}
}

void CTempAddTrains::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	for(int colno =0;colno<m_engine_list.GetItemCount(); colno++)
	{
		if(m_engine_list.GetItemState(colno,LVIS_SELECTED) == LVIS_SELECTED)
		{ 
			m_nSelList = colno;

			CString prefName = m_engine_list.GetItemText(m_nSelList, 0);
			m_strEngineId = m_engine_list.GetItemText(m_nSelList, 1);

			for(int i=0; i<m_comboxEngineType.GetCount(); i++)
			{
				CString ss;
				m_comboxEngineType.GetLBText(i, ss);
				if(ss == prefName)
				{
					m_comboxEngineType.SetCurSel(i);
					break;
				}
			}

			DWORD nItemData = m_engine_list.GetItemData(m_nSelList);
			DWORD nSel = nItemData&0xFF;
			m_comboxStartStation.SetCurSel(nSel);

			nSel = (nItemData&0xFF00)>>8;
			m_comboxEndStation.SetCurSel(nSel);

			nSel = (nItemData&0xFF0000)>>16;
			m_comboxQianYin.SetCurSel(nSel);

			UpdateData(FALSE);
			break;
		}	
	}

	*pResult = 0;
}
void CTempAddTrains::OnBnClickedButton1()
{
	UpdateData(TRUE);
	
	CString strBStationName, strEStationName;
	DWORD nItemData=0;
	int nSel = m_comboxStartStation.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ������ʼվ");
		return;
	}
	nItemData = nSel;
	m_comboxStartStation.GetLBText(nSel, strBStationName);
	
	nSel = m_comboxEndStation.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ��������վ");
		return;
	}
	m_comboxStartStation.GetLBText(nSel, strEStationName);
	nItemData += (nSel<<8);

	nSel = m_comboxQianYin.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ����ǣ������վ");
		return;
	}
	nItemData += (nSel<<16);
    
	nSel = m_comboxEngineType.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ��������");
		return;
	}
	//added by zhaoliang for prohibit add str engine id
	if (m_strEngineId.IsEmpty())
	{
		AfxMessageBox("������ ������");
		return;
	}
	bool ifnumber = true ;
	for (int i = 0 ; i <= m_strEngineId.GetLength()-1 ; i++)
	{
		if (m_strEngineId.GetAt(i)<'0' || m_strEngineId.GetAt(i)>'9')
		{
			ifnumber = false ;
			break;
		}

	}
	if (ifnumber == false)
	{
		AfxMessageBox("������������ ����");
		return;
	}
    //ע�͵��� 0
	/*while (m_strEngineId.GetLength()<5)
	{
		m_strEngineId.Insert(0,"0");
	}*/
	//added end

	CString pref;
	m_comboxEngineType.GetWindowText(pref);
	int nCount = m_engine_list.GetItemCount();
	if(nCount >= 5)
	{
		AfxMessageBox("���������������5��");
		return;
	}

	m_nSelList = m_engine_list.InsertItem(nCount, pref);
	m_engine_list.SetItemText(m_nSelList, 1, m_strEngineId);
	if((nItemData&0xFF0000)==0)
		m_engine_list.SetItemText(m_nSelList, 2, "ǣ��");
	else
		m_engine_list.SetItemText(m_nSelList, 2, "����");
	
	m_engine_list.SetItemText(m_nSelList, 3, strBStationName);
	m_engine_list.SetItemText(m_nSelList, 4, strEStationName);

	m_engine_list.SetItemData(m_nSelList, nItemData);

	return;
}

void CTempAddTrains::OnBnClickedButton4()
{
	if(m_nSelList == -1 || m_nSelList >= m_engine_list.GetItemCount())
	{
		AfxMessageBox("��ѡ�� Ҫ�޸ĵ��г�");
		return;
	}

	UpdateData(TRUE);
	CString strBStationName, strEStationName;

	DWORD nItemData=0;
	int nSel = m_comboxStartStation.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ������ʼվ");
		return;
	}
	nItemData = nSel;
	m_comboxStartStation.GetLBText(nSel, strBStationName);
	
	nSel = m_comboxEndStation.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ��������վ");
		return;
	}
	m_comboxStartStation.GetLBText(nSel, strEStationName);
	nItemData += (nSel<<8);

	nSel = m_comboxQianYin.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ����ǣ������վ");
		return;
	}
	nItemData += (nSel<<16);
    
	nSel = m_comboxEngineType.GetCurSel();
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ�� ��������");
		return;
	}

	CString pref;
	m_comboxEngineType.GetWindowText(pref);

	m_engine_list.SetItemText(m_nSelList, 0, pref);
	m_engine_list.SetItemText(m_nSelList, 1, m_strEngineId);
	if((nItemData&0xFF0000)==0)
		m_engine_list.SetItemText(m_nSelList, 2, "ǣ��");
	else
		m_engine_list.SetItemText(m_nSelList, 2, "����");
	m_engine_list.SetItemText(m_nSelList, 3, strBStationName);
	m_engine_list.SetItemText(m_nSelList, 4, strEStationName);

	m_engine_list.SetItemData(m_nSelList, nItemData);

	return;
}

void CTempAddTrains::OnBnClickedButton2()
{
	if(m_nSelList == -1 || m_nSelList >= m_engine_list.GetItemCount())
	{
		AfxMessageBox("��ѡ�� Ҫ�޸ĵ��г�");
		return;
	}

	m_engine_list.DeleteItem(m_nSelList);

	return;
}

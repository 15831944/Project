// TrainAttributeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TrainAttributeSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern void  SetTrainAbsTimeType(CTG_TRAIN* ptrain, BYTE run_type);
extern BYTE  GetTrainAbsTimeType(CTG_TRAIN* ptrain);
/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet

IMPLEMENT_DYNAMIC(CTrainAttributeSheet, CPropertySheet)

CTrainAttributeSheet::CTrainAttributeSheet(CWnd* pParentWnd, LPCTSTR pszCaption, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pDoc = NULL;
	m_page1 = NULL;
	m_page2 = NULL;
}

CTrainAttributeSheet::~CTrainAttributeSheet()
{
	delete m_page1;
	delete m_page2;
}


BEGIN_MESSAGE_MAP(CTrainAttributeSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTrainAttributeSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet message handlers

void CTrainAttributeSheet::AddPropertyPage()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ABOUTDLL);
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
	m_psh.hIcon = m_hIcon;
    
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("��ע��: �ó������Ѿ�ɾ��, �����޸��г�����", "����");
		return;
	}
	m_train = *pTrain;
	m_page1 = new  CPageTrainAttribute(&m_train, m_pDoc);
	m_page2 = new  CPageTrainAttributeOther(&m_train, m_pDoc);
    m_page1->m_psp.dwFlags &= ~PSP_HASHELP;
    m_page2->m_psp.dwFlags &= ~PSP_HASHELP;
	AddPage(m_page1);
 	AddPage(m_page2); 
}

BOOL CTrainAttributeSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	RECT rc; // ��������ҳ�Ի���Ĵ�С 
	GetWindowRect (&rc); 
	rc.bottom += 30; //����������30�㣬��OK��Ť������ʾ���� 
	MoveWindow (&rc); //�������� 
	GetDlgItem(IDOK)->ShowWindow(SW_SHOW); //��ʾ���ص�OK��ť 
	GetDlgItem(IDOK)->EnableWindow(); //����OK��ť
	GetDlgItem(IDCANCEL)->ShowWindow(SW_SHOW); //��ʾ���ص�Cancel��ť 
	GetDlgItem(IDCANCEL)->EnableWindow(); //����Cancel��ť 
	
	if(!m_pDoc->IsCanEditTrain())
	{
		CWnd *pWnd = GetDlgItem(IDOK);
		if(pWnd)
		    pWnd->EnableWindow(FALSE);
	}
	    
	SetCenterPosition();
	return bResult;
}

void CTrainAttributeSheet::SetCenterPosition()
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

BOOL CTrainAttributeSheet::Create(CWnd* pParentWnd , DWORD dwStyle , DWORD dwExStyle)
{
	// TODO: �ڴ����ר�ô����/����û���
	AddPropertyPage();
	return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

BOOL CTrainAttributeSheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(HIWORD(wParam)   ==   BN_CLICKED)             
	{             
		switch(LOWORD(wParam)) 
		{      
		case IDOK:             
			{
				CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
				if(NULL == pTrain)
				{
					MessageBox("��ע��: �ó������Ѿ�ɾ��, �޸��г�����ʧ��", "����");
					return FALSE;
				}

				int nNotChangeRunType=0;
				if (pTrain->myTrainInfo.type!=this->m_page1->m_nTrainType 
					|| pTrain->myTrainInfo.run_type!=this->m_page1->m_nTrainAbs)
				{
					std::map<int, TrainAttribute>::iterator it = m_pDoc->mTrainType.mTrainAttributeMap.find(this->m_page1->m_nTrainType);
					if(it != m_pDoc->mTrainType.mTrainAttributeMap.end())
					{
						if(it->second.run_type != this->m_page1->m_nTrainAbs)
						{
							CString str1,str2;
							std::map<int, CString>::iterator it1 = m_pDoc->mTrainType.mRunTypeNameMap.find(it->second.run_type);
							if(it1 != m_pDoc->mTrainType.mRunTypeNameMap.end())
							{
								str1=(*it1).second;
							}

							it1 = m_pDoc->mTrainType.mRunTypeNameMap.find(this->m_page1->m_nTrainAbs);
							if(it1 != m_pDoc->mTrainType.mRunTypeNameMap.end())
							{
								str2=(*it1).second;
							}
							CString text;
							text.Format("��ע��, ����%s��������Ϊ\"%s\"��ѡ����������\"%s\"��ͬ!\n��ȷʵҪ��ô�޸���?",
								m_pDoc->GetTrainShowId(pTrain),str1,str2);
							m_pDoc->sysprint(4,0,"[TG]: %s", text);
							if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION)!=IDYES)
								return FALSE;
							m_pDoc->sysprint(4,0,"[TG]: ����Ա���ʹ��:%s",str2);
							nNotChangeRunType=1;
						}
					}
					m_pDoc->sysprint(4,0,"[TG]: ����Ա�޸��г����� %s type %d run_type %d (ԭ����Ϊ type %d run_type %d) nNotChangeRunType=%d", 
								m_pDoc->GetTrainShowId(pTrain),this->m_page1->m_nTrainType, this->m_page1->m_nTrainAbs,
								pTrain->myTrainInfo.type,pTrain->myTrainInfo.run_type, nNotChangeRunType);
				}

				SendTrainUpdate(nNotChangeRunType); 
				DestroyWindow(); 
				return   TRUE; 
			}
		case IDCANCEL:             
			{
				DestroyWindow(); 
				return   TRUE; 
			}
		}
	}
	return CPropertySheet::OnCommand(wParam, lParam);
}

void CTrainAttributeSheet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CPropertySheet::PostNcDestroy();
}

void CTrainAttributeSheet::SendTrainUpdate(int nNotChangeRunType)
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
		return;

	CTG_TRAIN train = *pTrain;
	// page1 data
	train.myTrainInfo.type     =  this->m_page1->m_nTrainType;
	train.myTrainInfo.run_type =  this->m_page1->m_nTrainAbs;

	if(!m_pDoc->mTrainType.mAbsTimeTypeNameMap.empty())
	{
		BYTE nOldRunType = GetTrainAbsTimeType(&train);
		BYTE nNewRunType = m_page1->m_nAbsTimeType;
		if(nOldRunType!=nNewRunType)
		{
			CString text;
			text.Format("��ע��, ���޸��� ����ʱ������ �⽫���¼���ó���������ʱ��! ��ȷʵҪ�޸���?");
			if(MessageBox(text, "��ʾ", MB_YESNO|MB_ICONQUESTION)!=IDYES)
				return;
			SetTrainAbsTimeType(&train, nNewRunType);
			m_pDoc->sysprint(4,0,"[TG]: ����Ա�޸��г�����ʱ������ %s type %d run_type %d abs_time_type %d", 
					m_pDoc->GetTrainShowId(train),
					train.myTrainInfo.type, 
					train.myTrainInfo.run_type,
					nNewRunType);
		}
	}
	
	if(m_page1->m_check_join_adjust)
		train.myTrainInfo.adjust_flag  = train.myTrainInfo.adjust_flag | 0x1;
	else
		train.myTrainInfo.adjust_flag  = train.myTrainInfo.adjust_flag & 0xfffffffe;

	// page2 data
	if(!m_page2->bNotChanged)
	{
		//page2 data
		if(m_page2->m_check1)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x01; 
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x0e;

		if(m_page2->m_check2)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x02;
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x0d;

		if(m_page2->m_check3)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x04;
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x0b;

		if(m_page2->m_check4)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x08;
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x07;

		if(m_page2->m_check5)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x10;
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0xe0;

		if(m_page2->m_check6)
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0x20;
		else
			train.myTrainInfo.no_meet_flag = train.myTrainInfo.no_meet_flag | 0xd0;

		train.myTrainInfo.space_rank  = this->m_page2->m_nCX;   //���޵ȼ�
		train.myTrainInfo.length_rank = this->m_page2->m_nCC;   //�����ȼ�
		train.myTrainInfo.super_rank  = this->m_page2->m_nZY;    //ר�˵ȼ�
		train.myTrainInfo.military_rank = this->m_page2->m_nJJ;  //���˵ȼ�

		long speed_limit = 0;//atoi(this->m_page2->m_speed );	//�ٶȵȼ�
		/*for(UINT i=0; i<train.GetRecordNum(); i++)
		{
			TgTRAIN_RECORD* prec=train.GetRecordByRecIndex(i);
			if(prec)
				prec->speed_limit=speed_limit;
		}*/
		m_pDoc->sysprint(4,0,"[TG]: ����Ա�޸��г��������� %s space_rank %d length_rank %d super_rank %d military_rank %d", 
			m_pDoc->GetTrainShowId(train),
			train.myTrainInfo.space_rank, 
			train.myTrainInfo.length_rank,
			train.myTrainInfo.super_rank,
			train.myTrainInfo.military_rank);
	}

	m_pDoc->SendUpdateTrain(train, SCHD_UPDATE, TRAIN_ATTRIB_CHANGED, nNotChangeRunType, 0);
}
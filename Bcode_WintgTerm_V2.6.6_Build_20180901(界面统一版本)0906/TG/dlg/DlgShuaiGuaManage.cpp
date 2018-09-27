// DlgShuaiGuaManage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TG.h"
#include "DlgShuaiGuaManage.h"
#include ".\dlgshuaiguamanage.h"


// CDlgShuaiGuaManage �Ի���

IMPLEMENT_DYNAMIC(CDlgShuaiGuaManage, CDialog)
CDlgShuaiGuaManage::CDlgShuaiGuaManage(CTGDoc* pdoc, CTG_TRAIN* train,CWnd* pParent)
	: CDialog(CDlgShuaiGuaManage::IDD, pParent)
{
	mTrain = *train;
	m_pDoc = pdoc;
    dlg_title="";
}

CDlgShuaiGuaManage::~CDlgShuaiGuaManage()
{
}

void CDlgShuaiGuaManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_train_record_list);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
}


BEGIN_MESSAGE_MAP(CDlgShuaiGuaManage, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnNMClickList2)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CDlgShuaiGuaManage ��Ϣ�������
BOOL CDlgShuaiGuaManage::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWnd* pWnd = GetDlgItem(IDC_EDIT1); 
	if(pWnd)
	{
		pWnd->SetWindowText(m_pDoc->GetTrainShowId(mTrain));
	}

	DWORD dwStyle = m_train_record_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_train_record_list.SetExtendedStyle(dwStyle);

	m_train_record_list.InsertColumn(0, "���",LVCFMT_CENTER,40);
	m_train_record_list.InsertColumn(1, "��վ����",LVCFMT_CENTER,90);
	m_train_record_list.InsertColumn(2, "˦������",LVCFMT_CENTER,90);
	m_train_record_list.InsertColumn(3, "˦����Ϣ",LVCFMT_CENTER,120);
	
	for(int i=0; i<mTrain.GetRecordNum(); i++)
	{		
		TgTRAIN_RECORD* rec = mTrain.GetRecordByRecIndex(i);

		CString staname = m_pDoc->GetStationName(rec->station);

		CComboBox* pbox = (CComboBox*)GetDlgItem(IDC_COMBO5);
		if(pbox)
		{
			pbox->AddString(staname);
			/*if(i==mTrain.GetRecordNum()-1)
				pbox->SetCurSel(0);*/
			if (!sta_name.IsEmpty() && strcmp(sta_name,staname)==0)
				pbox->SetCurSel(i);
		}

		CString itemno;
		itemno.Format("%d",i+1) ;
		int nRow = m_train_record_list.InsertItem(i, itemno);   
	
		m_train_record_list.SetItemText(nRow,1,staname);
		CString str="";
		if(rec->szStationText.Find('-')>=0)
			str += "˦";
		if(rec->szStationText.Find('+')>=0)
			str += "��";
		m_train_record_list.SetItemText(nRow,2,str);
		//modified by zhaoliang for shuaigua manage 
		//�����ж���ӵ������Ǳ� ����Ա �޸Ļ��� OPMS �޸�
		//Ĭ������£���վ��Ϣ��˦�ҹ��������Ի����ڵ�����ȫ��Ĭ��Ϊ ����Ա�޸�
		//����Ա�޸�Ϊ 0  OPMSΪ 1
		//�������Ա��ӳ�վ��Ϣʱ������ʾ˦����Ϣ���Ӷ���˦����Ϣ���

		m_train_record_list.SetItemText(nRow,3,rec->szStationText); 

		m_train_record_list.SetItemData(nRow, i);
	}

	m_combox.SetCurSel(0);

	((CEdit*)GetDlgItem(IDC_EDIT2))->SetWindowText("-");

	Statistic();
	
	UpdateData(FALSE);

	return TRUE;
}


//����
void CDlgShuaiGuaManage::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO5);
	if(NULL == pBox)
		return;

	int nSel = pBox->GetCurSel();
	if(-1 == nSel || nSel >= m_train_record_list.GetItemCount())
		return;

	CString strWork;
	strWork=m_train_record_list.GetItemText(nSel, 2);
	if(!strWork.IsEmpty())
	{
		if(MessageBox("��վ������ҵ��Ϣ,ȷ���޸���", "��ʾ", MB_YESNO)==IDNO)
			return;
	}
	m_combox.GetWindowText(sg_type);
	m_train_record_list.SetItemText(nSel, 2, sg_type);

	((CEdit*)GetDlgItem(IDC_EDIT2))->GetWindowText(sg_info);
	m_train_record_list.SetItemText(nSel, 3, sg_info);
	Statistic();
	return;
}



void CDlgShuaiGuaManage::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO5);
	if(NULL == pBox)
		return;

	int nSel = pBox->GetCurSel();
	if(-1 == nSel || nSel >= m_train_record_list.GetItemCount())
		return;

	m_combox.GetWindowText(sg_type);
	m_train_record_list.SetItemText(nSel, 2, sg_type);

	((CEdit*)GetDlgItem(IDC_EDIT2))->GetWindowText(sg_info);
	m_train_record_list.SetItemText(nSel, 3, sg_info);

    UpdateData(FALSE);
	Statistic();
	return;
}

void CDlgShuaiGuaManage::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO5);
	if(NULL == pBox)
		return;

	int nSel = pBox->GetCurSel();
	if(-1 == nSel || nSel >= m_train_record_list.GetItemCount())
		return;

	m_train_record_list.SetItemText(nSel, 2, "");
	m_train_record_list.SetItemText(nSel, 3, "");

    UpdateData(FALSE);
	Statistic();
	return;
}

void CDlgShuaiGuaManage::Statistic()
{
	m_nAttachHeavyCnt=0;
	m_nAttachEmptyCnt=0;

	CString strTemp, strHeavyCar, strEmptyCar;
	for(int nSel=0;nSel<m_train_record_list.GetItemCount();nSel++)
	{
		int nCarCount = 0;
		
		strTemp =m_train_record_list.GetItemText(nSel,3);
		
		int pos = 0;
		pos = strTemp.Find('/');
		if(-1 != pos)
		{
			strHeavyCar = strTemp.Left(pos);
			strEmptyCar = strTemp.Right(strTemp.GetLength() - pos - 1);
			pos = strHeavyCar.Find('+');
			if(-1 != pos)
			{
				strHeavyCar = strHeavyCar.Right(strHeavyCar.GetLength() - pos - 1);
				nCarCount = atoi(strHeavyCar);
				m_nAttachHeavyCnt += nCarCount;
			}
			pos = strEmptyCar.Find('+');
			if(-1 != pos)
			{
				strEmptyCar = strEmptyCar.Right(strEmptyCar.GetLength() - pos - 1);
				nCarCount = atoi(strEmptyCar);
				m_nAttachEmptyCnt += nCarCount;
			}
		}
		else
		{
			strHeavyCar = strTemp;
			pos = strHeavyCar.Find('+');
			if(-1 != pos)
			{
				strHeavyCar = strHeavyCar.Right(strHeavyCar.GetLength() - pos - 1);
				nCarCount = atoi(strHeavyCar);
				m_nAttachHeavyCnt += nCarCount;
			}
		}
	}

	dlg_title.Format("��վ˦�ҳ���ҵ�ƻ������س� %d��,�ҿճ� %d��", m_nAttachHeavyCnt, m_nAttachEmptyCnt);
	SetWindowText(dlg_title);
}
void CDlgShuaiGuaManage::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    //modified by zhaoliang for shuaigua manage 
	for(int nSel=0;nSel<m_train_record_list.GetItemCount();nSel++)
	{
		
		mTrain.GetRecordByRecIndex(nSel)->szStationText = m_train_record_list.GetItemText(nSel,3);

	}

	OnOK();
}

//ѡ��ĳ��
void CDlgShuaiGuaManage::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	for(int colno =0;colno<m_train_record_list.GetItemCount(); colno++)
	{
		
		if (m_train_record_list.GetItemState(colno,LVIS_SELECTED) == LVIS_SELECTED)
		{ 
			CComboBox* pbox = (CComboBox*)GetDlgItem(IDC_COMBO5);
			if(pbox)
			{
				pbox->SetCurSel(colno);
				sg_type = m_train_record_list.GetItemText(colno, 2);
				sg_info = m_train_record_list.GetItemText(colno, 3);

				if(sg_type.IsEmpty())
				{
					m_combox.SetCurSel(0);
				}
				else
				{
					m_combox.SetWindowText(sg_type);
				}

				CEdit* pBox1 = (CEdit*)GetDlgItem(IDC_EDIT2);
				if(NULL != pBox1)
				{
					if (sg_type.IsEmpty())
						pBox1->SetWindowText("-");
					else
						pBox1->SetWindowText(sg_info);
				}
			}
		}	
	}

	*pResult = 0;
}

void CDlgShuaiGuaManage::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel = m_combox.GetCurSel();
	CString text="";
	if(nSel == 0)
		text = "-";
	else if(nSel == 1)
		text = "+";
	else if(nSel == 2)
		text = "+/-";

	CWnd* pWnd = GetDlgItem(IDC_EDIT2); 
	if(pWnd)
	{
		pWnd->SetWindowText(text);
	}

}

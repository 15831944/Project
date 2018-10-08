// DlgTDMSBasicTip.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTDMSBasicGen.h"
#include ".\dlgtdmsbasicgen.h"
#include "DlgConfirm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgSelTDMSBasicGen::CDlgSelTDMSBasicGen(CWnd* pParent, CTG_TrainScheduleBase* pData, CWorkScheduleHolder* pHolder)
	: CDialog(CDlgSelTDMSBasicGen::IDD, pParent)
{
	m_pData   = pData;
	m_pHolder = pHolder;
}

CDlgSelTDMSBasicGen::~CDlgSelTDMSBasicGen()
{
}

void CDlgSelTDMSBasicGen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_all_list);
	DDX_Control(pDX, IDC_LIST2, m_sel_list);
}

BEGIN_MESSAGE_MAP(CDlgSelTDMSBasicGen, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedGenBasic)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedClearSel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedAppendMultiSel)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedAppendAll)
	
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
	ON_LBN_DBLCLK(IDC_LIST2, OnLbnDblclkList2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelTDMSBasicGen message handlers
BOOL CDlgSelTDMSBasicGen::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_all_list.ResetContent();
	m_sel_list.ResetContent();

	CTG_TrainPtrGroup group;
	m_pData->FindAllTrains(group);
	for(int n=0; n<group.GetTrainCount(); n++)
	{
		CTG_SingleTrain* ptrain=group.GetTrainPtr(n);
		if(ptrain==NULL)
			continue;
		CTG_TrainRecord *prec=ptrain->GetTrainFirstRecord();
		if(prec==NULL)
			continue;

		DWORD gid   = ptrain->GetTrainIndex();
		CString str = ptrain->GetTrainID();
		long tm1=0;
		if(prec->IsStartRec())
			tm1=prec->GetDepartTime();
		else
			tm1=prec->GetArriveTime();

		CString ss;
		ss.Format("%02d:%02d:%02d--%s", tm1/3600, (tm1/60)%60, tm1%60, str);
		str=ss;
		
		int nSel=m_all_list.AddString(str);
		m_all_list.SetItemData(nSel, gid);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelTDMSBasicGen::OnBnClickedClearSel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(MessageBox("��ȷ��Ҫ���ѡ������ͼ����ͼ������?","��ʾ",MB_YESNO)==IDYES)
	{
		m_sel_list.ResetContent();
	}
}

void CDlgSelTDMSBasicGen::OnBnClickedAppendAll()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(MessageBox("��ȷ��Ҫ���������г���ͼ����ͼ��?","��ʾ",MB_YESNO)!=IDYES)
	{
		return;
	}
	
	m_sel_list.ResetContent();
	for(int idx=0; idx<m_all_list.GetCount(); idx++) 
	{
		CString str;  
		m_all_list.GetText(idx,str);  
		DWORD gid=m_all_list.GetItemData(idx);

		AddGenBasicTrain(str, gid);
	}
}

void CDlgSelTDMSBasicGen::OnBnClickedAppendMultiSel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iSelCount=m_all_list.GetSelCount();
	if(iSelCount<=0)
		return;
	CArray<int,int> aryListBoxSel; 
	aryListBoxSel.SetSize(iSelCount);  
	m_all_list.GetSelItems(iSelCount, aryListBoxSel.GetData());   

	for (int i=0;i<iSelCount;i++)  
	{
		CString str;  
		m_all_list.GetText(aryListBoxSel[i],str);  
		DWORD gid=m_all_list.GetItemData(aryListBoxSel[i]);

		CString str_result=AddGenBasicTrain(str, gid);
		if(str_result!="")
		{
			MessageBox(str_result, "��ʾ");
		}
	}
}

CString CDlgSelTDMSBasicGen::AddGenBasicTrain(CString str, DWORD gid)
{
	CString str_result;
	for(int i=0; i<m_sel_list.GetCount(); i++)
	{
		CString str2;  
		m_sel_list.GetText(i,str2); 
		DWORD tgid=m_sel_list.GetItemData(i);
		if(tgid==gid)
		{
			str_result.Format("����%s�Ѿ���ѡ�п���,����Ҫ�ٴ�����%s",str2,str);
			break;
		}
	}
	if(i>=m_sel_list.GetCount())
	{
		str_result="";
		int iSel=m_sel_list.AddString(str);
		m_sel_list.SetItemData(iSel,gid);
	}

	return str_result;
}

void CDlgSelTDMSBasicGen::OnLbnDblclkList1()
{
	int nSel=m_all_list.GetCurSel();
	if(nSel>=0)
	{
		CString str;  
		m_all_list.GetText(nSel,str);  
		DWORD gid=m_all_list.GetItemData(nSel);

		CString str_result=AddGenBasicTrain(str, gid);
		if(str_result!="")
		{
			MessageBox(str_result, "��ʾ");
		}
	}
}

void CDlgSelTDMSBasicGen::OnLbnDblclkList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel=m_sel_list.GetCurSel();
	if(nSel>=0)
	{
		m_sel_list.DeleteString(nSel);
	}
}

void CDlgSelTDMSBasicGen::OnBnClickedGenBasic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(MessageBox("��ȷ��Ҫ��ѡ���г���ͼ����ͼ��?","��ʾ",MB_YESNO)!=IDYES)
	{
		return;
	}

	if(m_pHolder==NULL)
	{
		GpcLogTrainMessage(0,0,"��ѡTDMS����ͼ��ͼ�쳣.");
		return;
	}
	CTG_ScheduleInfo schdinfo;
	CTG_TrainScheduleBase *pBasicPrevData=m_pHolder->GetBasicPrevScheduleData();
	pBasicPrevData->GetScheduleInfo(schdinfo); 

	CStringArray strTextList, strTextList2;
	for(int n=0; n<m_sel_list.GetCount(); n++)
	{
		CString str;
		m_sel_list.GetText(n,str);
		DWORD gid=m_sel_list.GetItemData(n);

		CTG_SingleTrain* ptrain = m_pData->GetSingleTrain(gid);
		if(NULL!=ptrain)
		{
			CTG_SingleTrain *pold=pBasicPrevData->GetSingleTrain(ptrain->GetTrainIndex()); 
			if(pold)
			{
				CString tmp;
				tmp.Format("����%s��ͼ������Ԥ������ͼ���г���%s,���߾�����ͬ��ȫ��ID", str, pold->GetTrainID());
				strTextList2.Add(tmp);
			}
			else
			{
				CTG_SingleTrain *pold=pBasicPrevData->GetSingleTrain(ptrain->GetTrainID()); 
				if(pold)
				{
					CString tmp;
					tmp.Format("����%s��Ԥ������ͼ���Ѵ���", str);
					strTextList.Add(tmp);
				}
			}
		}
	}

	if(strTextList2.GetCount()>0)
	{
		CString text="��ע��,��ͼ��������������,������ͼ";
		CDlgConfirm dlg(text, strTextList2, this);
		dlg.DoModal();

		MessageBox("������ͼ!","��ʾ");
		return;
	}

	if(strTextList.GetCount()>0)
	{
		CString text="��ע��,��ͼ��������������:";
		CDlgConfirm dlg(text, strTextList, this);
		dlg.DoModal();
		
		if(MessageBox("���Ƿ�ȷ�ϼ�����ͼ?","��ʾ",MB_YESNO)!=IDYES)
			return;
	}
	for(int n=0; n<m_sel_list.GetCount(); n++)
	{
		CString str;
		m_sel_list.GetText(n,str);
		DWORD gid=m_sel_list.GetItemData(n);

		CTG_SingleTrain* ptrain = m_pData->GetSingleTrain(gid);
		if(NULL!=ptrain)
		{
			GpcLogTrainMessage(0,0,"��ѡTDMS����ͼ��ͼ%s(%u).", str, gid);
			CTG_SingleTrain train=*ptrain;
			CTG_SingleTrain *pold=pBasicPrevData->GetSingleTrain(train.GetTrainIndex());
			if(pold==NULL)
			{
				pBasicPrevData->AppendTrain(schdinfo,train,SYCFLAG_NULL);
				GpcLogTrainMessage(0,0,"��ѡTDMS����ͼ��ͼ�г� %s(%u).",train.GetTrainID(), train.GetTrainIndex());
			}
		}
	}
}

void CDlgSelTDMSBasicGen::OnClose()
{
	DestroyWindow();
}

void CDlgSelTDMSBasicGen::PostNcDestroy() 
{
	delete this;
}

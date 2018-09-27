// TccCommand_FullCancel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
//#include "sysruninfo.h"
#include "tcc_dlg.h"
#include "tcc.h"
#include "TccCommand_FullCancel.h"
#include "TccCommandSendDlg.h"

namespace TCC_C3
{
// CTccCommand_FullCancel �Ի���

IMPLEMENT_DYNAMIC(CTccCommand_FullCancel, CDialog)
CTccCommand_FullCancel::CTccCommand_FullCancel(
	stano_t nTccID, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	m_pInfoProvider = pInfoProvider;
	m_pInfoProvider->AddRef();

	m_pAdviseSink = pAdviseSink;
	m_pAdviseSink->AddRef();

	m_nTccID = nTccID;
}

CTccCommand_FullCancel::~CTccCommand_FullCancel()
{
	if(m_pInfoProvider)
	{
		m_pInfoProvider->Release();
		m_pInfoProvider = NULL;
	}

	if(m_pAdviseSink)
	{
		m_pAdviseSink->Release();
		m_pAdviseSink = NULL;
	}
}

void CTccCommand_FullCancel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TCC_CMD_TCC, m_TccStation);
	DDX_Control(pDX, IDC_TCC_CMD_ABSLINE, m_AbsLine);
}

BEGIN_MESSAGE_MAP(CTccCommand_FullCancel, CDialog)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_TCC, OnCbnSelchangeTccStation)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_TCCDSEND, OnBnClickedBtnDtccsend)
END_MESSAGE_MAP()

// CTccCommand_FullCancel ��Ϣ�������
BOOL CTccCommand_FullCancel::OnInitDialog()
{
	CDialog::OnInitDialog();

	ITccClientInfoProvider *pInfoProvider = m_pInfoProvider;
	if(NULL == pInfoProvider)
	{
		MessageBox("û�а�װTccClientInfoProvider�ӿ�!");
		return FALSE;
	}
	ITccAdviseSink *pAdviseSink = m_pAdviseSink;
	if(NULL == pAdviseSink)
	{
		MessageBox("û�а�װTccAdviseSink�ӿ�!");
		return FALSE;
	}
	if(!TccInitialize(m_pInfoProvider))
	{
		return FALSE;
	}

	INT nIndex;
	CString str;

	// ��ʼ��Ŀ������վ
	std::vector<stano_t> staList;
	pInfoProvider->GetStationList(staList);
	for(int i=0; i<staList.size(); i++)
	{
		str = pInfoProvider->GetStationName(staList[i]);
		nIndex = m_TccStation.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_TccStation.SetItemData(nIndex, staList[i]);
		if(m_nTccID == staList[i])
			m_TccStation.SetCurSel(nIndex);
	}
	OnCbnSelchangeTccStation();

	
	//��ʼ���Ի���λ��
	CWnd* m_pParentWnd = GetParent();
	CRect rectParentClient;
	m_pParentWnd->GetClientRect(&rectParentClient);
	ClientToScreen(&rectParentClient);
	CPoint pt = rectParentClient.CenterPoint();

	CRect rectWnd,rectWnd2;
	GetWindowRect(&rectWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// Ŀ������ѡ���, ��·�б����ű仯
void CTccCommand_FullCancel::OnCbnSelchangeTccStation()
{
	m_AbsLine.ResetContent();

	INT nIndex;
	nIndex = m_TccStation.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_AbsLine.EnableWindow(FALSE);
		return;
	}
	m_AbsLine.EnableWindow(TRUE);

	stano_t nTccID = m_TccStation.GetItemData(nIndex);

	std::vector<lineid_t> lines;
	m_pInfoProvider->GetTccLineList(nTccID, lines);
	if(0 == lines.size())
	{
		CString str;
		str.Format("Ŀ��վ%dû����·�б�", nTccID);
		MessageBox(str);
		return ;
	}
	for(int i=0; i<lines.size(); i++)
	{
		nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(nTccID, lines[i]) );
		ASSERT(CB_ERR != nIndex);
		m_AbsLine.SetItemData(nIndex, lines[i]);
	}
	m_AbsLine.SetCurSel(-1);
}

void CTccCommand_FullCancel::OnBnClickedCancel()
{
	OnCancel();
}


void CTccCommand_FullCancel::OnBnClickedBtnDtccsend()
{
	TCC_COMMAND cmd;

	cmd.nConsoleID = m_pInfoProvider->GetConsoleID();

	INT nIndex;
	nIndex = m_TccStation.GetCurSel();
	if(CB_ERR == nIndex)
	{
		MessageBox("û��ѡ������վ!");
		return;
	}
	cmd.nTccID = m_TccStation.GetItemData(nIndex);

	nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
	{
		MessageBox("û��ѡ����·!");
		return;
	}
	cmd.nTccLineID = (lineid_t)m_AbsLine.GetItemData(nIndex);
	cmd.nLineID = m_pInfoProvider->GetLineID(cmd.nTccID, cmd.nTccLineID);

	SysidCreate(&cmd.sys_id);			//�ڲ���ʶ
	cmd.nCmdID = 0xFFFF;			//�����(�пر���)
	cmd.nSpeedMaxVal = 0;	        //����ֵ(�пر���),�˴���0���ɣ����ɻ��ж�bCancelLimit�����Զ�ת��Ϊ0xFF
	cmd.bCancelLimit = TRUE;        // ����ΪTRUE
	cmd.bFullLimit = TRUE;
	cmd.bStartNow = TRUE;
	cmd.bEndless  = TRUE;

	cmd.nKmMark[0] = 0;
	cmd.nKmMark[1] =9999999;

	//cmd.nTccKmMark[0] = 0;	//����վ����귶Χ(�пر���)
	//cmd.nTccKmMark[1] = 9999999;
	cmd.nTccKmMarkFlag[0] = cmd.nTccKmMarkFlag[1] = 0;

	CTccCommandSendDlg dlg(cmd, CTccCommandSendDlg::TCC_COMMAND_FULLCANCEL, NULL, m_pInfoProvider, m_pAdviseSink, this);
	dlg.DoModal();
}


}
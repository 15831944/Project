// NeibPlanFuncChoiceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NeibPlanFuncChoiceDlg.h"
#include ".\neibplanfuncchoicedlg.h"


// CNeibPlanFuncChoiceDlg �Ի���

IMPLEMENT_DYNAMIC(CNeibPlanFuncChoiceDlg, CDialog)
CNeibPlanFuncChoiceDlg::CNeibPlanFuncChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeibPlanFuncChoiceDlg::IDD, pParent)
{
	nRet=0;
}

CNeibPlanFuncChoiceDlg::~CNeibPlanFuncChoiceDlg()
{
}

void CNeibPlanFuncChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNeibPlanFuncChoiceDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CNeibPlanFuncChoiceDlg ��Ϣ�������

void CNeibPlanFuncChoiceDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	nRet=0;
	OnOK();
}

void CNeibPlanFuncChoiceDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	nRet=1;
	OnOK();
}

void CNeibPlanFuncChoiceDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	nRet=2;
	OnOK();
}

BOOL CNeibPlanFuncChoiceDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		nRet=0;
		OnOK();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

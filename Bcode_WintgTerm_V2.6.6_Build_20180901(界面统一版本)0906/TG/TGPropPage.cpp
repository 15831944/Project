// TGPropPage.cpp : CTGPropPage ����ҳ���ʵ�֡�

#include "stdafx.h"
#include "TG.h"
#include "TGPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTGPropPage, COlePropertyPage)



// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CTGPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CTGPropPage, "TG.TGPropPage.1",
	0x4b566035, 0xd572, 0x4c02, 0x8f, 0x4f, 0x30, 0xc6, 0x4f, 0x82, 0xd2, 0x9f)



// CTGPropPage::CTGPropPageFactory::UpdateRegistry -
// ��ӻ��Ƴ� CTGPropPage ��ϵͳע�����

BOOL CTGPropPage::CTGPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_TG_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CTGPropPage::CTGPropPage - ���캯��

CTGPropPage::CTGPropPage() :
	COlePropertyPage(IDD, IDS_TG_PPG_CAPTION)
{
}



// CTGPropPage::DoDataExchange - ��ҳ�����Լ��ƶ�����

void CTGPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CTGPropPage ��Ϣ�������

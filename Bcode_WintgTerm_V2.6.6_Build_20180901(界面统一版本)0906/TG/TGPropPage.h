#pragma once

// TGPropPage.h : CTGPropPage ����ҳ���������


// CTGPropPage : �й�ʵ�ֵ���Ϣ������� TGPropPage.cpp��

class CTGPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CTGPropPage)
	DECLARE_OLECREATE_EX(CTGPropPage)

// ���캯��
public:
	CTGPropPage();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_TG };

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ��Ϣӳ��
protected:
	DECLARE_MESSAGE_MAP()
};


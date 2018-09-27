#pragma once
#include "resource.h"
#include "tg_schedule_base.h"
// CDlgTestTD �Ի���

class CDlgDataXMLShow : public CDialog
{
	DECLARE_DYNAMIC(CDlgDataXMLShow)

public:
	CDlgDataXMLShow(CTG_LogicObject *pobj, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDataXMLShow();

// �Ի�������
	enum { IDD = IDD_DIALOG_XML_SHOW };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
	
private:
	CTreeCtrl m_tree;
	CTG_LogicObject *m_pobj;
public:
	void DisplayXML();
};


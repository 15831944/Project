#pragma once

#include "mytoolbar.h"
// CTYPlanFrame ���

class CTYPlanFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTYPlanFrame)
protected:
	CTYPlanFrame();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CTYPlanFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CMyToolBar m_wndToolBar;
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnClose();
};



#pragma once
#include "mytoolbar.h"
#include "tgframe.h"

// CTGFrame ���
class CWorkTgFrame : public CTgFrame
{
	DECLARE_DYNCREATE(CWorkTgFrame)
protected:
	CWorkTgFrame();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWorkTgFrame();
private:
	int    m_nNotifyAlert;
	int    m_nNotifyPlan;
	int    m_nNotifyNeigPlan;
	int    m_nSoundCout;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMemoryWnd();
	LRESULT OnMemoryNotify(WPARAM wParam, LPARAM lParam);
};



#pragma once
#include "mytoolbar.h"
#include "tgframe.h"

// CTGFrame ���
class CShiftTgFrame : public CTgFrame
{
	DECLARE_DYNCREATE(CShiftTgFrame)
protected:
	CShiftTgFrame();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CShiftTgFrame();
private:
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
};



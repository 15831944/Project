#pragma once
#include "mytoolbar.h"
#include "tgframe.h"

// CTGFrame ���
class CBasicTgFrame : public CTgFrame
{
	DECLARE_DYNCREATE(CBasicTgFrame)
protected:
	CBasicTgFrame();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CBasicTgFrame();
private:
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
};



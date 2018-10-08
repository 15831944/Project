// TGFrame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "ShiftTGFrame.h"
#include "sysshare.h"
#include "tgview.h"
#include "function.h"
#include "tgmsg.h"
// CTGFrame

IMPLEMENT_DYNCREATE(CShiftTgFrame, CTgFrame)

CShiftTgFrame::CShiftTgFrame()
{
}

CShiftTgFrame::~CShiftTgFrame()
{
}


BEGIN_MESSAGE_MAP(CShiftTgFrame, CTgFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CTGFrame ��Ϣ�������
int CShiftTgFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
		CTgFrame::OnCreate(lpCreateStruct);
		AddButtonToBar(8,"��ȡ����",ID_SHIFT_GET_TRAIN_FROM_OPMS,0);
		AddButtonToBar(9,"��ȡʩ��",ID_SHIFT_GET_MARK_FROM_OPMS,0);
		AddButtonToBar(10,"",ID_DDML_MANAGE,0,FALSE);
		if(GetTGFunctionConfig()->IsFormatView())
			AddButtonToBar(11,"��ͻ���",ID_TRAINPLAN_SIDE_MAN,0,FALSE);
		else
			AddButtonToBar(11,"���μ��",ID_TRAINPLAN_SIDE_MAN,0,FALSE);
		AddButtonToBar(12,"��·���",ID_TRAINPLAN_ROUTE_MAN,0,FALSE);
		AddButtonToBar(13,"��ƻ��Ƚ�",ID_TRAINPLAN_COMPARE,0);
		AddButtonToBar(14,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(15,"���˳���ͼ",ID_SHIFT_GENTRAIN_TO_WORK,0);
		AddButtonToBar(16,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(17,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(18,"�г���ҵ",ID_TRAIN_WORK_PLAN,0);
		AddButtonToBar(19,"",ID_DDML_MANAGE,0,FALSE);
		return 0;
}


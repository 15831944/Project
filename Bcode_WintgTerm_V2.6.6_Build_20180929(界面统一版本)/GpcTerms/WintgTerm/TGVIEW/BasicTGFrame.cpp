// TGFrame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "BasicTGFrame.h"
#include "sysshare.h"
#include "tgview.h"
#include "function.h"
#include "tgmsg.h"
// CTGFrame

IMPLEMENT_DYNCREATE(CBasicTgFrame, CTgFrame)

CBasicTgFrame::CBasicTgFrame()
{
}

CBasicTgFrame::~CBasicTgFrame()
{
}


BEGIN_MESSAGE_MAP(CBasicTgFrame, CTgFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CTGFrame ��Ϣ�������
int CBasicTgFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
		CTgFrame::OnCreate(lpCreateStruct);
		AddButtonToBar(8,"TDMS��ȡ",ID_BASIC_GET_TRAIN_FROM_OPMS,0);	
		AddButtonToBar(9,"����ͼ���",ID_TRAINPLAN_SIDE_MAN,0,FALSE);
		AddButtonToBar(10,"��·���",ID_TRAINPLAN_ROUTE_MAN,0,FALSE);
		AddButtonToBar(11,"����ͼ�Ƚ�",ID_TRAINPLAN_COMPARE,0,FALSE);
		AddButtonToBar(12,"��ʼ����",ID_INSERT_NEW_TRAIN,0,FALSE);
		AddButtonToBar(13,"��������",ID_DRAW_NEW_TRAIN_END,0,FALSE);
		AddButtonToBar(14,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(15,"���˳���ͼ",ID_BASIC_GENTRAIN_TO_WORK,0);
		AddButtonToBar(16,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(17,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(18,"",ID_DDML_MANAGE,0,FALSE);
		AddButtonToBar(19,"",ID_DDML_MANAGE,0,FALSE);
		return 0;
}


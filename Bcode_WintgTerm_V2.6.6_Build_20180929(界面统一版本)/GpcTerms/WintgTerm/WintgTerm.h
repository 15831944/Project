// WintgTerm.h : WintgTerm Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// CWintgTermApp:
// �йش����ʵ�֣������ WintgTerm.cpp
//

class CWintgTermApp : public CWinApp
{
public:
	CWintgTermApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWintgTermApp theApp;

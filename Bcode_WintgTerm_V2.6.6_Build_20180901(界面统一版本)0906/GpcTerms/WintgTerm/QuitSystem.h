#pragma once

#include ".\Logo.h"
// CQuitSystem �Ի���

class CQuitSystem : public CDialog
{
	DECLARE_DYNAMIC(CQuitSystem)

public:
	CQuitSystem(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CQuitSystem();

// �Ի�������
	enum { IDD = IDD_DIALOG_QUIT };
	CBitmap mBmp;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CLogo m_logoExit;
};

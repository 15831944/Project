#pragma once


#include "afxwin.h"
#include "resource.h"

namespace TCC
{

class CCmdTypeSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdTypeSelectDlg)

public:
	CCmdTypeSelectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCmdTypeSelectDlg();

// �Ի�������
	enum { IDD = IDD_CMD_TYPE_SELECT };

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

};

}
#pragma once
#include "utility/xmlview.h"

// CDlgGridInput �Ի���

class CDlgGridInput : public CDialog
{
	enum{MAX_FIELD_NUM=128};
	DECLARE_DYNAMIC(CDlgGridInput)
	CXMLDataGrid  m_grid;

	PropertyInfoPara m_aFielddef[MAX_FIELD_NUM];
	int  m_nFieldcnt;
public:
	CDlgGridInput(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGridInput();

// �Ի�������
	enum { IDD = IDD_DLG_GRID_INPUT };
	CString       m_xml;
	CString       m_xmlnode;	
	CString       m_caption;
	void   CDlgGridInput::Init(const char *caption,const PropertyInfoPara fielddef[],int fieldcnt, const char *initxml,const char *node);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

#pragma once
#include "afxcmn.h"


// CTGColorDlg �Ի���

class CTGColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CTGColorDlg)

public:
	CTGColorDlg(CWnd* pParent);   // ��׼���캯��
	virtual ~CTGColorDlg();

// �Ի�������
	enum { IDD = IDD_DLG_TG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slider;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	BYTE mColor;
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
};

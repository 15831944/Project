#pragma once

#include "resource.h"
#include "tgdoc.h"
// CDlgNoteText �Ի���

class CDlgNoteText : public CDialog
{
	DECLARE_DYNAMIC(CDlgNoteText)

public:
	CDlgNoteText(int displayType,CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent);   // ��׼���캯��
	virtual ~CDlgNoteText();

// �Ի�������
	enum { IDD = IDD_DIALOG_NOTE_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CString m_text;
	CString m_text2;
	USHORT	m_roundFlag;
	USHORT  m_colorFlag;
	int m_nType;

private:
	int m_displayType;
	CTg_BLOCK * m_pFigure;
	CTGDoc *m_pDoc;
	
private:
	void InitIdentify();
 	
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioShigu();
	afx_msg void OnBnClickedRadioNote();
	afx_msg void OnBnClickedRadioText();
	afx_msg void OnBnClickedRadioImport();
	afx_msg void OnBnClickedRadioBao();
	afx_msg void OnBnClickedRadioNo();
	afx_msg void OnBnClickedRadioRedRect();
	afx_msg void OnBnClickedRadioBlueRect();
	afx_msg void OnBnClickedRadioRedRound();
	afx_msg void OnBnClickedRadioBlueRound();
	afx_msg void OnBnClickedRadioRedWord();
	afx_msg void OnBnClickedRadioBlueWord();
};

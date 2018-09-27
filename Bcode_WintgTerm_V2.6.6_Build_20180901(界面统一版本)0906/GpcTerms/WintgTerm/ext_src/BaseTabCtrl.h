#if !defined(AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_)
#define AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl window
#define MAX_PAGE_COUNT 16

class AFX_EXT_CLASS CBaseTabCtrl : public CTabCtrl
{
// Construction
public:
	CBaseTabCtrl();
// Attributes
public:
	INT m_nPageCount;
	//ҳ������
	INT m_nCurPage;
	UINT m_PageIDDs[MAX_PAGE_COUNT];
	UINT m_BitmapIDDs[MAX_PAGE_COUNT];
	CDialog* m_pPages[MAX_PAGE_COUNT];
	//ͼ������
	CImageList* m_pImagelist;

// Operations
public:
	BOOL DrawTabText(CDC *pDC, const char *lpszText, const COLORREF rgb, CRect *pRect);
	BOOL DrawBkBitmap( CDC *pDC, const int iTab, const CRect *pRect);
	BOOL DrawTabIcon(CDC *pDC, const TC_ITEM tci, CRect *pRect );

private:
	COLORREF m_curItemColor;
	COLORREF m_itemColor;
	COLORREF m_bkColor;
	int m_nYOffset;
	int m_nXOffset;
	CFont m_textFont;
	COLORREF m_textColor;

	void SetRect(int nIndex);
protected:
	BOOL OnCommPaint();
	BOOL OnButStylePaint();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItemColor(COLORREF rgb);	// ����δѡ�������ɫ
	void SetCurItemColor(COLORREF rgb);	// ����ѡ�������ɫ
	void SetTextYOffset(int y);			// �����ı���Y�����ϵ�ƫ��
	void SetTextXOffset(int x);			// �����ı���X�����ϵ�ƫ��
	void SetTextFont(HFONT hFont);		// �����ı�������
	void SetTextFont(LOGFONT *logfont);	// �����ı�������
	void SetTextColor(COLORREF rgb);	// �����ı�����ɫ
	void SetBkColor( COLORREF clr );	// ����Tab�ؼ��ı�����ɫ
	/*���һ���Ի���ؼ���TABҳ*/
	BOOL AddPage(LPCTSTR lpTitle, CDialog *pDialog, UINT ID, INT nIcon=-1, UINT nBkBitmapID=0);
	BOOL SetImageList(CImageList* pImageList);
	BOOL SetCurPage( int nIndex );
	BOOL UpdateData( BOOL flag );
	BOOL SetImageList( int nX, int nY, UINT uStartID, UINT uEndID );
	CDialog* GetPage(int nIndex);
	virtual ~CBaseTabCtrl();
	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseTabCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_)

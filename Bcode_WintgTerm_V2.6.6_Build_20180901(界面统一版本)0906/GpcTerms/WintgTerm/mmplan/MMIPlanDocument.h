#pragma once


// CMMIPlanDocument �ĵ�

class CMMIPlanDocument : public CDocument
{
	DECLARE_DYNCREATE(CMMIPlanDocument)

public:
	CMMIPlanDocument();
	virtual ~CMMIPlanDocument();
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};

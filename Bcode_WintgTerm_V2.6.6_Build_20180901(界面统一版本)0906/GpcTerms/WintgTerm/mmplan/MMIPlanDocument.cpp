// MMIPlanDocument.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "MMIPlanDocument.h"


// CMMIPlanDocument

IMPLEMENT_DYNCREATE(CMMIPlanDocument, CDocument)

CMMIPlanDocument::CMMIPlanDocument()
{
}

BOOL CMMIPlanDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CMMIPlanDocument::~CMMIPlanDocument()
{
}


BEGIN_MESSAGE_MAP(CMMIPlanDocument, CDocument)
END_MESSAGE_MAP()


// CMMIPlanDocument ���

#ifdef _DEBUG
void CMMIPlanDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CMMIPlanDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMMIPlanDocument ���л�

void CMMIPlanDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CMMIPlanDocument ����

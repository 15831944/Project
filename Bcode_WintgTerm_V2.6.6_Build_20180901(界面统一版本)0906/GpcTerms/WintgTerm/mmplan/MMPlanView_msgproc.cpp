#include "stdafx.h"
#include "resource.h"
#include "MMPlanView.h"
#include "comutil.h"
#include "MMIPlanFrame.h"
#include "utility/xml_utility.h"
#include "WSMaintanPlan.h"
#include "DlgSimpleInput.h"
#include "tgview/tgdocument.h"
#include "DlgGridInput.h"
#include "WebReference.h"
#include "tgpspdata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#import  <msxml.dll> named_guids
using namespace MSXML;




BOOL  CMMPlanView::WriteMPLANFile(const char *planid,const char *node,const char *xml)
{
	 CString  filenameprefix;
	 filenameprefix.Format("MMPLAN-%s",planid);
	 return WriteXMLFile(xml,node,filenameprefix);
}

BOOL  CMMPlanView::ReadMPLANFile(CString &xml,const char *planid,const char *node)
{
	 CString  filenameprefix;
	 filenameprefix.Format("MMPLAN-%s",planid);
	 return ReadXMLFile(xml,node,filenameprefix);
}
BOOL   CMMPlanView::ProcessXML_Msg_List(const char *xml,const char *node)
{
	 CString  plan_id;
	 
	 CTD_MPLANMARK obj;
	 obj.LoadFromXML(xml,node);
	 int index=AddMMPlan(obj.PLANID,TRUE);
	 if(index<0)
		 {
			 AfxMessageBox("ʩ���ƻ�̫���޷�����");
			 return FALSE;
	     }
	 if(m_cMMplans[index]->m_bUpdatedFromMsg==FALSE)
			 return TRUE;  // �Ѿ�����
	  m_cMMplans[index]->m_mark.LoadFromXML(xml,node); // �����һ�Σ�
      WriteMPLANFile(obj.PLANID,XMLTYPE_MPLAN_MARK,xml);
	  return TRUE;
}

BOOL   CMMPlanView::ProcessXML_Msg_Info(const char *xml,const char *node)
{
		 CString nodestr;
		 CString  plan_id;
		 nodestr.Format("%s/%s",node,"PLANID");
		 if(!GetXMLNodeValue(plan_id,xml,nodestr))
			 return FALSE;
		 int index=FindMMPlan(plan_id);
		 if(index>=0)
		 {
			m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		    WriteMPLANFile(plan_id,XMLTYPE_MPLAN_INFO,xml);
		 }
		 return TRUE;
}

BOOL   CMMPlanView::ProcessXML_Msg(const char *xml,const char *node)
{
	 CString  info;
	 
	 if(!stricmp(node,XMLTYPE_MPLAN_MARK))
		 return ProcessXML_Msg_Info(xml,node);
	 else
	 if(!stricmp(node,XMLTYPE_MPLAN_INFO))
	 {
		 return ProcessXML_Msg_List(xml,node);
	 }
	 else
	 {
		AfxMessageBox("�޷�ʶ�����Ϣ");
		return FALSE;
	 }
	 return TRUE;
}

#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				dest = (const char *)pNode->Gettext(); \
			} \
			else dest.Empty();};

extern BOOL  InitMarkFromXML(MSXML::IXMLDOMNodePtr pNodeRoot,CTD_MPLANMARK *pobj);
void CMMPlanView::ProcessXML_WS_List(const char *xml,const char *node)
 {
	  MSXML::IXMLDOMDocumentPtr pXMLDoc;
	  HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	  if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			return;
		}
	  
	  if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
		{
			AfxMessageBox("Web���񷵻ص�XML��ʽ����");
			return;
		} 
		
	   MSXML::IXMLDOMNodeListPtr pRecAbsList = pXMLDoc->selectNodes(node);//ds_MaintainPlan_List/
	   int rec_count = pRecAbsList->Getlength();
	   for(int i = 0; i <rec_count && m_nCountOfPlan<MAX_MMPLAN_NUM ; i++)
	    {
			CString  guid;
			GET_NODE_STRVALUE(guid,pRecAbsList->Getitem(i),"ID")
			if(guid.IsEmpty())
			{
				AfxMessageBox("WS���صĽ����û��GUID");
				continue;
			}
		 	MSXML::IXMLDOMNodePtr pNodeXml;
			CString  marknode;
			marknode.Format("TITLE/%s",XMLTYPE_MPLAN_MARK);
			pNodeXml = pRecAbsList->Getitem(i)->selectSingleNode(_bstr_t(marknode)); // ������Ϣ�Ƿ���TITLE֮�µ�
			if(pNodeXml==NULL)
				continue;
			CMMPlanListInfo *pObj=new CMMPlanListInfo;
			InitMarkFromXML(pNodeXml,&pObj->m_mark);
			pObj->m_id=pObj->m_mark.PLANID;
			pObj->m_guid =guid;
			pObj->m_bUpdatedFromMsg=FALSE;
			
			m_cMMplans[m_nCountOfPlan++]=pObj;
		}
	 ShowMPlanList();
 }

// TgDocument.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sysshare.h"
#include "TgDocument.h"
#include "function.h"
#include "icommtask.h"
#include "schd_newserver.h"
#include "utility/xml_utility.h"
#include "td/td_frame.h"
#include "td/td_data_def.h"
#include "wsdcmd.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

CPlanInformListItem *CTgDocument::GetPlanInformByID(DWORD id)
{
    list<CPlanInformListItem>::iterator iter=m_listPlanInform.begin();
	while(iter!=m_listPlanInform.end())
	{
		if(iter->m_id==id)
			 return &*iter;
		iter++;
	}
	return NULL;
}

BOOL   CTgDocument::ProcessXML_PlanInform(const char *xml,const char *node)
{
	list<CPlanInformListItem>::iterator iter=m_listPlanInform.begin();
	DWORD avaid=time(NULL);
	// 20090710 ���ж��ظ��ƻ���
	while(iter!=m_listPlanInform.end())
	{
		if(iter->m_id>avaid)
			avaid=iter->m_id+1;
		iter++;
	}
	if(iter==m_listPlanInform.end()) // �¼ƻ�
	{
		CPlanInformListItem item;
		item.m_id=avaid;
		item.m_token=node;
		item.m_content=xml;
		item.m_recvtime=time(NULL);
		if(stricmp(node,TOKEN_SS_PLAN_INFORM))
			item.m_signtime=0;
		else
			item.m_signtime=item.m_recvtime;
		m_listPlanInform.push_front(item);
	}
	//GpcLogTrainMessage(0, 0, "%s", xml);
	XMLInform inform;
	inform.content=xml;
	inform.token=node;
	UpdateAllViews(NULL,TGDATA_XML_INFORM,&inform);
	return TRUE;
}

BOOL   CTgDocument::ProcessXML_FTNMsg(const char *xml,const char *node)
{
	LOAD_XML(xml, node, pRootNode)
	
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);
	
	long seq,cnt,srccode,msgtm;
	CString jht,sttm,endtm;
	CString msgid;
	
	GET_NODE_STRVALUE(msgid,pRootNode,"MSGID")
	GET_NODE_INTVALUE(srccode,pRootNode,"SRCCODE")
	GET_NODE_STRVALUE(jht,pRootNode,"JHTNM")
	GET_NODE_INTVALUE(msgtm,pRootNode,"RECVTM")

	GET_NODE_STRVALUE(sttm,pRootNode,"STTM")
	GET_NODE_STRVALUE(endtm,pRootNode,"EDTM")
	GET_NODE_INTVALUE(cnt,pRootNode,"COUNT")
	GET_NODE_INTVALUE(seq,pRootNode,"SEQ")

	CString tmp;
	if (seq==0)
	{
		m_strRecvMsg="";
		m_nRecvMsgId = msgtm;
		m_nRecvMsgCnt = cnt;

		tmp="";
		tmp+="<";tmp+=node;tmp+=">";

		InsertXMLNodeValue(tmp,"MSGID",msgid);
		InsertXMLNodeValue(tmp,"SRCCODE",srccode);	
		InsertXMLNodeValue(tmp,"JHTNM",jht);

		InsertXMLNodeValue(tmp,"STTM",sttm);
		InsertXMLNodeValue(tmp,"EDTM",endtm);
		InsertXMLNodeValue(tmp,"COUNT",cnt);
		
		m_strRecvMsg += tmp;
	}


	MSXML::IXMLDOMNodePtr pTrain = pRootNode->selectSingleNode("ST_TRAIN");
	if (pTrain)
	{
		tmp="";
		if (m_nRecvMsgId==msgtm)
		{
			tmp.Format("%s", LPCSTR(pTrain->xml));
			m_strRecvMsg += tmp;
		}
	}
	
	if (seq==m_nRecvMsgCnt-1)
	{
		tmp="";
		tmp+="</";tmp+=node;tmp+=">";
		m_strRecvMsg += tmp;
		
		m_nRecvMsgId = 0;
		m_nRecvMsgCnt = 0;

		ProcessXML_PlanInform(m_strRecvMsg, node);
		int len = m_strRecvMsg.GetLength();
		//GpcLogTrainMessage(0,0,"�յ��׶μƻ���Ϣ %s", m_strRecvMsg);

		m_strRecvMsg="";
	}

	return TRUE;
}

BOOL CTgDocument::ProcessXML_KDPlanMsg(const char *xml,const char *node)
{
	GpcLogTrainMessage(0,0,"����͵�����conid %d token: %s", m_nCurrentSectionNo, node);
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		GpcLogTrainMessage(0,0,"û�п���TD5.0���ܲ�����͵�����");
		return TRUE;
	}

	if(0!=stricmp(node,TOKEN_25_AD_PLAN_INFORM))
		return TRUE;

	LOAD_XML(xml, node, pRootNode)
	
	long seq,cnt;
		
	GET_NODE_INTVALUE(cnt,pRootNode,"COUNT")
	GET_NODE_INTVALUE(seq,pRootNode,"SEQ")

	CString tmp;
	if (seq==1)
	{
		m_strRecvMsg="";
		m_nRecvMsgCnt = cnt;

		tmp="";
		tmp+="<";tmp+=node;tmp+=">";
		InsertXMLNodeValue(tmp,"COUNT",cnt);
		InsertXMLNodeValue(tmp,"SEQ",cnt);
		m_strRecvMsg += tmp;
	}

	MSXML::IXMLDOMNodePtr pTrain = pRootNode->selectSingleNode("ST_TRAIN");
	if (pTrain)
	{
		tmp="";
		tmp.Format("%s", LPCSTR(pTrain->xml));
		m_strRecvMsg += tmp;
	}
		
	if (seq==m_nRecvMsgCnt)
	{
		tmp="";
		tmp+="</";tmp+=node;tmp+=">";
		m_strRecvMsg += tmp;
		
		m_nRecvMsgId = 0;
		m_nRecvMsgCnt = 0;

		ProcessXML_PlanInform(m_strRecvMsg, node);
		int len = m_strRecvMsg.GetLength();
		GpcLogTrainMessage(0,0,"�յ��͵��׶μƻ���Ϣ %s", m_strRecvMsg);

		m_strRecvMsg="";
	}
	return TRUE;
}

BOOL   CTgDocument::RequestCrewInform(std::vector<ULONG>& crewgid) // ���������Ϣ
{
	if(crewgid.size()<=0)
	{
		GpcLogTrainMessage(0,0,"����Աû��ѡ�����������Ϣ");
		return TRUE;
	}
	CString token="REQ_CREW_INFO";
	CString dest="<REQ_CREW_INFO>", tmp;
	dest+="<GID>";
	for(int i=0; i<crewgid.size(); i++)
	{
		tmp.Format("%d,", crewgid[i]);
		dest+=tmp;
	}
	dest+="</GID>";
	dest+="</REQ_CREW_INFO>";

	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	GpcLogTrainMessage(0,0,"����Ա���ݳ�����Ϣ֪ͨ,��TD(%d)���� ������Ϣ: %s", eid, token, dest);
	TGSendStringMessage(token,dest,eid);

	return TRUE;
}

// MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
BOOL   CTgDocument::RequestTrainExpand_KD_YL_CREW_FromOPMS(TRAIN_INDEX GTID, int MSGID)
{
	CString token,tmp,xml;
	Archive ar;
	
	token="REQ_KEDIAO_INFO";
	xml="<REQ_KEDIAO_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_KEDIAO_INFO>";

	
	GpcLogTrainMessage(0,0,"��TD���� �͵���Ϣtoken:%s XML:%s", token, xml);
	CLBTH::MsgStringMessage msg;
	msg.SetTokenContent(token,xml);
	ar << msg;

	token="REQ_YINGYE_INFO";
	xml="<REQ_YINGYE_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_YINGYE_INFO>";

	GpcLogTrainMessage(0,0,"��TD���� Ӫҵ��Ϣtoken:%s XML:%s", token, xml);
	msg.SetTokenContent(token,xml);
	ar << msg;

	token="REQ_CREW_INFO";
	xml="<REQ_CREW_INFO>";
	tmp.Format("<GID>%u</GID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_CREW_INFO>";

	GpcLogTrainMessage(0,0,"��TD���� ������Ϣtoken:%s XML:%s", token, xml);
	msg.SetTokenContent(token,xml);
	ar << msg;
	
	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	SendArMessage(eid,ar,0);

	return TRUE;
}

// MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
BOOL   CTgDocument::RequestTrainExpand_ENGINECREW_TICKE_FromOPMS(TRAIN_INDEX GTID, int MSGID, CString& TNO) 
{
	CString token,tmp,xml;
	Archive ar;
	
	token="REQ_PASSENGER_TICKET_INFO";
	xml="<REQ_PASSENGER_TICKET_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	tmp.Format("<TNO>%s</TNO>", TNO);
	xml+=tmp;
	xml+="</REQ_PASSENGER_TICKET_INFO>";
	
	GpcLogTrainMessage(0,0,"��TD���� ��Ʊ��Ϣtoken:%s XML:%s", token, xml);
	CLBTH::MsgStringMessage msg;
	msg.SetTokenContent(token,xml);
	ar << msg;

	token="REQ_ENGINE_CREW_INFO";
	xml="<REQ_ENGINE_CREW_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_ENGINE_CREW_INFO>";

	GpcLogTrainMessage(0,0,"��TD���� ������������Ϣtoken:%s XML:%s", token, xml);
	msg.SetTokenContent(token,xml);
	ar << msg;
	
	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	SendArMessage(eid,ar,0);
	return TRUE;
}

// MSGID�� 1. �հ�ͼ 2. ����ͼ 3.�����ƻ��Ի��� 4.��̨�ƻ��Ի��� 5.˦�ҶԻ���
BOOL   CTgDocument::RequestTrainExpand_KD_YL_FromOPMS(TRAIN_INDEX GTID, int MSGID)
{
	CString token,tmp,xml;
	Archive ar;
	
	token="REQ_KEDIAO_INFO";
	xml="<REQ_KEDIAO_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_KEDIAO_INFO>";

	
	GpcLogTrainMessage(0,0,"��TD���� �͵���Ϣtoken:%s XML:%s", token, xml);
	CLBTH::MsgStringMessage msg;
	msg.SetTokenContent(token,xml);
	ar << msg;

	token="REQ_YINGYE_INFO";
	xml="<REQ_YINGYE_INFO>";
	tmp.Format("<GTID>%u</GTID>", GTID);
	xml+=tmp;
	tmp.Format("<MSGID>%u</MSGID>", MSGID);
	xml+=tmp;
	xml+="</REQ_YINGYE_INFO>";

	GpcLogTrainMessage(0,0,"��TD���� Ӫҵ��Ϣtoken:%s XML:%s", token, xml);
	msg.SetTokenContent(token,xml);
	ar << msg;
	
	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	SendArMessage(eid,ar,0);

	return TRUE;
}


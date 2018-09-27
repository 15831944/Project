#include "stdafx.h"
#include "tg.h"
#include "sysshare.h"
#include "TgDocument.h"
#include "function.h"
#include "icommtask.h"
#include "utility/xml_utility.h"
#include "comutil.h"
#include "utility/ctc_data_type_def.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

extern DWORD AddToMemoryNotify(const char *desc,time_t begintime,int repeat,int type,DWORD objindex, int flag=1);

BOOL CTgDocument::ProcessXML_TYPlanMsg(const char *xml,const char *node)
{
	GpcLogTrainMessage(0,0,"����TDͨ�Ż�������������Ϣconid %d token: %s", m_nCurrentSectionNo, node);
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);

	if(!stricmp(node,TOKEN_23_ACK_SPEC_PLAN))
		return ProcessXML_Msg_TEYUN(xml,node);

	return TRUE;
}

BOOL CTgDocument::ProcessXML_Msg_TEYUN(const char *xml,const char *node)
{
	if(m_pTyWaitDlg==NULL)
	{
		CView* parent=((CWintg *)AfxGetApp())->GetTYPlanSignDlgParent();
		m_pTyWaitDlg=new CDlgTYMsg(this, parent);
		m_pTyWaitDlg->SetNotReady();
		m_pTyWaitDlg->ShowWindow(SW_SHOW);
		m_pTyWaitDlg->UpdateDisplay("�������˼ƻ�ǩ�նԻ���");
	}

	if(m_pTyWaitDlg==NULL)
	{
		GpcLogTrainMessage(0, 0, "m_pTyWaitDlg�Ի��򴴽�ʧ��!");
		return FALSE;
	}
	else if (!::IsWindow(m_pTyWaitDlg->m_hWnd))
	{
		GpcLogTrainMessage(0, 0, "m_pTyWaitDlgδ�����ɶԻ���,���˼ƻ�����ʧ��!");
		return FALSE;
	}

	m_pTyWaitDlg->SetDlgWindowFocus();

	BOOL rt=FALSE;
	CTD_SPECPLAN spplan;

	rt=spplan.LoadSPECPLAN(xml, node);
	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_TEYUN �������˼ƻ���XMLʧ��");
		AfxMessageBox("�������˼ƻ���XML����");
		return FALSE;
	}

	if(spplan.MSGID=="0")
	{
		GpcLogTrainMessage(0, 0, "��ϢidΪ:%s�ǿ�,���ܴ���", spplan.MSGID);
		return FALSE;
	}
	
	GpcLogTrainMessage(0,0, "�յ����˼ƻ�, SRCCODE:%d, MSGID:%d, COUNT:%d, JHTNM:%s, DATE:%d, SEQ:%d",
		spplan.SRCCODE, spplan.MSGID, spplan.COUNT, spplan.JHTNM, spplan.DATE, spplan.SEQ);
	if(!m_pTyWaitDlg->IsNotReady())
	{
		m_pTyWaitDlg->SetThisPacketInfo(spplan.SRCCODE, spplan.MSGID, spplan.COUNT, spplan.JHTNM);
		//TYPLAN-19700101
		DeletePrevTYPLANFile(spplan.DATE);
	}

	bool bRecieve=false;
	if(m_pTyWaitDlg->IsNotReady())
		bRecieve=m_pTyWaitDlg->AddSeq(spplan.SRCCODE, spplan.MSGID, spplan.COUNT, spplan.SEQ);

	if(bRecieve)
	{
		CString str;
		str.Format("�������˼ƻ���Ϣ:�ܹ�%d��, �յ� %d ��", spplan.COUNT, m_pTyWaitDlg->GetRecSeqCount());
		GpcLogTrainMessage(0,0,str);
		m_pTyWaitDlg->UpdateDisplay(str); 


		CString plan_xml;
		//spplan.SaveSPECPLANToXML(plan_xml,"ACK_SPEC_PLAN");
		spplan.SaveSPECPLANToFile(plan_xml,"ACK_SPEC_PLAN");
		if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
			WriteTYPLANFile(spplan.DATE, spplan.TRACKID,"ACK_SPEC_PLAN",plan_xml);
		else
			WriteTYPLANFile(spplan.DATE, spplan.SEQ,"ACK_SPEC_PLAN",plan_xml);
		
		// ֪ͨViewˢ����ʾ
		TYplanInform inform;
		inform.change_flag=TYplanInform::TYPLAN_LIST_CHANGED;
		inform.index=0;	
		inform.id=spplan.MSGID;
		UpdateAllViews(NULL,TGDATA_TYPLAN_UPDATED,&inform);

		time_t currtm = time(NULL);
		if (spplan.STTM>currtm)
		{
			str.Format("�뼰ʱ��ע�����г�:%s",spplan.TNO);
			AddToMemoryNotify(str, spplan.STTM-30*60,1,0,0);
		}
	}
	else
		GpcLogTrainMessage(0,0,"���˼ƻ�����,bRecieve=fasle");

	return TRUE;
}

BOOL  CTgDocument::WriteTYPLANFile(long date,int index,const char *node,const char *xml)
{
	CString  filenameprefix;
	CTime tm = CTime(date);
	CString tmp = tm.Format("%Y%m%d");
	filenameprefix.Format("TYPLAN-%s-%d",tmp,index);
	return WriteXMLFile(xml,node,filenameprefix);
}

BOOL  CTgDocument::ReadTYPLANFile(CString &xml,long date,int index, const char *node)
{
	CString  filenameprefix;
	filenameprefix.Format("TYPLAN-%d-%d",date,index);
	return ReadXMLFile(xml,node,filenameprefix);
}

void CTgDocument::DeletePrevTYPLANFile(long date)
{
	CString  filenameprefix;
	CTime tm = CTime(date);
	CString tmp = tm.Format("%Y%m%d");
	CString filepath=gpcdef.GetXMLDir();
	filenameprefix.Format("%s\\TYPLAN-%s-*.xml",filepath, tmp);
	
	CFileFind  fileFind;
	
	BOOL flag = fileFind.FindFile(filenameprefix);
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString tmp;
		tmp.Format("%s\\%s",filepath, fileFind.GetFileName());
		DeleteFile(tmp);
	}
}

int   CTgDocument::FindTYPlanByMsgID(int id)
{
	for(int i=0;i<mSpecPlanList.size();i++)
	{
		if (mSpecPlanList[i].m_id == id)
		{
			mSpecPlanList[i].m_nCount++;
			return i;
		}
	}
	return -1;
}

int   CTgDocument::AddTYPlan(int id, BOOL frommsg, DWORD tm)
{
	for(int i=0;i<mSpecPlanList.size();i++)
	{
		if(mSpecPlanList[i].m_id==id)
			return i;
	}

	CTYPlanListInfo info;
	info.m_id=id;
	info.m_nCount=1;
	info.m_nWorkTm=tm;
	mSpecPlanList.push_back(info);
	return (int)(mSpecPlanList.size()-1);
}

void  CTgDocument::ClearAllTYPlan()
{
	mSpecPlanList.clear();
}

BOOL  CTgDocument::LoadTYListFromFile()
{
	TYplanInform inform;
	inform.change_flag=TYplanInform::TYPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(NULL,TGDATA_TYPLAN_UPDATED,&inform);
	return TRUE;
}

BOOL CTgDocument::ProcessXML_Msg_TEYUN_Update(const char *xml,const char *node)
{
	GpcLogTrainMessage(0,0,"�������˸�����Ϣconid %d token: %s", m_nCurrentSectionNo, node);
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);

	if(!GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		GpcLogTrainMessage(0,0,"û�п���TD5.0���ܲ��������˸���");
		return TRUE; // TD5.0���ܴ������
	}

	if(m_pTyWaitDlg==NULL)
	{
		CView* parent=((CWintg *)AfxGetApp())->GetTYPlanSignDlgParent();
		m_pTyWaitDlg=new CDlgTYMsg(this, parent);
		m_pTyWaitDlg->SetNotReady();
		m_pTyWaitDlg->ShowWindow(SW_SHOW);
		m_pTyWaitDlg->UpdateDisplay("�������˼ƻ�����ǩ�նԻ���");
	}

	if(m_pTyWaitDlg==NULL)
	{
		GpcLogTrainMessage(0, 0, "m_pTyWaitDlg�Ի��򴴽�ʧ��!");
		return FALSE;
	}
	else if (!::IsWindow(m_pTyWaitDlg->m_hWnd))
	{
		GpcLogTrainMessage(0, 0, "m_pTyWaitDlgδ�����ɶԻ���,���˼ƻ����´���ʧ��!");
		return FALSE;
	}

	m_pTyWaitDlg->SetDlgWindowFocus();

	BOOL rt=FALSE;
	CTD_SPECPLAN spplan;

	rt=spplan.LoadSPECPLAN(xml, node);
	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "�������˼ƻ���XMLʧ��");
		AfxMessageBox("�������˸��¼ƻ���XML����");
		return FALSE;
	}

	if(spplan.MSGID=="0")
	{
		GpcLogTrainMessage(0, 0, "��ϢidΪ:%s�ǿ�,���ܴ���", spplan.MSGID);
		return FALSE;
	}
	
	GpcLogTrainMessage(0,0, "�յ����˸��¼ƻ�, SRCCODE:%d, MSGID:%d, COUNT:%d, JHTNM:%s, DATE:%d, SEQ:%d",
		spplan.SRCCODE, spplan.MSGID, spplan.COUNT, spplan.JHTNM, spplan.DATE, spplan.SEQ);
	
	CString str;
	str.Format("�������˸��¼ƻ���Ϣ:��%d��", spplan.COUNT);
	GpcLogTrainMessage(0,0,str);
	m_pTyWaitDlg->UpdateDisplay(str); 

	CString plan_xml;
	spplan.SaveSPECPLANToFile(plan_xml,"ACK_SPEC_PLAN");
	WriteTYPLANFile(spplan.DATE, spplan.TRACKID,"ACK_SPEC_PLAN",plan_xml);
	
	// ֪ͨViewˢ����ʾ
	TYplanInform inform;
	inform.change_flag=TYplanInform::TYPLAN_LIST_CHANGED;
	inform.index=0;	
	inform.id=spplan.MSGID;
	UpdateAllViews(NULL,TGDATA_TYPLAN_UPDATED,&inform);

	time_t currtm = time(NULL);
	if (spplan.STTM>currtm)
	{
		str.Format("�뼰ʱ��ע�����г�:%s",spplan.TNO);
		AddToMemoryNotify(str, spplan.STTM-30*60,1,0,0);
	}
	
	return TRUE;
}

BOOL  CTgDocument::RequestSPECFromCAD(time_t start,time_t end)
{
	if(start==0)
		start=m_tMMplanStartTime;
	if(end==0)
		end=m_tMMplanEndTime;

	CTime cst(start);
	CTime cet(end);
	GpcLogTrainMessage(0, 0, "��ȡ���˼ƻ�ʱ�䷶Χ %s -- %s", cst.Format("%Y-%m-%d %H:%M:%S"), cet.Format("%Y-%m-%d 23:59:59"));

	CString token="REQ_SPEC_PLAN";
	CString dest="<REQ_SPEC_PLAN>";
	InsertXMLNodeValue(dest,"CONID",GetCurrentSectionNo());
	InsertXMLNodeTime(dest,"TM",cst.GetTime());
	dest+="</REQ_SPEC_PLAN>";

	long eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 
	TGSendStringMessage(token,dest,eid);
	GpcLogTrainMessage(0, 0, "��CAD(%d)���� %s %s", eid, token, dest);

	if(cst.GetDay()!=cet.GetDay())
	{
		token="REQ_SPEC_PLAN";
		dest="<REQ_SPEC_PLAN>";
		InsertXMLNodeValue(dest,"CONID",GetCurrentSectionNo());
		InsertXMLNodeTime(dest,"TM", cet.GetTime());
		dest+="</REQ_SPEC_PLAN>";

		TGSendStringMessage(token,dest,eid);
		GpcLogTrainMessage(0, 0, "��CAD(%d)���� %s %s", eid, token, dest);
	}

	TYplanInform inform;
	inform.change_flag=TYplanInform::TYPLAN_LIST_CHANGED;
	inform.index=-1;
	UpdateAllViews(NULL,TGDATA_TYPLAN_UPDATED,&inform);
	return TRUE;
}

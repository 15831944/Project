#include "stdafx.h"
#include  "sysincl.h"
#include  "icommtask.h"
#include "ComWindow.h"
#include "tg_function_config.h"

#include <memshare.h>
#pragma comment(lib,"version.lib")

CComWindow g_cMainComWindow;
bool       g_inited;

long  g_lServerEntityID=0;
long  g_lLocalEntityID=0;

bool  InitComTask(long local_eid, const char *configfile, short nMSSPort) // 20150703 nMSSPortӦ�е������ĺ�������,��WintgTerm��LayoutTerm�ֱ������Լ��ļ�ض˿�
{
	g_cMainComWindow.CreateEx(0,			  
		AfxRegisterWndClass(NULL),
		"ͨ�ż�ش���",
		WS_OVERLAPPED|WS_CAPTION|WS_BORDER,
		0,0,480,210,
		AfxGetMainWnd()->GetSafeHwnd(),NULL,NULL);
	
	g_cMainComWindow.m_cUniComm.SetMainConfigFile(configfile);  
	g_cMainComWindow.m_cUniComm.SetCommTaskConfigFile(configfile); 
	g_cMainComWindow.m_cUniComm.SetLocalEntityID(local_eid);
	
	if (nMSSPort>0)
	{
		try
		{
			SetMSCommPortInit(nMSSPort);
			GpcLogSysMessage(0,"�ۺ�ά�����ܴ�, �˿ں�Ϊ %d", nMSSPort);
		}
		catch(...)
		{
			GpcLogSysMessage(0,"[Error] unicomm.ocx ��֧�� �ۺ�ά������");
			gpcdef.SetDisableMSSAlarmPort(); // �رն˿�
		}
	}

	g_inited=true;  // ���������ǿ���start �п��ܻص�AddRela  2009.01.20
	g_cMainComWindow.m_cUniComm.Start();
	if(g_lServerEntityID>0)
		g_cMainComWindow.m_cUniComm.SetRelateEntityID(g_lServerEntityID); // ������Start() �Ժ����,������STATUS==254ʱ����
	g_lLocalEntityID=local_eid;

	return g_inited;
}

bool EndComTask()
{
	if(g_inited)
	{ 
		GpcLogSysMessage(0,"Unicommͨ��������ֹ.");
		Sleep(200);
		g_cMainComWindow.m_cUniComm.Stop(); 
		g_cMainComWindow.DestroyWindow(); 
		g_inited=false;
	}
	return true;
}

bool RestartComTask()
{
	if(!g_inited)
		return false;
	g_cMainComWindow.m_cUniComm.Stop();
	g_cMainComWindow.m_cUniComm.Start();
	return true;
}

bool StopComTask(short restart_delay)
{
	if(!g_inited)
		return false;
	g_cMainComWindow.m_cUniComm.StopT(restart_delay);
	return true;
}

//Ĭ���Զ�LABEL,�Ա���ID����
bool  SendCommMessage(long dest_id,short flag, BYTE *msgbuf,int msglen) 
{
	if(!g_inited)
		return false;

	BYTE *pdata=NULL;

	if(dest_id==0)
		dest_id=g_lServerEntityID;

	COleSafeArray byte_array;
	SAFEARRAYBOUND rgsabounds[1];
	rgsabounds[0].cElements=msglen; // Ԫ�ظ���
	rgsabounds[0].lLbound =0;  // �����0��ʼ
	byte_array.Create(VT_I1, 1, rgsabounds); // 1ά

	byte_array.AccessData((LPVOID*)&pdata);

	if(pdata)
	{
		memcpy(pdata,msgbuf,msglen);
	}
	byte_array.UnaccessData();

	if(g_cMainComWindow.m_cUniComm.PostMessage(0,dest_id,0,flag,byte_array)) //Ĭ���Զ�LABEL,�Ա���ID����
		return false;
	else
		return true;

}


bool  PostLogMessage(int type,int pri,const char *msg)
{
	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return false;

	if(!g_inited)
		return false;
	if(g_cMainComWindow.m_cUniComm.LogMessage(type,pri,g_lLocalEntityID,msg))
		return false;
	else
		return true;
}

// AddReleaEntityӦ����Start�Ժ����CommStatusΪ254ʱ����!!  // 2009.1.22
bool  AddReleaEntity(int entity_id)
{
	if(!g_inited)
		return false;
	return  g_cMainComWindow.m_cUniComm.SetRelateEntityID(entity_id);
}

bool  IsEntityActive(int entity_id)
{
	if(!g_inited)
		return false;
	return g_cMainComWindow.m_cUniComm.GetRelateEntityStatus(entity_id);
}

bool IsCommConnected()
{
	if(!g_inited)
		return false;
	if(g_cMainComWindow.m_cUniComm.GetCommStatus()>=2)
		return true;
	else
		return false;
}

int GetCommTaskStatus()
{
	if(!g_inited)
		return -1;
	else
		return g_cMainComWindow.m_cUniComm.GetCommStatus();
}

static bool g_bCommWindowVisible=false;
bool  IsCommWindowVisible()
{
	return g_bCommWindowVisible;
}

void  ShowCommWindow(bool visible)
{

	if(!g_inited)
		return;
	if(!g_bCommWindowVisible)
	{
		g_bCommWindowVisible=true;
		g_cMainComWindow.CenterWindow(); 
		g_cMainComWindow.ShowWindow(SW_SHOW);
	}
	else
	{
		g_bCommWindowVisible=false;;
		g_cMainComWindow.ShowWindow(SW_HIDE);
	}

}

int  GetChineseChar(BYTE* logbuf, int len)
{
	int flag=0;
	for(int i=0; i<len; i++)
	{
		BYTE ch = logbuf[i];
		if(ch>=128) // ����
		{
			flag++;
		}
	}

	// ��������
	if(flag%2 == 0)
		return len;
	else
		return len-1;
}

void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...)
{
	static char logbuf[81920];
	memset(logbuf, 0, sizeof(logbuf));

	va_list va;
	va_start(va,fmt);
	vsnprintf(logbuf,sizeof(logbuf)-1,fmt,va);
	va_end(va);

	char tmp[256];

	int nOffset = 0;
	int nLogLen = min(strlen(logbuf), sizeof(logbuf));
	while(nLogLen > nOffset)
	{
		memset(tmp, 0, sizeof(tmp));
		if(nLogLen-nOffset > 240)
		{
			int len = GetChineseChar((BYTE*)logbuf+nOffset, 240);
			memcpy(tmp, logbuf+nOffset, len);
			nOffset += len;
		}
		else
		{
			memcpy(tmp, logbuf+nOffset, nLogLen-nOffset);
			nOffset = nLogLen;
		}

		tmp[255]=0;
	
		sysprint(10,process_id,tmp);
	
		if(g_inited && GetTGFunctionConfig()->IsSendLogToLogsvr())
		{
			g_cMainComWindow.m_cUniComm.LogMessage(2,10,g_lLocalEntityID,tmp);
		}
	}
}

void GpcLogSysMessage(short process_id, const char *fmt, ...)
{
	char tmp[256];
	va_list va;
	va_start(va,fmt);
	vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[255]=0;

	sysprint(10,process_id,tmp);

	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return;

	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.LogMessage(1,10,g_lLocalEntityID, tmp);

}

void GpcLogDebugMessage(short process_id, const char *fmt, ...)
{
	char tmp[256];
	va_list va;
	va_start(va,fmt);
	vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[255]=0;

	sysprint(10,process_id,tmp);

	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return;

	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.LogMessage(0,10,g_lLocalEntityID, tmp);
}

// Ĭ�ϲ�����ŷ���
bool SendBMsg(CLBTH::BaseMsg* pMsgs[], int num,long dest_entity,short flag)
{
	if(dest_entity==0)
		dest_entity=g_lServerEntityID;

	static char msgbuf[80960];
	int rs = -1;
	int i = 0;
	int data_len;
	while(i < num)
	{
		CLBTH::Archive ar(CLBTH::Archive::store, sizeof(msgbuf), msgbuf);
		CLBTH::HRCODE hr;
		for(; i<num; i++)
		{
			hr = (*pMsgs)->serialize(ar);
			if(hr.failed())
			{
				if(hr.sys_code != CLBTH::HRCODE::E_WRITE_OUTOFBUFFER)
					return false;
				else
					break;
			}
			pMsgs ++;
		}
		data_len = ar.getCurrentPos();
		SendCommMessage(dest_entity,flag,(BYTE *)msgbuf,data_len);
		//	wfp.Write(msgbuf, data_len);
		//GpcLogTrainMessage(0, 0, "��ʵ�� %d ���� %d�ֽ�����:", dest_entity, data_len);
		//CString tmp, log;
		//for(int i=0; i<data_len; i++)
		//{
		//	tmp.Format("%0.2X ", (BYTE)msgbuf[i]);
		//	log += tmp;
		//}
		//GpcLogTrainMessage(0, 0, "%s", log);
	}
	return true;
}

bool SendBMsg(CLBTH::BaseMsg* pOneMsg,long dest_entity,short  flag)
{
	CLBTH::BaseMsg* pMsgs[1];
	pMsgs[0]=pOneMsg;
	return SendBMsg(pMsgs,1,dest_entity,flag);
}

bool SendStringMessage(const char *token,const char *content,long eid)
{
	CLBTH::MsgStringMessage msg;
	msg.SetTokenContent(token,content);
	return SendBMsg(&msg,eid,0);
}
int  get_max_comm_length(long entity)
{
	return 4000;//20130428 �¼ƻ��ְ���С ԭֵΪ4096
}
// ��������ӷ�������ȡ
bool  SendArMessage(long entity, CLBTH::Archive &ar,int flag)
{
	return SendCommMessage(entity,flag,ar.getBuffer(),ar.getCurrentPos());
}

bool CheckSendMessage(long entity, CLBTH::Archive &ar, short next_msglen,int flag)
{
	short max=get_max_comm_length(entity);
	short cur=ar.getCurrentPos();

	if (max < cur) 
	{
		//task.ErrLog("fatal err in relay message %d %d to e%d", max, cur, entity);
		//return false;
	}

	bool rs=TRUE;
	if (max < cur+next_msglen) 
	{
		if (!SendArMessage(entity,ar,flag))
			rs = false;
		ar.seek(CLBTH::Archive::BEGIN, 0);
	}
	return rs;
}
bool  CheckSendMessage(long entity, CLBTH::Archive &ar, const CLBTH::BaseMsg &msg,int flag)
{
	CLBTH::BaseMsg &t = (CLBTH::BaseMsg &)msg;
	size_t len = t.getObjectLength();
	/*
	if (SUCCESS == CONFIG.IfSupportMessageCheckLength()) 
	{
	Archive lar;
	lar << msg;
	if (len != lar.getCurrentPos()) 
	{
	task.ErrLog("construct msg %d err write_len %d cal %d", msg.getTypeIDVirtual(), lar.getCurrentPos(), len);
	len = lar.getCurrentPos();
	}
	}
	*/
	return CheckSendMessage(entity, ar, len,flag);
}


void SetDefaultServerEntityID(long eid)
{
	g_lServerEntityID=eid;
}


void  SetRelaEntityIDForStatusDetecting(int *pArray,int count)
{
	if(!g_inited)
		return;
	//g_cMainComWindow.m_cUniComm.ResetRelateEntityID();
	for(int i=0;i<count;i++)
	{
		g_cMainComWindow.m_cUniComm.SetRelateEntityID(*pArray++); 
	}
}

long  GetLocalEntityID()
{
	return g_lLocalEntityID;
}

void SetMSCommPortInit(short port)
{
	g_cMainComWindow.m_cUniComm.SetnMSPort(port);
}
//�������Է�Χ10-20�����У�
//10 ����ͼ�������ͬ��״̬ 11 ����ͼ��ocxͬ��״̬
//12 �հ�ƻ��������ͬ��״̬ 13 �հ�ƻ���ocxͬ��״̬
//14-20 Ԥ��
void SetMSCapacityInit()
{
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(MS_ERROR_STATUS,10,20);
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(EVENT_STATUS,0,1);
}
//��������������: 20130419 hjz
//10---svr_db
//11---svr_mdb
//12---svr_sdb
//13---svr_basic
//14---svr_shift
//15---svr_history
//16---tdcommsvr
//17-100---����svr_tg
void SetMSLinkInit(int Num)
{
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(LINK_STATUS,MS_LINK_STATUS_DB,MS_LINK_STATUS_TG+Num-1);
}
void SetMSCapacityAlarm(short AttribIndex, char* alarm, short flag)
{
	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.SetMSStatus(MS_ERROR_STATUS,0,AttribIndex,NULL,0,0,0,alarm,flag);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]���ۺ�ά��ϵͳ���ͱ�����Ϣ,����: %d, ״̬: %s",AttribIndex, alarm);
}

void SetMSLinkAlarm(short AttribIndex, unsigned long Value, short flag)
{
	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.SetMSStatus(LINK_STATUS,0,AttribIndex,NULL,Value,0,0,NULL,flag);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]���ۺ�ά��ϵͳ����������Ϣ,����: %d, ״̬: %d",AttribIndex,Value);
}

void SetMSVersionAlarm(LPCTSTR versionStatus)
{
	if(!g_inited)
		return;
	CString tmp;
	tmp.Format("version");
	
	g_cMainComWindow.m_cUniComm.SetMSStatus(EVENT_STATUS,0,0,LPCTSTR(tmp),0,0,0,versionStatus,0);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]���ۺ�ά��ϵͳ���Ͱ汾��Ϣ,�汾: %s",versionStatus);
}
int	AddTGSectionList(int nSectionNo,int nTgSectionNum,int *nTgSectionList, int max);
int  GetMSLinkAttribIndexByEntity(long entity)
{
	if (entity<=0)
		return 0;

	if (entity == GetSpecialEntityID(SERVER_TYPE_DB,0))
		return MS_LINK_STATUS_DB;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_DATASTORE,0))
		return MS_LINK_STATUS_MDB;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_PARA,0))
		return MS_LINK_STATUS_SDB;	
	else if (entity == GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0))
		return MS_LINK_STATUS_BASIC;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_SHIFT_TG,0))
		return MS_LINK_STATUS_SHIFT;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_HISTDATA,0))
		return MS_LINK_STATUS_HISTORY;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_TDCOMM,0))
		return MS_LINK_STATUS_TD;
	else
	{
		int nTgSectionNum=0;
		nTgSectionNum=gpcdef.GetSvrTgEntityCnt();
	
		for(int k=0; k<nTgSectionNum; k++)
		{
			if (entity==gpcdef.GetSvrTgEntityIDByIndex(k))
				return gpcdef.GetSvrTgAttributNoByIndex(k);
		}
	}

	return 0;
}

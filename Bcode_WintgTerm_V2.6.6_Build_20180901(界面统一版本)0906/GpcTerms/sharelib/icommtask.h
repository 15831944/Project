#pragma once
#include "../BaseMsg/clbth.h"
#include "defines.h"
#include <list>
using namespace std;
////////// COMMON  DATA DEFINITION ////////
typedef struct{
		long  source_id;
		int   msg_id;
		const CLBTH::BaseMsg *pmsg;
		short need_delete;
	}UNI_MESSAGE_STRUCT;

typedef struct{
	char user_code[24];
	int  job_id; ///�������ͣ��ƻ�Ա������
	int  gpc_id;  //Ψһ��,��usernameת������   
	int  con_id;  //����  
}USER_INFO_STRUCT;

enum ATTRIB_TYPE
{
	NULL_STATUS=0,
	STANDBY_STATUS=1,
	LINK_STATUS=2,
	CAPACITY_STATUS=3,
	QUEUE_STATUS=4,
	TIME_STATUS=5,
	MS_ERROR_STATUS=6,
	EVENT_STATUS=7,
	ALARM_INFO=10,
};

typedef struct{
	    int    msgid;
	    HWND   hwnd;
		int    flag; // post mode
}MSGPROCWNDSTRUCT;

extern list<MSGPROCWNDSTRUCT> msgProcWndList;

#define  FREE_UNI_MESSAGE_STRUCT(p) if(p->need_delete){delete p->pmsg; delete p;}
#define  UM_UNI_MESSAGE  (WM_USER+1000)
#define  UM_SECTION_SWITCH (WM_USER+1001)

typedef  bool (*DIRECT_MSG_HANDLER)(long entity_id, unsigned char *msgbuf,int buflen);
typedef  bool (*COMM_EVENT_HANDLER)(int evt_type,long entity_id, int status);
///////////////////////////////////////////
bool StopComTask(short restart_delay=0);   // ֹͣ����ͨ��, ��������������Զ�����ͨ�ţ�0���Զ�����
bool RestartComTask(); // ��������ͨ��
bool  InitComTask(long local_eid, const char *configfile, short nMSSPort); // 2015.07.03 nMSSPortΪ�ۺ�ά���˿�, WinTGTerm��LayoutTerm�ֱ�����
bool  EndComTask();
bool  SendCommMessage(long dest_id,long label,short flag,BYTE *msgbuf,int msglen);  // flag==1 ����ţ�flag==2��ҪӦ��
bool  SendCommMessage(long dest_id,short flag, BYTE *msgbuf,int msglen); // flag==1 ����ţ�flag==2��ҪӦ��
bool  SendXMLMessage(const char *content,long eid,long source_eid=0,short flag=0); // source_eid=0 ����ID,flag==1 ����ţ�flag==2��ҪӦ��
bool  PostLogMessage(int type,int pri,const char *msg);
bool  IsCommConnected();
int   GetCommTaskStatus();
void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...);
void GpcLogSysMessage(short process_id, const char *fmt, ...);
void GpcLogDebugMessage(short process_id, const char *fmt, ...);
void SaveLogMessageStr(const char *key,time_t tm,const char *str);
bool  SendBMsg(CLBTH::BaseMsg* pMsgs[], int num,long dest_entity,short flag=0); // Ĭ���Ա���ID,�Զ�LABEL,������ŷ���
bool  SendStringMessage(const char *token,const char *content,long eid);
bool SendBMsg(CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag=0); // Ĭ���Ա���ID,�Զ�LABEL,������ŷ���
bool  CheckSendMessage(long entity, CLBTH::Archive &ar, const CLBTH::BaseMsg &msg,int flag=0);  // �����ܷ����������
bool  SendArMessage(long entity, CLBTH::Archive &ar,int flag=0);  
bool  RegisterMsgProcHandleWnd(HWND hwnd,int msgid,int flag); // flag 0: send_message 1: post_message
bool  UnRegisterMsgProcHandleWnd(HWND hwnd,int msgid=-1);
bool  RegisterDirectMsgHandler(DIRECT_MSG_HANDLER p);
bool  UnRegisterDirectMsgHandler(DIRECT_MSG_HANDLER p);
bool  UnRegisterXMLDirectMsgHandler(DIRECT_MSG_HANDLER p);
bool  RegisterXMLDirectMsgHandler(DIRECT_MSG_HANDLER p);
bool  RegisterCommEventHandler(COMM_EVENT_HANDLER p);

void SetDefaultServerEntityID(long eid);
void SetCurrentUserInfo(const USER_INFO_STRUCT *pUserInfo);

void  ShowCommWindow(bool visible);

/// some special message //////////////////////
bool  SendMsgGpcAccessInfo(ushort uAccessStep);
bool  SendMsgGpcLogin();

bool  IsUserLogined();
void  SetUserLoginStatus(int  flag); //0: δ��¼ 1:��¼�ɹ� -1:��¼ʧ��(�޷��ظ�)
bool  IsCommWindowVisible();
void  SetRelaEntityIDForStatusDetecting(int *pArray,int count);
int   AutoLogin(const char *userid, const char *username, const char *password,int shiftid=0); // �Զ���¼,�����
void  SetLoginWindowOpenFlag(bool flag);
bool  IsLoginWindowOpened();

void  SetCommServerMode(bool flag); // ����Ϊͨ�ŷ�����ģʽ����ԭ����������Ӧ
bool  IsInCommServerMode(); // �Ƿ�ͨ�ŷ�����ģʽ����ԭ����������Ӧ
bool  AddReleaEntity(int entity_id);
bool  IsEntityActive(int entity_id);
long  GetLocalEntityID();
bool IsRtServer(int entityid);

#define  SERVER_TYPE_GEN         0    
#define  SERVER_TYPE_RT          1    
#define  SERVER_TYPE_TG          2
#define  SERVER_TYPE_PARA        3
#define  SERVER_TYPE_DATASTORE   4
#define  SERVER_TYPE_TDCOMM      5
#define  GPCTERM_TYPE_DCMD       6
#define  SERVER_TYPE_HISTDATA    7
#define  SERVER_TYPE_DB          8

#define  SERVER_TYPE_BASIC_TG    9
#define  SERVER_TYPE_SHIFT_TG    10
#define  SERVER_TYPE_GSMR        11
#define  SERVER_TYPE_PREDICT     12
#define  SERVER_TYPE_MMPLAN      13
#define  SERVER_TYPE_ALARM       14//SVR_ALARM
#define  SERVER_TYPE_ROUTE       15//SVR_ROUTE
#define  SERVER_TYPE_MGATE       20
#define  SERVER_TYPE_GATEWAY	 25

long  GetSpecialEntityID(int type,int con_id=0);  //�������ͻ�ȡ��ͬ��EID��con_id������GPC��(����GPCTERM_TYPE_DCMD)
void  SetMSCapacityInit();//�����ۺ�ά��ϵͳ��������״̬�ĳ�ʼ��
void  SetMSCapacityAlarm(short AttribIndex, char* alarm, short flag);//���ۺ�ά��ϵͳ���汨����Ϣ
void  SetMSVersionAlarm(LPCTSTR versionStatus); //���ۺ�ά��ϵͳ�������汾��
void  SetMSLinkInit(int Num); //�����ۺ�ά��ϵͳ��������״̬�ĳ�ʼ��
void  SetMSLinkAlarm(short AttribIndex, unsigned long Value, short flag); //���ۺ�ά��ϵͳ��������״̬
int  GetMSLinkAttribIndexByEntity(long entity);
void SetMSCommPortInit(short port);
//����״̬������
//11---svr_mdb
//12---svr_sdb
//13---svr_basic
//14---svr_shift
//15---svr_history
//16---tdcommsvr
//17-100---svr_tg
#define MS_LINK_STATUS_DB		10
#define MS_LINK_STATUS_MDB		11
#define MS_LINK_STATUS_SDB		12
#define MS_LINK_STATUS_BASIC	13
#define MS_LINK_STATUS_SHIFT	14
#define MS_LINK_STATUS_HISTORY  15
#define MS_LINK_STATUS_TD		16
#define MS_LINK_STATUS_TG		17
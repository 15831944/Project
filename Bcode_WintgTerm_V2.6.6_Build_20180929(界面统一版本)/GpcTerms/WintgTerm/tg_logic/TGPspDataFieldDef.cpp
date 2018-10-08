// MyData.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright ?1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGPspData.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "utility/ctc_data_type_def.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ʩ���ƻ�����ԭʼ��Ϣ
PropertyInfoPara g_mmplan_info[]=
{
	{0,0,CTC_DATA_STR,"PLANID","ID"},
	{2,ID_PD_ST_MARK_TYPE,CTC_DATA_MARK_TYPE,"TYPE","��������"},
	//{5,ID_PD_ST_MARK_DATE,CTC_DATA_DATETIME,"TM","����"},
	{4,ID_PD_ST_MARK_START_TIME,CTC_DATA_DATETIME,"STTM","��ʼʱ��"},
	{4,ID_PD_ST_MARK_END_TIME,CTC_DATA_DATETIME,"EDTM","����ʱ��"},
	{2,ID_PD_ST_MARK_STATION1,CTC_DATA_STR,"STST","��ʼվ"},
	{2,ID_PD_ST_MARK_STATION2,CTC_DATA_STR,"EDST","����վ"},
	{2,ID_PD_ST_MARK_TEXT,CTC_DATA_STR,"text","��ע"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"STKM","��ʼλ��(Km)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"STM","��ʼλ��(m)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"ETKM","����λ��(Km)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"ETM","����λ��(m)"},
	{2,ID_PD_ST_MARK_SLOWTIME,CTC_DATA_INT,"OTHER","����"},
///////////////////////////////////////////////////////////////////////////
	
	{0,0,CTC_DATA_STR,"ID","�ƻ�ID"},
	{0,0,CTC_DATA_STR,"FLAG","����"},  // 2 ʩ��  3 ά��
	{0,0,CTC_DATA_STR,"JHH","�ƻ�ID"}, 
	{0,0,CTC_DATA_STR,"DBH","�籨��"}, 
	{0,0,CTC_DATA_STR,"MID","�¼ƻ�ID"},
	{0,0,CTC_DATA_STR,"WORKPOS","ʩ���ص�"},
	{0,0,CTC_DATA_STR,"LINENM","ʩ����·"},
	{0,0,CTC_DATA_DIRECTION,"DIR","������"}, // 0 ��,1�� ,2����, 3����
	{0,0,CTC_DATA_STR,"WORKKM","ʩ����ʼ���"},
	
	//{2,0,CTC_DATA_DATETIME,"STTM","��ʼʱ��"},
	//{2,0,CTC_DATA_DATETIME,"EDTM","����ʱ��"},

	{2,0,CTC_DATA_STR,"WORKTP","ʩ������"},
	{2,0,CTC_DATA_STR,"PJNM","ʩ����Ŀ"},

	{0,0,CTC_DATA_STR,"UNITNM","ʩ����λ"},
	{0,0,CTC_DATA_STR,"LEADER","ʩ��������"},

	{0,0,CTC_DATA_STR,"RANK","ʩ���ȼ�"},
	{0,0,CTC_DATA_STR,"WKCON","ʩ������"},
	{0,0,CTC_DATA_STR,"SPLIM","��������"},
	{0,0,CTC_DATA_STR,"LIMINFO","�г����ƿ�(�����)"}
};
int    count_of_mark_def()
{
    return 12;  // ע��,����ֵ������������ֱ�ӹ�ϵ
}
int    count_of_g_mmplan_info()
{
    return  sizeof(g_mmplan_info)/sizeof(g_mmplan_info[0]);
}

PropertyInfoPara g_mmplan_note_view_info[]=
{
	{0,0,CTC_DATA_STR,"TITLE","ʩ����Ŀ"}, // ����ID,�籨��,�¼ƻ�ID,����,�ȼ�,�ص�,��̵�
	{1,0,CTC_DATA_STR,"UNITNM","ʩ����λ"}, // ����������
	{1,0,CTC_DATA_STR,"WKCON","ʩ������"},
	{0,0,CTC_DATA_STR,"SPLIM","��������"},
	{0,0,CTC_DATA_STR,"LIMINFO","�г����ƿ�(�����)"}
};

int    count_of_g_mmplan_note_view_info()
{
    return  sizeof(g_mmplan_note_view_info)/sizeof(g_mmplan_note_view_info[0]);
}

PropertyInfoPara g_mmplan_note_list_info[]=
{
	{0,0,CTC_DATA_STR,"PLANID","ʩ����ʶ"},
	{1,ID_PD_MPLAN_LIST_SUBID,CTC_DATA_STR,"SUBID","�ƻ���"},
	{1,0,CTC_DATA_STR,"TM","ʩ��ʱ��"},
	{1,0,CTC_DATA_STR,"TYPE","����"},
	{1,0,CTC_DATA_STR,"WORKPOS","ʩ����Ŀ"},
//	{1,0,CTC_DATA_STR,"TRNINFO","·�ó�"},
};

int    count_of_g_mmplan_note_list_info()
{
    return  sizeof(g_mmplan_note_list_info)/sizeof(g_mmplan_note_list_info[0]);
}

struct PropertyInfoPara  g_recordinfo_def[]=
{
	{3,ID_PD_ST_STATION_RECINDEX,CTC_DATA_INT,"index","վ��"},
	{3,ID_PD_ST_STATION,CTC_DATA_STATION,"station_name","��վ����"},
	{3,ID_PD_ST_ARRIVE_TRAINID,CTC_DATA_STR,"arrive_id","���ﳵ��"},
	{3,ID_PD_ST_DEPART_TRAINID,CTC_DATA_STR,"depart_id","��������"},
	{1,ID_PD_ST_IMPINFO,CTC_DATA_STR,"impinfo","������Ϣ"},
	{1,ID_PD_ST_STPINFO,CTC_DATA_STR,"stpinfo","��ͣ��Ϣ"},
	{2,ID_PD_ST_MIN_STOP_TIME,CTC_DATA_INT,"min_stop_time","Ӫҵʱ��"},
	{4,ID_PD_ST_SCHD_ARRIVE_TIME,CTC_DATA_DATETIME,"plan_arrive","ͼ������"},
	{4,ID_PD_ST_SCHD_DEPART_TIME,CTC_DATA_DATETIME,"plan_depart","ͼ������"},
	{4,ID_PD_ST_ARRIVE_TIME,CTC_DATA_DATETIME,"arrive","ʵ�ʵ���"},
	{4,ID_PD_ST_DEPART_TIME,CTC_DATA_DATETIME,"depar","ʵ�ʷ���"},
	{3,ID_PD_ST_ARRIVE_SIDE_ID,CTC_DATA_SIDE,"arrive_side","�ӳ��ɵ�"},
	{3,ID_PD_ST_DEPART_SIDE_ID,CTC_DATA_SIDE,"depart_side","�����ɵ�"},
};

int    count_of_recordinfo_def()
{
	return sizeof(g_recordinfo_def)/sizeof(g_recordinfo_def[0]);
}

PropertyInfoPara g_mmplan_dcmd_def[]=
{
	{0,0,CTC_DATA_STR,"PLANID","ʩ���ƻ�ID"},
    {0,0,CTC_DATA_INT,"CMDINDEX","�������"}, // ��CMD_DEST ��ΪCMDINDEX
	{0,0,CTC_DATA_STR,"PRNM","��Ŀ����"},
	{0,0,CTC_DATA_INT,"CMDTP","��������"},
	{0,0,CTC_DATA_INT,"SUBTP","����������"},
	{0,0,CTC_DATA_STR,"TOPIC","�������"},
	{0,0,CTC_DATA_STR,"CONTENT","��������"},
	//{0,0,CTC_DATA_DATETIME,"SDTM","����ʱ��"},
	//{0,0,CTC_DATA_STR,"SDUNIT","�������"},
	//{0,0,CTC_DATA_STR,"SENDER","������"},
	//{0,0,CTC_DATA_STR,"RECVUNIT","�������"},
};

int    count_of_mmplan_dcmd_def()
{
	return sizeof(g_mmplan_dcmd_def)/sizeof(g_mmplan_dcmd_def[0]);
}

PropertyInfoPara g_dcmd_viewlist_def[]=
{
	{0,ID_PD_MPLAN_DCMD_INDEX,CTC_DATA_STR,"CMDINDEX","����ID"},
	{0,0,CTC_DATA_STR,"PRNM","ʩ����Ŀ"},
	{0,0,CTC_DATA_STR,"RECVER","���λ"},
	{0,0,CTC_DATA_STR,"TOPIC","����"},
	{0,ID_PD_MPLAN_DCMD_CONTENT,CTC_DATA_STR,"CONTENT","��������"},
};

int    count_of_viewlist_def()
{
	return sizeof(g_dcmd_viewlist_def)/sizeof(g_dcmd_viewlist_def[0]);
}

PropertyInfoPara g_mmplan_jieshi_def[]=
{
	{0,0,CTC_DATA_STR,"PLANID","ʩ���ƻ�ID"},
	{0,0,CTC_DATA_STR,"CMDID","��ʾ�����"},
	{0,0,CTC_DATA_STR,"CONTENT","��������"},
};

int    count_of_jieshi_def()
{
	return sizeof(g_mmplan_jieshi_def)/sizeof(g_mmplan_jieshi_def[0]);
}

PropertyInfoPara g_quebao_def[]=
{
	//{0,0,CTC_DATA_STR,"RKTM","���ʱ��"},
	{0,0,CTC_DATA_STR,"IDX","���"},
	{0,0,CTC_DATA_STR,"TYPE","����"},
	{0,0,CTC_DATA_STR,"YZ","�޳�����"},
	{0,0,CTC_DATA_STR,"CNO","����"},
	{0,0,CTC_DATA_STR,"SWT","����"},
	{0,0,CTC_DATA_STR,"LEN","����"},
	{0,0,CTC_DATA_STR,"LDFLAG","���ر�־"},

	{0,0,CTC_DATA_STR,"LD","����"},
	{0,0,CTC_DATA_STR,"FIRSTNM","��һ��վ"},
	{0,0,CTC_DATA_STR,"LSTSTNM","���վ"},
	{0,0,CTC_DATA_STR,"OBJNM","��������"},

	{0,0,CTC_DATA_STR,"RCVNM","�ջ���"},
	{0,0,CTC_DATA_STR,"FZNM","��վ��"},
	{0,0,CTC_DATA_STR,"PBCNT","���"},
	{0,0,CTC_DATA_STR,"HSFLAG","���ͱ�־"},
	{0,0,CTC_DATA_STR,"ZBFLAG","�Ա���־"},
	{0,0,CTC_DATA_STR,"JYFLAG","���ñ�־"},
};

int    count_of_quebao_def()
{
	return sizeof(g_quebao_def)/sizeof(g_quebao_def[0]);
}

PropertyInfoPara g_mplan_trninfo_def[]=
{
	{0,0,CTC_DATA_STR,"ID","���"},
	{0,0,CTC_DATA_STR,"DPTNO","��������"},
	{0,0,CTC_DATA_STR,"DPTST","������վ"},
	{0,0,CTC_DATA_STR,"ARVST","���복վ"},
	{0,0,CTC_DATA_STR,"ARVNO","���복��"},
	{0,0,CTC_DATA_STR,"STPTM","����ͣ��ʱ��"},
	{0,0,CTC_DATA_STR,"INFO","������Ϣ"},
};

int    count_of_mplan_trninfo_def()
{
	return sizeof(g_mplan_trninfo_def)/sizeof(g_mplan_trninfo_def[0]);
}

PropertyInfoPara g_typlan_def[]=
{
	{0,0,CTC_DATA_STR,"TNO","���˳���"},
	{0,0,CTC_DATA_DATETIME,"STTM","��ʼʱ��"}, // ��CMD_DEST ��ΪCMDINDEX
	{0,0,CTC_DATA_DATETIME,"EDTM","����ʱ��"},
	{0,0,CTC_DATA_STR,"STST","��ʼ��վ"},
	{0,0,CTC_DATA_STR,"EDST","������վ"},
	{0,0,CTC_DATA_INT,"NOTECNT","֪ͨ������"},
};

int    count_of_typlan_def()
{
	return sizeof(g_typlan_def)/sizeof(g_typlan_def[0]);
}

PropertyInfoPara g_typlan_dcmd_def[]=
{
	{0,ID_PD_TYPLAN_DCMD_ID,CTC_DATA_STR,"CID","�������"}, // ��CMD_DEST ��ΪCMDINDEX
	{0,0,CTC_DATA_STR,"CRECV","�����"},
	{0,0,CTC_DATA_INT,"CTYPE","��������"},
	{0,0,CTC_DATA_STR,"CSID","����������"},
	{0,0,CTC_DATA_STR,"CTITLE","�������"},
	{0,0,CTC_DATA_STR,"CPNM","ʩ����Ŀ"},
	{0,0,CTC_DATA_INT,"CRCNT","�����������"},
	{0,0,CTC_DATA_STR,"CRPT","�������"},
	{0,ID_PD_TYPLAN_DCMD_CONTENT,CTC_DATA_STR,"CCONTENT","��������"},
};

int    count_of_typlan_dcmd_def()
{
	return sizeof(g_typlan_dcmd_def)/sizeof(g_typlan_dcmd_def[0]);
}
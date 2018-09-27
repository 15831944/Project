#include "stdafx.h"
#include "ctc_data_type_def.h"
#include <string.h>
#include "tg_structdef.h"
#include "sysshare.h"

struct IDNameStruct
{
	int value;
	char *name;
};


const char *GetTypeStringByIndex(int index,int *value,IDNameStruct *info,int infocount)
{
	if(index>=0 && index<infocount)
	{
		*value=info[index].value; 
		return info[index].name; 
	}
	else
		return NULL;
}	

const char *GetTypeStringByValue(int value,IDNameStruct *info,int infocount)
{
	for(int i=0;i<infocount;i++)
	{
		if(value==info[i].value)
			return info[i].name; 
	}
	return NULL;
}	

#define BEGIN_DEF_TYPE_INFO(def) \
	IDNameStruct def[]={ 
#define SET_TYPE_INFO(value,name) {value,name}, 
#define END_DEF_TYPE  }; 

#define TYPE_INFO_COUNT(def) sizeof(def)/sizeof(def[0])
#define GET_TYPE_STRING(def,index,value)  GetTypeStringByIndex(index,value,def,sizeof(def)/sizeof(def[0]))
#define GET_VALUE_STRING(def,value)  GetTypeStringByValue(value,def,sizeof(def)/sizeof(def[0]))

BEGIN_DEF_TYPE_INFO(DirectionInfos)
SET_TYPE_INFO(0,"����")
SET_TYPE_INFO(1,"����")
SET_TYPE_INFO(2,"˫��")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(DepartCondInfos)
SET_TYPE_INFO(0,"���մ���")
SET_TYPE_INFO(1,"���ռƻ�")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(BoolInfos)
SET_TYPE_INFO(0,"��")
SET_TYPE_INFO(1,"��")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(RankInfos)
SET_TYPE_INFO(0,"����")
SET_TYPE_INFO(1,"����")
SET_TYPE_INFO(2,"һ��")
SET_TYPE_INFO(3,"����")
SET_TYPE_INFO(4,"����")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(SpaceInfos)
SET_TYPE_INFO(0,"����")
SET_TYPE_INFO(1,"һ��")
SET_TYPE_INFO(2,"����")
SET_TYPE_INFO(3,"����")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(BlockTypeInfos)
SET_TYPE_INFO(TG_ABS_INHIBIT,"�������")
SET_TYPE_INFO(TG_ABS_SLOW,"��������")
SET_TYPE_INFO(TG_ABS_FIX_MARK,"�����ۺ��촰��")
SET_TYPE_INFO(TG_SECTION_ELEC_FIX,"��������")
SET_TYPE_INFO(TG_SECTION_ELEC_FIX_LANZHOU,"���ݵ�������")
SET_TYPE_INFO(TG_CHANGE_BLOCK_TYPE,"������������ʽ")
SET_TYPE_INFO(TG_SIDE_INHIBIT,"վ�ڷ���")
SET_TYPE_INFO(TG_SIDE_SLOW,"վ������")
SET_TYPE_INFO(TG_SIDE_TIANCHUANG,"վ���촰��")
SET_TYPE_INFO(TG_SIDE_OCCUPY,"�ɵ�ռ��")
SET_TYPE_INFO(TG_GRAPH_NOTE,"����ע��")

END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(MMPlanStatusInfo)
SET_TYPE_INFO(0,"δ��ʼ")
SET_TYPE_INFO(1,"ʩ����ʼ")
SET_TYPE_INFO(2,"ʩ������")
END_DEF_TYPE

// ע��˴�����Ӧ��TG.OCX һ��
BEGIN_DEF_TYPE_INFO(NotMeetTypeInfo) 
SET_TYPE_INFO(0,"��")
SET_TYPE_INFO(NOTMEET_CRH,"���ᶯ��")
SET_TYPE_INFO(NOTMEET_SEPECIAL_Z,"����ֱ��")
SET_TYPE_INFO(NOTMEET_PASSENGER,"����ͳ�")
SET_TYPE_INFO(NOTMEET_FREIGHT,"�����г�")
SET_TYPE_INFO(NOTMEET_SPACE,"���ᳬ��")

SET_TYPE_INFO(NOTMEET_200_orUp,"����200������")
SET_TYPE_INFO(NOTMEET_120_200,"����120-200")
SET_TYPE_INFO(NOTMEET_120_Low,"����120����")
SET_TYPE_INFO(NOTMEET_ALL,"��������")
END_DEF_TYPE

const char *GetCTCTypeChoiceString(CTC_DATA_DEF tp, int index, int *value,long para)
{
	switch(tp)
	{
	case CTC_DATA_DIRECTION:
		return GET_TYPE_STRING(DirectionInfos,index,value);
	case CTC_DATA_DEPART_COND:
		return GET_TYPE_STRING(DepartCondInfos,index,value);
	case CTC_DATA_BOOL:
		return GET_TYPE_STRING(BoolInfos,index,value);
	case CTC_DATA_SPACE:
		return GET_TYPE_STRING(SpaceInfos,index,value);
	case CTC_DATA_RANK:
		return GET_TYPE_STRING(RankInfos,index,value);
	case CTC_DATA_MARK_TYPE:
		return GET_TYPE_STRING(BlockTypeInfos,index,value);
	case CTC_DATA_MMPLAN_STATUS:
		return GET_TYPE_STRING(MMPlanStatusInfo,index,value);
	case CTC_DATA_STATION:
		{
			int nStationList[128];
			int stationCount=128;
			stationCount=gpcdef.GetStationList(para,stationCount,nStationList);
			if(index>=0 && index<stationCount)
				return gpcdef.GetStationName(nStationList[index]);
			else
				return NULL;
		}
		break;

	case CTC_DATA_NOT_MEET:
		return GET_TYPE_STRING(NotMeetTypeInfo,index,value);
		break;
	}
	return NULL;
}

const char *GetCTCValueDispString(CTC_DATA_DEF tp, int value,long para)
{
	static char tmp[128];
	memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));

	switch(tp)
	{
	case CTC_DATA_DIRECTION:
		return GET_VALUE_STRING(DirectionInfos,value);
	case CTC_DATA_DEPART_COND:
		return GET_VALUE_STRING(DepartCondInfos,value);
	case CTC_DATA_BOOL:
		return GET_VALUE_STRING(BoolInfos,value);
	case CTC_DATA_SPACE:
		return GET_VALUE_STRING(SpaceInfos,value);
	case CTC_DATA_RANK:
		return GET_VALUE_STRING(RankInfos,value);
	case CTC_DATA_MARK_TYPE:
		return GET_VALUE_STRING(BlockTypeInfos,value);
	case CTC_DATA_MMPLAN_STATUS:
		return GET_VALUE_STRING(MMPlanStatusInfo,value);
	case CTC_DATA_DATETIME:
		{
			CTime ctm=value;
			strcpy(tmp,ctm.Format("%m��%d��%H:%M"));
			return tmp;
		}

	default:
		sprintf(tmp,"%d",value);
		return tmp;
	}
	return NULL;
}

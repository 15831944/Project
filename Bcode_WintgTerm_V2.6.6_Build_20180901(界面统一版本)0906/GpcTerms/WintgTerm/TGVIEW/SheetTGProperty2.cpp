#include "stdafx.h"
#include "SheetTGProperty.h"
#include "ext_src/XmlNodeWrapper.h"
#include "tg_BinXml.h"
#include "TgPspData.h"
#include "memshare.h"
#include "tg_basicpara.h"
#include "tg_function_config.h"
#include <SuperPassDlg.h>

extern time_t  ConvertToLongTime(time_t t,time_t reftm,int hour_range_left,int hour_range_right);
extern BOOL  ChangeTrainElecLocoFlag(CTG_SingleTrain *pTrain,int station,BOOL elec);
extern CTGBasicParam *GetTGBasicParam();
void CSheetTGProperty::UpdateShowThis(CTgView *pview,const CTG_LogicObject *pobj,int para,BOOL switchobject, int conid)
{
	if(pview==NULL)
		return;

	m_pTgView=pview;
	
	if(para==0x7fff) // object deleted!
	{
		m_PspType=PSP_TYPE_NULL;
		m_pCurrentObject=NULL;
		m_trainInfoControl.DeleteAllItems();   // 20090630
		m_trainInfoControl.SetGeneralInfo("");
		m_nSelStationRecIndex=-1;
		m_nSelStationNo=0;
		m_nCurrentTrainIndex=0;
		return;
	}

	if(pobj==NULL)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		const CTG_SingleTrain *ptrain=(const CTG_SingleTrain *)pobj;
		m_pCurrentObject=ptrain;
		CreateProperty(PSP_TYPE_TRAIN);

			m_nSectionID=conid;
			m_nSelStationRecIndex=para;
			m_nCurrentTrainIndex=ptrain->GetTrainIndex();
			
			const CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex_Const(m_nSelStationRecIndex);
			if(prec)
				m_nSelStationNo=prec->GetStationNo();
			else
				m_nSelStationNo=0;

			InitStationProperty();
			UpdatePropertyData(pobj);
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		m_pCurrentObject=pobj;
	
		CreateProperty(PSP_TYPE_MARK);
		UpdatePropertyData(pobj);
	}
	
}

struct PropertyInfoPara  g_train_property[]
={
	{0,ID_PD_TRAIN_INFO,CTC_DATA_STR,"INFO","������Ϣ"},
	{2,ID_PD_TRAIN_SHIFT_INDEX,CTC_DATA_INT,"shift_train_index","ȫ��ID"},
	{1,ID_PD_TRAIN_ID,CTC_DATA_STR,"train_id","���κ�"},
	{3,ID_PD_TRAIN_TYPE,CTC_DATA_TRAIN_TYPE,"type","�г�����"},
	{3,ID_PD_TRAIN_RUN_TYPE,CTC_DATA_TRAIN_RUN_TYPE,"run_type","��������"},
	{3,ID_PD_TRAIN_ABS_TIME_TYPE,CTC_DATA_TRAIN_ABS_TIME_TYPE,"abs_time","����ʱ������"},
	{3,ID_PD_TRAIN_SUPER_RANK,CTC_DATA_RANK,"space_rank","���޵ȼ�"},
	{3,ID_PD_TRAIN_MILITARY_RANK,CTC_DATA_SPACE,"military_rank","���˵ȼ�"},
	{3,ID_PD_TRAIN_IMPORTRANK,CTC_DATA_SPACE,"super_rank","�ص��г�"},
	{3,ID_PD_TRAIN_ELEC,CTC_DATA_BOOL,"elec","����ǣ���г�"},

	{1,ID_PD_TRAIN_TRACT_ENGINE,CTC_DATA_STR,"tract_engine","ǣ������"},
	{1,ID_PD_TRAIN_ATTACH_ENGINE,CTC_DATA_STR,"attach_engine","���һ���"},
	{1,ID_PD_TRAIN_LENGTH,CTC_DATA_INT,"lentg","����"},
	{1,ID_PD_TRAIN_WEIGHT,CTC_DATA_INT,"weight","����"},

	{1,ID_PD_TRAIN_COUNT,CTC_DATA_INT,"count","����"},
	
	
	
	{0,ID_PD_TRAIN_EXT_INFO,CTC_DATA_STR,"INFO","��ؼƻ���Ϣ"},
	{4,ID_PD_TRAIN_START_PLAN_TIME,CTC_DATA_DATETIME,"start_plan_time","ʼ��ͼ��ʱ��"},
	{4,ID_PD_TRAIN_START_ACTUAL_TIME,CTC_DATA_DATETIME,"start_actual_time","ʼ��ʵ��ʱ��"},
	{4,ID_PD_TRAIN_SBORDER_PLAN_TIME,CTC_DATA_DATETIME,"sborder_plan_time","�ֽ�ڽ���ͼ��ʱ��"},
	{4,ID_PD_TRAIN_SBORDER_ACTUAL_TIME,CTC_DATA_DATETIME,"sborder_actual_time","�ֽ�ڽ���ʵ��ʱ��"},
	{1,ID_PD_TRAIN_SBORDER_DELAYTIME,CTC_DATA_DATETIME,"sborder_delay_time","�ֽ�ڽ������(��)"},

	{2,ID_PD_TRAIN_START_STA_NAME,CTC_DATA_STR,"start_sta_name","ʼ��վ"},
	{2,ID_PD_TRAIN_END_STA_NAME,CTC_DATA_STR,"end_sta_name","�յ�վ"},
	//// FOLLOWING STATION
	{0,ID_PD_ST_INFO,CTC_DATA_STR,"RECORD","��վ��ҵ"},
	{1,ID_PD_ST_STATION,CTC_DATA_STR,"station_name","��վ����"},
	{4,ID_PD_ST_SCHD_ARRIVE_TIME,CTC_DATA_DATETIME,"plan_arrive","ͼ������"},
	{4,ID_PD_ST_SCHD_DEPART_TIME,CTC_DATA_DATETIME,"plan_depart","ͼ������"},
	{4,ID_PD_ST_ARRIVE_TIME,CTC_DATA_DATETIME,"arrive","�ƻ�����"},
	{4,ID_PD_ST_DEPART_TIME,CTC_DATA_DATETIME,"depar","�ƻ�����"},
	{3,ID_PD_ST_ENTRY_ID,CTC_DATA_ENTRY,"entry","��һ����"},  // readonly
	{3,ID_PD_ST_EXIT_ID,CTC_DATA_ENTRY,"exit","��һ����"},
	{2,ID_PD_ST_MIN_STOP_TIME,CTC_DATA_INT,"min_stop_time","Ӫҵʱ��"},
	{3,ID_PD_ST_ARRIVE_SIDE_ID,CTC_DATA_SIDE,"arrive_side","�ӳ��ɵ�"},
	{3,ID_PD_ST_DEPART_SIDE_ID,CTC_DATA_SIDE,"depart_side","�����ɵ�"},
	{3,ID_PD_ST_PLAN_ARRIVE_SIDE_ID,CTC_DATA_SIDE,"plan_arrive_side","ͼ���ӳ��ɵ�"},
	{3,ID_PD_ST_PLAN_DEPART_SIDE_ID,CTC_DATA_SIDE,"plan_depart_side","ͼ�������ɵ�"},
    {1,ID_PD_ST_FORCE_SIDE,CTC_DATA_STR,"force_side","�ɵ�����"},
	{1,ID_PD_ST_AUTOTRIG,CTC_DATA_STR,"trig_type","�Դ���־"},
	{3,ID_PD_ST_ARRIVE_RANK,CTC_DATA_RANK,"arrive_space_rank","�ӳ�����"},
	{3,ID_PD_ST_DEPART_RANK,CTC_DATA_RANK,"depart_space_rank","��������"},
	{3,ID_PD_ST_DEPART_BY_PLAN,CTC_DATA_DEPART_COND,"depart_acc_plan","����ʱ��"},
	{3,ID_PD_ST_TECH_STOP,CTC_DATA_BOOL,"tech_stop","����ͣ��"},
	{3,ID_PD_ST_NOT_MEET,CTC_DATA_NOT_MEET,"not_meet","���ᳵ��"},
	{3,ID_PD_ST_ELEC_LOCO,CTC_DATA_BOOL,"elec_loco","����ǣ��"},
	
	{3,ID_PD_ST_MANUALSET_PASS,CTC_DATA_BOOL,"man_pass","�涨ͨ��"},
	{1,ID_PD_ST_IMPINFO,CTC_DATA_STR,"impinfo","������Ϣ"},
	{1,ID_PD_ST_STPINFO,CTC_DATA_STR,"stpinfo","��ͣ��Ϣ"},
};



#define TRAIN_PROPERTY_NUM  sizeof(g_train_property)/sizeof(g_train_property[0])

int GetStationPropertyOffset()
{
	for(int i=0;i<TRAIN_PROPERTY_NUM;i++)
	{
		if(g_train_property[i].id ==ID_PD_ST_STATION)
			return i;
	}
	return 0;
}

struct PropertyInfoPara  g_mark_property[]
={
	{0,ID_PD_ST_INFO,CTC_DATA_STR,"MARK","ʩ��������ע��Ϣ"},
	{1,ID_PD_ST_MARK_STATION1,CTC_DATA_STR,"STST","��ʼվ"},
	{1,ID_PD_ST_MARK_STATION2,CTC_DATA_STR,"EDST","����վ"},
	{1,ID_PD_ST_MARK_TEXT,CTC_DATA_STR,"text","��ע"},
};
#define PLAN_MARK_PROPERTY_NUM  sizeof(g_mark_property)/sizeof(g_mark_property[0])

struct PropertyInfoPara  g_con_property[]
={
	{0,ID_PD_ST_INFO,CTC_DATA_STR,"WORKSCHEDULEINFO","����ʵ����Ϣ"},
	{1,ID_PD_CON_ID,CTC_DATA_INT,"section_id","���κ�"},
	{1,ID_PD_CON_NAME,CTC_DATA_STR,"section_name","��������"},
	{1,ID_PD_CON_SHIFT,CTC_DATA_STR,"shift_name","���"},
	{1,ID_PD_CON_START_TIME,CTC_DATA_STR,"start_time","������ʼʱ��"},
	{1,ID_PD_CON_END_TIME,CTC_DATA_STR,"end_time","�������ʱ��"},
	{1,ID_PD_CON_PASSENGER_TRAIN_COUNT,CTC_DATA_INT,"pass_train_cnt","�ÿ��г�����"},
	{1,ID_PD_CON_FREIGHT_TRAIN_COUNT,CTC_DATA_INT,"frei_train_cnt","�����г�����"},
	{1,ID_PD_CON_TRAIN_COUNT,CTC_DATA_INT,"train_cnt","�г�����"},
	{1,ID_PD_CON_MARK_COUNT,CTC_DATA_INT,"mark_cnt","ʩ����������"}
};
#define PLAN_CON_PROPERTY_NUM  sizeof(g_con_property)/sizeof(g_con_property[0])

struct PropertyInfoPara  g_con_basic_property[]
={
	{0,ID_PD_ST_INFO,CTC_DATA_STR,"BASICSCHEDULEINFO","����ͼ����Ϣ"},
	{1,ID_PD_CON_PASSENGER_TRAIN_COUNT_BASIC,CTC_DATA_INT,"pass_train_cnt","ͼ���ͳ���"},
	{1,ID_PD_CON_FREIGHT_TRAIN_COUNT_BASIC,CTC_DATA_INT,"frei_train_cnt","ͼ��������"},
	{1,ID_PD_CON_TRAIN_COUNT_BASIC,CTC_DATA_INT,"train_cnt","ͼ���г�����"}
};
#define PLAN_CON_BASIC_PROPERTY_NUM sizeof(g_con_basic_property)/sizeof(g_con_basic_property[0])

struct PropertyInfoPara  g_con_shift_property[]
={
	{0,ID_PD_ST_INFO,CTC_DATA_STR,"SHIFTSCHEDULEINFO","�����ƻ���Ϣ"},
	{1,ID_PD_CON_PASSENGER_TRAIN_COUNT_SHIFT,CTC_DATA_INT,"pass_train_cnt","��ƻ��ͳ���"},
	{1,ID_PD_CON_FREIGHT_TRAIN_COUNT_SHIFT,CTC_DATA_INT,"frei_train_cnt","��ƻ�������"},
	{1,ID_PD_CON_TRAIN_COUNT_SHIFT,CTC_DATA_INT,"train_cnt","��ƻ��г�����"},
	{1,ID_PD_CON_MARK_COUNT_SHIFT,CTC_DATA_INT,"mark_cnt","��ƻ�ʩ������"}
};
#define PLAN_CON_SHIFT_PROPERTY_NUM sizeof(g_con_shift_property)/sizeof(g_con_shift_property[0])

BOOL  CSheetTGProperty::CreateProperty(PSP_TYPE type)
{
	if(type==m_PspType)
		return TRUE;
	m_PspType=type;
	if(m_PspType==PSP_TYPE_TRAIN)
	{
		m_trainInfoControl.DeleteAllItems();
		if(!CreateProperty(&m_trainInfoControl,g_train_property,TRAIN_PROPERTY_NUM))
			return FALSE;
		return TRUE;
	}
	else if(m_PspType==PSP_TYPE_MARK)
	{
		m_trainInfoControl.DeleteAllItems();
		if(!CreateProperty(&m_trainInfoControl,g_mark_property,PLAN_MARK_PROPERTY_NUM))
			return FALSE;
	}
	else if(m_PspType==PSP_TYPE_SUM)
	{
		m_sumInfoControl.DeleteAllItems();
		if(!CreateProperty(&m_sumInfoControl,g_con_property,PLAN_CON_PROPERTY_NUM))
			return FALSE;
		if(!CreateProperty(&m_sumInfoControl,g_con_basic_property,PLAN_CON_BASIC_PROPERTY_NUM))
			return FALSE;
		if(!CreateProperty(&m_sumInfoControl,g_con_shift_property,PLAN_CON_SHIFT_PROPERTY_NUM))
			return FALSE;
	}
    return TRUE;
}

BOOL  CSheetTGProperty::CreateProperty(CPropTree *pPropTree,struct PropertyInfoPara *pInfo,int infoCount)
{
	CPropTreeItem* pRoot;
	int i;

	for(i=0;i<infoCount;i++)
	{
		int psp_flag=pInfo[i].flag%100;
		BOOL read_only = ((pInfo[i].flag>=100) || m_bReadOnly)? TRUE:FALSE;
		if(psp_flag==0)
		{
			pRoot =pPropTree->InsertItem(new CPropTreeItem());
			pRoot->SetLabelText(pInfo[i].caption);
			if(pInfo[i].id!= ID_PD_TRAIN_EXT_INFO) // Ĭ�ϲ�չ��������Ϣ
				pRoot->Expand(); // have this item expanded by default
		}
		else if(psp_flag==1)
		{
			CPropTreeItem* pItem;
			pItem = pPropTree->InsertItem(new CPropTreeItemStatic(), pRoot);
			pItem->SetLabelText(pInfo[i].caption);
		}
		else if(psp_flag==2)
		{
			CPropTreeItemEdit* pEdit;
			pEdit = (CPropTreeItemEdit*)pPropTree->InsertItem(new CPropTreeItemEdit(), pRoot);
			pEdit->SetLabelText(pInfo[i].caption);
			pEdit->ReadOnly(read_only); 

		}
		else if(psp_flag==3)
		{
			// 2014.05.27 ���� ����ʱ������
			bool enable=GetTGBasicParam()->IsEnableABSTimeType();
			if(!enable && pInfo[i].id==ID_PD_TRAIN_ABS_TIME_TYPE)
				continue;
			CPropTreeItemCombo* pCombo;
			pCombo = (CPropTreeItemCombo*)pPropTree->InsertItem(new CPropTreeItemCombo(), pRoot);
			pCombo->SetLabelText(pInfo[i].caption);
		
			DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VSCROLL        //| WS_VISIBLE
				| CBS_AUTOHSCROLL | CBS_DROPDOWNLIST; // for grid style only
			pCombo->CreateComboBox(dwStyle);
			CommboAddString(pCombo, pInfo[i].data_type, pInfo[i].id);
			pCombo->ReadOnly(read_only);
			
			if (read_only && gpcdef.IsEnableAssistantModifyPropTree())
			{
				if (pInfo[i].id==ID_PD_TRAIN_SHIFT_INDEX ||
					pInfo[i].id==ID_PD_TRAIN_TYPE ||
					pInfo[i].id==ID_PD_TRAIN_RUN_TYPE ||
					pInfo[i].id==ID_PD_TRAIN_ABS_TIME_TYPE ||
					pInfo[i].id==ID_PD_TRAIN_SUPER_RANK ||
					pInfo[i].id==ID_PD_TRAIN_MILITARY_RANK ||
					pInfo[i].id==ID_PD_TRAIN_IMPORTRANK || 
					pInfo[i].id==ID_PD_TRAIN_ELEC) //�г�����
					pCombo->ReadOnly(FALSE);
			}
		}
		else if(psp_flag==4)
		{
			CPropTreeItemDateTime *pdatactrl;

			pdatactrl = (CPropTreeItemDateTime*)pPropTree->InsertItem(new CPropTreeItemDateTime(), pRoot);
			pdatactrl->SetLabelText(pInfo[i].caption);		  
			//pCombo->CreateComboBoxBool();	// create the ComboBox control and auto fill with TRUE/FALSE values
			DWORD dwStyle = WS_BORDER | DTS_TIMEFORMAT;
			pdatactrl->CreateCtrl(dwStyle);	
			pdatactrl->ReadOnly(read_only);

		}
	}
	//set group info
	CStringArray itemArray;
	CString tmp;

	for ( i = 0; i < infoCount; i++)
	{
		tmp += pInfo[i].caption;
		tmp+= "%";
	}
	itemArray.Add(tmp);


	pPropTree->SetGroupInfo(itemArray);
	return TRUE;
}

void  CSheetTGProperty::CommboAddString(CPropTreeItemCombo* pCombo, CTC_DATA_DEF datatp, int id)
{
	int tmpindex=0;
	int rela_value=-1;

	do
	{
		const char *ptmpstr=TGPSP_GetItemChoice(m_pCurrentObject, datatp,TGPSP_MAKE_LONG_ID(m_nSelStationRecIndex,id),m_nSelStationNo,tmpindex,&rela_value);
		if(ptmpstr==NULL)
			break;
	
		pCombo->AddStringWithRelaValue(ptmpstr,rela_value);
		tmpindex++;
	}while(true);

	return;
}

BOOL  CSheetTGProperty::InitStationProperty()
{
	bool bIsNotThisSectionStation=GetTGFunctionConfig()->IsNotThisSectionStation(m_nSectionID,  m_nSelStationNo);
	struct PropertyInfoPara *pInfo=g_train_property;
	int    infoCount=TRAIN_PROPERTY_NUM;
	CPropTree *pPropTree=&m_trainInfoControl;

	int i;
	int offset=GetStationPropertyOffset();
	for(i=offset;i<infoCount;i++)
	{
		int psp_flag=pInfo[i].flag%100;
		BOOL read_only = (pInfo[i].flag>=100 || m_bReadOnly)? TRUE:FALSE;
		if(!read_only && m_pCurrentObject->GetObjectType()==TG_OBJ_TRAIN)
		{
			if(pInfo[i].id==ID_PD_ST_ENTRY_ID) //��һ����
				read_only=TRUE;
			else if(pInfo[i].id==ID_PD_ST_EXIT_ID) //��һ����
			{
				if(GetTGFunctionConfig()->IsDisenableSheetTabExit())
					read_only=TRUE;
				else
				{
					const CTG_TrainRecord *prec=((const CTG_SingleTrain *)m_pCurrentObject)->GetTrainRecordByRecIndex_Const(m_nSelStationRecIndex);
					if(prec && prec->IsTerminalRec())
						read_only=TRUE;
				}
			}
			else if(pInfo[i].id==ID_PD_ST_MIN_STOP_TIME)
				read_only=TRUE;
			else if(pInfo[i].id==ID_PD_ST_ARRIVE_SIDE_ID || pInfo[i].id==ID_PD_ST_DEPART_SIDE_ID)
				read_only=TRUE;
			else if(pInfo[i].id==ID_PD_ST_PLAN_ARRIVE_SIDE_ID || pInfo[i].id==ID_PD_ST_PLAN_DEPART_SIDE_ID)
				read_only=TRUE;

			else if (pInfo[i].id==ID_PD_ST_SCHD_ARRIVE_TIME ||pInfo[i].id==ID_PD_ST_ARRIVE_TIME)	
				read_only=TRUE;	

			else if ( pInfo[i].id==ID_PD_ST_SCHD_DEPART_TIME || pInfo[i].id==ID_PD_ST_DEPART_TIME)
				read_only=TRUE;
		}
		if(psp_flag==3)  //��վ�仯���»�ȡ�б���Ϣ
		{
			CPropTreeItemCombo* pCombo=(CPropTreeItemCombo*)pPropTree->GetItemByTextLabel(pInfo[i].caption);
			if(!pCombo)
				continue;
			pCombo->ResetContent(); 
			CommboAddString(pCombo, pInfo[i].data_type, pInfo[i].id);
		}
		if(bIsNotThisSectionStation)
			read_only=TRUE;
		CPropTreeItem *psub=pPropTree->GetItemByTextLabel(pInfo[i].caption);
		if(psub)
			psub->ReadOnly(read_only);
	}
	return TRUE;
}
BOOL  CSheetTGProperty::UpdatePropertyData(const CTG_LogicObject  *pobj)
{
	BOOL nodata=TRUE;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		const CTG_SingleTrain *ptrain=(const CTG_SingleTrain *)pobj;
		CString gen_info=GetObjectAllInfo(ptrain);
		m_trainInfoControl.SetGeneralInfo(gen_info); //GetAlarmInfo());
		UpdateTrainProperty(&m_trainInfoControl,g_train_property,TRAIN_PROPERTY_NUM,pobj);
		nodata=FALSE;
	}

	if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		const CTG_SingleMark *pMark=(const CTG_SingleMark *)pobj;
		CString gen_info=GetObjectAllInfo(pMark);
		m_trainInfoControl.SetGeneralInfo(gen_info);
		UpdateTrainProperty(&m_trainInfoControl,g_mark_property,PLAN_MARK_PROPERTY_NUM,pobj);
		nodata=FALSE;
	}
	if(pobj->GetObjectType()==TG_OBJ_SCHD)
	{
		CTG_ScheduleInfo schdinfo;
		const CTG_TrainScheduleBase *pbase=(const CTG_TrainScheduleBase *)pobj;
		pbase->GetScheduleInfo(schdinfo);
		if(schdinfo.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) 
			UpdateTrainProperty(&m_sumInfoControl,g_con_property,PLAN_CON_PROPERTY_NUM,pobj);
		else
		if(schdinfo.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
			UpdateTrainProperty(&m_sumInfoControl,g_con_shift_property,PLAN_CON_SHIFT_PROPERTY_NUM,pobj);
		else
		if(schdinfo.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
			UpdateTrainProperty(&m_sumInfoControl,g_con_basic_property,PLAN_CON_BASIC_PROPERTY_NUM,pobj);
		nodata=FALSE;
	}
	m_trainInfoControl.Invalidate();
	return TRUE;
}
BOOL  CSheetTGProperty::UpdateTrainProperty(CPropTree *pPropTree,struct PropertyInfoPara *pInfo,int infoCount,const CTG_LogicObject  *pobj)
{
	int i;
	for(i=0;i<infoCount;i++)
	{
		CPropTreeItem* pItem=pPropTree->GetItemByTextLabel(pInfo[i].caption);
		if(!pItem)
			continue;
		int psp_flag=pInfo[i].flag%100;

		if(!pobj)
		{
			if(psp_flag==3) // time
				pItem->SetRelaItemValue((LPARAM)-1);
			else if(psp_flag==4)
				pItem->SetRelaItemValue((LPARAM)0);
			else
				pItem->SetItemValue((LPARAM)"");
		}
		else
		{
			if(psp_flag==4 || psp_flag==3) // time
			{
				if(pInfo[i].id ==ID_PD_ST_STATION)
					pItem->SetRelaItemValue(m_nSelStationNo);
				else
				{
					long xv=TGPSP_GetItemIntValue(pobj,TGPSP_MAKE_LONG_ID(m_nSelStationRecIndex,pInfo[i].id), m_nSelStationNo);
					if(psp_flag==4) // time
					{
						if(xv<0)
							xv=0;
					}
					pItem->SetRelaItemValue((LPARAM)xv);
				}
			}
			else
			{
				const char *strvalue=TGPSP_GetItemValue(pobj,TGPSP_MAKE_LONG_ID(m_nSelStationRecIndex,pInfo[i].id),m_nSelStationNo);
				if(strvalue) //����ʹ���ַ�����ʽ
					pItem->SetItemValue((LPARAM)strvalue);
				else
				{
					int  intvalue=TGPSP_GetItemIntValue(pobj,TGPSP_MAKE_LONG_ID(m_nSelStationRecIndex,pInfo[i].id),m_nSelStationNo);
					if(intvalue>=0)
						pItem->SetRelaItemValue(intvalue);
				}
			}
		}
	}
	pPropTree->Invalidate();
	return TRUE;
}

void CSheetTGProperty::OnTrainInfoItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult) // ���Կ��޸�֪ͨ
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	if(m_pCurrentObject==NULL)
		return;
	if(!pNMPropTree->pItem)
		return;
	CPropTree *pPropTree;
	struct PropertyInfoPara *pInfo;
	CTG_LogicObject  *pObj;
	CTG_SingleTrain train;
	CTG_SingleMark  mark;
	int infoCount;
	int i;
	CString label=pNMPropTree->pItem->GetLabelText();
	if(m_pCurrentObject->GetObjectType()==TG_OBJ_TRAIN)
	{
		pInfo=g_train_property;
		infoCount=TRAIN_PROPERTY_NUM;
		pPropTree=&m_trainInfoControl;
		train=*(const CTG_SingleTrain *)m_pCurrentObject;
		pObj=&train;
	}
	else if(m_pCurrentObject->GetObjectType()==TG_OBJ_MARK)
	{
		pInfo=g_mark_property;
		infoCount=PLAN_MARK_PROPERTY_NUM;
		pPropTree=&m_trainInfoControl;
		mark=*(const CTG_SingleMark *)m_pCurrentObject;
		pObj=&mark;
	}
	else
		return;

	for( i=0;i<infoCount;i++)
	{
		if(label==pInfo[i].caption)
		{
			break;
		}
	}
	if(i>=infoCount)
		return;
	//////////////////////////////////////
	if(pInfo[i].id!=ID_PD_ST_STATION)
	{
		int psp_flag=pInfo[i].flag%100;

		////////////////////////////////////////
		if(psp_flag==4 || psp_flag==3 || pInfo[i].data_type==CTC_DATA_INT) // time
		{
			int value=pNMPropTree->pItem->GetRelaItemValue();
			bool bRet=TGPSP_SetItemIntValue(pObj,pInfo[i].caption,TGPSP_MAKE_LONG_ID(m_nSelStationRecIndex,pInfo[i].id),m_nSelStationNo,value);
			if(bRet)
			{
				GpcLogTrainMessage(0,0,"����Ա�޸�:%s  ID=%d %s %s: %d",((CTG_SingleTrain *)pObj)->GetTrainID(),
						((CTG_SingleTrain *)pObj)->GetTrainIndex(), 
						gpcdef.GetStationName(m_nSelStationNo), 
						pInfo[i].caption,
						value);
			}
		}
	}

	UpdateLocalData(pObj);
	UpdatePropertyData(pObj);
	*plResult = 0;
}

void  RemoveTrainAbsTimeType(CTG_SingleTrain* ptrain)
{
	long train_source=ptrain->GetTrainInfo()->m_data.train_source;
	train_source &= 0x00FFFFFF;
	ptrain->GetTrainInfo()->m_data.train_source=train_source;
}

void  SetTrainAbsTimeType(CTG_SingleTrain* ptrain, BYTE run_type)
{
	long nTmpAbsTimeType=run_type;
	nTmpAbsTimeType = nTmpAbsTimeType<<24;
	nTmpAbsTimeType &= 0xFF000000;
	nTmpAbsTimeType |= 0x80000000;  // ��λ��1
				
	long train_source=ptrain->GetTrainInfo()->m_data.train_source;
	train_source &= 0x00FFFFFF;
	train_source |= nTmpAbsTimeType;

	ptrain->GetTrainInfo()->m_data.train_source=train_source;
}

BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain)
{
	long train_source=ptrain->GetTrainInfo_Const()->m_data.train_source;
	if(train_source&0x80000000)
	{
		train_source &= 0x7F000000;
		BYTE tmp=(train_source>>24);
		return tmp;
	}
	else
	{
		return ptrain->GetTrainInfo_Const()->m_data.run_type;
	}
}

BOOL  SetNewTrainAbsRunTime(CTG_SingleTrain& train, int rec_index, BYTE run_type)// ���¼�����������ʱ��
{
	if(train.HasActualRecord()!=-1)
	{
		CString str;
		str.Format("���� %s ����ʵ�ʵ�,�����޸��г�����ʱ��!", train.GetTrainID());
		AfxMessageBox(str);
		return FALSE;
	}
	
	if(train.GetTrainRecordCount()==1)
		return FALSE;

	CTGBasicParam * pBasicPara = GetTGBasicParam();
	CTG_TrainRecord* prec=train.GetTrainRecordByRecIndex(0);
	if(prec==NULL)
		return FALSE;

	long nBaseTime=prec->GetDepartTime();
	int  nStation=prec->GetStationNo();
	int  nExit=prec->GetExit();

	bool bPass=false;
	if(prec->IsStartRec())
		bPass=false;
	else 
	{
		if(prec->IsPassRec())
			bPass=true;
		else
			bPass=false;
	}
		
	int start_extra_time=0;
	if(!bPass)
		start_extra_time=pBasicPara->CalculateTrainStartExtra(nStation, nExit, TRAIN_OUT_STATION, run_type);

	int abs_run_time=pBasicPara->CalculateTrainRunTime(nStation, nExit, TRAIN_OUT_STATION, run_type);

	for(int i=1; i<train.GetTrainRecordCount(); i++)
	{
		prec=train.GetTrainRecordByRecIndex(i);
		if(prec==NULL)
			return FALSE;
		
		int stop_extra_time=0;
		if(i==train.GetTrainRecordCount()-1)
		{
			if(prec->IsTerminalRec())
				stop_extra_time=pBasicPara->CalculateTrainStopExtra(prec->GetStationNo(),prec->GetEntry(),TRAIN_INTO_STATION, run_type);
			else
				stop_extra_time=0;
		}
		else
		{
			if(prec->IsPassRec())
				stop_extra_time=0;
			else
				stop_extra_time=pBasicPara->CalculateTrainStopExtra(prec->GetStationNo(),prec->GetEntry(),TRAIN_INTO_STATION, run_type);
		}
		int stop_time=prec->m_data.depart-prec->m_data.arrive;
		prec->m_data.arrive=nBaseTime+start_extra_time+abs_run_time+stop_extra_time;
		prec->m_data.depart=stop_time+prec->m_data.arrive;

		nBaseTime=prec->m_data.depart;
	
		if(prec->IsPassRec())
			start_extra_time=0;
		else
			start_extra_time=pBasicPara->CalculateTrainStartExtra(prec->GetStationNo(), prec->GetExit(), TRAIN_OUT_STATION, run_type);
		abs_run_time=pBasicPara->CalculateTrainRunTime(prec->GetStationNo(), prec->GetExit(), TRAIN_OUT_STATION, run_type);
	}

	return TRUE;
}

void CaculateTrainSlowTime(CTG_SingleTrain *single)
{
	// 2014.05.27 ���� ����ʱ������
	int nSaveType=255;
	bool enable=GetTGBasicParam()->IsEnableABSTimeType();
	if(enable)
	{
        BYTE nNewRunType=GetTrainAbsTimeType(single);
		// ����
		nSaveType=single->GetTrainInfo()->m_data.run_type;
		single->GetTrainInfo()->m_data.run_type=nNewRunType;
	}
	CTGBasicParam * pBasicPara = GetTGBasicParam();
	for(int rec_index=0;rec_index<single->GetTrainRecordCount()-1;rec_index++)
	{
		CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
		CTG_TrainRecord *precord_next=single->GetTrainRecord(rec_index+1);
		if(!precord || !precord_next) continue;

		int real_run_time=precord_next->GetArriveTime()-precord->GetDepartTime();
		
		int stop_extra_time  =0;
		int start_extra_time =0;
		if(!precord_next->IsPassRec() || precord_next->IsTerminalRec())
			stop_extra_time =pBasicPara->CalculateTrainStopExtra(precord_next->GetStationNo(),precord_next->GetEntry(),TRAIN_INTO_STATION,single->GetRunType());
		if(!precord->IsPassRec() || precord->IsStartRec())
			start_extra_time=pBasicPara->CalculateTrainStartExtra(precord->GetStationNo(),precord->GetExit(),TRAIN_OUT_STATION,single->GetRunType());

		int default_run_time=pBasicPara->CalculateTrainRunTime(precord->GetStationNo(),precord->GetExit(),TRAIN_OUT_STATION,single->GetRunType());
		int slow_time=real_run_time-default_run_time-stop_extra_time-start_extra_time;
		slow_time = (slow_time/60)*60;
		// �����˻�
		TG_SetRunFastSlowTime(precord->m_data.run_fast_slow_time,slow_time);
	}
	if(enable)
	{
		// �ָ�
		single->GetTrainInfo()->m_data.run_type=nSaveType;
	}
}

bool  CSheetTGProperty::TGPSP_SetItemIntValue(CTG_LogicObject *pobj,char* caption,int id, int stationid, long value)
{
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return false;
	
	CTG_SingleTrain *pTrain=(CTG_SingleTrain *)pobj;
	CTG_TrainInfo *pTrainInfo=pTrain->GetTrainInfo();
	if(pTrainInfo==NULL)
		return false;
	
	CString cap;
	
	int sid=TGPSP_GET_PROP_ID(id);
	if(sid == ID_PD_TRAIN_SHIFT_INDEX)
	{
		if(GetTGFunctionConfig()->IsEnableModifyGID())
		{
			if(pTrainInfo->m_data.shift_train_index!=value)
			{
				cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(), caption);
				if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
				{
					GpcLogTrainMessage(0,0,"����Ա�˹��趨 train:%s shift_train_index:%d-->%d", 
						pTrain->GetTrainID(), pTrainInfo->m_data.shift_train_index, value);
					pTrainInfo->m_data.shift_train_index=value;
					return true;
				}
			}
		}
		else
		{
			AfxMessageBox("�����޸�ȫ��ID");
		}
		return false;
	}
	
	if(sid == ID_PD_TRAIN_RUN_TYPE)
	{
		if(pTrainInfo->m_data.run_type!=value)
		{
			cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(),caption);
			if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
			{
				GpcLogTrainMessage(0,0,"����Ա�˹��趨 train:%s run_type:%d-->%d", 
					pTrain->GetTrainID(), pTrainInfo->m_data.run_type, value);
				pTrainInfo->m_data.run_type=value;
				return true;
			}
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_ABS_TIME_TYPE)
	{
		BYTE nOldAbsTimeType=GetTrainAbsTimeType(pTrain);
		if(nOldAbsTimeType!=value)
		{
			cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(),caption);
			if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
			{
				BYTE nNewAbsTimeType=value;
				if(SetNewTrainAbsRunTime(*pTrain, 0, nNewAbsTimeType))
				{
					SetTrainAbsTimeType(pTrain, nNewAbsTimeType);
					CaculateTrainSlowTime(pTrain);  // 20140903 ����ϻ�����
					GpcLogTrainMessage(0,0,"����Ա�˹��趨 train:%s ʹ������ʱ������ %d ���¼�������ʱ��", pTrain->GetTrainID(), nNewAbsTimeType);
					return true;
				}
			}
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_SUPER_RANK)
	{
		cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
		{
			pTrain->SetSpaceRank(value, 0, true,  true); 
			return true;
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_MILITARY_RANK)
	{
		if(pTrainInfo->m_data.military_rank!=value)
		{
			cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(),caption);
			if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
			{
				GpcLogTrainMessage(0,0,"����Ա�˹��趨 train:%s military_rank:%d-->%d", 
					pTrain->GetTrainID(), pTrainInfo->m_data.military_rank, value);
				pTrainInfo->m_data.military_rank=value;
				return true;
			}
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_IMPORTRANK)
	{
		if(pTrainInfo->m_data.super_rank!=value)
		{
			cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(), caption);
			if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
			{
				GpcLogTrainMessage(0,0,"����Ա�˹��趨 train:%s super_rank:%d-->%d", 
					pTrain->GetTrainID(), pTrainInfo->m_data.super_rank, value);
				pTrainInfo->m_data.super_rank=value;
				return true;
			}
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_TYPE)
	{
		if(pTrainInfo->m_data.type!=value)
		{
			BYTE run_type,prio,locotype;
			if(!GetTGBasicParam()->GetTrainTypeDef(value,&run_type,&prio,&locotype))
			{
				GpcLogTrainMessage(0,0,"����Ա�޸��г�:%s ����Ϊ %d, ���ǻ�ȡ run_type, prio, locotypeʧ��!", 
						pTrain->GetTrainID(), pTrainInfo->m_data.super_rank, value);
				return false;
			}
			CString str;
			const char* p=GetTGBasicParam()->GetTrainTypeNameById(run_type);
			if(p)
				str=p;
			else
				str="δ֪";
			if(pTrainInfo->m_data.run_type == run_type)
				cap.Format("��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(), caption);
			else
				cap.Format("��ȷʵҪ�޸��г� %s �� %s ��? �޸��г�����������趨\"��������\"Ϊ%s",pTrain->GetTrainID(), caption, str);
			cap +="\n\nע��:\n�޸��г����Ϳ�������������� ���޵ȼ��ȱ仯,��ȷ���г�";

			if(MessageBox(cap,"��ʾ",MB_YESNO|MB_ICONQUESTION)==IDYES)
			{
				pTrainInfo->m_data.type=value;
				pTrainInfo->m_data.run_type= run_type;
				pTrainInfo->m_data.priority = prio;

				pTrain->SetDefaultAttribByDefinedFlag(locotype);
				if(locotype==1)
					ChangeTrainElecLocoFlag(pTrain,-1,TRUE);
				else if(locotype==2)
					ChangeTrainElecLocoFlag(pTrain,-1,FALSE);

				return true;
			}
		}
		return false;
	}
	else if(sid == ID_PD_TRAIN_ELEC)
	{
		cap.Format("��ע��,��ȷʵҪ�޸��г� %s �� %s ��?",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"��ʾ",MB_YESNO)==IDYES)
		{
			if(value==1)
				ChangeTrainElecLocoFlag(pTrain,-1,TRUE);
			else
				ChangeTrainElecLocoFlag(pTrain,-1,FALSE);

			return true;
		}
		return false;
	}

	int rec_index=TGPSP_GET_STATION_ID(id);
	CTG_TrainRecord *pRecord=pTrain->GetTrainRecordByRecIndex(rec_index); 
	if(pRecord==NULL)
		return false;
	if(pRecord->GetStationNo()!=stationid)
		return false;
	
	if(sid == ID_PD_ST_SCHD_ARRIVE_TIME)
	{
		pRecord->m_data.plan_arrive=value;
	}
	else if(sid == ID_PD_ST_SCHD_DEPART_TIME)
	{
		pRecord->m_data.plan_depart=value;
	}
	else if(sid == ID_PD_ST_MIN_STOP_TIME)
	{
		pRecord->m_data.min_stop_time=value*60;
	}
	else if(sid == ID_PD_ST_ARRIVE_TIME)
	{
		time_t x=ConvertToLongTime(value,pRecord->m_data.arrive,12,12);
		if(x>0)
		{
			if(pRecord->IsPassRec())
			{
				pRecord->m_data.arrive=x;
				pRecord->m_data.depart=x;
			}
			else
				pRecord->m_data.arrive=x;
		}
	}
	else if(sid == ID_PD_ST_DEPART_TIME)
	{
		time_t x=ConvertToLongTime(value,pRecord->m_data.depart,12,12);
		if(x>0)
			pRecord->m_data.depart=x;
	}
	else if(sid == ID_PD_ST_DEPART_BY_PLAN)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?",pTrain->GetTrainID(), gpcdef.GetStationName(stationid), caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			TG_ManualSetTrigeRoute(pRecord->m_data);
			if(value)
			{
				TG_SetMakeDepartRouteByPlan(pRecord->m_data);
				GpcLogTrainMessage(0,0,"ͨ�����Դ���,����Ա�˹��趨train:%s station:%d rec_index:%d,����ʱ�� Ϊ���ռƻ�", 
					pTrain->GetTrainID(), pRecord->m_data.station, rec_index);
			}
			else
			{
				TG_SetMakeDepartRouteByMeet(pRecord->m_data);
				GpcLogTrainMessage(0,0,"ͨ�����Դ���,����Ա�˹��趨train:%s station:%d rec_index:%d,����ʱ�� Ϊ���մ���", 
					pTrain->GetTrainID(), pRecord->m_data.station, rec_index);
			}
			return true;
		}
	}
	else if(sid == ID_PD_ST_TECH_STOP)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?",pTrain->GetTrainID(), gpcdef.GetStationName(stationid),caption);
		CSuperPassDlg  passdlg;
		passdlg.m_strPrompt = "����������";
		passdlg.m_strTitle = "ȷ���޸� ����ͣ��";

		if (passdlg.DoModal() != IDOK)
		{
			return false;
		}
		else
		{
			if(passdlg.m_super_pass.Compare("1234") != 0)
			{
				MessageBox("�������!","����", MB_OK);
				return false;
			}
		}

		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			if(value)
			{
				TG_SetTechStopTrain(pRecord->m_data);
				GpcLogTrainMessage(0,0,"ͨ�����Դ���,����Ա�˹��趨train:%s station:%d rec_index:%d, Ϊ����ͣ��", 
					pTrain->GetTrainID(), pRecord->m_data.station, rec_index);
			}
			else
			{
				TG_ClearTechStopTrain(pRecord->m_data);
				GpcLogTrainMessage(0,0,"ͨ�����Դ���,����Ա�˹��趨train:%s station:%d rec_index:%d, Ϊ�Ǽ���ͣ��", 
					pTrain->GetTrainID(), pRecord->m_data.station, rec_index);
			}
		
			return true;	
		}
	}
	else if(sid == ID_PD_ST_ARRIVE_SIDE_ID)
	{
		if(pRecord->m_data.arrive_side!=value || pRecord->m_data.depart_side!=value)
		{
			CString cap;
			cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?\nע��:\n1.���ʵ�ɵ��Ƿ�����ӷ�������\n2.������ȷ��������̶�",
				pTrain->GetTrainID(), gpcdef.GetStationName(stationid),caption);
			if(MessageBox(cap,"����",MB_YESNO)==IDYES)
			{
				if(!TG_IsForceSide(pRecord->m_data))
				{
					pRecord->m_data.arrive_side=value;
					pRecord->m_data.depart_side=value;
					return true;
				}
				else
				{	
					AfxMessageBox("�ɵ����������޸Ĺɵ�");
				}
			}
		}
	}
	else if(sid == ID_PD_ST_DEPART_SIDE_ID)
	{
		if(pRecord->m_data.depart_side!=value)
		{
			CString cap;
			cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?\nע��:\n1.���ʵ�ɵ��Ƿ����㷢������\n2.������ȷ��������̶�",
				pTrain->GetTrainID(), gpcdef.GetStationName(stationid),caption);
			if(MessageBox(cap,"����",MB_YESNO)==IDYES)
			{
				if(!TG_IsForceSide(pRecord->m_data))
				{
					if(pRecord->m_data.arrive_side!=value)
					{
						CString cap;
						cap.Format("��ע��,���� %s �ĳ����ɵ��͵���ɵ���һ��,��ȷ���޸� %s ��?",pTrain->GetTrainID(),caption);
						if(MessageBox(cap,"����",MB_YESNO)==IDYES)
						{
							pRecord->m_data.depart_side=value;
							return true;
						}
					}
					else
					{
						pRecord->m_data.depart_side=value;
						return true;
					}
				}
				else
				{	
					AfxMessageBox("�ɵ����������޸Ĺɵ�");
				}
			}
		}
	}
	else if(sid == ID_PD_ST_DEPART_RANK)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %s ��?\n ע��:�޸����ȷ���г������Ƿ���ȷ.",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			pTrain->SetSpaceRank(value, rec_index, false, false);
			return true;
		}
	}
	else if(sid == ID_PD_ST_ARRIVE_RANK)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %s ��?\n ע��:�޸����ȷ���г������Ƿ���ȷ.",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			if(rec_index==0)
				pTrain->SetSpaceRank(value, 0, true, true); 
			else
				pTrain->SetSpaceRank(value, rec_index, true, false);
			return true;
		}
	}
	else if(sid == ID_PD_ST_ELEC_LOCO)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %s ��?\n ע��:�޸����ȷ���г�����ǣ����Χ.",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			if(value==1)
				ChangeTrainElecLocoFlag(pTrain,stationid,TRUE);
			else
				ChangeTrainElecLocoFlag(pTrain,stationid,FALSE);
			return true;
		}
	}
	else if(sid == ID_PD_ST_MANUALSET_PASS)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %s ��?",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			if(value==1)
				TG_SetDefinePass(pRecord->m_data);
			else
				TG_ReSetDefinePass(pRecord->m_data);
			return true;
		}
	}
	else if(sid == ID_PD_ST_NOT_MEET)
	{
		CString cap;
		cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %s ��?\nע��:�޸����ȷ�Ͻ����Ƿ�����!",pTrain->GetTrainID(),caption);
		if(MessageBox(cap,"����",MB_YESNO)==IDYES)
		{
			for(int recindex=rec_index;recindex<pTrain->GetTrainRecordCount();recindex++)
			{
				CTG_TrainRecord *prectmp=pTrain->GetTrainRecord(recindex);
				prectmp->m_data.can_not_meet=value;
			}
			return true;
		}
	}
	else if(sid == ID_PD_ST_EXIT_ID)
	{
		ushort st1=0,etr1=255,st2=0,etr2=255;
		GetTGBasicParam()->ConvertAbsnumToStationEntry(&st1,&etr1,&st2,&etr2,value);
		CTG_TrainRecord *pRecordNext=pTrain->GetTrainRecord(pRecord->GetIndex()+1); 
		if(pRecordNext)
		{
			int exit=-1, entry=-1;
			if(st1==pRecord->GetStationNo()  && st2== pRecordNext->GetStationNo())
			{
				exit=etr1;
				entry=etr2;
			}
			else if(st2==pRecord->GetStationNo() && st1== pRecordNext->GetStationNo())
			{
				exit=etr2;
				entry=etr1;
			}
		
			if(exit>0 && entry>0 && (pRecord->m_data.exit!=exit || pRecordNext->m_data.entry!=entry))
			{
				CString cap;
				cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?\nע��:�޸����ȷ���г����з���!",
								pTrain->GetTrainID(), gpcdef.GetStationName(stationid),caption);
				if(MessageBox(cap,"����",MB_YESNO)==IDYES)
				{
					pRecord->m_data.exit=exit;
					pRecordNext->m_data.entry=entry;

					int abs_dir;
					BOOL  train_down_direction=GetTGBasicParam()->IsTrainDirectionDown(pRecord->GetStationNo(),pRecord->GetExit(),TRAIN_OUT_STATION);
					BOOL  train_up_direction=GetTGBasicParam()->IsTrainDirectionUp(pRecord->GetStationNo(),pRecord->GetExit(),TRAIN_OUT_STATION);

					if(GetTGBasicParam()->GetAbsAttrib(value,&abs_dir))
					{
						// 20090917XIAN, �����г��ó����
						if(train_down_direction && abs_dir==0 || train_up_direction  && abs_dir==1)  
							TG_SetFlagReverse(pRecord->m_data.flag);
						else
							TG_ClearFlagReverse(pRecord->m_data.flag);	
					}

					return true;
				}
			}
		}
		else  // ���һվ����
		{
			int exit=-1;
			if(st1==pRecord->GetStationNo())
				exit=etr1;
			else if(st2==pRecord->GetStationNo())
				exit=etr2;

			if(exit>0 && pRecord->m_data.exit!=exit)
			{
				CString cap;
				cap.Format("��ע��,��ȷʵҪ�޸ĳ��� %s �� %sվ�� %s ��?\nע��:�޸����ȷ���г����з���!",
					pTrain->GetTrainID(), gpcdef.GetStationName(stationid),caption);
				if(MessageBox(cap,"����",MB_YESNO)==IDYES)
				{
					pRecord->m_data.exit=exit;
					return true;
				}
			}
		}
	}

	return false;
}
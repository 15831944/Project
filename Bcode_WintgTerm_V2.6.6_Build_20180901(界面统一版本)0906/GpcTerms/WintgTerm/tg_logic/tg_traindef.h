#ifndef _DEF_TRAIN_DEF_H
#define _DEF_TRAIN_DEF_H

#include "schedule.h"
#include <list>
/* �ڲ����ݴ������� */

class CTG_LogicObject{
public:
	BOOL    m_flagDataSyncedWithMMI; // �ͽ��������ͬ��״̬
	BOOL    m_flagDataSyncedWithDatabase;// �����ݿ������ͬ��״̬
	BOOL    m_flagDataSyncedWithServer;// �ͷ�������ͬ��״̬
	
	unsigned long m_flagNeedToSync;  // 32 BIT ��ʾ 0-31�������ߵ�ͬ������,�Ƿ�Ӧ��ͬ��
	unsigned long m_flagSyncStatus;  // 32 BIT ��ʾ 0-31�������ߵ�ͬ��״̬
};

class CTG_TrainInfo{
 public:
	TG_TrainInfo m_data;
	BOOL         m_changed;
	CTG_TrainInfo()
	{
		memset(&m_data,0,sizeof(m_data));
	}
};

class CTG_TrainEngineInfo{
	TG_TrainEngineInfo m_data;
	BOOL         m_changed;
 public:
	CTG_TrainEngineInfo()
	{
		memset(&m_data,0,sizeof(m_data));
	}
};

class CTG_TrainRecord{
 public:
	TG_TrainRecord  m_data;
	BOOL         m_changed;
	CTG_TrainRecord()
	{
		memset(&m_data,0,sizeof(m_data));
	}
};

class CTG_BlockInfo::public CTG_LogicObject{
};

class CTG_SingleTrain::public CTG_LogicObject{
   
protected:
	TRAIN_INDEX        m_nTrainIndex;

	CTG_TrainInfo                m_cTrainInfo;
	list<CTG_TrainEngineInfo>    m_listEngineInfo;
    list<CTG_TrainRecord>        m_listRecordInfo;

	TIME    m_LastBroadCastTime;


public:
	CTG_SingleTrain();
	~CTG_SingleTrain();

	TRAIN_INDEX GetTrainIndex() const { return m_TrainIndex; }
	void SetTrainIndex(TRAIN_INDEX x) {  m_TrainIndex = x;   }

	cTrainDef& operator = (const cTrainDef & def);
	cTrainDef& operator = (const TrainInfo & traininfo);
	cTrainDef& operator = (const TrainEngineInfo & engine);

	void   Init();
	short  IsValid();

	TIME   GetStartTime() const;
	TIME   GetEndTime() const;
	short  IfInTimeScope(TIME stime, TIME etime) const;
friend : // Import and export
	
};

///// SEARCH FLAGS DEFINITION

#define  SERCH_FLAG_ALL            1000	
#define  SERCH_FLAG_MMI_NOSYNC     1001	
#define  SERCH_FLAG_SERVER_NOSYNC  1002	
#define  SERCH_FLAG_DATABSE_NOSYNC 1003	
// 0 - 31 ��ʾ����32�������ߵ� NOSYNC��ѯ

class CTG_Iterator{
	CTG_TrainSchedule *m_pTrainSchedule;
	CTG_SingleTrain   *m_pSingleTrain;

 public:
	list<CTG_SingleTrain>::iterator  train_iterator;
	list<CTG_BlockInfo>::iterator  block_iterator;
	list<CTG_TrainRecord>::iterator  record_iterator;
    
	BOOL  SearchTrains(CTG_TrainSchedule *p,int search_flag); //search_flag �μ�ǰ�涨��
	BOOL  SearchBlocks(CTG_TrainSchedule *p,int search_flag); //search_flag �μ�ǰ�涨��
	CTG_SingleTrain *GetSingleTrain();
	CTG_BlockInfo *GetBlock();
	CTG_TrainRecord *GetTrainRecord();
};

class CTG_TrainSchedule{
 friend class CTG_Iterator;
protected:
	list<CTG_SingleTrain> m_listTrains;
	list<CTG_BlockInfo>   m_listBlocks;

	int       m_nSectionID;
	long      m_nShiftID;
    TIME      m_tStartTime,m_tEndTime;
public:
	///////// INIT ///////////
	void  Init(int section_id,TIME start,TIME end);
	void  Init(int section_id,long shift_id);
	///////// GET
    CTG_SingleTrain *GetSingleTrain(TRAIN_INDEX train_index);
    //////////	PURGE
	void  PurgeTrains();
	void  PurgeBlocks();

	//////////  ���ݿ�����ӿ�
    BOOL  OpenDatabase(ICTCDatabase *pInterface,BOOL readonly);
	BOOL  CloseDatabase();
	BOOL  ReadDataFromDatabase(BOOL purge_first);
	BOOL  WriteDataToDatabase(BOOL nosyncd_only);
	//////////  �����������ӿ�
	BOOL  ProcessServerMessage();
	//BOOL  RequestDataFromServer(BOOL purge_first);
	BOOL  ProcessOldCTCMessage();
    
	//////////  �ƻ����Ͳ����ӿ�
	BOOL  SetScheduleReceiverRange(int index,TIME from,TIME to);
	///////////////////////////////////////////////
};


#endif /** THE END **/

#ifndef __INCLUDE_TRAIN_SCHEDULE_WORK
#define __INCLUDE_TRAIN_SCHEDULE_WORK
class CTG_TrainScheduleWork: public CTG_TrainScheduleBase{

public:
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

#endif
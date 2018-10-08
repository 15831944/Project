#pragma once
#include "cadstruct.h"
class CSingleTrain;
class CWintgDoc;
class CMultiAbsMark;
///////////// CALL BY WINTG APP
BOOL  DB_Init(CString strDataPath); //new!!!
BOOL  DB_StartLocalDataServer(const char *cfgfilename);
BOOL  DB_EndLocalDataServer();
BOOL  DB_InitData(int conid);
BOOL  DB_DeinitData();
BOOL  DB_InLocalDBSyncMode();    // �Ƿ񱾵�ͬ����ģʽ
BOOL  DB_InLocalDBActiveMode();  // �Ƿ񵥻�ģʽ
BOOL  DB_InDirectDBMode();   // �Ƿ����ݿ�ֱ��ģʽ,�͵�����һ��,����ʹ�ñ������ݿⱸ��
void  DB_SetLocalDBMode(int mode); //// 0: nothing  1: ��������ͬ����ģʽ  2: ���ؼ���ģʽ(������)
int   DB_GetLocalDBMode();
void  DB_SetLocalDBToSyncMode(); //����Ϊ����ͬ����ģʽ
void  DB_SetLocalDBToActiveMode();//����Ϊ����ģʽ
void  DB_SetLocalDBToIdleMode(); // �������ݿ�����ģʽ��ԭ��ģʽ
BOOL DB_LocalDBStatusOK(); //�жϱ������ݿ��Ƿ����
int   CallMessageLoop();// ������Ϣѭ��


//////////////////////////// ����GPC����Ӧ,CALL BY  forms
BOOL  DB_LoadTGInitData(int con_id,int schd_type,int shift_id,unsigned long starttime,unsigned long endtime);
BOOL  DB_ProcessTGRequestMessage(BYTE *ptr,int msglen);
BOOL  DB_RequestBasicSchedule(int con_id,int startdate);
BOOL  DB_RequestWorkSchedule(int con_id,int shift_id,unsigned long  starttime,unsigned long endtime);
BOOL  DB_RequestHistorySchedule(int con_id,int shift_id,unsigned long  starttime,unsigned long endtime);
BOOL   DB_RequestSideBlockInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestAbsBlockInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestSideInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestAbsSlowMoveInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateSideInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateAbsInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateAbsSlowMoveInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateSchdule_SingleTrain(int con_id, int schd_type,int oper_type,int change_sub_type,const char *train_id,CSingleTrain *pTrain);
BOOL   DB_UpdateSchdule_TrainID(HWND hwnd, int con_id, int usSchdType,unsigned long train_index,const char *oldid,const char *newid);
BOOL   DB_UpdateSchdule_BlockMark(int con_id, int schd_type,int oper_type,const struct _tagMarkItem *pItem);
BOOL   DB_UpdateSchdule_DeleteTrain(int con_id,int schd_type,unsigned long train_index);

/////////////  ͬ����������������, call by db_msgproc.cpp ///////////
int Gen_TG_MARK_ITEM_Message(int con_id,int sche_type,int oper_type,const struct _tagMarkItem *rec,BYTE *buf);
BOOL  DB_SyncSchedule_SingleTrain(int con_id,int schd_type,CSingleTrain *pTrain,BOOL delete_first=TRUE);
BOOL  DB_SyncSchedule_DeleteAll(int con_id,int schd_type);
BOOL  DB_SyncWorkSchedule_CurrentWorkDocTrains(unsigned long train_index,BOOL delete_old=TRUE); //�Ƿ���Ҫɾ���ɼ�¼��������������Σ�������ɾ�����������ܴ�
BOOL  DB_SyncWorkSchedule_CurrentWorkDocMarks(int con_id,CMultiAbsMark *pMarks,unsigned long mark_index);
BOOL  DB_SyncSchedule_Mark(int con_id,int schd_type,const struct _tagMarkItem *pItem,BOOL delete_first);
BOOL  DB_SyncSchedule_DeleteTrain(int con_id,int schd_type,unsigned long trainIndex);
BOOL  DB_SyncSchedule_DeleteMark(int con_id,int schd_type,unsigned long mark_index);
BOOL  DB_RestoreWorkScheduleToCAD(int con_id,unsigned long  starttime,unsigned long endtime);
BOOL  DB_ProcessServerTGMessage(int source_eid,BYTE *ptr,int msglen);

///////////// ��̬���ݲ�ѯ����, call by dialogs,forms...
int   DB_GetPasswayInfoCount(int con_id);  
BOOL  DB_RequestTrainTypeInfo(TRAIN_TYPE_INFO  type_info[],int *limit);
///////////// ��ǰ��ӳ�����ȫ�ֵ�DOC�в��Ұ������ڣ� ///////////////
BOOL  DB_AddRoutedTrain(int con_id,int iRouteNo,int iDateTime,const char *strTrainId,int iStartFlag,int iEndFlag ); //����·�ӳ�

///////////// ������
BOOL DB_UpdateLineupSchedule(CLBTH::MsgGroupTrainWorkFormat *pMsg);
BOOL DB_DeleteAllLineupSchedule(int con_id,int shift_id);
BOOL  DB_RequestLineupSchedule(int con_id,int shift_id,HWND hwnd); // ��ʱͨ����Ϣ�ķ�ʽ�����ؽ��
BOOL  DB_UpdateNoteInfo(int con_id,int shd_type,int shift_id,const char *notestr,int notelen);
BOOL  DB_RequestNoteInfo(int con_id,int shd_type,int shift_id,HWND hwnd); // ��ʱͨ����Ϣ�ķ�ʽ�����ؽ��
BOOL DB_UpdateTrainSubaoInfo(CLBTH::MsgSubaoFormat *pMsgSubaoFormat,HWND hwnd);
BOOL DB_QueryTrainSubaoInfo(CLBTH::MsgSubaoQuery *pMsgSubaoQuery,HWND hwnd);
BOOL DB_QueryStationStoreCarInfo(CLBTH::MsgSimpleStationCarQuery *pmsg,HWND hwnd);
BOOL DB_UpdateStationStoreCarInfo(CLBTH::MsgSimpleStationCarFormat *pMsgSimpleStationCarFormat,HWND hwnd);
BOOL DB_QueryStationStoreCarInfo_Simple(int stationIndex,int phaseIndex,struct _tagStationStoreTrain *pinfo);
BOOL DB_UpdateStationStoreCarInfo_Simple(int stationIndex,int phaseIndex,time_t inputTime,const struct _tagStationStoreTrain *pinfo,HWND hwnd);
BOOL DB_DelteStationStoreCarInfo(CLBTH::MsgSimpleStationCarFormat *pBuf);
///////////// NEW /////////
BOOL  DB_PollingData(void);
BOOL  DB_ProcessServerCommStatus(long server_id,BOOL  active_flag);

int   DB_GetEntityStatus(long eid);
void  DB_SetEntityStatus(long eid,int status);

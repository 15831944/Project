#pragma once
#include <vector>
#include "caddata.h"
// ������section�޹ص�����
class CCADDataServer;
class CGlobalTGData{
       vector<CADDATA_ABSTIME>  m_listAbsTime;
	   vector<CAD_TRAIN_TYPE_INFO> m_arrayTrainType;
       

	   BOOL m_bDataOK;
  public:
	CGlobalTGData();
	~CGlobalTGData();
  public:
	  BOOL DataOK() {return m_bDataOK;}
      BOOL ProcessServerMessage(int msg_type,const char *msg,int msglen);
	  BOOL LoadFromDatabase(CCADDataServer *pDatabase);
};


BOOL CGlobalTGData::LoadFromDatabase(CCADDataServer *pDb_svr)
{
	pDb_svr->Query_ReadTrainType();
	pDb_svr->Query_ReadPasswayInfo(con_id);
	delete pw;
	pw=new CDlgWait("���ڴ����ݿ��ж�ȡ��������ʱ����Ϣ......");
	pDb_svr->Query_ReadAbsTimeInfo(con_id); 

}
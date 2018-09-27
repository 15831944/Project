#ifndef SINGLETRAIN_FILE_H
#define SINGLETRAIN_FILE_H

#include "tg_structdef.h"
#import  <msxml.dll> named_guids

#include "IF.h"
#include "tgtype.h"
#include "messages.h"

using namespace MSXML;
typedef DWORD MARK_KEY;
#define IsFlagPass(rec)            (rec.arrive == rec.depart) 
#define IsFlagStop(rec)            (rec.arrive != rec.depart)

struct PTofTrainID
{
	int stationno;
	BYTE layer;
	CPoint ptActual;
	void init()
	{
		stationno = 0;
		layer = 0;
		ptActual.SetPoint(0,0);
	}
	void Set(const PTofTrainID& rhs)
	{
		stationno = rhs.stationno;
		layer = rhs.layer;
		ptActual = rhs.ptActual;
	}
};

struct TRAINPERPOINTINFO
{
	WORD   record_index;
	WORD   start_depart_flag;
	CPoint point;
	int    nSubSection;
	void init()
	{
		record_index = 0;
		nSubSection = 0;
		start_depart_flag = 0;
		point.SetPoint(0,0);
	}
	void Set(const TRAINPERPOINTINFO& rhs)
	{
		record_index = rhs.record_index;
		start_depart_flag = rhs.start_depart_flag;
		point = rhs.point;
		nSubSection = rhs.nSubSection;
	}
};

const int MAX_REC = 300;
const int MAX_DAY_OF_BASIC_TRAIN = 5; // ����ͼ�г������������
class  CBaseTrain
{
public:
	CBaseTrain();
	CBaseTrain(const CBaseTrain& sSource);
	CBaseTrain(CString trainId, ULONG nTrainIndex);
	CBaseTrain(CString trainId,int nDirectFlag = -1);
	CBaseTrain& operator=(const CBaseTrain& sSource);
	virtual ~CBaseTrain();

	void InitSingleTrain();
private:
	void CopyTrain(const CBaseTrain& sSource);
	
public:
	CPoint GetContinuePoint( );
	bool  IsSameSelRect(int n, int m);
	
	void OffsetPosition(int x);
public:
	int m_nDirectFlag;
	int m_nTrainType;  // �ͻ���

	int m_nPointCount;
	TRAINPERPOINTINFO m_pPerPointInfo[MAX_REC];
		
	PTofTrainID stPttoTrainid[16];
	int ptoftrainid_count;	

	int days;
	CRect myPosition[MAX_DAY_OF_BASIC_TRAIN];

public:
	// �Զ�����ģʽ��,��¼�ϴ��г�����ʱ�Ŀ�ʼʱ��
	int m_nUpdateTime;

public:
	void InitShowTrainIDRect();
	void AddShowTrainIDRect(CRect& rect);
	bool IsInShowTrainIDRect(const CPoint& pt);
	void AddTrainTerminalRect(int uStartX, int uStartY, int uEndX, int uEndY);
	bool IsInTrainTerminalRect(const CRect& tmprect);
private:
	// �г��յ�����λ�þ���,
	int m_nTerminalMarkedRectCount;
	CRect m_nTerminalMarkRect[8]; 
	// ���κ�λ�þ���,��������Ƶ���������ʾ������Ϣ
	int m_nShowTrainIDNum;
	CRect m_nShowTrainIDRect[8]; 
};

#endif
#if !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
#define AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WintgDoc.h : header file
//
#include "WinDef.h"
#include "Tg_TRAIN.h"
#include "TG_Manage.h"
#include ".\chezuhaomanger.h"

#include "tg_message_define.h"
#include "TGCtrl.h"

#include "tgtype.h"
#include "stationDef.h"
#include "traintypecfg.h"

#include <vector>  

#define ERR_NOT_FIND -1
#define FIND_ONCE	1
#define FIND_TWICE 2


#define MINIMUM_UNIT	25
#define MAX_STATION_PER_SECTION         128

const int SCHEDULE_TRAIN_TYPE = 4;
const int MAX_SECTION_NUM = 64;
const int CTC_STATION = 1;
const int TDCS_STATION = 0;
const int NONE_SYSTEM_STATION = -1;
#define MAX_SIDES	255

//#define DRAW_TRAIN_RECT
#define X_TIME_OFFSET		0
#define ABS_DESP_MARK		1
#define REVERSE_MARK		2
#define MAX_ENGINEER_NUM	8

const DWORD STAY_TO_PASS		= 0x00000001;
const DWORD PASS_TO_STAY		= 0x00000002;
const DWORD TRAIN_ATTRIB_CHANGED= 0x00000004;
const DWORD FUGUA_ENGINE		= 0x00000008;

const DWORD CHANGE_TRAINID = 0x00000010;
const DWORD HAT_TRAINID    = 0x00000020;
const DWORD ZHEFAN_TRAINID = 0x00000040;
const DWORD MODIFY_SIDE    = 0x00000080;

const DWORD SET_ABS_TOP = 0x00000100;
const DWORD CONTINUE_DRAW_TRAIN = 0x00000200;
const DWORD MUTI_REC_STAY_TO_PASS = 0x00000400;
const DWORD DEPART_BY_PLAN = 0x00000800;

const DWORD DEPART_BY_MEET = 0x00001000;
const DWORD TRAIN_PLAN_PARA = 0x00002000;
const DWORD HELP_TRAIN = 0x00004000;
const DWORD TURN_REALITY = 0x00008000;

const DWORD MOUSE_TURN_REALITY = 0x00010000;
const DWORD MODIFY_TRAIN_TIME = 0x00020000;
const DWORD JIE_JIAOLUT_RELATION= 0x00040000;

#define VERTICAL_SPACE                  80 

// ����״̬
#define NORMAL_STATUS			0
#define INSERT_TRAIN			1
#define MODIFY_TRAIN			2

#define INSERT_TRAIN_GOON		4
#define INSERT_TRAIN_RETURN		5
#define INSERT_TRAIN_ABS_A		6     //ǰ�е�����
#define INSERT_TRAIN_ABS_B		7    //�����䷵��

#define RECEIVE_POINT_STATUS	8
#define WAIT_STATUS				9

// �����״̬
#define  DRAW_MARK_ITEM_STATUS  20
#define  MODIFY_MARK  21
#define  MOVE_MUTI_TRAINS   22
#define  MUTI_TRAINS_SEL_INIT    23
#define  MUTI_TRAINS_SEL_ING    24

#define  ABS_INHIBIT_MARK   1//�������
#define  ABS_FIX_MARK       2//�����ۺ��촰��
#define  ABS_SLOW_MARK      3//��������
#define  SIDE_SLOW_MARK     4//�ɵ�����
#define  SIDE_INHIBIT_MARK  5//�ɵ�����

#define  GRAPH_NOTE_MARK    7//ͼ��ע��
#define  TEXT_NOTE_MARK     8//ͼ��ע��-������
#define  SELF_CLOSE_MARK    9//�任����
#define  SIDE_TIANCHUANG_MARK     10//�ɵ��촰

#define	 ABS_ELE_FIX_MARK	11	//�������������
#define  SIDE_OCCUPY_MARK   12  //�ɵ�ռ��
#define  SIDE_LOST_ELE_MARK   13  //�ɵ�ͣ��

#define  NEW_GRAPH_NOTE_MARK   15//��ͼ��ע��
#define	 NEW_ABS_ELE_FIX_MARK	16//���ݵ�������

using namespace std ; 

struct STATISTIC_FLAG
{
	enum{
		LATE=0,
		EARLY=1,
		RIGHT=2,
	};
};
struct POS_FLAG
{
	enum{
		UP	 = 1,
		DOWN = 2,
	};
};

struct MASTERROUTE
{
	int nRouteSetNo;
	int nRouteNo;
	int nAllRouteNo;
};

struct CSideInfo 
{
  int side_number;
  bool fhighlow;
  char sidename[15];
  BYTE  nPlatform; // վ̨ 0:�� 1:�� 2:��
  short side_property;
  short direct; // 1:����; 2:����;
  CSideInfo()
  {
	  fhighlow=false;
	  nPlatform=0;
	  side_number=0;
	  side_property=21;
	  direct=0;
	  memset(sidename,0, sizeof(sidename));
  }
};

struct DispDistance
{
	int   sub_section;
	int   station_number;
	char  station_name[MAX_STA_NAME_LEN];
	CString distance;
	float deltadistance;
	
	BYTE number_of_side;
	CSideInfo side[MAX_SIDES];

	bool fHigh;
	
	WORD  up_line_count;
    WORD  up_line_list[LINE_MAX_CNT];

	WORD  down_line_count;
    WORD  down_line_list[LINE_MAX_CNT];

	BYTE  show_sation_sep;
	WORD  show_sides_mode ;  
	BYTE  inhibit_sametime_dir;
	BYTE  isNeedThick; 
	BYTE  electric_arms; // 0:������(Ĭ��) 1:�� 2:�� 3:����
    
	int   nStaProperty; // ��վ���� 1: CTC��վ 0: TDCS��վ -1: ���豸
	bool  bShowADTime;  // ��������ʾ
    
	int   nStationYPos;
 	int   nSideYPos[MAX_SIDES];
	int   nSecondStationYPos;
	
	WORD  usStationStatus;
 	WORD  usLircStatus;
    BYTE  nRouteCtrlMode;
    BYTE  nStationCtrlMode;

	BYTE  nStpcPlanStatus;
	BYTE  nLircPlanStatus;

	DWORD clStpcFlag;
	DWORD clLircFlag;
	long  clrStationLine;

	int  entry;
	int  exit;
	int  m_nCzhPrintFlag;
	DispDistance()
	{
		sub_section = 0;
		station_number = 0;
		electric_arms = 0;
		number_of_side = 0;
		
		nStpcPlanStatus = 255;
		nLircPlanStatus = 255;
        
	    clStpcFlag=255;
	    clLircFlag=255; 
		clrStationLine=-1;
		entry=0;
		exit=0;
		m_nCzhPrintFlag=0;
		show_sation_sep=0;
		show_sides_mode=0;
		inhibit_sametime_dir=0;
		nStaProperty=0;
		deltadistance=0.0;

		memset(station_name,0,sizeof(station_name));

		up_line_count=0;
		down_line_count=0;
		for(int i=0; i<LINE_MAX_CNT; i++)
		{  
			up_line_list[i]=0;
			down_line_list[i]=0;
		}
	}
};

struct VIEW_PORT
{
	enum
	{
		TIME_FLAG_VIEW_HEIGHT = 32,
		STATION_VIEW_WIDTH1 = 150,
		STATION_VIEW_WIDTH2 = 200,
	};
};

struct DRAW_LINE_STRUCT
{
	BYTE type;
	COLORREF clrLine;
	COLORREF plancolor;
	BYTE nPenWidth;
	bool bExitEngine;
	bool bAbsLine;
	int nEngine;
	bool bSchdLine;
};

enum RESULT
{
	SAME=0,
	NOT_SAME=1,
	UNKNOW=2,
};

struct Entry_Exit_Set
{
	int station;
	int nextstation;
	int line_num;
	Entry_Exit_Set()
	{
		station=0;
		nextstation=0;
		line_num=0;
	}
};

class  CTGForm;
class  CTGDoc : public CDocument
{
protected: 
	// protected constructor used by dynamic creation
	CTGDoc();          
	DECLARE_DYNCREATE(CTGDoc)

public:
	void SetTgCtrlPtr(CTGCtrl* ptgctrl);
	CTGCtrl* m_ptgctrl;
	CTG_Mange* myPDataManage;

public:
	COLORREF m_nManualRealColor; // �˹�תʵ�� ��������ʾ��ɫ
	int m_nTrainLeapLineFlag;
   
	BOOL m_bShowAdjustErrorStatus;
	BOOL m_bShowErrorStatus;
	
	BOOL m_bShowConflict;

	BOOL m_bShowOPMSTrainsInfo;
		
	BOOL m_bStopStationWaveLine;
	
	bool m_bShowVirtualStation;  //��ʾ���⳵վ
	bool m_bShowDepartFlag;		//��ʾ����ʱ���Ǻ�
	BOOL m_nShowWorkTechFlag;		//��ʾ��ҵ����
	bool m_nShowWorkSideFlag;		//��ʾ��ҵ�ɵ�
	bool m_nShowErrWorkSideFlag;	//ʼ����ʾ����ɵ�
	BOOL m_nShowElecLocoFlag;   //��ʾ����ǣ����
	bool m_bUseStringKiloMeter;  //�Ƿ�ʹ���ַ����Ĺ����
	
	BOOL m_bShowRouteNotAutoTrigerFlag;
	BOOL m_bShowRouteTrigerOkFlag;
	bool m_bShowSelCZHLine;
	bool m_bShowAllCZHLine;
	bool m_bShowEngineCrewMenu; // ������������˵�
	bool m_bShowPlanNoteText; // �Ƿ���ʾ�ƻ�̨�·����ص�����
	int m_nPageHeight;
	int m_nPageWidth;

    //����ʵ�����������ٽ��ƻ����Ĺ���
	BOOL m_bLimitDistrictFunction;
	//��ʾ����ϻ�ʱ��
	BOOL m_bShowFastOrSlowTime;
	//��ʾ�ֽ�ڣ�����վ�����ʱ�� �׶μƻ�
	bool m_bShowAcceptTruckLate;
	bool m_bShowAcceptTrainLate;
	//����ͨ��
	BOOL m_bByPassFlag;
	//�Ƿ���ʾ�����г���־
	BOOL m_bShowReverse;
	
	//�Ƿ���ʾ�����
	BOOL m_bShowdistanceStone;
	// ��ֹ�޸�ʵ�ʵ�
	BOOL m_bInhibitEditRealTime;
    //ʵ�ʵ��Ƿ�����޸�
	BOOL m_bEditRealLine;
    //�Ƿ����ɾ�����Ĳ���
	bool m_bDeleteMutiTrain;
	//�Ƿ����ɾ�����Ĳ���
	bool m_bMoveMutiTrain;

    // ��ʾɸѡ���
	BYTE m_nShowFlag;

	// �Ƿ����վ���־
	bool m_bCalcStationIndex;
	// ��ʾ�۷�����
	bool m_bShowReturnTrainid;
public:
	int GetShiftId();	
	long m_nShiftDate;
    int  m_nShiftId;
	int  m_nShiftNo;

	ULONG m_nBeginTm;
	WORD  m_nAutoMove;
	WORD  m_nHistoryLen;
	
	int m_fPixelPerTenMinute;
	int m_fPixelPerHalfHour;
	int m_fPixelPerMinute;
	int m_fPixelPerHour;
	
	int m_nViewFormat; 

	TIME m_TGGraphStartTime;
	int mGraphBeginPixel;
	int mGraphEndPixel;
 	
	int m_nAdjustStartX;
	int m_nAdjustEndX;

	int m_nCurrentSectionNo;
	
	BOOL m_bGoodTrainNotTechStopNeedPwd;
	int m_nCurrentStationList[MAX_STATION_PER_SECTION];
	int m_nStationNum;
		
	int m_nDispDistanceNum;
	DispDistance m_sDispDistance[512];

public:
	CTG_TRAIN  m_myFocusTrain;
	CTg_BLOCK  m_myFocusBlock;
	
	int nCurrentX; //��ǰʱ��x����
    int nCurrentMouseX; //��ǰ�������λ��

	int m_nCurrentNodeIndex;			 //������ڵ�nodeIndex
	int m_nCurrentStationNoOfMouse;     //������ڵĳ�վ��

	CFont fontText;
	CFont fontTech;
	CFont fontSide;
	CFont fontSecondTime;
    CFont fontADTime;
	CFont fontADTimeBold;
	CFont fontAbsTime;
	CFont fontEarlyLastTime;
	CFont fontAnalyTime;
	CFont fontPressure;
	CFont mBlockFont;
	CFont mBlockFontTime;
		
	CBrush m_brushBlack;
	CBrush m_brushWhite;
	CBrush m_brushBlue;

	CFont fontReturnTrain;
	CFont fontNomorlTrain;
	CFont fontElecTrain;
	CFont fontSpaceTrain;
	CFont fontLengthTrain;
	CFont fontMilitaryTrain;
	CFont fontSuperTrain;
	CFont fontEngine;

	CPen  mLeftPen;
	CPen  mRightPen;

	int m_nVerticalPos;
	int m_nHorizontalPos;

	int m_nOperatorStatus;
	
	enum TRAIN_DIRECT
	{
		UP      = 1,
		DOWN	= 2,
		DUAL	= 3,
	};

	enum FOCUS_POINT
	{
		POINT_PLAN =0,       //�ƻ���
		POINT_REAL,          //ʵ�ʵ�
        POINT_ROUTE_OK,      //��·�Ѵ���
		POINT_ROUTE_NOT_OK,  //��·δ������
        POINT_INIT           //��ʼ��
	}focus_point;
	
	int m_nADTimeSystle;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitTGState();
	int GetTrainIds(CStringArray& trainidary, CTG_TRAIN *train);
	int GetTgType();
	int GetTgTermType();
	int GetDispDistanceIndex(int Pointy);
	const DispDistance* GetDispDistance(int staid);
	DispDistance* GetDispDistanceByIdx(UINT index);
	
	int GetHistoryRegionWidth();
	void SetGraphBeginEndPixel();
	void DrawChordLine(CDC* pDC,POINT pt1,POINT pt2,int dir);
	
	void DrawBasicStationStopLine(CDC* pDC,POINT ptlStart,POINT ptlEnd);
	void DrawSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
	void DrawBasicSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
 	void DrawActualLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
	void DrawLine(CDC* pCDC, const DRAW_LINE_STRUCT& param,const CPoint& ptlStart,const CPoint& ptlEnd);
  
    WORD GetLineColor(const int factor, const BYTE nFlag, COLORREF& schd_color, COLORREF& real_color, COLORREF& vert_color);
	void ResetLineWidth();
	
 	
 	long ConvertXPostoTime(int xPos);
	int  ConvertYPostoStationNo(int yPos,int& node_index,TgTRAIN_RECORD *last=NULL);
    int  GetStationSideNo(int yPos,int node_index);

	void SetStaticTimeByCfg();
	void PointBetweenMidOfTwoStation(int yPos, int& sta1, int& sta2);
    int  PointBetweenMidOfTwoStation(int yPos, TgTRAIN_RECORD *rec, int& direct);
    
	int  ConvertYPostoPrimarySide(int yPos,int &nRouteSetNo);
	
	CString QueryCurrentTrainStationAndside(CPoint point,ULONG index);
	ULONG QueryCurrentBlock(const CPoint& point);

	void DrawSingleTrain(CDC *pCDC,CTG_TRAIN *single,int nDrawMode,bool bShowSecond=false);
	void UpdateBackgrond(CDC* pCDC, CRect& rect);
	void DrawSeperatorBar(CDC *pCDC,long lColor);
	bool IsCanModifyTrainStation(const TgTRAIN_MORE_INFO& train_info);
	bool InitStationVerticalPos(bool bInitStaDistance, bool bWholeScreen);
	bool IsGoodTrainNotTechStopNeedPwd();
	void ReadStationSide();
	
	bool EntrySideConnect(int station, int side, int exit);
	CStationDef *m_pStationDef[MAX_STATION_PER_SECTION];//2007.04.18 ���Ӷ�����ɵ�����
	BYTE GetLateTime(long plan_time, long real_time, CString& strDeltm);

	bool IsOnlyShowMark(void) const;
	bool IsOnlyShowTrain(void) const;
	bool IsShowTrainAndMark(void) const;

	void SetOnlyShowMark(void)		{ m_nShowFlag=1;}
	void SetOnlyShowTrain(void)		{ m_nShowFlag=2;}
	void SetShowTrainAndMark(void)	{ m_nShowFlag=3;}
public:
	CString QueryCurrentTrain(CPoint *point,ULONG& index);
	void InitEarlyLastTime(CTG_TRAIN* single);
	void CreateGDIInfo(CDC* pDC);
private:
	void GetRrcArlRect(CPoint pt, CRect& rect, int nFlagX, int nFlagY);
    void GetRrcRect(CPoint pt, double dltX, double dltY, CRect& rect, int nFlag);
	virtual ~CTGDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	void SetAbsReturnFlag(CTG_TRAIN* train);
	void SetBasicTrainDate(CTG_TRAIN* train);
	void ResetBasicTrainDate(CTG_TRAIN* train);
	void SendAppendMark(CTg_BLOCK* block);
	void SendUpdateMark(CTg_BLOCK* block);
	void SendAppendMark(CTg_BLOCK& block);
	void SendUpdateMark(CTg_BLOCK& block);

	void SendAppendTrain(CTG_TRAIN* train, CString strChangeTrainId, BYTE nRunType);
	void SendUpdateTrain(CTG_TRAIN* train, BYTE Flag = SCHD_UPDATE, DWORD nFlagEx = 0, WORD sta = 0, WORD rec_index = 0, CString trainid = "");
	void SendUpdateTrain(CTG_TRAIN& train, BYTE Flag = SCHD_UPDATE, DWORD nFlagEx = 0, WORD sta = 0, WORD rec_index = 0, CString trainid = "");
	void SendDeleteTrain(CTG_TRAIN& train);
	CTG_TRAIN* GetTrainByIndex(long trainIndex);
	CTG_TRAIN* GetTrainByTrainId(CString trainid, UINT& nFindNum);

	void SendDeleteTrainXml(ULONG train_index);
	void SendDeleteTrainXml(std::vector<ULONG>& selectedTrains);
	void SendDeleteBlockXml(ULONG block_index, int flag=1);  // flag:0������ʾ�Ի��� 1ֱ��ɾ��
	
	BOOL GetOperatorPriorByDistrict(int xMouse,WORD focus_pointInfo = POINT_PLAN);
	int  GetNodeIndexByAbs(int curNodeIndex, int abs_num,int dir);
	int  GetNodeSubSectionByIndex(int index);
	
	void DrawLanzhouMarkOfTractionEngine(CDC* pCDC,CTG_TRAIN* single,int nDirect);
	void DrawMarkOfTractionEngine(CDC* pCDC,CPoint pBasic,CSize trainidSize,CTG_TRAIN* single,int nDirect);
	int  GenerateRecordsBetweenNodes(int sectionNo,int preNodeIndex,int nextNodeIndex,CTG_TRAIN* single,long curTime);
	
	int  GetIncreaseLayer(CTG_TRAIN* single,const CPoint& ptlOrig,int nDirect,int statinno, const CSize& trainidSize);
	
	int  GetMultiNodeJudgeIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec,int& index,int forwardindex,int reverseindex,WORD usFlag,int section,BYTE bJudgeFlag=3);
	int  JudgeMultiNodeIndex(CTG_TRAIN * single,TgTRAIN_RECORD* rec,int& index,WORD usFlag,int section);
	int  JudgeMultiNodeIndexbySeq(CTG_TRAIN * single,TgTRAIN_RECORD* rec,CList<int,int>& list,TgTRAIN_RECORD* _start_rec,TgTRAIN_RECORD* _end_rec);
	int  GetNodeIndexBySubSection(int stationNo,int subsection);
	
	BOOL AdjustRectSizeByBasicPoint(CRect &orignRect, CString drawText,CPoint basicpt,int workThread=0);
	bool JudgeNeedWaveLine(TgTRAIN_RECORD *rec, int nDirectFlag = DOWN_DIRECT);
 	void DrawByPassFlag(CDC * pCDC,CTG_TRAIN *single);
	void DrawTrainFocusRect(CDC*pCDC, CTG_TRAIN* single);
	void DrawFastOrSlowTime(CDC *pCDC, TgTRAIN_RECORD *rec, POINT &ptlStart, POINT &ptlEnd,BOOL isFast,int minute,int dir = 2);

	int  ConvertYPostoStationNos(int yPos, int &node_index, int &nStationNum);
	BOOL PtInRects(RECT rects, POINT pt);   

	WORD GetEntryFromNodeIndex(int node_index, int dir, int abs_line[]);
	void GetEntryExitFromNodeIndex_abs(int node_index,WORD& entry,WORD *exitlist, WORD& exitnum,CString *sExplain,WORD* entry_next,WORD* station_next,int dir,int next_station=0);
	void GetEntryExitFromNodeIndex(int node_index,int& entry,int& exit,int dir = DOWN_DIRECT);
 	void GetExitEntryFromTwoNodeIndex(int prev_node_index,int node_index,int& exit,int& entry,int dir = DOWN_DIRECT);
    BOOL JudgeNodesConnectedThroughThirdParty(WORD cur_node_index,WORD pre_node_index,int &replace_node_index);
	// ��ȡ���俪���г�,������������߱�
	int GetEntryByNodeIndexForFrom_abs(int node_index, int station, int dir, int abs_line_list[32]);
	// ��ȡ���������г�,���ó��������߱�
	int GetExitByNodeIndexForTo_abs(int node_index, int nstation, int dir, int abs_line_list[32]);
	int GetExitByNodeIndex12ForTo_abs(int node_index1, int node_index2, int abs_line_list[32]);
    // �ж�һ����վ�Ƿ��ڱ�̨��
	bool IsThisSectionStation(int station);
	BOOL JudeNodesConnected(WORD node_index1,WORD node_index2);
   
	int  GetNodeExitIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section);
	int  GetNodeEntryIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section);

    int  GetEntryExitDirection(CTG_TRAIN * single,TgTRAIN_RECORD* rec,WORD usFlag,int section);

	
	CFont* GetDrawTrainNoFont(const TgTRAIN_MORE_INFO& nTrainInfo, CString& strTrainId, bool& bDrawRect);
	
	void DrawMarkOfStart(CDC *pCDC,CTG_TRAIN * single, CString strTrainId,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width, TgTRAIN_RECORD* rec);
	void DrawMarkOfArrive(CDC *pCDC,CTG_TRAIN * single,CString strTrainId,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width, TgTRAIN_RECORD* rec);
 
	void DrawStopOutofSignalMark(CDC *pCDC, CPoint ptlStart, CPoint ptlEnd, int direct);
	void DrawMarkOfDepart(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width,const TgTRAIN_RECORD* rec);	
	void DrawMarkOfEnd(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect, COLORREF line_color,USHORT flag, int line_width);
	void DrawMarkOfEndLittle(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect,COLORREF Color, USHORT flag, int line_width);
    void DrawMarkOfZheFanTrainID(CDC *pCDC, CPoint ptlStart, int nDirect,COLORREF Color, CString& trainid,CTG_TRAIN * single);
	
	long  GetXPositionConsiderSecond(long nTime);

	long  GetOffsetMinuteFromGraphBegin(long nTime);
	long  GetOffsetPixelFromGraphBegin(long nTime);
	
	void SetGraphStartTime(ULONG nShiftId);
	void SetGraphStartTime_New(ULONG nShiftId);
 	
	void DrawArrivalTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag=0);
	void DrawDepartTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag=0);

	
	void DrawTrainWorkSide(CDC * pCDC,CTG_TRAIN * single,int nDrawMode);
	void DrawTrainWorkText(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainNoteText(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainGiveoutDirection(CDC * pCDC,TgTRAIN_RECORD * prec,int direct_flag, const CPoint& ptlStart,CTG_TRAIN * single);
	void DrawTrainAcceptDirection(CDC * pCDC,TgTRAIN_RECORD * prec,int direct_flag, const CPoint& ptlStart,CTG_TRAIN * single);

	void DrawTrainTechDepartMark(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainTrigerMark(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainRouteOKMark(CDC * pCDC,CTG_TRAIN * single);
    
    int  GetStationVPos(CTG_TRAIN * single, TgTRAIN_RECORD* rec,int usSideFlag,int usFlag = EXIT_POSITION);
	int  GetStationVPos(int dist_index, int usSideFlag);
	int  GetStationSideVPos(const TgTRAIN_RECORD* rec, const int index, const int side_no);
	int  GetAbsYPosition(CTG_TRAIN * single, const int rec_index, WORD position);

	void GetSepState(int index, bool& bIndexSep);
	//{{AFX_MSG(CTGDoc)
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DrawTrainBenqianTime(CDC *pDC, CTG_TRAIN * single, TgTRAIN_RECORD* first, const CPoint& ptlEnd, CRect& tempRect);
	void DrawBadMark(CDC *pCDC, CTG_TRAIN * single, const CPoint &ptlStart, const CPoint &ptlEnd, const TgTRAIN_RECORD* first);
    void DrawNoMeetMark(CDC* pCDC, const CPoint& ptlStart, const CPoint& ptlEnd, long no_meet_flag, COLORREF clrText, CString strTrainID);

	void DrawTrainDecorateGreenLine(CDC* pCDC, const CPoint& ptlStart, const CPoint& ptlEnd);
	void DrawUnknowTrainFlag(CDC *pCDC, CTG_TRAIN * single);
	
public:
	bool FindTrainByTrainIndex(long& position, DWORD& train_index, const CString& trainId);
	int  FindTrainByTrainId(const CString& trainId, long postion[], long indexlist[]);
	int	 FindTrainByTime(DWORD btm, DWORD etm, ULONG train_index[]);
	BOOL IfRecordInTimeScope(TgTRAIN_RECORD *record, DWORD begin_time, DWORD end_time);
	BOOL IfTrainInTimeScope(CTG_TRAIN *ptrain, DWORD begin_time, DWORD end_time, BOOL IsStart);

	//�����޸Ķ೵�������˻�ʱ��
	BOOL IfTrainInStationScope(CTG_TRAIN *ptrain, int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time);

	bool GetOperatorRight(BYTE oper);	
	CString GetDataPath() { return m_ptgctrl->mConfigTg.m_context.GetDataPath();}
	CString GetStationName(int stano);
	CString GetSideName(int stano, int sideno);
	int  GetStationProperty(int stano);
	
	CString QueryTrainFromAndToStation(CTG_TRAIN* single);
  
public:
	int  GetAbsByStationNoAndEntry(WORD station, WORD entry);
	int  ConvertAbsnumStationToEntry(WORD station,WORD abs_num);
	bool ConvertStationEntryToAbsnum(WORD station, WORD entry_num,WORD *abs_num,WORD *dir);
	// ���ݸ���վ�Ϳ��ӣ���ȡ��Ӧ�ÿ������������һ��վ��վ��
	int  GetAbsLineOtherStation(WORD station, WORD entry_num, WORD& line_no);
	// ���ݸ���վ�Ϳ��ӣ���ȡ��Ӧ�ÿ������������һ��վ��վ��
	int  GetAbsPreStationIndexByNextStationAndEntry(WORD station, WORD entry_num, bool bStopAbsUp, int in_station_index);
    

	bool IsCanEditTrain();	
	TIME GetBeginTime() {return m_TGGraphStartTime;}
	void SetSchdBeginTime(void);

	int  GetLineNoByStations(int from_st, int to_st);
	void GetViewScope(TIME& begin, TIME& end);
	void GetAcceptDataScope(TIME& begin, TIME& end);
public:
	void  ReGeneratetStationInOutIndex(CTG_TRAIN& train);
	bool  CheckStationInOutIndex(CTG_TRAIN& train);
	void  GenTrainExitEntryByStationIndex(CTG_TRAIN& train);
	void  SetStationInOutIndex(CTG_TRAIN& train);
	
	int  SetTrainExitByCfgForUI(CTG_TRAIN* train, int rec_index,int abslineno[]);
	int  SetTrainEntryByCfgForUI(CTG_TRAIN* train, int rec_index, int abslineno[]);
	int  SetTrainExitByCfg(CTG_TRAIN* train, int rec_index);
	int  SetTrainEntryByCfg(CTG_TRAIN* train, int rec_index);

	void  CheckABC(CTG_TRAIN& train);
	void  CheckFirstLast(CTG_TRAIN& train);
	void  SetTrainFirstRecIndex(CTG_TRAIN& train, bool bFlag);
	void  SetTrainMidRecIndex(CTG_TRAIN& train, int rec_index);
	void  SetTrainLastRecIndex(CTG_TRAIN& train, bool bFlag);
	
	int   GetTrainDirectionByTrainId(CString strTrainId);
	void  SetTrainDirect(CTG_TRAIN& train);
	void  SetTrainSideAndSchdReal(CTG_TRAIN& train);
	void  SetTrainWidthColor(CTG_TRAIN* ptrain);
	
	void  SetTrainRect(CTG_TRAIN* ptrain);
	void  ShowRect(CDC*pCDC, const CRect& rect);
	void  SetWorkTrainRect(CTG_TRAIN* ptrain);
	void  SetBasicTrainRect(CTG_TRAIN* ptrain);
	BOOL  SetABSStart(CTG_TRAIN* train, CString m_strTrainId, long usTimeValue, int ypos, std::vector<int>& abs_line_list);
private:
	COLORREF GetSecondBkColor(LONG x); // ���յ�����λ��,��ȡ��ͼ������ɫ
	CString GetTimeString(long time, CString& strSecond, bool bShowSecond);
	bool GetFlagDrawTrainTime(int stano);

	bool TwoStationSubSectionSame(int index1, int index2);
	RESULT StationInTwoSubSection(int index1, int index2);

	bool GetNodeIndexOfDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index);
	bool GetNodeIndexOfAllDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);
	bool GetNodeIndexOfGoodDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);
	bool GetNodeIndexOfPessagerDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);

	void DrawAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsZhFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsSelfCloseMark(CTg_BLOCK& block, CDC *pDC, int nDirect);

	void DrawSideInhibitMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideSlowMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideOccupyMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideTianchuangMark(CTg_BLOCK& block, CDC *pDC);

	void DrawNoteTextMark(CTg_BLOCK& block, CDC *pDC);
	void DrawNoteTextMark2(CTg_BLOCK& block, CDC *pDC);

	// ���ݻ��� G21
	void DrawLanzhouAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawLanzhouAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsLanzhouEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	
public:
	int   GetNoteChineseChar(CString strNotes, int len);
	void  SetGraphPosition(CTg_BLOCK* block, CRect rect);
	void ParseMarkReasonText(const CString& text, CString& dcmdno, CString& reason);
	CString CombineReasonText(const CString& dcmdno, const CString& reason);
public:
	bool GetABSLine(int lineNo, ABS_LINE& lineInfo);
	bool GenDefaultBlock(CTg_BLOCK& block);
	
	void SetBlockDriect(CTg_BLOCK& block);
	bool GetABSLineNoByIndex(int index1, int index2);
	bool GetIndexByStation(WORD stano1, WORD stano2, int& index1, int& index2);
	void SetDocContext();
	
	void sysprint(short msg_level,unsigned short process_id,char *format, ... );

	int  GetVPosition(int idx, int flag);
	int  GetSubSection(int idx);
	int  GetEntry(int node_index);
	int  GetExit(int node_index);
public:
	CString GetTrainShowId(CTG_TRAIN& train);	
	CString GetTrainShowId(CTG_TRAIN* train);
	CString GetTrainShowId(const int station, const TgTRAIN_RECORD& rec, CTG_TRAIN* train, bool bStar=false);

	CString GetABSStrDirectDesp(int direct, USHORT nAbsNo, USHORT nAbsNum);
	void    GetPlotParamXml(ULONG nFlag);

	CStationDef* GetStationSideDef(int station);
	void CheckTrainSideInfo(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary);

	bool  IsEnableChangeSide(HWND hWnd, TgTRAIN_RECORD *rec, CTG_TRAIN *pTrain);
	bool  SideSetEntry(HWND hWnd, int arrive_side, int depart_side, int rec_index, CTG_TRAIN *pTrain);
	bool  ArriveSideSetEntry(HWND hWnd, int arrive_side, int rec_index, CTG_TRAIN *pTrain);
	bool  DepartSideSetEntry(HWND hWnd, int depart_side, int rec_index, CTG_TRAIN *pTrain);

	bool  IsEntrySideConnect(int station, int side, int entry);
	bool  IsSideExitConnect(int station, int side, int exit);

	bool  CheckSideEntry(int arl_side_no, int dep_side_no, const TgTRAIN_RECORD* rec, int& flag);
	// ���溯������ֱ�ӵ���
	int   OffsetWorkScheduleTimeByRouteOk(CTG_TRAIN* train, int nRecordNo, int nTimeOffset, int nFlag);
private:
	bool IsMutilStation(int station);
	
	CString GetStationSidesName(const int nStaNo, const USHORT nSidesNum, const USHORT nSidesNo[]);
	bool TrainHaveActualPoint(CTG_TRAIN* train);

	int  GetEngineCount(const CTG_TRAIN *single, UINT rec_idx);

	void DrawStopSidePoint(CDC*pCDC, const CPoint& pt_side, COLORREF lColor);
	
	void DrawStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, int nDrawMode);
	void DrawStopLine3LeapLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const int lColor, int lActualColor, int nDrawMode, int in_index, int out_index);
	void DrawStopLine1(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	void DrawStopLine2(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	void DrawStopLine3(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	
	bool DrawAbsLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second, CPoint& ptStart, CPoint& ptEnd);
	void DrawAbsStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second);
	void DrawTrainStartOrAcceptMark(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const TgTRAIN_MORE_INFO& train_info);
	void DrawTrainEndOrDepartMark(CDC*pCDC, CTG_TRAIN* single,  TgTRAIN_RECORD* second);
    
	void DrawTrainRescuePart(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first);
	BOOL DrawUniteTrainMark(CDC*pCDC, TgTRAIN_RECORD *first);
	void DrawDivideOrCombineTrainMark(CDC*pCDC, TgTRAIN_RECORD *first);
   
	bool IsShowSidePos(int nSideNo, int index);

	void DrawMutiTrainSeparate(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first);
	WORD GetStationAbsPosition(CTG_TRAIN* single, TgTRAIN_RECORD* rec);
	void DrawAbsText(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd, bool bStopAbs);
	void DrawUniteTrainAbsLine(CDC*pCDC, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd);

	void DrawStatStationArrival(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point, int nDrawMode);
	void DrawStatStationDepart2(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode);

	void DrawStatStationDepart(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode);
	
	void PressureStopLine(CDC * pCDC,TgTRAIN_RECORD* first, BYTE in_index, BYTE out_index, const CPoint& ptSide, const CPoint& ptDepSide);
	void DrawEngineLine(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD* first, const CPoint& ptlStart, const CPoint& ptlEnd);
	void DrawAbsPartRunLine(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD* first, const CPoint& ptlStart);
public:
	int GetCascoIdByLocoId(int id);
	CString GetLocoNameByLocoId(int id);
    void SetBasicNoStatLateTime(CTG_TRAIN* train);
	int  GenExitEntryByInitABS(int conid, int station1, int station2, int index1, int index2, int& exit, int& entry);

	int  GetSideUseByYPos(int yPos, int index);
	int  ConvertYPostoSideUseStationNo(int yPos, int& node_index);
	void EraseTrainIndex(ULONG train_index) { m_nEraseTrainIndex=train_index;}
private:	   
	COLORREF mjcolor, mfcolor;
	TRAIN_INDEX  m_nEraseTrainIndex;
	int m_nTrainNoMaxLevel;
	CWordArray   locoId;
	CWordArray   cascoId;
	CStringArray locoPref;
	CStringArray locoName;

	int m_linecount;
	struct ABS_LINE_INFO
	{
		int  lineNo;
		int  dir;
	}m_line_list[128]; 
	long m_nFirstArlMinuteOffset, m_nSecondArlMinuteOffset;
	long m_nFirstDepMinuteOffset, m_nSecondDepMinuteOffset;
	// ����λ�ÿ�����,������ͬ
	int  m_nFirstArlSecond, m_nSecondArlSecond;
	int  m_nFirstDepSecond, m_nSecondDepSecond;
	//X.Y���ƫ����
	int x_point_offset;
	int y_point_offset;

	CString time_char;

	bool first_a_time_status,second_a_time_status;
	bool first_d_time_status,second_d_time_status;

public:
	CPoint m_selmarkpoint;
public:
	void SetDirectAtStation(CTG_TRAIN* train);
	void DrawSingleMark(CTg_BLOCK& block, CDC *pCDC);
	void FrameRectExtend(CDC *pDC,CRect &rc);
	void ClearTrainRelation(TgTRAIN_RECORD* rec);
	
	bool SetBlockDirect(CTg_BLOCK* block);
	int  GetNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list);
	void SetTimeValid(CTG_TRAIN* train);
	int  GetContextLineWidth();
	int  GetEarlyLateTime();
	bool IsShowLocoNo();
	int  GetShowTrainType();
	int  GetShowTrainDirect();
	bool IsShowStationMark(CTg_BLOCK& block, UINT nIndex, int& y1, int& y2);
    int  GetMutiNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list);
	const CTG_TRAIN* GetTrainByPoint(CPoint& pt);

	int  GetTrainDirectByTrainId(CString trainid);
	CTG_TRAIN* GetPrevTrainByIndex(long index);
public:
	struct ContinueDrawInfo
	{
		long m_nTrainIndex;
		int   m_nRecIndex;
		bool  m_bContinueDraw;
	}m_ContinueDrawInfo;
	void ResetContinueDraw();
	void SetContinueDraw(int rec_index, long trainIndex);
	bool IsContinueDrawTrain(int& rec_index, long& trainIndex);
public:
	CDWordArray  m_aryStatus;
	CStringArray m_aryEngine;
	std::map<ULONG, ULONG> m_noearlylatetime;
	bool m_bAcceptCadData;

public:
	TrainTypeCfg mTrainType;
	SPECIAL_LINE_STYLE line_style;
	bool mIsSchdLineSameToActualLine;
	bool mIsChangeVertColor;
	int  mMouseMoveMode;  // 0:�����ƶ�����ʾ�� 1��Ĭ��ֵ���뼶�ƶ�����ʾ�� 2���뼶�ƶ�����ʾ��
	int  mDisplayUnknownText;
	int  m_nBlockNum;
	bool m_bIsVerifyTrainRect;
	int  m_nTrainZoomRangeX;
	int  m_nTrainZoomRangeY;
	SPECIAL_TRAIN_ID_SHOW trainid_style;
	DECORATE_STYLE decorateStyle;
	bool m_bElecTrainDrawGreenDecorate;
public:
	void DrawBasicTrainConnectLine(std::map<ULONG, TRAIN_CZH_INFO>::iterator& it, std::map<ULONG, TRAIN_CZH_INFO>::iterator& next_it, TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	void DrawBasicTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	void DrawTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	int  GetStationSideVPos(const int index, const int side_no, int direct);
	
public:
	void ModifyTrainCZH(CString czh, CString newczh);
	void DeleteTrainCZH(CString czh);
	void AddNewTrainCZH(CString czh);
	void AddTrainToCZHGroup(CString czh, CString trainid);
	bool DelTrainFromCZHGroup(CString czh, CString trainindex);

	void AddTrainToCZHGroup(DWORD prev_train_index, CString trainid);
	void SetFormView(CTGForm* pView);
	bool GenTrainReturnTrainID(CString czh, CStringArray& text);
	void ReGenTrainReturnTrainID(CTG_TRAIN& train);
	void ReGenTrainCZHRelation(CString czh); 
	bool IsCanSetInterCityReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex);
	bool IsCanSetInterCityStartReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex);
	
	void ReGenTrainCZHRelation();
	void ReGenTrainJiaoluRelation();
	
	bool IsAbsBySide(HWND m_hWnd, BYTE sideno, int rec_index, CTG_TRAIN *pTrain, int param);
	void SetLayer(CDC* pLayer1, CDC* pLayer2, CDC* pLayer3) 
	{
		m_pLayer1=pLayer1; 
		m_pLayer2=pLayer2;
		m_pLayer3=pLayer3;
	}  // �Ż�Ч��ʹ��
public:
	void DrawAllCheZuHaoLine(CDC * pCDC);
	TRAIN_CHEZUHAO* GetCurrentSelTrainCZH();
    void SetCurrentSelTrainCZH(CString czh);
	CCheZuHaoManger myChezuhao;

private:
	CString m_CurSelCZH;
	CDC    * m_pLayer1;   // �Ż�Ч��ʹ��
	CDC    * m_pLayer2;   // �Ż�Ч��ʹ�� 
	CDC    * m_pLayer3;   // �Ż�Ч��ʹ�� 
	CTGForm* m_pView;

// �������Զ�ƽ������ʩ��
public:
	void AddMarkMoveIndex(ULONG index);
	void DelMarkMoveIndex(ULONG index);
	bool IsMarkMove(ULONG index);
	
	void ReadFromFile();
	void WriteToFile();
	CString mfilePath;
	std::map<ULONG,ULONG> mark_move_index;

public:
	CString GetThisTrainReturnTrainID(CTG_TRAIN& train, int rec_index);
	void GetJiaoluDlgName(CString& title1, CString& title2);
	void GetChezuhaoDlgName(CString& title1, CString& title2);
	BYTE GetEarlyLateTimeRd(long train_index, int station, int rec_index, CString& trainid);
	void SetLateEarlyTimeRd(long train_index, int station, int rec_index, BYTE flag, CString& trainid);

// ���Ͼ�����?��
	void SetAllTrainsErrorSide(int dis_index, int station_no);
    void TrytoFindAbsEngineReturnTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list);
    void TrytoFindAbsPartRunTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list);
	bool TrytoMoveOtherTrain(CTG_TRAIN* ptrain, CTG_TRAIN& othertrain);

	void CheckSideRelation(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary);
	void ReLayoutBmpPageSize(int nSelDistanceNo, HWND hWnd);
	void SetTrainRelationSideModify(CTG_TRAIN* ptrain);
	
private:
	std::vector<Entry_Exit_Set> mSetLineNoAry;
	std::vector<Station_Direction_Tip> mDirectionTipAry;
	std::vector<StationGiveoutTip> mGiveoutTipAry;
	std::vector<StationAcceptTip> mAcceptTipAry;
public:
	// ���˳���ʾ����
	bool FindTrainByTrainIndexForSingleTrain(long& position, DWORD& train_index, const CString& trainId);
	int  FindTrainByTrainIdForSingleTrain(const CString& trainId, long postion[], long indexlist[]);
	// ���˳�ģʽ
	bool m_bShowSingleTrain;
	bool  m_bLineBold;
	bool m_bShowWholeScreen; // �Ƿ�ȫ����ʾ
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)

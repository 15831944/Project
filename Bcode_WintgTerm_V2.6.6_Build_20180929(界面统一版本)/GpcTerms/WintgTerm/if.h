#pragma once
//#include "defines.h"

// display flag define
struct TRAIN_DISPLY
{
	enum
	{
		DIRECT = 1,
		TYPE,
		LATETRAIN,
		LINE,
		REVERSE,
		TRACTION,
	};

	struct DIRECT
	{
		enum 
		{
			UP =	1,
			DOWN	= 2,
			DUAL	= 3,
		};
	};
	struct TYPE
	{
		enum{
			FREIGHT = 1,
			PASSENGER	= 2,
			ALL	= 3,
		};
	};
	struct LATETRAIN
	{
		enum{
			LATE_FREIGHT_TRAIN = 1,
			LATE_PASSENGER_TRAIN = 2,
			LATE_ALL_TRAIN	 = 3,
		};
	};
	struct LINE
	{
		enum{
		THIN =	1,
		BOLD =	2,
		};
	};
	// draw traction position
	struct TRACTION
	{
		enum{
		NOSHOW	=	0, 
		TOP=	1,
		MIDDLE=	2,
		BOTTOM=	3,
		};
	};
	struct REVERSE
	{
		enum{
		SHOW_WAVE = 1,  // ���ܲ�����
		SHOW_REVERSE	=		2,  // ��ʾ�����г�
		SHOW_ENGINE		=		3, // ��ʾ������
		EDIT_REAL_LINE	=		4,  // ʵ�����Ƿ�����޸�
		SHOW_KILO_MARK	=		5,  // �Ƿ���ʾ�����
		BY_PASS_FLAG	=		6,  // ����ͨ����־
		CONSIDER_SIDE	=		7,
		ADJUST_AFFIRM	=		8,
		ADJUST_PART_RESULT	=	9,
		SHOW_TRAIN_ID		=	10,
		SHOW_EARLY_LATE_TIME=	11, // ��ʾ�����
		SHOW_CONFLICT		=	12, // ��ʾ��ͻ
		SHOW_ADJUST_ERROR	=	13,
		SHOW_ERROR			=	14,
		SHOW_ELEC_LOCO		=	15,
		ACCEPT_TRUCK_LATE	=	16,
		SHOW_ACCEPT_TRAIN_LATE	=17,
		SHOW_FAST_SLOW_TIME		=18,
		SHOW_TRUCK_ON_TIME	=	19, 
		SHOW_FALSE_TRAIN	=	20, 
		SHOW_TRAIN_ON_TIME	=	21,
		SHOW_WORK_SIDE		=	22,
		SHOW_ERR_WORK_SIDE	=	23,
		SHOW_WHOLE_SCREEN	=	24,
		SET_SELF_NEXT_STD	=	25,
		SHOW_BASIC_SCHD_SIDE =  26,
		};
	};
};

struct STATION
{
	enum{
		DISTRIBUTED,	// ��ɢ����
		CENTER_CTRL,	// ������ȫ����
		STATION_CTRL,	// ��վ��ȫ����
		EMERGENCY,	// �ǳ�վ��
	};
};

struct ROUTE
{
	enum{
		PLAN_CTRL,	 // ��ͼ��·
		SAVE_ROUTE,  // �洢��·
		FORBIT_AUTO, // �˹���·
	};
};


struct STATION_STATUS
{
	enum{
		ACTIVE = 0,	   // վ��
		INACTIVE,  // վ��
	};
};
// trains operator
struct TWO_TRAIN_OPERATOR
{
	enum  OPER
	{
		SPLIT_TRAIN	= 1,
		COMBINE_TRAIN = 2,
		UNITE_TRAIN = 3,
		RESCUE_TRAIN = 4,
	};
	struct RESULT
	{
		enum 
		{
			OPER_OK = 1,
			OPER_CANCEL = 2,
		};
	};
};

// draw special train flag
struct DRAW
{
	struct TRAIN
	{
		enum{
		RETURN			=1,	// �����۷�
		GOON			=2,	// ����ǰ��
		TO_ABS_TRAIN	=3,   // ������ҵ ������
		FROM_ABS_TRAIN	=4,   // ������ҵ ���䷵��
		};
	};
	struct MARK
	{
		enum{
		IF_ABS_INHIBIT		=1,	// �������
		IF_ABS_SLOW			=2,	// ��������
		IF_ABS_ELEM_FIX		=3,   // �����������
		IF_ABS_SELF_CLOSE	=4,   // ������������ʽ
		STATION_INHIBIT		=5,	// վ�ڷ���
		STATION_SLOW		=6,	// վ������
		STATION_FIX			=7,	// վ���촰��
		STATION_SIDE_OCCUPY	=8,	// վ�ڹɵ�ռ��
		TEXT_NOTE			=9,	// ����ע��
		GRAH_NOTE			=10,	// ͼ���ע
		PE_NOTE				=11,	// ���±�ע
		OTHER_MARK			=20,	// ����
		};
	};
};
struct GRAPH_ZOOM
{
	enum{
		SMALL = 1,
		NORMAL = 2,
		BIG = 3,
	};
};

struct TG_TYPE
{
	enum 
	{
		WORK_TG  = 1,
		BASIC_TG = 2,
		QUERY_TG = 3,
	};
};
// ����
struct BACKGROUD
{
	struct BLOCK
	{
		enum {
		ONE  = 1,
		TWO  = 2,
		FIVE = 5,
		};
	};
	
	struct TIME
	{
		struct BEGIN
		{
			enum {
			ZERO  = 0,
			SIX   = 6,
			TOWNTY = 12,
			};
		};
		struct LENGTH
		{
			enum {
			TOWNTYFOUR  = 24,
			THIRTY		= 30,
			THIRTYSIX	= 36,
			};
		};
	};
};

struct RECOVER
{
	enum{
		SINGLT_TRAIN = 1,
		MULTI_TRAIN
	};
};

struct PS_LINE
{
	enum 
	{
		SOLID = 1,
		DASH  = 2,
	};
};

struct OPER_RIGHT
{
	enum
	{
	NONE_EDIT=	0,
	TG_EDIT	=	1, 
	MARK_EDIT=	2,
	TG_AND_MARK_EDIT = 3,
	};
};

//struct TRAIN_INFO_TYPE
//{
//	enum{
//		WHOLE_TRAIN_INFO = 1,
//		STATION_ABS_LOAD_TEXT = 2,
//		WHOLE_CONSOLE_TRAIN_INFO = 3,
//		ENGINE_ADJUST_BEGIN_STATION = 4,
//		CHANGE_TRAIN_ID = 5,
//		CHANGE_MISC_TRAIN_INFO = 6,
//		CHANGE_BASIC_TRAIN_INFO = 7,
//		WHOLE_TRAIN_INFO_INSERT = 8,
//	};
//};
//
//struct RECORD_CHANGE_TYPE
//{
//	enum{
//		SCHD_TIME_STATUS_FLAG = 10,
//		SCHD_CHANGE_ONE_TRAINRECORD = 12,
//		SCHD_CHANGE_SIDE = 13,
//		SCHD_ADJUST_SUB = 14,
//		SCHD_REPORT_POINT = 15,
//		SCHD_CHANGE_TRAIN = 16,
//	};
//};
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
		BSCHD_OPER,
		MUTI_SEL,
		KEY_DOWN,
		EDIT_REAL_TIME,
		LOCK_SIDE,
		GREEN_DECORATE_LINE,
		TRAIN_CZH,
		SWITCH_STATION,
		NOT_SHOW_SIDE_CTC_STATION,
		RECIEVE_POINT_LIMIT_TIME,
		LOCK_SIDE_NOT_TECH_STOP,
		SET_MOVE_SAME_CZ_TRAIN,
		SET_NOT_THIS_SECTION_STATION,
		NOT_DELETE_REAL_TIME,
		CHANGE_TRAINID_TIME_POS_VIEW, // �����۷�����������ʾ--������
		SET_NO_LIMIT_STATION, // ���÷������ͼ�в���յĳ�վ
		SET_LIMIT_MODIFY_ACT_TIME, // �Ƿ������޸�ʵ�ʵ�
		GEN_TRAINS_RELATION, // ���ɵ�ǰ�г������ϵ�ļ�czh_relation.xml 20150623�����ǳǼ�
		INTER_CITY_STPLAN, // ֧�ֳǼʼƻ����� --�����ǳǼ�20150623
		ERROR_SIDE_STATION, // �����ʹɳ�վ,���Ͼ�
		ENABLE_SHOW_RUNTIME_WND, // �Ƿ���ʾ��������ʱ�ִ���
		DRAW_FOUCS_TRAIN_ACCEPT_GIVEOUT_MARK, // �Ƿ���ƽ��㳵���뽻������
		DISENABLE_SHEET_TAB,	// �Ƿ������Ҳ����Դ��ڣ� ��Ҫ�򿪶Ի����н��е������Իָ�
		TRAIN_NEW_CZH, // �³����ϵ
		SET_TRAIN_RETURN_ID, // ���ó����Ƿ��޸�����������
		SHOW_TRAIN_NO_LEVEL_COUNT,	// ���ó��κ���ʾ����
		BASIC_MOVE_NULTI_STATION, // �Ƿ����ͼ��������ƽ��
		ERASE_BEFORE_MODIFY, // ����ʱ,����������
		DISABLE_MODIFY_PLAN_TM, // ��ֹ�޸�ͼ��ʱ��
		RUN_FAST_SLOW_WITH_BASIC, // �����˻�ͬ����ͼ�Ƚ���ʾ
		SET_PIXEL_PER_MINUTE, // ����ÿ����������,���ڷŴ���Сʹ��
	};
};

struct GREEN_DECORATE_LINE
{
	enum 
	{
		DIESEL_LOCOMOTIVE = 0,
		ELEC_LOCOMOTIVE	= 1,
	};
};

struct MUTI_SEL
{
	enum 
	{
		TRAIN			= 1,
		MARK			= 2,
		TRAIN_AND_MARK	= 3,
	};
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
	SHOW_EARLY_LATE_TIME=	11, // ��ʾ�����
	SHOW_CONFLICT		=	12, // ��ʾ��ͻ
	SHOW_ADJUST_ERROR	=	13,
	SHOW_ERROR			=	14,
	SHOW_ELEC_LOCO		=	15,
	ACCEPT_TRUCK_LATE	=	16,
	ACCEPT_TRAIN_LATE	=	17,
	SHOW_FAST_SLOW_TIME	=	18,
	SHOW_ERR_WORK_SIDE	=	23,
	
	SHOW_BASIC_SCHD_SIDE =  26,
	SHOW_ABS_DESP =  27,
	SHOW_TECH_STOP_MARK=28,  // �Ƿ���ʾ����ͣ�����Ƿ���
	//ENABLE_LOCK_STATION_SIDE=29,// ���������ɵ�
	SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND=30, // �Ƿ���ʾ��·���Դ�˫ԲȦ
	SHOW_ROUTE_TRIGER_OK_FLAG=31, // �Ƿ���ʾ��·�Ѵ�����־
	SHOW_SEL_CZH_LINE=32, // �Ƿ���ʾѡ�г��鹴����
	RECEIVE_POINT=33, // ����յ�
	ENABLE_CZH_OPER=34, // �������
	SHIFT_SCHD_OPER=35, // �հ�ƻ�����
	SHOW_RETURN_TRAINID=36, //��ʾ�۷�����
	SHOW_DEPART_MEET_MARK=37,  // �Ƿ���ʾ����ʱ���Ǻ�
	SHOW_VIRTUAL_STATION=38, //��ʾ���⳵վ
	EDIT_TRIGER_LINE_TIME=39, // ��ʱ����,�༭��·�Ѵ�������ʱ��
	MOVE_SAME_CZ_TRAIN=40, // �Ƿ�ƽ��ͬ��������г�
	USE_STRING_KILO_METER=41, // �Ƿ�ʹ���ַ��������
	SHOW_TRANS_INFO=42, // �Ƿ���ʾ������Ϣ
	SHOW_ALL_CZH_LINE=43, // �Ƿ���ʾ���г��鹴����
	SHOW_ENGINE_CREW_INFO=44, // �Ƿ���ʾ����������Ϣ�˵�
	FIRE_REAL_TIME_EVENT=45, // �Ƿ񷢳�ʵ�ʵ��¼�,����ʹ����
	GOOD_TRAIN_STAY_TO_PASS_NEED_PWD=46, // �����г��Ǽ���ͣ��,������ͨ��������
	SHOW_PLAN_NOTE_TEXT=47,	// �Ƿ���ʾ�ƻ�̨�·����ص�����
	WHOLE_SCREEN_STATE=48, // ȫ����ʾ״̬
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
		IN_ABS_PART_RUN = 5,
		ATT_ENGINE_RETURN = 6,
		IN_ABS_WHOLE_HELP_RUN = 7,
	};
};

// draw special train flag
struct TRAIN
{
	enum{
		RETURN			=1,	// �����۷�
		GOON			=2,	// ����ǰ��
		TO_ABS_TRAIN	=3,   // ������ҵ ��������ҵ
		FROM_ABS_TRAIN	=4,   // ������ҵ ������ҵ����
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
	IF_ABS_TOTAL_FIX	=10,	// �����ۺ��촰����
	PE_NOTE				=11,	// ���±�ע
	STATION_SIDE_LOST_ELE	=12,	// վ�ڹɵ�ͣ��
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

struct TERM_TG_TYPE
{
	enum 
	{
		NOW_BASIC_TG = 1,   // ���û���ͼ
		PRE_BASIC_TG = 2,   // Ԥ������ͼ
		READY_BASIC_TG = 3, // �鵵����ͼ
		SHIFT_TG = 4,
		WORK_TG  = 5,
		QUERY_TG = 6,
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

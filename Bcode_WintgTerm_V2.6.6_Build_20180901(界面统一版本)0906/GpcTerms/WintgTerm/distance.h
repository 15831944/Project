#ifndef DISTANCE_HEADER
#define DISTANCE_HEADER

#include "defines.h"
//#include "ccrt_dis.h"
#include "dbfunc.h"


#ifndef Pstruct
#define Pstruct struct
#endif

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int

#define MAX_SIDES 128
#define MAX_ENTRY 50
#define DISTANCE_FILE "distance.dat"
#define MAX_SECTIONS 64
#define MAX_STAION_LINE_PER_SECTION  64
#define MAX_TRAIN_NUMBER  300
#define STAGE_PER_WORK  3
#define MAX_STATIONS_PER_SECTION     128
#define MAX_STATIONS_PER_ROUTE	128
 
Pstruct _entry_info {
  uchar entry_number;
  bool  fhighlow;
  //2002/3/22
  ushort abs_line;
  bool  entry_type;// 1:in , 0: out 
  //end 
  };

Pstruct _side_info 
{
  uchar side_number;
  ushort length,limit;
  ushort side_property;
  bool fhighlow;
  char sidename[10];
  };


Pstruct _node_info {
	ushort   sub_section;
    ushort   index;
	char     text[50];
	ushort   station_number;
	ushort   up_line_count;
    ushort   up_line_list[50];
	ushort   down_line_count;
    ushort   down_line_list[50];
	ushort   space;
	bool     show_sides;
	bool     isNeedThick;  
    bool     show_statistic;               // �Ƿ�ͳ��
    ushort   up_pessenger_train;           // ���пͳ�ͳ�����ѡ��  0: ����  1: ����  2��������  3: ��ͳ��
    ushort   up_goods_train;               // ���л���ͳ�����ѡ��  0: ����  1: ����  2��������  3:��ͳ��
	ushort   down_pessenger_train;         // ���пͳ�ͳ�����ѡ��ͨ��
	ushort   down_goods_train;             // ���л���ͳ�����ѡ��,ͬ��
   
    ushort   inhibit_sametime_dir;
	
	ushort   show_sides_mode ;  
	ushort   show_sides_TanChuang_UpDn ;   // �ڵ�ͼ����ʾ�촰����λ��  0������ʾ��1:վ�����Ϸ���2��վ�����·�, 3:���¾���ʾ

	char kilo_mark[64];
};

Pstruct _section_node_info {
    Pstruct _node_info node_info[MAX_STATIONS_PER_ROUTE];
    ushort  node_count;
    ushort  sub_section_count;
};

Pstruct _distance {
  ushort station_number;
  char station_name[50];
  char distance[64];
  float deltadistance;
  ushort x,y;
  uchar number_of_side;
  uchar number_of_entry;
  ushort  rateAttr;
  struct _side_info side[MAX_SIDES];
  bool fHigh;
  bool fCarsChg;
  struct _entry_info entry[MAX_ENTRY];
  // 2002/3/22 
  Pstruct _node_info node_info;
  //end 
  };

#endif

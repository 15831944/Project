//
#ifndef TG_BLOCK_H
#define TG_BLOCK_H

#include "tg_structdef.h"

typedef DWORD MARK_KEY;
//ʹ��slow_time�ֶεͶ�λ��ʾͼ��ע������ΧԲȦ��λ��
//00��ʾû��ԲȦ��01��ʾ��Ȧ��10��ʾ��Ȧ
#define SetSLWRoundNone(slow_time)   ((slow_time) = (0xFFF8&(slow_time))|0x0000)
#define SetRectBlue(slow_time)       ((slow_time) = (0xFFF8&(slow_time))|0x0001)
#define SetRectRed(slow_time)        ((slow_time) = (0xFFF8&(slow_time))|0x0002)
#define SetCircleRed(slow_time)      ((slow_time) = (0xFFF8&(slow_time))|0x0003)
#define SetCircleBlue(slow_time)     ((slow_time) = (0xFFF8&(slow_time))|0x0004)

#define IsSLWRoundNone(slow_time)   ((0x0007&(slow_time)) == 0x0000)
#define IsRectBlue(slow_time)		((0x0007&(slow_time)) == 0x0001)
#define IsRectRed(slow_time)		((0x0007&(slow_time)) == 0x0002)
#define IsCircleRed(slow_time)		((0x0007&(slow_time)) == 0x0003)
#define IsCircleBlue(slow_time)		((0x0007&(slow_time)) == 0x0004)

//bit 4 ������ʾ������ɫ
//0��ʾ ��ɫ  1��ʾ ��ɫ
#define SetWordRed(slow_time)       (slow_time &= ~0x0010)
#define SetWordBlue(slow_time)      (slow_time |= 0x0010)
#define IsWordBlue(slow_time)		(slow_time & 0x0010)

//����λ���õ��ĺ�
//ʹ��line�ĸ�λ�ֽ��еĵͶ�λ����������λ��
//00�Ϸ�  01Ĭ���������м�  10�·�
//����վ�ڷ��������У��촰�޵�����λ��
//����λ��Ϊ�Ϸ����·������У���.
#define SetLineLinePosTop(line)    ((line) = (0xFCFF&(line))|0x0000)
#define SetLineLinePosMiddle(line) ((line) = (0xFCFF&(line))|0x0100)
#define SetLineLinePosBottom(line) ((line) = (0xFCFF&(line))|0x0200)

#define IsLineLinePosTop(line)	    ((0x0300&(line)) == 0x0000)
#define IsLineLinePosMiddle(line)	((0x0300&(line)) == 0x0100)
#define IsLineLinePosBottom(line)	((0x0300&(line)) == 0x0200)

//����λ��ʹ�ø�λ�ֽڵĵ͵���λ
//0��ʾ�Ϸ���ȱʡ��ʽ,1��ʾ�·�
#define SetLineTextPosTop(line)		((line) = (0xE3FF&(line))|0x0000)
#define SetLineTextPosBottom(line)  ((line) = (0xE3FF&(line))|0x0400)

#define SetLineTextPosLeft(line)    ((line) = (0xE3FF&(line))|0x1000)
#define SetLineTextPosMiddle(line)  ((line) = (0xE3FF&(line))|0x1400)
#define SetLineTextPosRight(line)   ((line) = (0xE3FF&(line))|0x1800)

#define IsLineTextPosTop(line)		((0x1C00&(line)) == 0x0000)
#define IsLineTextPosBottom(line)	((0x1C00&(line)) == 0x0400)
#define IsLineTextPosLeft(line)		((0x1C00&(line)) == 0x1000)
#define IsLineTextPosMiddle(line)	((0x1C00&(line)) == 0x1400)
#define IsLineTextPosRight(line)	((0x1C00&(line)) == 0x1800)


//�Ƿ��Ǽ������ߵı�־ʹ�ø�λ�ֽڵĵ͵���λ
//0��ʾʵ�ߣ�ȱʡ��ʽ,1��ʾ����
#define SetLineSolid(line) ((line) = (0xF7FF&(line))|0x0000)
#define SetLineDash(line) ((line) = (0xF7FF&(line))|0x0800)

#define IsLineSolide(line)	((0x0800&(line)) == 0x0000)
#define IsLineDash(line)	((0x0800&(line)) == 0x0800)

//ʹ�õ��ֽڵĵͶ�λ
//00���У�01���У�10������ȱʡ��11�޷��򣬱�ʾѡ����һ������˵���������
#define SetLineDirectionUp(line) ((line) = (0xFFFC&(line))|0x0000)
#define SetLineDirectionDown(line) ((line) = (0xFFFC&(line))|0x0001)
#define SetLineDirectionDual(line) ((line) = (0xFFFC&(line))|0x0002)
#define SetLineDirectionNone(line) ((line) = (0xFFFC&(line))|0x0003)

#define IsLineDirectionUp(line) ((0x0003&(line)) == 0x0000)
#define IsLineDirectionDown(line) ((0x0003&(line)) == 0x0001)
#define IsLineDirectionDual(line) ((0x0003&(line)) == 0x0002)
#define IsLineDirectionNone(line) ((0x0003&(line)) == 0x0003)

//ʹ�õ��ֽڵĵ�����λ
//00���������01���ͳ�������������
#define SetLineFlagInhibit(line) ((line) = (0xFFF3&(line))|0x0000)
#define SetLineFlagLightTrain(line) ((line) = (0xFFF3&(line))|0x0004)

#define IsLineFlagInhibit(line) ((0x000C&(line)) == 0x0000)
#define IsLineFlagLightTrain(line) ((0x000C&(line)) == 0x0004)


#define SetUpdateByOtherGPC(block) ( ((block)->dwFlag) |= 0x1)
#define SetDeleteByOtherGPC(block) ( ((block)->dwFlag) |= 0x2)
#define IsUpdateByOtherGPC(block)  ( (((block)->dwFlag)&0x1) == 0x1)
#define IstDeleteByOtherGPC(block) ( (((block)->dwFlag)&0x2) == 0x2)

#define SetBlockPlan(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0000)
#define SetBlockActive(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0001)
#define SetBlockComplete(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0002)

#define IsBlockPlan(adjust_status)  ((0x0007&(adjust_status)) == 0x0000)
#define IsBlockActive(adjust_status) ((0x0007&(adjust_status)) == 0x0001)
#define IsBlockComplete(adjust_status) ((0x0007&(adjust_status)) == 0x0002)


#define SetSlowSpeed(speed) ((speed)/5)
#define GetSlowSpeed(speed) ((speed)*5)

#define MAX_SCHD_BLOCK_ELE_COUNT 512
#define MAX_SCHD_BLOCK_TEXT_LEN  256

class CTg_BLOCK
{
public:
	DWORD     combstation;
	MARK_KEY  mark_index;
	long	  start_time;
	long      end_time;
	BYTE      type;           
	BYTE      flag;  
	USHORT    line;
	USHORT    station;
	USHORT    index;
	USHORT    position;       
	USHORT    count;
	USHORT	  ele_number[MAX_SCHD_BLOCK_ELE_COUNT];
	char	  text[MAX_SCHD_BLOCK_TEXT_LEN];

	USHORT    s_meter;
	USHORT    e_meter;
	USHORT    e_kilo;
	USHORT    slow_time;

	// 1. �����Դ����gpc,����TDʱ,�����������ǻ滭
	// 2. �����û���ǰ������ڵ�����ѡ�г�վ
	USHORT    station1;
	USHORT    station2; 
	BYTE	  direction; // 1: ���� 2: ���� 3: ������
	
	// add your code here
	bool	  bFlag; 
    CRect     myPosition;

	// ��Ϊ��־ʹ��
	DWORD     dwFlag;  // bit0: �ñ���Ѿ�������GPC���� 1:�� 0:��
					   // bit1: �ñ���Ѿ�������GPCɾ�� 1:�� 0:��

	// ���ڱ�ʾʩ�����ŵ�״̬��Ϣ
	DWORD	 adjust_status; // // bit 1-0:  0 �ƻ���1 ִ�С�2 ʵ��
		                   
public:
	CTg_BLOCK();
	CTg_BLOCK(const CTg_BLOCK& rhs);
	CTg_BLOCK& operator=(const CTg_BLOCK& rhs);
	~CTg_BLOCK();
	void Clear();
	
public:
	CString toXML(DWORD nResult=0);

	CString toString();
private:
	void Copy(const CTg_BLOCK& rhs);

public:
	bool IfInTimeScope(TIME stime, TIME etime) const;
	void RestoreDrawFlag();
	void SetColor(COLORREF  newColor);
	void SetDrawText(CString &text);
	CString GetKiloMDesp();
	void SetReasonTextFromTDMS();
	bool GetKiloMValue(ULONG& beginKilo, ULONG& endKilo);
public:
	BOOL bMoveCenter,bMoveRight,bMoveLeft,bMoveTop,bMoveBottom;
	BOOL m_bFocus;
	COLORREF mColor;
	CRect cRect,lRect,rRect,tRect,bRect,m_drawRect;

public:
	void OffsetPosition(int x);
	// �Զ�����ģʽ��,��¼�ϴ��г�����ʱ�Ŀ�ʼʱ��
	int m_nUpdateTime;
};

#endif
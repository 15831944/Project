#ifndef _DEF_TRAIN_TEXT_DEF_H
#define _DEF_TRAIN_TEXT_DEF_H

#include "traindef.h"

/* cTrainDef�����ڴ汣�����ݣ�Ϊ�˼����ڴ�ռ��ʹ�ý�TrainRecord
 * �ڵ�����Text���嵽TrainRecord�ⲿ������cTrainDef��Ϊ��������ʱ
 * ��Ϊ���㣬��˶����������ݽṹ��ֱ�Ӱ���Text��Ϣ */

struct cTrainTextDef : public cTrainDef
{
	const    int m_MaxRecCount;
	const    int m_MaxTextLength;

public:
	short*    m_pIndexList;
	char*     m_pAbsText;
	char*     m_pStationText;

public:
	cTrainTextDef();
	~cTrainTextDef();

	char*  GetAbsText(int index) const;
	char*  GetStationText(int index) const;
};


#endif /** THE END **/

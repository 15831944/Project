#ifndef _CTC_DEFINES_H_
#define _CTC_DEFINES_H_

#include "private_defines.h"

CLBTH_NAMESPACE_BEGIN

#define MAX_STA_NAME_LEN        32

//--------------------------- �������Ͷ���------- -----------------------------------
typedef	IDWORD		ENTITY_ID;				// �豸ʵ���
typedef IWORD		STATION_NO;			// վ��
typedef char		STAT_NAME[MAX_STA_NAME_LEN];			// ��վ��
const ENTITY_ID		INVALID_ENTITY_ID = 0;		// ��Чʵ���
const STATION_NO	INVALID_STATION_NO = 0;	// ��Чվ��

typedef I_TIME		TIME;					// ʱ��
typedef IWORD		TIME_INTERVAL;		// ʱ����( ��)
const TIME          INVALID_TIME =(TIME) -1;    // ��Чʱ��

typedef IBYTE		WORK_STATUS;				// �豸����״̬
const WORK_STATUS	ACTIVE		= 0;
const WORK_STATUS	INACTIVE	= 1;

const ENTITY_ID		SERVER_ENTITY=0;
//--------------------------- ��λ���---------------- --------------------------------
typedef IWORD		INDI_CHANGE;			// ��λ�仯
const IWORD			MAX_BITBLOCK_LEN = 1000;	// ��λȫ�����󳤶�
const int				MAX_INDI_PER_MSG = 300;	// һ����Ϣ����λ�仯��������

//--------------------------- ����ģʽ-------------------------------------------------

typedef IBYTE		STATION_CTRL_MODE;		// ��վ����ģʽ
typedef IBYTE		ROUTE_CTRL_MODE;		// ��·����ģʽ
typedef IBYTE		STATION_CTRL_RESULT;	// ��վ����ģʽ������
typedef IBYTE		ROUTE_CTRL_RESULT;		// ��·����ģʽ������
typedef IWORD	STATION_CTC_FLAG;		// ��վCTC������־
typedef IWORD	ZONE_ID;					// ������

const STATION_CTRL_MODE		SMODE_DISTRIBUTED	= 0;	// ��ɢ����
const STATION_CTRL_MODE		SMODE_CENTER_CTRL	= 1;	// ������ȫ����
const STATION_CTRL_MODE		SMODE_STATION_CTRL	= 2;	// ��վ��ȫ����
const STATION_CTRL_MODE		SMODE_EMERGENCY		= 3;	// �ǳ�վ��
const STATION_CTRL_MODE		SMODE_EMG_CENTER_CTRL = 4;	// �������Ŀ���

const ROUTE_CTRL_MODE		RMODE_PLAN_CTRL		= 0;	// �ƻ�����
const ROUTE_CTRL_MODE		RMODE_SAVE_ROUTE	= 1;	// �洢��·
const ROUTE_CTRL_MODE		RMODE_FORBIT_AUTO	= 2;	// ��ֹ�Զ�

const STATION_CTRL_RESULT	SREQUEST_SUCCESS	= 0;	// �ɹ�
const STATION_CTRL_RESULT	SREQUEST_INVALID	= 1;	// ���ڴ�����һ������
const STATION_CTRL_RESULT	SREQUEST_REFUSED	= 2;	// �������߾ܾ�
const STATION_CTRL_RESULT	SREQUEST_TIMEOUT	= 3;	// ��ʱʧ��
const STATION_CTRL_RESULT	SREQUEST_FORBIT	= 4;	// ��ֹ����ת��
const STATION_CTRL_RESULT	SREQUEST_WAIT		= 5;	// �����Ա����,��ȴ�
const STATION_CTRL_RESULT	SREQUEST_EQUAL	= 6;	// �����ģʽ��������ͬ
const STATION_CTRL_RESULT	SREQUEST_NO_RIGHT	= 7;	// û��Ȩ��
const STATION_CTRL_RESULT	SREQUEST_NO_OWNER	= 8;	// ������û������
const STATION_CTRL_RESULT	SREQUEST_STATION_URGENT	= 9;	// ���ڷǳ�վ��, �޷�ת��

const ROUTE_CTRL_RESULT		RREQUEST_SUCCESS	= 0;	// �ɹ�
const ROUTE_CTRL_RESULT		RREQUEST_NO_RIGHT	= 1;	// û��Ȩ��
const ROUTE_CTRL_RESULT		RREQUEST_FORBIT	= 2;	// ��ֹ����ת��
const ROUTE_CTRL_RESULT		RREQUEST_EQUAL	= 3;	// �����ģʽ��������ͬ

const STATION_CTC_FLAG		DEFAULT_CTC_FLAG	= 0;
const STATION_CTC_FLAG		CTC_FLAG_NO_PLAN_CTRL	= 0x0001;	// �Ƿ�ʹ�üƻ������(0Ϊʹ�ã�1Ϊ��ʹ��)

//---------------------------- Ȩ������ -------------------------------
typedef IDWORD	USER_RIGHT;
const USER_RIGHT		UR_TRAIN_ROUTE	= 0x00000001;
const USER_RIGHT		UR_TRAIN_BUTTON	= 0x00000002;
const USER_RIGHT		UR_SHUNT_ROUTE	= 0x00000004;
const USER_RIGHT		UR_SHUNT_BUTTON	= 0x00000008;
const USER_RIGHT		UR_MAINTAIN		= 0x00000010;
const USER_RIGHT		UR_STA_NOPERSON	= 0x00000020;
const USER_RIGHT		UR_URGENCY_OPER = 0x00000040;

const USER_RIGHT		UR_NO_RIGHT		= 0;
/*	Reserved
const USER_RIGHT		UR_STMODE_CTRL	= 0x00000010;
const USER_RIGHT		UR_RTMODE_CTRL	= 0x00000020;
const USER_RIGHT		UR_TRAIN_NO_CTRL	= 0x00000040;
const USER_RIGHT		UR_SG_BUTTON		= 0x00000080;
const USER_RIGHT		UR_MANUAL_OPER	= 0x00000100;
*/

//------------------------ ��·������Ͷ���----------------------------------------

typedef IBYTE	ROUTE_LIST_OPER_TYPE;	// ��·������������
const ROUTE_LIST_OPER_TYPE	RLIST_OPER_READ_ONLY	= 0;	// ֻ��
const ROUTE_LIST_OPER_TYPE	RLIST_OPER_MODIFY		= 1;	// �޸�

typedef IDWORD	ROUTE_LIST_HANDLE;		// ��·���е�Ψһ��ʶ
const ROUTE_LIST_HANDLE		RLIST_H_TRAIN			= 0;	// �г���·����
const ROUTE_LIST_HANDLE		RLIST_H_TRAIN_HISTORY	= 1;	// �г���ʷ��·����
const ROUTE_LIST_HANDLE		RLIST_H_SHUNT_HISTORY	= 2;	// ������ʷ��·����
const ROUTE_LIST_HANDLE		RLIST_H_MIN_SHUNT		= 10;	// �Ե�����ҵ��, ֵ>10

typedef IDWORD	ROUTE_LIST_ID;			// ��·������һ����·�����к�(�����ɻ�����)
typedef IWORD	ROUTE_ID;				// �ض���վ�е�һ���̶���·�����к�
const ROUTE_LIST_ID			NULL_ROUTE_LIST_ID	= 0;
const ROUTE_ID				NULL_ROUTE_ID			= 0;

typedef IBYTE	SHUNT_GOU_INDEX;		// ��������
typedef IWORD	SHUNT_GOU_TYPE;			// ����������

typedef IBYTE	ROUTE_OPER_RESULT;		// ��·�˹�������Ӧ���
const ROUTE_OPER_RESULT		ROPER_SUCCESS			= 0;	// �����ɹ�
const ROUTE_OPER_RESULT		ROPER_NO_RIGHT			= 1;	// û��Ȩ��
const ROUTE_OPER_RESULT		ROPER_UNCHANGABLE		= 2;	// ��·�����޷�����
const ROUTE_OPER_RESULT		ROPER_LOCKED			= 3;	// ��·�����ѱ�����
const ROUTE_OPER_RESULT		ROPER_TRIG_FAIL			= 4;	// ��·����ʧ��(�������߼���ͻ)
const ROUTE_OPER_RESULT		ROPER_PLAN_FAIL			= 5;	// ��·����ʧ��(��ƻ���ͻ)
const ROUTE_OPER_RESULT		ROPER_INVALID				= 6;	// ��Ч����( ��·����ģʽ����)
const ROUTE_OPER_RESULT		ROPER_ROUTE_UNKNOWN		= 7;	// ͨ����ť�Ҳ�����·����Ϣ�Ƿ�
const ROUTE_OPER_RESULT		ROPER_PROG_ERROR			= 8; // ���ɻ��������
const ROUTE_OPER_RESULT		ROPER_STATION_URGENT	= 9;	// ���ڷǳ�վ��
const ROUTE_OPER_RESULT		ROPER_OBSOLETE_MSG	= 10;	// ��ʱ��Ϣ
const ROUTE_OPER_RESULT		ROPER_INVALID_MSG		= 11;	// ��Ϣ������Ч
const ROUTE_OPER_RESULT		ROPER_STDETAIL_FAIL	= 12;	// Υ��վϸ�涨

typedef IBYTE	ROUTE_REQUEST_RESULT;
const ROUTE_REQUEST_RESULT	RL_REQ_SUCCESS			= 0; // �ɹ�
const ROUTE_REQUEST_RESULT	RL_REQ_INVALID				= 1; // ��Ч( ����ʷ�������޸�)
const ROUTE_REQUEST_RESULT	RL_REQ_IN_LOCK				= 2; // �����޸�ʱ�����޸���
const ROUTE_REQUEST_RESULT	RL_REQ_FULL				= 3; // ������̫��
const ROUTE_REQUEST_RESULT	RL_REQ_NO_RIGHT			= 4; // û��Ȩ��
const ROUTE_REQUEST_RESULT	RL_REQ_WRONG_STA		= 5; // �޴�վ��

typedef IWORD	ROUTE_TYPE;				// ��·���ͱ�־
										/* bit 0-2: �ӳ�/����/ͨ��/����
										bit 3-5: ��·��Դ(�ƻ�ֱ�Ӳ���/�ƻ�ѡ·�õ�/�˹�����/�˹���ť����/ɾ���ƻ�����(����Ϊ��Ч��·)
										bit 6: ����/�����·
										bit 7: �Ƿ�ƻ�����ͨ��
*/
const IWORD	ROUTE_TRAIN_TYPE_MASK	= 0x0007;	// ��·�ӷ�����־����
const IWORD	ROUTE_SOURCE_MASK		= 0x0038;	// ��·��Դ����
const IWORD	ROUTE_BASIC_TYPE_MASK	= 0x0040;	// ��·������־����
const IWORD	ROUTE_PLAN_PASS_MASK		= 0x0080;	// ��·�Ƿ�ƻ�ͨ������
const IWORD	ROUTE_EXT_TYPE_MASK		= 0x0600;	// ��·��չ��������
const IWORD	ROUTE_SPEC_TYPE_MASK		= 0x1800;	// ��·��������������
const IWORD	ROUTE_INTERSECT_MAIN_SIDE_MASK	= 0x2000;	// ������·������־
const IWORD	ROUTE_PASSENGER_MASK		= 0x4000;	// �ͳ���־����

typedef IBYTE	ROUTE_TRAIN_TYPE;		// ��·�ӷ�����־(��ROUTE_TYPE��)
const ROUTE_TRAIN_TYPE		RTRAIN_ARRIVE	= 0;	// �ӳ�
const ROUTE_TRAIN_TYPE		RTRAIN_DEPART	= 1;	// ����
const ROUTE_TRAIN_TYPE		RTRAIN_PASS		= 2;	// ͨ��
const ROUTE_TRAIN_TYPE		RTRAIN_SHUNT	= 3;	// ����
const ROUTE_TRAIN_TYPE		RTRAIN_TRANS	= 4;	// ת��

typedef IBYTE	ROUTE_SOURCE;			// ��·��Դ(��ROUTE_TYPE��)
const ROUTE_SOURCE			RSOURCE_PLAN		= 0;	// �ƻ�ֱ�Ӳ���
const ROUTE_SOURCE			RSOURCE_CHOICE		= 1;	// �ƻ�ѡ·�õ�
const ROUTE_SOURCE			RSOURCE_MANUAL		= 2;	// �˹�����
const ROUTE_SOURCE			RSOURCE_BUTTON		= 3;	// �˹���ť����
const ROUTE_SOURCE			RSOURCE_REMOVED		= 4;	// ɾ���ƻ�����(����Ϊ��Ч��·)

typedef IBYTE	ROUTE_BASIC_TYPE;		// ��·������־(��ROUTE_TYPE��)
const ROUTE_BASIC_TYPE		BASIC_ROUTE			= 0;	// ������·
const ROUTE_BASIC_TYPE		REVISED_ROUTE		= 1;	// �����·

typedef IBYTE	ROUTE_EXT_TYPE;			// ��·��չ����
const ROUTE_EXT_TYPE			REXT_NO_EXT			= 0;	// һ���·
const ROUTE_EXT_TYPE			REXT_LONG_CONT		= 1;	// ��������·
const ROUTE_EXT_TYPE			REXT_LONG_TRAIN		= 2;	// ���г���·
const ROUTE_EXT_TYPE			REXT_LONG_SHUNT		= 3;	// ��������·

typedef IBYTE	ROUTE_SPEC_TYPE;
const ROUTE_SPEC_TYPE		RSPEC_NONE				= 0;	// һ���·
const ROUTE_SPEC_TYPE		RSPEC_VIRTUAL_PLAN	= 1;	// ����ƻ���·
const ROUTE_SPEC_TYPE		RSPEC_VIRTUAL_COMP	= 2;	// ������Ͻ�·
const ROUTE_SPEC_TYPE		RSPEC_COMP				= 3;	// ��Ͻ�·

// �п����� ״̬����
const IWORD TCC_COMMAND_STATUS_NONE       = 0;	//��ʼֵ
const IWORD TCC_COMMAND_STATUS_CONFIRM    = 1;	//��ȷ��
const IWORD TCC_COMMAND_STATUS_EXECUTING  = 2;	//ִ����
const IWORD TCC_COMMAND_STATUS_CANCELED   = 3;	//��ȡ��
const IWORD TCC_COMMAND_STATUS_FAILED   = 4;	//��ʧ��

inline ROUTE_TRAIN_TYPE GET_ROUTE_TRAIN_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_TRAIN_TYPE)(nType & ROUTE_TRAIN_TYPE_MASK);
}
inline void SET_ROUTE_TRAIN_TYPE(ROUTE_TRAIN_TYPE nTrainType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_TRAIN_TYPE_MASK;
	nType |= ((IWORD)nTrainType) & ROUTE_TRAIN_TYPE_MASK;
}

inline ROUTE_SOURCE GET_ROUTE_SOURCE(ROUTE_TYPE nType)
{
	return (ROUTE_SOURCE)((nType & ROUTE_SOURCE_MASK)>>3);
}
inline void SET_ROUTE_SOURCE(ROUTE_SOURCE nSource, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_SOURCE_MASK;
	nType |= (((IWORD)nSource)<<3) & ROUTE_SOURCE_MASK;
}

inline ROUTE_BASIC_TYPE GET_ROUTE_BASIC_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_BASIC_TYPE)((nType & ROUTE_BASIC_TYPE_MASK)>>6);
}
inline void SET_ROUTE_BASIC_TYPE(ROUTE_BASIC_TYPE nBasic, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_BASIC_TYPE_MASK;
	nType |= (((IWORD)nBasic)<<6) & ROUTE_BASIC_TYPE_MASK;
}

inline bool	IS_PASS_ROUTE_IN_PLAN(ROUTE_TYPE nType)
{
	return (nType & ROUTE_PLAN_PASS_MASK) != 0;
}

inline void	SET_PLAN_PASS_ROUTE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_PLAN_PASS_MASK;
	else
		nType &= ~ROUTE_PLAN_PASS_MASK;
}

inline ROUTE_EXT_TYPE GET_ROUTE_EXT_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_EXT_TYPE)((nType & ROUTE_EXT_TYPE_MASK)>>9);
}
inline void SET_ROUTE_EXT_TYPE(ROUTE_EXT_TYPE nExtType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_EXT_TYPE_MASK;
	nType |= (((IWORD)nExtType)<<9) & ROUTE_EXT_TYPE_MASK;
}

inline ROUTE_SPEC_TYPE GET_ROUTE_SPEC_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_SPEC_TYPE)((nType & ROUTE_SPEC_TYPE_MASK)>>11);
}
inline void SET_ROUTE_SPEC_TYPE(ROUTE_SPEC_TYPE nSpecType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_SPEC_TYPE_MASK;
	nType |= (((IWORD)nSpecType)<<11) & ROUTE_SPEC_TYPE_MASK;
}

inline bool	IS_SHUNT_INTERSECT_MAIN_SIDE(ROUTE_TYPE nType)
{
	return (nType & ROUTE_INTERSECT_MAIN_SIDE_MASK) != 0;
}
inline void	SET_INTERSECT_MAIN_SIDE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_INTERSECT_MAIN_SIDE_MASK;
	else
		nType &= ~ROUTE_INTERSECT_MAIN_SIDE_MASK;
}

inline bool	IS_PASSENGER_PLAN_ROUTE(ROUTE_TYPE nType)
{
	return (nType & ROUTE_PASSENGER_MASK) != 0;
}
inline void	SET_PASSENGER_PLAN_ROUTE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_PASSENGER_MASK;
	else
		nType &= ~ROUTE_PASSENGER_MASK;
}

typedef IWORD	ROUTE_STATUS;			// ��·״̬��־
										/*	bit 0-2: ��·������־
										bit 3-7: ����ʧ�ܴ���
										bit 8: ��·��������(�Զ�/�˹�)
*/
const IWORD	ROUTE_TRIG_STATUS_MASK	= 0x000F;	// ��·������־����
const IWORD	ROUTE_FAIL_TIMES_MASK	= 0x00F0;	// ��·����ʧ�ܴ�������
const IWORD	ROUTE_TRIG_TYPE_MASK	= 0x0100;	// ��·������������
const IWORD	ROUTE_AUTO_SET_TRIG_TYPE_MASK	= 0x0200;	// �Ƿ��Զ����ý�·������������
const IWORD	ROUTE_MANUAL_LOCK_MASK	= 0x0400;	// ��·�˹����Ľ�·��־
const IWORD	ROUTE_ALARM_MASK		= 0x8000;	// �Ƿ��ѱ�����־����
const IWORD	ROUTE_PREDICT_MASK		= 0x6000;	// �Ƿ��ѷ��ͽ�·Ԥ���־����
const IWORD	ROUTE_DEPART_PREDICT_MASK	= 0x1800;	// �Ƿ��ѷ��ͷ���Ԥ���־����
const IWORD	ROUTE_SHUNTREQ_MASK	= 0x2000;	// �Ƿ����յ���������
const IWORD	ROUTE_SHUNTTRIG_MASK	= 0x4000;	// �Ƿ���׼���������

typedef IBYTE	ROUTE_TRIG_STATUS;		// ��·������־(��ROUTE_STATUS��)
const ROUTE_TRIG_STATUS		RTSTATUS_WAIT		= 0;		// �ȴ�����
const ROUTE_TRIG_STATUS		RTSTATUS_DOING		= 1;		// ���ڽ��д���
const ROUTE_TRIG_STATUS		RTSTATUS_OK			= 2;		// �������
const ROUTE_TRIG_STATUS		RTSTATUS_OCCUPY		= 3;		// ��·��ռ��
const ROUTE_TRIG_STATUS		RTSTATUS_CLEAR		= 4;		// ��·�ѳ���
const ROUTE_TRIG_STATUS		RTSTATUS_TIMEOUT	= 5;		// ��·������ʱ
const ROUTE_TRIG_STATUS		RTSTATUS_WAIT_PLAN_CHECK	= 6;		// ���ڽ��мƻ���ͻ���
const ROUTE_TRIG_STATUS		RTSTATUS_CANCEL	= 7;		// ���˹�ȡ��
const ROUTE_TRIG_STATUS		RTSTATUS_STADOWN	= 8;	// վ��

typedef IBYTE	ROUTE_TRIG_TYPE;		// ��·��������(��ROUTE_STATUS��)
const ROUTE_TRIG_TYPE		RTRIG_MANUAL		= 0;		// �˹�����
const ROUTE_TRIG_TYPE		RTRIG_AUTO			= 1;		// �Զ�����

const IWORD	RPD_NOT_SEND	= 0;		// δ���ͽ�·Ԥ��
const IWORD	RPD_SENDING	= 1;		// �ѷ��ͽ�·Ԥ�棬�ȴ���ִ
const IWORD	RPD_SEND_OK	= 2;		// ��·Ԥ�����յ���ִ
const IWORD	RPD_SEND_FAIL	= 3;		// ��·Ԥ�淢��ʧ��

const IWORD	DP_NOT_SEND	= 0;		// δ���ͷ���Ԥ��
const IWORD	DP_SEND		= 1;		// �ѷ��ͷ���Ԥ��
const IWORD	DP_RECV		= 2;		// ����Ԥ�����յ�
const IWORD	DP_ACK			= 3;		// Ԥ����ͬ��

inline ROUTE_TRIG_STATUS GET_TRIG_STATUS(ROUTE_STATUS nStatus)
{
	return (ROUTE_TRIG_STATUS)(nStatus & ROUTE_TRIG_STATUS_MASK);
}
inline void SET_TRIG_STATUS(ROUTE_TRIG_STATUS nTrigStatus, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_TRIG_STATUS_MASK;
	nStatus |= ((IWORD)nTrigStatus) & ROUTE_TRIG_STATUS_MASK;
}

inline IBYTE GET_TRIG_FAIL_TIMES(ROUTE_STATUS nStatus)
{
	return (IBYTE)((nStatus & ROUTE_FAIL_TIMES_MASK)>>4);
}
inline void SET_TRIG_FAIL_TIMES(IBYTE nTimes, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_FAIL_TIMES_MASK;
	if ( nTimes > 15 )
		nTimes = 15;
	nStatus |= (((IWORD)nTimes)<<4) & ROUTE_FAIL_TIMES_MASK;
}

inline ROUTE_TRIG_TYPE GET_ROUTE_TRIG_TYPE(ROUTE_STATUS nStatus)
{
	return (ROUTE_TRIG_TYPE)((nStatus & ROUTE_TRIG_TYPE_MASK)>>8);
}
inline void SET_ROUTE_TRIG_TYPE(ROUTE_TRIG_TYPE nType, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_TRIG_TYPE_MASK;
	nStatus |= (((IWORD)nType)<<8) & ROUTE_TRIG_TYPE_MASK;
}

inline bool IS_ROUTE_REPORTED_ALARM(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_ALARM_MASK) != 0;
}
inline void SET_ROUTE_ALARM_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b ) 
		nStatus |= ROUTE_ALARM_MASK;
	else
		nStatus &= ~ROUTE_ALARM_MASK;
}

inline IWORD GET_ROUTE_PREDICT_FLAG(ROUTE_STATUS nStatus)
{
	return (IWORD)((nStatus & ROUTE_PREDICT_MASK)>>13);
}
inline void SET_ROUTE_PREDICT_FLAG(IWORD nFlag, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_PREDICT_MASK;
	nStatus |= (nFlag<<13) & ROUTE_PREDICT_MASK;
}

inline bool CAN_AUTO_SET_TRIG_TYPE(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_AUTO_SET_TRIG_TYPE_MASK) == 0;
}
inline void SET_AUTO_TRIG_TYPE_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus &= ~ROUTE_AUTO_SET_TRIG_TYPE_MASK;
	else
		nStatus |= ROUTE_AUTO_SET_TRIG_TYPE_MASK;
}

inline bool IS_MANUAL_LOCKED(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_MANUAL_LOCK_MASK) != 0;
}
inline void SET_MANUAL_LOCK_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_MANUAL_LOCK_MASK;
	else
		nStatus &= ~ROUTE_MANUAL_LOCK_MASK;
}

inline IWORD GET_DEPART_PREDICT_FLAG(ROUTE_STATUS nStatus)
{
	return (IWORD)((nStatus & ROUTE_DEPART_PREDICT_MASK)>>11);
}
inline void SET_DEPART_PREDICT_FLAG(IWORD nFlag, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_DEPART_PREDICT_MASK;
	nStatus |= (nFlag<<11) & ROUTE_DEPART_PREDICT_MASK;
}

inline bool IS_SHUNT_REQUEST(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_SHUNTREQ_MASK) != 0;
}
inline void SET_SHUNT_REQUEST_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_SHUNTREQ_MASK;
	else
		nStatus &= ~ROUTE_SHUNTREQ_MASK;
}

inline bool IS_SHUNT_WAIT_TRIG(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_SHUNTTRIG_MASK) != 0;
}
inline void SET_SHUNT_WAIT_TRIG_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_SHUNTTRIG_MASK;
	else
		nStatus &= ~ROUTE_SHUNTTRIG_MASK;
}


const IBYTE	MAX_ROUTE_BUTTON_COUNT = 6;		// ��ʾһ����·�����İ�ť������
const IBYTE	MAX_SIDE_COUNT_IN_ROUTE	= 8;		// һ����·;���ɵ��������
const IBYTE	MAX_ROUTE_COUNT_IN_ONE_PLAN = 16;	// ��һ���ƻ����ɵ���Ͻ�·�����Ľ�·������

//------------------------ CTC��ť�������-------------------------------------

typedef IDWORD	MANUAL_OPER_ID;				// �˹����������к�
typedef IBYTE	MANUAL_OPER_TYPE;			// ��ť���������Ρ��źŻ��ȵĲ�������
typedef IBYTE	MANUAL_OPER_RESULT;			// ������Ӧ����

const MANUAL_OPER_TYPE		ROUTE_OPER_SET		= 1;	// �˹����н�·
const MANUAL_OPER_TYPE		ROUTE_OPER_CANCEL	= 2;	// �˹�ȡ����·
const MANUAL_OPER_TYPE		ROUTE_OPER_YINDAO	= 3;	// �˹�������·
const MANUAL_OPER_TYPE		ROUTE_OPER_TRY_LOCK	= 4;	// �˹����Ž�·
const MANUAL_OPER_TYPE		ROUTE_OPER_SHACKLE  = 5;	// ��·����

const MANUAL_OPER_TYPE		SWITCH_OPER_LOCK		= 10;	// ������
const MANUAL_OPER_TYPE		SWITCH_OPER_UNLOCK	= 11;	// ������
const MANUAL_OPER_TYPE		SWITCH_OPER_NORMAL	= 12;	// ������
const MANUAL_OPER_TYPE		SWITCH_OPER_REVERSE	= 13;	// ������
const MANUAL_OPER_TYPE		SWITCH_OPER_CEN_FAULT	= 14;	// �����Ĺ����
const MANUAL_OPER_TYPE		SWITCH_OPER_TIP_FAULT	= 15;	// ���������

const MANUAL_OPER_TYPE		SIGNAL_OPER_REOPEN	= 21;	// �źŻ����¿���
const MANUAL_OPER_TYPE		MANUAL_OPER_CALLON_LOCK	= 22;	// ��������
const MANUAL_OPER_TYPE		MANUAL_OPER_UNLOCK	= 23;	// ���˽�
const MANUAL_OPER_TYPE		MANUAL_OPER_BLOCK	= 24;	// ����
const MANUAL_OPER_TYPE		MANUAL_OPER_UNLOCK_STA	= 25;	// ȫվ����
const MANUAL_OPER_TYPE		MANUAL_OPER_SLOPE_UNLOCK	= 26;	// �µ�����
const MANUAL_OPER_TYPE		MANUAL_OPER_CIRCUIT_BAD	= 27;	// ��·����
const MANUAL_OPER_TYPE		SIGNAL_OPER_AUTOPASS		= 28;	// ��ͨ����
const MANUAL_OPER_TYPE		MANUAL_OPER_CIRCUIT_FAULT	= 29;	// ��������
const MANUAL_OPER_TYPE		MANUAL_OPER_NO_ELEC	= 30; 	// �˹�ͣ��
const MANUAL_OPER_TYPE		MANUAL_OPER_LOCO_OCCUPY	= 36; 	// ��ռ
const MANUAL_OPER_TYPE		SIGNAL_OPER_OPENSIG	= 37; 	// �źŻ�����
const MANUAL_OPER_TYPE		SIGNAL_OPER_CLOSESIG	= 38; 	// �źŻ��ص�

const MANUAL_OPER_TYPE		BUTTON_OPER_DOWN		= 31;	// ���Ը�����
const MANUAL_OPER_TYPE		BUTTON_OPER_UP		= 32;	// ���Ը��ָ�
const MANUAL_OPER_TYPE		BUTTON_OPER_PRESS	= 33;	// �Ը�һ�β���
const MANUAL_OPER_TYPE		BUTTON_OPER_COUNT	= 34; 	// ��ť����
const MANUAL_OPER_TYPE		BUTTON_OPER_FUNC		= 35; 	// ���ܰ�ť

const MANUAL_OPER_RESULT	MOPER_SUCCESS		= 0;	// �����ɹ�(�ѷ�������ϵͳ)
const MANUAL_OPER_RESULT	MOPER_INTERLOCK_FAIL	= 1;	// ��������ͻ
const MANUAL_OPER_RESULT	MOPER_PLAN_FAIL		= 2;	// ��ƻ���ͻ
const MANUAL_OPER_RESULT	MOPER_LOCKED		= 3;	// �����豸������
const MANUAL_OPER_RESULT	MOPER_NO_RIGHT	= 4;	// û��Ȩ��
const MANUAL_OPER_RESULT	MOPER_TIMEOUT		= 5;	// ��ʱ
const MANUAL_OPER_RESULT	MOPER_ROUTE_UNKNOWN = 6;	// ͨ����ť�Ҳ�����·����Ϣ�Ƿ�
const MANUAL_OPER_RESULT	MOPER_STATION_URGENT	= 7;	// ���ڷǳ�վ��
const MANUAL_OPER_RESULT	MOPER_PROG_ERROR		= 8; // ���ɻ��������
const MANUAL_OPER_RESULT	MOPER_OBSOLETE_MSG	= 9;	// ��ʱ��Ϣ
const MANUAL_OPER_RESULT	MOPER_INVALID_MSG	= 10;	// ��Ϣ������Ч
const MANUAL_OPER_RESULT	MOPER_STDETAIL_FAIL	= 11;	// Υ��վϸ�涨

const	IWORD	MAX_CTC_OUTPUT_HANDLE_COUNT	= 64;	// һ��CTC��������е������λ������

const	short	MOPER_PARA_CBAD_COMMON		= 0;		// ��·��������: ͨ������, ��ǰ
const	short	MOPER_PARA_CBAD_SWITCH_N	= 1;		// ��·��������: ��λ
const	short	MOPER_PARA_CBAD_SWITCH_R	= 2;		// ��·��������: ��λ
const	short	MOPER_PARA_CBAD_ACK_IDLE	= 3;		// ��·��������: ȷ�Ͽ���
const   short   MOPER_PARA_CBAD_SWITCH_C	= 4;		// ��·��������: ����

inline bool	IS_CRITICAL_MANUAL_OPER( MANUAL_OPER_TYPE nType )
{
	return (	nType != ROUTE_OPER_SET && nType != MANUAL_OPER_NO_ELEC );
}

//----------------------------- վ����ʾ��Ϣ���--------------------------------
typedef IBYTE		DEVICE_TYPE;		// �豸��DMIS����
typedef IDWORD	DEVICE_STATUS;		// �豸״̬
typedef short		DMIS_ID;			// �豸��DMIS��
typedef short		SIDE_ID;			// �ɵ���
typedef short		ENTRY_ID;			// ����ں�
typedef short		BUTTON_ID;		// ��ť��
typedef char		BUTTON_NAME[20];	// ��ť����
typedef char		TRAIN_NO[10];		// ���κ�
const IBYTE		MAX_TRAIN_NO_LEN	= 10;	// ���κ��ַ�������󳤶�
const IWORD		MAX_DMIS_STR_LEN	= 256;	// DMIS��ʾ��Ϣ�и����ַ�������󳤶�(���벻С�ڳ��κ���󳤶�)
const IBYTE		MAX_RESULT_COMMENT_LEN	= 200;	// ������Ӧ����˵������󳤶�
const DMIS_ID	INVALID_DMIS_ID	= -1;

//------------------------------ �г��ƻ����--------------------------------------

typedef IDWORD	TRAIN_INDEX;
const TRAIN_INDEX	NULL_TRAIN_INDEX = 0;
typedef IDWORD	RECE_TRAIN_TYPE;
typedef short	SIDE_REGION_FLAG;

typedef IDWORD	TRAIN_KEY;

typedef IBYTE		SCHEDULE_ID;		// ����ͼID��BASIC_SCHEDULE_TYPE(1) PREV_SCHEDULE_TYPE(2) ��11--11+MAX_BASIC_ACHIVE_SCHEDULE_COUNT-1Ϊ�鵵����ͼID������SCHD_TYPE�ֶ���
typedef IBYTE		CONID;				// GPC̨��  by wuxiang at 0219
typedef IWORD		GPC_ID;
typedef IBYTE		JOB_ID;
typedef char		USERNAME[20];		// �û���
typedef char		PASSWORD[20];		// ����
typedef IDWORD		SHIFT_ID;

const IWORD	MAX_STPLAN_DSTN_CNT	= 100;
const IWORD	MAX_ATTACH_ENGINE_COUNT	= 4;
const IWORD  MAX_TRAIN_ID_CHG_COUNT  = 4;
const IWORD  MAX_ABS_STOP_COUNT      = 4;
const IWORD	MAX_STPLAN_DEL_CNT	= 10;
const CONID	INVALID_CONID	= 0xFF;
typedef IBYTE     ENTRY;
typedef IBYTE     SIDE;
typedef IWORD     ABS;
const ENTRY  INVALID_ENTRY = (ENTRY)-1;
const SIDE   INVALID_SIDE  = (SIDE)-1;

//const IWORD  MAX_STA_NAME_LEN   = 32;
const IWORD  MAX_TRAIN_NAME_LEN = 24;
const IWORD  MAX_BLOCK_ELE_COUNT = 512;
const IWORD  MAX_BLOCK_TEXT_LEN		  = 256;
const IWORD  MAX_TRAIN_TEXT_LENGTH    = 256;
const IWORD  MAX_COMMENT_LENGTH       = 2048;

//------------------------------����֪ͨ�����------------------------------------

typedef IDWORD		SHUNT_INDEX;
typedef char		SHUNT_NOTE_NAME[30];
typedef IWORD		SHUNT_NOTE_FLAG;
typedef TRAIN_NO	ENGINE_NO;

typedef char		CARRIAGE_NO[10];		// ����
typedef short		STATION_PROP;			// ��վ����
typedef short		STATION_GRADE;			// ��վ����
typedef short		STATION_TECHNICAL;		// ��վ��������
typedef short		STATION_OPERATION;		// ��վҵ������
typedef short		BUREAU_NO;			// ·�֡��־ֺ�
typedef char		BUREAU_CODE[8];			// ·�֡��־֡���վ����
typedef char		BUREAU_NAME[32];		// ·�֡��־�����
typedef short		SIDE_NO;			// �ɵ���
typedef short		SIDE_TONRANK;			// ��ֵȼ�
typedef	char		SIDE_NAME[20];			// �ɵ���
typedef short		SHUNT_TIME;
typedef char		SIDE_REMARK[20];

const IWORD	MAX_SHUNTNOTE_DSTN_CNT	= 100;
const IWORD	MAX_SHUNTNOTE_STATION_CNT	= 10;
const IWORD	MAX_SHUNTNOTE_STORECAR_CNT	= 10;
const IWORD	MAX_SHUNTNOTE_STORECAR_LEN	= 10;
const IWORD	MAX_SHUNTNOTE_PICPACK_LEN	= 10;

// ----------------------------- �������----------------------------------------------
typedef IBYTE	ALARM_TYPE;			// ������Ϣ������
const ALARM_TYPE	TRAIN_ALARM	= 0;	// �г�����
const ALARM_TYPE	SYS_ALARM		= 1;	// ϵͳ����

typedef IWORD	ALARM_ID;		// ��������ID
// ���ɻ�����
const ALARM_ID	ALARM_LIRC_COMMON_INFO				= 0; // һ��ͨ����Ϣ
const ALARM_ID	ALARM_LIRC_SYS_ERROR					= 1; // ϵͳ�߼�����
const ALARM_ID	ALARM_LIRC_CIS_COMM_FAIL				= 2; // ΢������ͨ�Ź���
const ALARM_ID	ALARM_LIRC_CIS_SYS_INFO				= 3; // ΢������ϵͳ����
const ALARM_ID	ALARM_LIRC_DOB_SEND_FAIL				= 4; // DOB���ʧ��
const ALARM_ID	ALARM_LIRC_DIB_COMM_FAIL				= 5; // DIBͨ�Ź���
const ALARM_ID	ALARM_LIRC_MS_COMM_FAIL				= 6; // ����ͨ�Ź���
const ALARM_ID	ALARM_LIRC_MASTER_SWITCH			= 7; // ���ɻ������л�
const ALARM_ID	ALARM_LIRC_CTRL_MODE_SWITCH			= 8; // ��վ����ģʽת��
const ALARM_ID	ALARM_LIRC_ROUTE_MODE_SWITCH		= 9; // ��·����ģʽת��
const ALARM_ID	ALARM_LIRC_SHUNT_COLLISION			= 10; // �������г��ƻ���ͻ
const ALARM_ID	ALARM_LIRC_TRAIN_COLLISION			= 11; // �г����г��ƻ���ͻ
const ALARM_ID	ALARM_LIRC_MANUAL_INTERLOCK_FAIL	= 12; // �˹������·ʱ������ͻ
const ALARM_ID	ALARM_LIRC_AUTO_INTERLOCK_FAIL		= 13; // �Զ������·ʱ������ͻ
const ALARM_ID	ALARM_LIRC_SIDE_COLLISION				= 14; // �г��ƻ��ɵ����ͳ�ͻ
const ALARM_ID	ALARM_LIRC_TRAINPLAN_INVALID			= 15; // �г��ƻ���Ϣ��Ч
const ALARM_ID	ALARM_LIRC_TRAINROUTE_MISMATCH		= 16; // �г���·ָ�����վ��ʵ�ʲ���
const ALARM_ID	ALARM_LIRC_INPUT_TRAINID_MISMATCH	= 17; // �г���·ָ������˹�����ĳ��κŲ���
const ALARM_ID	ALARM_LIRC_TRAINPLAN_NOTPROC		= 18; // �г��ƻ��޷��Զ�����
const ALARM_ID	ALARM_LIRC_CTC_TIMEOUT				= 19; // �豸������ʱ
const ALARM_ID	ALARM_LIRC_FORCE_CREATE_ROUTE		= 20; // �˹�ǿ�ư����ͻ��·
const ALARM_ID	ALARM_LIRC_MANUAL_TRIG				= 21; // �Զ�����תΪ�˹�����
const ALARM_ID	ALARM_LIRC_AUTO_RETRIG				= 22; // �Զ����°����·
const ALARM_ID	ALARM_LIRC_SHUNT_NOT_CLEAR			= 23; // ������·û�м�ʱ����
const ALARM_ID	ALARM_LIRC_DUP_SHUNTNOTE			= 24; // ������ҵ�����ظ�
const ALARM_ID	ALARM_LIRC_WIRELESS					= 25; // ����ͨ�ű���
const ALARM_ID	ALARM_LIRC_CTC_CHECK					= 26; // վ����ⱨ��
const ALARM_ID	ALARM_LIRC_CENTER_DOWN				= 27; // ���ɻ��������ж�
const ALARM_ID	ALARM_LIRC_STA_DOWN					= 28; // ���ɻ��복վ�ж�
const ALARM_ID	ALARM_LIRC_TRAIN_ROUTE_LATE			= 29; // �г���·δ��ʱ����
const ALARM_ID	ALARM_LIRC_LINE_COLLISION				= 30; // �г��ƻ���·���ͳ�ͻ
const ALARM_ID	ALARM_LIRC_ELEC_MISMATCH				= 31; // ������־��һ��
const ALARM_ID	ALARM_LIRC_STCC_INACTIVE				= 32; // �п�ͨ���ж�
const ALARM_ID	ALARM_LIRC_STCC_UNINITED				= 33; // �п�δ��ʼ��
const ALARM_ID	ALARM_LIRC_STCC_INITED				= 34; // �п�ͨ�Żָ�
const ALARM_ID	ALARM_LIRC_TRAINPLAN_TIME_MISMATCH	= 35; // �ƻ�����˳����Ԥ�Ʋ�һ��
const ALARM_ID	ALARM_LIRC_TCC_INPUT_MISMATCH		= 36; // �п����ɲ�һ��
const ALARM_ID	ALARM_LIRC_RES_DOWN		                     = 37; // ���ɻ�����ԴӦ��������豸�ж�
const ALARM_ID	ALARM_LIRC_RES_UNDESTECT		              = 38; //��ԴӦ����δ���
const ALARM_ID	ALARM_LIRC_RES_SYS_FAIL		              = 39; //��ԴӦ����ϵͳ����
const ALARM_ID	ALARM_LIRC_RES_TCC_LEU_FAIL		       = 40;//�п���LEU����
const ALARM_ID	ALARM_LIRC_RES_LEU_RES_FAIL		       = 41;//LEU��Ӧ��������
const ALARM_ID	ALARM_LIRC_RES_RES_CHECK_FAIL		= 42;//��ԴӦ��������У�˴���
const ALARM_ID	ALARM_LIRC_RES_DEV_FAIL		 		= 43;//Ӧ��������豸����
const ALARM_ID	ALARM_LIRC_IFS_SIGNALCLOSE		 	= 44;//�źŷ������ر�
const ALARM_ID	ALARM_LIRC_TRAIN_PREDICT_NOTACK		= 45;//��վδͬ�ⷢ��
const ALARM_ID	ALARM_LIRC_TRAIN_PREDICT_MISMATCH	= 46;//��վԤ����Ϣ��ƥ��
const ALARM_ID	ALARM_LIRC_ROUTE_SET_OK				= 47;	// ��·�Ѱ�����ʾ
const ALARM_ID	ALARM_LIRC_ROUTE_SET_ERR				= 48;	// ��·��챨��
const ALARM_ID	ALARM_LIRC_TRAINPLAN_SYS_ERROR		= 49; // �г��ƻ�����ϵͳ�߼�����

const ALARM_ID  ALARM_LIRC_TDCS3_ALARM_BASE			= 65;//TDCS3.0���� < ALARM_LIRC_IP_ALARM_BASE 


const ALARM_ID	ALARM_LIRC_IP_ALARM_BASE				= 100; // ���ɻ�IP����

const ALARM_ID	ALARM_LIRC_TDCS3_EXT_ALARM_BASE			= 150;

const ALARM_ID	ALARM_TSRI_ALARM_BASE				= 10000; // TSR����

const ALARM_ID	ALARM_CAD_ALARM_BASE				= 20000; // CAD����
const ALARM_ID	ALARM_CAD_TASK_DEAD					= 20001; // CAD�߳���
const ALARM_ID	ALARM_CAD_CONN_OK					= 20002; // CAD��������COMMSVR
const ALARM_ID	ALARM_CAD_DB_CONN					= 20003; // CAD�����������ݿ�
const ALARM_ID	ALARM_CAD_DB_DISCONN				= 20004; // CAD�߳������ݿ�Ͽ�
const ALARM_ID	ALARM_CAD_CONN_FAIL					= 20005; // CAD����Ͽ�COMMSVR

const ALARM_ID	ALARM_STPC_NOT_SATISFY_ARR			= 30000; // �����ն˳��򱨾���������ӳ�����
const ALARM_ID	ALARM_STPC_NOT_SATISFY_DPT			= 30001; // �����ն˳��򱨾��������㷢������
const ALARM_ID	ALARM_STPC_ROUTE_MISMATCH_PLAN		= 30002; // �����ն˳��򱨾����ӳ��ƻ���һ��
const ALARM_ID	ALARM_STPC_FLOW_ERR			        = 30003; // �����ն˳��򱨾����ӷ��������쳣
const ALARM_ID	ALARM_STPC_NO_PLAN			        = 30004; // �����ն˳��򱨾����г���־���޴˳�


const IWORD		MAX_ALARM_CONTENT_LEN	= 256;	// ����������󳤶�

// ----------------------------- ��¼���----------------------------------------------
const IWORD		LOGIN_MAX_STATION_COUNT	= 1024;	// ��¼��Ϣ�а�����վ�����ֵ
const IWORD		LOGIN_MAX_ENTITY_COUNT	= 2048;	// ��¼��Ϣ�а�����ʵ�������ֵ
const short		MAX_SCHEDULE_NAME_LEN	= 256;	// ����ͼ������󳤶�

typedef IBYTE	LOG_REPLY_TYPE;		// ��¼��Ӧ����
const LOG_REPLY_TYPE	LOGIN_SUCCESS	= 0;	// ��¼�ɹ�
const LOG_REPLY_TYPE	LOGIN_RELOG		= 1;	// �ظ���¼
const LOG_REPLY_TYPE	LOGIN_INVALID	= 2;	// ��¼��Ϣ�Ƿ�
// ------------------------------GSMR�����Ϣ------------------------------------------
const IBYTE     MAX_GOU_PER_SHUNT   =16;
const IBYTE     MAX_TRAIN_POSITION  =8;
const IBYTE     MAX_STATION_LIST    =8;

// -----------------------------�ۺ�ά������-------------------------------------------
// �������
const IBYTE	ATTR_RUN	= 1;	// ��������
const IBYTE	ATTR_CONN	= 2;	// ��������
const IBYTE	ATTR_CAPA	= 3;	// ��������
const IBYTE	ATTR_QUE	= 4;	// ��̬��������
const IBYTE	ATTR_TIME	= 5;	// ʱ������
const IBYTE	ATTR_EVENT	= 6;	// �¼���������
const IBYTE	ATTR_USER	= 7;	// �û��Զ�������

// �������
const IBYTE	CMSALARM_RUN		= 1;	// ����״̬�쳣
const IBYTE	CMSALARM_CONN		= 2;	// ����״̬�쳣
const IBYTE	CMSALARM_CAPA		= 3;	// ��������
const IBYTE	CMSALARM_TIME		= 4;	// ʱ�䲻ͬ��
const IBYTE	CMSALARM_EVENT		= 5;	// �¼�����

//����ctci��������Ϣ���
const IWORD MSG_GSMR_TRAININFO=1; //���߳�����Ϣ
const IWORD MAX_GSMR_CONTENT_LEN=139; //��Ϣ������󳤶�
CLBTH_NAMESPACE_END

#endif


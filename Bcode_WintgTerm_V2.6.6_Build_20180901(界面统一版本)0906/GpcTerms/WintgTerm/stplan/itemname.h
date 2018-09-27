#ifndef _ITEM_NAME_
#define _ITEM_NAME_
/* some log use chinese name */

#define SEND_STPLAN_ORDER     \
	"%s-%s �´﷢�ͼƻ������վ�� %02d ̨�� %02d �����豸��%04d �����豸��%04d ������%s ����ʱ�� %s-%s ʱ�䷶Χ (%s-%s %s-%s)"

#define REQUEST_STPLAN        \
	"%s-%s �յ��ƻ����������վ�� %02d ̨�� %02d �����豸��%04d ʱ�䷶Χ (%s-%s %s-%s)"

#define SEND_STPLAN_BEGIN     \
	"%s-%s ��ʼ���ͼƻ��� ��վ�� %02d ̨�� %02d �����豸��%04d �����豸��%04d ������%s ����ʱ�� %s-%s"

#define SEND_STPLAN_INSERT    \
	"%s-%s �������Ӽƻ��� ��վ�� %02d ̨�� %02d �����豸��%04d �����豸��%04d �г����� %d-%d �г�����%d ���еȼ�%d ���ﳵ�� %s �������� %s ����ʱ�� %s-%s(ͼ������ %d) ����ʱ�� %s-%s(ͼ������ %d) ��� %d �ɵ� %d %d ���� %d ��Сͣվʱ�� %d ������ǰ����ʱ�� %x ���ø��� %d ״̬λ(status) %02X ͣ��״̬(stop_condition) %02X ��־λ(flag) %02X"

#define SEND_BASICSTPLAN_INSERT    \
	"���ͻ���ͼ�ƻ��� ��վ�� %02d �����豸��%04d ���ﳵ�� %s �������� %s ����ʱ�� %s(%d) ����ʱ�� %s(%d) ��� %d �ɵ� %d ���� %d ǰһվ %d ��һվ %d"

#define SEND_STPLAN_MEET_SAME_DIR    \
	"�ó� ���� %d-%d "

#define SEND_STPLAN_MEET_DIFF_DIR    \
	"�ᳵ ���� %d-%d "

#define DEPART_TRAIN_BY_PLAN    \
	"���ƻ����� %d "

#define SEND_STPLAN_DELETE    \
	"%s-%s ����ɾ���ƻ��� ��վ�� %02d ̨�� %02d �����豸��%04d �����豸��%04d �г����� %d-%d ���ﳵ�� %s �������� %s ����ʱ�� %s-%s ����ʱ�� %s-%s"

#define SEND_STPLAN_END       \
	"%s-%s �������ͼƻ��� ��վ�� %02d ̨�� %02d �����豸��%04d �����豸��%04d ���Ӹ��� %d ɾ������ %d"

#define SEND_STPLAN_ANSWER_PLAN_RECV          \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �ƻ�����"

#define SEND_STPLAN_ANSWER_PLAN_SIGN          \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �ƻ�ǩ��"

#define SEND_STPLAN_ANSWER_PLAN_SENDING       \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �ƻ�������"

#define SEND_STPLAN_ANSWER_PLAN_NO_CHANGE     \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �ƻ��ޱ仯"

#define SEND_STPLAN_ANSWER_PLAN_NO_DATA       \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �޼ƻ�����"

#define SEND_STPLAN_ANSWER_PLAN_ENTITY_DEAD   \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �����豸�����ж�"

#define SEND_STPLAN_ANSWER_PLAN_CONFIG_WRONG  \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d �¼ƻ����ô���"

#define SEND_STPLAN_ANSWER_ERR                \
	"%s-%s ���ͼƻ�Ӧ�� ��վ�� %02d ̨�� %02d �����豸��%04d ����״̬"

#define SEND_STPLAN_RECV     \
	"%s-%s ���ܼƻ���ִ�� ��վ�� %02d ̨�� %02d ����"

#define SEND_STPLAN_SIGN     \
	"%s-%s ���ܼƻ���ִ�� ��վ�� %02d ̨�� %02d ǩ��"

#define SEND_STPLAN_RECV_ALL \
	"%s-%s ���ܼƻ���ִ�� ��վ�� %02d ̨�� %02d ȫ������"

#define SEND_STPLAN_SIGN_ALL \
	"%s-%s ���ܼƻ���ִ�� ��վ�� %02d ̨�� %02d ȫ��ǩ��"

#define SEND_STPLAN_RECV_WRONG \
	"%s-%s ���ܼƻ���ִ�� ��վ�� %02d ̨�� %02d ����"

#define UPDATE_STPLAN_SIDE \
	"%s-%s �����޸ļƻ��ɵ��� ��վ�� %02d ̨�� %02d �г����� %d-%d �����豸��%04d"

#define UPDATE_BASIC_SCHEDULE \
	"%s-%s ̨�� %02d %d->%d �豸��%04d ���»���ͼ %s"

#define UPDATE_BASIC_SCHEDULE_ERR_INVALID_SCHDTYPE \
	"%s-%s ̨�� %02d %d->%d ���»���ͼ������Ч������ͼ����"

#define UPDATE_BASIC_SCHEDULE_ERR_SAVE_OLD \
	"%s-%s ̨�� %02d %d->%d ���»���ͼ���󣺱��������ͼ"

#define UPDATE_BASIC_SCHEDULE_ERR_UPDATE_NEW \
	"%s-%s ̨�� %02d %d->%d ���»���ͼ����ת������ͼʧ��"

#define UPDATE_BASIC_SCHEDULE_OK \
	"%s-%s ̨�� %02d %d->%d �豸��%04d ���»���ͼ�ɹ�"

#define ENTITY_ACTIVE \
	"%s-%s �豸%04d ����"

#define ENTITY_INACTIVE \
	"%s-%s �豸%04d �Ͽ�"

#define TCC_SVR_ALARM "����δִ�е��п�����: "

#define COPY_RIGHT         "��˹���ź����޹�˾��Ȩ����"

#endif /** THE END **/

#ifndef _MSG_DEF_H_
#define  _MSG_DEF_H_

#include "private_msg_def.h"

CLBTH_NAMESPACE_BEGIN


// ������λ��־
// DMIS_STATUS
// STATION_CTC_FLAG
// SHUNT_GOU_TYPE

// ------------------ DSS ��Ϣ-----------------------------------------------------------
// վ��ICON��ʾ
class AFX_EXT_CLASS MsgDSSDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgDSSDisplay)

public:
	MsgDSSDisplay();
	virtual ~MsgDSSDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
	DEVICE_TYPE nType;  //�豸����
	DMIS_ID   nID;  //dmis��
	DEVICE_STATUS nStatus;  //�豸״̬
	IWORD   nStrLen;            // strMoreInfo�ĳ���
	char    strMoreInfo[MAX_DMIS_STR_LEN];  // ��nType�ǳ��δ�ʱ, ��ʾ���κ�; �����¼������Ϣ(һ��Ϊ��)
};
DECLARE_SERIALIZE_OPERATOR(MsgDSSDisplay)

// ���κŲ�����������ӡ�ɾ�����޸�
class AFX_EXT_CLASS MsgTrainNoOper : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainNoOper)

public:
	MsgTrainNoOper();
	virtual ~MsgTrainNoOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;  //վ��
	DMIS_ID     nWindowID;   //���δ���
	IBYTE       nOldLen;     //�ϳ��κų���
	TRAIN_NO    tnOldTrain;  //�ϳ��κ�
	IBYTE       nNewLen;     //�³��κų���
	TRAIN_NO    tnNewTrain;  //�³��κ�
	IDWORD      nIPIndex;    //IP����
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainNoOper)

// ���κ��˹�ȷ����Ϣ
class AFX_EXT_CLASS MsgTrainConfirm : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainConfirm)

public:
	MsgTrainConfirm();
	virtual ~MsgTrainConfirm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;   //վ��
	DMIS_ID     nWindowID;    //���δ���
	IBYTE       nTrainNoLen;  //���κų���
	TRAIN_NO    tnTrainNo;    //���κ�
	IDWORD      nIPIndex;     //IP����
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainConfirm)

// ���κ������޸�
class AFX_EXT_CLASS MsgSetTrainAttrib : public BaseMsg
{
	DECLARE_CLBTH(MsgSetTrainAttrib)

public:
	MsgSetTrainAttrib();
	virtual ~MsgSetTrainAttrib();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
	DMIS_ID   nWindowID;  //���δ���
	IBYTE   nTrainNoLen;  //���κų���
	TRAIN_NO  tnTrainNo;  //���κ�
	IDWORD    nIPIndex; //IP����
	short   nTrainType; //�г�����
	short   nTrainRate; //�г��ٶ�
	char    nLocoNoLen; //�����ų���
	char    szLocoNo[50]; //������
};
DECLARE_SERIALIZE_OPERATOR(MsgSetTrainAttrib)

// ����ͣ���˹�������Ϣ
class AFX_EXT_CLASS MsgSetTrainStop : public BaseMsg
{
	DECLARE_CLBTH(MsgSetTrainStop)

public:
	MsgSetTrainStop();
	virtual ~MsgSetTrainStop();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;       //վ��
	DMIS_ID     nWindowID;        //���δ���
	TRAIN_NO    tnTrainNo;        //���κ�
	IDWORD      nIPIndex;         //IP����
	enum { TRAIN_START=1, TRAIN_STOP=2 };
	IBYTE       nStopInfo;          // 1��ʾ������2��ʾ��ͣ��
};
DECLARE_SERIALIZE_OPERATOR(MsgSetTrainStop)


// վ��
class AFX_EXT_CLASS MsgStationDown : public BaseMsg
{
	DECLARE_CLBTH(MsgStationDown)

public:
	MsgStationDown();
	virtual ~MsgStationDown();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgStationDown)

// վ��
class AFX_EXT_CLASS MsgStationUp : public BaseMsg
{
	DECLARE_CLBTH(MsgStationUp)

public:
	MsgStationUp();
	virtual ~MsgStationUp();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgStationUp)

class AFX_EXT_CLASS MsgDssInitComplete : public BaseMsg
{
	DECLARE_CLBTH(MsgDssInitComplete)

public:
	MsgDssInitComplete();
	virtual ~MsgDssInitComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDssInitComplete)

// DSS��վ�豸�˹�����(�����������õ�)
class AFX_EXT_CLASS MsgDSSManualDeviceOper : public BaseMsg
{
	DECLARE_CLBTH(MsgDSSManualDeviceOper)

public:
	MsgDSSManualDeviceOper();
	virtual ~MsgDSSManualDeviceOper();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;       //վ��
	MANUAL_OPER_TYPE  nOperType;        //��������
	DEVICE_TYPE       nDeviceType;      //�豸����
	DMIS_ID           nDeviceID;        //�豸dmis��
	I_TIME            tmOperTime;       //����ʱ��
	char              strDeviceName[20];//�豸����
	short             nOperPara;        //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgDSSManualDeviceOper)

// ------------------------------ ��ť���������Ϣ -----------------------------------------------

// ��ť����
class AFX_EXT_CLASS MsgManualButtonPress : public BaseMsg
{
	DECLARE_CLBTH(MsgManualButtonPress)

public:
	MsgManualButtonPress();
	virtual ~MsgManualButtonPress();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // �˹�������ΨһID��ʶ
	STATION_NO    nStationNo; //վ��
	DEVICE_TYPE   nDeviceType;  //�豸����
	DMIS_ID     nDmisID;  //dmis��
	char      strDeviceName[20];  //�豸����
	IBYTE     nOperPara;  //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgManualButtonPress)

// ��ť���
class AFX_EXT_CLASS MsgManualButtonCancel : public BaseMsg
{
	DECLARE_CLBTH(MsgManualButtonCancel)

public:
	MsgManualButtonCancel();
	virtual ~MsgManualButtonCancel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // �˹�������ΨһID��ʶ
	STATION_NO    nStationNo; //վ��
	DEVICE_TYPE   nDeviceType;  //�豸����
	DMIS_ID     nDmisID;  //dmis��
	char      strDeviceName[20];  //�豸����
	IBYTE     nOperPara;  //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgManualButtonCancel)

// ��·����
class AFX_EXT_CLASS MsgManualRouteSet : public BaseMsg
{
	DECLARE_CLBTH(MsgManualRouteSet)

public:
	MsgManualRouteSet();
	virtual ~MsgManualRouteSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif

public:
	MANUAL_OPER_ID  nOperID;  // �˹�������ΨһID��ʶ
	STATION_NO    nStationNo; //վ��
	IBYTE     nButtonCount; //��ť����
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];  //��ťID
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];  //��ť����
	ROUTE_ID    nRouteID; //��·��
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT }; //0��ʾ��Ч���г�������
	ROUTE_TYPE    nRouteType; //��·����
	I_TIME      tmStartTime;  //��·��ʼʱ��
	TIME_INTERVAL tmExistTime;  // ��·Ԥ�ƴ���ʱ��
	IBYTE     bForceFlag;   // 1:ǿ��ִ��(�����������߼����); 0:�����վϸ�ȼ��, ��ͬ
	TRAIN_NO    tnTrainNo;  //���κ�
};
DECLARE_SERIALIZE_OPERATOR(MsgManualRouteSet)

// �豸�˹�����(�����١�����������)
class AFX_EXT_CLASS MsgManualDeviceOper : public BaseMsg
{
	DECLARE_CLBTH(MsgManualDeviceOper)

public:
	MsgManualDeviceOper();
	virtual ~MsgManualDeviceOper();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif
	void SetDeviceName(const char* pstrName);

public:
	MANUAL_OPER_ID  nOperID;          //����ID
	STATION_NO    nStationNo;         //վ��
	MANUAL_OPER_TYPE  nOperType;      //��������
	DEVICE_TYPE   nDeviceType;      //�豸����
	DMIS_ID     nDeviceID;          //�豸dmis��
	I_TIME      tmOperTime;       //����ʱ��
	char      strDeviceName[20];  //�豸����
	short     nOperPara;          //��������
	IBYTE     bForceFlag;         //1:ǿ��ִ��(�����������߼����); 0:�����վϸ�ȼ��
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOper)

// �豸�ؼ��˹�����(������·ȡ�������˽��)
class AFX_EXT_CLASS MsgManualDeviceOperEx : public MsgManualDeviceOper
{
	DECLARE_CLBTH(MsgManualDeviceOperEx)

public:
	MsgManualDeviceOperEx();
	virtual ~MsgManualDeviceOperEx();
	void    MakeBaseDeviceOperMsg( MsgManualDeviceOper& rBaseMsg );

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD  nRandomPara;      // ���������
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOperEx)

// ��·����
class AFX_EXT_CLASS MsgManualRouteTryLock : public BaseMsg
{
	DECLARE_CLBTH(MsgManualRouteTryLock)

public:
	MsgManualRouteTryLock();
	virtual ~MsgManualRouteTryLock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif

public:
	MANUAL_OPER_ID  nOperID;  // �˹�������ΨһID��ʶ
	STATION_NO    nStationNo; //վ��
	IBYTE     nButtonCount; //��ť��
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];  //��ťID��
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];  //��ť����
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //��·���ͣ���Ч���г�������
	ROUTE_TYPE    nRouteType; //��·����
	ROUTE_ID    nRouteID; //��·ID
	I_TIME      tmStartTime;  //��·��ʼʱ��
	TIME_INTERVAL tmExistTime;  // ��·Ԥ�ƴ���ʱ��
	IBYTE     bForceFlag;   // 1:ǿ��ִ��(�����������߼����); 0:�����վϸ�ȼ��, ��ͬ
};
DECLARE_SERIALIZE_OPERATOR(MsgManualRouteTryLock)

// ����ʹ��
class AFX_EXT_CLASS MsgManualDeviceOperEnable : public BaseMsg
{
	DECLARE_CLBTH(MsgManualDeviceOperEnable)

public:
	MsgManualDeviceOperEnable();
	virtual ~MsgManualDeviceOperEnable();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   // վ��
	MANUAL_OPER_TYPE  nOperType;    // �������
	I_TIME        tmOperTime; // ������ʼʱ��
	IDWORD        nRandomPara;  // ���������
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOperEnable)

// ������
/*
class AFX_EXT_CLASS MsgManualSwitchOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualSwitchOper)

public:
MsgManualSwitchOper();
virtual ~MsgManualSwitchOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nSwitchID;
MANUAL_OPER_TYPE  nOperType;  // 0:����; 1:����; 2:����
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualSwitchOper)

// ���β���
class AFX_EXT_CLASS MsgManualTrackOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualTrackOper)

public:
MsgManualTrackOper();
virtual ~MsgManualTrackOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nTrackID;
MANUAL_OPER_TYPE  nOperType;  // 0:����; 1:����; 2:���; 3:���
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualTrackOper)

// �źŻ�����
class AFX_EXT_CLASS MsgManualSignalOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualSignalOper)

public:
MsgManualSignalOper();
virtual ~MsgManualSignalOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nSignalID;
MANUAL_OPER_TYPE  nOperType;  // 0:�����ر�; 1:���¿���; 2:���
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualSignalOper)
*/

// �˹������Ļ�Ӧ
class AFX_EXT_CLASS MsgManualOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgManualOperResult)

public:
	MsgManualOperResult();
	virtual ~MsgManualOperResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	bool    CanForceOper() const;
	void    SetForceOperFlag( bool b=true );
	MANUAL_OPER_RESULT  GetResult();

public:
	MANUAL_OPER_ID    nOperID;    // ���nOperID�Ļ�Ӧ
	MANUAL_OPER_RESULT  nResult;    // ������Ӧ���  0: �ɹ�, 1:������ͻ, 2:�ƻ���ͻ, 3:��Чָ��
	char        strComment[MAX_RESULT_COMMENT_LEN]; // ����˵��
	IBYTE       nCommentLen;  // ����˵���ĳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgManualOperResult)

// ------------------------------ ��վ���·����ģʽת�� ----------------------------------------------

// ��վ����ģʽת������(����GPC/STPC��LiRC�����LiRC��GPC/STPC(������)����ȷ��)
class AFX_EXT_CLASS MsgStationModeRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeRequest)

public:
	MsgStationModeRequest();
	virtual ~MsgStationModeRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID     nOriginalID;    // ԭʼ������
	STATION_NO    nStationNo;     // �����Ŀ�공վ
	ZONE_ID       nZoneID;        // ������(Ŀǰֵֻȡ0xFFFF,��ʾ��վ����ģʽ)
	STATION_CTRL_MODE nMode;      // �����ģʽ
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeRequest)

// ��վ����ģʽ������(����LiRC����GPC/STPC����������GPC/STPC����LiRC����׼���)
class AFX_EXT_CLASS MsgStationModeResult : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeResult)

public:
	MsgStationModeResult();
	virtual ~MsgStationModeResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID           nOriginalID;    // ԭʼ������
	STATION_NO          nStationNo;     // �����Ŀ�공վ
	ZONE_ID             nZoneID;        // ������(Ŀǰֵֻȡ0xFFFF,��ʾ��վ����ģʽ)
	STATION_CTRL_MODE   nMode;          // �����ģʽ
	STATION_CTRL_RESULT nResult;        // ������
	char        strComment[MAX_RESULT_COMMENT_LEN]; // ���˵��
	IBYTE       nCommentLen;    // ���˵���ĳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeResult)

// ��·����ģʽ����(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteModeRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteModeRequest)

public:
	MsgRouteModeRequest();
	virtual ~MsgRouteModeRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //վ��
	ROUTE_CTRL_MODE   nMode;      //��·����ģʽ
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteModeRequest)

// ��·����ģʽ������(LiRC->GPC/STPC)
class AFX_EXT_CLASS MsgRouteModeResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteModeResult)

public:
	MsgRouteModeResult();
	virtual ~MsgRouteModeResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_CTRL_MODE   nMode;        //�����ģʽ
	ROUTE_CTRL_RESULT nResult;      //������
	char        strComment[MAX_RESULT_COMMENT_LEN]; //���˵��
	IBYTE       nCommentLen;    //���˵���ĳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteModeResult)

// ��վ����״̬����(LiRC->GPC/STPC/BL/SERVER)
class AFX_EXT_CLASS MsgStationModeReport : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeReport)

public:
	MsgStationModeReport();
	virtual ~MsgStationModeReport();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;     // վ��
	ZONE_ID           nZoneID;        // ������
	STATION_CTRL_MODE nStationMode;   // ��վ����ģʽ
	ROUTE_CTRL_MODE   nRouteMode;     // ��·����ģʽ
	USER_RIGHT        nRight;         // �û�Ȩ��
	STATION_CTC_FLAG  nCTCFlag;       // ��վCTC������־
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeReport)

// ��վCTC������־(�ƻ�����)����
class AFX_EXT_CLASS MsgStationCTCFlagSet : public BaseMsg
{
	DECLARE_CLBTH(MsgStationCTCFlagSet)

public:
	MsgStationCTCFlagSet();
	virtual ~MsgStationCTCFlagSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //վ��
	STATION_CTC_FLAG  nFlag;      //
	STATION_CTC_FLAG  nMask;      //
};
DECLARE_SERIALIZE_OPERATOR(MsgStationCTCFlagSet)

// ��ѯ��վ����ģʽ
class AFX_EXT_CLASS MsgStationModeQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeQuery)

public:
	MsgStationModeQuery();
	virtual ~MsgStationModeQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO      nStationNo; //��վ
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeQuery)

// ------------------------------ ��·�����Ϣ ----------------------------------------------------

// ��·��������(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteListRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListRequest)

public:
	MsgRouteListRequest();
	virtual ~MsgRouteListRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //վ��
	ROUTE_LIST_OPER_TYPE  nOperType;  // ��������
	ROUTE_LIST_HANDLE   nHandle;  // ����Ľ�·����
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListRequest)

// ��·�����ͷ�(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteListRelease : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListRelease)

public:
	MsgRouteListRelease();
	virtual ~MsgRouteListRelease();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //վ��
	ROUTE_LIST_OPER_TYPE  nOperType;  // ��������
	ROUTE_LIST_HANDLE   nHandle;  // ����Ľ�·����
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListRelease)

// ��·���������Ӧlirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteListResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListResult)

public:
	MsgRouteListResult();
	virtual ~MsgRouteListResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //վ��
	ROUTE_LIST_OPER_TYPE  nOperType;  // ��������
	ROUTE_LIST_HANDLE   nHandle;  // ����Ľ�·����
	ROUTE_REQUEST_RESULT  nResult;  //��·����������
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListResult)

// ��·����,���Ľ�·����(gpc/stpc====>lirc),����
class AFX_EXT_CLASS MsgRouteUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteUpdate)

public:
	MsgRouteUpdate();
	virtual ~MsgRouteUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //���ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·���к�
	IBYTE       nButtonCount; //��ť��
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];  //��ťID
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];  //��ť����
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //��·����:��Ч���г�������
	ROUTE_TYPE    nRouteType; //��·����
	ROUTE_ID      nNewRouteID;  // �µĽ�·
	ROUTE_LIST_ID   nGiveWayTo;   // �µı��ý�·
	ROUTE_TRIG_TYPE   nTrigFlag;    // �½�·�Ĵ�������(0:�˹�����, 1:�Զ�����)
	SIDE_ID       nSideID;   //Ҫ���ĵĹɵ��� ֵΪ��1��ʾ������
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteUpdate)

// ��·�ɵ�����gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteSideUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteSideUpdate)

public:
	MsgRouteSideUpdate();
	virtual ~MsgRouteSideUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //����ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·���к�
	SIDE_ID       nSideID;   //Ҫ���ĵĹɵ�
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteSideUpdate)

// ��·��ť����gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteButtonUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteButtonUpdate)

public:
	MsgRouteButtonUpdate();
	virtual ~MsgRouteButtonUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //վ��
	ROUTE_LIST_HANDLE nHandle;  //����ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·���к�
	IBYTE       nButtonCount; //��ť��
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];  //��ťID
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];  //��ť����
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //��·���ͣ���Ч���г�������
	ROUTE_TYPE    nRouteType; //��·����
	ROUTE_ID      nNewRouteID;  // �µĽ�·
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteButtonUpdate)

// ������·�����г����θ���gpc/stpc====>lirc,
class AFX_EXT_CLASS MsgRouteGiveWayUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGiveWayUpdate)

public:
	MsgRouteGiveWayUpdate();
	virtual ~MsgRouteGiveWayUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //����ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·���к�
	ROUTE_LIST_ID   nGiveWayTo;   // �µı��ý�·
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGiveWayUpdate)

// ��·�Զ���������
class AFX_EXT_CLASS MsgRouteTrigFlagUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteTrigFlagUpdate)

public:
	MsgRouteTrigFlagUpdate();
	virtual ~MsgRouteTrigFlagUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //����ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·���к�
	ROUTE_TRIG_TYPE   nTrigFlag;    // �½�·�Ĵ�������(0:�˹�����, 1:�Զ�����)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteTrigFlagUpdate)

// ��·��ϸ���gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteGroupUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupUpdate)

public:
	MsgRouteGroupUpdate();
	virtual ~MsgRouteGroupUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; // վ��
	ROUTE_LIST_HANDLE nHandle;    //����ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // Ҫ���ĵĽ�·
	ROUTE_ID      nNewRouteID;  // �µ�Ԥ�����·ID
	ROUTE_ID      nzRoutes[MAX_ROUTE_COUNT_IN_ONE_PLAN];  // �µ�Ԥ������ϰ����Ľ�·
	IWORD       nRouteCount;  // �µ�Ԥ������ϰ����Ľ�·����
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupUpdate)


// ��·���� (����)
class AFX_EXT_CLASS MsgRouteInsert : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInsert)

public:
	MsgRouteInsert();
	virtual ~MsgRouteInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//վ��
	ROUTE_LIST_HANDLE nHandle;//����ĵĽ�·����
	TRAIN_NO      tnTrainNo;//���κ�
	IBYTE       nButtonCount;//��ť��
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];//��ťID��
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];//��ť����
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };//0��ʾ��Ч���г�������
	ROUTE_TYPE    nRouteType;//��·����
	ROUTE_ID      nNewRouteID;  // �µĽ�·
	ROUTE_LIST_ID   nNewPos;    // ��������λ��(��nNewPos֮��,��nNewPosΪ0,���뵽��λ)
	SHUNT_GOU_INDEX   nGouIndex;    // ��������(�г���·ʱΪ0)
	ROUTE_LIST_ID   nGiveWayTo;   // ���ý�·
	ROUTE_TRIG_TYPE   nTrigFlag;    // �½�·�Ĵ�������(0:�˹�����, 1:�Զ�����)
	IWORD       nOperID;    // �������(����Ϣ���ͷ�����)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInsert)

// ��·ɾ��gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteDelete : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteDelete)

public:
	MsgRouteDelete();
	virtual ~MsgRouteDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//վ��
	ROUTE_LIST_HANDLE nHandle;//���޸ĵĽ�·����
	ROUTE_LIST_ID   nListID;//��·�б�ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteDelete)

// ��·˳�������û�ã�
class AFX_EXT_CLASS MsgRouteMove : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteMove)

public:
	MsgRouteMove();
	virtual ~MsgRouteMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //���޸ĵĽ�·����
	ROUTE_LIST_ID   nListID;    // ���ƶ��Ľ�·
	ROUTE_LIST_ID   nNewPos;    // �ƶ�����λ��(��nNewPos֮��,��nNewPosΪ0,�ƶ�����λ)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteMove)

// ��·�˹�����gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteTrig : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteTrig)

public:
	MsgRouteTrig();
	virtual ~MsgRouteTrig();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//վ��
	ROUTE_LIST_HANDLE nHandle;//���޸ĵĽ�·����
	ROUTE_LIST_ID   nListID;//���޸ĵĽ�·����ID
	I_TIME        tmTrigTime;//����ʱ��
	IBYTE       bForceFlag;   // �Ƿ�ǿ�ƴ���(�����мƻ���ͻ���)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteTrig)

// ������·�鴥��gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteGroupTrig : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupTrig)

public:
	MsgRouteGroupTrig();
	virtual ~MsgRouteGroupTrig();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//վ��
	ROUTE_LIST_HANDLE nHandle;//���޸ĵĽ�·����
	IWORD       nGroupID;//��·��ID
	I_TIME        tmTrigTime;//����ʱ��
	IBYTE       bForceFlag;   // �Ƿ�ǿ�ƴ���(�����мƻ���ͻ���)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupTrig)

// ��·������Ӧ
class AFX_EXT_CLASS MsgRouteOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteOperResult)

public:
	MsgRouteOperResult();
	virtual ~MsgRouteOperResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	bool    CanForceOper() const;
	void    SetForceOperFlag( bool b=true );
	ROUTE_OPER_RESULT GetResult();

public:
	STATION_NO      nStationNo;//վ��
	ROUTE_LIST_HANDLE nHandle;//�����Ľ�·����
	ROUTE_LIST_ID   nListID;//��·����ID
	enum OPER_TYPE
	{
		ROUTE_INSERT=0, //��·����
		ROUTE_DELETE, //��·ɾ��
		ROUTE_UPDATE, //��·����
		ROUTE_MOVE, //��·�ƶ�
		ROUTE_TRIG  //��·����
	};
	OPER_TYPE     nOperType;//��������
	ROUTE_OPER_RESULT nResult;//��·�������
	char        strComment[MAX_RESULT_COMMENT_LEN]; // ���˵��
	IBYTE       nCommentLen;    // ���˵���ĳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteOperResult)

// ��·�������Ӧ
class AFX_EXT_CLASS MsgRouteGroupOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupOperResult)

public:
	MsgRouteGroupOperResult();
	virtual ~MsgRouteGroupOperResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //վ��
	ROUTE_LIST_HANDLE nHandle;  //�����Ľ�·
	IWORD       nGroupID; //��·��ID
	enum OPER_TYPE
	{
		ROUTE_INSERT=0, //��·����
		ROUTE_DELETE, //��·ɾ��
		ROUTE_UPDATE, //��·����
		ROUTE_MOVE, //��·�ƶ�
		ROUTE_TRIG  //��·����
	};
	OPER_TYPE     nOperType;  //��������
	ROUTE_OPER_RESULT nResult;  //��·�������
	char        strComment[MAX_RESULT_COMMENT_LEN]; // ���˵��
	IBYTE       nCommentLen;    // ���˵���ĳ���
	IBYTE       nCanForceOper;    // �Ƿ����ǿ��
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupOperResult)

// ��·����ʾ��Ϣ
class AFX_EXT_CLASS MsgRouteLongInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteLongInfo)

public:
	MsgRouteLongInfo();
	virtual ~MsgRouteLongInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;          //վ��
	ROUTE_LIST_HANDLE nHandle;             //���޸ĵĽ�·
	ROUTE_LIST_ID     nListID;             //��·ID
	ROUTE_LIST_ID     nPos;                // λ��,��nPos֮��(��nPosΪ0, ������λ)
	TRAIN_NO          tnTrainNo;           //���κ�
	IBYTE             nButtonCount;        //��ť��
	BUTTON_ID         nButtonID[MAX_ROUTE_BUTTON_COUNT];  //��ťID
	ROUTE_ID          nRouteID;            //��·ID
	ROUTE_TYPE        nType;               //��·����
	SIDE_ID           nSideID;             //�ɵ�ID
	ENTRY_ID          nEntryID;            //��ں�
	ENTRY_ID          nExitID;             //���ں�
	SHUNT_GOU_INDEX   nGouIndex;           //����������
	SHUNT_GOU_TYPE    nGouType;            //����������
	ROUTE_LIST_ID     nGiveWayTo;          //���ý�·
	ROUTE_STATUS      nStatus;             //��·״̬
	I_TIME            tmStart;             //��ʼʱ��
	TIME_INTERVAL     tOccupy;             //ռ��ʱ��
	IWORD             nGroupID;            //��ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteLongInfo)

// ��·����ʾ��Ϣ��lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteLongInfoNew : public MsgRouteLongInfo
{
	DECLARE_CLBTH(MsgRouteLongInfoNew)

public:
	MsgRouteLongInfoNew();
	virtual ~MsgRouteLongInfoNew();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        tmPlan;       // �ƻ�ִ��ʱ��
	TRAIN_INDEX   nTrainIndex;  // �г���train_index
	IBYTE         nRecIndex;    // �г���rec_index
	char          strDesc[MAX_DMIS_STR_LEN];    // ��·��������
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteLongInfoNew)


// ��·�̱�ʾ��Ϣ(��ֻ��ROUTE_STATUS�ı�ʱ���Ͷ̱�ʾ)lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteShortInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteShortInfo)

public:
	MsgRouteShortInfo();
	virtual ~MsgRouteShortInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //վ��
	ROUTE_LIST_HANDLE nHandle;    //��·
	ROUTE_LIST_ID   nListID;      //��·ID
	ROUTE_STATUS    nStatus;      //��·״̬
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteShortInfo)

// ��·��ʾɾ��lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteInfoDel : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoDel)

public:
	MsgRouteInfoDel();
	virtual ~MsgRouteInfoDel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //վ��
	ROUTE_LIST_HANDLE nHandle;    //��ɾ���Ľ�·
	ROUTE_LIST_ID   nListID;      //��·ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoDel)

//û��
class AFX_EXT_CLASS MsgRouteInfoMove : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoMove)

public:
	MsgRouteInfoMove();
	virtual ~MsgRouteInfoMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;  //վ��
	ROUTE_LIST_HANDLE nHandle;   //���ƶ��Ľ�·
	ROUTE_LIST_ID   nMoveID;     //�ƶ���Ľ�·ID
	ROUTE_LIST_ID   nPrevID;     //�ƶ�ǰ�Ľ�·ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoMove)

// ��·��ʾˢ��lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteInfoRefresh : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoRefresh)
public:
	MsgRouteInfoRefresh();
	virtual ~MsgRouteInfoRefresh();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO      nStationNo;
	ROUTE_LIST_HANDLE nHandle;
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoRefresh)

// ��·���ȼ��Ƚ� ��û�ã�lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteCompare : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteCompare)
public:
	MsgRouteCompare();
	virtual ~MsgRouteCompare();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;         // վ��
	enum { MAX_ROUTE_COUNT = 16 };
	struct
	{
		TRAIN_NO tnTrain;                           // ���κ�
		ROUTE_TRAIN_TYPE nRunType;                  // �ӷ�������
		ROUTE_LIST_ID nListID;                      // ��·���
		char strDesc[64];                           // ��·˵��
	}       Routes[MAX_ROUTE_COUNT];
	IBYTE         nRouteCount;                    // Routes����Ԫ�ظ���
	TIME_INTERVAL nTimeout;                       // ��ʱʱ��
	IBYTE         nResult;                        // �˹�ָʾ���
	IBYTE         nzPriority[MAX_ROUTE_COUNT];    // ����˳��

	enum
	{
		RS_NOW,         // ���ڰ���
		RS_DELAY,       // �ӳٰ���
		RS_MANUAL,      // �ֹ�����
		RS_SYS_DEFAULT  // ��ϵͳĬ��
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteCompare)


// ----------------------- ���ɻ�֮�䡢���ɻ���Baselevel֮�����Ϣ ------------------------------------------------------------

// ��λȫ��
class AFX_EXT_CLASS MsgBitblock : public BaseMsg
{
	DECLARE_CLBTH(MsgBitblock)

public:
	MsgBitblock();
	virtual ~MsgBitblock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;               //վ��
	IWORD     nBitblockLen;                 //��λ����
	IBYTE     szBitblock[MAX_BITBLOCK_LEN]; //��λ
};
DECLARE_SERIALIZE_OPERATOR(MsgBitblock)

// ��λ�仯
class AFX_EXT_CLASS MsgIndiChange : public BaseMsg
{
	DECLARE_CLBTH(MsgIndiChange)

public:
	MsgIndiChange();
	virtual ~MsgIndiChange();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;//վ��
	IWORD     nIndiChangeCount;//��λ�仯��
	INDI_CHANGE   szIndiChange[MAX_INDI_PER_MSG];//��λ�仯
};
DECLARE_SERIALIZE_OPERATOR(MsgIndiChange)

// ����״̬����
class AFX_EXT_CLASS MsgMasterStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgMasterStatus)

public:
	MsgMasterStatus();
	virtual ~MsgMasterStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo; //վ��
	IBYTE     nMasterFlag;    // ������־(0:����, 1:����)
	IBYTE     nABFlag;      // AB����־(0:A��, 1:B��)
};
DECLARE_SERIALIZE_OPERATOR(MsgMasterStatus)

// ���ɻ�״̬����(����MMI��س���)
class AFX_EXT_CLASS MsgLiRCStatusReport : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCStatusReport)

public:
	MsgLiRCStatusReport();
	virtual ~MsgLiRCStatusReport();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;//վ��
	WORK_STATUS nNetStatus;//����״̬
	IBYTE     bHasCISConn;//�����Ƿ�����
	IBYTE     bHasBoardConn;//comm���Ƿ�����
	IBYTE     bHasSimuConn;//ģ���Ƿ�����
	WORK_STATUS nCISStatus[2];//����ͨ��״̬
	IDWORD      nDIBStatus[2];//DIBͨ��״̬
	IDWORD      nDOBStatus[2];//DOBͨ��״̬
	IWORD     nDIBStart;//DIB��ʼ����
	IWORD     nDIBCount;//DIB����
	IWORD     nDOBStart;//DOB��ʼ����
	IWORD     nDOBCount;//DOB����
	WORK_STATUS nSimuStatus;//ģ��״̬
	WORK_STATUS nWirelessStatus;//����״̬
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCStatusReport)

// ���ɻ�״̬����(���͵���ά�޻�)
class AFX_EXT_CLASS MsgLiRCStatusReportNew : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCStatusReportNew)

public:
	MsgLiRCStatusReportNew();
	virtual ~MsgLiRCStatusReportNew();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;
	enum
	{
		ST_MASTER = 0x01,
		ST_SLAVE  = 0x10
	};
	IBYTE     nMasterStatus;    // ����״̬

	enum
	{
		ST_MCA    = 0x01,
		ST_MCB    = 0x10
	};
	IBYTE     nABFlag;      // AB����־

	enum
	{
		WORK_ON = 0x01,
		WORK_OFF  = 0x10,
		NO_CONN = 0x11
	};
	IBYTE     nNetStatus;     // ����״̬
	IBYTE     nStbyStatus;      // �ȱ�����״̬
	IBYTE     nInternalStatus;    // �ڲ�����״̬
	IBYTE     nCISStatus[2];    // ����ͨ��״̬
	IBYTE     nStccStatus[2];   // �п�ͨ��״̬
	IBYTE     nWirelessStatus;    // ����ͨ��״̬
	IWORD     nDIBCount;      // ����
	IDWORD      nDIBStatus[2];    // ��״̬
	IBYTE     nOtherStatus[8];    // Ԥ��
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCStatusReportNew)


class AFX_EXT_CLASS MsgCTCOutput : public BaseMsg
{
	DECLARE_CLBTH(MsgCTCOutput)

public:
	MsgCTCOutput();
	virtual ~MsgCTCOutput();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;//վ��
	I_TIME      tm;//
	IBYTE     nCtrlID;//
	IBYTE     nHandleCount;//
	short     nOutputHandles[MAX_CTC_OUTPUT_HANDLE_COUNT];//
	short     nInputHandles[MAX_CTC_OUTPUT_HANDLE_COUNT];//
};
DECLARE_SERIALIZE_OPERATOR(MsgCTCOutput)

// Ԥ�Ƴ���ʱ��
class AFX_EXT_CLASS MsgTrainDepartTime : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDepartTime)
public:
	MsgTrainDepartTime();
	virtual ~MsgTrainDepartTime();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;//վ��
	TRAIN_INDEX   nTrainIndex;//��������
	TRAIN_NO    tnTrainNo;//���κ�
	I_TIME      tTime;  //����ʱ��
	IBYTE     nTimeSource;//
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDepartTime)

// ���κŲ���
class AFX_EXT_CLASS MsgIPTrainOper : public BaseMsg
{
	DECLARE_CLBTH(MsgIPTrainOper)
public:
	MsgIPTrainOper();
	virtual ~MsgIPTrainOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum TOP_TYPE { INIT, COMMON, SYNC, EXTERNAL };
	TOP_TYPE  nType;
	IBYTE nzInfo[128];
	IWORD nInfoLen;
};
DECLARE_SERIALIZE_OPERATOR(MsgIPTrainOper)

// ��վ����Ԫ��CTC״̬
class AFX_EXT_CLASS MsgIconCTCStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgIconCTCStatus)
public:
	MsgIconCTCStatus();
	virtual ~MsgIconCTCStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IBYTE nzInfo[128];//״̬����
	IWORD nInfoLen;//���ݳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgIconCTCStatus)

// --------------------------- ������Ϣ --------------------------------------------------------------------

class AFX_EXT_CLASS MsgAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarm)

public:
	MsgAlarm();
	virtual ~MsgAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	virtual I_BOOL NeedRespond();

public:
	ALARM_TYPE    nType;     // ϵͳ�������г�����
	IBYTE         nVerbose;  // ��������(1-3��)
	ALARM_ID      nAlarmID;  // �������
	I_TIME        nTime;     // ����ʱ��
	char          szContent[MAX_ALARM_CONTENT_LEN]; // ������������
	IWORD         nLen;                             // �������ݳ���
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarm)

class AFX_EXT_CLASS MsgAlarmEx : public MsgAlarm
{
	DECLARE_CLBTH(MsgAlarmEx)

public:
	MsgAlarmEx();
	virtual ~MsgAlarmEx();

public:
	virtual I_BOOL NeedRespond();

public:
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmEx)

class AFX_EXT_CLASS MsgAlarmExd: public MsgAlarm
{
	DECLARE_CLBTH(MsgAlarmExd)

public:
	MsgAlarmExd();
	MsgAlarmExd(const MsgAlarm &msg, ENTITY_ID src);
	virtual ~MsgAlarmExd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	virtual I_BOOL NeedRespond();

public:
	ENTITY_ID   source;

};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmExd)

// ����ע����Ϣ
class AFX_EXT_CLASS MsgAlarmReg : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarmReg)

public:
	MsgAlarmReg();
	virtual ~MsgAlarmReg();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   nSrcEntityID;   // ����Դʵ���
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};
	ReqStatus  nStatus;         // ע���ر�ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmReg)

// ������ѯ��Ϣ
class AFX_EXT_CLASS MsgAlarmReq : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarmReq)

public:
	MsgAlarmReq();
	virtual ~MsgAlarmReq();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   nSrcEntityID;   // ����Դʵ��� NULL_ENTITY--ȫ��
	//  ALARM_TYPE    nType;        // ϵͳ�������г����� -1--ȫ��
	//  IBYTE     nVerbose;     // ��������(1-3��) -1--ȫ��
	//  ALARM_ID    nAlarmID;     // ������� -1--ȫ��
	I_TIME      nStartTime;     // ��ѯʱ�䷶Χ��ʼ
	I_TIME      nEndTime;     // ��ѯʱ�䷶Χ����
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmReq)

// --------------------------- ��¼��Ϣ --------------------------------------------------------------------
class AFX_EXT_CLASS MsgLogin : public BaseMsg
{
	DECLARE_CLBTH(MsgLogin)

public:
	MsgLogin();
	virtual ~MsgLogin();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD        nStationCount;                      // վ����
	STATION_NO   Stations[LOGIN_MAX_STATION_COUNT];  // վ���б�
	IWORD        nEntityCount;                       // ʵ����
	ENTITY_ID    Entities[LOGIN_MAX_ENTITY_COUNT];   // ʵ����б�
	IBYTE        nMasterFlag;        // ������־(0:����, 1:����)
	IBYTE        nABFlag;            // AB����־(0:A��, 1:B��)
};                              
DECLARE_SERIALIZE_OPERATOR(MsgLogin)

class AFX_EXT_CLASS MsgLoginReply : public BaseMsg
{
	DECLARE_CLBTH(MsgLoginReply)

public:
	MsgLoginReply();
	virtual ~MsgLoginReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	LOG_REPLY_TYPE  nReplyType;     // ��¼��Ӧ��
};
DECLARE_SERIALIZE_OPERATOR(MsgLoginReply)

//�����ն�����������Ϣ
class AFX_EXT_CLASS MsgDataStyleReq:public BaseMsg
{
	DECLARE_CLBTH(MsgDataStyleReq)

public:
	MsgDataStyleReq();
	virtual ~MsgDataStyleReq();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum DataStyle
	{
		DISPLAY     = 0x01,
		RENTID      = 0x02,
		REPORTTIME  = 0x04,
		BITBLOCK    = 0x08,
		LIRCSTATUS  = 0x10,
		ABSBLOCK    = 0x20,
		ABSCIRCUIT  = 0x40,
		IP_EVENT    = 0x80
	};
	IBYTE      nStatCount;                       //״̬��
	STATION_NO nStno[LOGIN_MAX_STATION_COUNT];   //վ��
	IDWORD     nStyle[LOGIN_MAX_STATION_COUNT];  //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq)


// inform connect status of two entities ͨ������״̬
class AFX_EXT_CLASS MsgEntityConnStatus: public BaseMsg
{    
	DECLARE_CLBTH(MsgEntityConnStatus)

public:
	MsgEntityConnStatus();
	virtual ~MsgEntityConnStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID s_server;        //�����ʵ���
	ENTITY_ID s_client;        //�ͻ���ʵ���
	IWORD     board_no;        //������
	IBYTE     nABFlag;         //A/B����ʾ
	enum ConnectStatus {       //����״̬
		ENTITY_CONNECT,          //���ӽ���
		ENTITY_CLOSE,            //���ӹر�
		ENTITY_DATA_LOST         //����
	} status;
};
DECLARE_SERIALIZE_OPERATOR(MsgEntityConnStatus)

class AFX_EXT_CLASS MsgGpcAccessInfo: public BaseMsg
{    //  step 0: GPC send usercode and job_id...
	DECLARE_CLBTH(MsgGpcAccessInfo)

public:
	MsgGpcAccessInfo();
	virtual ~MsgGpcAccessInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	USERNAME  user_code;   //�û�����
	JOB_ID    job_id;      //����id
	GPC_ID    gpc_id;      //gpcʵ���
	CONID     con_id;      //����̨��
	enum ACCESS_STEP{
		WANT_PWD,            //��������
		PWD_OK               //����ȷ��
	} step;
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcAccessInfo)

class AFX_EXT_CLASS MsgGpcAccessReply: public BaseMsg
{    // cad reply with name/pwd and status
	DECLARE_CLBTH(MsgGpcAccessReply)

public:
	MsgGpcAccessReply();
	virtual ~MsgGpcAccessReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	USERNAME   user_code;          //�û�����
	JOB_ID     job_id;             //����id
	GPC_ID     gpc_id;             //gpcʵ���
	CONID      con_id;             //����̨��
	USERNAME   user_name;          //�û���
	PASSWORD   pwd;                //����
	char       os_identify[4];
	char       sys_version[16];

	enum ACCESS_STATUS {                   //��½״̬
		GPC_ACCESS_USERID_OK,                //�û�����ȷ
		GPC_ACCESS_SERVER_READY,             //�������ܾ�
		GPC_ACCESS_USERID_NOT_EXIST,         //�û��Ų�����
		GPC_ACCESS_INVALID_GPC_ID,           //��Ч��gpc��
		GPC_ACCESS_JOBID_NOT_EXIST,          //����id������
		GPC_ACCESS_CONID_NOT_EXIST,          //����̨�Ų�����
		GPC_ACCESS_GPCID_ALREADY_ONLINE,     //gpc���Ѿ���½
		GPC_ACCESS_USERID_CONID_DISMATCH,    //�û��ź͵���̨�Ų�ƥ��
		GPC_ACCESS_USERID_JOBID_DISMATCH,    //�û��ź͹���id��ƥ��
		GPC_LOGIN_CHECK,                     //��½ȷ��
		GPC_LOGIN_CHECK_OK,                  //��½ȷ�ϳɹ�
		GPC_LOGIN_CHECK_FAIL,                //��½ȷ��ʧ��
		GPC_ACCESS_UNKNOWN_MESSAGE  }        //δ֪��Ϣ
	status;
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcAccessReply)

class AFX_EXT_CLASS MsgGpcLogin : public BaseMsg
{
	DECLARE_CLBTH(MsgGpcLogin)

public:
	MsgGpcLogin();
	virtual ~MsgGpcLogin();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum  USER_RIGHT {//�û�Ȩ��
		READ_ONLY,//ֻ��
		READ_WRITE//��д
	};
	IWORD     sta_count;//�û���Ȩ�޵ĳ�վ��
	STATION_NO    sta_list[LOGIN_MAX_STATION_COUNT];//�û���Ȩ�޵ĳ�վ�б�

	IWORD     entity_count;//�û���Ȩ�޵�ʵ����
	ENTITY_ID   entity_list[LOGIN_MAX_ENTITY_COUNT];//�û���Ȩ�޵�ʵ���б�
	USER_RIGHT    user_right[LOGIN_MAX_ENTITY_COUNT];//�û���Ȩ��

	CONID     con_id; //����̨��
	IBYTE     job_id;//����id
	USERNAME    user_name;//�û���

};
DECLARE_SERIALIZE_OPERATOR(MsgGpcLogin)

// report time and report echo
class AFX_EXT_CLASS MsgReportTime: public BaseMsg
{
	DECLARE_CLBTH(MsgReportTime)

public:
	MsgReportTime();
	virtual ~MsgReportTime();

	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID         con_id;       //����̨��
	TRAIN_NO      train_id;     //���κ�
	TRAIN_INDEX   train_index;  //��������
	IBYTE         rec_index;    //��¼����
	TRAIN_KEY     train_key;    //���ιؼ���
	enum OPER{
		STPC_REPORT_TIME,         //��վ����
		STPC_UPDATE_TIME,         //��վ���±���
		AUTO_REPORT_TIME,         //�Զ�����
		GPC_CONFIRM_REPORT,       //gpcȷ�ϱ���
		REPORT_ECHO }  oper;
		enum RUN_ATTRIB {           //���з�ʽ����
			ARRIVE,               //����
			DEPART,               //����
			PASS }    run_attrib; //ͨ��
			enum TRAIN_ATTRIB {        //��������
				FAKE_TRAIN,           //�ٳ���
				TRUE_TRAIN            //�泵��
			}     train_attrib;

			STATION_NO station;       //վ��
			STATION_NO from_st;       //��һվ
			STATION_NO to_st;         //��һվ
			ENTRY_ID   entry;         //��� -1 not used
			ENTRY_ID   exit;          //����
			SIDE_ID    side;          //�ɵ�
			I_TIME     ltime;         // report time (second)
			I_TIME     delay_time;    // delay time (second)
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTime)

// change train_id reported from LIRC to cad
class AFX_EXT_CLASS MsgReportTrainId: public BaseMsg
{
	DECLARE_CLBTH(MsgReportTrainId)

public:
	MsgReportTrainId();
	virtual ~MsgReportTrainId();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_NO    old_train_id;//�ϳ��κ�
	TRAIN_NO    new_train_id;//�³��κ�
	TRAIN_KEY   train_key;   //���ιؼ���
	STATION_NO  station;     //��վ

	enum OPER{
		MSG_CHANGE_TRAIN_ID,//���������Ϣ
		MSG_REMOVE_TRAIN_ID,//ɾ��������Ϣ
		MSG_INSERT_TRAIN_ID} oper;//���복����Ϣ
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTrainId)

// send work schedule
class AFX_EXT_CLASS MsgStPlanSendOrder: public BaseMsg
{  // GPC orders CAD to send plan
	DECLARE_CLBTH(MsgStPlanSendOrder)

public:
	MsgStPlanSendOrder();
	virtual ~MsgStPlanSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID       con_id;//����̨��

	USERNAME    sender_name;//������
	I_TIME        begin_time;//�ƻ���ʼʱ��
	I_TIME        end_time;//�ƻ�����ʱ��
	I_TIME        send_time;//����ʱ��

	ENTITY_ID   st;
	short       prev_sta_count;
	short       next_sta_count;
	enum StPlanEchoMode {
		PLAN_NO_ECHO,
		PLAN_EACH_ECHO,
		PLAN_TOTAL_ECHO
	}     st_echo;

	STATION_NO  station;//վ��
	STATION_NO  station_dmis; //��dmisϵͳ�е�վ��
	short   count;//
	IBYTE   entry[MAX_STPLAN_DSTN_CNT]; //��� -1 both
	IBYTE   exit[MAX_STPLAN_DSTN_CNT];  //���� -1 both
	IBYTE   yard[MAX_STPLAN_DSTN_CNT];  // -1 both
	IBYTE   train_type[MAX_STPLAN_DSTN_CNT];  //�������� -1 both, 0 passenger 1 freight
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendOrder)

class AFX_EXT_CLASS MsgStPlanStatusRequest: public BaseMsg
{ // GPC requests all plans' recv status from CAD
	DECLARE_CLBTH(MsgStPlanStatusRequest)

public:
	MsgStPlanStatusRequest();
	virtual ~MsgStPlanStatusRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;//վ��
	CONID       con_id;//����̨��
	IBYTE        st_cnt;//�����Ŀ����
	ENTITY_ID   st[MAX_STPLAN_DSTN_CNT];          // destination
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanStatusRequest)

class AFX_EXT_CLASS MsgStPlanStatusAnswer: public BaseMsg
{   //  CAD answers MsgStPlanStatusRequest to GPC
	DECLARE_CLBTH(MsgStPlanStatusAnswer)

public:
	MsgStPlanStatusAnswer();
	virtual ~MsgStPlanStatusAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID       con_id;//����̨��
	STATION_NO  station;//վ��
	ENTITY_ID   dest;//Ŀ��ʵ���
	enum PLAN_RECV_STATUS {
		PLAN_RECV,//�ƻ�����
		PLAN_SIGN,//�ƻ�ǩ��
		PLAN_SENDING,//�ƻ����ڷ���
		PLAN_NO_CHANGE,//�ƻ�û�仯
		PLAN_NO_DATA,//�ƻ�������
		PLAN_ENTITY_DEAD,//ʵ������Ӧ
		PLAN_CONFIG_WRONG,//���ô���
		PLAN_INIT//�ƻ���ʼ��
	} recv_status;
	I_TIME        begin_time;//�ƻ���ʼʱ��
	I_TIME        end_time;//�ƻ�����ʱ��
	I_TIME    send_time;//�ƻ�����ʱ��
	I_TIME    recv_time;//�ƻ�����ʱ��
	USERNAME  sender_name;//����������
	USERNAME    recver_name;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanStatusAnswer)

class AFX_EXT_CLASS MsgStPlanRequest: public BaseMsg
{    // STPC/LIRC request stplan
	DECLARE_CLBTH(MsgStPlanRequest)

public:
	MsgStPlanRequest();
	virtual ~MsgStPlanRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;//վ��
	CONID       con_id;//����̨��
	I_TIME        begin_time;//����ƻ���ʼʱ��
	I_TIME        end_time;//����ƻ�����ʱ��
	enum StPlanRequestMode {
		PLAN_FROM_TIME,
		PLAN_FROM_ORDER
	} mode;
	enum StPlanRequestType{
		PLAN_SENDED,
		PLAN_RECVED,
		PLAN_ALL
	} data_type;
	enum StPlanRequestConId{
		PLAN_ALL_CON,//�������е���̨�ƻ�
		PLAN_ONE_CON//����1������̨�ƻ�
	} request_con;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanRequest)

// �ƻ��´￪ʼ��Ϣ  CAD sends plan init to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanSendInit: public BaseMsg
{    
	DECLARE_CLBTH(MsgStPlanSendInit)

public:
	MsgStPlanSendInit();
	virtual ~MsgStPlanSendInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;      //վ��
	CONID       con_id;       //����̨��
	IBYTE       flag;         // keep for future
	ENTITY_ID   gpc;          //�����߱���
	USERNAME    sender_name;  //����������
	I_TIME      send_time;    //����ʱ��
	enum StPlanEchoMode {
		PLAN_NO_ECHO,
		PLAN_EACH_ECHO,
		PLAN_TOTAL_ECHO
	}     send_echo;          //Ӧ�����Ҫ��
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendInit)

class AFX_EXT_CLASS MsgStPlanInsert: public BaseMsg
{    // CAD send plan data(insert or change) to STPC/LIRC
	DECLARE_CLBTH(MsgStPlanInsert)

public:
	MsgStPlanInsert();
	virtual ~MsgStPlanInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID          con_id;                   // ����̨����
	ENTITY_ID      gpc;                      // �����߱���
	TRAIN_INDEX    train_index;              // �г�����
	IBYTE          rec_index;                // ��¼����
	STATION_NO     station;                  // ��վվ��
	I_TIME         arrive;                   // ��վ����  0 Ϊδ�´�
	I_TIME         depart;                   // ����ʱ��  0 Ϊδ�´�
	I_TIME         plan_arrive;              // ͼ������
	I_TIME         plan_depart;              // ͼ������
	ENTRY_ID       entry;                    // ���
	ENTRY_ID       exit;                     // ����
	SIDE_ID        arrive_side;              // �ɵ�
	SIDE_ID        depart_side;              // �ɵ�
	IWORD          min_stop_time;            // ��Сͣվʱ��

	short          prev_sta_count;           //ǰ��վ��
	STATION_NO     prev_sta[8];              //ǰ��վվ��
	ENTRY_ID       prev_entry[8];            //ǰ��վ���
	ENTRY_ID       prev_exit[8];             //ǰ��վ����
	SIDE_ID        prev_side[8];             //ǰ��վ�ɵ�
	I_TIME         prev_arrive[8];           //ǰ��վ����ʱ��
	I_TIME         prev_depart[8];           //ǰ��վ�뿪ʱ��
	IBYTE          prev_status[8];           //ǰ��վ״̬
	IBYTE          prev_flag[8];             //

	short          next_sta_count;           //��վ��
	STATION_NO     next_sta[8];              //��վվ��
	ENTRY_ID       next_entry[8];            //��վ���
	ENTRY_ID       next_exit[8];             //��վ����
	SIDE_ID        next_side[8];             //��վ�ɵ�
	I_TIME         next_arrive[8];           //��վ����ʱ��
	I_TIME         next_depart[8];           //��վ�뿪ʱ��
	IBYTE          next_status[8];           //��վ״̬
	IBYTE          next_flag[8];             //

	TRAIN_NO       arrive_train_id;                     //���ﳵ��
	TRAIN_NO       depart_train_id;                     //��������
	IBYTE          engine_count;                        //��������
	TRAIN_NO       engine_id[MAX_ATTACH_ENGINE_COUNT];  //��������
	IBYTE          engine_type[MAX_ATTACH_ENGINE_COUNT];
	// ��������
	// bit 7-6:  from dispatcher/subao
	// bit 5-4:  0 - traction  1 - attach
	// bit 3-0:  0 - diesel    1 - elec
	IWORD          early_depart_time;      // �Ƿ������ǰ����
	// 0: can not early depart
	// else: the time span could be taken early
	//   -1: no limit
	IBYTE           status;                //   �Ƿ�ƻ�����ʵ�ʵ�
	// bit 0:  0 : arrival time is plan
	//         1 : arrival time is actual
	// bit 1:  0 : depart time is plan
	//         1 : depart time is actual
	// bit 2:  0  arrive time is actual by manul
	//          1  arrive time is actual by auto
	// bit 3:  0  arrive time is actual by manul
	//          1  arrive time is actual by auto
	// bit 4:  0  without tech stop
	//          1  tech stop ����ͣ��
	// bit 5:  0  �޵���ǣ��
	//          1  ����ǣ��
	// bit 6:  1: ѹǰ 0: ����
	// bit 7:  1: ѹ�� 0: ����

	IBYTE           stop_condition;        // ͣ��������
	// bit 0:    3.0�����ػ���ͼ�ɵ�
	// bit 1:    defined pass                 ԭ�涨ͨ��
	// bit 2:    cannot stop without plan     �޼ƻ�����ͣ��
	// bit 3 :   can not use other side       ���ñ���ɵ�
	// bit 4 :   cannot pass by non-main line ���ò���ͨ��
	// bit 5 :   cannot auto make arrive route
	// bit 6 :   cannot auto make depart route
	// bit 7 :   0 make depart route by meet,  1 make depart route as plan

	IBYTE           run_fast_slow_time;    // �����˻�ʱ��
	// bit 7:   0   run too fast
	//          1   run too slow
	// bit 6-0: time (minutes)
	IBYTE           flag;                  // ��վ���������롢ʼ�����յ�
	// bit  2-0:
	//     001 - give out this train ����
	//     010 - terminal this train�յ�
	//     011 - end this train, temporily terminal
	//     100 - keep this train
	//     101 - relay this train
	//     110 - unite this train
	//     111 - divide this train
	// bit  4-3:    01 - accept this train����
	//              10 - start this trainʼ��
	// bit 5-6:  �����ǳǼ��۷����� ԭ���۷� վ���۷� 
	// bit 7:    3.0�����ػ���ͼ�����
	IBYTE           type;                  //  �г�����
	//  detailed train type  (bit 7: 0 - passenger 1 -freight)
	IBYTE           run_type;              //  main type : EXPRESS_P_TRAIN, FAST_P_TRAIN,
	//         NORMAL_P_TRAIN, PACK_F_TRAIN,
	//         FIVE_F_TRAIN,  NORMAL_F_TRAIN,
	IBYTE           space_rank;            // �г����޵ȼ���
	// 0: no out of space, else: rank of space
	IBYTE           length_rank;           // �г������ȼ���
	// 0: no out of length, else: rank of length
	IBYTE           super_rank;            // �ص��г��ȼ���
	// 0: not super_train, else: rank of super_train
	IBYTE           military_rank;         // �����г��ȼ���
	// 0: not military_train, else: rank of military_train
	IBYTE           disconn_count;         // ˦��������
	IBYTE           conn_count;            // �ҳ�������

	IWORD           train_length;          //����
	IWORD           train_weight;          //����

	char            notes[64];             //���ּ���

	IBYTE           meet_count;            //�����г�����
	TRAIN_INDEX     meet_train_index[16];  //�����г�����
	IBYTE           meet_rec_index[16];    //�����г�˳��
	enum            MEET_STATUS {
		MEET_SAME_DIR,
		MEET_DIFF_DIR
	}              meet_status[16];
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanInsert)

// �ƻ��´�ɾ����Ϣ  CAD send plan data(delete) to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanDelete: public BaseMsg
{       
	DECLARE_CLBTH(MsgStPlanDelete)

public:
	MsgStPlanDelete();
	virtual ~MsgStPlanDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID     gpc;                   // ������ʵ���
	CONID         con_id;                // ����̨��
	STATION_NO    station;               // վ��                                        
	TRAIN_INDEX   train_index;           // ��������
	IBYTE         rec_index;             // ��¼����
	TRAIN_NO      arrive_train_id;       // ���ﳵ��
	TRAIN_NO      depart_train_id;       // ��������
	I_TIME        arrive;                // ��վ����  0 Ϊδ�´�
	I_TIME        depart;                // ����ʱ��  0 Ϊδ�´�
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanDelete)

// �ƻ��´������Ϣ  CAD send plan complete to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanSendComplete: public BaseMsg
{ 
	DECLARE_CLBTH(MsgStPlanSendComplete)

public:
	MsgStPlanSendComplete();
	virtual ~MsgStPlanSendComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID         con_id;            //����̨��
	ENTITY_ID     gpc;               //������ʵ���
	STATION_NO    station;           //վ��
	IBYTE         insert_cnt;        //���복����
	IBYTE         delete_cnt;        //ɾ��������
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendComplete)

// STPC/LIRC send CAD recv status of each plan
class AFX_EXT_CLASS MsgStPlanTrainRecv: public BaseMsg
{   
	DECLARE_CLBTH(MsgStPlanTrainRecv)

public:
	MsgStPlanTrainRecv();
	virtual ~MsgStPlanTrainRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO     station;//վ��
	CONID          con_id;//����̨��
	TRAIN_INDEX    train_index;//��������
	IBYTE           rec_index;//��¼����
	enum RUN_ATTRIB {//���з�ʽ����
		ARRIVE,//����
		DEPART,//�뿪
		PASS   //ͨ�� 
	}    attrib;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanTrainRecv)

// �ƻ������´�ǩ����Ϣ STPC/LIRC send recv all msg to CAD, then CAD relays to GPC
class AFX_EXT_CLASS MsgStPlanRecvALL: public BaseMsg
{     
	DECLARE_CLBTH(MsgStPlanRecvALL)

public:
	MsgStPlanRecvALL();
	virtual ~MsgStPlanRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID    gpc;          //�ƻ�������ʵ���
	ENTITY_ID    st;           //�ƻ�������ʵ���
	STATION_NO   station;      //վ��
	CONID        con_id;       //����̨��
	enum RECV_STATUS{          //����״̬
		RECV,                    //����
		SIGN,                    //ǩ��
		WRONG                    //����
	}           status;      
	I_TIME        send_time;   //�ƻ�����ʱ��
	I_TIME        recv_time;   //�ƻ�����ʱ��
	USERNAME      recver_name; //�ƻ�����������
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanRecvALL)

// �г�����ͼ�´�
class AFX_EXT_CLASS MsgBaseTrainPlan: public BaseMsg
{     
  DECLARE_CLBTH(MsgBaseTrainPlan)

public:
  MsgBaseTrainPlan();
  virtual ~MsgBaseTrainPlan();

private:
  virtual HRCODE doSerialize(Archive& ar);
protected:
  virtual size_t countObjectLength();

public:
    STATION_NO station;
    enum { MAX_PLAN_COUNT = 100 };
    struct base_plan
    {
    	TRAIN_NO			arrive_train;
    	TRAIN_NO			depart_train;
    	SIDE_ID				side;
    	ENTRY_ID			entry;
    	ENTRY_ID			exit;
    	STATION_NO			prev_station;
    	STATION_NO			next_station;
    	int					arrive_time; //seconds since 0:00:00 in a day
    	int					depart_time; //same as above
		char                reserve[8];
		base_plan()
		{
			memset(arrive_train, 0, sizeof(arrive_train));
			memset(depart_train, 0, sizeof(depart_train));
			side=0;entry=0;exit=0;prev_station=0;next_station=0;
			arrive_time=0;
			depart_time=0;
			memset(reserve, 0, sizeof(reserve));
		}
    } base_plan_record[MAX_PLAN_COUNT];
    IBYTE plan_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgBaseTrainPlan)

class AFX_EXT_CLASS MsgShuntNoteInit: public BaseMsg
{        //  begin sending one note
	DECLARE_CLBTH(MsgShuntNoteInit)

public:
	MsgShuntNoteInit();
	virtual ~MsgShuntNoteInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Դ����ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Դ
	ENTITY_ID    source_entity;//Դʵ��
	SHUNT_INDEX  shunt_index;//����������
	STATION_NO   station;//վ��
	I_TIME         send_time;//����ʱ��
	USERNAME     sender_name;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteInit)

class AFX_EXT_CLASS MsgShuntNoteComplete: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteComplete)

public:
	MsgShuntNoteComplete();
	virtual ~MsgShuntNoteComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ����ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Ϣ��Դ
	ENTITY_ID    source_entity;//Դʵ���
	SHUNT_INDEX  shunt_index;// ����������
	STATION_NO   station;//վ��
	I_TIME         send_time;//����ʱ��
	USERNAME     sender_name; //����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteComplete)

class AFX_EXT_CLASS MsgShuntNoteSingleGouInit: public BaseMsg
{        //  begin sending one note
	DECLARE_CLBTH(MsgShuntNoteSingleGouInit)

public:
	MsgShuntNoteSingleGouInit();
	virtual ~MsgShuntNoteSingleGouInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ:��ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Ϣ��Դ
	ENTITY_ID    source_entity;//Դʵ��
	SHUNT_INDEX  shunt_index;//����������
	IWORD   shunt_gou_index;//����������
	STATION_NO   station;//վ��
	I_TIME         send_time;//����ʱ��
	USERNAME     sender_name;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouInit)

class AFX_EXT_CLASS MsgShuntNoteSingleGouComplete: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteSingleGouComplete)

public:
	MsgShuntNoteSingleGouComplete();
	virtual ~MsgShuntNoteSingleGouComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ:��ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Ϣ��Դ
	ENTITY_ID    source_entity;//Դʵ��
	SHUNT_INDEX  shunt_index; //����������
	IWORD   shunt_gou_index;//����������
	STATION_NO   station;//վ��
	I_TIME         send_time;//����ʱ��
	USERNAME     sender_name; //����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouComplete)

class AFX_EXT_CLASS MsgShuntNoteGpcOperEcho: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteGpcOperEcho)

public:
	MsgShuntNoteGpcOperEcho();
	virtual ~MsgShuntNoteGpcOperEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX  shunt_index;//����������
	STATION_NO   station;//վ��
	enum OPER_TYPE {
		OPER_INSERT_ONE,

		OPER_INSERT_HEAD,
		OPER_UPDATE_HEAD,
		OPER_DELETE_HEAD,

		OPER_INSERT_GOU,//���빴
		OPER_UPDATE_GOU,//���¹�
		OPER_DELETE_GOU,//ɾ����

		OPER_INSERT_ROUTE,//�����·
		OPER_UPDATE_ROUTE,//���½�·
		OPER_DELETE_ROUTE,//ɾ����·

		OPER_UPDATE_STORECAR,
		OPER_DELETE_STORECAR,

		OPER_UPDATE_PICTURE,
		OPER_DELETE_PICTURE,

		OPER_INSERT_STATUS,
		OPER_UPDATE_STATUS,
	}            oper;
	enum OPER_ECHO {
		OPER_SUCCESS,//�����ɹ�
		OPER_FAILED//����ʧ��
	}            echo;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteGpcOperEcho)

// shuntnote data
class AFX_EXT_CLASS MsgShuntNoteSendOrder: public BaseMsg
{     // GPC orders CAD to send ShuntNote
	DECLARE_CLBTH(MsgShuntNoteSendOrder)

public:
	MsgShuntNoteSendOrder();
	virtual ~MsgShuntNoteSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//����������
	STATION_NO     station;//վ��
	I_TIME           send_time;//����ʱ��
	USERNAME       sender_name;   //����������
	IBYTE           entity_cnt;//ʵ����
	ENTITY_ID      st_entity[MAX_SHUNTNOTE_DSTN_CNT];       // destination
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSendOrder)

class AFX_EXT_CLASS MsgShuntNoteRecvALL: public BaseMsg
{    // STPC/LIRC send CAD recv status of shuntnote
	DECLARE_CLBTH(MsgShuntNoteRecvALL)

public:
	MsgShuntNoteRecvALL();
	virtual ~MsgShuntNoteRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	enum SHUNTNOTE_SEND_STATUS {//������¼����״̬
		SHUNT_RECEIVE,//����
		SHUNT_SIGNATURE,//ǩ��
		SHUNT_SENDING,//������
		SHUNT_ENTITY_DEAD,//ʵ������Ӧ
		SHUNT_NOT_SEND,//δ����
		SHUNT_RECV_OUTDATE,//���չ���
		SHUNT_INIT//��ʼ��
	} status;

	I_TIME            send_time;//����ʱ��
	I_TIME            recv_time;//����ʱ��
	USERNAME    sender_name;//����������
	USERNAME        recver_name;  //����������
	STATION_NO      station;//վ��
	ENTITY_ID       gpc;//gpcʵ���
	ENTITY_ID       st;//��վʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRecvALL)

class AFX_EXT_CLASS MsgShuntNoteSingleGouRecvALL: public BaseMsg
{    // STPC/LIRC send CAD recv status of shuntnote
	DECLARE_CLBTH(MsgShuntNoteSingleGouRecvALL)

public:
	MsgShuntNoteSingleGouRecvALL();
	virtual ~MsgShuntNoteSingleGouRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	IWORD     shunt_gou_index;//����������
	enum GOU_SEND_STATUS {//���ƻ�����״̬
		GOU_RECEIVE,//����
		GOU_SIGNATURE,//ǩ��
		GOU_SENDING,//������
		GOU_ENTITY_DEAD,//ʵ������Ӧ
		GOU_NOT_SEND,//δ����
		GOU_RECV_OUTDATE,//���չ���
		GOU_INIT//��ʼ��
	} status;

	I_TIME            send_time;//����ʱ��
	I_TIME            recv_time;//����ʱ��
	USERNAME    sender_name;//����������
	USERNAME        recver_name;//����������
	STATION_NO      station;//վ��
	ENTITY_ID       gpc;//gpcʵ���
	ENTITY_ID       st;//��վʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouRecvALL)

class AFX_EXT_CLASS MsgShuntNoteDelete: public BaseMsg
{        // GPC orders CAD to delete one note
	DECLARE_CLBTH(MsgShuntNoteDelete)

public:
	MsgShuntNoteDelete();
	virtual ~MsgShuntNoteDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;         //����������
	STATION_NO      station;             //վ��
	enum DELETE_ITEMS{                   //ɾ������
		SHUNTNOTE_ALL,                     //ɾ������
		SHUNTNOTEGOU,                      //ɾ����
		SHUNTNOTEROUTELIST,                //ɾ����·
		SHUNTNOTESTORECAR}  delete_items;  //ɾ���洢��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteDelete)

class AFX_EXT_CLASS MsgShuntNoteInsert: public BaseMsg
{        // GPC orders CAD to Insert one note
	DECLARE_CLBTH(MsgShuntNoteInsert)

public:
	MsgShuntNoteInsert();
	virtual ~MsgShuntNoteInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	STATION_NO      station;  //վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteInsert)

class AFX_EXT_CLASS MsgShuntNoteUpdate: public BaseMsg
{        // GPC orders CAD to Update one note
	DECLARE_CLBTH(MsgShuntNoteUpdate)

public:
	MsgShuntNoteUpdate();
	virtual ~MsgShuntNoteUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	STATION_NO      station;  //վ��
	enum UPDATE_ITEMS{//��������
		SHUNTNOTEHEAD,//������¼ͷ
		SHUNTNOTEGOU,//������¼��
		SHUNTNOTEROUTELIST,//������¼��·
		SHUNTNOTESTORECAR,//������¼�洢��
		SHUNTNOTESTATUS//������¼״̬
	} update_items;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteUpdate)

class AFX_EXT_CLASS MsgShuntNoteListRequest: public BaseMsg
{  //  GPC request shuntnote list from cad
	DECLARE_CLBTH(MsgShuntNoteListRequest)

public:
	MsgShuntNoteListRequest();
	virtual ~MsgShuntNoteListRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO   station;//վ��
	I_TIME         begin_time;//������¼����ʼʱ��
	I_TIME         end_time;//������¼�������ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListRequest)

class AFX_EXT_CLASS MsgShuntNoteListInit: public BaseMsg
{        //  CAD begin sending note list
	DECLARE_CLBTH(MsgShuntNoteListInit)

public:
	MsgShuntNoteListInit();
	virtual ~MsgShuntNoteListInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ����ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Ϣ��Դ
	STATION_NO   station;//վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListInit)

class AFX_EXT_CLASS MsgShuntNoteListComplete: public BaseMsg
{    //  CAD fishish sending note list
	DECLARE_CLBTH(MsgShuntNoteListComplete)

public:
	MsgShuntNoteListComplete();
	virtual ~MsgShuntNoteListComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ����ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;//��Ϣ��Դ
	STATION_NO   station;//վ��
	IWORD   count;//������¼��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListComplete)

class AFX_EXT_CLASS MsgShuntNoteRequest: public BaseMsg
{     //  GPC request one shuntnote from cad
	DECLARE_CLBTH(MsgShuntNoteRequest)

public:
	MsgShuntNoteRequest();
	virtual ~MsgShuntNoteRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:

	STATION_NO   station;//վ��
	SHUNT_INDEX    shunt_index;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRequest)

class AFX_EXT_CLASS MsgShuntNoteStatus: public BaseMsg
{    // send MsgShuntNoteStatus between cad and gpc
	DECLARE_CLBTH(MsgShuntNoteStatus)

public:
	MsgShuntNoteStatus();
	virtual ~MsgShuntNoteStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	STATION_NO    station;//վ��
	ENTITY_ID        st_entity;//��վʵ���
	enum SHUNTNOTE_SEND_STATUS{//������¼����״̬
		SHUNT_RECEIVE,//����
		SHUNT_SIGNATURE,//ǩ��
		SHUNT_SENDING,//������
		SHUNT_ENTITY_DEAD,//ʵ������Ӧ
		SHUNT_NOT_SEND,//δ����
		SHUNT_INIT//��ʼ��
	} status;
	I_TIME            send_time;//����ʱ��
	I_TIME            recv_time;//����ʱ��
	USERNAME    sender_name;//����������
	USERNAME        recver_name;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteStatus)

class AFX_EXT_CLASS MsgShuntNoteHead: public BaseMsg
{           //  ������ҵ֪ͨ����ͷ��Ϣ
	DECLARE_CLBTH(MsgShuntNoteHead)

public:
	MsgShuntNoteHead();
	virtual ~MsgShuntNoteHead();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//��Ϣ��Դ����ЧΪ0,������,���ɻ�
	SOURCE_TYPE   nSourceType;                //��Ϣ��Դ
	SHUNT_INDEX   shunt_index;             //  ������Ψһ����
	char          shunt_note_name[24];     //  ��___������
	STATION_NO    station;                 //  ���Ƴ�վ
	IBYTE          shunt_gou_count;         //  ����������
	IBYTE          flag;                    //  bit 0:  0 - ��ͼ��1- ��ͼ(���������ҵ)
	I_TIME          start_time;              //  ������ҵ֪ͨ����ҵ��ʼʱ��
	I_TIME          end_time;                //  ������ҵ֪ͨ����ҵ����ʱ��
	TRAIN_NO      train_id;                //  ����ҵ���κ�
	ENGINE_NO     engine_id;               //  ��ҵ�����Ż��߳��κ�
	TRAIN_INDEX   train_index;             //  Ԥ��
	I_TIME          edit_time;               //  ��дʱ��
	USERNAME      shunt_group_name;        //  ����������
	USERNAME      shunt_leader_name;       //  �����쵼������
	char          shunt_comment[64];       //  ��ע
	enum   SHUNTNOTE_STATUS {
		SHUNTNOTE_EDIT,
		SHUNTNOTE_SENDED,
		SHUNTNOTE_COMPLETE
	}     shunt_status;
	enum OPER_STATUS {
		OPER_NOT_START = 0,
		OPER_START,
		OPER_FINISH
	}     oper_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteHead)

class AFX_EXT_CLASS MsgShuntNoteGou: public BaseMsg
{      //  ������ҵ֪ͨ������Ϣ
	DECLARE_CLBTH(MsgShuntNoteGou)

public:
	MsgShuntNoteGou();
	virtual ~MsgShuntNoteGou();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };
	SOURCE_TYPE   nSourceType;
	SHUNT_INDEX  shunt_index;           //  ������Ψһ����
	STATION_NO station;                    //  ��վ
	IWORD  shunt_gou_index;            //  ������Ψһ����
	IWORD  shunt_gou_type;             //  ����������
	//    BIT7-0: 0 ��,1 ˦, 2�������ͷ,3 �����ժͷ
	//    BIT10,8: ǣ���߱��,7��ʾ���������,
	//              0��ʾ����ǣ����
	//    BIT11: �Ƿ�Ӱ������
	//    BIT12~13:��ִ��״̬: 0 δ��ʼִ��, 1 �ѿ�ʼִ��, 2 ִ�����
	SIDE_ID   side;                     //  ��ҵ�ɵ�
	IBYTE      direction;                //  ��ҵ����
	IWORD    shunt_car_count;          //  ��ҵ������
	IWORD    shunt_note_route_count;   //  ��·����
	IWORD    shunt_work_time;          //  ������·��ҵʱ��
	char      gou_comment[30];          //  ��ע
	SIDE_NAME szTrackQCX;   //  ǣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteGou)

class AFX_EXT_CLASS MsgShuntNoteRouteList: public BaseMsg
{    //  ������ҵ֪ͨ����·��Ϣ
	DECLARE_CLBTH(MsgShuntNoteRouteList)

public:
	MsgShuntNoteRouteList();
	virtual ~MsgShuntNoteRouteList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX   shunt_index;           //  ������Ψһ����
	STATION_NO  station;                    //  ��վ
	IWORD   shunt_gou_index;            //  ����������
	IWORD   shunt_route_index;          //  ��������·˳��������
	IWORD   route_id;                   //  ��·��ʶ
	IWORD   shunt_route_occ_time;       //  ������·ռ��ʱ��
	IWORD   flag;                       //  Ԥ��
	I_TIME     start_time;                 //  ������·����ʱ��
	IWORD   group_id;     // ��·����ID
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRouteList)

class AFX_EXT_CLASS MsgShuntNoteStoreCar: public BaseMsg
{  // ������ҵ֪ͨ��ͼƬ�ڳ�վ�泵�Ľṹ���ɹ��༭
	DECLARE_CLBTH(MsgShuntNoteStoreCar)

public:
	MsgShuntNoteStoreCar();
	virtual ~MsgShuntNoteStoreCar();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//����������
	STATION_NO     station;//վ��
	IWORD   count;//վ�泵��
	IWORD   len[MAX_SHUNTNOTE_STORECAR_CNT];//
	IBYTE     info[MAX_SHUNTNOTE_STORECAR_CNT][MAX_SHUNTNOTE_STORECAR_LEN];//վ�泵��Ϣ
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteStoreCar)

class AFX_EXT_CLASS MsgShuntNotePicture: public BaseMsg
{  // ������ҵ֪ͨ��ͼƬ�´�Ľṹ�����ɱ༭
	DECLARE_CLBTH(MsgShuntNotePicture)

public:
	MsgShuntNotePicture();
	virtual ~MsgShuntNotePicture();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//����������
	STATION_NO     station;//վ��
	IWORD   pic_len;
	IWORD   index;
	IWORD   count;
	IBYTE     pic[MAX_SHUNTNOTE_PICPACK_LEN];
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNotePicture)

// subao info
class AFX_EXT_CLASS MsgSubaoFormat: public BaseMsg
{
	DECLARE_CLBTH(MsgSubaoFormat)

public:
	MsgSubaoFormat();
	virtual ~MsgSubaoFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
	void init();

protected:
	virtual size_t countObjectLength();

public:
	enum   SUBAO_OPER {//�ٱ�����
		SUBAO_STPC_REPORT,//�����ն��ϴ��ٱ�
		SUBAO_GPC_INSERT,//gpc�����ٱ�
		SUBAO_GPC_CHANGE_MISC,//
		SUBAO_GPC_DELETE, //gpcɾ���ٱ�
		SUBAO_GPC_CHANGE_KEY,//gpc�ı�ؼ���
		SUBAO_SERVER_RELAY,//�����������ٱ�
		SUBAO_FROM_RADIO//�������ߵ��ٱ�
	} oper;
	I_TIME          input_time;//����ʱ��
	SHIFT_ID      shift_id;//�հ�ƻ�id
	SHIFT_ID      old_shift_id;//���հ�ƻ�id
	TRAIN_INDEX   train_index;//��������
	TRAIN_INDEX   old_train_index;//�ϳ�������
	TRAIN_NO      train_id;//���κ�
	TRAIN_NO      old_train_id;//�ϳ��κ�
	STATION_NO    report_sta;                     //�ϱ���վ
	CONID         con_id;//����̨��
	IBYTE          schd_type;//����������

	char          car_des[50];                    //�г����� $
	char          orig[20];                       //��Դ   @
	char          dest[20];                       //��վ   @

	IBYTE          engine_count;                   //bit 7-4 ���һ�������  3-0 ǣ����������
	struct         {
		char          engine_type;                //�����ͺ� $
		ENGINE_NO     engine_id;                  //������   @
	} subao_engine [8];

	USERNAME      driver_name;                    //˾�� @
	USERNAME      train_leader_name;              //���� @
	char          cover[10];                      //� $
	char          tail_no[10];                    //β�� $

	IBYTE          space_rank;                     //���޼��� $
	IBYTE          length_rank;                    //�������� $
	IBYTE          super_rank;                     //ר�˼��� $
	IBYTE          military_rank;                  //���˼��� $
	IBYTE          speed_limit;                    //���� $
	IBYTE          e_engine;                       //������־  @
	IBYTE          print_flag;                     //�Ƿ��ӡ  $
	short         pass_cnt;                       //�ͳ�����  @
	short         load_cnt;                       //�س�����  @
	short         pk_count;                       //�񳵣��գ�����   $
	short         pz_count;                       //�񳵣��أ�����   $
	short         ck_count;                       //�������գ�����   $
	short         cz_count;                       //�������أ�����   $
	short         pik_count;                      //ƽ�����գ�����   $
	short         piz_count;                      //ƽ�����أ�����   $
	short         gk_count;                       //�޳����գ�����   $
	short         gz_count;                       //�޳����أ�����   $
	short         o_count;                        //������������     $
	short         s_count;                        //�س�����         $
	short         total_count;                    //�ϼ�����         @
	short         van_count;                      //���           $
	short         subao_type;                     //��������         $
	short         speed;                          //�ٶ�             $
	short         noutil_cnt;                     //���ó���         @
	short         cal_len;                        //�ǳ�             @
	ISDWORD          total_weight;                   //ȫ��             @

};
DECLARE_SERIALIZE_OPERATOR(MsgSubaoFormat)

// query subao info
class AFX_EXT_CLASS MsgSubaoQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgSubaoQuery)

public:
	MsgSubaoQuery();
	virtual ~MsgSubaoQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
	void init();

protected:
	virtual size_t countObjectLength();

public:
	CONID con_id;//����̨��
	IBYTE  schd_type;//����������

	TRAIN_INDEX train_index;//��������
	SHIFT_ID    shift_id;//�հ�ƻ�id
	enum SUBAO_QUERY_FILTER {//�ٱ���ѯ������
		SUBAO_QUERY_BY_TRAININDEX,//������������ѯ�ٱ�
		SUBAO_QUERY_BY_SHIFTID//���հ�ƻ�id��ѯ�ٱ�
	} query_filter;
};
DECLARE_SERIALIZE_OPERATOR(MsgSubaoQuery)

// gpc��Ϣ���������
class AFX_EXT_CLASS MsgGpcMsgEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgGpcMsgEcho)
	int m_Version;

public:
	MsgGpcMsgEcho(int version = 0);
	virtual ~MsgGpcMsgEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ISDWORD msgid;//��Ϣid
	ISDWORD ecode;//
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcMsgEcho)

// �ְ����ݿ�����Ϣ
class AFX_EXT_CLASS MsgHugeDataCtrl: public BaseMsg
{
	DECLARE_CLBTH(MsgHugeDataCtrl)

public:
	MsgHugeDataCtrl();
	virtual ~MsgHugeDataCtrl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid;                           //��Ϣid
	enum STATUS {MSG_BEGIN, MSG_END, MSG_FAIL} status;//״̬����Ϣ��ʼ����Ϣ��������Ϣʧ��
	IDWORD msgpara;                                   //��Ϣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgHugeDataCtrl)

class AFX_EXT_CLASS MsgSimButton: public BaseMsg
{
	DECLARE_CLBTH(MsgSimButton)

public:
	MsgSimButton();
	virtual ~MsgSimButton();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD nStationNo;//վ��
	IBYTE nButtonCount;//��ť��
	IDWORD sButtonInput[10];//��ť
};
DECLARE_SERIALIZE_OPERATOR(MsgSimButton)


class AFX_EXT_CLASS MsgBasicTrainListInit: public BaseMsg
{
	DECLARE_CLBTH(MsgBasicTrainListInit)

public:
	MsgBasicTrainListInit();
	virtual ~MsgBasicTrainListInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID gpc;//gpcʵ���
	CONID   con_id;//����̨��
	STATION_NO  station;//վ��
	I_TIME    stime;//
	I_TIME    etime;//
	I_TIME    shift_begin_time;//�հ�ƻ��еĿ�ʼʱ��
	I_TIME    shift_end_time; //�հ�ƻ��еĽ���ʱ��
	enum  INIT_TRAINTYPE {//��������
		INIT_PASSENGER,//�ͳ�
		INIT_FREIGHT,//����
		INIT_ALL_TYPE//�ͻ���
	} train_type;
	enum  INIT_DIRECTION {//���з���
		INIT_UP,//����
		INIT_DOWN,//����
		INIT_ALL_DIR//������
	} direction;
	enum  INIT_START_ACCEPT {//
		INIT_START,//
		INIT_ACCEPT,//
		INIT_ALL_SA//
	} flag;
	enum  INIT_HAT_FLAG {//��ñ���
		INIT_NORMAL_TRAIN,//��ͨ����
		INIT_HAT_TRAIN//��ñ����
	} hat_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicTrainListInit)

//////////////
class AFX_EXT_CLASS MsgBasicTrainListReply: public BaseMsg
{
	DECLARE_CLBTH(MsgBasicTrainListReply)

public:
	MsgBasicTrainListReply();
	virtual ~MsgBasicTrainListReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_INDEX train_index;//��������
	TRAIN_NO  train_id;//���κ�
	I_TIME    arrive;//����ʱ��
	I_TIME    depart;//�뿪ʱ��
	enum BASIC_TRAIN_LIST_TYPE {//����ͼ��������
		BT_PASSENGER,//�ͳ�
		BT_FREIGHT,//����
		BT_ALL_TYPE//�ͻ���
	} type;
	enum BASIC_TRAIN_LIST_DIR {//����ͼ�������з���
		BT_UP,//����
		BT_DOWN,//����
		BT_ALL_DIR//������
	} dir;
	enum BASIC_TRAIN_LIST_HAT {//����ͼ���Ƿ��ñ
		BT_HAT_TRAIN,//��ñ����
		BT_NORMAL_TRAIN//��ͨ����
	} hat_train;
	enum BASIC_TRAIN_LIST_EXIST {//����ͼ�Ƿ���ڳ���
		BT_EXIST,//����
		BT_NONE//������
	} exist_train;
	enum BASIC_TRAIN_LIST_ADD {
		BT_NORMAL_SCHEDULE,
		BT_LONG_STOP,
		BT_LONG_INCREASE
	} add_train;
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicTrainListReply)

// ���ı�ʾ������Ϣ����Rt������
class AFX_EXT_CLASS MsgDssDisplayOrder: public BaseMsg
{
	DECLARE_CLBTH(MsgDssDisplayOrder)

public:
	MsgDssDisplayOrder();
	virtual ~MsgDssDisplayOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short sta_count;                             //��վ��
	STATION_NO sta_list[LOGIN_MAX_STATION_COUNT];//��վ
	enum DSS_ORDER {DSS_INIT, DSS_CLOSE} flag;   //��ʾ����ʾ��ʼ��,��ʾ�ر�
};
DECLARE_SERIALIZE_OPERATOR(MsgDssDisplayOrder)

//////////////
class AFX_EXT_CLASS MsgEntityConnStatusOrder: public BaseMsg
{
	DECLARE_CLBTH(MsgEntityConnStatusOrder)

public:
	MsgEntityConnStatusOrder();
	virtual ~MsgEntityConnStatusOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short      entity_count;                        //ʵ����
	short      msg_dispatch[LOGIN_MAX_ENTITY_COUNT];//
	ENTITY_ID  entity_list[LOGIN_MAX_ENTITY_COUNT]; //ʵ���
	enum CONN_ORDER {CONN_INIT, CONN_CLOSE} flag;   //���ӱ�ʾ�����ӳ�ʼ�������ӹر�
};            
DECLARE_SERIALIZE_OPERATOR(MsgEntityConnStatusOrder)

//////////////   
class AFX_EXT_CLASS MsgTimeSync: public BaseMsg
{
	DECLARE_CLBTH(MsgTimeSync)

public:
	MsgTimeSync();
	virtual ~MsgTimeSync();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME ltime;//ͬ��ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTimeSync)
////////////////////////////////////////////////////
// message for trian form: get from tmis
////////////////////////////////////////////////////
// request
class AFX_EXT_CLASS MsgLoadTrainFormRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadTrainFormRequest)

public:
	MsgLoadTrainFormRequest();
	virtual ~MsgLoadTrainFormRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;//gpc��
	JOB_ID    job_id;//������
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;//ʵ���
	USERNAME  user_code;//�û���
	TRAIN_INDEX train_index;      //��������
	TRAIN_NO  train_id;
	enum TRAIN_FORM_LOAD_REQUEST{
		LOAD_ASM_TRAIN_FORM_HEAD = 1,
		LOAD_ASM_TRAIN_FORM = 2,
		LOAD_ASM = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadTrainFormRequest)

////////////////////////////////////////////////////
// train form head information
class AFX_EXT_CLASS MsgLoadAsmTrainFormHead: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadAsmTrainFormHead)

public:
	MsgLoadAsmTrainFormHead();
	virtual ~MsgLoadAsmTrainFormHead();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;       //����̨��
	char    con_name[19];//����̨����
	STATION_NO  station_id;     //վ��
	TRAIN_INDEX train_index;      //��������
	TRAIN_NO  train_id;//���κ�
	TRAIN_INDEX TMIS_train_id;      // tmis train_id
	char    marshing_train_id[20];    // ���鳵��
	char    runing_train_id[20];    // ���г���
	char    marshing_station_code[4]; // ����վ����
	STATION_NO  marshing_station_id;    // ����վ��
	STATION_NO  shunting_station_id;    // ����վ��
	char    shunting_station_code[4]; // ����վ����
	I_TIME    plan_time;      // �ƻ�����ʱ�䣨14��
	I_TIME    actl_time;      // ʵ�ʿ���ʱ�䣨14��
	I_TIME    send_time;      // �´�ʱ�䣨14��
	short   toal_cnt;     // ������3��
	short   cal_len;      // ������5��
	short   weight;             // ���أ�5��
	short   self_weight;            // ���أ�5��
	short   van_cnt;      // ����ϼ�
	char    reporter[10];     // �����ˣ�10��
	char    assigner[10];     // ǩ���ˣ�10��
	char    sg_station_name[10];    // ˦��վ��
	STATION_NO  sg_station_id;      // ˦��վ��
	char    sg_station_code[6];   // ˦��վ����
	short   weight_1;     // �г�����1
	short   weight_2;     // �г�����2
	short   marshing_cnt;     // ��������
	IBYTE   ht_flag;      // ��β��־��1��
	IBYTE   no_use_flag;      // �з����ñ�־(1)
	IBYTE   car_no_count_flag;    // ������������־(1)
	IBYTE   car_count_flag;     // ����������־(1)
	IBYTE   self_car_flag;      // �Ա������(1)
	IBYTE   received_tmis_flag;   // ��TMIS��ȡflag(1)
	I_TIME    received_tmis_time;   // ��TMIS��ȡʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadAsmTrainFormHead)

////////////////////////////////////////////////////
// train form carriage information
class AFX_EXT_CLASS MsgLoadAsmTrainForm: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadAsmTrainForm)

public:
	MsgLoadAsmTrainForm();
	virtual ~MsgLoadAsmTrainForm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;       //����̨��
	TRAIN_INDEX train_index;      //��������
	TRAIN_INDEX TMIS_train_id;//tmis�еĳ�������
	short   car_pos;      // ˳���
	short   car_type_id;      // ����id��
	CARRIAGE_NO car_no;       // ����(8)
	IBYTE   tanker_oil_type;    // �޳�����
	IBYTE   load_flag;      // ���ر�־
	IBYTE   car_empty_weight_flag;  //���ر�־
	short   car_suttle;     // ����(4,1)
	short   car_len_cnt;      // ����(2,1)
	short   car_load;     // ����
	char    car_type[4];//����
	char    fisrt_station_code[4];    // ��һ��վ����
	char    last_station_code[4];   // ���վ����
	STATION_NO  first_station_id;//��һվվ��
	STATION_NO  last_station_id;//���վվ��
	char    freight_name[40];   // ��������
	char    freight_code[12];   // ����Ʒ������
	char    receiver[40];     // �ջ���
	char    depart_station_code[4];   // ��վ����
	STATION_NO  depart_station_id;    // ��վid
	short   car_van_cnt;      // ���
	char    car_note[40];     // ����
	char    car_type_code[4];   // ���ִ���
	char    container_type_1_cnt[4];  // ��װ������1
	char    weight_container_1_cnt[4];  // �ؼ�װ����1
	char    empty_container_1_cnt[4]; // �ؼ�װ����1
	char    container_type_2_cnt[4];  // ��װ������1
	char    weight_container_2_cnt[4];  // �ؼ�װ����1
	char    empty_container_2_cnt[4]; // �ؼ�װ����1
	char    first_station_name[12];   // ��һ��վ����
	char    last_station_name[12];    // ���վ����
	char    depart_station_name[12];  // ��վ����
	char    aim_bureau[4];      // ����
	char    aim_subbureau[4];   // ���־�
	char    direction_no[4];    // �����
	char    frieght_no_use_code[4];   // Ʒ�������ô���
	char    railway_company_flag[4];  // ·���־
	char    no_count_flag[4];   // ��������־
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadAsmTrainForm)

class AFX_EXT_CLASS MsgLoadTrainFormResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadTrainFormResponse)

public:
	MsgLoadTrainFormResponse();
	virtual ~MsgLoadTrainFormResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;          //ʵ���
	TRAIN_INDEX train_index;      //��������
	enum TRAIN_FORM_LOAD_RESULT{
		LOAD_ASM_INIT = 0,
		LOAD_ASM_TRAIN_FORM_HEAD_SUCCESS = 1,
		LOAD_ASM_HEAD_SUCC_CAR_FAILED = 2,
		LOAD_ASM_HEAD_FAILED_CAR_SUCC = 3,
		LOAD_ASM_TRAIN_FORM_SUCCESS = 4,
		LOAD_ASM_SUCCESSED = 5,
		LOAD_ASM_TRAIN_FORM_HEAD_FAILED = -1,
		LOAD_ASM_HEAD_FAILD_CAR_SUCCESS = -2,
		LOAD_ASM_HEAD_SUCC_CAR_FALID = -3,
		LOAD_ASM_TRAIN_FORM_FAILED = -4,
		LOAD_ASM_FAILD = -5,
	}load_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadTrainFormResponse)

////////////////////////////////////////////////////
// message for station store carriage
////////////////////////////////////////////////////
// request
class AFX_EXT_CLASS MsgLoadSscRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscRequest)

public:
	MsgLoadSscRequest();
	virtual ~MsgLoadSscRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	JOB_ID    job_id;     //������
	STATION_NO  station_no;       //վ��
	ENTITY_ID entily_id;      //ʵ���
	USERNAME  user_code;      //�û���
	enum SSC_LOAD_REQUEST{
		LOAD_SSC_BLOCK = 1,
		LOAD_SSC_CARRIAGE = 2,
		LOAD_SSC_ALL = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscRequest)
// response
class AFX_EXT_CLASS MsgLoadSscResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscResponse)

public:
	MsgLoadSscResponse();
	virtual ~MsgLoadSscResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_no;       //վ��
	ENTITY_ID entily_id;          //ʵ���
	short   block_count;          //
	short   carriage_count;     //
	short   side_count; //�ɵ���
	enum SSC_LOAD_RESPONSE{
		LOAD_SSC_NO_DATA = 0,
		LOAD_SSC_BLOCK_SUCCESS = 1,
		LOAD_SSC_CARRIAGE_SUCCESS = 2,
		LOAD_SSC_ALL_SUCCESS = 3,
		LOAD_SSC_BLOCK_FAILED = -1,
		LOAD_SSC_BLOCK_FAILED_CAR_SUCC = -2,
		LOAD_SSC_BLOCK_SUCC_CAR_FAILED = -3,
		LOAD_SSC_CARRIAGE_FAILED = -4,
		LOAD_SSC_ALL_FAILED = -5,
	}load_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscResponse)

class AFX_EXT_CLASS MsgUpdateSscRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscRequest)

public:
	MsgUpdateSscRequest();
	virtual ~MsgUpdateSscRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;          //ʵ���
	short   block_cnt;
	short   car_cnt;  //����
	enum SSC_UPDATE_FLAG{
		UPDATE_BLOCK = 1,
		UPDATE_CARRIAGE = 2,
		UPDATE_ALL = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscRequest)

class AFX_EXT_CLASS MsgUpdateSscRequestEnd: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscRequestEnd)

public:
	MsgUpdateSscRequestEnd();
	virtual ~MsgUpdateSscRequestEnd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;  //ʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscRequestEnd)

class AFX_EXT_CLASS MsgUpdateSscResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscResponse)

public:
	MsgUpdateSscResponse();
	virtual ~MsgUpdateSscResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;    //ʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscResponse)

class AFX_EXT_CLASS MsgUpdateSscResponseEnd: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscResponseEnd)

public:
	MsgUpdateSscResponseEnd();
	virtual ~MsgUpdateSscResponseEnd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;     //gpc��
	STATION_NO  station_id;       //վ��
	ENTITY_ID entily_id;      //ʵ���
	short   succ_block_cnt;
	short   succ_carriage_cnt;
	short   failed_block_cnt;
	short   failed_carriage_cnt;
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscResponseEnd)

//////////////////////////////////////////////////////////////
// block
//////////////////////////////////////////////////////////////
// load
class AFX_EXT_CLASS MsgLoadSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscBlock)

public:
	MsgLoadSscBlock();
	virtual ~MsgLoadSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id; //gpc��
	STATION_NO  station_no;       //վ��
	ENTITY_ID entily_id;  //ʵ���
	short   side_no;  //�ɵ���
	short   index_in_side;//�ɵ�����
	short   car_cnt;  //����
	IBYTE   ht_flag;  //��β��־��1��
	IBYTE   oil_type;//�ͳ�����
	IBYTE   car_empty_flag; //�Ƿ�ճ���ʾ
	IBYTE   flag; //��־
	IBYTE   car_type; //����
	char    block_desc[9];
	char    freight_flag[8];  //���˱�ʾ
	CARRIAGE_NO first_car_no;//��һ������
	CARRIAGE_NO last_car_no;//���һ������
	I_TIME    last_edit_time;//���༭ʱ��
	char    direction_no[4];//�����
	char    end_station_code[4];//����վ����
	STATION_NO  end_station_no;//����վվ��
	short   positon;  //λ��
	char    status[4];  //״̬
	char    work_status[4]; //����״̬
	char    work_flag[4]; //������־
	char    note[8];  //ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscBlock)
// load
class AFX_EXT_CLASS MsgInsertSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgInsertSscBlock)

public:
	MsgInsertSscBlock();
	virtual ~MsgInsertSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id;       //gpc��
	STATION_NO  station_no;       //վ��
	ENTITY_ID entily_id;  //ʵ���
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
	short   car_cnt;  //����
	IBYTE   ht_flag;  //��β��־��1��
	IBYTE   oil_type; //�ͳ�����
	IBYTE   car_empty_flag; //�Ƿ�ճ���ʾ
	IBYTE   flag; //��־
	IBYTE   car_type; //����
	char    block_desc[9];
	char    freight_flag[8];  //���˱�ʾ
	CARRIAGE_NO first_car_no; //��һ������
	CARRIAGE_NO last_car_no;  //���һ������
	I_TIME    last_edit_time; //���༭ʱ��
	char    direction_no[4];//�����
	char    end_station_code[4];  //����վ����
	STATION_NO  end_station_no; //����վվ��
	short   positon;  //λ��
	char    status[4];  //״̬
	char    work_status[4]; //����״̬
	char    work_flag[4]; //������־
	char    note[8];  //ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgInsertSscBlock)
// update block detail information
class AFX_EXT_CLASS MsgUpdateSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlock)

public:
	MsgUpdateSscBlock();
	virtual ~MsgUpdateSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	GPC_ID    gpc_id; //gpc��
	STATION_NO  station_no;       //վ��
	ENTITY_ID entily_id;  //ʵ���
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
	short   car_cnt;  //����
	IBYTE   ht_flag;  //��β��־��1��
	IBYTE   oil_type; //�ͳ�����
	IBYTE   car_empty_flag; //�Ƿ�ճ���ʾ
	IBYTE   flag;
	IBYTE   car_type; //����
	char    block_desc[9];
	char    freight_flag[8];  //���˱�ʾ
	CARRIAGE_NO first_car_no; //��һ������
	CARRIAGE_NO last_car_no;  //���һ������
	I_TIME    last_edit_time; //���༭ʱ��
	char    direction_no[4];//�����
	char    end_station_code[4];  //����վ����
	STATION_NO  end_station_no; //����վվ��
	short   positon;  //λ��
	char    status[4];  //״̬
	char    work_status[4]; //����״̬
	char    work_flag[4]; //������־
	char    note[8];  //ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlock)
// update block index or porsition
class AFX_EXT_CLASS MsgUpdateSscBlockPos: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockPos)

public:
	MsgUpdateSscBlockPos();
	virtual ~MsgUpdateSscBlockPos();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
	short   positon;  //λ��
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockPos)
// update block status��work_flag or work_status
class AFX_EXT_CLASS MsgUpdateSscBlockStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockStatus)

public:
	MsgUpdateSscBlockStatus();
	virtual ~MsgUpdateSscBlockStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
	IBYTE   ht_flag;  //��β��־��1��
	IBYTE   oil_type; //�ͳ�����
	IBYTE   car_empty_flag; //�Ƿ�ճ���ʾ
	IBYTE   flag; //��־
	char    status[4];  //״̬
	char    work_status[4]; //����״̬
	char    work_flag[4]; //������־
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockStatus)
// update block note or freight_flag
class AFX_EXT_CLASS MsgUpdateSscBlockNote: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockNote)

public:
	MsgUpdateSscBlockNote();
	virtual ~MsgUpdateSscBlockNote();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;//�ɵ���
	short   index_in_side;//�ɵ�����
	char    freight_flag[8];//���˱�ʾ
	char    note[8];//ע��
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockNote)
// DELETE BLOCK
class AFX_EXT_CLASS MsgDeleteSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgDeleteSscBlock)

public:
	MsgDeleteSscBlock();
	virtual ~MsgDeleteSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
};
DECLARE_SERIALIZE_OPERATOR(MsgDeleteSscBlock)
// OPER RESULT
class AFX_EXT_CLASS MsgOperBlockReaponse: public BaseMsg
{
	DECLARE_CLBTH(MsgOperBlockReaponse)

public:
	MsgOperBlockReaponse();
	virtual ~MsgOperBlockReaponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   index_in_side;  //�ɵ�����
	enum DEL_UPDATE_SSC_BLOCK_RESULT{//ɾ�����·������
		NO_FOUND = 0,//û�з���
		UPDATE_DETAIL_SUCCESS = 1,//����ϸ�ڳɹ�
		UPDATE_POS_SUCCESS = 2,//����λ�óɹ�
		UPDATE_STATUS_SUCCESS = 3,//����״̬�ɹ�
		UPDATE_NOTE_SUCCESS = 4,//����ע�ͳɹ�
		DELETE_SUCCESS = 5,//ɾ���ɹ�
		INSERT_SUCCESS = 6,//����ɹ�
		UPDATE_DETAIL_FAILED = -1,//����ϸ��ʧ��
		UPDATE_POS_FAILED = -2,//����λ��ʧ��
		UPDATE_STATUS_FAILED = -3,//����״̬ʧ��
		UPDATE_NOTE_FAILED = -4,//����ע��ʧ��
		DELETE_BLOCK_FAILED = -5,//ɾ������ʧ��
		INSERT_FAILED = -6,//����ʧ��
	}del_update_block_rs;
};
DECLARE_SERIALIZE_OPERATOR(MsgOperBlockReaponse)

//////////////////////////////////////////////////////////////
// carriage
//////////////////////////////////////////////////////////////
// load
class AFX_EXT_CLASS MsgLoadSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscCarriage)

public:
	MsgLoadSscCarriage();
	virtual ~MsgLoadSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   block_index_in_side;
	short   car_index_in_block;
	CARRIAGE_NO car_no;//����
	short   car_type_id;//����id��
	char    car_type[4];//����
	IBYTE   oil_type; //�ͳ�����
	IBYTE   load_flag;//���ر�־
	short   car_suttle;//����
	short   car_len_cnt;//����
	short   car_load;//����
	STATION_NO  first_station_no;//��һ��վվ��
	char    first_station_code[4];//��һ��վ����
	STATION_NO  last_station_no;//���վվ��
	char    last_station_code[4];//���վ����
	char    freight_name[38];//��������
	char    freight_code[8];//����Ʒ������
	char    receiver[40];//�ջ���
	char    depart_station_code[4];//��վ����
	STATION_NO  depart_station_no;//��վվ��
	short   car_van_cnt;//����
	char    car_note[40];//����
	char    car_type_code[4];//���ִ���
	char    container_type_1_cnt[4];//��װ������1
	char    weight_container_1_cnt[4];//�ؼ�װ����1
	char    empty_contrainer_1_cnt[4];//�ռ�װ����1
	char    container_type_2_cnt[4];//��װ������2
	char    weight_container_2_cnt[4];//�ؼ�װ����2
	char    empty_contrainer_2_cnt[4];//�ռ�װ����2
	char    car_empty_weight_flag[4];//���ر�־
	char    first_station_name[12];//��һ��վ����
	char    last_station_name[12];//���վ����
	char    depart_station_name[12];//��վ����
	char    aim_bureau[4];//����
	char    aim_subbereau[4];//���־�
	char    direction_no[4];//�����
	char    freight_no_use_code[4];//��������Ʒ������
	char    rail_company_flag[4];//��·��˾��ʾ
	char    no_count_flag[4];//��������־
	char    current_status[4];//��ǰ״̬
	I_TIME    arrive_time;//����ʱ��
	TRAIN_INDEX arrive_train_index;//���ﳵ������
	TRAIN_NO  arrive_train_no;//���ﳵ�κ�
	IBYTE   arrive_flag;//�����ʾ
	IBYTE   arrive_reason;//����ԭ��
	I_TIME    work_complete_time;//�������ʱ��
	I_TIME    depart_time;//����ʱ��
	TRAIN_INDEX depart_train_index;//������������
	IBYTE   depart_flag;//������ʾ
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscCarriage)
// insert detail carriage information
class AFX_EXT_CLASS MsgInsertSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgInsertSscCarriage)

public:
	MsgInsertSscCarriage();
	virtual ~MsgInsertSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   block_index_in_side;  //
	short   car_index_in_block;   //
	CARRIAGE_NO car_no; //����
	short   car_type_id;  //����id��
	char    car_type[4];  //����
	IBYTE   oil_type; //�ͳ�����
	IBYTE   load_flag;  //���ر�־
	short   car_suttle; //����
	short   car_len_cnt;  //����
	short   car_load; //����
	STATION_NO  first_station_no; //��һ��վվ��
	char    first_station_code[4];  //��һ��վ����
	STATION_NO  last_station_no;  //���վվ��
	char    last_station_code[4]; //���վ����
	char    freight_name[38]; //��������
	char    freight_code[8];  // ����Ʒ������
	char    receiver[40]; //�ջ���
	char    depart_station_code[4]; //��վ����
	STATION_NO  depart_station_no;  //��վվ��
	short   car_van_cnt;  //����
	char    car_note[40]; //����
	char    car_type_code[4]; //���ִ���
	char    container_type_1_cnt[4];  //��װ������1
	char    weight_container_1_cnt[4];  //�ؼ�װ����1
	char    empty_contrainer_1_cnt[4];  //�ռ�װ����1
	char    container_type_2_cnt[4];  //��װ������2
	char    weight_container_2_cnt[4];  //�ؼ�װ����2
	char    empty_contrainer_2_cnt[4];  //�ռ�װ����2
	char    car_empty_weight_flag[4]; //���ر�־
	char    first_station_name[12]; //��һ��վ����
	char    last_station_name[12];  //���վ����
	char    depart_station_name[12];  //��վ����
	char    aim_bureau[4];  //����
	char    aim_subbereau[4]; //���־�
	char    direction_no[4];//�����
	char    freight_no_use_code[4]; //��������Ʒ������
	char    rail_company_flag[4]; //��·��˾��ʾ
	char    no_count_flag[4]; //��������־
	char    current_status[4];  //��ǰ״̬
	I_TIME    arrive_time;  //����ʱ��
	TRAIN_INDEX arrive_train_index; //���ﳵ������
	TRAIN_NO  arrive_train_no;  //���ﳵ�κ�
	IBYTE   arrive_flag;  //�����ʾ
	IBYTE   arrive_reason;  //����ԭ��
	I_TIME    work_complete_time; //�������ʱ��
	I_TIME    depart_time;  //����ʱ��
	TRAIN_INDEX depart_train_index; //������������
	IBYTE   depart_flag;  //������ʾ
};
DECLARE_SERIALIZE_OPERATOR(MsgInsertSscCarriage)
// update detail information
class AFX_EXT_CLASS MsgUpdateSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscCarriage)

public:
	MsgUpdateSscCarriage();
	virtual ~MsgUpdateSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   block_index_in_side;
	short   car_index_in_block;
	CARRIAGE_NO car_no;   //����(8)
	short   car_type_id;  //����id��
	char    car_type[4];  //����
	IBYTE   oil_type; //�ͳ�����
	IBYTE   load_flag;//���ر�־
	short   car_suttle;//����
	short   car_len_cnt;//����
	short   car_load;//����
	STATION_NO  first_station_no;//��һ��վվ��
	char    first_station_code[4];  //��һ��վ����
	STATION_NO  last_station_no;  //���վվ��
	char    last_station_code[4]; //���վ����
	char    freight_name[38]; //��������
	char    freight_code[8];  // ����Ʒ������
	char    receiver[40]; //�ջ���
	char    depart_station_code[4]; //��վ����
	STATION_NO  depart_station_no;  //��վվ��
	short   car_van_cnt;  //����
	char    car_note[40]; //����
	char    car_type_code[4]; //���ִ���
	char    container_type_1_cnt[4];  //��װ������1
	char    weight_container_1_cnt[4];  //�ؼ�װ����1
	char    empty_contrainer_1_cnt[4];  //�ռ�װ����1
	char    container_type_2_cnt[4];  //��װ������2
	char    weight_container_2_cnt[4];  //�ؼ�װ����2
	char    empty_contrainer_2_cnt[4];  //�ռ�װ����2
	char    car_empty_weight_flag[4]; //���ر�־
	char    first_station_name[12]; //��һ��վ����
	char    last_station_name[12];  //���վ����
	char    depart_station_name[12];  //��վ����
	char    aim_bureau[4];  //����
	char    aim_subbereau[4]; //���־�
	char    direction_no[4];//�����
	char    freight_no_use_code[4]; //��������Ʒ������
	char    rail_company_flag[4]; //��·��˾��ʾ
	char    no_count_flag[4]; //��������־
	char    current_status[4];  //��ǰ״̬
	I_TIME    arrive_time;  //����ʱ��
	TRAIN_INDEX arrive_train_index; //���ﳵ������
	TRAIN_NO  arrive_train_no;  //���ﳵ�κ�
	IBYTE   arrive_flag;  //�����ʾ
	IBYTE   arrive_reason;  //����ԭ��
	I_TIME    work_complete_time; //�������ʱ��
	I_TIME    depart_time;  //����ʱ��
	TRAIN_INDEX depart_train_index; //������������
	IBYTE   depart_flag;  //������ʾ
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscCarriage)
// update detail information
class AFX_EXT_CLASS MsgDeleteSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgDeleteSscCarriage)

public:
	MsgDeleteSscCarriage();
	virtual ~MsgDeleteSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;  //�ɵ���
	short   block_index_in_side;  //
	short   car_index_in_block; //
	CARRIAGE_NO car_no; //����
	//  enum SSC_DELETE_FLAG
};
DECLARE_SERIALIZE_OPERATOR(MsgDeleteSscCarriage)
// OPER RESULT
class AFX_EXT_CLASS MsgOperSscCarriageReaponse: public BaseMsg
{
	DECLARE_CLBTH(MsgOperSscCarriageReaponse)

public:
	MsgOperSscCarriageReaponse();
	virtual ~MsgOperSscCarriageReaponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //����̨��
	STATION_NO  station_no;       //վ��
	short   side_no;//�ɵ���
	short   block_index_in_side;//
	short   car_index_in_block;//
	CARRIAGE_NO car_no;//����
	enum DEL_UPDATE_SSC_CARRIAGE_RESULT{//ɾ�����»������
		NO_FOUND = 0,//û����
		UPDATE_DETAIL_SUCCESS = 1,//����ϸ�ڳɹ�
		UPDATE_POS_SUCCESS = 2,//����λ�óɹ�
		UPDATE_STATUS_SUCCESS = 3,//����״̬�ɹ�
		UPDATE_NOTE_SUCCESS = 4,//����ע�ͳɹ�
		DELETE_SUCCESS = 5,//ɾ���ɹ�
		INSERT_SUCCESS = 6,//����ɹ�
		UPDATE_DETAIL_FAILED = -1,//����ϸ��ʧ��
		UPDATE_POS_FAILED = -2,//����λ��ʧ��
		UPDATE_STATUS_FAILED = -3,//����״̬ʧ��
		UPDATE_NOTE_FAILED = -4,//����ע��ʧ��
		DELETE_BLOCK_FAILED = -5,//
		INSERT_FAILED = -6,//����ʧ��
	}del_update_carriage_rs;
};
DECLARE_SERIALIZE_OPERATOR(MsgOperSscCarriageReaponse)

// send next console schedule
class AFX_EXT_CLASS MsgNextPlanSendOrder: public BaseMsg
{  // GPC orders CAD to send plan
	DECLARE_CLBTH(MsgNextPlanSendOrder)

public:
	MsgNextPlanSendOrder();
	virtual ~MsgNextPlanSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	USERNAME    sender_name;//�����û���
	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
	I_TIME        send_time;//����ʱ��

	short       next_count;//
	struct NextPlanSendOrder{
		STATION_NO  station;//վ��

		short   count;//
		IBYTE   entry[MAX_STPLAN_DSTN_CNT]; // ��� -1 both
		IBYTE   exit[MAX_STPLAN_DSTN_CNT];  // ���� -1 both
		IBYTE   yard[MAX_STPLAN_DSTN_CNT];  // -1 both
		IBYTE   train_type[MAX_STPLAN_DSTN_CNT];  //�������� -1 both, 0 passenger 1 freight
	}     next_order[MAX_STPLAN_DSTN_CNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendOrder)

class AFX_EXT_CLASS MsgNextPlanRequest: public BaseMsg
{    // request next console plan
	DECLARE_CLBTH(MsgNextPlanRequest)

public:
	MsgNextPlanRequest();
	virtual ~MsgNextPlanRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   requester_entity;//����ʵ���
	ENTITY_ID   master_entity;//��ʵ���

	CONID       requester_con_id;//�������̨��
	CONID       master_con_id;//������̨��

	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��

	USERNAME    requester_name;//�����û���
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanRequest)

class AFX_EXT_CLASS MsgNextPlanSendInit: public BaseMsg
{    // CAD sends plan init to next console
	DECLARE_CLBTH(MsgNextPlanSendInit)

public:
	MsgNextPlanSendInit();
	virtual ~MsgNextPlanSendInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
	I_TIME        send_time;//����ʱ��
	USERNAME    sender_name;//�����û���
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendInit)

class AFX_EXT_CLASS MsgNextPlanSendComplete: public BaseMsg
{ // CAD send plan complete to next console
	DECLARE_CLBTH(MsgNextPlanSendComplete)

public:
	MsgNextPlanSendComplete();
	virtual ~MsgNextPlanSendComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
	I_TIME        send_time;  //����ʱ��

	short       insert_cnt;//������
	USERNAME    sender_name;//�����û���
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendComplete)

class AFX_EXT_CLASS MsgNextPlanInsert: public BaseMsg
{    // CAD send plan data(insert or change) to next console
	DECLARE_CLBTH(MsgNextPlanInsert)

public:
	MsgNextPlanInsert();
	virtual ~MsgNextPlanInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
	I_TIME        send_time;  //����ʱ��
	USERNAME    sender_name;  //�����û���

	TRAIN_NO    arrive_train_id;//���ﳵ�κ�
	TRAIN_NO    depart_train_id;//�������κ�
	TRAIN_INDEX train_index;//��������
	STATION_NO  station;//վ��
	I_TIME        arrive;//����ʱ��
	I_TIME        depart;//����ʱ��
	IBYTE        entry;//���
	IBYTE        exit;//����
	IBYTE        arrive_side;//����ɵ�
	IBYTE        depart_side;//�����ɵ�
	IWORD   early_depart_time;//��ǰ����ʱ��
	IBYTE     run_fast_slow_time;//���������
	IBYTE     speed_limit;//�ٶ�����
	IBYTE     stop_condition;//ͣ������
	IWORD   adjust_status;//����״̬
	IWORD   flag; //��ʾ
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanInsert)

class AFX_EXT_CLASS MsgNextPlanTrainRecv: public BaseMsg
{    // gpc send CAD recv each plan
	DECLARE_CLBTH(MsgNextPlanTrainRecv)

public:
	MsgNextPlanTrainRecv();
	virtual ~MsgNextPlanTrainRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
	I_TIME        send_time;  //����ʱ��

	TRAIN_NO    arrive_train_id;//���ﳵ�κ�
	TRAIN_NO    depart_train_id;//�������κ�
	TRAIN_INDEX train_index;//��������
	STATION_NO  station;//վ��
	I_TIME        arrive;//����ʱ��
	I_TIME        depart;//����ʱ��
	IBYTE        entry;//���
	IBYTE        exit;//����
	IBYTE        side;//�ɵ�
	IWORD      early_depart_time;//��ǰ����ʱ��
	IBYTE        run_fast_slow_time;//���������
	IBYTE        speed_limit;//�ٶ�����
	IBYTE        stop_condition;//ͣ������
	IWORD      adjust_status;//����״̬
	IWORD      flag;//��ʾ

	IWORD      passway;//��·

	enum RECV_STATUS {//����״̬
		RECV_ACCEPT,//����
		RECV_REJECT//�ܾ�
	}       recv_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanTrainRecv)

class AFX_EXT_CLASS MsgNextPlanRecvStatus: public BaseMsg
{     // next console send recv all msg to CAD, then CAD relays to GPC
	DECLARE_CLBTH(MsgNextPlanRecvStatus)

public:
	MsgNextPlanRecvStatus();
	virtual ~MsgNextPlanRecvStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//�ƻ���ʼʱ��
	I_TIME        end_time;//�ƻ�����ʱ��
	I_TIME        send_time;//�ƻ�����ʱ��
	I_TIME    recv_time;//�ƻ�����ʱ��

	enum PLAN_RECV_STATUS {//�ƻ�����״̬
		PLAN_RECV,//�ƻ�����
		PLAN_SENDING,//�ƻ�������
		PLAN_NO_CHANGE,//�ƻ�û�б仯
		PLAN_NO_DATA,//�ƻ�������
		PLAN_ENTITY_DEAD,//ʵ������Ӧ
		PLAN_CONFIG_WRONG,//���ô���
		PLAN_INIT//�ƻ���ʼ��
	} recv_status;

	USERNAME  sender_name;//�����û���
	USERNAME    recver_name;//�����û���
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanRecvStatus)

class AFX_EXT_CLASS MsgNextPlanSignStatus: public BaseMsg
{     // next console send recv all msg to CAD, then CAD relays to GPC
	DECLARE_CLBTH(MsgNextPlanSignStatus)

public:
	MsgNextPlanSignStatus();
	virtual ~MsgNextPlanSignStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//����ʵ���
	ENTITY_ID   recv_entity;//����ʵ���

	CONID       send_con_id;//���͵���̨��
	CONID       recv_con_id;//���յ���̨��

	I_TIME        begin_time;//�ƻ���ʼʱ��
	I_TIME        end_time;//�ƻ�����ʱ��
	I_TIME        send_time;//�ƻ�����ʱ��
	I_TIME    recv_time;//�ƻ�����ʱ��

	USERNAME  sender_name;//�����û���
	USERNAME    recver_name;//�����û���

	short       accept_count;//������
	short       reject_count;//�ܾ���

	TRAIN_NO    accept_train[100];//���ճ���
	TRAIN_NO    reject_train[100];//�ܾ�����
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSignStatus)

class AFX_EXT_CLASS MsgWordPhasePlanSend: public BaseMsg
{    // gpc to cad, cad to gpc and cad to stpc
	DECLARE_CLBTH(MsgWordPhasePlanSend)

public:
	MsgWordPhasePlanSend();
	virtual ~MsgWordPhasePlanSend();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//�հ�ƻ�id��
	CONID   con_id;//����̨��
	IBYTE   schd_type;//����������
	IBYTE   phase_id;//�׶μƻ�id
	short   recv_count;//���ս׶μƻ�ʵ����
	ENTITY_ID recv_list[128];//���ս׶μƻ�ʵ��
	ENTITY_ID send_entity;//����ʵ���
	I_TIME    edit_time;  //�༭ʱ��
	I_TIME    send_time;  //����ʱ��
	USERNAME  send_name;  //�����û���
	USERNAME  permit_name;  //����û���
	IWORD   start_sta_weather;  //��ʼվ����
	IWORD   end_sta_weather;  //����վ����
	short   phase_len;  //�׶μƻ�����
	char    phase_content[2048];  //�׶μƻ�����
	enum    WORDPHASE_OPER {
		WORDPHASE_SERVER_ECHO,  //
		WORDPHASE_SAVE, //�ƻ�����
		WORDPHASE_SEND  //�ƻ�����
	}   phase_oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanSend)

class AFX_EXT_CLASS MsgWordPhasePlanStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgWordPhasePlanStatus)

public:
	MsgWordPhasePlanStatus();
	virtual ~MsgWordPhasePlanStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//�հ�ƻ�id��
	CONID   con_id;//����̨��
	IBYTE   schd_type;//����������
	IBYTE   phase_id;//�׶μƻ�id��
	ENTITY_ID send_entity;//����ʵ���
	ENTITY_ID recv_entity;  //����ʵ���
	I_TIME    send_time;//����ʱ��
	I_TIME    recv_time;//����ʱ��
	I_TIME    edit_time;  //�༭ʱ��
	USERNAME  send_name;//�����û���
	USERNAME  permit_name;//����û���
	USERNAME  recv_name;//�����û���

	enum WORDPHASE_RECV_STATUS {//����״̬
		WORDPHASE_RECV,//����
		WORDPHASE_SIGN, //ǩ��
		WORDPHASE_SENDING,//������
		WORDPHASE_ENTITY_DEAD,//ʵ������Ӧ
		WORDPHASE_CONFIG_WRONG,//���ô���
		WORDPHASE_ERROR,//����
		WORDPHASE_INIT//��ʼ��
	} recv_status;

};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanStatus)

class AFX_EXT_CLASS MsgWordPhasePlanQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgWordPhasePlanQuery)

public:
	MsgWordPhasePlanQuery();
	virtual ~MsgWordPhasePlanQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//�հ�ƻ�id��
	CONID   con_id;//����̨��
	IBYTE   schd_type;//����������
	IBYTE   phase_count;//�׶μƻ���
	IBYTE   phase_id[4];//�׶μƻ���

};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanQuery)

class AFX_EXT_CLASS MsgSimpleStationCarFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarFormat)

public:
	MsgSimpleStationCarFormat();
	virtual ~MsgSimpleStationCarFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME          input_time;//����ʱ��
	STATION_NO    station;//վ��
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;

	short         index;//����
	char          side_name[20];//�ɵ���
	IBYTE          side;//�ɵ�
	/* ��station_now_car.xml�Ķ�Ӧ��ϵ   */
	IBYTE            car_flag;   /* work_explain�����                */
	IBYTE            car_type;   /* (��-0x80,��-0x00) & common1����� */
	short           car_count;//����
	char            destine[512];//
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarFormat)

class AFX_EXT_CLASS MsgSimpleStationCarCtrl : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarCtrl)

public:
	MsgSimpleStationCarCtrl();
	virtual ~MsgSimpleStationCarCtrl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME          input_time;//����ʱ��
	STATION_NO    station;//վ��
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;
	enum MSG_STATUS {MSG_BEGIN, MSG_END} status;//��Ϣ״̬:��Ϣ��ʼ,��Ϣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarCtrl)

class AFX_EXT_CLASS MsgSimpleStationCarQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarQuery)

public:
	MsgSimpleStationCarQuery();
	virtual ~MsgSimpleStationCarQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME     begin_time;//��ѯ��ʼʱ��
	I_TIME     end_time;//��ѯ����ʱ��
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;
	short      sta_count;//վ��
	STATION_NO sta_list[64];//վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarQuery)

class AFX_EXT_CLASS MsgSimpleStationCarConvert : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarConvert)

public:
	MsgSimpleStationCarConvert();
	virtual ~MsgSimpleStationCarConvert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	};
	STACAR_PHASE  from_phase;//�����ĸ��׶�
	STACAR_PHASE  to_phase;//ת�����ĸ�����
	I_TIME            from_input_time;//
	I_TIME            to_input_time;//

	short           sta_count;//վ��
	STATION_NO      sta_list[64];//վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarConvert)

class AFX_EXT_CLASS MsgNotePadFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgNotePadFormat)

public:
	MsgNotePadFormat();
	virtual ~MsgNotePadFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID         shift_id;//�հ�ƻ�id��

	CONID            con_id;//����̨��
	IBYTE             schd_type;//����������

	short            data_len;//���ݳ���
	char             data[4000];//����
};
DECLARE_SERIALIZE_OPERATOR(MsgNotePadFormat)

class AFX_EXT_CLASS MsgNotePadQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgNotePadQuery)

public:
	MsgNotePadQuery();
	virtual ~MsgNotePadQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID         shift_id;//�հ�ƻ�id��

	CONID            con_id;//����̨��
	IBYTE             schd_type;//����������
};
DECLARE_SERIALIZE_OPERATOR(MsgNotePadQuery)

class AFX_EXT_CLASS MsgAccidentOper : public BaseMsg
{
	DECLARE_CLBTH(MsgAccidentOper)

public:
	MsgAccidentOper();
	virtual ~MsgAccidentOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID            con_id;//����̨��
	IBYTE             schd_type;//����������
	SHIFT_ID         shift_id;//�հ�ƻ�id��
	short            index;//����
	short            note_len;//ע�ͳ���
	char             note[2400];//ע��
	char             subject[40];//����

	enum AccidentOper {
		ACCIDENT_SERVER_ECHO,//
		ACCIDENT_INSERT_UPDATE,//
		ACCIDENT_DELETE//
	}                 oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgAccidentOper)

class AFX_EXT_CLASS MsgAccidentQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgAccidentQuery)

public:
	MsgAccidentQuery();
	virtual ~MsgAccidentQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID            con_id;//����̨��
	IBYTE             schd_type;//����������
	SHIFT_ID         shift_id;  //�հ�ƻ�id��
};
DECLARE_SERIALIZE_OPERATOR(MsgAccidentQuery)

class AFX_EXT_CLASS MsgUserManager : public BaseMsg
{
	DECLARE_CLBTH(MsgUserManager)

public:
	MsgUserManager();
	virtual ~MsgUserManager();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short count;//����
	struct {
		USERNAME  user_code;//�û���
		USERNAME  user_name;//�û���
		PASSWORD  user_pwd;//�û�����
		JOB_ID    job_id;//������
		IBYTE   console_count;//����̨��
		CONID   con_list[20];//����̨��
	} data[30];

	enum USER_OPER{//�û�����
		USERMGR_INSERT,//����
		USERMGR_DELETE,//ɾ��
		USERMGR_CHANGE,//�ı�
		USERMGR_CHANGE_PROP,//
		USERMGR_SERVER_ECHO//
	} oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgUserManager)

class AFX_EXT_CLASS MsgUserManagerQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgUserManagerQuery)

public:
	MsgUserManagerQuery();
	virtual ~MsgUserManagerQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
};
DECLARE_SERIALIZE_OPERATOR(MsgUserManagerQuery)

class AFX_EXT_CLASS MsgRouteStatusInform : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteStatusInform)

public:
	MsgRouteStatusInform();
	virtual ~MsgRouteStatusInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO      station;//վ��
	TRAIN_NO      train_id;//���κ�
	TRAIN_INDEX     train_index;//��������
	IBYTE       rec_index;//��������
	CONID       con_id;//����̨��

	ENTRY_ID      entry;//���
	ENTRY_ID      exit;//����
	SIDE_ID       side;//�ɵ�

	enum ROUTE_ATTRIBUTE {//��·����
		PASS_ROUTE,//ͨ����·
		ARRIVE_ROUTE,//�����·
		DEPART_ROUTE//������·
	}                   attrib;
	ROUTE_TRIG_STATUS   status;//��·����״̬
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteStatusInform)

class AFX_EXT_CLASS MsgGroupTrainWorkFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkFormat)

public:
	MsgGroupTrainWorkFormat();
	virtual ~MsgGroupTrainWorkFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//����̨��
	IBYTE       schd_type;  //����������
	short       rec_count;  //������
	struct {
		I_TIME      input_time; //����ʱ��
		TRAIN_INDEX   train_index;  //�г�����
		TRAIN_NO    train_id; //���κ�
		STATION_NO    station;  //վ��
		IBYTE     work_type;  //��������
		IBYTE     side; //�ɵ�
		char      work_content[20]; //��������
	}         data[40];
	enum GROUP_TRAIN_WORK_OPER {
		GPC_SET,  //gpc����
		GPC_DELETE, //gpcɾ��
		SERVER_ECHO   //
	}         oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkFormat)

class AFX_EXT_CLASS MsgGroupTrainWorkDel : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkDel)

public:
	MsgGroupTrainWorkDel();
	virtual ~MsgGroupTrainWorkDel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//����̨��
	IBYTE       schd_type;//����������
	short       rec_count;//������

	I_TIME        input_time[40];
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkDel)

class AFX_EXT_CLASS MsgGroupTrainWorkQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkQuery)

public:
	MsgGroupTrainWorkQuery();
	virtual ~MsgGroupTrainWorkQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	SHIFT_ID            shift_id;//�հ�ƻ��е�id
	CONID       con_id;//����̨��
	IBYTE       schd_type;//����������
	short       rec_count;//

	TRAIN_INDEX     train_index[256];//��������
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkQuery)

class AFX_EXT_CLASS MsgAdjustSequenceLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgAdjustSequenceLoad)

public:
	MsgAdjustSequenceLoad();
	virtual ~MsgAdjustSequenceLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID con_id;
};
DECLARE_SERIALIZE_OPERATOR(MsgAdjustSequenceLoad)

class AFX_EXT_CLASS MsgAdjustSequenceData : public BaseMsg
{
	DECLARE_CLBTH(MsgAdjustSequenceData)

public:
	MsgAdjustSequenceData();
	virtual ~MsgAdjustSequenceData();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID   con_id;//����̨��
	short   count;//
	enum  OPER {
		ADJUSTSEQ_SERVER_ECHO,
		ADJUSTSEQ_GPC_SET,
		ADJUSTSEQ_GPC_REFRESH
	}     oper;
	struct {
		TRAIN_INDEX train_index;//��������
		IWORD   prio;
	}     data[600];
};
DECLARE_SERIALIZE_OPERATOR(MsgAdjustSequenceData)

class AFX_EXT_CLASS MsgAutoAdjustInform : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustInform)

public:
	MsgAutoAdjustInform();
	virtual ~MsgAutoAdjustInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID   con_id;//����̨��
	I_TIME    start_time;//��ʼʱ��
	I_TIME    end_time;//����ʱ��
	enum  OPER {
		AUTOADJUST_SERVER_ECHO_BEGIN,
		AUTOADJUST_SERVER_ECHO_END,
		AUTOADJUST_SERVER_ECHO_FAILED,
		AUTOADJUST_GPC_SET,
	}     oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustInform)

class AFX_EXT_CLASS MsgAutoAdjustSingleInform : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustSingleInform)

public:
	MsgAutoAdjustSingleInform();
	virtual ~MsgAutoAdjustSingleInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID        con_id;//����̨��
	TRAIN_INDEX  train_index;//��������
	short        adjust_begin_sta_index;//������ʼ�ĳ�վ����
	short        adjust_end_sta_index;//���������ĳ�վ����
	enum    SINGLE_ADJUST_PRIO {
		SINGLE_HIGH_CHANGE_FOLLOWING,
		SINGLE_HIGH_CHANGE_SELF_ONLY,
		SINGLE_NORMAL_CHANGE_FOLLOWING,
		SINGLE_NORMAL_CHANGE_SELF_ONLY,
		SINGLE_LOW,
	}     prio;
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustSingleInform)

class AFX_EXT_CLASS MsgAutoAdjustUndo : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustUndo)

public:
	MsgAutoAdjustUndo();
	virtual ~MsgAutoAdjustUndo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID        con_id;//����̨��
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustUndo)

// ���ɻ�����GPCID��GPC������Ϣ
class AFX_EXT_CLASS MsgLiRCToGpc : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCToGpc)
public:
	MsgLiRCToGpc();
	virtual ~MsgLiRCToGpc();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	GPC_ID      nGpcID;//gpc��
	IWORD     nMsgLen;//��Ϣ����
	enum { BUFFER_SIZE = 2048 };
	IBYTE     Msg[BUFFER_SIZE];//��Ϣ
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCToGpc)

// shuntnote data
class AFX_EXT_CLASS MsgShuntNoteSend2LocoMotive: public BaseMsg
{     // GPC orders LIRC to send ShuntNote to locomotive
	DECLARE_CLBTH(MsgShuntNoteSend2LocoMotive)

public:
	MsgShuntNoteSend2LocoMotive();
	virtual ~MsgShuntNoteSend2LocoMotive();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX   shunt_index;             //  ��ҵ��
	STATION_NO    nstation;                //  վ��
	TRAIN_NO    train_id;                //  ���κ�
	ENGINE_NO     engine_id;               //  ��ҵ�����Ż��߳��κ�
	I_TIME          send_time;               //  �·�ʱ��

	enum   SEND_WAY {
		SEND_INTELLIGENT,                         //���ܷ���
		SEND_IMMEDIATELY                          //��������
	}send_way;                             //   ���ͷ�ʽ

	enum   OPER_TYPE {
		SEND_2_LOCOMOTIVE,                        //���͸�����
		DELETE_FROM_LIRC                          //�����ɻ���ɾ��
	}oper_type;                             //   ��������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSend2LocoMotive)


class AFX_EXT_CLASS MsgShuntNote2LocoMotiveStatus: public BaseMsg
{    // send MsgShuntNoteStatus from lirc to gpc
	DECLARE_CLBTH(MsgShuntNote2LocoMotiveStatus)

public:
	MsgShuntNote2LocoMotiveStatus();
	virtual ~MsgShuntNote2LocoMotiveStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//����������
	STATION_NO      nstation;                //  վ��
	enum STATUS{
		SHUNT_RECEIVE,//��ҵ������
		SHUNT_SIGNATURE,//��ҵ��ǩ��
		SHUNT_SENDING,//��ҵ��������
		SHUNT_NOT_SEND,//��ҵ��δ����
		SHUNT_INIT,//��ҵ����ʼ��
		SHUNT_DELETED     //��ҵ���ѱ�ɾ��
	} status;
	I_TIME            send_time;//����ʱ��
	I_TIME            sign_time;//ǩ��ʱ��
	USERNAME    sign_name;//ǩ������

};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNote2LocoMotiveStatus)

class AFX_EXT_CLASS MsgDataRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgDataRequest)

public:
	MsgDataRequest();
	virtual ~MsgDataRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid;//��Ϣid
};
DECLARE_SERIALIZE_OPERATOR(MsgDataRequest)

class AFX_EXT_CLASS MsgServerProcessEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgServerProcessEcho)

public:
	MsgServerProcessEcho();
	virtual ~MsgServerProcessEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid; //��Ϣid
	I_TIME   recv_time;//����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgServerProcessEcho)

class AFX_EXT_CLASS MsgQueryTrainForRoute: public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainForRoute)

public:
	MsgQueryTrainForRoute();
	virtual ~MsgQueryTrainForRoute();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // �˹�������ΨһID��ʶ
	STATION_NO    nStationNo;//վ��
	IBYTE     nButtonCount;//��ť��
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];//��ť��
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];//��ť����
	ROUTE_ID    nRouteID;//��·��
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };//��Ч���г�������
	ROUTE_TYPE    nRouteType;//��·����
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainForRoute)

class AFX_EXT_CLASS MsgAnswerTrainForRoute: public BaseMsg
{
	DECLARE_CLBTH(MsgAnswerTrainForRoute)

public:
	MsgAnswerTrainForRoute();
	virtual ~MsgAnswerTrainForRoute();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;//�˹�����id
	STATION_NO    nStationNo;//վ��
	ROUTE_ID    nRouteID;//��·��
	TRAIN_NO    tnTrainNo;//���κ�
};
DECLARE_SERIALIZE_OPERATOR(MsgAnswerTrainForRoute)

// ͨ��״̬����
class AFX_EXT_CLASS MsgCommStatusReport: public BaseMsg
{
	DECLARE_CLBTH(MsgCommStatusReport)

public:
	MsgCommStatusReport();
	virtual ~MsgCommStatusReport();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum
	{
		CSR_ID_LIRC = 1,
		CSR_ID_STPC = 2,
		CSR_ID_BL   = 3,
		CSR_ID_CAD  = 4,
		CSR_ID_GPC  = 5,

		CSR_DATA_LOST   = 1,    // ��⵽���ݶ�ʧ
		CSR_REMOTE_DOWN     = 2,    // ��⵽�Է��ж�
		CSR_LOCAL_UP    = 3       // ������λͨ��
	};

	IBYTE   nStatus;//״̬
	IBYTE        nIdentify;
};
DECLARE_SERIALIZE_OPERATOR(MsgCommStatusReport)

// ̽����Ϣ
class AFX_EXT_CLASS MsgPeerDetect: public BaseMsg
{
	DECLARE_CLBTH(MsgPeerDetect)

public:
	MsgPeerDetect();
	virtual ~MsgPeerDetect();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:

};
DECLARE_SERIALIZE_OPERATOR(MsgPeerDetect)

//�˴�ΪGSM-R������Ϣ
//���߳��κ���Ϣ��BASELEVEL�յ����͵�LIRC��SERVER
class AFX_EXT_CLASS MsgRadioTrainInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioTrainInfo)

public:
	MsgRadioTrainInfo();
	virtual ~MsgRadioTrainInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //���Σ�for wireless
	TRAIN_NO    locomotive; //������,for wireless
	IBYTE        motivetype; //��������
	STATION_NO  station_no; //�ó���ǰ���ڳ�վ
	IDWORD       distance;   //����꣨�ף�
	short       speed;      //�ٶȣ�0.1 KM/h��
	short       car_cnt;     //����
	short       car_len;        //�ǳ�   (0.1)
	ISDWORD        total_weight;   //ȫ��   (0.1��)
	IBYTE        train_status;   //1����,2ͣ��,Ĭ��0
	IBYTE        train_com;      //�г������� 0x55 �г�����, 0x77 �г������� ����ȷ��
	USERNAME    driver;         //˾������
	TRAIN_NO    conf_trainno;   //CTCȷ�ϳ��κ�
	char        othermsg[22];   //������Ϣ

};
DECLARE_SERIALIZE_OPERATOR(MsgRadioTrainInfo)

class AFX_EXT_CLASS MsgGsmrTrainInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgGsmrTrainInfo)

public:
	MsgGsmrTrainInfo();
	virtual ~MsgGsmrTrainInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IWORD	token; //��־MSG_GSMR_TRAININFO 1
	IWORD	nLen; //���ݳ���
	char	content[4096];
};
DECLARE_SERIALIZE_OPERATOR(MsgGsmrTrainInfo)

///��������վ��ͼ��Ϣ
///BASELEVEL�յ��������ͻ�������վ��ͼ��Ϣ����BL�ж�Ӧ��ó�����վ��ͼ������Lirc���ʹ���Ϣ��
//Lirc�յ�����Ϣ����ó����ͱ�ʾ��Ϣ��
class AFX_EXT_CLASS MsgMotiveRequestDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgMotiveRequestDisplay)

public:
	MsgMotiveRequestDisplay();
	virtual ~MsgMotiveRequestDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  station_no; //�ó������ʾ�ĳ�վ

};
DECLARE_SERIALIZE_OPERATOR(MsgMotiveRequestDisplay)

//����ϢĿǰԤ�������޴�Ӧ��

class AFX_EXT_CLASS MsgMotiveDeviceAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgMotiveDeviceAlarm)

public:
	MsgMotiveDeviceAlarm();
	virtual ~MsgMotiveDeviceAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  station_no; //�����г����ڵĳ�վ
	IWORD        alarm_status; //����״̬
	IBYTE        alarm_length;
	char        alarm_dis[256]; //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgMotiveDeviceAlarm)
//�г���·Ԥ��
class AFX_EXT_CLASS MsgRouteForetell : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteForetell)

public:
	MsgRouteForetell();
	virtual ~MsgRouteForetell();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  station_no; //ҪԤ��ĳ�վ
	IBYTE        flag;       //�ӷ�����־1:����ͨ�� 2:����ͨ��  3:��վͣ��
	IBYTE        side;       //�ɵ���
	IBYTE        describe_len;
	char        describe[256];  //����
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteForetell)
//���߱�ʾ��Ϣ
class AFX_EXT_CLASS MsgRadioMotiveDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioMotiveDisplay)

public:
	MsgRadioMotiveDisplay();
	virtual ~MsgRadioMotiveDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  station_no; //Ҫ���͵ĳ�վ
	enum { MAX_CONTENT_SIZE = 600 };
	IWORD        elementlen; //Ԫ�����͵ĸ���
	IBYTE        elementdata[MAX_CONTENT_SIZE]; //Ԫ��״̬����
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioMotiveDisplay)

class AFX_EXT_CLASS MsgTrainIDUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainIDUpdate)

public:
	MsgTrainIDUpdate();
	virtual ~MsgTrainIDUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    org_train_id; //ԭ����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	TRAIN_NO    update_train_id;  //���ĺ�ĳ��κ�
	char        describe[22];  //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainIDUpdate)

//�г���ʻ���ܽ������ɻ����͸�BL
class AFX_EXT_CLASS MsgTrainOutofControl : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainOutofControl)

public:
	MsgTrainOutofControl();
	virtual ~MsgTrainOutofControl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOutofControl)

class AFX_EXT_CLASS MsgRadioShuntNoteRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioShuntNoteRequest)

public:
	MsgRadioShuntNoteRequest();
	virtual ~MsgRadioShuntNoteRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  stationno;  //��ҵ��վ��
	IWORD        shunt_index; //��ҵ����
	IBYTE        gou_index;   //�����
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioShuntNoteRequest)

class AFX_EXT_CLASS MsgRadioShuntNote : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioShuntNote)

public:
	MsgRadioShuntNote();
	virtual ~MsgRadioShuntNote();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //����
	TRAIN_NO    locomotive; //������
	IBYTE        motivetype; //��������
	STATION_NO  stationno;  //��ҵ��վ��
	IWORD        shunt_index; //��ҵ����
	I_TIME        send_time;   //��ҵ���´�ʱ��(time_t)
	I_TIME        start_time;  //��ҵ��ʼʱ��(time_t)
	I_TIME        occ_time;    //��ҵԤ��ʱ��(time_t)
	USERNAME    shunt_group_name;//����������
	USERNAME    shunt_leader_name;//������������
	IBYTE        oper_len;         //��ҵ����
	char        operation_describe[64];//��ҵ˵��
	IBYTE        gou_count; //�ܹ���

	struct GOU_LIST{
		IWORD  shunt_gou_index;            //  ������Ψһ����
		IWORD  shunt_gou_type;             //  ����������
		//    BIT7-0: 0 ��,1 ˦, 2�������ͷ,3 �����ժͷ
		//    BIT10,8: ǣ���߱��,7��ʾ���������,
		//              0��ʾ����ǣ����
		//    BIT11: �Ƿ�Ӱ������
		SIDE_ID   side;                     //  ��ҵ�ɵ�
		IWORD    shunt_car_count;          //  ��ҵ������
		IWORD    shunt_note_route_count;   //  ��·����
		IWORD    shunt_work_time;          //  ������·��ҵʱ��
		IBYTE   route_describe_len;
		char   route_describe[256];          //  ������·�ļ���
		IBYTE   gou_describe_len;
		char   gou_describe[256];            //  ������ҵ�ı�ע
	}gou_list[MAX_GOU_PER_SHUNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioShuntNote)

//GPC��STPC�������ߵ�������ʱ���ȷ��ʹ���Ϣ��ѯ����λ��
class AFX_EXT_CLASS MsgQueryTrainPosition : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainPosition)

public:
	MsgQueryTrainPosition();
	virtual ~MsgQueryTrainPosition();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id; //����
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainPosition)

class AFX_EXT_CLASS MsgReplyTrainPosition : public BaseMsg
{
	DECLARE_CLBTH(MsgReplyTrainPosition)

public:
	MsgReplyTrainPosition();
	virtual ~MsgReplyTrainPosition();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id; //����
	IBYTE        train_count;
	struct TRAIN_INFO{
		TRAIN_NO    locomotive; //������
		IBYTE        motivetype; //��������
		STATION_NO  stationno;  //Ŀǰ���ڳ�վ��վ��
	}train_info[MAX_TRAIN_POSITION];
};
DECLARE_SERIALIZE_OPERATOR(MsgReplyTrainPosition)

//BL�жϳ������ĸ���������վ��ͼ��Ϣ�����ʹ���Ϣ����������ѯ�ó���ǰ��վ
class AFX_EXT_CLASS MsgQueryPreStationList : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryPreStationList)

public:
	MsgQueryPreStationList();
	virtual ~MsgQueryPreStationList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id;   //����
	STATION_NO  cur_stno;   //��ǰ���ڳ�վ
	IBYTE        requestcnt; //����վ��
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryPreStationList)

//���������Ͳ�ѯ�����BL
class AFX_EXT_CLASS MsgReplyPreStationList : public BaseMsg
{
	DECLARE_CLBTH(MsgReplyPreStationList)

public:
	MsgReplyPreStationList();
	virtual ~MsgReplyPreStationList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id;   //����
	STATION_NO  cur_stno;   //��ǰ���ڳ�վ
	IBYTE        requestcnt; //����վ��
	STATION_NO  st_list[MAX_STATION_LIST]; //��ѯ��վ���б�
};
DECLARE_SERIALIZE_OPERATOR(MsgReplyPreStationList)

class AFX_EXT_CLASS MsgGsmrLocalMsg: public BaseMsg
{
	DECLARE_CLBTH(MsgGsmrLocalMsg)

public:
	MsgGsmrLocalMsg();
	virtual ~MsgGsmrLocalMsg();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char DestIpaddress[24];  //Ŀ��IP��ַ
	IWORD DestPort;           //Ŀ��˿�
	IBYTE MsgId;              //��ϢID
	IBYTE CommandId;          //�����
	IWORD Msglen;             //��Ϣ����
	IBYTE MsgContent[4096];         //��Ϣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgGsmrLocalMsg)



//���ݳ��κŲ�ѯ������Ϣ->
class AFX_EXT_CLASS MsgQueryLocomotiveInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryLocomotiveInfo)

public:
	MsgQueryLocomotiveInfo();
	virtual ~MsgQueryLocomotiveInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	IWORD        console_no;    // ����̨ ̨��
	TRAIN_NO    train_id;       // ����

};
DECLARE_SERIALIZE_OPERATOR(MsgQueryLocomotiveInfo)

//���ݳ��κŲ�ѯ������Ϣ->����
class AFX_EXT_CLASS MsgLocomotiveInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgLocomotiveInfo)

public:
	MsgLocomotiveInfo();
	virtual ~MsgLocomotiveInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	IWORD        console_no;    // ����̨ ̨��
	TRAIN_NO    train_id;       // ����
	short loco_count;    //��STPC֪�����͵��г�
	struct LOCO_LIST{
		TRAIN_NO    locomotive;     // ������
		IDWORD       kilosignal;     // �����
	}loco_list[6];
};
DECLARE_SERIALIZE_OPERATOR(MsgLocomotiveInfo)

//���õ�����״̬
class AFX_EXT_CLASS MsgElecArmSet : public BaseMsg
{
	DECLARE_CLBTH(MsgElecArmSet)

public:
	MsgElecArmSet();
	virtual ~MsgElecArmSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	short      elec_arm_id;           // �����ۺ�
	STATION_NO elec_sta;              // ��վ��

	ENTITY_ID  set_entity;            // ������
	I_TIME       set_time;              // ����ʱ��

	ISDWORD       status;                // ����״̬
	char       remark[128];           // ע��

	enum { ELEC_ENABLE = 0xAA, ELEC_DISABLE = 0x55, PREP_ENABLE=0x5BEC, PREP_DISABLE=0x5BCE  };

};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmSet)

//���������״̬
class AFX_EXT_CLASS MsgElecArmQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgElecArmQuery)

public:
	MsgElecArmQuery();
	virtual ~MsgElecArmQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	short    elec_arm_list[128];      // �����б�
	short    elec_arm_count;          // �������

	STATION_NO   elec_sta;
	enum SOURCE { LIRC, GPC} query_source;
};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmQuery)

//�ظ�������״̬
class AFX_EXT_CLASS MsgElecArmAnswer : public MsgElecArmSet
{
	DECLARE_CLBTH(MsgElecArmAnswer)
public:
	MsgElecArmAnswer();
	virtual ~MsgElecArmAnswer();
private:
protected:

};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmAnswer)

//����㴫��
class AFX_EXT_CLASS MsgTrainDelayTime : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDelayTime)

public:
	MsgTrainDelayTime();
	virtual ~MsgTrainDelayTime();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO  nStationNo;//վ��
	TRAIN_NO  tnTrain;//���κ�
	ISDWORD     nDelayTime;//�ӳ�ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDelayTime)

class AFX_EXT_CLASS MsgPackRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgPackRequest)
public:
	MsgPackRequest();
	virtual ~MsgPackRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short        flag;   // set 0

	short        entity_count;//ʵ����
	ENTITY_ID    sender[16];//������
	ENTITY_ID    recver[16];//������
};
DECLARE_SERIALIZE_OPERATOR(MsgPackRequest)

class AFX_EXT_CLASS MsgCollector: public BaseMsg
{
	DECLARE_CLBTH(MsgCollector)
public:
	MsgCollector();
	virtual ~MsgCollector();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID    sender;//������
	ENTITY_ID    recver;//������

	MSGHEAD::msgid_t id;//��Ϣid
	char         para[16];//����
};
DECLARE_SERIALIZE_OPERATOR(MsgCollector)

class AFX_EXT_CLASS MsgPackAnswer: public BaseMsg
{
	DECLARE_CLBTH(MsgPackAnswer)
public:
	MsgPackAnswer();
	virtual ~MsgPackAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short     msg_attrib;//��Ϣ����
	ENTITY_ID dest_id;//Ŀ��ʵ��
	ENTITY_ID source_id;//Դʵ��
	I_TIME      tm;//
	I_TIME      subtm;//
	ISDWORD      label;//��ʶ
	short     data_len;//
	short     check_sum;//У����
	ISDWORD      spare;//

	short pack_len;//����
	char  package[4000];//������
};

DECLARE_SERIALIZE_OPERATOR(MsgPackAnswer)


// �µ�����Ϣ����

class AFX_EXT_CLASS MsgDCmdTitle : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTitle)

public:
	MsgDCmdTitle();
	virtual ~MsgDCmdTitle();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	// ��������Ψһʶ�� send_entity+create_time
	ENTITY_ID     send_entity;     // ����ʵ��
	I_TIME        create_time;     // ����ʱ��

	I_TIME        send_time;       // ����ʱ��
	I_TIME        edit_time;       // �༭ʱ��

	CONID         con_id;          // GPC����ʱʹ�ã�����̨����
	STATION_NO    source_station;  // STPC����ʱʹ�ã�վ��

	short         bian_ma;         // �������ͱ���
	char          cmd_no[16];      // �������
	char          cmd_name[512];   // ������������,���Ը������ͱ�������֪
	char          rereader[32];    // ����������
	char          dispatcher[32];  // ����Ա����
	char          unit[32];        // ���λ
	char          reader[32];      // ֵ����������
	I_TIME        read_time;       // ֵ������ǩ��ʱ��
	char          recver[128];     // ���λ���

	enum Status
	{
		EDIT,          //  �༭
		SEND,          //  ����
		RECV,          //  ȫ������
		SIGN,          //  ȫ��ǩ��
		RE_SEND,       //  ת����̨�����־
	}   status;

	enum CMD_DESTINATION
	{
		GPC_2_DIRECOR_ON_DUTY,
		DIRECOR_ON_DUTY_2_GPC,
		GPC_2_GPC,
		GPC_2_STPC,
		GPC_2_LIRC,
		GPC_2_STPC_AND_LIRC, //û���õ���ʵ���Ϸ����η��͵�
		STPC_2_LIRC,
		STPC_2_GPC,          //������������
	} cmd_destination;

	enum CMD_OPER
	{
		NORMAL_CMD,          // ������������
		WORK_CMD,            // ʩ����������
		TRANSFER_CMD,    // ת������������ں��벻��ʱʹ��
		REQUEST_CMD          // �����������
	} cmd_oper;

	friend class MsgDCmdData;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitle)

class AFX_EXT_CLASS MsgDCmdSendList : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdSendList)

public:
	MsgDCmdSendList();
	virtual ~MsgDCmdSendList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// ��������Ψһʶ��
	ENTITY_ID     send_entity;    // ����ʵ��
	I_TIME        create_time;    // ����ʱ��

	//��������ߵ�������
	enum CMD_TYPE
	{
		DISPATCH_COMMAND = 0x01,          // ��������
		ROUTE_TICKET,                     // ·Ʊ
		GREEN_ADMISSION,                  // ��ɫ���֤
		RED_ADMISSION,                    // ��ɫ���֤
		SPY_ON_SHUNT_NOTE,                // ��վ���ٵ���֪ͨ��
		USE_LIGHT_CAR_ADMISSION,          // �����г�ʹ��˵����
		FORETELL_TRAIN_ROUTE,             // �г���·Ԥ����Ϣ
		JUST_LINE_PASS,                   // ����ͨ����
		BY_LINE_PASS,                     // ����ͨ����
		JUST_LINE_SLOW_PASS,              // ���߻���ͨ����
		ENTER_JUST_LINE_STOP,             // ��վ����ͣ����
		ENTER_BY_LINE_STOP,               // ��վ����ͣ����
		SHUNTING_JOB = 0x11,              // ������ҵ
		SHUNTING_REQUEST_RECEIVED = 0x12, // �����������յ�
		IN_OUT_CHECK = 0x20,              // �������
		OTHER_COMMAND,                    // ��������
	} cmd_type;


	// ������������
	enum RECV_STATUS
	{
		COMMAND_RECV,                //�������
		COMMAND_SIGN,                //����ǩ��
		COMMAND_LOCOMOTIVE_RECV,     //��������
		COMMAND_LOCOMOTIVE_SIGN,     //˾��ǩ��
		COMMAND_SENDED,              // ���ɻ��ѷ���
		COMMAND_ENTITY_DEAD,         //ʵ������Ӧ
		COMMAND_ERROR,               //����
		COMMAND_DENY,                // �ܾ�
		COMMAND_INIT                 //��ʼ��
	};

	//��ͨ�����stpc,gpc��
	struct RECV_LIST
	{
		ENTITY_ID    recv_entity;         // ����ʵ��
		STATION_NO   station;             // վ�Ż�gpc��
		RECV_STATUS  recv_status;         // ����״̬
		I_TIME       recv_time;           // ����/ǩ��ʱ��
		char         sign_name[32];

		char         chaosong_memo[128];   // ����ע��
	};

	short       recv_count;              // ������Ŀ,stpc or GPC
	RECV_LIST   recv_list[128];          // �����б�,stpc or GPC

	short       chaosong_recv_count;     // ������Ŀ,ֻ��GPC����
	RECV_LIST   chaosong_list[128];      // �����б�

	enum   SEND_WAY                      // ���ͷ�ʽ
	{
		SEND_INTELLIGENT,                  // ���ܷ���
		SEND_IMMEDIATELY                   // ��������
	} send_way;

	short  train_list_count;             // ���ߵ��������Ŀ�����
	short  index;                        // �����ɻ����ã���ǰ�յ������ߵ��������б����������0��ʼ���
	struct TRAIN_LIST
	{
		ENTITY_ID   recv_lirc;            // ����ʵ��
		RECV_STATUS recv_status;          // ����״̬
		I_TIME      recv_time;            // ����/ǩ��ʱ��
		char        sign_name[32];        // ǩ����

		TRAIN_NO    train_id;             // ���Σ� for wireless
		TRAIN_NO    locomotive;           // ������,for wireless
		STATION_NO  station;              // ����վ��
	}train_list[32];

	friend class MsgDCmdData;
};

DECLARE_SERIALIZE_OPERATOR(MsgDCmdSendList)

class AFX_EXT_CLASS MsgDCmdContent : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdContent)

public:
	MsgDCmdContent();
	virtual ~MsgDCmdContent();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// ��������Ψһʶ��
	ENTITY_ID   send_entity;    // ����ʵ��
	I_TIME      create_time;    // ����ʱ��

	short       content_seq;    // �������ݰ�˳��, ��1��ʼ
	char        content[2048];  // �������ݰ�����

	friend class MsgDCmdData;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdContent)

class AFX_EXT_CLASS MsgDCmdData : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdData)

public:
	MsgDCmdData();
	virtual ~MsgDCmdData();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	MsgDCmdTitle       title;            // �������
	MsgDCmdSendList    sendlist;         // ������б�
	MsgDCmdContent     first_package;    // ��һ����������, content_seq��Ϊ0

	short  tcc_len;
	char   tcc_total[512];    // ��MsgDCmdTcc Serialize д��

	ISDWORD   content_len;       // ����һ�������ĳ���
	short     content_count;     // ����һ�������ķְ�����

	friend class MsgDCmdInsert;
	friend class MsgDCmdSend;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdData)

class AFX_EXT_CLASS MsgDCmdInsert : public MsgDCmdData
{
	DECLARE_CLBTH(MsgDCmdInsert)

public:
	MsgDCmdInsert();
	virtual ~MsgDCmdInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:


};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdInsert)

class AFX_EXT_CLASS MsgDCmdDelete : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdDelete)

public:
	MsgDCmdDelete();
	virtual ~MsgDCmdDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID   con_id;//����̨��
	ENTITY_ID   send_entity;//�����ʵ��
	I_TIME        create_time;//�����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdDelete)


class AFX_EXT_CLASS MsgDCmdTitleLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTitleLoad)

public:
	MsgDCmdTitleLoad();
	virtual ~MsgDCmdTitleLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//����̨��
	I_TIME        begin_time;//��ʼʱ��
	I_TIME        end_time;//����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitleLoad)

class AFX_EXT_CLASS MsgDCmdLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdLoad)

public:
	MsgDCmdLoad();
	virtual ~MsgDCmdLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//����ʵ��
	I_TIME        create_time;//����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdLoad)

class AFX_EXT_CLASS MsgDCmdLoad2 : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdLoad2)

public:
	MsgDCmdLoad2();
	virtual ~MsgDCmdLoad2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//�����ʵ��
	I_TIME        create_time;//�����ʱ��
	I_TIME        send_time;//�����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdLoad2)
class AFX_EXT_CLASS MsgDCmdSend : public MsgDCmdData
{
	DECLARE_CLBTH(MsgDCmdSend)

public:
	MsgDCmdSend();
	virtual ~MsgDCmdSend();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:


};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdSend)

class AFX_EXT_CLASS MsgDCmdRecv : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRecv)

public:
	MsgDCmdRecv();
	virtual ~MsgDCmdRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// ��������Ψһʶ��
	ENTITY_ID   send_entity;    // ����ʵ��
	I_TIME      create_time;    // ����ʱ��

	// ���ߵ�������
	TRAIN_NO    train_id;       // ����
	TRAIN_NO    locomotive;     // ������
	STATION_NO  station;        // վ��

	I_TIME      recv_time;      // ���ջ�ǩ��ʱ��
	ENTITY_ID   recv_entity;    // ���ջ�ǩ��ʵ��

	enum RECV_STATUS
	{
		COMMAND_RECV              = 0,// �������
		COMMAND_SIGN              = 1,// ����ǩ��
		COMMAND_LOCOMOTIVE_RECV   = 2,// ��������
		COMMAND_LOCOMOTIVE_SIGN   = 3,// ˾��ǩ��
		COMMAND_SENDED            = 4,// ���ɻ��ѷ���
		COMMAND_ENTITY_DEAD       = 5,// ʵ������Ӧ
		COMMAND_ERROR             = 6,// ����
		COMMAND_DENY              = 7,// �ܾ�
		COMMAND_INIT              = 8,// ��ʼ��
		COMMAND_DEV_RECV          = 9,// ����ת������ִ

		COMMAND_ACK               = 21, // ͬ��
		COMMAND_READ              = 22, // ���Ķ�
		COMMAND_NACK              = 23, // ��ͬ��
		COMMAND_SEND_FAIL         = 24, // �����ʧ��
		COMMAND_SEND_FAIL_LOCAL   = 25, // ��վ����ʧ��
		COMMAND_WAIT_SEND         = 26  // �ȴ����ͻ���
	} recv_status;

	char    sign_name[32];  //ǩ������
	char    cmdno[16];      //�������
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRecv)

class AFX_EXT_CLASS MsgDCmdStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdStatus)

public:
	MsgDCmdStatus();
	virtual ~MsgDCmdStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	MsgDCmdTitle       title;//�������
	MsgDCmdSendList    sendlist;//������б�

	short  tcc_len;//tcc����
	char   tcc_total[512];    // ��MsgDCmdTcc Serialize д��

};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdStatus)

class AFX_EXT_CLASS MsgDcmd_WirelessBroadcast:public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_WirelessBroadcast)

public:
	MsgDcmd_WirelessBroadcast();
	virtual ~MsgDcmd_WirelessBroadcast();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char broadcast_id[128];   //����Ǳ���Ⱥ����Ӧ�ĵ�������Id���ڷ�����Ϣ��ҲҪ���������ͻ����������Ҷ�Ӧ��������Ը�����״̬
	char dcmd_id[128];//uuid
	I_TIME gen_time;
	IBYTE is_startnow;      //�Ƿ�������ʼ��
	IBYTE is_neverstop;     //�Ƿ�������Ч��
	I_TIME plan_start_time;   //�ƻ���ʼʱ�䣬����������ʼ��Ϊ1ʱ���˴�Ϊ0
	I_TIME plan_end_time;     //�ƻ�����ʱ�䣬����������Ч��Ϊ1ʱ���˴�Ϊ0

	IWORD station_id;

	//  char station_name[128];
	enum DIRECTION
	{
		DOWN_DIRECTION  = 1,  //1������
		UP_DIRECTION,     //2������
		ALL_DIRECTION,      //3��������
	}
	direction;
	enum STATUS
	{
		EDIT,       //  0��δ�·�
		SEND,       //  1�����������ɻ��������ڵȴ�״̬
		BEGIN_BROADCAST,    //  2������ʼȺ��
		END_BROADCAST,    //  3������ֹȺ��
		CANCEL        //  4��������ȡ��
	}
	status;
	enum TRAINTYPE
	{
		PASSENGER_CAR = 1,  //1:�ͳ�
		FREIGHT_WAGGON,     //2:����
		ALL_TRAIN,        //3:�ͻ���
	}
	train_type;

	I_TIME real_start_time;//�����Ŀ�ʼʱ��
	I_TIME real_end_time;//�����Ľ���ʱ��

	friend class MsgDcmd_BroadCast_Send;
	friend class MsgDcmd_BroadCast_Command;
};

class AFX_EXT_CLASS MsgDcmd_BroadCast_Send : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Send)

public:
	MsgDcmd_BroadCast_Send();
	virtual ~MsgDcmd_BroadCast_Send();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum { MAX_SEND_COUNT = 16 };
	MsgDcmd_WirelessBroadcast wbInfo[MAX_SEND_COUNT];
	IBYTE  nSendDestCount;
	IBYTE  nUsedIndex;

	struct DCMD
	{
		char dcmd_id[128];//uuid
		char dcmd_style_title[128];//title
		char dcmd_num[16];//cmdno
		char org_cmd_num[16];//

		ENTITY_ID send_entity;//����ʵ��
		I_TIME      create_time;//�����ʱ��
		char    creater_name[32];//�����������
		I_TIME    send_time;//�����ʱ��
		I_TIME    edit_time;//����༭ʱ��

		IBYTE   sender_type;//����������
		IDWORD    sender_id;//������id
		char    sender_name[32];//����������
		char    sender_unit_name[32];//
		char    rereader_name[32];//
		char    dutychief_name[32];//
		I_TIME    author_time;//
		IBYTE   author_status;//
		char    recver[512];//������

		//��������ߵ�������
		enum CMD_TYPE
		{
			DISPATCH_COMMAND = 0x01, // ��������
			ROUTE_TICKET,          // ·Ʊ
			GREEN_ADMISSION,         // ��ɫ���֤
			RED_ADMISSION,           // ��ɫ���֤
			SPY_ON_SHUNT_NOTE,       // ��վ���ٵ���֪ͨ��
			USE_LIGHT_CAR_ADMISSION, // �����г�ʹ��˵����
			FORETELL_TRAIN_ROUTE,    // �г���·Ԥ����Ϣ
			JUST_LINE_PASS,          // ����ͨ����
			BY_LINE_PASS,            // ����ͨ����
			JUST_LINE_SLOW_PASS,     // ���߻���ͨ����
			ENTER_JUST_LINE_STOP,    // ��վ����ͣ����
			ENTER_BY_LINE_STOP,      // ��վ����ͣ����
			SHUNTING_JOB = 0x11,     // ������ҵ
			SHUNTING_REQUEST_RECEIVED = 0x12, // �����������յ�
			IN_OUT_CHECK = 0x20,              // �������
			OTHER_COMMAND,                    // ��������
		}
		cmd_type;

		enum CMD_DESTINATION
		{
			GPC_2_DIRECOR_ON_DUTY,
			DIRECOR_ON_DUTY_2_GPC,
			GPC_2_GPC,
			GPC_2_STPC,
			GPC_2_LIRC,
			GPC_2_STPC_AND_LIRC, //û���õ���ʵ���Ϸ����η��͵�
			STPC_2_LIRC,
			STPC_2_GPC,          //������������
		}
		cmd_destination;

		enum CMD_OPER
		{
			NORMAL_CMD,          // ������������
			WORK_CMD,            // ʩ����������
			TRANSFER_CMD,    // ת������������ں��벻��ʱʹ��
			REQUEST_CMD          // �����������
		}
		cmd_oper;

		IBYTE need_author;
		int     tcc_len;

		enum   SEND_WAY                        // ���ͷ�ʽ
		{
			SEND_INTELLIGENT,                  // ���ܷ���
			SEND_IMMEDIATELY                   // ��������
		}
		send_way;

		IWORD status;
		char    jhh[64];    // ʩ���ƻ���
		char        content[2048];  // �������ݰ�����
	}
	dcmdcontent;
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Send)

class AFX_EXT_CLASS MsgDcmd_BroadCast_Status : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Status)

public:
	MsgDcmd_BroadCast_Status();
	virtual ~MsgDcmd_BroadCast_Status();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char broadcast_id[128];   //����Ǳ���Ⱥ����Ӧ�ĵ�������Id���ڷ�����Ϣ��ҲҪ���������ͻ����������Ҷ�Ӧ��������Ը����������ʾ״̬
	char dcmd_id[128];      //�����
	ENTITY_ID station_entity;   //��վ ͨ��ʵ���
	char station_name[128];   //վ�������п��ޡ�
	enum DIRECTION
	{
		DOWN_DIRECTION  = 1,  //1������
		UP_DIRECTION,     //2������
		ALL_DIRECTION,      //3��������
	}
	direction;  //����
	IWORD status;       //ִ��״̬������Ҫ�������ɻ����صĽ����ˢ�µģ�0��δ�·� 1��
	I_TIME oper_time;//����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Status)

class AFX_EXT_CLASS MsgDcmd_BroadCast_Command : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Command)

public:
	MsgDcmd_BroadCast_Command();
	virtual ~MsgDcmd_BroadCast_Command();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	//  MsgDcmd_WirelessBroadcast wbInfo;
	char broadcast_id[128];   //����Ǳ���Ⱥ����Ӧ�ĵ�������Id���ڷ�����Ϣ��ҲҪ���������ͻ����������Ҷ�Ӧ��������Ը����������ʾ״̬
	char dcmd_id[128];
	IWORD commandid;              //0Ϊ��ѯ��ǰִ��״̬��1Ϊ������ʼ ��2Ϊ������ֹ
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Command)

class AFX_EXT_CLASS MsgDCmdCurrentNoRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdCurrentNoRequest)

public:
	MsgDCmdCurrentNoRequest();
	virtual ~MsgDCmdCurrentNoRequest();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	// ��������Ψһʶ��
	ENTITY_ID   send_entity;    // ����ʵ��
	I_TIME        create_time;    // ����ʱ��

	enum CMD_OPER
	{
		NORMAL_CMD,          // ������������
		WORK_CMD             // ʩ����������
	} cmd_oper;

	enum FLAG
	{
		EDIT,//�༭
		SEND//����
	} flag;

	CONID       conid;//����̨��

	friend class MsgDCmdCurrentNo;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest)

class AFX_EXT_CLASS MsgDCmdCurrentNoRequest2 : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdCurrentNoRequest2)

public:
	MsgDCmdCurrentNoRequest2();
	virtual ~MsgDCmdCurrentNoRequest2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// ��������Ψһʶ��
	ENTITY_ID   send_entity;    // ����ʵ��
	I_TIME        create_time;    // ����ʱ��

	enum CMD_OPER
	{
		NORMAL_CMD,          // ������������
		WORK_CMD             // ʩ����������
	} cmd_oper;

	enum FLAG
	{
		EDIT,//�༭
		SEND//����
	} flag;

	CONID       conid;//����̨��

	char   begin_cmdno[16];//��ʼ�����
	char   end_cmdno[16];//���������

	friend class MsgDCmdCurrentNo;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest2)

class AFX_EXT_CLASS MsgDCmdCurrentNo : public MsgDCmdCurrentNoRequest
{
	DECLARE_CLBTH(MsgDCmdCurrentNo)

public:
	MsgDCmdCurrentNo();
	virtual ~MsgDCmdCurrentNo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char   cmdno[16];//�����
	I_TIME   send_time;//����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNo)

class AFX_EXT_CLASS MsgDCmdRefresh : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRefresh)

public:
	MsgDCmdRefresh();
	virtual ~MsgDCmdRefresh();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID      conid;//����̨��

	ENTITY_ID  send_entity;//����ʵ���
	I_TIME       create_time;//��������ʱ��

	I_TIME       ltime;//
	ENTITY_ID  source;//Դʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRefresh)

class AFX_EXT_CLASS MsgDCmdTitleClose : public MsgDCmdTitleLoad
{
	DECLARE_CLBTH(MsgDCmdTitleClose)
public:
	MsgDCmdTitleClose();
	virtual ~MsgDCmdTitleClose();
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitleClose)

class AFX_EXT_CLASS MsgDCmdRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRequest)

public:
	MsgDCmdRequest();
	virtual ~MsgDCmdRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char   cmdno[16];//�����
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRequest)

class AFX_EXT_CLASS MsgDCmdAnswer : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdAnswer)

public:
	MsgDCmdAnswer();
	virtual ~MsgDCmdAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	char   cmdno[16];//�����
	short  status;   //�����ִ״̬
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdAnswer)


//��ҪӦ�����Ϣ
class AFX_EXT_CLASS MsgGpcMessage: public BaseMsg
{
	DECLARE_CLBTH(MsgGpcMessage)
public:
	MsgGpcMessage();
	virtual ~MsgGpcMessage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ISDWORD      label;     //��Ϣ��ʾ��

	ENTITY_ID source_id;//Դʵ���
	ENTITY_ID dest_id;//Ŀ��ʵ���

	enum ACTION_TYPE {
		Msg_REQUEST = 0,   //������Ϣ
		Msg_REPLY          //��Ӧ��Ϣ
	};
	enum ANSWER_TYPE {
		ANSWER_OK = 0,     //ȷ��
		ANSWER_DENAY       //�ܾ�
	};
	IBYTE cmd_type;//��������
	IBYTE answer_type;//������

	I_TIME      tm_request; //����ʱ��
	I_TIME      tm_reply;   //��Ӧʱ��

	ISDWORD      spare;

	short pack_len;       //��Ϣ����
	char  package[1024];  //��Ϣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcMessage)

// ֱ���������
class AFX_EXT_CLASS MsgDirectOutput: public BaseMsg
{
	DECLARE_CLBTH(MsgDirectOutput)
public:
	MsgDirectOutput();
	virtual ~MsgDirectOutput();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	bool AddOutput(IWORD handle,IWORD flag);
	int  GetCount();
	bool GetOutput(int index,IWORD &handle,IWORD &flag);
private:
	int   nOutputHandleCount;
	int   nOutputHandles[16];
	int   nOutputHandleChecks[16];  // ����У��
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult2)

//��������2
class AFX_EXT_CLASS MsgDataStyleReq2:public BaseMsg
{
	DECLARE_CLBTH(MsgDataStyleReq2)

public:
	MsgDataStyleReq2();
	virtual ~MsgDataStyleReq2();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	enum DataStyle
	{
		DISPLAY   = 0x0001,
		RENTID    = 0x0002,
		REPORTTIME  = 0x0004,
		BITBLOCK  = 0x0008,
		LIRCSTATUS  = 0x0010,
		SCHD_TRAIN  = 0x0020,
		SCHD_BLOCK  = 0x0040,
		HSTY_TRAIN  = 0x0080,
		HSTY_BLOCK  = 0x0100,
		PREV_TRAIN  = 0x0200,
		PREV_BLOCK  = 0x0400,
		BASC_TRAIN  = 0x0800,
		BASC_BLOCK  = 0x1000,
		SHIFT_TRAIN = 0x2000,
		SHIFT_BLOCK = 0x4000,

		SHUNT         = 0x00008000,
		MMPLAN      = 0x00010000,
		BASIC_ARCHIVE = 0x00020000,
		SCHEDULE_NAME = 0x00040000,
		TRAINLOG      = 0x00080000,

		//    WHOLE_TRAIN  =  0x0020,
		//    WHOLE_BLOCK  =  0x0040,

		WHOLE_TRAIN  =  0x00100000,
		WHOLE_BLOCK  =  0x00200000,
		WBASIC_TRAIN =  0x00400000,
	};
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};

	IBYTE nStatCount;//״̬��
	STATION_NO nStno[LOGIN_MAX_STATION_COUNT];//վ��
	IDWORD      nStyle[LOGIN_MAX_STATION_COUNT];//
	ReqStatus  nStatus[LOGIN_MAX_STATION_COUNT];//״̬

	I_TIME  nStartTime;
	I_TIME  nEndTime;
};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq2)

//---------------------------- RBC ��Ϣ----------------------------------------------------------------

// RBC�г���Ϣ
class AFX_EXT_CLASS MsgRBCTrainInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTrainInfo)
public:
	MsgRBCTrainInfo();
	virtual ~MsgRBCTrainInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD    nUniqueID;      // RBC�ṩ���г�Ψһʶ���
	IWORD   nRBCID;       // �г�����RBC���
	char      strRBCName[32];   // RBC����
	TRAIN_NO  tnTrainNo;      // ���κ�
	IWORD   nTrainLen;      // �г�����
	IWORD   nMaxSpeed;      // ����ٶ�(km/h)

	IWORD   nTrainMode;     // ������Ӫģʽ
	// 0:��ȫ���; 1:Ŀ��; 2:Ա������; 3:����; 4:����; 5:����; 6:����
	// 7:ð��8:ð����13:STM  15:������
	IBYTE   nTrainLevel;      // CTCS����
	// 0:0��; 1:STM��; 2:1��; 3:2��; 4:3��; 0xFF:������

	IWORD   nSpeed;       // �ٶ�(km/h)

	IWORD   nPositionStatus;    // λ����Ч��
	// 0:δ֪; 1:��Ч; 2:����; 3:��Ч; 4:ERTMS����֮��

	IWORD   nEmergencyStop; // ����ͣ��
	// 0:��;  1:��
	IDWORD    nRequestAct;    // Ԥ��
	IBYTE   nDir;       // �г����з���0:δ֪ 1:����, 2:����

	enum { DEV_NAME_LEN = 16 };
	struct DEVINFO
	{
		IWORD   nID;
		STATION_NO  nSta;
		char      strName[DEV_NAME_LEN];
	};

	DEVINFO   sTrainPos;      // �г�λ��

	IDWORD    nTrainOffset;   // ���Ϊ��ǰ���źŻ�����
	// ֣��Ϊ���ǰ�˹����
	DEVINFO   sEOA;       // �ƶ���Ȩ�յ㣬
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTrainInfo)

// �г�����(������ɾ��)
class AFX_EXT_CLASS MsgRBCTrainOperation: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTrainOperation)
public:
	MsgRBCTrainOperation();
	virtual ~MsgRBCTrainOperation();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IDWORD    nUniqueID;  // RBC�ṩ���г�Ψһʶ���
	enum { TRN_QUERY, TRN_REMOVE };
	IBYTE   nOper;    // ��ѯ��ɾ��
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTrainOperation)

// RBC������Ϣ
class AFX_EXT_CLASS MsgRBCAlarm: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCAlarm)
public:
	MsgRBCAlarm();
	virtual ~MsgRBCAlarm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD   nAlarmGroup;        // �������
	IWORD   nAlarmCode;     // ��������
	I_TIME    nAlarmTime;     // ʱ��
	IDWORD    nMilliSeconds;      // ʱ��(����)
	IWORD   nPriority;        // �ȼ�
	IWORD   nAlarmState;      // ״̬
	char      strSite[64];        // ����������
	char      strObject[64];      // ��������
	char      strContent[256];      // ��������
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCAlarm)

// RBC����״̬����
class AFX_EXT_CLASS MsgRBCStatusReport : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCStatusReport)
public:
	MsgRBCStatusReport();
	virtual ~MsgRBCStatusReport();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int nCount; // RBC����
	enum { MAX_RBC_COUNT = 64 };
	struct
	{
		IWORD nRBCID;//RBC��
		WORK_STATUS nStatus;//״̬
	} szStatus[MAX_RBC_COUNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCStatusReport)

//�ı���Ϣ
class AFX_EXT_CLASS MsgRBCTxt : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTxt)
public:
	MsgRBCTxt();
	virtual ~MsgRBCTxt();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IWORD   nOperID;    // ������
	ENTITY_ID nSender;    // ������ߵ�entity
	TRAIN_NO  tnTrain;    // ���κ�
	IDWORD    nNIDEngine;   // �г�ID��C3Ψһ��ʶ��
	char        strTxt[128];    //�ı���Ϣ

	//YB.P add @2011-07-11
	int nConfirm;
	int nDispTime;
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTxt)

// ����ͣ�������
class AFX_EXT_CLASS MsgRBCEmergencySend : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCEmergencySend)
public:
	MsgRBCEmergencySend();
	virtual ~MsgRBCEmergencySend();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IWORD   nOperID;    // ������
	ENTITY_ID nSender;    // ������ߵ�entity
	TRAIN_NO  tnTrain;    // ���κ�
	IDWORD    nNIDEngine; // �г�ID��C3Ψһ��ʶ��
	I_TIME    tmSend;   // �����ʱ��
	IBYTE   nFlag;    // ������ǣ�0x55�����ý�����0xAA��ȡ��������
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCEmergencySend)

// ����ͣ������ͻ�ִ
class AFX_EXT_CLASS MsgRBCEmergencyResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCEmergencyResult)
public:
	MsgRBCEmergencyResult();
	virtual ~MsgRBCEmergencyResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD   nOperID;    // ������
	ENTITY_ID nSender;    // ������ߵ�entity
	TRAIN_NO  tnTrain;    // ���κ�
	IDWORD    nNIDEngine; // �г�ID��C3Ψһ��ʶ��
	I_TIME    tmSend;   // �����ʱ��
	IBYTE   nFlag;    // ������ǣ�0x55�����ý�����0xAA��ȡ��������
	IBYTE   nStatus;    // ��ִ���������£�
	enum
	{
		CTC_RECV=0,     // RBCͨ�Ż��յ�����
		CTC_SEND,     // RBCͨ�Ż��ѷ�������
		RBC_RECV,     // RBC�ѽ�������յ��ɹ��ķ�����
		RBC_REJECT,   // RBC����������յ����ɹ�������
		CMD_OK,     // ������ִ�У��г�״̬�ı�
		CMD_TIMEOUT,  // ���ʱ���г�״̬δ�ı�
		CMD_NOTSEND   // ���������㣬���ܷ���
	};
	char  strResult[256];   // ע���ַ�����������ʾ���Э������ɹ�ʱ�ķ�����Ϣ
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCEmergencyResult)

//-------------------------------------------------------------------------------------------------------------------

class AFX_EXT_CLASS MsgParamentRequest:public BaseMsg
{
	DECLARE_CLBTH(MsgParamentRequest)

public:
	MsgParamentRequest();
	virtual ~MsgParamentRequest();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	ENTITY_ID  entity;     // INVALID_ENTITY_ID for ALL
	int        scope;
	short      type_count;
	int        type[4];
	enum PARA_TYPE
	{ 
		MAINTN,
		WINTG , 
		LAYOUT,
		DCMD  
	} ;
};
DECLARE_SERIALIZE_OPERATOR(MsgParamentRequest)

class AFX_EXT_CLASS MsgParamentReply:public MsgParamentRequest
{
	DECLARE_CLBTH(MsgParamentReply)

public:
	MsgParamentReply();
	virtual ~MsgParamentReply();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	int        int_parament[4];
	char       str_parament[4][2048];

	/* 
	TYPE: MAINTN
	int_parament:
	bit0 :  Ȩ�޷���Ȩ��

	TYPE: WINTG 
	int_parament:
	bit0: ����ͼ��ѯȨ��
	bit1: �����߱༭Ȩ��
	bit2: ʩ�����ű༭Ȩ��
	bit3: ����ͼ�޸�Ȩ��
	bit4: ����ͼ����Ȩ��
	bit5: ʵ��ͼ�޸�Ȩ��*
	bit6: �����հ�ƻ�Ȩ��
	bit7: �¼ƻ�Ȩ��

	*/

};
DECLARE_SERIALIZE_OPERATOR(MsgParamentReply)

class AFX_EXT_CLASS MsgParamentOperation: public MsgParamentReply
{
	DECLARE_CLBTH(MsgParamentOperation)

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgParamentOperation();
	virtual ~MsgParamentOperation();

public:
	enum Operation {
		PARAOPER_SERVER_ECHO_OK,
		PARAOPER_SERVER_ECHO_FAIL,
		PARAOPER_INSERT_UPDATE,
		PARAOPER_DELETE
	}                 oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgParamentOperation)

// ���õ������Ա��������������Ϣ
class AFX_EXT_CLASS MsgSetDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgSetDDyName)

public:
	MsgSetDDyName();
	virtual ~MsgSetDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;    //����̨id
	SHIFT_ID   shift_id; //�հ�ƻ�id
	USERNAME   user_name;//�û���
};
DECLARE_SERIALIZE_OPERATOR(MsgSetDDyName)


class AFX_EXT_CLASS MsgGetDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgGetDDyName)

public:
	MsgGetDDyName();
	virtual ~MsgGetDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;//����̨id
	SHIFT_ID   shift_id;//�հ�ƻ�id
};
DECLARE_SERIALIZE_OPERATOR(MsgGetDDyName)

class AFX_EXT_CLASS MsgGiveDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgGiveDDyName)

public:
	MsgGiveDDyName();
	virtual ~MsgGiveDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;//����̨id
	SHIFT_ID   shift_id;//�հ�ƻ�id
	USERNAME   user_name1;//�û���1
	USERNAME   user_name2;//�û���2
};
DECLARE_SERIALIZE_OPERATOR(MsgGiveDDyName)

class AFX_EXT_CLASS MsgReportTimeTransfer :public MsgReportTime
{
	DECLARE_CLBTH(MsgReportTimeTransfer)
public:
	MsgReportTimeTransfer();
	MsgReportTimeTransfer(const MsgReportTime& msg);
	virtual ~MsgReportTimeTransfer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO transfer_sta;//ת����վ
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTimeTransfer)

class AFX_EXT_CLASS MsgStringMessage:public BaseMsg
{
	DECLARE_CLBTH(MsgStringMessage)

public:
	MsgStringMessage();
	virtual ~MsgStringMessage();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

	IDWORD  len;//��Ϣ����
	char * pcontent;//

private:
	char token[128];//��ʶ
	char content[4096];//����

	void expand(IDWORD new_len);

public:
	I_BOOL SetTokenContent(const char *sToken,const char* sText, int nTextLength=-1);
	const char* GetToken()  const;
	const char* GetContent() const;
	IDWORD GetContentLength() const;
};
DECLARE_SERIALIZE_OPERATOR(MsgStringMessage)

class AFX_EXT_CLASS MsgStringMessage2 : public BaseMsg
{
	DECLARE_CLBTH(MsgStringMessage2)

public:
	MsgStringMessage2();
	virtual ~MsgStringMessage2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	char token[128];//��ʾ
	char content[4096];//����

	char sysid[128];//
	IDWORD package_index;//����
	IDWORD package_count;//����
	IDWORD totallength;//�ܳ�
	//����Ϣ����
	IDWORD currentlength;//��ǰ����
	char *pcontent; //
	I_TIME packagetime;//
public:
	I_BOOL SetTokenContent(const char *sToken,const char* sText);
	I_BOOL SetPackageInfo(const char *sSysid,IDWORD nCount,IDWORD nIndex,IDWORD nLength);
	const char* GetToken() const;
	const char* GetContent() const;

	const char* GetSysid() const;
	IDWORD GetPackageIndex() const;
	IDWORD GetPackageCount() const;
	IDWORD GetTotalLength() const;

	I_BOOL IsSamePackage(const MsgStringMessage2* msgSM2) const;
	I_TIME GetPackageTime() const;
	ISDWORD SelfPackage();
	ISDWORD Package(const MsgStringMessage2* msgSM2);
};
DECLARE_SERIALIZE_OPERATOR(MsgStringMessage2)


//�����ն���������
class AFX_EXT_CLASS MsgDataStyleReq3:public MsgDataStyleReq2
{
	DECLARE_CLBTH(MsgDataStyleReq3)

public:
	MsgDataStyleReq3();
	virtual ~MsgDataStyleReq3();

	CONID nConId[LOGIN_MAX_STATION_COUNT];

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq3)

//�鵵����ͼ��������
class AFX_EXT_CLASS MsgDataStyleReq4:public MsgDataStyleReq2
{
	DECLARE_CLBTH(MsgDataStyleReq4)

public:
	MsgDataStyleReq4();
	virtual ~MsgDataStyleReq4();

	SCHEDULE_ID nScheduleId[LOGIN_MAX_STATION_COUNT];

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq4)
// �г�Ԥ����Ϣ
class AFX_EXT_CLASS MsgTrainDepartPredict : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDepartPredict)

public:
	MsgTrainDepartPredict();
	virtual ~MsgTrainDepartPredict();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO nSourceStno;   // ������վ
	STATION_NO nDestStno;   // ����Ŀ�공վ
	ENTRY_ID nDestEntry;    // Ŀ�공վ�ӳ����
	TRAIN_NO strTid;      // ���κ�
	TRAIN_INDEX nTrainIndex;  //��������
	IBYTE nRecIndex;//��������

	ENTITY_ID nSrcEntity;   // ����Ԥ���ʵ���

	enum { LIRC, STPC, GPC };
	IBYTE nSrcType;       // ����Ԥ���ʵ������

	enum
	{
		YUGAO,    // ����Ԥ��
		SHOUDAO,    // �յ�����Ԥ��
		TONGYI,   // ͬ�ⷢ��
		QUXIAO    // ȡ������
	};
	IBYTE nOperType;        // Ԥ������

	I_TIME nOperTime;       // Ԥ��ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDepartPredict)

// �����豸����
class AFX_EXT_CLASS MsgCISFIRAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgCISFIRAlarm)

public:
	MsgCISFIRAlarm();
	virtual ~MsgCISFIRAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO nStno;                // ��վ��
	enum { MAX_FIR_LEN = 512 };
	IBYTE nzFIRInfo[MAX_FIR_LEN];    // ������Ϣ
	IWORD nFIRLen;                   // ������Ϣ����
};
DECLARE_SERIALIZE_OPERATOR(MsgCISFIRAlarm)


//�����·��Ϣ�����ɻ�����
class AFX_EXT_CLASS MsgTrackCircuit : public BaseMsg
{
	DECLARE_CLBTH(MsgTrackCircuit)

public:
	MsgTrackCircuit();
	virtual ~MsgTrackCircuit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { TRACKCIRCUIT_COUNT = 30 };
	IBYTE nzTrackCircuitInfo[TRACKCIRCUIT_COUNT];//�����·��Ϣ
	IWORD nTrackcircuitLen;//�����·����
};
DECLARE_SERIALIZE_OPERATOR(MsgTrackCircuit)

// ����ͼ�޸��Դ�
class AFX_EXT_CLASS MsgStPlanAutoTrigUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgStPlanAutoTrigUpdate)

public:
	MsgStPlanAutoTrigUpdate();
	virtual ~MsgStPlanAutoTrigUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID          con_id;
	ENTITY_ID      gpc;           //gpcʵ���
	TRAIN_INDEX    train_index;   // �г�����
	IBYTE          rec_index;     // ��¼����
	STATION_NO     station;       // ��վվ��

	enum { ARRIVE, DEPART };      //�������
	IBYTE run_type;               //��������
	IBYTE trig_flag;              // 0:���Դ�, 1: �Դ�
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdate)

// ����ͼ�޸��Դ���ִ
class AFX_EXT_CLASS MsgStPlanAutoTrigUpdateResult : public BaseMsg
{
	DECLARE_CLBTH(MsgStPlanAutoTrigUpdateResult)

public:
	MsgStPlanAutoTrigUpdateResult();
	virtual ~MsgStPlanAutoTrigUpdateResult();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID          con_id;
	TRAIN_INDEX    train_index;                     // �г�����
	IBYTE           rec_index;                       // ��¼����
	STATION_NO     station;                         // ��վվ��

	enum { ARRIVE, DEPART };  //������
	IBYTE run_type;
	IBYTE trig_flag;    // 0:���Դ�, 1: �Դ�

	enum { PLAN_UPDATED, PLAN_NOT_FOUND };//�ƻ����¡��ƻ�������
	IBYTE plan_res;

	enum { ROUTE_UPDATED, ROUTE_NOT_UPDATE }; //��·����,��·������
	IBYTE route_res;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdateResult)

// ������ҵ��������Ϣ
// ����״̬�仯
class AFX_EXT_CLASS MsgShuntStatusUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntStatusUpdate)

public:
	MsgShuntStatusUpdate();
	virtual ~MsgShuntStatusUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // վ��
	SHUNT_INDEX nShuntIndex; // ������ҵ��Ψһ���
	IWORD nGouIndex; // �����
	IBYTE nStatus; // ״̬
	enum
	{
		NOTE_NOT_START,
		NOTE_START,
		NOTE_FINISH,
		GOU_NOT_START,
		GOU_START,
		GOU_FINISH
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntStatusUpdate)

// ��������
class AFX_EXT_CLASS MsgShuntTimeReport : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntTimeReport)

public:
	MsgShuntTimeReport();
	virtual ~MsgShuntTimeReport();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // վ��
	SHUNT_INDEX nShuntIndex; // ������ҵ��Ψһ���
	IWORD nGouIndex; // �����
	IBYTE nTimeType; // ��������
	enum
	{
		NOTE_FINISH,
		GOU_FINISH
	};
	IBYTE nOperType; // ��������
	enum
	{
		MANUAL_REPORT,  //�ֹ�����
		AUTO_REPORT //�Զ�����
	};
	I_TIME tmRptTime; // ����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntTimeReport)

// ��������
class AFX_EXT_CLASS MsgShuntRouteGroupRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntRouteGroupRequest)

public:
	MsgShuntRouteGroupRequest();
	virtual ~MsgShuntRouteGroupRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // վ��
	ROUTE_LIST_HANDLE nHandle; // ������ҵ��Ψһ���
	IWORD nGouIndex; // �����
	I_TIME tmReqTime; // ����ʱ��
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntRouteGroupRequest)


// �����Ų�����������ӡ�ɾ�����޸�
class AFX_EXT_CLASS MsgShuntLocoNoOper : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntLocoNoOper)

public:
	MsgShuntLocoNoOper();
	virtual ~MsgShuntLocoNoOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //վ��
	DEVICE_TYPE nDmisType;  //��Ҫ�ǵ���͹ɵ�
	DMIS_ID   nDmisNo;  //dmis��
	IBYTE   nOldLen;  //�ϵ����ų���
	TRAIN_NO  tnOldTrain; //�ϵ�����
	IBYTE   nNewLen;  //�µ����ų���
	TRAIN_NO  tnNewTrain; //�µ�����
	IDWORD    nIPIndex; //IP����
	IBYTE   nLocoType;  //����������
	IBYTE   reserve[11];  //Ԥ��

	char*   GetLocoDesc();
	char*   SetLocoDesc(const char *psz, unsigned short nLen);
private:
	char    *pszLocoDesc;//����������
	unsigned short    nLocoDescBufLen;//��������������
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntLocoNoOper)


/**************************************************************************/
/*-------------------------------�ƶ���Ȩ��Ϣ-----------------------------*/
/**************************************************************************/


/********************************Ȩ������**********************************/
class AFX_EXT_CLASS MsgRMRequest:public BaseMsg
{
	DECLARE_CLBTH(MsgRMRequest)
public:
	MsgRMRequest();
	virtual ~MsgRMRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //�����Ŀ��ʵ���
	ENTITY_ID nSrcID;   //�����ߵ�ʵ���
	char strSrcName[32];  //�������ն��豸����
	char strUserName[32]; //�������û���
	char strPassword[32]; //����������
};
DECLARE_SERIALIZE_OPERATOR(MsgRMRequest)

/******************************Ȩ��������********************************/
class AFX_EXT_CLASS MsgRMResult:public BaseMsg
{
	DECLARE_CLBTH(MsgRMResult)
public:
	MsgRMResult();
	virtual ~MsgRMResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //�����Ŀ��ʵ��
	ENTITY_ID nSrcID; //�����ߵ�ʵ���
	IBYTE nResult;  //������
	enum  //�����б�
	{
		RQ_OK,  //����ɹ�
		RQ_REJECT,  //�����߾ܾ�
		RQ_NO_OWNER,  //�޹�����
		RQ_LOCKED,  //�����ʵ���ѱ���һ�ն�����
		RQ_INVALID, //�û�������У�����
		RQ_NO_ENTITY  //����ʵ����Ч
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRMResult)

/********************************Ȩ��֪ͨ**********************************/
class AFX_EXT_CLASS MsgRMReport:public BaseMsg
{
	DECLARE_CLBTH(MsgRMReport)
public:
	MsgRMReport();
	virtual ~MsgRMReport();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //Ȩ�޵�Ŀ��ʵ���
	ENTITY_ID nSrcID; //ӵ��Ȩ���ߵ�ʵ��ʵ���
	char strSrcName[32]; //Ȩ��ӵ�����ն��豸����
	char strUserName[32]; //Ȩ��ӵ�����û���
	I_TIME tmRight; //Ȩ�޷����ȡ��ʱ��
	IBYTE nResult; //Ȩ��ʹ����/ȡ����־
	enum
	{
		RT_INIT, //Ȩ�޳�ʼ��ͨ��
		RT_REQUEST,  //Ȩ��������
		RT_USING,  //Ȩ��ʹ����
		RT_RELEASE //Ȩ��ȡ��
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRMReport)

/******************************Ȩ�޹���ע��********************************/
class AFX_EXT_CLASS MsgRMRegister:public BaseMsg
{
	DECLARE_CLBTH(MsgRMRegister)
public:
	MsgRMRegister();
	virtual ~MsgRMRegister();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nzDestIDs[256]; //�������Ŀ��ʵ�����
	int nDestCount;   //�������Ŀ��ʵ�����
	ENTITY_ID nControllerID; //�����ն�ʵ���
};
DECLARE_SERIALIZE_OPERATOR(MsgRMRegister)

/********************************��װ��Ϣ**********************************/
class AFX_EXT_CLASS MsgRMOperPack:public BaseMsg
{
	DECLARE_CLBTH(MsgRMOperPack)
public:
	MsgRMOperPack();
	virtual ~MsgRMOperPack();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IBYTE nzMsgBuf[4096]; //��Ϣ�ֽ���
	int nMsgLen;  //��Ϣ����
	ENTITY_ID nDestID; //Ŀ��ʵ��
};
DECLARE_SERIALIZE_OPERATOR(MsgRMOperPack)

/********************************ʵ�����**********************************/
class AFX_EXT_CLASS MsgDestEntityUpdate:public BaseMsg
{
	DECLARE_CLBTH(MsgDestEntityUpdate)
public:
	MsgDestEntityUpdate();
	virtual ~MsgDestEntityUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nzEntityAdd[256]; //��������ʵ���б�
	int nEntityAddCount;   //��������ʵ�����
	ENTITY_ID nzEntityRemoved[256]; //ɾ������ʵ���б�
	int nEntityRemovedCount; //ɾ������ʵ�����
};
DECLARE_SERIALIZE_OPERATOR(MsgDestEntityUpdate)

// ����ͼ��������
class AFX_EXT_CLASS MsgBasicScheduleSet : public BaseMsg
{
	DECLARE_CLBTH(MsgBasicScheduleSet)

public:
	MsgBasicScheduleSet();
	virtual ~MsgBasicScheduleSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID nConId;//����̨���κ�
	SCHEDULE_ID nScheduleId;//����ͼid
	I_TIME        nStartTime, nEndTime;//��ʼʱ��,����ʱ��
	I_TIME  nSetNameTime, nEstimateStartTime;//����ͼ��������ʱ�䡢���ƿ�ʼʱ��
	ENTITY_ID nSetNameEntity;//����ͼ��������ʵ���
	char nScheduleName[MAX_SCHEDULE_NAME_LEN];//����ͼ����
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleSet)


// ----------------------- IP����״̬��Ϣ -----------------------------------------------

// �г�λ���ƶ�
class AFX_EXT_CLASS IPMsgTrainMove : public BaseMsg
{
	DECLARE_CLBTH(IPMsgTrainMove)

public:
	IPMsgTrainMove();
	virtual ~IPMsgTrainMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;              // ����ʱ��
	TRAIN_NO  szPrevTrain;    // ԭ���κ�
	IDWORD    nPrevIndex;       // ԭ�г�IP����
	STATION_NO  nPrevSt;      // ԭ�г����δ�λ��
	DMIS_ID   nPrevWin;       //ǰһ�����δ���
	TRAIN_NO  szNextTrain;    // �ֳ��κ�
	IDWORD    nNextIndex;       // ���г�IP����
	STATION_NO  nNextSt;      // ���г����δ�λ��
	DMIS_ID   nNextWin; //��һ�����δ���
	IBYTE   nOperLvl;   // �г������ȼ�(0:TMP, 1:Radio, 2:STPC, 4:Dispatch)
	IBYTE   nTrainLvl;    // �г����еȼ�(1:�ؿ�,2:��ͨ�ͳ�,3:����)
	IWORD   nFlag;      // �г������־(bit5: ����������־)
};
DECLARE_SERIALIZE_OPERATOR(IPMsgTrainMove)

// ��·����
class AFX_EXT_CLASS IPMsgRouteStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgRouteStatus)

public:
	IPMsgRouteStatus();
	virtual ~IPMsgRouteStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // ����ʱ��
	STATION_NO  nStation;         // վ��
	TRAIN_NO  szTrain;            // ���κ�
	IDWORD    nTrainIndex;        // ����IP����
	IBYTE   nRouteType;       // ��·���ͣ�1:�ӳ�2:����3:ͨ����
	ENTRY_ID  nEntry;         // �ӳ���
	ENTRY_ID  nExit;          // ������
	SIDE_ID   nzSides[MAX_SIDE_COUNT_IN_ROUTE]; // ;���ɵ�
	IBYTE   nSideCount;       // �ɵ�����
	IDWORD    nRouteFlag;       // ��·�����־(bit0:�г�������־, bit1:��·������־, bit2:��·��·������־)
	IBYTE   nMinSwitch;       // ��·;����С�޲��
	IBYTE   nMinDypassSwitch;   // ��·;��������С�޲��
	enum { ROUTE_SET, ROUTE_CANCEL01 };//��·����,��·ȡ��
	IBYTE   nRouteStatus;     // ��·����ȡ����־
};
DECLARE_SERIALIZE_OPERATOR(IPMsgRouteStatus)

// �ɵ�ʹ��״̬(ռ��/����/��ȫ����ɵ�)
class AFX_EXT_CLASS IPMsgSideStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgSideStatus)

public:
	IPMsgSideStatus();
	virtual ~IPMsgSideStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);

protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // ����ʱ��
	STATION_NO  nStation;         // վ��
	TRAIN_NO  szTrain;            // ���κ�
	IDWORD    nTrainIndex;        // ����IP����
	SIDE_ID   nSide;              // �ɵ���
	enum { SIDE_OCC, SIDE_CLEAR, SIDE_WHOLE_OCC };
	IBYTE   nSideStatus;  //�ɵ�״̬
};
DECLARE_SERIALIZE_OPERATOR(IPMsgSideStatus)

// �г���վ
class AFX_EXT_CLASS IPMsgTrainStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgTrainStatus)

public:
	IPMsgTrainStatus();
	virtual ~IPMsgTrainStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);

protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // ����ʱ��
	STATION_NO  nStation;         // վ��
	TRAIN_NO  szTrain;            // ���κ�
	IDWORD    nTrainIndex;        // ����IP����
	enum { TRAIN_ENTER, TRAIN_EXIT };
	IBYTE nTrainStatus;   // ����վ��־
	ENTRY_ID  nEntry;             // ��վ��
	SIDE_ID   nSide;          // Ŀ��ɵ�
};
DECLARE_SERIALIZE_OPERATOR(IPMsgTrainStatus)


// ----------------------- �ۺ�ά����Ϣ -----------------------------------------------

// ���Բ�ѯ
class AFX_EXT_CLASS CmsMsgAttrRequest : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgAttrRequest)

public:
	CmsMsgAttrRequest();
	virtual ~CmsMsgAttrRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // ̽��ID
	IDWORD nDetectDev;  // ̽�ⷢ����ID
	ENTITY_ID nDestID;  // Ŀ���豸ʵ���
	IBYTE nABFlag;    // Ŀ���豸AB����־
	IBYTE nAttrType;    // �������
	IDWORD nAttrGroupID;  // �������ݿ�ID����Ӧ��attributes��������
	short nAttrIndex; // ��������
	IWORD nReplyPort; // �������͵�UDP�˿�
	ENTITY_ID nPeerID;  // ͨ�ŶԷ�ʵ�壨���ڶ�̬���в�ѯ��
	IBYTE nPeerABFlag;  // ͨ�ŶԷ�AB����־�����ڶ�̬���в�ѯ��
	char strAttrName[20]; // �������������Զ������ԣ�
	char strPara[128];    // ��ѯ�����������Զ������ԣ�
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgAttrRequest)

// ����ע��
class AFX_EXT_CLASS CmsMsgSvrRegister : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgSvrRegister)

public:
	CmsMsgSvrRegister();
	virtual ~CmsMsgSvrRegister();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nSvrID; // CMSSʵ���
	ENTITY_ID nDestID;  // Ŀ���豸ʵ���
	IBYTE nABFlag;    // Ŀ���豸AB����־
	IWORD nReplyPort; // �������͵�UDP�˿�
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgSvrRegister)

// ������Ӧ��
class AFX_EXT_CLASS CmsMsgSingleAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgSingleAttrReply)

public:
	CmsMsgSingleAttrReply();
	virtual ~CmsMsgSingleAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // ̽��ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	IDWORD nDetectDev;  // ̽�ⷢ����ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	ENTITY_ID nSrcID; // �豸ʵ���
	IBYTE nABFlag;    // �豸AB����־
	IBYTE nAttrType;    // �������
	IDWORD nAttrGroupID;  // ���������ݿ�ID����Ӧ��attributes��������
	short nAttrIndex; // ��������
	IDWORD nAttrSingleID;// ������ID����Ӧ��attr_conn/attr_capacity��������
	IDWORD nValue;    // ����ֵ��������0������1������2��δ֪/�жϣ�/����/����/ʱ�䣩
	IDWORD nMaxValue; // �������ֵ���������������ԣ�
	IBYTE nzUserValue[800]; // �û��Զ�������ֵ
	short nUserValueLen;  // �û��Զ�������ֵ����
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgSingleAttrReply)

// ������Ӧ��
class AFX_EXT_CLASS CmsMsgGroupAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgGroupAttrReply)

public:
	CmsMsgGroupAttrReply();
	virtual ~CmsMsgGroupAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // ̽��ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	IDWORD nDetectDev;  // ̽�ⷢ����ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	ENTITY_ID nSrcID; // �豸ʵ���
	IBYTE nABFlag;    // �豸AB����־
	IBYTE nAttrType;    // �������
	IDWORD nAttrGroupID;  // �������ݿ�ID����Ӧ��attributes��������
	IDWORD nValue[200]; // ����ֵ������/������
	short nValueStart;  // ����ֵ��ʼ����
	short nValueCount;  // ����ֵ�����Ա��
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgGroupAttrReply)

// ����������Ӧ��
class AFX_EXT_CLASS CmsMsgCapaAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgCapaAttrReply)

public:
	CmsMsgCapaAttrReply();
	virtual ~CmsMsgCapaAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // ̽��ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	IDWORD nDetectDev;  // ̽�ⷢ����ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	ENTITY_ID nSrcID; // �豸ʵ���
	IBYTE nABFlag;    // �豸AB����־
	IDWORD nAttrGroupID;  // �������ݿ�ID����Ӧ��attributes��������
	IDWORD nValue[100]; // ����ֵ
	IDWORD nMaxValue[100];  // �������ֵ
	short nValueStart;  // ����ֵ��ʼ����
	short nValueCount;  // ����ֵ�����Ա��
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgCapaAttrReply)

// ��̬��������Ӧ��
class AFX_EXT_CLASS CmsMsgQueueAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgQueueAttrReply)

public:
	CmsMsgQueueAttrReply();
	virtual ~CmsMsgQueueAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // ̽��ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	IDWORD nDetectDev;  // ̽�ⷢ����ID�����Ʋ�ѯ��Ϣ�ж�Ӧ�ֶΣ�
	ENTITY_ID nSrcID; // �豸ʵ���
	IBYTE nABFlag;    // �豸AB����־
	IDWORD nAttrGroupID;  // �������ݿ�ID����Ӧ��attributes��������
	IDWORD nAttrSingleID;// ������ID����Ӧ��attr_capacity��������
	enum { MAX_QUEUE_COUNT = 60 };
	struct
	{
		ENTITY_ID nPeerID;  // ����ͨ�ŶԷ�ʵ���
		IBYTE nPeerABFlag;  // �Է�AB����־
		short nIndex;   // ����Indedx
		IDWORD nValue;    // ����ʹ����
		IDWORD nMaxValue; // �����������ֵ
	} szQueue[MAX_QUEUE_COUNT];
	IBYTE nQueueCount;    // �����������
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgQueueAttrReply)

// ����
class AFX_EXT_CLASS CmsMsgAlarm : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgAlarm)

public:
	CmsMsgAlarm();
	virtual ~CmsMsgAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nSrcID; // �豸ʵ���
	IBYTE nABFlag;    // �豸AB����־
	I_TIME tmAlarm;   // ��������ʱ��
	IBYTE nLevel;   // ��������
	IBYTE nType;      // �������
	IBYTE nAttrType;    // ��������������
	IDWORD nAttrGroupID;  // �������������ݿ�ID
	short nAttrIndex; // �����ǵ���������
	char strAlarm[256]; // ��������
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgAlarm)

// �ͻ��˼���豸ע��
class AFX_EXT_CLASS CmsMsgClientDevRegister : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgClientDevRegister)

public:
	CmsMsgClientDevRegister();
	virtual ~CmsMsgClientDevRegister();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nClientID;  // CMSCʵ���
	IWORD nReplyPort;   // ��������״̬��UDP�˿ں�
	enum { MAX_DEV_COUNT = 200 };
	struct
	{
		ENTITY_ID nDevID; // ����豸ʵ���
		IBYTE nABFlag;    // ����豸AB����־
	} szDevices[MAX_DEV_COUNT];
	IBYTE nDevCount;      // ע���豸����
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgClientDevRegister)


class AFX_EXT_CLASS MsgHILCTransmit:public BaseMsg
{
	DECLARE_CLBTH(MsgHILCTransmit)

public:
	MsgHILCTransmit();
	virtual ~MsgHILCTransmit();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	char token[32];     //��ʶ

	IDWORD  hilc_len;    //HILC���ݳ���
	char    hilc_content[256];//HILC����
};
DECLARE_SERIALIZE_OPERATOR(MsgHILCTransmit)

class AFX_EXT_CLASS MsgChangeSideReq:public BaseMsg
{
	DECLARE_CLBTH(MsgChangeSideReq)

public:
	MsgChangeSideReq();
	virtual ~MsgChangeSideReq();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	STATION_NO station;
	IWORD count;
	TRAIN_KEY train_index[256];
    IBYTE rec_index[256];
	SIDE  arrive_side[256];
	SIDE  depart_side[256];
};
DECLARE_SERIALIZE_OPERATOR(MsgChangeSideReq)

class AFX_EXT_CLASS MsgChangeSideAck:public BaseMsg
{
	DECLARE_CLBTH(MsgChangeSideAck)

public:
	MsgChangeSideAck();
	virtual ~MsgChangeSideAck();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	IWORD result;
};
DECLARE_SERIALIZE_OPERATOR(MsgChangeSideAck)

CLBTH_NAMESPACE_END

#endif


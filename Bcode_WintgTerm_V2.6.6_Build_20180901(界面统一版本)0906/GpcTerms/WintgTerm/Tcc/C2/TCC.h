
/*
 * TCC�ͻ���(CTC-GPC/STPC, DMIS-CPC/STPC)�ӿڶ���
 * 
 * ������Ҫ�ӿ�: 
 *     ITccClientInfoProvider--�ͻ�����Ϣ�ṩ��
 *     ITccAdviseSink--�ͻ��˰�װTCC�¼�(���͡�ȡ����������)
 *
 * ������Ҫ�ṹ:
 *     TCC_LIMITSPEED_CMD--������������
 *     TCC_STATUS--�п����ĵ�״̬
 *
 * Mar 28, 2006, wangshug
 * Copyright (c) 2006 CASCO LTD.
 * 
 */

/*
 * TCC�ṩȱʡ����������Ų���(��command-id.xml), һ�㽨��ͻ�
 * ����д�������ز���
 * 
 * ���ɻ����ص���������״̬����ת��ΪTCC�������Ӧ״̬
 * 
 */

#ifndef __TCC_H__
#define __TCC_H__

#pragma warning( disable:4786 )

#pragma once

#import <msxml4.dll> //raw_interfaces_only
#include <vector>

#pragma pack(push, 8)
namespace TCC {
#undef AFX_EXT_API  // CUIHU!!!
#define AFX_EXT_API 

	//Check if MSXML 4.0 is installed
	bool isMSXMLInstalled();

	class ITccClientInfoProvider;
	class ITccAdviseSink;

	typedef unsigned short stano_t;	//վ������
	typedef unsigned short cmdid_t;	//�п����������
	typedef unsigned short lineid_t;	//�п���·������

	typedef UUID sysid_t;

	//���������
	struct  kmmark_t {
		DWORD val : 24;	//����
		DWORD km_cl: 6;	//���� ������ChainLength
		DWORD m_cl : 2;	//��   ������ChainLength

	public:
		kmmark_t(DWORD dw = 0);
		kmmark_t& operator=(const kmmark_t& rhs);
		DWORD GetDWORD();
		
		bool operator!=(const kmmark_t& rhs) const;
		bool operator==(const kmmark_t& rhs) const;
		bool operator<(const kmmark_t& rhs) const;
		bool operator<=(const kmmark_t& rhs) const;
		bool operator>(const kmmark_t& rhs) const;
		bool operator>=(const kmmark_t& rhs) const;

		CString GetKcl() const;
		CString GetMcl() const;

		CString toString() const;
		long DistanceTo(const kmmark_t& rhs) const;
	} ;


	//�������������
	typedef struct __DCMD_ID {
		char sz[17];

		__DCMD_ID(const char *lpsz = NULL){ memset(sz, 0, sizeof(sz)); if(lpsz) strncpy(sz, lpsz, sizeof(sz) -1); }
		__DCMD_ID(const __DCMD_ID& rhs){ operator=(rhs); }
		__DCMD_ID& operator=(const __DCMD_ID& rhs){ memcpy(sz, rhs.sz, sizeof(sz)); return *this;}
		operator const char* () const { return sz; }
	} dcmd_id_t;
	AFX_EXT_API inline bool operator==(const dcmd_id_t& lhs, const dcmd_id_t& rhs){ return strncmp(lhs.sz, rhs.sz, sizeof(lhs.sz)) == 0; }
	AFX_EXT_API inline bool operator!=(const dcmd_id_t& lhs, const dcmd_id_t& rhs){ return !operator==(lhs, rhs); }

	enum dir_t {
		DIR_UNKNOWN = 0,
		DIR_DOWN = 1,
		DIR_UP   = 2,
		DIR_DUAL = 3,
	};

	// sysid�Ƚ�
	AFX_EXT_API bool operator==(sysid_t& lhs, sysid_t& rhs);
	AFX_EXT_API bool operator!=(sysid_t& lhs, sysid_t& rhs);

	// sysid����
	AFX_EXT_API void SysidCreate(sysid_t *sysid);
	AFX_EXT_API void SysidCreateNil(sysid_t *sysid);

	// sysid���ַ����໥ת��
	AFX_EXT_API CString SysidToString(sysid_t);
	AFX_EXT_API sysid_t SysidFromString(CString str);

	// sysid���ֽ������໥ת��
	AFX_EXT_API int SysidToByteArray(const sysid_t& sysid, BYTE arr[], int num);
	AFX_EXT_API int SysidFromByteArray(sysid_t& sysid, const BYTE arr[], int num);
	// ����״̬
	enum {
		CONFIRM   = 0x0001,	// ȷ��״̬
		EXECUTING = 0x0002,	// ִ����
		CANCELED  = 0x0003,	// ��ȡ��
	};

	struct  TCC_COMMAND;
	struct  STOCK_COMMAND;
	struct  RAW_STOCK_COMMAND;
	
// �����ɻ����������������ʽ
struct  TCC_COMMAND {
	WORD		nConsoleID;		//����̨ ̨��
	sysid_t		sys_id;			//�ڲ���ʶ
	stano_t		nStaID[2];		//��ʼ������վ
	lineid_t	nLineID;	//��·
	BOOL		bFullLimit;	//�Ƿ�ȫ������
	kmmark_t	nKmMark[2];		//ʼĩ�����(����Ϊ��λ)
	BOOL		bCancelLimit;	//�Ƿ�ȡ������

	cmdid_t		nCmdID;		//�����(�пر���)
	WORD		nSpeedMaxVal;	//����ֵ
	BOOL		bStartNow;		//�Ƿ�������ʼ
	BOOL		bEndless;		//�Ƿ�������Ч
	time_t		nTime[2];		//��ʼ������ʱ��

	stano_t		nTccID;		//����վ(�пر���)
	lineid_t	nTccLineID;	//����վ��·��(�пر���)
	DWORD		nTccSenderID;	//����վ������

	TCC_COMMAND();
	bool operator==(const TCC_COMMAND& rhs);
	bool operator!=(const TCC_COMMAND& rhs);

	//���һ��RAW_STOCK_COMMAND
	void ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd);
	CString toString(ITccClientInfoProvider *pInfoProvider);
};

// �����������ȷ�������ʽ
struct  STOCK_COMMAND {
	dcmd_id_t	dcmd_id;	//���������

	//��Ϊ���ҵ�������ʱ��Ҫ�������������, ���������ѭ��ʹ�õ�, ������Ϊ����ʹ��
	DWORD	  dcmd_send_entity;	//������������
	time_t	  dcmd_create_time;

	WORD		nConsoleID;	//����̨ ̨��
	sysid_t		sys_id;		//�ڲ���ʶ
	cmdid_t		nCmdID;		//�����
	stano_t		nStaID[2];		//��ʼ������վ
	lineid_t	nLineID;		//��·
	CString		szTrainID;		//���κ�
	WORD		nSpeedMaxVal;	//����ֵ

	BOOL		bFullLimit;	//�Ƿ�ȫ������
	kmmark_t	nKmMark[2];		//ʼĩ�����(����Ϊ��λ)
	BOOL		bStartNow;		//�Ƿ�������ʼ
	BOOL		bEndless;		//�Ƿ�������Ч
	time_t		nTime[2];		//��ʼ������ʱ��

	BOOL		bCancelLimit;	//�Ƿ�ȡ������

	DWORD		nCreatorID;	//������
	DWORD		nCheckorID;	//����У����
	DWORD		nConfirmorID;	//ȷ�Ϸֽ���
	sysid_t		raw_sysid;		//ԭʼ�����ʶ
	time_t		nCreateTime;	//�����ʱ��

	struct _TCC_STRUCT {
		stano_t		nID;		//����վ
		lineid_t	nTccLineID;//��·
		DWORD		nCheckorID;//��վУ����(����վ��ȷ�Ϸֽ���)
		WORD		nStatus;	//����վ����״̬
		DWORD		nSenderID;	//����վ�������
		time_t		nTime;		//����վ������Чʱ��
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			nStatus = 0;
			nSenderID = 0;
			nTime = 0;
		}
	};
	std::vector<_TCC_STRUCT> tcc;//����վ�б�

	STOCK_COMMAND();
	bool operator==(const STOCK_COMMAND& rhs);
	bool operator!=(const STOCK_COMMAND& rhs);

	WORD GetTccStatus(stano_t nTccID);
	DWORD GetTccSender(stano_t nTccID);

	BOOL GetTccCommand(TCC_COMMAND *pcmd, stano_t nTccID);
	// �ֽ�Ϊ�п���� ����ֵΪ��Ч������� 
	INT  Decompose(TCC_COMMAND cmd[], INT num);
	//���һ��RAW_STOCK_COMMAND
	void ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd);

	CString toString(ITccClientInfoProvider *pInfoProvider);
};

// ���ر����ԭʼ��������
struct  RAW_STOCK_COMMAND
{
	dcmd_id_t	dcmd_id;	//���������

	//BYTE     nCmdType;	//��������  1=STATION_LIMIT 2=ABSAPP_LIMIT 3=ABSAPP_FULL_LIMIT 4=KM_LIMIT
	//BYTE     nDirection;//���ٷ���  1���� 2���� 3˫��

    WORD		nConsoleID;		//����̨ ̨��
	sysid_t		raw_sysid;	//ԭʼ�����ʶ
	cmdid_t		nCmdID;	//�����
	stano_t		nStaID[2];	//��ʼ������վ
	lineid_t	nLineID;	//��·

	BOOL		bFullLimit;//�Ƿ�ȫ������
	kmmark_t	nKmMark[2];		//ʼĩ�����(����Ϊ��λ)
	BOOL		bStartNow;	//�Ƿ�������ʼ
	BOOL		bEndless;	//�Ƿ�������Ч
	time_t		nTime[2];	//��ʼ������ʱ��

	BOOL		bCancelLimit;	//�Ƿ�ȡ������

	DWORD		nCreatorID;		//������
	DWORD		nCheckorID;		//����У����
	DWORD		nConfirmorID;	//ȷ�Ϸֽ���
	time_t		nCreateTime;	//�����ʱ��

	struct _TCC_STRUCT {
		stano_t		nID;			//����վ
		lineid_t	nTccLineID;	//��·
		DWORD		nCheckorID;	//��վУ����(����վ��ȷ�Ϸֽ���)
		BYTE		bTccValidate;	//����վ��У���
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			bTccValidate = 1;
		}
	} ;
	std::vector<_TCC_STRUCT> tcc;//����վ�б�

	struct _TRAINCMD {
		sysid_t sys_id;			//ÿ�˳�һ�����һ��ϵͳ��ʶ
		CString szTrainID;		//���κ�
		WORD	nSpeedMaxVal;	//����ֵ
		WORD	nSpeed;			//����ֵ
        
        //ÿ�˳����������վ״̬
		struct _TCC_STRUCT {
            WORD	nStatus;	//����վ����״̬
            DWORD	nSenderID;	//����վ�������
			time_t	nTime;		//����վ������Чʱ��
		} ;
		std::vector<_TCC_STRUCT> tcc;

		_TRAINCMD();
	} train_cmd[5];

	BOOL bGenCancelCommand;
	RAW_STOCK_COMMAND();
	RAW_STOCK_COMMAND& operator=(const RAW_STOCK_COMMAND& rhs);
	bool operator==(const RAW_STOCK_COMMAND& rhs);
	bool operator!=(const RAW_STOCK_COMMAND& rhs);

    BOOL SetStatus(sysid_t& sysid, stano_t nTccID, WORD nStatus);
    BOOL SetSender(sysid_t& sysid, stano_t nTccID, DWORD nSenderID);
    BOOL SetTime(sysid_t& sysid, stano_t nTccID, DWORD nTime);
    BOOL SetTccCheckor(stano_t nTccID, DWORD nCheckor);
    
    // �ֽ�ΪSTOCK_COMMAND�� ����ֵΪ��Ч�������
    int Decompose(STOCK_COMMAND cmd[], int num);
	CString toString(ITccClientInfoProvider *pInfoProvider);
};

//enum {
//	TCC_UNKNOWN            = 0x0000,	// δ֪
//
//	LIRC_RECEIVED          = 0x0001,	// ���ɻ��յ�
//	LIRC_SEND_FAIL         = 0x0002,	// ���ɻ�����ʧ��(Э��֡����)
//	TCC_RECEIVED           = 0x0003,	// �п��յ�(������ȷ)
//	TCC_ERR_KM             = 0x0004,	// �����Ƿ�
//	TCC_ERR_SPEED          = 0x0005,	// ���ٷǷ�
//	TCC_ERR_REGION_LONG    = 0x0006,	// ����̫��
//	TCC_ERR_REGION_EXIST   = 0x0007,	// �����Ѵ���
//	TCC_ERR_REGION_INVALID = 0x0008,	// ����Ƿ�
//	TCC_ERR_DUP_CMDNO      = 0x0009,	// ������ظ�
//	TCC_ERR_TIME_FAIL      = 0x000a,	// ʱ��У��ʧ��
//
//	TCC_ACCEPT             = 0x000b,    // �пؽ��ܣ���ȷ��
//	TCC_WAITFOR_EXEC       = 0x000c,	// �Ŷӣ��ȴ�ִ��
//	TCC_EXECUTING          = 0x000d,	// ����ִ��
//	TCC_ZOMBIE             = 0x000e,	// ִ�н���(����ʱ�䵽)
//	TCC_CANCELED           = 0x000f,	// ��ȡ��(�˹�, ��������)
//	
//	LOC_WAITFOR_CANCEL     = 0x0010,	// �ȴ�ȡ��
//	LOC_EDIT               = 0x0011,	// �༭����
//	LOC_SENDED             = 0x0012,	// �����������·�
//	LOC_SEND_FAIL          = 0x0013,	// ���������·���
//
//	TCC_CONFIRM_OK         = 0x0014,	// ��ȷ��
//	TCC_CONFIRM_CANCEL     = 0x0015,	// ��ȡ��
//	TCC_CONFIRM_TIMEOUT    = 0x0016,	// ��ʱ��δȷ��
//};

// Interface
class  ITccInterface
{
public:
	ITccInterface();
	virtual ~ITccInterface();
	void AddRef();
	void Release();
private:
	LONG nRefCount;
};

// Out Interface
// �ͻ��˱�������ʵ�ָýӿ�
class  ITccAdviseSink : public ITccInterface
{
public:
	// ��һ���п����ķ�������������ͽ���ɺ���ֵ����(0:�ɹ���1:ʧ��)
	virtual int OnSendTccCommand(TCC_COMMAND& cmd, HWND hTccDlg) = 0;
	virtual int OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir, HWND hTccDlg) = 0;
	// �������������б�
	virtual int OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg) = 0;

	// ��ʵʱ�б���ȡ������
	virtual int OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg) = 0;
	// ��ȷ���б���ȡ������
	virtual int OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg) = 0;

	// ����ȷ������
	virtual int OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTddDlg) = 0;
	// ɾ��ĳ����վȷ������
	virtual int OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg) = 0;

	virtual INT OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag) = 0;
	virtual int OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag) = 0;

	virtual int OnTccCommandFileChanged(
					LPCSTR szFilename, 
					DWORD dwOper) = 0;

	// Ԥ��
	virtual int OnTccCommandStatusChanged(
					TCC_COMMAND &cmd,
					DWORD status,
					DWORD dwParam) = 0;

	enum {
		SELF_CONFIRM_COMMAND_ID,
		OTHER_CONFIRM_COMMAND_ID,
	};
	virtual BOOL ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg);
};

// �ͻ��˱�������ʵ�ָýӿ�����TCC�ṩ������Ϣ
class  ITccClientInfoProvider : public ITccInterface
{
public:
	// TCC����Ŀ¼(��../data/DDML/tcc), ���ڴ��Tccԭʼ����
	virtual CString GetTccDataDirectory() = 0;
	// TCC����Ŀ¼(��../data/tcc), ���ڴ��Tcc����������
	virtual CString GetTccConfigDirectory() = 0;

	// �õ���վ�б�����ֵΪʵ��ȡ�õ��б���Ŀ
	virtual void GetStationList(std::vector<stano_t>& staList) = 0;
	// ����վ�ŵõ�վ��
	virtual CString GetStationName(stano_t nStaID) = 0;

	// �õ����λ�ͷ�����
	virtual CString GetOrgSender() = 0;
	virtual CString GetPersonSender() = 0;

	virtual WORD  GetConsoleID() = 0;

	// Absline��Ŷ�ÿһ������������վ��˵����Ψһ��

	// �õ�����վ��Absline�б�
	//virtual void GetAbslineList(
	//				stano_t nStaID, 
	//				stano_t nStaID2, 
	//				std::vector<lineid_t>& nAbslineID) = 0;
	// ����Absline��ŵõ�����������
	//virtual CString GetAbslineName(
	//					stano_t nStaID, 
	//					stano_t nStaID2, 
	//					lineid_t nAbslineID) = 0;
	// ����վ�ź�Absline��ŵõ�����������
	//virtual CString GetAbslineName(
	//					stano_t nStaID, 
	//					lineid_t nAbslineID) = 0;

	// �ж�nStaID��nStaID2 �Ƿ�����վ
	virtual BOOL IsLinkedStation(
						stano_t nStaID, 
						stano_t nStaID2) = 0;

	// վnStaID2�����վnStaID�����䷽����(���з���, ���з���)
	virtual dir_t GetStationDirection(
						stano_t nStaID, 
						stano_t nStaID2) = 0;
	// �õ�վnStaID������վ�б�
	virtual void GetLinkedStationList(
						stano_t nStaID, 
						std::vector<stano_t>& staList) = 0;
	// ����վ��, Tcc��·�źͷ���(������)�õ�����վ
	virtual stano_t GetLinkedStation(
						stano_t nStaID, 
						lineid_t nTccLineID,
						dir_t   dir) = 0;


		virtual void GetTccBlockLineList(stano_t nStaID, std::vector<lineid_t>& lines) = 0;
		virtual CString GetTccBlockLineName(stano_t nStaID, lineid_t nTccLineID) = 0;
		virtual BOOL IsTccBlockLine(stano_t nStaID, lineid_t nTccLineID) = 0;

	virtual void GetTccLineList(stano_t nStaID, std::vector<lineid_t>& lines) = 0;
	// ȡ��վnStaID��TccLine�б�����վnStaID2��������ЩTccLine
	virtual void GetTccLineList(
					stano_t nStaID,
					stano_t nStaID2,
					std::vector<lineid_t>& lines) = 0;
	// ȡ��վnStaID����վnStaID2����·nTccLineID2��ƥ���TccLine
	virtual lineid_t GetTccLine(
					stano_t nStaID,
					stano_t nStaID2,
					lineid_t nTccLineID2) = 0;
	// ȡ��վnStaID��nTccLineID������
	virtual CString GetTccLineName(stano_t nStaID, lineid_t nTccLineID) = 0;
	// ȡ��Ŀ��վnStaID��TCC��·nTccLineID��������
	// DIR_UP ��������
	// DIR_DOWN ��������
	// DIR_DUAL ����������
	// DIR_UNKNOWN ��·�Ų�����
	virtual dir_t GetTccLineAbsNum(stano_t nStaID, lineid_t nTccLineID) = 0;
	// ȡ��TccLine�ķ���ֵ(�����л�˫��)
	virtual dir_t GetTccLineDirection(stano_t nStaID, lineid_t nTccLineID) = 0;

	// ȡ���ڲ���·��
	virtual int GetLineID(TCC::stano_t nTccID, TCC::lineid_t nTccLineID) = 0;
	// ���ݹ����ȡ�����վ
	virtual std::vector<TCC::stano_t> GetTccList(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd) = 0;

	// У��վ�ŵĺϷ���
	virtual BOOL ValidateStationID(stano_t nStaID) = 0;

	virtual BOOL IsStationListChanged() = 0;

	// �õ������������ֵ��Χ
	virtual BOOL  IsValidCommandID(cmdid_t nCmdID);
	virtual cmdid_t GetInvalidCommandID();
	virtual cmdid_t GetMinCommandID();
	virtual cmdid_t GetMaxCommandID();
	virtual cmdid_t GetCommandID();
	virtual void    NextCommandID();
	virtual cmdid_t SaveCommandID(cmdid_t nCommandID);

	virtual bool IsTCCReadOnly()=0;
	virtual bool IsTCCExecute()=0;
	virtual bool IsCTCGPC()=0;
	
};


enum {
	STATIONLIST_CHANGED,
	LOC_COMMAND_STORE_CHANGED,
};
// �п���������������
AFX_EXT_API HWND TccDialogOpen();
AFX_EXT_API void TccDialogHide();
AFX_EXT_API void TccDialogClose();
AFX_EXT_API void TccDialogUpdate(DWORD dwReason);
AFX_EXT_API HWND TccDialog(
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink, 
		CWnd *pParent, 
		LPCSTR szCaption = NULL);

// ע�� ע���п���Ϣ
void RegisterTCCMsgProcHandleWnd();
void UnRegisterTCCMsgProcHandleWnd();

/*
  �п�״̬
 */
struct  TCC_STATUS {
	BYTE comm_active;	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
	BYTE stcc_master[2];	// �п�����״̬
	BYTE ilock_channel[2];	// ����ͨ��״̬
	BYTE ctc_channel[2];	// CTCͨ��״̬

	BYTE leu_channel[4];	// LEU ͨ��״̬
	BYTE leu_ack[16];	// LEUӦ����״̬, һ��leu��4��Ӧ����
	BYTE init_section_num;	// ��ʼ���������(���Ϊ8)
	lineid_t init_line_no[8];	// ����������·��
	dir_t    init_direction[8]; // ��Ӧ����·����
	TCC_STATUS() { memset(this, 0, sizeof(TCC_STATUS)); }
};
/*
  ��ʾ�пر����״̬
 */
//AFX_EXT_API 
//BOOL Receive_TccReport(DWORD nTccID, TCC_STATUS& status);

/*
  ���п�������ʾ��ʵʱ�����б���
  Input:
     cmd �п�ʵʱ����
	 nStatus Ԥ��
  Output:
     TRUE �ɹ���ʾ
	 FALSE �ܾ���ʾ(����վ�Ų�һ��)
 */
//AFX_EXT_API
//BOOL Receive_TccCommand(TCC_COMMAND& cmd, DWORD nStatus);

/*
  ��ʾ�·��п�����ʱ�Ĳ���״̬
  Input:
     cmd �´���п�����
	 nStatus ���ɻ�������п�״̬(ĿǰԤ��)
	 strComment �����������
 */
//AFX_EXT_API
//void Receive_TccCommandResult(
//	TCC_COMMAND& cmd, 
//	DWORD nStatus, 
//	LPCSTR strComment);

/*
  ��ʾ�п���·���ٳ�ʼ�������Ľ��
  Input:
     nTccID վ��
	 nTccLineID ��·��
	 dir ��·����(���з���, ���з���)
	 nStatus Ԥ��
	 strComment ��ʼ���������
 */
//AFX_EXT_API
//void Receive_TccLineInitialCommandResult(
//	stano_t nTccID, 
//	lineid_t nTccLineID, 
//	dir_t dir, 
//	DWORD nStatus, 
//	LPCSTR strComment);


// ���������ص�ȷ�������б�
//AFX_EXT_API BOOL Receive_StockCommand(STOCK_COMMAND& cmd);
//AFX_EXT_API BOOL Receive_HistoryCommand(STOCK_COMMAND& cmd);

/*
  ԭʼ������������
  Input:
     pcmd �ṩ��������ķ��ؿռ�
	 pInfoProvider �ṩ�ͻ��˲���
	 pParent ������
  Output:
	 TRUE �ɹ�����
	 FALSE ��������
     pcmd �����������������
 */
AFX_EXT_API
BOOL RawStockCommandInput(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);

/*
  ԭʼ������������
  Input:
     pcmd ����������������
	 pInfoProvider �ṩ�ͻ��˲���
	 pParent ������
  Output:
	 TRUE �ɹ�����
	 FALSE ��������
     pcmd ���������������
 */
AFX_EXT_API
BOOL RawStockCommandReEdit(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);

/*
  ԭʼ��������У��
  Input:
     pcmd ��У�����������
	 pInfoProvider �ṩ�ͻ��˲���
	 pParent ������
  Output:
	 TRUE У��ͨ��
	 FALSE У��ʧ��
 */
AFX_EXT_API
BOOL RawStockCommandValid(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);


/*
  ԭʼ����������ļ�����
  һ���ļ���ֻ��ȡһ��ԭʼ����,���Ա����ļ�ʱ����ע���ļ���
  ������ź�SYSID�Ķ�Ӧ
 */ 
AFX_EXT_API
BOOL LoadCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);

AFX_EXT_API
BOOL SaveCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);


// ��������ź�SYSID��ɵ��ļ�����
AFX_EXT_API
CString GetCommandExtraFileName(sysid_t& sysid, cmdid_t& nCmdID);

AFX_EXT_API
CString GetCommandExtFileName();

//���ӳ���������������
AFX_EXT_API 
CString ComposeKMStr(DWORD km);
//�õ�KM�ı�־����
AFX_EXT_API
BYTE GetLongKmFlag(DWORD km);
//�õ�M�ı�־����
AFX_EXT_API
BYTE GetLongMFlag(DWORD km);

//
AFX_EXT_API
BOOL ReceiveCommandIDConfirmation(bool bValid, int nConsoleID, cmdid_t nCmdID, DWORD dwCookie);


};

#pragma pack(pop)

#endif

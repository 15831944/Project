
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

#ifndef __TCC_C3_H__
#define __TCC_C3_H__

#pragma warning( disable:4786 )

#pragma once

#import <msxml4.dll> //raw_interfaces_only
#include <vector>

namespace TCC_C3 {
#undef AFX_EXT_API // CUIHU!!!
#define AFX_EXT_API

	//Check if MSXML 4.0 is installed
	bool isMSXMLInstalled();

	class ITccClientInfoProvider;
	class ITccAdviseSink;

	typedef WORD stano_t;	//վ������
	typedef WORD cmdid_t;	//�п����������
	typedef WORD lineid_t;	//�п���·������
	typedef UUID sysid_t;
	//���������
	struct  kmmark_t {
		BYTE flag;	//�����ϵ, Dϵ DKϵ�ȵ�
		struct _INFO {
			DWORD val : 24;	//����
			DWORD km_cl: 6;	//���� ������ChainLength
			DWORD m_cl : 2;	//��   ������ChainLength

			bool operator<(const _INFO& rhs) const
			{
				DWORD km = val/1000;
				DWORD m  = val%1000;
				DWORD rhs_km = rhs.val/1000;
				DWORD rhs_m  = rhs.val%1000;

				return km<rhs_km
					|| km==rhs_km && km_cl<rhs.km_cl
					|| km==rhs_km && km_cl==rhs.km_cl && m<rhs_m
					|| km==rhs_km && km_cl==rhs.km_cl && m==rhs_m && m_cl<rhs.m_cl;
			}
			bool operator==(const _INFO& rhs) const
			{
				return val==rhs.val && km_cl==rhs.km_cl && m_cl==rhs.m_cl;
			}
			bool operator<=(const _INFO& rhs) const
			{
				return operator<(rhs) || operator==(rhs);
			}
			bool operator>(const _INFO& rhs) const
			{
				return !operator<=(rhs);
			}
			bool operator>=(const _INFO& rhs) const
			{
				return !operator<(rhs);
			}
		} info;

	public:
		kmmark_t() { memset(this, 0, sizeof(kmmark_t)); }
		kmmark_t(DWORD dw) { flag = 0; *(DWORD*)&info = dw; }
		kmmark_t(const kmmark_t& rhs) { memcpy(this, &rhs, sizeof(kmmark_t)); }
		kmmark_t& operator=(const kmmark_t& rhs) { memcpy(this, &rhs, sizeof(kmmark_t)); return *this; }
		DWORD GetDWORD() { return *(DWORD*)&info; }
		
		bool Equal(const kmmark_t& rhs) const;
		bool Less(const kmmark_t& rhs, int builtin_id) const;
		bool LessOrEqual(const kmmark_t& rhs, int builtin_id) const;
		bool Unequal(const kmmark_t& rhs) const;
		bool Greater(const kmmark_t& rhs, int builtin_id) const;
		bool GreaterOrEqual(const kmmark_t& rhs, int builtin_id) const;

		CString GetKcl() const;
		CString GetMcl() const;

		CString toString(ITccClientInfoProvider *pInfoProvider, int builtin_id) const;
		long DistanceTo(const kmmark_t& rhs, int builtin_id) const;
	} ;
	//�������ȱȽ�
	/*AFX_EXT_API inline bool operator==(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator==(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator==(int lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator!=(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator!=(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator!=(int lhs, const kmmark_t& rhs);

	//�����ƫ��Ƚ�
	AFX_EXT_API inline bool operator<(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator<(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator<(int lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator<=(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator<=(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator<=(int lhs, const kmmark_t& rhs);

	AFX_EXT_API inline bool operator>(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator>(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator>(int lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator>=(const kmmark_t& lhs, const kmmark_t& rhs);
	AFX_EXT_API inline bool operator>=(const kmmark_t& lhs, int rhs);
	AFX_EXT_API inline bool operator>=(int lhs, const kmmark_t& rhs);*/

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
	AFX_EXT_API INT SysidToByteArray(const sysid_t& sysid, BYTE arr[], INT num);
	AFX_EXT_API INT SysidFromByteArray(sysid_t& sysid, const BYTE arr[], INT num);
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
	WORD     nConsoleID;	//����̨ ̨��
	sysid_t  sys_id;		//�ڲ���ʶ
	stano_t  nStaID[2];		//��ʼ������վ
	lineid_t nLineID;		//��·
	BOOL	 bFullLimit;	//�Ƿ�ȫ������
	kmmark_t nKmMark[2];	//ʼĩ�����(����Ϊ��λ)
	BOOL	 bCancelLimit;	//�Ƿ�ȡ������

	cmdid_t	 nCmdID;		//�����(�пر���)
	WORD	 nSpeedMaxVal;	//����ֵ(�пر���)
	BOOL	 bStartNow;		//�Ƿ�������ʼ
	BOOL	 bEndless;		//�Ƿ�������Ч
	time_t	 nTime[2];		//��ʼ������ʱ��

	stano_t	 nTccID;		//����վ(�пر���)
	lineid_t nTccLineID;	//����վ��·��(�пر���)
	DWORD	 nTccSenderID;	//����վ������
	//kmmark_t nTccKmMark[2];	//����վ����귶Χ(�пر���)
	char     nTccKmMarkFlag[2]; //����վ������Ͻ��־ 

	TCC_COMMAND();
	bool operator==(const TCC_COMMAND& rhs);
	bool operator!=(const TCC_COMMAND& rhs);

	//���һ��RAW_STOCK_COMMAND
	void ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd);
	CString toString(ITccClientInfoProvider *pInfoProvider);
};

// �����������ȷ�������ʽ
struct  STOCK_COMMAND {
	dcmd_id_t dcmd_id;	//���������

	//��Ϊ���ҵ�������ʱ��Ҫ�������������, ���������ѭ��ʹ�õ�, ������Ϊ����ʹ��
	DWORD	  dcmd_send_entity;	//������������
	time_t	  dcmd_create_time;

	WORD     nConsoleID;	//����̨ ̨��
	sysid_t  sys_id;		//�ڲ���ʶ
	cmdid_t	 nCmdID;		//�����
	stano_t  nStaID[2];		//��ʼ������վ
	lineid_t nLineID;		//��·
	CString  szTrainID;		//���κ�
	WORD	 nSpeedMaxVal;	//����ֵ

	BOOL	 bFullLimit;	//�Ƿ�ȫ������
	kmmark_t nKmMark[2];	//ʼĩ�����(����Ϊ��λ)
	BOOL	 bStartNow;		//�Ƿ�������ʼ
	BOOL	 bEndless;		//�Ƿ�������Ч
	time_t	 nTime[2];		//��ʼ������ʱ��

	BOOL	 bCancelLimit;	//�Ƿ�ȡ������

	DWORD	 nCreatorID;	//������
	DWORD	 nCheckorID;	//����У����
	DWORD	 nConfirmorID;	//ȷ�Ϸֽ���
	sysid_t  raw_sysid;		//ԭʼ�����ʶ
	time_t   nCreateTime;	//�����ʱ��

	struct _TCC_STRUCT {
		stano_t	 nID;		//����վ
		lineid_t nTccLineID;//��·
		DWORD    nCheckorID;//��վУ����(����վ��ȷ�Ϸֽ���)
		WORD	 nStatus;	//����վ����״̬
		DWORD	 nSenderID;	//����վ�������
		time_t	 nTime;		//����վ������Чʱ��
		//kmmark_t nKmMark[2];//����վ����귶Χ
		char     nKmMarkFlag[2]; //����վ������Ͻ��־ 
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			nStatus = 0;
			nSenderID = 0;
			nTime = 0;
			nKmMarkFlag[0] = nKmMarkFlag[1] = 0;
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
	dcmd_id_t dcmd_id;	//���������

	BYTE     nCmdType;	//��������  1=STATION_LIMIT 2=ABSAPP_LIMIT 3=ABSAPP_FULL_LIMIT 4=KM_LIMIT
	BYTE     nDirection;//���ٷ���  1���� 2���� 3˫��

	WORD     nConsoleID;//����̨ ̨��
	sysid_t	 raw_sysid;	//ԭʼ�����ʶ
	cmdid_t	 nCmdID;	//�����
	stano_t  nStaID[2];	//��ʼ������վ
	lineid_t nLineID;	//��·

	BOOL	 bFullLimit;//�Ƿ�ȫ������
	kmmark_t nKmMark[2];//ʼĩ�����(����Ϊ��λ)
	BOOL	 bStartNow;	//�Ƿ�������ʼ
	BOOL	 bEndless;	//�Ƿ�������Ч
	time_t	 nTime[2];	//��ʼ������ʱ��

	BOOL	 bCancelLimit;	//�Ƿ�ȡ������

	DWORD	 nCreatorID;	//������
	DWORD	 nCheckorID;	//����У����
	DWORD	 nConfirmorID;	//ȷ�Ϸֽ���
	time_t   nCreateTime;	//�����ʱ��

	struct _TCC_STRUCT {
		stano_t	 nID;			//����վ
		lineid_t nTccLineID;	//��·
		DWORD	 nCheckorID;	//��վУ����(����վ��ȷ�Ϸֽ���)
		//kmmark_t nKmMark[2];	//����վ����귶Χ
		char     nKmMarkFlag[2]; //����վ������Ͻ��־ 
		BYTE	 bTccValidate;	//����վ��У���
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			bTccValidate = 0;
			nKmMarkFlag[0] = nKmMarkFlag[1] = 0;
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
	INT Decompose(STOCK_COMMAND cmd[], INT num);
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
	virtual INT OnSendTccCommand(TCC_COMMAND& cmd, DWORD flag, HWND hTccDlg) = 0;
	virtual INT OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir, HWND hTccDlg) = 0;
	// �������������б�
	virtual INT OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg) = 0;

	// ��ʵʱ�б���ȡ������
	virtual INT OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg) = 0;
	// ��ȷ���б���ȡ������
	virtual INT OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg) = 0;

	// ����ȷ������
	virtual INT OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTddDlg) = 0;
	// ɾ��ĳ����վȷ������
	virtual INT OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg) = 0;

	virtual INT OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag) = 0;
	virtual INT OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag) = 0;

	virtual INT OnTccCommandFileChanged(
					LPCSTR szFilename, 
					DWORD dwOper) = 0;

	// Ԥ��
	virtual INT OnTccCommandStatusChanged(
					TCC_COMMAND &cmd,
					DWORD status,
					DWORD dwParam) = 0;

	virtual void OnLogPrint(LPCSTR szLog)  = 0;

	struct STOCK_COMMAND_STATUS_CHANGE_INFO {
		WORD     nConsoleID;	//����̨ ̨��
		sysid_t  sys_id;		//�ڲ���ʶ
		cmdid_t	 nCmdID;		//�����
		stano_t	 nTccID;		//����վ

		DWORD    nCheckorID;//��վУ����(����վ��ȷ�Ϸֽ���)
		DWORD	 nSenderID;	//����վ�������

		WORD	 nStatus;	//����վ����״̬
		DWORD	 nTime;		//����վ������Чʱ��
	};
	enum STOCKCOMMAND_STATUS_SETTING_MODE { MAN_SET_STOCKCOMMAND_STATUS, LIRC_SET_STOCKCOMMAND_STATUS, };
	virtual void SetStockCommandStatus(STOCKCOMMAND_STATUS_SETTING_MODE mode, STOCK_COMMAND_STATUS_CHANGE_INFO& info, HWND hTccDlg) = 0;

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
	virtual DWORD GetCurrentEntityID() = 0;

	//�����ǰ׺
	bool GetKmsysCode(int builtin_id, const CString& szPrex, int *pnCode);
	bool GetKmsysPrex(int builtin_id, const kmmark_t& kmmark, CString& szPrex);
	int  GetBlockKmsysCode();

	// �����鵵
	// input :
	//       builtin_id �ڱ���·��
	//       nKmBegin, nKmEnd ��Ҫ�鵵�Ĺ����
	// output:
	//       AdjustKmMarker =true�鵵�ɹ�, =false�鵵ʧ��
	//       nKmBegin, nKmEnd �鵵��Ĺ����
	//       nHitTrackNum �鵵���������Ĺ����·��
	bool AdjustKmMarker(int builtin_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd, int *nHitTrackNum);

	// �鵵����������վ��ȫ��������
	// input:
	//       builtin_id �ڱ���·��
	//       nKmBegin, nKmEnd �ѹ鵵�Ĺ����
	// output:
	//       GetFullLimitKmMarker =true�ɹ�, =falseʧ��
	//       nKmBegin, nKmEnd ��Чȫ����Ĺ����
	//       nHitFullSecNum ȫ������Ŀ
	bool GetFullLimitKmMarker(int builtin_id, kmmark_t *nKmBegin, kmmark_t *nKmEnd, int *nHitFullSecNum);

	// ����վ������ʼ�����
	bool GetHitTccList(int builtin_id, dir_t dir, kmmark_t nKmBegin, kmmark_t nKmEnd, std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tcclist);

	//�õ�builtin_id��·�б�
	bool GetLineList(std::vector<int>& linelist);
	//�õ�Ŀ��վnTccID��builtin_id��·�б�
	bool GetLineList(stano_t nTccID, std::vector<int>& linelist);
	bool GetLineList(stano_t nTccID, stano_t nTccID2, std::vector<int>& linelist);
	lineid_t GetTccLineID(stano_t nStaID, int builtin_id);
	int  GetLineID(stano_t nStaID, lineid_t tccline_id);
	CString GetLineName(int builtin_id);

	// ȡ��վnStaID��TccLine�б�
	void GetTccLineList(stano_t nStaID, std::vector<lineid_t>& lines);
	bool GetBlockList(stano_t nTccID, std::vector<lineid_t>& blocklist);
	// ȡ��վnStaID��TccLine�б�����վnStaID2��������ЩTccLine
	void GetTccLineList(
					stano_t nStaID,
					stano_t nStaID2,
					std::vector<lineid_t>& lines);
	// ȡ��վnStaID����վnStaID2����·nTccLineID2��ƥ���TccLine
	lineid_t GetTccLine(
					stano_t nStaID,
					stano_t nStaID2,
					lineid_t nTccLineID2);
	// ȡ��վnStaID��nTccLineID������
	CString GetTccLineName(stano_t nStaID, lineid_t tccline_id);
	// ȡ��TccLine�ķ���ֵ(�����л�˫��)
	dir_t GetTccLineDirection(stano_t nStaID, lineid_t tccline_id);
	dir_t GetLineDirection(int builtin_id);
	
	bool GetLineKmMarkerScope(int builtin_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd);
	bool GetLineSpeedList(int builtin_id, dir_t dir, std::vector<int>& speedlist);
	bool GetTccLineSpeedList(stano_t nTccID, int tccline_id, dir_t dir, std::vector<int>& speedlist);
	bool GetTccLineKmMarkerScope(stano_t nTccID, lineid_t tccline_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd);

	//�м�վ������������վ
	stano_t GetParentStaID(stano_t nZC_TccID);
	//�Ƿ��м�վ
	bool    IsZC_TccID(stano_t nZC_TccID);

	// �ж�nStaID��nStaID2 �Ƿ�����վ
	BOOL IsLinkedStation(stano_t nStaID, stano_t nStaID2);

	// վnStaID2�����վnStaID�����䷽����(���з���, ���з���)
	dir_t GetStationDirection(stano_t nStaID, stano_t nStaID2);
	// �õ�վnStaID������վ�б�
	void GetLinkedStationList(stano_t nStaID, std::vector<stano_t>& staList);
	// ����վ��, Tcc��·�źͷ���(������)�õ�����վ
	stano_t GetLinkedStation(stano_t nStaID, lineid_t nTccLineID, dir_t dir);

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

	// Ȩ�޿���
	virtual bool IsTCC2ReadOnly()=0;
	virtual bool IsTCC2Execute()=0;
	virtual bool IsCTC2GPC()=0;
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
	stano_t nStaID;//����վ
	int nStaNum;
	BYTE comm_active;	// �п�ͨ��״̬	(0=��;1=����;2=˫��)
	BYTE init_status;	//��ʼ��״̬: δ��ʼ��, ��ʼ��������, ��ʼ�������
	struct {
		stano_t nTccID;
		BYTE standby_status; //���� ˫��
		BYTE master_status;	// 1:a�� 2:b��  �п�����״̬

		BYTE ilock_channel[2];// ����������ͨ��״̬
		BYTE ctc_channel[2];// ��CTC����ͨ��״̬
		BYTE switch_channel[2];//�뽻��������ͨ��״̬

		struct {
			BOOL bValid;	//�Ƿ���Ч
			BYTE leu_channel[2];// LEU ͨ��״̬
			BYTE leu_ack[4];//LEUӦ����״̬, һ��leu��4��Ӧ����
		} leu[16];

		BYTE nZCNum;
		struct {
			stano_t nStaID;// �м�վ վ��
			BYTE channel[2];//ֻ�е�һ����վ���� ���м�վͨ��������״̬
		} ZC[8];
	} sta[32];

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
	 result ���ɻ�������п�״̬(ĿǰԤ��)
	 strComment �����������
 */
struct TCC_COMMAND_RESULT {
	TCC_COMMAND cmd;
	CString strComment;
	int result;
};
//BOOL Receive_TccCommandResult(TCC_COMMAND_RESULT& cmd_result, HWND);

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


// �п���������������
HWND GetTccDialogWndHandle();
HWND GetTccCommandSendDlgHandle();
void TccInitComplete(DWORD nTccID);
// 

//
AFX_EXT_API
BOOL ReceiveCommandIDConfirmation(bool bValid, int nConsoleID, cmdid_t nCmdID, DWORD dwCookie);

CString GetStockCommandStatusString(DWORD nStatus);

};

#endif

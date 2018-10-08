#pragma once
#include <vector>
#include <map>
#import <msxml.dll> named_guids
using namespace std;
using namespace MSXML;

#define IsReceiveTMISPlan(wintg_operate)   ( (wintg_operate&0x1) == 0x1)
#define IsSendPlanToStation(wintg_operate) ( (wintg_operate&0x2) == 0x2)
#define IsInhbitAddWorkTrain(wintg_operate) ( (wintg_operate&0x4) == 0x4)
#define IsSecretTrainNotVisible(wintg_operate) ( (wintg_operate&0x8) == 0x8)

#define IsAssistantModifyPropTreeList(wintg_operate) ((wintg_operate&0x10) == 0x10)

struct GPCUser
{
	char desp[50];
	char name[10];
	int  id;
	int  right; 
	CWordArray	ary_section;

	GPCUser();
	GPCUser(const GPCUser& rhs);
	GPCUser& operator=(const GPCUser& rhs);
};

struct TGStation
{
	int  id;
	char name[64];
	char fKmMark[64];
	BYTE adtime;
	BYTE staclass;
	BYTE property;   //��վ����:-1, ���豸, 0,TDCS, 1, CTC
	TGStation();
	TGStation(const TGStation& rhs);
	TGStation& operator=(const TGStation& rhs);
};

struct TGSection
{
	int		id;
	char	name[50];
	int		selfstd;
	int		nextstd;
	
	CWordArray	ary_station;
	TGSection();
	TGSection(const TGSection& rhs);
	TGSection& operator=(const TGSection& rhs);
};

struct TBL
{
	enum{
	APP   = 0,
	MARK  = 1,
	TRAIN = 2,
	};
};

struct Table
{
	DWORD dwAppCtrl[40];
	DWORD dwTrainCtrl[40];
	DWORD dwMarkCtrl[40];
};

struct RIGHT
{
	char id;
	int  operate;
	DWORD train;
	DWORD mark;
	DWORD app;
	DWORD wintg_operate;
	RIGHT()
	{
		id=0;
		operate = 0;
		train = 0;
		mark = 0;
		app = 0;
		wintg_operate = 0;
	}
};

class CLoadGPCDef
{
public:
	CLoadGPCDef(void);
	~CLoadGPCDef(void);
public:
	bool Load(CString pathname);

public:
	CString GetSectionXML();
	CString GetStationXML();
	CString GetCurrSectionSetXML(const int gpcid, const int section_id);
    
	bool GetUserRight(const int Identify, RIGHT& right);

	int GetSectionListCount();
	int GetSectionNameByIndex(int index, CString &name);
	CString GetSectionNameById(int section);

private:
	BOOL LoadGPCDEF(CString szFileName);
	int  LoadUser(char *ptr);
	int  LoadSectInfo(char *ptr);
	int  LoadStationInfo(char *ptr);

	std::vector<GPCUser>   mVectUser;
	std::vector<TGSection> mVectSection;
	std::vector<TGStation> mVectStation;
	
private:
	BOOL LoadTGNOXML(CString szFileName);
    bool SetRight(MSXML::IXMLDOMNodeListPtr list, long& right, BYTE flag);
	
	Table mMapTable;
	std::vector<RIGHT> mUserRight;

public:
	// ��Ծ��վ���Ʊ�־ 1: �ޱ�� 2: ���뽻�� 3: ����
	int m_nLeapLineFlag;
	// �Ƿ���Ƽ���ͣ���־ --- 0: �����Ƽ���ͣ���־ 1: ����ͣ�� ���ƨ� 2: �Ǽ���ͣ�� ���ƨ�
	int m_nDrawTenicalStop;
	// �߿�
	int m_LineWidth;
        // ��������1,2,5
	int m_backgroudNum;
	// ��·׼���ñ�־----0: ������ 1: ��ɫ�� 2: ԲȦ
    int m_nDrawRouteOkFlag;
	
	void SaveXML(CString filename, CString xml);
};

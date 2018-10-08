#pragma once
#include <map>
#include <vector>
using namespace std;

struct SIDE_INFO
{
	int  sideno;
	char sidename[20];
	bool bCrhSide;  // �Ƿ񶯳���ɵ�
	BYTE nPlatform; // վ̨ 0: �� 1:�� 2:��վ̨
	short side_property;
	short direct; // 1:����; 2:����;
	SIDE_INFO()
	{
		sideno = -1;
		memset(sidename, 0, 20);
		bCrhSide = false;
		nPlatform=0;
		side_property=21;
		direct=0;
	}
};

struct LINE_INFO 
{                	
	int  stano;
	bool fHigh;                                                     
	std::vector<SIDE_INFO> vecSideInfo;
	
	LINE_INFO();
	~LINE_INFO();
	LINE_INFO(const LINE_INFO& rhs);
	LINE_INFO& operator=(const LINE_INFO& rhs);
	void Clear();
};   

class CLineCfg
{
public:
	CLineCfg(void);
	~CLineCfg(void);

	void ClearLineCfg();
	void AddLineCfg(const LINE_INFO& line);
public:
	bool IsCRHSide(int stano, int sideno);
	bool  GetStaLineCfg(int staid, LINE_INFO& line);
private:
	std::map<int, LINE_INFO> mapStaSideInfo;
};

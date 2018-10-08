
#ifndef __TCC_C3_GPC_INFOPROVIDER_H__
#define __TCC_C3_GPC_INFOPROVIDER_H__

#include "tcc.h"


#pragma once

namespace TCC_C3
{

class CTccGpcInfoProvider : public ITccClientInfoProvider
{
public:
	CTccGpcInfoProvider();
	virtual ~CTccGpcInfoProvider();

public:
	bool IsTCC2ReadOnly(){ return m_bReadOnly;}
	bool IsTCC2Execute(){ return m_bExecute;}
	bool IsCTC2GPC(){ return m_bCtcGpc;}
	BOOL LoadTccLineConfig();
	void SetTCCRight(int SectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc);

public:
	static void LoadTccLineNameConfig(CString strFilename);

private:
	bool m_bReadOnly;
	bool m_bExecute;
	bool m_bCtcGpc;

	int  m_nCurrentSectionNo;  // 
	INT  m_nSecNo;

public:
	// TCC����Ŀ¼(��../data/DDML/tcc), ���ڴ��Tccԭʼ����
	virtual CString GetTccDataDirectory();

	// TCC����Ŀ¼(��../data/tcc), ���ڴ��Tcc����������
	virtual CString GetTccConfigDirectory();

	// �õ���վ�б�����ֵΪʵ��ȡ�õ��б���Ŀ
	virtual void GetStationList(std::vector<stano_t>& staList);
	// ����վ�ŵõ�վ��
	virtual CString GetStationName(stano_t nStaID);

	// �õ����λ�ͷ�����
	virtual CString GetOrgSender();
	virtual CString GetPersonSender();

	virtual WORD  GetConsoleID();
	virtual DWORD GetCurrentEntityID();

	// У��վ�ŵĺϷ���
	virtual BOOL ValidateStationID(stano_t nStaID);

	virtual BOOL IsStationListChanged();

	// �õ������������ֵ��Χ
	//virtual BOOL  IsValidCommandID(cmdid_t nCmdID);
	//virtual cmdid_t GetInvalidCommandID();
	//virtual cmdid_t GetMinCommandID();
	//virtual cmdid_t GetMaxCommandID();
	//virtual cmdid_t GetNextCommandID(cmdid_t nCmdID);
};

};

#endif

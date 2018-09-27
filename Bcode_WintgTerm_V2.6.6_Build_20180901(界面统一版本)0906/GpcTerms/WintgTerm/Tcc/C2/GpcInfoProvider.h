
#ifndef __TCC_GPC_INFOPROVIDER_H__
#define __TCC_GPC_INFOPROVIDER_H__

#include "tcc.h"



#pragma once

namespace TCC
{
class CTccGpcInfoProvider : public ITccClientInfoProvider
{
public:
	CTccGpcInfoProvider();
	virtual ~CTccGpcInfoProvider();
		
public:
	bool IsTCCReadOnly(){ return m_bReadOnly;}
	bool IsTCCExecute(){ return m_bExecute;}
	bool IsCTCGPC(){ return m_bCTCGpc;}
	BOOL LoadTccLineConfig();
	void SetTCCRight(int SectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc);

public:
	static void LoadTccLineNameConfig(CString strFilename);

private:
	bool m_bReadOnly;
	bool m_bExecute;
	bool m_bCTCGpc;
	int  m_nCurrentSectionNo;  // 
	INT  m_nSecNo;

public:
	// TCC����Ŀ¼(��../data/DDML/tcc), ���ڴ��Tccԭʼ����
	virtual CString GetTccDataDirectory();

	// TCC����Ŀ¼(��../data/tcc), ���ڴ��Tcc����������
	virtual CString GetTccConfigDirectory();

	// �õ���վ�б�����ֵΪʵ��ȡ�õ��б���Ŀ
	virtual void GetStationList(std::vector<TCC::stano_t>& staList);
	// ����վ�ŵõ�վ��
	virtual CString GetStationName(TCC::stano_t nStaID);

	// �õ����λ�ͷ�����
	virtual CString GetOrgSender();
	virtual CString GetPersonSender();

	virtual WORD  GetConsoleID();

	// �ж�nStaID��nStaID2 �Ƿ�����վ
	virtual BOOL IsLinkedStation(TCC::stano_t nStaID, TCC::stano_t nStaID2);
	// վnStaID2�����վnStaID�����䷽����(���з���, ���з���)
	virtual TCC::dir_t GetStationDirection(
						TCC::stano_t nStaID, 
						TCC::stano_t nStaID2);
	// �õ�վnStaID������վ�б�
	virtual void GetLinkedStationList(TCC::stano_t nStaID, std::vector<TCC::stano_t>& staList);
	// ����վ��, Tcc��·�źͷ���(������)�õ�����վ
	virtual TCC::stano_t  GetLinkedStation(
		TCC::stano_t nStaID, 
		TCC::lineid_t nTccLineID,
		TCC::dir_t dir);

	// ȡ��վnStaID��TccLine�б�
	virtual void GetTccLineList(
					TCC::stano_t nStaID, 
					std::vector<TCC::lineid_t>& lines);
	// ȡ��վnStaID��TccLine�б�����վnStaID2��������ЩTccLine
	virtual void GetTccLineList(
					TCC::stano_t nStaID,
					TCC::stano_t nStaID2,
					std::vector<TCC::lineid_t>& lines);

	virtual void GetTccBlockLineList(TCC::stano_t nStaID, std::vector<TCC::lineid_t>& lines);
	virtual CString GetTccBlockLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	virtual BOOL IsTccBlockLine(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);

	// ȡ��վnStaID����վnStaID2����·nTccLineID2��ƥ���TccLine
	virtual TCC::lineid_t GetTccLine(
					TCC::stano_t nStaID,
					TCC::stano_t nStaID2,
					TCC::lineid_t nTccLineID2);
	// ȡ��վnStaID��nTccLineID������
	virtual CString GetTccLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	// ȡ��Ŀ��վnStaID��TCC��·nTccLineID��������
	// DIR_UP ��������
	// DIR_DOWN ��������
	// DIR_DUAL ����������
	// DIR_UNKNOWN ��·�Ų�����
	virtual TCC::dir_t GetTccLineAbsNum(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	// ȡ��TccLine�ķ���ֵ(�����л�˫��)
	virtual TCC::dir_t GetTccLineDirection(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);

	// ȡ���ڲ���·��
	virtual int GetLineID(TCC::stano_t nTccID, TCC::lineid_t nTccLineID);
	// ���ݹ����ȡ�����վ
	std::vector<TCC::stano_t> GetTccList(int builtin_id, TCC::kmmark_t nKmBegin, TCC::kmmark_t nKmEnd);

	// У��վ�ŵĺϷ���
	virtual BOOL ValidateStationID(TCC::stano_t nStaID);

	virtual BOOL IsStationListChanged();
};

}
#endif

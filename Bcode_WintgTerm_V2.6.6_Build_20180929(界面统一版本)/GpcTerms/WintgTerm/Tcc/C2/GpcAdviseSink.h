
#ifndef __TCC_GPC_ADIVSESINK_H__
#define __TCC_GPC_ADVISESINK_H__

#include "tcc.h"
#include "GpcInfoProvider.h"

#pragma once

namespace TCC
{

class CTccGpcAdviseSink : public ITccAdviseSink
{
public:
	CTccGpcAdviseSink(CTccGpcInfoProvider *pInfoProvider, CWnd *pWintgForm = NULL)
		: m_pWintgForm(pWintgForm), m_pInfoProvider(pInfoProvider) { }
	virtual ~CTccGpcAdviseSink() {}

public:
	// ��һ���п����ķ�������������ͽ���ɺ���ֵ����(0:�ɹ���1:ʧ��)
	virtual INT OnSendTccCommand(TCC::TCC_COMMAND& cmd, HWND hTccDlg);
	virtual INT OnSendInitCommand(
		stano_t nTccID, 
		lineid_t nTccLineID, 
		dir_t dir,
		HWND hTccDlg);
	// �������������б�
	virtual INT OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg);

	// ��ʵʱ�б���ȡ������
	virtual INT OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg);
	// ��ȷ���б���ȡ������
	virtual INT OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg);

	// ����ȷ������
	virtual INT OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTddDlg);
	// ɾ��ĳ����վȷ������
	virtual INT OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg);

	virtual INT OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag);
	virtual INT OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag);

	virtual INT OnTccCommandFileChanged(
					LPCSTR szFilename, 
					DWORD dwOper);

	// Ԥ��
	virtual INT OnTccCommandStatusChanged(
					TCC_COMMAND &cmd,
					DWORD status,
					DWORD dwParam);


	BOOL ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg);

private:
	CWnd *m_pWintgForm;
	CTccGpcInfoProvider *m_pInfoProvider;
};

}

#endif

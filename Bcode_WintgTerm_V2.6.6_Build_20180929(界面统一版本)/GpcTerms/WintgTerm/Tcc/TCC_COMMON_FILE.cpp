#include "stdafx.h"

#include "TCC_COMMON_FILE.h"
extern bool TGSendStringMessage(const char *token,const char *content,long eid);
extern BOOL  GenXMLMessage_InformDCMDToFront(CString &token,CString &dest);

#define MAX_TRAINID_CMD_NUM 5 //������Ŀ
TCC_C3::RAW_STOCK_COMMAND FromC2ToC3(TCC::RAW_STOCK_COMMAND& c2)
{
	TCC_C3::RAW_STOCK_COMMAND c3;
	
	c3.dcmd_id = (const char*)c2.dcmd_id;	//���������

	c3.nCmdType = 0;
	c3.nDirection = 0;

	c3.nConsoleID = c2.nConsoleID;	//
	c3.raw_sysid = c2.raw_sysid;	//ԭʼ�����ʶ
	c3.nCmdID = c2.nCmdID;			//�����
	c3.nStaID[0] = c2.nStaID[0];	//��ʼ������վ
	c3.nStaID[1] = c2.nStaID[1];
	c3.nLineID = c2.nLineID;	//��·

	c3.bFullLimit = c2.bFullLimit;//�Ƿ�ȫ������
	c3.nKmMark[0].flag       = 0;
	c3.nKmMark[0].info.km_cl = ((c2.nKmMark[0].km_cl));// >> 24) & 0x3F);
	c3.nKmMark[0].info.m_cl  = ((c2.nKmMark[0].m_cl));// >> 30) & 0x01);
	c3.nKmMark[0].info.val   = (c2.nKmMark[0].val);// & 0x0FFFFFF);//ʼĩ�����(����Ϊ��λ)
	c3.nKmMark[1].flag       = 0;
	c3.nKmMark[1].info.km_cl = ((c2.nKmMark[1].km_cl));// >> 24) & 0x3F);
	c3.nKmMark[1].info.m_cl  = ((c2.nKmMark[1].m_cl));// >> 30) & 0x01);
	c3.nKmMark[1].info.val   = (c2.nKmMark[1].val);// & 0x0FFFFFF);//ʼĩ�����(����Ϊ��λ)
	c3.bStartNow = c2.bStartNow;	//�Ƿ�������ʼ
	c3.bEndless = c2.bEndless;	//�Ƿ�������Ч
	c3.nTime[0] = c2.nTime[0];	//��ʼ������ʱ��
	c3.nTime[1] = c2.nTime[1];

	c3.bCancelLimit = c2.bCancelLimit;	//�Ƿ�ȡ������

	c3.nCreatorID = c2.nCreatorID;	//������
	c3.nCheckorID = c2.nCheckorID;	//����У����
	c3.nConfirmorID = c2.nConfirmorID;	//ȷ�Ϸֽ���
	c3.nCreateTime = c2.nCreateTime;	//�����ʱ��

	c3.tcc.resize(c2.tcc.size());
	for(int i=0; i<c3.tcc.size(); i++)
	{
		c3.tcc[i].nID = c2.tcc[i].nID;			//����վ
		c3.tcc[i].nTccLineID = c2.tcc[i].nTccLineID;	//��·
		c3.tcc[i].nCheckorID = c2.tcc[i].nCheckorID;	//��վУ����(����վ��ȷ�Ϸֽ���)
		c3.tcc[i].nKmMarkFlag[0] = 0; //����վ������Ͻ��־ 
		c3.tcc[i].nKmMarkFlag[1] = 0;
		c3.tcc[i].bTccValidate = c2.tcc[i].bTccValidate;	//����վ��У���
	}

	for(i=0; i<sizeof(c3.train_cmd)/sizeof(c3.train_cmd[0]); i++)
	{
		c3.train_cmd[i].sys_id = c2.train_cmd[i].sys_id;			//ÿ�˳�һ�����һ��ϵͳ��ʶ
		c3.train_cmd[i].szTrainID = c2.train_cmd[i].szTrainID;		//���κ�
		c3.train_cmd[i].nSpeedMaxVal = c2.train_cmd[i].nSpeedMaxVal;	//����ֵ
		c3.train_cmd[i].nSpeed       = c2.train_cmd[i].nSpeed;			//����ֵ

		//ÿ�˳����������վ״̬
		c3.train_cmd[i].tcc.resize(c3.tcc.size());
		for(int j=0; j<c3.train_cmd[i].tcc.size(); j++)
		{
			c3.train_cmd[i].tcc[j].nStatus = c2.train_cmd[i].tcc[j].nStatus;	//����վ����״̬
			c3.train_cmd[i].tcc[j].nSenderID = c2.train_cmd[i].tcc[j].nSenderID;	//����վ�������
			c3.train_cmd[i].tcc[j].nTime = c2.train_cmd[i].tcc[j].nTime;		//����վ������Чʱ��
		}
	}

	c3.bGenCancelCommand = c2.bGenCancelCommand;

	return c3;
}

void  ConVertRAW_STOCK_COMMANToTcc ( CLBTH::MsgDCmdTcc *pMsgDCmdTcc, const TCC_C3::RAW_STOCK_COMMAND& raw_cmd)
{
	pMsgDCmdTcc->nCmdType = raw_cmd.nCmdType;
	pMsgDCmdTcc->nDirection = raw_cmd.nDirection;

	TCC_C3::SysidToByteArray(raw_cmd.raw_sysid, pMsgDCmdTcc->raw_sysid, sizeof(pMsgDCmdTcc->raw_sysid));
	pMsgDCmdTcc->nCmdID = raw_cmd.nCmdID;		//�����
	pMsgDCmdTcc->nConsoleID = raw_cmd.nConsoleID;//
	pMsgDCmdTcc->nStaID[0] = raw_cmd.nStaID[0];	//��ʼ������վ
	pMsgDCmdTcc->nStaID[1] = raw_cmd.nStaID[1];
	pMsgDCmdTcc->nLineID = raw_cmd.nLineID;		//��·
	pMsgDCmdTcc->bFullLimit = (BYTE)raw_cmd.bFullLimit;	//�Ƿ�ȫ������
	pMsgDCmdTcc->nKmMark[0].nKmsysCode = raw_cmd.nKmMark[0].flag;
	pMsgDCmdTcc->nKmMark[0].k_cl       = raw_cmd.nKmMark[0].info.km_cl;
	pMsgDCmdTcc->nKmMark[0].m_cl       = raw_cmd.nKmMark[0].info.m_cl;
	pMsgDCmdTcc->nKmMark[0].val        = raw_cmd.nKmMark[0].info.val;//ʼĩ�����(����Ϊ��λ)
	pMsgDCmdTcc->nKmMark[1].nKmsysCode = raw_cmd.nKmMark[1].flag;
	pMsgDCmdTcc->nKmMark[1].k_cl       = raw_cmd.nKmMark[1].info.km_cl;
	pMsgDCmdTcc->nKmMark[1].m_cl       = raw_cmd.nKmMark[1].info.m_cl;
	pMsgDCmdTcc->nKmMark[1].val        = raw_cmd.nKmMark[1].info.val;//ʼĩ�����(����Ϊ��λ)
	pMsgDCmdTcc->bStartNow = (BYTE)raw_cmd.bStartNow;		//�Ƿ�������ʼ
	pMsgDCmdTcc->bEndless = (BYTE)raw_cmd.bEndless;		//�Ƿ�������Ч
	pMsgDCmdTcc->nTime[0] = raw_cmd.nTime[0];	//��ʼ������ʱ��
	pMsgDCmdTcc->nTime[1] = raw_cmd.nTime[1];
	pMsgDCmdTcc->bCancelLimit = (BYTE)raw_cmd.bCancelLimit;	//�Ƿ�ȡ������
	pMsgDCmdTcc->nCreatorID = raw_cmd.nCreatorID;
	pMsgDCmdTcc->nCheckorID = raw_cmd.nCheckorID;
	pMsgDCmdTcc->nConfirmorID = raw_cmd.nConfirmorID;
	pMsgDCmdTcc->nCreateTime= raw_cmd.nCreateTime;
	/////////////////////// 2008.12.18  ����У���־�޷�����Ĺ���
	if(raw_cmd.nCheckorID)
		pMsgDCmdTcc->bChecked=TRUE; 
	else
		pMsgDCmdTcc->bChecked=FALSE; 
	///////////////////////////
	pMsgDCmdTcc->nTccNum = raw_cmd.tcc.size();//����վ����
	for(int i=0; i<raw_cmd.tcc.size(); i++)
	{
		pMsgDCmdTcc->tcc[i].nTccID = raw_cmd.tcc[i].nID;//����վ
		pMsgDCmdTcc->tcc[i].nTccLineID = raw_cmd.tcc[i].nTccLineID;//��·
		pMsgDCmdTcc->tcc[i].nCheckorID = raw_cmd.tcc[i].nCheckorID;
		pMsgDCmdTcc->tcc[i].nKmMark[0].nKmsysCode = ((raw_cmd.tcc[i].nKmMarkFlag[0] & 0x03) << 6) +(raw_cmd.nKmMark[0].flag & 0x3F);
		pMsgDCmdTcc->tcc[i].nKmMark[0].k_cl       = raw_cmd.nKmMark[0].info.km_cl;
		pMsgDCmdTcc->tcc[i].nKmMark[0].m_cl       = raw_cmd.nKmMark[0].info.m_cl;
		pMsgDCmdTcc->tcc[i].nKmMark[0].val        = raw_cmd.nKmMark[0].info.val;//ʼĩ�����(����Ϊ��λ)
		pMsgDCmdTcc->tcc[i].nKmMark[1].nKmsysCode = ((raw_cmd.tcc[i].nKmMarkFlag[1] & 0x03) << 6) + (raw_cmd.nKmMark[1].flag & 0x3F);
		pMsgDCmdTcc->tcc[i].nKmMark[1].k_cl       = raw_cmd.nKmMark[1].info.km_cl;
		pMsgDCmdTcc->tcc[i].nKmMark[1].m_cl       = raw_cmd.nKmMark[1].info.m_cl;
		pMsgDCmdTcc->tcc[i].nKmMark[1].val        = raw_cmd.nKmMark[1].info.val;//ʼĩ�����(����Ϊ��λ)
		pMsgDCmdTcc->tcc[i].bNeedCheck = raw_cmd.tcc[i].bTccValidate;	//�Ƿ���ҪУ��
	}

	pMsgDCmdTcc->nTrainNum = MAX_TRAINID_CMD_NUM;
	for(int i=0; i< MAX_TRAINID_CMD_NUM; i++)
	{
		strcpy(pMsgDCmdTcc->train_cmd[i].szTrainID, raw_cmd.train_cmd[i].szTrainID);
		pMsgDCmdTcc->train_cmd[i].nSpeedMaxVal = raw_cmd.train_cmd[i].nSpeedMaxVal;
		pMsgDCmdTcc->train_cmd[i].nSpeed = raw_cmd.train_cmd[i].nSpeed;
	}
}

void SendCancelDCmdC3(TCC_C3::RAW_STOCK_COMMAND& c3, int con_id, int verstion)
{
	// tcc
	CLBTH::Archive ar_tcc( CLBTH::Archive::store);	
	CLBTH::MsgDCmdTcc  mMsgDCmdTcc;

	ConVertRAW_STOCK_COMMANToTcc(&mMsgDCmdTcc, c3);

	mMsgDCmdTcc.serialize(ar_tcc);

	int  tcc_len = ar_tcc.getCount();	
	BYTE* tcc = new BYTE[tcc_len];
	if(tcc)
	{
		memset(tcc, 0 , tcc_len);
		memcpy(tcc, ar_tcc.getBuffer(),ar_tcc.getCount());
	}

	CString xml, tmp;
	xml = "<CANCEL_RUNTIME_TCC_DCMD>";
	{
		// �汾
		tmp.Format("<tcc_version id=\"%d\"/>", verstion);
		xml += tmp;
		// ���վ
		xml += " <recv_station>";
		for(int i=0; i<c3.tcc.size(); i++)
		{
			tmp.Format(" <stano no=\"%d\"/>", c3.tcc[i].nID);
			xml += tmp;
		}
		xml += " </recv_station>";

		// TCC��
		xml += " <tcc_content>";
		for(int i=0; i < tcc_len; i++)
		{
			tmp.Format("%0.2X", tcc[i]);
			xml += tmp;
		}
		xml += "</tcc_content>";
	}
	xml += "</CANCEL_RUNTIME_TCC_DCMD>";
    
	delete[] tcc;
	
	
	long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,con_id);
	CString token,content;
	GenXMLMessage_InformDCMDToFront(token,content);
	TGSendStringMessage(token,content,nEntityOfDCMD);

	token="CANCEL_RUNTIME_TCC_DCMD";

	CStdioFile file;
	if(file.Open("c:\\canceldcmd.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		file.WriteString(xml);
		file.Close();
	}

	TGSendStringMessage(token,xml,nEntityOfDCMD);

	sysprint(5,0,"����������ն� %d ����ȡ������", nEntityOfDCMD);
}

void SendCancelDCmdC2(TCC::RAW_STOCK_COMMAND& c2, int con_id)
{
	TCC_C3::RAW_STOCK_COMMAND raw_cmd;
	raw_cmd = FromC2ToC3(c2);

	raw_cmd.bCancelLimit = TRUE;
	raw_cmd.bGenCancelCommand = FALSE;

	if(raw_cmd.nCmdType == 0)
	{
		if (raw_cmd.nLineID <= 0)
			raw_cmd.nCmdType = 1;
		else
			raw_cmd.nCmdType = 4;
	}

	SendCancelDCmdC3(raw_cmd, con_id, 1);
}
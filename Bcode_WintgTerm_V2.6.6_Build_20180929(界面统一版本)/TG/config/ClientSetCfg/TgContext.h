#pragma once
#include <vector>
#include "afxtempl.h"
using namespace std;

class CTgContext
{
public:
	~CTgContext(void);
	CTgContext();
	
public:
	bool  LoadTGNoXML(CString pathname);

public:
	void  SetTrainFuncCtrlWord(DWORD word) { mTrainFuncCtrlWord = word;}
	DWORD GetTrainFuncCtrlWord(){ return mTrainFuncCtrlWord;}

	void  SetMarkFuncCtrlWord(DWORD word) { mMarkFuncCtrlWord = word;}
	DWORD GetMarkFuncCtrlWord(){ return mMarkFuncCtrlWord;}

	DWORD GetAppFuncCtrlWord(){ return mAppCtrlWord;}

public:
	// ---------------------����----------------------------
	bool IsCtrlModifyTrainStation(void) { return (m_lctrlword&0x00000001)? true:false;}

	// -------------------App Ctrl----------------------
	bool IsDrawStopSidePoint(void)   {	        return (mAppCtrlWord&0x00000001) ? true : false;}
	// �Ϻ���Ҫ����,�����ֲ�Ҫ����
	bool IsTechstopModifyNeedPwd(void)   {	    return (mAppCtrlWord&0x00000002) ? true : false;}
	// �Ϻ��ֲ���Ҫ������,��������Ҫ
	bool IsEndDrawLineDlgNeed(void)   {	        return (mAppCtrlWord&0x00000004)  ? true : false;}
	// �Ƿ��ɢ��ʾ�೵�κ� �Ϻ��ַ�,��������
	bool IsShowMutiTrainSeparate(void)   {	    return (mAppCtrlWord&0x00000008) ? true : false;}

	// �Ƿ�����ʾ�೵�κ��ڽ���ʼ���� ���о־���
	bool IsShowMutiTrainCenter(void)   {	        return (mAppCtrlWord&0x00000010) ? true : false;}
	// ���������߽����������Ҽ�����������,���������Ի��� �Ϻ��ֲ���Ҫ, ����Ҫ
	bool IsRButtonGenTg(void)   {					return (mAppCtrlWord&0x00000020) ? true : false;}
	// ���Ͼ�Ĭ�ϷǼ���ͣ�㣬��������Ĭ�ϼ���ͣ��
	bool IsSetTechStop(void){						return (mAppCtrlWord&0x00000040) ? true : false;}
	// �����趯�����,��������Ҫ.�޸Ĺɵ��Ƿ��ж�����ɵ����
	bool IsCheckCrhSide(void){						return (mAppCtrlWord&0x00000080) ? true : false;}

	// ��Ӫҵ��վͨ���䵽��,�ϲ�����Ҫ����
	bool IsNoTechPassToStayNeedPwd(void){	        return (mAppCtrlWord&0x00000100) ? true : false;}
	// ��Ӫҵ��վ������ͨ��,�Ϻ�����Ҫ����
	bool IsNoTechStayToPassNeedPwd(void){	        return (mAppCtrlWord&0x00000200) ? true : false;}
	// ���Ͼ�����ʾ�򣬽�·һ�ų�����ͳ�������ͨ���Ƿ���Ҫ��ʾ��
	bool IsStayToPassNeedInfoWnd(void){	    return (mAppCtrlWord&0x00000400) ? true : false;}
	// ���Ͼ�����ʾ�򣬽�·һ�ų�����ͳ�ͨ���䵽���Ƿ���Ҫ��ʾ��
	bool IsPassToStayNeedInfoWnd(void){	    return (mAppCtrlWord&0x00000800) ? true : false;}

	// ��¤�����⴦��,��¤������������
	bool IsXLHSet(void){	                return (mAppCtrlWord&0x00001000) ? true : false;}
	// ѡ��������˫�����������·���öԻ���
	bool IsLBtnDblClkRouteSet() {	        return (mAppCtrlWord&0x00002000) ? true : false;}
	// �Ƿ���ƽ�·׼���ñ�־
	bool IsDrawRouteOkMark() {	            return (mAppCtrlWord&0x00004000) ? true : false;}
	// �Ƿ���ʾ�����г�����
	bool IsShowReverseTrainId(){	        return (mAppCtrlWord&0x00008000) ? true : false;}
	// ͳ�������ԲȦ��ʾλ�� ��λ����ʾ����� ����۽�
	bool IsEarlyLateTimeDrawRuijiao(){	    return (mAppCtrlWord&0x00010000) ? true : false;}
	// �Ƿ�����೵ѡ��
	bool IsCanSelMutiTrain(void){	        return (mAppCtrlWord&0x00020000) ? true : false;}
	// �Ƿ����������ʾ����
	bool IsElecEngineShowDianzi(void){	        return (mAppCtrlWord&0x00040000) ? true : false;}
	// �ͳ�ͼ��������ʵ��ͨ���Ƿ�ͳ�Ʒ���
	bool IsTrainStayRealPassStatisticDepart(void){	        return (mAppCtrlWord&0x00080000) ? true : false;}
	// ѡ���г������Ȧ�Ƿ�Ӵ�
	bool IsSelTrainBold(void){	        return (mAppCtrlWord&0x00100000) ? true : false;}
	// ���������� ���ּ���ͣ�����Ƿ���λ��
	bool IsTechStopFlagNotSame(void){	        return (mAppCtrlWord&0x00200000) ? true : false;}
	// ������,�г����5���ӣ���5���ӣ����ڣ�Ӧ��ʾ��Ȧ���֣�5����������ʾΪ��Ȧ����
	bool IsHrbStaticEarlyLateTimeColor(void){	        return (mAppCtrlWord&0x00400000) ? true : false;}
	// ������,������Ϣ�ڽ�������ʾΪ����ɫ
	bool IsHrbTransportColor(void){	        return (mAppCtrlWord&0x00800000) ? true : false;}
	// ������,�����ͳ��ʹ�ô���
	bool IsHrbEarlyLateTimeBold(void){	        return (mAppCtrlWord&0x01000000) ? true : false;}
	// �Զ�����
	bool IsEnableAutoAdjust(void){	        return (mAppCtrlWord&0x02000000) ? true : false;}
	// �������ص㳵�ɵ��޸Ŀ���
	bool IsCheckCrhAndImpTrackChange(void){	        return (mAppCtrlWord&0x04000000) ? true : false;}
	
	// --------------------APP Ctrl Ex-----------------------
	bool IsInputAbsStopInfo(){	    return (mAppCtrlWordEx&0x00000001) ? true : false;}
	bool IsSetNoLateEarlyTime(){	    return (mAppCtrlWordEx&0x00000002) ? true : false;}
	bool IsUpAndDownTimeView(){	    return (mAppCtrlWordEx&0x00000004) ? true : false;}
	bool IsLimitNxtNodeBackPreNode(){	    return (mAppCtrlWordEx&0x00000008) ? true : false;}

    bool IsDrawLanzhouAbsInhibitMark(){	    return (mAppCtrlWordEx&0x00000010) ? true : false;}
	bool IsDrawLanzhouAbsSlowMark(){	    return (mAppCtrlWordEx&0x00000020) ? true : false;}
	bool IsGenLanzhouAbsEleFixMark(){	    return (mAppCtrlWordEx&0x00000040) ? true : false;}
	bool IsDrawLanzhouEngine(){	    return (mAppCtrlWordEx&0x00000080) ? true : false;}

	bool IsDrawSideNamePerFourHours(){	    return (mAppCtrlWordEx&0x00000100) ? true : false;}
	bool IsEnableTaiyuanCopyTrain(){	    return (mAppCtrlWordEx&0x00000200) ? true : false;}
	bool IsEnableShuaiguaManage(){	    return (mAppCtrlWordEx&0x00000400) ? true : false;}
	bool IsEnableModifySideByKeyboard(){	    return (mAppCtrlWordEx&0x00000800) ? false : true;}

	bool IsEnableInhibitDrawBlackArc(){	    return (mAppCtrlWordEx&0x00001000) ? true : false;}
	bool IsEnableNoDrawLineInNoSameSubTable(){	    return (mAppCtrlWordEx&0x00002000) ? true : false;}
	bool IsEnableReverseShowSelTrainRect(void){	    return (mAppCtrlWordEx&0x00004000) ? true : false;}
	// ͬһ�ֶ�����ͬ��վ���Ƿ�����
	bool IsEnableLeapLineInSameSubTable(){	    return (mAppCtrlWordEx&0x00008000) ? true : false;}
	// ̫ԭ�ֵ�����ͨ����Ҫ����
	bool IsStayToPassNeedPwd(){	        return (mAppCtrlWordEx&0x00010000) ? true : false;}
	// ������, �����ʾ�ֽ�������
	bool IsShowStaticTimeAtMouse(){	    return (mAppCtrlWordEx&0x00020000) ? true : false;}
	// ������, �����ͳ����ȫ���������ļ�
	bool IsStaticsTimeCompleteByTGDEF(){	    return (mAppCtrlWordEx&0x00040000) ? true : false;}
	// ���ݾ�, ���ý�·�Դ�
	bool IsCanDirectSetRouteTriger(){	    return (mAppCtrlWordEx&0x00080000) ? true : false;}
	// ���ݾ�, ��ʾ�����ؽ�·�Դ���־
	bool IsShowRouteStatusFlag(){	    return (mAppCtrlWordEx&0x00100000) ? true : false;}
	// ������, ��ʾ����ͼ��������˸
	bool IsFlashBasicTrain(){	    return (mAppCtrlWordEx&0x00200000) ? true : false;}
	// ���ݾ�, ��Сͣ��ʱ��ͼ���ͣ������
	bool IsEnableMinTimeTechStop(){	    return (mAppCtrlWordEx&0x00400000) ? true : false;}
	// ������, �۷�������ʾ��������
	bool IsShowZheFanTrainID(){	    return (mAppCtrlWordEx&0x00800000) ? true : false;}	
	//	���ݾ�, �޸��г��ɵ��������������
	bool IsShowWarningDlgByChangeSide(){ return (mAppCtrlWordEx&0x01000000) ? true : false; }
	//  �޸��г��ɵ������ĵ��������ֱ�����ɵ�������
	bool IsGenDCMDByChangeSide(){  return (mAppCtrlWordEx&0x02000000) ? true : false; }
	//  ���Ͼ�, ���ƿ����ε�������ҵ�г�
	bool IsEnableDrawAbsWorkTrain(){  return (mAppCtrlWordEx&0x04000000) ? true : false; }
	//  ���ݾ�, �ƻ��߲������ڼ��������ͨ�Ա�����ʾ
	bool IsParaDlgCheckEntryConnect(){  return (mAppCtrlWordEx&0x08000000) ? true : false; }
	//  �Ϻ���, ��վҳ��������ʾ��Χ����ֹվ������
	bool IsWideStationView(){  return (mAppCtrlWordEx&0x10000000) ? true : false; }
	//  �Ϻ���, ����ʾ����������
	bool IsNotShowTipText(void){	    return (mAppCtrlWordEx&0x20000000) ? true : false;}

	// --------------------Mark Ctrl-------------------
	bool IsCanCopyMark(void)   {	        return (mMarkFuncCtrlWord&0x00000001) ? true : false;}
	bool IsCanDeleteMark(void)   {	        return (mMarkFuncCtrlWord&0x00000002) ? true : false;}
	bool IsCanGenMarkDCmd(void)   {	        return (mMarkFuncCtrlWord&0x00000004) ? true : false;}
    // ̫ԭ�� ֱ���޸���ֹʱ��
	bool IsDirectModifyBlockTime(void)   {	return (mMarkFuncCtrlWord&0x00000008) ? true : false;}
	// ������ ����ʩ�����ŵĽ���״̬�Ĳ���
	bool IsCanModifyBlockStatus(void)	{	return (mMarkFuncCtrlWord&0x00000010) ? true : false;}
	// ������ ����ʩ�������Զ�����������������ʾλ��
	bool IsEnableDiffShowBlockPostion(void)	{	return (mMarkFuncCtrlWord&0x00000020) ? true : false;}
	
	// ---------------------TG Ctrl---------------------
	bool IsCanCopyTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000001)  ? true : false;}
	bool IsCanRenameTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000002) ? true : false;}
	bool IsCanAdjustTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000004) ? true : false;}
	bool IsCanSetNebSectionRefuseTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000008) ? true : false;}
	bool IsCanModifyTrainInOutMark(void)   {	        return (mTrainFuncCtrlWord&0x00000010) ? true : false;}
	bool IsCanAddTransportInfo(void)   {	        return (mTrainFuncCtrlWord&0x00000020) ? true : false;}
	bool IsCanCalcEarlylateTime(void)   {	        return (mTrainFuncCtrlWord&0x00000040) ? true : false;} // no use
	bool IsCanStopTrainAbs(void)   {	        return (mTrainFuncCtrlWord&0x00000080) ? true : false;}
	bool IsCanModifyRoute(void)   {	        return (mTrainFuncCtrlWord&0x00000100) ? true : false;}
	bool IsCanModifySide(void)   {	        return (mTrainFuncCtrlWord&0x00000200) ? true : false;}
	bool IsCanModifyZFTrainId(void)   {	        return (mTrainFuncCtrlWord&0x00000400) ? true : false;}
	bool IsCanHatTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000800) ? true : false;}
	bool IsCanEngineTrain(void)   {	        return (mTrainFuncCtrlWord&0x00001000) ? true : false;}
	bool IsCanModifyDepartSJ(void)   {	        return (mTrainFuncCtrlWord&0x00002000) ? true : false;}
	bool IsCanDeleteTrain(void)   {	        return (mTrainFuncCtrlWord&0x00004000) ? true : false;}
	bool IsCanPASSToStayCnvt(void)   {	        return (mTrainFuncCtrlWord&0x00008000) ? true : false;}
	bool IsCanTechStop(void)   {	        return (mTrainFuncCtrlWord&0x00010000) ? true : false;}
    bool IsCannotMeetTrain(void){	        return (mTrainFuncCtrlWord&0x00020000) ? true : false;}
	bool IsCanSetTrigeRouteFlag(void){	    return (mTrainFuncCtrlWord&0x00040000) ? true : false;}
	bool IsCanSetFjkLateTime(void){	    return (mTrainFuncCtrlWord&0x00080000) ? true : false;}
	bool IsCanSetPressure(void){	    return (mTrainFuncCtrlWord&0x00100000) ? true : false;} // no use
	bool IsCanMutiTrainRecStayToPass(void){	    return (mTrainFuncCtrlWord&0x00200000) ? true : false;}
	bool IsCanBianzu(void){	    return (mTrainFuncCtrlWord&0x00400000) ? true : false;} // no use
	bool IsCanAutoAjust(void){	    return (mTrainFuncCtrlWord&0x00800000) ? true : false;}
	
		
	// ���Կ���
	bool IsCanModifyStationIdx(void) { return (mDebugCtrlWord&0x00000001) ? true : false;}
public:
	//wintg_operate
	bool IsSecretTrainNotVisible(void) { return (m_wintg_oper_right&0x8) ?  true : false;}

public:
	void SetLimitModifyTimeLen(int b){ m_nLimitTimeLen = b;}
	int  GetLimitModifyTimeLen(void) {return m_nLimitTimeLen;}
	void SetLimitModifyActTimeFlag(int limit)
	{ 
		if(limit>0)
			m_bLimitModifyActTime = true;
		else
			m_bLimitModifyActTime = false;
	}
	bool IsLimitModifyTime() {return m_bLimitModifyActTime;}
	
	void SetShiftSchdCanModify(int b){ m_nModifyShiftSchd = b;}
	int  GetShiftSchdCanModify(void) {return m_nModifyShiftSchd;}

	void SetBasicSchdCanModify(int b){ m_nModifyBasicSchd = b;}
	int  GetBasicSchdCanModify(void) {return m_nModifyBasicSchd;}

	void SetOperateRight(BYTE right){ m_oper_right = right;}
	BYTE GetOperateRight()
	{ 
		if(m_nModifyBasicSchd>0)
			return m_oper_right;
		else
			return 0;
	}
	void SetWinTGOperateRight(DWORD right){ m_wintg_oper_right = right;}
	
	void SetLeapLineFlag(BYTE flag){m_nLeapLineFlag = flag;}
	BYTE GetLeapLineFlag(){return m_nLeapLineFlag;}

	void SetDrawTenicalStopMark(int drawflag){ m_nDrawTenicalStop = drawflag;}
	int  GetDrawTenicalStopMark(){return m_nDrawTenicalStop;}

	void SetDrawRouteOkFlag(int flag){ m_nRouteOkFlag = flag;}
	int  GetDrawRouteOkFlag(){return m_nRouteOkFlag;}
	void GetDrawRouteOkJFColor(COLORREF& jcolor, COLORREF& fcolor) {jcolor=m_jcolor; fcolor=m_fcolor;}
	CString GetDataPath(){return m_datapath;}
	void	SetDataPath(LPCSTR path){ m_datapath = path;}
	
	void SetDrawCurrTimeBar(bool draw){m_bDrawTimebar = draw;}
	bool IsDrawCurrTimeBar(){return m_bDrawTimebar;}

	void SetBackgroudBlock(USHORT dlt) {backgroudblock = dlt;}
	USHORT GetBackgroudBlock() {return backgroudblock;}

	void SetKaCtrlFlag(DWORD flag){ m_lctrlword = flag;}

	bool IsCanGenNodeIndex() { return m_genUpdateNodeIndex;}

	void SetLateEarlyTime(int nFlag) {m_nLateEarlyTime = nFlag;}
	int  GetLateEarlyTime() {return m_nLateEarlyTime;}

	void SetLineWidth(int nFlag) {m_nLineWidth = nFlag;}
	int  GetLineWidth() {return m_nLineWidth;}
	
	void SetShowLocoNo() {m_bShowTractionEngine = !m_bShowTractionEngine;}
	bool GetShowLocoNo() { return m_bShowTractionEngine; }

	void SetShowTrainType(int flag) {m_nShowTrainFlag = flag;}
	int  GetShowTrainType() { return m_nShowTrainFlag; }

	void SetShowTrainDirect(int flag) {m_nShowDirectFlag = flag;}
	int  GetShowTrainDirect() { return m_nShowDirectFlag; }

	DWORD GetColorManualReal() {return m_nColorManualReal;}

	bool IsRouteOkInhibitModifySchd() { return (m_nRouteOkInhibitModifySchd>0)?true:false;}
	bool IsMoveAfterTime() { return (m_nMoveAfterTm>0)? true:false;}
	bool IsShowSideEntryNoPassFlag(COLORREF& clr);

	void SetEnableLockSide(int enable);
	bool IsEnableLockSide() { return m_bEnableLockSide;}

	void SetElecDrawGreenDecorate(int enable);
	bool IsElecDrawGreenDecorate() { return m_bElecDrawGreenDecorate;}
		
	void EnableCZH(int enable);
	bool IsEnableCZH() { return m_bEnableCZH;}

	void EnableNewCZH(int enable);
	bool IsEnableNewCZH() { return m_bEnableNewCZH;}

	void EnableSetCZHReturnTrainID(int enable);
	bool IsSetCZHReturnTrainID() { return m_bSetReturnTrainID;}
	
	void EnableInterCityStplan(int enable);
	bool IsEnableInterCityStplan() { return mSupportInterCityStPlan;}

	void SetMoveSameCZTrain(bool b);
	bool IsMoveSameCZTrain();

	bool IsSwtichStation(int station);
	void AddSwtichStation(int station);

	bool IsNotShowSideCtcStation(int station);
	void AddNotShowSideCtcStation(int station);

	// �ж�����Ͻ������������,�Ƿ񵯳�ѡ��Ի���
	bool IsShowAcceptChioceDlg(){	return m_bIsShowAcceptChioceDlg;}

	bool IsAutoAddBlockEndTime(int type);
	int  GetAutoAddBlockEndTime();

	void LockSideNotTechStop(int b);
	bool IsLockSideNotTechStop() {return m_nLockSideNotTechStop;}	
	bool IsOnlyShowSelLine() {return m_bOnlyShowSelLine;}

	void SetProhibitDeleteRealTimeFlag(short b);
	bool IsProhibitDeleteRealTime() {return m_bProhibitDeleteRealTime;}

	int  GetADTimeStytle(){return m_nADStytle;}

	bool IsErrorSideStation(int station);
	void AddErrorSideStation(int station);
private:
	DWORD mAppWordTable[65];
	DWORD mAppExWordTable[65];
private:
	int nAutoAddBlockEndTime;
	int nAutoAddBlockTypeCount;
	int AutoAddBlockType[3];
	// �ж�����Ͻ������������,�Ƿ񵯳�ѡ��Ի���
	bool m_bIsShowAcceptChioceDlg;
	// ���ر�־
	ULONG m_lctrlword;
	// ����Ȩ��
	BYTE  m_oper_right;
	// �������Ȩ��
	DWORD  m_wintg_oper_right;
	// ����ͼ�޸Ŀ���
	int  m_nModifyBasicSchd;
    // �հ�ƻ��޸�
	int  m_nModifyShiftSchd;
	// ��Ծ��վ���Ʊ�־ 1: �ޱ�� 2: ���뽻�� 3: ����
	BYTE m_nLeapLineFlag;
	// �Ƿ���Ƽ���ͣ���־ --- 0: �����Ƽ���ͣ���־ 1: ����ͣ�� ���ƨ� 2: �Ǽ���ͣ�� ���ƨ�
	int m_nDrawTenicalStop;
	// ��·׼���ñ�־----0: ������ 1: ��ɫ�� 2: ԲȦ 3:�Ӻ� ����
	int m_nRouteOkFlag;

	// 0:ʮ�ָ�ֻ��ʾ��%10 1:ʮ�ָ���ʾ�� 2:Сʱ��
	int m_nADStytle; 
	
	COLORREF m_jcolor;
	COLORREF m_fcolor;

	// �����ͳ��
	int m_nLateEarlyTime;
	// ����
	int m_nLineWidth;
	//�Ƿ���ʾ������
	bool m_bShowTractionEngine; 	
	
	// ��ʾ�г����� 1: �ͳ� 2: ���� 3: �ͻ���
	int m_nShowTrainFlag;
	// ��ʾ�г����� 1: ���� 2: ���� 3: ������
	int m_nShowDirectFlag;

	// ����·��
	CString m_datapath;
	// ����ͼ����
	BYTE m_tgtype;


	// ��������
	USHORT  backgroudblock;
    // �Ƿ����ʱ����
	bool m_bDrawTimebar;
	

	// �����߹�����
	DWORD mTrainFuncCtrlWord;   
	// ͼ�깦����
	DWORD mMarkFuncCtrlWord; 
	// Ӧ�ù�����
	DWORD mAppCtrlWord;  
	// ���Կ���
	DWORD mDebugCtrlWord;
	// �ڵ�����
	bool m_genUpdateNodeIndex;
	// �������书��
	DWORD mAppCtrlWordEx;

	// �˹�������ɫ��ʾ��ͬ
	DWORD m_nColorManualReal;

	// �Ƿ����ùɵ�����
	bool m_bEnableLockSide;
	// true: ������������  false:��ȼ�������� 
	bool m_bElecDrawGreenDecorate;
	
	// �������
	bool m_bEnableCZH;
	bool m_bEnableNewCZH;
	bool m_bSetReturnTrainID;
	// �Ƿ�ƽ��ͬ��������г�
	bool m_bIsMoveSameCZTrain;
	// ͬ���������� 1:ϸ�� 2:����
	int m_nCZConnectLineSize;
	COLORREF m_clrCZConnectLine;

	// ʵ�ʵ���޸�ʱ�䷶Χ
	int  m_nLimitTimeLen;
	// �Ƿ������޸�ʵ�ʵ�,ֻ���������޸�ʵ�ʵ�ʱ,
	bool m_bLimitModifyActTime;

	// �ÿ��г��ķǼ���ͣ��Ĭ�������ɵ�
	bool m_nLockSideNotTechStop;
	// �Ƿ�ֻ��ʾѡ�г�վ�ߺ�
	bool m_bOnlyShowSelLine;
	// �������ͼʱ����ֹɾ��ʵ�ʵ� 0������ֹ 1����ֹ
	bool m_bProhibitDeleteRealTime;
	// 20150623�Ƿ�֧�������ǳǼʼƻ�Ҫ��
	bool mSupportInterCityStPlan; // true:������ false:ͨ��·�ּƻ�
	
public:
	//  ���ݾ�,ÿСʱ��ʾ�ɵ�����
	bool m_bDrawSideNamePerHours;
	//  ���ݾ�,�ɵ�չ��,ʼ����ʾ�ɵ�
	bool m_bDefaultShowSide;
	//  ���ݾ�,ͬ��·�����г��Ƿ�Ӵ���ʾ
	bool m_bRelationTrainBold;
	//  ���ݾ�,�޸��г���·�Ƿ����ص㳵��ʽ��Ŀ��ʾ
	bool m_bModifyRelationSetTrainImport;
	// վ��ʩ��������ʾ����
	int m_nStationBlockShowMode; // 0: ��վչ�����ʼ����ʾ 1:ֻҪ��վչ��ʱ����ʾ
	// ��·������־
	int m_nRouteTrigerMode; // 0: ����ʾ��·������־ 1: ��ʾ��·������־
	// ��·��ɴ���,�Ƿ�����޸ļƻ�
	int m_nRouteOkInhibitModifySchd;
	// ��·��������޸ļƻ����Ƿ�ƽ�ƺ�����
	int m_nMoveAfterTm;
	// ��ʾ�ɵ�����ڲ�ͬ��־--�ɵ���ɫ��ͬ
	int m_nShowSideEntryNoPass;
	// ���Ų�������߼���ʾ��ʽ: 0:����Ų��,����ʾ�뼶 1:Ĭ��ֵ,�뼶Ų��,������ʾ 2:�뼶Ų��,�뼶��ʾ
	int m_nMouseMoveLineFlag;
	// δ֪�г��Ƿ���ʾ"δ֪�г�"���� 0:����ʾ 1:Ĭ��ֵ,��ʾ
	int m_nDisplayUnknownText;
	// �Ƿ��Ż��г�����
	bool m_bVerifyTrainRect;
	// ��������Χ����
	int m_nZoomRangeX;
	int m_nZoomRangeY;
	COLORREF m_clrNoPassSide;

	std::vector<int> swtichStation;
	std::vector<int> notshowsidectcstation;
	std::vector<int> errorSideStation;

private:
	int m_nCurrentPixelPerMinute;
	
public:
	void SetPixelPerMinute(int nPixelPerMinute);
	int GetPixelPerMinute(void);
	int IncreateCurrentUnit(short count);
	int DecreateCurrentUnit(short count);

	void AddCrhStopStation(int station);
	bool IsCrhStopStation(int station);

	void GetCZHLineCfg(int &width, COLORREF& clr) { width=m_nCZConnectLineSize; clr=m_clrCZConnectLine;}
	int GetMouseMoveLineFlag() { return m_nMouseMoveLineFlag;}
	int IsDisplayUnknownText() { return m_nDisplayUnknownText;}
	bool IsVerifyTrainRect() { return m_bVerifyTrainRect;}
	int GetZoomRangeX() { return m_nZoomRangeX;}
	int GetZoomRangeY() { return m_nZoomRangeY;}
private:
	std::vector<int> mCrhStopStation; // ����������

};

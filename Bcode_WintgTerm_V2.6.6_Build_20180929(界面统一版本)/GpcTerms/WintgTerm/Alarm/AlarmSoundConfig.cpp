// AlarmSoundConfig.cpp: implementation of the CAlarmSoundConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmSoundConfig.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// ================================================================
// alarm sound file setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, IDINFO, idInfo, ALARM_ID, "Alarm ID define")

int CAlarmSoundConfig::IDINFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nID = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nLevel = atoi(token);

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nType = atoi(token);

	token = strtok(NULL, ",\r\n");
	if(token)
	{
		szAlarmTitle = token;
	}

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::IDINFO::formatString()
{
	return "; ��ʽ: id=flag��level, type, title\n"
		"; flag: 0x01λ��0-����ʾ��1-��ʾ����0x02λ��0-�ɱ����1-���ɱ����\n"
		"; leve: 0-���ɻ����壬����Ϊ��Ӧ����\n"
		"; type: 0-���ɻ����壬1-TRAIN��2-SYS\n"
		"; title����������";
}

bool CAlarmSoundConfig::IDINFO::save(ofstream& out)
{
		out << "\t" << nID << "=" << nFlag << ",\t" 
			<< nLevel << ",\t" << nType << ",\t"
			<< szAlarmTitle;
	return true;
}

int CAlarmSoundConfig::IDINFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm level setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, LEVEL_INFO, levelInfo, ALARM_LEVEL, "Alarm LEVEL define")

int CAlarmSoundConfig::LEVEL_INFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nLevel = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nIcon = atoi(token);
	if(0 > nIcon || 2 < nIcon)
		nIcon = 2;

	token = strtok(NULL, ",\r\n");
	if(token)
		szTitle = token;

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::LEVEL_INFO::formatString()
{
	return "; ��ʽ: level=flag��ico, title\n"
		"; level: ����1��2��3����������δ���壬ͬ��3��\n"
		"; flag: 0x01λ��0-����ʾ��1-��ʾ����0x02λ��0-�ɱ����1-���ɱ����\n"
		"; ico: δ����\n"
		"; title: δ����";

}

bool CAlarmSoundConfig::LEVEL_INFO::save(ofstream& out)
{
		out << "\t" << nLevel << "=" << nFlag << ",\t" 
			<< nIcon << ",\t" << szTitle;
	return true;
}

int CAlarmSoundConfig::LEVEL_INFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm type setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, TYPE_INFO, typeInfo, ALARM_TYPE, "Alarm TYPE define")

int CAlarmSoundConfig::TYPE_INFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nType = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nIcon = atoi(token);
	if(0 > nIcon || 2 < nIcon)
		nIcon = 2;

	token = strtok(NULL, ",\r\n");
	if(token)
		szTitle = token;

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::TYPE_INFO::formatString()
{
	return "; ��ʽ: type=flag��ico, title\n"
		"; type: ��������\n"
		"; flag: 0x01λ��0-����ʾ��1-��ʾ����0x02λ��0-�ɱ����1-���ɱ����\n"
		"; ico: δ����\n"
		"; title: δ����";

}

bool CAlarmSoundConfig::TYPE_INFO::save(ofstream& out)
{
		out << "\t" << nType << "=" << nFlag << ",\t" 
			<< nIcon << ",\t" << szTitle;
	return true;
}

int CAlarmSoundConfig::TYPE_INFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm sound file setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, IDFILEINFO, idfileInfo, ALARMIDS_SOUNDFILE, "Alarm sound file define")

int CAlarmSoundConfig::IDFILEINFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	static int __id = 0;
	nID = __id++;

	int nAlarmID = 0;
	
	token = strtok(ptr, "= /,");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	int num = atoi(token);
	for(int i=0; i<num; i++)
	{
		token = strtok(NULL, "/ ,");
		if(NULL == token)
			return casco::clbth::S_XLY_FALSE;
		nAlarmID = atoi(token);
		alarm_ids.push_back(nAlarmID);
	}

	token = strtok(NULL, " \n\r\t");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	soundfile = token;
	
	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::IDFILEINFO::formatString()
{
	return "; ��ʽ: =����{/AlarmID}, �����ļ�����";
}

bool CAlarmSoundConfig::IDFILEINFO::save(ofstream& out)
{
	int num = alarm_ids.size();
	out << "\t=" << num;
	for(int i=0; i<num; i++)
	{
		out << "/" << alarm_ids[i];
	}

	out << ",\t" << soundfile.c_str();
	return true;
}

int CAlarmSoundConfig::IDFILEINFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}
// ================================================================








//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_SINGLETON(CConfigData)

CAlarmSoundConfig::CAlarmSoundConfig()
{
	INITIALIZE_SIMPLEDATA(this);
}

CAlarmSoundConfig::~CAlarmSoundConfig()
{
	
}

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <gpcdef.h>
#include <sysshare.h>

BOOL CAlarmSoundConfig::loadProfile(CString szPathName)
{
	CString szPath;
	szPath.Format("%s\\sound\\alarm", szPathName);

	m_szFilename = szPath + "\\Alarm.CFG";

	m_szLocalFilename.Format("%s\\Alarm_%s.CFG", szPath, gpcdef.GetCurUserName());

	CString filename;
	if(0 != _access(m_szLocalFilename, 0))
	{
		filename = m_szFilename;
		sysprint(0, 0, "[ALARM] ʹ��ͨ�������ļ� %s", filename);
	}
	else
	{
		filename = m_szLocalFilename;
		sysprint(0, 0, "[ALARM] ʹ�ø��Ի������ļ� %s", filename);
	}

	if(0 != _access(filename, 04))
	{
		sysprint(0, 0, "[ALARM] �����ļ� %s ���ɶ�", filename);
		return FALSE;
	}

	if(load((LPCSTR)filename))
	{
		return TRUE;
	}

	sysprint(5, 0, "[ALARM] �����ļ� %s ����ȷ", filename);

	return FALSE;
}

bool CAlarmSoundConfig::saveProfile()
{
	//if(0 != _access(m_szLocalFilename, 0))
	//{
	//	if(0 != _access(m_szFilename, 04))
	//	{
	//		sysprint(5, 0, "[ALARM] ͨ�������ļ� %s ���ɶ����޷����ɸ��Ի����� %s", m_szFilename, m_szLocalFilename);
	//		return false;
	//	}

	//	if(!CopyFile(m_szFilename, m_szLocalFilename, FALSE))
	//	{
	//		sysprint(5, 0, "[ALARM] ͨ�������ļ� %s �����Ի����ļ� %s ����", m_szFilename, m_szLocalFilename);
	//		return false;
	//	}
	//	sysprint(0, 0, "[ALARM] ����ͨ�������ļ������Ի������ļ� %s", m_szLocalFilename);
	//}

	if(0 == _access(m_szLocalFilename, 0))
	{
		if(0 != _access(m_szLocalFilename, 06))
		{
			if(0 != _chmod(m_szLocalFilename, _S_IREAD | _S_IWRITE))
			{
				sysprint(5, 0, "[ALARM] ���Ի������ļ� %s ����д�������޷��޸��ļ���д����", m_szLocalFilename);
				return false;
			}
		}
	}

	if(save((LPCSTR)m_szLocalFilename))
	{
		sysprint(5, 0, "[ALARM] ������Ի������ļ� %s",  m_szLocalFilename);
	}
	else
	{
		sysprint(5, 0, "[ALARM] ������Ի������ļ� %s ʧ��",  m_szLocalFilename);
		return false;
	}

	return true;
}

CString CAlarmSoundConfig::GetSoundFileName(int nAlarmID)
{
	CString sndFile;

	for(int i=0; i<idfileInfo.size(); i++)
	{
		const vector<int>& vec = idfileInfo[i].alarm_ids;
		if(vec.end() != find(vec.begin(), vec.end(), nAlarmID))
		{
			sndFile = idfileInfo[i].soundfile.c_str();
			return sndFile;
		}
	}
	return sndFile;
}


#pragma once
#include "DEFINES.H"
unsigned short  OsGetTaskId(unsigned short *pTaskId); //�����ԣ�����
void SetSysPrintLogMask(const char *prefix,short level,unsigned short mask);
#define LOG_TYPE_DEBUG   0x0001
#define LOG_TYPE_INFO    0x0002
#define LOG_TYPE_SYS     0x0004
#define LOG_TYPE_ERROR   0x0008

#define LOG_LVL_DEBUG   1   //  
#define LOG_LVL_INFO    10  //  
#define LOG_LVL_SYS     20  // 
#define LOG_LVL_ERR     20  // 

void sysprint(short msg_level,unsigned short type_mask,char *format, ... ); //msg_level��Խ�󼶱�Խ�ߣ�type_mask �ñ��ر�ʾ����Ϣ����,0��ʾ�ܼ�¼
void LogCTCAlarms(time_t tm,char *format, ... );
void ShowLogFiles(const char *key);
int  RunShellCommand(const char *cmdline);
void SaveLogMessage(const char *key,time_t tm,const char *format,...);
void OsSleep(unsigned long tm,int flag); // flag not used!
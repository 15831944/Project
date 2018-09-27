// DynCertificate.h: interface for the CDynCertificate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNCERTIFICATE_H__6E4307D9_04D1_432B_9A23_8B4B2768BF28__INCLUDED_)
#define AFX_DYNCERTIFICATE_H__6E4307D9_04D1_432B_9A23_8B4B2768BF28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���п���ʶ��ķ���ֵ
#define AUTH_SUCCESS 0	//�ɹ�
#define AUTH_ERR_NET 1	//�������
#define AUTH_USER_NOTEXIST 2	//�û�������
#define AUTH_PWD_MISMATCH 3		//���벻ƥ��
#define AUTH_ERR_OTHER 4		//��������



class CDynCertificate  
{
public:
	int DynVerify(char* user, char* pwd);
	int DynUninit();
	int DynInit();
	CDynCertificate();
	virtual ~CDynCertificate();

};

#endif // !defined(AFX_DYNCERTIFICATE_H__6E4307D9_04D1_432B_9A23_8B4B2768BF28__INCLUDED_)

// TCC.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "tcc_base.h"
#include "GpcInfoProvider.h"
#include "sysshare.h"

namespace TCC_C3
{

map<int, LINE_STRUCT> line_map;
map<stano_t, TCC_STRUCT> tcc_map;
int gBlockKmsysCode = 0;
		
bool kmmark_t::Equal(const kmmark_t& rhs) const
{
	return flag == rhs.flag && info == rhs.info;
}

bool kmmark_t::Less(const kmmark_t& rhs, int builtin_id) const
{
	if(line_map.end() == line_map.find(builtin_id))
	{
		ASSERT(FALSE);
		return info < rhs.info;
	}
	const LINE_STRUCT& line = line_map[builtin_id];

	// 0000+000Ϊ��Сֵ
	// 9999+999Χ���ֵ
	if(info.val == 0)
	{
		return rhs.info.val != 0;
	}
	else if(info.val == 9999999)
	{
		return false;
	}
	else if(rhs.info.val == 0)
	{
		return false;
	}
	else if(rhs.info.val == 9999999)
	{
		return info.val != 9999999;
	}
	else if(flag == rhs.flag)
	{
		kmmark_t k1 = *this,	k2 = rhs;
		for(int i=0; i<line.km_codes.size(); i++)
		{
			const LINE_STRUCT::_KM_CODE& kmcode = line.km_codes[i];

			if(flag == line.km_codes[i].nCode)
			{
				if(!line.km_codes[i].bInc)
				{
					k1 = rhs;
					k2 = *this;
				}
				break;
			}
		}

		return k1.info < k2.info;
		//if (k1.info.val < k2.info.val)
		//	return true;
		//else if (k1.info.val == k2.info.val)
		//{
		//	if (k1.info.km_cl < k2.info.km_cl)
		//		return true;
		//	else if (k1.info.km_cl == k2.info.km_cl)
		//		return k1.info.m_cl < k2.info.m_cl;
		//}
		//return false;
	}
	else 
	{
		float left = 0,	right = 0;
		bool bFindLeft = false,	bFindRight = false;
		long toleft = 0,	toright = 0;
		long toleft2= 0,	toright2= 0;

		int n = 0;
		for(int i=0; i<line.gd_stas.size(); i++)
		for(int j=0; j<line.gd_stas[i].secs.size(); j++)
		{
			n++;

			const LINE_STRUCT::_STA_GD::_SEC_GD& sec = line.gd_stas[i].secs[j];

			if(!bFindLeft)
			{
				if(this->Equal(sec.nKmBegin))
				{
					left = n - 0.5;
					bFindLeft = true;
				}
				else if(this->Equal(sec.nKmEnd))
				{
					left = n + 0.5;
					bFindLeft = true;
				}
				else if(sec.nKmBegin.flag == sec.nKmEnd.flag)
				{
					if(this->flag == sec.nKmBegin.flag)
					{
						if(this->GreaterOrEqual(sec.nKmBegin, builtin_id) && this->LessOrEqual(sec.nKmEnd, builtin_id))
						{
							left = n;
							bFindLeft = true;
						}
					}
				}
				else
				{
					if(this->flag == sec.nKmBegin.flag)
					{
						toleft2 = info.val - sec.nKmBegin.info.val;
						toleft2 = abs(toleft2);
						if(toleft2 < toleft || toleft == 0)
						{
							left = n - 0.3;
							toleft = toleft2;
						}
					}
					else if(this->flag == sec.nKmEnd.flag)
					{
						toleft2 = info.val - sec.nKmEnd.info.val;
						toleft2 = abs(toleft2);
						if(toleft2 < toleft || toleft == 0)
						{
							left = n + 0.3;
							toleft = toleft2;
						}
					}
				}
			}
			
			if(!bFindRight)
			{
				if(rhs.Equal(sec.nKmBegin))
				{
					right = n - 0.5;
					bFindRight = true;
				}
				else if(rhs.Equal(sec.nKmEnd))
				{
					right = n + 0.5;
					bFindRight = true;
				}
				else if(sec.nKmBegin.flag == sec.nKmEnd.flag)
				{
					if(rhs.flag == sec.nKmBegin.flag)
					{
						if(rhs.GreaterOrEqual(sec.nKmBegin, builtin_id) && rhs.LessOrEqual(sec.nKmEnd, builtin_id))
						{
							right = n;
							bFindRight = true;
						}
					}
				}
				else
				{
					if(rhs.flag == sec.nKmBegin.flag)
					{
						toright2 = rhs.info.val - sec.nKmBegin.info.val;
						toright2 = abs(toright2);
						if(toright2 < toright || toright == 0)
						{
							right = n - 0.3;
							toright = toright2;
						}
					}
					else if(rhs.flag == sec.nKmEnd.flag)
					{
						toright2 = rhs.info.val - sec.nKmEnd.info.val;
						toright2 = abs(toright2);
						if(toright2 < toright || toright == 0)
						{
							right = n + 0.3;
							toright = toright2;
						}
					}
				}
			}
		}

		/*int left = 0,	right = line.km_codes.size();
		for(int i=0; i<line.km_codes.size(); i++)
		{
			if(flag == line.km_codes[i].nCode)
				left = i;
			if(rhs.flag == line.km_codes[i].nCode)
				right = i;
		}*/

		return left < right;
	}
}

bool kmmark_t::LessOrEqual(const kmmark_t& rhs, int builtin_id) const
{
	return Less(rhs, builtin_id) || Equal(rhs);
}
		
bool kmmark_t::Unequal(const kmmark_t& rhs) const
{
	return !Equal(rhs);
}

bool kmmark_t::Greater(const kmmark_t& rhs, int builtin_id) const
{
	return !LessOrEqual(rhs, builtin_id);
}

bool kmmark_t::GreaterOrEqual(const kmmark_t& rhs, int builtin_id) const
{
	return !Less(rhs, builtin_id);
}

long kmmark_t::DistanceTo(const kmmark_t& rhs, int builtin_id) const
{
	long length = 0;

	return 0;
}

bool ITccClientInfoProvider::GetKmsysCode(int builtin_id, const CString& szPrex, int *pnCode) 
{
	ASSERT(FALSE);
	if(line_map.end() == line_map.find(builtin_id))
		return false;

	bool bRet = false;

	const LINE_STRUCT& line = line_map[builtin_id];
	
	CString sz = szPrex;
	sz.MakeUpper();
	if(sz.IsEmpty())
		sz = line.szDefKmsysPrex;

	for(int i=0; i<line.km_codes.size(); i++)
	{
		if(sz == line.km_codes[i].szPrex)
		{
			*pnCode = line.km_codes[i].nCode;
			return true;
		}
	}

	return false;
}

bool ITccClientInfoProvider::GetKmsysPrex(int builtin_id, const kmmark_t& kmmark, CString& szPrex) 
{
	if(line_map.end() == line_map.find(builtin_id))
		return false;

	const LINE_STRUCT& line = line_map[builtin_id];
	for(int i=0; i<line.km_codes.size(); i++)
	{
		LINE_STRUCT::_KM_CODE kmcode = line.km_codes[i];

		if(kmmark.flag == kmcode.nCode && kmmark.info.val >= kmcode.nMIN && kmmark.info.val < kmcode.nMAX)
		{
			szPrex = kmcode.szPrex;
			return true;
		}
	}
	return false;
}

int  ITccClientInfoProvider::GetBlockKmsysCode()
{
	return gBlockKmsysCode;
}

bool parseKmMark(
	CString szKmMark, 
	CString& szPrex, CString& szKm, CString& szKcl,
	CString& szM, CString& szMcl)
{
	//ȥ���ո�
	szKmMark.Replace(" ", "");

	//�Ӻ���ǰ����,��Ϊǰ׺���ܰ�����ֵ,����������

	//ȡa
	for(int i=szKmMark.GetLength()-1; i>=0; i--)
	{
		if(::isdigit( szKmMark.GetAt(i) ))
			break;
	}
	szMcl = szKmMark.Right( szKmMark.GetLength() - (i+1) );
	szKmMark.TrimRight(szMcl);

	//ȡ��ֵ
	for(i=szKmMark.GetLength()-1; i>=0; i--)
	{
		if(!::isdigit( szKmMark.GetAt(i) ))
			break;
	}
	szM = szKmMark.Right( szKmMark.GetLength() - (i+1) );
	szKmMark.TrimRight(szM);
	
	if(!szKmMark.IsEmpty() && szKmMark.GetAt(i) == '+')
	{
		szKmMark.TrimRight("+");

		//ȡAA��
		for(i=szKmMark.GetLength()-1; i>=0; i--)
		{
			if(::isdigit( szKmMark.GetAt(i) ))
				break;
		}
		szKcl = szKmMark.Right( szKmMark.GetLength() - (i+1) );
		szKmMark.TrimRight(szKcl);

		//ȡ��ֵ
		for(i=szKmMark.GetLength()-1; i>=0; i--)
		{
			if(!::isdigit( szKmMark.GetAt(i) ))
				break;
		}
		szKm = szKmMark.Right( szKmMark.GetLength() - (i+1) );
		szKmMark.TrimRight(szKm);
	}
	else
	{
		DWORD val = atol(szM);
		szKm.Format("%d", val/1000);
		szM.Format("%d", val%1000);
	}

	//ȡǰ׺
	szPrex = szKmMark;
	if(szKm.IsEmpty())
		szKm = "0";
	if(szM.IsEmpty())
		szM = "0";

	return true;
}

bool parseKmMark(const LINE_STRUCT& line, CString szKmMark, kmmark_t *pKmMark)
{
	*pKmMark = 0;
	if(NULL == pKmMark)
		return false;

	CString szPrex, szKm, szKcl, szM, szMcl;
	if(!parseKmMark(szKmMark, szPrex, szKm, szKcl, szM, szMcl))
		return false;

	kmmark_t km;

	if(!szMcl.IsEmpty())
	{
		if(szMcl != "a")
			return false;
		km.info.m_cl = 1;
	}

	//ȡ��ֵ
	DWORD val = atol(szM);
	if(val > 1000)
		return false;
	km.info.val = val;

	val = atol(szKm);
	if(val > 10000)
		return false;
	km.info.val = val * 1000 + km.info.val;
	
	km.info.km_cl = StringtoKcl(szKcl);
	if(!szKcl.IsEmpty() && km.info.km_cl == 0)
		return false;

	//ȡǰ׺
	if(0 >= line.builtin_id)
		km.flag = gBlockKmsysCode;
	else
	{
		szPrex.MakeUpper();
		if(szPrex.IsEmpty())
			szPrex = line.szDefKmsysPrex;

		for(int i=0; i<line.km_codes.size(); i++)
		{
			LINE_STRUCT::_KM_CODE kmcode = line.km_codes[i];

			if(szPrex != kmcode.szPrex)
				continue;

			if(km.info.val != 0 && km.info.val != 9999999
				&& (km.info.val< kmcode.nMIN || km.info.val >= kmcode.nMAX)
				)
				continue;
				
			km.flag  = kmcode.nCode;
			break;
		}
		if(i >= line.km_codes.size())
			return false;
	}
	*pKmMark = km;

	if(0 >= line.builtin_id)
		return (pKmMark->info.val == 0 || pKmMark->info.val == 9999999) 
		&& pKmMark->info.km_cl == 0 && pKmMark->info.m_cl ==0;
	else
		return pKmMark->flag != -1;
}

const LINE_STRUCT* GetLineStruct(int builtin_id)
{
	if(line_map.find(builtin_id) == line_map.end())
		return NULL;
	return &line_map[builtin_id];
}

/*
 * �����鵵
 * input: 
 *     builtin_id �ڲ���·��ţ������ظ���
 *     nKmBegin   ��Ҫ�鵵����ʼ�����
 *     nKmEnd     ��Ҫ�鵵�Ľ��������
 * output:
 *     nKmBegin   �鵵�����ʼ�����
 *     nKmEnd     �鵵��Ľ��������
 *     nHitTrackNum �鵵������·��
 */
bool ITccClientInfoProvider::AdjustKmMarker(int builtin_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd, int *nHitTrackNum)
{
	//invalid input data
	if(NULL == nKmBegin || NULL == nKmEnd || NULL == nHitTrackNum)
		return false;
	
	*nHitTrackNum = 0;

	if(line_map.end() == line_map.find(builtin_id))
		return false;
	const LINE_STRUCT& line = line_map[builtin_id];

	if(nKmEnd->LessOrEqual(*nKmBegin, builtin_id))
		return false;

	int  nSecNum = 0;
	bool bFindBegin = false;
	bool bFindEnd   = false;
	for(int i=0; i<line.gd_stas.size() && !(bFindBegin && bFindEnd); i++)
	{
		switch(line.nOrientation)
		{
		case DIR_DOWN:
			break;
		case DIR_UP:
			break;
		case DIR_DUAL:
			break;
		default:
			break;
		}
		for(int j=0; j<line.gd_stas[i].secs.size() && !(bFindBegin && bFindEnd); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD& sec = line.gd_stas[i].secs[j];			
			
			if(nKmEnd->LessOrEqual(sec.nKmBegin, builtin_id) || nKmBegin->GreaterOrEqual(sec.nKmEnd, builtin_id))
				continue;
				
			if(!bFindBegin
				&& nKmBegin->GreaterOrEqual(sec.nKmBegin, builtin_id)
				&& nKmBegin->Less(sec.nKmEnd, builtin_id))
			{
				*nKmBegin = sec.nKmBegin;
				bFindBegin= true;
				nSecNum ++;
			}
			if(!bFindEnd
				&& nKmEnd->Greater(sec.nKmBegin, builtin_id)
				&& nKmEnd->LessOrEqual(sec.nKmEnd, builtin_id))
			{
				*nKmEnd = sec.nKmEnd;
				bFindEnd= true;
				nSecNum ++;
			}
			if(nKmBegin->Less(sec.nKmBegin, builtin_id)
				&& nKmEnd->Greater(sec.nKmEnd, builtin_id))
				nSecNum ++;
		}
	}
	//ASSERT(bFindBegin && bFindEnd);
	
	*nHitTrackNum = nSecNum;
	return 0 <= nSecNum;
}		

// ����5������������վ������ ���ù����
bool ITccClientInfoProvider::GetFullLimitKmMarker(int builtin_id, kmmark_t *nKmBegin, kmmark_t *nKmEnd, int *nHitFullSecNum)
{
	//invalid input data
	if(NULL == nKmBegin || NULL == nKmEnd || NULL == nHitFullSecNum)
		return false;

	*nHitFullSecNum = 0;

	if(line_map.end() == line_map.find(builtin_id))
		return false;
	const LINE_STRUCT& line = line_map[builtin_id];

	if(nKmEnd->LessOrEqual(*nKmBegin, builtin_id))
		return false;

	int nHit_In = 0, nHit_Out = 0, nHit_Cross = 0;
	kmmark_t nSecKmBegin = 0,	nSecKmEnd = 0;
	for(int i=0; i<line.fullsecs.size(); i++)
	{
		const LINE_STRUCT::_FULLSEC& fullsec = line.fullsecs[i];

		if(nKmBegin->GreaterOrEqual(fullsec.nKmBegin, builtin_id)
			&& nKmEnd->LessOrEqual(fullsec.nKmEnd, builtin_id))
		{
			if(0 == nHit_In)
			{
				nSecKmBegin = fullsec.nKmBegin;
				nSecKmEnd   = fullsec.nKmEnd;
			}
			nHit_In ++;
			//return true;
		}
		else if(nKmBegin->LessOrEqual(fullsec.nKmBegin, builtin_id)
			&& nKmEnd->GreaterOrEqual(fullsec.nKmEnd, builtin_id))
		{
			if(0 == nHit_Out + nHit_In)
			{
				nSecKmBegin = fullsec.nKmBegin;
				nSecKmEnd   = fullsec.nKmEnd;
			}

			nHit_Out ++;
		}
		else if(nKmBegin->GreaterOrEqual(fullsec.nKmBegin, builtin_id) && nKmBegin->Less(fullsec.nKmEnd, builtin_id)
			|| nKmEnd->Greater(fullsec.nKmBegin, builtin_id) && nKmEnd->LessOrEqual(fullsec.nKmEnd, builtin_id))
		{
			if(0 == nHit_In + nHit_Out + nHit_Cross)
			{
				nSecKmBegin = fullsec.nKmBegin;
				nSecKmEnd   = fullsec.nKmEnd;
			}

			nHit_Cross ++;
		}
	}
	if(1 == nHit_In || 
		1 == nHit_In + nHit_Out ||
		1 == nHit_In + nHit_Out + nHit_Cross)
	{
		*nKmBegin = nSecKmBegin;
		*nKmEnd   = nSecKmEnd;
		*nHitFullSecNum = 1;
	}
	else
	{
		*nHitFullSecNum = nHit_In + nHit_Out + nHit_Cross;
	}

	return *nHitFullSecNum > 0;
}

/*
 * ����վ������ʼ�����
 * input: 
 *       builtin_id
 *       nKmBegin
 *       nKmBegin
 *       maindir
 * output:
 *       tcclist
 *
 * ע��:
 *     ����������nKmBegin��nKmEnd�鵵.
 */
bool ITccClientInfoProvider::GetHitTccList(int builtin_id, dir_t dir, kmmark_t nKmBegin, kmmark_t nKmEnd, vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tcclist)
{
	//invalid input data
	if(nKmEnd.LessOrEqual(nKmBegin, builtin_id))
		return false;
	
	if(line_map.end() == line_map.find(builtin_id))
		return false;
	const LINE_STRUCT& line = line_map[builtin_id];

	if(nKmBegin.GreaterOrEqual(line.nKmEnd, builtin_id) || nKmEnd.LessOrEqual(line.nKmBegin, builtin_id))
		return false;

	stano_t nTccID = line.nLineFirstTccID;
	while(nTccID)
	{
		TCC_STRUCT tcc = tcc_map[nTccID];

		nTccID = 0;
		const TCC_STRUCT::_LINE& tcc_line = tcc.line_map[builtin_id];
		ASSERT(tcc_line.builtin_id == builtin_id);

				kmmark_t nTccKmBegin = 0, nTccKmEnd = 0;
				switch(line.nOrientation)
				{
				case DIR_DOWN:	// ������
					switch(dir)
					{
					case DIR_DOWN: //��������
					case DIR_UNKNOWN:
						nTccKmBegin = tcc_line.nKmBegin;
						nTccKmEnd   = tcc_line.nKmEnd;
						break;
					case DIR_UP:	//��������
						nTccKmBegin = tcc_line.nKmBegin_R;
						nTccKmEnd   = tcc_line.nKmEnd_R;
						break;
					case DIR_DUAL: //˫��
						if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
						{
							if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}
						else
						{
							if(tcc_line.nKmBegin.Unequal(0))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}

						if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
							nTccKmEnd = tcc_line.nKmEnd;
						else
							nTccKmEnd = tcc_line.nKmEnd_R;
						break;
					default:
						ASSERT(FALSE);
						break;
					}
					break;

				case DIR_UP:	//������
					switch(dir)
					{
					case DIR_DOWN: //��������
						nTccKmBegin = tcc_line.nKmBegin_R;
						nTccKmEnd   = tcc_line.nKmEnd_R;
						break;
					case DIR_UP:	//��������
					case DIR_UNKNOWN:
						nTccKmBegin = tcc_line.nKmBegin;
						nTccKmEnd   = tcc_line.nKmEnd;
						break;
					case DIR_DUAL: //˫��
						if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
						{
							if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}
						else
						{
							if(tcc_line.nKmBegin.Unequal(0))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}
						
						if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
							nTccKmEnd = tcc_line.nKmEnd;
						else
							nTccKmEnd = tcc_line.nKmEnd_R;
						break;

					default:
						ASSERT(FALSE);
						break;
					}
					break;

				case DIR_DUAL:	//˫����
					switch(dir)
					{
					case DIR_DOWN: //������
						nTccKmBegin = tcc_line.nKmBegin;
						nTccKmEnd   = tcc_line.nKmEnd;
						break;
					case DIR_UP:	//������
						nTccKmBegin = tcc_line.nKmBegin_R;
						nTccKmEnd   = tcc_line.nKmEnd_R;
						break;
					case DIR_DUAL: //˫��
					case DIR_UNKNOWN:
						if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
						{
							if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}
						else
						{
							if(tcc_line.nKmBegin.Unequal(0))
								nTccKmBegin = tcc_line.nKmBegin;
							else
								nTccKmBegin = tcc_line.nKmBegin_R;
						}

						if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
							nTccKmEnd = tcc_line.nKmEnd;
						else
							nTccKmEnd = tcc_line.nKmEnd_R;
						break;
					default:
						ASSERT(FALSE);
						break;
					}
					break;
				default:
					ASSERT(FALSE);
					break;
				}

				RAW_STOCK_COMMAND::_TCC_STRUCT tcc_km;
				tcc_km.nID = tcc.nTccID;
				tcc_km.nTccLineID = tcc_line.tccline_id;

				nTccID = tcc_line.nNextTccID;
				// [L0, L1]=[nKmBegin, nKmEnd] ���ٷ�Χ
				// [T0, T1]=[nTccKmBegin, nTccKmEnd] �пع�Ͻ��Χ
				// {F0, F1} ���า�Ǳ�־

				if(nTccKmBegin.GreaterOrEqual(nKmEnd, builtin_id)
					|| nTccKmEnd.LessOrEqual(nKmBegin, builtin_id)
					)
				{	//b��a������
					continue;
				}

				// �ٽ����վ��
				if(nTccKmBegin.LessOrEqual(nKmBegin, builtin_id))
					tcc_km.nKmMarkFlag[0] = 0;
				else
					tcc_km.nKmMarkFlag[0] = line.bKmmarkAscend ? 2 : 1;//nTccKmBegin;

				if(nTccKmEnd.GreaterOrEqual(nKmEnd, builtin_id))
					tcc_km.nKmMarkFlag[1] = 0;
				else
					tcc_km.nKmMarkFlag[1] = line.bKmmarkAscend ? 1 : 2;//nTccKmEnd;

				tcclist.push_back(tcc_km);
				
				/*if(nTccKmBegin.GreaterOrEqual(nKmBegin, builtin_id))	
				{
					tcc_km.nKmMarkFlag[0] = line.bKmmarkAscend ? 2 : 1;//nTccKmBegin;
					if(nTccKmEnd.LessOrEqual(nKmEnd, builtin_id))//�м�վ
					{
						tcc_km.nKmMarkFlag[1] = line.bKmmarkAscend ? 1 : 2;//nTccKmEnd;
						tcclist.push_back(tcc_km);
					}
					else if(nTccKmBegin.Less(nKmEnd, builtin_id))//��βվ
					{
						tcc_km.nKmMarkFlag[1] = 0;//nKmEnd;
						tcclist.push_back(tcc_km);
					}
				}
				else if(nTccKmEnd.Greater(nKmBegin, builtin_id))//��ͷվ
				{
					tcc_km.nKmMarkFlag[0] = 0;//nKmBegin;
					if(nTccKmEnd.Greater(nKmEnd, builtin_id))//ֻ��һ��վ
						tcc_km.nKmMarkFlag[1] = 0;//nKmEnd;
					else
						tcc_km.nKmMarkFlag[1] = line.bKmmarkAscend ? 1 : 2;//nTccKmEnd;
					
					tcclist.push_back(tcc_km);
				}*/
	}
	return true;
}		

bool ITccClientInfoProvider::GetLineList(std::vector<int>& linelist)
{
	linelist.clear();

	for(std::map<int, LINE_STRUCT>::iterator iter = line_map.begin();
		iter != line_map.end();
		iter++)
	{
		linelist.push_back(iter->second.builtin_id);
	}

	return linelist.size() != 0;
}

bool ITccClientInfoProvider::GetLineList(stano_t nTccID, std::vector<int>& linelist)
{
	linelist.clear();

	if(tcc_map.end() == tcc_map.find(nTccID))
		return false;
	const TCC_STRUCT& tcc = tcc_map[nTccID];

	for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter ++)
	{
		linelist.push_back(iter->second.builtin_id);
	}

	return linelist.size() != 0;
}

bool ITccClientInfoProvider::GetLineList(stano_t nStaID, stano_t nStaID2, std::vector<int>& lines)
{
	if((0 == nStaID )|| ( 0 == nStaID2))
	{
		if(0 != nStaID)
		{
			GetLineList(nStaID,lines);
		}
		else if(0 != nStaID2)
		{
			GetLineList(nStaID2,lines);
		}
	}
	else
	{
		lines.clear();

		if(tcc_map.end() == tcc_map.find(nStaID))
			return false;
		const TCC_STRUCT& tcc = tcc_map[nStaID];

		for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
			iter != tcc.line_map.end();
			iter ++)
		{
			const TCC_STRUCT::_LINE& tcc_line = iter->second;
			if(0 < tcc_line.tccline_id && tcc_line.builtin_id > 0
				&& (tcc_line.nNextTccID == nStaID2 || tcc_line.nPrevTccID == nStaID2))
			{
				lines.push_back(tcc_line.builtin_id);
			}
		}
	}
	return 0 < lines.size();
}

bool ITccClientInfoProvider::GetBlockList(stano_t nTccID, std::vector<lineid_t>& blocklist)
{
	blocklist.clear();

	if(tcc_map.end() == tcc_map.find(nTccID))
		return false;
	const TCC_STRUCT& tcc = tcc_map[nTccID];

	for(std::vector<TCC_STRUCT::_BLOCK>::const_iterator iter = tcc.blocks.begin();
		iter < tcc.blocks.end();
		iter ++)
	{
		blocklist.push_back(iter->tccline_id);
	}

	return blocklist.size() != 0;
}

void ITccClientInfoProvider::GetTccLineList(stano_t nStaID, std::vector<lineid_t>& lines)
{
	lines.clear();

	if(tcc_map.end() == tcc_map.find(nStaID))
		return;
	const TCC_STRUCT& tcc = tcc_map[nStaID];

	for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter ++)
	{
		const TCC_STRUCT::_LINE& tcc_line = iter->second;
		if(0 != tcc_line.tccline_id)
			lines.push_back(tcc_line.tccline_id);
	}

	for(std::vector<TCC_STRUCT::_BLOCK>::const_iterator iter2 = tcc.blocks.begin();
		iter2 != tcc.blocks.end();
		iter2 ++)
	{
		const TCC_STRUCT::_BLOCK& tcc_block = *iter2;
		if(0 != tcc_block.tccline_id)
			lines.push_back(tcc_block.tccline_id);
	}
}

void ITccClientInfoProvider::GetTccLineList(stano_t nStaID, stano_t nStaID2, std::vector<lineid_t>& lines)
{
	if((0 == nStaID )|| ( 0 == nStaID2))
	{
		if(0 != nStaID)
		{
			GetTccLineList(nStaID,lines);
		}
		else if(0 != nStaID2)
		{
			GetTccLineList(nStaID2,lines);
		}
	}
	else
	{
		lines.clear();

		if(tcc_map.end() == tcc_map.find(nStaID))
			return ;
		const TCC_STRUCT& tcc = tcc_map[nStaID];

		for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
			iter != tcc.line_map.end();
			iter ++)
		{
			const TCC_STRUCT::_LINE& tcc_line = iter->second;
			if(0 != tcc_line.tccline_id 
				&& (tcc_line.nNextTccID == nStaID2 || tcc_line.nPrevTccID == nStaID2))
			{
				lines.push_back(tcc_line.tccline_id);
			}
		}
	}
}

lineid_t ITccClientInfoProvider::GetTccLine(stano_t nStaID, stano_t nStaID2, lineid_t nTccLineID2)
{
	if(0 >= nTccLineID2)
		return 0;

	lineid_t nTempTccLine = 0;

	if(tcc_map.end() == tcc_map.find(nStaID) || tcc_map.end() == tcc_map.find(nStaID2))
		return nTempTccLine;

	TCC_STRUCT tcc = tcc_map[nStaID2];
	int builtin_id = 0;

	std::map<int, TCC_STRUCT::_LINE>::iterator iter;
	for(iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter ++)
	{
		const TCC_STRUCT::_LINE& tcc_line = iter->second;
		if(tcc_line.tccline_id == nTccLineID2)
		{
			builtin_id = tcc_line.builtin_id;
			break;
		}
	}

	tcc = tcc_map[nStaID];
	for(iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter ++)
	{
		const TCC_STRUCT::_LINE& tcc_line = iter->second;
		if(tcc_line.builtin_id == builtin_id)
		{
			nTempTccLine = tcc_line.tccline_id;
			break;
		}
	}

	return nTempTccLine;
}

CString ITccClientInfoProvider::GetTccLineName(stano_t nTccID, lineid_t tccline_id)
{
	CString szName("");
	if(0 >= tccline_id)
		return szName;

	if(tcc_map.end() == tcc_map.find(nTccID))
		return szName;
	const TCC_STRUCT& tcc = tcc_map[nTccID];

	for(std::vector<TCC_STRUCT::_BLOCK>::const_iterator iter = tcc.blocks.begin();
		iter < tcc.blocks.end();
		iter ++)
	{
		if(iter->tccline_id == tccline_id)
		{
			return szName = iter->szName;
		}
	}
	for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter2 = tcc.line_map.begin();
		iter2 != tcc.line_map.end();
		iter2 ++)
	{
		if(iter2->second.tccline_id == tccline_id)
		{
			return GetLineName(iter2->second.builtin_id);
		}
	}

	return szName;
}

int ITccClientInfoProvider::GetLineID(stano_t nStaID, lineid_t tccline_id)
{
	if(0 >= tccline_id)
		return 0;

	if(tcc_map.end() == tcc_map.find(nStaID))
		return 0;
	const TCC_STRUCT& tcc = tcc_map[nStaID];

	for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter++)
	{
		const TCC_STRUCT::_LINE& tcc_line = iter->second;
		if(tcc_line.tccline_id == tccline_id)
		{
			return tcc_line.builtin_id;
		}
	}

	return 0;
}

lineid_t ITccClientInfoProvider::GetTccLineID(stano_t nStaID, int builtin_id)
{
	lineid_t tccline_id = 0;

	do {
		if(tcc_map.end() == tcc_map.find(nStaID))
			break;
		TCC_STRUCT& tcc = tcc_map[nStaID];

		if(tcc.line_map.end() == tcc.line_map.find(builtin_id))
			break;
		const TCC_STRUCT::_LINE& tcc_line = tcc.line_map[builtin_id];
		tccline_id = tcc_line.tccline_id;
	} while(0);

	return tccline_id;
}

CString ITccClientInfoProvider::GetLineName(int builtin_id)
{
	CString szName="";
	
	std::map<int, LINE_STRUCT>::iterator iter = line_map.find(builtin_id);
	if(line_map.end() != iter)
		szName = iter->second.szName;

	return szName;
}

dir_t ITccClientInfoProvider::GetTccLineDirection(stano_t nStaID, lineid_t nTccLineID)
{
	dir_t dir = DIR_UNKNOWN;

	do
	{
		if(0 >= nTccLineID)
			break;

		if(tcc_map.end() == tcc_map.find(nStaID))
			break;
		const TCC_STRUCT& tcc = tcc_map[nStaID];

		int builtin_id = 0;
		for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
			iter != tcc.line_map.end();
			iter ++)
		{
			const TCC_STRUCT::_LINE& tcc_line = iter->second;
			if(tcc_line.tccline_id == nTccLineID)
			{
				builtin_id = tcc_line.builtin_id;
				break;
			}
		}
		if(0 == builtin_id)
			break;

		if(line_map.end() == line_map.find(builtin_id))
			break;
		const LINE_STRUCT& line = line_map[builtin_id];
		
		dir = line.nOrientation;
	}while(0);

	return dir;
}

dir_t ITccClientInfoProvider::GetLineDirection(int builtin_id)
{
	dir_t dir = DIR_UNKNOWN;

	do
	{
		if(line_map.end() == line_map.find(builtin_id))
			break;
		const LINE_STRUCT& line = line_map[builtin_id];
		ASSERT(builtin_id == line.builtin_id);
		
		dir = line.nOrientation;
	}while(0);

	return dir;
}

bool ITccClientInfoProvider::GetLineKmMarkerScope(int builtin_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd)
{
	//invalid input data
	if(builtin_id <= 0 || NULL == nKmBegin || NULL == nKmEnd)
		return false;

	if(line_map.end() == line_map.find(builtin_id))
		return false;
	const LINE_STRUCT& line = line_map[builtin_id];
	ASSERT(line.builtin_id == builtin_id);

	switch(line.nOrientation)
	{
	case DIR_DOWN:	// ������
		switch(dir)
		{
		case DIR_DOWN: //��������
		case DIR_UNKNOWN:
			*nKmBegin = line.nKmBegin;
			*nKmEnd   = line.nKmEnd;
			break;
		case DIR_UP:	//��������
			*nKmBegin = line.nKmBegin_R;
			*nKmEnd   = line.nKmEnd_R;
			break;
		case DIR_DUAL: //˫��
			if(line.nKmBegin.Unequal(0) && line.nKmBegin_R.Unequal(0))
			{
				if(line.nKmBegin.Less(line.nKmBegin_R, builtin_id))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}
			else
			{
				if(line.nKmBegin.Unequal(0))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}

			if(line.nKmEnd.Greater(line.nKmEnd_R, builtin_id))
				*nKmEnd = line.nKmEnd;
			else
				*nKmEnd = line.nKmEnd_R;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;

	case DIR_UP:	//������
		switch(dir)
		{
		case DIR_DOWN: //��������
			*nKmBegin = line.nKmBegin_R;
			*nKmEnd   = line.nKmEnd_R;
			break;
		case DIR_UP:	//��������
		case DIR_UNKNOWN:
			*nKmBegin = line.nKmBegin;
			*nKmEnd   = line.nKmEnd;
			break;
		case DIR_DUAL: //˫��
			if(line.nKmBegin.Unequal(0) && line.nKmBegin_R.Unequal(0))
			{
				if(line.nKmBegin.Less(line.nKmBegin_R, builtin_id))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}
			else
			{
				if(line.nKmBegin.Unequal(0))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}

			if(line.nKmEnd.Greater(line.nKmEnd_R, builtin_id))
				*nKmEnd = line.nKmEnd;
			else
				*nKmEnd = line.nKmEnd_R;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;

	case DIR_DUAL:	//˫����
		switch(dir)
		{
		case DIR_DOWN: //������
			*nKmBegin = line.nKmBegin;
			*nKmEnd   = line.nKmEnd;
			break;
		case DIR_UP:	//������
			*nKmBegin = line.nKmBegin_R;
			*nKmEnd   = line.nKmEnd_R;
			break;
		case DIR_DUAL: //˫��
		case DIR_UNKNOWN:
			if(line.nKmBegin.Unequal(0) && line.nKmBegin_R.Unequal(0))
			{
				if(line.nKmBegin.Less(line.nKmBegin_R, builtin_id))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}
			else
			{
				if(line.nKmBegin.Unequal(0))
					*nKmBegin = line.nKmBegin;
				else
					*nKmBegin = line.nKmBegin_R;
			}

			if(line.nKmEnd.Greater(line.nKmEnd_R, builtin_id))
				*nKmEnd = line.nKmEnd;
			else
				*nKmEnd = line.nKmEnd_R;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	return true;
}

bool ITccClientInfoProvider::GetLineSpeedList(int builtin_id, dir_t dir, std::vector<int>& speedlist)
{
	if(line_map.end() == line_map.find(builtin_id))
		return false;
	const LINE_STRUCT& line = line_map[builtin_id];
	ASSERT(line.builtin_id == builtin_id);

	switch(line.nOrientation)
	{
	case DIR_DOWN:	// ������
		switch(dir)
		{
		case DIR_DOWN: //��������
		case DIR_UNKNOWN:
			speedlist = line.speeds;
			break;
		case DIR_UP:	//��������
			speedlist = line.speeds_R;
			break;
		case DIR_DUAL: //˫��
			for(int i=0; i<line.speeds.size(); i++)
			{
				if(line.speeds_R.end() != find(line.speeds_R.begin(), line.speeds_R.end(), line.speeds[i]))
					speedlist.push_back(line.speeds[i]);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;
	case DIR_UP:	//������
		switch(dir)
		{
		case DIR_DOWN: //��������
			speedlist = line.speeds_R;
			break;
		case DIR_UP:	//��������
		case DIR_UNKNOWN:
			speedlist = line.speeds;
			break;
		case DIR_DUAL: //˫��
			for(int i=0; i<line.speeds.size(); i++)
			{
				if(line.speeds_R.end() != find(line.speeds_R.begin(), line.speeds_R.end(), line.speeds[i]))
					speedlist.push_back(line.speeds[i]);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;
	case DIR_DUAL:	//˫����
		switch(dir)
		{
		case DIR_DOWN: //������
			speedlist = line.speeds;
			break;
		case DIR_UP:	//������
			speedlist = line.speeds_R;
			break;
		case DIR_DUAL: //˫��
		case DIR_UNKNOWN:
			for(int i=0; i<line.speeds.size(); i++)
			{
				if(line.speeds_R.end() != find(line.speeds_R.begin(), line.speeds_R.end(), line.speeds[i]))
					speedlist.push_back(line.speeds[i]);
			}
			break;
		default:
			break;
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return true;
}

bool ITccClientInfoProvider::GetTccLineSpeedList(stano_t nTccID, int tccline_id, dir_t dir, std::vector<int>& speedlist)
{
	//invalid input data
	if(0 >= nTccID || 0 >= tccline_id)
		return false;

	if(tcc_map.end() == tcc_map.find(nTccID))
		return false;
	const TCC_STRUCT& tcc = tcc_map[nTccID];
	ASSERT(tcc.nTccID == nTccID);

	int builtin_id = GetLineID(nTccID, tccline_id);
	if(0 != builtin_id)
	{
		return GetLineSpeedList(builtin_id, dir, speedlist);
	}

	for(int i=0; i<tcc.blocks.size(); i++)
	{
		if(tcc.blocks[i].tccline_id == tccline_id)
		{
			speedlist = tcc.blocks[i].speeds;
			return true;
		}
	}
	return false;
}

bool ITccClientInfoProvider::GetTccLineKmMarkerScope(stano_t nTccID, lineid_t tccline_id, dir_t dir, kmmark_t *nKmBegin, kmmark_t *nKmEnd)
{
	if(0 >= nTccID || 0 >= tccline_id)
		return false;

	int builtin_id = GetLineID(nTccID, tccline_id);
	if(0 >= builtin_id)
	{	//����
		*nKmBegin = 0;
		*nKmEnd   = 9999999;
		return true;
	}

	if(tcc_map.end() == tcc_map.find(nTccID))
		return false;
	TCC_STRUCT& tcc = tcc_map[nTccID];
	ASSERT(tcc.nTccID == nTccID);

	if(tcc.line_map.end() == tcc.line_map.find(builtin_id))
		return false;
	TCC_STRUCT::_LINE& tcc_line = tcc.line_map[builtin_id];
	ASSERT(tcc_line.builtin_id == builtin_id);

	if(line_map.end() == line_map.find(builtin_id))
		return false;
	switch(line_map[builtin_id].nOrientation)
	{
	case DIR_DOWN:	// ������
		switch(dir)
		{
		case DIR_DOWN: //��������
		case DIR_UNKNOWN:
			*nKmBegin = tcc_line.nKmBegin;
			*nKmEnd   = tcc_line.nKmEnd;
			break;
		case DIR_UP:	//��������
			*nKmBegin = tcc_line.nKmBegin_R;
			*nKmEnd   = tcc_line.nKmEnd_R;
			break;
		case DIR_DUAL: //˫��
			if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
			{
				if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}
			else
			{
				if(tcc_line.nKmBegin.Unequal(0))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}

			if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
				*nKmEnd = tcc_line.nKmEnd;
			else
				*nKmEnd = tcc_line.nKmEnd_R;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;

	case DIR_UP:	//������
		switch(dir)
		{
		case DIR_DOWN: //��������
			*nKmBegin = tcc_line.nKmBegin_R;
			*nKmEnd   = tcc_line.nKmEnd_R;
			break;
		case DIR_UP:	//��������
		case DIR_UNKNOWN:
			*nKmBegin = tcc_line.nKmBegin;
			*nKmEnd   = tcc_line.nKmEnd;
			break;
		case DIR_DUAL: //˫��
			if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
			{
				if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}
			else
			{
				if(tcc_line.nKmBegin.Unequal(0))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}

			if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
				*nKmEnd = tcc_line.nKmEnd;
			else
				*nKmEnd = tcc_line.nKmEnd_R;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		break;

	case DIR_DUAL:	//˫����
		switch(dir)
		{
		case DIR_DOWN: //������
			*nKmBegin = tcc_line.nKmBegin;
			*nKmEnd   = tcc_line.nKmEnd;
			break;
		case DIR_UP:	//������
			*nKmBegin = tcc_line.nKmBegin_R;
			*nKmEnd   = tcc_line.nKmEnd_R;
			break;
		case DIR_DUAL: //˫��
		case DIR_UNKNOWN:
			if(tcc_line.nKmBegin.Unequal(0) && tcc_line.nKmBegin_R.Unequal(0))
			{
				if(tcc_line.nKmBegin.Less(tcc_line.nKmBegin_R, builtin_id))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}
			else
			{
				if(tcc_line.nKmBegin.Unequal(0))
					*nKmBegin = tcc_line.nKmBegin;
				else
					*nKmBegin = tcc_line.nKmBegin_R;
			}

			if(tcc_line.nKmEnd.Greater(tcc_line.nKmEnd_R, builtin_id))
				*nKmEnd = tcc_line.nKmEnd;
			else
				*nKmEnd = tcc_line.nKmEnd_R;
			break;
		default:
			break;
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return true;
}

/*void ITccClientInfoProvider::GetSimpleSecList(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd, std::vector<SIMPLE_SEC_STRUCT>& seclist)
{
	if(line_map.end() == line_map.find(builtin_id))
		return;
	const LINE_STRUCT& line = line_map[builtin_id];
	ASSERT(line.builtin_id == builtin_id);

	BOOL bStationIsInMark = FALSE;

	for(int i=0; i<line.gd_stas.size(); i++)
	{
		kmmark_t nStaKmBegin = 0x00FFFFFF;//~( 1<< (8 * sizeof(long) - 1));
		kmmark_t nStaKmEnd = 0;
		bStationIsInMark = FALSE;

		SIMPLE_SEC_STRUCT data;
		data.bStation = TRUE;
		for(int j=0; j<line.gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD& sec = line.gd_stas[i].secs[j];

			if(nStaKmBegin.Greater(sec.nKmBegin, builtin_id))
				nStaKmBegin = sec.nKmBegin;
			if(nStaKmEnd.Less(sec.nKmEnd, builtin_id))
				nStaKmEnd   = sec.nKmEnd;

			data.bStation = FALSE;
			data.nKmMark[0] = sec.nKmBegin;
			data.nKmMark[1] = sec.nKmEnd;
			data.szName     = sec.szName;
			if(nKmBegin.LessOrEqual(sec.nKmBegin, builtin_id) && nKmEnd.GreaterOrEqual(sec.nKmEnd, builtin_id))
			{
				bStationIsInMark = TRUE;
				seclist.push_back(data);
			}
		}
		if(bStationIsInMark)
		{
			data.bStation = TRUE;
			data.nKmMark[0] = nStaKmBegin;
			data.nKmMark[1] = nStaKmEnd;
			data.szName = line.gd_stas[i].szName;
			seclist.push_back(data);
		}
	}
}*/


stano_t ITccClientInfoProvider::GetParentStaID(stano_t nZC_TccID)
{
	stano_t nParentStaID = 0;

	do {
		if(tcc_map.end() == tcc_map.find(nZC_TccID))
			break;
		const TCC_STRUCT& tcc = tcc_map[nZC_TccID];
		ASSERT(tcc.nTccID == nZC_TccID);

		nParentStaID = tcc.nParentStaID;
	} while(0);

	return nParentStaID;
}

bool ITccClientInfoProvider::IsZC_TccID(stano_t nZC_TccID)
{
	bool bRet = false;

	do {
		if(tcc_map.end() == tcc_map.find(nZC_TccID))
			break;
		const TCC_STRUCT& tcc = tcc_map[nZC_TccID];
		ASSERT(tcc.nTccID == nZC_TccID);

		if(0 >= tcc.nParentStaID)
			break;

		bRet = true;
	} while(0);

	return bRet;
}


bool parseNumberList(std::vector<int>& num_list, CString szNumberListString)
{
	num_list.clear();

	CString szNumber;
	int iPos = 0;
	do {
		szNumber = szNumberListString.SpanIncluding("0123456789");
		szNumberListString.TrimLeft(szNumber);
		szNumberListString.TrimLeft(szNumberListString.SpanExcluding("0123456789"));
		//szNumberListString = szNumberListString.Right(str.GetLength() - szNumber.GetLength());
		//szNumber = str.Tokenize(", ", iPos);
		if(szNumber.IsEmpty())
			break;
		num_list.push_back(atoi(szNumber));
	} while(TRUE);
	return true;
}

BOOL LoadSpeedList(MSXML2::IXMLDOMDocumentPtr pDoc, 
	std::vector<int>& linespeeds,
	std::vector<int>& linespeeds_r,
	std::vector<int>& blockspeeds)
{
	CString str;
	_variant_t varVal;
	MSXML2::IXMLDOMNodePtr pNode;
	if(pNode = pDoc->selectSingleNode("root/speed/line/@speed"))
	{
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;

		SysFreeString(varVal.bstrVal);
		varVal.Clear();

		parseNumberList(linespeeds, str);
	}
	if(0 == linespeeds.size())
	{
		AfxMessageBox("root/speed/line/@speedδ��������д���ʹ��ȱʡֵ45");
		linespeeds.push_back(45);
	}

	//��������ֵ�б�, û�ж�����������һ��
	if(pNode = pDoc->selectSingleNode("root/speed/line/@speed_r"))
	{
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;

		SysFreeString(varVal.bstrVal);
		varVal.Clear();

		parseNumberList(linespeeds_r, str);
		if(0 == linespeeds_r.size())
		{
			AfxMessageBox("root/speed/line/@speed_r�����д���ʹ������ֵ");
			linespeeds_r = linespeeds;
		}
	}
	else
	{
		linespeeds_r = linespeeds;
	}

	//��������ֵ�б�
	if(pNode = pDoc->selectSingleNode("root/speed/block/@speed"))
	{
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;

		SysFreeString(varVal.bstrVal);
		varVal.Clear();

		parseNumberList(blockspeeds, str);
	}
	if(0 == blockspeeds.size())
	{
		AfxMessageBox("root/speed/block/@speedδ��������д���ʹ��ȱʡֵ45");
		blockspeeds.push_back(45);
	}

	return TRUE;
}

BOOL LoadLineProperty(MSXML2::IXMLDOMDocumentPtr pDoc, 
	std::vector<int>& linespeeds,
	std::vector<int>& linespeeds_r,
	std::vector<int>& blockspeeds)
{
	CString str, str2, prompt;
	_variant_t varVal;
	MSXML2::IXMLDOMNodePtr pNode;

	// ��ȡ��·����
	MSXML2::IXMLDOMNodeListPtr pNodeList = pDoc->selectNodes("root/lineproperty/line");
	for(int i=0; i<pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode_line = pNodeList->Getitem( i );

		LINE_STRUCT linedata;
		if(pNode = pNode_line->selectSingleNode("@builtin_id"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			linedata.builtin_id = atoi(str);

			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		if(linedata.builtin_id <= 0)
		{
			prompt.Format("root/lineproperty/line/@builtin_id=\"%s\" �Ƿ�, ����!", str);
			AfxMessageBox(prompt);
			continue;
		}
		if(line_map.end() != line_map.find(linedata.builtin_id))
		{
			prompt.Format("root/lineproperty/line/@builtin_id=\"%s\" �ظ�, ����!", str);
			AfxMessageBox(prompt);
			continue;
		}

		if(pNode = pNode_line->selectSingleNode("@name"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			linedata.szName = str;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		if(pNode = pNode_line->selectSingleNode("@orientation"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			switch(atoi(str))
			{
			case 1:
				linedata.nOrientation = DIR_DOWN;
				break;
			case 2:
				linedata.nOrientation = DIR_UP;
				break;
			case 3:
			default:
				linedata.nOrientation = DIR_DUAL;
				break;
				//linedata.nOrientation = DIR_UNKNOWN;
			}

			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		else
		{
			prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@orientation û�����ã�����δ֪����˫����!",
				linedata.builtin_id);
			AfxMessageBox(prompt);
			linedata.nOrientation = DIR_DUAL;
		}
		if(pNode = pNode_line->selectSingleNode("@kmmarkpartial"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			str.MakeLower();
			if(str == "ascend")
			{
				linedata.bKmmarkAscend = true;
			}
			else if(str == "descend")
			{
				linedata.bKmmarkAscend = false;
			}
			else
			{
				prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@kmmarkpartial=\"%s\" ���ò���ȷ!",
					linedata.builtin_id, str);
				AfxMessageBox(prompt);
				switch(linedata.nOrientation)
				{
				case DIR_DOWN:
					linedata.bKmmarkAscend = true;
					break;
				case DIR_UP:
					linedata.bKmmarkAscend = false;
					break;
				case DIR_DUAL:
				case DIR_UNKNOWN:
					linedata.bKmmarkAscend = true;
					break;
				}
			}
		}
		else
		{
			prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@kmmarkpartial û������!",
				linedata.builtin_id);
			AfxMessageBox(prompt);
			switch(linedata.nOrientation)
			{
			case DIR_DOWN:
				linedata.bKmmarkAscend = true;
				break;
			case DIR_UP:
				linedata.bKmmarkAscend = false;
				break;
			case DIR_DUAL:
			case DIR_UNKNOWN:
				linedata.bKmmarkAscend = true;
				break;
			}
		}

		//����δ�� ����δ��, �Թ�������Ϊ׼
		//�������� ����δ��, �������ͬ����
		//����δ�� ��������, ������Ϊ����������, ��������
		if(pNode = pNode_line->selectSingleNode("@speed"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			parseNumberList(linedata.speeds, str);
			if(0 == linedata.speeds.size())
			{
				prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@speed=\"%s\" �����д��� ʹ��root/speed/line/@speed����!",
					linedata.builtin_id, str);
				AfxMessageBox(prompt);
				linedata.speeds = linespeeds;
			}
			linedata.speeds_R = linedata.speeds;
		}
		else
		{
			linedata.speeds = linespeeds;
		}
		if(pNode = pNode_line->selectSingleNode("@speed_r"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			BOOL b = 0 != linedata.speeds_R.size();
			parseNumberList(linedata.speeds_R, str);
			if(0 == linedata.speeds_R.size())
			{
				if(b)//��������
				{
					prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@speed_r=\"%s\"�����д��� ʹ��speed����!", 
						linedata.builtin_id, str);
					AfxMessageBox(prompt);
					linedata.speeds_R = linedata.speeds;
				}
				else
				{
					prompt.Format("root/lineproperty/line[@builtin_id=\"%d\"]/@speed_r=\"%s\"�����д��� ʹ��root/speed/line/@speed_r����!", 
						linedata.builtin_id, str);
					AfxMessageBox(prompt);
					linedata.speeds_R = linespeeds_r;
				}
			}
		}
		else
		{
			if(0 == linedata.speeds_R.size())
				linedata.speeds_R = linespeeds_r;
		}

		//�����ϵ
		MSXML2::IXMLDOMNodeListPtr pNodeList2 = pNode_line->selectNodes("kmsys");
		if(0 == pNodeList2->Getlength())
		{
			LINE_STRUCT::_KM_CODE km_code;
			km_code.szPrex = "";
			km_code.nCode = 0;
			km_code.bInc = true;
			
			linedata.szDefKmsysPrex = "";
			linedata.km_codes.push_back(km_code);
		}
		else
		{
			bool bSetDefPrex = false;

			for(int j=0; j<pNodeList2->Getlength(); j++)
			{
				LINE_STRUCT::_KM_CODE km_code;

				pNode = pNodeList2->Getitem(j)->selectSingleNode("@code");
				if(NULL == pNode)
					continue;

				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				//str.Trim(" ");
				str.Replace(" ", "");

				//ǰ׺
				str2 = str.SpanExcluding(":");
				str2.MakeUpper();
				str.TrimLeft(str2).TrimLeft(":");
				km_code.szPrex = str2;
				//ֵ
				str2 = str.SpanExcluding(":");
				str.TrimLeft(str2).TrimLeft(":");
				km_code.nCode = atoi(str2);

				//��
				str2 = str.SpanExcluding(":");
				str.TrimLeft(str2).TrimLeft(":");
				if(str2 == "DEC")
					km_code.bInc = false;
				else if(str2 == "INC")
					km_code.bInc = true;
				else
				{
					AfxMessageBox("�����ϵ����������");
					km_code.bInc = true;
				}

				//��Χ
				DWORD nMIN = 0,	nMAX = (DWORD)-1;
				bool bScopeSet = true;

				if(!str.IsEmpty())
				{
					str2 = str.SpanIncluding("0123456789");
					str.TrimLeft(str2).TrimLeft("-");
					nMIN = atol(str2);
					bScopeSet = false;
				}

				if(!str.IsEmpty())
				{
					str2 = str.SpanIncluding("0123456789");
					str.TrimLeft(str2);
					nMAX = atol(str2);
					bScopeSet = true;
				}
				if(bScopeSet)
				{
					if(nMIN > nMAX)
					{
						nMAX = nMAX ^ nMIN;
						nMIN = nMAX ^ nMIN;
						nMAX = nMAX ^ nMIN;
					}
					km_code.nMIN = nMIN;
					km_code.nMAX = nMAX;
				}
				else
				{
					str.Format("�����ϵ%s:%d��Χ����������, ����",
						km_code.szPrex, km_code.nCode);
					AfxMessageBox(str);
					continue;
				}
				for(int k=0; k<linedata.km_codes.size(); k++)
				{
					if(linedata.km_codes[k].szPrex != km_code.szPrex
						&& linedata.km_codes[k].nCode != km_code.nCode)
						continue;

					//��Χ�޽���
					if(linedata.km_codes[k].nMIN > km_code.nMAX
						|| linedata.km_codes[k].nMAX < km_code.nMIN)
						continue;

					break;
				}
				if(k < linedata.km_codes.size())
				{
					str.Format("�����ϵ%s:%d��Χ����[%d-%d]���ֽ�����, ����",
						km_code.szPrex, km_code.nCode, 
						km_code.nMIN, km_code.nMAX);
					AfxMessageBox(str);
					continue;
				}

				if(pNode = pNodeList2->Getitem(j)->selectSingleNode("@default"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					if(!bSetDefPrex)
					{
						linedata.szDefKmsysPrex = km_code.szPrex;
						bSetDefPrex = true;
					}
				}

				linedata.km_codes.push_back(km_code);
			}
			if(!bSetDefPrex && linedata.km_codes.size() > 0)
				linedata.szDefKmsysPrex = linedata.km_codes[0].szPrex;
		}

        line_map[linedata.builtin_id] = linedata;
	}
	// ���߹����
	if(pNode = pDoc->selectSingleNode("root/lineproperty/block/@kmsys_code"))
	{
		varVal = pNode->GetnodeTypedValue();
		str = varVal.bstrVal;

		SysFreeString(varVal.bstrVal);
		varVal.Clear();

		gBlockKmsysCode = atoi(str);
	}

	return TRUE;
}

BOOL LoadLineSectionList(MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList;
	CString str, str2, prompt;
	_variant_t varVal;
	MSXML2::IXMLDOMNodePtr pNode;

	//��ȡ��·��������
	pNodeList = pDoc->selectNodes("root/seclist/line");
	for(int iLine=0; iLine<pNodeList->Getlength(); iLine++)
	{
		MSXML2::IXMLDOMNodePtr pNode_line = pNodeList->Getitem( iLine );

		int builtin_id = 0;
		CString szPromptPrex = "root/seclist/line";

		if(pNode = pNode_line->selectSingleNode("@builtin_id"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			builtin_id = atoi(str);
			if(0 >= builtin_id)
			{
				prompt.Format("%s/@builtin_id=\"%s\" �Ƿ�, ����!", szPromptPrex, str);
				AfxMessageBox(prompt);
				continue;
			}
			szPromptPrex += "[@builtin_id=\"" + str + "\"]";
		}
		else
		{
			prompt.Format("%s/@builtin_id û����, ����!", szPromptPrex, str);
			AfxMessageBox(prompt);
			continue;
		}

		if(line_map.end() == line_map.find(builtin_id))
		{
			prompt.Format("%s ��·��lineproperty��û��, ����!", szPromptPrex);
			AfxMessageBox(prompt);
			continue;
		}
		LINE_STRUCT& line = line_map[builtin_id];
		ASSERT(line.builtin_id == builtin_id);

		kmmark_t nKmBegin = 0;
		MSXML2::IXMLDOMNodeListPtr pNodeList_station = pNode_line->selectNodes("station");
		for(int jSta=0; jSta<pNodeList_station->Getlength(); jSta++)
		{
			MSXML2::IXMLDOMNodePtr pNode_station = pNodeList_station->Getitem(jSta);
			CString szStaName;

			if(pNode = pNode_station->selectSingleNode("@name"))
			{
				varVal = pNode->GetnodeTypedValue();
				szStaName = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}
			//for(int k=0; k<line.stas.size(); k++)
			//{
			//	if(line.stas[k].szName == szStaName && !str.IsEmpty())
			//		break;
			//}
			vector<LINE_STRUCT::_STA_GD::_SEC_GD> *psecs;
			//if(k >= line.stas.size())
			//{
			LINE_STRUCT::_STA_GD sta;
				sta.szName = szStaName;
				line.gd_stas.push_back(sta);
				psecs = &line.gd_stas[line.gd_stas.size() -1].secs;
			//}
			//else
			//{
			//	psecs = &line.stas[k].secs;
			//}

			MSXML2::IXMLDOMNodeListPtr pNodeList_section = pNode_station->selectNodes("section");
			for(int kSec=0; kSec<pNodeList_section->Getlength(); kSec++)
			{
				LINE_STRUCT::_STA_GD::_SEC_GD sec;

				CString szSectionPromptPrex;
				szSectionPromptPrex.Format("%s/station[@name=\"%s\"(��%d��)]", szPromptPrex, szStaName, jSta+1);

				MSXML2::IXMLDOMNodePtr pNode_section = pNodeList_section->Getitem(kSec);

				str = "";
				if(pNode = pNode_section->selectSingleNode("@name"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;
					sec.szName = str;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();
				}
				prompt.Format("/section[@name=\"%s\"(��%d��)]", str, kSec+1);
				szSectionPromptPrex += prompt;

				if(pNode = pNode_section->selectSingleNode("@begin"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					if(!parseKmMark(line, str, &sec.nKmBegin))
					{
						prompt.Format("%s/@begin=\"%s\" �Ƿ�, ����", szSectionPromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szSectionPromptPrex += "[@begin=\"" + str + "\"]";
				}
				else
				{
					sec.nKmBegin = nKmBegin;
					szSectionPromptPrex += "[@begin=\"\"]";
				}
				if(nKmBegin.Unequal(0) && sec.nKmBegin.Unequal(nKmBegin) )
				{
					prompt.Format("%s ����һ�����������, ����", szSectionPromptPrex);
					AfxMessageBox(prompt);
					continue;
				}

				if(pNode = pNode_section->selectSingleNode("@end"))
				{
					varVal = pNode->GetnodeTypedValue();
					str = varVal.bstrVal;

					SysFreeString(varVal.bstrVal);
					varVal.Clear();

					if(!parseKmMark(line, str, &sec.nKmEnd))
					{
						prompt.Format("%s/@end=\"%s\" �Ƿ�, ����", szSectionPromptPrex, str);
						AfxMessageBox(prompt);
						continue;
					}
					szSectionPromptPrex += "[@end=\"" + str + "\"]";
				}
				else
				{
					prompt.Format("%s/@end û����, ����", szSectionPromptPrex);
					AfxMessageBox(prompt);
					continue;
				}
				nKmBegin = sec.nKmEnd;

				if(sec.nKmBegin.flag != sec.nKmEnd.flag)
				{
					if(pNode = pNode_section->selectSingleNode("@length"))
					{
						varVal = pNode->GetnodeTypedValue();
						str = varVal.bstrVal;

						SysFreeString(varVal.bstrVal);
						varVal.Clear();

						sec.nLength = atol(str);
						if(sec.nLength <= 0)
						{
							prompt.Format("%s/@length �Ƿ�, ȡȱʡֵ500", szSectionPromptPrex);
							AfxMessageBox(prompt);
							sec.nLength = 500;
						}
					}
					else
					{
						prompt.Format("%s/@length û����, ȡȱʡֵ500", szSectionPromptPrex);
						AfxMessageBox(prompt);
						sec.nLength = 500;
					}
				}
				else
				{
					if(sec.nKmBegin.GreaterOrEqual(sec.nKmEnd, builtin_id))
					{
						prompt.Format("%s ����귴��, ����", szSectionPromptPrex);
						AfxMessageBox(prompt);
						continue;
					}
					sec.nLength = sec.nKmEnd.info.val - sec.nKmBegin.info.val;
					sec.nLength = abs(sec.nLength);
				}

				bool bCrossed = false;
				vector<LINE_STRUCT::_STA_GD::_SEC_GD>::iterator iter;
				for(iter = psecs->begin(); iter < psecs->end(); iter ++)
				{
					if((*iter).nKmEnd.flag != sec.nKmBegin.flag || (*iter).nKmEnd.LessOrEqual(sec.nKmBegin, builtin_id))
						continue;

					if(sec.nKmBegin.flag != sec.nKmEnd.flag)
						break;

					if((*iter).nKmEnd.LessOrEqual(sec.nKmEnd, builtin_id) || (*iter).nKmBegin.Less(sec.nKmEnd, builtin_id))
						bCrossed = true;

					break;
				}
				if(bCrossed)
				{
					prompt.Format("%s ��ǰ��Ĺ鵵�����н���, ����", szSectionPromptPrex);
					AfxMessageBox(prompt);
					continue;
				}
				else
					psecs->insert(iter, sec);
			}
		}

		//վ��ȫ���䷶Χ����
		MSXML2::IXMLDOMNodeListPtr pNodeList_fullseclist = pNode_line->selectNodes("fullseclist/fullsec");
		for(int j=0; j<pNodeList_fullseclist->Getlength(); j++)
		{
			MSXML2::IXMLDOMNodePtr pNode_fullsec = pNodeList_fullseclist->Getitem(j);


			CString szFullSecPromptPrex;
			szFullSecPromptPrex.Format("%s/fullseclist/fullsec(��%d��)", szPromptPrex, j+1);

			LINE_STRUCT::_FULLSEC fullsec;

			if(pNode = pNode_fullsec->selectSingleNode("@begin"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &fullsec.nKmBegin))
				{
					prompt.Format("%s/fullseclist/fullsec/@begin=\"%s\" �Ƿ�, ����", szPromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szFullSecPromptPrex += "[@begin=\"" + str + "\"]";
			}
			else
			{
				prompt.Format("%s/fullseclist/fullsec/@begin û���䣬 ����", szPromptPrex, str);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_fullsec->selectSingleNode("@end"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &fullsec.nKmEnd))
				{
					prompt.Format("%s/@end=\"%s\" �Ƿ�, ����", szPromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szFullSecPromptPrex += "[@end=\"" + str + "\"]";
			}
			else
			{
				prompt.Format("%s/@end= û����, ����", szPromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			BOOL bFindBegin = FALSE,	bFindEnd = FALSE;
			for(int k=0; k<line.gd_stas.size() && !(bFindBegin && bFindEnd); k++)
			{
				for(int l=0; l<line.gd_stas[k].secs.size() && !(bFindBegin && bFindEnd); l++)
				{
					if(!bFindBegin && fullsec.nKmBegin.Equal(line.gd_stas[k].secs[l].nKmBegin))
						bFindBegin = TRUE;
					if(!bFindEnd && fullsec.nKmEnd.Equal(line.gd_stas[k].secs[l].nKmEnd))
						bFindEnd = TRUE;
				}
			}
			if( !bFindBegin || !bFindEnd )
			{
				prompt.Format("%s ���Ǳ��������˵�, ����", szPromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			line.fullsecs.push_back(fullsec);
		}
	}

	return TRUE;
}

BOOL LoadTccLineScope(MSXML2::IXMLDOMDocumentPtr pDoc, std::vector<int>& blockspeeds)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList;
	CString str, prompt;
	_variant_t varVal;
	MSXML2::IXMLDOMNodePtr pNode;

	//�пع�Ͻ����·��Χ
	pNodeList = pDoc->selectNodes("root/tcclist/tcc");
	for(int i=0; i<pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode_tcc = pNodeList->Getitem(i);

		TCC_STRUCT tcc;
		CString szPromptPrex;

		if(pNode = pNode_tcc->selectSingleNode("@id"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			tcc.nTccID = atoi(str);
			if(tcc.nTccID <= 0)
			{
				prompt.Format("root/tcclist/tcc/@id=\"%s\" �пر�ŷǷ�, ����!", szPromptPrex, str);
				AfxMessageBox(prompt);
				continue;
			}
			szPromptPrex.Format("root/tcclist/tcc[@id=\"%s\"]", str);
		}
		else
		{
			prompt.Format("root/tcclist/tcc/@id û����, ����!", str);
			AfxMessageBox(prompt);
			continue;
		}


		//�������ɻ�
		if(pNode = pNode_tcc->selectSingleNode("@parent"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;

			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			tcc.nParentStaID = atoi(str);

			if(tcc.nParentStaID <= 0)
			{
				prompt.Format("%s/@parent=\"%s\" �п�parent��ŷǷ�, ����!", szPromptPrex, str);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_tcc->selectSingleNode("@name"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc.szName = str;
			}
		}

		MSXML2::IXMLDOMNodeListPtr pNodeList_line = pNode_tcc->selectNodes("linelist/line");
		// ���8����
		for(int j=0; j<pNodeList_line->Getlength(); j++)
		{
			TCC_STRUCT::_LINE tcc_line;
			CString szTccLinePromptPrex;
		
			MSXML2::IXMLDOMNodePtr pNode_line = pNodeList_line->Getitem(j);

			if(pNode = pNode_line->selectSingleNode("@builtin_id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc_line.builtin_id = atoi(str);
				if(tcc_line.builtin_id <= 0)
				{
					prompt.Format("%s/linelist/line/@builtin_id=\"%s\" ���Ϸ�, ����", 
						szPromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex.Format("%s/linelist/line[@builtin_id=\"%s\"]", szPromptPrex, str);
			}
			else
			{
				prompt.Format("%s/linelist/line/@builtin_id û����, ����", str);
				AfxMessageBox(prompt);
				continue;
			}

			if(line_map.end() == line_map.find(tcc_line.builtin_id))
			{
				prompt.Format("%s, ��·�����б���û��������·������", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}
			LINE_STRUCT& line = line_map[tcc_line.builtin_id];
			ASSERT(line.builtin_id == tcc_line.builtin_id);

			if(tcc.line_map.end() != tcc.line_map.find(tcc_line.builtin_id))
			{
				prompt.Format("%s ��·�Ѵ��ڣ�����", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_line->selectSingleNode("@tccline_id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc_line.tccline_id = atoi(str);
				if(tcc_line.tccline_id <= 0)
				{
					prompt.Format("%s/@tccline_id=\"%s\" ���Ϸ�, ����", 
						szTccLinePromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex += "[@tccline_id=" + str + "\"]";
				if(tcc_line.tccline_id == tcc.line_map[tcc_line.builtin_id].tccline_id)
				{
					prompt.Format("%s �Ѵ��ڣ�����", szTccLinePromptPrex);
					AfxMessageBox(prompt);
					continue;
				}
			}
			else
			{
				prompt.Format("%s/@tccline_id û����, ����", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_line->selectSingleNode("@next_station"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc_line.nNextTccID = atoi(str);
			}

			if(pNode = pNode_line->selectSingleNode("@begin"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &tcc_line.nKmBegin))
				{
					prompt.Format("%s/@begin=\"%s\" �Ƿ�, ����",
						szTccLinePromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex += "[@begin=" + str + "\"]";
			}
			else
			{
				prompt.Format("%s/@begin û����, ����", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_line->selectSingleNode("@end"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &tcc_line.nKmEnd))
				{
					prompt.Format("%s/@end=\"%s\" �Ƿ�, ����",
						szTccLinePromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex += "[@end=" + str + "\"]";
			}
			else
			{
				prompt.Format("%s/@end û����, ����", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			if(tcc_line.nKmBegin.Equal(tcc_line.nKmEnd))
			{
				prompt.Format("%s ��@begin>=@end, ���Ϸ�������", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}
			else
			{
				if(tcc_line.nKmBegin.Greater(tcc_line.nKmEnd, tcc_line.builtin_id))
				{
					kmmark_t temp;
					temp = tcc_line.nKmBegin;
					tcc_line.nKmBegin = tcc_line.nKmEnd;
					tcc_line.nKmEnd = temp;
				}
			}


			if(pNode = pNode_line->selectSingleNode("@rbegin"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &tcc_line.nKmBegin_R))
				{
					prompt.Format("%s/@rbegin=\"%s\" �Ƿ�, ����", szTccLinePromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex += "[@rbegin=" + str + "\"]";
			}

			if(pNode = pNode_line->selectSingleNode("@rend"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				if(!parseKmMark(line, str, &tcc_line.nKmEnd_R))
				{
					prompt.Format("%s/@rend=\"%s\" �Ƿ�, ����", szTccLinePromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				szTccLinePromptPrex += "[@rend=" + str + "\"]";
			}

			if (tcc_line.nKmEnd_R.info.val != 0 && tcc_line.nKmBegin_R.info.val == 0
				|| tcc_line.nKmEnd_R.info.val == 0 && tcc_line.nKmBegin_R.info.val != 0)
			{
				prompt.Format("%s ��@rbegin, @rend, ���Ϸ�������", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			else if(tcc_line.nKmBegin_R.Equal(tcc_line.nKmEnd_R) && tcc_line.nKmEnd_R.info.val != 0)
			{
				prompt.Format("%s ��@rbegin==@rend, ���Ϸ�������", szTccLinePromptPrex);
				AfxMessageBox(prompt);
				continue;
			}
			else if(tcc_line.nKmBegin_R.Greater(tcc_line.nKmEnd_R, tcc_line.builtin_id)
				&& tcc_line.nKmEnd_R.info.val != 0 && tcc_line.nKmBegin_R.info.val != 0)
			{
				kmmark_t temp;
				temp = tcc_line.nKmBegin_R;
				tcc_line.nKmBegin_R = tcc_line.nKmEnd_R;
				tcc_line.nKmEnd_R = temp;
			}

			//������·�Ĺ���귶Χ
			if(tcc_line.nKmBegin.Less(line.nKmBegin, line.builtin_id) || line.nKmBegin.info.val == 0)
			{
				line.nKmBegin = tcc_line.nKmBegin;
				//line.nLineFirstTccID = tcc.nTccID;
			}
			if(tcc_line.nKmEnd.Greater(line.nKmEnd, line.builtin_id) || line.nKmEnd.info.val == 0)
			{
				line.nKmEnd = tcc_line.nKmEnd;
				//line.nLineLastTccID = tcc.nTccID;
			}
			if(tcc_line.nKmBegin_R.Less(line.nKmBegin_R, line.builtin_id) || line.nKmBegin_R.info.val == 0)
			{
				line.nKmBegin_R = tcc_line.nKmBegin_R;
			}
			if(tcc_line.nKmEnd_R.Greater(line.nKmEnd_R, line.builtin_id) || line.nKmEnd_R.info.val == 0)
				line.nKmEnd_R = tcc_line.nKmEnd_R;

			tcc_line.nNextTccID = tcc_line.nPrevTccID = 0;

			TCC_STRUCT::_LINE *pPrevTccLine, *pTccLine;
			pPrevTccLine = pTccLine = NULL;
			stano_t nTccID = line.nLineFirstTccID;
			stano_t nPrevTccID = 0;
			while(nTccID)
			{
				pTccLine = &tcc_map[nTccID].line_map[line.builtin_id];

				if(pTccLine->nKmBegin.Less(tcc_line.nKmBegin, line.builtin_id)
					|| pTccLine->nKmBegin.Equal(tcc_line.nKmBegin)
						&& pTccLine->nKmEnd.Less(tcc_line.nKmEnd, line.builtin_id))
				{
					if(pTccLine->nNextTccID <= 0)
					{
						nPrevTccID = nTccID;
						nTccID = 0;
					}
					else
					{
						nPrevTccID = nTccID;
						nTccID = pTccLine->nNextTccID;
					}
					pPrevTccLine = pTccLine;
					pTccLine = NULL;
				}
				else
					break;
			}
			if(pPrevTccLine)
			{
				tcc_line.nNextTccID = pPrevTccLine->nNextTccID;
				pPrevTccLine->nNextTccID = tcc.nTccID;
				if(pTccLine)
				{
					tcc_line.nPrevTccID = pTccLine->nPrevTccID;
					pTccLine->nPrevTccID = tcc.nTccID;
				}
				else
				{
					tcc_line.nPrevTccID = nPrevTccID;
					line.nLineLastTccID = tcc.nTccID;
				}
			}
			else
			{
				tcc_line.nPrevTccID = 0;
				tcc_line.nNextTccID = nTccID;
				if(pTccLine)
					pTccLine->nPrevTccID = tcc.nTccID;
				line.nLineFirstTccID = tcc.nTccID;
			}

			tcc.line_map[tcc_line.builtin_id] = tcc_line;
		}

		//����
		MSXML2::IXMLDOMNodeListPtr pNodeList_block = pNode_tcc->selectNodes("blocklist/block");
		for(int j=0; j<pNodeList_block->Getlength(); j++)
		{
			TCC_STRUCT::_BLOCK tcc_block;
		
			MSXML2::IXMLDOMNodePtr pNode_block = pNodeList_block->Getitem(j);

			if(pNode = pNode_block->selectSingleNode("@tccline_id"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				tcc_block.tccline_id = atoi(str);
				if(tcc_block.tccline_id <= 0)
				{
					prompt.Format("%s/blocklist/block/@tccline_id=\"%s\" ���Ϸ�, ����", szPromptPrex, str);
					AfxMessageBox(prompt);
					continue;
				}
				for(int iblock=0; iblock<tcc.blocks.size(); iblock++)
				{
					if(tcc.blocks[iblock].tccline_id == tcc_block.tccline_id)
						break;
				}
				if(iblock < tcc.blocks.size())
				{
					prompt.Format("%s/blocklist/block[@tccline_id=\"%d\"] ��·�Ѵ��ڣ�����", 
						szPromptPrex, tcc_block.tccline_id);
					AfxMessageBox(prompt);
					continue;
				}
				std::map<int, TCC_STRUCT::_LINE>::const_iterator iter;
				for(iter = tcc.line_map.begin(); iter != tcc.line_map.end(); iter ++)
				{
					if(iter->second.tccline_id == tcc_block.tccline_id)
					{
						break;
					}
				}
				if(iter != tcc.line_map.end())
				{
					prompt.Format("%s/blocklist/block[@tccline_id=\"%d\"] ��·�Ѵ��ڣ�����", 
						szPromptPrex, tcc_block.tccline_id);
					AfxMessageBox(prompt);
					continue;
				}
			}
			else
			{
				prompt.Format("%s/blocklist/block/@tccline_id û���䣬����", szPromptPrex);
				AfxMessageBox(prompt);
				continue;
			}

			if(pNode = pNode_block->selectSingleNode("@name"))
			{
				varVal = pNode->GetnodeTypedValue();
				tcc_block.szName = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();
			}

			if(pNode = pNode_block->selectSingleNode("@speed"))
			{
				varVal = pNode->GetnodeTypedValue();
				str = varVal.bstrVal;

				SysFreeString(varVal.bstrVal);
				varVal.Clear();

				parseNumberList(tcc_block.speeds, str);
			}
			else
			{
				tcc_block.speeds = blockspeeds;
			}

			tcc.blocks.push_back(tcc_block);
		}

		tcc_map[tcc.nTccID] = tcc;
	}

	return TRUE;
}

void CTccGpcInfoProvider::SetTCCRight(int nSectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc)
{
	m_bReadOnly = bReadOnly;
	m_bExecute  = bExecute;
	m_bCtcGpc   = bCTCGpc;
	m_nCurrentSectionNo=nSectionNo;
	return;
}

void CTccGpcInfoProvider::LoadTccLineNameConfig(CString strFilename)
{
	gpcdef.ClearC3TccLineName();

	LPCSTR szFilename = strFilename;

	BOOL bRet = FALSE;

	CoInitialize(NULL);

	MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
	HRESULT hr;
	hr = pXMLDOMDoc.CreateInstance(
		__uuidof(MSXML2::DOMDocument40), 
		NULL, 
		CLSCTX_INPROC_SERVER);

	pXMLDOMDoc->put_async(VARIANT_FALSE);
	pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
	pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

	_variant_t vXml(szFilename);
	VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
	SysFreeString(vXml.bstrVal);
	if(VARIANT_FALSE == vBool)
	{
		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format("\nLine: %d", pError->Getline());

			_bstr_t strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText()
				+ _bstr_t(str);

			AfxMessageBox(strRes);
		}
		return;
	}

	// ȡ����
	CString str, str2, prompt;
	_variant_t varVal;
	
	// ��ȡ��·����
	MSXML2::IXMLDOMNodePtr pNode;
	MSXML2::IXMLDOMNodeListPtr pNodeList = pXMLDOMDoc->selectNodes("root/lineproperty/line");
	for(int i=0; i<pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode_line = pNodeList->Getitem( i );

		int builtin_id=0;
		if(pNode = pNode_line->selectSingleNode("@builtin_id"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			builtin_id = atoi(str);

			SysFreeString(varVal.bstrVal);
			varVal.Clear();
		}
		if(builtin_id <= 0)
		{
			prompt.Format("root/lineproperty/line/@builtin_id=\"%s\" �Ƿ�, ����!", str);
			AfxMessageBox(prompt);
			continue;
		}
		
		if(pNode = pNode_line->selectSingleNode("@name"))
		{
			varVal = pNode->GetnodeTypedValue();
			str = varVal.bstrVal;
			
			SysFreeString(varVal.bstrVal);
			varVal.Clear();

			gpcdef.AddC3TccLineName(builtin_id, str);
		}
	}
	
	CoUninitialize();

	return;
}

BOOL CTccGpcInfoProvider::LoadTccLineConfig()
{
	CString strFilename;
	strFilename.Format("%s\\tccline.cfg", GetTccConfigDirectory());

	LPCSTR szFilename = strFilename;

	line_map.clear();
	tcc_map.clear();
	
	BOOL bRet = FALSE;

	CoInitialize(NULL);

	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(
			__uuidof(MSXML2::DOMDocument40), 
			NULL, 
			CLSCTX_INPROC_SERVER);

		pXMLDOMDoc->put_async(VARIANT_FALSE);
		pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		if(VARIANT_FALSE == vBool)
		{
			MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
			if(S_OK != pError->GeterrorCode())
			{
				CString str; 
				str.Format("\nLine: %d", pError->Getline());

				_bstr_t strRes = _bstr_t("Validation failed on ") + szFilename
					+ _bstr_t("\n=============")
					+ _bstr_t("\nReason: ") + pError->Getreason()
					+ _bstr_t("\nText: ") + pError->GetsrcText()
					+ _bstr_t(str);

				AfxMessageBox(strRes);
			}
			return FALSE;
		}

		// ȡ����
		CString str, str2, prompt;
		_variant_t varVal;
		MSXML2::IXMLDOMNodePtr pNode;

		std::vector<int> linespeeds, linespeeds_r, blockspeeds;
	
		// ��ȡ����ֵ�б�
		if(!LoadSpeedList(pXMLDOMDoc, linespeeds, linespeeds_r, blockspeeds))
		{
		}

		// ��ȡ��·����
		if(!LoadLineProperty(pXMLDOMDoc, linespeeds, linespeeds_r, blockspeeds))
		{
		}

		//��ȡ��·��������
		if(!LoadLineSectionList(pXMLDOMDoc))
		{
		}

		//�пع�Ͻ����·��Χ
		if(!LoadTccLineScope(pXMLDOMDoc, blockspeeds))
		{
		}
	} while(0);

	CoUninitialize();

	return bRet;
}

CTccGpcInfoProvider::CTccGpcInfoProvider()
	: m_nSecNo(0)
{
//	memset(m_nBelow45, 0x0, sizeof(m_nBelow45));
}

CTccGpcInfoProvider::~CTccGpcInfoProvider()
{
}

CString CTccGpcInfoProvider::GetTccDataDirectory()
{
	CString path = gpcdef.GetDDMLDataDir();
	path.TrimRight("\\");
	path.TrimRight("/");
	path += "\\DDML\\TCC";

	CString sz;
	//sz.Format("\\q%03d", pDoc->m_nCurrentSectionNo);
	return path + sz;
}

CString CTccGpcInfoProvider::GetTccConfigDirectory()
{
	CString path = gpcdef.GetDataDir();
	path.TrimRight("\\");
	path.TrimRight("/");

	CString dir = gpcdef.GetTccCfgDir(3);
	if(dir.IsEmpty())
	{
		;
	}
	else
		path += "\\" + dir;

	return path;
}


// �õ���վ�б�����ֵΪʵ��ȡ�õ��б���Ŀ
void CTccGpcInfoProvider::GetStationList(std::vector<stano_t>& staList)
{
	staList.clear();

	//Ӧ����Ҫ�󣬴˳�վ�б�͵������������б�һ��
	int num=500;
	int stano[500];
	if(gpcdef.GetStaList(3, num, stano))
	{
		for(int i=0; i<num; i++)
		{
			WORD station=stano[i];
			int k=0;
			for(k=0; k<staList.size(); k++)
			{
				if(station == staList[k])
					break;
			}
			if(k>=staList.size())
			{
				staList.push_back(station);
			}
		}
	}
	
	return; 
}

// ����վ�ŵõ�վ��
CString CTccGpcInfoProvider::GetStationName(stano_t nStaID)
{
	CString szStaName;
	do {
		if(tcc_map.end() == tcc_map.find(nStaID))
			break;
		const TCC_STRUCT& tcc = tcc_map[nStaID];
		ASSERT(tcc.nTccID == nStaID);

		if(tcc.nParentStaID)
			szStaName = tcc.szName;
	} while(0);

	if(szStaName.IsEmpty())
		szStaName = gpcdef.GetStationName(nStaID);
	if(szStaName=="����")
		return "";
	return szStaName;
}


// �õ����λ�ͷ�����
CString CTccGpcInfoProvider::GetOrgSender()
{
	return CString("Casco Ltd.");
}

CString CTccGpcInfoProvider::GetPersonSender()
{
	return CString("SCX");
}

WORD CTccGpcInfoProvider::GetConsoleID()
{
	return m_nCurrentSectionNo;//gpcdef.GetDispatchNoFromDefaultUser();//������ָ�ĸ�����̨
}

DWORD CTccGpcInfoProvider::GetCurrentEntityID()
{
	GPCENTITY  entityInfo;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
		return entityInfo.nEntityId;

	return 0;
}

// վnStaID2�����վnStaID�����䷽����(���з���, ���з���)
dir_t ITccClientInfoProvider::GetStationDirection(stano_t nStaID, stano_t nStaID2)
{
	dir_t dir = DIR_UNKNOWN;

	do
	{
		if(nStaID == 0 || nStaID2 == 0)
			break;

		if(tcc_map.end() == tcc_map.find(nStaID))
			break;
		const TCC_STRUCT& tcc = tcc_map[nStaID];
		ASSERT(tcc.nTccID == nStaID);

		for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
			iter != tcc.line_map.end();
			iter ++)
		{
			const TCC_STRUCT::_LINE& tcc_line = iter->second;
			if(tcc_line.nNextTccID == nStaID2)
			{
				dir = DIR_DOWN;
				break;
			}
			if(tcc_line.nPrevTccID == nStaID2)
			{
				dir = DIR_UP;
				break;
			}
		}
	}while(0);

	return dir;
}

// �ж�nStaID��nStaID2 ������״̬
BOOL ITccClientInfoProvider::IsLinkedStation(stano_t nStaID, stano_t nStaID2)
{
	std::vector<stano_t> staList;
	GetLinkedStationList(nStaID, staList);

	return staList.end() != find(staList.begin(), staList.end(), nStaID2);
}

// �õ�nStaIDվ������վ�б�
void ITccClientInfoProvider::GetLinkedStationList(stano_t nStaID, std::vector<stano_t>& staList)
{
	staList.clear();

	if(tcc_map.end() == tcc_map.find(nStaID))
		return;
	const TCC_STRUCT& tcc = tcc_map[nStaID];
	ASSERT(tcc.nTccID == nStaID);

	for(std::map<int, TCC_STRUCT::_LINE>::const_iterator iter = tcc.line_map.begin();
		iter != tcc.line_map.end();
		iter ++)
	{
		const TCC_STRUCT::_LINE& tcc_line = iter->second;

		if(0 != tcc_line.nNextTccID)// && !IsZC_TccID(tcc_line.nNextTccID))
		{
			if( staList.end() == find(staList.begin(), staList.end(), tcc_line.nNextTccID) )
				staList.push_back(tcc_line.nNextTccID);
		}

		if(0 != tcc_line.nPrevTccID)// && !IsZC_TccID(tcc_line.nPrevTccID))
		{
			if( staList.end() == find(staList.begin(), staList.end(), tcc_line.nPrevTccID) )
				staList.push_back(tcc_line.nPrevTccID);
		}
	}
}

// ����վ�ź�Absline��ŵõ���������һ��վ
stano_t ITccClientInfoProvider::GetLinkedStation(stano_t nStaID, lineid_t nTccLineID, dir_t dir)
{
	stano_t nTempStaID = 0;

	do {
		if(nStaID <= 0 || nTccLineID <= 0)
			break;
	    
		if(tcc_map.end() == tcc_map.find(nStaID))
			break;
		TCC_STRUCT& tcc = tcc_map[nStaID];
		ASSERT(tcc.nTccID == nStaID);

		if(tcc.line_map.end() == tcc.line_map.find(nTccLineID))
			break;
		const TCC_STRUCT::_LINE& tcc_line = tcc.line_map[nTccLineID];
		ASSERT(tcc_line.tccline_id == nTccLineID);

		switch(dir)
		{
		case DIR_DOWN:
			nTempStaID = tcc_line.nNextTccID;
			break;

		case DIR_UP:
			nTempStaID = tcc_line.nPrevTccID;
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	} while(0);

	return nTempStaID;
}

// У��վ�ŵĺϷ���
BOOL CTccGpcInfoProvider::ValidateStationID(stano_t nStaID)
{
	ushort RcvSTCount;
	int nDispatchNo = gpcdef.GetDispatchNoFromDefaultUser();
	RcvSTCount = gpcdef.GetRcvDCStationNum(nDispatchNo);

	for(int i=0; i<RcvSTCount; i++)
	{
		if(gpcdef.GetRcvDCStationList(nDispatchNo, i) == nStaID)
			break;
	}

	return i < RcvSTCount;

}

BOOL CTccGpcInfoProvider::IsStationListChanged()
{
	BOOL bRet = FALSE;
	if(m_nSecNo != m_nCurrentSectionNo)
		bRet = TRUE;
	m_nSecNo = m_nCurrentSectionNo;
	return bRet;
}

// �õ������������ֵ��Χ
//BOOL CTccGpcInfoProvider::IsValidCommandID(cmdid_t nCmdID)
//{
//	return nCmdID != GetInvalidCommandID();
//}
//
//cmdid_t CTccGpcInfoProvider::GetInvalidCommandID()
//{
//	return 0;
//}
//
//cmdid_t CTccGpcInfoProvider::GetMinCommandID()
//{
//	return 1;
//}
//
//cmdid_t CTccGpcInfoProvider::GetMaxCommandID()
//{
//	return 9999;
//}
//
//cmdid_t CTccGpcInfoProvider::GetNextCommandID(cmdid_t nCmdID)
//{
//	cmdid_t nNextCmdID = nCmdID + 1;
//	
//	if(nNextCmdID == GetInvalidCommandID())
//		nNextCmdID ++;
//
//	if(nNextCmdID > GetMaxCommandID() || nNextCmdID < GetMinCommandID())
//		nNextCmdID = GetMinCommandID();
//
//	return nNextCmdID;
//}


};
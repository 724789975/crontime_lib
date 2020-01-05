#ifndef __CRON_TIME_DEF_H__
#define __CRON_TIME_DEF_H__

#include "meta_header/t_bit_set.h"
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <stdio.h>
#include <time.h>

/***********************************************************************
*	*	*	*	*	*
-	-	-	-	-	-
|	|	|	|	|	|
|	|	|	|	|	+----- 星期中星期几 (0 - 6) (星期天 为0)
|	|	|	|	+---------- 月份 (1 - 12)
|	|	|	+--------------- 一个月中的第几天 (1 - 31)
|	|	+-------------------- 小时 (0 - 23)
|	+------------------------- 分钟 (0 - 59)
+------------------------------ 秒 (0 - 59)

0 * * * * *
每月每天每小时的第 0 秒执行一次

0 0 6-12/3 * 12 *
在 12 月内, 每天的早上 6 点到 12 点，每隔 3 个小时 0 分钟执行一次

0 0 17 * * 1-5
周一到周五每天下午 5:00

0 20 0-23/2 * * *
每月每天的午夜 0 点 20 分, 2 点 20 分, 4 点 20 分....执行

0 0 0,13,18,21 * * *
每天的0点、13点、18点、21点都执行一次
***********************************************************************/

#define	SECONDS_PER_MINUTE	60
#define	SECONDS_PER_HOUR	3600

#define	FIRST_MINUTE	0
#define	LAST_MINUTE	59
#define	MINUTE_COUNT	(LAST_MINUTE - FIRST_MINUTE + 1)

#define	FIRST_HOUR	0
#define	LAST_HOUR	23
#define	HOUR_COUNT	(LAST_HOUR - FIRST_HOUR + 1)

#define	FIRST_DOM	1
#define	LAST_DOM	31
#define	DOM_COUNT	(LAST_DOM - FIRST_DOM + 1)

#define	FIRST_MONTH	1
#define	LAST_MONTH	12
#define	MONTH_COUNT	(LAST_MONTH - FIRST_MONTH + 1)

#define	FIRST_DOW	0
#define	LAST_DOW	6
#define	DOW_COUNT	(LAST_DOW - FIRST_DOW + 1)

#ifdef WIN32
#define LOCALTIME(p_tt, p_tm) localtime_s(p_tm, p_tt)
#else
#define LOCALTIME(p_tt, p_tm) localtime_r(p_tt, p_tm)
#endif // WIN32


namespace CronTime
{
	BeginEnumBitSet(ESecond)
	{
		EndEnumBitSetWithCount(ESecond, SECONDS_PER_MINUTE)
	};
	EnumBitSet(ESecond);
	BeginEnumBitSet(EMinute)
	{
		EndEnumBitSetWithCount(EMinute, MINUTE_COUNT)
	};
	EnumBitSet(EMinute);
	BeginEnumBitSet(EHour)
	{
		EndEnumBitSetWithCount(EHour, HOUR_COUNT)
	};
	EnumBitSet(EHour);
	BeginEnumBitSet(EDom)
	{
		EndEnumBitSetWithCount(EDom, DOM_COUNT)
	};
	EnumBitSet(EDom);
	BeginEnumBitSet(EMonth)
	{
		EndEnumBitSetWithCount(EMonth, MONTH_COUNT)
	};
	EnumBitSet(EMonth);
	BeginEnumBitSet(EDow)
	{
		EndEnumBitSetWithCount(EDow, DOW_COUNT)
	};
	EnumBitSet(EDow);

	class CronTimeException : public std::exception
	{
	public:
		CronTimeException(std::string szFile, int dwLine, std::string szFuncName)
			: m_strFile(szFile)
			, m_dwLine(dwLine)
			, m_szFuncName(szFuncName)
		{}
		virtual char const* what() const
		{
			std::stringstream  strStream;
			strStream << "CronTime 异常 : [" << m_strFile << ", " << m_dwLine << ", " << m_szFuncName << "]";
			return strStream.str().c_str();
		}

	private:
		int m_dwLine;
		std::string m_strFile;
		std::string m_szFuncName;
	};

//#define CTException
#ifdef CTException
#define ThrowCTException(szFile, dwLine, szFuncName) throw CronTimeException(szFile, dwLine, szFuncName)
#else
#define ThrowCTException(szFile, dwLine, szFuncName)
#endif // CTException


	class Timer
	{
	public:
		Timer(const char* szCronStr, unsigned int dwLen)
		{
			m_bIsValid = true;
			const char* szTemp1 = szCronStr;

			//解析','
			const char* _szTemp_1 = szTemp1;
			bool bComma = false;
			//
			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//秒
			for (const char*  i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oSecond, _szTemp_1, i - _szTemp_1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oSecond, _szTemp_1, szCronStr - _szTemp_1);
			}
			else
			{
				_Parse(m_oSecond, szTemp1, szCronStr - szTemp1);
			}
			szTemp1 = ++szCronStr;
			_szTemp_1 = szTemp1;
			bComma = false;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//分
			for (const char* i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oMinute, _szTemp_1, i - _szTemp_1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oMinute, _szTemp_1, szCronStr - _szTemp_1);
			}
			else
			{
				_Parse(m_oMinute, szTemp1, szCronStr - szTemp1);
			}
			szTemp1 = ++szCronStr;
			_szTemp_1 = szTemp1;
			bComma = false;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//时
			for (const char* i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oHour, _szTemp_1, i - _szTemp_1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oHour, _szTemp_1, szCronStr - _szTemp_1);
			}
			else
			{
				_Parse(m_oHour, szTemp1, szCronStr - szTemp1);
			}
			szTemp1 = ++szCronStr;
			_szTemp_1 = szTemp1;
			bComma = false;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//天 / 月
			for (const char* i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oDom, _szTemp_1, i - _szTemp_1, 1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oDom, _szTemp_1, szCronStr - _szTemp_1, 1);
			}
			else
			{
				_Parse(m_oDom, szTemp1, szCronStr - szTemp1, 1);
			}
			szTemp1 = ++szCronStr;
			_szTemp_1 = szTemp1;
			bComma = false;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//月
			for (const char* i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oMonth, _szTemp_1, i - _szTemp_1, 1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oMonth, _szTemp_1, szCronStr - _szTemp_1, 1);
			}
			else
			{
				_Parse(m_oMonth, szTemp1, szCronStr - szTemp1, 1);
			}
			szTemp1 = ++szCronStr;
			_szTemp_1 = szTemp1;
			bComma = false;

			if (dwLen <= 0)
			{
				m_bIsValid = false;
				ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
				return;
			}
			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//天 / 周
			for (const char* i = szTemp1; i < szCronStr; ++i)
			{
				if (*i == ',')
				{
					_Parse(m_oDow, _szTemp_1, i - _szTemp_1, 1);
					_szTemp_1 = ++i;
					bComma = true;
				}
			}
			if (bComma)
			{
				_Parse(m_oDow, _szTemp_1, szCronStr - _szTemp_1);
			}
			else
			{
				_Parse(m_oDow, szTemp1, szCronStr - szTemp1);
			}

			if (m_bIsValid)
			{
				//如果 周天数跟月天数 都存在不满的情况 那么就是无效的
				bool bWValid = true;
				for (unsigned int i = 0; i < m_oDow.GetCount(); ++i)
				{
					if (!m_oDow.GetBit(i))
					{
						bWValid = false;
						break;
					}
				}

				bool bMValid = true;
				for (unsigned int i = 0; i < m_oDom.GetCount(); ++i)
				{
					if (!m_oDom.GetBit(i))
					{
						bMValid = false;
						break;
					}
				}
				m_bIsValid = bWValid || bMValid;
				if (!m_bIsValid)
				{
					ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
				}
			}
		}

		bool GetValid()const { return m_bIsValid; }

		bool Hit(const time_t& ttNow)const
		{
			struct tm sCurrent;
			LOCALTIME(&ttNow, &sCurrent);
			return m_oSecond.GetBit(sCurrent.tm_sec) && m_oMinute.GetBit(sCurrent.tm_hour)
				&& m_oHour.GetBit(sCurrent.tm_hour) && m_oDom.GetBit(sCurrent.tm_mday - 1)
				&& m_oMonth.GetBit(sCurrent.tm_mon) && m_oDow.GetBit(sCurrent.tm_wday);
		}

		time_t GetNextMatch(time_t ttNow)const
		{
			//有时区问题 所以要取下时间
			struct tm sCurrent;
			LOCALTIME(&ttNow, &sCurrent);
			if (!(m_oDom.GetBit(sCurrent.tm_mday - 1) && m_oMonth.GetBit(sCurrent.tm_mon) && m_oDow.GetBit(sCurrent.tm_wday)))
			{
				//如果天数不匹配 就从第二天的0点开始
				sCurrent.tm_sec = 0;
				sCurrent.tm_min = 0;
				sCurrent.tm_hour = 24;
				ttNow = mktime(&sCurrent);
			}
			//匹配秒
			while (!m_oSecond.GetBit((++ttNow) % SECONDS_PER_MINUTE));
			//匹配分
			while (!m_oMinute.GetBit((ttNow / SECONDS_PER_MINUTE) % MINUTE_COUNT))
			{
				ttNow += SECONDS_PER_MINUTE;
			}

			while (!m_oHour.GetBit(sCurrent.tm_hour))
			{
				ttNow += SECONDS_PER_HOUR;
				LOCALTIME(&ttNow, &sCurrent);
			}

			while (!m_oDom.GetBit(sCurrent.tm_mday - 1))
			{
				ttNow += SECONDS_PER_HOUR * HOUR_COUNT;
				LOCALTIME(&ttNow, &sCurrent);
			}

			while (!m_oDow.GetBit(sCurrent.tm_wday))
			{
				ttNow += SECONDS_PER_HOUR * HOUR_COUNT;
				LOCALTIME(&ttNow, &sCurrent);
			}

			while (!m_oMonth.GetBit(sCurrent.tm_mon))
			{
				++sCurrent.tm_mon;
				ttNow = mktime(&sCurrent);
				LOCALTIME(&ttNow, &sCurrent);
			}


			return ttNow;
		}

		void DumpInfo(std::ostream& refStream)
		{
			m_oSecond.DumpInfo(refStream);
			m_oMinute.DumpInfo(refStream);
			m_oHour.DumpInfo(refStream);
			m_oDom.DumpInfo(refStream);
			m_oMonth.DumpInfo(refStream);
			m_oDow.DumpInfo(refStream);
		}
	private:
		void _Parse(BitSetInterface& refBitSet, const char* szCron, const unsigned int& dwLen,
			const unsigned int& dwLow = 0)
		{
			//先查找'/'
			bool bSplit1 = false;
			const char* szTemp = szCron;
			for (;szTemp < szCron + dwLen; ++szTemp)
			{
				if (*szTemp == '/')
				{
					++szTemp;
					bSplit1 = true;
					break;
				}
			}

			if (bSplit1)
			{
				//从什么时候开始 间隔多久执行 第二个一定是数字
				unsigned int dwNum2 = 0;
				if (!_GetNumber(szTemp, dwNum2))
				{
					m_bIsValid = false;
					ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
					return;
				}
				dwNum2 -= dwLow;
				if (dwNum2 >= refBitSet.GetCount())
				{
					m_bIsValid = false;
					ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
					return;
				}

				//是不是有'-'分割
				bool bSplit2 = false;
				const char* szTemp1 = szCron;
				for (; szTemp1 < szCron + dwLen; ++szTemp1)
				{
					if (*szTemp1 == '-')
					{
						++szTemp1;
						bSplit2 = true;
						break;
					}
				}
				unsigned int dwNum_1 = 0;
				unsigned int dwNum_2 = refBitSet.GetCount() - 1;
				if (bSplit2)
				{
					//如果有分割 那么就是时间段 内 每隔多长时间 (1-23/3 * * * *)
					if (!_GetNumber(szCron, dwNum_1))
					{
						m_bIsValid = false;
						ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
						return;
					}
					dwNum_1 -= dwLow;
					if (!_GetNumber(szTemp1, dwNum_2))
					{
						m_bIsValid = false;
						ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
						return;
					}
					dwNum_2 -= dwLow;
					if (dwNum_2 <= dwNum_1)
					{
						m_bIsValid = false;
						ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
						return;
					}
					if (dwNum_2 >= refBitSet.GetCount())
					{
						m_bIsValid = false;
						ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
						return;
					}
				}
				else
				{
					if (*szCron == '*')
					{
						dwNum_1 = 0;
					}
					else
					{
						if (!_GetNumber(szCron, dwNum_1))
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						dwNum_1 -= dwLow;
						if (dwNum_1 >= refBitSet.GetCount())
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
					}
				}
				for (unsigned int i = dwNum_1; i <= dwNum_2; i += dwNum2)
				{
					refBitSet.SetBit(i, true);
				}
			}
			else
			{
				//如果第一个是* 那么就是所有的都有效
				if (*szCron == '*')
				{
					for (unsigned int i = 0; i < refBitSet.GetCount(); ++i)
					{
						refBitSet.SetBit(i, true);
					}
				}
				else
				{
					//是不是有'-'分割
					bool bSplit2 = false;
					const char* szTemp = szCron;
					for (; szTemp < szCron + dwLen; ++szTemp)
					{
						if (*szTemp == '-')
						{
							++szTemp;
							bSplit2 = true;
							break;
						}
					}
					if (bSplit2)
					{
						//如果有分割 那么就是时间段
						unsigned int dwNum1 = 0;
						if (!_GetNumber(szCron, dwNum1))
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						dwNum1 -= dwLow;
						unsigned int dwNum2 = 0;
						if (!_GetNumber(szTemp, dwNum2))
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						dwNum2 -= dwLow;
						if (dwNum1 >= dwNum2)
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						if (dwNum2 >= refBitSet.GetCount())
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						for (unsigned int i = dwNum1; i <= dwNum2; ++i)
						{
							refBitSet.SetBit(i, true);
						}
					}
					else
					{
						unsigned int dwNum = 0;
						if (!_GetNumber(szCron, dwNum))
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						dwNum -= dwLow;
						if (dwNum >= refBitSet.GetCount())
						{
							m_bIsValid = false;
							ThrowCTException(__FILE__, __LINE__, __FUNCTION__);
							return;
						}
						refBitSet.SetBit(dwNum, true);
					}
				}
			}
		}

		bool _GetNumber(const char* ref_szCron, unsigned int& dwOut)
		{
			dwOut = 0;
			int bRet = false;
			while ((*ref_szCron) >= '0' && (*ref_szCron) <= '9')
			{
				dwOut = dwOut * 10 + *ref_szCron - '0';
				bRet = true;
				++ref_szCron;
			}
			return bRet;
		}

		bool _MatchDay(const tm& refTM)
		{
			return m_oDow.GetBit(refTM.tm_mday - 1) || m_oDom.GetBit(refTM.tm_wday);
		}

		ESecondBitSet m_oSecond;
		EMinuteBitSet m_oMinute;
		EHourBitSet m_oHour;
		EDomBitSet m_oDom;
		EMonthBitSet m_oMonth;
		EDowBitSet m_oDow;

		bool m_bIsValid;
	};
}

#endif // __CRON_TIME_DEF_H__


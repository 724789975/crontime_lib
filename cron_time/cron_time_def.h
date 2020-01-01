#ifndef __CRON_TIME_DEF_H__
#define __CRON_TIME_DEF_H__

#include "meta_header/t_bit_set.h"
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

/* note on DOW: 0 and 7 are both Sunday, for compatibility reasons. */
#define	FIRST_DOW	0
#define	LAST_DOW	7
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

	class Timer
	{
	public:
		Timer(const char* szCronStr, unsigned int dwLen)
		{
			m_bIsValid = true;
			const char* szTemp1 = szCronStr;
			//
			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//秒
			_Parse(m_oSecond, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
				--dwLen;
			}
			//分
			_Parse(m_oMinute, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//时
			_Parse(m_oHour, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//天 / 月
			_Parse(m_oDom, szTemp1, szCronStr - szTemp1, 1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//月
			_Parse(m_oMonth, szTemp1, szCronStr - szTemp1, 1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//天 / 周
			_Parse(m_oDow, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

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
				m_bIsValid = bWValid && bMValid;
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
		void _Parse(BetSetInterface& refBitSet, const char* szCron, const unsigned int& dwLen,
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
					assert(0);
					m_bIsValid = false;
					return;
				}
				dwNum2 -= dwLow;
				assert(dwNum2 < refBitSet.GetCount());
				if (dwNum2 >= refBitSet.GetCount())
				{
					m_bIsValid = false;
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
						assert(0);
						m_bIsValid = false;
						return;
					}
					dwNum_1 -= dwLow;
					if (!_GetNumber(szTemp1, dwNum_2))
					{
						assert(0);
						m_bIsValid = false;
						return;
					}
					dwNum_2 -= dwLow;
					assert(dwNum_1 < dwNum_2);
					if (dwNum_2 >= dwNum_1)
					{
						m_bIsValid = false;
						return;
					}
					assert(dwNum_2 < refBitSet.GetCount());
					if (dwNum_2 >= refBitSet.GetCount())
					{
						m_bIsValid = false;
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
							assert(0);
							m_bIsValid = false;
							return;
						}
						dwNum_1 -= dwLow;
						assert(dwNum_1 < refBitSet.GetCount());
						if (dwNum_1 >= refBitSet.GetCount())
						{
							m_bIsValid = false;
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
							assert(0);
							m_bIsValid = false;
							return;
						}
						dwNum1 -= dwLow;
						unsigned int dwNum2 = 0;
						if (!_GetNumber(szTemp, dwNum2))
						{
							assert(0);
							m_bIsValid = false;
							return;
						}
						dwNum2 -= dwLow;
						assert(dwNum1 < dwNum2);
						if (dwNum1 >= dwNum2)
						{
							m_bIsValid = false;
							return;
						}
						assert(dwNum2 < refBitSet.GetCount());
						if (dwNum2 >= refBitSet.GetCount())
						{
							m_bIsValid = false;
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
							assert(0);
							m_bIsValid = false;
							return;
						}
						dwNum -= dwLow;
						assert(dwNum < refBitSet.GetCount());
						if (dwNum >= refBitSet.GetCount())
						{
							m_bIsValid = false;
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


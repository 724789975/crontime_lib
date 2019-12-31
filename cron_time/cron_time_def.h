#ifndef __CRON_TIME_DEF_H__
#define __CRON_TIME_DEF_H__

#include "meta_header/t_bit_set.h"
#include <stdexcept>
#include <assert.h>

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

namespace CronTime
{
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
			const char* szTemp1 = szCronStr;
			//秒
			//TODO

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
			_Parse(m_oDom, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//月
			_Parse(m_oMonth, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;

			while (*szCronStr != ' ' && *szCronStr != '\0' && dwLen > 0)
			{
				++szCronStr;
			}
			//天 / 周
			_Parse(m_oDow, szTemp1, szCronStr - szTemp1);
			szTemp1 = ++szCronStr;
		}

		void DumpInfo(std::ostream& refStream)
		{
			m_oMinute.DumpInfo(refStream);
			m_oHour.DumpInfo(refStream);
			m_oDom.DumpInfo(refStream);
			m_oMonth.DumpInfo(refStream);
			m_oDow.DumpInfo(refStream);
		}
	private:
		void _Parse(BetSetInterface& refBitSet, const char* szCron, const unsigned int& dwLen)
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
				}
				assert(dwNum2 < refBitSet.GetCount());

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
					}
					if (!_GetNumber(szTemp1, dwNum_2))
					{
						assert(0);
					}
					assert(dwNum_1 < dwNum_2);
					assert(dwNum_2 < refBitSet.GetCount());
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
						}
						assert(dwNum_1 < refBitSet.GetCount());
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
						}
						unsigned int dwNum2 = 0;
						if (!_GetNumber(szTemp, dwNum2))
						{
							assert(0);
						}
						assert(dwNum1 < dwNum2);
						assert(dwNum2 < refBitSet.GetCount());
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
						}
						assert(dwNum < refBitSet.GetCount());
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

		EMinuteBitSet m_oMinute;
		EHourBitSet m_oHour;
		EDomBitSet m_oDom;
		EMonthBitSet m_oMonth;
		EDowBitSet m_oDow;
	};
}

#endif // __CRON_TIME_DEF_H__


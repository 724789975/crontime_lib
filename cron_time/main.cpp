#include "cron_time_def.h"
#include <iostream>

int main()
{
	std::string sz = "0 0 * 2 1 *";
	CronTime::Timer t(sz.c_str(), sz.size());
	t.DumpInfo(std::cout);

	time_t n = time(0);
	bool b = t.Hit(n);

	for (size_t i = 0; i < 10; i++)
	{
		n = t.GetNextMatch(n);
		std::cout << n << "\n";
	}

	sz = "3-5,7-20 1-9/3 * 4 5-8,9-11 *";
	CronTime::Timer t1(sz.c_str(), sz.size());
	t1.DumpInfo(std::cout);

	b = t1.Hit(time(0));

	for (size_t i = 0; i < 10; i++)
	{
		n = t1.GetNextMatch(n);
		std::cout << n << "\n";
	}

	return 0;
}
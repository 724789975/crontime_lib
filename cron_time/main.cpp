#include "cron_time_def.h"
#include <iostream>

int main()
{
	std::string sz = "* * 1 2 *";
	CronTime::Timer t(sz.c_str(), sz.size());
	t.DumpInfo(std::cout);

	sz = "1-9/3 * 4 5-8 *";
	CronTime::Timer t1(sz.c_str(), sz.size());
	t1.DumpInfo(std::cout);

	return 0;
}
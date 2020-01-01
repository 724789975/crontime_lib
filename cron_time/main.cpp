#include "cron_time_def.h"
#include <iostream>

int main()
{
	std::string sz = "* * * 2 1 *";
	CronTime::Timer t(sz.c_str(), sz.size());
	t.DumpInfo(std::cout);

	bool b = t.Hit(time(0));

	sz = "* 1-9/3 * 4 5-8 *";
	CronTime::Timer t1(sz.c_str(), sz.size());
	t1.DumpInfo(std::cout);

	b = t1.Hit(time(0));

	return 0;
}
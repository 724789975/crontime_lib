#include "cron_time_def.h"
#include <iostream>
#include <exception>

int main()
{
	try
	{
		std::string sz = "0 0 * 2 * *";
		CronTime::Timer t(sz.c_str(), sz.size());
		t.DumpInfo(std::cout);

		time_t n = time(0);
		bool b = t.Hit(n);

		for (size_t i = 0; i < 10; i++)
		{
			n = t.GetNextMatch(n);
			std::cout << n << "\n";
		}

		sz = "3-5 1-9/3 * 4 5-8,9-11 *";
		CronTime::Timer t1(sz.c_str(), sz.size());
		t1.DumpInfo(std::cout);

		n = time(0);
		b = t1.Hit(n);

		for (size_t i = 0; i < 10; i++)
		{
			n = t1.GetNextMatch(n);
			std::cout << n << "\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}

	return 0;
}
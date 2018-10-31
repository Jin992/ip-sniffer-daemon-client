#include "../includes/shared.h"

int check_device(char * dev_name)
{
	struct ifaddrs *addrs,*tmp;

	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
		{
			if (strcmp(tmp->ifa_name, dev_name) == 0)
			{
				freeifaddrs(addrs);
				return EXIT_SUCCESS;
			}
		}
		tmp = tmp->ifa_next;
	}
	freeifaddrs(addrs);
	return EXIT_FAILURE;
}
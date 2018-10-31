#include "../includes/daemon.h"

static void usage(char *app_name)
{
	printf("Usage:\n");
	printf("\t%s\t\t\t-\tto launch daemon.\n", app_name);
	printf("\t%s -s [iface name]\t-\tto launch daemon and start.\n", app_name);
}

int start(int argc, char **argv, ctrl_t *c_unit)
{
	if (argc == 2)
	{
		printf("%s : invalid flag %s", argv[0], argv[1]);
		usage(argv[0]);
		return -1;
	}
	else if (argc == 3)
	{
		if (strcmp(argv[1], "-s") == 0)
		{
			if (check_device(argv[2]) == 1)
			{
				printf("Invalid iface: %s", argv[2]);
				return -1;
			}
			else
			{
				c_unit->start = 1;
				memset(c_unit->cur_device, 0, IFACE_SIZE);
				memcpy(c_unit->cur_device, argv[2], strlen(argv[2]));
			}
		}
		else
		{
			printf("%s : invalid flag %s.\n", argv[0], argv[1]);
			usage(argv[0]);
			return -1;
		}
	}
	else
		c_unit->start = 0;
	return 0;
}

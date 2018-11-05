#include "../includes/daemon.h"

static int ctrl_init(ctrl_t *c_unit)
{
	struct ifaddrs *addrs,*tmp;

	if (c_unit->start == 0)
	{
		getifaddrs(&addrs);
		tmp = addrs;
		if (tmp != NULL)
			tmp = tmp->ifa_next;
		else
			return -1;
		if (tmp != NULL) {
			if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
				memset(c_unit->cur_device, 0, IFACE_SIZE);
				memcpy(c_unit->cur_device, tmp->ifa_name, strlen(tmp->ifa_name));
			} else
				return -1;
		} else
			return -1;
		freeifaddrs(addrs);
	}
	c_unit->stop = 0;
	c_unit->root = NULL;
	return EXIT_SUCCESS;
}

int sniffer(ctrl_t *c_unit)
{
	key_t 			key;
	int 			msqid;
	int				sock_raw;
	int				client_recv;

	syslog(LOG_NOTICE, "-> starting...");
	/*ETH_P_ALL  -  receive all protocol packets*/
	sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock_raw < 0)
	{
		syslog(LOG_NOTICE, "-> socket error: %s.", strerror(errno));
		return (-1);
	}
	if ((msqid = init_msq(&key)) == -1)
		return (-1);
	load_session(c_unit);
	while (1)
	{
		client_recv = client_handler(msqid, c_unit, &sock_raw);
		if (client_recv == -1)
		{
			close(sock_raw);
			return(-1);
		}
		else if (client_recv == 1)
			break;
		if (c_unit->start == 0)
		{
			sleep(1);
			continue;
		}
		if (c_unit->stop == 1)
			c_unit->start = 1;
		if (rqvst_handler(sock_raw, c_unit) == -1)
		{
			close(sock_raw);
			return (-1);
		}
	}
	close(sock_raw);
	if (msgctl(msqid, IPC_RMID, NULL) == -1)
	{
		syslog(LOG_NOTICE, "-> msgctl error %s.", strerror(errno));
		return (-1);
	}
	if (save_cur_session(c_unit->root))
	{
		syslog(LOG_NOTICE, "-> error: session not saved.");
		return (-1);
	}
	else
		syslog(LOG_NOTICE, "-> session successfully saved.");
	clean_tree(c_unit->root);
	syslog(LOG_NOTICE, "-> terminatig daemon....");
	return (EXIT_SUCCESS);
}


int main(int argc, char **argv)
{
	pid_t			pid, sid;
	ctrl_t			c_unit;

	if (start(argc, argv, &c_unit))
		return EXIT_FAILURE;
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	sid = setsid();

	if (sid < 0)
	{
		printf("setsid error\n");
		exit(EXIT_FAILURE);
	}
	if (chdir("/") < 0)
	{
		printf("chdir error\n");
		exit(EXIT_FAILURE);
	}
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	close(STDIN_FILENO);

	openlog("sniffer", LOG_PID, LOG_DAEMON);
	if (ctrl_init(&c_unit) == -1)
	{
		syslog(LOG_NOTICE, "-> iface error.");
		return -1;
	}
	if (sniffer(&c_unit) == -1)
	{
		closelog();
		exit(EXIT_FAILURE);
	}
	closelog();
	exit(EXIT_SUCCESS);
}



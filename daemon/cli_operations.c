#include "../includes/daemon.h"

/*Get sum of packets from all ifaces*/
static uint32_t total_qnt(iface_t *list)
{
	iface_t *tmp = list;
	uint32_t qnt;

	qnt = 0;
	while (tmp != NULL)
	{
		if (strcmp(tmp->name, "s_qnt") != 0)
			qnt += tmp->qnt;
		tmp = tmp->next;
	}
	return qnt;
}

/*Get total packets qnt from ip all ifaces*/
static uint32_t get_ip_recv_qnt( ctrl_t *c_unit, uint32_t ip)
{
	node_t *tmp;
	uint32_t t;

	tmp = avl_search(c_unit->root, ip);
	if (tmp == NULL)
		return (0);
	t = total_qnt(tmp->iface);
	return t;
}

/*Get packets qnt from chosen iface*/
static uint32_t get_iface_stat(iface_t *list, char *name)
{
	iface_t *tmp = list;

	while (tmp != NULL)
	{
		if (strcmp(tmp->name, name) == 0)
			return tmp->qnt;
		tmp = tmp->next;
	}
	return 0;
}

/*Send  ip -> package qnt values from chosen iface*/
static int send_iface_stat(int msqid, node_t *node, char *iface)
{
	node_t *tmp = node;
	uint32_t qnt;

	if (tmp != NULL)
	{
		if (send_iface_stat(msqid, tmp->left, iface) == -1)
			return -1;
		if ((qnt = get_iface_stat(tmp->iface, iface)) > 0)
		{
			if (send_msg(msqid, 0, tmp->ip, qnt, MSG_OUT) == -1)
				return -1;
		}
		if (send_iface_stat(msqid,tmp->right, iface) == -1)
			return (-1);
	}
	return 0;
}

int op_start( ctrl_t *c_unit)
{
	c_unit->start = 1;
	syslog(LOG_NOTICE, "-> starting daemon.");
	return 0;
}

int op_stop(ctrl_t *c_unit)
{

	syslog(LOG_NOTICE, "-> stoping daemon.");
	c_unit->start = 0;
	return 0;
}

int op_show(int msqid, ctrl_t *c_unit, uint32_t ip)
{
	if (send_msg(msqid, 0, ip, get_ip_recv_qnt(c_unit, ip), MSG_OUT) == -1)
	{
		syslog(LOG_NOTICE, "send_msg error.");
		return (-1);
	}
	return EXIT_SUCCESS;
}

int op_select(int sock_raw, ctrl_t *c_unit, msgbuf_t *buf)
{
	char		iface[IFACE_SIZE];
	uint32_t	stat;

	memset(iface, 0, IFACE_SIZE);
	unpack_status_package(buf->data, &stat, iface);
	memset(c_unit->cur_device, 0, IFACE_SIZE + 1);
	memcpy(c_unit->cur_device, iface, IFACE_SIZE);
	if ((setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, \
                     c_unit->cur_device, (socklen_t)strlen(c_unit->cur_device))) < 0)
	{
		syslog(LOG_NOTICE, "%s.", strerror(errno));
		return (-1);
	}
	syslog(LOG_NOTICE, "-> interface changed to %s", c_unit->cur_device);
	return 0;
}

int op_stat(int msqid, ctrl_t *c_unit, msgbuf_t *buf)
{
	char				iface[IFACE_SIZE];
	struct ifaddrs		*addrs,*tmp;
	uint32_t			stat;

	memset(iface, 0, IFACE_SIZE);
	unpack_status_package(buf->data, &stat, iface);
	syslog(LOG_NOTICE, "-> stat for %s iface\n", iface);
	if (strcmp(iface, "all") == 0)
	{
		getifaddrs(&addrs);
		tmp = addrs;
		while (tmp)
		{
			if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
			{
				if (send_iface_stat(msqid, c_unit->root, tmp->ifa_name) == -1)
					return -1;
			}
			tmp = tmp->ifa_next;
		}
		freeifaddrs(addrs);
	}
	else
	{
		if (send_iface_stat(msqid, c_unit->root, iface) == -1)
			return -1;
	}
	if (send_msg(msqid, 20, 0, 0, MSG_OUT) == -1)
		return -1;
	return 0;
}

int op_daemon_stat(int msqid, ctrl_t *c_unit)
{
	msgbuf_t buf;

	pack_status_package(buf.data, c_unit->start, c_unit->cur_device);
	buf.mtype = MSG_OUT;
	if (msgsnd(msqid, &buf, MSG_SIZE, 0) == -1) {
		perror("msgsnd");
		return -1;
	}
	return 0;
}



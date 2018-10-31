//
// Created by jin on 29.10.18.
//
#include "../includes/client.h"

int recv_daemon_status(int msqid)
{
	msgbuf_t 		buf;
	uint32_t		status = 0;
	char			dev_name[20];

	if (send_msg(msqid, DAEMON, 0, 0, MSG_OUT) == -1)
	{
		perror("send_msg");
		return (-1);
	}
	if (msgrcv(msqid, &buf, MSG_SIZE, MSG_IN, MSG_NOERROR) == -1)
	{
		if (errno != ENOMSG)
		{
			perror("msgrcv");
			return (-1);
		}
	}
	unpack_status_package(buf.data, &status, dev_name);
	printf("->daemon Status [%s] iface [%s]\n", status == 0? "OFF" : "ON", dev_name);
	return (EXIT_SUCCESS);
}

int recv_msg(int msqid)
{
	msgbuf_t 		buf;
	uint32_t		ip = 0;
	uint32_t 		qnt = 0;
	uint32_t 		op = 0;
	char			*ip_str;

	if (msgrcv(msqid, &buf, MSG_SIZE, MSG_IN, MSG_NOERROR) == -1)
	{
		if (errno != ENOMSG)
		{
			perror("msgrcv");
			return (-1);
		}
	}
	unpack_package(buf.data, &op, &ip, &qnt);
	if((ip_str = decode_ip(ip)) == NULL)
		return -1;
	if (op == 0)
		printf("-> [%u] packets received from %s\n", qnt, ip_str);
	free(ip_str);
	return (op);
}

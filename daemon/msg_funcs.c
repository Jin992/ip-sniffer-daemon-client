#include "../includes/daemon.h"

int init_msq(key_t *key)
{
	int msqid;

	if ((*key = ftok(FTOK_PATH, 'b')) == -1) {
		syslog(LOG_NOTICE, "-> ftok error %s.", strerror(errno));
		return (-1);
	}

	if ((msqid = msgget(*key, 0666 | IPC_CREAT)) == -1) {
		syslog(LOG_NOTICE, "-> msgget error : %s.", strerror(errno));
		return (-1);
	}
	return msqid;
}

int client_handler(int msqid, ctrl_t *c_unit, int sock_raw)
{
	msgbuf_t	buf;
	uint32_t	op = 0, qnt = 0, ip = 0;
	char		*decoded_ip;


	if (msgrcv(msqid, &buf, MSG_SIZE, MSG_IN, IPC_NOWAIT) == -1)
	{
		if (errno != ENOMSG) {
			syslog(LOG_NOTICE, "-> msrcv erroe : %s.", strerror(errno));
			return (-1);
		}
	}
	else
	{
		unpack_package(buf.data, &op, &ip, &qnt);
		if ((decoded_ip = decode_ip(ip)) == NULL)
			return -1;
		switch (op) {
			case OP_START:
				op_start(c_unit);
				break;
			case OP_STOP:
				op_stop(c_unit);
				break;
			case OP_SHOW_IP_QNT:
				if (op_show(msqid, c_unit, ip) == -1)
					return -1;
				break;
			case OP_SELECT_IFACE:
				if (op_select(sock_raw, c_unit, &buf) == -1)
					return -1;
				break;
			case OP_STAT_IFACE:
				if (op_stat(msqid, c_unit, &buf) == -1)
					return -1;
				break;
			case OP_DAEMON:
				if (op_daemon_stat(msqid, c_unit) == -1)
					return -1;
				break;
			case OP_SHUTDOWN:
				free(decoded_ip);
				return 1;
			default:
				syslog(LOG_NOTICE, "-> wrong op.");
				break;
		}
		free(decoded_ip);
	}
	return (EXIT_SUCCESS);
}


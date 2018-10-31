#include "../includes/shared.h"

int send_msg(int msqid, uint32_t  op, uint32_t ip, uint32_t qnt, int out)
{
	msgbuf_t		buf;

	buf.mtype = out;
	pack_package(buf.data, op, ip, qnt);
	if (msgsnd(msqid, &buf, MSG_SIZE, 0) == -1)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
#include "../includes/shared.h"

int pack_package(char* buf, uint32_t op, uint32_t ip, uint32_t qnt)
{
	int st;

	memset(buf, 0, MSG_SIZE);
	st = 0;
	memcpy(buf + st, &op, sizeof(op));
	st += sizeof(op);
	memcpy(buf + st, &ip, sizeof(ip));
	st += sizeof(ip);
	memcpy(buf + st, &qnt, sizeof(qnt));
	st += sizeof(qnt);

	return st;
}

int pack_status_package(char *buf, uint32_t status, char *device)
{
	int offset = 0;
	uint32_t device_len;

	device_len = (uint32_t)strlen(device);
	memset(buf, 0, MSG_SIZE);
	memcpy(buf + offset, &status, sizeof(status));
	offset += sizeof(status);
	memcpy(buf + offset, &device_len, sizeof(device_len));
	offset += sizeof(device_len);
	memcpy(buf + offset, device, (size_t)device_len);

	return (EXIT_SUCCESS);
}

void unpack_status_package(char *buf, uint32_t *status, char *device)
{
	size_t dev_len;

	*status = *(uint32_t *)&buf[0];
	dev_len = *(uint32_t *)&buf[4];
	memset(device, 0, dev_len + 1);
	memcpy(device, &buf[8], dev_len);
	device[dev_len + 1] = 0;
}

void unpack_package(const char *buf, uint32_t *op, uint32_t *ip, uint32_t *qnt)
{
	*op = *(uint32_t *)&buf[0];
	*ip = *(uint32_t *)&buf[4];
	if (qnt != NULL)
		*qnt = *(uint32_t *)&buf[8];
}
#include "../includes/daemon.h"

/*Add request to storage*/
static void add_recv(unsigned char * buffer, char *cuf_iface, ctrl_t *c_unit)
{
	struct sockaddr_in source;
	struct iphdr *iph = (struct iphdr *)buffer;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	c_unit->root = insert(c_unit->root, encode_ip(inet_ntoa(source.sin_addr)), INIT_QNT, cuf_iface);
	syslog(LOG_NOTICE, "-> %s.", inet_ntoa(source.sin_addr) );
}

/*Get incoming socket request from socket*/
int rqvst_handler(int sock_raw, ctrl_t *c_unit)
{
	socklen_t		saddr_size;
	ssize_t 		data_size;
	struct sockaddr	saddr;
	unsigned char	*buffer;

	if (!(buffer = (unsigned char *)malloc(sizeof(unsigned char) * BUFFSIZE)))
		return (-1);
	saddr_size = sizeof(saddr);
	data_size = recvfrom(sock_raw, buffer, BUFFSIZE, 0, &saddr, &saddr_size);
	if (data_size < 0)
	{
		syslog(LOG_NOTICE, "-> recvfrom error, failed to get packet.");
		if (c_unit->root != NULL)
			clean_tree(c_unit->root);
		free(buffer);
		return -1;
	}
	add_recv(buffer, c_unit->cur_device, c_unit);
	free(buffer);
	return (EXIT_SUCCESS);
}

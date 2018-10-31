#include "../includes/shared.h"

int isValidIpAddress(char *ipAddress)
{
	struct sockaddr_in sa;

	int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
	return result != 0;
}

uint32_t encode_ip(char *ip_str)
{
	char	*ptr;
	uint8_t	encoded[IP_BLOCKS];
	int		i;

	if (ip_str == NULL)
		return 0;
	i = 0;
	ptr = ip_str;
	encoded[i++] = (uint8_t )strtoul(ptr, &ptr, 10);
	while (i < IP_BLOCKS)
		encoded[i++] = (uint8_t)strtoul(ptr + 1, &ptr, 10);
	return *(uint32_t *)&encoded;
}

char*	decode_ip(uint32_t ip)
{
	uint8_t decode[IP_BLOCKS];
	char	*ip_str;
	int		size;

	size = 0;
	for (int i = 0; i < IP_BLOCKS; ++i)
	{
		decode[i] = (uint8_t)(ip >> (i * 8));
		if (decode[i] < 10)
			size++;
		else if (decode[i] < 100)
			size += 2;
		else
			size += 3;
	}
	if (!(ip_str = (char *)malloc(sizeof(char) * (size + 3 + 1))))
		return NULL;
	sprintf(ip_str, "%d.%d.%d.%d", decode[0], decode[1], decode[2], decode[3]);
	return ip_str;

}
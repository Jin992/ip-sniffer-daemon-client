//
// Created by jin on 31.10.18.
//

#ifndef SHARED_H
#define SHARED_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/msg.h>
#include <ifaddrs.h>

#define MSG_SIZE  24
#define IP_BLOCKS 4
#define FTOK_PATH "/etc/host.conf"

typedef struct			msgbuf_s
{
	long mtype;
	char data[MSG_SIZE];
}						msgbuf_t;

int 		pack_package(char* buf, uint32_t op, uint32_t ip, uint32_t qnt);
void 		unpack_status_package(char *buf, uint32_t *status, char *device);
int 		pack_status_package(char *buf, uint32_t status, char *device);
void 		unpack_package(const char *buf, uint32_t *op, uint32_t *ip, uint32_t *qnt);
void		*ft_free_str_arr(char **tab);
char		**ft_strsplit(char const *s, char c);
int 		send_msg(int msqid, uint32_t  op, uint32_t ip, uint32_t qnt, int out);
char*		decode_ip(uint32_t ip);
char		*ft_strnew(size_t size);
int			isValidIpAddress(char *ipAddress);
size_t		two_d_arr_len(char **arr);
uint32_t	encode_ip(char *ip_str);
int			check_device(char * dev_name);


#endif
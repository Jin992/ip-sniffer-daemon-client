//
// Created by jin on 29.10.18.
//
#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <ctype.h>
#include "shared.h"


#define MSG_IN		3
#define MSG_OUT     2
#define U_INPUT_SIZE 64

#define START 1
#define STOP  2
#define SHOW_IP_QNT 3
#define SELECT_IFACE 4
#define STAT_IFACE 5
#define DAEMON 6

int 	recv_msg(int msqid);
int		recv_daemon_status(int msqid);

#endif //
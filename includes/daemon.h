//
// Created by jin on 26.10.18.
//
#ifndef SNIFFER_H
#define SNIFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "shared.h"

#define SESSION_FILE "/tmp/session"
#define MSG_IN		2
#define MSG_OUT     3
/*OP codes*/
#define OP_START 1
#define OP_STOP  2
#define OP_SHOW_IP_QNT 3
#define OP_SELECT_IFACE 4
#define OP_STAT_IFACE 5
#define OP_DAEMON 6
#define OP_SHUTDOWN 7
#define BUFFSIZE 65536
#define IFACE_SIZE 16
#define INIT_QNT 1

typedef struct     iface_s
{
	char			name[IFACE_SIZE];
	uint32_t		qnt;
	struct iface_s	*next;
}					iface_t;

typedef struct		node_s
{
	struct node_s	*left;
	struct node_s	*right;
	iface_t			*iface;
	uint32_t		ip;
	uint32_t		qnt;
	uint32_t		height;
}					node_t;

typedef struct ctrl_s
{
	uint32_t 	start;
	uint32_t 	stop;
	char 		cur_device[20];
	node_t 		*root;
}				ctrl_t;

/*IP-tree(storage) avl_tree.c*/
node_t		*insert(node_t *node, uint32_t ip, uint32_t qnt, char *iface_name);
void		print_tree(node_t *root);
node_t		*avl_search(node_t *root,  uint32_t ip);
void		clean_tree(node_t *node);

/*Iface_list funcs iface_list.c*/
int			new_iface( iface_t **list,  uint32_t  qnt, char *iface_name);
iface_t		*search_iface(iface_t *list, char *iface_name);
void		clean_list(iface_t **list);

/*Session session.c*/
int			save_cur_session(node_t *storage);
void		load_session(ctrl_t *c_unit);

/*Msg queue/msg funcs msq_funcs.c*/
int			client_handler(int msqid, ctrl_t *c_unit, int sock_raw);
int			init_msq(key_t *key);

/*Client-operatins operations.c*/
int			op_start(ctrl_t *c_unit);
int			op_stop(ctrl_t *c_unit);
int			op_show(int msqid, ctrl_t *c_unit, uint32_t ip);
int			op_select(int sock_raw, ctrl_t *c_unit, msgbuf_t *buf);
int			op_stat(int msqid, ctrl_t *c_unit, msgbuf_t *buf);
int			op_daemon_stat(int msqid, ctrl_t *c_unit);

/*Sniffer recv sock_recv_hndl.c*/
int			rqvst_handler(int sock_raw, ctrl_t *c_unit);

int start(int argc, char **argv, ctrl_t *c_unit);




#endif

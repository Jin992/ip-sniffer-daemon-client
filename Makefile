DAEMON_NAME = sniffer_daemon
CLIENT_NAME = sniffer_client
CC = @gcc
CFLAGS = -Wall -Wextra -Werror


SHARED = ./shared/check_iface.c ./shared/ip_funcs.c ./shared/package.c\
		 ./shared/send_msg.c ./shared/str_split.c

CLIENT = ./client/main.c ./client/msg_funcs.c

DAEMON = ./daemon/main.c ./daemon/cli_operations.c ./daemon/iface_list.c\
		 ./daemon/msg_funcs.c ./daemon/session.c ./daemon/ip_storage.c\
		 ./daemon/socket_recv_handler.c ./daemon/start.c

SHARED_OBJ = $(SHARED:.c=.o)
CLIENT_OBJ  = $(CLIENT:.c=.o)
DAEMON_OBJ  = $(DAEMON:.c=.o)


all: client daemon

client: $(CLIENT_OBJ) $(SHARED_OBJ)
	$(CC) -Iincludes/ $(CLIENT_OBJ) $(SHARED_OBJ) -o $(CLIENT_NAME)

daemon: $(DAEMON_OBJ) $(SHARED_OBJ)
	$(CC) -Iincludes/ $(DAEMON_OBJ) $(SHARED_OBJ) -o $(DAEMON_NAME)

clean:
	@/bin/rm -f $(CLIENT_OBJ) $(SHARED_OBJ) $(DAEMON_OBJ)

fclean: clean
	@/bin/rm -f $(CLIENT_OBJ) $(SHARED_OBJ) $(DAEMON_OBJ)
	@/bin/rm -f $(CLIENT_NAME) $(DAEMON_NAME)

re: fclean all
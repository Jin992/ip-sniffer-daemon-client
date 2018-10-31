
#include "../includes/client.h"

static int select_recv(int msqid, char **split_input)
{
    msgbuf_t buf;

    if (send_msg(msqid, STOP, 0, 0, MSG_OUT) == -1)
        return  -1;
    pack_status_package(buf.data, SELECT_IFACE, split_input[2]);
    buf.mtype = MSG_OUT;
    if (msgsnd(msqid, &buf, MSG_SIZE, 0) == -1)
        return  -1;
    printf("->> changing iface to %s\n", split_input[2]);
    return 0;
}

static int stat_recv(int msqid, char *iface)
{
    msgbuf_t buf;
    int revc_n;

    pack_status_package(buf.data, STAT_IFACE, iface);
    buf.mtype = MSG_OUT;
    if (msgsnd(msqid, &buf, MSG_SIZE, 0) == -1)
        return  -1;
    while ((revc_n = recv_msg(msqid)) == 0);
    if (revc_n == -1)
        return  -1;
    return 0;
}

static void print_help()
{
    printf("->> start                   - run daemon\n");
    printf("->> stop                    - stop daemon\n");
    printf("->> show [IP] count         - ​print number of packets received from ip address\n");
    printf("->> select iface [Iface]    - ​(select interface for sniffing\n");
    printf("->> stat optional [Iface]   - show all collected statistics for particular interface\n");
    printf("->>                           if ifaceomitted - for all interfaces.\n");
    printf("->> daemon                  - show daemon current status\n");
    printf("->> quit                    - exit from clietn\n");
    printf("->> shutdown                - turn off daemon\n");
    printf("->> --help                  - ​show usage information\n");
}

int main()
{
    key_t       key;
    int         msqid;
    char        user_input[U_INPUT_SIZE];
    char        **split_input;

    if((key = ftok("/etc/host.conf", 'b')) == -1)
    {
        perror("->> ftok :");
        exit(EXIT_FAILURE);
    }
    if ((msqid = msgget(key, 0666)) == -1)
    {
        perror("->> msget :");
        exit(EXIT_FAILURE);
    }
    if (recv_daemon_status(msqid) == -1)
        return EXIT_FAILURE;
    while (1)
    {
        memset(user_input, 0, 64);

        printf("sniffer-client:> ");
        fgets(user_input, 64, stdin);
        strtok(user_input, "\n");
        if ((split_input = ft_strsplit(user_input, ' ')) == NULL)
            exit(EXIT_FAILURE);
        if (split_input[0] == '\0')
        {
            ft_free_str_arr(split_input);
            continue;
        }
        if (strcmp(split_input[0], "start") == 0 )
        {
            if (two_d_arr_len(split_input) == 1)
            {
                if (send_msg(msqid, START, 0, 0, MSG_OUT) == -1)
                    return EXIT_FAILURE;
                if (recv_daemon_status(msqid) == -1)
                    return EXIT_FAILURE;
            }
            else
                printf("->> usage : start\n");
        }
        else if (strcmp(split_input[0], "stop") == 0)
        {
            if (two_d_arr_len(split_input) == 1)
            {
                if (send_msg(msqid, STOP, 0, 0, MSG_OUT) == -1)
                    return EXIT_FAILURE;
                if (recv_daemon_status(msqid) == -1)
                    return EXIT_FAILURE;
            }
            else
                printf("->> usage : stop\n");
        }
        else if (strcmp(split_input[0], "show") == 0)
        {
            if (two_d_arr_len(split_input) == 3 && strcmp(split_input[2], "count") == 0)
            {
                if (isValidIpAddress(split_input[1]))
                {
                    if (send_msg(msqid, SHOW_IP_QNT, encode_ip(split_input[1]), 0, MSG_OUT) == -1)
                        exit(EXIT_FAILURE);
                    if (recv_msg(msqid) == -1)
                        exit(EXIT_FAILURE);
                }
                else
                    printf("->> wrong ip address\n");
            }
            else
                printf("->usage : show [IP] count\n");
        }
        else if (strcmp(split_input[0], "select") == 0)
        {
            if (two_d_arr_len(split_input) == 3 && strcmp(split_input[1], "iface") == 0)
            {
                if (check_device(split_input[2]) == 0)
                {
                    if (select_recv(msqid, split_input) == -1)
                        exit(EXIT_FAILURE);
                }
                else
                    printf("->> no such iface\n");
            }
            else
                printf("->> usage : select iface [iface]\n");
        }
        else if (strcmp(split_input[0], "stat") == 0)
        {
            if (two_d_arr_len(split_input) == 2)
            {
                if (stat_recv(msqid, split_input[1]) == -1)
                    exit(EXIT_FAILURE);
            }
            else if (two_d_arr_len(split_input) == 1)
            {
                if (stat_recv(msqid, "all") == -1)
                    exit(EXIT_FAILURE);
            }
            else
                printf("->> usage : stat optional[Iface]\n");
        }
        else if (strcmp(split_input[0], "daemon") == 0)
        {
            if (two_d_arr_len(split_input) == 1)
            {
                if (recv_daemon_status(msqid) == -1)
                    exit(EXIT_FAILURE);
            }
            else
                printf("->> usage : daemon\n");
        }
        else if (strcmp(split_input[0], "--help") == 0)
        {
            if (two_d_arr_len(split_input) == 1)
                print_help();
            else
                printf("->> usage : --help\n");
        }
        else if (strcmp(split_input[0], "quit") == 0)
        {
            if (two_d_arr_len(split_input) == 1)
                break;
            else
                printf("->> usage : quit\n");
        }
        else if ((strcmp(split_input[0], "shutdown") == 0))
        {
            if (two_d_arr_len(split_input) == 1)
            {
                if (send_msg(msqid, 7, 0, 0, MSG_OUT) == -1)
                    exit(EXIT_FAILURE);
                ft_free_str_arr(split_input);
                break;
            }
        }
        else
        {
            printf("->> command not found\n");
        }
        ft_free_str_arr(split_input);
    }

    return EXIT_SUCCESS;
}
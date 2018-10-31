#include "../includes/daemon.h"

static void	write_to_file(node_t *root, FILE *fp)
{
	char *tmp;
	iface_t *ptr;
	char res[128];

	if (root != NULL)
	{
		ptr = root->iface;
		memset(res, 0, 128);

		write_to_file(root->left, fp);
		tmp = decode_ip(root->ip);
		sprintf(res, "%s:%d", tmp, root->qnt);
		fwrite(res, 1, strlen(res), fp);
		while (ptr != NULL)
		{
			memset(res, 0, 128);
			sprintf(res, ":%s:%d", ptr->name, ptr->qnt);
			fwrite(res, 1, strlen(res), fp);
			ptr = ptr->next;
		}
		fwrite("\n", 1, 1, fp);
		free(tmp);
		write_to_file(root->right, fp);
	}
}

static node_t *load_prev_session(FILE **fp)
{
	node_t	*node;
	char	*line;
	size_t	size;
	char	*end;
	char	**split_str;
	size_t	pairs;

	node = NULL;
	line = NULL;
	size = 0;
	while ( getline(&line, &size, *fp) > 0)
	{
		if ((split_str = ft_strsplit(line, ':')) == NULL)
			exit(EXIT_FAILURE);
		if ((pairs = two_d_arr_len(split_str)) % 2 != 0)
		{
			ft_free_str_arr(split_str);
			return NULL;
		}
		if ((node = insert(node, encode_ip(split_str[0]),
						   (uint32_t)strtoul(split_str[1], &end, 10), "s_qnt")) == NULL)
			exit(EXIT_FAILURE);
		for(int i = 2; i < pairs; i+=2)
		{
			if ((node = insert(node, encode_ip(split_str[0]),
							   (uint32_t) strtoul(split_str[i + 1], &end, 10), split_str[i])) == NULL)
				exit(EXIT_FAILURE);
		}
		free(line);
		ft_free_str_arr(split_str);
		line = NULL;
		size = 0;
	}
	if (line != NULL)
		free(line);
	return node;
}

void load_session(ctrl_t *c_unit)
{
	FILE			*fp;

	fp = fopen(SESSION_FILE, "r");
	if (fp == NULL)
		syslog(LOG_NOTICE, "-> unable to load session : session file doesn't exist.");
	else
	{
		syslog(LOG_NOTICE, "-> loading previous session...");
		if ((c_unit->root = load_prev_session(&fp)) == NULL)
			syslog(LOG_NOTICE, "-> error: uabnle to load previous session.\n-> starting new session.");
		else
			syslog(LOG_NOTICE, "-> session loaded.");
		fclose(fp);
	}
}

int		save_cur_session(node_t *storage)
{
	FILE *session;

	session = fopen(SESSION_FILE, "w");
	if (session == NULL)
		return EXIT_FAILURE;
	write_to_file(storage, session);
	fclose(session);
	return EXIT_SUCCESS;

}

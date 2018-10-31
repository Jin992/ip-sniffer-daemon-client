#include "../includes/daemon.h"

int new_iface( iface_t **list,  uint32_t  qnt, char *iface_name)
{
	iface_t *iface;

	if (!(iface = (iface_t *)malloc(sizeof(iface_t) * 1)))
		return EXIT_FAILURE;
	iface->qnt = qnt;
	memcpy(iface->name, iface_name, strlen(iface_name));
	iface->name[strlen(iface_name)] = 0;
	iface->next = (*list);
	(*list) = iface;
	return EXIT_SUCCESS;
}

iface_t *search_iface(iface_t *list, char *iface_name)
{
	iface_t *tmp = list;

	while (tmp != NULL)
	{
		if (strcmp(tmp->name, iface_name) == 0)
		{
			tmp->qnt++;
			return (tmp);
		}
		tmp = tmp->next;
	}
	return (NULL);
}

void clean_list(iface_t **list)
{
	iface_t *cur = *list;
	iface_t *next;

	while (cur != NULL)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}
	*list = NULL;
}
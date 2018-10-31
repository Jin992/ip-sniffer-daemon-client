#include "../includes/daemon.h"

static uint32_t	height(node_t *node)
{
	if (node == NULL)
		return 0;
	return node->height;
}

static uint32_t		max(const uint32_t a, const uint32_t b)
{
	return (a > b) ? a : b;
}

static int		get_balance(node_t *n)
{
	if (n == NULL)
		return 0;
	return height(n->left) - height(n->right);
}

static node_t *new_node(uint32_t ip, uint32_t qnt, char *iface_name)
{
	node_t *node;

	if (!(node = (node_t *)malloc(sizeof(node_t) * 1)))
		return NULL;
	node->iface = NULL;
	if ((new_iface(&node->iface, qnt, iface_name)))
	{
		free(node);
		return NULL;
	}
	node->ip = ip;
	node->qnt = qnt;
	node->height = 1;
	node->left = NULL;
	node->right = NULL;
	return node;
}

static node_t	*right_rotate(node_t *y)
{
	node_t *x;
	node_t *t2;

	x = y->left;
	t2 = x->right;
	x->right =  y;
	y->left = t2;
	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;
	return x;
}

static node_t	*left_rotate(node_t *x)
{
	node_t *y;
	node_t *t2;

	y = x->right;
	t2 = y->left;
	y->left = x;
	x->right = t2;
	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;
	return y;
}

node_t	*insert(node_t *node, uint32_t ip, uint32_t qnt, char *iface_name)
{
	int balance;

	if (node == NULL)
		return (new_node(ip, qnt, iface_name));
	if (ip < node->ip)
		node->left = insert(node->left, ip, qnt, iface_name);
	else if (ip > node->ip)
		node->right = insert(node->right, ip, qnt, iface_name);
	else
	{
		if (!(search_iface(node->iface, iface_name)))
		{
			if (new_iface(&node->iface, qnt,iface_name))
				exit(EXIT_FAILURE);
			else
				return node;
		}
		return node;
	}
	node->height = 1 + max(height(node->left), height(node->right));
	balance = get_balance(node);
	if (balance > 1 && ip < node->left->ip)
		return right_rotate(node);
	if (balance < -1 && ip > node->right->ip)
		return left_rotate(node);
	if (balance > 1 && ip > node->left->ip)
	{
		node->left = left_rotate(node->left);
		return right_rotate(node);
	}
	if (balance < -1 && ip < node->right->ip)
	{
		node->right = right_rotate(node->right);
		return left_rotate(node);
	}
	return node;
}

void	print_tree(node_t *root)
{
	char *tmp;

	if (root != NULL)
	{
		print_tree(root->left);
		tmp = decode_ip(root->ip);
		syslog(LOG_NOTICE,"ip %s qnt %d iface %s iface_qnt %d.", tmp, root->qnt, root->iface->name, root->iface->qnt);
		free(tmp);
		print_tree(root->right);
	}
}

node_t	*avl_search(node_t *root,  uint32_t ip)
{
	if (root == NULL || root->ip == ip)
		return root;
	if (root->ip < ip)
		return avl_search(root->right, ip);
	return avl_search(root->left, ip);
}

void clean_tree(node_t *node)
{
	if (!node)
		return;
	clean_tree(node->left);
	clean_tree(node->right);
	clean_list(&node->iface);
	free(node);
	node = NULL;
}
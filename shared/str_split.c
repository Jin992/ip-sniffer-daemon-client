#include "../includes/shared.h"

size_t two_d_arr_len(char **arr)
{
	size_t len;

	len = 0;
	if (arr == NULL)
		return 0;
	while (arr[len] != NULL)
		len++;
	return len;
}

char	*ft_strnew(size_t size)
{
	char *res;

	res = (char *)malloc(sizeof(char) * (size + 1));
	if (res == NULL)
		return (NULL);
	bzero(res, size + 1);
	return (res);
}

static char	*ft_strsub(char const *s, unsigned int start, size_t len)
{
	char	*sub_str;
	size_t	i;

	i = 0;
	if (s == NULL || len == 0)
		return (ft_strnew(0));
	sub_str = ft_strnew(len);
	if (sub_str == NULL)
		return (NULL);
	while (i < len)
		sub_str[i++] = s[start++];
	return (sub_str);
}

static char	**ft_new_str_arr(char **str_arr, size_t size)
{
	str_arr = (char **)malloc(sizeof(char *) * (size + 1));
	if (str_arr == NULL)
		return (NULL);
	str_arr[size] = 0;
	return (str_arr);
}

void	*ft_free_str_arr(char **tab)
{
	size_t i;

	i = 0;
	if (tab == 0)
		return (tab);
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
	tab = 0;
	return (0);
}


static size_t		ft_get_words(char const *s, char c)
{
	size_t words;

	words = 0;
	if (s == NULL)
		return (0);
	while (*s != '\0')
	{
		if (*s != c && (*(s + 1) == c || *(s + 1) == '\0'))
			words++;
		s++;
	}
	return (words);
}

static char		**ft_array_filler(char **res, char const *s, char c)
{
	char		**res_start;
	char const	*s_tmp;

	res_start = res;
	while (*s != '\0')
	{
		s_tmp = s;
		while (*s != c && *s != '\0')
			s++;
		if (s - s_tmp != 0)
		{
			*res = ft_strsub(s_tmp, 0, s - s_tmp);
			if (*res == NULL)
				return ((char **)ft_free_str_arr(res_start));
			res++;
		}
		if (*s == '\0')
			break ;
		s = s_tmp + (s - s_tmp) + 1;
	}
	return (res_start);
}

char			**ft_strsplit(char const *s, char c)
{
	char **res;

	res = NULL;
	if (s == NULL)
		return (NULL);
	if (ft_get_words(s, c) == 0)
		return (ft_new_str_arr(res, 0));
	res = ft_new_str_arr(res, ft_get_words(s, c));
	if (res == NULL)
		return (NULL);
	return (ft_array_filler(res, s, c));
}
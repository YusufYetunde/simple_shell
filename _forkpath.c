#include "main.h"

/**
 * arrdup - duplicate a (NULL-terminated) array
 * @arr: the array to duplicate
 *
 * Return: a duplicate of arr
 */
char **arrdup(char **arr)
{
	char **dup = NULL;
	size_t len = 0;

	if (!arr)
		return (NULL);
	while (arr[len++])
		;
	dup = malloc(sizeof(*dup) * len);
	if (!dup)
		return (NULL);
	while (len--)
		dup[len] = _strdup(arr[len]);
	return (dup);
}
/**
 * arrjoin - join two arrays
 * @arr1: the first array
 * @arr2: the second array
 *
 * Return: a dynamically-allocated array of the elements from arr1 and arr2
 */
char **arrjoin(char **arr1, char **arr2)
{
	char **new;
	size_t arr1_len = 0, arr2_len = 0, new_len;

	if (arr1)
	{
		while (arr1[arr1_len])
			++arr1_len;
	}
	if (arr2)
	{
		while (arr2[arr2_len])
			++arr2_len;
	}
	new = malloc(sizeof(char *) * (arr1_len + arr2_len + 1));
	if (!new)
		return (NULL);

	new_len = 0;
	if (arr1)
	{
		while (*arr1)
			new[new_len++] = _strdup(*arr1++);
	}
	if (arr2)
	{
		while (*arr2)
			new[new_len++] = _strdup(*arr2++);
	}
	new[new_len] = NULL;
	return (new);
}
/**
  * search_path - searches for the directory with the executable program
  * @info: argument passed
  * @path: argument passed
  * Return: pointer to directory string
  */
char *search_path(info_t *info, list_t *path)
{
	char *pathname, *command = *info->tokens;
	struct stat sb;

	while (path)
	{
		if (*path->str == '\0')
			pathname = strjoin(NULL, "/", info->cwd, command);
		else
			pathname = strjoin(NULL, "/", path->str, command);
		if (stat(pathname, &sb) == 0)
		{
			if ((sb.st_mode & S_IFMT) != S_IFDIR)
				return (pathname);
		}
		free(pathname);
		path = path->next;
	}
	return (NULL);
}

#include "main.h"

/**
 * _realloc - reallocate a buffer
 * @old: pointer to the buffer
 * @old_size: current size of the buffer
 * @new_size: desired size of the buffer
 * Return: If memory allocation fails, return NULL.
 * Otherwise, return a pointer to the new buffer.
 */
static void *_realloc(void *old, size_t old_size, size_t new_size)
{
	void *new = NULL;

	if (old)
	{
		if (new_size)
		{
			new = malloc(new_size);
			if (new)
			{
				_memcpy(new, old, old_size < new_size ? old_size : new_size);
				free(old);
			}
		}
		else
		{
			free(old);
		}
	}
	return (new);
}
/**
 * _getline_next - read a line of input
 * @buf: pointer to the static buffer
 * @line: address of a pointer to the line
 * @size: address of a pointer to the line size
 * @n: number of characters to copy from the buffer
 * Return: If memory allocation fails, return NULL.
 * Otherwise, return a pointer to the line of input.
 */
 static char *_getline_next(buf_t *buf, char **line, size_t *size, size_t n)
{
	char *temp = NULL;

	if (*line)
		temp = _realloc(*line, *size, *size + n);
	else
		temp = malloc(n + 1);
	if (temp)
	{
		*line = temp;
		if (*size)
			*size -= 1;
		_memcpy(*line + *size, buf->next, n);
		*size += n;
		(*line)[*size] = '\0';
		*size += 1;
	}
	else
	{
		free(*line);
		*line = NULL;
		*size = 0;
	}
	return (*line);
}

/**
 * _getline_buf - create, get, and delete buffers
 * @table: buffers indexed by file descriptor
 * @fd: file descriptor
 * Return: NULL or a pointer to the buffer associated with fd
 */
static buf_t *_getline_buf(buf_table_t *table, const int fd)
{
	buf_table_node_t *item = NULL;
	size_t index = fd % GETLINE_TABLE_SIZE;
	if (table)
	{
		if (fd < 0)
		{
			for (index = 0; index < GETLINE_TABLE_SIZE; index += 1)
			{
				while ((item = (*table)[index]))
				{
					(*table)[index] = item->next;
					free(item);
				}
			}
		}
		else
		{
			item = (*table)[index];
			while (item && item->fd != fd)
				item = item->next;
			if (item == NULL)
			{
				item = malloc(sizeof(*item));
				if (item)
				{
					item->fd = fd;
					item->buf.next = NULL;
					item->buf.remaining = 0;
					item->next = (*table)[index];
					(*table)[index] = item;
				}
			}
		}
	}
	return (item ? &item->buf : NULL);
}

/**
 * _getline - read a line of input
 * @fd: file descriptor from which to read
 * Return: If an error occurs or there are no more lines, return NULL.
 * Otherwise, return the next line of input.
 */
char *_getline(const int fd)
{
	static buf_table_t table;
	buf_t *buf = _getline_buf(&table, fd);
	char *line = NULL;
	size_t size = 0;
	ssize_t eol = 0, n_read = 0;
	if (buf)
	{
		do {
			if (buf->remaining == 0)
				buf->next = buf->buffer;
			if (n_read)
				buf->remaining = n_read;
			if (buf->remaining)
			{
				eol = _memchr(buf->next, '\n', buf->remaining);
				if (eol == -1)
				{
					if (_getline_next(buf, &line, &size, buf->remaining))
						buf->next += buf->remaining, buf->remaining = 0;
					else
						break;
				}
				else
				{
					if (_getline_next(buf, &line, &size, eol + 1))
						buf->next += eol + 1, buf->remaining -= eol + 1;
					break;
				}
			}
		} while ((n_read = read(fd, buf->buffer, GETLINE_BUFFER_SIZE)) > 0);
		if (n_read == -1)
		{
			free(line);
			line = NULL;
			size = 0;
		}
	}
	return (line);
}
/**
 * init_info - initialize shell info
 * @argc: the arg count
 * @argv: the arg values
 * Return: pointer to the info
 */
info_t *init_info(int argc, char **argv)
{
	static info_t info;
	char *error = NULL;

	info.argc = argc;
	info.argv = argv;
	info.fileno = STDIN_FILENO;
	if (argc > 1)
	{
		info.file = argv[1];
		info.fileno = open(info.file, O_RDONLY);
		if (info.fileno == -1)
		{
			error = strjoin(NULL, " ", "Can't open", info.file);
			perrorl_default(*argv, info.lineno, error, NULL);
			free(error);
			info.status = 127;
			exit(free_info(&info));
		}
	}
	info.interactive = isatty(info.fileno);
	info.pid = getpid();
	info.cwd = getcwd(NULL, 0);
	info.env = env_to_dict(environ);
	return (&info);
}
/**
 * free_info - free and nullify dynamically allocated info
 * @info: pointer to the info
 * Return: current exit status
 */
int free_info(info_t *info)
{
	free(info->line);
	info->line = _getline(-1);
	free_tokens(&info->tokens);
	free(info->cwd);
	info->cwd = NULL;
	free(info->exe);
	info->exe = NULL;
	free_dict(&info->env);
	free_list(&info->path);
	free_dict(&info->aliases);
	free_cmdlist(&info->commands);
	return (info->status);
}

#include "main.h"

/**
 * read_input - get input
 * @info: shell information
 *
 * Return: line size
 */
bool read_input(info_t *info)
{
	char *line = NULL, *temp = NULL;

	if (info->interactive)
		write(STDERR_FILENO, "$ ", 2);

	info->lineno += 1;
	while (_read_input(&info->line, info->fileno) &
		(QUOTE_DOUBLE | QUOTE_SINGLE | QUOTE_ESCAPE))
	{
		temp = line;
		line = strjoin(NULL, "", temp, info->line);
		free(temp);
		free(info->line);
		if (info->interactive)
			write(STDERR_FILENO, "> ", 2);
		info->lineno += 1;
	}
	if (line)
	{
		temp = info->line;
		info->line = strjoin(NULL, "", line, temp);
		free(temp);
		free(line);
	}
	return (info->line);
}


/**
 * _read_input - read a single line
 * @lineptr: line buffer
 * @fd: file descriptor to read from
 *
 * Return: ending quote state
 */
quote_state_t _read_input(char **lineptr, int fd)
{
	char *line = *lineptr = _getline(fd);
	static quote_state_t state = QUOTE_NONE;
	size_t index = 0;

	if (line)
	{
		switch (state & (QUOTE_DOUBLE | QUOTE_SINGLE))
		{
		case QUOTE_DOUBLE:
		case QUOTE_SINGLE:
			do {
				index += quote_state_len(line + index, state);
				if (line[index] == '\0')
					continue;
				if (state & (QUOTE_DOUBLE | QUOTE_SINGLE))
					index += 1;
				/* fall through */
		case 0:
				state = quote_state(line[index]);
				if (state & (QUOTE_DOUBLE | QUOTE_SINGLE | QUOTE_ESCAPE))
					index += 1;
			} while (line[index]);
		}
	}
	return (state);
}
/**
 * remove_comments - remove comments (#) from a command
 * @cmd: pointer to the cmd to process
 */
void remove_comments(cmdlist_t *cmd)
{
	char **tokens, **new, *tmp;

	for (tokens = cmd->tokens; *tokens; ++tokens)
	{
		if (**tokens == '#')
		{
			tmp = *tokens;
			*tokens = NULL;
			new = arrdup(cmd->tokens);
			*tokens = tmp;
			free_tokens(&(cmd->tokens));
			cmd->tokens = new;
			free_cmdlist(&(cmd->next));
			return;
		}
	}
}
/**
 * parse - parse input
 * @info: shell information
 *
 * Description: This function expands aliases, variables, and word splitting
 *
 * Return: the final number of tokens
 */
int parse(info_t *info)
{
	char **tokens, *tok;
	size_t n = 0;
	cmdlist_t *cmd = info->commands = cmd_to_list(info->line);

	while (cmd)
	{
		remove_comments(cmd);
		if (!cmd->tokens)
		{
			cmd = cmd->next;
			del_cmd(&(info->commands), n);
			continue;
		}
		expand_aliases(info->aliases, &(cmd->tokens));
		if (!cmd->tokens)
		{
			cmd = cmd->next;
			del_cmd(&(info->commands), n);
			continue;
		}
		expand_vars(info, &(cmd->tokens));
		if (!cmd->tokens)
		{
			cmd = cmd->next;
			del_cmd(&(info->commands), n);
			continue;
		}
		tokens = cmd->tokens;
		for (tok = *tokens; tok; tok = *(++tokens))
		{
			*tokens = dequote(tok);
			free(tok);
		}
		cmd = cmd->next;
		++n;
	}
	return (n);
}

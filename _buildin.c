#include "main.h"

/**
 * get_builtins - get the builtins
 * Return: pointer to a NULL-terminated statically-allocated array of builtins
 */
const builtin_t *get_builtins(void)
{
	static builtin_t builtins[] = {
		{"alias", __alias, ALIAS_HELP, ALIAS_DESC},
		{"cd", __cd, CD_HELP, CD_DESC},
		{"env", __env, ENV_HELP, ENV_DESC},
		{"exec", __exec, EXEC_HELP, EXEC_DESC},
		{"exit", __exit, EXIT_HELP, EXIT_DESC},
		{"help", __help, HELP_HELP, HELP_DESC},
		{"setenv", __setenv, SETENV_HELP, SETENV_DESC},
		{"unsetenv", __unsetenv, UNSETENV_HELP, UNSETENV_DESC},
		{0}
	};
	return (builtins);
}
/**
 * get_builtin - get a builtin by name
 * @name: the name of the builtin to retrieve
 * Return: NULL if no match is found, otherwise a pointer to the builtin
 */
const builtin_t *get_builtin(const char *name)
{
	const builtin_t *builtin = NULL;

	for (builtin = get_builtins(); builtin->name; builtin += 1)
	{
		if (_strcmp(name, builtin->name) == 0)
			return (builtin);
	}
	return (NULL);
}
/**
 * __help - show help for builtins commands
 * @info: shell info
 * Return: status
 */
int __help(struct info *info)
{
	const builtin_t *bp = NULL;
	char * const *args = info->tokens + 1;
	const char *desc = NULL;
	size_t len = 0;

	if (*args)
	{
		info->status = EXIT_FAILURE;
		while (*args)
		{
			bp = get_builtin(*args);
		       	if (bp)
			{
				write(STDOUT_FILENO, bp->name, _strlen(bp->name));
				write(STDOUT_FILENO, ": ", 2);
				write(STDOUT_FILENO, bp->help, _strlen(bp->help));
				write(STDOUT_FILENO, "\n", 1);
				for (desc = bp->desc; (len = _strlen(desc)); desc += len + 1)
				{
					write(STDOUT_FILENO, "    ", 4);
					write(STDOUT_FILENO, desc, len);
					write(STDOUT_FILENO, "\n", 1);
				}
				info->status = EXIT_SUCCESS;
			}
			args += 1;
		}
		if (info->status == EXIT_FAILURE)
			perrorl_default(*info->argv, info->lineno, "No topics match",
					*info->tokens, *(args - 1), NULL);
		return (info->status);
	}
	info->status = EXIT_SUCCESS;
	for (bp = get_builtins(); bp->name; bp += 1)
	{
		write(STDOUT_FILENO, bp->help, _strlen(bp->help));
		write(STDOUT_FILENO, "\n", 1);
	}
	return (info->status);
}
/**
 * __exit - exits from shell
 * @info: arguments passed
 * Return: int
 */
int __exit(info_t *info)
{
	char **args = info->tokens + 1;

	if (*args)
	{
		if (_isnumber(*args) && atou(*args) <= INT_MAX)
		{
			info->status = atou(*args);
		}
		else
		{
			perrorl_default(*info->argv, info->lineno, *args,
					*info->tokens, "Illegal number", NULL);
			info->status = 2;
			return (info->status);
		}
	}
	if (info->file)
		close(info->fileno);
	exit(free_info(info));
}
/**
 * __env - displays environment
 * @info: arguments passed
 * Return: int
 */
int __env(info_t *info)
{
	env_t *var;

	info->status = EXIT_SUCCESS;

	for (var = info->env; var; var = var->next)
	{
		if (var->key)
			write(STDOUT_FILENO, var->key, _strlen(var->key));
		write(STDOUT_FILENO, "=", 1);
		if (var->val)
			write(STDOUT_FILENO, var->val, _strlen(var->val));
		write(STDOUT_FILENO, "\n", 1);
	}
	return (info->status);
}

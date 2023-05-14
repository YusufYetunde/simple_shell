#include "main.h"

/**
 * __alias_add - add an alias to a list of aliases
 * @aliases: a pointer to a list of aliases
 * @name: the name of the alias
 * @value: the value of the alias
 */
void __alias_add(alias_t **aliases, const char *name, const char *value)
{
	alias_t *alias = get_dict_node(aliases ? *aliases : NULL, name);
	if (alias)
	{
		free(alias->val);
		alias->val = _strdup(value);
	}
	else
	{
		add_dict_node_end(aliases, name, value);
	}
}
/**
 * __alias_print - print an alias
 * @alias: the alias to print
 */
void __alias_print(alias_t *alias)
{
	write(STDOUT_FILENO, alias->key, _strlen(alias->key));
	write(STDOUT_FILENO, "='", 2);
	write(STDOUT_FILENO, alias->val, _strlen(alias->val));
	write(STDOUT_FILENO, "'\n", 2);
}
/**
 * __alias - create and display aliases
 * @info: shell info struct
 * Return: status
 */
int __alias(info_t *info)
{
	alias_t *alias;
	char *name, **args = info->tokens + 1;
	ssize_t name_len;

	info->status = EXIT_SUCCESS;
	if (*args)
	{
		do {
			name_len = _strchr(*args, '=');
			if (name_len == -1)
			{
				alias = get_dict_node(info->aliases, *args);
				if (alias)
				{
					__alias_print(alias);
				}
				else
				{
					perrorl("not found", *info->tokens, *args, NULL);
					info->status = EXIT_FAILURE;
				}
			
			}
			else
			{
				name = _strndup(*args, name_len);
				__alias_add(&info->aliases, name, *args + name_len + 1);
				free(name);
			}
		} while (*++args);
	}
	else
	{
		for (alias = info->aliases; alias; alias = alias->next)
			__alias_print(alias);
	}
	return (info->status);
}
/**
 * __setenv - sets the environment variables
 * @info: arguments passed
 * Return: status
 */
int __setenv(info_t *info)
{
	env_t *var;
	char **args = info->tokens + 1, *val;

	if (args[0])
	{
		if (args[1])
		{
			if (args[2])
			{
				perrorl("Too many arguments.", *info->tokens, NULL);
				info->status = EXIT_FAILURE;
				return (info->status);
			}
			val = args[1];
		}
		else
		{
			val = "";
		}
		var = get_dict_node(info->env, args[0]);
		if (var)
		{
			free(var->val);
			var->val = _strdup(val);
		}
		else
		{
			add_dict_node_end(&info->env, args[0], val);
		}
		info->status = EXIT_SUCCESS;
	}
	else
	{
		__env(info);
	}
	return (info->status);
}
/**
 * __unsetenv - unsets the environment variable
 * @info: arguments passed
 * Return: status
 */
int __unsetenv(info_t *info)
{
	char **args = info->tokens + 1;

	if (*args)
	{
		while (*args)
			del_dict_node(&info->env, *args++);
		info->status = EXIT_SUCCESS;
	}
	else
	{
		perrorl("Too few arguments.", *info->tokens, NULL);
		info->status = EXIT_FAILURE;
	}
	return (info->status);
}

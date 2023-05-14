#include "main.h"

/**
 * __cd_error - print an error upon failure to change directory
 * @info: shell info
 * @dir: directory
 */
void __cd_error(info_t *info, char *dir)
{
	char *error = strjoin(NULL, " ", "can't cd to", dir);
	perrorl_default(*info->argv, info->lineno, error, *info->tokens, NULL);
	info->status = 2;
	free(error);
}
/**
 * __cd_success - update the environment upon success
 * @info: shell info
 */
void __cd_success(info_t *info)
{
	char **tokens = info->tokens;
	char *setenv_tokens[] = {"setenv", NULL, NULL, NULL};

	info->tokens = setenv_tokens;
	setenv_tokens[1] = "OLDPWD";
	setenv_tokens[2] = info->cwd;
	__setenv(info);
	free(info->cwd);
	info->cwd = getcwd(NULL, 0);
	setenv_tokens[1] = "PWD";
	setenv_tokens[2] = info->cwd;
	__setenv(info);
	info->tokens = tokens;
	info->status = EXIT_SUCCESS;
}
/**
 * __cd - changes the directory
 * @info: arguments passed
 * Return: int
 */
int __cd(info_t *info)
{
	char *dir = NULL, **args = info->tokens + 1;

	info->status = EXIT_SUCCESS;
	if (*args)
	{
		if (!_strcmp(*args, "-"))
		{
			dir = get_dict_val(info->env, "OLDPWD");
			if (!dir)
				dir = info->cwd;
			info->status = chdir(dir);
			if (!info->status)
			{
				write(STDOUT_FILENO, dir, _strlen(dir));
				write(STDOUT_FILENO, "\n", 1);
			}
		}
		else
		{
			dir = *args;
			info->status = chdir(dir);
		}
	}
	else
	{
		dir = get_dict_val(info->env, "HOME");
		if (dir)
			info->status = chdir(dir);
	}
	if (info->status != -1)
		__cd_success(info);
	else
		__cd_error(info, dir);
	return (info->status);
}
/**
 * __exec - replace the running shell with a new program
 * @info: arguments passed
 * Return: int
 */
int __exec(info_t *info)
{
	char *exe, **args = info->tokens + 1, **env = NULL;

	if (!*args)
		return ((info->status = EXIT_SUCCESS));
	info->tokens = args;
	args = arrdup(args);
	if (_strchr(*args, '/') == -1)
	{
		free_list(&info->path);
		info->path = str_to_list(get_dict_val(info->env, "PATH"), ':');
		exe = search_path(info, info->path);
	}
	else
	{
		exe = _strdup(*args);
	}
	info->tokens -= 1;
	if (access(exe, X_OK) == 0)
	{
		env = dict_to_env(info->env);
		free_info(info);
		execve(exe, args, env);
		perrorl_default(*info->argv, info->lineno, "Not found",
				*info->tokens, *args, NULL);
		free(exe);
		free_tokens(&args);
		free_tokens(&env);
		exit(127);
	}
	perrorl_default(*info->argv, info->lineno, "Permission denied",
			*info->tokens, *args, NULL);
	free(exe);
	free_tokens(&args);
	free_info(info);
	exit(126);
}

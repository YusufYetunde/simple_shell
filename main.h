#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

extern char **environ;

#define GETLINE_BUFFER_SIZE 4096
#define GETLINE_TABLE_SIZE 127

#define ALIAS_HELP "alias [KEY[=VALUE] ...]"
#define ALIAS_DESC								\
	"Define and display aliases.\n\0"					\
	"If given no arguments, existing alias definitions are displayed.\0"	\
	"Otherwise, an alias is defined for each KEY=VALUE pair provided.\0"	\
	"For each KEY with no VALUE the corresponding alias is displayed.\0"	\
	"If VALUE ends with a space, the following word will be expanded.\0"	\
	"\0"

#define CD_HELP "cd [DIR]"
#define CD_DESC									\
	"Change the current working directory to DIR.\n\0"			\
	"If DIR is omitted, it defaults to the value of the variable HOME.\0"	\
	"If DIR is -, the current directory reverts to its previous value.\0"	\
	"\0"

#define ENV_HELP "env"
#define ENV_DESC								\
	"Print the environment.\0"						\
	"\0"

#define EXEC_HELP "exec COMMAND [ARGS ...]"
#define EXEC_DESC								\
	"Replace the shell with the given command.\n\0"				\
	"COMMAND is executed, replacing the executing shell.\0"			\
	"ARGS are passed as positional arguments to COMMAND.\0"			\
	"If the command cannot be executed, the shell exits.\0"			\
	"\0"

#define EXIT_HELP "exit [STATUS]"
#define EXIT_DESC								\
	"Exit the shell with a status of STATUS.\n\0"				\
	"If STATUS is omitted, the exit status is that of the last command.\0"	\
	"\0"

#define HELP_HELP "help [BUILTIN]"
#define HELP_DESC								\
	"Display information about builtin commands.\n\0"			\
	"If BUILTIN is omitted, the available commands are displayed.\0"	\
	"\0"

#define SETENV_HELP "setenv [NAME [VALUE]]"
#define SETENV_DESC								\
	"Set the environment variable NAME to VALUE.\n\0"			\
	"If NAME is omitted, the shell execution environment is displayed.\0"	\
	"If VALUE is omitted, the value of NAME is set to an empty string.\0"	\
	"\0"

#define UNSETENV_HELP "unsetenv NAME"
#define UNSETENV_DESC								\
	"Remove the variable NAME from the environment.\0"			\
	"\0"

typedef enum quote_state
{
	QUOTE_NONE   = 0x0,
	QUOTE_WORD   = 0x1,
	QUOTE_DOUBLE = 0x2,
	QUOTE_SINGLE = 0x4,
	QUOTE_ESCAPE = 0x8
} quote_state_t;
typedef size_t (*quote_state_fp)(const char *, quote_state_t *);
/**
 * struct list - singly linked list
 * @str: dynamically-allocated string
 * @next: pointer to the next node
 */
typedef struct list
{
	char *str;
	struct list *next;
} list_t;
/**
 * struct dict - singly linked list of key-value pairs
 * @key: variable name
 * @val: value of variable
 * @next: pointer to the next node
 */
typedef struct dict
{
	char *key;
	char *val;
	struct dict *next;
} dict_t;
typedef dict_t env_t;
typedef dict_t alias_t;
/**
 * struct history - shell command history
 * @head: a pointer to the head of the history list
 * @filename: the name of the history to file
 * @n: the number of entries in the history list
 */
typedef struct history
{
	struct list *head;
	char *filename;
	size_t n;
} history_t;
/**
 * enum cmdlist_sep_n - numeric values for commmand list separators
 * @SEMICOLON: ;
 * @AMPERSAND: &
 * @AND: &&
 * @OR: ||
 */
typedef enum cmdlist_sep_n
{
	SEMICOLON = 1,
	AMPERSAND = 2,
	AND       = 4,
	OR        = 8
} cmdlist_sep_n_t;

/**
 * struct cmdlist_sep - command list separator structure
 * @sep: the command separator
 * @n: the corresponding numeric value
 */
typedef struct cmdlist_sep
{
	const char *sep;
	enum cmdlist_sep_n n;
} cmdlist_sep_t;

/**
 * struct cmdlist - a linked list of commands
 * @next: the next command
 * @tree: a binary tree of commands
 * @tokens: the tokens for each command in the tree
 */
typedef struct cmdlist
{
	struct cmdlist *next;
	struct cmdtree *tree;
	char **tokens;
} cmdlist_t;

/**
 * struct cmdtree - a binary tree of commands
 * @success: the command to execute upon failure
 * @failure: the command to execute upon success
 * @tokens: a simple command with no separators
 * @sep: the preceding list separator
 */
typedef struct cmdtree
{
	struct cmdtree *success;
	struct cmdtree *failure;
	const char * const *tokens;
	struct cmdlist_sep sep;
} cmdtree_t;
/**
 * struct info - shell state
 * @interactive: arguments passed
 * @argc: arguments passed
 * @argv: arguments passed
 * @file: arguments passed
 * @fileno: arguments passed
 * @status: arguments passed
 * @line: arguments passed
 * @lineno: arguments passed
 * @tokens: arguments passed
 * @pid: arguments passed
 * @cwd: arguments passed
 * @exe: arguments passed
 * @env: arguments passed
 * @path: arguments passed
 * @aliases: arguments passed
 * @history: arguments passed
 * @commands: arguments passed
 */
typedef struct info
{
	int interactive;
	int argc;
	char **argv;
	char *file;
	int fileno;
	int status;
	char *line;
	size_t lineno;
	char **tokens;
	pid_t pid;
	char *cwd;
	char *exe;
	env_t *env;
	list_t *path;
	alias_t *aliases;
	history_t *history;
	cmdlist_t *commands;
} info_t;
/**
 * struct buf_s - input buffer
 * @buffer: the buffer
 * @next: pointer to the next value
 * @remaining: number of values remaining
 */
typedef struct buf_s
{
	char buffer[GETLINE_BUFFER_SIZE];
	char *next;
	size_t remaining;
} buf_t;
/**
 * struct buf_table_node_s - input buffer hash table
 * @fd: file descriptor
 * @buf: associated buffer
 * @next: next buffer in chain
 */
typedef struct buf_table_node_s
{
	int fd;
	struct buf_s buf;
	struct buf_table_node_s *next;
} buf_table_node_t;
typedef buf_table_node_t *buf_table_t[GETLINE_TABLE_SIZE];
typedef int (*builtin_fp)(info_t *);
/**
 * struct builtin - builtin command
 * @name: command name
 * @f: function to call
 * @help: command usage
 * @desc: command description
 */
typedef struct builtin
{
	const char *name;
	builtin_fp f;
	const char *help;
	const char *desc;
} builtin_t;




bool _isalnum(int c);
bool _isalpha(int c);
bool _isdigit(int c);
bool _isident(int c);
bool _isspace(int c);
bool _isquote(int c);
bool _isnumber(const char *s);
int _isspecial_double(char c);

ssize_t _strchr(const char *str, char chr);
ssize_t _strnchr(const char *str, char chr, size_t n);

int _strcmp(const char *s1, const char *s2);
int _strncmp(const char *s1, const char *s2, size_t n);

char *_strcpy(char *dest, const char *src);
char *_strncpy(char *dest, const char *src, size_t n);

char *_strdup(const char *str);
char *_strndup(const char *str, size_t n);

ssize_t _strlen(const char *str);
ssize_t _strnlen(const char *str, size_t n);

char *strjoin(size_t *n, const char *sep, const char *pre, const char *suf);
char *strjoina(size_t *n, const char *sep, const char **array);
char *strjoinl(size_t *n, const char *sep, ...);
ssize_t _memchr(const void *src, unsigned char chr, size_t n);
void *_memcpy(void *dest, const void *src, size_t n);
void *_memdup(const void *src, size_t n);
void *_memset(void *dest, unsigned char chr, size_t n);
unsigned int atou(char *s);
char *num_to_str(size_t n);

quote_state_t quote_state(char c);
quote_state_fp quote_state_f(quote_state_t s);
size_t quote_state_len(const char *str, quote_state_t state);
size_t _quote_state_none(const char *str, quote_state_t *state);
size_t _quote_state_word(const char *str, quote_state_t *state);
size_t _quote_state_double(const char *str, quote_state_t *state);
size_t _quote_state_single(const char *str, quote_state_t *state);
size_t _quote_state_escape(const char *str, quote_state_t *state);
char *dequote(const char *str);
size_t dequote_len(const char *str);

char *get_dict_val(dict_t *head, const char *key);
dict_t *get_dict_node(dict_t *head, const char *key);
dict_t *add_dict_node_end(dict_t **headptr, const char *key, const char *val);
dict_t *del_dict_node(dict_t **headptr, const char *key);
void free_dict(dict_t **headptr);

env_t *env_to_dict(char **env);
env_t *_env_to_dict(env_t **tailptr, char **env);
char **dict_to_env(env_t *head);

list_t *str_to_list(const char *str, char delim);
list_t *_str_to_list(list_t **tailptr, const char *str, char delim);
list_t *add_node(list_t **headptr, const char *str);
list_t *add_node_end(list_t **headptr, const char *str);
void free_list(list_t **headptr);

void perrorl(const char *msg, ...);
void perrorl_default(const char *arg0, size_t lineno, const char *msg, ...);

cmdlist_t *cmd_to_list(const char *cmd);
cmdlist_t *_cmd_to_list(cmdlist_t **tailptr, char *split, size_t count);
size_t split_cmd(char *cmd);
cmdlist_t *add_cmd_end(cmdlist_t **headptr, const char *cmd);
cmdlist_t *del_cmd(cmdlist_t **headptr, size_t index);
char **pop_cmd(cmdlist_t **headptr);
void free_cmdlist(cmdlist_t **headptr);
cmdtree_t *cmd_to_tree(const char * const *tokens);
void free_cmdtree(cmdtree_t **rootptr);

info_t *init_info(int argc, char **argv);
int free_info(info_t *info);

char *_getline(const int fd);


char *search_path(info_t *info, list_t *path);

char **tokenize(const char *str);
char **tokenize_noquote(const char *str);
size_t count_tokens(const char *str);
size_t count_tokens_noquote(const char *str);
void free_tokens(char ***tokens);
char **arrdup(char **arr);
char **arrjoin(char **arr1, char **arr2);

const struct builtin *get_builtin(const char *name);
const struct builtin *get_builtins(void);

int __alias(info_t *info);
int __cd(info_t *info);
int __env(info_t *info);
int __exec(info_t *info);
int __exit(info_t *info);
int __help(info_t *info);
int __setenv(info_t *info);
int __unsetenv(info_t *info);

bool read_input(info_t *info);
quote_state_t _read_input(char **lineptr, int fd);

int parse(info_t *info);

int execute(info_t *info);
int _execute(info_t *info);

void expand_aliases(alias_t *aliases, char ***tokptr);
char *expand_alias(alias_t *aliases, char ***tokptr);

void expand_vars(info_t *info, char ***tokptr);
char **_expand_vars(info_t *info, char ***tokptr);

void remove_comments(cmdlist_t *cmd);

void _sigint(int signal);



#endif

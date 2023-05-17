#include "main.h"

/**
 * expand_aliases - perform recursive alias expansion on the current command
 * @aliases: alias list
 * @tokptr: pointer to the current tokens
 *
 * Return: If expansion succeeds, return a pointer t to the otherwise 0
 */
void expand_aliases(alias_t *aliases, char ***tokptr)
{
	char **new, **old, *name, *value, *temp;

	if (!*tokptr)
		return;
	do {
		name = expand_alias(aliases, tokptr);
		value = get_dict_val(aliases, name);
		if (value && *value && _isspace(value[_strlen(value) - 1]))
		{
			old = *tokptr;
			new = arrdup(old + 1);

			expand_aliases(aliases, &new);
			temp = *(old + 1);

			*(old + 1) = NULL;
			*tokptr = arrjoin(old, new);
			*(old + 1) = temp;

			free_tokens(&old);
			free_tokens(&new);
		}
	} while (name && **tokptr && _strcmp(name, **tokptr));
}


/**
 * expand_alias - perform a single alias expansion on the current command
 * @aliases: alias list
 * @tokptr: pointer to the current tokens
 *
 * Return: If expansion succeeds, return a pointer the alias name.
 * Otherwise, return NULL.
 */
char *expand_alias(alias_t *aliases, char ***tokptr)
{
	char **alias_tokens, **tokens = *tokptr;

	if (!*tokens)
		return (NULL);

	while (aliases)
	{
		if (!_strcmp(*tokens, aliases->key))
		{
			alias_tokens = tokenize(aliases->val);
			*tokptr = arrjoin(alias_tokens, tokens + 1);

			free_tokens(&tokens);
			free_tokens(&alias_tokens);

			return (aliases->key);
		}
		aliases = aliases->next;
	}
	return (NULL);
}
/**
 * expand_vars - perform variable expansion on the current set of tokens
 * @info: shell information
  * @tokptr: pointer to the current tokens
 */
void expand_vars(info_t *info, char ***tokptr)
{
	char **new = NULL, **old, **tmp, **tokens;

	for (tokens = *tokptr; **tokptr; ++(*tokptr))
	{
		old = new;
		tmp = _expand_vars(info, tokptr);
		new = arrjoin(old, tmp);
		free_tokens(&old);
		free_tokens(&tmp);
		free(**tokptr);
	}
	free(tokens);
	*tokptr = new;
}


/**
 * _expand_vars - perform variable expansion on a token
 * @info: shell information
  * @tokptr: pointer to the current tokens
 *
 * Return: the expanded token
 */
char **_expand_vars(info_t *info, char ***tokptr)
{
	char *var = NULL, *val = NULL, *tok = **tokptr;
	size_t pos = 0, var_len, val_len;
	quote_state_t state = QUOTE_NONE;

	while (var_len = val_len = 1, tok[pos])
	{
		if (quote_state_len(tok + pos, state) == 0)
		{
			if ((state & (QUOTE_DOUBLE | QUOTE_SINGLE)) && !tok[++pos])
				break;
			state = quote_state(tok[pos]);
			if (state & (QUOTE_DOUBLE | QUOTE_SINGLE | QUOTE_ESCAPE))
				++pos;
			continue;
		}
		if ((state & (QUOTE_DOUBLE)) && (quote_state(tok[pos]) & QUOTE_ESCAPE))
		{
			if (!tok[++pos] || !tok[++pos])
				break;
			state = quote_state(tok[pos]);
			if (state & (QUOTE_DOUBLE | QUOTE_SINGLE | QUOTE_ESCAPE))
				++pos;
			continue;
		}
		if (state & (QUOTE_SINGLE))
		{
			pos += quote_state_len(tok + pos, state);
			if (tok[pos])
				++pos;
			continue;
		}
		if (state & (QUOTE_ESCAPE))
		{
			if (!tok[++pos] || !tok[++pos])
				break;
			state = quote_state(tok[pos]);
			if (state & (QUOTE_DOUBLE | QUOTE_SINGLE | QUOTE_ESCAPE))
				++pos;
			continue;
		}
		if (tok[pos] != '$')
		{
			++pos;
			continue;
		}
		if (tok[pos + 1] == '$')
		{
			val = num_to_str(info->pid);
		}
		else if (tok[pos + 1] == '?')
		{
			val = num_to_str(info->status);
		}
		else if (_isident(tok[pos + 1]) && !_isdigit(tok[pos + 1]))
		{
			while (_isident(tok[pos + var_len + 1]))
				++var_len;

			var = _strndup(tok + pos + 1, var_len);
			val = get_dict_val(info->env, var);

			if (val)
				val = _strdup(val);
			else
				val = _strdup("");

			free(var);
			var = NULL;
		}
		if (val)
		{
			val_len = _strlen(val);
			**tokptr = malloc(sizeof(char) * (
					pos + val_len + _strlen(tok + pos + var_len) + 1
					));
			_memcpy(**tokptr, tok, pos);
			_memcpy(**tokptr + pos, val, val_len);
			_strcpy(**tokptr + pos + val_len, tok + pos + var_len + 1);

			free(tok);
			tok = **tokptr;

			free(val);
			val = NULL;
		}
		pos += val_len;
	}
	return (tokenize(**tokptr));
}

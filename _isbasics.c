#include "main.h"
/**
 * _isalpha - checks if the character is alphabetic
 * @c: character to check
 * Return: If c is alphabetic, return 1. Otherwise, return 0
 */
bool _isalpha(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}
/**
 * _isdigit - checks if the character is digit
 * @c: character to check
 * Return: int
 */
bool _isdigit(int c)
{
	return (c >= '0' && c <= '9');
}
/**
 * _isnumber - check if a string consists only of digits
 * @s: pointer to string
 * Return: true or false
 */
bool _isnumber(const char *s)
{
	if (s)
	{
		while (*s)
		{
			if (!_isdigit(*s++))
				return (false);
		}
		return (true);
	}
	return (false);
}
/**
 * _isalnum - checks if the character is alphanumeric
 * @c: character to check
 * Return: If c is alphanumeric, return 1. Otherwise, return 0.
 */
bool _isalnum(int c)
{
	return (_isalpha(c) || _isdigit(c));
}
/**
 * _isident - checks if the character is a valid identifier character
 * @c: character to check
 * Return: If c is an identifier character, return 1. Otherwise, return 0.
 */
bool _isident(int c)
{
	return (c == '_' || _isalnum(c));
}

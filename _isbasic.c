#include "main.h"
/**
 * _isquote - check if a character is a quote
 * @c: the character to check
 * Return: If c is a quote, return 1. Otherwise, return 0.
 */
bool _isquote(int c)
{
	return (c == '"' || c == '\'' || c == '\\');
}
/**
 * _isspace - check if a character is whitespace
 * @c: the character to check
 * Return: If c is whitespace, return 1. Otherwise, return 0.*/
bool _isspace(int c)
{
	return (c == ' ' || (c >= 0x09 && c <= 0x0d));
}
/**
 * _isspecial_double - check if a character is special inside double quotes
 * @c: the character to check
 * Return: If c is special, return 1. Otherwise, return 0.
 */
int _isspecial_double(char c)
{
	return (c == '"' || c == '$' || c == '\\');
}

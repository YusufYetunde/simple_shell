#include "main.h"
/**
 * atou - convert a string to an integer
 * @s: character to check
 * Return: int
 */
unsigned int atou(char *s)
{
	size_t i;
	unsigned int number = 0;
	unsigned int to_add;

	for (i = 0; s[i] != '\0'; i++)
	{
		if (UINT_MAX / 10 < number)
			return (UINT_MAX);
		number *= 10;
		to_add = s[i] - '0';
		if (UINT_MAX - to_add < number)
			return (UINT_MAX);
		number += to_add;
	}
	return (number);
}
/**
 * _num_to_str - converts number to string
 * @buf: string
 * @n: number passed
 */
static void _num_to_str(char **buf, size_t n)
{
	if (n > 9)
		_num_to_str(buf, n / 10);
	**buf = '0' + n % 10;
	*buf += 1;
	**buf = '\0';
}
/**
 * num_to_str - converts number to string
 * @n: number passed
 * Return: pointer to string
 */
char *num_to_str(size_t n)
{
	size_t tmp = n, len = 1;
	char *buf;

	while (tmp /= 10)
		len++;
	buf = malloc(len + 1);
	if (!buf)
		return (NULL);
	_num_to_str(&buf, n);
	return (buf - len);
}

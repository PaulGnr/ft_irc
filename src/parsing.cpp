#include "ft_irc.hpp"

int	parsing(char *buf)
{
	if (!strncmp(buf, "/die", strlen("/die")))
		return (0);
	return (1);
}

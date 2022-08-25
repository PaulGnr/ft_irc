#include "ft_irc.hpp"

int	sendall(int dest_fd, const char *buf, int *nbytes)
{
	int	total = 0;
	int	bytesleft = *nbytes;
	int	n;

	while (total < *nbytes)
	{
		n = send(dest_fd, buf + total, bytesleft, 0);
		if (n == -1) break;
		total += n;
		bytesleft -= n;
	}
	*nbytes = total;

	return (n == -1 ? -1 : 0);
}

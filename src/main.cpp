#include "ft_irc.hpp"

int	main(int argc, char **argv)
{
	int	sockfd;

	if (argc != 3)
	{
		std::cerr << "Error : Wrong number of arguments." << std::endl;
		return (1);
	}

	sockfd = get_listener_socket(argv);

	poll_handler(sockfd);

	return (0);
}

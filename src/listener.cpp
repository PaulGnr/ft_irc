#include "ft_irc.hpp"

int	get_listener_socket(char **argv)
{
	struct addrinfo	hints, *servinfo, *p;
	int				status, sockfd, yes = 1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
	{
		std::cerr << "Error : getaddrinfo : " << gai_strerror(status) << std::endl;
		exit(1);
	}
	debug("getaddrinfo OK");

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "Error : socket : " << std::strerror(errno) << std::endl;
			continue;
		}
		debug("socket OK");

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			std::cerr << "Error : setsockopt : " << std::strerror(errno) << std::endl;
			close(sockfd);
			freeaddrinfo(servinfo);
			exit(1);
		}
		debug("setsockopt OK");

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			std::cerr << "Error : bind : " << std::strerror(errno) << std::endl;
			close(sockfd);
			continue;
		}
		debug("bind OK");

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		std::cerr << "Error : server failed to bind" << std::endl;
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		std::cerr << "Error : listen : " << std::strerror(errno) << std::endl;
		exit(1);
	}
	debug("listen OK");

	return (sockfd);
}

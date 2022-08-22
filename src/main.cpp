#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 10
#define DEBUG 1

void	debug(std::string str)
{
	std::cout << str << std::endl;
}

void	add_to_pfds(struct pollfd **pfds, int new_fd, int *fd_count, int *fd_size)
{
	if (*fd_count == *fd_size)
	{
		*fd_size *= 2;
		// A continuer ici
	}
}

int	main(int argc, char **argv)
{
	struct addrinfo			hints, *servinfo, *p;
	int						status, sockfd, new_fd, yes = 1;
	struct sockaddr_storage	remoteaddr;
	socklen_t				addrlen;
	struct pollfd			*pfds;
	int						fd_count, fd_size = 5;

	if (argc != 3)
	{
		std::cerr << "Error : Wrong number of arguments." << std::endl;
		return (1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
	{
		std::cerr << "Error : getaddrinfo : " << gai_strerror(status) << std::endl;
		return (1);
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
		return (1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		std::cerr << "Error : listen : " << std::strerror(errno) << std::endl;
		return (1);
	}
	debug("listen OK");

	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;
	fd_count = 1;

	std::cout << "Waiting for connections..." << std::endl;

	for(;;)
	{
		int	poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			std::cerr << "Error : poll : " << std::strerror(errno) << std::endl;
			close(sockfd);
			return (1);
		}

		for (int i = 0; i < fd_count; i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				if (pfds[i].fd == sockfd)
				{
					addrlen = sizeof remoteaddr;
					new_fd = accept(sockfd, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1)
						std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
					else
					{
					}
				}
			}
		}
	}

	return (0);
}

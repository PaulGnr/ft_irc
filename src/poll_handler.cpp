#include "ft_irc.hpp"

static void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static void	add_to_pfds(struct pollfd **pfds, int new_fd, int *fd_count, int *fd_size)
{
	if (*fd_count == *fd_size)
	{
		struct pollfd	*new_pfds;

		*fd_size *= 2;
		new_pfds = new struct pollfd[*fd_size];
		for (int i = 0; i < *fd_size; ++i)
			new_pfds[i] = *pfds[i];
		delete [] *pfds;
		*pfds = new_pfds;
	}
	(*pfds)[*fd_count].fd = new_fd;
	(*pfds)[*fd_count].events = POLLIN;
	(*pfds)[*fd_count].revents = 0;

	++(*fd_count);
}

static void	del_from_pfds(struct pollfd *pfds, int i, int *fd_count)
{
	close(pfds[i].fd);
	pfds[i] = pfds[--(*fd_count)];
}

static void	poll_loop(struct pollfd pfds[], int sockfd, int fd_count, int fd_size)
{
	int						new_fd;
	struct sockaddr_storage	remoteaddr;
	socklen_t				addrlen;
	char					buf[256];
	char					remoteIP[INET6_ADDRSTRLEN];

	for(;;)
	{
		int	poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			std::cerr << "Error : poll : " << std::strerror(errno) << std::endl;
			close(sockfd);
			exit(1);
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
						add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
						std::cout << "pollserver : New connection from ";
						std::cout << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << " on socket " << new_fd << std::endl;
					}
				}
				else
				{
					int	nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
					int	sender_fd = pfds[i].fd;

					if (nbytes <= 0)
					{
						if (nbytes == 0)
							std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
						else
							std::cerr << "recv" << std::endl;
						del_from_pfds(pfds, i, &fd_count);
					}
					else
					{
						if (!parsing(buf))
						{
							while (fd_count)
								del_from_pfds(pfds, 0, &fd_count);
							delete [] pfds;
							exit(0);
						}
						for (int j = 0; j < fd_count; ++j)
						{
							int	dest_fd = pfds[j].fd;

							if (dest_fd != sockfd && dest_fd != sender_fd)
							{
								if (sendall(dest_fd, buf, &nbytes) == -1)
									std::cerr << "send" << std::endl;
							}
						}
					}
				}
			}
		}
	}

	delete [] pfds;
}

void	poll_handler(int sockfd)
{
	struct pollfd			*pfds;
	int						fd_count = 0, fd_size = 5;

	pfds = new struct pollfd[fd_size];

	add_to_pfds(&pfds, sockfd, &fd_count, &fd_size);

	std::cout << "Waiting for connections..." << std::endl;

	poll_loop(pfds, sockfd, fd_count, fd_size);
}

#include "ft_irc.hpp"

static void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	poll_handler(Server &server)
{
	int						new_fd;
	struct sockaddr_storage	remoteaddr;
	socklen_t				addrlen;
	char					remoteIP[INET6_ADDRSTRLEN];
	char					buf[256];
	std::vector<struct pollfd>	&pfds = server.getPfds();
	
	std::cout << "Waiting for connections..." << std::endl;
	for(;;)
	{
		int	poll_count = poll(pfds.data(), pfds.size(), -1);

		if (poll_count == -1)
		{
			std::cerr << "Error : poll : " << std::strerror(errno) << std::endl;
			close(server.getListener());
			exit(1);
		}

		for (PFDIT i = pfds.begin(); i != pfds.end(); ++i)
		{
			std::cout << i->fd << " " << i->events << " " << i->revents << std::endl;
			if (i->revents & POLLIN)
			{
				if (i->fd == server.getListener())
				{
					addrlen = sizeof remoteaddr;
					new_fd = accept(server.getListener(), (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1)
						std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
					else
					{
						server.addUser(new_fd, remoteaddr);
						std::cout << "pollserver : New connection from ";
						std::cout << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << " on socket " << new_fd << std::endl;
					}
				}
				else
				{
					std::cout << i->fd << std::endl;
					User	&user = server.getUser(i);
					user.clearMsg();
					int	sender_fd = user.getPfd()->fd;
					int	nbytes = 0;

					memset(buf, 0, 256);
					nbytes = recv(sender_fd, buf, 256, 0);
					std::cout << buf << std::endl;

					if (nbytes <= 0)
					{
						if (nbytes == 0)
							std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
						else
							std::cerr << "recv" << std::endl;
						server.delUser((pfds.begin() - i) / (sizeof i * 16) - 1);
					}
					else
					{
						for (std::vector<User *>::iterator j = server.getUsers().begin();j != server.getUsers().end(); ++j)
						{
							int	dest_fd = (*j)->getPfd()->fd;

							if (dest_fd != sender_fd)
							{
								if (sendall(dest_fd, buf, &nbytes) == -1)
									std::cerr << "send" << std::endl;
							}
						}
					}
				}
			}
			/*
				if (server.getPfds()[i].fd == server.getListener())
				{
					addrlen = sizeof remoteaddr;
					new_fd = accept(server.getListener(), (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1)
						std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
					else
					{
						server.addUser(new_fd, remoteaddr);
						std::cout << "pollserver : New connection from ";
						std::cout << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << " on socket " << new_fd << std::endl;
					}
				}
				else
				{
					server.getUsers()[i + 1]->clearMsg();
					int	sender_fd = server.getPfds()[i].fd;
					int	nbytes = 0;

					memset(buf, 0, 256);
					while (!std::strstr(buf, "\r\n"))
					{
						memset(buf, 0, 256);
						nbytes = recv(sender_fd, buf, 256, 0);
						if (nbytes <= 0)
							break;
						server.getUsers()[i + 1]->appendMsg(buf);
					}

					if (nbytes <= 0)
					{
						if (nbytes == 0)
							std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
						else
							std::cerr << "recv" << std::endl;
						server.delUser(i);
					}
					else
					{
						if (!parsing(server.getUsers()[i + 1]->getMessage().c_str(), *(server.getUsers()[i + 1])))
						{
							//server.die();
							exit(0);
						}
						for (unsigned long j = 0; j < server.getUsers().size(); ++j)
						{
							int	dest_fd = server.getUsers()[j]->getPfd()->fd;

							if (dest_fd != sender_fd)
							{
								if (sendall(dest_fd, buf, &nbytes) == -1)
									std::cerr << "send" << std::endl;
							}
						}
					}
				}
			}
			*/
		}
	}
}

#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _password(password), _host("localhost")
{
	_createListener();
}

Server::~Server()
{}

std::vector<struct pollfd>	&Server::getPfds(void) {return (_pfds);}
std::vector<User *>			&Server::getUsers(void) {return (_users);}
std::string					Server::getPort(void) const {return (_port);}
std::string					Server::getPassword(void) const {return (_password);}
int							Server::getListener(void) const {return (_listener);}

void	Server::poll_handler(void)
{
	int						new_fd;
	struct sockaddr_storage	remoteaddr;
	socklen_t				addrlen;
	char					remoteIP[INET6_ADDRSTRLEN];
	char					buf[256];

	std::cout << "Waiting for connections..." << std::endl;
	for(;;)
	{
		int	poll_count = poll(_pfds.data(), _pfds.size(), -1);

		if (poll_count == -1)
		{
			std::cerr << "Error : poll : " << std::strerror(errno) << std::endl;
			close(_listener);
			exit(1);
		}

		for (size_t i = 0; i < _pfds.size(); ++i)
		{
			if (_pfds[i].revents & POLLIN)
			{
				if (_pfds[i].fd == _listener)
				{
					addrlen = sizeof remoteaddr;
					new_fd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1)
						std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
					else
					{
						addUser(new_fd, remoteaddr);
						std::cout << "pollserver : New connection from ";
						std::cout << inet_ntop(remoteaddr.ss_family, _get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << " on socket " << new_fd << std::endl;
					}
				}
				else
				{
					int	sender_fd = _pfds[i].fd;
					int	nbytes = 0;

					_users[i - 1]->clearMessage();
					memset(buf, 0, sizeof buf);
					while (!std::strstr(buf, "\r\n"))
					{
						memset(buf, 0, sizeof buf);
						nbytes = recv(sender_fd, buf, sizeof buf, 0);
						if (nbytes <= 0)
							break ;
						_users[i - 1]->appendMessage(buf);
					}
					
					if (nbytes <= 0)
					{
						if (nbytes == 0)
							std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
						else
							std::cerr << "recv" << std::endl;
						delUser(i);
					}
					else
					{
						_users[i - 1]->parse_info();
						_sendMsg(_users[i - 1], sender_fd);
					}
				}
			}
		}
	}
}

void	Server::addUser(int fd, struct sockaddr_storage &addr)
{
	struct pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	_users.push_back(new User(&(_pfds.back()), &addr));
}

void	Server::delUser(int i)
{
	close(_pfds[i].fd);
	_pfds.erase(_pfds.begin() + i);
	if (i > 0)
		_users.erase(_users.begin() + i - 1);
}

void	Server::_createListener(void)
{
	struct addrinfo	hints, *servinfo, *p;
	int				status, yes = 1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, _port.c_str(), &hints, &servinfo)) != 0)
	{
		std::cerr << "Error : getaddrinfo : " << gai_strerror(status) << std::endl;
		exit(1);
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "Error : socket : " << std::strerror(errno) << std::endl;
			continue;
		}

		if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			std::cerr << "Error : setsockopt : " << std::strerror(errno) << std::endl;
			close(_listener);
			freeaddrinfo(servinfo);
			exit(1);
		}

		if (bind(_listener, p->ai_addr, p->ai_addrlen) == -1)
		{
			std::cerr << "Error : bind : " << std::strerror(errno) << std::endl;
			close(_listener);
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		std::cerr << "Error : server failed to bind" << std::endl;
		exit(1);
	}

	if (listen(_listener, BACKLOG) == -1)
	{
		std::cerr << "Error : listen : " << std::strerror(errno) << std::endl;
		exit(1);
	}

	struct pollfd	pfd;

	pfd.fd = _listener;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
}

void	*Server::_get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	Server::_sendMsg(User *user, int sender_fd)
{
	std::string	msg = user->getMessage();
	int			nbytes;

	if (user->getMessage().length())
	{
		for (size_t j = 0; j < _pfds.size(); ++j)
		{
			int	dest_fd = _pfds[j].fd;

			if (dest_fd != _listener && dest_fd != sender_fd)
			{
				nbytes = msg.length();
				if (_sendall(dest_fd, msg.c_str(), &nbytes) == -1)
					std::cerr << "send" << std::endl;
			}
		}
	}
}

int	Server::_sendall(int dest_fd, const char *buf, int *nbytes)
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

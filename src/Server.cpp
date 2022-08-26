#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _password(password)
{
	createListener();
}

Server::~Server()
{}

std::vector<struct pollfd>	&Server::getPfds(void) {return (_pfds);}
std::vector<User *>			&Server::getUsers(void) {return (_users);}
std::string					Server::getPort(void) const {return (_port);}
std::string					Server::getPassword(void) const {return (_password);}
int							Server::getListener(void) const {return (_listener);}
User						&Server::getUser(PFDIT &it)
{
	std::vector<User *>	user = getUsers();
	unsigned long	i = 0;

	std::cout << (*it).fd << std::endl;
	while (it->fd != user[i]->getPfd()->fd && i < user.size())
	{
		std::cout << it->fd << " " << user[i]->getPfd()->fd << std::endl;
		++i;
	}
	return (*(user[i]));
}

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

		for (unsigned long i = 0; i < _pfds.size(); ++i)
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
						std::cout << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << " on socket " << new_fd << std::endl;
					}
				}
				else
				{
					int	sender_fd = _pfds[i].fd;
					int	nbytes = 0;

					_users[i - 1]->clearMsg();
					memset(buf, 0, sizeof buf);
					while (!std::strstr(buf, "\r\n"))
					{
						memset(buf, 0, sizeof buf);
						nbytes = recv(sender_fd, buf, sizeof buf, 0);
						if (nbytes <= 0)
							break ;
						_users[i - 1]->appendMsg(buf);
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
						_parse_user_info(sender_fd, buf);
						for (unsigned long j = 0; j < _pfds.size(); ++j)
						{
							int	dest_fd = _pfds[j].fd;

							if (dest_fd != _listener && dest_fd != sender_fd)
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

void	Server::createListener(void)
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

void	*Server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	Server::_parse_user_info(int sender_fd, std::string buf)
{
	std::string	nick;
	std::string	user;
	size_t		i = 0;

	std::cout << buf << std::endl;

	nick = buf.substr(buf.find("NICK") + 5);
	while (i < nick.find("\r"))
		i++;
	nick = nick.substr(0, i);

	i = 0;
	user = buf.substr(buf.find("USER") + 5);
	while (i < user.find(' '))
		i++;
	user = user.substr(0, i);

	std::cout << "NICK: -" << nick << "-" << std::endl;
	std::cout << "USER: -" << user << "-" << std::endl;

	// boucle infinie
	// _welcome(sender_fd, nick, user);
	(void)sender_fd;

	// segfault
	// _users[sender_fd]->setNickname(nick);
	// _users[sender_fd]->setUser(user);
}

void	Server::_welcome(int sender_fd, std::string nick, std::string user)
{
	// degueu mais temporaire
	std::string	host = "localhost";

	std::string	m1 = "001 " + nick + " :Welcome to the " + host + " network, " + nick + "[" + user + "@" + host + "]\r\n";
	std::string	m2 = "002 " + nick + " :Your host is " + host + ", running version 1.2.3\r\n";
	std::string	m3 = "003 " + nick + " :This server was created 18:07:30\r\n";
	std::string	m4 = "004 " + nick + " localhost irssi 1.2.3 (20210409 0011)\r\n";

	send(sender_fd, m1.c_str(), m1.length(), 0);
	send(sender_fd, m2.c_str(), m2.length(), 0);
	send(sender_fd, m3.c_str(), m3.length(), 0);
	send(sender_fd, m4.c_str(), m4.length(), 0);
}

#include "Server.hpp"

bool	running;

Server::Server() : _port(""), _password(""), _host("")
{}

Server::Server(std::string port, std::string password): _port(port), _password(password), _host("localhost")
{
	_createListener();
	_createCmd();
	_createModeOption();
	_poll_handler();
}

Server::~Server()
{}

std::vector<struct pollfd>	&Server::getPfds(void) {return (_pfds);}
std::map<int, User *>		&Server::getUsers(void) {return (_users);}
std::string					Server::getPort(void) const {return (_port);}
std::string					Server::getPassword(void) const {return (_password);}
int							Server::getListener(void) const {return (_listener);}

void	Server::_poll_handler(void)
{
	pollfd server_fd = {_listener, POLLIN, 0};
	_pfds.push_back(server_fd);

	std::cout << "Waiting for clients..." << std::endl;

	while (running)
	{
		poll(_pfds.data(), _pfds.size(), -1);

		for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); it++)
		{
			if (it->revents & POLLHUP)
			{
				std::cout << "pollserver : socket " << it->fd << " hung up" << std::endl;
				_delUser(it);
			}
			if (it->revents & POLLIN)
			{
				if (it->fd == _listener)
				{
					_clientConnect();
					break ;
				}
				_clientMessage(it);
			}
			if (it == _pfds.end())
				break;
		}
	}
	_clean();
}

void	Server::_createListener(void)
{
	struct	addrinfo	hint, *serv_address = NULL;

	bzero(&hint, sizeof(hint));

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, _port.c_str(), &hint, &serv_address))
		throw std::runtime_error("Error while getting address info.");

	int sockfd = socket(serv_address->ai_family, serv_address->ai_socktype, serv_address->ai_protocol);

	if (sockfd < 0)
		throw std::runtime_error("Error while opening socket.");

	if (bind(sockfd, serv_address->ai_addr, serv_address->ai_addrlen) < 0)
	{
		freeaddrinfo(serv_address);
		close(sockfd);
		throw std::runtime_error("Error while binding socket.");
	}
	freeaddrinfo(serv_address);

	if (listen(sockfd, 1000) < 0)
	{
		close(sockfd);
		throw std::runtime_error("Error while listening on socket.");
	}
	_listener = sockfd;
}

void	Server::_clientConnect(void)
{
	int						new_fd;
	struct sockaddr_storage	remoteaddr;
	socklen_t				addrlen;
	char					remoteIP[INET6_ADDRSTRLEN];

	addrlen = sizeof remoteaddr;
	new_fd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);
	if (new_fd == -1)
		std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
	else
	{
		_addUser(new_fd, remoteaddr);
		char port[1000];
		getnameinfo((struct sockaddr *)&remoteaddr, addrlen, remoteIP, INET6_ADDRSTRLEN, port, INET6_ADDRSTRLEN, 0);
		std::cout << "New connection from " << _host << ":" << port << " on socket " << new_fd << std::endl;
	}
}

void	Server::_clientMessage(pfds_iterator &it)
{
	try
	{
		User	*user = _users.at(it->fd);
		int		nbytes;

		nbytes = _getMessage(user);
		if (nbytes <= 0)
		{
			if (nbytes == 0) // disconnect
				std::cout << "pollserver : socket " << it->fd << " hung up" << std::endl;
			else // error
				std::cerr << "Error : recv" << std::endl;
			_delUser(it);
		}
		else
			_handleCmd(user);
	}
	catch (const std::out_of_range &e)
	{
		std::cout << "Out of range from _clientMessage" << std::endl;
	}
}

int	Server::_getMessage(User *user)
{
	int			fd = user->getFd();
	int			nbytes = 0;
	char		buf[1024];
	std::string	str;

	user->clearMsg();
	while (str.rfind("\r\n") != str.length() - 2 || str.length() < 2)
	{
		memset(buf, 0, sizeof(buf));
		nbytes = recv(fd, buf, sizeof(buf), 0);
		if (nbytes <= 0)
			break ;
		str.append(buf);
	}
	user->setMessage(str);
	return (nbytes);
}

void	Server::_addUser(int fd, struct sockaddr_storage &addr)
{
	struct pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	_users.insert(std::make_pair(fd, new User(fd, &addr)));
}

void	Server::_delUser(pfds_iterator &it)
{
	try
	{
		User	*user = _users.at(it->fd);

		if (it->fd > 0)
		{
			_users.erase(it->fd);
			close(it->fd);
		}
		_pfds.erase(it);
		while (user->isInChan())
			_partCmd(user, user->getFirstChan()->getName() + " :" + &(user->getFirstChan()->getName()[1]));
		delete user;
	}
	catch (const std::out_of_range &e)
	{
		std::cout << "delUser error :Wrong fd (problem with pollfd)" << std::endl;
	}
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

User	*Server::_getUserByNick(std::string nick)
{
	users_iterator	it;

	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == nick)
			break;
	}
	if (it == _users.end())
		return (NULL);
	return (it->second);
}

void	Server::_close(User *user)
{
	user->sendReply("Please, enter PASS first. Disconnecting.");
	int	fd = user->getFd();
	close(fd);
	for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); ++it)
	{
		if (fd == it->fd)
		{
			_pfds.erase(it);
			break;
		}
	}
	_users.erase(fd);
	delete user;
}

void	Server::_clean(void)
{
	for (chans_iterator it = _chans.begin(); it != _chans.end(); ++it)
	{
		delete it->second;
	}
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		delete it->second;
	}
	for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); ++it)
	{
		close(it->fd);
	}
}

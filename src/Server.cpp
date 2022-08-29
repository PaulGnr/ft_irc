#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _password(password), _host("localhost")
{
	_createListener();
}

Server::~Server()
{}

std::vector<struct pollfd>	&Server::getPfds(void) {return (_pfds);}
std::map<int, User *>		&Server::getUsers(void) {return (_users);}
std::string					Server::getPort(void) const {return (_port);}
std::string					Server::getPassword(void) const {return (_password);}
int							Server::getListener(void) const {return (_listener);}

void	Server::poll_handler(void)
{
	pollfd server_fd = {_listener, POLLIN, 0};
	_pfds.push_back(server_fd);

	server_log("Waiting for clients...");

	while (true)
	{
		// wait for incoming connections
		poll(_pfds.data(), _pfds.size(), -1);

		for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); it++)
		{
			if (it->revents & POLLIN)
			{
				if (it->fd == _listener)
				{
					_clientConnect();
					// std::cout << "client connect" << std::endl;
					break ;
				}
				_clientMessage(it);
				// std::cout << "message" << std::endl;
			}
		}
	}
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
		addUser(new_fd, remoteaddr);
		char port[1000];
		getnameinfo((struct sockaddr *)&remoteaddr, addrlen, remoteIP, INET6_ADDRSTRLEN, port, INET6_ADDRSTRLEN, 0);
		std::cout << "New connection from " << _host << ":" << port << " on socket " << new_fd << std::endl;
		// std::cout << inet_ntop(remoteaddr.ss_family, _get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
	}
}

void	Server::_clientMessage(pfds_iterator &it)
{
	char	buf[1024];
	int		sender_fd = it->fd;
	User	*user = _users.at(sender_fd);
	int		nbytes = 0;

	// get message
	user->clearMessage();
	memset(buf, 0, sizeof buf);
	while (!std::strstr(buf, "\r\n"))
	{
		memset(buf, 0, sizeof buf);
		nbytes = recv(sender_fd, buf, sizeof buf, 0);
		if (nbytes <= 0)
			break ;
		user->appendMessage(buf);
	}
	// < 0 = error, == 0 = user disconnect else execute msg
	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
		else
			std::cerr << "recv" << std::endl;
		delUser(sender_fd);
		--it;
	}
	else
	{
		user->parse_info(getPassword());
		if (!user->hasBeenWelcomed())
		{
			delUser(sender_fd);
			--it;
		}
		_sendMsg(user, sender_fd);
	}
}

void	Server::addUser(int fd, struct sockaddr_storage &addr)
{
	struct pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	_users.insert(std::make_pair(fd, new User(fd, &addr)));
}

void	Server::delUser(int i)
{
	if (i > 0)
		_users.erase(i);
	close(i);
	for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); ++it)
	{
		if (it->fd == i)
		{
			_pfds.erase(it);
			break;
		}
	}
}

void	Server::_createListener(void)
{
	struct	addrinfo	hint, *serv_address = NULL;

	bzero(&hint, sizeof(hint));

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, _port.c_str(), &hint, &serv_address);

	//int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int sockfd = socket(serv_address->ai_family, serv_address->ai_socktype, serv_address->ai_protocol);

	if (sockfd < 0)
		throw std::runtime_error("Error while opening socket.");

	// Bind the socket to the current IP address on selected port
	if (bind(sockfd, serv_address->ai_addr, serv_address->ai_addrlen) < 0)
		throw std::runtime_error("Error while binding socket.");

	// Let socket be able to listen for requests
	if (listen(sockfd, 1000) < 0)
		throw std::runtime_error("Error while listening on socket.");
	_listener = sockfd;

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

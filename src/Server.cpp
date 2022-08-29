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
			if (it->revents & POLLHUP)
			{
				std::cout << "client disconnect" << std::endl;
				break ;
			}
			if (it->revents & POLLIN)
			{
				if (it->fd == _listener)
				{
					std::cout << "client connect" << std::endl;
					break ;
				}
				std::cout << "message" << std::endl;
			}
		}
	}
}

// void	Server::poll_handler(void)
// {
// 	int						new_fd;
// 	struct sockaddr_storage	remoteaddr;
// 	socklen_t				addrlen;
// 	char					remoteIP[INET6_ADDRSTRLEN];
// 	char					buf[256];

// 	std::cout << "Waiting for connections..." << std::endl;

// 	while (true)
// 	{
// 		if (poll(_pfds.data(), _pfds.size(), -1) < 0)
// 		{
// 			std::cerr << "Error : poll : " << std::strerror(errno) << std::endl;
// 			close(_listener);
// 			exit(1);
// 		}
// 		std::cout << "while" << std::endl;

// 		size_t i = 0;
// 		for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); it++, i++)
// 		{
// 			std::cout << "i: " << i << ", size: " << _pfds.size() << std::endl;

// 			if (it->revents == 0)
// 			{
// 				std::cout << "continue" << std::endl;
// 				continue ;
// 			}

// 			if ((it->revents & POLLIN) == POLLIN)
// 			{
// 				std::cout << "pollin" << std::endl;
// 				if (it->fd == _listener)
// 				{
// 					std::cout << "sock" << std::endl;
// 					addrlen = sizeof remoteaddr;
// 					new_fd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);
// 					if (new_fd == -1)
// 						std::cerr << "Error : accept : " << std::strerror(errno) << std::endl;
// 					else
// 					{
// 						addUser(new_fd, remoteaddr);
// 						char port[1000];
// 						getnameinfo((struct sockaddr *)&remoteaddr, addrlen, remoteIP, INET6_ADDRSTRLEN, port, INET6_ADDRSTRLEN, 0);
// 						std::cout << "New connection from " << _host << ":" << port << " on socket " << new_fd << std::endl;
// 						// std::cout << inet_ntop(remoteaddr.ss_family, _get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
// 					}
// 				}
// 				else
// 				{
// 					std::cout << "message1" << std::endl;
// 					int	sender_fd = it->fd;
// 					int	nbytes = 0;
// 					_users[i - 1]->clearMessage();
// 					std::cout << "debug" << std::endl;
// 					memset(buf, 0, sizeof buf);
// 					while (!std::strstr(buf, "\r\n"))
// 					{
// 						memset(buf, 0, sizeof buf);
// 						nbytes = recv(sender_fd, buf, sizeof buf, 0);
// 						if (nbytes <= 0)
// 							break ;
// 						_users[i - 1]->appendMessage(buf);
// 					}

// 					if (nbytes <= 0)
// 					{
// 						std::cout << "pollhup" << std::endl;
// 						if (nbytes == 0)
// 							std::cout << "pollserver : socket " << sender_fd << " hung up" << std::endl;
// 						else
// 							std::cerr << "recv" << std::endl;
// 						delUser(i);
// 					}
// 					else
// 					{
// 						std::cout << "message2" << std::endl;
// 						_users[i - 1]->parse_info(getPassword());
// 						_sendMsg(_users[i - 1], sender_fd);
// 					}
// 				}
// 			}
// 		}
// 	}
// }

void	Server::addUser(int fd, struct sockaddr_storage &addr)
{
	struct pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	_users.insert(std::make_pair(fd, new User(&(_pfds.back()), &addr)));
}

void	Server::delUser(int i)
{
	if (i > 0)
		_users.erase(_pfds[i].fd);
	close(_pfds[i].fd);
	_pfds.erase(_pfds.begin() + i);
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

	// Forcefully attaching socket to the port
	// int val = 1;
	// if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
	// 	throw std::runtime_error("Error while setting socket options.");

	/*
	 * As requested from subject we set the socket to NON-BLOCKING mode
	 * allowing it to return any data that the system has in it's read buffer
	 * for that socket, but, it won't wait for that data.
	 */
	// if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
	// 	throw std::runtime_error("Error while setting socket to NON-BLOCKING.");
	// }

	//struct sockaddr_in serv_address = {};

	// Clear address structure, should prevent some segmentation fault and artifacts
	//bzero(&serv_address, sizeof(serv_address));

	/*
	 * htons() convert unsigned short int to big-endian network byte order as expected from TCP protocol standards
	 */
	/*
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;
	serv_address.sin_port = htons(std::stoi(_port));
	*/

	// Bind the socket to the current IP address on selected port
	if (bind(sockfd, serv_address->ai_addr, serv_address->ai_addrlen) < 0)
	//if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		throw std::runtime_error("Error while binding socket.");

	// Let socket be able to listen for requests
	if (listen(sockfd, 1000) < 0)
		throw std::runtime_error("Error while listening on socket.");
	_listener = sockfd;



	// struct addrinfo	hints, *servinfo, *p;
	// int				status, yes = 1;

	// memset(&hints, 0, sizeof hints);
	// hints.ai_family = AF_UNSPEC;
	// hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE;

	// if ((status = getaddrinfo(NULL, _port.c_str(), &hints, &servinfo)) != 0)
	// {
	// 	std::cerr << "Error : getaddrinfo : " << gai_strerror(status) << std::endl;
	// 	exit(1);
	// }

	// for (p = servinfo; p != NULL; p = p->ai_next)
	// {
	// 	if ((_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
	// 	{
	// 		std::cerr << "Error : socket : " << std::strerror(errno) << std::endl;
	// 		continue;
	// 	}

	// 	if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
	// 	{
	// 		std::cerr << "Error : setsockopt : " << std::strerror(errno) << std::endl;
	// 		close(_listener);
	// 		freeaddrinfo(servinfo);
	// 		exit(1);
	// 	}

	// 	if (bind(_listener, p->ai_addr, p->ai_addrlen) == -1)
	// 	{
	// 		std::cerr << "Error : bind : " << std::strerror(errno) << std::endl;
	// 		close(_listener);
	// 		continue;
	// 	}
	// 	break;
	// }

	// freeaddrinfo(servinfo);

	// if (p == NULL)
	// {
	// 	std::cerr << "Error : server failed to bind" << std::endl;
	// 	exit(1);
	// }

	// if (listen(_listener, BACKLOG) == -1)
	// {
	// 	std::cerr << "Error : listen : " << std::strerror(errno) << std::endl;
	// 	exit(1);
	// }

	// pollfd	pfd = {_listener, POLLIN, 0};
	// _pfds.push_back(pfd);
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

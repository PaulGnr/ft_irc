#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _password(password), _host("localhost")
{
	_createListener();
	_createCmd();
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

	std::cout << "Waiting for clients..." << std::endl;

	while (true)
	{
		poll(_pfds.data(), _pfds.size(), -1);

		for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); it++)
		{
			if (it->revents & POLLHUP) // En fait y en a besoin avec irssi
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

	int sockfd = socket(serv_address->ai_family, serv_address->ai_socktype, serv_address->ai_protocol);

	if (sockfd < 0)
		throw std::runtime_error("Error while opening socket.");

	if (bind(sockfd, serv_address->ai_addr, serv_address->ai_addrlen) < 0)
		throw std::runtime_error("Error while binding socket.");

	if (listen(sockfd, 1000) < 0)
		throw std::runtime_error("Error while listening on socket.");
	_listener = sockfd;
}

void	Server::_createCmd(void)
{
	_cmd.insert(std::make_pair("CAP", &Server::_caplsCmd));
	_cmd.insert(std::make_pair("PASS", &Server::_passCmd));
	_cmd.insert(std::make_pair("NICK", &Server::_nickCmd));
	_cmd.insert(std::make_pair("USER", &Server::_userCmd));
	_cmd.insert(std::make_pair("JOIN", &Server::_joinCmd));
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

int	Server::_getMessage(User *user)
{
	int			fd = user->getFd();
	int			nbytes = 0;
	char		buf[1024];
	std::string	str;

	while (str.rfind("\r\n") != str.length() - 2 || str.length() <= 2)
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

/*
void	Server::_clientMessage(pfds_iterator &it)
{
	User	*user = _users.at(it->fd);
	char	buf[1024];
	int		nbytes = 0;

	// get message
	user->clearMessage();
	memset(buf, 0, sizeof(buf));
	while (!std::strstr(buf, "\r\n"))
	{
		memset(buf, 0, sizeof(buf));
		nbytes = recv(it->fd, buf, sizeof(buf), 0);
		if (nbytes <= 0)
			break ;
		user->appendMessage(buf);
	}

	// <0: error, ==0: user disconnect, >0: execute msg
	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "pollserver : socket " << it->fd << " hung up" << std::endl;
		else
			std::cerr << "Error : recv" << std::endl;
		_delUser(it);
	}
	else
	{
		user->parse_info(getPassword());
		if (!user->hasBeenWelcomed())
			_delUser(it);
		_sendMsg(user, it->fd);
	}
}
*/

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
	User	*user = _users.at(it->fd);
	if (it->fd > 0)
		_users.erase(it->fd);
	close(it->fd);
	_pfds.erase(it);
	it--;
	delete user;
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

void	Server::_handleCmd(User *user)
{
	std::string	msg = user->getMessage();
	std::string	cmd;
	std::string	buf;

	// std::cout << "msg:" << msg << std::endl;
	// std::cout << "cmd:" << cmd << std::endl;
	// std::cout << "buf:" << buf << std::endl;

	while (msg.length())
	{
		if (msg.find("\r\n") != std::string::npos)
		{
			if (msg.find(' ') != std::string::npos)
				cmd = msg.substr(0, msg.find(' '));
			else
				cmd = msg.substr(0, msg.find("\r\n"));
			if (cmd.length() == msg.find("\r\n"))
				buf.clear();
			else
				buf = msg.substr(cmd.length() + 1, msg.find("\r\n") - cmd.length() - 1);
			try
			{
				std::cout << "try cmd: " << cmd << std::endl;
				(this->*(_cmd.at(cmd)))(user, buf);
				/* Pour au-dessus : Partie un peu tricky, en gros je sors le
				 * pointeur sur fonction correspondant a la cmd dans la map,
				 * ensuite je le dereference avec * et ensuite je vais chercher
				 * la fonction qui correspond a cette instance de class Server
				 * avec le this->, et enfin je mets les arguments que je veux
				 * lui envoyer
				 */
				msg.erase(0, msg.find("\r\n") + 2);
			}
			catch (const std::out_of_range &e)
			{
				std::cout << "fail: " << e.what() << std::endl;
				msg.clear();
				user->sendReply(ERR_UNKNOWNCOMMAND(user->getNickname(), cmd)); //Changer message erreur
			}
		}
		else
		{
			msg.clear();
			user->sendReply("Message error"); //Changer message erreur
		}
	}
	user->clearMsg();
}

void	Server::_caplsCmd(User *user, std::string buf)
{
	if (!buf.compare("LS"))
		return;
	user->sendReply("Error of command"); //Changer message erreur
}

void	Server::_passCmd(User *user, std::string buf)
{
	if (user->hasBeenWelcomed())
	{
		user->sendReply(ERR_ALREADYREGISTERED(user->getNickname()));
		return;
	}
	if (!buf.length())
	{
		user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "PASS"));
		return;
	}
	if (buf.compare(_password))
	{
		user->sendReply(ERR_PASSWDMISMATCH(user->getNickname()));
		return;
	}
	user->setPasswdOK(true);
	if (user->getNickname().length() && user->getUser().length())
		user->welcome();
}

void	Server::_nickCmd(User *user, std::string buf)
{
	if (!buf.length())
	{
		user->sendReply(ERR_NONICKNAMEGIVEN(user->getNickname()));
		return;
	}
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		/*
		if (it->getNickname() == buf)
		{
			user->sendReply(ERR_NICKNAMEINUSE(user->getNickname()));
			return;
		}
		*/
	}
	user->setNickname(buf);
	if (user->getUser().length() && user->getPasswdOK() && !user->hasBeenWelcomed())
		user->welcome();
}

void	Server::_userCmd(User *user, std::string buf)
{
	if (!buf.length())
	{
		user->sendReply("Error : need more info"); //Changer message erreur
		return;
	}
	if (buf.find(' ') != std::string::npos)
		buf = buf.substr(0, buf.find(' '));
	user->setUser(buf);
	if (user->getNickname().length() && user->getPasswdOK() && !user->hasBeenWelcomed())
		user->welcome();
}

void	Server::_joinCmd(User *user, std::string buf)
{
	std::cout << "join asked with:" << buf << std::endl;
	user->sendReply("join asked");
}

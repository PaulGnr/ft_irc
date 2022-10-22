#include "Server.hpp"

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

	if (getaddrinfo(NULL, _port.c_str(), &hint, &serv_address))
		throw std::runtime_error("Error while getting address info.");

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
	_cmd.insert(std::make_pair("QUIT", &Server::_quitCmd));
	_cmd.insert(std::make_pair("PING", &Server::_pingCmd));
	_cmd.insert(std::make_pair("MODE", &Server::_modeCmd));
	_cmd.insert(std::make_pair("TOPIC", &Server::_topicCmd));
	_cmd.insert(std::make_pair("JOIN", &Server::_joinCmd));
	_cmd.insert(std::make_pair("PART", &Server::_partCmd));
	_cmd.insert(std::make_pair("PRIVMSG", &Server::_privmsgCmd));
	_cmd.insert(std::make_pair("NOTICE", &Server::_noticeCmd));
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

User	*Server::_getUserByNick(std::string nick)
{
	users_iterator	it;

	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == nick)
			break;
	}
	return (it->second);
}

void	Server::_handleCmd(User *user)
{
	std::string	msg = user->getMessage();
	std::string	cmd;
	std::string	buf;

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
				msg.erase(0, msg.find("\r\n") + 2);
				user->sendReply(ERR_UNKNOWNCOMMAND(user->getNickname(), cmd));
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
	if (buf != "LS")
		return (user->sendReply(ERR_UNKNOWNCOMMAND(user->getNickname(), "CAP")));
}

void	Server::_passCmd(User *user, std::string buf)
{
	if (user->hasBeenWelcomed())
		return (user->sendReply(ERR_ALREADYREGISTERED(user->getNickname())));
	if (!buf.length())
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "PASS")));
	if (buf.compare(_password))
		return (user->sendReply(ERR_PASSWDMISMATCH(user->getNickname())));
	user->setPasswdOK(true);
	if (user->getNickname().length() && user->getUser().length())
		user->welcome();
}

void	Server::_nickCmd(User *user, std::string buf)
{
	if (buf.empty())
		return (user->sendReply(ERR_NONICKNAMEGIVEN(user->getNickname())));
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == buf)
			return (user->sendReply(ERR_NICKCOLLISION(user->getNickname())));
	}
	user->setNickname(buf);
	if (user->getUser().length() && user->getPasswdOK() && !user->hasBeenWelcomed())
		user->welcome();
}

void	Server::_userCmd(User *user, std::string buf)
{
	if (user->hasBeenWelcomed())
		return (user->sendReply(ERR_ALREADYREGISTERED(user->getNickname())));
	if (buf.empty())
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "USER")));
	if (buf.find(' ') != std::string::npos)
		buf = buf.substr(0, buf.find(' '));
	user->setUser(buf);
	if (user->getNickname().length() && user->getPasswdOK() && !user->hasBeenWelcomed())
		user->welcome();
}

void	Server::_quitCmd(User *user, std::string buf)
{
	while (user->isInChan())
		_partCmd(user, user->getFirstChan()->getName() + " :" + &(user->getFirstChan()->getName()[1]));
	user->sendReply(RPL_QUIT(user->getNickname(), (buf.empty() ? "Leaving" : buf)));
	close(user->getFd());
	_users.erase(user->getFd());
}

void	Server::_pingCmd(User *user, std::string buf)
{
	if (buf.empty())
		return (user->sendReply(ERR_NOORIGIN()));
	if (buf != _host && buf != "IRC")
		return (user->sendReply(ERR_NOSUCHSERVER(buf)));
	user->sendReply(RPL_PONG(user->getNickname(), _host));
}

void	Server::_modeCmd(User *user, std::string buf)
{
	std::cout << "buf : <" << buf << ">" << std::endl;
	if (buf.empty())
		user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE"));
	else if (buf[0] == '#' || buf[0] == '&')
		_channelModeCmd(user, buf);
	else
		_userModeCmd(user, buf);
}

void	Server::_topicCmd(User *user, std::string buf)
{
	std::string	chan_name;
	if (buf.find(' ') != std::string::npos)
		chan_name = buf.substr(0, buf.find(' '));
	else
		chan_name = buf;
	try
	{
		Channel*	chan = _chans.at(chan_name);
		if (buf.find(':') == std::string::npos)
		{
			if (chan->getTopic() == "")
				return (user->sendReply(RPL_NOTOPIC(user->getNickname(), chan_name)));
			return (user->sendReply(RPL_TOPIC(user->getNickname(), chan_name, chan->getTopic())));
		}
		std::string	topic = buf.substr(buf.find(':') + 1);

		if (chan->hasMode('t') && !chan->userIsOperator(user))
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(chan_name)));
		chan->setTopic(topic, user->getNickname());
		return (chan->broadcast(user, RPL_TOPIC(user->getNickname(), chan_name, chan->getTopic())));
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOTONCHANNEL(chan_name));
	}
}

void	Server::_joinCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "JOIN")));
	std::vector<std::string>	channels = _getChannels(buf);
	std::vector<std::string>	keys = _getKeys(buf, channels.size());
	Channel						*channel;

	for (std::vector<std::string>::iterator chan = channels.begin(); chan != channels.end(); ++chan)
	{
		if ((*chan)[0] != '#' && (*chan)[0] != '&')
			return (user->sendReply(ERR_BADCHANMASK(*chan)));
		try
		{
			channel = _chans.at(*chan);
			if (channel->isInviteOnly())
			{
				user->sendReply(ERR_INVITEONLYCHAN(channel->getName()));
				break;
			}
			if (channel->isFull())
			{
				user->sendReply(ERR_CHANNELISFULL(channel->getName()));
				break;
			}
			if (channel->userIsBan(user))
			{
				user->sendReply(ERR_BANNEDFROMCHAN(channel->getName()));
				break;
			}
			if (channel->isKeyProtect())
			{
				if (!channel->checkKey(keys[chan - channels.begin()]))
				{
					user->sendReply(ERR_BADCHANNELKEY(channel->getName()));
					break;
				}
			}
			channel->addUser(user);
			channel->broadcast(user, RPL_JOIN(user->getNickname(), *chan));
			if (channel->getTopic() != "")
			{
				user->sendReply(RPL_TOPIC(user->getNickname(), channel->getName(), channel->getTopic()));
				channel->rpl_topicwhotime(user);
			}
			channel->rpl_namreply(user);
		}
		catch (const std::out_of_range &e)
		{
			channel = _createChan(user, *chan, keys[chan - channels.begin()]);
			channel->broadcast(user, RPL_JOIN(user->getNickname(), *chan));
			channel->rpl_namreply(user);
		}
	}
}

void	Server::_partCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	std::string	channel_name;
	Channel		*channel;

	if (buf.find(' ') != std::string::npos)
		channel_name = buf.substr(0, buf.find(' '));
	else
		channel_name = buf;
	try
	{
		channel = _chans.at(channel_name);
		if (channel->userIsIn(user))
		{
			channel->broadcast(user, RPL_PART(user->getNickname(), channel_name));
			user->delChan(channel);
			if (channel->getUserCount() == 0)
				this->_delChannel(channel);
		}
		else
			user->sendReply(ERR_NOTONCHANNEL(channel_name));
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOSUCHCHANNEL(channel_name));
	}
}

void	Server::_msgToUser(User *user, std::string dest, std::string msg)
{
	std::string	nick = user->getNickname();

	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == dest)
			return (it->second->sendReply(RPL_PRIVMSG(nick, dest, msg)));
	}
	user->sendReply(ERR_NOSUCHNICK(user->getNickname()));
}

void	Server::_msgToChannel(User *user, std::string dest, std::string msg)
{
	try
	{
		Channel	*channel = _chans.at(dest);

		if (!channel->userIsIn(user) && channel->isNoOutside())
			return (user->sendReply(ERR_CANNOTSENDTOCHAN(dest)));
		channel->privmsg(user, msg);
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOSUCHCHANNEL(dest));
	}
}

void	Server::_privmsgCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.find(':') == std::string::npos)
		return (user->sendReply(ERR_NOTEXTTOSEND(user->getNickname())));
	std::string	msg = buf.substr(buf.find(':') + 1);
	std::string dest = buf.substr(0, buf.find(':'));
	size_t		start = dest.find_first_not_of(" ");

	dest = dest.substr(start, dest.find_last_not_of(" ") - start + 1);
	if (dest[0] == '#' || dest[0] == '&')
		_msgToChannel(user, dest, msg);
	else
		_msgToUser(user, dest, msg);
}

void	Server::_noticeCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.find(':') == std::string::npos)
		return;
	std::string	msg = buf.substr(buf.find(':') + 1);
	std::string dest = buf.substr(0, buf.find(':'));
	size_t		start = dest.find_first_not_of(" ");

	dest = dest.substr(start, dest.find_last_not_of(" ") - start + 1);
	if (dest[0] == '#' || dest[0] == '&')
	{
		try
		{
			Channel	*channel = _chans.at(dest);

			if (!channel->userIsIn(user) && channel->isNoOutside())
				return;
			channel->privmsg(user, msg);
		}
		catch (const std::out_of_range &e) {}
	}
	else
	{
		for (users_iterator it = _users.begin(); it != _users.end(); ++it)
		{
			if (it->second->getNickname() == dest)
				return (it->second->sendReply(RPL_PRIVMSG(user->getNickname(), dest, msg)));
		}
	}
}

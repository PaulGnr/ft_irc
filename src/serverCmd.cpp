#include "ft_irc.hpp"

void	Server::_createCmd(void)
{
	_cmd.insert(std::make_pair("CAP", &Server::_caplsCmd));
	_cmd.insert(std::make_pair("PASS", &Server::_passCmd));
	_cmd.insert(std::make_pair("NICK", &Server::_nickCmd));
	_cmd.insert(std::make_pair("USER", &Server::_userCmd));
	_cmd.insert(std::make_pair("QUIT", &Server::_quitCmd));
	_cmd.insert(std::make_pair("JOIN", &Server::_joinCmd));
	_cmd.insert(std::make_pair("PART", &Server::_partCmd));
	_cmd.insert(std::make_pair("MODE", &Server::_modeCmd));
	_cmd.insert(std::make_pair("TOPIC", &Server::_topicCmd));
	_cmd.insert(std::make_pair("NAMES", &Server::_namesCmd));
	_cmd.insert(std::make_pair("LIST", &Server::_listCmd));
	_cmd.insert(std::make_pair("INVITE", &Server::_inviteCmd));
	_cmd.insert(std::make_pair("KICK", &Server::_kickCmd));
	_cmd.insert(std::make_pair("PRIVMSG", &Server::_privmsgCmd));
	_cmd.insert(std::make_pair("NOTICE", &Server::_noticeCmd));
	_cmd.insert(std::make_pair("WHO", &Server::_whoCmd));
	_cmd.insert(std::make_pair("PING", &Server::_pingCmd));
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
				if (buf.find_first_not_of(' ') != std::string::npos)
					buf = buf.substr(buf.find_first_not_of(' '));
				else
					buf.clear();
				if (!buf.empty())
					buf = buf.substr(0, buf.find_last_not_of(' ') + 1);
				std::cout << "try cmd: <" << cmd << ">" << std::endl;
				std::cout << "with buf: <" << buf << ">" << std::endl;
				if (cmd != "CAP" && cmd != "PASS" && !user->getPasswdOK())
				{
					_close(user);
					break;
				}
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
				//std::cout << "fail: " << e.what() << std::endl;
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
	if (buf.find(' ') != std::string::npos)
		buf = buf.substr(0, buf.find_first_of(' '));
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
	std::string	username;
	std::string	hostname;
	std::string	server;
	std::string	realname;
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "USER")));
	username = buf.substr(0, buf.find(' '));
	buf = buf.substr(buf.find(' '));
	buf = buf.substr(buf.find_first_not_of(' '));
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "USER")));
	hostname = buf.substr(0, buf.find(' '));
	buf = buf.substr(buf.find(' '));
	buf = buf.substr(buf.find_first_not_of(' '));
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "USER")));
	server = buf.substr(0, buf.find(' '));
	buf = buf.substr(buf.find(' '));
	buf = buf.substr(buf.find_first_not_of(' '));
	if (buf.find(':') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "USER")));
	realname = buf.substr(1, buf.find_last_not_of(' '));
	user->setUser(username);
	user->setRealname(realname);
	if (user->getNickname().size() && user->getPasswdOK() && !user->hasBeenWelcomed())
		user->welcome();
}

void	Server::_quitCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	Channel	*channel;

	while (user->isInChan())
	{
		channel = user->getFirstChan();
		channel->broadcast(user, RPL_QUIT(user->getNickname(), (buf.empty() ? "Leaving" : buf)));
		user->delChan(channel);
		if (channel->getUserCount() == 0)
			_delChannel(channel);
	}
	int	fd = user->getFd();

	close(fd);
	_users.erase(fd);
	for (pfds_iterator it = _pfds.begin(); it != _pfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pfds.erase(it);
			break;
		}
	}
	delete user;
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
		{
			user->sendReply(ERR_BADCHANMASK(*chan));
			continue;
		}
		try
		{
			channel = _chans.at(*chan);
			if (channel->isInviteOnly() && !channel->userIsInvitee(user))
			{
				user->sendReply(ERR_INVITEONLYCHAN(channel->getName()));
				continue;
			}
			if (channel->isFull())
			{
				user->sendReply(ERR_CHANNELISFULL(channel->getName()));
				continue;
			}
			if (channel->userIsBan(user))
			{
				user->sendReply(ERR_BANNEDFROMCHAN(user->getNickname(), channel->getName()));
				continue;
			}
			if (channel->isKeyProtect())
			{
				if (!channel->checkKey(keys[chan - channels.begin()]))
				{
					user->sendReply(ERR_BADCHANNELKEY(channel->getName()));
					continue;
				}
			}
			if (channel->userIsInvitee(user))
				channel->delInvitee(user);
			channel->addUser(user);
			channel->broadcast(user, RPL_JOIN(user->getNickname(), *chan));
			if (channel->getTopic() != "")
			{
				user->sendReply(RPL_TOPIC(user->getNickname(), channel->getName(), channel->getTopic()));
				channel->rpl_topicwhotime(user);
			}
			channel->rpl_namreply(user, true);
		}
		catch (const std::out_of_range &e)
		{
			channel = _createChan(user, *chan, keys[chan - channels.begin()]);
			channel->broadcast(user, RPL_JOIN(user->getNickname(), *chan));
			channel->rpl_namreply(user, true);
		}
	}
}

void	Server::_partCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "PART")));
	std::vector<std::string>	channels = _getChannels(buf);
	Channel						*channel;

	for (std::vector<std::string>::iterator chan = channels.begin(); chan != channels.end(); ++chan)
	{
		try
		{
			channel = _chans.at(*chan);
			if (channel->userIsIn(user))
			{
				channel->broadcast(user, RPL_PART(user->getNickname(), *chan));
				user->delChan(channel);
				if (channel->getUserCount() == 0)
					_delChannel(channel);
			}
			else
				user->sendReply(ERR_NOTONCHANNEL(user->getNickname(), *chan));
		}
		catch (const std::out_of_range &e)
		{
			user->sendReply(ERR_NOSUCHCHANNEL(*chan));
		}
	}
}

void	Server::_modeCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
		user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE"));
	else if (buf[0] == '#' || buf[0] == '&')
		_channelModeCmd(user, buf);
	else
		_userModeCmd(user, buf);
}

void	Server::_topicCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "TOPIC")));
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
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(user->getNickname(), chan_name)));
		if (!chan->userIsIn(user))
			return (user->sendReply(ERR_NOTONCHANNEL(user->getNickname(), chan_name)));
		chan->setTopic(topic, user->getNickname());
		return (chan->broadcast(user, RPL_TOPIC(user->getNickname(), chan_name, chan->getTopic())));
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOTONCHANNEL(user->getNickname(), chan_name));
	}
}

void	Server::_namesCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
	{
		for (chans_iterator it = _chans.begin(); it != _chans.end(); ++it)
		{
			if (!it->second->userIsIn(user) && (it->second->isPrivate() || it->second->isSecret()))
				continue;
			it->second->rpl_namreply(user, false);
		}
		std::string	nicks;
		for (users_iterator it = _users.begin(); it != _users.end(); ++it)
		{
			if (it->second->isVisible() && (!it->second->isInVisibleChannel() || !it->second->isInChan()))
				nicks += it->second->getNickname() + " ";
		}
		if (nicks.size())
			nicks.erase(nicks.size() - 1, 1);
		user->sendReply(RPL_NAMREPLY(user->getNickname(), "=", "*", nicks));
		user->sendReply(RPL_ENDOFNAMES(user->getNickname(), "*"));
	}
	else
	{
		try
		{
			Channel	*channel = _chans.at(buf);

			if (!channel->userIsIn(user) && (channel->isPrivate() || channel->isSecret()))
				return (user->sendReply(RPL_ENDOFNAMES(user->getNickname(), buf)));
			channel->rpl_namreply(user, true);
		}
		catch (const std::out_of_range &e)
		{
			user->sendReply(RPL_ENDOFNAMES(user->getNickname(), buf));
		}
	}
}

void	Server::_listCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	Channel	*channel;
	std::stringstream	ss;
	std::string			count;

	user->sendReply(RPL_LISTSTART(user->getNickname()));
	if (buf.empty())
	{
		for (chans_iterator it = _chans.begin(); it != _chans.end(); ++it)
		{
			channel = it->second;
			count = "";
			ss << channel->getUserCount();
			ss >> count;
			ss.clear();
			if (channel->isSecret() && !channel->userIsIn(user))
				continue;
			else if (channel->isPrivate() && !channel->userIsIn(user))
				user->sendReply(RPL_LIST(user->getNickname(), channel->getName(), count, "Prv"));
			else
				user->sendReply(RPL_LIST(user->getNickname(), channel->getName(), count, channel->getTopic()));
		}
	}
	else
	{
		std::vector<std::string>	channels;

		channels = _getChannels(buf);
		for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			try
			{
				channel = _chans.at(*it);
				count = "";
				ss << channel->getUserCount();
				ss >> count;
				ss.clear();
				if (channel->isPrivate() && !channel->userIsIn(user))
					user->sendReply(RPL_LIST(user->getNickname(), channel->getName(), count, "Prv"));
				else if (channel->isSecret() && !channel->userIsIn(user))
					continue;
				else
					user->sendReply(RPL_LIST(user->getNickname(), channel->getName(), count, channel->getTopic()));

			}
			catch (const std::out_of_range &e) {}
		}
	}
	user->sendReply(RPL_LISTEND(user->getNickname()));
}

void	Server::_inviteCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty() || buf.find(' ') == std::string::npos)
		user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "INVITE"));
	std::string	target_name = buf.substr(0, buf.find(' '));
	buf = buf.substr(buf.find(' ') + 1);
	std::string	channel_name = buf.substr(0, buf.find(' '));
	User	*target = _getUserByNick(target_name);

	if (!target)
		return (user->sendReply(ERR_NOSUCHNICK(user->getNickname(), target_name)));
	try
	{
		Channel	*channel = _chans.at(channel_name);

		if (!channel->userIsIn(user))
			return (user->sendReply(ERR_NOTONCHANNEL(user->getNickname(), channel_name)));
		if (channel->userIsIn(target))
			return (user->sendReply(ERR_USERONCHANNEL(user->getNickname(), target_name, channel_name)));
		if (!channel->userIsOperator(user))
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(user->getNickname(), channel_name)));
		user->sendReply(RPL_INVITING(user->getNickname(), target_name, channel_name));
		target->sendReply(RPL_INVITE(target_name, user->getNickname(), channel_name));
		channel->addInvitee(target);
	}
	catch (const std::out_of_range &e)
	{
	}
}

void	Server::_kickCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.find(' ') == std::string::npos)	
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "KICK")));
	std::string	channel_name = buf.substr(0, buf.find(' '));

	buf = buf.substr(buf.find(' ') + 1);
	try
	{
		Channel	*channel = _chans.at(channel_name);
		if (!channel->userIsIn(user))
			return (user->sendReply(ERR_NOTONCHANNEL(user->getNickname(), channel_name)));
		std::string	target_name = buf.substr(0, buf.find(' '));
		buf = buf.substr(buf.find(' ') + 1);

		User	*target = _getUserByNick(target_name);
		if (!target)
			return (user->sendReply(ERR_USERNOTINCHANNEL(user->getNickname(), target_name, channel_name)));
		if (!channel->userIsIn(target))
			return (user->sendReply(ERR_USERNOTINCHANNEL(user->getNickname(), target_name, channel_name)));
		if (!channel->userIsOperator(user))
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(user->getNickname(), channel_name)));
		channel->broadcast(user, RPL_KICK(user->getNickname(), target_name, channel_name, buf));
		target->delChan(channel);
		if (channel->getUserCount() == 0)
			_delChannel(channel);
	}
	catch (const std::out_of_range &e)
	{
		return (user->sendReply(ERR_NOSUCHCHANNEL(channel_name)));
	}
}

void	Server::_privmsgCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.find(':') == std::string::npos)
		return (user->sendReply(ERR_NOTEXTTOSEND(user->getNickname())));
	if (buf.find(':') == 0)
		return (user->sendReply(ERR_NORECIPIENT(user->getNickname(), "PRIVMSG")));
	std::string	msg = buf.substr(buf.find(':') + 1);
	std::string dest = buf.substr(0, buf.find(' '));
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
	if (buf.find(':') == 0)
		return;
	std::string	msg = buf.substr(buf.find(':') + 1);
	std::string dest = buf.substr(0, buf.find(' '));
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

void	Server::_whoCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	buf = buf.substr(0, buf.find(' '));
	try
	{
		Channel	*channel = _chans.at(buf);

		if ((!channel->isPrivate() && !channel->isSecret())
			|| (channel->isPrivate() && channel->userIsIn(user))
			|| (channel->isSecret() && channel->userIsIn(user)))
		{
			channel->rpl_whoreply(user);
		}
	}
	catch (const std::out_of_range &e)
	{
		User	*who = _getUserByNick(buf);

		if (who && (who->isVisible() || who == user))
			user->sendReply(RPL_WHOREPLY(user->getNickname(), "*", who->getUser(), who->getHostname(), who->getServer(), who->getNickname(), who->getRealname()));
	}
	user->sendReply(RPL_ENDOFWHO(user->getNickname(), buf));
}

void	Server::_pingCmd(User *user, std::string buf)
{
	if (!user->hasBeenWelcomed())
		return;
	if (buf.empty())
		return (user->sendReply(ERR_NOORIGIN()));
	if (buf != _host && buf != "IRC")
		return (user->sendReply(ERR_NOSUCHSERVER(buf)));
	user->sendReply(RPL_PONG(user->getNickname(), _host));
}

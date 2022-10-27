#include "Server.hpp"

void	Server::_createModeOption(void)
{
	_chanModeOption.insert(std::make_pair('o', &Server::_chanModeO));
	_chanModeOption.insert(std::make_pair('p', &Server::_chanModeP));
	_chanModeOption.insert(std::make_pair('s', &Server::_chanModeS));
	_chanModeOption.insert(std::make_pair('i', &Server::_chanModeI));
	_chanModeOption.insert(std::make_pair('t', &Server::_chanModeT));
	_chanModeOption.insert(std::make_pair('n', &Server::_chanModeN));
	_chanModeOption.insert(std::make_pair('m', &Server::_chanModeM));
	_chanModeOption.insert(std::make_pair('l', &Server::_chanModeL));
	_chanModeOption.insert(std::make_pair('b', &Server::_chanModeB));
	_chanModeOption.insert(std::make_pair('v', &Server::_chanModeV));
	_chanModeOption.insert(std::make_pair('k', &Server::_chanModeK));
}

void	Server::_channelModeCmd(User *user, std::string buf)
{
	std::string	chan_name;

	if (buf.find(' ') == std::string::npos)
		chan_name = buf;
	else
		chan_name = buf.substr(0, buf.find(' '));
	try
	{
		Channel	*channel = _chans.at(chan_name);
		if (chan_name == buf)
			return (channel->rpl_channelmodeis(user));
		buf = buf.substr(buf.find(' ') + 1);
		if (buf[0] == 'b')
			return(channel->rpl_banlist(user));
		if (!channel->userIsOperator(user))
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(user->getNickname(), chan_name)));
		if (buf[0] != '+' && buf[0] != '-')
			return (user->sendReply(ERR_UMODEUNKNOWNFLAG()));
		size_t	i = 0;
		std::string	mode;
		std::string	str;

		if (buf.find(' ') == std::string::npos)
			mode = buf;
		else
			mode = buf.substr(0, buf.find(' '));
		while (mode[++i])
		{
			try
			{
				(this->*_chanModeOption.at(mode[i]))(mode[0], channel, user, buf);
			}
			catch (const std::out_of_range &e)
			{
				user->sendReply(ERR_UNKNOWNMODE((str = buf[i])));
			}
		}
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOSUCHCHANNEL(chan_name));
	}
}

void	Server::_chanModeO(char sign, Channel *channel, User *user, std::string buf)
{
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE")));
	std::string	nick = buf.substr(buf.find(' ') + 1);
	User		*target = _getUserByNick(nick);

	if (!channel->userIsIn(target))
		return (user->sendReply(ERR_NOSUCHNICK(user->getNickname(), nick)));
	if (sign == '+' && !channel->userIsOperator(target))
	{
		channel->addOperator(target);
		channel->broadcast(user, RPL_CHANNELMODE(user->getNickname(), channel->getName(), buf));
	}
	else if (sign == '-' && channel->userIsOperator(target))
	{
		channel->delOperator(target);
		channel->broadcast(user, RPL_CHANNELMODE(user->getNickname(), channel->getName(), buf));
	}
}

void	Server::_chanModeP(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('p');
	if (sign == '-')
		channel->delMode('p');
}

void	Server::_chanModeS(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('s');
	if (sign == '-')
		channel->delMode('s');
}

void	Server::_chanModeI(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('i');
	if (sign == '-')
		channel->delMode('i');
}

void	Server::_chanModeT(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('t');
	if (sign == '-')
		channel->delMode('t');
}

void	Server::_chanModeN(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('n');
	if (sign == '-')
		channel->delMode('n');
}

void	Server::_chanModeM(char sign, Channel *channel, User *user, std::string buf)
{
	(void)user;
	(void)buf;
	if (sign == '+')
		channel->addMode('m');
	if (sign == '-')
		channel->delMode('m');
}

void	Server::_chanModeL(char sign, Channel *channel, User *user, std::string buf)
{
	std::stringstream	ss;

	std::cout << "_chanModeL buf <" << buf << ">" << std::endl;
	if (sign == '-')
	{
		channel->setLimit(-1);
		channel->delMode('l');
	}
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE")));
	size_t	limit;

	buf = buf.substr(buf.find_first_not_of(' '));
	ss << buf.substr(0, buf.find(' '));
	ss >> limit;
	if (sign == '+')
	{
		channel->setLimit(limit);
		channel->addMode('l');
	}
	std::cout << "channel mode : <" << channel->getMode() << ">" << std::endl;
}

void	Server::_chanModeB(char sign, Channel *channel, User *user, std::string buf)
{
	if (buf.find(' ') == std::string::npos)
		return (channel->rpl_banlist(user));
	std::string	nick = buf.substr(buf.find_last_of(' ') + 1);
	User*		target = _getUserByNick(nick);

	if (target == _users.end()->second)
		return (user->sendReply(ERR_NOSUCHNICK(user->getNickname(), nick)));
	if (sign == '-')
		channel->delBan(target);
	if (sign == '+')
	{
		channel->addBan(target);
		if (channel->userIsIn(target))
		{
			channel->broadcast(user, RPL_KICK(user->getNickname(), nick, channel->getName(), "banned"));
			channel->delUser(target);
			if (channel->getUserCount() == 0)
				_delChannel(channel);
		}
	}
}

void	Server::_chanModeV(char sign, Channel *channel, User *user, std::string buf)
{
	std::string	nick;

	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE +k")));
	nick = nick.substr(nick.find(' ') + 1);

	User*		target = _getUserByNick(nick);

	if (target == _users.end()->second)
		return (user->sendReply(ERR_NOSUCHNICK(user->getNickname(), nick)));
	if (sign == '-')
		channel->delModerate(target);
	if (sign == '+')
		channel->addModerate(target);
}

void	Server::_chanModeK(char sign, Channel *channel, User *user, std::string buf)
{
	std::string	key;

	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE +k")));
	key = buf.substr(buf.find(' ') + 1);
	if (sign == '+')
	{
		channel->addMode('k');
		channel->setKey(key);
	}
	if (sign == '-')
		channel->delMode('k');
}

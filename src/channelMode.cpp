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
			return (user->sendReply(RPL_CHANNELMODEIS(chan_name, channel->getMode(), "")));
		if (!channel->userIsOperator(user))
			return (user->sendReply(ERR_CHANOPRIVSNEEDED(chan_name)));
		buf = buf.substr(buf.find(' ') + 1);
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
		channel->showOperators();
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
		return (user->sendReply(ERR_NOSUCHNICK(nick)));
	if (sign == '+' && !channel->userIsOperator(target))
	{
		channel->addOperator(target);
		channel->broadcast(user, RPL_CHANNELMODE(user->getNickname(), channel->getName(), buf), 0);
	}
	else if (sign == '-' && channel->userIsOperator(target))
	{
		channel->delOperator(target);
		channel->broadcast(user, RPL_CHANNELMODE(user->getNickname(), channel->getName(), buf), 0);
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
	if (sign == '-')
		channel->setLimit(-1);
	if (buf.find(' ') == std::string::npos)
		return (user->sendReply(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE")));
	size_t	limit;
	limit = stol(buf.substr(buf.find(' ') + 1));
	if (sign == '+')
		channel->setLimit(limit);
}

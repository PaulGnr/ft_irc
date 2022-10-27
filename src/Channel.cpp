#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key) : _name(name), _key(key), _limit(-1)
{
	if (key != "")
		_mode.append(1, 'k');
}

Channel::~Channel(void)
{}

std::string	Channel::getName(void) {return (_name);}
std::string	Channel::getTopic(void) {return (_topic);}
int			Channel::getUserCount(void) {return (_users.size());}
std::string	Channel::getMode(void) {return (_mode);}

void	Channel::setKey(std::string key) {_key = key;}
void	Channel::setLimit(size_t limit) {_limit = limit;}
void	Channel::setTopic(std::string topic, std::string nick)
{
	std::stringstream	ss;

	_topic = topic;
	_topicSetter = nick;
	ss << time(NULL);
	ss >> _topicTime;
}

void	Channel::addUser(User *user)
{
	_users.insert(std::make_pair(user->getFd(), user));
	user->addChan(this);
}

void	Channel::delUser(User *user)
{
	_users.erase(user->getFd());
	_operators.erase(user->getFd());
	_moderate.erase(user->getFd());
}

void	Channel::addOperator(User *user)
{
	_operators.insert(std::make_pair(user->getFd(), user));
}

void	Channel::delOperator(User *user)
{
	_operators.erase(user->getFd());
}

void	Channel::addBan(User *user)
{
	_ban.insert(std::make_pair(user->getFd(), user));
}

void	Channel::delBan(User *user)
{
	_ban.erase(user->getFd());
}

void	Channel::addModerate(User *user)
{
	_moderate.insert(std::make_pair(user->getFd(), user));
}

void	Channel::delModerate(User *user)
{
	_moderate.erase(user->getFd());
}

void	Channel::addInvitee(User *user)
{
	_invitee.insert(std::make_pair(user->getFd(), user));
}

void	Channel::delInvitee(User *user)
{
	_invitee.erase(user->getFd());
}

bool	Channel::userIsIn(User *user)
{
	try
	{
		_users.at(user->getFd());
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
	return (true);
}

bool	Channel::userIsOperator(User *user)
{
	try
	{
		_operators.at(user->getFd());
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
	return (true);
}

bool	Channel::userIsBan(User *user)
{
	try
	{
		_ban.at(user->getFd());
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
	return (true);
}

bool	Channel::userIsModerate(User *user)
{
	try
	{
		_moderate.at(user->getFd());
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
	return (true);
}

bool	Channel::userIsInvitee(User *user)
{
	try
	{
		_invitee.at(user->getFd());
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
	return (true);
}

bool	Channel::isEmpty()
{
	if (_users.size())
		return (false);
	return (true);
}

void	Channel::addMode(char c)
{
	if (_mode.find(c) == std::string::npos)
		_mode.append(1, c);
}

void	Channel::delMode(char c)
{
	if (_mode.find(c) != std::string::npos)
		_mode.erase(_mode.find(c));
}

bool	Channel::wrongMode(char c)
{
	if (c == 'o' || c == 'p' || c == 's' || c == 'i' || c == 't' || c == 'n' || c == 'b' || c == 'v')
		return (false);
	return (true);
}

bool	Channel::hasMode(char c)
{
	if (_mode.find(c) != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::isPrivate(void)
{
	if (_mode.find('p') != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::isSecret(void)
{
	if (_mode.find('s') != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::isInviteOnly(void)
{
	if (_mode.find('i') != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::isNoOutside(void)
{
	if (_mode.find('n') != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::isFull(void)
{
	if (_limit == (size_t)-1)
		return (false);
	if (_users.size() < _limit)
		return (false);
	return (true);
}

bool	Channel::isKeyProtect(void)
{
	if (_mode.find('k') != std::string::npos)
		return (true);
	return (false);
}

bool	Channel::checkKey(std::string key)
{
	if (_key == key)
		return (true);
	return (false);
}

void	Channel::rpl_topicwhotime(User *user)
{
	user->sendReply(RPL_TOPICWHOTIME(user->getNickname(), _name, _topicSetter, _topicTime));
}

void	Channel::rpl_whoreply(User *user)
{
	User	*who;
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		who = it->second;
		if (!who->isVisible())
			break;
		user->sendReply(RPL_WHOREPLY(user->getNickname(), _name, who->getUser(), who->getHostname(), who->getServer(), who->getNickname(), who->getRealname()));
	}
}

void	Channel::rpl_namreply(User *user, bool endList)
{
	std::string	nicks;
	std::string	symbol;

	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (!it->second->isVisible())
			continue;
		if (_operators.find(it->first) != _operators.end())
			nicks += "@";
		if (_moderate.find(it->first) != _moderate.end())
			nicks += "+";
		nicks += it->second->getNickname() + " ";
	}
	if (nicks.size())
		nicks.erase(nicks.size() - 1, 1);
	if (_mode.find('s') != std::string::npos)
		symbol = "@";
	else if (_mode.find('p') != std::string::npos)
		symbol = "*";
	else
		symbol = "=";
	user->sendReply(RPL_NAMREPLY(user->getNickname(), symbol, _name, nicks));
	if (endList)
		user->sendReply(RPL_ENDOFNAMES(user->getNickname(), _name));
}

void	Channel::rpl_banlist(User *user)
{
	for (std::map<int, User *>::iterator it = _ban.begin(); it != _ban.end(); ++it)
	{
		user->sendReply(RPL_BANLIST(user->getNickname(), _name, it->second->getNickname()));
	}
	user->sendReply(RPL_ENDOFBANLIST(user->getNickname(), _name));
}

void	Channel::rpl_channelmodeis(User *user)
{
	std::string			modes;
	std::string			params;
	std::stringstream	ss;

	for (size_t i = 0; i < _mode.size(); ++i)
	{
		if (i > 0)
			modes += " ";
		modes += '+';
		modes += _mode[i];
		if (_mode[i] == 'l')
		{
			params = "";
			ss << _limit;
			ss >> params;
			ss.clear();
			modes += " " + params;
		}
	}
	user->sendReply(RPL_CHANNELMODEIS(user->getNickname(), _name, modes));
}

void	Channel::broadcast(User *user, std::string msg)
{
	(void)user;
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		it->second->sendReply(msg);
	}
}

void	Channel::privmsg(User *user, std::string msg)
{
	std::string	nick = user->getNickname();

	if (_mode.find('n') != std::string::npos)
	{
		try
		{
			user->sendReply(ERR_CANNOTSENDTOCHAN(_name));
			_users.at(user->getFd());
		}
		catch (const std::out_of_range &e)
		{
			return;
		}
	}
	if (_mode.find('m') != std::string::npos && !userIsOperator(user) && !userIsModerate(user))
		return (user->sendReply(ERR_CANNOTSENDTOCHAN(_name)));
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second != user)
			it->second->sendReply(RPL_PRIVMSG(nick, _name, msg));
	}
}

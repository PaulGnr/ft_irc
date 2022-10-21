#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key) : _name(name), _key(key), _limit(-1)
{
	if (key != "")
		_mode.append(1, 'k');
}

Channel::~Channel(void)
{}

std::string	Channel::getName(void) {return (_name);}
int			Channel::getUserCount(void) {return (_users.size());}
std::string	Channel::getMode(void) {return (_mode);}

void	Channel::setKey(std::string key) {_key = key;}
void	Channel::setLimit(size_t limit) {_limit = limit;}

void	Channel::addUser(User *user)
{
	_users.insert(std::make_pair(user->getFd(), user));
	user->addChan(this);
}

void	Channel::delUser(User *user)
{
	_users.erase(user->getFd());
	_operators.erase(user->getFd());
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

void	Channel::showOperators(void)
{
	std::cout << "Operators :" << std::endl;
	for (users_iterator it = _operators.begin(); it != _operators.end(); ++it)
		std::cout << it->second->getNickname() << std::endl;
}

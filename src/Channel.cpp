#include "Channel.hpp"

std::string	Channel::getName(void) {return (_name);}
std::string	Channel::getKey(void) {return (_key);}
User		*Channel::getAdmin(void) {return (_admin);}
std::string	Channel::getMode(void) {return (_mode);}

Channel::Channel(const std::string &name, const std::string &key) : _name(name), _key(key)
{}

Channel::~Channel(void)
{}

void	Channel::setAdmin(User *user)
{
	_admin = user;
	_operators.push_back(user);
}

void	Channel::setMode(std::string mode)
{
	_mode = mode;
}

void	Channel::addUser(User *user)
{
	this->_users.insert(std::pair<std::string, User *>(user->getNickname(), user));
}

bool	Channel::userIsIn(User *user)
{
	try
	{
		_users.at(user->getNickname());
		return (true);
	}
	catch (const std::out_of_range &e)
	{
		return (false);
	}
}

void	Channel::broadcast(User *user, std::string msg, bool priv)
{
	std::string	nick = user->getNickname();

	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (priv && user->getFd() != it->second->getFd())
			it->second->sendReply(RPL_PRIVMSG(nick, _name, msg));
		else
			it->second->sendReply(msg);
	}
}

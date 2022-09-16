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
	std::cout << user->getNickname() << " is the new admin of " << _name << std::endl;
	_admin = user;
	_operators.push_back(user);
}

void	Channel::setMode(std::string mode)
{
	_mode = mode;
}

void	Channel::addUser(int fd, User *user)
{
	this->_users.insert(std::pair<int, User *>(fd, user));
}

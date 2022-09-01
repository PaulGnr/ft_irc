#include "Channel.hpp"

std::string	Channel::getName(void) {return (_name);}
std::string	Channel::getKey(void) {return (_key);}

void	Channel::setAdmin(User *user)
{
	std::cout << user->getNickname() << " is the new admin of " << _name << std::endl;
	_admin = user;
}

Channel::Channel(const std::string &name, const std::string &key) : _name(name), _key(key)
{}

Channel::~Channel(void)
{}

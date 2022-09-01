#include "Server.hpp"

int		Server::_chanExists(std::string name)
{
	for (chans_iterator it = _chans.begin(); it != _chans.end(); it++)
	{
		std::cout << "debug" << std::endl;
		std::cout << "checking: " << it->second->getName() << " == " << name << std::endl;
		if (it->second->getName() == name)
			return (true);
	}
	return (false);
}

void		Server::_createChan(User *user, std::string name)
{
	Channel	*chan = new Channel(name, "");

	std::cout << "New channel " << name << " created."  << std::endl;
	chan->setAdmin(user);
	this->_addChannel(chan);
}

void	Server::_addChannel(Channel *chan) {
	this->_chans.insert(std::pair<std::string, Channel *>(chan->getName(), chan));
}

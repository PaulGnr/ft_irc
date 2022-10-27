#include "Server.hpp"

int		Server::_chanExists(std::string name)
{
	for (chans_iterator it = _chans.begin(); it != _chans.end(); ++it)
	{
		if (it->second->getName() == name)
			return (true);
	}
	return (false);
}

Channel		*Server::_createChan(User *user, std::string name, std::string key)
{
	Channel	*chan = new Channel(name, key);

	std::cout << "New channel " << name << " created."  << std::endl;
	chan->addUser(user);
	chan->addOperator(user);
	_addChannel(chan);
	return (chan);
}

void	Server::_addChannel(Channel *chan) {
	this->_chans.insert(std::pair<std::string, Channel *>(chan->getName(), chan));
}

void	Server::_delChannel(Channel *chan) {
	std::cout << "Channel " << chan->getName() << " deleted." << std::endl;
	_chans.erase(chan->getName());
	delete chan;
}

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

void		Server::_createChan(User *user, std::string name, std::string key)
{
	Channel	*chan = new Channel(name, key);

	std::cout << "New channel " << name << " created."  << std::endl;
	chan->setAdmin(user);
	chan->addUser(user->getFd(), user);
	this->_addChannel(chan);
}

void	Server::_addChannel(Channel *chan) {
	this->_chans.insert(std::pair<std::string, Channel *>(chan->getName(), chan));
}

std::vector<std::string>	Server::_getChannels(std::string buf)
{
	std::vector<std::string>	channels;
	std::string					chan;

	if (buf.find(' ') != std::string::npos)
		buf = buf.substr(0, buf.find(' '));
	while (buf.find(',') != std::string::npos)
	{
		chan = buf.substr(0, buf.find(','));
		buf = buf.substr(buf.find(',') + 1);
		channels.push_back(chan);
	}
	channels.push_back(buf);
	return (channels);
}

std::vector<std::string>	Server::_getKeys(std::string buf, size_t size)
{
	std::vector<std::string>	keys(size);
	std::string					key;
	size_t						i = 0;

	std::cout << "getKeys : <" << buf << ">" << std::endl;
	if (buf.find(' ') != std::string::npos)
	{
		buf = buf.substr(buf.find(' ') + 1);
		if (buf.find(' ') != std::string::npos)
			buf = buf.substr(0, buf.find(' '));
	}
	else
		buf = "";
	while (buf.find(',') != std::string::npos)
	{
		key = buf.substr(0, buf.find(','));
		buf = buf.substr(buf.find(',') + 1);
		keys[i++] = key;
	}
	keys[i] = buf;
	return (keys);
}

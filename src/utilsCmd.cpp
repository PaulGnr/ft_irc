#include "ft_irc.hpp"

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
	if (!buf.empty() && buf != "#")
		channels.push_back(buf);
	return (channels);
}

std::vector<std::string>	Server::_getKeys(std::string buf, size_t size)
{
	std::vector<std::string>	keys(size);
	std::string					key;
	size_t						i = 0;

	if (buf.find(' ') != std::string::npos)
	{
		buf = buf.substr(buf.find(' ') + 1);
		if (buf.find(' ') != std::string::npos)
			buf = buf.substr(0, buf.find(' '));
	}
	else
		buf = "";
	while (buf.find(',') != std::string::npos && i + 1 < size)
	{
		key = buf.substr(0, buf.find(','));
		buf = buf.substr(buf.find(',') + 1);
		keys[i++] = key;
	}
	keys[i] = buf;
	return (keys);
}

void	Server::_msgToUser(User *user, std::string dest, std::string msg)
{
	std::string	nick = user->getNickname();

	if (dest == "botIrma")
		return (_callIrma(user, msg));
	for (users_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == dest)
			return (it->second->sendReply(RPL_PRIVMSG(nick, dest, msg)));
	}
	user->sendReply(ERR_NOSUCHNICK(user->getNickname(), dest));
}

void	Server::_msgToChannel(User *user, std::string dest, std::string msg)
{
	try
	{
		Channel	*channel = _chans.at(dest);

		if (!channel->userIsIn(user) && channel->isNoOutside())
			return (user->sendReply(ERR_CANNOTSENDTOCHAN(dest)));
		if (msg.find("botIrma") == 0)
			return (_channelCallIrma(channel, user, msg));
		channel->privmsg(user, msg);
	}
	catch (const std::out_of_range &e)
	{
		user->sendReply(ERR_NOSUCHCHANNEL(dest));
	}
}

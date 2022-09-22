#include "Server.hpp"

void	Server::_userModeCmd(User *user, std::string buf)
{
	std::string		nick = buf.substr(0, buf.find(' '));
	users_iterator	it = _users.begin();

	while (it != _users.end())
	{
		if (it->second->getNickname() == nick)
			break;
		++it;
	}
	if (it == _users.end())
	{
		user->sendReply(ERR_NOSUCHNICK(nick));
		return;
	}
	if (it->second != user)
	{
		user->sendReply(ERR_USERSDONTMATCH());
		return;
	}
	buf = buf.substr(buf.find(' ') + 1);
	if (buf[0] != '+' && buf[0] != '-')
	{
		user->sendReply(ERR_UMODEUNKNOWNFLAG());
		return;
	}
	size_t	i = 0;
	std::string	str;

	while (buf[++i])
	{
		if (user->wrongMode(buf[i]))
			user->sendReply(ERR_UNKNOWNMODE((str = buf[i])));
		else if (buf[0] == '+' && user->mode.find(buf[i]) == std::string::npos)
			user->mode.push_back(buf[i]);
		else if (buf[0] == '-' && user->mode.find(buf[i]) != std::string::npos)
			user->mode.erase(user->mode.find(buf[i]));
	}
	std::cout << nick << "->mode : <" << user->mode << ">" << std::endl;
}

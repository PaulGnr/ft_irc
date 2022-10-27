#include "Server.hpp"

void	Server::_userModeCmd(User *user, std::string buf)
{
	std::string	nick;

	if (buf.find(' ') == std::string::npos)
	{
		nick = buf;
		buf.clear();
	}
	else
	{
		nick = buf.substr(0, buf.find(' '));
		buf = buf.substr(buf.find(' ') + 1);
	}
	User*	target = _getUserByNick(nick);

	if (target == NULL)
		return (user->sendReply(ERR_NOSUCHNICK(user->getNickname(), nick)));
	if (target != user)
		return (user->sendReply(ERR_USERSDONTMATCH()));
	if (buf.empty())
		return (user->sendReply(RPL_UMODEIS(nick, user->getMode())));
	if (buf[0] != '+' && buf[0] != '-')
		return (user->sendReply(ERR_UMODEUNKNOWNFLAG()));
	size_t		i = 0;
	std::string	str;

	while (buf[++i])
	{
		if (user->wrongMode(buf[i]))
			user->sendReply(ERR_UNKNOWNMODE((str = buf[i])));
		else if (buf[0] == '+' && buf[i] != 'o')
			user->addMode(buf[i]);
		else if (buf[0] == '-')
			user->delMode(buf[i]);
	}
}

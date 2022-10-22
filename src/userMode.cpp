#include "Server.hpp"

void	Server::_userModeCmd(User *user, std::string buf)
{
	std::string	nick = buf.substr(0, buf.find(' '));
	User*		target = _getUserByNick(nick);

	if (target == NULL)
		return (user->sendReply(ERR_NOSUCHNICK(nick)));
	if (target != user)
		return (user->sendReply(ERR_USERSDONTMATCH()));
	buf = buf.substr(buf.find(' ') + 1);
	if (buf[0] != '+' && buf[0] != '-')
		return (user->sendReply(ERR_UMODEUNKNOWNFLAG()));
	size_t		i = 0;
	std::string	str;

	while (buf[++i])
	{
		if (user->wrongMode(buf[i]))
			user->sendReply(ERR_UNKNOWNMODE((str = buf[i])));
		else if (buf[0] == '+')
			user->addMode(buf[i]);
		else if (buf[0] == '-')
			user->delMode(buf[i]);
	}
	std::cout << nick << "->mode : <" << user->getMode() << ">" << std::endl;
}

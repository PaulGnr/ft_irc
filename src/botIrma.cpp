#include "ft_irc.hpp"
void	Server::_callIrma(User *user, std::string msg)
{
	std::string	irma = "botIrma";

	msg = msg.substr(0, msg.find_last_not_of(' ') + 1);
	if (msg[msg.size() - 1] != '?')
		return (user->sendReply(":IRC :" + user->getNickname() + " :I see no question in your message"));
	std::srand(std::time(NULL));
	int	randVar = std::rand();

	if (randVar % 3 == 0)
		return (user->sendReply(RPL_PRIVMSG(irma, user->getNickname(), "Yes, of course!")));
	if (randVar % 3 == 1)
		return (user->sendReply(RPL_PRIVMSG(irma, user->getNickname(), "Absolutely not.")));
	if (randVar % 3 == 2)
		return (user->sendReply(RPL_PRIVMSG(irma, user->getNickname(), "Maybe...")));
}

void	Server::_channelCallIrma(Channel *channel, User *user, std::string msg)
{
	std::string	irma = "botIrma";

	msg = msg.substr(0, msg.find_last_not_of(' ') + 1);
	if (msg[msg.size() - 1] != '?')
		return (user->sendReply(":IRC :" + user->getNickname() + " :I see no question in your message"));
	std::srand(std::time(NULL));
	int	randVar = std::rand();

	if (randVar % 3 == 0)
		return (user->sendReply(RPL_PRIVMSG(irma, channel->getName(), "Yes, of course!")));
	if (randVar % 3 == 1)
		return (user->sendReply(RPL_PRIVMSG(irma, channel->getName(), "Absolutely not.")));
	if (randVar % 3 == 2)
		return (user->sendReply(RPL_PRIVMSG(irma, channel->getName(), "Maybe...")));
}

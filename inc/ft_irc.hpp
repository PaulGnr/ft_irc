#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <sstream>
#include <ctime>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <functional>
#include <iterator>
#include <netdb.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"

class Server;
class Channel;
class User;

#define	RPL_WELCOME(nick, server, user, hostname)	":" + server + " 001 " + nick + " :Welcome to the " + server + " network, " + nick + "[" + user + "@" + hostname + "]"
	;
#define	RPL_YOURHOST(nick, server)					":" + server + " 002 " + nick + " :Your host is " + server + ", running version 1.2.3"
#define	RPL_CREATED(nick, server, timestamp)		":" + server + " 003 " + nick + " :This server was created " + timestamp
#define	RPL_MYINFO(nick, server)					":" + server + " 004 " + nick + " " + server + " irssi 1.2.3 (20210409 0011)"

#define ERR_NOSUCHNICK(nick)				":IRC 401 :" + nick + " :No such nick/channel"
#define ERR_NOSUCHSERVER(server)			":IRC 402 :" + server + " :No such server"
#define ERR_NOSUCHCHANNEL(channel)			":IRC 403 :" + channel + " :No such channel"
#define ERR_CANNOTSENDTOCHAN(channel)		":IRC 404 :" + channel + " :Cannot send to channel"
#define ERR_TOOMANYTARGETS(target)			":IRC 407 :" + target + " :Duplicate recipients. No message delivered"
#define ERR_NOORIGIN()						":IRC 409 :No origin specified"
#define ERR_NOTEXTTOSEND(nick)				":IRC 412 :" + nick + " :No text to send"
#define ERR_UNKNOWNCOMMAND(nick, command)	":IRC 421 :" + nick + " " + command + " :Unknown command"
#define ERR_NONICKNAMEGIVEN(nick)			":IRC 431 :" + nick + " :No nickname given"
#define ERR_NICKNAMEINUSE(nick)				":IRC 433 :" + nick + " :Nickname is already in use"
#define ERR_NICKCOLLISION(nick)				":IRC 436 :" + nick + " :Nickname collision KILL"
#define ERR_NOTONCHANNEL(channel)			":IRC 442 :" + channel + " :You're not on that channel"
#define ERR_NEEDMOREPARAMS(nick, command)	":IRC 461 :" + nick + " " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED(nick)			":IRC 462 :" + nick + " :You may not reregister"
#define ERR_PASSWDMISMATCH(nick) 			":IRC 464 :" + nick + " :Password incorrect"
#define ERR_CHANNELISFULL(channel) 			":IRC 471 :" + channel + " :Cannot join channel (+l)"
#define ERR_UNKNOWNMODE(mode) 				":IRC 472 :" + mode + " :is unknown mode char to me"
#define ERR_INVITEONLYCHAN(channel) 		":IRC 473 :" + channel + " :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN(channel) 		":IRC 474 :" + channel + " :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY(channel) 			":IRC 475 :" + channel + " :Cannot join channel (+k)"
#define ERR_BADCHANMASK(channel) 			":IRC 476 :" + channel + " :is a bad channel mask"
#define ERR_CHANOPRIVSNEEDED(channel) 		":IRC 482 :" + channel + " :You're not channel operator"
#define ERR_UMODEUNKNOWNFLAG() 				":IRC 501 :Unknown MODE flag"
#define ERR_USERSDONTMATCH() 				":IRC 502 :Cant change mode for other users"

#define	RPL_UMODEIS(nick, mode)	":IRC 221 " + nick + " " + mode
#define	RPL_CHANNELMODEIS(nick, channel, mode, params)	":IRC 324 " + nick + " " + channel + " " + mode + " " + params
#define	RPL_NOTOPIC(nick, channel)	":IRC 331 " + nick + " " + channel + " :No topic is set"
#define	RPL_TOPIC(nick, channel, topic)	":IRC 332 " + nick + " " + channel + " :" + topic
#define	RPL_TOPICWHOTIME(client, channel, nick, setat)	":IRC 333 " + client + " " + channel + " " + nick + " " + setat
#define	RPL_NAMREPLY(client, symbol, channel, nicks)	":IRC 353 " + client + " " + symbol + " " + channel + " :" + nicks
#define	RPL_ENDOFNAMES(client, channel)	":IRC 366 " + client + " " + channel + " :End of /NAMES list"

#define	RPL_CHANNELMODE(nick, channel, mode)	":" + nick + "@IRC MODE " + channel + " :" + mode

#define RPL_JOIN(nick, channel)			":" + nick + "@IRC JOIN :" + channel
#define RPL_PART(nick, channel)			":" + nick + "@IRC PART :" + channel
#define RPL_PONG(nick, server)			":" + nick + "@IRC PONG " + server
#define RPL_PRIVMSG(nick, channel, msg)	":" + nick + "@IRC PRIVMSG " + channel + " :" + msg
#define RPL_QUIT(nick, reason)			":" + nick + "@IRC QUIT :Quit: " + reason

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

std::string	timestamp(void);
//int		get_listener_socket(char **argv);

#endif

#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <sstream>
#include <ctime>
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

#define ERR_NOSUCHNICK(nick)				":server 401 " + nick + " :No such nick/channel"
#define ERR_NOSUCHSERVER(server)			":server 402 " + server + " :No such server"
#define ERR_TOOMANYTARGETS(target)			":server 407 " + target + " :Duplicate recipients. No message delivered"
#define ERR_NOORIGIN()						":server 409 :No origin specified"
#define ERR_NOTEXTTOSEND(nick)				":server 412 " + nick + " :No text to send"
#define ERR_UNKNOWNCOMMAND(nick, command)	":server 421 " + nick + " " + command + " :Unknown command"
#define ERR_NONICKNAMEGIVEN(nick)			":server 431 " + nick + " :No nickname given"
#define ERR_NICKNAMEINUSE(nick)				":server 433 " + nick + " :Nickname is already in use"
#define ERR_NICKCOLLISION(nick)				":server 436 " + nick + " :Nickname collision KILL"
#define ERR_NEEDMOREPARAMS(nick, command)	":server 461 " + nick + " " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED(nick)			":server 462 " + nick + " :You may not reregister"
#define ERR_PASSWDMISMATCH(nick) 			":server 464 " + nick + " :Password incorrect"
#define ERR_UNKNOWNMODE(mode) 				":server 472 " + mode + " :is unknown mode char to me"
#define ERR_BADCHANMASK(channel) 			":server 476 " + channel + " :is a bad channel mask"
#define ERR_CHANOPRIVSNEEDED(channel) 		":server 482 " + channel + " :You're not channel operator"
#define ERR_UMODEUNKNOWNFLAG() 				":server 501 :Unknown MODE flag"
#define ERR_USERSDONTMATCH() 				":server 502 :Cant change mode for otherusers"

#define RPL_QUIT(nick, reason)			": " + nick + " QUIT :Quit: " + reason
#define RPL_PONG(nick, server)			":" + nick + " PONG " + server

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

std::string	timestamp(void);
//int		get_listener_socket(char **argv);

#endif

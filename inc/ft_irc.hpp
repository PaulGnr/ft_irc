#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <netdb.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "Channel.hpp"
#include "Server.hpp"
#include "User.hpp"

class Server;
class User;

#define ERR_UNKNOWNCOMMAND(nick, command)	":server 421 " + nick + " " + command + " :Unknown command"
#define ERR_NONICKNAMEGIVEN(nick)			":server 431 " + nick + " :No nickname given"
#define ERR_NICKNAMEINUSE(nick)				":server 433 " + nick + " :Nickname in use"
#define ERR_NEEDMOREPARAMS(nick, command)	":server 461 " + nick + " " + command + " :Need more parameters"
#define ERR_ALREADYREGISTERED(nick)			":server 462 " + nick + " :Already registered"
#define ERR_PASSWDMISMATCH(nick) 			":server 464 " + nick + " :Password incorrect"

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

void		send_msg(int fd, std::string msg);
std::string	timestamp(void);
void		server_log(std::string msg);
//int		get_listener_socket(char **argv);

#endif

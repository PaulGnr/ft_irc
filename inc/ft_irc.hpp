#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
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

// not used yet
#define RPL_WELCOME(nick, host, user)	"001 " + nick + " :Welcome to the " + host + " network, " + nick + "[" + user + "@" + host + "]\r\n"
#define RPL_YOURHOST(nick)				"002 " + nick + " :Your host is 127.0.0.1, running version 1.2.3\r\n"
#define RPL_CREATED(nick)				"003 " + nick + " :This server was created 18:07:30\r\n"
#define RPL_MYINFO(nick)				"004 " + nick + " localhost irssi 1.2.3 (20210409 0011) 1 2\r\n"

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

//int		get_listener_socket(char **argv);

#endif

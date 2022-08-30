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

#define ERR_PASSWDMISMATCH(nick) ("464 " + nick + " :Password incorrect")
#define ERR_UNKNOWNCOMMAND(source, command)				"421 " + source + " " + command + " :Unknown command"

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

void		send_msg(int fd, std::string msg);
std::string	timestamp(void);
void		server_log(std::string msg);
//int		get_listener_socket(char **argv);

#endif

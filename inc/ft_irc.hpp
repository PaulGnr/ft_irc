#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
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

#define BACKLOG 10
#define DEBUG 0
#define PFDIT std::vector<struct pollfd>::iterator

int		get_listener_socket(char **argv);
void	poll_handler(Server &server);
int		sendall(int dest_fd, const char *buf, int *nbytes);

#endif

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

#define BACKLOG 10
#define DEBUG 0

int		get_listener_socket(char **argv);
void	poll_handler(int sockfd);
int		sendall(int dest_fd, char *buf, int *nbytes);
int		parsing(char *buf);
void	debug(std::string str);

#endif

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include <arpa/inet.h>

#define PFDIT std::vector<struct pollfd>::iterator

class User;

class Server
{
	typedef std::vector<pollfd>::iterator pfds_iterator;

	public:
		Server(std::string port, std::string password);
		~Server();

		std::vector<struct pollfd>	&getPfds(void);
		std::vector<User *>			&getUsers(void);
		int							getPort(void) const;
		std::string					getPassword(void) const;
		int							getListener(void) const;

		void	poll_handler(void);
		void	addUser(int fd, struct sockaddr_storage &addr);
		void	delUser(int i);

	private:
		int							_listener;
		int							_port;
		std::string					_password;
		std::string					_host;
		std::vector<struct pollfd>	_pfds;
		std::vector<User *>			_users;

		void	_createListener(void);
		void*	_get_in_addr(struct sockaddr *sa);
		void	_sendMsg(User *user, int sender_fd);
		int		_sendall(int dest_fd, const char *buf, int *nbytes);
};

#endif

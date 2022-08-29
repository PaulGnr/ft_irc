#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include <arpa/inet.h>
#include <map>
#include <string>

class User;

class Server
{
	typedef std::vector<pollfd>::iterator pfds_iterator;

	public:
		Server(std::string port, std::string password);
		~Server();

		std::vector<struct pollfd>	&getPfds(void);
		std::map<int, User *>		&getUsers(void);
		std::string					getPort(void) const;
		std::string					getPassword(void) const;
		int							getListener(void) const;

		void	poll_handler(void);
		void	addUser(int fd, struct sockaddr_storage &addr);
		void	delUser(pfds_iterator &it);

	private:
		int							_listener;
		std::string					_port;
		std::string					_password;
		std::string					_host;
		std::vector<struct pollfd>	_pfds;
		std::map<int, User *>		_users;

		void	_createListener(void);
		void	_clientConnect(void);
		void	_clientDisconnect(void);
		void	_clientMessage(pfds_iterator &it);
		void*	_get_in_addr(struct sockaddr *sa);
		void	_sendMsg(User *user, int sender_fd);
		int		_sendall(int dest_fd, const char *buf, int *nbytes);
};

#endif

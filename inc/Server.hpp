#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include <arpa/inet.h>

#define PFDIT std::vector<struct pollfd>::iterator

class User;
 
class Server
{
	public:
		Server(std::string port, std::string password);
		~Server();

		std::vector<struct pollfd>	&getPfds(void);
		std::vector<User *>			&getUsers(void);
		std::string					getPort(void) const;
		std::string					getPassword(void) const;
		int							getListener(void) const;
		User						&getUser(PFDIT &it);

		void	poll_handler(void);
		void	addUser(int fd, struct sockaddr_storage &addr);
		void	delUser(int i);

	private:
		std::vector<struct pollfd>	_pfds;
		std::vector<User *>			_users;
		std::string					_port;
		std::string					_password;
		int							_listener;

		void	createListener(void);
		void	*get_in_addr(struct sockaddr *sa);
};

#endif

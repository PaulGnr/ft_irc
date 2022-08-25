#ifndef USER_HPP
# define USER_HPP

#include "ft_irc.hpp"
 
class User
{
	public:
		User();
		~User();
		User(struct pollfd *pfd, struct sockaddr_storage *addr);

		void	setPseudo(std::string pseudo);
		void	setHostname(std::string hostname);
		void	setUser(std::string user);
		void	setServer(std::string server);
		void	setWelcomed(bool welcomed);
		void	setAddr(struct sockaddr_storage *addr);

		std::string		getPseudo(void) const;
		std::string		getHostname(void) const;
		std::string		getUser(void) const;
		std::string		getServer(void) const;
		std::string		getMessage(void) const;
		bool			hasBeenWelcomed(void) const;
		struct pollfd*	getPfd(void) const;

		void	appendMsg(std::string msg);
		void	clearMsg(void);

	private:

		std::string				_pseudo;
		std::string				_hostname;
		std::string				_user;
		std::string				_server;
		std::string				_message;
		bool					_welcomed;
		struct pollfd			*_pfd;
		struct sockaddr_storage	*_addr;
};

std::ostream&	operator<<(std::ostream &o, const User &user);

#endif

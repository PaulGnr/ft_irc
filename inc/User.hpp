#ifndef USER_HPP
# define USER_HPP

#include "ft_irc.hpp"

class User
{
	public:
		User();
		~User();
		User(int fd, struct sockaddr_storage *addr);

		void	setNick(std::string nick);
		void	setHostname(std::string hostname);
		void	setUser(std::string user);
		void	setServer(std::string server);
		void	setWelcomed(bool welcomed);
		void	setAddr(struct sockaddr_storage *addr);

		std::string	getNick(void) const;
		std::string	getHostname(void) const;
		std::string	getUser(void) const;
		std::string	getServer(void) const;
		std::string	getMessage(void) const;
		bool		hasBeenWelcomed(void) const;
		int			getFd(void) const;

		void	appendMessage(std::string msg);
		void	clearMessage(void);
		void	parse_info(std::string server_pass);

	private:

		std::string				_nick;
		std::string				_hostname;
		std::string				_user;
		std::string				_server;
		std::string				_message;
		bool					_welcomed;
		int						_fd;
		struct sockaddr_storage	*_addr;

		void			_irssiClient(std::string server_pass);
		void			_ncClient(std::string server_pass);
		void			_firstRecv(void);
		std::string		_parser_utils(std::string info, std::string end);
		void			_welcome(void);
};

std::ostream&	operator<<(std::ostream &o, const User &user);

#endif

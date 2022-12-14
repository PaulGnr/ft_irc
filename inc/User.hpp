#ifndef USER_HPP
# define USER_HPP

#include "ft_irc.hpp"

class User
{
	typedef	std::map<std::string, Channel *>::iterator chans_iterator;

	public:
		User();
		~User();
		User(int fd, struct sockaddr_storage *addr);

		void	setNickname(std::string nickname);
		void	setHostname(std::string hostname);
		void	setUser(std::string user);
		void	setRealname(std::string realname);
		void	setServer(std::string server);
		void	setMessage(std::string message);
		void	setWelcomed(bool welcomed);
		void	setPasswdOK(bool passwdOK);
		void	setAddr(struct sockaddr_storage *addr);

		std::string	getNickname(void) const;
		std::string	getHostname(void) const;
		std::string	getUser(void) const;
		std::string	getRealname(void) const;
		std::string	getServer(void) const;
		std::string	getMessage(void) const;
		std::string	getMode(void) const;
		bool		hasBeenWelcomed(void) const;
		bool		getPasswdOK(void) const;
		int			getFd(void) const;

		void		addMode(char c);
		void		delMode(char c);
		void		addChan(Channel *chan);
		void		delChan(Channel *chan);
		bool		isVisible(void);
		bool		isInVisibleChannel(void);
		bool		isInChan(void);
		Channel*	getFirstChan(void);

		void	sendQuit(std::string buf);
		void	sendReply(std::string reply);
		void	clearMsg(void);
		void	welcome(void);

		bool	wrongMode(char c);

	private:

		std::string							_nickname;
		std::string							_hostname;
		std::string							_user;
		std::string							_realname;
		std::string							_server;
		std::string							_message;
		std::string							_mode;
		bool								_welcomed;
		bool								_passwdOK; // Changer nom de variable si on trouve mieux
		int									_fd;
		std::map<std::string, Channel *>	_chans;
		struct sockaddr_storage				*_addr;
};

std::ostream&	operator<<(std::ostream &o, const User &user);

#endif

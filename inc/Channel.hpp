#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

class User;

class Channel
{
	public:
		Channel(const std::string &name, const std::string &key);
		~Channel(void);

		std::string	getName(void);
		std::string	getKey(void);
		User		*getAdmin(void);
		std::string	getMode(void);

		void		setAdmin(User *user);
		void		setMode(std::string mode);
		void		addUser(int fd, User *user);

	private:
		std::map<int, User *>	_users;
		User					*_admin;
		std::string				_name;
		std::string				_topic;
		std::string				_key;
		std::string				_mode;
		std::vector<User *>		_operators;
		std::vector<User *>		_ban;
};

#endif

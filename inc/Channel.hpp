#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

class User;

class Channel
{
	typedef	std::map<std::string, User *>::iterator users_iterator;

	public:
		Channel(const std::string &name, const std::string &key);
		~Channel(void);

		std::string	getName(void);
		std::string	getKey(void);
		User		*getAdmin(void);
		std::string	getMode(void);

		void		setAdmin(User *user);
		void		setMode(std::string mode);

		void		addUser(User *user);
		void		delUser(User *user);
		bool		userIsIn(User *user);

		void		broadcast(User *user, std::string msg, bool priv); 

	private:
		std::map<std::string, User *>	_users;
		User					*_admin;
		std::string				_name;
		std::string				_topic;
		std::string				_key;
		std::string				_mode;
		std::vector<User *>		_operators;
		std::vector<User *>		_ban;
};

#endif

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
	public:
		Channel(const std::string &name);
		~Channel(void);
		std::string	getName(void);

	private:
		std::string				_name;
		std::map<int, User *>	_users;
		User					*_admin;
};

#endif

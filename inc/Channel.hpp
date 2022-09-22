#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

class User;

class Channel
{
	typedef	std::map<int, User *>::iterator users_iterator;

	public:
		Channel(const std::string &name, const std::string &key);
		~Channel(void);

		std::string	getName(void);
		std::string	getKey(void);
		std::string	getMode(void);

		void	setKey(std::string key);
		void	setLimit(size_t limit);

		void	addUser(User *user);
		void	delUser(User *user);
		void	addOperator(User *user);
		void	delOperator(User *user);
		bool	userIsIn(User *user);
		bool	userIsOperator(User *user);
		bool	isEmpty(void);

		void	addMode(char c);
		void	delMode(char c);
		void	addModerate(User *user);
		bool	wrongMode(char c);

		bool	isInviteOnly(void);
		bool	isNoOutside(void);
		bool	isFull(void);

		void	broadcast(User *user, std::string msg, bool priv); 

		void	showOperators(void); //Debug

	private:
		std::string				_name;
		std::string				_topic;
		std::string				_key;
		std::string				_mode;
		std::map<int, User *>	_users;
		std::map<int, User *>	_operators; //A voir si on peut mettre plusieurs ope
		std::map<int, User *>	_ban;
		std::map<int, User *>	_moderate; //A voir si on autorise les moderate ou interdit
		size_t					_limit;
};

#endif

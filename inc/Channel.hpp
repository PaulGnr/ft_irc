#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

class User;

class Channel
{
	typedef	std::map<int, User*>::iterator users_iterator;

	public:
		Channel(const std::string &name, const std::string &key);
		~Channel(void);

		std::string	getName(void);
		std::string	getTopic(void);
		std::string	getMode(void);
		int			getUserCount(void);

		void	setKey(std::string key);
		void	setLimit(size_t limit);
		void	setTopic(std::string topic, std::string nick);

		void	addUser(User *user);
		void	delUser(User *user);
		void	addOperator(User *user);
		void	delOperator(User *user);
		void	addBan(User *user);
		void	delBan(User *user);
		void	addModerate(User *user);
		void	delModerate(User *user);
		void	addInvitee(User *user);
		void	delInvitee(User *user);
		bool	userIsIn(User *user);
		bool	userIsOperator(User *user);
		bool	userIsBan(User *user);
		bool	userIsModerate(User *user);
		bool	userIsInvitee(User *user);
		bool	isEmpty(void);

		void	addMode(char c);
		void	delMode(char c);
		bool	wrongMode(char c);
		bool	hasMode(char c);

		bool	isPrivate(void);
		bool	isSecret(void);
		bool	isInviteOnly(void);
		bool	isNoOutside(void);
		bool	isFull(void);
		bool	isKeyProtect(void);

		bool	checkKey(std::string key);

		void	rpl_topicwhotime(User *user);
		void	rpl_whoreply(User *user);
		void	rpl_namreply(User *user, bool endList);
		void	rpl_banlist(User *user);
		void	rpl_channelmodeis(User *user);

		void	broadcast(User *user, std::string msg); 
		void	privmsg(User *user, std::string msg); 

	private:
		std::string				_name;
		std::string				_key;
		std::string				_mode;
		std::string				_topic;
		std::string				_topicSetter;
		std::string				_topicTime;
		std::map<int, User *>	_users;
		std::map<int, User *>	_operators;
		std::map<int, User *>	_ban;
		std::map<int, User *>	_moderate;
		std::map<int, User *>	_invitee;
		size_t					_limit;
};

#endif

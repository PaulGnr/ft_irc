#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"

class User;
class Channel;

class Server
{
	typedef std::vector<pollfd>::iterator pfds_iterator;
	typedef std::map<int, User *>::iterator users_iterator;
	typedef std::map<std::string, Channel *>::iterator chans_iterator;
	typedef	void (Server::*fct)(User *, std::string);
	/* Pour le typedef au-dessus : Sert pour la map _cmd, c'est un pointeur sur
	 * fonction de Server avec en argument User * et std::string, c'est les
	 * fonction en bas de la class
	 */

	public:
		Server();
		Server(std::string port, std::string password);
		~Server();

		std::vector<struct pollfd>	&getPfds(void);
		std::map<int, User *>		&getUsers(void);
		std::string					getPort(void) const;
		std::string					getPassword(void) const;
		int							getListener(void) const;

	private:
		int									_listener;
		std::string							_port;
		std::string							_password;
		std::string							_host;
		std::vector<struct pollfd>			_pfds;
		std::map<int, User *>				_users;
		std::map<std::string, Channel *>	_chans;
		std::map<std::string, fct>			_cmd;
		std::map<char, void (Server::*const)(char, Channel *, User *, std::string)>			_chanModeOption;
		/* Pour au-dessus : map des cmd en fonction de la string envoye en debut
		 * de chaque message, exemple "PASS asdf" la string sera PASS, mais la
		 * fonction prendra en argument le user et std::string buf qui sera
		 * buf = "asdf" pour cet exemple (voir fonction _createCmd)
		 */

		// Server related functions

		void	_poll_handler(void);
		void	_createListener(void);
		void	_createCmd(void);
		void	_clientConnect(void);
		void	_clientMessage(pfds_iterator &it);
		int		_getMessage(User *user);
		void	_addUser(int fd, struct sockaddr_storage &addr);
		void	_delUser(pfds_iterator &it);
		void*	_get_in_addr(struct sockaddr *sa);
		void	_sendMsg(User *user, int sender_fd);
		int		_sendall(int dest_fd, const char *buf, int *nbytes);
		User*	_getUserByNick(std::string nick);

		// Command related functions

		void	_handleCmd(User *user);

		void	_caplsCmd(User *user, std::string buf);
		void	_passCmd(User *user, std::string buf);
		void	_nickCmd(User *user, std::string buf);
		void	_userCmd(User *user, std::string buf);
		void	_quitCmd(User *user, std::string buf);
		void	_modeCmd(User *user, std::string buf);
		void	_topicCmd(User *user, std::string buf);
		void	_pingCmd(User *user, std::string buf);
		void	_joinCmd(User *user, std::string buf);
		void	_partCmd(User *user, std::string buf);
		void	_privmsgCmd(User *user, std::string buf);
		void	_noticeCmd(User *user, std::string buf);

		// Mode Cmd

		void	_createModeOption(void);
		void	_channelModeCmd(User *user, std::string buf);

		void	_chanModeO(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeP(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeS(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeI(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeT(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeN(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeM(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeL(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeB(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeV(char sign, Channel *channel, User *user, std::string buf);
		void	_chanModeK(char sign, Channel *channel, User *user, std::string buf);


		void	_userModeCmd(User *user, std::string buf);

		// Privmsg Cmd

		void	_msgToUser(User *user, std::string dest, std::string msg);
		void	_msgToChannel(User *user, std::string dest, std::string msg);

		// Channel related functions

		int							_chanExists(std::string name);
		Channel*					_createChan(User *user, std::string name, std::string key);
		void						_addChannel(Channel *chan);
		void						_delChannel(Channel *chan);
		std::vector<std::string>	_getChannels(std::string buf);
		std::vector<std::string>	_getKeys(std::string buf, size_t size);
};

#endif

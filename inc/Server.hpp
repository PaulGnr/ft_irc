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
		Server(std::string port, std::string password);
		~Server();

		std::vector<struct pollfd>	&getPfds(void);
		std::map<int, User *>		&getUsers(void);
		std::string					getPort(void) const;
		std::string					getPassword(void) const;
		int							getListener(void) const;

		void	poll_handler(void);

	private:
		int									_listener;
		std::string							_port;
		std::string							_password;
		std::string							_host;
		std::vector<struct pollfd>			_pfds;
		std::map<int, User *>				_users;
		std::map<std::string, Channel *>	_chans;
		std::map<std::string, fct>			_cmd;
		/* Pour au-dessus : map des cmd en fonction de la string envoye en debut
		 * de chaque message, exemple "PASS asdf" la string sera PASS, mais la
		 * fonction prendra en argument le user et std::string buf qui sera
		 * buf = "asdf" pour cet exemple (voir fonction _createCmd)
		 */

		// Server related functions

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

		// Command related functions

		void	_handleCmd(User *user);

		void	_caplsCmd(User *user, std::string buf);
		void	_passCmd(User *user, std::string buf);
		void	_nickCmd(User *user, std::string buf);
		void	_userCmd(User *user, std::string buf);
		void	_quitCmd(User *user, std::string buf);
		void	_modeCmd(User *user, std::string buf);
		void	_pingCmd(User *user, std::string buf);
		void	_joinCmd(User *user, std::string buf);
		void	_privmsgCmd(User *user, std::string buf);

		// Utils

		void	_channelModeCmd(User *user, std::string buf);
		void	_nickModeCmd(User *user, std::string buf);
		bool	_wrongChannelMode(char c);
		bool	_wrongNickMode(char c);

		// Channel related functions

		int		_chanExists(std::string name);
		void	_createChan(User *user, std::string name, std::string key);
		void	_addChannel(Channel *chan);
		std::vector<std::string>	_getChannels(std::string buf);
		std::vector<std::string>	_getKeys(std::string buf, size_t size);
};

#endif

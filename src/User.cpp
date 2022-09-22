#include "User.hpp"

User::User(): mode(""), _nickname(""), _hostname(""), _user(""), _server(""), _welcomed(false), _passwdOK(false), _addr(NULL)
{}

User::User(int fd, struct sockaddr_storage *addr): mode(""), _nickname(""), _hostname("localhost"), _user(""), _server("IRC"), _message(""), _welcomed(false), _passwdOK(false), _fd(fd), _addr(addr)
{}

User::~User()
{}

void	User::setNickname(std::string nickname) {_nickname= nickname;}
void	User::setHostname(std::string hostname) {_hostname = hostname;}
void	User::setUser(std::string user) {_user = user;}
void	User::setServer(std::string server) {_server = server;}
void	User::setMessage(std::string message) {_message = message;}
void	User::setWelcomed(bool welcomed) {_welcomed = welcomed;}
void	User::setPasswdOK(bool passwdOK) {_passwdOK = passwdOK;}
void	User::setAddr(struct sockaddr_storage *addr) {_addr = addr;}

std::string		User::getNickname(void) const {return (_nickname);}
std::string		User::getHostname(void) const {return (_hostname);}
std::string		User::getUser(void) const {return (_user);}
std::string		User::getServer(void) const {return (_server);}
std::string		User::getMessage(void) const {return (_message);}
bool			User::hasBeenWelcomed(void) const {return (_welcomed);}
bool			User::getPasswdOK(void) const {return (_passwdOK);}
int				User::getFd(void) const {return (_fd);}

void	User::sendReply(std::string reply)
{
	reply.append("\r\n");
	size_t	total = 0;
	size_t	nbytes = reply.length();
	int		n;

	while (total < nbytes)
	{
		n = send(_fd, &(reply[total]), nbytes - total, 0);
		if (n == -1) break;
		total += n;
	}

	if (n == -1)
	{
		std::cout << "Error User::sendReply" << std::endl;//Throw erreur ici
	}
}

void	User::clearMsg(void) {
	_message.clear();
}

void	User::welcome(void)
{
	sendReply(RPL_WELCOME(_nickname, _server, _user, _hostname));
	sendReply(RPL_YOURHOST(_nickname, _server));
	sendReply(RPL_CREATED(_nickname, _server, timestamp()));
	sendReply(RPL_MYINFO(_nickname, _server));
	_welcomed = true;
}

bool	User::wrongMode(char c)
{
	if (c == 'i' || c == 'w' || c == 's' || c == 'o')
		return (false);
	return (true);
}

std::ostream&	operator<<(std::ostream &o, const User &user)
{
	o << "User :" << std::endl;
	o << "\t- nickname ->              " << user.getNickname() << std::endl;
	o << "\t- hostname ->          " << user.getHostname() << std::endl;
	o << "\t- user ->              " << user.getUser() << std::endl;
	o << "\t- server ->            " << user.getServer() << std::endl;
	o << "\t- has been welcomed -> " << user.hasBeenWelcomed() << std::endl;
	o << "\t- pfd ->               " << user.getFd() << std::endl;
	o << "\t- msg ->               " << user.getMessage() << std::endl;
	return (o);
}

#include "User.hpp"

User::User(): _nick(""), _hostname(""), _user(""), _server(""), _welcomed(false), _passwdOK(false), _addr(NULL)
{}

User::User(int fd, struct sockaddr_storage *addr): _nick(""), _hostname(""), _user(""), _server("localhost"), _message(""), _welcomed(false), _passwdOK(false), _fd(fd), _addr(addr)
{}

User::~User()
{}

void	User::setNick(std::string nick) {_nick= nick;}
void	User::setHostname(std::string hostname) {_hostname = hostname;}
void	User::setUser(std::string user) {_user = user;}
void	User::setServer(std::string server) {_server = server;}
void	User::setMessage(std::string message) {_message = message;}
void	User::setWelcomed(bool welcomed) {_welcomed = welcomed;}
void	User::setPasswdOK(bool passwdOK) {_passwdOK = passwdOK;}
void	User::setAddr(struct sockaddr_storage *addr) {_addr = addr;}

std::string		User::getNick(void) const {return (_nick);}
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
	std::string	rpl_1 = ":server 001 " + _nick + " :Welcome to the " + _server + " network, " + _nick + "[" + _user + "@" + _server + "]\r\n";
	std::string	rpl_2 = ":server 002 " + _nick + " :Your host is " + _server + ", running version 1.2.3\r\n";
	std::string	rpl_3 = ":server 003 " + _nick + " :This server was created " + timestamp() + "\r\n";
	std::string	rpl_4 = ":server 004 " + _nick + " localhost irssi 1.2.3 (20210409 0011)\r\n";

	send(_fd, rpl_1.c_str(), rpl_1.length(), 0);
	send(_fd, rpl_2.c_str(), rpl_2.length(), 0);
	send(_fd, rpl_3.c_str(), rpl_3.length(), 0);
	send(_fd, rpl_4.c_str(), rpl_4.length(), 0);
	_welcomed = true;
}

std::ostream&	operator<<(std::ostream &o, const User &user)
{
	o << "User :" << std::endl;
	o << "\t- nick ->              " << user.getNick() << std::endl;
	o << "\t- hostname ->          " << user.getHostname() << std::endl;
	o << "\t- user ->              " << user.getUser() << std::endl;
	o << "\t- server ->            " << user.getServer() << std::endl;
	o << "\t- has been welcomed -> " << user.hasBeenWelcomed() << std::endl;
	o << "\t- pfd ->               " << user.getFd() << std::endl;
	o << "\t- msg ->               " << user.getMessage() << std::endl;
	return (o);
}

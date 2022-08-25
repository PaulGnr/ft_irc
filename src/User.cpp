#include "User.hpp"

User::User(): _nickname(""), _hostname(""), _user(""), _server(""), _welcomed(false), _pfd(NULL), _addr(NULL)
{}

User::User(struct pollfd *pfd, struct sockaddr_storage *addr): _nickname(""), _hostname(""), _user(""), _server(""), _message(""), _welcomed(false), _pfd(pfd), _addr(addr)
{}

User::~User()
{}

void	User::setNickname(std::string nickname) {_nickname = nickname;}
void	User::setHostname(std::string hostname) {_hostname = hostname;}
void	User::setUser(std::string user) {_user = user;}
void	User::setServer(std::string server) {_server = server;}
void	User::setWelcomed(bool welcomed) {_welcomed = welcomed;}
void	User::setAddr(struct sockaddr_storage *addr) {_addr = addr;}

std::string		User::getNickname(void) const {return (_nickname);}
std::string		User::getHostname(void) const {return (_hostname);}
std::string		User::getUser(void) const {return (_user);}
std::string		User::getServer(void) const {return (_server);}
std::string		User::getMessage(void) const {return (_message);}
bool			User::hasBeenWelcomed(void) const {return (_welcomed);}
struct pollfd*	User::getPfd(void) const {return (_pfd);}

void	User::appendMsg(std::string msg)
{
	_message.append(msg);
	_message.erase(_message.find_last_not_of("\r\n") + 1);
	_message.append("\n");
}

void	User::clearMsg(void) {
	_message.clear();
}

std::ostream&	operator<<(std::ostream &o, const User &user)
{
	o << "User :" << std::endl;
	o << "\t- nickname ->            " << user.getNickname() << std::endl;
	o << "\t- hostname ->          " << user.getHostname() << std::endl;
	o << "\t- user ->              " << user.getUser() << std::endl;
	o << "\t- server ->            " << user.getServer() << std::endl;
	o << "\t- has been welcomed -> " << user.hasBeenWelcomed() << std::endl;
	o << "\t- pfd ->               " << user.getPfd()->fd << std::endl;
	o << "\t- msg ->               " << user.getMessage() << std::endl;
	return (o);
}

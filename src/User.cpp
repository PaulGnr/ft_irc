#include "User.hpp"

User::User(): _nick(""), _hostname(""), _user(""), _server(""), _welcomed(false), _pfd(NULL), _addr(NULL)
{}

User::User(struct pollfd *pfd, struct sockaddr_storage *addr): _nick(""), _hostname(""), _user(""), _server("localhost"), _message(""), _welcomed(false), _pfd(pfd), _addr(addr)
{}

User::~User()
{}

void	User::setNick(std::string nick) {_nick= nick;}
void	User::setHostname(std::string hostname) {_hostname = hostname;}
void	User::setUser(std::string user) {_user = user;}
void	User::setServer(std::string server) {_server = server;}
void	User::setWelcomed(bool welcomed) {_welcomed = welcomed;}
void	User::setAddr(struct sockaddr_storage *addr) {_addr = addr;}

std::string		User::getNick(void) const {return (_nick);}
std::string		User::getHostname(void) const {return (_hostname);}
std::string		User::getUser(void) const {return (_user);}
std::string		User::getServer(void) const {return (_server);}
std::string		User::getMessage(void) const {return (_message);}
bool			User::hasBeenWelcomed(void) const {return (_welcomed);}
struct pollfd*	User::getPfd(void) const {return (_pfd);}

void	User::appendMessage(std::string msg)
{
	_message.append(msg);
}

void	User::clearMessage(void) {
	_message.clear();
}

void	User::parse_info(void)
{
	size_t	pos;

	if ((pos = _message.find("CAP LS")) != std::string::npos)
		_message.erase(pos, _message.find("\r\n") + 2);
	if ((pos = _message.find("PING")) != std::string::npos)
		_message.erase(pos, _message.find("\r\n") + 2);
	// A enlever plus tard
	if ((pos = _message.find("MODE")) != std::string::npos)
		_message.erase(pos, _message.find("\r\n") + 2);

	_nick = _parser_utils("NICK", '\r');
	_user = _parser_utils("USER", ' ');

	//std::cout << "NICK: -" << _nick << "-" << std::endl;
	//std::cout << "USER: -" << _user << "-" << std::endl;

	if (!_welcomed && _nick.length() && _user.length())
		_welcome();
}

std::string	User::_parser_utils(std::string info, char end)
{
	std::string	str;
	size_t		i = 0;
	size_t		pos;

	if (_message.find(info) != std::string::npos)
	{
		str = _message.substr(_message.find(info) + info.length() + 1);
		while (i < str.find(end))
			++i;
		str = str.substr(0, i);
		if ((pos = _message.find("\r\n")) != std::string::npos)
			_message.erase(0, pos + 2);
		return (str);
	}
	return ("");
}

void	User::_welcome(void)
{
	std::string	m1 = "001 " + _nick + " :Welcome to the " + _server + " network, " + _nick + "[" + _user + "@" + _server + "]\r\n";
	std::string	m2 = "002 " + _nick + " :Your host is " + _server + ", running version 1.2.3\r\n";
	std::string	m3 = "003 " + _nick + " :This server was created 18:07:30\r\n";
	std::string	m4 = "004 " + _nick + " localhost irssi 1.2.3 (20210409 0011)\r\n";

	send(_pfd->fd, m1.c_str(), m1.length(), 0);
	send(_pfd->fd, m2.c_str(), m2.length(), 0);
	send(_pfd->fd, m3.c_str(), m3.length(), 0);
	send(_pfd->fd, m4.c_str(), m4.length(), 0);
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
	o << "\t- pfd ->               " << user.getPfd()->fd << std::endl;
	o << "\t- msg ->               " << user.getMessage() << std::endl;
	return (o);
}

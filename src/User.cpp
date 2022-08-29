#include "User.hpp"

User::User(): _nick(""), _hostname(""), _user(""), _server(""), _welcomed(false), _addr(NULL)
{}

User::User(int fd, struct sockaddr_storage *addr): _nick(""), _hostname(""), _user(""), _server("localhost"), _message(""), _welcomed(false), _fd(fd), _addr(addr)
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
int				User::getFd(void) const {return (_fd);}

void	User::appendMessage(std::string msg)
{
	_message.append(msg);
}

void	User::clearMessage(void) {
	_message.clear();
}

void	User::parse_info(std::string server_pass)
{
	size_t	pos;
	std::string user_pass;

	if (!_welcomed)
	{
		if ((pos = _message.find("CAP LS")) != std::string::npos)
		{
			_message.erase(pos, _message.find("\r\n") + 2);
			_irssiClient(server_pass);
		}
		else// if (!_nick.length() || !_user.length())
			_ncClient(server_pass);
	}

	// std::cout << "NICK: -" << _nick << "-" << std::endl;
	// std::cout << "USER: -" << _user << "-" << std::endl;
	// std::cout << "u_PASS: -" << user_pass << "-" << std::endl;
	// std::cout << "s_PASS: -" << server_pass << "-" << std::endl;
}

void	User::_irssiClient(std::string server_pass)
{
	std::string user_pass = _parser_utils("PASS", "\r\n");
	_nick = _parser_utils("NICK", "\r\n");
	_user = _parser_utils("USER", " ");
	_message.erase(0, _message.find("\r\n") + 2);
	if (user_pass.compare(server_pass) == 0)
		_welcome();
	else
		send_msg(_fd, ERR_PASSWDMISMATCH(_nick));
}

void	User::_ncClient(std::string server_pass)
{
	std::string	user_pass = _parser_utils("PASS", " ");
	
	if (user_pass.compare(server_pass) != 0)
		send_msg(_fd, ERR_PASSWDMISMATCH(_nick));
	else
	{
		_nick = _parser_utils("NICK", " ");
		_user = _parser_utils("USER", "\r\n");
		if (!_nick.length() || !_user.length())
			send_msg(_fd, "Wrong nickname or username");
		else
		{
			send_msg(_fd, "Welcome !");
			_welcomed = true;
		}
	}
}

std::string	User::_parser_utils(std::string info, std::string end)
{
	std::string	str;
	size_t		i = 0;
	size_t		pos;

	if (_message.find(info) != std::string::npos)
	{
		str = _message.substr(_message.find(info) + info.length() + 1);
		while (i < str.find(end.c_str()))
			++i;
		str = str.substr(0, i);
		if ((pos = _message.find(end.c_str())) != std::string::npos)
			_message.erase(0, pos + end.length());
		return (str);
	}
	return ("");
}

void	User::_welcome(void)
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

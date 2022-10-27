#include "User.hpp"

User::User(): _nickname(""), _hostname(""), _user(""), _realname(""), _server(""), _welcomed(false), _passwdOK(false), _addr(NULL)
{}

User::User(int fd, struct sockaddr_storage *addr): _nickname(""), _hostname("localhost"), _user(""), _realname(""), _server("IRC"), _message(""), _welcomed(false), _passwdOK(false), _fd(fd), _addr(addr)
{}

User::~User()
{}

void	User::setNickname(std::string nickname) {_nickname= nickname;}
void	User::setHostname(std::string hostname) {_hostname = hostname;}
void	User::setUser(std::string user) {_user = user;}
void	User::setRealname(std::string realname) {_realname = realname;}
void	User::setServer(std::string server) {_server = server;}
void	User::setMessage(std::string message) {_message = message;}
void	User::setWelcomed(bool welcomed) {_welcomed = welcomed;}
void	User::setPasswdOK(bool passwdOK) {_passwdOK = passwdOK;}
void	User::setAddr(struct sockaddr_storage *addr) {_addr = addr;}

std::string		User::getNickname(void) const {return (_nickname);}
std::string		User::getHostname(void) const {return (_hostname);}
std::string		User::getUser(void) const {return (_user);}
std::string		User::getRealname(void) const {return (_realname);}
std::string		User::getServer(void) const {return (_server);}
std::string		User::getMessage(void) const {return (_message);}
std::string		User::getMode(void) const {return (_mode);}
bool			User::hasBeenWelcomed(void) const {return (_welcomed);}
bool			User::getPasswdOK(void) const {return (_passwdOK);}
int				User::getFd(void) const {return (_fd);}

void	User::addMode(char c)
{
	if (_mode.find(c) == std::string::npos)
		_mode.append(1, c);
}

void	User::delMode(char c)
{
	if (_mode.find(c) != std::string::npos)
		_mode.erase(_mode.find(c), 1);
}

void	User::addChan(Channel *chan)
{
	_chans.insert(std::make_pair(chan->getName(), chan));
}

void	User::delChan(Channel *chan)
{
	_chans.erase(chan->getName());
	chan->delUser(this);
}

bool	User::isVisible(void)
{
	if (_mode.find('i') != std::string::npos)
		return (false);
	return (true);
}

bool	User::isInVisibleChannel(void)
{
	for (chans_iterator it = _chans.begin(); it != _chans.end(); ++it)
	{
		if (!it->second->isPrivate() && !it->second->isSecret())
			return (true);
	}
	return (false);
}

bool	User::isInChan(void)
{
	if (_chans.size() != 0)
		return (true);
	return (false);
}

Channel*	User::getFirstChan(void)
{
	return (_chans.begin()->second);
}

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
		//std::cout << "Error User::sendReply" << std::endl;//Throw erreur ici
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

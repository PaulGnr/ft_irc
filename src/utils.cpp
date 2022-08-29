#include "ft_irc.hpp"

// doublon?
void	send_msg(int fd, std::string msg)
{
	msg.append("\r\n");
	send(fd, msg.c_str(), msg.length(), 0);
}

std::string	timestamp(void)
{
	time_t		rawtime;
	struct tm	*timeinfo;
	char		buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string time(buffer);

	return (time);
}

void	server_log(std::string msg)
{
	std::cout << "[" << timestamp() << "] " << msg << std::endl;
}

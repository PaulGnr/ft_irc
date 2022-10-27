#include "ft_irc.hpp"

extern bool	running;

bool	portIsOK(std::string port)
{
	std::string::size_type	i;

	for (size_t j = 0; j < port.size(); ++j)
	{
		if (port[j] < '0' || port[j] > '9')
			return (false);
	}
	std::stringstream	ss;
	ss << port;
	ss >> i;
	if (i <= 0 || i > 65535)
		return (false);
	return (true);
}

void	sigHandler(int signum)
{
	(void)signum;
	running = false;
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Error : Wrong number of arguments." << std::endl;
		return (1);
	}

	if (!portIsOK(argv[1]))
	{
		std::cerr << "Error : Bad port number." << std::endl;
		return (1);
	}
	if (!argv[2][0])
	{
		std::cerr << "Error : No password given." << std::endl;
		return (1);
	}
	running = true;
	signal(SIGINT, sigHandler);
	try
	{
		Server	server(argv[1], argv[2]);
	}
	catch (const std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}

	return (0);
}

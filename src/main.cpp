#include "ft_irc.hpp"

extern bool	running;

bool	portIsOK(std::string port)
{
	std::string::size_type	i;
	int						i_port;

	try
	{
		std::istringstream(port) >> i_port;
		i = int(log10(i_port) + 1);
		if (i != port.size())
			return (false);
		if (i <= 0 || i > 65535)
			return (false);
		return (true);
	}
	catch (const std::exception &e)
	{
		return (false);
	}
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

#include "ft_irc.hpp"

bool	portIsOK(std::string port)
{
	std::string::size_type	i;
	int						i_port;

	try
	{
		i_port = stoi(port, &i);
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

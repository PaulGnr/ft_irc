#include "ft_irc.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Error : Wrong number of arguments." << std::endl;
		return (1);
	}
	
	Server	server(argv[1], argv[2]);
	server.poll_handler();

	return (0);
}

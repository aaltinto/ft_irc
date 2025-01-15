#include <iostream>
#include "server.hpp"
#include "client.hpp"

int err(std::string err)
{
	std::cout << err << std::endl;
}

int main(int ac, char **argv)
{
	if (ac != 3)
		return err("Has 2 arguments: ./ircserv <port> <password>");
	try
	{
		Server server(std::atoi(argv[1]), std::string(argv[2]));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	
}
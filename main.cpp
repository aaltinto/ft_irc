#include <iostream>
#include "server.hpp"
#include "client.hpp"
#include <signal.h>

int err(std::string err)
{
	std::cout << err << std::endl;
	return 1;
}

int main(int ac, char **argv)
{
	if (ac != 3)
		return err("Has 2 arguments: ./ircserv <port> <password>");
	try
	{
		std::string port = std::string(argv[1]);
		std::string password = std::string(argv[2]);
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		Server server(port, password);
		server.startServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}

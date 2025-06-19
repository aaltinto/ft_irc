#include <iostream>
#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include <cerrno>
#include <cstring>
#include <signal.h>

int err(std::string err)
{
	std::cerr << err << std::endl;
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
		if (signal(SIGINT, Server::signalHandler) == SIG_ERR)
			throw std::runtime_error(std::strerror(errno));
		if (signal(SIGQUIT, Server::signalHandler) == SIG_ERR)
			throw std::runtime_error(std::strerror(errno));
		Server server(port, password);
		server.startServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}

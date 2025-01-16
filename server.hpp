#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "client.hpp"

class Server
{
	private:
		std::string _password;
		int _port;
		int _serverSocketFd;
		static bool _signal;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _fds;

	public:
		Server(std::string port, std::string password);
		Server(Server const &server);
		Server const &operator=(Server &server);
		~Server();

		void serverInit();
		// void serverSocketCreate();
		void acceptNewClient();
		void RecieveNewData(int fd);

		static void signalHandler(int signum);

		void closeFds();
		void clearClient(int fd);
};

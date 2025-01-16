#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "client.hpp"
#include "chanel.hpp"

class Server
{
	private:
		std::string _password;
		int _port;
		int _serverSocketFd;
		static bool _signal;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _fds;
		std::vector<Chanels> _chanels;

	public:
		Server(std::string port, std::string password);
		Server(Server const &server);
		Server const &operator=(Server &server);
		~Server();

		void serverSocketCreate();
		void startServer();
		void acceptNewClient();
		void recieveNewData(int fd);

		void addChanel(Chanels &chanel);

		static void signalHandler(int signum);

		void closeFds();
		void clearClient(int fd);
};

#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "../includes/client.hpp"
#include "../includes/channel.hpp"

class Server
{
	private:
		std::string _password;
		int _port;
		int _serverSocketFd;
		static bool _signal;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _fds;
		std::vector<Channels> _channels;

	public:
		Server(std::string port, std::string password);
		Server(Server const &server);
		Server const &operator=(Server &server);
		~Server();

		void serverSocketCreate();
		void startServer();
		void acceptNewClient();
		void recieveNewData(int fd);

		void join(std::vector<std::string> args, int fd);
		void topic(std::vector<std::string> args, int fd);
		void quit(std::vector<std::string> args, int fd);

		Client &getClient(int fd);

		void addChannel(std::string channelName, Client &client);

		static void signalHandler(int signum);

		void closeFds();
		void clearClient(int fd);
};

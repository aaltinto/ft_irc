#pragma once

#include <string>

class Server
{
	private:
		int _port;
		int _serverSocketFd

	public:
		Server();
		Server(Server const &server);
		Server const &operator=(Server &server);
		~Server();

		int getFd() const;
		std::string getIpAdd() const;

		void setFd(int const fd);
		void setIpAddr(std::string const ipAddr);
};

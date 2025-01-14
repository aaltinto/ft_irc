#pragma once

#include <string>

class Client
{
	private:
		int _fd;
		std::string _ipAddr;
	public:
		Client();
		Client(Client const &client);
		Client const &operator=(Client &client);
		~Client();

		int getFd() const;
		std::string getIpAdd() const;

		void setFd(int const fd);
		void setIpAddr(std::string const ipAddr);
};

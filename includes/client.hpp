#pragma once

#include <string>
#include <vector>

class Client
{
	private:
		int _fd;
		std::string _nick;
		std::string _username;
		std::string _realName;
		std::string _ipAddr;
		bool _isAuth;
		std::vector<std::string> _joinedChannels;
	public:
		Client();
		Client(Client const &client);
		Client const &operator=(Client const &client);
		~Client();

		int getFd() const;
		bool isAuth() const;
		void auth(bool value = true);
		std::string getNick() const;
		std::string getRealName() const;
		std::string getIpAdd() const;
		std::string getUsername() const;
		std::string getFullIdenifer() const;
		std::vector<std::string> getJoinedChannels() const;

		void addChannel(std::string const channel);
		void removeChannel(std::string const channel);
		void setFd(int const fd);
		void setIpAddr(std::string const ipAddr);
		void setNick(std::string const nick);
		void setRealName(std::string const nick);
		void setUsername(std::string const username);
};

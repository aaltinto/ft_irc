#pragma once

#include <iostream>
#include <string>

class bot
{
	private:
		/* Private Members */
		int			_socket;
		std::string	_server;
		std::string	_port;
		std::string _password;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		bool		_connected;
		bool		_authenticated;
		bool		_running;
		std::vector<std::string> _channels;

	public:
		/* Constructors & Destructors */
		bot(std::string const &server, std::string const &port, std::string const &password,
			std::string const &nickname, std::vector<std::string> const &channels, std::string const &username = "ircbot",
			std::string const &realname = "IRC Bot");
		bot(bot const &copy);
		~bot(void);

		/* Public Methods */
		bool connectServ();
		void disconnect();
		void run();
		void processMessage(std::string const &rawMessage);
		std::vector<std::string> parseMessage(std::string const &rawMessage);
		void handleMessage(std::vector <std::string> &parts);
		void sendRawMessage(std::string const &message);

		/* Basic Operators */
		bot const	&operator=(bot const &copy);
};


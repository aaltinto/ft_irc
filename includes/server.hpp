#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include "../includes/mode.hpp"

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
		static int command_in_command;
		Server(std::string port, std::string password);
		Server(Server const &server);
		Server const &operator=(Server &server);
		~Server();

		void serverSocketCreate();
		void startServer();
		void acceptNewClient();
		void recieveNewData(int fd);


		//messages
		void sendTopic(int fd, Channels channel);


		//commands
		void join(std::vector<std::string> args, int fd);
		void handleJoin(Client client, Channels channel);
		void nick(std::vector<std::string> args, int fd);
		void user(std::vector<std::string> args, int fd);
		void privmsg(std::vector<std::string> args, int fd);
		void topic(std::vector<std::string> args, int fd);
		void part(std::vector<std::string> args, int fd);
		void quit(std::vector<std::string> args, int fd);
		void kick(std::vector<std::string> args, int fd);
		void pass(std::vector<std::string> args, int fd);
		void invite(std::vector<std::string> args, int fd);
		void mode(std::vector<std::string> args, int fd);
		std::vector<Mode> modeSlasher(std::vector<std::string> &args);

		

		void exec_command(int fd, std::vector<std::string> commando);
		//getters
		Channels *getChannelbyName(std::string channelName);
		int getChannelIndex(std::string channelName);
		Client *getClient(int fd);
		int getClientbyNick(std::string nickname);
		int getClientIndex(int fd);

		void addChannel(std::string channelName, Client client);
		void removeChannel(std::string channelName);

		static void signalHandler(int signum);

		void closeFds();
		void clearClient(int fd, std::string quitMsg = "Connection lost");
};


std::string to_lower(std::string str);
void sendMessage(int fd, const std::string message);

//errros
void noSuchChannel(Client *client, std::string channelName);
void noSuchNick(Client *client,std::string nickName, std::string channelName="");
void permissionDenied(Client *client, Channels channel);
void notInThatChannel(Client *client, Channels channel);
void invalidChannelName(Client *client, std::string channelName);
void notEnoughParameters(Client *client, std::string command);
void inviteOnlyChannel(Client *client, Channels *channel);
void passwordRequired(Client *client, Channels *channel);
void channelLimitExceeded(Client *client, Channels *channel);
void passwordIncorrect(Client *client, Channels *channel);
void invalidLimit(Client *client, Channels *channel);
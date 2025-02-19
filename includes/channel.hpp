#pragma once
#include <string>
#include <vector>
#include "../includes/client.hpp"
#include "../includes/mode.hpp"

class Channels
{
	private:
		std::string _name;
		std::string _created_at;
		std::string _create_time;
		std::string _topic_name;
		std::string _password;
		std::string _mods;
		bool _inviteOnly;
		bool _topicProtection;
		bool _IsProtected;
		size_t _limit;
		std::vector<std::string> _invitedClients;
		std::vector<Client> _clients;
		
	public:
		Channels(Client &client, std::string channelName);
		Channels(Channels const &channels);
		Channels const &operator=(Channels const &Channels);
		~Channels();
		
		//operations
		void joinChannel(Client &client);
		void partChannel(Client &client);
		void adminOps(Client &client, bool value = true);
		void addInvitedClient(std::string nickname);
		void removeInvitedClient(std::string nickname);
		void sendMessageToAll(std::string message, int excludeFd = 0);

		//checks
		bool checkClientIsIn(int fd);
		bool isTopicProtected();
		bool isProtected();
		bool isInviteOnly();
		bool isInvited(std::string nickname);
		int isAdmin(int fd);
		int checkLimitExceeded();

		//getters
		std::string getPass() const;
		std::string getChannelName() const;
		std::string getChannelClients();
		std::string getTopicName();
		int getClientByNick(std::string nickname);
		Client *getClient(int fd);
		int getClientIndex(int fd);
		std::string getMods() const;
		int getClientCount();
		std::string getTime();

		//setters
		void setTopicName(std::string topicName);
		void setChannelName(std::string channelName);
		void setPass(std::string pass);
		void setLimit(int limit=0);
		void setTime();

		void unknownModeFlag(int fd, char flag);

		//modes
		void setMode(std::vector<Mode> modes, int fd);
		void inviteOnly(Mode mode, int fd);
		void topicProtection(Mode mode, int fd);
		void activateLimit(Mode mode, int fd);
		void setKey(Mode mode, int fd);
		void setOperator(Mode mode, int fd);

};

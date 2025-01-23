#pragma once
#include <string>
#include <vector>
#include "client.hpp"

class Chanels
{
	private:
		int _topic;
		std::string _name;
		std::string _created_at;
		std::string _create_time;
		std::string _topic_name;
		std::vector<Client> _clients;
		std::vector<Client> _admins;
		
	public:
		Chanels(Client &client, std::string chanelName);
		Chanels(Chanels const &chanels);
		Chanels const &operator=(Chanels const &Chanels);
		~Chanels();
		
		void joinChanel(Client &client);

		std::string getChanelName();
		void setChanelName(std::string chanelName);
		
		std::string getTopicName();
		void setTopicName(std::string topicName);
		
		void setTime();
		std::string getTime();
};

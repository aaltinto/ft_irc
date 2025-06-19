#include "bot.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>

bot::bot(std::string const &server, std::string const &port, std::string const &password,
			std::string const &nickname, std::vector<std::string> const &channels, std::string const &username, std::string const &realname)
	: _socket(-1), _server(server), _port(port), _password(password),
	  _nickname(nickname), _username(username), _realname(realname),
	  _connected(false), _authenticated(false), _running(false), _channels(channels)
{
	_connected = connectServ();
	if (_connected) {
		std::cout << "Connected to " << _server << ":" << _port << std::endl;
	} else {
		std::cerr << "Failed to connect to " << _server << ":" << _port << std::endl;
		return;
	}
	if (!_password.empty()) {
		sendRawMessage("PASS " + _password);
		std::cout << "Sent password authentication" << std::endl;
	}
	if (!_nickname.empty()) {
		sendRawMessage("NICK " + _nickname);
		std::cout << "Set nickname to: " << _nickname << std::endl;
	}
	if (!_username.empty() && !_realname.empty()) {
		std::string userCmd = "USER " + _username + " 0 * :" + _realname;
		sendRawMessage(userCmd);
		std::cout << "Set user info - Username: " << _username << ", Realname: " << _realname << std::endl;
	}
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		sendRawMessage("JOIN " + _channels[i]);
		std::cout << "Joined channel: " << _channels[i] << std::endl;
	}
}

bot::bot(bot const &copy)
{
	*this = copy;
}

bot::~bot(void)
{

}

bot const	&bot::operator=(const bot &copy)
{
	if (this == &copy)
		return (*this);
	_socket = copy._socket;
	_server = copy._server;
	_port = copy._port;
	_password = copy._password;
	_nickname = copy._nickname;
	_username = copy._username;
	_realname = copy._realname;
	_connected = copy._connected;
	_authenticated = copy._authenticated;
	_running = copy._running;
	return (*this);
}

bool bot::connectServ()
{
	struct addrinfo hints, *result, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

	if (_connected) {
		std::cerr << "Already connected to server" << std::endl;
		return false;
	}
	
	getaddrinfo(_server.c_str(), _port.c_str(), &hints, &result);
	for (res = result; res != NULL; res = res->ai_next) {
        _socket = socket(res->ai_family, res->ai_socktype,
                     res->ai_protocol);
        if (_socket == -1)
            continue;

        if (connect(_socket, res->ai_addr, res->ai_addrlen) != -1)
            break;
        close(_socket);
    }
	freeaddrinfo(result);

	if (res == NULL) {
		std::cerr << "Failed to connect to " << _server << ":" << _port 
				  << " - " << std::strerror(errno) << std::endl;
		_socket = -1;
		return false;
	}
	return true;
	
}

void bot::disconnect()
{
	if (_connected && _socket != -1) {
		sendRawMessage("QUIT :Bot disconnecting");
		close(_socket);
		_socket = -1;
		_connected = false;
		_authenticated = false;
		std::cout << "Disconnected from server" << std::endl;
	}
}

void bot::sendRawMessage(const std::string &message)
{
	if (!_connected || _socket == -1) {
		std::cerr << "Cannot send message: not connected" << std::endl;
		return;
	}

	std::string fullMessage = message + "\r\n";
	ssize_t sent = send(_socket, fullMessage.c_str(), fullMessage.length(), 0);
	
	if (sent < 0) {
		std::cerr << "Failed to send message: " << std::strerror(errno) << std::endl;
	}
}

std::vector<std::string> bot::parseMessage(const std::string &rawMessage)
{
	std::vector<std::string> parts;
	size_t start = 1, end;

	while ((end = rawMessage.find(' ', start)) != std::string::npos) {
		if (rawMessage.find(':', start) == start) {
			parts.push_back(rawMessage.substr(start));
			return parts;
		}
		parts.push_back(rawMessage.substr(start, end - start));
		start = end + 1;
	}
	parts.push_back(rawMessage.substr(start));

	return parts;
}

static std::string extractSender(const std::string &rawMessage)
{
	if (rawMessage.empty())
		return "";
	size_t exclamPos = rawMessage.find('!');
	if (exclamPos != std::string::npos) {
		return rawMessage.substr(0, exclamPos);
	}
	return rawMessage;
}

bool checkGreenList(const std::string &sender, const std::vector<std::string> greenList)
{
	for (size_t i = 0; i < greenList.size(); ++i) {
		if (sender == greenList[i]) {
			return true;
		}
	}
	return false;
}

void bot::handleMessage(std::vector<std::string> &parts)

{
    greenList.push_back("aaltinto"); 
    
	std::string sender = extractSender(parts[0]);
	std::string target = parts[1];
	std::string channel = parts[2];
	if (target == "JOIN") 
	{
		if (checkGreenList(sender, greenList))
		{
			std::string message = "MODE " + channel + " +o " + sender;
			std::cout << message << std::endl;
			sendRawMessage(message);
		}
		if (_userCurseCount.find(sender) == _userCurseCount.end())
		{
			_userCurseCount[sender] = 0;
		}
		else
		{
			if (_userCurseCount[sender] >= 3)
			{
				std::cout << "User " << sender << " has been banned for excessive cursing." << std::endl;
				sendRawMessage("KICK " + channel + " " + sender + " :Excessive cursing");
				return;
			}
		}
	}
	if (target == "PRIVMSG")
	{
		std::string message = parts[3];
		std::vector<std::string> messageParts = parseMessage(message);
		for (size_t i = 0; i < messageParts.size(); ++i)
		{
			if (messageParts[i] == "!hello")
			{
				sendRawMessage("PRIVMSG " + channel + " :Hello " + sender + "!");
				return;
			}
			if (messageParts[i] == "!addCurse")
			{
				if (checkGreenList(sender, greenList)) {
					std::cout << "Adding curse word: " << messageParts[i + 1] << std::endl;
					blackList.push_back(messageParts[i + 1]);
					sendRawMessage("PRIVMSG " + channel + " :Curse word added: " + messageParts[i + 1]);
					return;
				} else {
					sendRawMessage("PRIVMSG " + channel + " :You do not have permission to add curse words.");
					return;
				}
			}
			else if (messageParts[i] == "!help")
			{
				sendRawMessage("PRIVMSG " + channel + " :Available commands: !hello, !help, !channels, !ping");
				return;
			}
			else if (messageParts[i] == "!channels")
			{
				std::string channelsList = "I'm in channels: ";
				for (size_t j = 0; j < _channels.size(); ++j)
				{
					channelsList += _channels[j];
					if (j < _channels.size() - 1) {
						channelsList += ", ";
					}
				}
				sendRawMessage("PRIVMSG " + channel + " :" + channelsList);
				return;
			}
			else if (messageParts[i] == "!ping")
			{
				sendRawMessage("PRIVMSG " + channel + " :Pong!");
				return;
			}
			else if (checkGreenList(messageParts[i], blackList)) {
				_userCurseCount[sender]++;
				if (_userCurseCount[sender] >= 3) {
					std::cout << "User " << sender << " has been banned for excessive cursing." << std::endl;
					sendRawMessage("KICK " + channel + " " + sender + " :Excessive cursing");
					return;
				}
				sendRawMessage("PRIVMSG " + channel + " :Please refrain from using offensive language, " + sender + ".");
				return;
			}
		}
	}
}

void bot::processMessage(const std::string &rawMessage)
{
	if (rawMessage.empty())
		return;
	std::vector<std::string> parts = parseMessage(rawMessage);
	if (parts.size() < 3) {
		std::cerr << "Invalid message format: " << rawMessage << std::endl;
		return;
	}
	handleMessage(parts);
}

void bot::run()
{
	if (!_connected) {
		std::cerr << "Bot is not connected" << std::endl;
		return;
	}

	_running = true;
	char buffer[1024];
	std::string messageBuffer;

	std::cout << "Bot is now running. Press Ctrl+C to stop." << std::endl;

	while (_running && _connected) {

		memset(buffer, 0, sizeof(buffer));
		ssize_t received = recv(_socket, buffer, sizeof(buffer) - 1, 0);
		
		if (received <= 0) {
			if (received == 0) {
				std::cout << "Server closed connection" << std::endl;
			} else {
				std::cerr << "Receive error: " << std::strerror(errno) << std::endl;
			}
			break;
		}

		buffer[received] = '\0';
		messageBuffer += buffer;

		size_t pos;
		while ((pos = messageBuffer.find("\r\n")) != std::string::npos) {
			std::string rawMessage = messageBuffer.substr(0, pos);
			messageBuffer.erase(0, pos + 2);
			processMessage(rawMessage);
		}
        if (!messageBuffer.empty())
            processMessage(messageBuffer);
        
	}
}
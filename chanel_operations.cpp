#include "server.hpp"
#include "client.hpp"
#include "chanel.hpp"

void Server::addChanel(Chanels &chanel)
{
	this->_chanels.push_back(chanel);
}

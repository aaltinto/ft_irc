#include <iostream>

int err(std::string err)
{
	std::cout << err << std::endl;
}

int main(int ac, char **argv)
{
	if (ac != 3)
		return err("Has 2 arguments: ./ircserv <port> <password>");
	
}
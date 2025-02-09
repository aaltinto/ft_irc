#include "../includes/mode.hpp"

Mode::Mode(int sign, char flag, std::string arg)
{
	this->_arg = arg;
	this->_flag = flag;
	this->_sign = sign;
}

Mode::Mode()
{
	this->_arg = "";
	this->_flag = 0;
	this->_sign = 0;
}

Mode::Mode(Mode const &mode)
{
	this->_arg = mode._arg;
	this->_flag = mode._flag;
	this->_sign = mode._sign;
}

Mode::~Mode()
{

}

Mode const &Mode::operator=(Mode const &mode)
{
	this->_arg = mode._arg;
	this->_flag = mode._flag;
	this->_sign = mode._sign;

	return *this;
}

void Mode::setFlag(char flag)
{
	this->_flag = flag;
}

void Mode::setArg(std::string arg)
{
	this->_arg = arg;
}

void Mode::setSign(int sign)
{
	this->_sign = sign;
}

char Mode::getFlag() const
{
	return this->_flag;
}

std::string Mode::getArg() const
{
	return this->_arg;
}

int Mode::getSign() const
{
	return this->_sign;
}
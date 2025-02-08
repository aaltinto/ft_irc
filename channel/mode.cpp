#include "../includes/mode.hpp"

Mode::Mode(int sign, char mode, std::string arg)
{
	this->_arg = arg;
	this->_mode = mode;
	this->_sign = sign;
}

Mode::Mode(Mode const &mode)
{
	this->_arg = mode._arg;
	this->_mode = mode._mode;
	this->_sign = mode._sign;
}

Mode::~Mode()
{

}

Mode const &Mode::operator=(Mode const &mode)
{
	this->_arg = mode._arg;
	this->_mode = mode._mode;
	this->_sign = mode._sign;
}
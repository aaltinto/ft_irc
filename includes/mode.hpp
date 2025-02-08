#pragma once

#include <string>
#include <vector>

class Mode
{
	private:
		int _sign;
		std::string _mode;
		std::string _arg;
	public:
		Mode(int sign, char mode, std::string arg);
		Mode(Mode const &mode);
		Mode const &operator=(Mode const &mode);
		~Mode();

		
};

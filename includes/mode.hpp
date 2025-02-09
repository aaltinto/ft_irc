#pragma once

#include <string>
#include <vector>

class Mode
{
	private:
		int _sign;
		char _flag;
		std::string _arg;
	public:
		Mode(int sign, char mode, std::string arg);
		Mode();
		Mode(Mode const &mode);
		Mode const &operator=(Mode const &mode);
		~Mode();

		void setFlag(char flag);
		void setArg(std::string arg);
		void setSign(int sign);

		char getFlag() const;
		std::string getArg() const;
		int getSign() const;

		
};

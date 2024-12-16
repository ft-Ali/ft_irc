
#include <iostream>
#pragma once
class Client{
	private:
		std::string _name;
	public:
		Client(std::string &name): _name(name){};
};
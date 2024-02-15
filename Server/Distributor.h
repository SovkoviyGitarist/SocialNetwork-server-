#pragma once
#include "Clients.h"


class Distributor
{
private:
	std::vector<Client> clients_vector;
	std::vector<Client::ptr> clients_ptr_vector;
	std::string UserId;
	std::string command;

public:
	Distributor();
	~Distributor();

	static void execute_command(std::string UserId, std::string command);

};

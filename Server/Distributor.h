#pragma once
#include "Clients.h"
#include <pqxx/pqxx>

class Distributor
{
private:
	std::vector<Client> clients_vector;
	std::vector<Client::ptr> clients_ptr_vector;


public:
	Distributor();
	~Distributor();

};

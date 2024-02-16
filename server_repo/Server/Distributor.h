#pragma once
#include "Clients.h"
#include <pqxx/pqxx>

class Distributor : public boost::enable_shared_from_this<Distributor>
{
private:
	static std::vector<Client> clients_vector;
	static std::vector<Client::ptr> clients_ptr_vector;
	std::string UserId;
	std::string command;

public:
	Distributor(std::string &UserId, std::string &command);
	~Distributor();

	typedef boost::shared_ptr<Distributor> ptr;


	static void execute_command();

};

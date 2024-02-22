#pragma once
#include "Clients.h"
#include <pqxx/pqxx>
#include <boost/regex.hpp>
#include <boost/bind.hpp>



class ClientLogic : public Client
{
private:
	Client::ptr this_client;

public:
	ClientLogic(Client::ptr client_ptr);
	~ClientLogic();

	typedef boost::shared_ptr<Client> ptr;

};

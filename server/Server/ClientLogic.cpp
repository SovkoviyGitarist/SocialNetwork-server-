#include "ClientLogic.h"



ClientLogic::ClientLogic(Client::ptr client_ptr) : this_client(client_ptr)
{
	std::cout << "new logic" << std::endl;
}

ClientLogic::~ClientLogic()
{
	std::cout << "distract new logic" << std::endl;
}
#include "Distributor.h"

Distributor::Distributor(std::string& UserId, std::string& command, boost::shared_ptr<ip::tcp::socket> sock) : UserId(UserId), command(command), sock_ptr(sock){}

Distributor::~Distributor()
{
}

void Distributor::execute_command()
{
	ptr this_distributor = shared_from_this();

	boost::regex new_client_expr("make_new_client");
	boost::regex existing_client_expr("connect_to_existing");
	boost::regex txt_sock_expr("connect_to_txt_msg_sock");
	boost::regex file_sock_expr("connect_to_file_msg_sock");

	if (boost::regex_search(this_distributor->command, new_client_expr))
	{

	}
	else if (boost::regex_search(this_distributor->command, existing_client_expr))
	{

	}
	else if (boost::regex_search(this_distributor->command, txt_sock_expr))
	{

	}
	else if (boost::regex_search(this_distributor->command, file_sock_expr))
	{

	}
}



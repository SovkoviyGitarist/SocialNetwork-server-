#pragma once
#include "Clients.h"
#include <pqxx/pqxx>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>	

using namespace boost::archive;

class ClientLogic : public Client
{
private:
	std::string acc_data_buffer;
	std::string file_msg_buffer;
	std::string txt_msg_buffer;

	Client::ptr this_client;



	void read_acc_data_sock(const error_code& err, size_t bytes);

	size_t read_complete(const error_code& err, size_t bytes);

	void acc_data_distributor(const error_code& err, size_t bytes);

	void change_avatar();

	void add_photo();

	void delete_photo();

	void user_list();

	void user_acc();

	void new_chat();

public:
	ClientLogic(Client::ptr client_ptr);
	~ClientLogic();

	void set_default_settings();

	void send_acc_data();

	void send_chat_list();

	void send_file_list();

	typedef boost::shared_ptr<Client> ptr;

};

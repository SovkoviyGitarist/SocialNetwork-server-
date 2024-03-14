#pragma once
#include "Clients.h"
#include <pqxx/pqxx>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>	
#include <boost/serialization/utility.hpp>

using namespace boost::archive;

class ClientLogic : public Client
{
private:
	std::string acc_data_buffer;
	std::string file_msg_buffer;
	std::string txt_msg_buffer;

	Client::ptr this_client;

	void read_file_msg_sock(const error_code& err, size_t bytes); //

	void read_txt_msg_sock(const error_code& err, size_t bytes); //

	void read_acc_data_sock(const error_code& err, size_t bytes); //

	size_t read_complete(std::string& msg, const error_code& err, size_t bytes); //

	void acc_data_distributor(const error_code& err, size_t bytes); //

	void txt_msg_distributor(const error_code& err, size_t bytes); //

	void file_msg_distributor(const error_code& err, size_t bytes); //

	void change_avatar(std::string& buff); //

	void add_photo(std::string& buff); //

	void delete_photo(std::string& buff); //

	void user_list(); //

	void user_acc(std::string& buff); //

	void new_chat(std::string& buff); //

public:
	ClientLogic(Client::ptr client_ptr);
	~ClientLogic();

	void start_read_file_msg_sock();

	void set_default_settings(); //

	void extract_existing_acc_data(); //

	void update_db_acc_data(std::string& ser_vec); //

	std::string send_acc_data(); //

	void send_chat_info(); //

	void send_txt_msg(std::string& buff); //

	void send_chat(std::string& buff); //

	void send_file_list(std::string& buff); //

	void send_file(std::string& buff); //

	void receive_file(std::string& buff); //

	typedef boost::shared_ptr<Client> ptr;

};

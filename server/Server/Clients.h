#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>

using namespace boost::asio;
using namespace boost::system;


class Client : public boost::enable_shared_from_this<Client>
{
private:
	int User_id;
	std::string Nickname;
	std::string Password;

public:
	Client(int id, std::string nickname, std::string password) : User_id(id), Nickname(nickname), Password(password),
		txt_msg_sock(nullptr), file_msg_sock(nullptr), acc_data_sock(nullptr) 
	{
		std::cout << "new client" << std::endl;
	}

	~Client() {}

	typedef boost::shared_ptr<Client> ptr;

	static std::vector<Client> clients_vector;
	static std::vector<Client::ptr> clients_ptr_vector;
	std::vector<std::string> client_acc_data_vector;
	std::vector<std::string> client_file_msg_vector;
	//each subvector contains txt chats relative to each individual user
	std::vector<std::vector<std::string>> client_txt_chats_vector;
	//-------------------------------------------


	//SelfPointer
	ptr self_pointer(){ return shared_from_this(); }

	//user data getters
	const int get_UserId() { return this->User_id; }
	const std::string get_nickname() { return this->Nickname; }
	const std::string get_password() { return this->Password; }

	//socket setters
	void set_txt_msg_sock(ip::tcp::socket& socket) { txt_msg_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket)); }
	void set_file_msg_sock(ip::tcp::socket& socket) { file_msg_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket)); }
	void set_acc_data_sock(ip::tcp::socket& socket) { acc_data_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket)); }

	//sockets
	boost::shared_ptr<ip::tcp::socket> txt_msg_sock;
	boost::shared_ptr<ip::tcp::socket> file_msg_sock;
	boost::shared_ptr<ip::tcp::socket> acc_data_sock;

	static io_context servise;
	
};


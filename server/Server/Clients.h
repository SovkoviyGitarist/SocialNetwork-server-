#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <thread>

class ClientLogic;

using namespace boost::asio;
using namespace boost::system;


class Client : public boost::enable_shared_from_this<Client>
{
protected:
	int User_id;
	std::string Nickname;
	std::string Password;

	bool txt_msg_sock_flag;
	bool file_msg_sock_flag;
	bool acc_data_sock_flag;

	Client() {}

public:

	Client(int id, std::string nickname, std::string password) : User_id(id), Nickname(nickname), Password(password),
		txt_msg_sock(nullptr), file_msg_sock(nullptr), acc_data_sock(nullptr), txt_msg_sock_flag(false), file_msg_sock_flag(false), acc_data_sock_flag(false)
	{
		std::cout << "new client" << std::endl;
	}

	~Client() 
	{
		std::cout << "distract new client" << std::endl;
	}

	typedef boost::shared_ptr<Client> ptr;

	static std::vector<Client::ptr> clients_ptr_vector;

	boost::shared_ptr<ClientLogic> this_logic;

	std::vector<std::string> acc_data; //contains client account data
	std::vector<std::pair<std::string, std::string>> chats_info; //contains pairs with id and nickname of users, which have chat with client
	std::pair<std::string, std::string> file; //container for file name and file data

	//each subvector below contains chats/file names relative to each individual user
	std::vector<std::vector<std::string>> file_names;
	std::vector<std::vector<std::string>> chats;

	//-------------------------------------------


	void logic_pointer(boost::shared_ptr<ClientLogic> self_pointer) { this->this_logic = self_pointer; }


	//user data getters
	const int get_UserId() { return this->User_id; }
	const std::string get_nickname() { return this->Nickname; }
	const std::string get_password() { return this->Password; }

	//socket setters
	void set_txt_msg_sock(ip::tcp::socket& socket) { this->txt_msg_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket)); this->txt_msg_sock_flag = true;}
	void set_file_msg_sock(ip::tcp::socket& socket) { this->file_msg_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket)); this->file_msg_sock_flag = true;}
	void set_acc_data_sock(ip::tcp::socket& socket) { this->acc_data_sock = boost::make_shared<ip::tcp::socket>(boost::move(socket));  this->acc_data_sock_flag = true;}

	//sockets
	boost::shared_ptr<ip::tcp::socket> txt_msg_sock;
	boost::shared_ptr<ip::tcp::socket> file_msg_sock;
	boost::shared_ptr<ip::tcp::socket> acc_data_sock;

	static io_context servise;

	static io_context acc_service;
	static io_context file_service;
	static io_context txt_service;

	static boost::thread acc_thread;
	static boost::thread file_thread;
	static boost::thread txt_thread;
};

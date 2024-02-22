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

using namespace boost::asio;
using namespace boost::system;


class Client : public boost::enable_shared_from_this<Client>
{
protected:
	int User_id;
	std::string Nickname;
	std::string Password;

	std::vector<std::string> acc_data; //contains client account data
	std::vector<std::pair<std::string, std::string>> chats_info; //contains pairs with id and nickname of users, which have chat with client
	std::pair<std::string, std::string> file; //container for file name and file data

	//each subvector below contains chats/file names relative to each individual user
	std::vector<std::vector<std::string>> file_names;
	std::vector<std::vector<std::string>> chats;

	Client() {}

public:

	Client(int id, std::string nickname, std::string password) : User_id(id), Nickname(nickname), Password(password),
		txt_msg_sock(nullptr), file_msg_sock(nullptr), acc_data_sock(nullptr) 
	{
		std::cout << "new client" << std::endl;
	}

	~Client() 
	{
		std::cout << "distract new client" << std::endl;
	}

	typedef boost::shared_ptr<Client> ptr;

	static std::vector<Client> clients_vector;
	static std::vector<Client::ptr> clients_ptr_vector;

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


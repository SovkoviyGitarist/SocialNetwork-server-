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
	std::string password;

public:
	Client(int Id, std::string Nickname, std::string password) : User_id(Id), Nickname(Nickname), password(password),
		txt_msg_sock(nullptr), file_msg_sock(nullptr), acc_data_sock(nullptr) {}

	~Client() {}

	typedef boost::shared_ptr<Client> ptr;

	//SelfPointer for new clients
	static ptr new_client(int Id, std::string Nickname, std::string password)
	{
		ptr new_client = boost::make_shared<Client>(Id, Nickname, password);
		return new_client; 
	}

	//SelfPointer
	ptr self_pointer(){ return shared_from_this(); }

	//user data getters
	const int get_UserId() { return this->User_id; }
	const std::string get_nickname() { return this->Nickname; }
	const std::string get_password() { return this->password; }

	//socket setters
	void set_txt_msg_sock(ip::tcp::socket&& socket) { txt_msg_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }
	void set_file_msg_sock(ip::tcp::socket&& socket) { file_msg_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }
	void set_acc_data_sock(ip::tcp::socket&& socket) { acc_data_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }

	//sockets
	std::unique_ptr<ip::tcp::socket> txt_msg_sock;
	std::unique_ptr<ip::tcp::socket> file_msg_sock;
	std::unique_ptr<ip::tcp::socket> acc_data_sock;

	static io_context servise;
	
};


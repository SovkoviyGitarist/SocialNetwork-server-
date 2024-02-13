#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;


class Client
{
private:
	int User_id;
	std::string Nickname;
	std::string password;
	~Client() {}

public:
	Client(int Id, std::string Nickname, std::string password) : User_id(Id), Nickname(Nickname), password(password),
		txt_msg_sock(nullptr), file_msg_sock(nullptr), acc_data_sock(nullptr) {}

	//user data getters
	int get_UserId() { return this->User_id; }
	std::string get_nickname() { return this->Nickname; }
	std::string get_password() { return this->password; }

	//socket setters
	void set_txt_msg_sock(ip::tcp::socket&& socket) { txt_msg_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }
	void set_file_msg_sock(ip::tcp::socket&& socket) { file_msg_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }
	void set_acc_data_sock(ip::tcp::socket&& socket) { acc_data_sock = std::make_unique<ip::tcp::socket>(std::move(socket)); }

	//sockets
	std::unique_ptr<ip::tcp::socket> txt_msg_sock;
	std::unique_ptr<ip::tcp::socket> file_msg_sock;
	std::unique_ptr<ip::tcp::socket> acc_data_sock;

};
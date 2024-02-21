#pragma once
#include "ClientLogic.h"
#include <boost/uuid/detail/md5.hpp>

using namespace boost::uuids::detail;


class Distributor : public boost::enable_shared_from_this<Distributor>
{
private:
	int UserId;
	std::string command;
    std::pair<std::string, std::string> nick_pass;

	void make_new_user(std::string& nickname, std::string& password);

	void split_command();

	std::string hash_func(const std::string& password);

public:
	Distributor(std::string &UserId, std::string &command, boost::shared_ptr<ip::tcp::socket> sock);
	~Distributor();

    boost::shared_ptr<ip::tcp::socket> sock_ptr;

    typedef boost::shared_ptr<Distributor> ptr;
    //-------------------------------------------

	void execute_command();

};
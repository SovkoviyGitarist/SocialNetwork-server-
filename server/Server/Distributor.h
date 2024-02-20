#pragma once
#include "Clients.h"
#include <pqxx/pqxx>
#include <boost/regex.hpp>


class Distributor : public boost::enable_shared_from_this<Distributor>
{
private:
	static std::vector<Client> clients_vector;
	static std::vector<Client::ptr> clients_ptr_vector;
	std::string UserId;
	std::string command;

public:
	Distributor(std::string &UserId, std::string &command, boost::shared_ptr<ip::tcp::socket> sock);
	~Distributor();

    boost::shared_ptr<ip::tcp::socket> sock_ptr;

    typedef boost::shared_ptr<Distributor> ptr;
    //-------------------------------------------


	void execute_command();

};


/*try
{
    pqxx::connection conn("dbname = SN_DB user = postgres password = root hostaddr = 127.0.0.1 port = 5432");
    if (conn.is_open()) {
        std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
    }
    else {
        std::cout << "Can't open database" << std::endl;
        return;
    }
    pqxx::work query(conn);
    pqxx::result result = query.exec("SELECT * FROM public.spendings ORDER BY id ASC");

    for (const auto& row : result)
    {
        std::cout << "ID: " << row["id"].as<int>() << ", Price " << row["price"].as<int>() << ", Time " << row["created_at"].as<std::string>() << std::endl;
    }
    query.commit();
}

catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return;
}*/
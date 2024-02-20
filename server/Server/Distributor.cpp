#include "Distributor.h"


Distributor::Distributor(std::string& UserId, std::string& command, boost::shared_ptr<ip::tcp::socket> sock) : UserId(UserId), command(command), sock_ptr{sock} {}

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
		boost::regex reg (":");
		boost::sregex_token_iterator iter(this_distributor->command.begin(), this_distributor->command.end(), reg, -1);
		this_distributor->nick_pass.first = *(++iter);
		this_distributor->nick_pass.second = *(++iter);

		make_new_user(this_distributor->nick_pass.first, this_distributor->nick_pass.second);
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

void Distributor::make_new_user(std::string &nickname, std::string &password)
{
	ptr this_distributor = shared_from_this();

	try
	{
		//create new user in database and then get his id from there
		pqxx::connection conn("dbname = SN_DB user = postgres password = root hostaddr = 127.0.0.1 port = 5432");
		if (conn.is_open()) {
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
		}
		else {
			std::cout << "Can't open database" << std::endl;
			return;
		}
		pqxx::work query(conn);
		query.exec("INSERT INTO public.users(nickname, password) VALUES ('" + nickname + "', '" + password + "');");
		pqxx::result result = query.exec("SELECT id FROM users WHERE nickname = '" + nickname + "';");

		this_distributor->UserId = result.front()["id"].as<std::string>();

		query.commit();
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}

	//Create new user as an object, push him and his reference into vector
	Client::ptr new_client = boost::make_shared<Client>(std::stoi(this_distributor->UserId), nickname);
	Client::clients_vector.push_back(*new_client);
	Client::clients_ptr_vector.push_back(new_client);
	new_client->set_acc_data_sock(*(this_distributor->sock_ptr));
}



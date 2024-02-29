#include "Distributor.h"


Distributor::Distributor(std::string& UserId, std::string& command, boost::shared_ptr<ip::tcp::socket> sock) : UserId(std::stoi(UserId)), command(command), sock_ptr{sock} 
{
	std::cout << "new Distributor" << std::endl;
}

Distributor::~Distributor()
{
	std::cout << "Distract new Distributor" << std::endl;
}

void Distributor::execute_command()
{
	boost::regex new_client_expr("make_new_client");
	boost::regex existing_client_expr("connect_to_existing");
	boost::regex txt_sock_expr("connect_to_txt_msg_sock");
	boost::regex file_sock_expr("connect_to_file_msg_sock");

	if (boost::regex_search(this_distributor->command, new_client_expr)) //write a unick nickname check
	{
		split_command();

		Client::ptr new_client = make_new_user(this_distributor->nick_pass.first, this_distributor->nick_pass.second);

		Client::acc_service.post([&new_client]() {new_client->this_logic->set_default_settings(); }); //start sending of acc data to user in special thread

		this_distributor.reset();
		return;
	}

	else if (boost::regex_search(this_distributor->command, existing_client_expr))
	{
		split_command();

		for (auto& client_ptr : Client::clients_ptr_vector) //rewrite to binary search with help of Id !!!
		{
			if ((client_ptr->get_nickname()) == (this_distributor->nick_pass.first) && (client_ptr->get_password()) == (this_distributor->nick_pass.second))
			{
				client_ptr->set_acc_data_sock(*(this_distributor->sock_ptr));

				boost::shared_ptr<ClientLogic> new_logic = boost::make_shared<ClientLogic>(client_ptr);
				client_ptr->logic_pointer(new_logic);
				
				Client::acc_service.post([&client_ptr]() {client_ptr->this_logic->send_acc_data(); }); //start sending of acc data to user in special thread

				this_distributor.reset();
				return;
			}
			else
			{
				std::string msg("ERROR: invalid nickname or password");
				async_write(*(this_distributor->sock_ptr), buffer(msg.c_str(), msg.size()), [](const error_code& err, size_t bytes) {err ? 1 : 0;});//check callback
				this_distributor.reset();
				return;
			}
		}
	}

	else if (boost::regex_search(this_distributor->command, txt_sock_expr))
	{
		for (auto& client_ptr : Client::clients_ptr_vector) //rewrite to binary search with help of Id!!!
		{
			if ((this_distributor->UserId) == (client_ptr->get_UserId()))
			{
				client_ptr->set_txt_msg_sock(*(this_distributor->sock_ptr));

				Client::txt_service.post([&client_ptr]() {client_ptr->this_logic->send_chat_list(); });// write function to send chat list to client in special thread

				this_distributor.reset();
				return;
			}
		}

	}

	else if (boost::regex_search(this_distributor->command, file_sock_expr))
	{
		for (auto& client_ptr : Client::clients_ptr_vector) //rewrite to binary search with help of Id!!!
		{
			if ((this_distributor->UserId) == (client_ptr->get_UserId()))
			{
				client_ptr->set_file_msg_sock(*(this_distributor->sock_ptr));

				
				Client::file_service.post([&client_ptr]() {client_ptr->this_logic->send_file_list(); });// write function to send files list to client in special thread

				this_distributor.reset();
				return;
			}
		}
	}
}



std::string Distributor::hash_func(const std::string& password)
{
	md5 hash;
	md5::digest_type digest;
	hash.process_bytes(password.data(), password.size());
	hash.get_digest(digest);
	const auto char_digest = reinterpret_cast<const char*>(&digest);
	std::string result;
	for (int i = 0; i < 16; i++)
	{
		result += std::to_string(char_digest[i]);
	}

	return result;
}


void Distributor::split_command()
{
	boost::regex reg(":");
	boost::sregex_token_iterator iter(this_distributor->command.begin(), this_distributor->command.end(), reg, -1);
	this_distributor->nick_pass.first = *(++iter);
	this_distributor->nick_pass.second = hash_func(*(++iter));
}




Client::ptr Distributor::make_new_user(std::string &nickname, std::string &password) //check this, because of new type of function
{
	try
	{
		//create new user in database and then get his id from there
		pqxx::connection conn("dbname = SN_DB user = postgres password = root hostaddr = 127.0.0.1 port = 5432");
		if (conn.is_open()) 
		{
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
		}
		else 
			throw std::runtime_error("Can't open database");

		pqxx::work query(conn);
		query.exec("INSERT INTO public.users(nickname, password) VALUES ('" + nickname + "', '" + password + "');");
		pqxx::result result = query.exec("SELECT id FROM users WHERE nickname = '" + nickname + "';");

		this_distributor->UserId = result.front()["id"].as<int>();

		query.commit();
		
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		throw; //write better exception later
	}

	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw; //write better exception later
	}

	//Create new user as an object and push his reference into vector
	Client::ptr new_client = boost::make_shared<Client>(this_distributor->UserId, nickname, password);

	new_client->set_acc_data_sock(*(this_distributor->sock_ptr));

	Client::clients_ptr_vector.push_back(new_client);
			
	//creating of new logic object and linking it with current client
	boost::shared_ptr<ClientLogic> new_logic = boost::make_shared<ClientLogic>(new_client);
	new_client->logic_pointer(new_logic);

	return new_client;
}


void Distributor::self_pointer(Distributor::ptr self_pointer)
{
	this_distributor = self_pointer;
}





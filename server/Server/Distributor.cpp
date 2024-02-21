#include "Distributor.h"


Distributor::Distributor(std::string& UserId, std::string& command, boost::shared_ptr<ip::tcp::socket> sock) : UserId(std::stoi(UserId)), command(command), sock_ptr{sock} 
{
	std::cout << "new Distributor" << std::endl;
}

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
		split_command();

		make_new_user(this_distributor->nick_pass.first, this_distributor->nick_pass.second);
		// write function to send acc data to user or something like that
	}

	else if (boost::regex_search(this_distributor->command, existing_client_expr))
	{
		split_command();

		for (auto& client_ptr : Client::clients_ptr_vector)
		{
			if ((client_ptr->get_nickname()) == (this_distributor->nick_pass.first) && (client_ptr->get_password()) == (this_distributor->nick_pass.second))
			{
				client_ptr->set_acc_data_sock(*(this_distributor->sock_ptr));
				// write function to send acc data to user or something like that
			}
			else
			{
				std::string msg("ERROR: invalid nickname or password");
				async_write(*(this_distributor->sock_ptr), buffer(msg.c_str(), msg.size()), [](const error_code& err, size_t bytes) {err ? 0 : 1;});//check callback
			}
		}
	}

	else if (boost::regex_search(this_distributor->command, txt_sock_expr))
	{
		for (auto& client_ptr : Client::clients_ptr_vector)
		{
			if ((this_distributor->UserId) == (client_ptr->get_UserId()))
			{
				client_ptr->set_txt_msg_sock(*(this_distributor->sock_ptr));

				// write function to send text msg/chat list to user or something like that
			}
		}

	}

	else if (boost::regex_search(this_distributor->command, file_sock_expr))
	{
		for (auto& client_ptr : Client::clients_ptr_vector)
		{
			if ((this_distributor->UserId) == (client_ptr->get_UserId()))
			{
				client_ptr->set_file_msg_sock(*(this_distributor->sock_ptr));

				// write function to send existing files list to user or something like that
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
		result += char_digest[i];
	}
	return result;
}

void Distributor::split_command()
{
	ptr this_distributor = shared_from_this();
	boost::regex reg(":");
	boost::sregex_token_iterator iter(this_distributor->command.begin(), this_distributor->command.end(), reg, -1);
	this_distributor->nick_pass.first = *(++iter);
	this_distributor->nick_pass.second = hash_func(*(++iter));
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

		this_distributor->UserId = result.front()["id"].as<int>();

		query.commit();
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}

	//Create new user as an object, push him and his reference into vector
	Client::ptr new_client = boost::make_shared<Client>(this_distributor->UserId, nickname, password);
	Client::clients_vector.push_back(*new_client);
	Client::clients_ptr_vector.push_back(new_client);
	new_client->set_acc_data_sock(*(this_distributor->sock_ptr));
	// write function to send acc data to user or something like that

}





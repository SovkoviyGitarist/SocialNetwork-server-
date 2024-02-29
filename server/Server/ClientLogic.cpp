#include "ClientLogic.h"

#define MEM_FN(x) boost::bind(&ClientLogic::x, this_client->this_logic)
#define MEM_FN1(x,y) boost::bind(&ClientLogic::x, this_client->this_logic, y)
#define MEM_FN2(x,y,z) boost::bind(&ClientLogic::x, this_client->this_logic, y, z)




ClientLogic::ClientLogic(Client::ptr client_ptr) : this_client(client_ptr)
{
	std::cout << "new logic" << std::endl;
}

ClientLogic::~ClientLogic()
{
	std::cout << "distract new logic" << std::endl;
}


void ClientLogic::read_acc_data_sock(const error_code& err, size_t bytes)
{
	this_client->this_logic->acc_data_buffer.clear();
	async_read(*(this_client->acc_data_sock), buffer(this_client->this_logic->acc_data_buffer), MEM_FN2(read_complete, _1, _2), MEM_FN2(acc_data_distributor, _1, _2));//check buffer
}

size_t ClientLogic::read_complete(const error_code& err, size_t bytes)
{
	boost::regex reg("END_OF_MESSAGE");
	if (err)
		return 0;
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, reg))
		return 0;
	else
		return 1;
}

void ClientLogic::acc_data_distributor(const error_code& err, size_t bytes)
{
	boost::regex change_avatar("change_avatar");
	boost::regex add_photo("add_photo");
	boost::regex delete_photo("delete_photo");
	boost::regex user_list("user_list");
	boost::regex user_acc("user_acc");
	boost::regex new_chat("new_chat");
	
	if (boost::regex_search(this_client->this_logic->acc_data_buffer, change_avatar))
		this_client->this_logic->change_avatar();
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, add_photo))
		this_client->this_logic->add_photo();
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, delete_photo))
		this_client->this_logic->delete_photo();
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, user_list))
		this_client->this_logic->user_list();
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, user_acc))
		this_client->this_logic->user_acc();
	else if (boost::regex_search(this_client->this_logic->acc_data_buffer, new_chat))
		this_client->this_logic->new_chat();
}

void ClientLogic::change_avatar()
{
}

void ClientLogic::add_photo()
{
}

void ClientLogic::delete_photo()
{
}

void ClientLogic::user_list()
{
}

void ClientLogic::user_acc()
{
}

void ClientLogic::new_chat()
{
}


void ClientLogic::send_acc_data()
{
	try
	{
		pqxx::connection conn("dbname = SN_DB user = postgres password = root hostaddr = 127.0.0.1 port = 5432");
		if (conn.is_open())
		{
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
		}
		else
			throw std::runtime_error("Can't open database");

		pqxx::work query(conn);
		pqxx::result result = query.exec("SELECT acc_data_vector FROM public.useraccdata WHERE user_id="+ std::to_string(this_client->get_UserId()) +";");

		this_client->acc_data.clear();

		std::stringstream des_vec(result.front()["acc_data_vector"].as<std::string>());
		binary_iarchive ia(des_vec);
		ia >> this_client->acc_data;

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

	std::stringstream ser_vec;
	binary_oarchive oa(ser_vec);
	oa << this_client->acc_data;
	std::string msg("acc_data|" + ser_vec.str());
	async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));
}

void ClientLogic::send_chat_list()
{
}

void ClientLogic::send_file_list()
{
}


void ClientLogic::set_default_settings()
{
	this_client->acc_data = { "Avatar|NULL", "Nickname|"+this_client->get_nickname(), "Photos|NULL", "Id|" + std::to_string(this_client->get_UserId())};
	std::stringstream ser_vec;
	binary_oarchive oa(ser_vec);
	oa << this_client->acc_data;

	try
	{
		pqxx::connection conn("dbname = SN_DB user = postgres password = root hostaddr = 127.0.0.1 port = 5432");
		if (conn.is_open())
		{
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
		}
		else
			throw std::runtime_error("Can't open database");

		pqxx::work query(conn);
		query.exec("INSERT INTO public.useraccdata (user_id, acc_data_vector) VALUES("+ std::to_string(this_client->get_UserId()) +", " + ser_vec.str() + "); ");

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

	this_client->this_logic->send_acc_data();
}



#include "ClientLogic.h"

#define MEM_FN(x) boost::bind(&ClientLogic::x, this_client->this_logic)
#define MEM_FN1(x,y) boost::bind(&ClientLogic::x, this_client->this_logic, y)
#define MEM_FN2(x,y,z) boost::bind(&ClientLogic::x, this_client->this_logic, y, z)
#define MEM_FN3(x,y,z,w) boost::bind(&ClientLogic::x, this_client->this_logic, y, z, w)



ClientLogic::ClientLogic(Client::ptr client_ptr) : this_client(client_ptr)
{
	std::cout << "new logic" << std::endl;
}

ClientLogic::~ClientLogic()
{
	std::cout << "distract new logic" << std::endl;
}

void ClientLogic::start_read_file_msg_sock()
{
	this_client->this_logic->file_msg_buffer.clear();
	async_read(*(this_client->file_msg_sock), buffer(this_client->this_logic->file_msg_buffer), MEM_FN3(read_complete, this_client->this_logic->file_msg_buffer, _1, _2), MEM_FN2(file_msg_distributor, _1, _2));//check buffer
}

void ClientLogic::read_file_msg_sock(const error_code& err, size_t bytes)
{
	this_client->this_logic->file_msg_buffer.clear();
	async_read(*(this_client->file_msg_sock), buffer(this_client->this_logic->file_msg_buffer), MEM_FN3(read_complete, this_client->this_logic->file_msg_buffer, _1, _2), MEM_FN2(file_msg_distributor, _1, _2));//check buffer
}

void ClientLogic::read_txt_msg_sock(const error_code& err, size_t bytes)
{
	this_client->this_logic->txt_msg_buffer.clear();
	async_read(*(this_client->txt_msg_sock), buffer(this_client->this_logic->txt_msg_buffer), MEM_FN3(read_complete, this_client->this_logic->txt_msg_buffer, _1, _2), MEM_FN2(txt_msg_distributor, _1, _2));//check buffer
}

void ClientLogic::read_acc_data_sock(const error_code& err, size_t bytes)
{
	this_client->this_logic->acc_data_buffer.clear();
	async_read(*(this_client->acc_data_sock), buffer(this_client->this_logic->acc_data_buffer), MEM_FN3(read_complete, this_client->this_logic->acc_data_buffer, _1, _2), MEM_FN2(acc_data_distributor, _1, _2));//check buffer
}

size_t ClientLogic::read_complete(std::string &msg, const error_code& err, size_t bytes)
{
	boost::regex reg("END_OF_MESSAGE");
	if (err)
		return 0;
	else if (boost::regex_search(msg, reg))
		return 0;
	else
		return 1;
}

void ClientLogic::acc_data_distributor(const error_code& err, size_t bytes)
{
	std::string buffer = std::move(this_client->this_logic->acc_data_buffer);

	boost::regex change_avatar("change_avatar");
	boost::regex add_photo("add_photo");
	boost::regex delete_photo("delete_photo");
	boost::regex user_list("user_list");
	boost::regex user_acc("user_acc");
	boost::regex new_chat("new_chat");
	
	if (boost::regex_search(buffer, change_avatar))
		this_client->this_logic->change_avatar(buffer);
	else if (boost::regex_search(buffer, add_photo))
		this_client->this_logic->add_photo(buffer);
	else if (boost::regex_search(buffer, delete_photo))
		this_client->this_logic->delete_photo(buffer);
	else if (boost::regex_search(buffer, user_list))
		this_client->this_logic->user_list();
	else if (boost::regex_search(buffer, user_acc))
		this_client->this_logic->user_acc(buffer);
	else if (boost::regex_search(buffer, new_chat))
		this_client->this_logic->new_chat(buffer);
}

void ClientLogic::txt_msg_distributor(const error_code& err, size_t bytes)
{
	std::string buffer = std::move(this_client->this_logic->txt_msg_buffer);

	boost::regex txt_message("txt_message");
	boost::regex chats_info("chats_info");
	boost::regex get_chat("get_chat");

	if (boost::regex_search(buffer, txt_message))
		this_client->this_logic->send_txt_msg(buffer);
	else if (boost::regex_search(buffer, chats_info))
		this_client->this_logic->send_chat_info();
	else if (boost::regex_search(buffer, get_chat))
		this_client->this_logic->send_chat(buffer);
}

void ClientLogic::file_msg_distributor(const error_code& err, size_t bytes)
{
	std::string buffer = std::move(this_client->this_logic->file_msg_buffer);

	boost::regex get_file_list("get_file_list");
	boost::regex send_file("send_file");
	boost::regex get_file("get_file");

	if (boost::regex_search(buffer, get_file_list))
		this_client->this_logic->send_file_list(buffer);
	else if (boost::regex_search(buffer, send_file))
		this_client->this_logic->send_file(buffer);
	else if (boost::regex_search(buffer, get_file))
		this_client->this_logic->receive_file(buffer);
}

void ClientLogic::change_avatar(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);
	this_client->acc_data[0] = "Avatar|"+ *(++iter);

	std::string ser_vec = std::move(this_client->this_logic->send_acc_data());

	this_client->this_logic->update_db_acc_data(ser_vec);
}

void ClientLogic::add_photo(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);
	std::string ser_photo, date;
	ser_photo = *(++iter);
	date = *(++iter);

	if (this_client->acc_data[2] == "Photos|NULL")
		this_client->acc_data[2] = "Photos|" + date + "~" + ser_photo + "|";

	else
		this_client->acc_data[2] += date + "~" + ser_photo + "|";

	std::string ser_vec = std::move(this_client->this_logic->send_acc_data());

	this_client->this_logic->update_db_acc_data(ser_vec);
}

void ClientLogic::delete_photo(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);
	std::string date = *(++iter);

	boost::regex del("\\|" + date + "~\S*\\|"); //chek this expr
	this_client->acc_data[2] = std::move(boost::regex_replace(this_client->acc_data[2], del, "|"));

	std::string ser_vec = std::move(this_client->this_logic->send_acc_data());

	this_client->this_logic->update_db_acc_data(ser_vec);
}

void ClientLogic::user_list()
{
	std::vector<std::pair<std::string, std::string>> user_list;

	for (const auto& client : Client::clients_ptr_vector)
	{
		std::pair<std::string, std::string> id_nick(std::to_string(client->get_UserId()), client->get_nickname());
		user_list.push_back(std::move(id_nick));
	}

	std::stringstream ser_vec;
	binary_oarchive oa(ser_vec);
	oa << user_list;
	std::string msg("user_list|" + ser_vec.str());
	async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));
}

void ClientLogic::user_acc(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);

	std::vector<std::string> user_acc_data;

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
		pqxx::result result = query.exec("SELECT acc_data_vector FROM public.useraccdata WHERE user_id=" + *(++iter) + ";");

		user_acc_data.push_back(result.front()["acc_data_vector"].as<std::string>());

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
	oa << user_acc_data;
	std::string msg("user_acc|" + *iter + "|" + ser_vec.str());
	async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));
}

void ClientLogic::new_chat(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);
	int user_id = std::stoi(*(++iter));
	
	
		bool chat{ false };
		std::string msg;

		for (const auto& info : this_client->chats_info)
		{
			if (info.first == user_id)
			{
				chat = true;
				break;
			}
		}

		if (chat)
		{
			msg = "new_chat|chat_exist";
			async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));
		}
		else
		{
			msg = "new_chat|made_new_chat";

			Client::txt_service.post([&]()
			{
				bool user_online{ false };
				std::pair<int, std::string> id_nick;
				std::pair<int, std::string> this_id_nick(this_client->get_UserId(), this_client->get_nickname());
				Client::ptr user;

				std::stringstream ser_chat_info;
				binary_oarchive oas(ser_chat_info);

				//find ptr of user, with whom will be new chat
				for (const auto& client : Client::clients_ptr_vector)
				{
					if (client->get_UserId() == user_id)
					{
						user = client;

						//geting user`s id and nick, updating current client`s chats_info
						id_nick.first = client->get_UserId();
						id_nick.second = client->get_nickname();
						this_client->chats_info.push_back(id_nick);

						if (client->acc_data_sock->is_open()) //if user online - update his chats_info and serialize it
						{
							client->chats_info.push_back(this_id_nick);
							oas << client->chats_info;
							user_online = true;
						}
						else //if user offline - get his chats_info from DB, update it and update DB
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
								pqxx::result result = query.exec("SELECT chat_names_vector FROM public.chats_info WHERE user_id="+ std::to_string(id_nick.first) +";");

								//deserialization of offline user`s chats_info
								std::stringstream des_vec(result.front()["chat_names_vector"].as<std::string>());
								binary_iarchive ia(des_vec);
								client->chats_info.clear();
								ia >> client->chats_info;

								//updating of chats_info
								client->chats_info.push_back(this_id_nick);

								//serialization of chats_info
								oas << client->chats_info;

								//updating DB
								query.exec("UPDATE public.chats_info SET chat_names_vector=" + ser_chat_info.str() + " WHERE user_id=" + std::to_string(id_nick.first) + ";");

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
						}
						break;
					}
			    }

				//create new chat and push it in chats vector of current client
				std::vector<std::string> new_chat;
				new_chat.push_back(std::to_string(this_client->get_UserId()) + "|" + std::to_string(user_id));
				this_client->chats.push_back(new_chat);

				//serialize new chat
				std::stringstream this_chat;
				binary_oarchive oac(this_chat);
				oac << new_chat;

				//serialize updated chats_info of current client
				std::stringstream this_ser_chat_info;
				binary_oarchive oat(this_ser_chat_info);
				oat << this_client->chats_info;



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

				//update chat_info of current client
				query.exec("UPDATE public.chats_info SET chat_names_vector=" + this_ser_chat_info.str() + " WHERE user_id=" + std::to_string(this_id_nick.first) + ";");

				//save new chat between two clients in DB
				query.exec("INSERT INTO public.chats(user_1_id, user_2_id, chat_vector) VALUES ("+ std::to_string(this_id_nick.first) +", "+ std::to_string(id_nick.first) + ", "+ this_chat.str() + ");");

				//if user online - update his chat_info
				if (user_online)
					query.exec("UPDATE public.chats_info SET chat_names_vector=" + ser_chat_info.str() + " WHERE user_id=" + std::to_string(id_nick.first) + ";");


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
			
				//send chats_info and new chat to current client
				std::string client_chat_info_msg("chat_info|" + this_ser_chat_info.str());
				async_write(*(this_client->txt_msg_sock), buffer(client_chat_info_msg.c_str(), client_chat_info_msg.size()), [&](const boost::system::error_code& err, std::size_t bytes)
				{
					std::string client_chat_msg("new_chat|" + this_chat.str()); //check this msg
					async_write(*(this_client->txt_msg_sock), buffer(client_chat_msg.c_str(), client_chat_msg.size()), MEM_FN2(read_txt_msg_sock, _1, _2));
				});
				 
				//send chats_info and new chat to user, if he online
				if (user_online)
				{
					std::string user_chat_info_msg("chat_info|" + ser_chat_info.str());
					async_write(*(user->txt_msg_sock), buffer(user_chat_info_msg.c_str(), user_chat_info_msg.size()), [&](const boost::system::error_code& err, std::size_t bytes)
					{
						std::string user_chat_msg("new_chat|" + this_chat.str()); //check this msg
						async_write(*(user->txt_msg_sock), buffer(user_chat_msg.c_str(), user_chat_msg.size()), boost::bind(&ClientLogic::read_txt_msg_sock, user->this_logic, _1, _2));
					});
				}
			});

			async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));
		}
}


void ClientLogic::extract_existing_acc_data()
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

	this_client->this_logic->send_acc_data();
}

void ClientLogic::update_db_acc_data(std::string &ser_vec)
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
		query.exec("UPDATE public.useraccdata SET acc_data_vector=" + ser_vec + " WHERE user_id=" + std::to_string(this_client->get_UserId()) + ";");

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

std::string ClientLogic::send_acc_data()
{
	std::stringstream ser_vec;
	binary_oarchive oa(ser_vec);
	oa << this_client->acc_data;
	std::string msg("acc_data|" + ser_vec.str());
	async_write(*(this_client->acc_data_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_acc_data_sock, _1, _2));

	return ser_vec.str();
}

void ClientLogic::send_chat_info()
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
		pqxx::result result = query.exec("SELECT chat_names_vector FROM public.chats_info WHERE user_id="+ std::to_string(this_client->get_UserId()) + ";");

		this_client->chats_info.clear();

		std::stringstream des_chat_info(result.front()["chat_names_vector"].as<std::string>());
		binary_iarchive ia(des_chat_info);
		ia >> this_client->chats_info;

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
	oa << this_client->chats_info;
	std::string msg("chats_info|" + ser_vec.str());
	async_write(*(this_client->txt_msg_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_txt_msg_sock, _1, _2));
}

void ClientLogic::send_txt_msg(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);
	
	int user_id = std::stoi(*(++iter));
	std::string msg = *(++iter);

	boost::shared_ptr<std::vector<std::string>> this_chat;

	std::string f_case(std::to_string(user_id) + "|" + std::to_string(this_client->get_UserId()));
	std::string s_case(std::to_string(this_client->get_UserId()) + "|" + std::to_string(user_id));

	for (auto &chats : this_client->chats) //find chat we need in client`s chats vector and update it
	{
		if (chats[1] == f_case || chats[1] == s_case)
		{
			chats.push_back(msg);
			this_chat = boost::make_shared<std::vector<std::string>>(chats);
			break;
		}
	}

	for (const auto& client : Client::clients_ptr_vector)
	{
		if (client->get_UserId() == user_id) //find user we need
		{
			if (client->txt_msg_sock->is_open()) //if user online - update his chats vector  and send msg to him
			{
				for (auto& chats : client->chats) //find chat we need in user`s chats vector and update it
				{
					if (chats[1] == f_case || chats[1] == s_case)
					{
						chats.push_back(msg);
						break;
					}
				}

				std::string message("txt_message|" + std::to_string(this_client->get_UserId()) + "|" + msg);
				async_write(*(client->txt_msg_sock), buffer(message.c_str(), message.size()), MEM_FN2(read_txt_msg_sock, _1, _2));

				break;
			}
			else //if user offline - update chat in DB
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

					//serialization of chat
					std::stringstream ser_vec;
					binary_oarchive oa(ser_vec);
					oa << *this_chat;

					//updating DB
					query.exec("UPDATE public.chats SET chat_vector="+ ser_vec.str() + " WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + ") OR (user_1_id=" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + ")");

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

				std::string msg("message_received");
				async_write(*(this_client->txt_msg_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_txt_msg_sock, _1, _2));

				break;
			}
		}
	}
}

void ClientLogic::send_chat(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);

	int user_id = std::stoi(*(++iter));
	std::vector<std::string> chat;
	std::string message;
	
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

		pqxx::result result = std::move(query.exec("SELECT chat_vector FROM public.chats WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + ") OR (user_1_id=" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + ")"));
		
		std::stringstream ser_chat (result.front()["chat_vector"].as<std::string>());
		binary_iarchive ia(ser_chat);
		ia >> chat;
		this_client->chats.push_back(chat);

		message = "chat|" + std::to_string(user_id) + "|" + ser_chat.str();

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

	async_write(*(this_client->txt_msg_sock), buffer(message.c_str(), message.size()), MEM_FN2(read_txt_msg_sock, _1, _2));
}

void ClientLogic::send_file_list(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);

	int user_id = std::stoi(*(++iter));
	std::string message;

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

		pqxx::result result = std::move(query.exec("SELECT file_names_vector FROM public.file_names WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + ") OR (user_1_id=" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + ")"));

		message = "file_list|" + std::to_string(user_id) + "|" + result.front()["file_names_vector"].as<std::string>();

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

	async_write(*(this_client->file_msg_sock), buffer(message.c_str(), message.size()), MEM_FN2(read_file_msg_sock, _1, _2));
}

void ClientLogic::send_file(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);

	int user_id = std::stoi(*(++iter));
	std::string filename = *(++iter);
	std::string ser_file = *(++iter);
	std::string message;

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

		//select old file names
		pqxx::result result = std::move(query.exec("SELECT file_names_vector FROM public.file_names WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + ") OR (user_1_id=" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + ")"));

		std::stringstream ser_file_names (result.front()["file_names_vector"].as<std::string>());
		binary_iarchive ia(ser_file_names);
		std::vector<std::string> file_names;
		ia >> file_names;

		file_names.push_back(filename);

		std::stringstream updated_file_names;
		binary_oarchive oa(updated_file_names);
		oa << file_names;

		//update file_names, which belongs to both clients
		query.exec("UPDATE public.file_names SET file_names_vector=" + updated_file_names.str() + " WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + ") OR (user_1_id=" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + ")");
		//insert new file`s data
		query.exec("INSERT INTO public.files (user_1_id, user_2_id, file_name, file_data) VALUES (" + std::to_string(this_client->get_UserId()) + ", " + std::to_string(user_id) + ", " + filename + ", " + ser_file + ")");

		message = "file_list|" + std::to_string(this_client->get_UserId()) + "|" + updated_file_names.str();

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

	for (const auto& client : Client::clients_ptr_vector)
	{
		if (client->get_UserId() == user_id) //find user we need
		{
			if (client->file_msg_sock->is_open()) //if user online - send new file list
			{
				async_write(*(client->file_msg_sock), buffer(message.c_str(), message.size()), MEM_FN2(read_file_msg_sock, _1, _2));

				break;
			}
			else //if user offline - notify client, that file was received
			{
				std::string msg("file_received");
				async_write(*(this_client->file_msg_sock), buffer(msg.c_str(), msg.size()), MEM_FN2(read_file_msg_sock, _1, _2));

				break;
			}
		}
	}
}

void ClientLogic::receive_file(std::string& buff)
{
	boost::regex reg("\\|");
	boost::sregex_token_iterator iter(buff.begin(), buff.end(), reg, -1);

	int user_id = std::stoi(*(++iter));
	std::string filename = *(++iter);
	std::string message;

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

		pqxx::result result = std::move(query.exec("SELECT file_data, date FROM public.files WHERE (user_1_id =" + std::to_string(user_id) + " AND user_2_id=" + std::to_string(this_client->get_UserId()) + " AND file_name='"+ filename +"') OR (user_1_id =" + std::to_string(this_client->get_UserId()) + " AND user_2_id=" + std::to_string(user_id) + " AND file_name='" + filename + "')"));

		message = "file|" + result.front()["file_data"].as<std::string>();

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
	
	async_write(*(this_client->file_msg_sock), buffer(message.c_str(), message.size()), MEM_FN2(read_file_msg_sock, _1, _2));
}



void ClientLogic::set_default_settings() 
{
	//set default settings of acc_data
	this_client->acc_data = { "Avatar|NULL", "Nickname|"+this_client->get_nickname(), "Photos|NULL", "Id|" + std::to_string(this_client->get_UserId())};
	std::stringstream ser_acc_data;
	binary_oarchive oaa(ser_acc_data);
	oaa << this_client->acc_data;

	//serialize chats_info
	std::stringstream ser_chats_info;
	binary_oarchive oac(ser_chats_info);
	oac << this_client->chats_info;

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
		//insert default acc_data in DB
		query.exec("INSERT INTO public.useraccdata (user_id, acc_data_vector) VALUES("+ std::to_string(this_client->get_UserId()) +", " + ser_acc_data.str() + "); ");
		//insert empty default chats_list in DB
		query.exec("INSERT INTO public.chats_info(user_id, chat_names_vector) VALUES (" + std::to_string(this_client->get_UserId()) + ", " + ser_chats_info.str() + ");");

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

	this_client->this_logic->extract_existing_acc_data();
}



#include "SocketFilter.h"

std::vector<boost::shared_ptr<Client>> Client::clients_ptr_vector;
io_context Client::servise;

io_context Client::acc_service;
io_context Client::file_service;
io_context Client::txt_service;

boost::thread Client::acc_thread([]() {acc_service.run(); });
boost::thread Client::file_thread([]() {file_service.run(); });
boost::thread Client::txt_thread([]() {txt_service.run(); });

ip::tcp::acceptor acceptor(Client::servise, ip::tcp::endpoint(ip::tcp::v4(), 8001));


void handle_accept(SockFilter::ptr filter, const error_code& err)
{
	filter->start();
	SockFilter::ptr new_filter = SockFilter::new_filter();
	acceptor.async_accept(new_filter->socket(), boost::bind(handle_accept, new_filter, _1));
}



int main()
{
	//autofill of clients_ptr_vector
	if (Client::clients_ptr_vector.empty())
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
			pqxx::result result = std::move(query.exec("SELECT * FROM public.users ORDER BY id ASC "));

			for (const auto& res : result)
			{
				boost::shared_ptr<Client> new_client = boost::make_shared<Client>(res["id"].as<int>(), res["nickname"].as<std::string>(), res["password"].as<std::string>());
				Client::clients_ptr_vector.push_back(std::move(new_client));
			}
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
	
	SockFilter::ptr filter = SockFilter::new_filter();
	acceptor.async_accept(filter->socket(), boost::bind(handle_accept, filter, _1));

	Client::acc_thread.join();
	Client::file_thread.join();
	Client::txt_thread.join();
	Client::servise.run();
	return 0;
}
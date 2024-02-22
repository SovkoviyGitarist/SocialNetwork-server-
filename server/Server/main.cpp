#include "SocketFilter.h"

std::vector<Client> Client::clients_vector;
std::vector<boost::shared_ptr<Client>> Client::clients_ptr_vector;
io_context Client::servise;

ip::tcp::acceptor acceptor(Client::servise, ip::tcp::endpoint(ip::tcp::v4(), 8001));


void handle_accept(SockFilter::ptr filter, const error_code& err)
{
	filter->start();
	SockFilter::ptr new_filter = SockFilter::new_filter();
	acceptor.async_accept(new_filter->socket(), boost::bind(handle_accept, new_filter, _1));
}



int main()
{
	//need to write a flag of online/offline user, so that server could know, send message data to user or just save it
	//And write auto fill of clients vector
	SockFilter::ptr filter = SockFilter::new_filter();
	acceptor.async_accept(filter->socket(), boost::bind(handle_accept, filter, _1));
	Client::servise.run();
}
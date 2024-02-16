#include "SocketFilter.h"


io_context Client::servise;
ip::tcp::acceptor acceptor(Client::servise, ip::tcp::endpoint(ip::tcp::v4(), 8001));


void handle_accept(SockFilter::ptr filter, const error_code& err)
{
	filter->start();
	SockFilter::ptr new_filter = SockFilter::new_filter();
	acceptor.async_accept(filter->socket(), boost::bind(handle_accept, filter, _1));
}

int main()
{
	SockFilter::ptr new_filter = SockFilter::new_filter();
	acceptor.async_accept(new_filter->socket(), boost::bind(handle_accept, new_filter, _1));
	Client::servise.run();
}
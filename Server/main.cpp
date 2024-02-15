#include "SocketFilter.h"




void handle_accept(io_ptr & servise_ptr, ip::tcp::acceptor &acceptor, SockFilter::ptr filter, const error_code& err)
{
	filter->start();
	SockFilter::ptr new_filter = SockFilter::new_filter(servise_ptr);
	acceptor.async_accept(filter->socket(), boost::bind(handle_accept, filter, _1));
}

int main()
{
	io_ptr servise_ptr = boost::make_shared<io_service>();
	ip::tcp::acceptor acceptor(*servise_ptr, ip::tcp::endpoint(ip::tcp::v4(), 8001));

	SockFilter::ptr new_filter = SockFilter::new_filter(servise_ptr);
	acceptor.async_accept(new_filter->socket(), boost::bind(handle_accept, servise_ptr, boost::ref(acceptor), new_filter, _1));
	servise_ptr->run();
}
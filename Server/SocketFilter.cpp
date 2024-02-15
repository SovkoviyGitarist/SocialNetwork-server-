#include "SocketFilter.h"

#define MEM_FN(x) boost::bind(&SockFilter::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&SockFilter::x, shared_from_this(), y)
#define MEM_FN2(x,y,z) boost::bind(&SockFilter::x, shared_from_this(), y, z)

SockFilter::SockFilter(io_ptr& servise_ptr) : buffer_socket(*servise_ptr), started(false) {}

SockFilter::~SockFilter() {}

SockFilter::ptr SockFilter::new_filter(io_ptr& servise_ptr)
{
	SockFilter::ptr new_filter = boost::make_shared<SockFilter>(servise_ptr);
	return new_filter;
}

ip::tcp::socket& SockFilter::socket()
{
	return buffer_socket;
}

void SockFilter::start()
{
	if (started)
		return;
	else
	{
		started = true;
		do_read();
	}
}

void SockFilter::do_read()
{
	async_read(buffer_socket, buffer(read_buffer), MEM_FN2(read_complete, _1, _2), MEM_FN2(SockFilter::on_read, _1, _2));
}

void SockFilter::on_read(const error_code& err, size_t bytes)
{
	servise_ptr->post(&SockFilter::distribute);
}

size_t SockFilter::read_complete(const error_code& err, size_t bytes)
{
	if (err)
		return 0;
	else
	{
		std::string msg(read_buffer, read_buffer + bytes);
		boost::regex reg("\\|");
		boost::sregex_token_iterator iter(msg.begin(), msg.end(), reg, -1);
		boost::sregex_token_iterator end;
		while (iter != end)
		{
			iter_vector.push_back(*iter);
		}
		return 1;
	}
}

void SockFilter::distribute()
{

}





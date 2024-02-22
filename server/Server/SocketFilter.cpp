#include "SocketFilter.h"

#define MEM_FN(x) boost::bind(&SockFilter::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&SockFilter::x, shared_from_this(), y)
#define MEM_FN2(x,y,z) boost::bind(&SockFilter::x, shared_from_this(), y, z)

SockFilter::SockFilter() : buffer_socket(Client::servise), started(false) 
{
	sock_ptr = boost::make_shared<ip::tcp::socket>(boost::move(buffer_socket));
	std::cout << "new filter" << std::endl;
}

SockFilter::~SockFilter() 
{
	std::cout << "Distract new filter" << std::endl;
}

SockFilter::ptr SockFilter::new_filter()
{
	SockFilter::ptr new_filter = boost::make_shared<SockFilter>();
	return new_filter;
}


ip::tcp::socket& SockFilter::socket()
{
	return *sock_ptr;
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
	async_read(*sock_ptr, buffer(read_buffer), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
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
		id_comm.first = *(iter);
		id_comm.second = *(++iter);
		return 1;
	}
}

void SockFilter::on_read(const error_code& err, size_t bytes)
{
	if (!err)
	{
		SockFilter::ptr this_filter = shared_from_this();
		Distributor::ptr new_distributor = boost::make_shared<Distributor>(this_filter->id_comm.first, this_filter->id_comm.second, sock_ptr );
		Client::servise.post([&new_distributor]() {new_distributor->execute_command(); }); //check this while testing
	}
	else
	{
		return;//write exception in future
	}
}






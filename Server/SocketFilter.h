#pragma once
#include "Clients.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

io_service service;

class SockFilter : public boost::enable_shared_from_this<SockFilter>
{
private:
	ip::tcp::socket buffer_socket;
	enum {max_size = 1024};
	char read_buffer[max_size];
	char write_buffer[max_size];
	bool started;

public:
	SockFilter();
	~SockFilter();

	typedef boost::shared_ptr<SockFilter> ptr;

	//SelfPointer for new filter
	static ptr new_filter();
	
	//Socket getter
	ip::tcp::socket& socket();

	void start();

	//read functions
	void do_read();
	size_t read_complete(const error_code& err, size_t bytes);
	void on_read(const error_code& err, size_t bytes);
	

	


};

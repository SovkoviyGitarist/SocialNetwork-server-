#pragma once
#include "Distributor.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>


class SockFilter : public boost::enable_shared_from_this<SockFilter>
{
private:
	ip::tcp::socket buffer_socket;
	std::vector<std::string> iter_vector;
	enum {max_size = 1024};
	char read_buffer[max_size];
	bool started;

	//read functions
	void do_read();
	size_t read_complete(const error_code& err, size_t bytes);

	void distribute(const error_code& err);
	
public:
	SockFilter();
	~SockFilter();

	typedef boost::shared_ptr<SockFilter> ptr;

	//SelfPointer for new filter
	static ptr new_filter();
	
	//Socket getter
	ip::tcp::socket& socket();

	void start();

};


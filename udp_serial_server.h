#ifndef __UDP_SERIAL_SERVER_H
#define __UDP_SERIAL_SERVER_H

#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>

#define UDP_SERVER_RCVBUF_SIZE 512

class udp_serial_server
{
protected:
	int fd;
	sig_atomic_t stop_pending;
	struct sockaddr peer;
	socklen_t peerlen;

public:
	explicit udp_serial_server(int);
	virtual ~udp_serial_server();

	udp_serial_server(const udp_serial_server&) = delete;
	udp_serial_server& operator =(const udp_serial_server&) = delete;

	void bind(const struct sockaddr*, socklen_t);

	void run();
	void stop();

protected:
	virtual void process(const void*, size_t) = 0;

	size_t reply(const void*, size_t);
};

#endif

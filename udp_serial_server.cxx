#include "globals.h"

#include <unistd.h>

#include "udp_serial_server.h"

// Don't know the header for this one
#define IPPROTO_UDP 17

#define self udp_serial_server

self::self(int domain) :
	fd(socket(domain, SOCK_DGRAM, IPPROTO_UDP)),
	stop_pending(0)
{
}

self::~self()
{
	close(fd);
}

void self::bind(const struct sockaddr* addr, socklen_t addrlen)
{
	::bind(fd, addr, addrlen);
}

void self::run()
{
	char buf[UDP_SERVER_RCVBUF_SIZE];
	while (!stop_pending)
	{
		peerlen = sizeof(peer);
		ssize_t len = recvfrom(fd, buf, UDP_SERVER_RCVBUF_SIZE, 0, &peer, &peerlen);
		if (stop_pending || len < 0)
			break;
		process(buf, (size_t)len);
	}
	stop_pending = 0;
}

void self::stop()
{
	stop_pending = 1;
}

size_t self::reply(const void* buf, size_t len)
{
	return sendto(fd, buf, len, 0, &peer, peerlen);
}

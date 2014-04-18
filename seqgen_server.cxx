#include "globals.h"

#include <sys/time.h>

#include "udp_serial_server.h"
#include "seqgen_server.h"

// Seconds between Jan 1 1970 and Jan 1 2010
#define SEQGEN_EPOCH 1262300400

// Sequence generator parameters
#define COUNTER_BITS 12
#define NODE_BITS 10

#define uint8_t unsigned char
#define uint64_t unsigned long long

#define self seqgen_server
#define parent udp_serial_server

namespace
{
	seqid_t time_seed()
	{
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		return ((tv.tv_sec - SEQGEN_EPOCH) * 1000ULL + tv.tv_usec / 1000ULL) << (COUNTER_BITS + NODE_BITS);
	}
	inline uint64_t htonll(uint64_t h)
	{
		uint64_t n;
		uint8_t* pn = (uint8_t*)&n;
		pn[0] = (uint8_t)(h >> 56);
		pn[1] = (uint8_t)(h >> 48);
		pn[2] = (uint8_t)(h >> 40);
		pn[3] = (uint8_t)(h >> 32);
		pn[4] = (uint8_t)(h >> 24);
		pn[5] = (uint8_t)(h >> 16);
		pn[6] = (uint8_t)(h >> 8);
		pn[7] = (uint8_t)h;
		return n;
	}
}

self::self(int domain) :
	parent(domain),
	nodeid(0U),
	curseed(0ULL)
{
}

seqid_t self::generate()
{
	reseed(time_seed());
	seqid_t id = curseed | nodeid;
	curseed += 1 << NODE_BITS;
	return id;
}

void self::process(const void* buf, size_t len)
{
	if (len == 0)
		return;
	unsigned char num = *(const unsigned char*)buf;
	if (!num || num > 64)
		return;
	seqid_t ids[num];
	for (unsigned char i = 0; i < num; ++i)
		ids[i] = htonll(generate());
	reply(ids, sizeof(ids));
}

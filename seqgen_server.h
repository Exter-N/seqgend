#ifndef __SEQGEN_SERVER_H
#define __SEQGEN_SERVER_H

#include "udp_serial_server.h"

typedef unsigned long long seqid_t;

class seqgen_server : public udp_serial_server
{
private:
	unsigned short nodeid;
	seqid_t curseed;

public:
	explicit seqgen_server(int);

	inline unsigned short get_node() const { return nodeid; }
	inline void set_node(unsigned short node) { nodeid = node; }

	inline seqid_t get_seed() const { return curseed; }
	inline void reseed(seqid_t seed)
	{
		if (seed > curseed)
			curseed = seed;
	}
	seqid_t generate();
	
protected:
	virtual void process(const void*, size_t);
};

#endif

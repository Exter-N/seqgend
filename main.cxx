#include "globals.h"

#include <fstream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>

#include "udp_serial_server.h"
#include "seqgen_server.h"
#include "strutil.h"
#include "string_map.h"

using namespace std;

seqgen_server* the_server(nullptr);

void handle_stop_signal(int)
{
	if (the_server)
		the_server->stop();
}

int main(int argc, char** argv)
{
	string_map config;
	{
		ifstream confstream((argc > 1) ? argv[1] : "/etc/seqgend.conf");
		parse_string_map(config, confstream);
	}

	unsigned short port = (unsigned short)stoi(config["port"]);
	bool netaccess = stoi(config["network-access"]) != 0;
	
	unsigned short nodeid = (unsigned short)stoi(config["node-id"]);
	
	string seedfile = config["seed-file"];

	seqgen_server server(AF_INET);
	the_server = &server;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = netaccess ? 0 : htonl(0x7F000001);
	server.bind((struct sockaddr*)&addr, sizeof(addr));

	server.set_node(nodeid);
	
	if (seedfile.size() > 0) {
		int fd = open(seedfile.c_str(), O_RDONLY);
		if (fd >= 0) {
			seqid_t seed;
			if (read(fd, &seed, sizeof(seed)) == sizeof(seed))
				server.reseed(seed);
			close(fd);
		}
	}

	struct sigaction stop_act;
	stop_act.sa_handler = handle_stop_signal;
	sigemptyset(&stop_act.sa_mask);
	stop_act.sa_flags = 0;
	sigaction(SIGINT, &stop_act, nullptr);
	sigaction(SIGTERM, &stop_act, nullptr);
	sigaction(SIGHUP, &stop_act, nullptr);

	server.run();
	
	if (seedfile.size() > 0) {
		int fd = creat(seedfile.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd >= 0) {
			seqid_t seed = server.get_seed();
			write(fd, &seed, sizeof(seed));
			close(fd);
		}
	}

	the_server = nullptr;

	return 0;
}

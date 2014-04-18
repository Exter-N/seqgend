CC=g++
CFLAGS=-Wall -Wextra -Werror -Wno-unused-parameter -std=c++11
LDFLAGS=-Wall -Wextra -Werror -Wno-unused-parameter
EXEC=seqgend
SRC=$(wildcard *.cxx)
OBJ=$(SRC:.cxx=.o)
OBJDB=$(SRC:.cxx=-db.o)
LIB=
INCLUDE=

all: $(EXEC) $(EXEC)-db

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -O3 -march=native -flto -fwhole-program -s $(LIB) -o $(EXEC) $^

$(EXEC)-db: $(OBJDB)
	$(CC) $(LDFLAGS) $(LIB) -o $(EXEC)-db $^

main.o: seqgen_server.h udp_serial_server.h string_map.h strutil.h
seqgen_server.o: seqgen_server.h udp_serial_server.h
udp_serial_server.o: udp_serial_server.h
string_map.o: string_map.h strutil.h
strutil.o: strutil.h

%.o: %.cxx globals.h
	$(CC) $(CFLAGS) -O3 -march=native -flto -fwhole-program $(INCLUDE) -o $@ -c $<

%-db.o: %.cxx %.o
	$(CC) $(CFLAGS) -g $(INCLUDE) -o $@ -c $<

.PHONY: clean mrproper install

install:
	install -o root -g root -m 755 seqgend /usr/sbin/seqgend
	install -o root -g root -m 644 seqgend.conf /etc/seqgend.conf

clean:
	rm *.o

mrproper: clean
	rm $(EXEC)
	rm $(EXEC)-db

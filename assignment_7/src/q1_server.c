#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ALGO_INIT SINGLE
#define BLK_SIZE 1024
#define DEBUG 1

enum algorithm {
	SINGLE,
	DOUBLE,
	CHKSUM,
	CYCLIC
};

struct pckt {
	char msg[BLK_SIZE];
	int parity;
};

/**
 * Function to give better information about the crash and exit with
 * a predefined value.
 */
void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void single_parity(struct pckt data)
{
	int err = 0;
	for (int i = 0; data.msg[i] != '\0'; ++i)
		err ^= data.msg[i] - '0';

	printf("Algorithm used: single parity check\n"
		   "message: %s\n"
		   "Error detected: %s\n",
		   data.msg,
		   err ^ data.parity ? "Yes" : "No");
}

void double_parity(const char *msg)
{
	if (DEBUG) printf("DOUBLE\n");
}

void check_sum(const char *msg)
{
	if (DEBUG) printf("check sum\n");
}

void cyclic_check(const char *msg)
{
	if (DEBUG) printf("CRC\n");
}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int sock, ser_sock, port, res;
	struct pckt data;
	enum algorithm method = ALGO_INIT;
	struct sockaddr_in addr;

	int opt = 1;
	int addrlen = sizeof(addr);

	if (argc != 3) {
		fprintf(stderr, "usage: server <address> <port>\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[2]);
	ser_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (!ser_sock)
		die("unable to connect to socket");
	if (setsockopt(ser_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt)))
		die("unable to attach to port");

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(port);

	if (bind(ser_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		die("cannot bind to port");
	if (listen(ser_sock, 3) < 0)
		die("cannot listen");

	printf("listening on %s port %s ...\n", argv[1], argv[2]);

	sock = accept(ser_sock, (struct sockaddr *)&addr, &addrlen);
	if (sock < 0)
		die("cannot accept");

	printf("connection established!\n\n");

	res = read(sock, &method, sizeof(method));
	if (res <= 0)
		die("cannot receive algorithm");

	res = read(sock, (void *)&data, sizeof(data));
	if (res <= 0)
		die("cannot receive message");

	switch (method) {
	case SINGLE: single_parity(data); break;
	case DOUBLE: double_parity(data.msg); break;
	case CHKSUM: check_sum(data.msg); break;
	case CYCLIC: cyclic_check(data.msg); break;
	default: printf("no such algorithm found\n");
	}

	return 0;
}

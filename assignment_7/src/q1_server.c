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
	int bit, size, div;
	int parityh[BLK_SIZE];
	int parityv[BLK_SIZE];
	unsigned int chksum;
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
		   err ^ data.bit ? "Yes" : "No");
}

void double_parity(int div, struct pckt data)
{
	int row, col, err = 0;
	int n = data.size / data.bit;
	char *msg = data.msg;

	for (int i = 0; i < data.bit; ++i) {
		for (int j = i * n; j < (i + 1) * n; ++j)
			data.parityh[i] ^= msg[j] - '0';
		if (data.parityh[i]) {
			row = i;
			err = 1;
			break;
		}
	}

	for (int i = 0; i < n; ++i) {
		for (int j = i; j < data.size; j += n)
			data.parityv[i] ^= msg[j] - '0';
		if (data.parityv[i]) {
			col = i;
			err = 1;
			break;
		}
	}

	printf("Algorithm used: double parity check\n"
		   "message: %s\n"
		   "Error detected: ",
		   data.msg);
	if (err)
		printf("Yes (Row: %d, Col: %d)\n", row, col);
	else
		printf("No\n");
}

void check_sum(struct pckt data)
{
	int n = data.size / data.bit;
	char str[n];
	char *msg = data.msg;
	unsigned int err = 0;
	unsigned int chksum = 0;

	for (int i = 0; msg[i] != '\0';) {
		for (int j = 0; j < n; ++j)
			str[j] = msg[i++];
		chksum += strtol(str, NULL, 2);
	}

	err = ~(chksum + data.chksum);
	printf("Algorithm used: checksum\n"
		   "message: %s\n"
		   "Error detected: %s\n",
		   data.msg,
		   err ? "Yes" : "No");
}

void cyclic_check(int sock, struct pckt data)
{
	int msg = strtol(data.msg, NULL, 2);
	int err = msg / data.div;

	printf("Algorithm used: cyclic redundancy check\n"
		   "message: %s\n"
		   "Error detected: %s\n",
		   data.msg,
		   err ? "Yes" : "No");
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
	case DOUBLE: {
		int div = 0;
		read(sock, (void *)&div, sizeof(div));
		double_parity(div, data);
		break;
	}
	case CHKSUM: check_sum(data); break;
	case CYCLIC: cyclic_check(sock, data); break;
	default: printf("no such algorithm found\n");
	}

	return 0;
}

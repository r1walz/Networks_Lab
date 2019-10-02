#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define ever (;;)
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

int check_msg(const char *msg)
{
	for (int i = 0; msg[i] != '\0'; ++i) {
		if (msg[i] != '0' && msg[i] != '1')
			return !TRUE;
	}
	return TRUE;
}

void single_parity(int sock, struct pckt data)
{
	data.parity = 0;
	for (int i = 0; data.msg[i] != '\0'; ++i)
		data.parity ^= data.msg[i] - '0';

	write(sock, (const void *)&data, sizeof(data));
}

void double_parity(int sock, struct pckt data)
{
	write(sock, (const void *)&data, sizeof(data));
}

void check_sum(int sock, struct pckt data)
{
	write(sock, (const void *)&data, sizeof(data));
}

void cyclic_check(int sock, struct pckt data)
{
	write(sock, (const void *)&data, sizeof(data));
}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int port, sock = 0;
	struct pckt data;
	enum algorithm method = ALGO_INIT;
	struct sockaddr_in serv_addr;

	if (argc != 3) {
		fprintf(stderr, "usage: client <address> <port>\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[2]);
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
		die("unable to connect to socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
		die("invalid address");

	printf("requesting connection from %s port %s ...\n", argv[1], argv[2]);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		die("connection failed");

	printf("connection established!\n\n");
	printf("Available error detection algorithms:\n"
		   "0) Single Parity Check\n"
		   "1) Double Parity Check\n"
		   "2) Checksum\n"
		   "3) Cyclic Redundancy Check\n\n"
	);

	while (TRUE) {
		printf("Enter which algorithm to use: ");
		scanf("%d", (int*)&method);
		if (SINGLE <= method && method <= CYCLIC)
			break;
		else
			printf("Enter value in the specified range\n");
	}

	if (DEBUG) printf("%d\n", method);
	write(sock, &method, sizeof(method));

	while (TRUE) {
		printf("Enter message: ");
		scanf("%1024s", (char *)&data.msg);
		if (check_msg(data.msg))
			break;
		else
			printf("only binary messages are allowed\n");
	}

	switch (method) {
	case SINGLE: single_parity(sock, data); break;
	case DOUBLE: double_parity(sock, data); break;
	case CHKSUM: check_sum(sock, data); break;
	case CYCLIC: cyclic_check(sock, data); break;
	default: printf("no such algorithm found\n");
	}

	return 0;
}

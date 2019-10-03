#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define BKL_SIZE 4
#define PRT_SIZE 3

struct pckt {
	char msg[BKL_SIZE];
	int parity[PRT_SIZE];
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

void hamming_code(struct pckt data)
{

}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int sock, ser_sock, port, res;
	struct sockaddr_in addr;
	struct pckt data;

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

	printf("connection established!\n");

	res = read(sock, (void *)&data, sizeof(data));
	if (res <= 0)
		die("cannot receive message");

	hamming_code(data);

	return 0;
}

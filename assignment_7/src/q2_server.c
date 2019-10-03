#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define BLK_SIZE 4
#define PRT_SIZE 3

struct pckt {
	char msg[BLK_SIZE];
	int parity[PRT_SIZE];
};

char *detect[] = {
	"No",
	"q0",
	"q1",
	"b2",
	"q2",
	"b0",
	"b3",
	"b1"
};

int correct[] = { -1, -1, -1, 1, -1, 3, 0, 2 };

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
	int idx;
	int s0 = data.parity[0] ^ (data.msg[3] - '0')
			 ^ (data.msg[2] - '0') ^ (data.msg[1] - '0');
	int s1 = data.parity[1] ^ (data.msg[2] - '0')
			 ^ (data.msg[1] - '0') ^ (data.msg[0] - '0');
	int s2 = data.parity[2] ^ (data.msg[3] - '0')
			 ^ (data.msg[2] - '0') ^ (data.msg[0] - '0');
	s0 += (s1 << 1) + (s2 << 2);

	printf("received message: %s\n", data.msg);
	printf("error detected: %s\n", detect[s0]);

	idx = correct[s0];
	if (idx != -1)
		data.msg[idx] - '0' ? --data.msg[idx] : ++data.msg[idx];
	printf("corrected message: %s\n", data.msg);
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

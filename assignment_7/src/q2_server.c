#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define BLK_SIZE 1024
#define PCKT_INIT { .msg = "", .emsg = "" }

struct pckt {
	char msg[BLK_SIZE];
	char emsg[BLK_SIZE];
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
	char *emsg;
	char *msg = data.msg;
	int a = strlen(msg), err = 0;
	int r = 0, d = 0, d1 = 0, idx = 0;
	int min, max = 0, bit, s, j;

	while (a + r + 1 > (int)pow(2, r))
		++r;

	emsg = (char *)malloc(sizeof(char) * (a + r));

	for (int i = 0; i < a + r; ++i)
		if (i == (int)pow(2, d) - 1)
			++d, emsg[i] = '0';
		else
			emsg[i] = msg[d1++];

	d1 = 0;
	for (int i = 0; i < a + r; i = pow(2, d1) - 1) {
		min = 0, max = i;
		++d1, bit = 0, j = s = i;
		while (j < a + r) {
			for (s = j; max >= min; ++min, ++s)
				if (emsg[s] == '1')
					++bit;
			j = s + i;
			min = 0;
		}

		if (!(bit % 2))
			emsg[i] = '0';
		else
			emsg[i] = '1';
	}

	for (int i = 0; i < a + r; ++i)
		if (data.emsg[i] != emsg[i])
			++err, idx = i;

	printf("received message: %s\n"
	       "error detected: ", data.emsg);

	switch (err) {
	case 0:
		printf("No\n");
		printf("extracted message is: %s\n", msg);
		break;
	case 1:
		printf("Yes, at %d position\n", idx);
		printf("extracted message is: %s\n", msg);
		msg[idx] - '0' ? --msg[idx] : ++msg[idx];
		printf("corrected message is: %s\n", msg);
		break;
	default:
		printf("Yes\n");
	}

	free(emsg);
}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int sock, ser_sock, port, res;
	struct sockaddr_in addr;
	struct pckt data = PCKT_INIT;

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

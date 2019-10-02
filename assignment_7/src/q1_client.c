#include <time.h>
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

int check_msg(struct pckt *data)
{
	int i;
	char *msg = data->msg;
	for (i = 0; msg[i] != '\0'; ++i) {
		if (msg[i] != '0' && msg[i] != '1')
			return !TRUE;
	}

	data->size = i;
	return TRUE;
}

void single_parity(struct pckt *data)
{
	data->bit = 0;
	for (int i = 0; data->msg[i] != '\0'; ++i)
		data->bit ^= data->msg[i] - '0';
}

void double_parity(struct pckt *data)
{
	int n;
	char *msg = data->msg;

	while (TRUE) {
		printf("Enter number of segments: ");
		scanf("%d", &data->bit);
		if (data->bit < data->size && !(data->size % data->bit))
			break;
		else
			printf("number of segments should divide "
				   "size of message (%d)\n", data->size);
	}

	n = data->size / data->bit;
	for (int i = 0; i < data->bit; ++i) {
		data->parityh[i] = 0;
		for (int j = i * n; j < (i + 1) * n; ++j)
			data->parityh[i] ^= msg[j] - '0';
	}
	for (int i = 0; i < n; ++i) {
		data->parityv[i] = 0;
		for (int j = i; j < data->size; j += n)
			data->parityv[i] ^= msg[j] - '0';
	}
}

void check_sum(struct pckt *data)
{
	int n;
	char *msg = data->msg;
	unsigned int chksum = 0;

	while (TRUE) {
		printf("Enter number of segments: ");
		scanf("%d", &data->bit);
		if (data->bit < data->size && !(data->size % data->bit))
			break;
		else
			printf("number of segments should divide "
				   "size of message (%d)\n", data->size);
	}

	n = data->size / data->bit;
	char str[n];

	for (int i = 0; msg[i] != '\0';) {
		for (int j = 0; j < n; ++j)
			str[j] = msg[i++];
		chksum += strtol(str, NULL, 2);
	}

	data->chksum = ~chksum;
}

void cyclic_check(struct pckt *data)
{
	int div, size, rem;
	while (TRUE) {
		struct pckt tmp;
		printf("Enter divisor: ");
		scanf("%1024s", (char *)&tmp.msg);
		if (check_msg(&tmp)) {
			div = strtol(tmp.msg, NULL, 2);
			size = tmp.size - 1;
			break;
		}
		else
			printf("only binary divisors are allowed\n");
	}

	for (int i = data->size; i < data->size + size; ++i)
		data->msg[i] = '0';
	data->msg[data->size + size] = '\0';
	rem = strtol(data->msg, NULL, 2) % div; // Someone write code for remainder function
	for (int i = 0, j = 4; i < size; ++i, j >>= 1)
		data->msg[data->size + i] += (rem & j) >> (size - i - 1);
	data->div = div;
}

void add_manual_error(struct pckt *data)
{
	int bits, pos;

	printf("Enter number of bits flipped: ");
	scanf("%d", &bits);

	while (bits--) {
		while (TRUE) {
			printf("Enter bit position: ");
			scanf("%d", &pos);

			if (0 <= pos && pos < data->size) {
				data->msg[pos] - '0' ? --data->msg[pos] : ++data->msg[pos];
				break;
			}
			else
				printf("Enter a value less than message size (%d)\n", data->size);
		}
	}
}

void add_probabilistic_error(struct pckt *data)
{
	int max = 1;
	double prob = 0.0;
	double arr[BLK_SIZE];

	srand(time(NULL));
	for (int i = 0; i < BLK_SIZE; ++i) {
		arr[i] = rand();
		if (arr[i] > max)
			max = arr[i];
	}
	for (int i = 0; i < BLK_SIZE; ++i)
		arr[i] /= max;

	while (TRUE) {
		printf("Enter probability: ");
		scanf("%lf", &prob);

		if (0.0 <= prob && prob <= 1.0)
			break;
		else
			printf("Please enter probability within "
				   "reasonable range (0.0 - 1.0)\n");
	}

	for (int i = 0; i < data->size; ++i)
		if (arr[i] < prob)
			data->msg[i] - '0' ? --data->msg[i] : ++data->msg[i];

}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int port, sock = 0;
	char err;
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

	write(sock, &method, sizeof(method));

	while (TRUE) {
		printf("Enter message: ");
		scanf("%1024s", (char *)&data.msg);
		if (check_msg(&data))
			break;
		else
			printf("only binary messages are allowed\n");
	}

	switch (method) {
	case SINGLE: single_parity(&data); break;
	case DOUBLE: double_parity(&data); break;
	case CHKSUM: check_sum(&data); break;
	case CYCLIC: cyclic_check(&data); break;
	default: printf("no such algorithm found\n");
	}

	printf("Introduce error? (Y/N): ");
	scanf(" %c", &err);

	if (err == 'y' || err == 'Y') {
		printf("Manual (M) or Probabilistic (P): ");
		scanf(" %c", &err);

		switch (err) {
		case 'm': case 'M': add_manual_error(&data); break;
		case 'p': case 'P': add_probabilistic_error(&data); break;
		default: printf("invalid selection sending original data\n");
		}
	}

	write(sock, (const void *)&data, sizeof(data));
	printf("%s was sent to the server\n", data.msg);

	return 0;
}

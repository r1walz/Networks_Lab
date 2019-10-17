#include <math.h>
#include <time.h>
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

int check_msg(struct pckt *data)
{
	char *msg = data->msg;
	for (int i = 0; msg[i] != '\0'; ++i) {
		if (msg[i] != '0' && msg[i] != '1')
			return !TRUE;
	}
	return TRUE;
}

void hamming_code(struct pckt *data)
{
	char *emsg;
	char *msg = data->msg;
	int a = strlen(data->msg);
	int r = 0, d = 0, d1 = 0;
	int min, max = 0, bit, s, j;

	while (a + r + 1 > (int)pow(2, r))
		++r;

	printf("number of data bits to be added: %d\n"
	       "Total bits: %d\n", r, a + r);
	emsg = (char *)malloc(sizeof(char) * (a + r));

	for (int i = 0; i < a + r; ++i)
		if (i == (int)pow(2, d) - 1)
			++d, emsg[i] = '0';
		else
			emsg[i] = msg[d1++];

	printf("encoded msg: %s\n", emsg);

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

	printf("Hamming code: %s\n", emsg);
	strcpy(data->emsg, emsg);
	free(emsg);
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

			if (0 <= pos && pos < BLK_SIZE) {
				data->emsg[pos] - '0' ? --data->emsg[pos] : ++data->emsg[pos];
				break;
			}
			else
				printf("Enter a value less than message size (%d)\n", BLK_SIZE);
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

	for (int i = 0; i < BLK_SIZE; ++i)
		if (arr[i] < prob)
			data->emsg[i] - '0' ? --data->emsg[i] : ++data->emsg[i];

}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int port, sock = 0;
	char err;
	struct pckt data = PCKT_INIT;
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
	printf("connection established!\n");

	while (TRUE) {
		printf("Enter message: ");
		scanf("%1024s", (char *)&data.msg);
		if (check_msg(&data))
			break;
		else
			printf("only binary messages are allowed\n");
	}

	hamming_code(&data);

	printf("Introduce error? [can correct upto 1 error] (Y/N): ");
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
	printf("%s was sent to the server\n", data.emsg);

	return 0;
}

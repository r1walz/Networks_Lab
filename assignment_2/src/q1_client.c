#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
 * Function to give better information about the crash and exit with
 * a predefined value.
 */
void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

/**
 * Main driver program.
 */
int main(int argc, const char *argv[])
{
	int port, sock = 0;
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

	return 0;
}

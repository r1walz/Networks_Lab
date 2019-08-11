#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1"
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define CIPHER_KEY 'S'
#define sendrecvflag 0
#define TRUE 1

/**
 * Print better error message and exit
 */
void die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

/**
 * Clear the buffer
 */
void clear_buf(char *buf)
{
	for (int i = 0; i < NET_BUF_SIZE; ++i)
		buf[i] = '\0';
}

/**
 * Encrypt using the cipher key
 */
char cipher(char ch)
{
	return ch ^ CIPHER_KEY;
}

/**
 * Print the received file to stdout
 */
int recv_file(char *buf, int size)
{
	int i = 0;
	char ch = 0;
	while (i < size) {
		ch = cipher(buf[i++]);

		if (ch == EOF)
			return 1;
		else
			printf("%c", ch);
	}

	return 0;
}

/**
 * Driver program
 */
int main()
{
	int sockfd, n_bytes;
	char net_buf[NET_BUF_SIZE];
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);

	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
	if (sockfd < 0)
		die("file descriptor not received!!\n");

	while (TRUE) {
		printf("Please enter file name to receive: ");
		scanf("%s", net_buf);
		sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag,
                       (struct sockaddr*)&addr_con, addrlen);
		printf("---------Start---------\n");

		while (TRUE) {
			clear_buf(net_buf);
			n_bytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag,
                                (struct sockaddr*)&addr_con, &addrlen);
			if (recv_file(net_buf, NET_BUF_SIZE))
				break;
		}

		printf("\n---------End-----------\n");
	}

	return 0;
}

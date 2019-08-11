#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define IP_PROTOCOL 0
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define CIPHER_KEY 'S'
#define sendrecvflag 0
#define nofile "File Not Found!"
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
	for (int i = 0; i < NET_BUF_SIZE; i++)
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
 * Prepare buffer to send through the fd
 */
int send_file(FILE *fp, char *buf, int size)
{
	if (fp == NULL) {
		strcpy(buf, nofile);
		int len = strlen(nofile);
		buf[len] = EOF;

		for (int i = 0; i <= len; i++)
			buf[i] = cipher(buf[i]);
		return 1;
	}

	char ch, ch2;
	for (int i = 0; i < size; i++) {
		ch = fgetc(fp);
		ch2 = cipher(ch);
		buf[i] = ch2;
		if (ch == EOF)
			return 1;
	}
	return 0;
}

/**
 * Driver program
 */
int main()
{
	int sockfd, n_bytes;

	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	char net_buf[NET_BUF_SIZE];
	FILE *fp;

	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = INADDR_ANY;

	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
	if (sockfd < 0)
		die("file descriptor not received!!\n");

	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) != 0)
		die("Binding Failed!\n");

	while (TRUE) {
		printf("Waiting for file name...\n");
		clear_buf(net_buf);

		n_bytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag,
				   (struct sockaddr*)&addr_con, &addrlen);
		fp = fopen(net_buf, "r");

		printf("\nFile Name Received: %s\n", net_buf);

		if (fp == NULL)
			printf("File open failed!\n");
		else
			printf("File Successfully opened!\n");

		while(TRUE) {
			int i = send_file(fp, net_buf, NET_BUF_SIZE);

			sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag,
			       (struct sockaddr*)&addr_con, addrlen);

			if (i)
				break;
			clear_buf(net_buf);
		}

		if (fp != NULL)
			fclose(fp);
	}
	return 0;
}

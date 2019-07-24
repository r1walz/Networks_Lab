#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SIZEOF(array) (sizeof(array) / sizeof(array[0]))
#define ICMP_HDR_INIT { .type = 8, .code = 0, .chksum = 0xfff7, .data = 0 }

int int_loop = 1;

typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t chksum;
	uint32_t data;
} icmp_hdr_t;

void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void int_handler(int sig)
{
	int_loop = 0;
}

void get_host(char *host, int size, const char *host_name, int flags)
{
	int res;
	struct addrinfo *infoptr;

	res = getaddrinfo(host_name, NULL, NULL, &infoptr);

	if (res)
		die("getaddrinfo");

	res = getnameinfo(infoptr->ai_addr, infoptr->ai_addrlen,
			  host, size, NULL, 0, flags);

	if (res)
		die("getnameinfo");
}

int main(int argc, const char *const *argv)
{
	int response;
	int res;
	unsigned int res_addr_size;
	char host_ip[1024], host_name[1024];

	clock_t start, end;
	struct sockaddr_in addr;
	struct sockaddr res_addr;

	unsigned char msg[64] = "";
	int seq = 0, N = 1, sent_packet = 0, lost_packet = 0, total_time = 0;
	int sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	icmp_hdr_t pckt = ICMP_HDR_INIT;

	if (argc == 2)
		N = -1; /* infinite pings */
	else if (argc == 3)
		N = atoi(argv[2]);
	else {
		fprintf(stderr, "usage: ping <address> [<number of pings>]\n");
		exit(EXIT_FAILURE);
	}

	if (sock == -1)
		die("cannot connect to socket");

	get_host(host_ip, SIZEOF(host_ip), argv[1], NI_NUMERICHOST);
	get_host(host_name, SIZEOF(host_name), argv[1], NI_NAMEREQD);

	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = inet_addr(host_ip);

	printf("PING %s (%s) %d bytes of data\n", argv[1], host_ip, pckt.data);

	signal(SIGINT, int_handler);

	while (seq++ != N && int_loop) {
		start = clock();
		res = sendto(sock, &pckt, sizeof(pckt), 0,
			     (struct sockaddr *)&addr, sizeof(addr));

		if (res < 0)
			die("cannot ping");
		else
			++sent_packet;

		response = recvfrom(sock, msg, sizeof(msg), 0,
				    &res_addr, &res_addr_size);

		end = clock();
		if (response < 0) {
			++lost_packet;
			printf("packet lost\n");
		}

		total_time += (end - start);
		printf("%d bytes from %s (%s): icmp_seq=%d time=%ldms\n",
			response, host_name, host_ip, seq, end - start);
	}

	printf("\n--- %s ping statictics ---\n", host_name);
	printf("%d packets transmitted, %d received, %.0f%% packet loss\n"
	       "time %dms, avg time %dms\n",
		sent_packet, sent_packet - lost_packet, lost_packet * (100.0 / sent_packet),
		total_time, total_time / sent_packet);

	return 0;
}

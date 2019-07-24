#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *const *argv)
{
	int res;
	char host[1024];
	struct addrinfo *infoptr;

	if (argc < 2) {
		fprintf(stderr, "usage: hostname <address>...\n");
		exit(EXIT_FAILURE);
	}

	printf("  IP Address   |    Hostname   \n");
	printf("---------------+---------------\n");

	while(--argc) {
		res = getaddrinfo(*++argv, NULL, NULL, &infoptr);

		if (res)
			die("getaddrinfo");

		res = getnameinfo(infoptr->ai_addr, infoptr->ai_addrlen,
				  host, sizeof(host), NULL, 0, NI_NAMEREQD);

		if (res)
			die("getnameinfo");

		printf("%14s | %s\n", *argv, host);
		freeaddrinfo(infoptr);
	}

	return 0;
}

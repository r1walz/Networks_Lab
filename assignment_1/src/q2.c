#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int main()
{
	char buf[1024];
	struct ifreq ifr;
	struct ifconf ifc;
	struct ifreq *it;
	const struct ifreq *end;
	unsigned char *mac_addr = NULL;

	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1) {
		die("cannot connet to socket");
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;

	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
		die("cannot get iface list");
	}

	it = ifc.ifc_req;
	end = it + (ifc.ifc_len / sizeof(struct ifreq));

	for (; it != end; ++it) {
		strcpy(ifr.ifr_name, it->ifr_name);

		if (!ioctl(sock, SIOCGIFFLAGS, &ifr)) {
			if (!(ifr.ifr_flags & IFF_LOOPBACK) &&
			    !ioctl(sock, SIOCGIFHWADDR, &ifr)) {
				free(mac_addr);
				mac_addr = (unsigned char *)malloc(6 * sizeof(char));
				memcpy(mac_addr, ifr.ifr_addr.sa_data, 6);
				break;
			    }
		} else
			die("cannot get socket config flags");
	}

	if (mac_addr) {
		printf("MAC address = ");
		for (int len = 0; len < 5; ++len)
			printf("%02x:", (unsigned char)mac_addr[len]);
		printf("%02x\n", mac_addr[5]);
	}

	free(mac_addr);
	return 0;
}

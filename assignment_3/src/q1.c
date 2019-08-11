#include<stdio.h>
#include <stdlib.h>

/**
 * Return a char representing the class of the IPv4 address
 */
char find_class(const char *str)
{
	char arr[4];
	int i = 0, ip = 0;

	for (i = 0; str[i] != '.'; ++i)
		arr[i] = str[i];
	for (int j = 1; i >= 0; --i, j *= 10)
		ip = ip + (str[i] - '0') * j;

	if (ip >=1 && ip <= 127)
		return 'A';
	else if (ip >= 128 && ip <= 191)
		return 'B';
	else if (ip >= 192 && ip <= 223)
		return 'C';
	else if (ip >= 224 && ip <= 239)
		return 'D';
	return 'E';
}

/**
 * Prints the network and host ID of the address
 */
void separate(const char *str, char ip_class)
{
	int i = 0, j = 0, dot_count = 0;
	char network[12], host[12];

	for (int k = 0; k < 12; k++)
		network[k] = host[k] = '\0';

	switch (ip_class) {
	case 'A':
		while (str[j] != '.')
			network[i++] = str[j++];
		i = 0, ++j;
		while (str[j] != '\0')
			host[i++] = str[j++];
		break;
	case 'B':
		while (dot_count < 2) {
			network[i++] = str[j++];
			if (str[j] == '.')
				++dot_count;
		}
		i = 0, ++j;
		while (str[j] != '\0')
			host[i++] = str[j++];
		break;
	case 'C':
		while (dot_count < 3) {
			network[i++] = str[j++];
			if (str[j] == '.')
				++dot_count;
		}
		i = 0, ++j;
		while (str[j] != '\0')
			host[i++] = str[j++];
		break;
	default:
		printf("In this Class, IP address is not divided "
		       "into Network and Host ID\n");
		return;
	}

	printf("Network ID is %s\n", network);
	printf("Host ID is %s\n", host);
}
/**
 * Driver program
 */
int main(int argc, const char *argv[])
{
	const char *str;
	char ip_class;

	if (argc != 2) {
		fprintf(stderr, "usage: nid <address>\n");
		exit(EXIT_FAILURE);
	}

	str = argv[1];
	ip_class = find_class(str);

	printf("Given IP address belongs to Class %c\n", ip_class);
	separate(str, ip_class);

	return 0;
}

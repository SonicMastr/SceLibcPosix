#include <malloc.h>
#include <netdb.h>
#include <stdio.h>

void freeaddrinfo(struct addrinfo *res) {
	while (res != NULL) {
		struct addrinfo *p = res;
		res = res->ai_next;
		free(p->ai_canonname);
		free(p);
	}
}
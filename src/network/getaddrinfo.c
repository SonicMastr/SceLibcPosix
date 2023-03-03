#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static struct addrinfo *malloc_ai(int port, u_long addr, int socktype, int proto) {
	struct addrinfo *ai = (struct addrinfo *)malloc(sizeof(struct addrinfo) + sizeof(struct sockaddr_in));
	if (ai) {
		memset(ai, 0, sizeof(struct addrinfo) + sizeof(struct sockaddr_in));
		ai->ai_addr = (struct sockaddr *)(ai + 1);
		/* XXX -- ssh doesn't use sa_len */
		ai->ai_addrlen = sizeof(struct sockaddr_in);
		ai->ai_addr->sa_len = sizeof(struct sockaddr_in);
		ai->ai_addr->sa_family = ai->ai_family = AF_INET;
		((struct sockaddr_in *)(ai)->ai_addr)->sin_port = port;
		((struct sockaddr_in *)(ai)->ai_addr)->sin_addr.s_addr = addr;
		ai->ai_socktype = socktype;
		ai->ai_protocol = proto;
		return ai;
	}

	return NULL;
}

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
	struct addrinfo *cur, *prev = NULL;
	struct hostent *hp;
	struct in_addr in;
	int i, port = 0, socktype, proto;

	if (hints && hints->ai_family != PF_INET && hints->ai_family != PF_UNSPEC)
		return EAI_FAMILY;

	socktype = (hints && hints->ai_socktype) ? hints->ai_socktype : SOCK_STREAM;
	if (hints && hints->ai_protocol)
		proto = hints->ai_protocol;
	else {
		switch (socktype) {
		case SOCK_DGRAM:
			proto = IPPROTO_UDP;
			break;
		case SOCK_STREAM:
			proto = IPPROTO_TCP;
			break;
		default:
			proto = 0;
			break;
		}
	}

	if (service) {
		if (isdigit((int)*service))
			port = htons(atoi(service));
		else {
			struct servent *se;
			char *pe_proto;

			switch (socktype) {
			case SOCK_DGRAM:
				pe_proto = "udp";
				break;
			case SOCK_STREAM:
				pe_proto = "tcp";
				break;
			default:
				pe_proto = NULL;
				break;
			}
			if ((se = getservbyname(service, pe_proto)) == NULL)
				return EAI_SERVICE;

			port = se->s_port;
		}
	}
	if (!node) {
		if (hints && hints->ai_flags & AI_PASSIVE)
			*res = malloc_ai(port, htonl(0x00000000), socktype, proto);
		else
			*res = malloc_ai(port, htonl(0x7f000001), socktype, proto);
		if (*res)
			return 0;
		else
			return EAI_MEMORY;
	}
	if (inet_aton(node, &in)) {
		*res = malloc_ai(port, in.s_addr, socktype, proto);
		if (*res)
			return 0;
		else
			return EAI_MEMORY;
	}
	if (hints && hints->ai_flags & AI_NUMERICHOST)
		return EAI_NODATA;
	if ((hp = gethostbyname(node)) && hp->h_name && hp->h_name[0] && hp->h_addr_list[0]) {
		for (i = 0; hp->h_addr_list[i]; i++) {
			if ((cur = malloc_ai(port, ((struct in_addr *)hp->h_addr_list[i])->s_addr, socktype, proto)) == NULL) {
				if (*res)
					freeaddrinfo(*res);
				return EAI_MEMORY;
			}
			if (prev)
				prev->ai_next = cur;
			else
				*res = cur;
			prev = cur;
		}
		if (hints && hints->ai_flags & AI_CANONNAME && *res) {
			/* NOT sasl_strdup for compatibility */
			if (((*res)->ai_canonname = strdup(hp->h_name)) == NULL) {
				freeaddrinfo(*res);
				return EAI_MEMORY;
			}
		}
		return 0;
	}
	return EAI_NODATA;
}
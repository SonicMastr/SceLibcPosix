#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <psp2/net/netctl.h>
#include "vitaerror.h"
#include "vitanet.h"

int gethostname(char *name, size_t len) {
	_net_init();
	if (!name) {
		errno = EFAULT;
		return -1;
	}
	if (len < 0) {
		errno = EINVAL;
		return -1;
	}

	memset(name, 0, len);

	SceNetCtlInfo info;
	int ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_DHCP_HOSTNAME, &info);

	if (ret < 0) {
		errno = __scenet_errno_to_errno(ret);
		return -1;
	}

	if (strlen(info.dhcp_hostname) > 0) {
		strncpy(name, info.dhcp_hostname, len - 1);
		return 0;
	}

	// fallback to "localhost"
	strncpy(name, "localhost", len - 1);
	return 0;
}
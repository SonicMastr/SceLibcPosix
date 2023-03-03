#include <netdb.h>

const char *gai_strerror(int code) {
	switch (code) {
	case EAI_BADFLAGS:
		return "Invalid value for ai_flags";
	case EAI_NONAME:
		return "hostname nor servname provided, or not known";
	case EAI_AGAIN:
		return "Temporary failure in name resolution";
	case EAI_FAIL:
		return "Non-recoverable failure in name resolution";
	case EAI_NODATA:
		return "No address associated with hostname";
	case EAI_FAMILY:
		return "ai_family not supported";
	case EAI_SOCKTYPE:
		return "ai_socktype not supported";
	case EAI_SERVICE:
		return "servname not supported for ai_socktype";
	case EAI_ADDRFAMILY:
		return "Address family for hostname not supported";
	case EAI_MEMORY:
		return "Memory allocation failure";
	case EAI_SYSTEM:
		return "System error returned in errno";
	case EAI_OVERFLOW:
		return "Argument buffer overflow";
	default:
		return "Unknown error";
	}
}
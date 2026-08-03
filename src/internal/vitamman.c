#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/vshbridge.h>
#include <kubridge.h>

#include <sys/mman.h>

#include "vitamman.h"

int sceKernelFreeMemBlockForVM(SceUID uid);

#define MMAN_MAX_REGIONS 256

typedef struct {
	void *addr;
	size_t len;
	SceUID uid;
	int prot;
} mman_region;

static mman_region _regions[MMAN_MAX_REGIONS];
static int _region_count;

static SceKernelLwMutexWork _mman_mutex __attribute__((aligned(8)));
static int _mman_ready;
static int _have_kubridge;

static SceUID _vm_uid = -1;
static uintptr_t _vm_base;

static int _module_loaded(const char *name) {
	int search_unk[2] = { 0, 0 };
	return _vshKernelSearchModuleByName(name, search_unk) >= 0;
}

#define KU_NID_MEM_PROTECT  0x566d2af1u
#define KU_NID_MEM_RESERVE  0xced3608cu
#define KU_NID_MEM_COMMIT   0x9c0cd758u
#define KU_NID_MEM_DECOMMIT 0x9bdf5881u

static int _import_resolved(const void *fn, uint32_t nid) {
	const volatile uint32_t *stub = (const volatile uint32_t *)((uintptr_t)fn & ~(uintptr_t)1);
	uint32_t target = stub[2];
	return target != 0 && target != nid;
}

static int _kubridge_usable(void) {
	if (!_module_loaded("kubridge"))
		return 0;

	return _import_resolved((const void *)kuKernelMemReserve, KU_NID_MEM_RESERVE) &&
	       _import_resolved((const void *)kuKernelMemCommit, KU_NID_MEM_COMMIT) &&
	       _import_resolved((const void *)kuKernelMemDecommit, KU_NID_MEM_DECOMMIT) &&
	       _import_resolved((const void *)kuKernelMemProtect, KU_NID_MEM_PROTECT);
}

void __init_mman(void) {
	sceKernelCreateLwMutex(&_mman_mutex, "mman mutex", SCE_KERNEL_MUTEX_ATTR_RECURSIVE, 1, NULL);

	_have_kubridge = _kubridge_usable();

	_region_count = 0;
	_mman_ready = 1;

	sceKernelUnlockLwMutex(&_mman_mutex, 1);
}

void __fini_mman(void) {
	if (!_mman_ready)
		return;

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	for (int i = 0; i < _region_count; i++) {
		if (_regions[i].uid < 0)
			continue;
		kuKernelMemDecommit(_regions[i].addr, _regions[i].len);

		int seen = 0;
		for (int j = 0; j < i; j++) {
			if (_regions[j].uid == _regions[i].uid) {
				seen = 1;
				break;
			}
		}
		if (!seen)
			sceKernelFreeMemBlock(_regions[i].uid);
	}
	_region_count = 0;

	if (_vm_uid >= 0) {
		sceKernelCloseVMDomain();
		sceKernelFreeMemBlockForVM(_vm_uid);
		_vm_uid = -1;
	}

	_mman_ready = 0;
	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	sceKernelDeleteLwMutex(&_mman_mutex);
}

static SceUInt32 _to_ku_prot(int prot) {
	SceUInt32 p = KU_KERNEL_PROT_NONE;
	if (prot & PROT_READ)
		p |= KU_KERNEL_PROT_READ;
	if (prot & PROT_WRITE)
		p |= KU_KERNEL_PROT_WRITE;
	if (prot & PROT_EXEC)
		p |= KU_KERNEL_PROT_EXEC;
	return p;
}

static mman_region *_region_find(void *addr) {
	for (int i = 0; i < _region_count; i++) {
		uintptr_t base = (uintptr_t)_regions[i].addr;
		if ((uintptr_t)addr >= base && (uintptr_t)addr < base + _regions[i].len)
			return &_regions[i];
	}
	return NULL;
}

static int _region_insert(void *addr, size_t len, SceUID uid, int prot) {
	if (_region_count >= MMAN_MAX_REGIONS)
		return -1;

	int i = _region_count - 1;
	while (i >= 0 && (uintptr_t)_regions[i].addr > (uintptr_t)addr) {
		_regions[i + 1] = _regions[i];
		i--;
	}
	_regions[i + 1].addr = addr;
	_regions[i + 1].len = len;
	_regions[i + 1].uid = uid;
	_regions[i + 1].prot = prot;
	_region_count++;
	return 0;
}

static void _region_remove(mman_region *r) {
	int i = r - _regions;
	memmove(&_regions[i], &_regions[i + 1], (_region_count - i - 1) * sizeof(*r));
	_region_count--;
}

static void _release_reservation(SceUID uid) {
	if (uid < 0)
		return;
	for (int i = 0; i < _region_count; i++) {
		if (_regions[i].uid == uid)
			return;
	}
	sceKernelFreeMemBlock(uid);
}

static int _vm_pool_init(void) {
	if (_vm_uid >= 0)
		return 0;

	SceUID uid = sceKernelAllocMemBlockForVM("SceLibcPosix_mman", MMAN_VM_POOL_SIZE);
	if (uid < 0)
		return uid;

	void *base = NULL;
	int ret = sceKernelGetMemBlockBase(uid, &base);
	if (ret < 0) {
		sceKernelFreeMemBlockForVM(uid);
		return ret;
	}

	sceKernelOpenVMDomain();

	_vm_uid = uid;
	_vm_base = (uintptr_t)base;
	return 0;
}

static void *_vm_pool_alloc(size_t len) {
	uintptr_t cand = _vm_base;
	uintptr_t end = _vm_base + MMAN_VM_POOL_SIZE;

	for (int i = 0; i < _region_count; i++) {
		if (_regions[i].uid >= 0)
			continue;

		uintptr_t start = (uintptr_t)_regions[i].addr;
		if (start >= cand + len)
			break;

		uintptr_t after = start + _regions[i].len;
		if (after > cand)
			cand = after;
	}

	if (cand > end || end - cand < len)
		return NULL;
	return (void *)cand;
}

void *__mman_map(void *hint, size_t len, int prot, int *err) {
	void *ret = NULL;

	*err = 0;
	len = MMAN_PAGE_ALIGN(len);
	if (!len) {
		*err = EINVAL;
		return NULL;
	}
	if (!_mman_ready) {
		*err = ENOMEM;
		return NULL;
	}
	if (hint && !_have_kubridge) {
		*err = ENOTSUP;
		return NULL;
	}

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	if (_have_kubridge) {
		void *base = hint;
		SceUID uid = kuKernelMemReserve(&base, len, SCE_KERNEL_MEMBLOCK_TYPE_USER_RW);
		if (uid < 0) {
			*err = ENOMEM;
			goto out;
		}
		if (kuKernelMemCommit(base, len, _to_ku_prot(prot), NULL) < 0) {
			sceKernelFreeMemBlock(uid);
			*err = ENOMEM;
			goto out;
		}
		if (_region_insert(base, len, uid, prot) < 0) {
			kuKernelMemDecommit(base, len);
			sceKernelFreeMemBlock(uid);
			*err = ENOMEM;
			goto out;
		}
		ret = base;
	} else {
		if (_vm_pool_init() < 0) {
			*err = ENOMEM;
			goto out;
		}
		void *p = _vm_pool_alloc(len);
		if (!p) {
			*err = ENOMEM;
			goto out;
		}
		if (_region_insert(p, len, -1, prot) < 0) {
			*err = ENOMEM;
			goto out;
		}
		ret = p;
	}

out:
	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_unmap(void *addr, size_t len) {
	int ret = -1;

	len = MMAN_PAGE_ALIGN(len);
	if (!len || !_mman_ready)
		return -1;

	uintptr_t s = (uintptr_t)addr;
	uintptr_t e = s + len;

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	for (int i = 0; i < _region_count;) {
		uintptr_t rs = (uintptr_t)_regions[i].addr;
		uintptr_t re = rs + _regions[i].len;

		if (re <= s || rs >= e) {
			i++;
			continue;
		}

		uintptr_t us = s > rs ? s : rs;
		uintptr_t ue = e < re ? e : re;
		SceUID uid = _regions[i].uid;
		int prot = _regions[i].prot;

		if (uid >= 0)
			kuKernelMemDecommit((void *)us, ue - us);

		if (us == rs && ue == re) {
			_region_remove(&_regions[i]);
			_release_reservation(uid);
		} else if (us == rs) {
			_regions[i].addr = (void *)ue;
			_regions[i].len = re - ue;
			i++;
		} else if (ue == re) {
			_regions[i].len = us - rs;
			i++;
		} else {
			_regions[i].len = us - rs;
			if (_region_insert((void *)ue, re - ue, uid, prot) < 0) {
				_regions[i].len = re - rs;
				goto out;
			}
			ret = 0;
			goto out;
		}

		ret = 0;
	}

out:
	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_prot_of(void *addr) {
	int prot = -1;

	if (!_mman_ready)
		return -1;

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);
	mman_region *r = _region_find(addr);
	if (r)
		prot = r->prot;
	sceKernelUnlockLwMutex(&_mman_mutex, 1);

	return prot;
}

int __mman_resize(void *addr, size_t old_len, size_t new_len) {
	int ret = -1;

	if (!_mman_ready)
		return -1;

	old_len = MMAN_PAGE_ALIGN(old_len);
	new_len = MMAN_PAGE_ALIGN(new_len);

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	mman_region *r = _region_find(addr);
	if (!r || r->addr != addr || r->len != old_len)
		goto out;

	if (new_len == old_len) {
		ret = 0;
		goto out;
	}

	if (new_len < old_len) {
		if (r->uid >= 0)
			kuKernelMemDecommit((char *)addr + new_len, old_len - new_len);
		r->len = new_len;
		ret = 0;
		goto out;
	}

	if (r->uid >= 0)
		goto out;

	uintptr_t grow_s = (uintptr_t)addr + old_len;
	uintptr_t grow_e = (uintptr_t)addr + new_len;
	if (grow_e > _vm_base + MMAN_VM_POOL_SIZE)
		goto out;

	for (int i = 0; i < _region_count; i++) {
		if (&_regions[i] == r)
			continue;
		uintptr_t os = (uintptr_t)_regions[i].addr;
		uintptr_t oe = os + _regions[i].len;
		if (os < grow_e && oe > grow_s)
			goto out;
	}

	memset((void *)grow_s, 0, grow_e - grow_s);
	r->len = new_len;
	ret = 0;

out:
	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_protect(void *addr, size_t len, int prot) {
	int ret = 0;

	if (!_mman_ready)
		return -1;

	len = MMAN_PAGE_ALIGN(len);

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	uintptr_t s = (uintptr_t)addr;
	uintptr_t e = s + len;
	for (int i = 0; i < _region_count; i++) {
		uintptr_t rs = (uintptr_t)_regions[i].addr;
		if (rs >= s && rs + _regions[i].len <= e)
			_regions[i].prot = prot;
	}

	if (_have_kubridge)
		ret = kuKernelMemProtect(addr, len, _to_ku_prot(prot));

	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_mapped(void *addr, size_t len) {
	int ret;

	if (!_mman_ready)
		return 0;

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	uintptr_t p = (uintptr_t)addr;
	uintptr_t e = MMAN_PAGE_ALIGN((uintptr_t)addr + len);
	ret = 1;
	while (p < e) {
		mman_region *r = _region_find((void *)p);
		if (!r) {
			ret = 0;
			break;
		}
		p = (uintptr_t)r->addr + r->len;
	}

	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_discard(void *addr, size_t len) {
	int ret = -1;

	if (!_mman_ready)
		return -1;

	sceKernelLockLwMutex(&_mman_mutex, 1, 0);

	mman_region *r = _region_find(addr);
	if (!r)
		goto out;

	uintptr_t rs = (uintptr_t)r->addr;
	uintptr_t re = rs + r->len;
	uintptr_t ds = (uintptr_t)addr & ~(uintptr_t)(MMAN_PAGE_SIZE - 1);
	uintptr_t de = MMAN_PAGE_ALIGN((uintptr_t)addr + len);
	if (ds < rs)
		ds = rs;
	if (de > re)
		de = re;
	if (de <= ds)
		goto out;

	int prot = r->prot;

	if (r->uid >= 0) {
		kuKernelMemDecommit((void *)ds, de - ds);
		if (kuKernelMemCommit((void *)ds, de - ds, _to_ku_prot(prot | PROT_WRITE), NULL) < 0)
			goto out;
		memset((void *)ds, 0, de - ds);
		if ((prot | PROT_WRITE) != prot)
			kuKernelMemProtect((void *)ds, de - ds, _to_ku_prot(prot));
	} else {
		memset((void *)ds, 0, de - ds);
	}

	ret = 0;

out:
	sceKernelUnlockLwMutex(&_mman_mutex, 1);
	return ret;
}

int __mman_sync(void *addr, size_t len) {
	if (!_mman_ready)
		return -1;

	if (_have_kubridge) {
		kuKernelFlushCaches(addr, len);
		return 0;
	}

	if (_vm_uid < 0)
		return 0;
	return sceKernelSyncVMDomain(_vm_uid, addr, len);
}

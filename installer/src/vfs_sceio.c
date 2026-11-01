/*
 * SQLite icin sceIo tabanli dosya katmani (VFS, surum 1).
 *
 * Neden: Vita'nin SceSqlite modulu app.db'yi okuyabiliyor ama yazmaya
 * baslarken yan dosya (work.db-journal) acamiyor ("unable to open database
 * file"). Bu katman tum dosyalari dogrudan sceIo ile acar. Ayrica db.c
 * gunlugu bellekte tutar (journal_mode=MEMORY); yazilan dosya zaten atilabilir
 * bir calisma kopyasi.
 *
 * Kilitler bostur: dosyayi yalnizca bu uygulama kullanir.
 */
#include "vfs_sceio.h"
#include "vsqlite.h"
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/threadmgr.h>
#include <string.h>

typedef struct {
	sqlite3_file base;
	SceUID fd;
	int delete_on_close;
	char path[256];
} nfile_t;

static int nf_close(sqlite3_file *f)
{
	nfile_t *p = (nfile_t *)f;
	if (p->fd >= 0) sceIoClose(p->fd);
	p->fd = -1;
	if (p->delete_on_close) sceIoRemove(p->path);
	return SQLITE_OK;
}

static int nf_read(sqlite3_file *f, void *buf, int amt, sqlite3_int64 off)
{
	nfile_t *p = (nfile_t *)f;
	if (sceIoLseek(p->fd, off, SCE_SEEK_SET) != off) return SQLITE_IOERR_READ;
	int got = 0;
	while (got < amt) {
		int n = sceIoRead(p->fd, (char *)buf + got, amt - got);
		if (n < 0) return SQLITE_IOERR_READ;
		if (n == 0) break;
		got += n;
	}
	if (got < amt) {
		memset((char *)buf + got, 0, amt - got);
		return SQLITE_IOERR_SHORT_READ;
	}
	return SQLITE_OK;
}

static int nf_write(sqlite3_file *f, const void *buf, int amt, sqlite3_int64 off)
{
	nfile_t *p = (nfile_t *)f;
	if (sceIoLseek(p->fd, off, SCE_SEEK_SET) != off) return SQLITE_IOERR_WRITE;
	int put = 0;
	while (put < amt) {
		int n = sceIoWrite(p->fd, (const char *)buf + put, amt - put);
		if (n <= 0) return SQLITE_IOERR_WRITE;
		put += n;
	}
	return SQLITE_OK;
}

static int nf_size(sqlite3_file *f, sqlite3_int64 *size)
{
	nfile_t *p = (nfile_t *)f;
	SceOff s = sceIoLseek(p->fd, 0, SCE_SEEK_END);
	if (s < 0) return SQLITE_IOERR_FSTAT;
	*size = s;
	return SQLITE_OK;
}

static int nf_truncate(sqlite3_file *f, sqlite3_int64 size)
{
	sqlite3_int64 cur;
	if (nf_size(f, &cur) != SQLITE_OK) return SQLITE_IOERR_TRUNCATE;
	if (size >= cur) return SQLITE_OK;
	SceIoStat st;
	memset(&st, 0, sizeof(st));
	st.st_size = size;
	return sceIoChstatByFd(((nfile_t *)f)->fd, &st, SCE_CST_SIZE) < 0 ? SQLITE_IOERR_TRUNCATE : SQLITE_OK;
}

static int nf_sync(sqlite3_file *f, int flags)
{
	(void)flags;
	sceIoSyncByFd(((nfile_t *)f)->fd, 0);
	return SQLITE_OK;
}

static int nf_lock(sqlite3_file *f, int l) { (void)f; (void)l; return SQLITE_OK; }
static int nf_reserved(sqlite3_file *f, int *r) { (void)f; *r = 0; return SQLITE_OK; }
static int nf_fcntl(sqlite3_file *f, int op, void *a) { (void)f; (void)op; (void)a; return SQLITE_NOTFOUND; }
static int nf_sector(sqlite3_file *f) { (void)f; return 512; }
static int nf_devchar(sqlite3_file *f) { (void)f; return 0; }

static const sqlite3_io_methods nf_methods = {
	1, nf_close, nf_read, nf_write, nf_truncate, nf_sync, nf_size,
	nf_lock, nf_lock, nf_reserved, nf_fcntl, nf_sector, nf_devchar,
};

static int temp_counter;

static int nv_open(sqlite3_vfs *v, const char *name, sqlite3_file *f, int flags, int *out)
{
	(void)v;
	nfile_t *p = (nfile_t *)f;
	memset(p, 0, sizeof(*p));
	p->fd = -1;
	if (name) {
		if (strlen(name) >= sizeof(p->path)) return SQLITE_CANTOPEN;
		strcpy(p->path, name);
	} else {
		/* Gecici dosya (temp_store=MEMORY ile normalde istenmez) */
		sceIoMkdir("ux0:data/nestfolder/kurulum", 0777);
		p->path[0] = 0;
		strcat(p->path, "ux0:data/nestfolder/kurulum/tmp");
		int n = ++temp_counter, len = (int)strlen(p->path);
		p->path[len++] = (char)('0' + (n / 10) % 10);
		p->path[len++] = (char)('0' + n % 10);
		p->path[len] = 0;
		flags |= SQLITE_OPEN_CREATE | SQLITE_OPEN_DELETEONCLOSE | SQLITE_OPEN_READWRITE;
	}
	int mode;
	if (flags & SQLITE_OPEN_READWRITE) {
		mode = SCE_O_RDWR;
		if (flags & SQLITE_OPEN_CREATE) mode |= SCE_O_CREAT;
		if (flags & SQLITE_OPEN_EXCLUSIVE) mode |= SCE_O_EXCL;
	} else {
		mode = SCE_O_RDONLY;
	}
	p->fd = sceIoOpen(p->path, mode, 0666);
	if (p->fd < 0) return SQLITE_CANTOPEN;
	p->delete_on_close = (flags & SQLITE_OPEN_DELETEONCLOSE) != 0;
	if (out) *out = flags;
	p->base.pMethods = &nf_methods;
	return SQLITE_OK;
}

static int nv_delete(sqlite3_vfs *v, const char *name, int sync)
{
	(void)v; (void)sync;
	SceIoStat st;
	if (sceIoGetstat(name, &st) < 0) return SQLITE_OK;
	return sceIoRemove(name) < 0 ? SQLITE_IOERR_DELETE : SQLITE_OK;
}

static int nv_access(sqlite3_vfs *v, const char *name, int flags, int *res)
{
	(void)v; (void)flags;
	SceIoStat st;
	*res = sceIoGetstat(name, &st) >= 0;
	return SQLITE_OK;
}

static int nv_fullpath(sqlite3_vfs *v, const char *name, int n, char *out)
{
	(void)v;
	if ((int)strlen(name) >= n) return SQLITE_CANTOPEN;
	strcpy(out, name);
	return SQLITE_OK;
}

static int nv_random(sqlite3_vfs *v, int n, char *out)
{
	(void)v;
	static unsigned x = 0x12345678u;
	for (int i = 0; i < n; i++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; out[i] = (char)x; }
	return n;
}

static int nv_sleep(sqlite3_vfs *v, int us) { (void)v; sceKernelDelayThread(us); return us; }

static int nv_time(sqlite3_vfs *v, double *t) { (void)v; *t = 2440587.5; return SQLITE_OK; }

static int nv_lasterr(sqlite3_vfs *v, int n, char *s) { (void)v; if (n > 0) s[0] = 0; return 0; }

static sqlite3_vfs nest_vfs = {
	1, sizeof(nfile_t), 255, 0, NEST_VFS_NAME, 0,
	nv_open, nv_delete, nv_access, nv_fullpath,
	0, 0, 0, 0,
	nv_random, nv_sleep, nv_time, nv_lasterr,
};

int nest_vfs_register(void)
{
	return sqlite3_vfs_register(&nest_vfs, 0);
}

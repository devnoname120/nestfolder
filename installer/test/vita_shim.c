/*
 * PC testi icin Vita API taklidi. main.c'yi oldugu gibi derler:
 *   - ur0:/ux0:/app0: yollari $NEST_ROOT/ur0/... altina eslenir
 *   - tuslar $NEST_KEYS dizisinden okunur: U D X O (bas-birak), '.' bos kare,
 *     S ekran goruntusu; dizi bitince 400 bos kareden sonra cikilir
 *   - ekran goruntuleri $NEST_ROOT/shotN.ppm, yeniden baslatmada reboot.ppm
 *   - sceIoRename, Vita gibi hedef varsa basarisiz olur
 */
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#include <psp2/shellutil.h>
#include <psp2/sqlite.h>
#include <psp2/sysmodule.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sqlite3.h>
int __real_sqlite3_open_v2(const char *f, sqlite3 **db, int fl, const char *v);

static char *root(void) { char *r = getenv("NEST_ROOT"); return r ? r : "."; }

static void map(const char *p, char *out)
{
	const char *c = strchr(p, ':');
	if (!c) { snprintf(out, 512, "%s", p); return; }
	snprintf(out, 512, "%s/%.*s/%s", root(), (int)(c - p), p, c + 1);
}

static void *fbmem;
static int shots, locked;

static void dump(const char *name)
{
	char path[512];
	snprintf(path, sizeof(path), "%s/%s", root(), name);
	FILE *f = fopen(path, "wb");
	fprintf(f, "P6 960 544 255\n");
	unsigned char *p = fbmem;
	for (int i = 0; i < 960 * 544; i++) fwrite(p + i * 4, 1, 3, f);   /* ABGR LE = R,G,B,A */
	fclose(f);
}

SceUID sceKernelAllocMemBlock(const char *n, SceKernelMemBlockType t, SceSize s, SceKernelAllocMemBlockOpt *o)
{ (void)n; (void)t; (void)o; fbmem = calloc(1, s); return 1; }
int sceKernelGetMemBlockBase(SceUID u, void **b) { (void)u; *b = fbmem; return 0; }
int sceDisplaySetFrameBuf(const SceDisplayFrameBuf *f, SceDisplaySetBufSync s) { (void)f; (void)s; return 0; }
int sceDisplayWaitVblankStart(void) { return 0; }
int sceCtrlSetSamplingMode(SceCtrlPadInputMode m) { (void)m; return 0; }
int sceShellUtilInitEvents(int u) { (void)u; return 0; }
int sceShellUtilLock(SceShellUtilLockType t) { (void)t; locked++; return 0; }
int sceShellUtilUnlock(SceShellUtilLockType t) { (void)t; locked--; return 0; }
int sceSysmoduleLoadModule(SceSysmoduleModuleId id) { (void)id; return 0; }
int sceSqliteConfigMallocMethods(SceSqliteMallocMethods *m) { (void)m; return 0; }
int sceKernelPowerTick(SceKernelPowerTickType t) { (void)t; return 0; }
int sceKernelDelayThread(SceUInt d) { (void)d; return 0; }
int sceKernelExitProcess(int r)
{
	dump("exit.ppm");
	printf("[shim] cikis (%d)\n", r);
	exit(0);
}
int scePowerRequestColdReset(void)
{
	dump("reboot.ppm");
	printf("[shim] YENIDEN BASLATMA (kilit=%d)\n", locked);
	exit(0);
}

static int kpos, idle, phase;
int sceCtrlPeekBufferPositive(int port, SceCtrlData *d, int n)
{
	(void)port; (void)n;
	const char *k = getenv("NEST_KEYS");
	memset(d, 0, sizeof(*d));
	if (!k || !k[kpos]) {
		if (++idle > 400) { dump("idle.ppm"); printf("[shim] tus dizisi bitti\n"); exit(0); }
		return 1;
	}
	if (phase) { phase = 0; kpos++; return 1; }   /* birak */
	char c = k[kpos];
	switch (c) {
	case 'U': d->buttons = SCE_CTRL_UP; break;
	case 'D': d->buttons = SCE_CTRL_DOWN; break;
	case 'X': d->buttons = SCE_CTRL_CROSS; break;
	case 'O': d->buttons = SCE_CTRL_CIRCLE; break;
	case 'T': {   /* Shell app.db'yi degistiriyormus gibi */
		char a[512]; map("ur0:shell/db/app.db", a);
		sqlite3 *db; __real_sqlite3_open_v2(a, &db, SQLITE_OPEN_READWRITE, NULL);
		sqlite3_exec(db, "UPDATE tbl_appinfo_icon SET title = title || '+' WHERE pageId = 100 AND pos = 0", 0, 0, 0);
		sqlite3_close(db); printf("[shim] app.db disaridan degistirildi\n"); kpos++; return 1; }
	case 'S': { char nm[32]; snprintf(nm, sizeof(nm), "shot%d.ppm", ++shots); dump(nm); kpos++; return 1; }
	default: kpos++; return 1;   /* '.' bos kare */
	}
	phase = 1;
	return 1;
}

SceUID sceIoOpen(const char *p, int flags, SceMode m)
{
	char path[512]; map(p, path);
	size_t L = strlen(p);
	if (L > 8 && !strcmp(p + L - 8, "-journal") && (flags & SCE_O_CREAT)) {
		printf("[shim] GUNLUK DOSYASI ACILMAK ISTENDI: %s\n", p);
		return (SceUID)0x80010002;
	}
	int f = 0;
	if ((flags & 3) == SCE_O_RDONLY) f = O_RDONLY;
	else if ((flags & 3) == SCE_O_WRONLY) f = O_WRONLY;
	else f = O_RDWR;
	if (flags & SCE_O_CREAT) f |= O_CREAT;
	if (flags & SCE_O_TRUNC) f |= O_TRUNC;
	if (flags & SCE_O_APPEND) f |= O_APPEND;
	if (flags & SCE_O_EXCL) f |= O_EXCL;
	int fd = open(path, f, m ? m : 0666);
	return fd < 0 ? (SceUID)0x80010002 : fd + 1;
}
int sceIoClose(SceUID fd) { return close(fd - 1) < 0 ? -1 : 0; }
int sceIoRead(SceUID fd, void *b, SceSize n) { return (int)read(fd - 1, b, n); }
int sceIoWrite(SceUID fd, const void *b, SceSize n) { return (int)write(fd - 1, b, n); }
SceOff sceIoLseek(SceUID fd, SceOff o, int w) { return lseek(fd - 1, o, w); }
int sceIoSyncByFd(SceUID fd, int f) { (void)f; return fsync(fd - 1); }
int sceIoChstatByFd(SceUID fd, const SceIoStat *st, unsigned int bits)
{ return (bits & SCE_CST_SIZE) ? ftruncate(fd - 1, st->st_size) : 0; }
int sceIoRemove(const char *p) { char a[512]; map(p, a); return unlink(a) < 0 ? -1 : 0; }
int sceIoMkdir(const char *p, SceMode m) { char a[512]; map(p, a); return mkdir(a, m) < 0 ? -1 : 0; }
int sceIoRename(const char *o, const char *n)
{
	char a[512], b[512]; map(o, a); map(n, b);
	if (access(b, F_OK) == 0) return (int)0x80010011;   /* Vita: hedef var */
	return rename(a, b) < 0 ? -1 : 0;
}
int sceIoGetstat(const char *p, SceIoStat *st)
{
	char a[512]; map(p, a);
	struct stat s;
	if (stat(a, &s) < 0) return (int)0x80010002;
	memset(st, 0, sizeof(*st));
	st->st_size = s.st_size;
	return 0;
}

/* sqlite3_open_v2 yolunu da esle (-Wl,--wrap=sqlite3_open_v2) */
#include <sqlite3.h>
int __real_sqlite3_open_v2(const char *f, sqlite3 **db, int fl, const char *v);
int __wrap_sqlite3_open_v2(const char *f, sqlite3 **db, int fl, const char *v)
{
	char a[512];
	if (v) return __real_sqlite3_open_v2(f, db, fl, v);   /* nest_sceio: yolu sceIo esler */
	map(f, a);
	return __real_sqlite3_open_v2(a, db, fl, v);
}

/* Sistem ayarlari: NEST_LANG=tr|en, NEST_ENTER=circle|cross */
#include <psp2/apputil.h>
#include <psp2/system_param.h>
int sceAppUtilInit(SceAppUtilInitParam *i, SceAppUtilBootParam *b) { (void)i; (void)b; return 0; }
int sceAppUtilSystemParamGetInt(unsigned int id, int *v)
{
	const char *l = getenv("NEST_LANG"), *e = getenv("NEST_ENTER");
	if (id == SCE_SYSTEM_PARAM_ID_LANG)
		*v = (l && !strcmp(l, "tr")) ? SCE_SYSTEM_PARAM_LANG_TURKISH : SCE_SYSTEM_PARAM_LANG_ENGLISH_US;
	else if (id == SCE_SYSTEM_PARAM_ID_ENTER_BUTTON)
		*v = (e && !strcmp(e, "circle")) ? SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE : SCE_SYSTEM_PARAM_ENTER_BUTTON_CROSS;
	else return -1;
	return 0;
}
